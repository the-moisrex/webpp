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
    using ptype = string_splits<"one", "two">;
    ptype pieces{"one two", " "};

    EXPECT_EQ(pieces.template view_of<"one">(), "one");
}


TEST(String, Splitter) {
    splitter   email_splitter{"test@email.com", '@'};
    auto const email = email_splitter.split();
    EXPECT_EQ(email[0], "test");
    stl::size_t                     index = 0;
    stl::array<stl::string_view, 2> parts{{"test", "email.com"}};
    for (auto part : email_splitter) {
        EXPECT_EQ(part, parts[index++])
    }
}


TEST(String, SplitterConstexpr) {
    constexpr splitter email_splitter{"test@email.com", '@'};
    constexpr auto     email = email_splitter.template split_to<2>();
    EXPECT_EQ(stl::get<0>(email), "test");
}
