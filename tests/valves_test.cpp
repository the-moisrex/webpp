// Created by moisrex on 4/8/23.

#include "../webpp/http/routes/valves.hpp"

#include "../webpp/http/routes/path.hpp"
#include "common_pch.hpp"

using namespace webpp;
using namespace webpp::http;

template <>
struct webpp::http::valvify<int> {
    static constexpr double call(int a) noexcept {
        return static_cast<double>(a);
    }
};

TEST(ValvesTests, ValveifyTest) {
    EXPECT_TRUE(bool(stl::same_as<decltype(valvify_or(stl::declval<stl::string_view>())),
                                  segment_string<stl::string_view>>));
    EXPECT_TRUE(bool(
      stl::same_as<decltype(valvify_or(stl::declval<const char*>())), segment_string<stl::string_view>>));
    EXPECT_TRUE(bool(stl::same_as<decltype(valvify_or(stl::declval<int>())), double>));

    using ttt = decltype(valve{} / int{1});
    EXPECT_TRUE(bool(istl::cvref_as<ttt, segment_valve<double>>));
    EXPECT_EQ((valve{} / int{1}).as_tuple(), segment_valve<double>{1.0}.as_tuple());
}
