// Created by moisrex on January 1th 2024.

#include "../webpp/uri/domain.hpp"
#include "../webpp/uri/uri.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;

using Types = testing::Types<stl::string, stl::string_view, stl::wstring, stl::wstring_view>;

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

template <typename T, typename Arg1, typename... Args>
constexpr decltype(auto) get_one(Arg1&& arg1, Args&&... args) {
    if constexpr (stl::is_convertible_v<T, Arg1>) {
        return stl::forward<Arg1>(arg1);
    } else if constexpr (stl::is_constructible_v<T, Arg1>) {
        return T{stl::forward<Arg1>(arg1)};
    } else {
        static_assert(sizeof...(Args) > 0, "No option for conversion");
        return get_one<T>(stl::forward<Args>(args)...);
    }
}

TYPED_TEST_SUITE(StructuredURITests, Types);

TYPED_TEST(StructuredURITests, StructuredDomain) {
    static TypeParam const        inp_domain = get_one<TypeParam>("domain.tld", L"domain.tld");
    basic_domain<TypeParam> const domain{inp_domain};
    EXPECT_TRUE(domain.is_valid());
    EXPECT_EQ(domain.tld(), get_one<TypeParam>("tld", L"tld"));
}

TYPED_TEST(StructuredURITests, StructuredFragment) {
    static TypeParam const data{get_one<TypeParam>("this is a fragment", L"this is a fragment")};
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

TYPED_TEST(StructuredURITests, StructuredUsername) {
    static TypeParam const               data{get_one<TypeParam>("it's username", L"it's username")};
    uri::basic_username<TypeParam> const user{data};
    EXPECT_TRUE(user.has_value());
    if constexpr (uri::basic_username<TypeParam>::is_modifiable) {
#ifdef __cpp_lib_string_contains
        EXPECT_FALSE(user.view().contains(' '));
#endif
        EXPECT_NE(data, user.view());
    } else {
        EXPECT_EQ(data, user.view());
    }
}

TYPED_TEST(StructuredURITests, StructuredPassword) {
    static TypeParam const               data{get_one<TypeParam>("it's password", L"it's password")};
    uri::basic_password<TypeParam> const pass{data};
    EXPECT_TRUE(pass.has_value());
    if constexpr (uri::basic_password<TypeParam>::is_modifiable) {
#ifdef __cpp_lib_string_contains
        EXPECT_FALSE(pass.view().contains(' '));
#endif
        EXPECT_NE(data, pass.view());
    } else {
        EXPECT_EQ(data, pass.view());
    }
}

TYPED_TEST(StructuredURITests, StructuredPort) {
    static TypeParam const           data{get_one<TypeParam>("8080", L"8080")};
    uri::basic_port<TypeParam> const port{data};
    EXPECT_TRUE(port.has_value());
    EXPECT_EQ(8080, port.value());
}

TYPED_TEST(StructuredURITests, StructuredScheme) {
    static TypeParam const             data{get_one<TypeParam>("https:", L"https:")};
    uri::basic_scheme<TypeParam> const scheme{data};
    EXPECT_TRUE(scheme.has_value());
    EXPECT_EQ(data.substr(0, data.size() - 1), scheme.view());
}

TYPED_TEST(StructuredURITests, StructuredSchemeLowered) {
    static TypeParam const data{get_one<TypeParam>("HTTPS:", L"HtTPS:")};

    EXPECT_EQ(data.size(), 6);

    uri::basic_scheme<TypeParam> const scheme{data};
    EXPECT_TRUE(scheme.has_value());
    if constexpr (uri::basic_scheme<TypeParam>::is_modifiable) {
        TypeParam out_str;
        ascii::lower_to(out_str, data.begin(), data.begin() + (data.size() - 1));
        EXPECT_EQ(out_str, ascii::to_lower_copy(data.substr(0, data.size() - 1)));

        EXPECT_EQ(get_one<TypeParam>("https", L"https"), scheme.view());
    } else {
        EXPECT_EQ(data.substr(0, data.size() - 1), scheme.view());
    }
}

TYPED_TEST(StructuredURITests, StructuredURI) {
    static TypeParam const data{get_one<TypeParam>("HTTPS:", L"HtTPS:")};

    uri::basic_uri<TypeParam> const url{data};
    EXPECT_TRUE(url.has_value());
    EXPECT_TRUE(url.has_scheme());
}
