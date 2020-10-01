// Created by moisrex on 9/24/20.
#include <gtest/gtest.h>
#include "../core/include/webpp/http/syntax/request_parser.hpp"
#include "../core/include/webpp/http/syntax/http_lexer.hpp"

#include <vector>
#include <string_view>
#include <array>
#include <tuple>
#include <algorithm>

using namespace webpp;

using req_parser = http_request_parser<std_traits>;

TEST(HTTPRequestParser, RequestLine) {
    std::vector<std::string_view> accepted_req_lines {
        "GET / HTTP/1.1",
        "POST / HTTP/1.1",
        "HEAD / HTTP/1.1",
        "SOMETHING / HTTP/1.1",
        "SOMETHING /some/path HTTP/1.1",
        "SOMETHING /some/path.html HTTP/1.1",
        "SOMETHING /some/path.html HTTP/1.0",
        "GET some/path.html HTTP/1.0"
    };

    for (std::string_view line : accepted_req_lines) {
        req_parser parser;
        auto original_line = line;
        EXPECT_EQ(200, parser.parse_request_line(line)) << "Request Line: " << original_line;
    }

    std::vector<std::string_view> not_accepted_req_lines {
      "SOME/THING / HTTP/1.1",
      // "SOMETHING # HTTP/1.1", // we don't want this method to check if the uri is right or wrong
      "TEST test",
      "TEST",
      "------",
      "123",
      // "GET1 / HTTP/1.1",
    };

    for (std::string_view line : not_accepted_req_lines) {
        req_parser parser;
        auto original_line = line;
        EXPECT_NE(200, parser.parse_request_line(line)) << "Request Line: " << original_line;
    }


    req_parser parser1;
    EXPECT_EQ(200, parser1.parse_request_line("GET /home HTTP/1.1"));
    EXPECT_EQ(parser1.http_version_view, "1.1");
    EXPECT_EQ(parser1.request_target_view, "/home");
    EXPECT_EQ(parser1.method_view, "GET");
    http_version ver = parser1.get_http_version();
    EXPECT_EQ(ver.major_value(), 1);
    EXPECT_EQ(ver.minor_value(), 1);
}


TEST(HTTPRequestParser, HeaderLexer) {
    using str = std::string_view;
    using arr = std::array<str, 2>;
    using vec = std::vector<std::tuple<str, arr>>;
    vec headers({
      {"one: string\r\n", arr{"one", "string"}},
      {"Second-One:    String   \r\n", arr{"Second-One", "String"}}
    });

    str sample_request = "one: 1\r\n"
    "two: 2\r\n"
    "The-One:Yes,NoSpaceIsNeeded\r\n";
    http_lexer<webpp::std_traits> lexer {
      .raw_view = sample_request
    };

    lexer.consume_all();
    EXPECT_EQ(std::count_if(std::begin(sample_request), std::end(sample_request), [] (auto c) {
                  return c == '\r' || c == '\n';
              }) / 2,lexer.header_views.size()) << "The lexer size is not a match";

    EXPECT_EQ(lexer.header_views.at(0).at(0), "one");
    EXPECT_EQ(lexer.header_views.at(0).at(1), "1");
}