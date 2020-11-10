#include "../core/include/webpp/convert/lexical_cast.hpp"

#include <gtest/gtest.h>

using namespace webpp;

TEST(Convert, LexicalCast) {
    EXPECT_EQ(20, lexical::cast<int>("20"));
    EXPECT_EQ(-43, lexical::cast<int>("-43"));
    EXPECT_EQ(43, lexical::cast<unsigned>(-43));
    EXPECT_EQ(43, lexical::cast<unsigned>("-43"));
    EXPECT_EQ("-43", lexical::cast<std::string_view>("-43"));
    EXPECT_EQ("-43", lexical::cast<std::string>("-43"));
    EXPECT_EQ("-43", lexical::cast<std::string>("-43"));
    EXPECT_EQ("-43", lexical::cast<std::string>(-43));

    // todo: test for floating points as well
}
