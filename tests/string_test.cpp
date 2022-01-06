// Created by moisrex on 11/20/20.
#include "../core/include/webpp/strings/iequals.hpp"
#include "../core/include/webpp/strings/join.hpp"
#include "../core/include/webpp/strings/splits.hpp"
#include "./common_pch.hpp"


using namespace webpp;
using namespace webpp::strings;
using namespace webpp::ascii;

TEST(String, iequals) {
    const auto one = "Hello World!";
    const auto two = "hELLo WORLd!";
    const auto res = iequals(one, two);
    EXPECT_TRUE(res);
    EXPECT_TRUE(iequals<char_case_side::second_lowered>("OnE", "one"));
    EXPECT_EQ(iequals("123 One Two aZ", "123 oNe TWo Az"), true);
    EXPECT_EQ(iequals("103 One Two aZ", "123 oNe TWo Az"), false);
    EXPECT_EQ(iequals("not the same size as", "123 oNe TWo Az"), false);
    EXPECT_EQ(iequals("Short", "SHorT"), true);
    EXPECT_EQ(iequals("Shor", "Short"), false);
}

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


TEST(String, JoinWith) {
    stl::string              one   = "one";
    stl::string              two   = "two";
    stl::string              three = "three";
    stl::vector<stl::string> strings;
    istl::collection::emplace(strings, one, two, three, "four");
    auto const res = join_with(strings, ' ');
    static_assert(
      stl::same_as<typename decltype(res)::allocator_type, typename decltype(two)::allocator_type>);
    EXPECT_EQ(res, "one two three four");

    using tup_type = stl::tuple<stl::string, stl::string, stl::string, stl::string_view>;
    tup_type   tup{one, two, three, "four"};
    const auto tup_res = join_with(tup, ' ');
    EXPECT_EQ(tup_res, "one two three four");
}




TEST(String, Splitter) {
    splitter email_splitter{"test@email.com", '@', "."};
    auto     it  = email_splitter.begin();
    auto     eit = email_splitter.end();
    EXPECT_EQ(it, it);
    EXPECT_EQ(eit, eit);
    EXPECT_NE(it, eit);
    it++;
    EXPECT_NE(it, eit);
    EXPECT_EQ(*it, "email");
    it++;
    EXPECT_EQ(*it, "com");
    ++it;
    EXPECT_EQ(it, eit);
    stl::size_t                     index = 0;
    stl::array<stl::string_view, 3> parts{{"test", "email", "com"}};
    for (auto part : email_splitter) {
        EXPECT_EQ(part, parts[index++]);
    }
    auto const email = email_splitter.split();
    EXPECT_EQ(email.size(), 3);
    EXPECT_EQ(email[0], "test");
    EXPECT_EQ(email[1], "email");
    EXPECT_EQ(email[2], "com");
    index = 0;
    for (auto part : email) {
        EXPECT_EQ(part, parts[index]) << part << " should be the same as " << parts[index];
        index++;
    }
}


TEST(String, SplitterConstexpr) {
    constexpr splitter email_splitter{"test@email.com", '@'};
    constexpr auto     email = email_splitter.split_array();
    EXPECT_EQ(stl::get<0>(email), "test");
}
