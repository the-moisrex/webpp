// Created by moisrex on 10/9/20.
#include <gtest/gtest.h>

#include "../core/include/webpp/http/headers/accept_encoding.hpp"
#include "../core/include/webpp/traits/std_traits.hpp"

using namespace webpp::http;
using namespace webpp;

TEST(Headers, AcceptEncoding) {
    accept_encoding<std_traits> parser{"gzip"};
    parser.parse();
    EXPECT_TRUE(parser.is_valid());
    EXPECT_TRUE(parser.is_allowed<parser.gzip>());
}
