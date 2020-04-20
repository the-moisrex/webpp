// Created by moisrex on 1/27/20.

#include "../core/include/webpp/utils/memory.h"

#include <gtest/gtest.h>
#include <string>

using namespace webpp;
using namespace std;

TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() >= 0);
}
