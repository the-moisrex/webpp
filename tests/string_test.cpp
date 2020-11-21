// Created by moisrex on 11/20/20.
#include "./common_pch.hpp"

// clang-format off
#include webpp_include(strings/join)
// clang-format on

#include <string>
#include <string_view>
#include <memory_resource>
using namespace webpp;

TEST(String, Join) {
    std::string one = "one ";
    std::pmr::string two = "two ";
    std::string_view three = "three";
    EXPECT_EQ(string::join(one, two, three, " four"), "one two three four");

    EXPECT_EQ(string::join(std::string("one, "), 1, 2), "one, 12");
}
