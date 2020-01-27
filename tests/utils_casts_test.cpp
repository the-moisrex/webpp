// Created by moisrex on 1/27/20.
#include <gtest/gtest.h>
#include <string>
#include <webpp/utils/casts.h>

using namespace webpp;
using namespace std;

TEST(Casts, ToInt) {
    EXPECT_EQ(to_uint("10"), 10);
    EXPECT_EQ(to_int("+10"), 10);
    EXPECT_EQ(to_int("-10"), -10);
    EXPECT_EQ(to_uint("-10"), 10);
    EXPECT_EQ(to<int>("-10"), -10);
    EXPECT_EQ(to<unsigned long long>("+1025153153"), 1025153153);
}
