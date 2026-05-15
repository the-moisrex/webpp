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
    void parse(stl::string_view input, char const*& begin) {
        begin           = input.data();
        char const* end = input.data() + input.size();
        parse_request_line(begin, end, req);
    }
};

class HttpParserHeaderTest : public ::testing::Test {
  protected:
    static http_parsing_state
    RunParseHeader(stl::string const& input, stl::size_t& advanced, stl::string_view& name, stl::string_view& value) {
        char const* buf     = input.data();
        char const* buf_end = buf + input.size();

        auto const state = http::parse_header(buf, buf_end, name, value);
        advanced         = static_cast<stl::size_t>(buf - input.data());
        return state;
    }

    template <typename Callback>
    static http_parsing_state RunParseHeaders(stl::string const& input, stl::size_t& advanced, Callback&& cb) {
        char const* buf     = input.data();
        char const* buf_end = buf + input.size();

        auto const state = http::parse_headers(buf, buf_end, stl::forward<Callback>(cb));
        advanced         = static_cast<stl::size_t>(buf - input.data());
        return state;
    }
};

TEST_F(HttpParserTest, ParsesValidGetRequest) {
    stl::string_view const input = "GET /index.html HTTP/1.1\r\nHost: localhost";
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::ok_request_line);
    EXPECT_EQ(req.method, "GET");
    EXPECT_EQ(req.target, "/index.html");
    EXPECT_EQ(req.version, "HTTP/1.1");

    // Pointer should be advanced exactly past the \r\n
    stl::ptrdiff_t const parsed_bytes = begin - input.data();
    EXPECT_EQ(parsed_bytes, 26);
    EXPECT_EQ(stl::string_view(begin), "Host: localhost"); // Remaining unparsed data
}

TEST_F(HttpParserTest, NeedMoreDataDuringMethod) {
    stl::string_view const input = "GE"; // Cut off before space
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::need_more_data);
    EXPECT_EQ(begin, input.data()); // Pointer should not advance on failure
}

TEST_F(HttpParserTest, NeedMoreDataDuringCRLF) {
    stl::string_view const input = "GET / HTTP/1.1\r"; // Missing \n
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::need_more_data);
    EXPECT_EQ(begin, input.data());
}

TEST_F(HttpParserTest, FailsOnInvalidMethodToken) {
    stl::string_view const input = "G@T / HTTP/1.1\r\n";
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_method);
}

TEST_F(HttpParserTest, FailsOnMissingMethod) {
    stl::string_view const input = " / HTTP/1.1\r\n"; // Space right away
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_method);
}

TEST_F(HttpParserTest, FailsOnInvalidTargetChar) {
    // Control character (0x01) in target is invalid
    stl::string const input = "GET /index\x01.html HTTP/1.1\r\n";
    char const*       begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_target);
}

TEST_F(HttpParserTest, FailsOnMissingTarget) {
    stl::string_view const input = "GET  HTTP/1.1\r\n"; // Double space implies empty target
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_target);
}

TEST_F(HttpParserTest, FailsOnInvalidVersionPrefix) {
    stl::string_view const input = "GET / HTP/1.1\r\n";
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_version);
}

TEST_F(HttpParserTest, FailsOnInvalidCRLF) {
    stl::string_view const input = "GET / HTTP/1.1\r\r"; // Carriage return twice instead of \r\n
    char const*            begin = nullptr;

    parse(input, begin);

    EXPECT_EQ(req.state, http_parsing_state::invalid_crlf);
}

TEST_F(HttpParserHeaderTest, EndOfHeadersCRLFReturnsOkHeadersEndAndAdvances) {
    stl::string const       input = "\r\n";
    stl::size_t             advanced{};
    webpp::stl::string_view name;
    webpp::stl::string_view value;

    auto const state = RunParseHeader(input, advanced, name, value);

    EXPECT_EQ(state, http_parsing_state::ok_headers_end);
    EXPECT_EQ(advanced, 2U);
}

TEST_F(HttpParserHeaderTest, EndOfHeadersLFReturnsOkHeadersEndAndAdvances) {
    stl::string const       input = "\n";
    stl::size_t             advanced{};
    webpp::stl::string_view name;
    webpp::stl::string_view value;

    auto const state = RunParseHeader(input, advanced, name, value);

    EXPECT_EQ(state, http_parsing_state::ok_headers_end);
    EXPECT_EQ(advanced, 1U);
}

TEST_F(HttpParserHeaderTest, SingleCRReturnsNeedMoreDataAndDoesNotAdvance) {
    stl::string const       input = "\r";
    stl::size_t             advanced{};
    webpp::stl::string_view name;
    webpp::stl::string_view value;

    auto const state = RunParseHeader(input, advanced, name, value);

    EXPECT_EQ(state, http_parsing_state::need_more_data);
    EXPECT_EQ(advanced, 0U);
}

TEST_F(HttpParserHeaderTest, CRNotFollowedByLFReturnsInvalidCRLFAndConsumesInput) {
    stl::string const       input = "\rX";
    stl::size_t             advanced{};
    webpp::stl::string_view name;
    webpp::stl::string_view value;

    auto const state = RunParseHeader(input, advanced, name, value);

    EXPECT_EQ(state, http_parsing_state::invalid_crlf);
    EXPECT_EQ(advanced, 2U); // current behavior: buffer advanced past 'X'
}

TEST_F(HttpParserHeaderTest, IsFlaggedAsPossibleLineFoldingCurrentBehavior) {
    stl::string const       input = "Host: example.com\r\n";
    stl::size_t             advanced{};
    webpp::stl::string_view name;
    webpp::stl::string_view value;

    auto const state = RunParseHeader(input, advanced, name, value);

    // Current implementation returns possible_line_folding for non-space/tab start.
    EXPECT_EQ(state, http_parsing_state::possible_line_folding);
    EXPECT_EQ(advanced, 0U);
    EXPECT_TRUE(name.empty());
}

TEST_F(HttpParserHeaderTest, LeadingWhitespaceLineReturnsInvalidChar) {
    stl::string const       input = "  Folded: yes\r\n";
    stl::size_t             advanced{};
    webpp::stl::string_view name;
    webpp::stl::string_view value;

    auto const state = RunParseHeader(input, advanced, name, value);

    EXPECT_EQ(state, http_parsing_state::invalid_char);
    EXPECT_EQ(advanced, 0U);
}

TEST_F(HttpParserHeaderTest, ParseHeadersEmptyBufferReturnsNeedMoreData) {
    stl::string const input = "";
    stl::size_t       advanced{};
    stl::size_t       callback_count = 0;

    auto const state = RunParseHeaders(input, advanced, [&](stl::string_view, stl::string_view) {
        ++callback_count;
    });

    EXPECT_EQ(state, http_parsing_state::need_more_data);
    EXPECT_EQ(advanced, 0U);
    EXPECT_EQ(callback_count, 0U);
}

TEST_F(HttpParserHeaderTest, OnlyCRLFReturnsOkHeadersEndAndNoCallback) {
    stl::string const input = "\r\n";
    stl::size_t       advanced{};
    stl::size_t       callback_count = 0;

    auto const state = RunParseHeaders(input, advanced, [&](stl::string_view, stl::string_view) {
        ++callback_count;
    });

    EXPECT_EQ(state, http_parsing_state::ok_headers_end);
    EXPECT_EQ(advanced, 2U);
    EXPECT_EQ(callback_count, 0U);
}

TEST_F(HttpParserHeaderTest, OnlyLFReturnsOkHeadersEndAndNoCallback) {
    stl::string const input = "\n";
    stl::size_t       advanced{};
    stl::size_t       callback_count = 0;

    auto const state = RunParseHeaders(input, advanced, [&](stl::string_view, stl::string_view) {
        ++callback_count;
    });

    EXPECT_EQ(state, http_parsing_state::ok_headers_end);
    EXPECT_EQ(advanced, 1U);
    EXPECT_EQ(callback_count, 0U);
}

TEST_F(HttpParserHeaderTest, NormalHeaderThenEndReturnsPossibleLineFoldingAndNoCallback) {
    stl::string const input = "Host: a\r\n\r\n";
    stl::size_t       advanced{};
    stl::size_t       callback_count = 0;

    auto const state = RunParseHeaders(input, advanced, [&](stl::string_view, stl::string_view) {
        ++callback_count;
    });

    // Current implementation returns possible_line_folding on the first header line.
    EXPECT_EQ(state, http_parsing_state::possible_line_folding);
    EXPECT_EQ(advanced, 0U);
    EXPECT_EQ(callback_count, 0U);
}

TEST_F(HttpParserHeaderTest, InvalidCRLFPropagatesError) {
    stl::string const input = "\rX";
    stl::size_t       advanced{};
    stl::size_t       callback_count = 0;

    auto const state = RunParseHeaders(input, advanced, [&](stl::string_view, stl::string_view) {
        ++callback_count;
    });

    EXPECT_EQ(state, http_parsing_state::invalid_crlf);
    EXPECT_EQ(advanced, 2U);
    EXPECT_EQ(callback_count, 0U);
}

TEST_F(HttpParserHeaderTest, SingleCRReturnsNeedMoreDataAndDoesNotAdvance2) {
    stl::string const input = "\r";
    stl::size_t       advanced{};
    stl::size_t       callback_count = 0;

    auto const state = RunParseHeaders(input, advanced, [&](stl::string_view, stl::string_view) {
        ++callback_count;
    });

    EXPECT_EQ(state, http_parsing_state::need_more_data);
    EXPECT_EQ(advanced, 0U);
    EXPECT_EQ(callback_count, 0U);
}
