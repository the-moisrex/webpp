#include <gtest/gtest.h>
#include "../core/include/webpp/utils/charset.h"

using namespace webpp;

TEST(CharsetTest, CharsetCreation) {
    constexpr auto chars = charset<'a', 'z'>();
    EXPECT_TRUE(chars.contains('a')) << "contains 'a'";
    EXPECT_TRUE(chars.contains('b')) << "contains 'b'";
    EXPECT_TRUE(chars.contains('z')) << "contains 'z'";
    EXPECT_FALSE(chars.contains('A')) << "shouldn't contain 'A'";
    EXPECT_FALSE(chars.contains('Z')) << "shouldn't contain 'Z'";
    EXPECT_FALSE(chars.contains('&')) << "shouldn't contain '&'";
    EXPECT_FALSE(chars.contains('\0')) << "shouldn't contain '\\0'";

    EXPECT_EQ(5, charset('1', '2', '3', '4', '5').size());
}
