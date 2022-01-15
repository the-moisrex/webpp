// Created by moisrex on 15/1/22.


#include "../core/include/webpp/concurrency/atomic_counter.hpp"
#include "common_pch.hpp"

#include <thread>

using namespace webpp;
using namespace webpp::stl;

TEST(ConcurrencyTest, AtomicCounter) {

    atomic_counter counter;

    jthread th{[&] {
        for (int i = 0; i != 100; i++)
            ++counter;
    }};

    for (int i = 0; i != 100; i++)
        ++counter;

    th.join();

    EXPECT_EQ(counter, 200);
    EXPECT_LT(counter, 3000);
}