#include <gtest/gtest.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <webpp/validators/validators.h>

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

TEST(ValidationsTest, NumberFunctions) {
    for (char i = '0'; i <= '9'; i++)
        EXPECT_TRUE(digit(i));
    for (char i = 'a'; i <= 'z'; i++)
        EXPECT_FALSE(digit('c'));

    EXPECT_TRUE(digit("123"));
    EXPECT_FALSE(digit("1.3"));

    EXPECT_TRUE(number("123"));
    EXPECT_TRUE(number("1.23"));
    EXPECT_TRUE(number("12.3333333333333351846162000"));
    EXPECT_TRUE(number("12.3333333333333351846162000"));
    EXPECT_TRUE(number(".3333333333333351846162000"));
    EXPECT_TRUE(number("0.3333333333333351846162000"));
    EXPECT_TRUE(number("0."));
    EXPECT_TRUE(number("0.0"));
    EXPECT_FALSE(number("0.0a"));
    EXPECT_TRUE(number("00"));
    EXPECT_FALSE(number("abc"));
    EXPECT_FALSE(number(" "));
    EXPECT_FALSE(number(""));
    EXPECT_FALSE(number('a'));
    EXPECT_TRUE(number('1'));
    EXPECT_TRUE(number('.'));
}
