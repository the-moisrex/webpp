#include <gtest/gtest.h>
#include <webpp/utils/charset.h>

using namespace webpp;

TEST(CharsetTest, CharsetCreation) {
    constexpr charset chars('a', 'z');
    EXPECT_TRUE(chars.contains('a'));
    EXPECT_TRUE(chars.contains('b'));
    EXPECT_TRUE(chars.contains('z'));
    EXPECT_FALSE(chars.contains('A'));
    EXPECT_FALSE(chars.contains('Z'));
    EXPECT_FALSE(chars.contains('&'));
    EXPECT_FALSE(chars.contains('\0'));
}
