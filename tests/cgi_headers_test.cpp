#include "../webpp/cgi/cgi_headers.hpp"

#include "./common/test.hpp"

using namespace webpp::http;
using namespace webpp::http::details;

// -----------------------------------------------------------------------------
// details::cgi_header_id Tests
// -----------------------------------------------------------------------------

TEST(CGIHeadersTest, HeaderIdConversion) {
    // Standard HTTP header
    EXPECT_EQ(cgi_header_id("HOST"), header_id("host"));
    EXPECT_EQ(cgi_header_id("USER_AGENT"), header_id("user-agent"));
    EXPECT_EQ(cgi_header_id("ACCEPT_ENCODING"), header_id("accept-encoding"));

    // Mixed case (though usually CGI gives uppercase)
    EXPECT_EQ(cgi_header_id("User_Agent"), header_id("user-agent"));
    EXPECT_EQ(cgi_header_id("content_type"), header_id("content-type"));
}

TEST(CGIHeadersTest, HeaderIdLengthLimit) {
    // max_header_name_length is 50.
    // Creating a string of length 49 should pass, 50 or more should fail.
    std::string valid_long_name(49, 'A');
    std::string expected_valid(49, 'a');
    EXPECT_NE(cgi_header_id(valid_long_name), invalid_header_id);
    EXPECT_EQ(cgi_header_id(valid_long_name), header_id(expected_valid));

    std::string invalid_long_name(50, 'A');
    EXPECT_EQ(cgi_header_id(invalid_long_name), invalid_header_id);

    std::string very_long_name(100, 'A');
    EXPECT_EQ(cgi_header_id(very_long_name), invalid_header_id);
}

TEST(CGIHeadersTest, HeaderIdInvalidCharacters) {
    // Characters outside a-z, A-Z, numbers, and '_' should yield invalid_header_id
    EXPECT_EQ(cgi_header_id("INVALID-CHAR"), invalid_header_id);
    EXPECT_EQ(cgi_header_id("INVALID CHAR"), invalid_header_id);
    EXPECT_NE(cgi_header_id("NUMBERS123"), invalid_header_id);
}

// -----------------------------------------------------------------------------
// cgi_headers_iterator Tests
// -----------------------------------------------------------------------------

class CGIHeadersIteratorTest : public ::testing::Test {
  protected:
    // Helper to convert vector of strings to char** environment array
    std::vector<char const*> create_envp(std::vector<std::string> const& env_strings) {
        std::vector<char const*> envp;
        for (auto const& str : env_strings) {
            envp.push_back(str.c_str());
        }
        envp.push_back(nullptr); // Null-terminated as per CGI spec
        return envp;
    }
};

TEST_F(CGIHeadersIteratorTest, EmptyEnvironment) {
    std::vector<char const*> envp = {nullptr};
    cgi_headers_iterator     it(const_cast<char**>(envp.data()));
    cgi_headers_iterator     end; // Default constructor acts as end iterator

    EXPECT_EQ(it, end);
}

TEST_F(CGIHeadersIteratorTest, StandardHttpHeaders) {
    std::vector<std::string> env = {
      "PATH=/usr/bin",              // Should be ignored
      "HTTP_HOST=localhost",        // Valid
      "HTTP_USER_AGENT=TestClient", // Valid
      "GATEWAY_INTERFACE=CGI/1.1"   // Should be ignored
    };
    auto envp = create_envp(env);

    cgi_headers_iterator it(const_cast<char**>(envp.data()));
    cgi_headers_iterator end;

    ASSERT_NE(it, end);
    EXPECT_EQ(it->first, header_id("host"));
    EXPECT_EQ(it->second, "localhost");

    ++it;
    ASSERT_NE(it, end);
    EXPECT_EQ(it->first, header_id("user-agent"));
    EXPECT_EQ(it->second, "TestClient");

    ++it;
    EXPECT_EQ(it, end);
}

TEST_F(CGIHeadersIteratorTest, SpecialHttpHeaders) {
    std::vector<std::string> env  = {"CONTENT_LENGTH=1024", "CONTENT_TYPE=application/json", "HTTP_ACCEPT=*/*"};
    auto                     envp = create_envp(env);

    cgi_headers_iterator it(const_cast<char**>(envp.data()));
    cgi_headers_iterator end;

    ASSERT_NE(it, end);
    EXPECT_EQ(it->first, header_id("content-length"));
    EXPECT_EQ(it->second, "1024");

    ++it;
    ASSERT_NE(it, end);
    EXPECT_EQ(it->first, header_id("content-type"));
    EXPECT_EQ(it->second, "application/json");

    ++it;
    ASSERT_NE(it, end);
    EXPECT_EQ(it->first, header_id("accept"));
    EXPECT_EQ(it->second, "*/*");

    ++it;
    EXPECT_EQ(it, end);
}

TEST_F(CGIHeadersIteratorTest, MalformedAndInvalidVariables) {
    std::vector<std::string> env = {
      "HTTP_INVALID-CHARS=value", // Contains '-', should skip due to invalid_header_id
      "HTTP_VALID=ok"             // Should be processed
    };
    auto envp = create_envp(env);

    cgi_headers_iterator it(const_cast<char**>(envp.data()));
    cgi_headers_iterator end;

    ASSERT_NE(it, end);
    EXPECT_EQ(it->first, header_id("valid"));
    EXPECT_EQ(it->second, "ok");

    ++it;
    EXPECT_EQ(it, end);
}

TEST_F(CGIHeadersIteratorTest, IteratorPostfixIncrement) {
    std::vector<std::string> env  = {"HTTP_A=1", "HTTP_B=2"};
    auto                     envp = create_envp(env);

    cgi_headers_iterator it(const_cast<char**>(envp.data()));
    cgi_headers_iterator end;

    ASSERT_NE(it, end);
    auto old_it = it++; // Postfix increment

    EXPECT_EQ(old_it->first, header_id("a"));
    EXPECT_EQ(it->first, header_id("b"));
}
