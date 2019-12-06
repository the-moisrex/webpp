// Created by moisrex on 11/29/19.
#include <gtest/gtest.h>
#include <webpp/utils/const_list.h>

using namespace webpp;
using namespace std;

TEST(ConstListTest, ConstList) {
    constexpr auto one = make_const_list("one", "two", 3);
    EXPECT_EQ(one.value(), "one");
    EXPECT_EQ(one.next().value(), "two");
    EXPECT_EQ(one.next().next().value(), 3);

    one.for_each([&](auto const& value) constexpr {
        EXPECT_TRUE(one.has(value));
    });
}
