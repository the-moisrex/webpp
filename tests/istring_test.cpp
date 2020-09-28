// Created by moisrex on 9/28/20.

#include "../core/include/webpp/strings/istring.hpp"
#include <gtest/gtest.h>

using namespace webpp;

TEST(IString, Creation) {
    std_istring test = "test";
    EXPECT_EQ(test, "test");

    std_istring_view test_view = "test view";
    EXPECT_EQ(test_view, "test view");
}

TEST(IString, TOLowerToUpper) {
    std_istring test = "test";
    EXPECT_EQ(test.to_upper_copy(), "TEST");
    test.to_upper();
    EXPECT_EQ(test, "TEST");
    EXPECT_TRUE(test.is_upper());
    EXPECT_EQ(test.to_lower_copy(), "test");
    test.to_lower();
    EXPECT_TRUE(test.is_lower());
}

