// Created by moisrex on 11/7/19.
#include <gtest/gtest.h>
#include <webpp/valves/valves.h>

using namespace webpp::is;

TEST(ValvesTests, Creation) {
    constexpr auto v = method("GET") or method("POST");
}
