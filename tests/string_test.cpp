// Created by moisrex on 11/20/20.
#include "../core/include/webpp/strings/join.hpp"
#include "../core/include/webpp/strings/string_vector.hpp"
#include "./common_pch.hpp"


using namespace webpp;
using namespace webpp::string;

TEST(String, Join) {
    stl::string      one   = "one ";
    stl::pmr::string two   = "two ";
    stl::string_view three = "three";
    auto const       res   = string::join(one, two, three, " four");
    static_assert(
      stl::same_as<typename decltype(res)::allocator_type, typename decltype(two)::allocator_type>);
    EXPECT_EQ(res, "one two three four");

    EXPECT_EQ(string::join(stl::string("one, "), 1, 2), "one, 12");
}



TEST(String, StringPieces) {
    string_splits<"one", "two"> pieces{"one two"};

    // fixme: I don't understand this line that I wrote!!!
    EXPECT_TRUE(pieces.template view_of<"one">().empty());
}
