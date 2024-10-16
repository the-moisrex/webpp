// Created by moisrex on 11/9/20.

#include "../webpp/uri/uri.hpp"

#include "../webpp/ip/ipv4.hpp"
#include "../webpp/uri/details/iiequals.hpp"
#include "../webpp/uri/path_traverser.hpp"
#include "common/tests_common_pch.hpp"

#include <ranges>

// also checkout:
//  - domain-test

using namespace webpp;


// uri_status_iterator is designed to work with ranges
static_assert(std::input_iterator<uri::uri_status_iterator>, "Input iterator");
static_assert(std::forward_iterator<uri::uri_status_iterator>, "Input iterator");
#if __cpp_lib_ranges
static_assert(std::ranges::input_range<uri::uri_status_iterator>, "Input range");
#endif

TEST(URIHelperTests, IIEquals) {
    EXPECT_TRUE(uri::iiequals<uri::details::TABS_OR_NEWLINES<char>>("\t\th\tel\rlo world\t.\n",
                                                                    "hello wor\t\t\t\t\tl\nd."));
}

using Types =
  testing::Types<uri::parsing_uri_context_segregated_view<>,
                 uri::parsing_uri_context_string<stl::string>,
                 uri::parsing_uri_context_string<stl::string_view>,
                 // uri::parsing_uri_context_string<stl::basic_string_view<char8_t>>,
                 uri::parsing_uri_context_u32,
                 uri::parsing_uri_context_segregated<>,
                 uri::parsing_uri_context<stl::string_view, char const*>>;

template <class T>
struct URITests : testing::Test {
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

TYPED_TEST_SUITE(URITests, Types);

TYPED_TEST(URITests, Generation) {
    uri::uri url;
    EXPECT_EQ(url.scheme_ref().size(), 0);

    auto const alloc = url.get_allocator();

    stl::string const str{alloc};
    EXPECT_EQ(str.size(), 0);

    url.scheme(stl::string_view{"https"});
    EXPECT_EQ(url.as_string(), "");
    url.clear_scheme();
    url.scheme(stl::string_view{"https:"});
    EXPECT_EQ(url.as_string(), "https:");
    url.hostname("webpp.dev");
    EXPECT_EQ(url.as_string(), "https://webpp.dev/");
}

TYPED_TEST(URITests, PathFromString) {
    uri::basic_path path{"/a/b/c/../d"};
    ASSERT_EQ(path.size(), 4);
    EXPECT_EQ(path[0], "");
    EXPECT_EQ(path[1], "a");
    path /= "nice";
    ASSERT_EQ(path.size(), 5);
    EXPECT_TRUE(stl::is_eq(path <=> stl::string{"/a/b/c/../d/nice"}));
    // EXPECT_EQ(path, "/a/b/c/../d/nice");
}

// TYPED_TEST(URITests, QueryParamGeneration) {
//     uri::uri url          = "https://localhost/api/v2/content";
//     url.queries["model"]  = "Encode this";
//     url.queries["locale"] = "English is a locale";
//     url.queries["text"]   = "This text has a \n newline in it.";
//     url.queries["token"]  = "f95d9af1-18da-439c-8326-55f9ff7d6f8c";
//     EXPECT_EQ(url.as_string(),
//               "https://localhost/api/v2/"
//               "content?locale=English%20is%20a%20locale&model=Encode%20this&text=This%20text%20has%20a%20%0A%"
//               "20newline%20in%20it.&token=f95d9af1-18da-439c-8326-55f9ff7d6f8c");
// }

TYPED_TEST(URITests, IntegralSchemeParsing) {
    constexpr stl::string_view      str = "http://";
    uri::parsing_uri_context_view<> context{.beg = str.begin(), .pos = str.begin(), .end = str.end()};
    uri::parse_scheme(context);
    auto const res = uri::get_value(context.status);
    EXPECT_EQ(res, uri::uri_status::valid_authority) << to_string(res);
    EXPECT_EQ(context.out.get_scheme(), "http");
    EXPECT_EQ(context.pos - str.begin(), 7);
}

TYPED_TEST(URITests, StringSchemeParsing) {
    constexpr stl::string_view str = "urn:testing";

    uri::parsing_uri_context<stl::string_view, char const*> context{
      .beg = str.data(),
      .pos = str.data(),
      .end = str.data() + str.size()};

    uri::parse_scheme(context);
    auto const res = static_cast<uri::uri_status>(context.status);
    EXPECT_EQ(res, uri::uri_status::valid_opaque_path) << to_string(res);
    EXPECT_EQ(context.out.get_scheme(), "urn");
    EXPECT_EQ(context.pos - str.data(), 4);
}

TYPED_TEST(URITests, ParseURI) {
    constexpr stl::string_view str     = "urn:testing";
    auto                       context = this->template get_context<TypeParam>(str);

    uri::parse_uri(context);
    auto const res = static_cast<uri::uri_status>(context.status);
    EXPECT_EQ(res, uri::uri_status::valid) << to_string(res);
    EXPECT_EQ(context.out.get_scheme(), "urn");
    EXPECT_EQ(context.out.get_path(), "testing");
}

TYPED_TEST(URITests, URIParsingWithWarnings) {
    uri::uri const url = "https:this-is-stupid";
    EXPECT_EQ(url.as_string(), "https://this-is-stupid/");
}

TYPED_TEST(URITests, URIStatusTest) {
    uri::uri_status_type status  = 0;
    status                      |= stl::to_underlying(uri::uri_status::missing_following_solidus);
    status                      |= stl::to_underlying(uri::uri_status::invalid_character);

    EXPECT_TRUE(uri::has_warnings(status));
    EXPECT_TRUE(uri::has_warnings(static_cast<uri::uri_status>(status)));
    EXPECT_FALSE(uri::has_error(static_cast<uri::uri_status>(status)));
    EXPECT_FALSE(uri::has_error(status));
    EXPECT_TRUE(uri::is_valid(status));
    EXPECT_TRUE(uri::is_valid(static_cast<uri::uri_status>(status)));
}

TYPED_TEST(URITests, URIStatusIterator) {
    uri::uri_status_type status  = 0;
    status                      |= stl::to_underlying(uri::uri_status::missing_following_solidus);
    status                      |= stl::to_underlying(uri::uri_status::invalid_character);

    uri::uri_status_type const original = status;

    int index = 0;
    for (auto const item : uri::uri_status_iterator{status}) {
        EXPECT_TRUE(
          item == uri::uri_status::missing_following_solidus || item == uri::uri_status::invalid_character)
          << "Index: " << index << "\n"
          << "Value: " << stl::to_underlying(item) << "\n"
          << "Original: " << original << "\n"
          << to_string(item);
        ++index;
    }
    EXPECT_EQ(index, 2);
}

#if __cpp_lib_ranges && __cpp_lib_ranges_join_with
TYPED_TEST(URITests, URIStatusRange) {
    uri::uri_status_type status  = 0;
    status                      |= stl::to_underlying(uri::uri_status::missing_following_solidus);
    status                      |= stl::to_underlying(uri::uri_status::invalid_character);

    stl::string const errors =
      uri::uri_status_iterator{status} | std::views::transform([](uri::uri_status const cur_status) {
          return to_string(cur_status);
      }) |
      std::views::join_with('\n') | std::ranges::to<std::string>();
#    if __cpp_lib_string_contains
    EXPECT_TRUE(errors.contains(to_string(uri::uri_status::missing_following_solidus))) << errors;
    EXPECT_TRUE(errors.contains(to_string(uri::uri_status::invalid_character))) << errors;
#    endif
}
#endif

TYPED_TEST(URITests, URIStatusIteratorWithValue) {
    uri::uri_status_type status  = 0;
    status                      |= stl::to_underlying(uri::uri_status::missing_following_solidus);
    status                      |= stl::to_underlying(uri::uri_status::invalid_character);
    status                      |= stl::to_underlying(uri::uri_status::valid_queries);

    uri::uri_status_type const original = status;

    int index = 0;
    for (auto const item : uri::uri_status_iterator{status}) {
        EXPECT_TRUE(item == uri::uri_status::missing_following_solidus ||
                    item == uri::uri_status::invalid_character || item == uri::uri_status::valid_queries)
          << "Index: " << index << "\n"
          << "Value: " << stl::to_underlying(item) << "\n"
          << "Original: " << original << "\n"
          << to_string(item);
        ++index;
    }
    EXPECT_EQ(index, 3);
}

TYPED_TEST(URITests, PercentEncodeDecodeIterator) {
    stl::string                out;
    constexpr stl::string_view inp     = "%D8%B3%D9%84%D8%A7%D9%85";
    constexpr stl::string_view decoded = "سلام";
    EXPECT_TRUE(decode_uri_component(inp, out, ALPHA_DIGIT<char>));
    EXPECT_EQ(out, decoded);

    out      = inp;
    auto ptr = out.begin();
    EXPECT_TRUE(decode_uri_component_inplace(ptr, out.end(), ALPHA_DIGIT<char>));
    out.resize(static_cast<stl::size_t>(ptr - out.begin()));
    EXPECT_EQ(out, decoded);

    stl::string output2;
    encode_uri_component(out, output2, ALPHA_DIGIT<char>);
    EXPECT_EQ(output2, inp) << out;

    stl::string output3;
    EXPECT_TRUE(decode_uri_component(output2, output3, ALPHA_DIGIT<char>));
    EXPECT_EQ(output3, decoded) << out;
}

TYPED_TEST(URITests, PercentEncodeDecodePointer) {
    stl::string                out;
    constexpr stl::string_view inp     = "%D8%B3%D9%84%D8%A7%D9%85";
    constexpr stl::string_view decoded = "سلام";
    EXPECT_TRUE(decode_uri_component(inp, out, ALPHA_DIGIT<char>));
    EXPECT_EQ(out, decoded);

    out       = inp;
    auto* ptr = out.data();
    EXPECT_TRUE(decode_uri_component_inplace(ptr,
                                             ptr + out.size(), // NOLINT(*-pro-bounds-pointer-arithmetic)
                                             ALPHA_DIGIT<char>));
    out.resize(static_cast<stl::size_t>(ptr - out.data()));
    EXPECT_EQ(out, decoded);

    stl::string output2;
    encode_uri_component(out, output2, ALPHA_DIGIT<char>);
    EXPECT_EQ(output2, inp) << out;

    stl::string output3;
    EXPECT_TRUE(decode_uri_component(output2, output3, ALPHA_DIGIT<char>));
    EXPECT_EQ(output3, decoded) << out;
}

TYPED_TEST(URITests, BasicURIParsing) {
    constexpr stl::string_view str =
      "https://username:password@example.com:1010/this/is/the/path?query1=one#hash";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_TRUE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::uri_status::has_credentials, uri::get_warning(context.status))
      << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "https");
    EXPECT_EQ(context.out.get_hostname(), "example.com");
    EXPECT_EQ(context.out.get_username(), "username");
    EXPECT_EQ(context.out.get_password(), "password");
    EXPECT_EQ(context.out.get_port(), "1010");
    EXPECT_EQ(context.out.get_path(), "/this/is/the/path");
    EXPECT_EQ(context.out.get_queries(), "query1=one");
    EXPECT_EQ(context.out.get_fragment(), "hash");
    EXPECT_TRUE(context.out.has_scheme());
    EXPECT_TRUE(context.out.has_username());
    // EXPECT_TRUE(context.out.has_authority());
    EXPECT_TRUE(context.out.has_credentials());
    EXPECT_TRUE(context.out.has_hostname());
    EXPECT_TRUE(context.out.has_path());
    EXPECT_TRUE(context.out.has_queries());
    EXPECT_TRUE(context.out.has_fragment());
}

TYPED_TEST(URITests, QueriesEnding) {
    constexpr stl::string_view str = "https://example.com/this/is/the/path?query1=";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_EQ(context.out.get_hostname(), "example.com");
    EXPECT_EQ(context.out.get_path(), "/this/is/the/path");
    if constexpr (TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_queries(), "query1");
    } else {
        EXPECT_EQ(context.out.get_queries(), "query1=");
    }
}

TYPED_TEST(URITests, InvalidSchemes) {
    constexpr stl::array<stl::string_view, 5> strs{
      "https::",
      "http::",
      "file|",
      "file/",
      "urn/",
      // todo: add more
    };

    for (auto const str : strs) {
        auto context = this->template get_context<TypeParam>(str);
        uri::parse_uri(context);

        EXPECT_TRUE(uri::has_error(context.status))
          << to_string(uri::get_value(context.status)) << "\n"
          << str;
    }
}

TYPED_TEST(URITests, ValidSchemes) {
    constexpr stl::array<stl::string_view, 12> strs{
      "file::",      // yep, I know!
      "file:::::::", // I know, I know!
      "https:",
      "https:/",
      "https://",
      "https:///",
      "https:///////////////////////",
      "http:",
      "file:",
      "file:",
      "urn:",

      // binary of "hello world" encoded within:
      "https:/\\\\//\\\\////\\\\\\\\//\\\\/\\\\/\\\\\\\\/\\\\\\\\///\\\\\\\\/\\\\\\\\///\\\\\\\\/"
      "\\\\\\\\\\\\\\\\//\\\\//////\\\\/\\\\/\\\\\\\\\\\\/\\\\\\\\/\\\\\\\\\\\\\\\\/\\\\\\\\\\\\//\\\\//"
      "\\\\\\\\/\\\\\\\\///\\\\\\\\//\\\\//",
      // todo: add more
    };

    for (auto const str : strs) {
        auto context = this->template get_context<TypeParam>(str);
        uri::parse_scheme(context);

        EXPECT_FALSE(uri::has_error(context.status))
          << to_string(uri::get_value(context.status)) << "\n"
          << str;
    }
}

TYPED_TEST(URITests, PathIteratorTest) {
    uri::path_iterator<default_traits> iter{"/page/one"};
    EXPECT_TRUE(iter.check_segment("page"));
    EXPECT_TRUE(iter.check_segment("one"));
    EXPECT_TRUE(iter.at_end());
    iter.reset();
    EXPECT_TRUE(iter.check_segment("page"));
    EXPECT_TRUE(iter.check_segment("one"));
    EXPECT_TRUE(iter.at_end());
}

TYPED_TEST(URITests, PathTraverser) {
    uri::basic_path<stl::string> the_path;
    EXPECT_TRUE(the_path.parse(stl::string_view{"/page/one"}));
    EXPECT_EQ(the_path.size(), 3);
    uri::path_traverser<stl::string> iter{the_path};
    EXPECT_TRUE(iter.check_segment("page")) << iter.segment();
    EXPECT_TRUE(iter.check_segment("one")) << iter.segment();
    EXPECT_TRUE(iter.at_end());
    iter.reset();
    EXPECT_TRUE(iter.check_segment("page")) << iter.segment();
    EXPECT_TRUE(iter.check_segment("one")) << iter.segment();
    EXPECT_TRUE(iter.at_end());
}

TYPED_TEST(URITests, OpaqueHostParser) {
    constexpr stl::string_view str = "urn://this/is/a/path";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "urn");
    EXPECT_EQ(context.out.get_hostname(), "this");
    EXPECT_EQ(context.out.get_path(), "/is/a/path");
}

TYPED_TEST(URITests, OpaqueHostParserWarning) {
    constexpr stl::string_view str = "urn://th%is/is/a/path";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_TRUE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::uri_status::invalid_character, uri::get_warning(context.status))
      << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "urn");
    EXPECT_EQ(context.out.get_hostname(), "th%is");
    EXPECT_EQ(context.out.get_path(), "/is/a/path");
}

TYPED_TEST(URITests, InvalidHostCharacter) {
    constexpr stl::string_view str = "https://th%is/is/a/path";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_FALSE(uri::is_valid(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::invalid_domain_code_point)
      << to_string(uri::get_value(context.status));
}

TYPED_TEST(URITests, OpaqueHostWithIPv6) {
    constexpr stl::string_view str = "ldap://[2001:db8::7]/c=GB?objectClass?one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "ldap");
    EXPECT_EQ(context.out.get_hostname(), "[2001:db8::7]");
    EXPECT_EQ(context.out.get_path(), "/c=GB");
    EXPECT_EQ(context.out.get_queries(), "objectClass?one");
}

TYPED_TEST(URITests, OpaqueHostWithCredentials) {
    constexpr stl::string_view str = "ldap://username:password@[2001:db8::7]:1515/c=GB?objectClass?one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_TRUE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "ldap");
    EXPECT_EQ(context.out.get_hostname(), "[2001:db8::7]");
    EXPECT_EQ(context.out.get_username(), "username");
    EXPECT_EQ(context.out.get_password(), "password");
    EXPECT_EQ(context.out.get_port(), "1515");
    EXPECT_EQ(context.out.get_path(), "/c=GB");
    EXPECT_EQ(context.out.get_queries(), "objectClass?one");
}

TYPED_TEST(URITests, FragmentOnNonSpecialSchemeAsFirstChar) {
    constexpr stl::string_view str = "ldap://#one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "ldap");
    EXPECT_EQ(context.out.get_fragment(), "one");
}

TYPED_TEST(URITests, IPv4AsHost) {
    constexpr stl::string_view str = "https://127.0.0.1/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "https");
    EXPECT_EQ(context.out.get_hostname(), "127.0.0.1");
    EXPECT_EQ(context.out.get_path(), "/page/one");
}

TYPED_TEST(URITests, InvalidIPv4AsHost) {
    constexpr stl::string_view str = "https://12l.0.0.1/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_FALSE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::ip_invalid_character)
      << to_string(uri::get_value(context.status));
}

TYPED_TEST(URITests, PathDot) {
    constexpr stl::string_view str = "https://127.0.0.1/./one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    if (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "/one");
    } else {
        EXPECT_EQ(context.out.get_path(), "/./one");
    }
}

TYPED_TEST(URITests, PathDotNormalized) {
    stl::string const str = "https://127.0.0.1/./one";

    uri::parsing_uri_context_string<stl::string> context{
      .beg = str.begin(),
      .pos = str.begin(),
      .end = str.end(),
    };
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_path(), "/one");
}

TYPED_TEST(URITests, SkipDotButNotSlash) {
    constexpr stl::string_view str = "https://127.0.0.1/page/.";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "/page/");
    } else {
        EXPECT_EQ(context.out.get_path(), "/page/.");
    }
}

TYPED_TEST(URITests, PathDotNormalizedABunch) {
    constexpr stl::string_view str =
      "https://127.0.0.1/..//./one/%2E./%2e/two/././././%2e/%2e/.././three/four/%2e%2e/five/.%2E//%2e";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "//three//");
    } else {
        EXPECT_EQ(context.out.get_path(),
                  "/..//./one/%2E./%2e/two/././././%2e/%2e/.././three/four/%2e%2e/five/.%2E//%2e");
    }
}

TYPED_TEST(URITests, Percent2ECheck) {
    constexpr stl::string_view str =
      "https://127.0.0.1/..//./zero/one/%2E./%2e/two/././././%2e/%2e/.././three/four/%2e%2e/five/.%2E/%2e/"
      "%%2e/%22e/%2ee/%ee/%e2/%e22/2%e/e2%/e22/%%%/222/eee/e2%/%2e2e2e/%e2e2e2e2/ee%/22%";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_TRUE(uri::has_warning(context.status, uri::uri_status::invalid_character))
      << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(
          context.out.get_path(),
          "//zero/three/%%2e/%22e/%2ee/%ee/%e2/%e22/2%e/e2%/e22/%%%/222/eee/e2%/%2e2e2e/%e2e2e2e2/ee%/22%");
    } else {
        EXPECT_EQ(
          context.out.get_path(),
          "/..//./zero/one/%2E./%2e/two/././././%2e/%2e/.././three/four/%2e%2e/five/.%2E/%2e/%%2e/%22e/"
          "%2ee/%ee/%e2/%e22/2%e/e2%/e22/%%%/222/eee/e2%/%2e2e2e/%e2e2e2e2/ee%/22%");
    }
}

TYPED_TEST(URITests, BackingUpOnEmptySegments) {
    constexpr stl::string_view str = "http://example.com////../..";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "//");
    } else {
        EXPECT_EQ(context.out.get_path(), "////../..");
    }
}

TYPED_TEST(URITests, LastEmptySegment) {
    constexpr stl::string_view str = "http://example.com/.//a/a/a/../../";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "//a/");
    } else {
        EXPECT_EQ(context.out.get_path(), "/.//a/a/a/../../");
    }
}

TYPED_TEST(URITests, PercentDecodingInDomains) {
    constexpr stl::string_view str = "https://www.ex%21ample.com/";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_hostname(), "www.ex!ample.com");
    } else {
        EXPECT_EQ(context.out.get_hostname(), "www.ex%21ample.com");
    }
}

TYPED_TEST(URITests, PathDotNormalizedABunchWithNewLines) {
    constexpr stl::string_view str =
      "https://127.0.0.1/.\r.//./one/%2\nE./%\n2e/two/./.\n/\n././%2e\n/%2e/.././three/f\nour/\r%2e%\r2e/"
      "five/\r.\r%2E/%2e";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_TRUE(uri::has_warning(context.status, uri::uri_status::invalid_character))
      << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "//three/");
    } else {
        EXPECT_EQ(context.out.get_path(),
                  "/.\r.//./one/%2\nE./%\n2e/two/./.\n/\n././%2e\n/%2e/.././three/f\nour/\r%2e%\r2e/five/"
                  "\r.\r%2E/%2e");
    }
}

TYPED_TEST(URITests, DoubleAtSign) {
    constexpr stl::string_view str = "http://username@username@127.0.0.1/?one==a#hash";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_username(), "username%40username");
    } else {
        EXPECT_EQ(context.out.get_username(), "username@username");
    }
    EXPECT_EQ(context.out.get_path(), "/");
    EXPECT_EQ(context.out.get_hostname(), "127.0.0.1");
    EXPECT_EQ(context.out.get_queries(), "one==a");
    EXPECT_EQ(context.out.get_fragment(), "hash");
}

TYPED_TEST(URITests, DoubleEqual) {
    constexpr stl::string_view str = "http://127.0.0.1/?one==a&&&&page=====one#hash";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_EQ(context.out.get_path(), "/");
    EXPECT_EQ(context.out.get_hostname(), "127.0.0.1");
    if constexpr (TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_queries(), "one==a&page=====one");
    } else {
        EXPECT_EQ(context.out.get_queries(), "one==a&&&&page=====one");
    }
    EXPECT_EQ(context.out.get_fragment(), "hash");
}

TYPED_TEST(URITests, EmptyQueryName) {
    constexpr stl::string_view str = "http://127.0.0.1/?=a&&&&page=====one#hash";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_EQ(context.out.get_path(), "/");
    EXPECT_EQ(context.out.get_hostname(), "127.0.0.1");
    if constexpr (TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_queries(), "=a&page=====one");
    } else {
        EXPECT_EQ(context.out.get_queries(), "=a&&&&page=====one");
    }
    EXPECT_EQ(context.out.get_fragment(), "hash");
}

TYPED_TEST(URITests, DontGetFooledURI) {
    constexpr stl::string_view str = "https://example.com:8080@real.example.org/";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_EQ(context.out.get_username(), "example.com");
    EXPECT_EQ(context.out.get_password(), "8080");
    EXPECT_EQ(context.out.get_path(), "/");
    EXPECT_EQ(context.out.get_hostname(), "real.example.org");
}

TYPED_TEST(URITests, WindowsDriveLetterAsHost) {
    constexpr stl::string_view str = "file://C|\\windows";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::has_warning(context.status, uri::uri_status::windows_drive_letter_as_host))
      << to_string(uri::get_warning(context.status));
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_path(), "/C:/windows");
    } else if constexpr (TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "/C|/windows");
    } else {
        EXPECT_EQ(context.out.get_path(), "/C|\\windows");
    }
}

TYPED_TEST(URITests, WindowsDriveLetterAsHostWithNewLine) {
    constexpr stl::string_view str = "file://\nC\r|\t\\\twind\tows";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::has_warning(context.status, uri::uri_status::windows_drive_letter_as_host))
      << to_string(uri::get_warning(context.status));
    EXPECT_TRUE(uri::has_warning(context.status, uri::uri_status::invalid_character))
      << to_string(uri::get_warning(context.status));
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_path(), "/C:/windows");
    } else if constexpr (TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "/\nC\r|\t/\twind\tows");
    } else {
        EXPECT_EQ(context.out.get_path(), "/\nC\r|\t\\\twind\tows");
    }
}

TYPED_TEST(URITests, WindowsDriveLetterUsed) {
    constexpr stl::string_view str = "file:///C|\\windows";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::has_warning(context.status, uri::uri_status::windows_drive_letter_used))
      << to_string(uri::get_warning(context.status));
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_path(), "/C:/windows");
    } else if constexpr (TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "/C|/windows");
    } else {
        EXPECT_EQ(context.out.get_path(), "/C|\\windows");
    }
}

TYPED_TEST(URITests, WindowsDriveLetterUsedStrict) {
    constexpr stl::string_view str = "file:///C|\\windows";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri<uri::strict_uri_parsing_options>(context);
    EXPECT_TRUE(uri::has_warning(context.status, uri::uri_status::reverse_solidus_used))
      << to_string(uri::get_warning(context.status));
    if constexpr (TypeParam::is_segregated || TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_path(), "/C|/windows");
    } else {
        EXPECT_EQ(context.out.get_path(), "/C|\\windows");
    }
}

TYPED_TEST(URITests, WindowsDriveLetterAsHostUppercasedScheme) {
    constexpr stl::string_view str = "FiLE://C|\\windows";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::has_warning(context.status, uri::uri_status::windows_drive_letter_as_host))
      << to_string(uri::get_warning(context.status));
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_path(), "/C:/windows");
    } else if constexpr (TypeParam::is_segregated) {
        EXPECT_EQ(context.out.get_path(), "/C|/windows");
    } else {
        EXPECT_EQ(context.out.get_path(), "/C|\\windows");
    }
}

TYPED_TEST(URITests, EmptyCredentials) {
    constexpr stl::array<stl::string_view, 4> strs{
      "https://@example.com/",
      "https://:@example.com/",

      // opaque hosts:
      "opaque://@example.com/",
      "opaque://:@example.com/",
    };

    for (auto const str : strs) {
        auto context = this->template get_context<TypeParam>(str);
        uri::parse_uri(context);
        EXPECT_EQ(context.out.get_hostname(), "example.com") << str;
        EXPECT_EQ(context.out.get_username(), "") << str;
        EXPECT_EQ(context.out.get_password(), "") << str;
    }
}

TYPED_TEST(URITests, AtSign2Percent40) {
    constexpr stl::string_view str = "https://@@:@:@::::@@a:1/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_EQ(context.out.get_hostname(), "a");
    EXPECT_EQ(context.out.get_port(), "1");
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_username(), "%40%40");
        EXPECT_EQ(context.out.get_password(), "%40%3A%40%3A%3A%3A%3A%40");
    } else {
        EXPECT_EQ(context.out.get_username(), "@@");
        EXPECT_EQ(context.out.get_password(), "@:@::::@");
    }
}

TYPED_TEST(URITests, HostMissing) {
    constexpr stl::array<stl::string_view, 8> strs{
      "https://username@:8080/",
      "https://username:password@/",
      "https:///",
      "https://:8080/",
      "https://username@more-username@/",
      "https://username@more-username:password@/",
      "https://username@more-username:password@:8080/",
      "https://username@@:8080/",

      // opaque hosts: (opaque hosts don't have ports, nor empty-host is an error)
      // "opaque://username@:8080/",
      // "opaque://username:password@/",
      // "opaque:///",
      // "opaque://:8080/",
      // "opaque://username@more-username@/",
      // "opaque://username@more-username:password@/",
      // "opaque://username@more-username:password@:8080/",
      // "opaque://username@@:8080/",
    };

    for (auto const str : strs) {
        auto context = this->template get_context<TypeParam>(str);
        uri::parse_uri(context);
        EXPECT_FALSE(uri::is_valid(context.status))
          << str << "\n"
          << to_string(uri::get_value(context.status));
        EXPECT_EQ(uri::get_value(context.status), uri::uri_status::host_missing)
          << str << "\n"
          << to_string(uri::get_value(context.status));
    }
}

TYPED_TEST(URITests, LocalIPv4Addr) {
    constexpr stl::array<stl::string_view, 17 * 2> strs{
      "https://0x0000000007F.0X1",
      "https://127.0.0.1/",
      "https://0x7F.1/",
      "https://0x7f000001",
      "https://127.0.0x0.1",
      "https://127.0X0.0x0.1",
      "https://127.0X0.0x0.0x1",
      "https://127.0.0x0.0x0000000000000000000000000000000000000000000000000000000000000001",
      "https://0x7F.0x00000000000000000000000001",
      "https://0x000000000000000007F.0x00000000000000000000000001",
      "https://0x000000000000000007F.0.0x00000000000000000000000001",
      "https://0x7f.0.0.0x1",
      "https://0x7F.0.0x000.0x1",
      "https://2130706433",
      "https://127.1",
      "https://127.0x00.1",
      "https://127.0x000000000000000.0.1",

      // with path:
      "https://127.0.0.1/page/one/.././.",
      "https://0x7F.1/page/one/.././.",
      "https://0x7f000001/page/one/.././.",
      "https://0x0000000007F.0X1/page/one/.././.",
      "https://127.0.0x0.1/page/one/.././.",
      "https://127.0X0.0x0.1/page/one/.././.",
      "https://127.0X0.0x0.0x1/page/one/.././.",
      "https://127.0.0x0.0x0000000000000000000000000000000000000000000000000000000000000001/page/one/.././.",
      "https://0x7F.0x00000000000000000000000001/page/one/.././.",
      "https://0x000000000000000007F.0x00000000000000000000000001/page/one/.././.",
      "https://0x000000000000000007F.0.0x00000000000000000000000001/page/one/.././.",
      "https://0x7f.0.0.0x1/page/one/.././.",
      "https://0x7F.0.0x000.0x1/page/one/.././.",
      "https://2130706433/page/one/.././.",
      "https://127.1/page/one/.././.",
      "https://127.0x00.1/page/one/.././.",
      "https://127.0x000000000000000.0.1/page/one/.././.",
    };

    for (auto const str : strs) {
        auto context = this->template get_context<TypeParam>(str);
        uri::parse_uri(context);
        EXPECT_TRUE(uri::is_valid(context.status))
          << str << "\n"
          << to_string(uri::get_value(context.status));
        if constexpr (TypeParam::is_modifiable) {
            EXPECT_EQ(context.out.get_hostname(), "127.0.0.1") << str;
        }
    }
}

TYPED_TEST(URITests, LocalIPv4AddrTrailingDots) {
    constexpr stl::array<stl::string_view, 17> strs{
      // with dots at the end:
      "https://0x7f000001..",
      "https://127.0.0.1...../",
      "https://0x7f.1..../",
      "https://0x0000000007f.0x1...",
      "https://127.0.0x0.1............",
      "https://127.0x0.0x0.1.............",
      "https://127.0x0.0x0.0x1..",
      "https://127.0.0x0.0x0000000000000000000000000000000000000000000000000000000000000001.........",
      "https://0x7f.0x00000000000000000000000001..",
      "https://0x000000000000000007f.0x00000000000000000000000001...",
      "https://0x000000000000000007f.0.0x00000000000000000000000001.....",
      "https://0x7f.0.0.0x1..........",
      "https://0x7f.0.0x000.0x1................................................................",
      "https://2130706433.................",
      "https://127.1............",
      "https://127.0x00.1........................",
      "https://127.0x000000000000000.0.1................",
    };

    for (auto const str : strs) {
        auto strict_context = this->template get_context<TypeParam>(str);
        uri::parse_uri(strict_context);
        EXPECT_FALSE(uri::is_valid(strict_context.status))
          << str << "\n"
          << to_string(uri::get_value(strict_context.status));

        // todo: strict_context should not be ipv4 localhost, write a test for that

        auto loose_context = this->template get_context<TypeParam>(str);
        uri::parse_uri<uri::loose_uri_parsing_options>(loose_context);
        EXPECT_TRUE(uri::is_valid(loose_context.status))
          << str << "\n"
          << to_string(uri::get_value(loose_context.status));
        if constexpr (TypeParam::is_modifiable) {
            EXPECT_EQ(loose_context.out.get_hostname(), "127.0.0.1") << str;
        }
    }
}

TYPED_TEST(URITests, IPv4EndingWithX) {
    constexpr stl::string_view str = "https://127.0.0.0x/";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_hostname(), "127.0.0.0");
    } else {
        EXPECT_EQ(context.out.get_hostname(), "127.0.0.0x");
    }
}

TYPED_TEST(URITests, StartingAndEndingWithX) {
    constexpr stl::string_view str = "https://0x/";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_hostname(), "0.0.0.0");
    } else {
        EXPECT_EQ(context.out.get_hostname(), "0x");
    }
}

// TYPED_TEST(URITests, PunnycodeBasic) {
//     constexpr stl::string_view str = "http://☕.example";
//
//     auto context = this->template get_context<TypeParam>(str);
//     uri::parse_uri(context);
//     if constexpr (TypeParam::is_modifiable) {
//         EXPECT_EQ(context.out.get_hostname(), "xn--53h.example");
//     } else {
//         EXPECT_EQ(context.out.get_hostname(), "☕.example");
//     }
// }

// TYPED_TEST(URITests, FileSchemePunnycodeBasic) {
//     constexpr stl::string_view str = "file://☕.example";
//
//     auto context = this->template get_context<TypeParam>(str);
//     uri::parse_uri(context);
//     if constexpr (TypeParam::is_modifiable) {
//         EXPECT_EQ(context.out.get_hostname(), "xn--53h.example");
//     } else {
//         EXPECT_EQ(context.out.get_hostname(), "☕.example");
//     }
// }

TYPED_TEST(URITests, IPv6WithCredentials) {
    constexpr stl::string_view str = "https://user:pass@[::1]:8080/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_EQ(context.out.get_hostname(), "[::1]");
    EXPECT_EQ(context.out.get_username(), "user");
    EXPECT_EQ(context.out.get_password(), "pass");
    EXPECT_EQ(context.out.get_path(), "/page/one");
}

TYPED_TEST(URITests, PortLikePassword) {
    constexpr stl::string_view str = "https://user:123@host/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_EQ(context.out.get_hostname(), "host");
    EXPECT_FALSE(context.out.has_port());
    EXPECT_EQ(context.out.get_username(), "user");
    EXPECT_EQ(context.out.get_password(), "123");
    EXPECT_EQ(context.out.get_path(), "/page/one");
}

TYPED_TEST(URITests, InvlaidPort) {
    constexpr stl::string_view str = "https://user:123@host:invalid/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_FALSE(uri::is_valid(context.status)) << str << "\n" << to_string(uri::get_value(context.status));
    EXPECT_EQ(uri::uri_status::port_invalid, uri::get_value(context.status))
      << str << "\n"
      << to_string(uri::get_value(context.status));
}

TYPED_TEST(URITests, InvlaidPort2) {
    constexpr stl::string_view str = "https://user:123@host:invalid[/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_FALSE(uri::is_valid(context.status)) << str << "\n" << to_string(uri::get_value(context.status));
    EXPECT_EQ(uri::uri_status::port_invalid, uri::get_value(context.status))
      << str << "\n"
      << to_string(uri::get_value(context.status));
}

TYPED_TEST(URITests, OutOfRangePort) {
    constexpr stl::string_view str = "https://user:123@host:77777/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_FALSE(uri::is_valid(context.status)) << str << "\n" << to_string(uri::get_value(context.status));
    EXPECT_EQ(uri::uri_status::port_out_of_range, uri::get_value(context.status))
      << str << "\n"
      << to_string(uri::get_value(context.status));
}

TYPED_TEST(URITests, FileSchemeBasic) {
    constexpr stl::string_view str = "file:///page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status)) << str << "\n" << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_path(), "/page/one");
}

TYPED_TEST(URITests, FileSchemeWithHost) {
    constexpr stl::string_view str = "file://0x7f.1/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status)) << str << "\n" << to_string(uri::get_value(context.status));
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_hostname(), "127.0.0.1");
    } else {
        EXPECT_EQ(context.out.get_hostname(), "0x7f.1");
    }
    EXPECT_EQ(context.out.get_path(), "/page/one");
}

TYPED_TEST(URITests, LocalhostFileScheme) {
    constexpr stl::string_view str = "file://localhost/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status)) << str << "\n" << to_string(uri::get_value(context.status));
    EXPECT_FALSE(context.out.has_hostname()) << "'localhost' hostname for 'file:' scheme gets removed.";
    EXPECT_EQ(context.out.get_path(), "/page/one");
}

TYPED_TEST(URITests, LocalhostFileSchemeStrict) {
    constexpr stl::string_view str = "file://localhost/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri<uri::strict_uri_parsing_options>(context);
    EXPECT_TRUE(uri::is_valid(context.status)) << str << "\n" << to_string(uri::get_value(context.status));
    EXPECT_FALSE(context.out.has_fragment()) << "This url doesn't have a hash.";
    EXPECT_FALSE(context.out.has_hostname()) << "localhost for file: scheme gets removed.";
    EXPECT_EQ(context.out.get_path(), "localhost/page/one");
    // todo: should this have a warning too?
}

TYPED_TEST(URITests, InsaneUrl) {
    auto const ctx = this->template parse_from_string<TypeParam>("e:@EEEE");
    EXPECT_TRUE(uri::is_valid(ctx.status));
    EXPECT_EQ(ctx.out.get_scheme(), "e");
    EXPECT_EQ(ctx.out.get_username(), "");
    EXPECT_EQ(ctx.out.get_password(), "");
    EXPECT_EQ(ctx.out.get_hostname(), "");
    EXPECT_EQ(ctx.out.get_port(), "");
    EXPECT_EQ(ctx.out.get_path(), "@EEEE");
}

TYPED_TEST(URITests, EmptyURI) {
    auto ctx = this->template parse_from_string<TypeParam>("");
    EXPECT_FALSE(uri::is_valid(ctx.status));
    EXPECT_EQ(uri::get_value(ctx.status), uri::uri_status::empty_string);
}

TYPED_TEST(URITests, HashOnly) {
    auto ctx = this->template parse_from_string<TypeParam>("#hash");
    EXPECT_FALSE(uri::is_valid(ctx.status));

    auto ctx2 = this->template parse_from_string<TypeParam>("?#hash");
    EXPECT_FALSE(uri::is_valid(ctx2.status));
}

TYPED_TEST(URITests, ToLowered) {
    auto url = this->template parse_from_string<TypeParam>("HTTP://EXAMPLE.COM");
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(url.out.get_scheme(), "http");
        EXPECT_EQ(url.out.get_hostname(), "example.com");
        // EXPECT_EQ(url.get_href(), "http://example.com/");
    } else {
        EXPECT_EQ(url.out.get_scheme(), "HTTP");
        EXPECT_EQ(url.out.get_hostname(), "EXAMPLE.COM");
        // EXPECT_EQ(url.out.get_href(), "HTTP://EXAMPLE.COM/");
    }
}

/// IPv4 host parsing is different from inet_pton4, so we need to test it separately
TYPED_TEST(URITests, NormalHostIPv4) {
    // NOLINTBEGIN(*-avoid-c-arrays)
    static constexpr stl::string_view valid_ipv4s[]{
      "0.0.0.0",         "192.168.1.1",     "255.255.255.255", "192.0.2.1",       "198.51.100.2",
      "203.0.113.3",     "10.0.0.4",        "172.16.0.5",      "192.168.0.6",     "127.0.0.7",
      "169.254.0.8",     "224.0.0.9",       "239.255.255.10",  "128.0.0.11",      "191.255.255.12",
      "223.255.255.13",  "240.0.0.14",      "255.255.255.15",  "1.2.3.4",         "5.6.7.8",
      "9.10.11.12",      "13.14.15.16",     "17.18.19.20",     "21.22.23.24",     "25.26.27.28",
      "29.30.31.32",     "33.34.35.36",     "37.38.39.40",     "41.42.43.44",     "45.46.47.48",
      "49.50.51.52",     "53.54.55.56",     "57.58.59.60",     "61.62.63.64",     "65.66.67.68",
      "69.70.71.72",     "73.74.75.76",     "77.78.79.80",     "81.82.83.84",     "85.86.87.88",
      "89.90.91.92",     "93.94.95.96",     "97.98.99.100",    "101.102.103.104", "105.106.107.108",
      "109.110.111.112", "113.114.115.116", "117.118.119.120", "121.122.123.124", "125.126.127.128",
      "129.130.131.132", "133.134.135.136", "137.138.139.140", "141.142.143.144", "145.146.147.148",
      "149.150.151.152", "153.154.155.156", "157.158.159.160", "161.162.163.164", "165.166.167.168",
      "169.170.171.172", "173.174.175.176", "177.178.179.180", "181.182.183.184", "185.186.187.188",
      "189.190.191.192", "193.194.195.196", "197.198.199.200", "201.202.203.204", "205.206.207.208",
      "209.210.211.212", "213.214.215.216", "217.218.219.220", "221.222.223.224", "225.226.227.228",
      "229.230.231.232", "233.234.235.236", "237.238.239.240", "241.242.243.244", "245.246.247.248",
      "249.250.251.252", "253.254.255.0",   "255.254.253.0",   "1.254.253.0",     "255.1.253.0",
      "255.254.1.0",     "255.254.253.1",   "1.1.253.0",       "255.1.1.0",       "255.254.1.1",
      "1.254.1.0",       "1.1.1.0",         "255.255.1.0",     "255.255.254.0",   "255.255.255.1",
      "255.255.254.1",   "255.254.255.1",   "254.255.255.1",   "255.1.255.0",     "1.255.254.0",
      "1.254.255.0",     "254.1.255.0",     "254.255.1.0",
    };

    static constexpr stl::string_view invalid_ipv4s[]{
      "0.0.0.256",
      "256.255.255.255",
      "256.0.0.0",
      "192.168. 224.0",
      "192.168.224.0 1",
    };
    stl::uint8_t ip_octets[4]{};

    for (auto const& _ip : valid_ipv4s) {
        auto       context         = this->template get_context<TypeParam>(_ip);
        bool const should_continue = uri::details::parse_host_ipv4<uri::loose_uri_parsing_options>(
          _ip.begin(),
          _ip.end(),
          ip_octets,
          context);

        EXPECT_TRUE(should_continue) << "ip: " << _ip << "; compiled ip: " << static_cast<int>(ip_octets[0])
                                     << "." << static_cast<int>(ip_octets[1]) << "."
                                     << static_cast<int>(ip_octets[2]) << "."
                                     << static_cast<int>(ip_octets[3]);
        EXPECT_EQ(ipv4{_ip}, ip_octets)
          << "Expected IP: " << ipv4{_ip}.string() << "\n"
          << "Parsed IP: " << static_cast<int>(ip_octets[0]) << "." << static_cast<int>(ip_octets[1]) << "."
          << static_cast<int>(ip_octets[2]) << "." << static_cast<int>(ip_octets[3]);
    }

    for (auto const& _ip : invalid_ipv4s) {
        auto       context         = this->template get_context<TypeParam>(_ip);
        bool const should_continue = uri::details::parse_host_ipv4<uri::loose_uri_parsing_options>(
          _ip.begin(),
          _ip.end(),
          ip_octets,
          context);

        EXPECT_FALSE(should_continue)
          << "ip: " << _ip << "; compiled ip: " << static_cast<int>(ip_octets[0]) << "."
          << static_cast<int>(ip_octets[1]) << "." << static_cast<int>(ip_octets[2]) << "."
          << static_cast<int>(ip_octets[3]);
    }
    // NOLINTEND(*-avoid-c-arrays)
}

/// IPv4 host parsing is different from inet_pton4, so we need to test it separately
TYPED_TEST(URITests, AbormalHostIPv4Loose) {
    // NOLINTBEGIN(*-avoid-c-arrays)
    static constexpr stl::pair<stl::string_view, ipv4> valid_ipv4s[]{
      stl::pair{                           stl::string_view{"0000000.0.0.0"},        ipv4::any()},
      {                                            "0x000000.0.0.0",        ipv4::any()},
      {                                           "0x000000.0.0.0.",        ipv4::any()},
      {TypeParam::is_modifiable ? "0X0000.0x.0.0" : "0x0000.0x.0.0",        ipv4::any()},
      {                                                         "0",        ipv4::any()},
      {                                                        "0x",        ipv4::any()},
      {                      TypeParam::is_modifiable ? "0X" : "0x",        ipv4::any()},
      {                                                     "00000",        ipv4::any()},
      {                                                    "00000.",        ipv4::any()},
      {                                                    "0x0000",        ipv4::any()},
      {                                                  "0x0000..",        ipv4::any()},
      {                                                 "0x0000...",        ipv4::any()},
      // {                                                          "",        ipv4::any()},
      {                                                         ".",        ipv4::any()},

      // 127.0.0.1 localhost IPs
      {                                                 "127.0.0.1",   ipv4::loopback()},
      {                                                "127.0.0.1.",   ipv4::loopback()},
      {                                               "127.0.0.1..",   ipv4::loopback()},
      {                                              "127.0.0.1...",   ipv4::loopback()},
      {                                                    "0x7f.1",   ipv4::loopback()},
      {                                                "2130706433",   ipv4::loopback()},
      {                                                  "0x7f.1..",   ipv4::loopback()},
      {                                                   "0x7f.1.",   ipv4::loopback()},
      {                                                  "0x7f.1..",   ipv4::loopback()},
      {                                                "0x7f000001",   ipv4::loopback()},
      {                                               "0x7f000001.",   ipv4::loopback()},
      {                                              "0x7f000001..",   ipv4::loopback()},
      {                                         "0x0000000007f.0x1",   ipv4::loopback()},
      {                                               "127.0.0x0.1",   ipv4::loopback()},
      {                                             "127.0x0.0x0.1",   ipv4::loopback()},
      {                                           "127.0x0.0x0.0x1",   ipv4::loopback()},
      {                              "127.0.0x0.0x0000000000000001",   ipv4::loopback()},
      {                             "127.0.0x0.0x0000000000000001.",   ipv4::loopback()},
      {                                 "0x7f.0x000000000000000001",   ipv4::loopback()},
      {                                "0x7f.0x000000000000000001.",   ipv4::loopback()},
      {                               "0x7f.0x000000000000000001..",   ipv4::loopback()},
      {                              "0x7f.0x000000000000000001...",   ipv4::loopback()},
      {                 "0x000000000000000007f.0x00000000000000001",   ipv4::loopback()},
      {               "0x000000000000000007f.0.0x00000000000000001",   ipv4::loopback()},
      {              "0x000000000000000007f.0.0x00000000000000001.",   ipv4::loopback()},
      {             "0x000000000000000007f.0.0x00000000000000001..",   ipv4::loopback()},
      {                                              "0x7f.0.0.0x1",   ipv4::loopback()},
      {                                          "0x7f.0.0x000.0x1",   ipv4::loopback()},
      {                                                "2130706433",   ipv4::loopback()},
      {                                               "2130706433.",   ipv4::loopback()},
      {                                              "2130706433..",   ipv4::loopback()},
      {                                                     "127.1",   ipv4::loopback()},
      {                                                "127.0x00.1",   ipv4::loopback()},
      {                                 "127.0x000000000000000.0.1",   ipv4::loopback()},
      {                                "127.0x000000000000000.0.1.",   ipv4::loopback()},
      {                              "127.0x000000000000000.0.1...",   ipv4::loopback()},

      // other
      {                                                    "000123",  ipv4{0, 0, 0, 83}},
      {                                                      "0xff", ipv4{0, 0, 0, 255}},
      {                                                     "1.256",   ipv4{1, 0, 1, 0}},
      {                                                "1.256.....",   ipv4{1, 0, 1, 0}},
      {                                                "4294967295",  ipv4::broadcast()},
      {                                               "4294967295.",  ipv4::broadcast()},
      {                                              "4294967295..",  ipv4::broadcast()},
      {                                             "4294967295...",  ipv4::broadcast()},
    };

    static constexpr stl::string_view invalid_ipv4s[]{
      "0.0.0.256",
      "0X000000.00x.0.0",
      "bad",
      "aaaaaaaaaaaaaa.123",
      "f.123",
      "0xfffffffffffffffffffff.123",
      "0.com",
      "00x0",
      "0X0000.00x.0.0",
      "x",
      "X",
      "00x",
      "123x",
      "x123",
      "x",
      "xxx",
      "0xx",
      "00xx",
      "0XX",
      "0.0.0.256",
      "256.255.255.255",
      "255.256.255.255",
      "255.255.255.256",
      "256.1",
      "256.0.0.0",
      "192.168. 224.0",
      "192.168.224.0 1",
      "4294967296",
      "1.4294967296",
      ".4294967296",
      ".1",
      " ",
      "",
      "....",
      "192.168..1",
      "192...1",
      "192...1.",
      "..1",
      "..0",
      ".1..",
      ".0..",
      ".0x1",
      "0..0x1",
      // with dot at the end:

      "0.0.0.256.",
      "0X000000.00x.0.0.",
      "bad.",
      "0.com.",
      "00x0.",
      "0X0000.00x.0.0.",
      "x.",
      "X.",
      "00x.",
      "123x.",
      "x123.",
      "x.",
      "xxx.",
      "0xx.",
      "00xx.",
      "0XX.",
      "0.0.0.256.",
      "256.255.255.255.",
      "255.256.255.255.",
      "255.255.255.256.",
      "256.1.",
      "256.0.0.0.",
      "192.168. 224.0.",
      "4294967296.",
      ".1.",
      "192.168.224.0 1.",
    };
    stl::uint8_t ip_octets[4]{};

    for (auto const& [_ip, expected_ip] : valid_ipv4s) {
        auto       context         = this->template get_context<TypeParam>(_ip);
        bool const should_continue = uri::details::parse_host_ipv4<uri::loose_uri_parsing_options>(
          _ip.begin(),
          _ip.end(),
          ip_octets,
          context);

        EXPECT_TRUE(should_continue)
          << "Original IP String: " << _ip << "\nParsed IP: " << static_cast<int>(ip_octets[0]) << "."
          << static_cast<int>(ip_octets[1]) << "." << static_cast<int>(ip_octets[2]) << "."
          << static_cast<int>(ip_octets[3]);
        EXPECT_EQ(expected_ip, ip_octets)
          << "Original IP String: " << _ip << "\n"
          << "Expected IP: " << expected_ip.string() << "\n"
          << "Parsed IP: " << static_cast<int>(ip_octets[0]) << "." << static_cast<int>(ip_octets[1]) << "."
          << static_cast<int>(ip_octets[2]) << "." << static_cast<int>(ip_octets[3]);
    }

    for (auto const& _ip : invalid_ipv4s) {
        auto       context = this->template get_context<TypeParam>(_ip);
        bool const should_continue =
          uri::details::parse_host_ipv4(_ip.begin(), _ip.end(), ip_octets, context);

        EXPECT_FALSE(should_continue)
          << "Original IP String: '" << _ip << "'\nParsed IP: " << static_cast<int>(ip_octets[0]) << "."
          << static_cast<int>(ip_octets[1]) << "." << static_cast<int>(ip_octets[2]) << "."
          << static_cast<int>(ip_octets[3]);
    }
    // NOLINTEND(*-avoid-c-arrays)
}

TYPED_TEST(URITests, AbormalHostIPv4) {
    // NOLINTBEGIN(*-avoid-c-arrays)
    static constexpr stl::pair<stl::string_view, ipv4> valid_ipv4s[]{
      stl::pair{                           stl::string_view{"0000000.0.0.0"},        ipv4::any()},
      {                                            "0x000000.0.0.0",        ipv4::any()},
      {                                           "0x000000.0.0.0.",        ipv4::any()},
      {TypeParam::is_modifiable ? "0X0000.0x.0.0" : "0x0000.0x.0.0",        ipv4::any()},
      {                                                         "0",        ipv4::any()},
      {                                                        "0x",        ipv4::any()},
      {                      TypeParam::is_modifiable ? "0X" : "0x",        ipv4::any()},
      {                                                     "00000",        ipv4::any()},
      {                                                    "00000.",        ipv4::any()},
      {                                                    "0x0000",        ipv4::any()},
      {                                                   "0x0000.",        ipv4::any()},
      {                                                   "0x0000.",        ipv4::any()},
      {                                                         ".",        ipv4::any()},

      // 127.0.0.1 localhost IPs
      {                                                 "127.0.0.1",   ipv4::loopback()},
      {                                                "127.0.0.1.",   ipv4::loopback()},
      {                                                    "0x7f.1",   ipv4::loopback()},
      {                                                "2130706433",   ipv4::loopback()},
      {                                                   "0x7f.1.",   ipv4::loopback()},
      {                                                "0x7f000001",   ipv4::loopback()},
      {                                               "0x7f000001.",   ipv4::loopback()},
      {                                         "0x0000000007f.0x1",   ipv4::loopback()},
      {                                               "127.0.0x0.1",   ipv4::loopback()},
      {                                             "127.0x0.0x0.1",   ipv4::loopback()},
      {                                           "127.0x0.0x0.0x1",   ipv4::loopback()},
      {                              "127.0.0x0.0x0000000000000001",   ipv4::loopback()},
      {                             "127.0.0x0.0x0000000000000001.",   ipv4::loopback()},
      {                                 "0x7f.0x000000000000000001",   ipv4::loopback()},
      {                                "0x7f.0x000000000000000001.",   ipv4::loopback()},
      {                 "0x000000000000000007f.0x00000000000000001",   ipv4::loopback()},
      {               "0x000000000000000007f.0.0x00000000000000001",   ipv4::loopback()},
      {              "0x000000000000000007f.0.0x00000000000000001.",   ipv4::loopback()},
      {                                              "0x7f.0.0.0x1",   ipv4::loopback()},
      {                                          "0x7f.0.0x000.0x1",   ipv4::loopback()},
      {                                                "2130706433",   ipv4::loopback()},
      {                                               "2130706433.",   ipv4::loopback()},
      {                                                     "127.1",   ipv4::loopback()},
      {                                                "127.0x00.1",   ipv4::loopback()},
      {                                 "127.0x000000000000000.0.1",   ipv4::loopback()},
      {                                "127.0x000000000000000.0.1.",   ipv4::loopback()},

      // other
      {                                                    "000123",  ipv4{0, 0, 0, 83}},
      {                                                      "0xff", ipv4{0, 0, 0, 255}},
      {                                                     "1.256",   ipv4{1, 0, 1, 0}},
      {                                                "4294967295",  ipv4::broadcast()},
      {                                               "4294967295.",  ipv4::broadcast()},
    };

    static constexpr stl::string_view invalid_ipv4s[]{
      "0000000.0.0.0..",
      "0x000000.0.0.0..",
      "0x0000.0x.0.0..",
      "0..",
      "0x..",
      "0x..",
      "00000..",
      "00000..",
      ".0x0000",
      ".0x0000.",
      "..0x0000.",

      // 127.0.0.1 localhost IPs
      "127.0.0.1..",
      "127.0.0.1...",
      "0x7f.1..",
      "2130706433..",
      "0x7f.1...",
      "0x7f000001..",
      "0x7f000001...",
      "0x0000000007f.0x1..",
      "127.0.0x0.1..",
      "127.0x0.0x0.1..",
      "127.0x0.0x0.0x1..",
      "127.0.0x0.0x0000000000000001..",
      "127.0.0x0.0x0000000000000001...",
      "0x7f.0x000000000000000001..",
      "0x7f.0x000000000000000001...",
      "0x000000000000000007f.0x00000000000000001..",
      "0x000000000000000007f.0.0x00000000000000001..",
      "0x000000000000000007f.0.0x00000000000000001...",
      "0x7f.0.0.0x1..",
      "0x7f.0.0x000.0x1..",
      "2130706433..",
      "2130706433...",
      "127.1..",
      "127.0x00.1..",
      "127.0x000000000000000.0.1..",
      "127.0x000000000000000.0.1...",

      // other
      "000123....",
      "0xff....",
      "1.256....",
      "4294967295....",
      "4294967295.....",
      "0.0.0.256",
      "0X000000.00x.0.0",
      "bad",
      "aaaaaaaaaaaaaa.123",
      "f.123",
      "0xfffffffffffffffffffff.123",
      "0.com",
      "00x0",
      "0X0000.00x.0.0",
      "x",
      "X",
      "00x",
      "123x",
      "x123",
      "x",
      "xxx",
      "0xx",
      "00xx",
      "0XX",
      "0.0.0.256",
      "256.255.255.255",
      "255.256.255.255",
      "255.255.255.256",
      "256.1",
      "256.0.0.0",
      "192.168. 224.0",
      "192.168.224.0 1",
      "4294967296",
      "1.4294967296",
      ".4294967296",
      ".1",
      " ",
      "",
      "....",
      "192.168..1",
      "192...1",
      "192...1.",
      "..1",
      "..0",
      ".1..",
      ".0..",
      ".0x1",
      "0..0x1",
      // with dot at the end:

      "0.0.0.256.",
      "0X000000.00x.0.0.",
      "bad.",
      "0.com.",
      "00x0.",
      "0X0000.00x.0.0.",
      "x.",
      "X.",
      "00x.",
      "123x.",
      "x123.",
      "x.",
      "xxx.",
      "0xx.",
      "00xx.",
      "0XX.",
      "0.0.0.256.",
      "256.255.255.255.",
      "255.256.255.255.",
      "255.255.255.256.",
      "256.1.",
      "256.0.0.0.",
      "192.168. 224.0.",
      "4294967296.",
      ".1.",
      "192.168.224.0 1.",
    };
    stl::uint8_t ip_octets[4]{};

    for (auto const& [_ip, expected_ip] : valid_ipv4s) {
        auto       context = this->template get_context<TypeParam>(_ip);
        bool const should_continue =
          uri::details::parse_host_ipv4(_ip.begin(), _ip.end(), ip_octets, context);

        EXPECT_TRUE(should_continue)
          << "Original IP String: " << _ip << "\nParsed IP: " << static_cast<int>(ip_octets[0]) << "."
          << static_cast<int>(ip_octets[1]) << "." << static_cast<int>(ip_octets[2]) << "."
          << static_cast<int>(ip_octets[3]);
        EXPECT_EQ(expected_ip, ip_octets)
          << "Original IP String: " << _ip << "\n"
          << "Expected IP: " << expected_ip.string() << "\n"
          << "Parsed IP: " << static_cast<int>(ip_octets[0]) << "." << static_cast<int>(ip_octets[1]) << "."
          << static_cast<int>(ip_octets[2]) << "." << static_cast<int>(ip_octets[3]);
    }

    for (auto const& _ip : invalid_ipv4s) {
        auto       context = this->template get_context<TypeParam>(_ip);
        bool const should_continue =
          uri::details::parse_host_ipv4(_ip.begin(), _ip.end(), ip_octets, context);

        EXPECT_FALSE(should_continue)
          << "Original IP String: '" << _ip << "'\nParsed IP: " << static_cast<int>(ip_octets[0]) << "."
          << static_cast<int>(ip_octets[1]) << "." << static_cast<int>(ip_octets[2]) << "."
          << static_cast<int>(ip_octets[3]);
    }
    // NOLINTEND(*-avoid-c-arrays)
}

TYPED_TEST(URITests, EmptyIPv4) {
    // NOLINTBEGIN(*-avoid-c-arrays)
    stl::uint8_t               ip_octets[4]{};
    constexpr stl::string_view _ip = "192.168.1.";

    auto       context         = this->template get_context<TypeParam>(_ip);
    bool const should_continue = uri::details::parse_host_ipv4<uri::strict_uri_parsing_options>(
      _ip.begin(),
      _ip.end(),
      ip_octets,
      context);

    EXPECT_FALSE(should_continue) << "Original IP String: " << _ip
                                  << "\nParsed IP: " << static_cast<int>(ip_octets[0]) << "."
                                  << static_cast<int>(ip_octets[1]) << "." << static_cast<int>(ip_octets[2])
                                  << "." << static_cast<int>(ip_octets[3]);

    EXPECT_FALSE(uri::is_valid(context.status)) << to_string(uri::get_value(context.status));
    EXPECT_EQ(uri::uri_status::ip_too_little_octets, uri::get_value(context.status))
      << to_string(uri::get_value(context.status));

    // NOLINTEND(*-avoid-c-arrays)
}

TYPED_TEST(URITests, NewlinesInURI) {
    auto const ctx = this->template parse_from_string<TypeParam>("http://example\t.\norg");
    EXPECT_TRUE(uri::is_valid(ctx.status)) << to_string(uri::get_value(ctx.status));
    EXPECT_TRUE(uri::has_warning(ctx.status, uri::uri_status::invalid_character));
    EXPECT_EQ(ctx.out.get_scheme(), "http");
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(ctx.out.get_hostname(), "example.org");
        EXPECT_EQ(ctx.out.get_path(), "/");
    } else {
        EXPECT_EQ(ctx.out.get_hostname(), "example\t.\norg");
        EXPECT_EQ(ctx.out.get_path(), "");
    }
}

TYPED_TEST(URITests, NewlinesInLocalhost) {
    auto const ctx = this->template parse_from_string<TypeParam>("file:///loc\nal\th\rost/page");
    EXPECT_TRUE(uri::is_valid(ctx.status)) << to_string(uri::get_value(ctx.status));
    EXPECT_TRUE(uri::has_warning(ctx.status, uri::uri_status::invalid_character));
    EXPECT_EQ(ctx.out.get_scheme(), "file");
    EXPECT_EQ(ctx.out.get_hostname(), "");
    EXPECT_EQ(ctx.out.get_path(), "/page");
}

TYPED_TEST(URITests, NewlinesInScheme) {
    auto const ctx = this->template parse_from_string<TypeParam>("\n\n\rhtt\rps\n:\r/\r/\rexample.org");
    EXPECT_TRUE(uri::is_valid(ctx.status)) << to_string(uri::get_value(ctx.status));
    EXPECT_TRUE(uri::has_warning(ctx.status, uri::uri_status::invalid_character));
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(ctx.out.get_scheme(), "https");
    } else {
        EXPECT_EQ(ctx.out.get_scheme(), "\n\n\rhtt\rps\n");
    }
    EXPECT_EQ(ctx.out.get_hostname(), "example.org");
}

TYPED_TEST(URITests, BlowUp) {
    // somehow this caused a crash, so I'm writing a test for it
    auto const ctx = this->template parse_from_string<TypeParam>("test-a-colon.html");
    EXPECT_FALSE(uri::is_valid(ctx.status)) << to_string(uri::get_value(ctx.status));
}

TYPED_TEST(URITests, SpacesInURIs) {
    auto const ctx = this->template parse_from_string<TypeParam>("http://example. org");
    EXPECT_FALSE(uri::is_valid(ctx.status)) << to_string(uri::get_value(ctx.status));
    EXPECT_TRUE(uri::has_error(ctx.status, uri::uri_status::invalid_domain_code_point))
      << to_string(uri::get_value(ctx.status));
}

TYPED_TEST(URITests, SpecialDots) {
    auto const ctx = this->template parse_from_string<TypeParam>("http://example｡org");
    EXPECT_FALSE(uri::is_valid(ctx.status)) << to_string(uri::get_value(ctx.status));
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(ctx.out.get_hostname(), "example.org");
    } else {
        EXPECT_EQ(ctx.out.get_hostname(), "example｡org");
    }
}

TYPED_TEST(URITests, EmptyHostNotAllowed) {
    auto const ctx = this->template parse_from_string<TypeParam>("http://username:password@:/");
    EXPECT_FALSE(uri::is_valid(ctx.status)) << to_string(uri::get_value(ctx.status));
    EXPECT_TRUE(uri::has_error(ctx.status, uri::uri_status::host_missing))
      << to_string(uri::get_value(ctx.status));


    auto const ctx2 = this->template parse_from_string<TypeParam>("http://username@/");
    EXPECT_FALSE(uri::is_valid(ctx2.status)) << to_string(uri::get_value(ctx2.status));
    EXPECT_TRUE(uri::has_error(ctx2.status, uri::uri_status::host_missing))
      << to_string(uri::get_value(ctx2.status));
}

TYPED_TEST(URITests, StupidSchemes) {
    // http
    auto const ctx = this->template parse_from_string<TypeParam>("http:");
    EXPECT_FALSE(uri::is_valid(ctx.status)) << to_string(uri::get_value(ctx.status));

    // random
    auto const ctx2 = this->template parse_from_string<TypeParam>("any-random-scheme:");
    EXPECT_TRUE(uri::is_valid(ctx2.status)) << to_string(uri::get_value(ctx2.status));

    // file
    auto const ctx3 = this->template parse_from_string<TypeParam>("file:");
    EXPECT_TRUE(uri::is_valid(ctx3.status)) << to_string(uri::get_value(ctx3.status));

    // ws
    auto const ctx4 = this->template parse_from_string<TypeParam>("ws:");
    EXPECT_FALSE(uri::is_valid(ctx4.status)) << to_string(uri::get_value(ctx4.status));
}
