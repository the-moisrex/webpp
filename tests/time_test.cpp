#include "../webpp/time/convert.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;
using namespace std::chrono;
using namespace std::chrono_literals;

TEST(TimeConvert, ToTimeval) {
    timeval tv50{.tv_sec = 0, .tv_usec = 50000u};
    EXPECT_EQ(tv50.tv_sec, to_timeval(50ms).tv_sec);
    EXPECT_EQ(tv50.tv_usec, to_timeval(50ms).tv_usec);

    EXPECT_EQ(tv50.tv_sec, to_timeval(50000us).tv_sec);
    EXPECT_EQ(tv50.tv_usec, to_timeval(50000us).tv_usec);

    timeval tv = to_timeval(milliseconds(1));
    EXPECT_EQ(tv.tv_sec, 0);
    EXPECT_EQ(tv.tv_usec, 1000);

    tv = to_timeval(milliseconds(2500));
    EXPECT_EQ(tv.tv_sec, 2);
    EXPECT_EQ(tv.tv_usec, 500000);

    tv = to_timeval(seconds(5));
    EXPECT_EQ(tv.tv_sec, 5);
    EXPECT_EQ(tv.tv_usec, 0);
}
