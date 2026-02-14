// Created by moisrex on January 1th 2024.

#include "../webpp/uri/domain.hpp"
#include "../webpp/uri/uri.hpp"
#include "common/test.hpp"

using namespace webpp;

TEST(StructuredURITests, StructuredDomain) {
    constexpr uri::basic_domain<char> domain = "domain.tld";
    EXPECT_EQ(domain.tld(), "tld");
}

TEST(StructuredURITests, StructuredURI) {
    using stl::literals::operator""sv;
    uri::uri const url{"HtTPS://"sv};
    EXPECT_TRUE(url.has_scheme());
    EXPECT_EQ(url.scheme(), "https:");
}

TEST(StructuredURITests, StructuredURIEquality) {
    uri::uri const url = "HtTPS://example.org/page/one?option=value&opt=val#fragment";
    EXPECT_TRUE(url.has_value());
    EXPECT_TRUE(url.has_scheme());
    EXPECT_EQ(url.scheme(), "https");
    EXPECT_EQ(url.hostname(), "example.org");
    EXPECT_EQ(url.path(), "/page/one");
    EXPECT_EQ(url.fragment(), "fragment");
    EXPECT_EQ(url.queries(), "option=value&opt=val");
}

TEST(StructuredURITests, AddDotToPath) {
    uri::uri url{"non-spec:/"};
    // EXPECT_TRUE(url);
    url.path("//p");
    EXPECT_EQ(url.path(), "//p");
    EXPECT_EQ(url.as_string(), "non-spec:/.//p");
}

TEST(StructuredURITests, UpdatePassword) {
    uri::uri url{"https://username:password@host:8000/path?query#fragment"};
    // EXPECT_TRUE(url);
    url.password("test");
    EXPECT_EQ(url.password(), "test");
    EXPECT_EQ(url.href(), "https://username:test@host:8000/path?query#fragment");
}

// https://github.com/nodejs/node/issues/46755
TEST(StructuredURITests, SchemeChangeParsing) {
    uri::uri url{"file:///var/log/system.log"};
    url.href("http://0300.168.0xF0");
    EXPECT_EQ(url.scheme(), "http");
    EXPECT_EQ(url.href(), "http://192.168.0.240/");
    // EXPECT_EQ(url.as_string(), "http://0300.168.0xF0/");
}

TEST(StructuredURITests, ClearOnSet) {
    using stl::literals::operator""sv;
    uri::uri url{"https://example.org/about"};
    url = "http://0300.168.0xF0"sv;
    EXPECT_EQ(url.scheme(), "http");
    EXPECT_FALSE(url.has_path());
    EXPECT_EQ(url.href(), "http://192.168.0.240");
}

TEST(StructuredURITests, StructuredPortGetsSerialized) {
    // https://url.spec.whatwg.org/#serialize-an-integer
    uri::uri const url{"https://example.org:080/about"};
    EXPECT_EQ(url.href(), "https://example.org:80/about");
    EXPECT_EQ(url.port(), 80);
    EXPECT_EQ(url.port(), "80");
}

TEST(StructuredURITests, StructuredPortGetsSerialized2) {
    uri::uri const url{"https://example.org:00000000000/about"};
    EXPECT_EQ(url.href(), "https://example.org:0/about");
    EXPECT_EQ(url.port(), 0);
    EXPECT_EQ(url.port(), "0");
}

TEST(StructuredURITests, OpaquePath) {
    uri::uri const url{"opaque:this:is/the:path"};
    EXPECT_FALSE(url.has_authority());
    EXPECT_EQ(url.href(), "opaque:this:is/the:path");
    EXPECT_EQ(url.path(), "this:is/the:path");
}

TEST(StructuredURITests, SpacesTest) {
    uri::uri url{"http://www.example.com/%37/ /"};
    EXPECT_TRUE(url.valid());
    EXPECT_EQ(url.href(), "http://www.example.com/%37/%20/");
    url.href("http://www.example.com/  /  /+/");
    EXPECT_TRUE(url.valid());
    EXPECT_EQ(url.href(), "http://www.example.com/%20%20/%20%20/+/");
    url = uri::uri("http://www.example.com/");
    EXPECT_FALSE(url.valid());
}
