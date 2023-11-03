#include "../webpp/ip/ipv4.hpp"

#include "../webpp/ip/inet_ntop.hpp"
#include "../webpp/ip/inet_pton.hpp"
#include "../webpp/ip/ip_validators.hpp"
#include "common/tests_common_pch.hpp"



using namespace webpp;

using ipv4_t = ipv4;

TEST(IPv4Tests, ConstEvalCraetion) {
    EXPECT_FALSE(ipv4::invalid().is_valid());
    EXPECT_TRUE(ipv4::create("127.0.0.1").is_loopback());
}

TEST(IPv4Tests, FreeFunctions) {
    EXPECT_EQ(to_prefix("255.255.255.0"), 24);
    EXPECT_EQ(to_prefix("255.255.255.128"), 25);
    EXPECT_EQ(to_prefix("0.255.255.128"), 0);
    EXPECT_EQ(to_prefix({255, 255, 255, 128}), 25);
    EXPECT_EQ(to_prefix({0, 255, 255, 128}), 0);
    EXPECT_EQ(to_prefix(0xFF'FF'FF'00u), 24);
    EXPECT_EQ(to_prefix(0xFF'00'FF'00u), 8);
    EXPECT_EQ(to_subnet(24), 0xFF'FF'FF'00);
    EXPECT_EQ(to_subnet(8), 0xFF'00'00'00);

    auto a = ::std::array<uint8_t, 4>({255, 0, 0, 0});
    auto b = ::std::array<uint8_t, 4>({255, 255, 255, 128});
    EXPECT_EQ(to_subnet_array(8), a);
    EXPECT_EQ(to_subnet_array(25), b);
}

TEST(IPv4Tests, Creation) {
    constexpr ipv4_t one{192, 168, 2, 1};
    ipv4_t const     two("192.168.2.1");
    EXPECT_EQ(one, two) << "one is: " << one.string() << "; two is: " << two.string();
    EXPECT_EQ(one.integer(), two.integer());
    EXPECT_EQ(one.string(), two.string());
    EXPECT_FALSE(one.is_zero());
}

TEST(IPv4Tests, Methods) {
    ipv4_t const ip{192, 168, 1, 1};
    EXPECT_TRUE(ip.is_in_subnet(ipv4{{192, 168, 0, 0}, 16}));
    EXPECT_TRUE(ip.is_in_subnet(ipv4{{192, 168, 1, 0}, 24}));
    EXPECT_TRUE(ip.in_range(ipv4{192, 168, 0, 1}, ipv4_t("192.168.2.1")));
    EXPECT_FALSE(ip.in_range(ipv4{192, 168, 1, 2}, ipv4_t("192.168.2.1")));
    EXPECT_TRUE(ip.is_private());
    EXPECT_FALSE(ip.is_public());
    EXPECT_FALSE(ip.is_zero());
    EXPECT_EQ(ip.string(), "192.168.1.1");

    auto octets = ip.octets();
    EXPECT_EQ(octets[0], 192);
    EXPECT_EQ(octets[1], 168);
    EXPECT_EQ(octets[2], 1);
    EXPECT_EQ(octets[3], 1);
}

TEST(IPv4Tests, Validation) {
    auto valid_ipv4s = {"0.0.0.0", "192.168.1.1", "255.255.255.255"};

    auto invalid_ipv4s = {
      "10.168.0001.100",
      "0.0.0.256",
      "256.255.255.255",
      "256.0.0.0",
      "192.168. 224.0",
      "192.168.224.0 1",
    };

    for (auto const& _ip : valid_ipv4s) {
        EXPECT_TRUE(static_cast<bool>(is::ipv4(_ip)));
        EXPECT_TRUE(ipv4(_ip).is_valid()) << "ip: " << _ip << "; compiled ip: " << ipv4(_ip).string();
        EXPECT_TRUE(is::ipv4(_ip)) << "ip: " << _ip << "; compiled ip: " << ipv4(_ip).string();
        ipv4_t const ip{_ip};
        (void) ip.integer(); // just to make sure it's parsed
        EXPECT_TRUE(ip.is_valid());
    }

    for (auto const& _ip : invalid_ipv4s) {
        EXPECT_FALSE(is::ipv4(_ip));
        EXPECT_FALSE(ipv4_t(_ip).is_valid()) << "ip: " << _ip << "; compiled ip: " << ipv4_t(_ip).string();
        ipv4_t const ip{_ip};
        (void) ip.integer(); // just to make sure it's parsed
        EXPECT_FALSE(ip.is_valid());
    }
}

TEST(IPv4Tests, CIDR) {
    auto valid_ipv4s   = {"192.168.1.1/12", "192.168.1.1/32"};
    auto invalid_ipv4s = {"192.168.1.1/", "192.168.1.1/12.34", "192.168.1.1/01", "192.168.1.1/33"};

    for (auto const& _ip : valid_ipv4s) {
        EXPECT_TRUE(static_cast<bool>(is::ipv4_prefix(_ip))) << _ip;
        EXPECT_TRUE(ipv4_t(_ip).is_valid()) << "ip: " << _ip << "; compiled ip: " << ipv4_t(_ip).string();
        EXPECT_TRUE(is::ipv4_prefix(_ip)) << "ip: " << _ip << "; compiled ip: " << ipv4_t(_ip).string();
        EXPECT_TRUE(ipv4_t(_ip).has_prefix()) << _ip;
        EXPECT_GE(ipv4_t(_ip).prefix(), 0) << _ip;
        EXPECT_LE(ipv4_t(_ip).prefix(), 32) << _ip;
    }

    for (auto const& _ip : invalid_ipv4s) {
        EXPECT_FALSE(is::ipv6(_ip)) << _ip;
        EXPECT_FALSE(ipv4_t(_ip).is_valid()) << _ip;
        EXPECT_FALSE(is::ipv4_prefix(_ip)) << _ip;
        EXPECT_FALSE(ipv4_t(_ip).has_prefix()) << _ip;
        // TODO: check cidr(prefix) method
    }
}

TEST(IPv4Tests, ToString) {
    ipv4_t const ip{192, 168, 1, 1};
    stl::string  str{"ip is: "};
    ip.to_string(str);
    EXPECT_EQ(str, "ip is: 192.168.1.1");
    EXPECT_EQ(str.size(), ascii::size("ip is: 192.168.1.1"));
}



TEST(IPv4Tests, InetP2NValidation) {
    // NOLINTBEGIN(*-avoid-c-arrays)
    static constexpr stl::string_view valid_ipv4s[]{
      "0.0.0.0",         "192.168.1.1",     "255.255.255.255", "192.0.2.1",       "198.51.100.2",
      "203.0.113.3",     "10.0.0.4",        "172.16.0.5",      "192.168.0.6",     "127.0.0.7",
      "169.254.0.8",     "224.0.0.9",       "239.255.255.10",  "128.0.0.11",      "191.255.255.12",
      "223.255.255.13",  "240.0.0.14",      "255.255.255.15",  "1.2.3.4",         "5.6.7.8",
      "9.10.11.12",      "13.14.15.16",     "17.18.19.20",     "21.22.23.24",     "25.26.27.28",
      "29.30.31.32",     "33.34.35.36",     "37.38.39.40",     "41.42.43.44",     "45.46.47.48",
      "49.50.51.52",     "53.54.55.56",     "57.58.59.60",     "61.62.63.64",     "65.66.67.68",
      "69.70.71.72",     "73.74.75.76",     "77.78.79.80",     "81.82.83.84",     "85.86.87.88",
      "89.90.91.92",     "93.94.95.96",     "97.98.99.100",    "101.102.103.104", "105.106.107.108",
      "109.110.111.112", "113.114.115.116", "117.118.119.120", "121.122.123.124", "125.126.127.128",
      "129.130.131.132", "133.134.135.136", "137.138.139.140", "141.142.143.144", "145.146.147.148",
      "149.150.151.152", "153.154.155.156", "157.158.159.160", "161.162.163.164", "165.166.167.168",
      "169.170.171.172", "173.174.175.176", "177.178.179.180", "181.182.183.184", "185.186.187.188",
      "189.190.191.192", "193.194.195.196", "197.198.199.200", "201.202.203.204", "205.206.207.208",
      "209.210.211.212", "213.214.215.216", "217.218.219.220", "221.222.223.224", "225.226.227.228",
      "229.230.231.232", "233.234.235.236", "237.238.239.240", "241.242.243.244", "245.246.247.248",
      "249.250.251.252", "253.254.255.0",   "255.254.253.0",   "1.254.253.0",     "255.1.253.0",
      "255.254.1.0",     "255.254.253.1",   "1.1.253.0",       "255.1.1.0",       "255.254.1.1",
      "1.254.1.0",       "1.1.1.0",         "255.255.1.0",     "255.255.254.0",   "255.255.255.1",
      "255.255.254.1",   "255.254.255.1",   "254.255.255.1",   "255.1.255.0",     "1.255.254.0",
      "1.254.255.0",     "254.1.255.0",     "254.255.1.0"};

    static constexpr stl::string_view invalid_ipv4s[]{
      "10.168.0001.100",
      "0.0.0.256",
      "256.255.255.255",
      "256.0.0.0",
      "192.168. 224.0",
      "192.168.224.0 1",
    };
    stl::uint8_t ip[4]{};

    for (auto const& _ip : valid_ipv4s) {
        EXPECT_TRUE(is::ipv4(_ip)) << _ip;
        auto beg = _ip.begin();
        EXPECT_EQ(inet_pton4(beg, _ip.end(), ip), inet_pton4_status::valid)
          << "ip: " << _ip << "; compiled ip: " << static_cast<int>(ip[0]) << "." << static_cast<int>(ip[1])
          << "." << static_cast<int>(ip[2]) << "." << static_cast<int>(ip[3]);

        // testing inet_ntop4
        stl::array<char, sizeof "255.255.255.255"> new_ip{};
        inet_ntop4(ip, new_ip.data());
        EXPECT_EQ(stl::string_view{_ip}, stl::string_view{new_ip.data()}) << new_ip.data();
    }

    for (auto const& _ip : invalid_ipv4s) {
        EXPECT_FALSE(is::ipv4(_ip)) << _ip;
        auto beg = _ip.data();
        EXPECT_NE(inet_pton4(beg, beg + _ip.size(), ip), inet_pton4_status::valid)
          << "ip: " << _ip << "; compiled ip: " << static_cast<int>(ip[0]) << "." << static_cast<int>(ip[1])
          << "." << static_cast<int>(ip[2]) << "." << static_cast<int>(ip[3]);
    }
    // NOLINTEND(*-avoid-c-arrays)
}

TEST(IPv4Tests, ErrorMessages) {
    EXPECT_EQ(to_string(inet_pton4_status::invalid_octet), (ipv4{"1.1.a.1"}.status_string()));
    EXPECT_EQ(to_string(inet_pton4_status::valid), (ipv4{"1.1.1.1"}.status_string()));
    EXPECT_EQ(to_string(inet_pton4_status::invalid_prefix), (ipv4{"1.1.1.1", "-1.255.1.3"}.status_string()));
}


TEST(IPv6Tests, StartsWith) {
    EXPECT_TRUE(ipv4::create("127.0.0.1").starts_with<1>({127u}, 8));
    EXPECT_TRUE(ipv4::create("127.0.0.1").starts_with(ipv4{127, 0, 0, 0}, 8));
    EXPECT_EQ(ipv4::create("127.0.0.1").mask(8), ipv4(127u, 0u, 0u, 0u));
    EXPECT_EQ(ipv4::create("127.2.3.1").mask(24), ipv4(127u, 2u, 3u, 0u));
}
