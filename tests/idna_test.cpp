// Created by moisrex on Fri 2024/02/09

#include "../webpp/uri/idna/idna_mappings.hpp"
#include "../webpp/uri/uri.hpp"
#include "common/tests_common_pch.hpp"

// NOLINTBEGIN(*-magic-numbers)
using namespace webpp;

using Types =
  testing::Types<uri::parsing_uri_context_string<stl::string>,
                 uri::parsing_uri_context_string<stl::string_view>,
                 // uri::parsing_uri_context_string<stl::basic_string_view<char8_t>>,
                 uri::parsing_uri_context_u32,
                 uri::parsing_uri_context_segregated<>,
                 uri::parsing_uri_context_segregated_view<>,
                 uri::parsing_uri_context<stl::string_view, char const*>>;

template <class T>
struct IDNATests : testing::Test {
  private:
    stl::string url_text;

  public:
    template <typename SpecifiedTypeParam, typename StrT = stl::string_view>
    [[nodiscard]] constexpr SpecifiedTypeParam get_context(StrT str) {
        using ctx_type    = SpecifiedTypeParam;
        using string_type = stl::remove_cvref_t<StrT>;
        using str_iter    = typename string_type::const_iterator;
        using iterator    = typename ctx_type::iterator;
        if constexpr (stl::convertible_to<str_iter, iterator>) {
            return SpecifiedTypeParam{.beg = str.begin(), .pos = str.begin(), .end = str.end()};
        } else if constexpr (stl::convertible_to<stl::string::iterator, iterator>) {
            url_text = str;
            return SpecifiedTypeParam{.beg = url_text.begin(),
                                      .pos = url_text.begin(),
                                      .end = url_text.end()};
        } else {
            return SpecifiedTypeParam{.beg = str.data(), .pos = str.data(), .end = str.data() + str.size()};
        }
    }

    template <typename SpecifiedTypeParam>
    [[nodiscard]] constexpr SpecifiedTypeParam parse_from_string(stl::string_view const str) {
        auto ctx = get_context<SpecifiedTypeParam, stl::string_view>(str);
        uri::parse_uri(ctx);
        return ctx;
    }
};

TYPED_TEST_SUITE(IDNATests, Types);

TYPED_TEST(IDNATests, LabelSeparators) {
    // https://www.unicode.org/reports/tr46/#Notation
    // Label Separators are:
    //   - U+002E ( . ) FULL STOP
    //   - U+FF0E ( ． ) FULLWIDTH FULL STOP
    //   - U+3002 ( 。 ) IDEOGRAPHIC FULL STOP
    //   - U+FF61 ( ｡ ) HALFWIDTH IDEOGRAPHIC FULL STOP

    auto const ctx1 = this->template parse_from_string<TypeParam>("http://example.org");
    EXPECT_FALSE(uri::is_valid(ctx1.status)) << to_string(uri::get_value(ctx1.status));

    auto const ctx2 = this->template parse_from_string<TypeParam>("http://example．org");
    EXPECT_FALSE(uri::is_valid(ctx2.status)) << to_string(uri::get_value(ctx2.status));

    auto const ctx3 = this->template parse_from_string<TypeParam>("http://example。org");
    EXPECT_FALSE(uri::is_valid(ctx3.status)) << to_string(uri::get_value(ctx3.status));

    auto const ctx4 = this->template parse_from_string<TypeParam>("http://example｡org");
    EXPECT_FALSE(uri::is_valid(ctx4.status)) << to_string(uri::get_value(ctx4.status));

    EXPECT_EQ(ctx1.out.get_hostname(), "example.org");
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(ctx2.out.get_hostname(), "example.org");
        EXPECT_EQ(ctx3.out.get_hostname(), "example.org");
        EXPECT_EQ(ctx4.out.get_hostname(), "example.org");
    }
}

/*
TEST(BasicIDNATests, TestingAllTheTable) {
    stl::size_t errors           = 0;
    stl::size_t picking_last_one = 0;
    stl::size_t picking_next_one = 0;

    stl::uint32_t           last_one = 0;
    std::set<stl::uint32_t> faileds;
    for (stl::uint32_t index = 0; index != uri::idna::details::idna_mapping_table.size(); ++index) {
        auto const cur = uri::idna::details::idna_mapping_table[index];
        if ((cur & uri::idna::details::mapped_mask) == 0) {
            continue;
        }
        auto length = (cur & ~uri::idna::details::mapped_mask) >> 24U;

        auto             range_start = cur & ~uri::idna::details::disallowed_mask;
        auto             range_end   = uri::idna::details::idna_mapping_table[index + 1];
        std::string_view action      = "disallowed";
        if ([[maybe_unused]] bool const is_mapped =
              (cur & uri::idna::details::disallowed_mask) != uri::idna::details::disallowed_mask)
        {
            range_end = range_start + length;
            action    = "mapped/ignored";
        }

        length = range_end - range_start;

        for (stl::uint32_t sub_index = range_start; sub_index <= range_end;) {
            auto             sub_pos = uri::idna::find_mapping_code_point(sub_index);
            std::string_view state   = "";
            if (*sub_pos != cur) {
                ++errors;
                sub_pos = uri::idna::find_mapping_code_point(sub_index);
                faileds.insert(cur & ~uri::idna::details::disallowed_mask);
                if (*sub_pos == last_one) {
                    state = " (last one) ";
                    ++picking_last_one;
                } else {
                    auto next_index = index + 1;
                    auto next       = uri::idna::details::idna_mapping_table[next_index];
                    while ((next & uri::idna::details::mapped_mask) == 0) {
                        ++next_index;
                        next = uri::idna::details::idna_mapping_table[next_index];
                    }
                    if (*sub_pos == next) {
                        ++picking_next_one;
                        state = " (next one) ";
                    }
                }
                EXPECT_EQ(*sub_pos, cur)
                  << "Index: " << index << "\n"
                  << "Sub Index: " << sub_index << " HexChar: " << std::hex << sub_index << std::dec
                  << " diff: " << (sub_index - range_start) << "\n"
                  << "Current: " << stl::hex << cur << " " << (cur & ~uri::idna::details::disallowed_mask)
                  << stl::dec << "\n"
                  << "Range start: " << range_start << "\n"
                  << "Range end: " << range_end << "\n"
                  << "length: " << length << "\n"
                  << "Position of the iterator: "
                  << stl::distance(uri::idna::details::idna_mapping_table.begin(), sub_pos)
                  << "\nCurrent: " << std::hex << (*sub_pos & ~uri::idna::details::disallowed_mask)
                  << std::dec << state << "\nExpected: " << std::hex
                  << (cur & ~uri::idna::details::disallowed_mask) << std::dec << "\naction: " << action;
            }

            auto const half  = length / 2;
            sub_index       += half;
            if (half == 0) {
                ++sub_index;
            }
        }

        last_one = cur;
    }
    EXPECT_EQ(errors, 0)
      << stl::accumulate(faileds.begin(), faileds.end(), std::string(), [](auto const& res, auto b) {
             return res + ", " + stl::to_string(b);
         }).substr(2);
    EXPECT_EQ(picking_last_one, 0);
    EXPECT_EQ(picking_next_one, 0);
}
*/



TEST(BasicIDNATests, PerformMappingTest) {
    // 'A' should be mapped to 'a'
    std::string out;
    EXPECT_TRUE(uri::idna::map(U'A', out));
    EXPECT_EQ(out, "a");
    out.clear();

    std::u32string out32;
    EXPECT_TRUE(uri::idna::map(U'\x1F244', out32));
    EXPECT_EQ(out32, U"\x3014\x70B9\x3015");

    std::u8string out8;
    EXPECT_TRUE(uri::idna::map(U'\x1F244', out8));
    EXPECT_EQ(out8, u8"\xE3\x80\x94\xE7\x82\xB9\xE3\x80\x95");
}

TEST(BasicIDNATests, UnicodeMapping) {
    // 'A' should be mapped to 'a'
    std::string out;
    EXPECT_TRUE(uri::idna::map(U"A", out));
    EXPECT_EQ(out, "a");
    out.clear();

    std::u32string out32;
    EXPECT_TRUE(uri::idna::map(U"\x1F244", out32));
    EXPECT_EQ(out32, U"\x3014\x70B9\x3015");

    std::u8string out8;
    EXPECT_TRUE(uri::idna::map(U"\x1F244", out8));
    EXPECT_EQ(out8, u8"\xE3\x80\x94\xE7\x82\xB9\xE3\x80\x95");
}

TEST(BasicIDNATests, MostMappings) {
    using uri::idna::mapped;
    using uri::idna::status_of;
    using uri::idna::details::disallowed;
    using uri::idna::details::valid;

    // awk -f gen-idna-tests.awk IdnaMappingTable.txt
    // awk -f gen-idna-tests.awk IdnaMappingTable.txt | sort --random-sort | head
    EXPECT_EQ(status_of(67'454), disallowed);
    EXPECT_EQ(status_of(119'751), disallowed);
    EXPECT_EQ(status_of(69'594), disallowed);
    EXPECT_EQ(status_of(72'361), disallowed);
    EXPECT_EQ(status_of(40'892), valid);
    EXPECT_EQ(status_of(71'812), disallowed);
    EXPECT_EQ(status_of(4021), valid);
    EXPECT_EQ(mapped<std::u32string>(8315), U"\x2212");               // mapped
    EXPECT_EQ(status_of(11'676), disallowed);
    EXPECT_EQ(status_of(6851), valid);
    EXPECT_EQ(mapped<std::u32string>(404), U"\x0263");                // mapped
    EXPECT_EQ(status_of(43'130), disallowed);
    EXPECT_EQ(status_of(70'046), valid);
    EXPECT_EQ(mapped<std::u32string>(63'972), U"\x7406");             // mapped
    EXPECT_EQ(mapped<std::u32string>(7586), U"\x0261");               // mapped
    EXPECT_EQ(status_of(2974), valid);
    EXPECT_EQ(status_of(64'221), disallowed);
    EXPECT_EQ(mapped<std::u32string>(120'540), U"\x03B5");            // mapped
    EXPECT_EQ(status_of(68'926), disallowed);
    EXPECT_EQ(mapped<std::u32string>(12'808), U"\x0028\x110C\x0029"); // mapped
    EXPECT_EQ(status_of(43'230), disallowed);
    EXPECT_EQ(mapped<std::u32string>(7690), U"\x1E0B");               // mapped
    EXPECT_EQ(mapped<std::u32string>(12'089), U"\x5F50");             // mapped
    EXPECT_EQ(status_of(68'942), valid);
    EXPECT_EQ(status_of(3400), valid);
    EXPECT_EQ(status_of(11'247), valid);
    EXPECT_EQ(status_of(1569), valid);
    EXPECT_EQ(mapped<std::u32string>(12'644), U""); // ignored
    EXPECT_EQ(status_of(66'289), valid);
    EXPECT_EQ(status_of(72'548), disallowed);
    EXPECT_EQ(status_of(127'128), disallowed);
    EXPECT_EQ(mapped<std::u32string>(12'614), U"\x110A");  // mapped
    EXPECT_EQ(mapped<std::u32string>(42'824), U"\xA749");  // mapped
    EXPECT_EQ(status_of(11'944), valid);
    EXPECT_EQ(mapped<std::u32string>(127'538), U"\x7981"); // mapped
    EXPECT_EQ(status_of(129'205), valid);
    EXPECT_EQ(status_of(67'386), disallowed);
    EXPECT_EQ(status_of(6903), disallowed);
    EXPECT_EQ(status_of(185'478), valid);
    EXPECT_EQ(status_of(2950), valid);
    EXPECT_EQ(status_of(7707), valid);
    EXPECT_EQ(status_of(4814), valid);
    EXPECT_EQ(status_of(66'008), valid);
    EXPECT_EQ(status_of(125'274), disallowed);
    EXPECT_EQ(status_of(2228), valid);
    EXPECT_EQ(status_of(71'892), valid);
    EXPECT_EQ(status_of(9932), valid);
    EXPECT_EQ(status_of(73'902), valid);
    EXPECT_EQ(status_of(123'211), disallowed);
    EXPECT_EQ(status_of(481), valid);
}

// NOLINTEND(*-magic-numbers)
