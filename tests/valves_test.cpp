// Created by moisrex on 4/8/23.

#include "../webpp/http/routes/valves.hpp"

#include "../webpp/http/routes/path.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;
using namespace webpp::http;

// for types that you don't have access to them, they need to be inside the namespace
namespace webpp::http {
    inline constexpr double tag_invoke(valvify_tag, int const a) noexcept {
        return static_cast<double>(a);
    }
} // namespace webpp::http

// for types that you have access to, you can use this way:
struct custom_type {
    friend inline constexpr int tag_invoke(stl::tag_t<valvify>, custom_type) noexcept {
        return 1;
    }
};

TEST(ValvesTests, ValveifyTest) {
    EXPECT_TRUE(
      static_cast<bool>(stl::same_as<valvified_type<stl::string_view>, segment_string<stl::string_view>>));
    EXPECT_TRUE(
      static_cast<bool>(stl::same_as<valvified_type<char const*>, segment_string<stl::string_view>>));
    EXPECT_TRUE(static_cast<bool>(stl::same_as<valvified_type<int>, double>));
    EXPECT_TRUE(static_cast<bool>(stl::same_as<valvified_type<custom_type>, int>));

    using ttt = decltype(valve{} / int{1});
    EXPECT_TRUE(static_cast<bool>(istl::cvref_as<ttt, segment_valve<double>>));
    EXPECT_EQ((valve{} / int{1}).as_tuple(), segment_valve<double>{1.0}.as_tuple());

    EXPECT_TRUE(static_cast<bool>(istl::cvref_as<decltype(valve{} / custom_type{}), segment_valve<int>>));
}
