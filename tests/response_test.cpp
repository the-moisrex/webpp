// Created by moisrex on 2/4/20.

#include <gtest/gtest.h>
#include <webpp/http/response.h>
#include <string>
#include <string_view>
#include <type_traits>

using namespace webpp;

TEST(Response, Type) {
    constexpr auto return_callback = [] {
        return response("Hello");
    };
    using ret_type = std::invoke_result_t<decltype(return_callback)>;
    constexpr bool one = std::is_same_v<ret_type, response>;
    constexpr bool two = std::is_convertible_v<ret_type, response>;
    constexpr bool three = std::is_convertible_v<response, response>;
    constexpr bool four = std::is_convertible_v<std::string, response>;
//    constexpr bool five = std::is_convertible_v<std::string_view, response>;
    EXPECT_TRUE(one);
    EXPECT_TRUE(two);
    EXPECT_TRUE(three);
    EXPECT_TRUE(four);
//    EXPECT_TRUE(five);
}

TEST(Response, Init) {
    auto res = response();
    auto res2 = response();

    EXPECT_EQ(res, res2);

    EXPECT_EQ(std::string(res.body.str("")), "");
    res2 << "Code";
    EXPECT_EQ(std::string(res2.body.str()), "Code");
    res = res2;
    EXPECT_EQ(std::string(res.body.str()), "Code");

    EXPECT_EQ(res, res2);
}