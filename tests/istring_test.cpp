// Created by moisrex on 9/28/20.

#include "../core/include/webpp/strings/istring.hpp"
#include <gtest/gtest.h>

using namespace webpp;

TEST(IString, Creation) {
    std_istring test = "test";
    EXPECT_EQ(test, "test");
}

