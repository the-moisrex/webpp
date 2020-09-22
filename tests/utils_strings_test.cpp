// Created by moisrex on 1/27/20.
#include "../core/include/webpp/utils/strings.hpp"

#include <gtest/gtest.h>

using namespace webpp;

TEST(Strings, StartsEndsWith) {
    EXPECT_TRUE(starts_with("_one", '_'));
    EXPECT_TRUE(starts_with("_one", "_"));

    EXPECT_FALSE(starts_with("_one", 'o'));
    EXPECT_FALSE(starts_with("_one", "o"));

    EXPECT_TRUE(ends_with("_one!", "!"));
    EXPECT_TRUE(ends_with("_one!", '!'));

    EXPECT_FALSE(ends_with("_one! ", "!"));
    EXPECT_FALSE(ends_with("_one! ", '!'));

    EXPECT_TRUE(starts_with("_one! ", "_one!"));
    EXPECT_FALSE(starts_with("_one! ", "__one!"));
}


TEST(Strings, ToLowerToUpper) {
    EXPECT_EQ(to_lower_copy('A'), 'a');
    EXPECT_EQ(to_lower_copy('-'), '-');
    EXPECT_EQ(to_upper_copy('-'), '-');
    EXPECT_EQ(to_upper_copy('A'), 'A');
    EXPECT_EQ(to_upper_copy('a'), 'A');
    EXPECT_EQ(to_upper_copy("ABC"), "ABC");
    EXPECT_EQ(to_upper_copy("ABC"), "ABC");

    std::string str = "this is a STRING";
    to_lower(str);
    EXPECT_EQ(str, "this is a string");
    to_upper(str);
    EXPECT_EQ(str, "THIS IS A STRING");
}
