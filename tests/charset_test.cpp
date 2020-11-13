#include "../core/include/webpp/strings/charset.hpp"

#include "./common_pch.hpp"

using namespace webpp;

TEST(CharsetTest, CharsetCreation) {
    constexpr auto chars = charset_range<char, 'a', 'z'>();
    EXPECT_TRUE(chars.contains('a')) << "contains 'a'";
    EXPECT_TRUE(chars.contains('b')) << "contains 'b'";
    EXPECT_TRUE(chars.contains('z')) << "contains 'z'";
    EXPECT_FALSE(chars.contains('A')) << "shouldn't contain 'A'";
    EXPECT_FALSE(chars.contains('Z')) << "shouldn't contain 'Z'";
    EXPECT_FALSE(chars.contains('&')) << "shouldn't contain '&'";
    EXPECT_FALSE(chars.contains('\0')) << "shouldn't contain '\\0'";

    charset chars2{'1', '2', '3', '4', '5'};
    EXPECT_EQ(5, chars2.size());
}
