// Created by moisrex on 9/24/20.
#include <gtest/gtest.h>
#include "../core/include/webpp/http/syntax/request_parser.hpp"
#include "../core/include/webpp/traits/std_traits.hpp"

#include <string>
#include <vector>
#include <string_view>

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
}
