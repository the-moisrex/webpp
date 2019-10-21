#include <gtest/gtest.h>
#include <webpp/utils/ipv6.h>
#include <webpp/validators/validators.h>

using namespace webpp;

TEST(IPv6Tests, Creation) {}

TEST(IPv6Tests, StrTests) {
    EXPECT_EQ(ipv6("::").short_str(), "::");
    EXPECT_EQ(ipv6("::1").short_str(), "::1");
    EXPECT_EQ(ipv6("::f0:1").short_str(), "::f0:1");
}

TEST(IPv6Tests, Validation) {
    auto valid_ipv6s = {"0000:0000:0000:0000:0000:0000:0000:0000", "fe00::1",
                        "fe80::217:f2ff:fe07:ed62",
                        "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                        "2001:0db8:0000:85a3:0000:0000:ac1f:8001"};

    auto invalid_ipv6s = {"02001:0000:1234:0000:0000:C1C0:ABCD:0876",
                          "2001:0000:1234:0000:00001:C1C0:ABCD:0876",
                          "2001:0000:1234: 0000:0000:C1C0:ABCD:0876",
                          "2001:0000:1234:0000:0000:C1C0:ABCD:0876 0",
                          "3ffe:0b00:0000:0001:0000:0000:000a",
                          "FF02:0000:0000:0000:0000:0000:0000:0000:0001",
                          "::1111:2222:3333:4444:5555:6666::",
                          "3ffe:b00::1::a"};

    for (auto const& _ip : valid_ipv6s) {
        EXPECT_EQ(ipv6(ipv6(_ip).short_str()), ipv6(_ip))
            << "ip: " << _ip << "\ncompiled ip: " << ipv6(_ip).short_str()
            << "\nlong ip: " << ipv6(_ip).str();
        EXPECT_TRUE(webpp::is::ipv6(_ip))
            << "ip: " << _ip << "; compiled ip: " << ipv6(_ip).short_str();
        EXPECT_TRUE(ipv6(_ip).is_valid())
            << "ip: " << _ip << "; compiled ip: " << ipv6(_ip).short_str();
    }

    for (auto const& _ip : invalid_ipv6s) {
        EXPECT_FALSE(webpp::is::ipv6(_ip))
            << "ip: " << _ip << "; compiled ip: " << ipv6(_ip).short_str();
        EXPECT_FALSE(ipv6(_ip).is_valid())
            << "ip: " << _ip << "; compiled ip: " << ipv6(_ip).short_str();
    }
}

TEST(IPv6Tests, CIDR) {
    auto valid_ipv6s = {"0000:0000:0000:0000:0000:0000:0000:0000/01",
                        "0000:0000:0000:0000:0000:0000:0000:0000/19",
                        "0000:0000:0000:0000:0000:0000:0000:0000/99",
                        "0000:0000:0000:0000:0000:0000:0000:0000/100",
                        "0000:0000:0000:0000:0000:0000:0000:0000/119",
                        "0000:0000:0000:0000:0000:0000:0000:0000/128",
                        "fe00::1/64",
                        "fe80::217:f2ff:fe07:ed62/60",
                        "::1/128",
                        "2001:db8::/48"};

    auto invalid_ipv6s = {"0000:0000:0000:0000:0000:0000:0000:0000/",
                          "0000:0000:0000:0000:0000:0000:0000:0000/012",
                          "0000:0000:0000:0000:0000:0000:0000:0000/129",
                          "0000:0000:0000:0000:0000:0000:0000:0000/130"};

    for (auto const& _ip : valid_ipv6s) {
        EXPECT_TRUE(webpp::is::ipv6(_ip)) << _ip;
        EXPECT_TRUE(ipv6(_ip).is_valid()) << _ip;
        EXPECT_TRUE(ipv6(_ip).has_prefix()) << _ip;
        EXPECT_GE(ipv6(_ip).prefix(), 0) << _ip;
        EXPECT_LE(ipv6(_ip).prefix(), 128) << _ip;
    }

    for (auto const& _ip : invalid_ipv6s) {
        EXPECT_FALSE(webpp::is::ipv6(_ip)) << _ip;
        EXPECT_FALSE(ipv6(_ip).is_valid()) << _ip;
        EXPECT_FALSE(ipv6(_ip).has_prefix()) << _ip;
        // TODO: check cidr(prefix) method
    }
}