#include <gtest/gtest.h>
#include <webpp/utils/ipv4.h>

using namespace webpp;

TEST(IpTests, IPv4Creation) {
    constexpr ipv4 one({192, 168, 1, 1});
    constexpr ipv4 two("192.168.1.1");
    EXPECT_TRUE(one.integer() == two.integer());
}
