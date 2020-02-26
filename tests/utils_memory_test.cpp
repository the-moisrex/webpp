// Created by moisrex on 1/27/20.

#include <gtest/gtest.h>
#include <string>
#include "../core/include/webpp/utils/memory.h"

using namespace webpp;
using namespace std;

TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() >= 0);
}
