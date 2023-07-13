
#include "../webpp/utils/property.hpp"
#include "common/tests_common_pch.hpp"


using namespace webpp;
using namespace std;

TEST(PropertiesTest, SimpleUsage) {
    EXPECT_EQ(property(10), 10);
    EXPECT_TRUE(property("hello"s) == "hello"s);
}
