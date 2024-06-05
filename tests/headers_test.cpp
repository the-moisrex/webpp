// Created by moisrex on 6/4/24.
#include "../webpp/http/status_code.hpp"
#include "common/tests_common_pch.hpp"


using namespace webpp;

#ifdef __cpp_lib_format
TEST(HeadersTest, StatusCodeSerialize) {
    EXPECT_EQ(std::format("{}", http::status_code::ok), "200 OK");
    EXPECT_EQ(std::format("{}", http::status_code::forbidden), "403 Forbidden");
}
#endif
