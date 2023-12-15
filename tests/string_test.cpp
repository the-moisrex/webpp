// Created by moisrex on 11/20/20.
#include "../webpp/std/string_concepts.hpp"
#include "../webpp/strings/iequals.hpp"
#include "../webpp/strings/join.hpp"
#include "../webpp/strings/splits.hpp"
#include "common/tests_common_pch.hpp"

#include <array>

// NOLINTBEGIN(*-avoid-c-arrays,*-magic-numbers)

using namespace webpp;
using namespace webpp::istl;
using namespace webpp::strings;
using namespace webpp::ascii;

TEST(String, Concepts) {
    EXPECT_FALSE((stl::is_same_v<int, char_type_of_t<int *>>) );
    EXPECT_TRUE((stl::is_same_v<char, char_type_of_t<char[]>>) );
    EXPECT_TRUE((stl::is_same_v<char, char_type_of_t<char[10]>>) );
    EXPECT_TRUE((stl::is_same_v<char, char_type_of_t<char const[10]>>) );
    EXPECT_TRUE((stl::is_same_v<char, char_type_of_t<char const(&)[20]>>) );
    EXPECT_TRUE((stl::is_same_v<wchar_t, char_type_of_t<wchar_t const *>>) );
    EXPECT_TRUE((stl::is_same_v<char, char_type_of_t<std::string>>) );
    EXPECT_TRUE((stl::is_same_v<int, char_type_of_t<std::basic_string_view<int>>>) );

    EXPECT_TRUE(istl::StringViewifiable<char const *>);
    EXPECT_TRUE(istl::StringViewifiable<char *>);
    EXPECT_TRUE(istl::StringViewifiable<char const[8]>);
    EXPECT_TRUE(istl::StringViewifiable<char const(&)[8]>);

    EXPECT_TRUE(istl::StringViewifiable<wchar_t const *>);
    EXPECT_TRUE(istl::StringViewifiable<wchar_t *>);
    EXPECT_TRUE(istl::StringViewifiable<const wchar_t[8]>);
    EXPECT_TRUE(istl::StringViewifiable<const wchar_t(&)[8]>);

    EXPECT_TRUE((stl::same_as<char_type_of_t_string_literals<char *>, char>) );
    EXPECT_TRUE((stl::same_as<char_type_of_t_string_literals<wchar_t[]>, wchar_t>) );
    EXPECT_TRUE((stl::same_as<char_type_of_t_string_literals<wchar_t const(&)[20]>, wchar_t>) );
    EXPECT_FALSE((istl::StringLiteral<char>) );
    EXPECT_FALSE((istl::StringLiteral<stl::string_view>) );
    EXPECT_FALSE((istl::StringLiteral<char **>) );
    EXPECT_FALSE((istl::StringLiteral<wchar_t>) );
    EXPECT_FALSE((istl::StringLiteral<int *>) );
    EXPECT_TRUE(istl::StringLiteral<char *>);
    EXPECT_TRUE(istl::StringLiteral<char[]>);
    EXPECT_TRUE(istl::StringLiteral<char const[]>);
    EXPECT_TRUE(istl::StringLiteral<char const[20]>);
    EXPECT_TRUE(istl::StringLiteral<char[20]>);
    EXPECT_TRUE(istl::StringLiteral<char const *>);
    EXPECT_TRUE(istl::StringLiteral<wchar_t const *>);
}

TEST(String, IEquals) {
    using webpp::ascii::iequals;

    auto const one = "Hello World!";
    auto const two = "hELLo WORLd!";
    auto const res = iequals(one, two);
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
    stl::string const        one   = "one";
    stl::string const        two   = "two";
    stl::string const        three = "three";
    stl::vector<stl::string> strings;
    istl::collection::emplace(strings, one, two, three, "four");
    auto const res = join_with(strings, ' ');
    static_assert(
      stl::same_as<typename decltype(res)::allocator_type, typename decltype(two)::allocator_type>);
    EXPECT_EQ(res, "one two three four");

    using tup_type = stl::tuple<stl::string, stl::string, stl::string, stl::string_view>;
    tup_type const tup{one, two, three, "four"};
    auto const     tup_res = join_with(tup, ' ');
    EXPECT_EQ(tup_res, "one two three four");
}

TEST(String, Splitter) {
    splitter const email_splitter{"test@email.com", '@', "."};
    auto           it  = email_splitter.begin();
    auto           eit = email_splitter.end();
    EXPECT_EQ(it, it);
    EXPECT_EQ(eit, eit);
    EXPECT_NE(it, eit);
    ++it;
    EXPECT_NE(it, eit);
    EXPECT_EQ(*it, "email");
    ++it;
    EXPECT_EQ(*it, "com");
    ++it;
    EXPECT_EQ(it, eit);
    stl::size_t                     index = 0;
    stl::array<stl::string_view, 3> parts{
      {"test", "email", "com"}
    };
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

    constexpr splitter email_splitter2{"test@email.com", stl::string_view{"@"}, '.'};
    constexpr auto     email2 = email_splitter2.split_array();
    ASSERT_EQ(email2.size(), 3);
    ASSERT_EQ(stl::tuple_size_v<decltype(email2)>, 3);
    EXPECT_EQ(stl::get<0>(email2), "test");
    EXPECT_EQ(stl::get<1>(email2), "email");
    EXPECT_EQ(stl::get<2>(email2), "com");
}

TEST(String, StringViewConcept) {
    EXPECT_FALSE(static_cast<bool>(StringViewifiable<stl::array<char const *, 4>>));
}

// NOLINTEND(*-avoid-c-arrays,*-magic-numbers)
