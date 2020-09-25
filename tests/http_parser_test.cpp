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
        EXPECT_EQ(200, parser.parse_request_line(line)) << "Request Line: " << line;
    }
}
