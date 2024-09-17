// Created by moisrex on 12/28/20.
#include "../webpp/unicode/unicode.hpp"

#include "../webpp/std/format.hpp"
#include "../webpp/unicode/normalization.hpp"
#include "common/tests_common_pch.hpp"

#include <filesystem>
#include <limits>
#include <string>

using webpp::fmt::format;
using webpp::stl::int64_t;
using webpp::stl::max;
using webpp::stl::min;
using webpp::stl::size_t;
using webpp::stl::string;
using webpp::stl::to_string;
using webpp::stl::u16string;
using webpp::stl::u32string;
using webpp::stl::u8string;
using webpp::unicode::canonical_decomposed;
using webpp::unicode::ccc_of;
using webpp::unicode::max_bmp;
using webpp::unicode::max_legal_utf32;
using webpp::unicode::prev_code_point;
using webpp::unicode::checked::append;
using webpp::unicode::unchecked::next_char;
using webpp::unicode::unchecked::next_char_copy;
using webpp::unicode::unchecked::prev_char_copy;
using webpp::unicode::unchecked::swap_code_points;

static constexpr bool enable_utf8_composition_tests = false;

// NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic)

TEST(Unicode, U8UnChecked) {
    char8_t const* str = u8"این یک تست است.";
    // have to use static_cast<int> because the google-test doesn't seem to understand char8_t
    EXPECT_EQ(static_cast<int>(u8"ی"[0]), static_cast<int>(*next_char_copy(str + 1)));
    EXPECT_EQ(static_cast<int>(u8"ن"[0]), static_cast<int>(*next_char_copy(str + 3)));
    EXPECT_EQ(static_cast<int>(u8"ی"[0]), static_cast<int>(*prev_char_copy(str + 3)));
    EXPECT_EQ(static_cast<int>(u8"ن"[0]), static_cast<int>(*prev_char_copy(str + 5)));
}

TEST(Unicode, U16UnChecked) {
    char16_t const* str = u"این یک تست است.";
    // have to use static_cast<int> because the google-test doesn't seem to understand char8_t
    EXPECT_EQ(static_cast<int>(u"ی"[0]), static_cast<int>(*next_char_copy(str)));
    EXPECT_EQ(static_cast<int>(u"ن"[0]), static_cast<int>(*next_char_copy(str + 1)));
    EXPECT_EQ(static_cast<int>(u"ی"[0]), static_cast<int>(*prev_char_copy(str + 5)));
    EXPECT_EQ(static_cast<int>(u"ن"[0]), static_cast<int>(*prev_char_copy(str + 3)));
}

TEST(Unicode, AppendBMPCodePoint) {
    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0041)); // 'A'
    ASSERT_EQ(utf16String, u"\u0041");
}

TEST(Unicode, AppendSupplementaryCodePoint) {
    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x1'F600)); // Grinning Face
    ASSERT_EQ(utf16String, u"\U0001F600");
}

TEST(Unicode, AppendMultipleCodePoints) {
    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0041));   // 'A'
    ASSERT_TRUE(append(utf16String, 0x1'F600)); // Grinning Face
    ASSERT_TRUE(append(utf16String, 0x0042));   // 'B'
    ASSERT_EQ(utf16String, u"\u0041\U0001F600\u0042");
}

TEST(Unicode, AppendMinBMPCodePoint) {
    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0000));
    ASSERT_EQ(utf16String.size(), 1);
    ASSERT_EQ(utf16String[0], 0);
}

TEST(Unicode, AppendMaxBMPCodePoint) {
    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0xFFFF));
    ASSERT_EQ(utf16String, u"\uFFFF");
}

TEST(Unicode, AppendMinSupplementaryCodePoint) {
    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x1'0000));
    ASSERT_EQ(utf16String, u"\U00010000");
}

TEST(Unicode, AppendMaxSupplementaryCodePoint) {
    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x10'FFFF));
    ASSERT_EQ(utf16String, u"\U0010FFFF");
}

TEST(Unicode, AppendInvalidCodePoint) {
    std::u16string utf16String;
    ASSERT_FALSE(append(utf16String, 0x11'0000)); // Invalid code point
    ASSERT_EQ(utf16String, u"");
}

TEST(Unicode, AppendAllowedBMPCodePoints) {
    std::u16string utf16String;
    for (char32_t codePoint = 0x0001; codePoint <= webpp::unicode::max_bmp<char32_t>; ++codePoint) {
        bool const res = append(utf16String, codePoint);
        if (webpp::unicode::is_surrogate(codePoint)) {
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
    std::u16string utf16String;
    for (char32_t codePoint = 0x1'0000; codePoint <= 0x10'FFFF; codePoint++) {
        EXPECT_TRUE(append(utf16String, codePoint));
        ASSERT_EQ(utf16String.size(), 2);
        utf16String.clear();
    }
}

TEST(Unicode, AppendAllowedAndInvalidCodePoints) {
    std::u16string utf16String;
    ASSERT_TRUE(append(utf16String, 0x0041));     // 'A'
    ASSERT_FALSE(append(utf16String, 0x11'0000)); // Invalid code point
    ASSERT_TRUE(append(utf16String, 0x1'F600));   // Grinning Face
    ASSERT_FALSE(append(utf16String, 0x11'0001)); // Invalid code point
    ASSERT_TRUE(append(utf16String, 0x0042));     // 'B'
    ASSERT_EQ(utf16String, u"\u0041\U0001F600\u0042");
}

TEST(Unicode, AppendMixOfCodePointTypes) {
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
    std::u16string utf16String;
    for (char32_t codePoint = max_bmp<char32_t> + 1; codePoint <= max_legal_utf32<char32_t>; codePoint++) {
        auto const prev_size = utf16String.size();
        bool const res       = append(utf16String, codePoint);
        if (webpp::unicode::is_surrogate(codePoint) || !webpp::unicode::is_code_point_valid(codePoint)) {
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
    std::u16string utf16String;
    ASSERT_FALSE(append(utf16String, 0xD800)); // Invalid leading surrogate
    ASSERT_FALSE(append(utf16String, 0xD801)); // Invalid leading surrogate
    ASSERT_FALSE(append(utf16String, 0xDC00)); // Invalid trailing surrogate
    ASSERT_FALSE(append(utf16String, 0xDC01)); // Invalid trailing surrogate
    ASSERT_EQ(utf16String, u"");
}

TEST(Unicode, AppendCodePointsWithInvalidCodePoint) {
    std::u16string utf16String;
    ASSERT_FALSE(append(utf16String, 0x11'0000)); // Invalid code point
    ASSERT_FALSE(append(utf16String, 0x11'0001)); // Invalid code point
    ASSERT_FALSE(append(utf16String, 0x1F'FFFF)); // Invalid code point
    ASSERT_EQ(utf16String, u"");
}

///////////////////////////////// Normalization /////////////////////////////////////


TEST(Unicode, BasicCodePointIterator) {
    std::u8string                       str = u8"تست";
    webpp::unicode::code_point_iterator iter{str.data()};
    iter.set_value(U'a');
    EXPECT_EQ(str, u8"a\xAAست");
}

TEST(Unicode, BasicCodePointIterator2) {
    std::u8string                       str = u8"تست";
    webpp::unicode::code_point_iterator iter{str.data()};
    ++iter;
    iter.set_value(U'a');
    EXPECT_EQ(str, u8"تa\xB3ت");
}

namespace {

    template <typename CharT = char32_t>
    string desc_ccc_of(CharT const code_point) {
        using webpp::unicode::details::ccc_index;
        using webpp::unicode::details::ccc_indices;
        using webpp::unicode::details::ccc_values;
        using webpp::unicode::details::trailing_zero_cccs;
        if (code_point >= static_cast<CharT>(trailing_zero_cccs)) [[unlikely]] {
            return "Definite Zero";
        }
        auto const code_point_range = static_cast<size_t>(code_point) >> ccc_index::chunk_shift;
        auto const remaining_pos    = static_cast<size_t>(code_point) & ccc_index::chunk_mask;
        // auto const        helper           = ccc_index[code_point_range];
        // auto const        mask             = static_cast<uint8_t>(helper);
        // auto const        shift            = static_cast<uint8_t>(helper >> 8);
        // auto const        index            = helper >> 16U;
        // auto const        masked_pos       = mask & code_point_index;
        // size_t const index_pos        = index + static_cast<size_t>(masked_pos);

        // auto const code_point_index = static_cast<uint8_t>(code_point);
        auto const code        = ccc_indices.at(code_point_range);
        // calculating the position of te value in the ccc_values table:
        size_t const index_pos = code.get_position(code_point);
        auto         res       = ccc_values.at(index_pos);

        string           around       = "[..., ";
        constexpr size_t details_span = 4ULL;
        for (
          size_t pos = static_cast<size_t>(max<int64_t>(static_cast<int64_t>(index_pos - details_span), 0LL));
          pos != min<size_t>(index_pos + details_span, ccc_values.size());
          ++pos)
        {
            if (pos == index_pos) {
                around += "[";
            }
            around += to_string(pos);
            around += "=";
            around += to_string(ccc_values.at(pos));
            if (pos == index_pos) {
                around += "]";
            }
            around += ", ";
        }
        around += "...]";
        return format(
          R"data(code: {}
index: {}
remaining pos: {}
actual pos: {} = {} + {}
result: {}
{}
)data",
          code.value(),
          // code.mask,
          code.pos,
          remaining_pos,
          // code.masked(remaining_pos),
          code.get_position(code_point),
          code.pos,
          remaining_pos,
          res,
          around);
    }

    template <typename CharT = char32_t>
    string desc_decomp_of(CharT const code_point) {
        using webpp::unicode::details::decomp_index;
        using webpp::unicode::details::decomp_indices;
        using webpp::unicode::details::decomp_values;
        using webpp::unicode::details::trailing_mapped_deomps;
        if (code_point >= static_cast<CharT>(trailing_mapped_deomps)) [[unlikely]] {
            return "Definite Zero";
        }
        auto const code_point_range = static_cast<size_t>(code_point) >> decomp_index::chunk_shift;
        auto const remaining_pos    = static_cast<size_t>(code_point) & decomp_index::chunk_mask;

        try {
            auto const   code      = decomp_indices.at(code_point_range);
            size_t const index_pos = code.get_position(code_point);
            auto         res       = decomp_values.at(index_pos);

            return format(
              R"data(code: {}
pos: {}
max length: {}
remaining pos: {}
actual pos: {} = {} + {}
result: {}
)data",
              code.value(),
              code.pos,
              static_cast<int>(code.max_length),
              remaining_pos,
              code.get_position(code_point),
              code.pos,
              remaining_pos,
              static_cast<int>(res));
        } catch (std::out_of_range const&) {
            return format("Out of range");
        }
    }
} // namespace

TEST(Unicode, getCcc) {
    using webpp::unicode::ccc_of;

    // clang-format off
    // Command to get samples:
    // awk 'BEGIN{FS=";"; OF=""} !/^\s*#/ { if ($4 != 0) { print "EXPECT_EQ(unicode::ccc_of(0x" $1 "), " $4 ") << desc_ccc_of(0x" $1 ");"; } }' UnicodeData.txt | sort -R | head
    // clang-format on
    EXPECT_EQ(ccc_of(0xFC58), 0) << desc_ccc_of(0xFC58);
    EXPECT_EQ(ccc_of(0x10'FFFD), 0) << desc_ccc_of(0x10'FFFD);
    EXPECT_EQ(ccc_of(0x11'FFFD), 0) << desc_ccc_of(0x11'FFFD);
    EXPECT_EQ(ccc_of(0x0), 0) << desc_ccc_of(0x0);
    EXPECT_EQ(ccc_of(0x031D), 220) << desc_ccc_of(0x031D);
    EXPECT_EQ(ccc_of(0x0322), 202) << desc_ccc_of(0x0322);
    EXPECT_EQ(ccc_of(0x0300), 230) << desc_ccc_of(0x0300);
    EXPECT_EQ(ccc_of(0x0336), 1) << desc_ccc_of(0x0336);
    EXPECT_EQ(ccc_of(0x0360), 234) << desc_ccc_of(0x0360);
    EXPECT_EQ(ccc_of(0x05C2), 25) << desc_ccc_of(0x05C2);
    EXPECT_EQ(ccc_of(0x1CE8), 1) << desc_ccc_of(0x1CE8);

    EXPECT_EQ(ccc_of(0x08D8), 230) << desc_ccc_of(0x08D8);
    EXPECT_EQ(ccc_of(0x1'11C0), 9) << desc_ccc_of(0x1'11C0);
    EXPECT_EQ(ccc_of(0x1'0A3F), 9) << desc_ccc_of(0x1'0A3F);
    EXPECT_EQ(ccc_of(0x081E), 230) << desc_ccc_of(0x081E);
    EXPECT_EQ(ccc_of(0x193B), 220) << desc_ccc_of(0x193B);
    EXPECT_EQ(ccc_of(0x034C), 230) << desc_ccc_of(0x034C);
    EXPECT_EQ(ccc_of(0x06DB), 230) << desc_ccc_of(0x06DB);
    EXPECT_EQ(ccc_of(0x0325), 220) << desc_ccc_of(0x0325);
    EXPECT_EQ(ccc_of(0x09BC), 7) << desc_ccc_of(0x09BC);
    EXPECT_EQ(ccc_of(0x2CF0), 230) << desc_ccc_of(0x2CF0);

    EXPECT_EQ(ccc_of(0x1'E8D4), 220) << desc_ccc_of(0x1'E8D4);
    EXPECT_EQ(ccc_of(0x1'E8D5), 220) << desc_ccc_of(0x1'E8D5);
    EXPECT_EQ(ccc_of(0x1'E8D6), 220) << desc_ccc_of(0x1'E8D6);
    EXPECT_EQ(ccc_of(0x1'E944), 230) << desc_ccc_of(0x1'E944);
    EXPECT_EQ(ccc_of(0x1'E945), 230) << desc_ccc_of(0x1'E945);
    EXPECT_EQ(ccc_of(0x1'E946), 230) << desc_ccc_of(0x1'E946);
    EXPECT_EQ(ccc_of(0x1'E947), 230) << desc_ccc_of(0x1'E947);
    EXPECT_EQ(ccc_of(0x1'E948), 230) << desc_ccc_of(0x1'E948);
    EXPECT_EQ(ccc_of(0x1'E949), 230) << desc_ccc_of(0x1'E949);
    EXPECT_EQ(ccc_of(0x1'E94A), 7) << desc_ccc_of(0x1'E94A);

    EXPECT_EQ(ccc_of(0xE'01EA), 0) << desc_ccc_of(0xE'01EA);
    EXPECT_EQ(ccc_of(0xE'01EB), 0) << desc_ccc_of(0xE'01EB);
    EXPECT_EQ(ccc_of(0xE'01EC), 0) << desc_ccc_of(0xE'01EC);
    EXPECT_EQ(ccc_of(0xE'01ED), 0) << desc_ccc_of(0xE'01ED);
    EXPECT_EQ(ccc_of(0xE'01EE), 0) << desc_ccc_of(0xE'01EE);
    EXPECT_EQ(ccc_of(0xE'01EF), 0) << desc_ccc_of(0xE'01EF);
    EXPECT_EQ(ccc_of(0xF'0000), 0) << desc_ccc_of(0xF'0000);
    EXPECT_EQ(ccc_of(0xF'FFFD), 0) << desc_ccc_of(0xF'FFFD);
    EXPECT_EQ(ccc_of(0x10'0000), 0) << desc_ccc_of(0x10'0000);
    EXPECT_EQ(ccc_of(0x10'FFFD), 0) << desc_ccc_of(0x10'FFFD);

    EXPECT_EQ(ccc_of(0x0000), 0) << desc_ccc_of(0x0000);
    EXPECT_EQ(ccc_of(0x0001), 0) << desc_ccc_of(0x0001);
    EXPECT_EQ(ccc_of(0x0002), 0) << desc_ccc_of(0x0002);
    EXPECT_EQ(ccc_of(0x0003), 0) << desc_ccc_of(0x0003);
    EXPECT_EQ(ccc_of(0x0004), 0) << desc_ccc_of(0x0004);
    EXPECT_EQ(ccc_of(0x0005), 0) << desc_ccc_of(0x0005);
    EXPECT_EQ(ccc_of(0x0006), 0) << desc_ccc_of(0x0006);
    EXPECT_EQ(ccc_of(0x0007), 0) << desc_ccc_of(0x0007);
    EXPECT_EQ(ccc_of(0x0008), 0) << desc_ccc_of(0x0008);
    EXPECT_EQ(ccc_of(0x0009), 0) << desc_ccc_of(0x0009);

    EXPECT_EQ(ccc_of(0x2DFC), 230) << desc_ccc_of(0x2DFC);
    EXPECT_EQ(ccc_of(0x05AA), 220) << desc_ccc_of(0x05AA);
    EXPECT_EQ(ccc_of(0x1BF3), 9) << desc_ccc_of(0x1BF3);
    EXPECT_EQ(ccc_of(0x0485), 230) << desc_ccc_of(0x0485);
    EXPECT_EQ(ccc_of(0x1'D171), 216) << desc_ccc_of(0x1'D171);
    EXPECT_EQ(ccc_of(0x1'14C2), 9) << desc_ccc_of(0x1'14C2);
    EXPECT_EQ(ccc_of(0x1'E945), 230) << desc_ccc_of(0x1'E945);
    EXPECT_EQ(ccc_of(0x065A), 230) << desc_ccc_of(0x065A);
    EXPECT_EQ(ccc_of(0x0615), 230) << desc_ccc_of(0x0615);
    EXPECT_EQ(ccc_of(0x1'E011), 230) << desc_ccc_of(0x1'E011);

    EXPECT_EQ(ccc_of(0x1'E00A), 230) << desc_ccc_of(0x1'E00A);
    EXPECT_EQ(ccc_of(0x081C), 230) << desc_ccc_of(0x081C);
    EXPECT_EQ(ccc_of(0x1'133B), 7) << desc_ccc_of(0x1'133B);
    EXPECT_EQ(ccc_of(0x1'D243), 230) << desc_ccc_of(0x1'D243);
    EXPECT_EQ(ccc_of(0x1DF8), 228) << desc_ccc_of(0x1DF8);
    EXPECT_EQ(ccc_of(0x08D2), 220) << desc_ccc_of(0x08D2);
    EXPECT_EQ(ccc_of(0x073F), 230) << desc_ccc_of(0x073F);
    EXPECT_EQ(ccc_of(0x2CF1), 230) << desc_ccc_of(0x2CF1);
    EXPECT_EQ(ccc_of(0x08D6), 230) << desc_ccc_of(0x08D6);
    EXPECT_EQ(ccc_of(0x0328), 202) << desc_ccc_of(0x0328);
}

namespace old_impl {
    using webpp::istl::Appendable;
    using webpp::istl::appendable_value_type_t;
    using webpp::istl::iter_append;
    using webpp::istl::NothrowAppendable;
    using webpp::istl::size_type_of_t;
    using webpp::stl::integral;
    using webpp::stl::make_unsigned_t;
    using webpp::unicode::max_bmp;
    using webpp::unicode::UTF16;
    using webpp::unicode::UTF32;
    using webpp::unicode::UTF8;

    namespace {
        /**
         * Append a code point to a string
         * "out" can be an iterator/pointer or a string
         */
        template <Appendable StrT, integral SizeT = size_type_of_t<StrT>, typename CharT = char32_t>
        SizeT append(StrT& out, CharT code_point) noexcept(NothrowAppendable<StrT>) {
            using char_type = appendable_value_type_t<StrT>;
            using uchar_t   = make_unsigned_t<CharT>;
            auto const ccp  = static_cast<uint32_t>(code_point);
            if constexpr (UTF8<char_type>) {
                if (ccp < 0x80U) { // one octet
                    iter_append(out, ccp);
                    return 1U;
                }
                if (ccp < 0x800) {                                   // two octets
                    iter_append(out, (ccp >> 6U) | 0xC0U);           // 0b110,'....
                    iter_append(out, (ccp & 0x3FU) | 0x80U);         // 0b10..'....
                    return 2U;
                }
                if (ccp < 0x1'0000U) {                               // three octets
                    iter_append(out, (ccp >> 12U) | 0xE0U);          // 0b1110'....
                    iter_append(out, ((ccp >> 6U) & 0x3FU) | 0x80U); // 0b10..'....
                    iter_append(out, (ccp & 0x3FU) | 0x80U);         // 0b10..'....
                    return 3U;
                }
                // four octets
                iter_append(out, (ccp >> 18U) | 0xF0U);           // 0b1111'0...
                iter_append(out, ((ccp >> 12U) & 0x3FU) | 0x80U); // 0b10..'....
                iter_append(out, ((ccp >> 6U) & 0x3FU) | 0x80U);  // 0b10..'....
                iter_append(out, (ccp & 0x3FU) | 0x80U);          // 0b10..'....
                return 4U;
            } else if constexpr (UTF16<char_type>) {
                if (ccp <= max_bmp<char_type>) {
                    iter_append(out, ccp); // normal case
                    return 1U;
                }
                iter_append(out, 0xD7C0U + (static_cast<uchar_t>(ccp) >> 10U));
                iter_append(out, 0xDC00U + (static_cast<uchar_t>(ccp) & 0x3FFU));
                return 2U;
            } else { // for char32_t or others
                iter_append(out, ccp);
                return 1U;
            }
        }
    } // namespace
} // namespace old_impl

namespace {
    std::u8string utf32_to_utf8(std::u32string const& utf32_str) {
        std::u8string utf8_str;
        utf8_str.reserve(utf32_str.length() * 4); // Estimate maximum size of UTF-8 string

        std::u8string test_str;
        for (char32_t const code_point : utf32_str) {
            old_impl::append(test_str, code_point);
            if (!webpp::unicode::checked::append(utf8_str, code_point)) {
                throw webpp::stl::invalid_argument("Invalid code point");
            }

            EXPECT_EQ(utf8_str, test_str);
        }

        return utf8_str;
    }

    constexpr char32_t utf8_to_utf32(std::u8string_view const input) {
        char32_t codepoint = 0;

        if (!input.empty() && (0b1000'0000U & input[0]) == 0b0000'0000U) {
            codepoint = static_cast<unsigned char>(input[0]);
        } else if (input.size() > 1 && (0b1110'0000U & input[0]) == 0b1100'0000U) {
            codepoint = static_cast<char32_t>(((0b0001'1111U & input[0]) << 6U) | (input[1] & 0b0011'1111U));
        } else if (input.size() > 2 && (0b1111'0000U & input[0]) == 0b1110'0000U) {
            codepoint = static_cast<char32_t>(((0b0000'1111U & input[0]) << 12U) |
                                              ((input[1] & 0b0011'1111U) << 6U) | (input[2] & 0b0011'1111U));
        } else if (input.size() > 3 && (0b1111'1000U & input[0]) == 0b1111'0000U) {
            codepoint = static_cast<char32_t>(
              ((input[0] & 0b0000'0111U) << 18U) | ((0b0011'1111U & input[1]) << 12U) |
              ((input[2] & 0b0011'1111U) << 6U) | (0b0011'1111U & input[3]));
        }

        return codepoint;
    }

    std::u32string utf8_to_utf32(std::u8string const& utf8_str) {
        std::u32string utf32_str;
        utf32_str.reserve(utf32_str.length() * 4); // Estimate maximum size of UTF-8 string

        for (auto pos = utf8_str.begin(); pos != utf8_str.end();) {
            auto const impl_copy = webpp::unicode::next_code_point_copy(pos, utf8_str.end());
            auto const impl2     = utf8_to_utf32(webpp::stl::u8string_view{pos, utf8_str.end()});
            webpp::unicode::unchecked::append(utf32_str, pos);

            EXPECT_EQ(utf32_str.back(), impl2);
            EXPECT_EQ(impl_copy, impl2);
        }

        return utf32_str;
    }
} // namespace

// Use this command to get the decomposed and its mapped values:
// awk 'BEGIN{FS=";"; OF=""} !/^\s*#/{gsub(/<[^>]*>/, "", $6); if($6 != "") print $1 ": " $6}' UnicodeData.txt
TEST(Unicode, CanonicalDecompose) {
    using webpp::unicode::canonical_decomposed;
    using webpp::unicode::toNFD;
    // clang-format off
    // Get more examples with these commands:
    //  All decompositions:
    //   awk 'BEGIN{FS=";"; OF=""} !/^\s*#/ {gsub(/<[^>]*>/, "", $6); $6 = " " $6; gsub(/\s+/, " ", $6); if ($6 != " ") { gsub(/\s+/, "\\\x", $6); print "EXPECT_EQ(canonical_decomposed<u8string>(U\'\\\x" $1 "\'), utf32_to_utf8(U\"" $6 "\")) << desc_decomp_of(U\'\\\x" $1 "\');"; } }' UnicodeData.txt | sort -R | head
    //  Canonical-only decompositions:
    //   awk 'BEGIN{FS=";"; OF=";"} !/^\s*#|</ {tst = " " $6; gsub(/\s+/, " ", tst); if (tst != " ") { gsub(/\s+/, "\\\x", tst); print "    EXPECT_EQ(canonical_decomposed<u8string>(U\'\\\x" $1 "\'), utf32_to_utf8(U\"" tst "\")) << desc_decomp_of(U\'\\\x" $1 "\') << \"\\\n  Line: " $0 "\";"; } }' UnicodeData.txt | sort -R | head | c
    // clang-format on

    static_assert(webpp::stl::same_as<char8_t, webpp::istl::appendable_value_type_t<u8string*>>,
                  "invalid value type");

    // special
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1f133'), U"D");
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xffc4'), U"\x3151");
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xD590'), utf32_to_utf8(U"\x1112\x1163"))
      << desc_decomp_of(U'\xD590');
    EXPECT_EQ(toNFD(u32string{U'\xD590'}), u32string{U"\x1112\x1163"}) << desc_decomp_of(U'\xD590');

    // start
    EXPECT_EQ(canonical_decomposed<u8string>(U'\0'), utf32_to_utf8(u32string{U"\0", 1}))
      << desc_decomp_of(U'\0');
    EXPECT_EQ(canonical_decomposed<u8string>(U'\1'), utf32_to_utf8(U"\1")) << desc_decomp_of(U'\1');

    // Canonical Decomposition start:
    // clang-format off
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C0'), utf32_to_utf8(U"\x0041\x0300")) << desc_decomp_of(U'\x00C0') << "\n  Line: 00C0;LATIN CAPITAL LETTER A WITH GRAVE;Lu;0;L;0041 0300;;;;N;LATIN CAPITAL LETTER A GRAVE;;;00E0;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C1'), utf32_to_utf8(U"\x0041\x0301")) << desc_decomp_of(U'\x00C1') << "\n  Line: 00C1;LATIN CAPITAL LETTER A WITH ACUTE;Lu;0;L;0041 0301;;;;N;LATIN CAPITAL LETTER A ACUTE;;;00E1;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C2'), utf32_to_utf8(U"\x0041\x0302")) << desc_decomp_of(U'\x00C2') << "\n  Line: 00C2;LATIN CAPITAL LETTER A WITH CIRCUMFLEX;Lu;0;L;0041 0302;;;;N;LATIN CAPITAL LETTER A CIRCUMFLEX;;;00E2;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C3'), utf32_to_utf8(U"\x0041\x0303")) << desc_decomp_of(U'\x00C3') << "\n  Line: 00C3;LATIN CAPITAL LETTER A WITH TILDE;Lu;0;L;0041 0303;;;;N;LATIN CAPITAL LETTER A TILDE;;;00E3;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C4'), utf32_to_utf8(U"\x0041\x0308")) << desc_decomp_of(U'\x00C4') << "\n  Line: 00C4;LATIN CAPITAL LETTER A WITH DIAERESIS;Lu;0;L;0041 0308;;;;N;LATIN CAPITAL LETTER A DIAERESIS;;;00E4;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C5'), utf32_to_utf8(U"\x0041\x030A")) << desc_decomp_of(U'\x00C5') << "\n  Line: 00C5;LATIN CAPITAL LETTER A WITH RING ABOVE;Lu;0;L;0041 030A;;;;N;LATIN CAPITAL LETTER A RING;;;00E5;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C7'), utf32_to_utf8(U"\x0043\x0327")) << desc_decomp_of(U'\x00C7') << "\n  Line: 00C7;LATIN CAPITAL LETTER C WITH CEDILLA;Lu;0;L;0043 0327;;;;N;LATIN CAPITAL LETTER C CEDILLA;;;00E7;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C8'), utf32_to_utf8(U"\x0045\x0300")) << desc_decomp_of(U'\x00C8') << "\n  Line: 00C8;LATIN CAPITAL LETTER E WITH GRAVE;Lu;0;L;0045 0300;;;;N;LATIN CAPITAL LETTER E GRAVE;;;00E8;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00C9'), utf32_to_utf8(U"\x0045\x0301")) << desc_decomp_of(U'\x00C9') << "\n  Line: 00C9;LATIN CAPITAL LETTER E WITH ACUTE;Lu;0;L;0045 0301;;;;N;LATIN CAPITAL LETTER E ACUTE;;;00E9;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00CA'), utf32_to_utf8(U"\x0045\x0302")) << desc_decomp_of(U'\x00CA') << "\n  Line: 00CA;LATIN CAPITAL LETTER E WITH CIRCUMFLEX;Lu;0;L;0045 0302;;;;N;LATIN CAPITAL LETTER E CIRCUMFLEX;;;00EA;";


    // Canonical Decomposition end:
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA14'), utf32_to_utf8(U"\x2A291")) << desc_decomp_of(U'\x2FA14') << "\n  Line: 2FA14;CJK COMPATIBILITY IDEOGRAPH-2FA14;Lo;0;L;2A291;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA15'), utf32_to_utf8(U"\x9EBB")) << desc_decomp_of(U'\x2FA15') << "\n  Line: 2FA15;CJK COMPATIBILITY IDEOGRAPH-2FA15;Lo;0;L;9EBB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA16'), utf32_to_utf8(U"\x4D56")) << desc_decomp_of(U'\x2FA16') << "\n  Line: 2FA16;CJK COMPATIBILITY IDEOGRAPH-2FA16;Lo;0;L;4D56;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA17'), utf32_to_utf8(U"\x9EF9")) << desc_decomp_of(U'\x2FA17') << "\n  Line: 2FA17;CJK COMPATIBILITY IDEOGRAPH-2FA17;Lo;0;L;9EF9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA18'), utf32_to_utf8(U"\x9EFE")) << desc_decomp_of(U'\x2FA18') << "\n  Line: 2FA18;CJK COMPATIBILITY IDEOGRAPH-2FA18;Lo;0;L;9EFE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA19'), utf32_to_utf8(U"\x9F05")) << desc_decomp_of(U'\x2FA19') << "\n  Line: 2FA19;CJK COMPATIBILITY IDEOGRAPH-2FA19;Lo;0;L;9F05;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA1A'), utf32_to_utf8(U"\x9F0F")) << desc_decomp_of(U'\x2FA1A') << "\n  Line: 2FA1A;CJK COMPATIBILITY IDEOGRAPH-2FA1A;Lo;0;L;9F0F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA1B'), utf32_to_utf8(U"\x9F16")) << desc_decomp_of(U'\x2FA1B') << "\n  Line: 2FA1B;CJK COMPATIBILITY IDEOGRAPH-2FA1B;Lo;0;L;9F16;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA1C'), utf32_to_utf8(U"\x9F3B")) << desc_decomp_of(U'\x2FA1C') << "\n  Line: 2FA1C;CJK COMPATIBILITY IDEOGRAPH-2FA1C;Lo;0;L;9F3B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA1D'), utf32_to_utf8(U"\x2A600")) << desc_decomp_of(U'\x2FA1D') << "\n  Line: 2FA1D;CJK COMPATIBILITY IDEOGRAPH-2FA1D;Lo;0;L;2A600;;;;N;;;;;";


    // Random Canonical Decompositions:
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F96E'), utf32_to_utf8(U"\x7DC7")) << desc_decomp_of(U'\x2F96E') << "\n  Line: 2F96E;CJK COMPATIBILITY IDEOGRAPH-2F96E;Lo;0;L;7DC7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x1FD3'), utf32_to_utf8(U"\x0390")) << desc_decomp_of(U'\x1FD3') << "\n  Line: 1FD3;GREEK SMALL LETTER IOTA WITH DIALYTIKA AND OXIA;Ll;0;L;0390;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x04E5'), utf32_to_utf8(U"\x0438\x0308")) << desc_decomp_of(U'\x04E5') << "\n  Line: 04E5;CYRILLIC SMALL LETTER I WITH DIAERESIS;Ll;0;L;0438 0308;;;;N;;;04E4;;04E4";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x04F0'), utf32_to_utf8(U"\x0423\x0308")) << desc_decomp_of(U'\x04F0') << "\n  Line: 04F0;CYRILLIC CAPITAL LETTER U WITH DIAERESIS;Lu;0;L;0423 0308;;;;N;;;;04F1;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F8A7'), utf32_to_utf8(U"\x614C")) << desc_decomp_of(U'\x2F8A7') << "\n  Line: 2F8A7;CJK COMPATIBILITY IDEOGRAPH-2F8A7;Lo;0;L;614C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x0B4B'), utf32_to_utf8(U"\x0B47\x0B3E")) << desc_decomp_of(U'\x0B4B') << "\n  Line: 0B4B;ORIYA VOWEL SIGN O;Mc;0;L;0B47 0B3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F809'), utf32_to_utf8(U"\x5099")) << desc_decomp_of(U'\x2F809') << "\n  Line: 2F809;CJK COMPATIBILITY IDEOGRAPH-2F809;Lo;0;L;5099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F9DF'), utf32_to_utf8(U"\x8F38")) << desc_decomp_of(U'\x2F9DF') << "\n  Line: 2F9DF;CJK COMPATIBILITY IDEOGRAPH-2F9DF;Lo;0;L;8F38;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x095A'), utf32_to_utf8(U"\x0917\x093C")) << desc_decomp_of(U'\x095A') << "\n  Line: 095A;DEVANAGARI LETTER GHHA;Lo;0;L;0917 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F969'), utf32_to_utf8(U"\x7CE3")) << desc_decomp_of(U'\x2F969') << "\n  Line: 2F969;CJK COMPATIBILITY IDEOGRAPH-2F969;Lo;0;L;7CE3;;;;N;;;;;";

    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F8F6'), utf32_to_utf8(U"\x6BBB")) << desc_decomp_of(U'\x2F8F6') << "\n  Line: 2F8F6;CJK COMPATIBILITY IDEOGRAPH-2F8F6;Lo;0;L;6BBB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xF92F'), utf32_to_utf8(U"\x52DE")) << desc_decomp_of(U'\xF92F') << "\n  Line: F92F;CJK COMPATIBILITY IDEOGRAPH-F92F;Lo;0;L;52DE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x1E60'), utf32_to_utf8(U"\x0053\x0307")) << desc_decomp_of(U'\x1E60') << "\n  Line: 1E60;LATIN CAPITAL LETTER S WITH DOT ABOVE;Lu;0;L;0053 0307;;;;N;;;;1E61;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x014F'), utf32_to_utf8(U"\x006F\x0306")) << desc_decomp_of(U'\x014F') << "\n  Line: 014F;LATIN SMALL LETTER O WITH BREVE;Ll;0;L;006F 0306;;;;N;LATIN SMALL LETTER O BREVE;;014E;;014E";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F8C5'), utf32_to_utf8(U"\x649D")) << desc_decomp_of(U'\x2F8C5') << "\n  Line: 2F8C5;CJK COMPATIBILITY IDEOGRAPH-2F8C5;Lo;0;L;649D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x1D161'), utf32_to_utf8(U"\x1D15F\x1D16F")) << desc_decomp_of(U'\x1D161') << "\n  Line: 1D161;MUSICAL SYMBOL SIXTEENTH NOTE;So;0;L;1D15F 1D16F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xFA94'), utf32_to_utf8(U"\x6756")) << desc_decomp_of(U'\xFA94') << "\n  Line: FA94;CJK COMPATIBILITY IDEOGRAPH-FA94;Lo;0;L;6756;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x01CD'), utf32_to_utf8(U"\x0041\x030C")) << desc_decomp_of(U'\x01CD') << "\n  Line: 01CD;LATIN CAPITAL LETTER A WITH CARON;Lu;0;L;0041 030C;;;;N;LATIN CAPITAL LETTER A HACEK;;;01CE;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2FA10'), utf32_to_utf8(U"\x2A0CE")) << desc_decomp_of(U'\x2FA10') << "\n  Line: 2FA10;CJK COMPATIBILITY IDEOGRAPH-2FA10;Lo;0;L;2A0CE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x06C2'), utf32_to_utf8(U"\x06C1\x0654")) << desc_decomp_of(U'\x06C2') << "\n  Line: 06C2;ARABIC LETTER HEH GOAL WITH HAMZA ABOVE;Lo;0;AL;06C1 0654;;;;N;ARABIC LETTER HAMZAH ON HA GOAL;;;;";

    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2226'), utf32_to_utf8(U"\x2225\x0338")) << desc_decomp_of(U'\x2226') << "\n  Line: 2226;NOT PARALLEL TO;Sm;0;ON;2225 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x0F5C'), utf32_to_utf8(U"\x0F5B\x0FB7")) << desc_decomp_of(U'\x0F5C') << "\n  Line: 0F5C;TIBETAN LETTER DZHA;Lo;0;L;0F5B 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xF919'), utf32_to_utf8(U"\x916A")) << desc_decomp_of(U'\xF919') << "\n  Line: F919;CJK COMPATIBILITY IDEOGRAPH-F919;Lo;0;L;916A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x021B'), utf32_to_utf8(U"\x0074\x0326")) << desc_decomp_of(U'\x021B') << "\n  Line: 021B;LATIN SMALL LETTER T WITH COMMA BELOW;Ll;0;L;0074 0326;;;;N;;;021A;;021A";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xF923'), utf32_to_utf8(U"\x85CD")) << desc_decomp_of(U'\xF923') << "\n  Line: F923;CJK COMPATIBILITY IDEOGRAPH-F923;Lo;0;L;85CD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x11938'), utf32_to_utf8(U"\x11935\x11930")) << desc_decomp_of(U'\x11938') << "\n  Line: 11938;DIVES AKURU VOWEL SIGN O;Mc;0;L;11935 11930;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x1EE5'), utf32_to_utf8(U"\x0075\x0323")) << desc_decomp_of(U'\x1EE5') << "\n  Line: 1EE5;LATIN SMALL LETTER U WITH DOT BELOW;Ll;0;L;0075 0323;;;;N;;;1EE4;;1EE4";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x1FE2'), utf32_to_utf8(U"\x03CB\x0300")) << desc_decomp_of(U'\x1FE2') << "\n  Line: 1FE2;GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA;Ll;0;L;03CB 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xF902'), utf32_to_utf8(U"\x8ECA")) << desc_decomp_of(U'\xF902') << "\n  Line: F902;CJK COMPATIBILITY IDEOGRAPH-F902;Lo;0;L;8ECA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x1E77'), utf32_to_utf8(U"\x0075\x032D")) << desc_decomp_of(U'\x1E77') << "\n  Line: 1E77;LATIN SMALL LETTER U WITH CIRCUMFLEX BELOW;Ll;0;L;0075 032D;;;;N;;;1E76;;1E76";

    EXPECT_EQ(canonical_decomposed<u8string>(U'\x22E1'), utf32_to_utf8(U"\x227D\x0338")) << desc_decomp_of(U'\x22E1') << "\n  Line: 22E1;DOES NOT SUCCEED OR EQUAL;Sm;0;ON;227D 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x045D'), utf32_to_utf8(U"\x0438\x0300")) << desc_decomp_of(U'\x045D') << "\n  Line: 045D;CYRILLIC SMALL LETTER I WITH GRAVE;Ll;0;L;0438 0300;;;;N;;;040D;;040D";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x30B2'), utf32_to_utf8(U"\x30B1\x3099")) << desc_decomp_of(U'\x30B2') << "\n  Line: 30B2;KATAKANA LETTER GE;Lo;0;L;30B1 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x1F71'), utf32_to_utf8(U"\x03AC")) << desc_decomp_of(U'\x1F71') << "\n  Line: 1F71;GREEK SMALL LETTER ALPHA WITH OXIA;Ll;0;L;03AC;;;;N;;;1FBB;;1FBB";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F903'), utf32_to_utf8(U"\x6D69")) << desc_decomp_of(U'\x2F903') << "\n  Line: 2F903;CJK COMPATIBILITY IDEOGRAPH-2F903;Lo;0;L;6D69;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x00F1'), utf32_to_utf8(U"\x006E\x0303")) << desc_decomp_of(U'\x00F1') << "\n  Line: 00F1;LATIN SMALL LETTER N WITH TILDE;Ll;0;L;006E 0303;;;;N;LATIN SMALL LETTER N TILDE;;00D1;;00D1";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xFA91'), utf32_to_utf8(U"\x6674")) << desc_decomp_of(U'\xFA91') << "\n  Line: FA91;CJK COMPATIBILITY IDEOGRAPH-FA91;Lo;0;L;6674;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F89F'), utf32_to_utf8(U"\x5FF9")) << desc_decomp_of(U'\x2F89F') << "\n  Line: 2F89F;CJK COMPATIBILITY IDEOGRAPH-2F89F;Lo;0;L;5FF9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xF9C7'), utf32_to_utf8(U"\x5289")) << desc_decomp_of(U'\xF9C7') << "\n  Line: F9C7;CJK COMPATIBILITY IDEOGRAPH-F9C7;Lo;0;L;5289;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2F8FD'), utf32_to_utf8(U"\x6CCD")) << desc_decomp_of(U'\x2F8FD') << "\n  Line: 2F8FD;CJK COMPATIBILITY IDEOGRAPH-2F8FD;Lo;0;L;6CCD;;;;N;;;;;";

    // clang-format on
}

TEST(Unicode, DecomposeInplace) {
    auto const test_decomp = [](u32string const& inp_str, u32string const& inp_res) {
        auto str = inp_str;
        webpp::unicode::canonical_decompose(str);
        EXPECT_EQ(str, inp_res);

        auto str8 = utf32_to_utf8(inp_str);
        webpp::unicode::canonical_decompose(str8);
        EXPECT_EQ(str8, utf32_to_utf8(inp_res));
    };

    test_decomp(u32string{U"\0", 1}, u32string{U"\0", 1});
    test_decomp(U"\1", U"\1");
    test_decomp(U"\x009F", U"\x009F");
    test_decomp(U"\x00A0", U"\x0020");
    test_decomp(U"\x00A8", U"\x0020\x0308");
    test_decomp(U"\x00AA", U"\x0061");
    test_decomp(U"\x00AF", U"\x0020\x0304");
    test_decomp(U"\x00B2", U"\x0032");
    test_decomp(U"\x00B3", U"\x0033");
    test_decomp(U"\x00B4", U"\x0020\x0301");
    test_decomp(U"\x00B5", U"\x03BC");
    test_decomp(U"\x00B8", U"\x0020\x0327");
    test_decomp(U"\x00B9", U"\x0031");

    test_decomp(U"\xFFC5", U"\x3152");
    test_decomp(U"\x03AD", U"\x03B5\x0301");
    test_decomp(U"\x1D731", U"\x03A6");
    test_decomp(U"\xFCF9", U"\x063A\x0649");
    test_decomp(U"\x32C3", U"\x0034\x6708");
    test_decomp(U"\x1FE1", U"\x03C5\x0304");
    test_decomp(U"\x1D53", U"\x0254");
    test_decomp(U"\xFC49", U"\x0645\x0649");
    test_decomp(U"\x1E067", U"\x0491");
    test_decomp(U"\xFD3A", U"\x0637\x0645");

    test_decomp(U"\x3160", U"\x1172");
    test_decomp(U"\x2FCB", U"\x9EF9");
    test_decomp(U"\x33E4", U"\x0035\x65E5");
    test_decomp(U"\x1F62", U"\x1F60\x0300");
    test_decomp(U"\xFFB5", U"\x3145");
    test_decomp(U"\x0453", U"\x0433\x0301");
    test_decomp(U"\x305A", U"\x3059\x3099");
    test_decomp(U"\xFFBA", U"\x314A");
    test_decomp(U"\x1D5CE", U"\x0075");
    test_decomp(U"\x0FA2", U"\x0FA1\x0FB7");
}

TEST(Unicode, DecomposeUTF32) {
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E0A'), U"\x44\x307") << desc_decomp_of(U'\0');

    EXPECT_EQ(utf8_to_utf32(utf32_to_utf8(U"\x29496")), U"\x29496");
    EXPECT_EQ(utf8_to_utf32(utf32_to_utf8(U"\x308")), U"\x308");

    EXPECT_EQ(canonical_decomposed<u32string>(U'\0'), (u32string{U"\0", 1})) << desc_decomp_of(U'\0');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\1'), (U"\1")) << desc_decomp_of(U'\1');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x009F'), (U"\x009F")) << desc_decomp_of(U'\x009F');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00A0'), (U"\x0020")) << desc_decomp_of(U'\x00A0');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00A8'), (U"\x0020\x0308")) << desc_decomp_of(U'\x00A8');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00AA'), (U"\x0061")) << desc_decomp_of(U'\x00AA');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00AF'), (U"\x0020\x0304")) << desc_decomp_of(U'\x00AF');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00B2'), (U"\x0032")) << desc_decomp_of(U'\x00B2');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00B3'), (U"\x0033")) << desc_decomp_of(U'\x00B3');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00B4'), (U"\x0020\x0301")) << desc_decomp_of(U'\x00B4');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00B5'), (U"\x03BC")) << desc_decomp_of(U'\x00B5');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00B8'), (U"\x0020\x0327")) << desc_decomp_of(U'\x00B8');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00B9'), (U"\x0031")) << desc_decomp_of(U'\x00B9');


    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA16'), (U"\x4D56")) << desc_decomp_of(U'\x2FA16');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3359'), (U"\x0031\x70B9")) << desc_decomp_of(U'\x3359');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B1'), (U"\x9234")) << desc_decomp_of(U'\xF9B1');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EEAB'), (U"\x0644")) << desc_decomp_of(U'\x1EEAB');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9FD'), (U"\x29496")) << desc_decomp_of(U'\x2F9FD');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFCA7'), (U"\x062C\x062D")) << desc_decomp_of(U'\xFCA7');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE09'), (U"\x064A")) << desc_decomp_of(U'\x1EE09');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F2'), (U"\x9C57")) << desc_decomp_of(U'\xF9F2');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D41E'), (U"\x0065")) << desc_decomp_of(U'\x1D41E');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E06D'), (U"\x04B1")) << desc_decomp_of(U'\x1E06D');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x249E'), (U"\x0028\x0063\x0029"))
      << desc_decomp_of(U'\x249E');

    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE3B'), (U"\x063A")) << desc_decomp_of(U'\x1EE3B');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8FE'), (U"\x6C67")) << desc_decomp_of(U'\x2F8FE');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0130'), (U"\x0049\x0307")) << desc_decomp_of(U'\x0130');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01DC'), (U"\x00FC\x0300")) << desc_decomp_of(U'\x01DC');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFF8B'), (U"\x30D2")) << desc_decomp_of(U'\xFF8B');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x32D1'), (U"\x30A4")) << desc_decomp_of(U'\x32D1');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3193'), (U"\x4E8C")) << desc_decomp_of(U'\x3193');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x02B8'), (U"\x0079")) << desc_decomp_of(U'\x02B8');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2076'), (U"\x0036")) << desc_decomp_of(U'\x2076');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D4D8'), (U"\x0049")) << desc_decomp_of(U'\x1D4D8');

    EXPECT_EQ(canonical_decomposed<u32string>(U'\x216C'), (U"\x004C")) << desc_decomp_of(U'\x216C');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFE94'), (U"\x0629")) << desc_decomp_of(U'\xFE94');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFCE9'), (U"\x0634\x0645")) << desc_decomp_of(U'\xFCE9');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F9D'), (U"\x0F9C\x0FB7")) << desc_decomp_of(U'\x0F9D');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x038C'), (U"\x039F\x0301")) << desc_decomp_of(U'\x038C');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F77'), (U"\x0FB2\x0F81")) << desc_decomp_of(U'\x0F77');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFC0A'), (U"\x0628\x064A")) << desc_decomp_of(U'\xFC0A');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB4E'), (U"\x05E4\x05BF")) << desc_decomp_of(U'\xFB4E');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F996'), (U"\x82E6")) << desc_decomp_of(U'\x2F996');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F859'), (U"\x214E4")) << desc_decomp_of(U'\x2F859');

    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA14'), (U"\x2A291")) << desc_decomp_of(U'\x2FA14');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA15'), (U"\x9EBB")) << desc_decomp_of(U'\x2FA15');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA16'), (U"\x4D56")) << desc_decomp_of(U'\x2FA16');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA17'), (U"\x9EF9")) << desc_decomp_of(U'\x2FA17');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA18'), (U"\x9EFE")) << desc_decomp_of(U'\x2FA18');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA19'), (U"\x9F05")) << desc_decomp_of(U'\x2FA19');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1A'), (U"\x9F0F")) << desc_decomp_of(U'\x2FA1A');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1B'), (U"\x9F16")) << desc_decomp_of(U'\x2FA1B');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1C'), (U"\x9F3B")) << desc_decomp_of(U'\x2FA1C');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1D'), (U"\x2A600")) << desc_decomp_of(U'\x2FA1D');

    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFFC5'), (U"\x3152")) << desc_decomp_of(U'\xFFC5');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03AD'), (U"\x03B5\x0301")) << desc_decomp_of(U'\x03AD');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D731'), (U"\x03A6")) << desc_decomp_of(U'\x1D731');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFCF9'), (U"\x063A\x0649")) << desc_decomp_of(U'\xFCF9');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x32C3'), (U"\x0034\x6708")) << desc_decomp_of(U'\x32C3');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE1'), (U"\x03C5\x0304")) << desc_decomp_of(U'\x1FE1');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D53'), (U"\x0254")) << desc_decomp_of(U'\x1D53');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFC49'), (U"\x0645\x0649")) << desc_decomp_of(U'\xFC49');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E067'), (U"\x0491")) << desc_decomp_of(U'\x1E067');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFD3A'), (U"\x0637\x0645")) << desc_decomp_of(U'\xFD3A');

    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3160'), (U"\x1172")) << desc_decomp_of(U'\x3160');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FCB'), (U"\x9EF9")) << desc_decomp_of(U'\x2FCB');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x33E4'), (U"\x0035\x65E5")) << desc_decomp_of(U'\x33E4');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F62'), (U"\x1F60\x0300")) << desc_decomp_of(U'\x1F62');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFFB5'), (U"\x3145")) << desc_decomp_of(U'\xFFB5');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0453'), (U"\x0433\x0301")) << desc_decomp_of(U'\x0453');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x305A'), (U"\x3059\x3099")) << desc_decomp_of(U'\x305A');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFFBA'), (U"\x314A")) << desc_decomp_of(U'\xFFBA');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D5CE'), (U"\x0075")) << desc_decomp_of(U'\x1D5CE');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0FA2'), (U"\x0FA1\x0FB7")) << desc_decomp_of(U'\x0FA2');
}

TEST(Unicode, DecomposeHangul) {
    EXPECT_EQ(canonical_decomposed<u8string>(static_cast<char32_t>(webpp::unicode::hangul_syllable_base)),
              utf32_to_utf8(U"\x1100\x1161"))
      << desc_decomp_of(static_cast<char32_t>(webpp::unicode::hangul_syllable_base));


    EXPECT_EQ(canonical_decomposed<u8string>(static_cast<char32_t>(webpp::unicode::hangul_syllable_base + 1)),
              utf32_to_utf8(U"\x1100\x1161\x11A8"))
      << desc_decomp_of(static_cast<char32_t>(webpp::unicode::hangul_syllable_base + 1));


    // not hangul
    EXPECT_EQ(canonical_decomposed<u8string>(U'\x2BA4'), utf32_to_utf8(U"\x2BA4"))
      << desc_decomp_of(U'\x2BA4');
}

namespace {
    // implementation from
    // https://github.com/ada-url/idna/blob/fff988508f659ef5c6494572ebea3d5db2466ed0/src/normalization.cpp#L97
    void sort_marks(std::u32string& input) {
        for (size_t idx = 1; idx < input.size(); idx++) {
            uint8_t const ccc = ccc_of(input[idx]);
            if (ccc == 0) {
                continue;
            } // Skip non-combining characters.
            auto const current_character = input[idx];
            size_t     back_idx          = idx;
            while (back_idx != 0 && ccc_of(input[back_idx - 1]) > ccc) {
                input[back_idx] = input[back_idx - 1];
                back_idx--;
            }
            input[back_idx] = current_character;
        }
    }
} // namespace

TEST(Unicode, UnicodeSwap) {
    // single byte move
    u8string str = u8" \1\2\3\4\5\6\7";
    auto     beg = str.begin();
    swap_code_points(beg + 2, beg + 6);
    EXPECT_EQ(str, u8" \1\6\3\4\5\2\7");


    // Unicode code point move
    str = u8" ✅✅✅ ❎❎❎";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, u8" ✅❎✅ ❎✅❎");


    // lhs's length is smaller than the rhs's length
    str = u8" 123 ❎❎❎";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, u8" 1❎3 ❎2❎");


    // rhs's length is smaller than the lhs's length
    str = u8" ❎❎❎ 123";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, u8" ❎2❎ 1❎3");

    str = u8" ❎❎❎ 12";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, u8" ❎2❎ 1❎");
}

TEST(Unicode, UnicodeSwap16) {
    // single byte move
    u16string str = u" \1\2\3\4\5\6\7";
    auto      beg = str.begin();
    swap_code_points(beg + 2, beg + 6);
    EXPECT_EQ(str, u" \1\6\3\4\5\2\7");


    // Unicode code point move
    str = u" ✅✅✅ ❎❎❎";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, u" ✅❎✅ ❎✅❎");


    // lhs's length is smaller than the rhs's length
    str = u" 123 ❎❎❎";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, u" 1❎3 ❎2❎");


    // rhs's length is smaller than the lhs's length
    str = u" ❎❎❎ 123";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, u" ❎2❎ 1❎3");

    str = u" ❎❎❎ 12";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, u" ❎2❎ 1❎");
}

TEST(Unicode, UnicodeSwap32) {
    // single byte move
    u32string str = U" \1\2\3\4\5\6\7";
    auto      beg = str.begin();
    swap_code_points(beg + 2, beg + 6);
    EXPECT_EQ(str, U" \1\6\3\4\5\2\7");


    // Unicode code point move
    str = U" ✅✅✅ ❎❎❎";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, U" ✅❎✅ ❎✅❎");


    // lhs's length is smaller than the rhs's length
    str = U" 123 ❎❎❎";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, U" 1❎3 ❎2❎");


    // rhs's length is smaller than the lhs's length
    str = U" ❎❎❎ 123";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, U" ❎2❎ 1❎3");

    str = U" ❎❎❎ 12";
    beg = str.begin();
    swap_code_points(next_char_copy(beg, 2), next_char_copy(beg, 6));
    EXPECT_EQ(str, U" ❎2❎ 1❎");
}

TEST(Unicode, PrevCodePoint8) {
    u8string str = u8"✅❎";
    auto     beg = str.begin();
    next_char(beg);
    auto code_point = prev_code_point(beg);
    EXPECT_EQ(code_point, U'✅') << static_cast<std::uint32_t>(U'❎');

    str = u8"1✅❎";
    beg = str.begin();
    next_char(beg, 2);
    EXPECT_EQ(prev_code_point(beg), U'✅');

    str = u8"1✅❎";
    beg = str.begin();
    next_char(beg);
    EXPECT_EQ(prev_code_point(beg), U'1');
}

TEST(Unicode, PrevCodePoint16) {
    u16string str = u"✅❎";
    auto      beg = str.begin();
    next_char(beg);
    auto const code_point = webpp::unicode::prev_code_point(beg);
    EXPECT_EQ(code_point, U'✅') << static_cast<std::uint32_t>(U'❎');

    str = u"1✅❎";
    beg = str.begin();
    next_char(beg, 2);
    EXPECT_EQ(webpp::unicode::prev_code_point(beg), U'✅');

    str = u"1✅❎";
    beg = str.begin();
    next_char(beg);
    EXPECT_EQ(webpp::unicode::prev_code_point(beg), U'1');
}

TEST(Unicode, PrevCodePoint32) {
    u32string str = U"✅❎";
    auto      beg = str.begin();
    next_char(beg);
    auto const code_point = webpp::unicode::prev_code_point(beg);
    EXPECT_EQ(code_point, U'✅') << static_cast<std::uint32_t>(U'❎');
}

TEST(Unicode, SortMarkTest) {
    // a + <U+0308> + <U+0328> ( diaeresis + ogonek) -> canonicalOrdering reorders the accents!
    u8string  str  = u8"a\xcc\x88\xcc\xa8";
    u32string str2 = utf8_to_utf32(str);
    webpp::unicode::canonical_reorder(str.begin(), str.end());
    sort_marks(str2);
    EXPECT_EQ(utf8_to_utf32(str), str2);
    EXPECT_EQ(str, utf32_to_utf8(str2));

    auto test_sorting = [](u32string bad_u32, u32string const& good_u32) {
        u8string       bad  = utf32_to_utf8(bad_u32);
        u8string const good = utf32_to_utf8(good_u32);
        webpp::unicode::canonical_reorder(bad.begin(), bad.end());
        sort_marks(bad_u32);
        EXPECT_EQ(utf8_to_utf32(bad), bad_u32);
        EXPECT_EQ(bad, utf32_to_utf8(bad_u32));
        EXPECT_EQ(bad, good);
        EXPECT_EQ(utf32_to_utf8(bad_u32), good);
        EXPECT_EQ(bad_u32, good_u32);
    };

    test_sorting(U"\x1D16D\x1D16E", U"\x1D16E\x1D16D");
    test_sorting(U"\x101FD\x10376", U"\x101FD\x10376");
    test_sorting(U"\x10376\x101FD", U"\x101FD\x10376");
    test_sorting(U"abc\x1D16D\x1D16E", U"abc\x1D16E\x1D16D");
    test_sorting(U"abc\x1D16D\x1D16E\x1\x2\x3", U"abc\x1D16E\x1D16D\x1\x2\x3");
}

TEST(Unicode, Compose) {
    // clang-format off
    // awk 'BEGIN{FS=";"; OF=""} !/^\s*#/ { $6 = " " $6; gsub(/\s+/, " ", $6); if ($6 != " " && $6 ~ /\S\s\S/ && !($6 ~ /</)) { $6 = gensub(/\s+/, ", 0x", 2, $6); gsub(/^\s/, "0x", $6); print "EXPECT_EQ(canonical_composed(" $6 "), 0x" $1 ");"; } }' UnicodeData.txt | sort -R | head
    // clang-format on
    using webpp::unicode::canonical_composed;

    // specials:
    EXPECT_EQ(canonical_composed(70'375, 43'456), webpp::unicode::replacement_char<char32_t>);

    // head:
    EXPECT_EQ(canonical_composed(0x0041, 0x0300), 0x00C0);
    EXPECT_EQ(canonical_composed(0x0041, 0x0301), 0x00C1);
    EXPECT_EQ(canonical_composed(0x0041, 0x0302), 0x00C2);
    EXPECT_EQ(canonical_composed(0x0041, 0x0303), 0x00C3);
    EXPECT_EQ(canonical_composed(0x0041, 0x0308), 0x00C4);
    EXPECT_EQ(canonical_composed(0x0041, 0x030A), 0x00C5);
    EXPECT_EQ(canonical_composed(0x0043, 0x0327), 0x00C7);
    EXPECT_EQ(canonical_composed(0x0045, 0x0300), 0x00C8);
    EXPECT_EQ(canonical_composed(0x0045, 0x0301), 0x00C9);
    EXPECT_EQ(canonical_composed(0x0045, 0x0302), 0x00CA);

    // tail:
    EXPECT_EQ(canonical_composed(0x1'D15F, 0x1'D16F), 0x1'D161);
    EXPECT_EQ(canonical_composed(0x1'D15F, 0x1'D170), 0x1'D162);
    EXPECT_EQ(canonical_composed(0x1'D15F, 0x1'D171), 0x1'D163);
    EXPECT_EQ(canonical_composed(0x1'D15F, 0x1'D172), 0x1'D164);
    EXPECT_EQ(canonical_composed(0x1'D1B9, 0x1'D165), 0x1'D1BB);
    EXPECT_EQ(canonical_composed(0x1'D1BA, 0x1'D165), 0x1'D1BC);
    EXPECT_EQ(canonical_composed(0x1'D1BB, 0x1'D16E), 0x1'D1BD);
    EXPECT_EQ(canonical_composed(0x1'D1BC, 0x1'D16E), 0x1'D1BE);
    EXPECT_EQ(canonical_composed(0x1'D1BB, 0x1'D16F), 0x1'D1BF);
    EXPECT_EQ(canonical_composed(0x1'D1BC, 0x1'D16F), 0x1'D1C0);

    // random:
    EXPECT_EQ(canonical_composed(0xFB49, 0x05C2), 0xFB2D);
    EXPECT_EQ(canonical_composed(0xFB49, 0x05C1), 0xFB2C);
    EXPECT_EQ(canonical_composed(0x30FD, 0x3099), 0x30FE);
    EXPECT_EQ(canonical_composed(0x30F2, 0x3099), 0x30FA);
    EXPECT_EQ(canonical_composed(0x30F1, 0x3099), 0x30F9);
    EXPECT_EQ(canonical_composed(0x30F0, 0x3099), 0x30F8);
    EXPECT_EQ(canonical_composed(0x30EF, 0x3099), 0x30F7);
    EXPECT_EQ(canonical_composed(0x30DB, 0x309A), 0x30DD);
    EXPECT_EQ(canonical_composed(0x30DB, 0x3099), 0x30DC);
    EXPECT_EQ(canonical_composed(0x30D8, 0x309A), 0x30DA);

    EXPECT_EQ(canonical_composed(0x2248, 0x0338), 0x2249);
    EXPECT_EQ(canonical_composed(0x2286, 0x0338), 0x2288);
    EXPECT_EQ(canonical_composed(0x03BF, 0x0301), 0x03CC);
    EXPECT_EQ(canonical_composed(0x0044, 0x0307), 0x1E0A);
    EXPECT_EQ(canonical_composed(0x1F31, 0x0300), 0x1F33);
    EXPECT_EQ(canonical_composed(0x0474, 0x030F), 0x0476);
    EXPECT_EQ(canonical_composed(0x0423, 0x030B), 0x04F2);
    EXPECT_EQ(canonical_composed(0x004E, 0x0323), 0x1E46);
    EXPECT_EQ(canonical_composed(0x03B5, 0x0300), 0x1F72);
    EXPECT_EQ(canonical_composed(0x1F20, 0x0301), 0x1F24);

    EXPECT_EQ(canonical_composed(0x039F, 0x0301), 0x038C);
    EXPECT_EQ(canonical_composed(0x2287, 0x0338), 0x2289);
    EXPECT_EQ(canonical_composed(0x220B, 0x0338), 0x220C);
    EXPECT_EQ(canonical_composed(0x0079, 0x030A), 0x1E99);
    EXPECT_EQ(canonical_composed(0x0067, 0x0307), 0x0121);
    EXPECT_EQ(canonical_composed(0x1F11, 0x0300), 0x1F13);
    EXPECT_EQ(canonical_composed(0x0112, 0x0301), 0x1E16);
    EXPECT_EQ(canonical_composed(0x01B0, 0x0300), 0x1EEB);
    EXPECT_EQ(canonical_composed(0x1F10, 0x0301), 0x1F14);
    EXPECT_EQ(canonical_composed(0x0061, 0x0304), 0x0101);

    EXPECT_EQ(canonical_composed(0x0074, 0x0326), 0x021B);
    EXPECT_EQ(canonical_composed(0x0064, 0x0331), 0x1E0F);
    EXPECT_EQ(canonical_composed(0x05D5, 0x05B9), 0xFB4B);
    EXPECT_EQ(canonical_composed(0x0055, 0x0308), 0x00DC);
    EXPECT_EQ(canonical_composed(0x05EA, 0x05BC), 0xFB4A);
    EXPECT_EQ(canonical_composed(0x0F71, 0x0F72), 0x0F73);
    EXPECT_EQ(canonical_composed(0x03C1, 0x0314), 0x1FE5);
    EXPECT_EQ(canonical_composed(0x00A8, 0x0301), 0x0385);
    EXPECT_EQ(canonical_composed(0x0079, 0x0301), 0x00FD);
    EXPECT_EQ(canonical_composed(0x22B4, 0x0338), 0x22EC);
}

TEST(Unicode, ComposeStr) {
    using std::u32string;
    using std::u8string;
    using webpp::unicode::canonical_compose;
    using webpp::unicode::canonical_composed;

    EXPECT_EQ(canonical_composed<char32_t>('a', 0x0300), U'\x00e0');
    EXPECT_EQ(canonical_composed<char32_t>(1488, 776), webpp::unicode::replacement_char<char32_t>);
    EXPECT_EQ(canonical_composed<char32_t>(111, 0x03'08bb), webpp::unicode::replacement_char<char32_t>);
    EXPECT_EQ(canonical_composed<u32string>(U"o\u0308bb"), U"\u00f6bb");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0300.\x05d0\x0308"), U"\x00e0.\x05d0\x0308");
    EXPECT_EQ(canonical_composed<u32string>(U"."), U".");

    u32string str;
    u8string  str8;

    str = U"\x22B4\x0338 \x22B4\x0338 \x22B4\x0338";
    canonical_compose(str);
    EXPECT_EQ(str, U"\x22EC \x22EC \x22EC");

    str = U"\x22B4\x0338\x22B4\x0338\x22B4\x0338";
    canonical_compose(str);
    EXPECT_EQ(str, U"\x22EC\x22EC\x22EC");

    if constexpr (enable_utf8_composition_tests) {
        str8 = utf32_to_utf8(U"\x22B4\x0338\x22B4\x0338\x22B4\x0338");
        canonical_compose(str8);
        EXPECT_EQ(str8, utf32_to_utf8(U"\x22EC\x22EC\x22EC"));
    }

    EXPECT_EQ(canonical_composed<u32string>(U"fa\x00df.de"), U"fa\x00df.de");
    EXPECT_EQ(canonical_composed<u32string>(U"fa\x00df.de"), U"fa\x00df.de");
    EXPECT_EQ(canonical_composed<u32string>(U"fa\x00df"), U"fa\x00df");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x0308"), U"\x05d0\x0308");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0300\x0308.\x05d0"), U"\x00e0\x0308.\x05d0");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0300\x0308"), U"\x00e0\x0308");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x200cb"), U"a\x200cb");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094d\x200cb"), U"a\x094d\x200cb");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094db"), U"a\x094db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094db"), U"a\x094db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094db"), U"a\x094db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094db"), U"a\x094db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094d\x200cb"), U"a\x094d\x200cb");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x200db"), U"a\x200db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x200db"), U"a\x200db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x200db"), U"a\x200db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x200db"), U"a\x200db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094d\x200db"), U"a\x094d\x200db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094d\x200db"), U"a\x094d\x200db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094d\x200db"), U"a\x094d\x200db");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x094d\x200db"), U"a\x094d\x200db");
    EXPECT_EQ(canonical_composed<u32string>(U"\x00a1"), U"\x00a1");
    EXPECT_EQ(canonical_composed<u32string>(U"\x00a1"), U"\x00a1");
    EXPECT_EQ(canonical_composed<u32string>(U"\x19da"), U"\x19da");
    EXPECT_EQ(canonical_composed<u32string>(U"\x19da"), U"\x19da");
    EXPECT_EQ(canonical_composed<u32string>(U"\xab60"), U"\xab60");
    EXPECT_EQ(canonical_composed<u32string>(U"\xab60"), U"\xab60");
    EXPECT_EQ(
      canonical_composed<u32string>(U"1234567890a\u03081234567890123456789012345678901234567890123456"),
      U"1234567890\u00e41234567890123456789012345678901234567890123456");
    EXPECT_EQ(
      canonical_composed<u32string>(U"1234567890a\u03081234567890123456789012345678901234567890123456"),
      U"1234567890\u00e41234567890123456789012345678901234567890123456");
    EXPECT_EQ(
      canonical_composed<u32string>(U"1234567890a\u03081234567890123456789012345678901234567890123456"),
      U"1234567890\u00e41234567890123456789012345678901234567890123456");
    EXPECT_EQ(
      canonical_composed<u32string>(U"1234567890a\u03081234567890123456789012345678901234567890123456"),
      U"1234567890\u00e41234567890123456789012345678901234567890123456");
    EXPECT_EQ(
      canonical_composed<u32string>(U"1234567890a\u03081234567890123456789012345678901234567890123456"),
      U"1234567890\u00e41234567890123456789012345678901234567890123456");
    EXPECT_EQ(canonical_composed<u32string>(U"bu\u0308cher.de"), U"b\u00fccher.de");
    EXPECT_EQ(canonical_composed<u32string>(U"bu\u0308cher.de"), U"b\u00fccher.de");
    EXPECT_EQ(canonical_composed<u32string>(U"bu\u0308cher.de"), U"b\u00fccher.de");
    EXPECT_EQ(canonical_composed<u32string>(U"bu\u0308cher.de"), U"b\u00fccher.de");
    EXPECT_EQ(canonical_composed<u32string>(U"bu\u0308cher.de"), U"b\u00fccher.de");
    EXPECT_EQ(canonical_composed<u32string>(U"bu\u0308cher.de"), U"b\u00fccher.de");
    EXPECT_EQ(canonical_composed<u32string>(U"bu\u0308cher"), U"b\u00fccher");
    EXPECT_EQ(canonical_composed<u32string>(U"o\u0308bb"), U"\u00f6bb");
    EXPECT_EQ(canonical_composed<u32string>(U"o\u0308bb"), U"\u00f6bb");
    EXPECT_EQ(canonical_composed<u32string>(U"o\u0308bb"), U"\u00f6bb");
    EXPECT_EQ(canonical_composed<u32string>(U"o\u0308bb"), U"\u00f6bb");
    EXPECT_EQ(canonical_composed<u32string>(U"o\u0308bb"), U"\u00f6bb");
    EXPECT_EQ(canonical_composed<u32string>(U"o\u0308bb"), U"\u00f6bb");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2.com"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c3"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2");
    EXPECT_EQ(canonical_composed<u32string>(U"\x03b2\x03bf\x0301\x03bb\x03bf\x03c2"),
              U"\x03b2\x03cc\x03bb\x03bf\x03c2");
    EXPECT_EQ(canonical_composed<u32string>(U"www.\x0dc1\x0dca\x200d\x0dbb\x0dd3.com"),
              U"www.\x0dc1\x0dca\x200d\x0dbb\x0dd3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"www.\x0dc1\x0dca\x200d\x0dbb\x0dd3.com"),
              U"www.\x0dc1\x0dca\x200d\x0dbb\x0dd3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"www.\x0dc1\x0dca\x200d\x0dbb\x0dd3.com"),
              U"www.\x0dc1\x0dca\x200d\x0dbb\x0dd3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0dc1\x0dca\x0dbb\x0dd3"), U"\x0dc1\x0dca\x0dbb\x0dd3");
    EXPECT_EQ(canonical_composed<u32string>(U"www.\x0dc1\x0dca\x0dbb\x0dd3.com"),
              U"www.\x0dc1\x0dca\x0dbb\x0dd3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"www.\x0dc1\x0dca\x0dbb\x0dd3.com"),
              U"www.\x0dc1\x0dca\x0dbb\x0dd3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"www.\x0dc1\x0dca\x0dbb\x0dd3.com"),
              U"www.\x0dc1\x0dca\x0dbb\x0dd3.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0dc1\x0dca\x200d\x0dbb\x0dd3"),
              U"\x0dc1\x0dca\x200d\x0dbb\x0dd3");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc"),
              U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x0627\x06cc"),
              U"\x0646\x0627\x0645\x0647\x0627\x06cc");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x0627\x06cc"),
              U"\x0646\x0627\x0645\x0647\x0627\x06cc");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc"),
              U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc"),
              U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc.com"),
              U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc.com"),
              U"\x0646\x0627\x0645\x0647\x200c\x0627\x06cc.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x0627\x06cc"),
              U"\x0646\x0627\x0645\x0647\x0627\x06cc");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x0627\x06cc.com"),
              U"\x0646\x0627\x0645\x0647\x0627\x06cc.com");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0646\x0627\x0645\x0647\x0627\x06cc.com"),
              U"\x0646\x0627\x0645\x0647\x0627\x06cc.com");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.c.d."), U"a.b.c.d.");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.c.d."), U"a.b.c.d.");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.c.d."), U"a.b.c.d.");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.c.d."), U"a.b.c.d.");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.c.d."), U"a.b.c.d.");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.c.d."), U"a.b.c.d.");
    EXPECT_EQ(canonical_composed<u32string>(U"u\x0308.xn--tda"), U"\x00fc.xn--tda");
    EXPECT_EQ(canonical_composed<u32string>(U"u\x0308.xn--tda"), U"\x00fc.xn--tda");
    EXPECT_EQ(canonical_composed<u32string>(U"u\x0308.xn--tda"), U"\x00fc.xn--tda");
    EXPECT_EQ(canonical_composed<u32string>(U"u\x0308.xn--tda"), U"\x00fc.xn--tda");
    EXPECT_EQ(canonical_composed<u32string>(U"u\x0308.xn--tda"), U"\x00fc.xn--tda");
    EXPECT_EQ(canonical_composed<u32string>(U"u\x0308.xn--tda"), U"\x00fc.xn--tda");
    EXPECT_EQ(canonical_composed<u32string>(U"u\x0308.xn--tda"), U"\x00fc.xn--tda");
    EXPECT_EQ(canonical_composed<u32string>(U"u\x0308.xn--tda"), U"\x00fc.xn--tda");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--a-a\x0308.pt"), U"xn--a-\x00e4.pt");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--a-a\x0308.pt"), U"xn--a-\x00e4.pt");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--a-a\x0308.pt"), U"xn--a-\x00e4.pt");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--a-a\x0308.pt"), U"xn--a-\x00e4.pt");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--a-a\x0308.pt"), U"xn--a-\x00e4.pt");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--a-a\x0308.pt"), U"xn--a-\x00e4.pt");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--a-a\x0308.pt"), U"xn--a-\x00e4.pt");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--a-a\x0308.pt"), U"xn--a-\x00e4.pt");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e"), U"\x65e5\x672c\x8a9e");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x65e5\x672c\x8a9e.jp"), U"\x65e5\x672c\x8a9e.jp");
    EXPECT_EQ(canonical_composed<u32string>(U"\x2615"), U"\x2615");
    EXPECT_EQ(canonical_composed<u32string>(U"\x2615"), U"\x2615");
    EXPECT_EQ(
      canonical_composed<u32string>(
        U"1."
        U"a\x00df\x200c\x200db\x200c\x200dc\x00df\x00df\x00df\x00dfd\x03c2\x03c3\x00df"
        U"\x00df\x00df\x00df\x00df\x00df\x00df\x00dfe\x00df\x00df\x00df\x00df\x00df\x00df"
        U"\x00df\x00df\x00df\x00dfx\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df"
        U"\x00dfy\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x0302\x00dfz"),
      U"1."
      U"a\x00df\x200c\x200db\x200c\x200dc\x00df\x00df\x00df\x00dfd\x03c2\x03c3\x00df\x00df\x00df\x00df\x00df"
      U"\x00df\x00df\x00dfe\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00dfx\x00df\x00df\x00df"
      U"\x00df\x00df\x00df\x00df\x00df\x00df\x00dfy\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x0302"
      U"\x00dfz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssss"
                U"ssssssssssssssxssssssssssssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssss"
              U"sssssssssssssssysssssssssssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssss"
                U"ssssssssssssssxssssssssssssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssss"
              U"sssssssssssssssysssssssssssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssss"
                U"ssssssssssssssxssssssssssssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssss"
              U"sssssssssssssssysssssssssssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssss"
                U"ssssssssssssssxssssssssssssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssss"
              U"sssssssssssssssysssssssssssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssss"
                U"ssssssssssssssxssssssssssssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssss"
              U"sssssssssssssssysssssssssssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssss"
                U"ssssssssssssssxssssssssssssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssss"
              U"sssssssssssssssysssssssssssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssssssss"
                U"sssssssssssyssssssssssssssss\x0302ssz"),
              U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxssssssssssssssssssssyssssssss"
              U"sssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssssssss"
                U"sssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxssssssssssssssssssssyssssssss"
              U"sssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssssssss"
                U"sssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxssssssssssssssssssssyssssssss"
              U"sssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssssssss"
                U"sssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxssssssssssssssssssssyssssssss"
              U"sssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssssssss"
                U"sssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxssssssssssssssssssssyssssssss"
              U"sssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssssssss"
                U"sssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxssssssssssssssssssssyssssssss"
              U"sssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"1."
                U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssssssss"
                U"sssssssssssyssssssssssssssss\x0302ssz"),
              U"1."
              U"assbcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxssssssssssssssssssssyssssssss"
              U"sssssss\x015dssz");
    EXPECT_EQ(canonical_composed<u32string>(
                U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssss"
                U"ssssssssssssssxssssssssssssssssssssyssssssssssssssss\x0302ssz"),
              U"ass\x200c\x200db\x200c\x200dcssssssssd\x03c3\x03c3ssssssssssssssssessssssssssssssssssssxsssss"
              U"sssssssssssssssysssssssssssssss\x015dssz");
    EXPECT_EQ(
      canonical_composed<u32string>(
        U"1."
        U"a\x00df\x200c\x200db\x200c\x200dc\x00df\x00df\x00df\x00dfd\x03c2\x03c3\x00df"
        U"\x00df\x00df\x00df\x00df\x00df\x00df\x00dfe\x00df\x00df\x00df\x00df\x00df\x00df"
        U"\x00df\x00df\x00df\x00dfx\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df"
        U"\x00dfy\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x0302\x00dfz"),
      U"1."
      U"a\x00df\x200c\x200db\x200c\x200dc\x00df\x00df\x00df\x00dfd\x03c2\x03c3\x00df\x00df\x00df\x00df\x00df"
      U"\x00df\x00df\x00dfe\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00dfx\x00df\x00df\x00df"
      U"\x00df\x00df\x00df\x00df\x00df\x00df\x00dfy\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x0302"
      U"\x00dfz");
    EXPECT_EQ(
      canonical_composed<u32string>(
        U"a\x00df\x200c\x200db\x200c\x200dc\x00df\x00df\x00df\x00dfd\x03c2\x03c3\x00df"
        U"\x00df\x00df\x00df\x00df\x00df\x00df\x00dfe\x00df\x00df\x00df\x00df\x00df\x00df"
        U"\x00df\x00df\x00df\x00dfx\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df"
        U"\x00dfy\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x0302\x00dfz"),
      U"a\x00df\x200c\x200db\x200c\x200dc\x00df\x00df\x00df\x00dfd\x03c2\x03c3\x00df\x00df\x00df\x00df\x00df"
      U"\x00df\x00df\x00dfe\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00dfx\x00df\x00df\x00df"
      U"\x00df\x00df\x00df\x00df\x00df\x00df\x00dfy\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x00df\x0302"
      U"\x00dfz");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200cx\x200dn\x200c-\x200d-b\x00df"),
              U"\x200cx\x200dn\x200c-\x200d-b\x00df");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200cx\x200dn\x200c-\x200d-bss"),
              U"\x200cx\x200dn\x200c-\x200d-bss");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200cx\x200dn\x200c-\x200d-bss"),
              U"\x200cx\x200dn\x200c-\x200d-bss");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200cx\x200dn\x200c-\x200d-bss"),
              U"\x200cx\x200dn\x200c-\x200d-bss");
    EXPECT_EQ(canonical_composed<u32string>(U"\x5919"), U"\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"\x5919"), U"\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200cx\x200dn\x200c-\x200d-bss"),
              U"\x200cx\x200dn\x200c-\x200d-bss");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200cx\x200dn\x200c-\x200d-b\x00df"),
              U"\x200cx\x200dn\x200c-\x200d-b\x00df");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200cx\x200dn\x200c-\x200d-b\x00df"),
              U"\x200cx\x200dn\x200c-\x200d-b\x00df");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--bssffl"), U"xn--bssffl");
    EXPECT_EQ(canonical_composed<u32string>(U"\x591e\x591c\x5919"), U"\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--bssffl"), U"xn--bssffl");
    EXPECT_EQ(canonical_composed<u32string>(U"\x591e\x591c\x5919"), U"\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--bssffl"), U"xn--bssffl");
    EXPECT_EQ(canonical_composed<u32string>(U"\x591e\x591c\x5919"), U"\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--bssffl"), U"xn--bssffl");
    EXPECT_EQ(canonical_composed<u32string>(U"\x591e\x591c\x5919"), U"\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--bssffl"), U"xn--bssffl");
    EXPECT_EQ(canonical_composed<u32string>(U"\x591e\x591c\x5919"), U"\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"\x5921\x591e\x591c\x5919"), U"\x5921\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"\x5921\x591e\x591c\x5919"), U"\x5921\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--bssffl"), U"xn--bssffl");
    EXPECT_EQ(canonical_composed<u32string>(U"\x591e\x591c\x5919"), U"\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--bssffl"), U"xn--bssffl");
    EXPECT_EQ(canonical_composed<u32string>(U"\x591e\x591c\x5919"), U"\x591e\x591c\x5919");
    EXPECT_EQ(canonical_composed<u32string>(U"xn--bssffl"), U"xn--bssffl");
    EXPECT_EQ(canonical_composed<u32string>(U"\x591e\x591c\x5919"), U"\x591e\x591c\x5919");
    EXPECT_EQ(
      canonical_composed<u32string>(U"a\u03081234567890123456789012345678901234567890123456789012345"),
      U"\u00e41234567890123456789012345678901234567890123456789012345");
    EXPECT_EQ(
      canonical_composed<u32string>(U"a\u03081234567890123456789012345678901234567890123456789012345"),
      U"\u00e41234567890123456789012345678901234567890123456789012345");
    EXPECT_EQ(
      canonical_composed<u32string>(U"a\u03081234567890123456789012345678901234567890123456789012345"),
      U"\u00e41234567890123456789012345678901234567890123456789012345");
    EXPECT_EQ(
      canonical_composed<u32string>(U"a\u03081234567890123456789012345678901234567890123456789012345"),
      U"\u00e41234567890123456789012345678901234567890123456789012345");
    EXPECT_EQ(
      canonical_composed<u32string>(U"a\u03081234567890123456789012345678901234567890123456789012345"),
      U"\u00e41234567890123456789012345678901234567890123456789012345");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b..-q--a\x0308-.e"), U"a.b..-q--\x00e4-.e");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b..-q--a\x0308-.e"), U"a.b..-q--\x00e4-.e");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b..-q--a\x0308-.e"), U"a.b..-q--\x00e4-.e");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b..-q--a\x0308-.e"), U"a.b..-q--\x00e4-.e");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b..-q--a\x0308-.e"), U"a.b..-q--\x00e4-.e");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b..-q--a\x0308-.e"), U"a.b..-q--\x00e4-.e");
    EXPECT_EQ(canonical_composed<u32string>(U"-q--a\x0308-"), U"-q--\x00e4-");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308..c"), U"\x00e4..c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308..c"), U"\x00e4..c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308..c"), U"\x00e4..c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308..c"), U"\x00e4..c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.-b."), U"\x00e4.-b.");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.-b."), U"\x00e4.-b.");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.-b."), U"\x00e4.-b.");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.-b."), U"\x00e4.-b.");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.b-.c"), U"\x00e4.b-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.b-.c"), U"\x00e4.b-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.b-.c"), U"\x00e4.b-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.b-.c"), U"\x00e4.b-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.b-.c"), U"\x00e4.b-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.b-.c"), U"\x00e4.b-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.-.c"), U"\x00e4.-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.-.c"), U"\x00e4.-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.-.c"), U"\x00e4.-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.-.c"), U"\x00e4.-.c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.bc--de.f"), U"\x00e4.bc--de.f");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.bc--de.f"), U"\x00e4.bc--de.f");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.bc--de.f"), U"\x00e4.bc--de.f");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.bc--de.f"), U"\x00e4.bc--de.f");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.bc--de.f"), U"\x00e4.bc--de.f");
    EXPECT_EQ(canonical_composed<u32string>(U"a\x0308.bc--de.f"), U"\x00e4.bc--de.f");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.\x0308c.d"), U"a.b.\x0308c.d");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.\x0308c.d"), U"a.b.\x0308c.d");
    EXPECT_EQ(canonical_composed<u32string>(U"a.b.\x0308c.d"), U"a.b.\x0308c.d");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0308c"), U"\x0308c");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x05c7"), U"\x05d0\x05c7");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x05c7"), U"\x05d0\x05c7");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d09\x05c7"), U"\x05d09\x05c7");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d09\x05c7"), U"\x05d09\x05c7");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x05ea"), U"\x05d0\x05ea");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x05ea"), U"\x05d0\x05ea");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x05f3\x05ea"), U"\x05d0\x05f3\x05ea");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x05f3\x05ea"), U"\x05d0\x05f3\x05ea");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d07\x05ea"), U"\x05d07\x05ea");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d07\x05ea"), U"\x05d07\x05ea");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x0667\x05ea"), U"\x05d0\x0667\x05ea");
    EXPECT_EQ(canonical_composed<u32string>(U"\x05d0\x0667\x05ea"), U"\x05d0\x0667\x05ea");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x0bcd\x200d"), U"\x0bb9\x0bcd\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x0bcd"), U"\x0bb9\x0bcd");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x0bcd"), U"\x0bb9\x0bcd");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x0bcd\x200d"), U"\x0bb9\x0bcd\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x200d"), U"\x0bb9\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9"), U"\x0bb9");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9"), U"\x0bb9");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x200d"), U"\x0bb9\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200d"), U"\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200d"), U"\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x0bcd\x200c"), U"\x0bb9\x0bcd\x200c");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x0bcd\x200c"), U"\x0bb9\x0bcd\x200c");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x200c"), U"\x0bb9\x200c");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0bb9\x200c"), U"\x0bb9\x200c");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200c"), U"\x200c");
    EXPECT_EQ(canonical_composed<u32string>(U"\x200c"), U"\x200c");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x200c\x06ed\x06ef"),
              U"\x0644\x0670\x200c\x06ed\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x06ed\x06ef"), U"\x0644\x0670\x06ed\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x06ed\x06ef"), U"\x0644\x0670\x06ed\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x200c\x06ed\x06ef"),
              U"\x0644\x0670\x200c\x06ed\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x200c\x06ef"), U"\x0644\x0670\x200c\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x06ef"), U"\x0644\x0670\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x06ef"), U"\x0644\x0670\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x200c\x06ef"), U"\x0644\x0670\x200c\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x200c\x06ed\x06ef"), U"\x0644\x200c\x06ed\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x06ed\x06ef"), U"\x0644\x06ed\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x06ed\x06ef"), U"\x0644\x06ed\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x200c\x06ed\x06ef"), U"\x0644\x200c\x06ed\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x200c\x06ef"), U"\x0644\x200c\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x06ef"), U"\x0644\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x06ef"), U"\x0644\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x200c\x06ef"), U"\x0644\x200c\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x06ed"), U"\x0644\x0670\x06ed");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644\x0670\x06ed"), U"\x0644\x0670\x06ed");
    EXPECT_EQ(canonical_composed<u32string>(U"\x06ef\x200c\x06ef"), U"\x06ef\x200c\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x06ef\x06ef"), U"\x06ef\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x06ef\x06ef"), U"\x06ef\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x06ef\x200c\x06ef"), U"\x06ef\x200c\x06ef");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644"), U"\x0644");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0644"), U"\x0644");
    EXPECT_EQ(canonical_composed<u32string>(U"a..b"), U"a..b");
    EXPECT_EQ(canonical_composed<u32string>(U"a..b"), U"a..b");
    EXPECT_EQ(canonical_composed<u32string>(U"\x06b9"), U"\x06b9");
    EXPECT_EQ(canonical_composed<u32string>(U"\xa863.\x07cf"), U"\xa863.\x07cf");
    EXPECT_EQ(canonical_composed<u32string>(U"\xa863"), U"\xa863");
    EXPECT_EQ(canonical_composed<u32string>(U"\x23cd4\x0303.\x114c2"), U"\x23cd4\x0303.\x114c2");
    EXPECT_EQ(canonical_composed<u32string>(U"\x23cd4\x0303"), U"\x23cd4\x0303");
    EXPECT_EQ(canonical_composed<u32string>(U"\x7e71\x115bf"), U"\x7e71\x115bf");
    EXPECT_EQ(canonical_composed<u32string>(U"\x7e71\x115bf\x200d"), U"\x7e71\x115bf\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U".\x1e008"), U".\x1e008");
    EXPECT_EQ(canonical_composed<u32string>(U".\x1e008"), U".\x1e008");
    EXPECT_EQ(canonical_composed<u32string>(U"\x1e008"), U"\x1e008");
    EXPECT_EQ(canonical_composed<u32string>(U"\x00df\x06eb.\x200d"), U"\x00df\x06eb.\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"ss\x06eb.\x200d"), U"ss\x06eb.\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"ss\x06eb.\x200d"), U"ss\x06eb.\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"ss\x06eb.\x200d"), U"ss\x06eb.\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"ss\x06eb"), U"ss\x06eb");
    EXPECT_EQ(canonical_composed<u32string>(U"ss\x06eb."), U"ss\x06eb.");
    EXPECT_EQ(canonical_composed<u32string>(U"ss\x06eb."), U"ss\x06eb.");
    EXPECT_EQ(canonical_composed<u32string>(U"ss\x06eb."), U"ss\x06eb.");
    EXPECT_EQ(canonical_composed<u32string>(U"ss\x06eb"), U"ss\x06eb");
    EXPECT_EQ(canonical_composed<u32string>(U"\x00df\x06eb"), U"\x00df\x06eb");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0774\x1e93f"), U"\x0774\x1e93f");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0774\x200c\x1e93f"), U"\x0774\x200c\x1e93f");
    EXPECT_EQ(canonical_composed<u32string>(U"\x2763\x200d.\x09cd\x11c3d\x0612\xa929"),
              U"\x2763\x200d.\x09cd\x11c3d\x0612\xa929");
    EXPECT_EQ(canonical_composed<u32string>(U"\x2763\x200d.\x09cd\x11c3d\x0612\xa929"),
              U"\x2763\x200d.\x09cd\x11c3d\x0612\xa929");
    EXPECT_EQ(canonical_composed<u32string>(U"\x2763"), U"\x2763");
    EXPECT_EQ(canonical_composed<u32string>(U"\x2763\x200d"), U"\x2763\x200d");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0349.\x2786b"), U"\x0349.\x2786b");
    EXPECT_EQ(canonical_composed<u32string>(U"\x0349"), U"\x0349");
    EXPECT_EQ(canonical_composed<u32string>(U"\x11c3f"), U"\x11c3f");
    EXPECT_EQ(canonical_composed<u32string>(U"\x850f.\x11c3a"), U"\x850f.\x11c3a");
    EXPECT_EQ(canonical_composed<u32string>(U"\x850f.\x11c3a"), U"\x850f.\x11c3a");
    EXPECT_EQ(canonical_composed<u32string>(U"\x850f"), U"\x850f");
}

TEST(Unicode, ComposedStr2) {
    using webpp::unicode::canonical_composed;
    using webpp::unicode::compose_hangul;

    EXPECT_EQ(canonical_composed<char32_t>(canonical_composed<char32_t>(0x1100, 0x1173), 0x11B2), 0xAE03);

    EXPECT_EQ(canonical_composed<u32string>(U"\u200c\u1100\u1173\u11b2.\u69b6-"), U"\u200c\uae03.\u69b6-");
    EXPECT_EQ(canonical_composed<u32string>(U"\u200c\u1100\u1173\u11b2.\u69b6-"), U"\u200c\uae03.\u69b6-");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1100\u1173\u11b2"), U"\uae03");
    EXPECT_EQ(canonical_composed<u32string>(U"\u200c\u1100\u1173\u11b2"), U"\u200c\uae03");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-.\u200c"), U"\u00e5\ub444-.\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-.\u200c"), U"\u00e5\ub444-.\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-.\u200c"), U"\u00e5\ub444-.\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-.\u200c"), U"\u00e5\ub444-.\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-.\u200c"), U"\u00e5\ub444-.\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-.\u200c"), U"\u00e5\ub444-.\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-"), U"\u00e5\ub444-");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-"), U"\u00e5\ub444-");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-.\u200c"), U"\u00e5\ub444-.\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"a\u030a\u1103\u116d\u11b7-.\u200c"), U"\u00e5\ub444-.\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1105\u116e\u11b0\u200d\u30368\u200c.\u1da16."),
              U"\ub8f1\u200d\u30368\u200c.\u1da16.");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1105\u116e\u11b0\u200d\u30368\u200c.\u1da16."),
              U"\ub8f1\u200d\u30368\u200c.\u1da16.");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1105\u116e\u11b0\u30368"), U"\ub8f1\u30368");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1105\u116e\u11b0\u200d\u30368\u200c"),
              U"\ub8f1\u200d\u30368\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1105\u116e\u11b0\u30368"), U"\ub8f1\u30368");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1105\u116e\u11b0\u200d\u30368\u200c"),
              U"\ub8f1\u200d\u30368\u200c");
    EXPECT_EQ(canonical_composed<u32string>(U"1.\u4c39\u200d-.\u110b\u116e\u11bf"),
              U"1.\u4c39\u200d-.\uc6c8");
    EXPECT_EQ(canonical_composed<u32string>(U"1.\u4c39\u200d-.\u110b\u116e\u11bf"),
              U"1.\u4c39\u200d-.\uc6c8");
    EXPECT_EQ(canonical_composed<u32string>(U"\u110b\u116e\u11bf"), U"\uc6c8");
    EXPECT_EQ(canonical_composed<u32string>(U"\ua846.\u2184\u0fb5\u1102\u116a\u11c1-"),
              U"\ua846.\u2184\u0fb5\ub1ae-");
    EXPECT_EQ(canonical_composed<u32string>(U"\ua846.\u2184\u0fb5\u1102\u116a\u11c1-"),
              U"\ua846.\u2184\u0fb5\ub1ae-");
    EXPECT_EQ(canonical_composed<u32string>(U"\u2184\u0fb5\u1102\u116a\u11c1-"), U"\u2184\u0fb5\ub1ae-");
    EXPECT_EQ(canonical_composed<u32string>(U"\ua9d0\u04cf\u1baa\u08f6.\u1102\u116f\u11bc"),
              U"\ua9d0\u04cf\u1baa\u08f6.\ub235");
    EXPECT_EQ(canonical_composed<u32string>(U"\ua9d0\u04cf\u1baa\u08f6.\u1102\u116f\u11bc"),
              U"\ua9d0\u04cf\u1baa\u08f6.\ub235");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1102\u116f\u11bc"), U"\ub235");
    EXPECT_EQ(canonical_composed<u32string>(U"\ua9d0\u04cf\u1baa\u08f6.\u1102\u116f\u11bc"),
              U"\ua9d0\u04cf\u1baa\u08f6.\ub235");
    EXPECT_EQ(canonical_composed<u32string>(U"\ua9d0\u04cf\u1baa\u08f6.\u1102\u116f\u11bc"),
              U"\ua9d0\u04cf\u1baa\u08f6.\ub235");
    EXPECT_EQ(canonical_composed<u32string>(U"\u08e6\u200d.\u1108\u1168\u11c0"), U"\u08e6\u200d.\ubf3d");
    EXPECT_EQ(canonical_composed<u32string>(U"\u08e6\u200d.\u1108\u1168\u11c0"), U"\u08e6\u200d.\ubf3d");
    EXPECT_EQ(canonical_composed<u32string>(U"\u08e6\u200d.\u1108\u1168\u11c0"), U"\u08e6\u200d.\ubf3d");
    EXPECT_EQ(canonical_composed<u32string>(U"\u08e6\u200d.\u1108\u1168\u11c0"), U"\u08e6\u200d.\ubf3d");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1da9e\u2d10.\u110d\u1168\u11a8"), U"\u1da9e\u2d10.\ucaa1");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1da9e\u2d10.\u110d\u1168\u11a8"), U"\u1da9e\u2d10.\ucaa1");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1da9e\u2d10.\u110d\u1168\u11a8"), U"\u1da9e\u2d10.\ucaa1");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1da9e\u2d10.\u110d\u1168\u11a8"), U"\u1da9e\u2d10.\ucaa1");
    EXPECT_EQ(canonical_composed<u32string>(U"\u200c\ua5a8.16.3\u1110\u116d\u11a9\u06f3"),
              U"\u200c\ua5a8.16.3\ud212\u06f3");
    EXPECT_EQ(canonical_composed<u32string>(U"\u200c\ua5a8.16.3\u1110\u116d\u11a9\u06f3"),
              U"\u200c\ua5a8.16.3\ud212\u06f3");
    EXPECT_EQ(canonical_composed<u32string>(U"3\u1110\u116d\u11a9\u06f3"), U"3\ud212\u06f3");
    EXPECT_EQ(canonical_composed<u32string>(U"\ua5a8.16.3\u1110\u116d\u11a9\u06f3"),
              U"\ua5a8.16.3\ud212\u06f3");
    EXPECT_EQ(canonical_composed<u32string>(U"\ua5a8.16.3\u1110\u116d\u11a9\u06f3"),
              U"\ua5a8.16.3\ud212\u06f3");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1111\u1175\u11bd11"), U"\ud55211");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1110\u1171\u11c2"), U"\ud29b");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1110\u1171\u11c2.\u0716"), U"\ud29b.\u0716");
    EXPECT_EQ(canonical_composed<u32string>(U"\u1110\u1171\u11c2.\u0716"), U"\ud29b.\u0716");
    EXPECT_EQ(canonical_composed<u32string>(U"-0.\u17cf\u1dfd\u1110\u1168\u11aa\u1109\u1175\u11b8"),
              U"-0.\u17cf\u1dfd\ud187\uc2ed");
    EXPECT_EQ(canonical_composed<u32string>(U"-0.\u17cf\u1dfd\u1110\u1168\u11aa\u1109\u1175\u11b8"),
              U"-0.\u17cf\u1dfd\ud187\uc2ed");
    EXPECT_EQ(canonical_composed<u32string>(U"-0.\u17cf\u1dfd\u1110\u1168\u11aa\u1109\u1175\u11b8"),
              U"-0.\u17cf\u1dfd\ud187\uc2ed");
    EXPECT_EQ(canonical_composed<u32string>(U"-0.\u17cf\u1dfd\u1110\u1168\u11aa\u1109\u1175\u11b8"),
              U"-0.\u17cf\u1dfd\ud187\uc2ed");
    EXPECT_EQ(canonical_composed<u32string>(U"\u17cf\u1dfd\u1110\u1168\u11aa\u1109\u1175\u11b8"),
              U"\u17cf\u1dfd\ud187\uc2ed");
}

TEST(Unicode, HangulCompose) {
    using webpp::unicode::canonical_composed;
    using webpp::unicode::compose_hangul;

    EXPECT_EQ(canonical_composed(0xAC00, 0x11A8), 0xAC01) << compose_hangul(0xAC00, 0x11A8); // 가 + ᆨ = 각
    EXPECT_EQ(canonical_composed(0xAC1C, 0x11B2), 0xAC27) << compose_hangul(0xAC1C, 0x11B2); // 개 + ᆲ = 갥
    EXPECT_EQ(canonical_composed(0xAC1C, 0x11B2), 0xAC27) << compose_hangul(0xAC24, 0x11B2); // 개 + ᆲ = 갧
}

TEST(Unicode, NoCompose) {
    using webpp::unicode::canonical_composed;

    EXPECT_EQ(canonical_composed(0x925, 0x0020), webpp::unicode::replacement_char<>);
    EXPECT_EQ(canonical_composed(0, 0), webpp::unicode::replacement_char<>);
    EXPECT_EQ(canonical_composed(1, 0), webpp::unicode::replacement_char<>);
    EXPECT_EQ(canonical_composed(std::numeric_limits<std::uint32_t>::max(), 0U),
              webpp::unicode::replacement_char<>);
    EXPECT_EQ(canonical_composed(0U, std::numeric_limits<std::uint32_t>::max()),
              webpp::unicode::replacement_char<>);
    EXPECT_EQ(canonical_composed(-1, 0), webpp::unicode::replacement_char<>);
    EXPECT_NE(canonical_composed(0x594, 0x0020), 0x00A8);
    EXPECT_NE(canonical_composed(0x307, 0x0061), 0x00AA);
    EXPECT_NE(canonical_composed(0x579, 0x0020), 0x00AF);
    EXPECT_NE(canonical_composed(0x741, 0x0032), 0x00B2);
    EXPECT_NE(canonical_composed(0x787, 0x0033), 0x00B3);
    EXPECT_NE(canonical_composed(0x437, 0x0020), 0x00B4);
    EXPECT_NE(canonical_composed(0x333, 0x03BC), 0x00B5);
    EXPECT_NE(canonical_composed(0x779, 0x0020), 0x00B8);
    EXPECT_NE(canonical_composed(0x101, 0x0031), 0x00B9);

    EXPECT_NE(canonical_composed(0x0073, 0x0326), 0x021B);
    EXPECT_NE(canonical_composed(0x0063, 0x0331), 0x1E0F);
    EXPECT_NE(canonical_composed(0x05D4, 0x05B9), 0xFB4B);
    EXPECT_NE(canonical_composed(0x0054, 0x0308), 0x00DC);
    EXPECT_NE(canonical_composed(0x05EB, 0x05BC), 0xFB4A);
    EXPECT_NE(canonical_composed(0x0F70, 0x0F72), 0x0F73);
    EXPECT_NE(canonical_composed(0x03C1, 0x0315), 0x1FE5);
    EXPECT_NE(canonical_composed(0x00A8, 0x0300), 0x0385);
    EXPECT_NE(canonical_composed(0x0079, 0x0302), 0x00FD);
    EXPECT_NE(canonical_composed(0x22B4, 0x0339), 0x22EC);
}

namespace {
    std::u32string convertToU32String(std::string const& hexString) {
        std::u32string     result;
        std::istringstream iss{hexString};
        std::uint32_t      hexCodePoint = 0;
        while (iss >> std::hex >> hexCodePoint) {
            result += static_cast<char32_t>(hexCodePoint);
            // std::cout << "---> " << hexString << " --- " << std::hex << hexCodePoint << std::endl;
        }
        return result;
    }

    std::string u32ToString(std::u32string const& hexString) {
        std::ostringstream oss;
        for (auto const codePoint : hexString) {
            oss << "\\x" << std::hex << static_cast<std::uint32_t>(codePoint);
        }
        return oss.str();
    }

    void check_idempotent(auto const& str, auto const& nfc, auto const& nfd) {
        using webpp::unicode::toNFC;
        using webpp::unicode::toNFD;

        // toNFC
        EXPECT_EQ(toNFC(str), toNFC(toNFC(str)))
          << "  Src: " << u32ToString(str) << "\n  NFC Layer 1: " << u32ToString(toNFC(str))
          << "\n  NFC Answer: " << u32ToString(nfc) << "\n  NFD Answer: " << u32ToString(nfd);
        EXPECT_EQ(toNFC(str), toNFC(toNFD(str)))
          << "  NFD: " << u32ToString(toNFD(str)) << "\n  Source: " << u32ToString(str)
          << "\n  NFC Answer: " << u32ToString(nfc) << "\n  NFD Answer: " << u32ToString(nfd);

        // toNFD
        EXPECT_EQ(toNFD(str), toNFD(toNFC(str)))
          << "  NFC: " << u32ToString(toNFC(str)) << "\n  Source: " << u32ToString(str)
          << "\n  NFC Answer: " << u32ToString(nfc) << "\n  NFD Answer: " << u32ToString(nfd);
        EXPECT_EQ(toNFD(str), toNFD(toNFD(str)))
          << "  Src: " << u32ToString(str) << "\n  NFD Layer 1: " << u32ToString(toNFD(str))
          << "\n  NFC Answer: " << u32ToString(nfc) << "\n  NFD Answer: " << u32ToString(nfd);

        // toNFKC
        // EXPECT_EQ(toNFKC(str), toNFC(toNFKC(str)));
        // EXPECT_EQ(toNFKC(str), toNFC(toNFKD(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFC(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFD(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFKC(str)));
        // EXPECT_EQ(toNFKC(str), toNFKC(toNFKD(str)));

        // toNFKD
        // EXPECT_EQ(toNFKD(str), toNFD(toNFKC(str)));
        // EXPECT_EQ(toNFKD(str), toNFD(toNFKD(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFC(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFD(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFKC(str)));
        // EXPECT_EQ(toNFKD(str), toNFKD(toNFKD(str)));
    }
} // namespace

TEST(Unicode, NormalizationTests) {
    using std::u32string;
    using std::u32string_view;
    using std::u8string;
    using std::u8string_view;
    using webpp::unicode::toNFC;
    using webpp::unicode::toNFD;
    // in the table from https://www.unicode.org/reports/tr15/#Design_Goals


    // NOLINTBEGIN(*-easily-swappable-parameters)
    auto const check =
      [](u32string const&                  source,
         u32string const&                  nfc,
         u32string const&                  nfd,
         [[maybe_unused]] u32string const& nfkc,
         [[maybe_unused]] u32string const& nfkd,
         std::string const&                line) {
          // NOLINTEND(*-easily-swappable-parameters)

          u8string const source8 = utf32_to_utf8(source);
          u8string const nfc8    = utf32_to_utf8(nfc);
          u8string const nfd8    = utf32_to_utf8(nfd);

          EXPECT_EQ(nfd, toNFD(source))
            << "  Source: " << u32ToString(source) << "\n  NFD: " << u32ToString(nfd)
            << "\n  NFC: " << u32ToString(nfc) << "\n  line: " << line;

          if constexpr (enable_utf8_composition_tests) {
              EXPECT_EQ(nfd8, toNFD(source8))
                << "  Source: " << u32ToString(source) << "  Source: " << u32ToString(source)
                << "\n  NFD: " << u32ToString(nfd) << "\n  NFC: " << u32ToString(nfc) << "\n  line: " << line;
          }

          EXPECT_EQ(nfc, toNFC(source))
            << "  Source: " << u32ToString(source) << "\n  NFD: " << u32ToString(nfd)
            << "\n  NFC: " << u32ToString(nfc) << "\n  line: " << line
            << "\n Calculated NFD: " << u32ToString(toNFD(source));

          if constexpr (enable_utf8_composition_tests) {
              EXPECT_EQ(nfc8, toNFC(source8))
                << "  Source: " << u32ToString(source) << "  Source: " << u32ToString(source)
                << "\n  NFD: " << u32ToString(nfd) << "\n  NFC: " << u32ToString(nfc) << "\n  line: " << line;
          }

          check_idempotent(source, nfc, nfd);

          if constexpr (enable_utf8_composition_tests) {
              check_idempotent(source8, nfc, nfd);
          }
      };

    // special cases:
    EXPECT_EQ(canonical_decomposed<u32string>(U'\u1e0a'), U"D\x307") << "Ḋ";
    EXPECT_EQ(canonical_decomposed<u32string>(u32string_view{U"\x1e0a"}), U"D\x307") << "Ḋ";
    check_idempotent(u32string{U"\u00b5"}, u32string{}, u32string{});


    std::filesystem::path const cur_file   = __FILE__;
    std::filesystem::path       file_path  = cur_file.parent_path();
    file_path                             /= "assets/NormalizationTest.txt";

    std::ifstream file(file_path);
    ASSERT_TRUE(file.is_open()) << "Error opening file: " << file_path;

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines or comments
        auto const lineCpy   = line;
        auto const hashStart = line.find_first_of('#');
        if (hashStart != std::string::npos) {
            line.resize(hashStart);
        }
        if (line.empty() || line[0] == '@') {
            continue;
        }

        std::istringstream          iss(line);
        std::string                 column;
        std::vector<std::u32string> columns(5);

        // Read the columns separated by semicolons
        std::size_t columnIndex = 0;
        while (std::getline(iss, column, ';') && columnIndex != 5) {
            if (column.starts_with(" #")) {
                break;
            }
            // std::cout << "Column: " << columnIndex << " | " << line << std::endl;
            columns[columnIndex++] = convertToU32String(column);
        }

        // Run the check function
        check(columns[0], columns[1], columns[2], columns[3], columns[4], lineCpy);
    }

    file.close();
}

// NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
