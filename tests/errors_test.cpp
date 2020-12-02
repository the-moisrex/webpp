// Created by moisrex on 11/30/20.


#include "../core/include/webpp/utils/errors.hpp"

#include "./common_pch.hpp"


using namespace webpp;


enum struct e1 : stl::uint64_t { one = success<e1>(), two = failure<e1>("two"), three = failure<e1>("three") };


TEST(Errors, Handler) {
    error_handler<e1> h1;
    h1.failure(e1::two);
    EXPECT_FALSE(h1.is_success());
    EXPECT_TRUE(h1.is_failure());
    h1.success();
    EXPECT_TRUE(h1 == e1::one);
    EXPECT_TRUE(h1.is_success());
    EXPECT_FALSE(h1.is_failure());

    h1.failure(e1::three);
    EXPECT_TRUE(h1.is_on(e1::three));
    EXPECT_TRUE(h1 == e1::three);

    EXPECT_EQ(static_cast<stl::uint64_t>(e1::three), failure<e1>("three"));
}
