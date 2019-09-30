#include <gtest/gtest.h>
#include <string_view>
#include <webpp/utils/ipv4.h>

using namespace webpp;

TEST(IPv4Tests, Creation) {
    constexpr ipv4 one{192, 168, 1, 1};
    ipv4 two("192.168.1.1");
    EXPECT_EQ(one, two) << "one is: " << one.str() << "; two is: " << two.str();
    EXPECT_EQ(one.integer(), two.integer());
    EXPECT_EQ(one.str(), two.str());
    EXPECT_FALSE(one.is_all_zero());
}

TEST(IPv4Tests, Methods) {
    ipv4 ip{192, 168, 1, 1};
    EXPECT_TRUE(ip.in_subnet({192, 168, 0, 0}, 16));
    EXPECT_TRUE(ip.in_subnet({192, 168, 1, 0}, 24));
    EXPECT_TRUE(ip.in_range({192, 168, 0, 1}, ipv4("192.168.2.1")));
    EXPECT_FALSE(ip.in_range({192, 168, 1, 2}, ipv4("192.168.2.1")));
    EXPECT_TRUE(ip.is_private());
    EXPECT_FALSE(ip.is_public());
    EXPECT_FALSE(ip.is_all_zero());
    EXPECT_EQ(ip.str(), "192.168.1.1");

    auto octets = ip.octets();
    EXPECT_EQ(octets[0], 192);
    EXPECT_EQ(octets[1], 168);
    EXPECT_EQ(octets[2], 1);
    EXPECT_EQ(octets[3], 1);
}
