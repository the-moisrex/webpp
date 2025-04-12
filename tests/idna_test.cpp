// Created by moisrex on Fri 2024/02/09

#include "../webpp/unicode/idna.hpp"

#include "../webpp/unicode/bidi.hpp"
#include "../webpp/unicode/joiners.hpp"
#include "../webpp/uri/uri.hpp"
#include "./common/bidi.hpp"
#include "./common/tests_common_pch.hpp"

// NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic, *-use-designated-initializers)
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
    for (stl::uint32_t index = 0; index != unicode::idna::details::idna_mapping_table.size(); ++index) {
        auto const cur = unicode::idna::details::idna_mapping_table[index];
        if ((cur & unicode::idna::details::mapped_mask) == 0) {
            continue;
        }
        auto length = (cur & ~unicode::idna::details::mapped_mask) >> 24U;

        auto             range_start = cur & ~unicode::idna::details::disallowed_mask;
        auto             range_end   = unicode::idna::details::idna_mapping_table[index + 1];
        std::string_view action      = "disallowed";
        if ([[maybe_unused]] bool const is_mapped =
              (cur & unicode::idna::details::disallowed_mask) != unicode::idna::details::disallowed_mask)
        {
            range_end = range_start + length;
            action    = "mapped/ignored";
        }

        length = range_end - range_start;

        for (stl::uint32_t sub_index = range_start; sub_index <= range_end;) {
            auto             sub_pos = unicode::idna::find_mapping_code_point(sub_index);
            std::string_view state   = "";
            if (*sub_pos != cur) {
                ++errors;
                sub_pos = unicode::idna::find_mapping_code_point(sub_index);
                faileds.insert(cur & ~unicode::idna::details::disallowed_mask);
                if (*sub_pos == last_one) {
                    state = " (last one) ";
                    ++picking_last_one;
                } else {
                    auto next_index = index + 1;
                    auto next       = unicode::idna::details::idna_mapping_table[next_index];
                    while ((next & unicode::idna::details::mapped_mask) == 0) {
                        ++next_index;
                        next = unicode::idna::details::idna_mapping_table[next_index];
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
                  << "Current: " << stl::hex << cur << " " << (cur & ~unicode::idna::details::disallowed_mask)
                  << stl::dec << "\n"
                  << "Range start: " << range_start << "\n"
                  << "Range end: " << range_end << "\n"
                  << "length: " << length << "\n"
                  << "Position of the iterator: "
                  << stl::distance(unicode::idna::details::idna_mapping_table.begin(), sub_pos)
                  << "\nCurrent: " << std::hex << (*sub_pos & ~unicode::idna::details::disallowed_mask)
                  << std::dec << state << "\nExpected: " << std::hex
                  << (cur & ~unicode::idna::details::disallowed_mask) << std::dec << "\naction: " << action;
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
    EXPECT_TRUE(unicode::idna::map(U'A', out));
    EXPECT_EQ(out, "a");
    out.clear();

    std::u32string out32;
    EXPECT_TRUE(unicode::idna::map(U'\x1F244', out32));
    EXPECT_EQ(out32, U"\x3014\x70B9\x3015");

    std::u8string out8;
    EXPECT_TRUE(unicode::idna::map(U'\x1F244', out8));
    EXPECT_EQ(out8, u8"\xE3\x80\x94\xE7\x82\xB9\xE3\x80\x95");
}

TEST(BasicIDNATests, UnicodeMapping) {
    // 'A' should be mapped to 'a'
    std::string out;
    EXPECT_TRUE(unicode::idna::map(U"A", out));
    EXPECT_EQ(out, "a");
    out.clear();

    std::u32string out32;
    EXPECT_TRUE(unicode::idna::map(U"\x1F244", out32));
    EXPECT_EQ(out32, U"\x3014\x70B9\x3015");

    std::u8string out8;
    EXPECT_TRUE(unicode::idna::map(U"\x1F244", out8));
    EXPECT_EQ(out8, u8"\xE3\x80\x94\xE7\x82\xB9\xE3\x80\x95");
}

TEST(BasicIDNATests, MostMappings) {
    using unicode::idna::mapped;
    using unicode::idna::status_of;
    using unicode::idna::details::disallowed;
    using unicode::idna::details::valid;

    // awk -f gen-idna-tests.awk IdnaMappingTable.txt
    // awk -f gen-idna-tests.awk IdnaMappingTable.txt | sort --random-sort | head
    EXPECT_EQ(status_of(0), valid);
    EXPECT_EQ(status_of(4), valid);
    EXPECT_EQ(status_of(8), valid);
    EXPECT_EQ(status_of(12), valid);
    EXPECT_EQ(status_of(16), valid);
    EXPECT_EQ(status_of(20), valid);
    EXPECT_EQ(status_of(24), valid);
    EXPECT_EQ(status_of(28), valid);
    EXPECT_EQ(status_of(32), valid);
    EXPECT_EQ(status_of(36), valid);

    EXPECT_EQ(status_of(128), disallowed);
    EXPECT_EQ(status_of(131), disallowed);
    EXPECT_EQ(status_of(134), disallowed);
    EXPECT_EQ(status_of(137), disallowed);
    EXPECT_EQ(status_of(140), disallowed);
    EXPECT_EQ(status_of(143), disallowed);
    EXPECT_EQ(status_of(146), disallowed);
    EXPECT_EQ(status_of(149), disallowed);
    EXPECT_EQ(status_of(152), disallowed);
    EXPECT_EQ(status_of(155), disallowed);

    EXPECT_EQ(mapped<std::u32string>(319), U"\x006C\x00B7"); // mapped
    EXPECT_EQ(mapped<std::u32string>(320), U"\x006C\x00B7"); // mapped
    EXPECT_EQ(status_of(446), valid);
    EXPECT_EQ(status_of(447), valid);
    EXPECT_EQ(status_of(450), valid);
    EXPECT_EQ(status_of(575), valid);
    EXPECT_EQ(status_of(576), valid);

    EXPECT_EQ(status_of(1480), disallowed);
    EXPECT_EQ(status_of(1481), disallowed);
    EXPECT_EQ(status_of(1482), disallowed);
    EXPECT_EQ(status_of(1483), disallowed);
    EXPECT_EQ(status_of(1484), disallowed);
    EXPECT_EQ(status_of(1485), disallowed);
    EXPECT_EQ(status_of(1486), disallowed);
    EXPECT_EQ(status_of(1487), disallowed);
    EXPECT_EQ(status_of(1488), valid);
    EXPECT_EQ(status_of(1490), valid);
    EXPECT_EQ(status_of(1492), valid);
    EXPECT_EQ(status_of(1494), valid);

    EXPECT_EQ(status_of(1512), valid);
    EXPECT_EQ(status_of(1514), valid);
    EXPECT_EQ(status_of(1515), disallowed);
    EXPECT_EQ(status_of(1516), disallowed);
    EXPECT_EQ(status_of(1517), disallowed);
    EXPECT_EQ(status_of(1518), disallowed);
    EXPECT_EQ(status_of(1519), valid);
    EXPECT_EQ(status_of(1520), valid);

    EXPECT_EQ(status_of(205'744), disallowed);
    EXPECT_EQ(status_of(205'742), valid);

    EXPECT_EQ(mapped<std::u32string>(173), U"");     // ignored
    EXPECT_EQ(mapped<std::u32string>(847), U"");     // ignored
    EXPECT_EQ(mapped<std::u32string>(4447), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(4448), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(6068), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(6069), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(6155), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(6156), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(6157), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(6158), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(6159), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8203), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8288), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8289), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8290), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8291), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8292), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8298), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8299), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8300), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8301), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8302), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(8303), U"");    // ignored
    EXPECT_EQ(mapped<std::u32string>(12'644), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'024), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'025), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'026), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'027), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'028), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'029), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'030), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'031), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'032), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'033), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'034), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'035), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'036), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'037), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'038), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'039), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'279), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(65'440), U"");  // ignored
    EXPECT_EQ(mapped<std::u32string>(113'824), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(113'825), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(113'826), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(113'827), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(119'155), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(119'156), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(119'157), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(119'158), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(119'159), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(119'160), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(119'161), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(119'162), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'760), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'783), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'806), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'829), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'852), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'875), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'898), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'921), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'944), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'967), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'990), U""); // ignored

    EXPECT_EQ(mapped<std::u32string>(917'925), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'926), U""); // ignored
    EXPECT_EQ(mapped<std::u32string>(917'925 + 48), U""); // ignored

    EXPECT_EQ(status_of(223), valid);                // deviation
    EXPECT_EQ(status_of(962), valid);                // deviation
    EXPECT_EQ(status_of(8204), valid);               // deviation
    EXPECT_EQ(status_of(8205), valid);               // deviation

    // awk -f gen-idna-tests.awk IdnaMappingTable.txt | grep map -B 3 --no-group-separator | head
    EXPECT_EQ(status_of(62), valid);
    EXPECT_EQ(status_of(63), valid);
    EXPECT_EQ(status_of(64), valid);
    EXPECT_EQ(mapped<std::u32string>(65), U"\x0061"); // mapped
    EXPECT_EQ(mapped<std::u32string>(66), U"\x0062"); // mapped
    EXPECT_EQ(mapped<std::u32string>(67), U"\x0063"); // mapped
    EXPECT_EQ(mapped<std::u32string>(68), U"\x0064"); // mapped
    EXPECT_EQ(mapped<std::u32string>(69), U"\x0065"); // mapped
    EXPECT_EQ(mapped<std::u32string>(70), U"\x0066"); // mapped
    EXPECT_EQ(mapped<std::u32string>(71), U"\x0067"); // mapped


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

TEST(BasicIDNATests, BidiMost) {
    using webpp::unicode::direction;
    using webpp::unicode::direction_of;

    EXPECT_EQ(direction_of(U'\x0041'), direction::L);
    EXPECT_EQ(direction_of(U'\x0600'), direction::AN);
    EXPECT_EQ(direction_of(U'\x0610'), direction::NSM);

    // 1734;HANUNOO SIGN PAMUDPOD;Mc;9;L;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1734'), direction::L);

    // 10101;AEGEAN WORD SEPARATOR DOT;Po;0;ON;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x10101'), direction::ON);

    // 1171E;AHOM CONSONANT SIGN MEDIAL RA;Mc;0;L;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1171E'), direction::L);

    // 1D6C1;MATHEMATICAL BOLD NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D6C1'), direction::ON);

    // 1D6FB;MATHEMATICAL ITALIC NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D6FB'), direction::ON);

    // 1D735;MATHEMATICAL BOLD ITALIC NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D735'), direction::ON);

    // 1D76F;MATHEMATICAL SANS-SERIF BOLD NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D76F'), direction::ON);

    // 1D7A9;MATHEMATICAL SANS-SERIF BOLD ITALIC NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D7A9'), direction::ON);

    // 1F4A9;PILE OF POO;So;0;ON;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1F4A9'), direction::ON);

    // 061D;ARABIC END OF TEXT MARK;Po;0;AL;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x61D'), direction::AL);

    // E01EF;VARIATION SELECTOR-256;Mn;0;NSM;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\xE01EF'), direction::NSM);

    for (char32_t cp = 0; cp < 0x10'fffdU + 10; cp += 1) {
        auto const lhs = direction_of(cp);
        auto const rhs = webpp::unicode::tests::find_direction(cp);

        // if these fail, that might be because the test table might be a few versions behind;
        // that's why we're ignoring some of the tests here:
        if (lhs != rhs && rhs == direction::NONE) {
            continue;
        }

        switch (cp) {
            case 0x1734:
            case 0x1'171E:
            case 0x1'D6C1:
            case 0x1'D6FB:
            case 0x1'D735:
            case 0x1'D76F:
            case 0x1'D7A9: continue;
            default: break;
        }

        EXPECT_EQ(lhs, rhs) << "Code Point: " << static_cast<int>(cp) << "\n    " << to_abbr(lhs) << " ("
                            << to_string(lhs) << ")  !==  " << to_abbr(rhs) << " (" << to_string(rhs) << ")";
    }
}

TEST(BasicIDNATests, BidiRules) {
    using webpp::stl::u32string_view;
    using webpp::unicode::validate_bidi_rule;

    // https://www.rfc-editor.org/rfc/rfc5893#section-4.1
    u32string_view const computer_word =
      U"\u0786"  // THAANA LETTER KAAFU (AL)
      U"\u07AE"  // THAANA OBOFILI (NSM)
      U"\u0782"  // THAANA LETTER NOONU (AL)
      U"\u07B0"  // THAANA SUKUN (NSM)
      U"\u0795"  // THAANA LETTER PAVIYANI (AL)
      U"\u07A9"  // THAANA LETTER EEBEEFILI (AL)
      U"\u0793"  // THAANA LETTER TAVIYANI (AL)
      U"\u07A6"  // THAANA ABAFILI (NSM)
      U"\u0783"  // THAANA LETTER RAA (AL)
      U"\u07AA"; // THAANA UBUFILI (NSM)

    // https://www.rfc-editor.org/rfc/rfc5893#section-4.2
    u32string_view const yivo_acronym =
      U"\u05D9"  // HEBREW LETTER YOD (R)
      U"\u05B4"  // HEBREW POINT HIRIQ (NSM)
      U"\u05D5"  // HEBREW LETTER VAV (R)
      U"\u05D0"  // HEBREW LETTER ALEF (R)
      U"\u05B8"; // HEBREW POINT QAMATS (NSM)


    EXPECT_TRUE(validate_bidi_rule(computer_word.begin(), computer_word.end()));
    EXPECT_TRUE(validate_bidi_rule(yivo_acronym.begin(), yivo_acronym.end()));


    // 3.  In an RTL label, the end of the label must be a character with
    //     Bidi property R, AL, EN, or AN, followed by zero or more
    //     characters with Bidi property NSM.
    u32string_view const invalid_computer_word =
      U"\u0786" // THAANA LETTER KAAFU (AL)
      U"\u07AE" // THAANA OBOFILI (NSM)
      U"\u0782" // THAANA LETTER NOONU (AL)
      U"\u07B0" // THAANA SUKUN (NSM)
      U"\u0795" // THAANA LETTER PAVIYANI (AL)
      U"\u07A9" // THAANA LETTER EEBEEFILI (AL)
      U"\u0793" // THAANA LETTER TAVIYANI (AL)
      U"\u07A6" // THAANA ABAFILI (NSM)
      U"\u0783" // THAANA LETTER RAA (AL)
      U"\u07AA" // THAANA UBUFILI (NSM)
      U"\u0294" // 0294          ; L # Lo       LATIN LETTER GLOTTAL STOP
      U"\u07AA" // THAANA UBUFILI (NSM)
      ;

    EXPECT_FALSE(validate_bidi_rule(invalid_computer_word.begin(), invalid_computer_word.end()));


    u32string_view const invalid_yivo_acronym =
      U"\u05D9"  // HEBREW LETTER YOD (R)
      U"\u05B4"  // HEBREW POINT HIRIQ (NSM)
      U"\u05D5"  // HEBREW LETTER VAV (R)
      U"\u05D0"  // HEBREW LETTER ALEF (R)
      U"\u0378"  // Should be (L), but this makes it invalid
      U"\u05B8"; // HEBREW POINT QAMATS (NSM)

    EXPECT_FALSE(validate_bidi_rule(invalid_yivo_acronym.begin(), invalid_yivo_acronym.end()));

    u32string_view const empty{};
    EXPECT_TRUE(validate_bidi_rule(empty.begin(), empty.end()));
}

TEST(BasicIDNATests, JoinerTypesTest) {
    using webpp::unicode::joiner_type;
    using webpp::unicode::joiner_type_of;

    EXPECT_EQ(joiner_type_of(U'\x0'), joiner_type::non_joining);
    EXPECT_EQ(joiner_type_of(U'\x1'), joiner_type::non_joining);

    // Generated using:
    // ./gen-joiners-tests.awk DerivedJoiningType.txt | grep -v transparent | grep -v join_causing | sort
    //     --random-sort | head -n 50
    EXPECT_EQ(joiner_type_of(U'\x84E'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x8A8'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x6BD'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x10AD9'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x1E92F'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x67C'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x8BC'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x10F33'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x634'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x1E921'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x0627'), joiner_type::right_joining);  // R
    EXPECT_EQ(joiner_type_of(U'\x67D'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x1E905'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x10F76'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\xA858'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x188A'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x758'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x62A'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\xA85A'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x06C0'), joiner_type::right_joining);  // R
    EXPECT_EQ(joiner_type_of(U'\x872'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x696'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x693'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x877'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\xA84A'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x699'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x76E'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\xA84F'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x10FBB'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x10F73'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x841'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x871'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x768'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x87B'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x10EC3'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x7D8'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x842'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\xA856'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x10BAB'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x7CE'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x6B8'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x720'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x63C'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x10FB5'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x1E927'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x775'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x7DA'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x8A9'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x184A'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x183F'), joiner_type::dual_joining);   // D

    EXPECT_EQ(joiner_type_of(U'\x11180'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1CF1A'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x10FB2'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x1E016'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x2DE1'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x10F4D'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x76B'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x81C'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\xF9C'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\x186D'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x13452'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x113BC'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xFAF'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\x1928'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\xE017F'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE011E'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE0137'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE4E'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\xE01C9'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1BC9E'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE01D5'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE0026'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1DA05'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x719'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x11C38'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x885'), joiner_type::join_causing);    // C
    EXPECT_EQ(joiner_type_of(U'\x10B80'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x1DA51'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xA948'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\xEBA'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\x1DA54'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x8AF'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x77D'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x1DA0C'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1E026'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1A77'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x1AC7'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x07FA'), joiner_type::join_causing);   // C
    EXPECT_EQ(joiner_type_of(U'\x1103F'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1920'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x331'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\x11102'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x11C94'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1852'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x0DD6'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x1DA55'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x81D'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\xAE3'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\xE0174'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xA85F'), joiner_type::dual_joining);   // D

    EXPECT_EQ(joiner_type_of(U'\x10ACD'), joiner_type::left_joining);  // L
    EXPECT_EQ(joiner_type_of(U'\x10D00'), joiner_type::left_joining);  // L
    EXPECT_EQ(joiner_type_of(U'\xA872'), joiner_type::left_joining);   // L
    EXPECT_EQ(joiner_type_of(U'\x10FCB'), joiner_type::left_joining);  // L
    EXPECT_EQ(joiner_type_of(U'\x10AD7'), joiner_type::left_joining);  // L

    EXPECT_EQ(joiner_type_of(U'\x874'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x0854'), joiner_type::right_joining);  // R
    EXPECT_EQ(joiner_type_of(U'\x10ADD'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x87A'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x0771'), joiner_type::right_joining);  // R
    EXPECT_EQ(joiner_type_of(U'\x672'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x10B89'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x8B2'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x10BAB'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x10FC3'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x632'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x676'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x072F'), joiner_type::right_joining);  // R

    EXPECT_EQ(joiner_type_of(U'\x200D'), joiner_type::join_causing);   // C
    EXPECT_EQ(joiner_type_of(U'\x885'), joiner_type::join_causing);    // C
    EXPECT_EQ(joiner_type_of(U'\x883'), joiner_type::join_causing);    // C
    EXPECT_EQ(joiner_type_of(U'\x884'), joiner_type::join_causing);    // C
    EXPECT_EQ(joiner_type_of(U'\x07FA'), joiner_type::join_causing);   // C
    EXPECT_EQ(joiner_type_of(U'\x180A'), joiner_type::join_causing);   // C
    EXPECT_EQ(joiner_type_of(U'\x0640'), joiner_type::join_causing);   // C
}

TEST(BasicIDNATests, GeneralCategoryTest) {
    using webpp::unicode::general_category;
    using webpp::unicode::general_category_of;
    using webpp::unicode::is_general_category_of;


    EXPECT_TRUE(is_general_category_of(U'\x112E6', general_category::M));
    EXPECT_TRUE(is_general_category_of(U'\x300', general_category::M));

    // cat UnicodeData.txt | awk 'BEGIN{FS=";"} { if ($3 == "Mn" || $3 == "Me" || $3 == "Mc") print
    // "EXPECT_EQ(general_category_of(U\'\\\x" $1 "\'), general_category::" $3 "); // " $2; }' | sort
    // --random-sort | head -n 20
    EXPECT_EQ(general_category_of(U'\x05AC'), general_category::Mn);  // HEBREW ACCENT ILUY
    EXPECT_EQ(general_category_of(U'\x05A8'), general_category::Mn);  // HEBREW ACCENT QADMA
    EXPECT_EQ(general_category_of(U'\x1A5A'), general_category::Mn);  // TAI THAM CONSONANT SIGN LOW PA
    EXPECT_EQ(general_category_of(U'\x17D1'), general_category::Mn);  // KHMER SIGN VIRIAM
    EXPECT_EQ(general_category_of(U'\x1DA5B'), general_category::Mn); // SIGNWRITING TONGUE TIP BETWEEN LIPS
    EXPECT_EQ(general_category_of(U'\x1DFC'), general_category::Mn);  // COMBINING DOUBLE INVERTED BREVE BELOW
    EXPECT_EQ(general_category_of(U'\xA8E6'), general_category::Mn);  // COMBINING DEVANAGARI DIGIT SIX
    EXPECT_EQ(general_category_of(U'\xE0178'), general_category::Mn); // VARIATION SELECTOR-137
    EXPECT_EQ(general_category_of(U'\x16F51'), general_category::Mc); // MIAO SIGN ASPIRATION
    EXPECT_EQ(general_category_of(U'\x0B41'), general_category::Mn);  // ORIYA VOWEL SIGN U
    EXPECT_EQ(general_category_of(U'\xE01BA'), general_category::Mn); // VARIATION SELECTOR-203
    EXPECT_EQ(general_category_of(U'\x05B2'), general_category::Mn);  // HEBREW POINT HATAF PATAH
    EXPECT_EQ(general_category_of(U'\x031D'), general_category::Mn);  // COMBINING UP TACK BELOW
    EXPECT_EQ(general_category_of(U'\x1A58'), general_category::Mn);  // TAI THAM SIGN MAI KANG LAI
    EXPECT_EQ(general_category_of(U'\xE017B'), general_category::Mn); // VARIATION SELECTOR-140
    EXPECT_EQ(general_category_of(U'\x112E6'), general_category::Mn); // KHUDAWADI VOWEL SIGN AI
    EXPECT_EQ(general_category_of(U'\x036F'), general_category::Mn);  // COMBINING LATIN SMALL LETTER X
    EXPECT_EQ(general_category_of(U'\x1182F'), general_category::Mn); // DOGRA VOWEL SIGN U
    EXPECT_EQ(general_category_of(U'\xE0114'), general_category::Mn); // VARIATION SELECTOR-37
    EXPECT_EQ(general_category_of(U'\x11233'), general_category::Mc); // KHOJKI VOWEL SIGN AU

    EXPECT_EQ(general_category_of(U'\x0300'), general_category::Mn);  // COMBINING GRAVE ACCENT
    EXPECT_EQ(general_category_of(U'\x0301'), general_category::Mn);  // COMBINING ACUTE ACCENT
    EXPECT_EQ(general_category_of(U'\x0302'), general_category::Mn);  // COMBINING CIRCUMFLEX ACCENT
    EXPECT_EQ(general_category_of(U'\x0303'), general_category::Mn);  // COMBINING TILDE
    EXPECT_EQ(general_category_of(U'\x0304'), general_category::Mn);  // COMBINING MACRON
    EXPECT_EQ(general_category_of(U'\x0305'), general_category::Mn);  // COMBINING OVERLINE
    EXPECT_EQ(general_category_of(U'\x0306'), general_category::Mn);  // COMBINING BREVE
    EXPECT_EQ(general_category_of(U'\x0307'), general_category::Mn);  // COMBINING DOT ABOVE
    EXPECT_EQ(general_category_of(U'\x0308'), general_category::Mn);  // COMBINING DIAERESIS
    EXPECT_EQ(general_category_of(U'\x0309'), general_category::Mn);  // COMBINING HOOK ABOVE
    EXPECT_EQ(general_category_of(U'\x030A'), general_category::Mn);  // COMBINING RING ABOVE
    EXPECT_EQ(general_category_of(U'\x030B'), general_category::Mn);  // COMBINING DOUBLE ACUTE ACCENT
    EXPECT_EQ(general_category_of(U'\x030C'), general_category::Mn);  // COMBINING CARON
    EXPECT_EQ(general_category_of(U'\x030D'), general_category::Mn);  // COMBINING VERTICAL LINE ABOVE
    EXPECT_EQ(general_category_of(U'\x030E'), general_category::Mn);  // COMBINING DOUBLE VERTICAL LINE ABOVE
    EXPECT_EQ(general_category_of(U'\x030F'), general_category::Mn);  // COMBINING DOUBLE GRAVE ACCENT
    EXPECT_EQ(general_category_of(U'\x0310'), general_category::Mn);  // COMBINING CANDRABINDU
    EXPECT_EQ(general_category_of(U'\x0311'), general_category::Mn);  // COMBINING INVERTED BREVE
    EXPECT_EQ(general_category_of(U'\x0312'), general_category::Mn);  // COMBINING TURNED COMMA ABOVE
    EXPECT_EQ(general_category_of(U'\x0313'), general_category::Mn);  // COMBINING COMMA ABOVE
}

TEST(BasicIDNATests, CheckValidiyCriteria) {
    using webpp::stl::array;
    using webpp::stl::string;
    using webpp::stl::string_view;
    using webpp::unicode::idna::idna_options;
    using webpp::unicode::idna::is_label_valid;

    struct opts {
        string_view str;
        bool        is_valid  = true;
        int         opt_index = 0;
    };

    static constexpr array<idna_options, 2> idna_opts{
      {
       idna_options{},
       idna_options{
          .CheckHyphens            = true,
          .CheckBidi               = true,
          .CheckJoiners            = true,
          .UseSTD3ASCIIRules       = true,
          .Transitional_Processing = true,
          .VerifyDnsLength         = true,
          .IgnoreInvalidPunycode   = true,
          .CheckNFC                = false, // todo
          .CheckDotInclusions      = true,
          .CheckStatusValues       = true,
        }, }
    };

    static constexpr array<opts, 134> tests{
      opts{"", true, -1},
      {"a", true, -1},
      {"-"},
      {"--"},
      {"---"},
      {"xn---", false},
      {"nn---"},
      {"nn---", false, 1},
      {"---", false, 1},
      {"--", false, 1},
      {"-", false, 1},
      {"correct", true, -1},
      {"a--b", false, 1},
      {"a--b", true, 0},

      // Basic ASCII & LDH (Letter-Digit-Hyphen)
      {"1", true, -1}, // Single digit
      {"a1", true, -1}, // Letter followed by digit
      {"1a", true, -1}, // Digit followed by letter
      {"a-b", true, -1}, // Hyphen in middle

      {"ABC", true, 0}, // STD3 Rule
      {"ABC", false, 1}, // STD3 Rule
    };

    for (auto const [str, is_valid, opts_index] : tests) {
        switch (opts_index) {
            case -1:
                EXPECT_EQ(is_valid, is_label_valid<idna_opts[0]>(str.begin(), str.end())) << str;
                EXPECT_EQ(is_valid, is_label_valid<idna_opts[1]>(str.begin(), str.end())) << str;
                break;
            case 0: EXPECT_EQ(is_valid, is_label_valid<idna_opts[0]>(str.begin(), str.end())) << str; break;
            case 1: EXPECT_EQ(is_valid, is_label_valid<idna_opts[1]>(str.begin(), str.end())) << str; break;
            default: break;
        }
    }
}

TEST(BasicIDNATests, ToASCIITest) {
    using webpp::unicode::idna::to_ascii;
    using enum webpp::unicode::idna::to_ascii_status;

    // EXPECT_EQ(to_ascii("one"), valid);
}

// NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic, *-use-designated-initializers)
