// Created by moisrex on 1/27/20.
#include "../core/include/webpp/utils/strings.h"
#include <gtest/gtest.h>

using namespace webpp;
using namespace std;

TEST(Casts, ToInt) {
    EXPECT_TRUE(starts_with("_one", '_'));
    EXPECT_TRUE(starts_with("_one", "_"));

    EXPECT_FALSE(starts_with("_one", 'o'));
    EXPECT_FALSE(starts_with("_one", "o"));

    EXPECT_TRUE(ends_with("_one!", "!"));
    EXPECT_TRUE(ends_with("_one!", '!'));

    EXPECT_FALSE(ends_with("_one! ", "!"));
    EXPECT_FALSE(ends_with("_one! ", '!'));
}
