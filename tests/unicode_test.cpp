// Created by moisrex on 12/28/20.
#include "../webpp/unicode/unicode.hpp"

#include "../webpp/std/format.hpp"
#include "../webpp/unicode/normalization.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;
using namespace std;

// NOLINTBEGIN(*-magic-numbers)

TEST(Unicode, U8UnChecked) {
    using namespace unicode::unchecked;

    char8_t const* str = u8"این یک تست است.";
    // have to use static_cast<int> because the google-test doesn't seem to understand char8_t
    EXPECT_EQ(static_cast<int>(u8"ی"[0]), static_cast<int>(*next_char_copy(str + 1)));
    EXPECT_EQ(static_cast<int>(u8"ن"[0]), static_cast<int>(*next_char_copy(str + 3)));
    EXPECT_EQ(static_cast<int>(u8"ی"[0]), static_cast<int>(*prev_char_copy(str + 3)));
    EXPECT_EQ(static_cast<int>(u8"ن"[0]), static_cast<int>(*prev_char_copy(str + 5)));
}

TEST(Unicode, U16UnChecked) {
    using namespace unicode::unchecked;

    char16_t const* str = u"این یک تست است.";
    // have to use static_cast<int> because the google-test doesn't seem to understand char8_t
    EXPECT_EQ(static_cast<int>(u"ی"[0]), static_cast<int>(*next_char_copy(str)));
    EXPECT_EQ(static_cast<int>(u"ن"[0]), static_cast<int>(*next_char_copy(str + 1)));
    EXPECT_EQ(static_cast<int>(u"ی"[0]), static_cast<int>(*prev_char_copy(str + 5)));
    EXPECT_EQ(static_cast<int>(u"ن"[0]), static_cast<int>(*prev_char_copy(str + 3)));
}

TEST(Unicode, AppendBMPCodePoint) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0041)); // 'A'
    ASSERT_EQ(utf16String, u"\u0041");
}

TEST(Unicode, AppendSupplementaryCodePoint) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x1'F600)); // Grinning Face
    ASSERT_EQ(utf16String, u"\U0001F600");
}

TEST(Unicode, AppendMultipleCodePoints) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0041));   // 'A'
    ASSERT_TRUE(append(utf16String, 0x1'F600)); // Grinning Face
    ASSERT_TRUE(append(utf16String, 0x0042));   // 'B'
    ASSERT_EQ(utf16String, u"\u0041\U0001F600\u0042");
}

TEST(Unicode, AppendMinBMPCodePoint) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0000));
    ASSERT_EQ(utf16String.size(), 1);
    ASSERT_EQ(utf16String[0], 0);
}

TEST(Unicode, AppendMaxBMPCodePoint) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0xFFFF));
    ASSERT_EQ(utf16String, u"\uFFFF");
}

TEST(Unicode, AppendMinSupplementaryCodePoint) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x1'0000));
    ASSERT_EQ(utf16String, u"\U00010000");
}

TEST(Unicode, AppendMaxSupplementaryCodePoint) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x10'FFFF));
    ASSERT_EQ(utf16String, u"\U0010FFFF");
}

TEST(Unicode, AppendInvalidCodePoint) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_FALSE(append(utf16String, 0x11'0000)); // Invalid code point
    ASSERT_EQ(utf16String, u"");
}

TEST(Unicode, AppendAllowedBMPCodePoints) {
    using unicode::checked::append;

    std::u16string utf16String;
    for (char32_t codePoint = 0x0001; codePoint <= unicode::max_bmp<char32_t>; ++codePoint) {
        bool const res = append(utf16String, codePoint);
        if (unicode::is_surrogate(codePoint)) {
            ASSERT_FALSE(res) << "0x" << std::hex << static_cast<uint16_t>(codePoint)
                              << " != " << static_cast<uint16_t>(utf16String.back()) << std::dec;
            continue;
        }
        EXPECT_TRUE(res);
        EXPECT_EQ(utf16String.back(), codePoint);
        ASSERT_EQ(utf16String.size(), 1);
        utf16String.clear();
    }
}

TEST(Unicode, AppendAllowedSupplementaryCodePoints) {
    using unicode::checked::append;

    std::u16string utf16String;
    for (char32_t codePoint = 0x1'0000; codePoint <= 0x10'FFFF; codePoint++) {
        EXPECT_TRUE(append(utf16String, codePoint));
        ASSERT_EQ(utf16String.size(), 2);
        utf16String.clear();
    }
}

TEST(Unicode, AppendAllowedAndInvalidCodePoints) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0041));     // 'A'
    ASSERT_FALSE(append(utf16String, 0x11'0000)); // Invalid code point
    ASSERT_TRUE(append(utf16String, 0x1'F600));   // Grinning Face
    ASSERT_FALSE(append(utf16String, 0x11'0001)); // Invalid code point
    ASSERT_TRUE(append(utf16String, 0x0042));     // 'B'
    ASSERT_EQ(utf16String, u"\u0041\U0001F600\u0042");
}

TEST(Unicode, AppendMixOfCodePointTypes) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0041));    // 'A'
    ASSERT_TRUE(append(utf16String, 0x1'F600));  // Grinning Face
    ASSERT_TRUE(append(utf16String, 0x0042));    // 'B'
    ASSERT_TRUE(append(utf16String, 0x1'0000));  // Supplementary code point
    ASSERT_TRUE(append(utf16String, 0x0043));    // 'C'
    ASSERT_TRUE(append(utf16String, 0x10'FFFF)); // Supplementary code point
    ASSERT_TRUE(append(utf16String, 0x0044));    // 'D'
    ASSERT_EQ(utf16String, u"\u0041\U0001F600\u0042\U00010000\u0043\U0010FFFF\u0044");
}

TEST(Unicode, AppendLargeNumberOfCodePoints) {
    using unicode::checked::append;

    std::u16string utf16String;
    for (char32_t codePoint = unicode::max_bmp<char32_t> + 1; codePoint <= unicode::max_legal_utf32<char32_t>;
         codePoint++)
    {
        auto const prev_size = utf16String.size();
        bool const res       = append(utf16String, codePoint);
        if (unicode::is_surrogate(codePoint) || !unicode::is_code_point_valid(codePoint)) {
            ASSERT_FALSE(res) << "0x" << std::hex << static_cast<uint32_t>(codePoint) << " --- "
                              << static_cast<uint32_t>(utf16String.back()) << std::dec;
            continue;
        }
        ASSERT_TRUE(res) << "0x" << std::hex << static_cast<uint16_t>(codePoint) << " --- "
                         << static_cast<uint16_t>(utf16String.back()) << std::dec;
        EXPECT_EQ(utf16String.size() - prev_size, 2) << std::hex << static_cast<uint32_t>(codePoint);
    }
}

TEST(Unicode, AppendCodePointsWithExtraData) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0041));    // 'A'
    ASSERT_TRUE(append(utf16String, 0x1'F600));  // Grinning Face
    ASSERT_TRUE(append(utf16String, 0x0042));    // 'B'
    ASSERT_TRUE(append(utf16String, 0x1'0000));  // Supplementary code point
    ASSERT_TRUE(append(utf16String, 0x0043));    // 'C'
    ASSERT_TRUE(append(utf16String, 0x10'FFFF)); // Supplementary code point
    ASSERT_TRUE(append(utf16String, 0x0044));    // 'D'
    ASSERT_EQ(utf16String, u"\u0041\U0001F600\u0042\U00010000\u0043\U0010FFFF\u0044");

    // Append some extra data
    utf16String += u"\u0001\u0002\u0003\u0004";

    ASSERT_EQ(utf16String, u"\u0041\U0001F600\u0042\U00010000\u0043\U0010FFFF\u0044\u0001\u0002\u0003\u0004");
}

TEST(Unicode, AppendCodePointsWithInvalidSurrogates) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_FALSE(append(utf16String, 0xD800)); // Invalid leading surrogate
    ASSERT_FALSE(append(utf16String, 0xD801)); // Invalid leading surrogate
    ASSERT_FALSE(append(utf16String, 0xDC00)); // Invalid trailing surrogate
    ASSERT_FALSE(append(utf16String, 0xDC01)); // Invalid trailing surrogate
    ASSERT_EQ(utf16String, u"");
}

TEST(Unicode, AppendCodePointsWithInvalidCodePoint) {
    using unicode::checked::append;

    std::u16string utf16String;
    ASSERT_FALSE(append(utf16String, 0x11'0000)); // Invalid code point
    ASSERT_FALSE(append(utf16String, 0x11'0001)); // Invalid code point
    ASSERT_FALSE(append(utf16String, 0x1F'FFFF)); // Invalid code point
    ASSERT_EQ(utf16String, u"");
}

///////////////////////////////// Normalization /////////////////////////////////////

template <typename CharT = char32_t>
[[nodiscard]] static constexpr stl::string desc_ccc_of(CharT const code_point) noexcept {
    using webpp::unicode::details::ccc_index;
    using webpp::unicode::details::ccc_indices;
    using webpp::unicode::details::ccc_values;
    using webpp::unicode::details::trailing_zero_cccs;
    if (code_point >= static_cast<CharT>(trailing_zero_cccs)) [[unlikely]] {
        return "Definite Zero";
    }
    auto const code_point_range = static_cast<stl::size_t>(code_point) >> ccc_index::chunk_shift;
    auto const code_point_index = static_cast<stl::uint8_t>(code_point & ccc_index::chunk_mask);
    // auto const        helper           = ccc_index[code_point_range];
    // auto const        mask             = static_cast<stl::uint8_t>(helper);
    // auto const        shift            = static_cast<stl::uint8_t>(helper >> 8);
    // auto const        index            = helper >> 16U;
    // auto const        masked_pos       = mask & code_point_index;
    // stl::size_t const index_pos        = index + static_cast<stl::size_t>(masked_pos);

    // auto const code_point_index = static_cast<stl::uint8_t>(code_point);
    auto const code             = ccc_indices[code_point_range];
    // calculating the position of te value in the ccc_values table:
    stl::size_t const index_pos = code.get_position(code_point);
    auto              res       = ccc_values[index_pos];

    stl::string around = "[..., ";
    for (stl::size_t pos = static_cast<stl::size_t>(stl::max(static_cast<long>(index_pos) - 3l, 0l));
         pos != stl::min(static_cast<stl::size_t>(index_pos + 3), ccc_values.size());
         ++pos)
    {
        around += stl::to_string(pos);
        around += "=";
        around += stl::to_string(ccc_values[pos]);
        around += ", ";
    }
    around += "...]";
    return fmt::format(
      R"data(code: {}
mask: {}
index: {}
sub code point index: {}
masked position: {}
actual index: {}
result: {}
{}
)data",
      code.value(),
      code.mask,
      // code.shift,
      code.pos,
      code_point_index,
      code.masked(code_point_index),
      (code.pos & code_point_index),
      res,
      around);
}

TEST(Unicode, getCcc) {
    EXPECT_EQ(unicode::ccc_of(0xFC58), 0) << desc_ccc_of(0xFC58);
    EXPECT_EQ(unicode::ccc_of(0x10'FFFD), 0) << desc_ccc_of(0x10'FFFD);
    EXPECT_EQ(unicode::ccc_of(0x11'FFFD), 0) << desc_ccc_of(0x11'FFFD);
    EXPECT_EQ(unicode::ccc_of(0x0), 0) << desc_ccc_of(0x0);
    EXPECT_EQ(unicode::ccc_of(0x031D), 220) << desc_ccc_of(0x031D);
    EXPECT_EQ(unicode::ccc_of(0x0322), 202) << desc_ccc_of(0x0322);
    EXPECT_EQ(unicode::ccc_of(0x0300), 230) << desc_ccc_of(0x0300);
    EXPECT_EQ(unicode::ccc_of(0x0336), 1) << desc_ccc_of(0x0336);
    EXPECT_EQ(unicode::ccc_of(0x0360), 234) << desc_ccc_of(0x0360);
    EXPECT_EQ(unicode::ccc_of(0x05C2), 25) << desc_ccc_of(0x05C2);
    EXPECT_EQ(unicode::ccc_of(0x1CE8), 1) << desc_ccc_of(0x1CE8);
}

// NOLINTEND(*-magic-numbers)
