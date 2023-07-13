#include "../webpp/ip/ip_address.hpp"

#include "common/tests_common_pch.hpp"

using namespace webpp;


TEST(IPAddressTests, Init) {
    ip_address addr{127, 0, 0, 1};
    EXPECT_TRUE(addr.is_valid()) << addr.string();
    EXPECT_TRUE(addr.is_loopback());
    EXPECT_TRUE(addr.is_v4());
    EXPECT_EQ(addr.as_v4(), (ipv4{127, 0, 0, 1}));
}



TEST(IPAddressTests, ParseString) {
    {
        ip_address addr{"127.0.0.1"};
        EXPECT_TRUE(addr.is_valid());
        EXPECT_TRUE(addr.is_loopback());
        EXPECT_TRUE(addr.is_v4());
        EXPECT_EQ(addr.as_v4(), (ipv4{127, 0, 0, 1}));
    }
    {
        ip_address addr{"::ffff:127.0.0.1"};
        EXPECT_TRUE(addr.is_valid());
        EXPECT_TRUE(addr.is_loopback()) << addr.status_string();
        EXPECT_FALSE(addr.is_v4());
        // todo: throws exception correctly, but we like exception-free classes, right?
        // EXPECT_EQ(addr.as_v4(), (ipv4{127, 0, 0, 1}));
        EXPECT_EQ(addr.as_v6(), (ipv6{"::ffff:127.0.0.1"}));
    }
}


TEST(IPAddressTests, Equality) {
    {
        ip_address addr{"127.0.0.1"};
        EXPECT_EQ(addr, (ipv4{127, 0, 0, 1}));
    }
    {
        ip_address addr{"::ffff:127.0.0.1"};
        EXPECT_EQ(addr, (ipv6{"::ffff:127.0.0.1"}));
    }
    {
        ip_address addr{"::ffff:127.0.0.1"};
        ip_address addr2{"::ffff:127.0.0.1"};
        EXPECT_EQ(addr, addr2);
    }
    {
        ip_address addr{"::ffff:127.0.0.1"};
        EXPECT_EQ(addr, "::ffff:127.0.0.1");
        EXPECT_NE(addr, "127.0.0.1");
        EXPECT_TRUE(addr != "127.0.0.1");
        EXPECT_TRUE(addr < "::ffff:128.0.0.1");
    }
    {
        ip_address addr{"::0fff:1"};
        ip_address addr2{"::ffff:127.0.0.1"};
        EXPECT_LE(addr, addr2);
        EXPECT_TRUE(addr < addr2);
    }
}


TEST(IPAddressTests, Methods) {
    ip_address addr{"::0fff:1/127"};
    EXPECT_TRUE(addr.is_valid());
    EXPECT_EQ(addr.prefix(), 127u);
    EXPECT_EQ(addr.status(), ip_address_status::valid);
}
