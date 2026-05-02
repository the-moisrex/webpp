// Created by moisrex on 9/24/20.
#include "../webpp/http/codec/http_parser.hpp"

#include "common/test.hpp"


using namespace webpp;
using namespace webpp::http;

class HttpParserTest : public ::testing::Test {
  protected:
    parsed_request_line req;

    void SetUp() override {
        req = parsed_request_line{}; // Reset state
    }

    // Helper to invoke parser cleanly
    void parse(std::string_view input, char const*& begin) {
        begin           = input.data();
        char const* end = input.data() + input.size();
        parse_request_line(begin, end, req);
    }
};

TEST_F(HttpParserTest, ParsesValidGetRequest) {
    std::string_view const input = "GET /index.html HTTP/1.1\r\nHost: localhost";
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::ok_request_line);
    EXPECT_EQ(req.method, "GET");
    EXPECT_EQ(req.target, "/index.html");
    EXPECT_EQ(req.version, "HTTP/1.1");

    // Pointer should be advanced exactly past the \r\n
    std::ptrdiff_t const parsed_bytes = begin - input.data();
    EXPECT_EQ(parsed_bytes, 26);
    EXPECT_EQ(std::string_view(begin), "Host: localhost"); // Remaining unparsed data
}

TEST_F(HttpParserTest, NeedMoreDataDuringMethod) {
    std::string_view const input = "GE"; // Cut off before space
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::need_more_data);
    EXPECT_EQ(begin, input.data()); // Pointer should not advance on failure
}

TEST_F(HttpParserTest, NeedMoreDataDuringCRLF) {
    std::string_view const input = "GET / HTTP/1.1\r"; // Missing \n
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::need_more_data);
    EXPECT_EQ(begin, input.data());
}

TEST_F(HttpParserTest, FailsOnInvalidMethodToken) {
    std::string_view const input = "G@T / HTTP/1.1\r\n";
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_method);
}

TEST_F(HttpParserTest, FailsOnMissingMethod) {
    std::string_view const input = " / HTTP/1.1\r\n"; // Space right away
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_method);
}

TEST_F(HttpParserTest, FailsOnInvalidTargetChar) {
    // Control character (0x01) in target is invalid
    std::string const input = "GET /index\x01.html HTTP/1.1\r\n";
    char const*       begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_target);
}

TEST_F(HttpParserTest, FailsOnMissingTarget) {
    std::string_view const input = "GET  HTTP/1.1\r\n"; // Double space implies empty target
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_target);
}

TEST_F(HttpParserTest, FailsOnInvalidVersionPrefix) {
    std::string_view const input = "GET / HTP/1.1\r\n";
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_version);
}

TEST_F(HttpParserTest, FailsOnInvalidCRLF) {
    std::string_view const input = "GET / HTTP/1.1\r\r"; // Carriage return twice instead of \r\n
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_crlf);
}

TEST_F(HttpParserTest, ToStringHelperCheck) {
    EXPECT_EQ(to_string(http_parsing_state::ok_request_line), "Request Line OK");
    EXPECT_EQ(to_string(http_parsing_state::invalid_method), "Invalid method");
}
