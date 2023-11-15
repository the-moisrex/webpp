// Created by moisrex on 11/9/20.

#include "../webpp/uri/uri.hpp"

#include "../webpp/uri/path_traverser.hpp"
#include "common/tests_common_pch.hpp"

// also checkout:
//  - domain-test

using namespace webpp;

using Types = testing::Types<uri::parsing_uri_context_string<stl::string>,
                             uri::parsing_uri_context_string<stl::string_view>,
                             // uri::parsing_uri_context_string<stl::basic_string_view<char8_t>>,
                             uri::parsing_uri_context_u32,
                             uri::parsing_uri_context_segregated<>,
                             uri::parsing_uri_context_segregated_view<>,
                             uri::parsing_uri_context<stl::string_view, const char*>>;
template <class T>
struct URITests : testing::Test {

  private:
    stl::string url_text;

  public:
    template <typename SpecifiedTypeParam>
    [[nodiscard]] constexpr SpecifiedTypeParam get_context(auto str) {
        using ctx_type    = SpecifiedTypeParam;
        using string_type = stl::remove_cvref_t<decltype(str)>;
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
};
TYPED_TEST_SUITE(URITests, Types);

TYPED_TEST(URITests, Generation) {
    uri::uri url;
    EXPECT_EQ(url.scheme.size(), 0);

    auto alloc = url.get_allocator<std::allocator<char>>();

    std::string const str{alloc};
    EXPECT_EQ(str.size(), 0);

    url.scheme = "https";
    EXPECT_EQ(url.to_string(), "https://");
    url.host = "webpp.dev";
    EXPECT_EQ(url.to_string(), "https://webpp.dev");
}

TYPED_TEST(URITests, PathFromString) {
    uri::basic_path path{"/a/b/c/../d"};
    EXPECT_EQ(path.size(), 6);
    EXPECT_EQ(path[0], "");
    EXPECT_EQ(path[1], "a");
    path /= "nice";
    EXPECT_TRUE(stl::is_eq(path <=> stl::string{"/a/b/c/../d/nice"}));
    // EXPECT_EQ(path, "/a/b/c/../d/nice");
}

TYPED_TEST(URITests, QueryParamGeneration) {
    uri::uri url          = "https://localhost/api/v2/content";
    url.queries["model"]  = "Encode this";
    url.queries["locale"] = "English is a locale";
    url.queries["text"]   = "This text has a \n newline in it.";
    url.queries["token"]  = "f95d9af1-18da-439c-8326-55f9ff7d6f8c";
    EXPECT_EQ(
      url.to_string(),
      "https://localhost/api/v2/content?locale=English%20is%20a%20locale&model=Encode%20this&text=This%20text%20has%20a%20%0A%20newline%20in%20it.&token=f95d9af1-18da-439c-8326-55f9ff7d6f8c");
}


TYPED_TEST(URITests, IntegralSchemeParsing) {
    stl::string_view const          str = "http://";
    uri::parsing_uri_context_view<> context{.beg = str.begin(), .pos = str.begin(), .end = str.end()};
    uri::parse_scheme(context);
    auto const res = static_cast<uri::uri_status>(context.status);
    EXPECT_EQ(res, uri::uri_status::valid_authority) << to_string(res);
    EXPECT_EQ(context.out.get_scheme(), "http");
    EXPECT_EQ(context.pos - str.begin(), 7);
}


TYPED_TEST(URITests, StringSchemeParsing) {
    stl::string_view const str = "urn:testing";

    uri::parsing_uri_context<stl::string_view, const char*> context{.beg = str.data(),
                                                                    .pos = str.data(),
                                                                    .end = str.data() + str.size()};

    uri::parse_scheme(context);
    auto const res = static_cast<uri::uri_status>(context.status);
    EXPECT_EQ(res, uri::uri_status::valid_opaque_path) << to_string(res);
    EXPECT_EQ(context.out.get_scheme(), "urn");
    EXPECT_EQ(context.pos - str.data(), 4);
}

TYPED_TEST(URITests, ParseURI) {
    stl::string_view const str = "urn:testing";

    auto       context = uri::parse_uri(str);
    auto const res     = static_cast<uri::uri_status>(context.status);
    EXPECT_EQ(res, uri::uri_status::valid_opaque_path) << to_string(res);
    EXPECT_EQ(context.out.get_scheme(), "urn");
    EXPECT_EQ(context.out.get_path(), "testing");
}


TYPED_TEST(URITests, URIParsingWithWarnings) {
    uri::uri url = "https:this-is-stupid";
    EXPECT_EQ(url.to_string(), "https://this-is-stupid/");
}

TYPED_TEST(URITests, URIStatusTest) {
    uri::uri_status_type status = 0;
    status |= stl::to_underlying(uri::uri_status::missing_following_solidus);
    status |= stl::to_underlying(uri::uri_status::invalid_character);

    EXPECT_TRUE(uri::has_warnings(status));
    EXPECT_TRUE(uri::has_warnings(static_cast<uri::uri_status>(status)));
    EXPECT_FALSE(uri::has_error(static_cast<uri::uri_status>(status)));
    EXPECT_FALSE(uri::has_error(status));
    EXPECT_TRUE(uri::is_valid(status));
    EXPECT_TRUE(uri::is_valid(static_cast<uri::uri_status>(status)));
}

TYPED_TEST(URITests, URIStatusIterator) {
    uri::uri_status_type status = 0;
    status |= stl::to_underlying(uri::uri_status::missing_following_solidus);
    status |= stl::to_underlying(uri::uri_status::invalid_character);

    uri::uri_status_type const original = status;

    int index = 0;
    for (auto item : uri::uri_status_iterator{status}) {
        EXPECT_TRUE(item == uri::uri_status::missing_following_solidus ||
                    item == uri::uri_status::invalid_character)
          << "Index: " << index << "\n"
          << "Value: " << stl::to_underlying(item) << "\n"
          << "Original: " << original << "\n"
          << to_string(item);
        ++index;
    }
    EXPECT_EQ(index, 2);
}


TYPED_TEST(URITests, URIStatusIteratorWithValue) {
    uri::uri_status_type status = 0;
    status |= stl::to_underlying(uri::uri_status::missing_following_solidus);
    status |= stl::to_underlying(uri::uri_status::invalid_character);
    status |= stl::to_underlying(uri::uri_status::valid_queries);

    uri::uri_status_type const original = status;

    int index = 0;
    for (auto item : uri::uri_status_iterator{status}) {
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
    stl::string            out;
    stl::string_view const inp     = "%D8%B3%D9%84%D8%A7%D9%85";
    stl::string_view const decoded = "سلام";
    EXPECT_TRUE(decode_uri_component(inp, out, ALPHA_DIGIT<char>));
    EXPECT_EQ(out, decoded);

    out      = inp;
    auto ptr = out.begin();
    EXPECT_TRUE(decode_uri_component_inplace(ptr, out.end(), ALPHA_DIGIT<char>));
    out.resize(static_cast<stl::size_t>(ptr - out.begin()));
    EXPECT_EQ(out, decoded);

    std::string output2;
    encode_uri_component(out, output2, ALPHA_DIGIT<char>);
    EXPECT_EQ(output2, inp) << out;

    std::string output3;
    EXPECT_TRUE(decode_uri_component(output2, output3, ALPHA_DIGIT<char>));
    EXPECT_EQ(output3, decoded) << out;
}

TYPED_TEST(URITests, PercentEncodeDecodePointer) {
    stl::string            out;
    stl::string_view const inp     = "%D8%B3%D9%84%D8%A7%D9%85";
    stl::string_view const decoded = "سلام";
    EXPECT_TRUE(decode_uri_component(inp, out, ALPHA_DIGIT<char>));
    EXPECT_EQ(out, decoded);

    out       = inp;
    auto* ptr = out.data();
    EXPECT_TRUE(decode_uri_component_inplace(ptr,
                                             ptr + out.size(), // NOLINT(*-pro-bounds-pointer-arithmetic)
                                             ALPHA_DIGIT<char>));
    out.resize(static_cast<stl::size_t>(ptr - out.data()));
    EXPECT_EQ(out, decoded);

    std::string output2;
    encode_uri_component(out, output2, ALPHA_DIGIT<char>);
    EXPECT_EQ(output2, inp) << out;

    std::string output3;
    EXPECT_TRUE(decode_uri_component(output2, output3, ALPHA_DIGIT<char>));
    EXPECT_EQ(output3, decoded) << out;
}


TYPED_TEST(URITests, BasicURIParsing) {
    stl::string_view const str =
      "https://username:password@example.com:1010/this/is/the/path?query1=one#hash";

    auto context = uri::parse_uri(str);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_TRUE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::uri_status::has_credentials, uri::get_warning(context.status))
      << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "https");
    EXPECT_EQ(context.out.get_host(), "example.com");
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
    EXPECT_TRUE(context.out.has_host());
    EXPECT_TRUE(context.out.has_path());
    EXPECT_TRUE(context.out.has_queries());
    EXPECT_TRUE(context.out.has_fragment());
}


TYPED_TEST(URITests, PathIteratorTest) {
    uri::path_iterator<> iter{"/page/one"};
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
    stl::string_view const str = "urn://this/is/a/path";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "urn");
    EXPECT_EQ(context.out.get_host(), "this");
    EXPECT_EQ(context.out.get_path(), "/is/a/path");
}

TYPED_TEST(URITests, OpaqueHostParserWarning) {
    stl::string_view const str = "urn://th%is/is/a/path";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    EXPECT_TRUE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::uri_status::invalid_character, uri::get_warning(context.status))
      << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "urn");
    EXPECT_EQ(context.out.get_host(), "th%is");
    EXPECT_EQ(context.out.get_path(), "/is/a/path");
}


TYPED_TEST(URITests, OpaqueHostWithIPv6) {
    stl::string_view const str = "ldap://[2001:db8::7]/c=GB?objectClass?one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "ldap");
    EXPECT_EQ(context.out.get_host(), "[2001:db8::7]");
    EXPECT_EQ(context.out.get_path(), "/c=GB");
    EXPECT_EQ(context.out.get_queries(), "objectClass?one");
}

TYPED_TEST(URITests, FragmentOnNonSpecialSchemeAsFirstChar) {
    stl::string_view const str = "ldap://#one";

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
    stl::string_view const str = "http://127.0.0.1/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_scheme(), "http");
    EXPECT_EQ(context.out.get_host(), "127.0.0.1");
    EXPECT_EQ(context.out.get_path(), "/page/one");
}

TYPED_TEST(URITests, InvalidIPv4AsHost) {
    stl::string_view const str = "http://12l.0.0.1/page/one";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    EXPECT_FALSE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::ip_invalid_character)
      << to_string(uri::get_value(context.status));
}

TYPED_TEST(URITests, PathDot) {
    stl::string_view const str = "http://127.0.0.1/./one";

    auto context = uri::parse_uri(str);
    EXPECT_TRUE(uri::is_valid(context.status));
    ASSERT_FALSE(uri::has_warnings(context.status)) << to_string(uri::get_warning(context.status));
    EXPECT_EQ(uri::get_value(context.status), uri::uri_status::valid)
      << to_string(uri::get_value(context.status));
    EXPECT_EQ(context.out.get_path(), "/one");
}

TYPED_TEST(URITests, PathDotNormalized) {
    stl::string const str = "http://127.0.0.1/./one";

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

TYPED_TEST(URITests, PathDotNormalizedABunch) {
    stl::string const str =
      "http://127.0.0.1/..//./one/%2E./%2e/two/././././%2e/%2e/.././three/four/%2e%2e/five/.%2E/%2e";

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
    EXPECT_EQ(context.out.get_path(), "//three/");
}


TYPED_TEST(URITests, DoubleAtSign) {
    stl::string_view const str = "http://username@username@127.0.0.1/?one==a#hash";

    auto context = this->template get_context<TypeParam>(str);
    uri::parse_uri(context);
    if constexpr (TypeParam::is_modifiable) {
        EXPECT_EQ(context.out.get_username(), "username%40username");
    } else {
        EXPECT_EQ(context.out.get_username(), "username@username");
    }
    EXPECT_EQ(context.out.get_path(), "/");
    EXPECT_EQ(context.out.get_host(), "127.0.0.1");
    EXPECT_EQ(context.out.get_queries(), "one==a");
    EXPECT_EQ(context.out.get_fragment(), "hash");
}
