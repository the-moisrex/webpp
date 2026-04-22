// Created by moisrex on 1/27/20.
#include "../webpp/convert/casts.hpp"
#include "../webpp/traits/std_traits.hpp"
#include "common/test.hpp"

using namespace webpp;
using namespace std;

TEST(Casts, ToInt) {
    EXPECT_EQ(to_uint("10"), 10U);
    EXPECT_EQ(to_int("+10"), 10);
    EXPECT_EQ(to_int("-10"), -10);
    EXPECT_EQ((to<int>("-10")), -10);
    EXPECT_EQ((to<unsigned long long>("+1025153153")), 1'025'153'153U);

    auto const invalids = {"one", "+two", "12a", "a11", "+a11", "111-751", "123,321", "123.123", "123+123"};

    for (auto const& str : invalids) {
        // EXPECT_EQ(to<int>(c), 0);
        // EXPECT_EQ(to_uint64(c), 0);
        try {
            [[maybe_unused]] auto _ = to<int>(str).value_safe();
            EXPECT_TRUE(false) << str;
        } catch (std::exception const& err) {
            EXPECT_TRUE(true);
        }
    }


    if (auto const value = to<int>("-10")) {
        EXPECT_EQ(value.value(), -10);
    }

    // Check if boolean cast works
    if (auto const value = to<std::uint32_t>("invalid")) {
        EXPECT_TRUE(false);
    }

    if (auto const value = to<std::uint32_t>("invalid"); !value) {
        EXPECT_EQ(value.error(), integer_casting_errors::invalid_character);
    }
}

TEST(Casts, ToIntOverflow) {
    using enum integer_casting_errors;

    EXPECT_EQ(to_uint32("-10"), negative_unsigned);
    EXPECT_EQ(to_uint8("+258"), overflow);
    EXPECT_EQ(to_uint8("+256"), overflow);
    EXPECT_EQ(to_uint8("252"), overflow);
    EXPECT_EQ(to_uint8("251"), overflow);
    EXPECT_EQ(to_uint8("250"), 250);
    EXPECT_EQ(to_uint8("-50"), negative_unsigned);
}
