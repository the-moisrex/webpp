// Created by moisrex on 6/2/23.

#include "../webpp/time/stopwatch.hpp"

#include "common_pch.hpp"

using namespace webpp;
using namespace std::chrono;
using namespace std::chrono_literals;

TEST(StopWatch, Init) {
    auto const            ten_secs_ago = high_resolution_clock::now() - 10s;
    time::stopwatch const watch{ten_secs_ago};
    EXPECT_GE(watch.elapse_now(), 10s);
}


TEST(StopWatch, Stop) {
    auto const      ten_secs_ago = high_resolution_clock::now() - 10s;
    time::stopwatch watch{ten_secs_ago};
    watch.stop();
    EXPECT_GE(watch.elapsed(), 10s);
}

TEST(StopWatch, Elapse) {
    auto const      ten_secs_ago = high_resolution_clock::now() - 10s;
    time::stopwatch watch{ten_secs_ago};
    watch.stop();
    EXPECT_EQ(watch.elapse(ten_secs_ago + 5s), 5s);
}


TEST(StopWatch, Duration) {
    time::stopwatch watch;
    watch += 10s;
    EXPECT_EQ(watch.elapsed(), 10s);
}


TEST(StopWatch, DurationSeconds) {
    time::stopwatch watch;
    watch += 10s;
    EXPECT_EQ(watch.elapsed_seconds(), 10s);
}
