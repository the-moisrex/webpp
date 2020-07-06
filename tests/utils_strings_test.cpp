// Created by moisrex on 1/27/20.
#include "../core/include/webpp/traits/std_traits.h"
#include "../core/include/webpp/utils/strings.h"

#include <gtest/gtest.h>

using namespace webpp;
using namespace std;

TEST(Strings, StartsEndsWith) {
    EXPECT_TRUE(starts_with<std_traits>("_one", '_'));
    EXPECT_TRUE(starts_with<std_traits>("_one", "_"));

    EXPECT_FALSE(starts_with<std_traits>("_one", 'o'));
    EXPECT_FALSE(starts_with<std_traits>("_one", "o"));

    EXPECT_TRUE(ends_with<std_traits>("_one!", "!"));
    EXPECT_TRUE(ends_with<std_traits>("_one!", '!'));

    EXPECT_FALSE(ends_with<std_traits>("_one! ", "!"));
    EXPECT_FALSE(ends_with<std_traits>("_one! ", '!'));

    EXPECT_TRUE(starts_with<std_traits>("_one! ", "_one!"));
    EXPECT_FALSE(starts_with<std_traits>("_one! ", "__one!"));
}
