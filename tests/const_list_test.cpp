// Created by moisrex on 11/29/19.
#include <gtest/gtest.h>
#include <string>
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

    auto str_one = one.reduce(
        [](string data, auto const& value) mutable {
            data.append(string((data.empty() ? "" : " ")));
            if constexpr (std::is_convertible_v<decltype(value), int>) {
                data.append(to_string(value));
            } else {
                data.append(value);
            }
            return data;
        },
        string(""));

    EXPECT_EQ(str_one, "one two 3");
}
