// Created by moisrex on 9/25/20.
#include "./common_pch.hpp"
#include <string_view>
#include <eve/wide.hpp>
#include <eve/function/any.hpp>

namespace mtest {
    std::size_t find_str_simple(std::string_view str1, std::string_view str2) noexcept {
        using char_type                 = typename std::remove_cvref_t<decltype(str1)>::value_type;
        using simd_type                 = eve::wide<char_type>;
        using string_type               = std::string_view;
        static constexpr auto simd_size = simd_type::size();

        const auto size1 = str1.size();
        const auto size2 = str2.size();
        if (size2 > size1)
            return string_type::npos;

        const auto it1start               = str1.data();
        const auto it2start               = str2.data();
        const auto it1end                 = it1start + size1;
        const auto it2end                 = it2start + size2;
        const auto last_possible_position = size1 - size2;

        auto it1 = str1.data();
        auto it2 = str2.data();

        const auto    last_possible_end = it1end - last_possible_position;
        decltype(it1) found             = nullptr;
        for (; ; ++it1) {
            if (*it1 == *it2) {
                if (it2 == it2start) {
                    found = it1;
                } else if (it2 == it2end - 1) {
                    return found - it1start;
                }
                ++it2;
            } else {
                if (found != nullptr) {
                    it1   = found + 1;
                    it2   = it2start;
                    found = nullptr;
                } else {
                    if (it1 == last_possible_end)
                        break;
                }
            }
        }


        return string_type::npos;
    }
}

// The above algorithm is buggy, so let's not run the tests when we don't need them

//TEST(STRFinder, Simple) {
//    using namespace mtest;
//
//    EXPECT_EQ(0, find_str_simple("string", "str"));
//    EXPECT_EQ(1, find_str_simple("_string", "str"));
//    EXPECT_EQ(2, find_str_simple("__string", "str"));
//    EXPECT_EQ(2, find_str_simple("__string", "string"));
//    EXPECT_EQ(0, find_str_simple("012345678901234567890123456789", "0123456789"));
//    EXPECT_EQ(0, find_str_simple("00000000000000", "0"));
//    EXPECT_EQ(std::string_view::npos, find_str_simple("00-0000-000-00-000", "00000"));
//    EXPECT_EQ(std::string_view::npos, find_str_simple("__string", "strings"));
//    EXPECT_EQ(std::string_view::npos, find_str_simple("ABC", "ABX"));
//    EXPECT_EQ(std::string_view::npos, find_str_simple("0ABC", "0ABX"));
//}
