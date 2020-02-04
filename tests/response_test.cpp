// Created by moisrex on 2/4/20.

#include <gtest/gtest.h>
#include <webpp/http/response.h>

using namespace webpp;

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