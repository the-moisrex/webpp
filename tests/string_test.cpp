// Created by moisrex on 11/20/20.
#include "../core/include/webpp/strings/join.hpp"
#include "../core/include/webpp/strings/splits.hpp"
#include "./common_pch.hpp"


using namespace webpp;
using namespace webpp::strings;

TEST(String, Join) {
    stl::string      one   = "one ";
    stl::string      two   = "two ";
    stl::string_view three = "three";
    auto const       res   = join(one, two, three, " four");
    static_assert(
      stl::same_as<typename decltype(res)::allocator_type, typename decltype(two)::allocator_type>);
    EXPECT_EQ(res, "one two three four");

    EXPECT_EQ(join(stl::string("one, "), 1, 2), "one, 12");
}



TEST(String, StringPieces) {
    using ptype       = string_splits<"one", "two">;
    using tup_type    = typename ptype::tuple_type;
    using ptr_t       = typename ptype::str_ptr;
    using tup_crafted = stl::tuple<ptr_t, ptr_t, ptr_t>;
    EXPECT_TRUE(static_cast<bool>(stl::is_same_v<tup_type, tup_crafted>));

    ptype pieces{"one two"};

    // fixme: I don't understand this line that I wrote!!!
    EXPECT_TRUE(pieces.template view_of<"one">().empty());
}
