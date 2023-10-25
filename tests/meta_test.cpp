// Created by moisrex on 10/24/23.


#include "../webpp/common/meta.hpp"

#include "common/tests_common_pch.hpp"

TEST(MetaTest, AssumeTest) {

    // EXPECT_TRUE(std::is_void_v<decltype(webpp_assume(true))>);

    // Ensure BOUNDED_ASSUME is usable in a constant expression
    static constexpr bool res = ((void([]() {
                                      webpp_assume(true);
                                  }()),
                                  true));
    EXPECT_TRUE(res);
}
