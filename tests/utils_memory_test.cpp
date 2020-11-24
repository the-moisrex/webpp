// Created by moisrex on 1/27/20.

#include "./common_pch.hpp"

// clang-format off
#include webpp_include(utils/memory)
// clang-format on

using namespace webpp;
using namespace std;

TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() >= 0);
}
