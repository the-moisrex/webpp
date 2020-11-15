// Created by moisrex on 11/9/20.

#include "../core/include/webpp/uri/uri.hpp"

#include "./common_pch.hpp"
#include <string>

using namespace webpp;

TEST(URITests, Generation) {
    uri::uri u;
    EXPECT_EQ(u.scheme.size(), 0);

    auto alloc = u.get_allocator<std::allocator<char>>();

    std::string str{alloc};
    EXPECT_EQ(str.size(), 0);

    u.scheme = "https";
    EXPECT_EQ(u.to_string(), "https:");
    u.host = "webpp.dev";
    EXPECT_EQ(u.to_string(), "https://webpp.dev");
}
