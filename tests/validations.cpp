#include <gtest/gtest.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <webpp/validators/validators.hpp>

using namespace webpp::is;
using namespace std;

TEST(ValidationsTest, EmptyFunction) {
    EXPECT_TRUE(empty(""));
    EXPECT_FALSE(empty("not empty"));
    string str = "";
    EXPECT_TRUE(empty(str));
    str = "not empty";
    EXPECT_FALSE(empty(str));
}

TEST(ValidationsTest, ContainsFunctions) {
    map<int, string> data;
    data[0] = "hello world";
    data[10] = "testing";

    EXPECT_TRUE(contains_key(data, 10));
    EXPECT_TRUE(contains_value(data, string("hello world")));
    EXPECT_FALSE(contains_key(data, 100));
    EXPECT_FALSE(contains_value(data, string("it's not gonna be there")));
}

TEST(ValidationsTest, TrimmedFunctions) {
    EXPECT_TRUE(trimmed("trimmed string"));
    EXPECT_TRUE(rtrimmed(" right trimmed"));
    EXPECT_TRUE(ltrimmed("left trimmed "));
}

TEST(ValidationsTest, IPFunctions) {
    EXPECT_TRUE(ipv4("255.255.255.255"));
    EXPECT_FALSE(ipv4("256.1.1.1"));
    EXPECT_TRUE(ipv4("127.0.0.1"));
    EXPECT_TRUE(ipv4("0.0.0.0"));
    EXPECT_TRUE(ipv4("192.168.0.0"));
    EXPECT_FALSE(ipv4("192.168.1.256"));
    EXPECT_TRUE(ipv4("192.168.0.255"));
}

TEST(ValidationsTest, EmailFunction) {
    EXPECT_TRUE(email("moisrex@gmail.com"))
        << "moisrex@gmail.com should be valid";
    EXPECT_TRUE(email("moisrex.test@gmail.com"))
        << "moisrex.test@gmail.com should be valid";
    EXPECT_FALSE(email("not an.email@123.com"))
        << "spaces are not allowd in emails";
}
