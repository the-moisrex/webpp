// Created by moisrex on 1/27/20.

#include "../core/include/webpp/memory/available_memory.hpp"
#include "./common_pch.hpp"


using namespace webpp;
using namespace std;

TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() >= 0);
}
