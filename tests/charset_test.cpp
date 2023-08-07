
#include "../webpp/strings/charset.hpp"

#include "common/tests_common_pch.hpp"



using namespace webpp;


inline constexpr auto LOWER_ALPHA_MAP = charmap_range<'a', 'z'>();
inline constexpr auto UPPER_ALPHA_MAP = charmap_range<'A', 'Z'>();
inline constexpr auto ALPHA_MAP       = charmap(LOWER_ALPHA_MAP, UPPER_ALPHA_MAP);
inline constexpr auto DIGIT_MAP       = charmap_range<'0', '9'>();
inline constexpr auto HEXDIG_MAP = charmap(DIGIT_MAP, charmap_range<'A', 'F'>(), charmap_range<'a', 'f'>());
inline constexpr auto ALPHA_DIGIT_MAP = charmap{ALPHA_MAP, DIGIT_MAP};


inline constexpr charset<char, 2> CRLF{0x0D, 0x0A}; // CR(\r), LF(\n)
inline constexpr charset<char, 2> OWS{0x20, 0x09};  // SP, HTAB



TEST(CharsetTest, CharsetCreation) {
    constexpr auto chars = charset_range<char, 'a', 'z'>();
    EXPECT_TRUE(chars.contains('a')) << "contains 'a'";
    EXPECT_TRUE(chars.contains('b')) << "contains 'b'";
    EXPECT_TRUE(chars.contains('z')) << "contains 'z'";
    EXPECT_FALSE(chars.contains('A')) << "shouldn't contain 'A'";
    EXPECT_FALSE(chars.contains('Z')) << "shouldn't contain 'Z'";
    EXPECT_FALSE(chars.contains('&')) << "shouldn't contain '&'";
    EXPECT_FALSE(chars.contains('\0')) << "shouldn't contain '\\0'";

    static constexpr charset chars2{'1', '2', '3', '4', '5'};
    EXPECT_EQ(5, chars2.size());
}


TEST(CharsetTest, CharMapCreation) {
    constexpr auto chars = charmap_range<'a', 'z'>();
    EXPECT_TRUE(chars.contains('a')) << "contains 'a'";
    EXPECT_TRUE(chars.contains('b')) << "contains 'b'";
    EXPECT_TRUE(chars.contains('z')) << "contains 'z'";
    EXPECT_FALSE(chars.contains('A')) << "shouldn't contain 'A'";
    EXPECT_FALSE(chars.contains('Z')) << "shouldn't contain 'Z'";
    EXPECT_FALSE(chars.contains('&')) << "shouldn't contain '&'";
    EXPECT_FALSE(chars.contains('\0')) << "shouldn't contain '\\0'";

    static constexpr charmap_half chars2{'1', '2', '3', '4', '5'};
    EXPECT_FALSE(chars2.contains('\0')) << "shouldn't contain '\\0'";
    EXPECT_TRUE(chars2.contains('5')) << "contains '5'";
    EXPECT_TRUE(chars2.contains('1')) << "contains '1'";

    static constexpr charmap_half chars3{ALPHA<char>, DIGIT<char>};
    EXPECT_FALSE(chars3.contains('\0')) << "shouldn't contain '\\0'";
}

/**
 * This is the "Token" specified in the HTTP Semantics RFC (https://httpwg.org/specs/rfc9110.html#tokens)
 *   token          = 1*tchar
 *   tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
 *                        / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
 *                        / DIGIT / ALPHA
 *                        ; any VCHAR, except delimiters
 **/
static constexpr bool token_char_map[256]{
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, true,  false, true,  true,  true,  true,  true,  false, false, true,  true,  false,
  true,  true,  false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  false, false,
  false, false, false, false, false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
  true,  false, false, false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
  true,  true,  true,  false, true,  false, true,  false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
  false};

static constexpr charmap_full token_charmap{
  ALPHA<>,
  DIGIT<>,
  charset{'!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~'}};

TEST(CharsetTest, CharMapExpectation) {
    EXPECT_EQ(token_charmap.size(), 256);
    for (stl::size_t index = 0; index != token_charmap.size(); ++index) {
        EXPECT_EQ(token_charmap[index], token_char_map[index]) << index;
    }
}
