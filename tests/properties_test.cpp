
#include "./common_pch.hpp"

// clang-format off
#include webpp_include(utils/property)
// clang-format on

using namespace webpp;
using namespace std;

TEST(PropertiesTest, SimpleUsage) {
    EXPECT_EQ(property(10), 10);
    EXPECT_TRUE(property("hello"s) == "hello"s);
}
