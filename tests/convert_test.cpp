#include "../core/include/webpp/convert/lexical_cast.hpp"

#include "./common_pch.hpp"

using namespace webpp;

TEST(Convert, LexicalCast) {
    EXPECT_EQ(20, lexical::cast<int>("20"));
    EXPECT_EQ(-43, lexical::cast<int>("-43"));
    // todo: make these two or at least the second one work
    EXPECT_EQ(static_cast<unsigned>(-43), lexical::cast<unsigned>(-43)) << "static_casting -43 to unsigned is not 43";
    EXPECT_EQ(static_cast<unsigned>(-43), lexical::cast<unsigned>("-43")) << "-43 cannot be converted to unsigned int that easily";
    EXPECT_EQ("-43", lexical::cast<std::string_view>("-43"));
    EXPECT_EQ("-43", lexical::cast<std::string>("-43"));
    EXPECT_EQ("-43", lexical::cast<std::string>("-43"));
    EXPECT_EQ("-43", lexical::cast<std::string>(-43));

    // todo: test for floating points as well
}
