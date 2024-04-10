// Created by moisrex on Fri 2024/02/09

#include "../webpp/uri/idna/idna_mappings.hpp"
#include "../webpp/uri/uri.hpp"
#include "common/tests_common_pch.hpp"

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

TYPED_TEST(IDNATests, MappingFindAlgorithmTest) {
    // 'A' should be mapped to 'a'
    auto const pos = uri::idna::find_mapping_byte('A');
    EXPECT_EQ(*pos, 2'147'483'713ULL)
      << "Position of the iterator: " << stl::distance(uri::idna::details::idna_mapping_table.begin(), pos);
    EXPECT_EQ(*stl::next(pos), 'a');
}
