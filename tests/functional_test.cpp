// Created by moisrex on 12/10/19.
#include <array>
#include <gtest/gtest.h>
#include <webpp/utils/functional.h>

using namespace webpp;

void test(int limit) {
    static auto i = 0;
    i++;
    EXPECT_TRUE(i < limit - 1);
}

TEST(FunctionalTests, DebouncedFunctions) {
    constexpr auto debounced = debounce<void(int)>();
    constexpr auto limit = 1000;
    for (int i = 0; i < limit; i++)
        debounced(limit);
}