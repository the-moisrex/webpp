// Created by moisrex on 1/27/20.

#include "../core/include/webpp/utils/memory.hpp"

#include "./common_pch.hpp"
#include <string>

using namespace webpp;
using namespace std;

TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() >= 0);
}
