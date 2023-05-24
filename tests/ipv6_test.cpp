#include "../webpp/ip/ipv6.hpp"

#include "../webpp/ip/inet_pton.hpp"
#include "../webpp/ip/ip_validators.hpp"
#include "../webpp/traits/std_traits.hpp"
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
    EXPECT_EQ("2001:0db8:0000:85a3:0000:0000:ac1f:8001", ip4.expanded_string());
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
    auto valid_ipv6s = {"0000:0000:0000:0000:0000:0000:0000:0000/1",
                        "0000:0000:0000:0000:0000:0000:0000:0000/0",
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
                          "0000:0000:0000:0000:0000:0000:0000:0000/01",
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

TEST(IPv6Tests, SpecialCases) {
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
      "::01",
      "::"};

    static constexpr stl::string_view invalid_ipv6s[]{"2001:0gb8:85a3:0000:0000:8a2e:0370:7334",
                                                      "2001:db8:1234:5678",
                                                      "2001:db8:1",
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


// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
static constexpr stl::string_view some_valid_ipv6s[]{
  "2001:db8:3333:4444:5555:6666:7777:8888",
  "2001:db8:3333:4444:cccc:dddd:eeee:ffff",
  "2001:db8::",          // implies that the last six segments are zero
  "::1234:5678",         // implies that the first six segments are zero
  "2001:db8::1234:5678", // implies that the middle four segments are zero
  "2001:0db8:0001::0ab9:c0a8:0102",
  "2001:db8:1::ab9:c0a8:102",
  "2001:0db8:85a3::8a2e:0370:7334",
  "2001:db8:1234::5678",
  "2001:db8::1",
  "2001::1",
  "::1234:5678:91.123.4.56", // implies that the first four ipv6 segments are zero
  "2001:db8:3333:4444:5555:6666:1.2.3.4",
  "::11.22.33.44",               // implies all six ipv6 segments are zero
  "2001:db8::123.123.123.123",   // implies that the last four ipv6 segments are zero
  "::1234:5678:1.2.3.4",         // implies that the first four ipv6 segments are zero
  "2001:db8::1234:5678:5.6.7.8", // implies that the middle two ipv6 segments are zero
  "::1",
  "::ffff:192.0.2.128", // IPv4-mapped IPv6 address
  "::FFFF:129.144.52.38",
  "::FAFF:129.144.52.38", // not a IPv4-Compatible IPv6 Address, but looks like one
  "1::129.144.52.38",     // not a IPv4-Compatible IPv6 Address, but looks like one
  "::",
  "::f0:1",

  // AI Generated:
  "2001:db8:0:0:0:0:0:1",                   // loopback address
  "2001:db8:ffff:ffff:ffff:ffff:ffff:ffff", // largest address
  "::",                                     // unspecified address
  "::1",                                    // loopback address shorthand
  "2001:db8::",                             // compressed address
  "2001:db8:0:1:0:0:0:0",                   // sequential address
  "2001:db8:0:0:0:ff:0:0",                  // multicast address
  "fe80::1",                                // link-local address
  "ff01::1",                                // multicast address
  "ff02::1",                                // multicast address
  "ff02::2",                                // multicast address
  "ff02::3",                                // multicast address
  "ff02::4",                                // multicast address
  "ff02::5",                                // multicast address
  "ff02::6",                                // multicast address
  "ff02::7",                                // multicast address
  "ff02::8",                                // multicast address
  "ff02::9",                                // multicast address
  "ff02::a",                                // multicast address
  "ff02::b",                                // multicast address
  "ff02::c",                                // multicast address
  "ff02::d",                                // multicast address
  "ff02::e",                                // multicast address
  "ff02::f",                                // multicast address
  "fe80::217:f2ff:fe07:ed62",               // link-local address
  "fe80::217:f2ff:fe07:ed63",               // link-local address
  "fe80::217:f2ff:fe07:ed64",               // link-local address
  "fe80::217:f2ff:fe07:ed65",               // link-local address
  "fe80::217:f2ff:fe07:ed66",               // link-local address
  "fe80::217:f2ff:fe07:ed67",               // link-local address
  "fe80::217:f2ff:fe07:ed68",               // link-local address
  "fe80::217:f2ff:fe07:ed69",               // link-local address
  "fe80::217:f2ff:fe07:ed6a",               // link-local address
  "fe80::217:f2ff:fe07:ed6b",               // link-local address
  "fe80::217:f2ff:fe07:ed6c",               // link-local address
  "fe80::217:f2ff:fe07:ed6d",               // link-local address
  "fe80::217:f2ff:fe07:ed6e",               // link-local address
  "fe80::217:f2ff:fe07:ed6f",               // link-local address
  "fe80::217:f2ff:fe07:ed70",               // link-local address
  "fe80::217:f2ff:fe07:ed71",               // link-local address
  "fe80::217:f2ff:fe07:ed72",               // link-local address
  "fe80::217:f2ff:fe07:ed73",               // link-local address
  "fe80::217:f2ff:fe07:ed74",               // link-local address
  "fe80::217:f2ff:fe07:ed75",               // link-local address
  "fe80::217:f2ff:fe07:ed76",               // link-local address
  "fe80::217:f2ff:fe07:ed77",               // link-local address
  "fe80::217:f2ff:fe07:ed78",               // link-local address
  "fe80::217:f2ff:fe07:ed79",               // link-local address
  "fe80::217:f2ff:fe07:ed7a",               // link-local address
  "fe80::217:f2ff:fe07:ed7b",               // link-local address
  "fe80::217:f2ff:fe07:ed7c",               // link-local address
  "fe80::217:f2ff:fe07:ed7d",               // link-local address
  "fe80::217:f2ff:fe07:ed7e",               // link-local address
  "fe80::217:f2ff:fe07:ed7f",               // link-local address
  "fe80::217:f2ff:fe07:ed80",               // link-local address
  "fe80::217:f2ff:fe07:ed81",               // link-local address
  "fe80::217:f2ff:fe07:ed82",               // link-local address
  "fe80::217:f2ff:fe07:ed83",               // link-local address
  "fe80::217:f2ff:fe07:ed84",               // link-local address
  "fe80::217:f2ff:fe07:ed85",               // link-local address
  "fe80::217:f2ff:fe07:ed86",               // link-local address
  "fe80::217:f2ff:fe07:ed87",               // link-local address
  "fe80::217:f2ff:fe07:ed88",               // link-local address
  "fe80::217:f2ff:fe07:ed89",               // link-local address
  "fe80::217:f2ff:fe07:ed8a",               // link-local address
  "fe80::217:f2ff:fe07:ed8b",               // link-local address
  "fe80::217:f2ff:fe07:ed8c",               // link-local address
  "fe80::217:f2ff:fe07:ed8d",               // link-local address
  "fe80::217:f2ff:fe07:ed8e",               // link-local address
  "fe80::217:f2ff:fe07:ed8f",               // link-local address
  "fe80::217:f2ff:fe07:ed90",               // link-local address
  "fe80::217:f2ff:fe07:ed91",               // link-local address
  "fe80::217:f2ff:fe07:ed92",               // link-local address
  "fe80::217:f2ff:fe07:ed93",               // link-local address
  "fe80::217:f2ff:fe07:ed94",               // link-local address
  "fe80::217:f2ff:fe07:ed95",               // link-local address
  "fe80::217:f2ff:fe07:ed96",               // link-local address
  "fe80::217:f2ff:fe07:ed97",               // link-local address
  "fe80::217:f2ff:fe07:ed98",               // link-local address
  "fe80::217:f2ff:fe07:ed99",               // link-local address
  "fe80::217:f2ff:fe07:ed9a",               // link-local address
  "fe80::217:f2ff:fe07:ed9b",               // link-local address
  "fe80::217:f2ff:fe07:ed9c",               // link-local address
  "fe80::217:f2ff:fe07:ed9d",               // link-local address
  "fe80::217:f2ff:fe07:ed9e",               // link-local address
  "fe80::217:f2ff:fe07:ed9f",               // link-local address
};
#if __has_include(<arpa/inet.h>)
#    include <arpa/inet.h>
TEST(IPv6Tests, ShortStrEquality) {

    stl::array<uint8_t, INET6_ADDRSTRLEN> ip_bin{};
    stl::array<char, 64>                  ip_str{};
    for (auto const& _ip : some_valid_ipv6s) {
        auto const ip6 = ipv6_t{_ip};
        ::inet_pton(AF_INET6, _ip.data(), ip_bin.data());
        ::inet_ntop(AF_INET6, ip_bin.data(), ip_str.data(), ip_str.size());
        EXPECT_STREQ(ip_str.data(), ip6.ip_string().c_str()) << _ip;
    }
}
#endif


TEST(IPv6Tests, PrefixesTest) {
    stl::array<int, 7> const              valid_prefixes{0, 1, 2, 10, 99, 100, 128};
    stl::array<stl::string_view, 7> const invalid_prefixes{"00", "01", "2a", "129", "-1", "001", "259"};

    for (auto _ip : some_valid_ipv6s) {
        for (auto prefix : valid_prefixes) {
            stl::string ip_prefixed{_ip};
            ip_prefixed += '/';
            ip_prefixed += stl::to_string(prefix);
            ipv6_t const ip6{ip_prefixed};
            EXPECT_TRUE(ip6.is_valid()) << _ip << "\n" << ip_prefixed;
            EXPECT_EQ(ip6.prefix(), prefix) << _ip << "\n" << ip_prefixed << "\n" << ip6.status_string();
        }

        for (auto prefix : invalid_prefixes) {
            stl::string ip_prefixed{_ip};
            ip_prefixed += '/';
            ip_prefixed += prefix;
            ipv6_t const ip6{ip_prefixed};
            EXPECT_FALSE(ip6.is_valid()) << _ip << "\n"
                                         << ip_prefixed << "\n"
                                         << static_cast<int>(ip6.prefix());
        }
    }
}



TEST(IPv6Tests, ErrorMessages) {
    EXPECT_EQ(to_string(inet_pton6_status::invalid_character), (ipv6{"::x"}.status_string()));
    EXPECT_EQ(to_string(inet_pton6_status::valid), (ipv6{"::1"}.status_string()));
    EXPECT_EQ(to_string(inet_pton6_status::invalid_prefix), (ipv6{"::1", 129}.status_string()));
}

TEST(IPv6Tests, LoopbackCheck) {
    ipv6_t ip6{"::ffff:127.0.0.1"};
    EXPECT_TRUE(ip6.is_loopback()) << ip6.string() << "\n" << ip6.expanded_string();
}
