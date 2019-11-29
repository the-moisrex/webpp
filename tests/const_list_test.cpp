// Created by moisrex on 11/29/19.
#include <gtest/gtest.h>
#include <webpp/utils/const_list.h>

using namespace webpp;
using namespace std;

TEST(ConstListTest, ConstList) {
    constexpr auto one = const_list("one")("two").append(3);
    EXPECT_EQ(one.value, "one");
    EXPECT_EQ(one.next.value, "two");
    EXPECT_EQ(one.next.next.value, 3);
}
