#include "../webpp/strings/charset.hpp"
#include "../webpp/strings/string_tokenizer.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;

TEST(StringTokenizerTest, Init) {
    constexpr stl::string_view str{"Hello;world"};
    string_tokenizer           tok{str};

    EXPECT_TRUE(tok.next(charset{';'}));
    EXPECT_EQ(tok.token(), "Hello");
    EXPECT_TRUE(tok.next(charset{';'}));
    EXPECT_EQ(tok.token(), "world");
}

TEST(StringTokenizerTest, EmptyNext) {
    constexpr stl::string_view str{"Hello;world"};
    string_tokenizer           tok{str};

    EXPECT_TRUE(tok.next(charset{';'}));
    EXPECT_EQ(tok.token(), "Hello");
    EXPECT_TRUE(tok.next(charset{';'}));
    EXPECT_EQ(tok.token(), "world");
}

TEST(StringTokenizerTest, QuotesTest) {
    std::string_view const input = R"(no-cache="foo,\" bar", private)";
    string_tokenizer       t(input);

    EXPECT_TRUE(t.next(charset{',', ' '}, charset{'"'}));
    EXPECT_EQ("no-cache=\"foo,\\\" bar\"", t.token()) << t.token();

    EXPECT_TRUE(t.next(charset{',', ' '}));
    EXPECT_EQ("private", t.token()) << t.token();
}

TEST(StringTokenizerTest, ReturnDelimsOptionTest) {
    std::string_view const input = R"(no-cache="foo,\" bar", private)";
    string_tokenizer       t(input);

    static constexpr auto options = static_cast<stl::uint8_t>(string_tokenizer_options::return_delims);

    EXPECT_TRUE(t.next<options>(charset{',', ' '}, charset{'"'}));
    EXPECT_EQ("no-cache=\"foo,\\\" bar\"", t.token()) << t.token();

    EXPECT_TRUE(t.next<options>(charset{',', ' '}));
    EXPECT_EQ(",", t.token()) << t.token();

    EXPECT_TRUE(t.next<options>(charset{',', ' '}));
    EXPECT_EQ(" ", t.token()) << t.token();

    EXPECT_TRUE(t.next<options>(charset{',', ' '}));
    EXPECT_EQ("private", t.token()) << t.token();
}

enum struct errors {
    ok,
    no_one,
    no_two
};

TEST(StringTokenizerTest, SimpleUsage) {
    using namespace webpp::stl;
    static constexpr auto OWS = charset{' ', '\t'};

    string_view const str{"one two; foo=baz1; bar='baz2'"};
    string_tokenizer  tok{str};

    string                        one, two;
    map<string_view, string_view> vals;
    errors                        err = errors::ok;

    tok.expect(ALPHA<>, one, err, errors::no_one);
    tok.skip_spaces();
    tok.expect(ALPHA<>, two, err, errors::no_two);
    tok.skip(charset{';'}, OWS);
    while (!tok.at_end()) {
        tok.skip_spaces();
        string_view name, value;
        tok.expect(ALPHA<>, name);
        tok.skip_spaces();
        tok.skip(charset{'='});
        tok.skip_spaces();
        tok.expect(charset(ALPHA<>, DIGIT<>, OWS), charset{'"', '\''}, value);
        vals[name] = value;
        tok.skip(charset{';'});
    }

    EXPECT_EQ(err, errors::ok);
    EXPECT_EQ(one, "one");
    EXPECT_EQ(two, "two");
    ASSERT_EQ(vals.size(), 2);
    EXPECT_EQ(vals["foo"], "baz1");
    EXPECT_EQ(vals["bar"], "baz2");
}

// enum struct toker_errors {
//     ok,
//     error1,
//     error2,
//     error3
// };
// TEST(StringTokenizerTest, ParserInit) {
//     stl::string_view str{"Hello;world"};
//     parser<string_tokenizer<>, toker_errors> toker{str};
//
//     EXPECT_EQ(toker.next(charset{';'}), toker_error::ok);
//     EXPECT_EQ(toker.token(), "Hello");
//     EXPECT_EQ(toker.next(), toker_error::OK);
//     EXPECT_EQ(toker.token(), "world");
//     EXPECT_EQ(toker.next(charset{'-'}, toker_error::error1), toker_error::error1);
// }
//
// TEST(StringTokenizerTest, ParserInit) {
//     stl::string_view str{"GET HTTP/1.1"};
//     parser<string_tokenizer<>, toker_errors> toker{str};
//
//     stl::string method;
//     toker.expect(toker_errors::error3, method, "POST", "GET", "CONNECT");
//     toker.skip_spaces();
//     toker.expect(toker_errors::error2, "HTTP/1.1");
//     EXPECT_EQ(toker_errors::ok, toker.status());
//     EXPECT_EQ(method, "GET");
// }
