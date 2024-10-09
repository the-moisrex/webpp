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
    utf_reducer   iter{str.data(), str.size()};
    iter.set_code_point(U'a');
    EXPECT_EQ(str, u8"a\xAAست");
}

TEST(UnicodeAlgos, BasicCodePointIterator2) {
    std::u8string str = u8"تست";
    utf_reducer   iter{str.data(), str.size()};
    ++iter;
    iter.set_code_point(U'a');
    EXPECT_EQ(str, u8"تa\xB3ت");
}
