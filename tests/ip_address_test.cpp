#include "../webpp/ip/address.hpp"
#include "common_pch.hpp"

using namespace webpp;


TEST(IPAddressTests, Init) {
    address addr{127, 0, 0, 1};
    EXPECT_TRUE(addr.is_valid());
    EXPECT_TRUE(addr.is_loopback());
    EXPECT_TRUE(addr.is_v4());
    EXPECT_EQ(addr.as_v4(), (ipv4{127, 0, 0, 1}));
}
