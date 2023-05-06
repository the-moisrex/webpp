
#include "../webpp/ip/ipv6.hpp"

#include "../webpp/ip/inet_pton.hpp"
#include "../webpp/traits/std_traits.hpp"
#include "../webpp/validators/validators.hpp"
#include "common_pch.hpp"



using namespace webpp;

using ipv6_t = ipv6;

TEST(IPv6Tests, Creation) {
    ipv6_t const ip1{"::"};
    EXPECT_TRUE(ip1.is_valid());
    EXPECT_TRUE(ip1.is_unspecified());

    ipv6_t const ip2{"::1"};
    EXPECT_FALSE(ip2.is_unspecified());
    EXPECT_TRUE(ip2.is_valid());
    EXPECT_FALSE(ip2.has_prefix());

    ipv6_t const ip3("fe00::1");
    EXPECT_TRUE(ip3.is_valid());
    EXPECT_FALSE(ip3.is_unspecified());

    ipv6_t const ip4("2001:db8:0:85a3::ac1f:8001");
    EXPECT_TRUE(ip4.is_valid());
    EXPECT_EQ("2001:db8:0:85a3::ac1f:8001", ip4.string());
}

TEST(IPv6Tests, Validation) {
    auto valid_ipv6s = {"0000:0000:0000:0000:0000:0000:0000:0000",
                        "fe00::1",
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
                          "3ffe:b00::1::a",
                          "invalid",
                          "this is not valid",
                          "this:is:not:valid",
                          "",
                          ":",
                          "/",
                          "/1",
                          "/01",
                          "1/1",
                          ":::",
                          ":::/12",
                          "::1:1:2::"};

    for (auto const& _ip : valid_ipv6s) {
        EXPECT_EQ(ipv6_t(ipv6_t(_ip).string()), ipv6_t(_ip))
          << "ip: " << _ip << "\ncompiled ip: " << ipv6_t(_ip).string()
          << "\nlong ip: " << ipv6_t(_ip).string()
          << "\nshort long ip: " << ipv6_t(ipv6_t(_ip).string()).string();
        EXPECT_TRUE(static_cast<bool>(is::ipv6(_ip)))
          << "ip: " << _ip << "; compiled ip: " << ipv6_t(_ip).string();
        EXPECT_TRUE(ipv6_t(_ip).is_valid()) << "ip: " << _ip << "; compiled ip: " << ipv6_t(_ip).string();
    }

    for (auto const& _ip : invalid_ipv6s) {
        EXPECT_FALSE(is::ipv6(_ip)) << "ip: " << _ip << "; compiled ip: " << ipv6_t(_ip).string();
        EXPECT_FALSE(ipv6_t(_ip).is_valid()) << "ip: " << _ip << "; compiled ip: " << ipv6_t(_ip).string();
    }
}

TEST(IPv6Tests, CIDR) {
    auto valid_ipv6s = {"0000:0000:0000:0000:0000:0000:0000:0000/01",
                        "0000:0000:0000:0000:0000:0000:0000:0000/19",
                        "0000:0000:0000:0000:0000:0000:0000:0000/99",
                        "0000:0000:0000:0000:0000:0000:0000:0000/100",
                        "0000:0000:0000:0000:0000:0000:0000:0000/119",
                        "1234::/128",
                        "fe00::1/64",
                        "fe80::217:f2ff:fe07:ed62/60",
                        "::1/128",
                        "::/128",
                        "2001:db8::/48"};

    auto invalid_ipv6s = {"0000:0000:0000:0000:0000:0000:0000:0000/",
                          "0000:0000:0000:0000:0000:0000:0000:0000/129",
                          "0000:0000:0000:0000:0000:0000:0000:0000/130",
                          ""};

    for (auto const& _ip : valid_ipv6s) {
        EXPECT_FALSE(is::ipv6(_ip)) << _ip;
        EXPECT_TRUE(is::ipv6_prefix(_ip)) << _ip;
        EXPECT_TRUE(ipv6_t(_ip).is_valid()) << _ip << "\n" << ipv6_t{_ip}.string();
        EXPECT_TRUE(ipv6_t(_ip).has_prefix()) << _ip;
        EXPECT_GE(ipv6_t(_ip).prefix(), 0) << _ip;
        EXPECT_LE(ipv6_t(_ip).prefix(), 128) << _ip;
    }

    for (auto const& _ip : invalid_ipv6s) {
        EXPECT_FALSE(is::ipv6(_ip)) << _ip;
        EXPECT_FALSE(is::ipv6_prefix(_ip)) << _ip;
        EXPECT_FALSE(ipv6_t(_ip).is_valid()) << _ip;
        EXPECT_FALSE(ipv6_t(_ip).has_prefix()) << _ip;
        // TODO: check cidr(prefix) method
    }
}

TEST(IPV6Tests, SpecialCases) {
    EXPECT_EQ(ipv6_t("::0.0.0.1"), ipv6_t("::1"));
    EXPECT_EQ(ipv6_t("::0.0.0.1"), ipv6_t("::1"));
}

TEST(IPv6Tests, StrTests) {
    EXPECT_EQ(ipv6_t("::").string(), "::");
    EXPECT_EQ(ipv6_t("::1").string(), "::1");
    EXPECT_EQ(ipv6_t("::f0:1").string(), "::f0:1");
}


TEST(IPv6Tests, IP2NTest) {

    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
    static constexpr stl::string_view valid_ipv6s[]{
      "2001:db8:3333:4444:5555:6666:7777:8888",
      "2001:db8:3333:4444:CCCC:DDDD:EEEE:FFFF",
      "2001:db8::",          // implies that the last six segments are zero
      "::1234:5678",         // implies that the first six segments are zero
      "2001:db8::1234:5678", // implies that the middle four segments are zero
      "2001:0db8:0001:0000:0000:0ab9:C0A8:0102",
      "2001:db8:1::ab9:C0A8:102",
      "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
      "2001:db8:1234::5678",
      "2001:db8::1",
      "2001:0:0:0:0:0:0:1",
      "2001::1",
      "::1234:5678:91.123.4.56", // implies that the first four IPv6 segments are zero
      "2001:db8:3333:4444:5555:6666:1.2.3.4",
      "::11.22.33.44",               // implies all six IPv6 segments are zero
      "2001:db8::123.123.123.123",   // implies that the last four IPv6 segments are zero
      "::1234:5678:1.2.3.4",         // implies that the first four IPv6 segments are zero
      "2001:db8::1234:5678:5.6.7.8", // implies that the middle two IPv6 segments are zero
      "::1",
      "::"};

    static constexpr stl::string_view invalid_ipv6s[]{"2001:0gb8:85a3:0000:0000:8a2e:0370:7334",
                                                      "2001:db8:1234:5678",
                                                      "2001:db8:1",
                                                      "::01",
                                                      ":01",
                                                      ":::",
                                                      ":"};
    stl::uint8_t                      ip[16]{};

    for (auto const& _ip : valid_ipv6s) {
        EXPECT_TRUE(is::ipv6(_ip)) << _ip;
        EXPECT_TRUE(ipv6_t(_ip).is_valid()) << _ip << "\n" << ipv6_t{_ip}.string();
        EXPECT_EQ(inet_pton6(_ip.data(), _ip.data() + _ip.size(), ip), inet_pton6_status::valid)
          << "ip: " << _ip;
    }

    for (auto const& _ip : invalid_ipv6s) {
        EXPECT_FALSE(is::ipv6(_ip)) << _ip;
        EXPECT_FALSE(ipv6_t(_ip).is_valid()) << _ip << "\n" << ipv6_t{_ip}.string();
        EXPECT_NE(inet_pton6(_ip.data(), _ip.data() + _ip.size(), ip), inet_pton6_status::valid)
          << "ip: " << _ip;
    }
    // NOLINTEND(cppcoreguidelines-avoid-c-arrays)
}
