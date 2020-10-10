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
    EXPECT_EQ(parser.allowed_encodings().size(), 2); // plus identity
    EXPECT_TRUE(parser.is_allowed<parser.gzip>());

    accept_encoding<std_traits> parser2{"gzip; q=0.05"};
    parser2.parse();
    EXPECT_TRUE(parser2.is_valid());
    EXPECT_TRUE(parser2.is_allowed<parser2.gzip>());
    EXPECT_FLOAT_EQ(parser2.get<parser2.gzip>()->quality, 0.05f);
}
