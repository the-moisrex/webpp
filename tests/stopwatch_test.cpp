// Created by moisrex on 6/2/23.

#include "../webpp/time/stopwatch.hpp"

#include "common_pch.hpp"

using namespace webpp;
using namespace std::chrono;
using namespace std::chrono_literals;

TEST(StopWatch, Init) {
    auto const            ten_secs_ago = high_resolution_clock::now() - 10s;
    time::stopwatch const watch{ten_secs_ago};
    EXPECT_GE(watch.elapsed(), 10s);
}
