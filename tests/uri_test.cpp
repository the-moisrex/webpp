// Created by moisrex on 11/9/20.

#include "../webpp/uri/uri.hpp"

#include "common/tests_common_pch.hpp"

// also checkout:
//  - domain-test

using namespace webpp;

TEST(URITests, Generation) {
    uri::uri u;
    EXPECT_EQ(u.scheme.size(), 0);

    auto alloc = u.get_allocator<std::allocator<char>>();

    std::string const str{alloc};
    EXPECT_EQ(str.size(), 0);

    u.scheme = "https";
    EXPECT_EQ(u.to_string(), "https://");
    u.host = "webpp.dev";
    EXPECT_EQ(u.to_string(), "https://webpp.dev");
}

TEST(URITests, PathFromString) {
    uri::basic_path path = "/a/b/c/../d";
    EXPECT_EQ(path.size(), 6);
    EXPECT_EQ(path[0], "");
    EXPECT_EQ(path[1], "a");
    path /= "nice";
    EXPECT_TRUE(stl::is_eq(path <=> stl::string{"/a/b/c/../d/nice"}));
    // EXPECT_EQ(path, "/a/b/c/../d/nice");
}

TEST(URITests, QueryParamGeneration) {
    uri::uri url;
    url                   = "https://localhost/api/v2/content";
    url.queries["model"]  = "Encode this";
    url.queries["locale"] = "English is a locale";
    url.queries["text"]   = "This text has a \n newline in it.";
    url.queries["token"]  = "f95d9af1-18da-439c-8326-55f9ff7d6f8c";
    EXPECT_EQ(
      url.to_string(),
      "https://localhost/api/v2/content?locale=English%20is%20a%20locale&model=Encode%20this&text=This%20text%20has%20a%20%0A%20newline%20in%20it.&token=f95d9af1-18da-439c-8326-55f9ff7d6f8c");
}


TEST(URITests, IntegralSchemeParsing) {
    stl::string_view                     str = "http://";
    uri::parsing_uri_context<char const> context{.beg = str.data(),
                                                 .pos = str.data(),
                                                 .end = str.data() + str.size()};
    uri::parse_scheme(context);
    auto const res = static_cast<uri::uri_status>(context.status);
    EXPECT_EQ(res, uri::uri_status::valid_authority) << to_string(res);
    EXPECT_EQ(context.out.scheme_end, 4);
    EXPECT_EQ(context.pos - str.data(), 7);
}


TEST(URITests, StringSchemeParsing) {
    stl::string_view str = "urn:testing";

    uri::parsing_uri_context<const char, stl::string_view> context{.beg = str.data(),
                                                                   .pos = str.data(),
                                                                   .end = str.data() + str.size()};

    uri::parse_scheme(context);
    auto const res = static_cast<uri::uri_status>(context.status);
    EXPECT_EQ(res, uri::uri_status::valid_opaque_path) << to_string(res);
    EXPECT_EQ(context.out.scheme, "urn");
    EXPECT_EQ(context.pos - str.data(), 4);
}


TEST(URITests, URIParsingWithWarnings) {
    uri::uri url;
    url = "https:this-is-stupid";
    EXPECT_EQ(url.to_string(), "https://this-is-stupid/");
}
