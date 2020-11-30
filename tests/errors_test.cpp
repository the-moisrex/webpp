// Created by moisrex on 11/30/20.


#include "./common_pch.hpp"

// clang-format off
#include webpp_include(utils/errors)
// clang-format on

using namespace webpp;


enum struct e1 {
      one = success<e1>(),
      two = failure<e1>("two"),
      three = failure<e1>("three")
};


TEST(Errors, Handler) {
    error_handler<e1> h1;
}

