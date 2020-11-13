#include "../core/include/webpp/strings/charset.hpp"
#include "../core/include/webpp/traits/std_traits.hpp"
#include "../core/include/webpp/validators/validators.hpp"

#include "./common_pch.hpp"
#include <map>
#include <string>
#include <vector>

using namespace webpp::ascii::is;
using namespace webpp::is;

TEST(ValidationsTest, ContainsFunctions) {
    std::map<int, std::string> data;
    data[0]  = "hello world";
    data[10] = "testing";

    EXPECT_TRUE(contains_key(data, 10));
    EXPECT_TRUE(contains_value(data, std::string("hello world")));
    EXPECT_FALSE(contains_key(data, 100));
    EXPECT_FALSE(contains_value(data, std::string("it's not gonna be there")));
}

TEST(ValidationsTest, TrimmedFunctions) {
    EXPECT_TRUE(trimmed("trimmed string"));
    EXPECT_TRUE(rtrimmed(" right trimmed"));
    EXPECT_TRUE(ltrimmed("left trimmed "));
}

TEST(ValidationsTest, IPv4Functions) {
    EXPECT_TRUE(ipv4("255.255.255.255"));
    EXPECT_FALSE(ipv4("256.1.1.1"));
    EXPECT_FALSE(ipv4("260.1.2.3"));
    EXPECT_TRUE(ipv4("127.0.0.1"));
    EXPECT_TRUE(ipv4("0.0.0.0"));
    EXPECT_TRUE(ipv4("192.168.0.0"));
    EXPECT_FALSE(ipv4("192.168.1.256"));
    EXPECT_TRUE(ipv4("192.168.0.255"));

    EXPECT_TRUE(ipv4_prefix("192.168.1.2:24"));
    EXPECT_TRUE(ipv4_prefix("192.168.1.2:16"));
    EXPECT_TRUE(ipv4_prefix("192.168.1.2:0"));
    EXPECT_TRUE(ipv4_prefix("192.168.1.2/24"));
    EXPECT_FALSE(ipv4_prefix("false_ip/24"));
    EXPECT_FALSE(ipv4_prefix("192.168.1.3/40"));
    EXPECT_FALSE(ipv4_prefix("192.168.1.3/false_prefix"));
    EXPECT_TRUE(ipv4_prefix("192.168.1.2-24",
                                               webpp::charset<char, 1>('-')));
}

TEST(ValidationTest, IPv6Functions) {
    auto valids = {"0102:0304:0506:0708:090a:0b0c:0d0e:0f00",
                   "0102:0304:0506:0708:090a:0B0C:0d0E:0F00",
                   "102:304:506:708:90a:B0C:d0E:F00",
                   "fd11::abcd:e0e0:d10e:0001",
                   "fd11:1234:5678:abcd:abcd:e0e0:d10e:1000",
                   "fd11:1234:5678:abcd:abcd:e0e0:d10e::",
                   "::1234:5678:abcd:abcd:e0e0:d10e:1000",
                   "ff03::0b",
                   "::",
                   "::1",
                   "1::1",
                   "1::",
                   "2001::d:a:b:127.0.0.1",
                   "64:ff9b::100.200.15.4",
                   "2001:db8::abc:def1:127.0.0.1"};

    auto invalids = {"2001:db8::a::b",
                     "2001:db8::abcd:efgh",
                     "0102:0304:0506:0708:090a:0B0C:0d0E:0F00:1",
                     "0102:0304:0506:0708:090a:0B0C:0d0E:0F00::",
                     "::0102:0304:0506:0708:090a:0B0C:0d0E:0F00::",
                     "::0102:0304:0506:0708:090a:0B0C:0d0E:0F00",
                     "11:11:0102:0304:0506:0708:090a:0B0C:0d0E:0F00",
                     "1:2:3:4:5:6:7:8:9",
                     "127.0.0.1::",
                     "2001::d:a:b:127.0.0.1::",
                     "2001:d:a:b:127.0.0.1::",
                     "2001:d:a:b::127.0.0.1:1",
                     "a:b:c:d",
                     "2001:db8::abc:def12:1:2",
                     "64:ff9b::123.231.0.257",
                     "64:ff9b::1.22.33",
                     "64:ff9b::1.22.33.44.5",
                     ".",
                     "",
                     "127.1.1.1",
                     ":.",
                     "::.",
                     ":0304:0506:0708:090a:0b0c:0d0e:0f00",
                     ":f:0:0:c:0:f:f:."};

    for (auto const& item : valids) {
        EXPECT_TRUE(ipv6(item)) << item;
        EXPECT_FALSE(ipv4(item)) << item;
        EXPECT_TRUE(ip(item)) << item;
        std::string ip  = item;
        std::string ip2 = item;
        std::string ip3 = item;
        ip.append("/64");
        EXPECT_TRUE(ipv6_prefix(ip))
          << "ip has a prefix: " << ip;
        ip2.append("/something bad");
        EXPECT_FALSE(ipv6_prefix(ip2));
        ip3.append("-128");
        EXPECT_TRUE(
          ipv6_prefix(ip3, webpp::charset<char, 1>('-')));
        EXPECT_TRUE(host("[" + std::string(item) + "]"))
          << "ip: " << item;
    }

    for (auto const& item : invalids) {
        EXPECT_FALSE(ipv6(item)) << item;
        std::string ip = item;
        ip.append("/64");
        EXPECT_FALSE(ipv6_prefix(ip));
        EXPECT_FALSE(host("[" + std::string(item) + "]"))
          << "ip: " << item;
    }
}

TEST(ValidationsTest, HostFunction) {
    auto valids = {
      "localhost",       "one.com", "example.notcom", "192.168.0.1",
      "255.255.255.255", "[::1]",   "127.0.0.1",
    };
    auto invalids = {"&^%&^%$&^%&^%$&^%$#@%$#@@!~#!@"};

    for (auto const& item : valids) {
        EXPECT_TRUE(host(item)) << "item is: " << item;
    }
    for (auto const& item : invalids) {
        EXPECT_FALSE(host(item)) << "item is: " << item;
    }
}

TEST(ValidationsTest, EmailFunction) {
    EXPECT_TRUE(email("moisrex@gmail.com"))
      << "moisrex@gmail.com should be valid";
    EXPECT_TRUE(email("moisrex.test@gmail.com"))
      << "moisrex.test@gmail.com should be valid";
    EXPECT_FALSE(email("not an.email@123.com"))
      << "spaces are not allowed in emails";
}

TEST(ValidationsTest, NumberFunctions) {
    for (char i = '0'; i <= '9'; i++)
        EXPECT_TRUE(digit(i));
    for (char i = 'a'; i <= 'z'; i++)
        EXPECT_FALSE(digit('c'));

    EXPECT_TRUE(digit(std::string_view{"123"}));
    EXPECT_FALSE(digit(std::string_view{"1.3"}));

    EXPECT_TRUE(number(std::string_view{"123"}));
    EXPECT_TRUE(number(std::string_view{"1.23"}));
    EXPECT_TRUE(number(std::string_view{"12.3333333333333351846162000"}));
    EXPECT_TRUE(number(std::string_view{"12.3333333333333351846162000"}));
    EXPECT_TRUE(number(std::string_view{".3333333333333351846162000"}));
    EXPECT_TRUE(number(std::string_view{"0.3333333333333351846162000"}));
    EXPECT_TRUE(number(std::string_view{"0."}));
    EXPECT_TRUE(number(std::string_view{"0.0"}));
    EXPECT_FALSE(number(std::string_view{"0.0a"}));
    EXPECT_TRUE(number(std::string_view{"00"}));
    EXPECT_FALSE(number(std::string_view{"abc"}));
    EXPECT_FALSE(number(std::string_view{" "}));
    EXPECT_FALSE(number(std::string_view{""}));
    EXPECT_FALSE(number('a'));
    EXPECT_TRUE(number('1'));
    EXPECT_TRUE(number('.'));
    EXPECT_TRUE(hex('a'));
    EXPECT_TRUE(hex('b'));
    EXPECT_TRUE(hex('B'));
    EXPECT_FALSE(hex('G'));
    EXPECT_FALSE(hex("str"));
    EXPECT_FALSE(hex(""));
}
