// Created by moisrex on 11/9/20.

#include "../core/include/webpp/uri/uri.hpp"

#include "./common_pch.hpp"
#include <string>

using namespace webpp;

TEST(URITests, Generation) {
    uri::uri u;
    u.scheme = "https";
    EXPECT_EQ(u.to_string(), "https:");
    u.host = "webpp.dev";
    EXPECT_EQ(u.to_string(), "http://webpp.dev");
}
