// Created by moisrex on 11/30/20.


#include "../core/include/webpp/utils/errors.hpp"

#include "./common_pch.hpp"


using namespace webpp;


enum struct e1 { one = success<e1>(), two = failure<e1>("two"), three = failure<e1>("three") };


TEST(Errors, Handler) {
    error_handler<e1> h1;
}
