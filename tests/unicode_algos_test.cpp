#include "../webpp/unicode/unicode.hpp"
#include "../webpp/unicode/utf_reducer.hpp"
#include "common/tests_common_pch.hpp"

#include <string>

using webpp::unicode::is_code_unit_start;
using webpp::unicode::utf_reducer;

TEST(UnicodeAlgos, UnitStart) {
    EXPECT_TRUE(is_code_unit_start(u'a'));
    EXPECT_TRUE(is_code_unit_start(u'\0'));
    EXPECT_TRUE(is_code_unit_start(U'\0'));
    EXPECT_TRUE(is_code_unit_start(u'\xB3'));
    EXPECT_FALSE(is_code_unit_start(static_cast<char>(0b1000'0111)));
    EXPECT_FALSE(is_code_unit_start(static_cast<char16_t>(0xDF62)));
    EXPECT_FALSE(is_code_unit_start(static_cast<char16_t>(0xDC37)));
    EXPECT_TRUE(is_code_unit_start(static_cast<char16_t>(0x01D8)));
    EXPECT_TRUE(is_code_unit_start(static_cast<char16_t>(0x52D8)));
}

TEST(UnicodeAlgos, BasicCodePointIterator) {
    std::u8string str = u8"تست";
    {
        utf_reducer reducer{str.data(), str.size()};
        auto [pin] = reducer.pins();
        pin        = U'a';
        reducer.reduce();
        str.resize(static_cast<std::size_t>(reducer.end() - str.data()));
    }
    EXPECT_EQ(str, u8"aست");
}

TEST(UnicodeAlgos, BasicReducer) {
    std::u8string str = u8"تست";
    {
        utf_reducer reducer{str.data(), str.size()};
        auto [pin] = reducer.pins();
        ++pin;
        pin = U'a';
        reducer.reduce();
        str.resize(static_cast<std::size_t>(reducer.end() - str.data()));
    }
    EXPECT_EQ(str, u8"تaت");
}

TEST(UnicodeAlgos, ReducerIterator) {
    std::u8string str = u8"تست";
    {
        utf_reducer reducer{str.begin(), str.size()};
        auto [pin] = reducer.pins();
        ++pin;
        pin.set(U'a');
        reducer.reduce();
        str.resize(static_cast<std::size_t>(reducer.end() - str.begin()));
    }
    EXPECT_EQ(str, u8"تaت");
}

TEST(UnicodeAlgos, ReducerPins) {
    std::u8string str = u8"تست";
    {
        utf_reducer<2> reducer{str.data(), str.size()};
        auto [pin1, pin2] = reducer.pins();
        ++pin2;
        pin2 = U'a';
        pin1 = U'م';
        reducer.reduce();
        str.resize(static_cast<std::size_t>(reducer.end() - str.data()));
    }
    EXPECT_EQ(str, u8"مaت");
}

TEST(UnicodeAlgos, Extra) {
    std::u8string str = u8"تست";
    {
        utf_reducer<2> reducer{str.data(), str.size()};
        auto [pin1, pin2] = reducer.pins();
        ++pin2;
        pin2 = U'a';
        pin1 = U'\u0800'; // E0-A0-80
        reducer.reduce();
        str.resize(static_cast<std::size_t>(reducer.end() - str.data()));
    }
    EXPECT_EQ(str, u8"\xE0\xA0\x{80}aت");
}

TEST(UnicodeAlgos, BasicRead) {
    std::u8string str = u8"تست";
    {
        utf_reducer reducer{str.data(), str.size()};
        auto [pin] = reducer.pins();
        EXPECT_EQ(*pin, U'ت');
        pin = U'a';
        EXPECT_EQ(*pin, U'a');
        reducer.reduce();
        EXPECT_EQ(*pin, U'a');
        str.resize(static_cast<std::size_t>(reducer.end() - str.data()));
    }
    EXPECT_EQ(str, u8"aست");
}

TEST(UnicodeAlgos, SimpleForward) {
    std::u8string str = u8"تست";
    {
        utf_reducer<1> reducer{str.data(), str.size()};
        auto [pin] = reducer.pins();
        pin        = U'a';
        ++pin;
        pin = U'b';
        ++pin;
        pin.spillover_set(U'\u0800'); // E0-A0-80
        reducer.reduce();
        str.resize(reducer.size());
    }
    EXPECT_EQ(str, u8"ab\xE0\xA0\x80");
}

// TEST(UnicodeAlgos, DoubleForward) {
//     std::u8string str = u8"تست";
//     {
//         utf_reducer<2> reducer{str.data(), str.size()};
//         auto [pin1, pin2 ] = reducer.pins();
//         pin1        = U'a';
//         ++pin1;
//         pin2 = pin1;
//         pin2 = U'b';
//         ++pin2;
//         pin2 = U'\u0800'; // E0-A0-80
//         reducer.reduce();
//         str.resize(static_cast<std::size_t>(reducer.end() - str.data()));
//     }
//     EXPECT_EQ(str, u8"ab\xE0\xA0\x80");
// }
