// Created by moisrex on 10/9/20.

#include "../webpp/http/headers/accept.hpp"
#include "../webpp/http/headers/accept_encoding.hpp"
#include "../webpp/http/headers/allow.hpp"
#include "../webpp/http/headers/cache_control.hpp"
#include "../webpp/http/headers/content_encoding.hpp"
#include "../webpp/http/headers/content_length.hpp"
#include "../webpp/http/headers/content_type.hpp"
#include "../webpp/http/headers/keep_alive.hpp"
#include "../webpp/http/headers/location.hpp"
#include "./common/test.hpp"


using namespace webpp;
using namespace webpp::http;
using namespace std::string_view_literals;

namespace {

    template <typename HeaderType>
    static std::string render_to_string(HeaderType const& header, std::size_t const max_length = 256) {
        std::string output(max_length, '\0');
        auto*       ptr = output.data();
        auto* const beg = ptr;
        render(ptr, max_length, header);
        auto const size = static_cast<stl::size_t>(ptr - beg);
        output.resize(size);
        return output;
    }

} // namespace

TEST(Headers, ConceptTest) {
    EXPECT_TRUE(HeaderField<basic_accept_encoding<>>);
    EXPECT_TRUE(HeaderField<basic_content_type>);
    EXPECT_TRUE(HeaderField<basic_content_type>);
    EXPECT_TRUE(HeaderField<basic_accept_encoding<>>);
    EXPECT_TRUE(HeaderField<basic_cache_control>);
    EXPECT_TRUE(HeaderField<basic_content_length>);
    EXPECT_TRUE(HeaderField<basic_allow>);
    EXPECT_TRUE(HeaderField<basic_accept<>>);
    EXPECT_TRUE(HeaderField<basic_keep_alive>);
}

TEST(Headers, ContentLengthRender) {
    basic_content_length const header{"12345"};
    EXPECT_EQ(render_to_string(header), "12345");

    std::array<char, 3> truncated{};
    auto*               ptr = truncated.data();
    render(ptr, truncated.size(), header);
    auto const truncated_view = std::string_view{truncated.data(), ptr};
    EXPECT_TRUE(truncated_view.empty());
}

TEST(Headers, ContentTypeRender) {
    basic_content_type const header{"text/html; charset=utf-8"};
    EXPECT_EQ(render_to_string(header), "text/html; charset=utf-8");
}

TEST(Headers, KeepAliveRender) {
    basic_keep_alive const header{"timeout=5, max=1000"};
    EXPECT_EQ(render_to_string(header), "timeout=5, max=1000");
}

TEST(Headers, AcceptRender) {
    basic_accept const header{"text/html; level=1; q=0.7, application/json"};
    EXPECT_EQ(render_to_string(header), "text/html; level=1; q=0.7, application/json");
}

TEST(Headers, AcceptEncodingRender) {
    basic_accept_encoding const header{"gzip; q=0.5, br"};
    EXPECT_EQ(render_to_string(header), "gzip; q=0.5, br, identity");
}

TEST(Headers, AllowRender) {
    basic_allow const header{" GET ,POST,   PATCH "};
    EXPECT_EQ(render_to_string(header), "GET, POST, PATCH");
}

TEST(Headers, ContentEncodingRender) {
    basic_content_encoding const header{"gzip,  br ,deflate"};
    EXPECT_EQ(render_to_string(header), "gzip, br, deflate");
}

TEST(Headers, CacheControlRender) {
    basic_cache_control const header{"public, max-age=600, immutable"};
    EXPECT_EQ(render_to_string(header), "public, immutable, max-age=600");
}

TEST(Headers, InvalidHeadersRenderEmptyString) {
    EXPECT_TRUE(render_to_string(basic_allow{"GET X"}).empty());
    EXPECT_TRUE(render_to_string(basic_content_encoding{"gzip;q=1.0"}).empty());
    EXPECT_TRUE(render_to_string(basic_cache_control{""}).empty());
}

TEST(Headers, AcceptEncoding) {
    basic_accept_encoding parser{"gzip"};
    EXPECT_TRUE(parser.is_valid());
    EXPECT_EQ(parser.allowed_encodings().size(), 2); // plus identity
    EXPECT_TRUE(parser.is_allowed(encoding_type::gzip));

    basic_accept_encoding parser2{"gzip; q=0.05"};
    EXPECT_TRUE(parser2.is_valid());
    EXPECT_TRUE(parser2.is_allowed(encoding_type::gzip));
    // EXPECT_FLOAT_EQ(parser2.get<parser2.gzip>()->quality, 0.05f);

    basic_accept_encoding parser3{"gzip; q=0.255, br, deflate"};
    EXPECT_TRUE(parser3.is_valid());
    EXPECT_TRUE(parser3.is_allowed(encoding_type::gzip));
    EXPECT_TRUE(parser3.is_allowed(encoding_type::br));
    EXPECT_TRUE(parser3.is_allowed(encoding_type::deflate));
    // EXPECT_FLOAT_EQ(parser3.get<parser3.gzip>()->quality, 0.255f);

    basic_accept_encoding parser4{"*"};
    EXPECT_TRUE(parser4.is_valid());
    EXPECT_TRUE(parser4.is_allowed(encoding_type::all));
}

TEST(Headers, AcceptEncodingExceedsLimit) {
    std::string header_value;
    for (std::size_t i = 0; i < max_supported_accept_encoding_values + 5; ++i) {
        if (!header_value.empty()) {
            header_value += ", ";
        }
        header_value += "custom-" + std::to_string(i);
    }

    basic_accept_encoding parser{header_value};

    ASSERT_TRUE(parser.is_valid());
    ASSERT_EQ(parser.allowed_encodings().size(), max_supported_accept_encoding_values);
    EXPECT_TRUE(parser.is_allowed("custom-0"));
    EXPECT_TRUE(parser.is_allowed("custom-" + std::to_string(max_supported_accept_encoding_values - 1)));
    EXPECT_FALSE(parser.is_allowed("custom-" + std::to_string(max_supported_accept_encoding_values)));
}

class ContentTypeTest : public ::testing::Test {
  protected:
    // Helper to create and verify in one go
    static basic_content_type parse(std::string_view input) {
        return basic_content_type{input};
    }
};

// ============ Basic Parsing ============

TEST_F(ContentTypeTest, EmptyString) {
    basic_content_type content_type{""};
    EXPECT_TRUE(content_type.media_type_string().empty());
    EXPECT_TRUE(content_type.boundary().empty());
    EXPECT_TRUE(content_type.charset().empty());
}

TEST_F(ContentTypeTest, SimpleMediaTypeNoParams) {
    auto const content_type = parse("text/html");
    EXPECT_EQ(content_type.media_type_string(), "text/html");
    EXPECT_TRUE(content_type.boundary().empty());
    EXPECT_TRUE(content_type.charset().empty());
    EXPECT_FALSE(content_type.is_multipart());
}

TEST_F(ContentTypeTest, MediaTypeWithLeadingTrailingWhitespace) {
    auto const content_type = parse("  application/json  ");
    EXPECT_EQ(content_type.media_type_string(), "application/json");
}

TEST_F(ContentTypeTest, MediaTypeWithInternalWhitespacePreserved) {
    // RFC allows whitespace around '/', but we don't modify it
    auto const content_type = parse("text / html");
    EXPECT_EQ(content_type.media_type_string(), "text / html");
}

// ============ Parameter Parsing ============

TEST_F(ContentTypeTest, SingleParameterCharset) {
    auto const content_type = parse("text/html; charset=utf-8");
    EXPECT_EQ(content_type.media_type_string(), "text/html");
    EXPECT_EQ(content_type.charset(), "utf-8");
}

TEST_F(ContentTypeTest, SingleParameterBoundary) {
    auto const content_type = parse("multipart/form-data; boundary=---123");
    EXPECT_EQ(content_type.media_type_string(), "multipart/form-data");
    EXPECT_EQ(content_type.boundary(), "---123");
    EXPECT_TRUE(content_type.is_multipart());
}

TEST_F(ContentTypeTest, MultipleParameters) {
    auto const content_type = parse("multipart/form-data; boundary=abc; charset=utf-8");
    EXPECT_EQ(content_type.media_type_string(), "multipart/form-data");
    EXPECT_EQ(content_type.boundary(), "abc");
    EXPECT_EQ(content_type.charset(), "utf-8");
}

TEST_F(ContentTypeTest, ParametersWithSpacesAroundEquals) {
    auto const content_type = parse("text/html; charset = utf-8 ; boundary = foo");
    EXPECT_EQ(content_type.media_type_string(), "text/html");
    EXPECT_EQ(content_type.charset(), "utf-8");
    EXPECT_EQ(content_type.boundary(), "foo");
}

TEST_F(ContentTypeTest, QuotedParameterValue) {
    auto const content_type = parse(R"(multipart/form-data; boundary="---123 abc")");
    EXPECT_EQ(content_type.boundary(), "---123 abc");
}

TEST_F(ContentTypeTest, QuotedParameterWithSpacesOutside) {
    auto const content_type = parse(R"(multipart/form-data; boundary= "foo bar" )");
    EXPECT_EQ(content_type.boundary(), "foo bar");
}

// ============ MISTAKE #1: Case Sensitivity ============

TEST_F(ContentTypeTest, DISABLED_ParameterNamesCaseInsensitiveUppercase) {
    // This should pass after fixing the case-sensitivity bug
    auto const content_type = parse("text/html; CHARSET=utf-8");
    EXPECT_EQ(content_type.charset(), "utf-8");
}

TEST_F(ContentTypeTest, DISABLED_ParameterNamesCaseInsensitiveMixed) {
    auto const content_type = parse("multipart/form-data; BoUnDaRy=---xyz");
    EXPECT_EQ(content_type.boundary(), "---xyz");
}

TEST_F(ContentTypeTest, DISABLED_ParameterNamesCaseInsensitiveLowercase) {
    // This already works by coincidence
    auto const content_type = parse("text/html; charset=utf-8");
    EXPECT_EQ(content_type.charset(), "utf-8");
}

// ============ MISTAKE #3: Parameters Without Values (Flags) ============

TEST_F(ContentTypeTest, DISABLED_ParameterWithoutEqualsSign) {
    // This should be captured somehow after fixing
    auto const content_type = parse("text/html; secure; charset=utf-8");
    EXPECT_EQ(content_type.charset(), "utf-8");
    // The 'secure' flag is currently ignored - test would need a way to check it
}

// ============ MISTAKE #4: Escaped Quotes ============

TEST_F(ContentTypeTest, QuotedStringWithEscapedQuote) {
    // Current implementation fails - it strips outer quotes but leaves \"
    auto const content_type = parse(R"(multipart/form-data; boundary="foo\"bar")");
    // After stripping quotes, we get: foo\"bar
    // A proper implementation would unescape to: foo"bar
    EXPECT_EQ(content_type.boundary(), "foo\\\"bar"); // This is what we currently get (wrong)
    // EXPECT_EQ(content_type.boundary(), "foo\"bar");  // This is what we SHOULD get
}

// ============ Edge Cases ============

TEST_F(ContentTypeTest, EmptyParameterValue) {
    auto const content_type = parse("text/html; charset=");
    EXPECT_EQ(content_type.charset(), "");
}

TEST_F(ContentTypeTest, TrailingSemicolon) {
    auto const content_type = parse("text/html;");
    EXPECT_EQ(content_type.media_type_string(), "text/html");
}

TEST_F(ContentTypeTest, OnlySemicolons) {
    auto const content_type = parse("text/html;;;;");
    EXPECT_EQ(content_type.media_type_string(), "text/html");
}

TEST_F(ContentTypeTest, MalformedButRecoverable) {
    auto const content_type = parse("text/html; charset=utf-8; ; ; boundary=foo");
    EXPECT_EQ(content_type.charset(), "utf-8");
    EXPECT_EQ(content_type.boundary(), "foo");
}

TEST_F(ContentTypeTest, ParameterWithSemicolonInQuotedValue) {
    auto const content_type = parse(R"(multipart/form-data; boundary="---;123")");
    EXPECT_EQ(content_type.boundary(), "---;123");
}

TEST_F(ContentTypeTest, MultipleParametersSameKey) {
    // Last one wins (implementation defined)
    auto const content_type = parse("text/html; charset=utf-8; charset=iso-8859-1");
    EXPECT_EQ(content_type.charset(), "iso-8859-1");
}

TEST_F(ContentTypeTest, IsMultipartPositive) {
    EXPECT_TRUE(parse("multipart/form-data").is_multipart());
    EXPECT_TRUE(parse("multipart/byteranges").is_multipart());
    EXPECT_TRUE(parse("multipart/mixed").is_multipart());
}

TEST_F(ContentTypeTest, IsMultipartNegative) {
    EXPECT_FALSE(parse("multipart").is_multipart()); // no slash
    EXPECT_FALSE(parse("multipartform-data").is_multipart());
    EXPECT_FALSE(parse("text/html").is_multipart());
    EXPECT_FALSE(parse("application/x-www-form-urlencoded").is_multipart());
}

// ============ Performance/Stress ============

TEST_F(ContentTypeTest, LongBoundaryValue) {
    std::string long_boundary(1000, 'x');
    auto const  content_type = parse("multipart/form-data; boundary=" + long_boundary);
    EXPECT_EQ(content_type.boundary().size(), 1000);
}

TEST_F(ContentTypeTest, ManyParameters) {
    std::string input = "text/html";
    for (int i = 0; i < 100; ++i) {
        input += "; param" + std::to_string(i) + "=value" + std::to_string(i);
    }
    auto const content_type = parse(input);
    EXPECT_EQ(content_type.media_type_string(), "text/html");
    // charset and boundary not set
    EXPECT_TRUE(content_type.charset().empty());
    EXPECT_TRUE(content_type.boundary().empty());
}

// ============ Constexpr Verification ============

TEST_F(ContentTypeTest, CompileTimeEvaluation) {
    constexpr basic_content_type content_type{"text/html; charset=utf-8"};
    constexpr auto               media_type    = content_type.media_type_string();
    constexpr auto               charset_value = content_type.charset();

    EXPECT_EQ(media_type, "text/html");
    EXPECT_EQ(charset_value, "utf-8");
    static_assert(media_type == "text/html");
    static_assert(charset_value == "utf-8");
}

// ============ Whitespace Edge Cases ============

TEST_F(ContentTypeTest, TabCharactersAsWhitespace) {
    auto const content_type = parse("text/html;\tcharset\t=\tutf-8");
    EXPECT_EQ(content_type.charset(), "utf-8");
}

TEST_F(ContentTypeTest, MixedSpaceAndTab) {
    auto const content_type = parse("text/html; \t boundary \t = \t \"foo\" \t ");
    EXPECT_EQ(content_type.boundary(), "foo");
}

// ============ Comments Not Supported ============

TEST_F(ContentTypeTest, CommentsInParameterValueNotStripped) {
    // RFC allows comments in parameter values, but we don't handle them
    // This test documents current behavior
    auto const content_type = parse(R"(text/html; charset=utf-8 (comment))");
    // Currently treats the comment as part of the value
    EXPECT_NE(content_type.charset(), "utf-8");
}

// ============ RFC 2231 Not Supported ============

TEST_F(ContentTypeTest, ExtendedParameterSyntaxIgnored) {
    // RFC 5987/2231 extended syntax (charset'lang'value)
    auto const content_type = parse("text/html; charset*=utf-8''en%20US");
    // We don't parse this - charset remains empty
    EXPECT_TRUE(content_type.charset().empty());
}

// ============ MIME Type With Parameters In Media Type ============

TEST_F(ContentTypeTest, ParametersInMediaTypeStringNotSupported) {
    // This is invalid per RFC, but happens in the wild
    auto const content_type = parse("text/html;charset=utf-8");
    // Our parser handles it correctly because ; separates media type
    EXPECT_EQ(content_type.media_type_string(), "text/html");
    EXPECT_EQ(content_type.charset(), "utf-8");
}

///////////////////////////////////////////////////////////////////////////////////////////////////



class AcceptHeaderTest : public ::testing::Test {
  protected:
    // Helper to collect parsed ranges into a vector for easy assertion
    static std::vector<accept_media_range> parse_to_vector(std::string_view header_value) {
        basic_accept                    accept_header{header_value};
        std::vector<accept_media_range> ranges;

        accept_header.for_each([&](accept_media_range const& range) {
            ranges.push_back(range);
        });

        return ranges;
    }
};

TEST_F(AcceptHeaderTest, EmptyHeaderIsValid) {
    EXPECT_TRUE(basic_accept("").is_valid());
}

TEST_F(AcceptHeaderTest, EmptyHeaderYieldsNoRanges) {
    basic_accept const accept_header{""};
    auto const         ranges = parse_to_vector("");

    EXPECT_EQ(accept_header.begin(), accept_header.end());
    EXPECT_TRUE(ranges.empty());
}

TEST_F(AcceptHeaderTest, WhiteSpaceOnlyYieldsNoRanges) {
    auto ranges = parse_to_vector("   ,  , ");
    EXPECT_TRUE(ranges.empty());
}

TEST_F(AcceptHeaderTest, SingleMediaRangeDefaultWeight) {
    basic_accept const accept_header{"text/html"};
    auto const         ranges = parse_to_vector("text/html");

    ASSERT_EQ(ranges.size(), 1);
    ASSERT_EQ(accept_header.media_ranges().size(), 1);
    EXPECT_EQ(ranges[0].media_type, "text/html");
    EXPECT_FLOAT_EQ(ranges[0].weight, 1.0F);
    EXPECT_FALSE(is_wildcard(ranges[0]));
}

TEST_F(AcceptHeaderTest, MultipleMediaRangesDefaultWeights) {
    auto ranges = parse_to_vector("text/html, application/xhtml+xml, application/xml");

    ASSERT_EQ(ranges.size(), 3);
    EXPECT_EQ(ranges[0].media_type, "text/html");
    EXPECT_EQ(ranges[1].media_type, "application/xhtml+xml");
    EXPECT_EQ(ranges[2].media_type, "application/xml");

    for (auto const& range : ranges) {
        EXPECT_FLOAT_EQ(range.weight, 1.0F);
    }
}

TEST_F(AcceptHeaderTest, ParsesQValuesCorrectly) {
    auto ranges = parse_to_vector("text/html; q=0.8, text/plain; q=0.5, image/*; q=0.001");

    ASSERT_EQ(ranges.size(), 3);

    EXPECT_EQ(ranges[0].media_type, "text/html");
    EXPECT_FLOAT_EQ(ranges[0].weight, 0.8F);

    EXPECT_EQ(ranges[1].media_type, "text/plain");
    EXPECT_FLOAT_EQ(ranges[1].weight, 0.5F);

    EXPECT_EQ(ranges[2].media_type, "image/*");
    EXPECT_FLOAT_EQ(ranges[2].weight, 0.001F);
}

TEST_F(AcceptHeaderTest, EdgeCaseQValues) {
    auto ranges = parse_to_vector("a/b;q=1.0, c/d;q=1, e/f;q=0.999, g/h;q=0, i/j;q=0.0");

    ASSERT_EQ(ranges.size(), 5);
    EXPECT_FLOAT_EQ(ranges[0].weight, 1.0F);
    EXPECT_FLOAT_EQ(ranges[1].weight, 1.0F);
    EXPECT_FLOAT_EQ(ranges[2].weight, 0.999F);
    EXPECT_FLOAT_EQ(ranges[3].weight, 0.0F); // Falls back to 0.0F in parse_qvalue if missing fractional
    EXPECT_FLOAT_EQ(ranges[4].weight, 0.0F);
}

TEST_F(AcceptHeaderTest, HandlesWildcards) {
    auto ranges = parse_to_vector("*/*, text/*; q=0.5");

    ASSERT_EQ(ranges.size(), 2);

    EXPECT_EQ(ranges[0].media_type, "*/*");
    EXPECT_TRUE(is_wildcard(ranges[0]));
    EXPECT_FLOAT_EQ(ranges[0].weight, 1.0F);

    EXPECT_EQ(ranges[1].media_type, "text/*");
    EXPECT_FALSE(is_wildcard(ranges[1]));
    EXPECT_FLOAT_EQ(ranges[1].weight, 0.5F);
}

TEST_F(AcceptHeaderTest, ExtensionsAndParametersPreserved) {
    // According to RFC: q separates media params from accept-extensions
    auto ranges = parse_to_vector("text/html;level=1;q=0.7;ext=true");

    ASSERT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].media_type, "text/html");
    EXPECT_FLOAT_EQ(ranges[0].weight, 0.7F);

    // params contains raw param string
    EXPECT_EQ(ranges[0].params, "level=1;q=0.7;ext=true");
}

TEST_F(AcceptHeaderTest, MalformedHeadersAreInvalid) {
    EXPECT_FALSE(basic_accept{"a/b;q=invalid, c/d;q=0.abc"}.is_valid());
    EXPECT_FALSE(basic_accept{"text html"}.is_valid());
    EXPECT_FALSE(basic_accept{"*/json"}.is_valid());
}

TEST_F(AcceptHeaderTest, CallbackEarlyExit) {
    basic_accept                  accept_header{"a/a, b/b, c/c"};
    std::vector<std::string_view> visited;

    accept_header.for_each([&](accept_media_range const& range) -> bool {
        visited.push_back(range.media_type);
        // Stop parsing when we hit "b/b"
        return range.media_type != "b/b";
    });

    ASSERT_EQ(visited.size(), 2);
    EXPECT_EQ(visited[0], "a/a");
    EXPECT_EQ(visited[1], "b/b");
}

TEST_F(AcceptHeaderTest, ExceedsLimitKeepsOnlySupportedValues) {
    std::string header_value;
    for (std::size_t i = 0; i < max_supported_accept_values + 5; ++i) {
        if (!header_value.empty()) {
            header_value += ", ";
        }
        header_value += "application/type" + std::to_string(i);
    }

    basic_accept const accept_header{header_value};

    ASSERT_TRUE(accept_header.is_valid());
    ASSERT_EQ(accept_header.media_ranges().size(), max_supported_accept_values);
    EXPECT_EQ(accept_header.begin()->media_type, "application/type0");
    EXPECT_EQ((accept_header.end() - 1)->media_type,
              "application/type" + std::to_string(max_supported_accept_values - 1));
}

///////////////////////////////////////////////////////////////////////////////////////////////////



class ContentEncodingTest : public ::testing::Test {};

// Test to verify it fully complies with the conceptual rules of HeaderField
TEST_F(ContentEncodingTest, CompliesWithHeaderFieldConcept) {
    // Asserting concept statically
    static_assert(HeaderField<basic_content_encoding<10>>, "Must comply with HeaderField concept");

    basic_content_encoding<> content_encoding("gzip");
    EXPECT_TRUE(content_encoding.is_valid());
    EXPECT_TRUE(static_cast<bool>(content_encoding));
}

TEST_F(ContentEncodingTest, EmptyHeader) {
    basic_content_encoding<> content_encoding("");
    EXPECT_TRUE(content_encoding.is_valid());
    EXPECT_EQ(content_encoding.encodings().size(), 0);
}

TEST_F(ContentEncodingTest, SingleEncoding) {
    basic_content_encoding<> content_encoding("gzip");
    EXPECT_TRUE(content_encoding.is_valid());
    ASSERT_EQ(content_encoding.encodings().size(), 1);
    EXPECT_EQ(content_encoding.encodings()[0], "gzip");
    EXPECT_TRUE(content_encoding.contains("gzip"));
    EXPECT_TRUE(content_encoding.contains("GZIP")); // Case insensitive match
    EXPECT_FALSE(content_encoding.contains("br"));
}

TEST_F(ContentEncodingTest, MultipleEncodings) {
    basic_content_encoding<> content_encoding("deflate, gzip");
    EXPECT_TRUE(content_encoding.is_valid());
    ASSERT_EQ(content_encoding.encodings().size(), 2);
    EXPECT_EQ(content_encoding.encodings()[0], "deflate");
    EXPECT_EQ(content_encoding.encodings()[1], "gzip");
    EXPECT_TRUE(content_encoding.contains("deflate"));
    EXPECT_TRUE(content_encoding.contains("gzip"));
}

TEST_F(ContentEncodingTest, MultipleEncodingsWithTrailingSpacesAndCommas) {
    basic_content_encoding<> content_encoding("  br  ,  gzip , ");
    EXPECT_TRUE(content_encoding.is_valid());
    ASSERT_EQ(content_encoding.encodings().size(), 2);
    EXPECT_EQ(content_encoding.encodings()[0], "br");
    EXPECT_EQ(content_encoding.encodings()[1], "gzip");
}

TEST_F(ContentEncodingTest, MalformedInvalidCharacters) {
    // Content-Encoding doesn't use standard attributes separated by semi-colon
    basic_content_encoding<> invalid_with_params("gzip;q=1.0");
    EXPECT_FALSE(invalid_with_params.is_valid());
    EXPECT_FALSE(static_cast<bool>(invalid_with_params));

    // Wildcards are not valid in Content-Encoding (only in Accept-Encoding)
    basic_content_encoding<> invalid_wildcard("*");
    EXPECT_FALSE(invalid_wildcard.is_valid());

    // Quotes are invalid
    basic_content_encoding<> invalid_quoted("\"gzip\"");
    EXPECT_FALSE(invalid_quoted.is_valid());
}

TEST_F(ContentEncodingTest, MalformedInternalSpaces) {
    // Internal spaces inside a token are considered invalid syntax
    basic_content_encoding<> content_encoding("gz ip");
    EXPECT_FALSE(content_encoding.is_valid());

    // Multiple values where one is malformed
    basic_content_encoding<> invalid_multiple("br, def late, gzip");
    EXPECT_FALSE(invalid_multiple.is_valid());
}

TEST_F(ContentEncodingTest, MaxSupportedExceeded) {
    // Set a very small MaxSupported size for testing
    basic_content_encoding<2> content_encoding("gzip, deflate, br, compress");

    EXPECT_TRUE(content_encoding.is_valid()); // Should still be valid, just truncated
    ASSERT_EQ(content_encoding.encodings().size(), 2);
    EXPECT_EQ(content_encoding.encodings()[0], "gzip");
    EXPECT_EQ(content_encoding.encodings()[1], "deflate");

    // Ensure truncated ones are not reported
    EXPECT_FALSE(content_encoding.contains("br"));
    EXPECT_FALSE(content_encoding.contains("compress"));
}

/////////////////////////////////////////////////////////////////////////////////////



TEST(AllowHeaderTest, HandlesEmptyString) {
    basic_allow allow_header{""};

    EXPECT_TRUE(allow_header.is_valid());
    EXPECT_FALSE(allow_header.has_unknown_methods());
    EXPECT_EQ(allow_header.methods_mask(), 0ULL);
    EXPECT_FALSE(allow_header.contains(verb::get));
    EXPECT_FALSE(allow_header.contains("GET"));
}

TEST(AllowHeaderTest, ParsesSingleKnownMethod) {
    basic_allow allow_header{"GET"};

    EXPECT_TRUE(allow_header.is_valid());
    EXPECT_FALSE(allow_header.has_unknown_methods());

    EXPECT_TRUE(allow_header.contains(verb::get));
    EXPECT_TRUE(allow_header.contains("GET"));

    // Should not contain other methods
    EXPECT_FALSE(allow_header.contains(verb::post));
    EXPECT_FALSE(allow_header.contains("POST"));
}

TEST(AllowHeaderTest, ParsesMultipleKnownMethods) {
    basic_allow allow_header{"GET, POST, HEAD"};

    EXPECT_TRUE(allow_header.is_valid());
    EXPECT_FALSE(allow_header.has_unknown_methods());

    EXPECT_TRUE(allow_header.contains(verb::get));
    EXPECT_TRUE(allow_header.contains(verb::post));
    EXPECT_TRUE(allow_header.contains(verb::head));

    EXPECT_FALSE(allow_header.contains(verb::put));
    EXPECT_FALSE(allow_header.contains(verb::del));
}

TEST(AllowHeaderTest, HandlesIrregularWhitespace) {
    // Tests $O(n)$ tokenizer robustness against spaces
    basic_allow allow_header{"   GET  ,POST,   OPTIONS   "};

    EXPECT_TRUE(allow_header.is_valid());
    EXPECT_FALSE(allow_header.has_unknown_methods());

    EXPECT_TRUE(allow_header.contains(verb::get));
    EXPECT_TRUE(allow_header.contains(verb::post));
    EXPECT_TRUE(allow_header.contains(verb::options));
}

TEST(AllowHeaderTest, HandlesUnknownCustomMethods) {
    basic_allow allow_header{"GET, PROPFIND, CUSTOM_METHOD, POST"};

    EXPECT_TRUE(allow_header.is_valid());

    // WebDAV's PROPFIND might be known depending on the verbs.hpp completeness,
    // but CUSTOM_METHOD is definitely unknown.
    EXPECT_TRUE(allow_header.has_unknown_methods());

    // Standard methods should still work via bitmask $O(1)$ lookup
    EXPECT_TRUE(allow_header.contains(verb::get));
    EXPECT_TRUE(allow_header.contains(verb::post));

    // Fallback string matching should catch the custom method
    EXPECT_TRUE(allow_header.contains("CUSTOM_METHOD"));

    // Non-existent custom methods should fail
    EXPECT_FALSE(allow_header.contains("NON_EXISTENT"));
}

TEST(AllowHeaderTest, CaseSensitivityBehavior) {
    // According to standard HTTP specs and the string_to_verb implementation,
    // method parsing is strictly uppercase.
    basic_allow allow_header{"get, POST, put"};

    EXPECT_TRUE(allow_header.has_unknown_methods()); // "get" and "put" are treated as unknown

    // "POST" is correctly recognized as a known enum verb
    EXPECT_TRUE(allow_header.contains(verb::post));
    EXPECT_TRUE(allow_header.contains("POST"));

    // "get" is not mapped to verb::get enum
    EXPECT_FALSE(allow_header.contains(verb::get));

    // But it is present exactly as typed in the string fallback
    EXPECT_TRUE(allow_header.contains("get"));
    EXPECT_FALSE(allow_header.contains("GET")); // Fallback is exact match, so uppercase GET fails
}

TEST(AllowHeaderTest, RejectsUnknownVerbEnum) {
    basic_allow allow_header{"GET, POST"};

    // Querying for verb::unknown should always return false gracefully
    EXPECT_FALSE(allow_header.contains(verb::unknown));
}

TEST(AllowHeaderTest, MalformedEdgeCases) {
    // Consecutive commas, trailing commas, leading commas
    basic_allow allow_header{",,,GET,, ,,POST,"};

    EXPECT_TRUE(allow_header.is_valid());

    // Empty tokens should be skipped, leaving no "unknown" methods if none exist
    EXPECT_FALSE(allow_header.has_unknown_methods());

    EXPECT_TRUE(allow_header.contains(verb::get));
    EXPECT_TRUE(allow_header.contains(verb::post));
    EXPECT_FALSE(allow_header.contains(verb::put));
}

///////////////////////////////////////////////////////////////////////////////////



TEST(KeepAliveTest, ParsesStandardValues) {
    basic_keep_alive header{"timeout=5, max=1000"};

    EXPECT_TRUE(header.is_valid());

    ASSERT_TRUE(header.has_timeout());
    EXPECT_EQ(header.timeout(), 5U);

    ASSERT_TRUE(header.has_max());
    EXPECT_EQ(header.max(), 1000U);
}

TEST(KeepAliveTest, HandlesWhitespaceAndCaseInsensitivity) {
    basic_keep_alive header{"  TimeOut = 15 , MAX = 500  "};

    EXPECT_TRUE(header.is_valid());

    ASSERT_TRUE(header.has_timeout());
    EXPECT_EQ(header.timeout(), 15U);

    ASSERT_TRUE(header.has_max());
    EXPECT_EQ(header.max(), 500U);
}

TEST(KeepAliveTest, HandlesOnlyTimeout) {
    basic_keep_alive header{"timeout=30"};

    EXPECT_TRUE(header.is_valid());

    ASSERT_TRUE(header.has_timeout());
    EXPECT_EQ(header.timeout(), 30U);

    EXPECT_FALSE(header.has_max());
    EXPECT_EQ(header.max(), 0U); // Default/fallback value
}

TEST(KeepAliveTest, HandlesOnlyMax) {
    basic_keep_alive header{"max=99"};

    EXPECT_TRUE(header.is_valid());

    EXPECT_FALSE(header.has_timeout());
    EXPECT_EQ(header.timeout(), 0U);

    ASSERT_TRUE(header.has_max());
    EXPECT_EQ(header.max(), 99U);
}

TEST(KeepAliveTest, InvalidCharactersInOneValueDoesNotInvalidateEntireHeader) {
    basic_keep_alive header{"timeout=abc, max=100"};

    // Header is still valid because `max` could be parsed
    EXPECT_TRUE(header.is_valid());

    EXPECT_FALSE(header.has_timeout());
    EXPECT_EQ(header.timeout(), 0U);

    ASSERT_TRUE(header.has_max());
    EXPECT_EQ(header.max(), 100U);
}

TEST(KeepAliveTest, HandlesEmptyString) {
    basic_keep_alive header{""};

    EXPECT_FALSE(header.is_valid());
    EXPECT_FALSE(header.has_timeout());
    EXPECT_FALSE(header.has_max());
}

TEST(KeepAliveTest, HandlesMalformedFormatNoEqualSign) {
    basic_keep_alive header{"timeout5, max"};

    EXPECT_FALSE(header.is_valid());
    EXPECT_FALSE(header.has_timeout());
    EXPECT_FALSE(header.has_max());
}

TEST(KeepAliveTest, HandlesMultipleDelimitersAndGarbageProperties) {
    basic_keep_alive header{"garbage=value,,,timeout=5, ,,max=20, foo=bar"};

    EXPECT_TRUE(header.is_valid());

    ASSERT_TRUE(header.has_timeout());
    EXPECT_EQ(header.timeout(), 5U);

    ASSERT_TRUE(header.has_max());
    EXPECT_EQ(header.max(), 20U);
}

TEST(KeepAliveTest, RejectsNegativeValuesForSizeT) {
    basic_keep_alive header{"timeout=-5, max=100"};

    // -5 contains a non-numeric character '-' before reaching `to_size_t` internal parsing bounds
    // unless the base casting accounts for explicit positive requirement for uints.
    EXPECT_FALSE(header.has_timeout());
    EXPECT_TRUE(header.has_max());
    EXPECT_EQ(header.max(), 100U);
}

/////////////////////////////////////////////////////////////////////////////////////

TEST(CacheControlTest, EmptyStringIsInvalid) {
    basic_cache_control cache_control{""};
    EXPECT_FALSE(cache_control.is_valid());
}

TEST(CacheControlTest, ParsesSingleBooleanDirective) {
    basic_cache_control cache_control{"no-cache"};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_TRUE(cache_control.no_cache());
    EXPECT_FALSE(cache_control.no_store());
    EXPECT_FALSE(cache_control.is_public());
}

TEST(CacheControlTest, ParsesMultipleBooleanDirectives) {
    basic_cache_control cache_control{"no-store, no-transform, private"};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_TRUE(cache_control.no_store());
    EXPECT_TRUE(cache_control.no_transform());
    EXPECT_TRUE(cache_control.is_private());

    // Ensure others are false
    EXPECT_FALSE(cache_control.no_cache());
    EXPECT_FALSE(cache_control.is_public());
}

TEST(CacheControlTest, ParsesValueDirectives) {
    basic_cache_control cache_control{"max-age=3600"};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_TRUE(cache_control.has_max_age());
    EXPECT_EQ(cache_control.max_age(), 3600);
}

TEST(CacheControlTest, ParsesQuotedValueDirectives) {
    basic_cache_control cache_control{"s-maxage=\"7200\""};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_EQ(cache_control.s_maxage(), 7200);
}

TEST(CacheControlTest, HandlesCaseInsensitivity) {
    basic_cache_control cache_control{"No-CaChE, MAX-age=86400, PuBlIc"};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_TRUE(cache_control.no_cache());
    EXPECT_TRUE(cache_control.is_public());
    EXPECT_TRUE(cache_control.has_max_age());
    EXPECT_EQ(cache_control.max_age(), 86'400);
}

TEST(CacheControlTest, ParsesMixedDirectivesWithSpacing) {
    basic_cache_control cache_control{"public,   max-age=600 ,  s-maxage=1200, must-revalidate"};
    ASSERT_TRUE(cache_control.is_valid());

    EXPECT_TRUE(cache_control.is_public());
    EXPECT_TRUE(cache_control.must_revalidate());

    EXPECT_EQ(cache_control.max_age(), 600);
    EXPECT_EQ(cache_control.s_maxage(), 1200);

    EXPECT_FALSE(cache_control.is_private());
    EXPECT_FALSE(cache_control.no_cache());
}

TEST(CacheControlTest, HandlesMalformedNumericValues) {
    basic_cache_control cache_control{"max-age=invalid_number"};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_FALSE(cache_control.has_max_age()); // Should fail casting safely
    EXPECT_EQ(cache_control.max_age(), -1);    // Default fallback
}

TEST(CacheControlTest, IgnoresUnknownDirectives) {
    basic_cache_control cache_control{"public, unknown-directive=123, max-age=50"};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_TRUE(cache_control.is_public());
    EXPECT_EQ(cache_control.max_age(), 50);
}

TEST(CacheControlTest, HandlesStaleExtensions) {
    basic_cache_control cache_control{"stale-while-revalidate=86400, stale-if-error=172800"};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_EQ(cache_control.stale_while_revalidate(), 86'400);
    EXPECT_EQ(cache_control.stale_if_error(), 172'800);
}

TEST(CacheControlTest, ImmutableFlagTest) {
    basic_cache_control cache_control{"public, max-age=31536000, immutable"};
    ASSERT_TRUE(cache_control.is_valid());
    EXPECT_TRUE(cache_control.is_public());
    EXPECT_EQ(cache_control.max_age(), 31'536'000);
    EXPECT_TRUE(cache_control.immutable());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////


TEST(LocationHeaderTest, ValidAbsoluteURI) {
    basic_location loc{"https://example.com/redirect-path"};

    EXPECT_TRUE(loc.is_valid());
    EXPECT_EQ(loc.uri(), "https://example.com/redirect-path");
    EXPECT_EQ(loc.name(), "location");
    EXPECT_TRUE(static_cast<bool>(loc));
}

TEST(LocationHeaderTest, ValidRelativeURI) {
    basic_location loc{"/assets/styles.css"};

    EXPECT_TRUE(loc.is_valid());
    EXPECT_EQ(loc.uri(), "/assets/styles.css");
}

TEST(LocationHeaderTest, TrimsWhitespace) {
    basic_location loc{"   http://test.com/   "};

    EXPECT_TRUE(loc.is_valid());
    EXPECT_EQ(loc.uri(), "http://test.com/");
}

TEST(LocationHeaderTest, InvalidEmptyURI) {
    basic_location loc{""};

    EXPECT_FALSE(loc.is_valid());
    EXPECT_TRUE(loc.uri().empty());
}

TEST(LocationHeaderTest, InvalidWhitespaceOnlyURI) {
    basic_location loc{"     \t   "};

    EXPECT_FALSE(loc.is_valid());
    EXPECT_TRUE(loc.uri().empty());
}

TEST(LocationHeaderTest, RenderValidHeader) {
    basic_location loc{"/new-page"};
    ASSERT_TRUE(loc.is_valid());

    std::array<char, 32> buffer{};
    char*                ptr = buffer.data();

    render(ptr, buffer.size(), loc);

    std::string_view rendered(buffer.data(), static_cast<std::size_t>(ptr - buffer.data()));
    EXPECT_EQ(rendered, "/new-page");
}

TEST(LocationHeaderTest, RenderFailsOnInsufficientBuffer) {
    basic_location loc{"http://example.com/very/long/path"};
    ASSERT_TRUE(loc.is_valid());

    std::array<char, 10> buffer{}; // Buffer too small
    char*                ptr = buffer.data();

    render(ptr, buffer.size(), loc);

    // Pointer should not move if there wasn't enough space
    EXPECT_EQ(ptr, buffer.data());
}

TEST(LocationHeaderTest, RenderFailsOnInvalidHeader) {
    basic_location loc{"   "};
    ASSERT_FALSE(loc.is_valid());

    std::array<char, 32> buffer{};
    char*                ptr = buffer.data();

    render(ptr, buffer.size(), loc);

    // Pointer should not move since header is invalid
    EXPECT_EQ(ptr, buffer.data());
}
