// Created by moisrex on January 1th 2024.

#include "../webpp/uri/domain.hpp"
#include "../webpp/uri/uri.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;

using Types = testing::Types<stl::string, stl::string_view>;

template <class T>
struct StructuredURITests : testing::Test {
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

TYPED_TEST_SUITE(StructuredURITests, Types);

TYPED_TEST(StructuredURITests, StructuredDomain) {
    static TypeParam const        inp_domain = "domain.tld";
    basic_domain<TypeParam> const domain{inp_domain};
    EXPECT_TRUE(domain.is_valid());
    EXPECT_EQ(domain.tld(), "tld");
}

TYPED_TEST(StructuredURITests, StructuredFragment) {
    static TypeParam const               data{"this is a fragment"};
    uri::basic_fragment<TypeParam> const fragment{data};
    EXPECT_TRUE(fragment.has_value());
    if constexpr (uri::basic_fragment<TypeParam>::is_modifiable) {
#ifdef __cpp_lib_string_contains
        EXPECT_FALSE(fragment.view().contains(' '));
#endif
        EXPECT_NE(data, fragment.view());
    } else {
        EXPECT_EQ(data, fragment.view());
    }
}
