// Created by moisrex on 9/28/20.

#include "../webpp/strings/istring.hpp"

#include "common/tests_common_pch.hpp"



using namespace webpp;

TEST(IString, Creation) {
    std_istring const test = "test";
    EXPECT_EQ(test, "test");

    std_istring_view const test_view = "test view";
    EXPECT_EQ(test_view, "test view");

    istring const hello = "hello";
    EXPECT_EQ(hello, "hello");
}

TEST(IString, TOLowerToUpper) {
    std_istring test = "test";
    EXPECT_EQ(test.ascii_to_upper_copy(), "TEST");
    test.ascii_to_upper();
    EXPECT_EQ(test, "TEST");
    EXPECT_TRUE(test.is_ascii_upper());
    EXPECT_EQ(test.ascii_to_lower_copy(), "test");
    test.ascii_to_lower();
    EXPECT_TRUE(test.is_ascii_lower());
}

TEST(IString, Trim) {
    std_istring test = "test";
    EXPECT_EQ("test", test.trim_copy());
    test = "  ";
    EXPECT_TRUE(test.trim_copy().empty());
    test = " t";
    EXPECT_EQ("t", test.trim_copy());
    test = "t    \t";
    EXPECT_EQ("t", test.rtrim_copy());
}

TEST(IString, Replace) {
    std_istring test = "000xxx000";
    test.replace('x', '1');
    EXPECT_EQ("000111000", test);
    test.replace('0', '2');
    EXPECT_EQ("222111222", test);
}

TEST(IString, Split) {
    std_istring const email_string = "user@domain.com";
    auto const        email        = email_string.splitter('@', '.');
    auto const        vec          = email.split();
    auto const        arr          = email.split_array();
    EXPECT_EQ(vec[0], "user");
    EXPECT_EQ(vec[1], "domain");
    EXPECT_EQ(vec[2], "com");

    EXPECT_EQ(arr[0], "user");
    EXPECT_EQ(arr[1], "domain");
    EXPECT_EQ(arr[2], "com");
}
