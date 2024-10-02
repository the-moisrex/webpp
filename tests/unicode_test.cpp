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

static constexpr bool enable_utf8_composition_tests = false; // todo

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
    //  UTF-32 version:
    //   awk 'BEGIN{FS=";"; OF=";"} !/^\s*#|</ {tst = " " $6; gsub(/\s+/, " ", tst); if (tst != " ") { gsub(/\s+/, "\\\x", tst); print "    EXPECT_EQ(canonical_decomposed<u32string>(U\'\\\x" $1 "\'), U\"" tst "\") << desc_decomp_of(U\'\\\x" $1 "\') << \"\\\n  Line: " $0 "\";"; } }' UnicodeData.txt
    // clang-format on

    static_assert(webpp::stl::same_as<char8_t, webpp::istl::appendable_value_type_t<u8string*>>,
                  "invalid value type");

    // special
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1D'), U"\x2A600") << desc_decomp_of(U'\xD590');
    EXPECT_EQ(canonical_decomposed<u8string>(U'\xD590'), utf32_to_utf8(U"\x1112\x1163"))
      << desc_decomp_of(U'\xD590');
    EXPECT_EQ(toNFD(u32string{U'\xD590'}), u32string{U"\x1112\x1163"}) << desc_decomp_of(U'\xD590');

    // start
    EXPECT_EQ(canonical_decomposed<u8string>(U'\0'), utf32_to_utf8(u32string{U"\0", 1}))
      << desc_decomp_of(U'\0');
    EXPECT_EQ(canonical_decomposed<u8string>(U'\1'), utf32_to_utf8(U"\1")) << desc_decomp_of(U'\1');

    // Canonical Decomposition start:
    // clang-format off
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C0'), U"\x0041\x0300") << desc_decomp_of(U'\x00C0') << "\n  Line: 00C0;LATIN CAPITAL LETTER A WITH GRAVE;Lu;0;L;0041 0300;;;;N;LATIN CAPITAL LETTER A GRAVE;;;00E0;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C1'), U"\x0041\x0301") << desc_decomp_of(U'\x00C1') << "\n  Line: 00C1;LATIN CAPITAL LETTER A WITH ACUTE;Lu;0;L;0041 0301;;;;N;LATIN CAPITAL LETTER A ACUTE;;;00E1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C2'), U"\x0041\x0302") << desc_decomp_of(U'\x00C2') << "\n  Line: 00C2;LATIN CAPITAL LETTER A WITH CIRCUMFLEX;Lu;0;L;0041 0302;;;;N;LATIN CAPITAL LETTER A CIRCUMFLEX;;;00E2;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C3'), U"\x0041\x0303") << desc_decomp_of(U'\x00C3') << "\n  Line: 00C3;LATIN CAPITAL LETTER A WITH TILDE;Lu;0;L;0041 0303;;;;N;LATIN CAPITAL LETTER A TILDE;;;00E3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C4'), U"\x0041\x0308") << desc_decomp_of(U'\x00C4') << "\n  Line: 00C4;LATIN CAPITAL LETTER A WITH DIAERESIS;Lu;0;L;0041 0308;;;;N;LATIN CAPITAL LETTER A DIAERESIS;;;00E4;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C5'), U"\x0041\x030A") << desc_decomp_of(U'\x00C5') << "\n  Line: 00C5;LATIN CAPITAL LETTER A WITH RING ABOVE;Lu;0;L;0041 030A;;;;N;LATIN CAPITAL LETTER A RING;;;00E5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C7'), U"\x0043\x0327") << desc_decomp_of(U'\x00C7') << "\n  Line: 00C7;LATIN CAPITAL LETTER C WITH CEDILLA;Lu;0;L;0043 0327;;;;N;LATIN CAPITAL LETTER C CEDILLA;;;00E7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C8'), U"\x0045\x0300") << desc_decomp_of(U'\x00C8') << "\n  Line: 00C8;LATIN CAPITAL LETTER E WITH GRAVE;Lu;0;L;0045 0300;;;;N;LATIN CAPITAL LETTER E GRAVE;;;00E8;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C9'), U"\x0045\x0301") << desc_decomp_of(U'\x00C9') << "\n  Line: 00C9;LATIN CAPITAL LETTER E WITH ACUTE;Lu;0;L;0045 0301;;;;N;LATIN CAPITAL LETTER E ACUTE;;;00E9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00CA'), U"\x0045\x0302") << desc_decomp_of(U'\x00CA') << "\n  Line: 00CA;LATIN CAPITAL LETTER E WITH CIRCUMFLEX;Lu;0;L;0045 0302;;;;N;LATIN CAPITAL LETTER E CIRCUMFLEX;;;00EA;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00CB'), U"\x0045\x0308") << desc_decomp_of(U'\x00CB') << "\n  Line: 00CB;LATIN CAPITAL LETTER E WITH DIAERESIS;Lu;0;L;0045 0308;;;;N;LATIN CAPITAL LETTER E DIAERESIS;;;00EB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00CC'), U"\x0049\x0300") << desc_decomp_of(U'\x00CC') << "\n  Line: 00CC;LATIN CAPITAL LETTER I WITH GRAVE;Lu;0;L;0049 0300;;;;N;LATIN CAPITAL LETTER I GRAVE;;;00EC;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00CD'), U"\x0049\x0301") << desc_decomp_of(U'\x00CD') << "\n  Line: 00CD;LATIN CAPITAL LETTER I WITH ACUTE;Lu;0;L;0049 0301;;;;N;LATIN CAPITAL LETTER I ACUTE;;;00ED;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00CE'), U"\x0049\x0302") << desc_decomp_of(U'\x00CE') << "\n  Line: 00CE;LATIN CAPITAL LETTER I WITH CIRCUMFLEX;Lu;0;L;0049 0302;;;;N;LATIN CAPITAL LETTER I CIRCUMFLEX;;;00EE;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00CF'), U"\x0049\x0308") << desc_decomp_of(U'\x00CF') << "\n  Line: 00CF;LATIN CAPITAL LETTER I WITH DIAERESIS;Lu;0;L;0049 0308;;;;N;LATIN CAPITAL LETTER I DIAERESIS;;;00EF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00D1'), U"\x004E\x0303") << desc_decomp_of(U'\x00D1') << "\n  Line: 00D1;LATIN CAPITAL LETTER N WITH TILDE;Lu;0;L;004E 0303;;;;N;LATIN CAPITAL LETTER N TILDE;;;00F1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00D2'), U"\x004F\x0300") << desc_decomp_of(U'\x00D2') << "\n  Line: 00D2;LATIN CAPITAL LETTER O WITH GRAVE;Lu;0;L;004F 0300;;;;N;LATIN CAPITAL LETTER O GRAVE;;;00F2;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00D3'), U"\x004F\x0301") << desc_decomp_of(U'\x00D3') << "\n  Line: 00D3;LATIN CAPITAL LETTER O WITH ACUTE;Lu;0;L;004F 0301;;;;N;LATIN CAPITAL LETTER O ACUTE;;;00F3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00D4'), U"\x004F\x0302") << desc_decomp_of(U'\x00D4') << "\n  Line: 00D4;LATIN CAPITAL LETTER O WITH CIRCUMFLEX;Lu;0;L;004F 0302;;;;N;LATIN CAPITAL LETTER O CIRCUMFLEX;;;00F4;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00D5'), U"\x004F\x0303") << desc_decomp_of(U'\x00D5') << "\n  Line: 00D5;LATIN CAPITAL LETTER O WITH TILDE;Lu;0;L;004F 0303;;;;N;LATIN CAPITAL LETTER O TILDE;;;00F5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00D6'), U"\x004F\x0308") << desc_decomp_of(U'\x00D6') << "\n  Line: 00D6;LATIN CAPITAL LETTER O WITH DIAERESIS;Lu;0;L;004F 0308;;;;N;LATIN CAPITAL LETTER O DIAERESIS;;;00F6;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00D9'), U"\x0055\x0300") << desc_decomp_of(U'\x00D9') << "\n  Line: 00D9;LATIN CAPITAL LETTER U WITH GRAVE;Lu;0;L;0055 0300;;;;N;LATIN CAPITAL LETTER U GRAVE;;;00F9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00DA'), U"\x0055\x0301") << desc_decomp_of(U'\x00DA') << "\n  Line: 00DA;LATIN CAPITAL LETTER U WITH ACUTE;Lu;0;L;0055 0301;;;;N;LATIN CAPITAL LETTER U ACUTE;;;00FA;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00DB'), U"\x0055\x0302") << desc_decomp_of(U'\x00DB') << "\n  Line: 00DB;LATIN CAPITAL LETTER U WITH CIRCUMFLEX;Lu;0;L;0055 0302;;;;N;LATIN CAPITAL LETTER U CIRCUMFLEX;;;00FB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00DC'), U"\x0055\x0308") << desc_decomp_of(U'\x00DC') << "\n  Line: 00DC;LATIN CAPITAL LETTER U WITH DIAERESIS;Lu;0;L;0055 0308;;;;N;LATIN CAPITAL LETTER U DIAERESIS;;;00FC;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00DD'), U"\x0059\x0301") << desc_decomp_of(U'\x00DD') << "\n  Line: 00DD;LATIN CAPITAL LETTER Y WITH ACUTE;Lu;0;L;0059 0301;;;;N;LATIN CAPITAL LETTER Y ACUTE;;;00FD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E0'), U"\x0061\x0300") << desc_decomp_of(U'\x00E0') << "\n  Line: 00E0;LATIN SMALL LETTER A WITH GRAVE;Ll;0;L;0061 0300;;;;N;LATIN SMALL LETTER A GRAVE;;00C0;;00C0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E1'), U"\x0061\x0301") << desc_decomp_of(U'\x00E1') << "\n  Line: 00E1;LATIN SMALL LETTER A WITH ACUTE;Ll;0;L;0061 0301;;;;N;LATIN SMALL LETTER A ACUTE;;00C1;;00C1";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E2'), U"\x0061\x0302") << desc_decomp_of(U'\x00E2') << "\n  Line: 00E2;LATIN SMALL LETTER A WITH CIRCUMFLEX;Ll;0;L;0061 0302;;;;N;LATIN SMALL LETTER A CIRCUMFLEX;;00C2;;00C2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E3'), U"\x0061\x0303") << desc_decomp_of(U'\x00E3') << "\n  Line: 00E3;LATIN SMALL LETTER A WITH TILDE;Ll;0;L;0061 0303;;;;N;LATIN SMALL LETTER A TILDE;;00C3;;00C3";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E4'), U"\x0061\x0308") << desc_decomp_of(U'\x00E4') << "\n  Line: 00E4;LATIN SMALL LETTER A WITH DIAERESIS;Ll;0;L;0061 0308;;;;N;LATIN SMALL LETTER A DIAERESIS;;00C4;;00C4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E5'), U"\x0061\x030A") << desc_decomp_of(U'\x00E5') << "\n  Line: 00E5;LATIN SMALL LETTER A WITH RING ABOVE;Ll;0;L;0061 030A;;;;N;LATIN SMALL LETTER A RING;;00C5;;00C5";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E7'), U"\x0063\x0327") << desc_decomp_of(U'\x00E7') << "\n  Line: 00E7;LATIN SMALL LETTER C WITH CEDILLA;Ll;0;L;0063 0327;;;;N;LATIN SMALL LETTER C CEDILLA;;00C7;;00C7";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E8'), U"\x0065\x0300") << desc_decomp_of(U'\x00E8') << "\n  Line: 00E8;LATIN SMALL LETTER E WITH GRAVE;Ll;0;L;0065 0300;;;;N;LATIN SMALL LETTER E GRAVE;;00C8;;00C8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00E9'), U"\x0065\x0301") << desc_decomp_of(U'\x00E9') << "\n  Line: 00E9;LATIN SMALL LETTER E WITH ACUTE;Ll;0;L;0065 0301;;;;N;LATIN SMALL LETTER E ACUTE;;00C9;;00C9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00EA'), U"\x0065\x0302") << desc_decomp_of(U'\x00EA') << "\n  Line: 00EA;LATIN SMALL LETTER E WITH CIRCUMFLEX;Ll;0;L;0065 0302;;;;N;LATIN SMALL LETTER E CIRCUMFLEX;;00CA;;00CA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00EB'), U"\x0065\x0308") << desc_decomp_of(U'\x00EB') << "\n  Line: 00EB;LATIN SMALL LETTER E WITH DIAERESIS;Ll;0;L;0065 0308;;;;N;LATIN SMALL LETTER E DIAERESIS;;00CB;;00CB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00EC'), U"\x0069\x0300") << desc_decomp_of(U'\x00EC') << "\n  Line: 00EC;LATIN SMALL LETTER I WITH GRAVE;Ll;0;L;0069 0300;;;;N;LATIN SMALL LETTER I GRAVE;;00CC;;00CC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00ED'), U"\x0069\x0301") << desc_decomp_of(U'\x00ED') << "\n  Line: 00ED;LATIN SMALL LETTER I WITH ACUTE;Ll;0;L;0069 0301;;;;N;LATIN SMALL LETTER I ACUTE;;00CD;;00CD";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00EE'), U"\x0069\x0302") << desc_decomp_of(U'\x00EE') << "\n  Line: 00EE;LATIN SMALL LETTER I WITH CIRCUMFLEX;Ll;0;L;0069 0302;;;;N;LATIN SMALL LETTER I CIRCUMFLEX;;00CE;;00CE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00EF'), U"\x0069\x0308") << desc_decomp_of(U'\x00EF') << "\n  Line: 00EF;LATIN SMALL LETTER I WITH DIAERESIS;Ll;0;L;0069 0308;;;;N;LATIN SMALL LETTER I DIAERESIS;;00CF;;00CF";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00F1'), U"\x006E\x0303") << desc_decomp_of(U'\x00F1') << "\n  Line: 00F1;LATIN SMALL LETTER N WITH TILDE;Ll;0;L;006E 0303;;;;N;LATIN SMALL LETTER N TILDE;;00D1;;00D1";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00F2'), U"\x006F\x0300") << desc_decomp_of(U'\x00F2') << "\n  Line: 00F2;LATIN SMALL LETTER O WITH GRAVE;Ll;0;L;006F 0300;;;;N;LATIN SMALL LETTER O GRAVE;;00D2;;00D2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00F3'), U"\x006F\x0301") << desc_decomp_of(U'\x00F3') << "\n  Line: 00F3;LATIN SMALL LETTER O WITH ACUTE;Ll;0;L;006F 0301;;;;N;LATIN SMALL LETTER O ACUTE;;00D3;;00D3";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00F4'), U"\x006F\x0302") << desc_decomp_of(U'\x00F4') << "\n  Line: 00F4;LATIN SMALL LETTER O WITH CIRCUMFLEX;Ll;0;L;006F 0302;;;;N;LATIN SMALL LETTER O CIRCUMFLEX;;00D4;;00D4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00F5'), U"\x006F\x0303") << desc_decomp_of(U'\x00F5') << "\n  Line: 00F5;LATIN SMALL LETTER O WITH TILDE;Ll;0;L;006F 0303;;;;N;LATIN SMALL LETTER O TILDE;;00D5;;00D5";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00F6'), U"\x006F\x0308") << desc_decomp_of(U'\x00F6') << "\n  Line: 00F6;LATIN SMALL LETTER O WITH DIAERESIS;Ll;0;L;006F 0308;;;;N;LATIN SMALL LETTER O DIAERESIS;;00D6;;00D6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00F9'), U"\x0075\x0300") << desc_decomp_of(U'\x00F9') << "\n  Line: 00F9;LATIN SMALL LETTER U WITH GRAVE;Ll;0;L;0075 0300;;;;N;LATIN SMALL LETTER U GRAVE;;00D9;;00D9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00FA'), U"\x0075\x0301") << desc_decomp_of(U'\x00FA') << "\n  Line: 00FA;LATIN SMALL LETTER U WITH ACUTE;Ll;0;L;0075 0301;;;;N;LATIN SMALL LETTER U ACUTE;;00DA;;00DA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00FB'), U"\x0075\x0302") << desc_decomp_of(U'\x00FB') << "\n  Line: 00FB;LATIN SMALL LETTER U WITH CIRCUMFLEX;Ll;0;L;0075 0302;;;;N;LATIN SMALL LETTER U CIRCUMFLEX;;00DB;;00DB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00FC'), U"\x0075\x0308") << desc_decomp_of(U'\x00FC') << "\n  Line: 00FC;LATIN SMALL LETTER U WITH DIAERESIS;Ll;0;L;0075 0308;;;;N;LATIN SMALL LETTER U DIAERESIS;;00DC;;00DC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00FD'), U"\x0079\x0301") << desc_decomp_of(U'\x00FD') << "\n  Line: 00FD;LATIN SMALL LETTER Y WITH ACUTE;Ll;0;L;0079 0301;;;;N;LATIN SMALL LETTER Y ACUTE;;00DD;;00DD";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00FF'), U"\x0079\x0308") << desc_decomp_of(U'\x00FF') << "\n  Line: 00FF;LATIN SMALL LETTER Y WITH DIAERESIS;Ll;0;L;0079 0308;;;;N;LATIN SMALL LETTER Y DIAERESIS;;0178;;0178";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0100'), U"\x0041\x0304") << desc_decomp_of(U'\x0100') << "\n  Line: 0100;LATIN CAPITAL LETTER A WITH MACRON;Lu;0;L;0041 0304;;;;N;LATIN CAPITAL LETTER A MACRON;;;0101;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0101'), U"\x0061\x0304") << desc_decomp_of(U'\x0101') << "\n  Line: 0101;LATIN SMALL LETTER A WITH MACRON;Ll;0;L;0061 0304;;;;N;LATIN SMALL LETTER A MACRON;;0100;;0100";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0102'), U"\x0041\x0306") << desc_decomp_of(U'\x0102') << "\n  Line: 0102;LATIN CAPITAL LETTER A WITH BREVE;Lu;0;L;0041 0306;;;;N;LATIN CAPITAL LETTER A BREVE;;;0103;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0103'), U"\x0061\x0306") << desc_decomp_of(U'\x0103') << "\n  Line: 0103;LATIN SMALL LETTER A WITH BREVE;Ll;0;L;0061 0306;;;;N;LATIN SMALL LETTER A BREVE;;0102;;0102";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0104'), U"\x0041\x0328") << desc_decomp_of(U'\x0104') << "\n  Line: 0104;LATIN CAPITAL LETTER A WITH OGONEK;Lu;0;L;0041 0328;;;;N;LATIN CAPITAL LETTER A OGONEK;;;0105;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0105'), U"\x0061\x0328") << desc_decomp_of(U'\x0105') << "\n  Line: 0105;LATIN SMALL LETTER A WITH OGONEK;Ll;0;L;0061 0328;;;;N;LATIN SMALL LETTER A OGONEK;;0104;;0104";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0106'), U"\x0043\x0301") << desc_decomp_of(U'\x0106') << "\n  Line: 0106;LATIN CAPITAL LETTER C WITH ACUTE;Lu;0;L;0043 0301;;;;N;LATIN CAPITAL LETTER C ACUTE;;;0107;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0107'), U"\x0063\x0301") << desc_decomp_of(U'\x0107') << "\n  Line: 0107;LATIN SMALL LETTER C WITH ACUTE;Ll;0;L;0063 0301;;;;N;LATIN SMALL LETTER C ACUTE;;0106;;0106";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0108'), U"\x0043\x0302") << desc_decomp_of(U'\x0108') << "\n  Line: 0108;LATIN CAPITAL LETTER C WITH CIRCUMFLEX;Lu;0;L;0043 0302;;;;N;LATIN CAPITAL LETTER C CIRCUMFLEX;;;0109;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0109'), U"\x0063\x0302") << desc_decomp_of(U'\x0109') << "\n  Line: 0109;LATIN SMALL LETTER C WITH CIRCUMFLEX;Ll;0;L;0063 0302;;;;N;LATIN SMALL LETTER C CIRCUMFLEX;;0108;;0108";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x010A'), U"\x0043\x0307") << desc_decomp_of(U'\x010A') << "\n  Line: 010A;LATIN CAPITAL LETTER C WITH DOT ABOVE;Lu;0;L;0043 0307;;;;N;LATIN CAPITAL LETTER C DOT;;;010B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x010B'), U"\x0063\x0307") << desc_decomp_of(U'\x010B') << "\n  Line: 010B;LATIN SMALL LETTER C WITH DOT ABOVE;Ll;0;L;0063 0307;;;;N;LATIN SMALL LETTER C DOT;;010A;;010A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x010C'), U"\x0043\x030C") << desc_decomp_of(U'\x010C') << "\n  Line: 010C;LATIN CAPITAL LETTER C WITH CARON;Lu;0;L;0043 030C;;;;N;LATIN CAPITAL LETTER C HACEK;;;010D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x010D'), U"\x0063\x030C") << desc_decomp_of(U'\x010D') << "\n  Line: 010D;LATIN SMALL LETTER C WITH CARON;Ll;0;L;0063 030C;;;;N;LATIN SMALL LETTER C HACEK;;010C;;010C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x010E'), U"\x0044\x030C") << desc_decomp_of(U'\x010E') << "\n  Line: 010E;LATIN CAPITAL LETTER D WITH CARON;Lu;0;L;0044 030C;;;;N;LATIN CAPITAL LETTER D HACEK;;;010F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x010F'), U"\x0064\x030C") << desc_decomp_of(U'\x010F') << "\n  Line: 010F;LATIN SMALL LETTER D WITH CARON;Ll;0;L;0064 030C;;;;N;LATIN SMALL LETTER D HACEK;;010E;;010E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0112'), U"\x0045\x0304") << desc_decomp_of(U'\x0112') << "\n  Line: 0112;LATIN CAPITAL LETTER E WITH MACRON;Lu;0;L;0045 0304;;;;N;LATIN CAPITAL LETTER E MACRON;;;0113;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0113'), U"\x0065\x0304") << desc_decomp_of(U'\x0113') << "\n  Line: 0113;LATIN SMALL LETTER E WITH MACRON;Ll;0;L;0065 0304;;;;N;LATIN SMALL LETTER E MACRON;;0112;;0112";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0114'), U"\x0045\x0306") << desc_decomp_of(U'\x0114') << "\n  Line: 0114;LATIN CAPITAL LETTER E WITH BREVE;Lu;0;L;0045 0306;;;;N;LATIN CAPITAL LETTER E BREVE;;;0115;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0115'), U"\x0065\x0306") << desc_decomp_of(U'\x0115') << "\n  Line: 0115;LATIN SMALL LETTER E WITH BREVE;Ll;0;L;0065 0306;;;;N;LATIN SMALL LETTER E BREVE;;0114;;0114";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0116'), U"\x0045\x0307") << desc_decomp_of(U'\x0116') << "\n  Line: 0116;LATIN CAPITAL LETTER E WITH DOT ABOVE;Lu;0;L;0045 0307;;;;N;LATIN CAPITAL LETTER E DOT;;;0117;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0117'), U"\x0065\x0307") << desc_decomp_of(U'\x0117') << "\n  Line: 0117;LATIN SMALL LETTER E WITH DOT ABOVE;Ll;0;L;0065 0307;;;;N;LATIN SMALL LETTER E DOT;;0116;;0116";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0118'), U"\x0045\x0328") << desc_decomp_of(U'\x0118') << "\n  Line: 0118;LATIN CAPITAL LETTER E WITH OGONEK;Lu;0;L;0045 0328;;;;N;LATIN CAPITAL LETTER E OGONEK;;;0119;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0119'), U"\x0065\x0328") << desc_decomp_of(U'\x0119') << "\n  Line: 0119;LATIN SMALL LETTER E WITH OGONEK;Ll;0;L;0065 0328;;;;N;LATIN SMALL LETTER E OGONEK;;0118;;0118";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x011A'), U"\x0045\x030C") << desc_decomp_of(U'\x011A') << "\n  Line: 011A;LATIN CAPITAL LETTER E WITH CARON;Lu;0;L;0045 030C;;;;N;LATIN CAPITAL LETTER E HACEK;;;011B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x011B'), U"\x0065\x030C") << desc_decomp_of(U'\x011B') << "\n  Line: 011B;LATIN SMALL LETTER E WITH CARON;Ll;0;L;0065 030C;;;;N;LATIN SMALL LETTER E HACEK;;011A;;011A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x011C'), U"\x0047\x0302") << desc_decomp_of(U'\x011C') << "\n  Line: 011C;LATIN CAPITAL LETTER G WITH CIRCUMFLEX;Lu;0;L;0047 0302;;;;N;LATIN CAPITAL LETTER G CIRCUMFLEX;;;011D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x011D'), U"\x0067\x0302") << desc_decomp_of(U'\x011D') << "\n  Line: 011D;LATIN SMALL LETTER G WITH CIRCUMFLEX;Ll;0;L;0067 0302;;;;N;LATIN SMALL LETTER G CIRCUMFLEX;;011C;;011C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x011E'), U"\x0047\x0306") << desc_decomp_of(U'\x011E') << "\n  Line: 011E;LATIN CAPITAL LETTER G WITH BREVE;Lu;0;L;0047 0306;;;;N;LATIN CAPITAL LETTER G BREVE;;;011F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x011F'), U"\x0067\x0306") << desc_decomp_of(U'\x011F') << "\n  Line: 011F;LATIN SMALL LETTER G WITH BREVE;Ll;0;L;0067 0306;;;;N;LATIN SMALL LETTER G BREVE;;011E;;011E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0120'), U"\x0047\x0307") << desc_decomp_of(U'\x0120') << "\n  Line: 0120;LATIN CAPITAL LETTER G WITH DOT ABOVE;Lu;0;L;0047 0307;;;;N;LATIN CAPITAL LETTER G DOT;;;0121;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0121'), U"\x0067\x0307") << desc_decomp_of(U'\x0121') << "\n  Line: 0121;LATIN SMALL LETTER G WITH DOT ABOVE;Ll;0;L;0067 0307;;;;N;LATIN SMALL LETTER G DOT;;0120;;0120";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0122'), U"\x0047\x0327") << desc_decomp_of(U'\x0122') << "\n  Line: 0122;LATIN CAPITAL LETTER G WITH CEDILLA;Lu;0;L;0047 0327;;;;N;LATIN CAPITAL LETTER G CEDILLA;;;0123;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0123'), U"\x0067\x0327") << desc_decomp_of(U'\x0123') << "\n  Line: 0123;LATIN SMALL LETTER G WITH CEDILLA;Ll;0;L;0067 0327;;;;N;LATIN SMALL LETTER G CEDILLA;;0122;;0122";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0124'), U"\x0048\x0302") << desc_decomp_of(U'\x0124') << "\n  Line: 0124;LATIN CAPITAL LETTER H WITH CIRCUMFLEX;Lu;0;L;0048 0302;;;;N;LATIN CAPITAL LETTER H CIRCUMFLEX;;;0125;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0125'), U"\x0068\x0302") << desc_decomp_of(U'\x0125') << "\n  Line: 0125;LATIN SMALL LETTER H WITH CIRCUMFLEX;Ll;0;L;0068 0302;;;;N;LATIN SMALL LETTER H CIRCUMFLEX;;0124;;0124";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0128'), U"\x0049\x0303") << desc_decomp_of(U'\x0128') << "\n  Line: 0128;LATIN CAPITAL LETTER I WITH TILDE;Lu;0;L;0049 0303;;;;N;LATIN CAPITAL LETTER I TILDE;;;0129;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0129'), U"\x0069\x0303") << desc_decomp_of(U'\x0129') << "\n  Line: 0129;LATIN SMALL LETTER I WITH TILDE;Ll;0;L;0069 0303;;;;N;LATIN SMALL LETTER I TILDE;;0128;;0128";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x012A'), U"\x0049\x0304") << desc_decomp_of(U'\x012A') << "\n  Line: 012A;LATIN CAPITAL LETTER I WITH MACRON;Lu;0;L;0049 0304;;;;N;LATIN CAPITAL LETTER I MACRON;;;012B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x012B'), U"\x0069\x0304") << desc_decomp_of(U'\x012B') << "\n  Line: 012B;LATIN SMALL LETTER I WITH MACRON;Ll;0;L;0069 0304;;;;N;LATIN SMALL LETTER I MACRON;;012A;;012A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x012C'), U"\x0049\x0306") << desc_decomp_of(U'\x012C') << "\n  Line: 012C;LATIN CAPITAL LETTER I WITH BREVE;Lu;0;L;0049 0306;;;;N;LATIN CAPITAL LETTER I BREVE;;;012D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x012D'), U"\x0069\x0306") << desc_decomp_of(U'\x012D') << "\n  Line: 012D;LATIN SMALL LETTER I WITH BREVE;Ll;0;L;0069 0306;;;;N;LATIN SMALL LETTER I BREVE;;012C;;012C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x012E'), U"\x0049\x0328") << desc_decomp_of(U'\x012E') << "\n  Line: 012E;LATIN CAPITAL LETTER I WITH OGONEK;Lu;0;L;0049 0328;;;;N;LATIN CAPITAL LETTER I OGONEK;;;012F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x012F'), U"\x0069\x0328") << desc_decomp_of(U'\x012F') << "\n  Line: 012F;LATIN SMALL LETTER I WITH OGONEK;Ll;0;L;0069 0328;;;;N;LATIN SMALL LETTER I OGONEK;;012E;;012E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0130'), U"\x0049\x0307") << desc_decomp_of(U'\x0130') << "\n  Line: 0130;LATIN CAPITAL LETTER I WITH DOT ABOVE;Lu;0;L;0049 0307;;;;N;LATIN CAPITAL LETTER I DOT;;;0069;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0134'), U"\x004A\x0302") << desc_decomp_of(U'\x0134') << "\n  Line: 0134;LATIN CAPITAL LETTER J WITH CIRCUMFLEX;Lu;0;L;004A 0302;;;;N;LATIN CAPITAL LETTER J CIRCUMFLEX;;;0135;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0135'), U"\x006A\x0302") << desc_decomp_of(U'\x0135') << "\n  Line: 0135;LATIN SMALL LETTER J WITH CIRCUMFLEX;Ll;0;L;006A 0302;;;;N;LATIN SMALL LETTER J CIRCUMFLEX;;0134;;0134";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0136'), U"\x004B\x0327") << desc_decomp_of(U'\x0136') << "\n  Line: 0136;LATIN CAPITAL LETTER K WITH CEDILLA;Lu;0;L;004B 0327;;;;N;LATIN CAPITAL LETTER K CEDILLA;;;0137;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0137'), U"\x006B\x0327") << desc_decomp_of(U'\x0137') << "\n  Line: 0137;LATIN SMALL LETTER K WITH CEDILLA;Ll;0;L;006B 0327;;;;N;LATIN SMALL LETTER K CEDILLA;;0136;;0136";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0139'), U"\x004C\x0301") << desc_decomp_of(U'\x0139') << "\n  Line: 0139;LATIN CAPITAL LETTER L WITH ACUTE;Lu;0;L;004C 0301;;;;N;LATIN CAPITAL LETTER L ACUTE;;;013A;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x013A'), U"\x006C\x0301") << desc_decomp_of(U'\x013A') << "\n  Line: 013A;LATIN SMALL LETTER L WITH ACUTE;Ll;0;L;006C 0301;;;;N;LATIN SMALL LETTER L ACUTE;;0139;;0139";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x013B'), U"\x004C\x0327") << desc_decomp_of(U'\x013B') << "\n  Line: 013B;LATIN CAPITAL LETTER L WITH CEDILLA;Lu;0;L;004C 0327;;;;N;LATIN CAPITAL LETTER L CEDILLA;;;013C;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x013C'), U"\x006C\x0327") << desc_decomp_of(U'\x013C') << "\n  Line: 013C;LATIN SMALL LETTER L WITH CEDILLA;Ll;0;L;006C 0327;;;;N;LATIN SMALL LETTER L CEDILLA;;013B;;013B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x013D'), U"\x004C\x030C") << desc_decomp_of(U'\x013D') << "\n  Line: 013D;LATIN CAPITAL LETTER L WITH CARON;Lu;0;L;004C 030C;;;;N;LATIN CAPITAL LETTER L HACEK;;;013E;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x013E'), U"\x006C\x030C") << desc_decomp_of(U'\x013E') << "\n  Line: 013E;LATIN SMALL LETTER L WITH CARON;Ll;0;L;006C 030C;;;;N;LATIN SMALL LETTER L HACEK;;013D;;013D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0143'), U"\x004E\x0301") << desc_decomp_of(U'\x0143') << "\n  Line: 0143;LATIN CAPITAL LETTER N WITH ACUTE;Lu;0;L;004E 0301;;;;N;LATIN CAPITAL LETTER N ACUTE;;;0144;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0144'), U"\x006E\x0301") << desc_decomp_of(U'\x0144') << "\n  Line: 0144;LATIN SMALL LETTER N WITH ACUTE;Ll;0;L;006E 0301;;;;N;LATIN SMALL LETTER N ACUTE;;0143;;0143";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0145'), U"\x004E\x0327") << desc_decomp_of(U'\x0145') << "\n  Line: 0145;LATIN CAPITAL LETTER N WITH CEDILLA;Lu;0;L;004E 0327;;;;N;LATIN CAPITAL LETTER N CEDILLA;;;0146;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0146'), U"\x006E\x0327") << desc_decomp_of(U'\x0146') << "\n  Line: 0146;LATIN SMALL LETTER N WITH CEDILLA;Ll;0;L;006E 0327;;;;N;LATIN SMALL LETTER N CEDILLA;;0145;;0145";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0147'), U"\x004E\x030C") << desc_decomp_of(U'\x0147') << "\n  Line: 0147;LATIN CAPITAL LETTER N WITH CARON;Lu;0;L;004E 030C;;;;N;LATIN CAPITAL LETTER N HACEK;;;0148;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0148'), U"\x006E\x030C") << desc_decomp_of(U'\x0148') << "\n  Line: 0148;LATIN SMALL LETTER N WITH CARON;Ll;0;L;006E 030C;;;;N;LATIN SMALL LETTER N HACEK;;0147;;0147";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x014C'), U"\x004F\x0304") << desc_decomp_of(U'\x014C') << "\n  Line: 014C;LATIN CAPITAL LETTER O WITH MACRON;Lu;0;L;004F 0304;;;;N;LATIN CAPITAL LETTER O MACRON;;;014D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x014D'), U"\x006F\x0304") << desc_decomp_of(U'\x014D') << "\n  Line: 014D;LATIN SMALL LETTER O WITH MACRON;Ll;0;L;006F 0304;;;;N;LATIN SMALL LETTER O MACRON;;014C;;014C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x014E'), U"\x004F\x0306") << desc_decomp_of(U'\x014E') << "\n  Line: 014E;LATIN CAPITAL LETTER O WITH BREVE;Lu;0;L;004F 0306;;;;N;LATIN CAPITAL LETTER O BREVE;;;014F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x014F'), U"\x006F\x0306") << desc_decomp_of(U'\x014F') << "\n  Line: 014F;LATIN SMALL LETTER O WITH BREVE;Ll;0;L;006F 0306;;;;N;LATIN SMALL LETTER O BREVE;;014E;;014E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0150'), U"\x004F\x030B") << desc_decomp_of(U'\x0150') << "\n  Line: 0150;LATIN CAPITAL LETTER O WITH DOUBLE ACUTE;Lu;0;L;004F 030B;;;;N;LATIN CAPITAL LETTER O DOUBLE ACUTE;;;0151;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0151'), U"\x006F\x030B") << desc_decomp_of(U'\x0151') << "\n  Line: 0151;LATIN SMALL LETTER O WITH DOUBLE ACUTE;Ll;0;L;006F 030B;;;;N;LATIN SMALL LETTER O DOUBLE ACUTE;;0150;;0150";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0154'), U"\x0052\x0301") << desc_decomp_of(U'\x0154') << "\n  Line: 0154;LATIN CAPITAL LETTER R WITH ACUTE;Lu;0;L;0052 0301;;;;N;LATIN CAPITAL LETTER R ACUTE;;;0155;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0155'), U"\x0072\x0301") << desc_decomp_of(U'\x0155') << "\n  Line: 0155;LATIN SMALL LETTER R WITH ACUTE;Ll;0;L;0072 0301;;;;N;LATIN SMALL LETTER R ACUTE;;0154;;0154";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0156'), U"\x0052\x0327") << desc_decomp_of(U'\x0156') << "\n  Line: 0156;LATIN CAPITAL LETTER R WITH CEDILLA;Lu;0;L;0052 0327;;;;N;LATIN CAPITAL LETTER R CEDILLA;;;0157;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0157'), U"\x0072\x0327") << desc_decomp_of(U'\x0157') << "\n  Line: 0157;LATIN SMALL LETTER R WITH CEDILLA;Ll;0;L;0072 0327;;;;N;LATIN SMALL LETTER R CEDILLA;;0156;;0156";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0158'), U"\x0052\x030C") << desc_decomp_of(U'\x0158') << "\n  Line: 0158;LATIN CAPITAL LETTER R WITH CARON;Lu;0;L;0052 030C;;;;N;LATIN CAPITAL LETTER R HACEK;;;0159;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0159'), U"\x0072\x030C") << desc_decomp_of(U'\x0159') << "\n  Line: 0159;LATIN SMALL LETTER R WITH CARON;Ll;0;L;0072 030C;;;;N;LATIN SMALL LETTER R HACEK;;0158;;0158";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x015A'), U"\x0053\x0301") << desc_decomp_of(U'\x015A') << "\n  Line: 015A;LATIN CAPITAL LETTER S WITH ACUTE;Lu;0;L;0053 0301;;;;N;LATIN CAPITAL LETTER S ACUTE;;;015B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x015B'), U"\x0073\x0301") << desc_decomp_of(U'\x015B') << "\n  Line: 015B;LATIN SMALL LETTER S WITH ACUTE;Ll;0;L;0073 0301;;;;N;LATIN SMALL LETTER S ACUTE;;015A;;015A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x015C'), U"\x0053\x0302") << desc_decomp_of(U'\x015C') << "\n  Line: 015C;LATIN CAPITAL LETTER S WITH CIRCUMFLEX;Lu;0;L;0053 0302;;;;N;LATIN CAPITAL LETTER S CIRCUMFLEX;;;015D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x015D'), U"\x0073\x0302") << desc_decomp_of(U'\x015D') << "\n  Line: 015D;LATIN SMALL LETTER S WITH CIRCUMFLEX;Ll;0;L;0073 0302;;;;N;LATIN SMALL LETTER S CIRCUMFLEX;;015C;;015C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x015E'), U"\x0053\x0327") << desc_decomp_of(U'\x015E') << "\n  Line: 015E;LATIN CAPITAL LETTER S WITH CEDILLA;Lu;0;L;0053 0327;;;;N;LATIN CAPITAL LETTER S CEDILLA;;;015F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x015F'), U"\x0073\x0327") << desc_decomp_of(U'\x015F') << "\n  Line: 015F;LATIN SMALL LETTER S WITH CEDILLA;Ll;0;L;0073 0327;;;;N;LATIN SMALL LETTER S CEDILLA;;015E;;015E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0160'), U"\x0053\x030C") << desc_decomp_of(U'\x0160') << "\n  Line: 0160;LATIN CAPITAL LETTER S WITH CARON;Lu;0;L;0053 030C;;;;N;LATIN CAPITAL LETTER S HACEK;;;0161;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0161'), U"\x0073\x030C") << desc_decomp_of(U'\x0161') << "\n  Line: 0161;LATIN SMALL LETTER S WITH CARON;Ll;0;L;0073 030C;;;;N;LATIN SMALL LETTER S HACEK;;0160;;0160";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0162'), U"\x0054\x0327") << desc_decomp_of(U'\x0162') << "\n  Line: 0162;LATIN CAPITAL LETTER T WITH CEDILLA;Lu;0;L;0054 0327;;;;N;LATIN CAPITAL LETTER T CEDILLA;;;0163;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0163'), U"\x0074\x0327") << desc_decomp_of(U'\x0163') << "\n  Line: 0163;LATIN SMALL LETTER T WITH CEDILLA;Ll;0;L;0074 0327;;;;N;LATIN SMALL LETTER T CEDILLA;;0162;;0162";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0164'), U"\x0054\x030C") << desc_decomp_of(U'\x0164') << "\n  Line: 0164;LATIN CAPITAL LETTER T WITH CARON;Lu;0;L;0054 030C;;;;N;LATIN CAPITAL LETTER T HACEK;;;0165;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0165'), U"\x0074\x030C") << desc_decomp_of(U'\x0165') << "\n  Line: 0165;LATIN SMALL LETTER T WITH CARON;Ll;0;L;0074 030C;;;;N;LATIN SMALL LETTER T HACEK;;0164;;0164";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0168'), U"\x0055\x0303") << desc_decomp_of(U'\x0168') << "\n  Line: 0168;LATIN CAPITAL LETTER U WITH TILDE;Lu;0;L;0055 0303;;;;N;LATIN CAPITAL LETTER U TILDE;;;0169;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0169'), U"\x0075\x0303") << desc_decomp_of(U'\x0169') << "\n  Line: 0169;LATIN SMALL LETTER U WITH TILDE;Ll;0;L;0075 0303;;;;N;LATIN SMALL LETTER U TILDE;;0168;;0168";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x016A'), U"\x0055\x0304") << desc_decomp_of(U'\x016A') << "\n  Line: 016A;LATIN CAPITAL LETTER U WITH MACRON;Lu;0;L;0055 0304;;;;N;LATIN CAPITAL LETTER U MACRON;;;016B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x016B'), U"\x0075\x0304") << desc_decomp_of(U'\x016B') << "\n  Line: 016B;LATIN SMALL LETTER U WITH MACRON;Ll;0;L;0075 0304;;;;N;LATIN SMALL LETTER U MACRON;;016A;;016A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x016C'), U"\x0055\x0306") << desc_decomp_of(U'\x016C') << "\n  Line: 016C;LATIN CAPITAL LETTER U WITH BREVE;Lu;0;L;0055 0306;;;;N;LATIN CAPITAL LETTER U BREVE;;;016D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x016D'), U"\x0075\x0306") << desc_decomp_of(U'\x016D') << "\n  Line: 016D;LATIN SMALL LETTER U WITH BREVE;Ll;0;L;0075 0306;;;;N;LATIN SMALL LETTER U BREVE;;016C;;016C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x016E'), U"\x0055\x030A") << desc_decomp_of(U'\x016E') << "\n  Line: 016E;LATIN CAPITAL LETTER U WITH RING ABOVE;Lu;0;L;0055 030A;;;;N;LATIN CAPITAL LETTER U RING;;;016F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x016F'), U"\x0075\x030A") << desc_decomp_of(U'\x016F') << "\n  Line: 016F;LATIN SMALL LETTER U WITH RING ABOVE;Ll;0;L;0075 030A;;;;N;LATIN SMALL LETTER U RING;;016E;;016E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0170'), U"\x0055\x030B") << desc_decomp_of(U'\x0170') << "\n  Line: 0170;LATIN CAPITAL LETTER U WITH DOUBLE ACUTE;Lu;0;L;0055 030B;;;;N;LATIN CAPITAL LETTER U DOUBLE ACUTE;;;0171;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0171'), U"\x0075\x030B") << desc_decomp_of(U'\x0171') << "\n  Line: 0171;LATIN SMALL LETTER U WITH DOUBLE ACUTE;Ll;0;L;0075 030B;;;;N;LATIN SMALL LETTER U DOUBLE ACUTE;;0170;;0170";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0172'), U"\x0055\x0328") << desc_decomp_of(U'\x0172') << "\n  Line: 0172;LATIN CAPITAL LETTER U WITH OGONEK;Lu;0;L;0055 0328;;;;N;LATIN CAPITAL LETTER U OGONEK;;;0173;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0173'), U"\x0075\x0328") << desc_decomp_of(U'\x0173') << "\n  Line: 0173;LATIN SMALL LETTER U WITH OGONEK;Ll;0;L;0075 0328;;;;N;LATIN SMALL LETTER U OGONEK;;0172;;0172";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0174'), U"\x0057\x0302") << desc_decomp_of(U'\x0174') << "\n  Line: 0174;LATIN CAPITAL LETTER W WITH CIRCUMFLEX;Lu;0;L;0057 0302;;;;N;LATIN CAPITAL LETTER W CIRCUMFLEX;;;0175;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0175'), U"\x0077\x0302") << desc_decomp_of(U'\x0175') << "\n  Line: 0175;LATIN SMALL LETTER W WITH CIRCUMFLEX;Ll;0;L;0077 0302;;;;N;LATIN SMALL LETTER W CIRCUMFLEX;;0174;;0174";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0176'), U"\x0059\x0302") << desc_decomp_of(U'\x0176') << "\n  Line: 0176;LATIN CAPITAL LETTER Y WITH CIRCUMFLEX;Lu;0;L;0059 0302;;;;N;LATIN CAPITAL LETTER Y CIRCUMFLEX;;;0177;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0177'), U"\x0079\x0302") << desc_decomp_of(U'\x0177') << "\n  Line: 0177;LATIN SMALL LETTER Y WITH CIRCUMFLEX;Ll;0;L;0079 0302;;;;N;LATIN SMALL LETTER Y CIRCUMFLEX;;0176;;0176";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0178'), U"\x0059\x0308") << desc_decomp_of(U'\x0178') << "\n  Line: 0178;LATIN CAPITAL LETTER Y WITH DIAERESIS;Lu;0;L;0059 0308;;;;N;LATIN CAPITAL LETTER Y DIAERESIS;;;00FF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0179'), U"\x005A\x0301") << desc_decomp_of(U'\x0179') << "\n  Line: 0179;LATIN CAPITAL LETTER Z WITH ACUTE;Lu;0;L;005A 0301;;;;N;LATIN CAPITAL LETTER Z ACUTE;;;017A;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x017A'), U"\x007A\x0301") << desc_decomp_of(U'\x017A') << "\n  Line: 017A;LATIN SMALL LETTER Z WITH ACUTE;Ll;0;L;007A 0301;;;;N;LATIN SMALL LETTER Z ACUTE;;0179;;0179";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x017B'), U"\x005A\x0307") << desc_decomp_of(U'\x017B') << "\n  Line: 017B;LATIN CAPITAL LETTER Z WITH DOT ABOVE;Lu;0;L;005A 0307;;;;N;LATIN CAPITAL LETTER Z DOT;;;017C;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x017C'), U"\x007A\x0307") << desc_decomp_of(U'\x017C') << "\n  Line: 017C;LATIN SMALL LETTER Z WITH DOT ABOVE;Ll;0;L;007A 0307;;;;N;LATIN SMALL LETTER Z DOT;;017B;;017B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x017D'), U"\x005A\x030C") << desc_decomp_of(U'\x017D') << "\n  Line: 017D;LATIN CAPITAL LETTER Z WITH CARON;Lu;0;L;005A 030C;;;;N;LATIN CAPITAL LETTER Z HACEK;;;017E;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x017E'), U"\x007A\x030C") << desc_decomp_of(U'\x017E') << "\n  Line: 017E;LATIN SMALL LETTER Z WITH CARON;Ll;0;L;007A 030C;;;;N;LATIN SMALL LETTER Z HACEK;;017D;;017D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01A0'), U"\x004F\x031B") << desc_decomp_of(U'\x01A0') << "\n  Line: 01A0;LATIN CAPITAL LETTER O WITH HORN;Lu;0;L;004F 031B;;;;N;LATIN CAPITAL LETTER O HORN;;;01A1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01A1'), U"\x006F\x031B") << desc_decomp_of(U'\x01A1') << "\n  Line: 01A1;LATIN SMALL LETTER O WITH HORN;Ll;0;L;006F 031B;;;;N;LATIN SMALL LETTER O HORN;;01A0;;01A0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01AF'), U"\x0055\x031B") << desc_decomp_of(U'\x01AF') << "\n  Line: 01AF;LATIN CAPITAL LETTER U WITH HORN;Lu;0;L;0055 031B;;;;N;LATIN CAPITAL LETTER U HORN;;;01B0;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01B0'), U"\x0075\x031B") << desc_decomp_of(U'\x01B0') << "\n  Line: 01B0;LATIN SMALL LETTER U WITH HORN;Ll;0;L;0075 031B;;;;N;LATIN SMALL LETTER U HORN;;01AF;;01AF";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01CD'), U"\x0041\x030C") << desc_decomp_of(U'\x01CD') << "\n  Line: 01CD;LATIN CAPITAL LETTER A WITH CARON;Lu;0;L;0041 030C;;;;N;LATIN CAPITAL LETTER A HACEK;;;01CE;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01CE'), U"\x0061\x030C") << desc_decomp_of(U'\x01CE') << "\n  Line: 01CE;LATIN SMALL LETTER A WITH CARON;Ll;0;L;0061 030C;;;;N;LATIN SMALL LETTER A HACEK;;01CD;;01CD";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01CF'), U"\x0049\x030C") << desc_decomp_of(U'\x01CF') << "\n  Line: 01CF;LATIN CAPITAL LETTER I WITH CARON;Lu;0;L;0049 030C;;;;N;LATIN CAPITAL LETTER I HACEK;;;01D0;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D0'), U"\x0069\x030C") << desc_decomp_of(U'\x01D0') << "\n  Line: 01D0;LATIN SMALL LETTER I WITH CARON;Ll;0;L;0069 030C;;;;N;LATIN SMALL LETTER I HACEK;;01CF;;01CF";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D1'), U"\x004F\x030C") << desc_decomp_of(U'\x01D1') << "\n  Line: 01D1;LATIN CAPITAL LETTER O WITH CARON;Lu;0;L;004F 030C;;;;N;LATIN CAPITAL LETTER O HACEK;;;01D2;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D2'), U"\x006F\x030C") << desc_decomp_of(U'\x01D2') << "\n  Line: 01D2;LATIN SMALL LETTER O WITH CARON;Ll;0;L;006F 030C;;;;N;LATIN SMALL LETTER O HACEK;;01D1;;01D1";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D3'), U"\x0055\x030C") << desc_decomp_of(U'\x01D3') << "\n  Line: 01D3;LATIN CAPITAL LETTER U WITH CARON;Lu;0;L;0055 030C;;;;N;LATIN CAPITAL LETTER U HACEK;;;01D4;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D4'), U"\x0075\x030C") << desc_decomp_of(U'\x01D4') << "\n  Line: 01D4;LATIN SMALL LETTER U WITH CARON;Ll;0;L;0075 030C;;;;N;LATIN SMALL LETTER U HACEK;;01D3;;01D3";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D5'), U"\x00DC\x0304") << desc_decomp_of(U'\x01D5') << "\n  Line: 01D5;LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON;Lu;0;L;00DC 0304;;;;N;LATIN CAPITAL LETTER U DIAERESIS MACRON;;;01D6;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D6'), U"\x00FC\x0304") << desc_decomp_of(U'\x01D6') << "\n  Line: 01D6;LATIN SMALL LETTER U WITH DIAERESIS AND MACRON;Ll;0;L;00FC 0304;;;;N;LATIN SMALL LETTER U DIAERESIS MACRON;;01D5;;01D5";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D7'), U"\x00DC\x0301") << desc_decomp_of(U'\x01D7') << "\n  Line: 01D7;LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE;Lu;0;L;00DC 0301;;;;N;LATIN CAPITAL LETTER U DIAERESIS ACUTE;;;01D8;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D8'), U"\x00FC\x0301") << desc_decomp_of(U'\x01D8') << "\n  Line: 01D8;LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE;Ll;0;L;00FC 0301;;;;N;LATIN SMALL LETTER U DIAERESIS ACUTE;;01D7;;01D7";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01D9'), U"\x00DC\x030C") << desc_decomp_of(U'\x01D9') << "\n  Line: 01D9;LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON;Lu;0;L;00DC 030C;;;;N;LATIN CAPITAL LETTER U DIAERESIS HACEK;;;01DA;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01DA'), U"\x00FC\x030C") << desc_decomp_of(U'\x01DA') << "\n  Line: 01DA;LATIN SMALL LETTER U WITH DIAERESIS AND CARON;Ll;0;L;00FC 030C;;;;N;LATIN SMALL LETTER U DIAERESIS HACEK;;01D9;;01D9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01DB'), U"\x00DC\x0300") << desc_decomp_of(U'\x01DB') << "\n  Line: 01DB;LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE;Lu;0;L;00DC 0300;;;;N;LATIN CAPITAL LETTER U DIAERESIS GRAVE;;;01DC;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01DC'), U"\x00FC\x0300") << desc_decomp_of(U'\x01DC') << "\n  Line: 01DC;LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE;Ll;0;L;00FC 0300;;;;N;LATIN SMALL LETTER U DIAERESIS GRAVE;;01DB;;01DB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01DE'), U"\x00C4\x0304") << desc_decomp_of(U'\x01DE') << "\n  Line: 01DE;LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON;Lu;0;L;00C4 0304;;;;N;LATIN CAPITAL LETTER A DIAERESIS MACRON;;;01DF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01DF'), U"\x00E4\x0304") << desc_decomp_of(U'\x01DF') << "\n  Line: 01DF;LATIN SMALL LETTER A WITH DIAERESIS AND MACRON;Ll;0;L;00E4 0304;;;;N;LATIN SMALL LETTER A DIAERESIS MACRON;;01DE;;01DE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01E0'), U"\x0226\x0304") << desc_decomp_of(U'\x01E0') << "\n  Line: 01E0;LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON;Lu;0;L;0226 0304;;;;N;LATIN CAPITAL LETTER A DOT MACRON;;;01E1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01E1'), U"\x0227\x0304") << desc_decomp_of(U'\x01E1') << "\n  Line: 01E1;LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON;Ll;0;L;0227 0304;;;;N;LATIN SMALL LETTER A DOT MACRON;;01E0;;01E0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01E2'), U"\x00C6\x0304") << desc_decomp_of(U'\x01E2') << "\n  Line: 01E2;LATIN CAPITAL LETTER AE WITH MACRON;Lu;0;L;00C6 0304;;;;N;LATIN CAPITAL LETTER A E MACRON;;;01E3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01E3'), U"\x00E6\x0304") << desc_decomp_of(U'\x01E3') << "\n  Line: 01E3;LATIN SMALL LETTER AE WITH MACRON;Ll;0;L;00E6 0304;;;;N;LATIN SMALL LETTER A E MACRON;;01E2;;01E2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01E6'), U"\x0047\x030C") << desc_decomp_of(U'\x01E6') << "\n  Line: 01E6;LATIN CAPITAL LETTER G WITH CARON;Lu;0;L;0047 030C;;;;N;LATIN CAPITAL LETTER G HACEK;;;01E7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01E7'), U"\x0067\x030C") << desc_decomp_of(U'\x01E7') << "\n  Line: 01E7;LATIN SMALL LETTER G WITH CARON;Ll;0;L;0067 030C;;;;N;LATIN SMALL LETTER G HACEK;;01E6;;01E6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01E8'), U"\x004B\x030C") << desc_decomp_of(U'\x01E8') << "\n  Line: 01E8;LATIN CAPITAL LETTER K WITH CARON;Lu;0;L;004B 030C;;;;N;LATIN CAPITAL LETTER K HACEK;;;01E9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01E9'), U"\x006B\x030C") << desc_decomp_of(U'\x01E9') << "\n  Line: 01E9;LATIN SMALL LETTER K WITH CARON;Ll;0;L;006B 030C;;;;N;LATIN SMALL LETTER K HACEK;;01E8;;01E8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01EA'), U"\x004F\x0328") << desc_decomp_of(U'\x01EA') << "\n  Line: 01EA;LATIN CAPITAL LETTER O WITH OGONEK;Lu;0;L;004F 0328;;;;N;LATIN CAPITAL LETTER O OGONEK;;;01EB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01EB'), U"\x006F\x0328") << desc_decomp_of(U'\x01EB') << "\n  Line: 01EB;LATIN SMALL LETTER O WITH OGONEK;Ll;0;L;006F 0328;;;;N;LATIN SMALL LETTER O OGONEK;;01EA;;01EA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01EC'), U"\x01EA\x0304") << desc_decomp_of(U'\x01EC') << "\n  Line: 01EC;LATIN CAPITAL LETTER O WITH OGONEK AND MACRON;Lu;0;L;01EA 0304;;;;N;LATIN CAPITAL LETTER O OGONEK MACRON;;;01ED;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01ED'), U"\x01EB\x0304") << desc_decomp_of(U'\x01ED') << "\n  Line: 01ED;LATIN SMALL LETTER O WITH OGONEK AND MACRON;Ll;0;L;01EB 0304;;;;N;LATIN SMALL LETTER O OGONEK MACRON;;01EC;;01EC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01EE'), U"\x01B7\x030C") << desc_decomp_of(U'\x01EE') << "\n  Line: 01EE;LATIN CAPITAL LETTER EZH WITH CARON;Lu;0;L;01B7 030C;;;;N;LATIN CAPITAL LETTER YOGH HACEK;;;01EF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01EF'), U"\x0292\x030C") << desc_decomp_of(U'\x01EF') << "\n  Line: 01EF;LATIN SMALL LETTER EZH WITH CARON;Ll;0;L;0292 030C;;;;N;LATIN SMALL LETTER YOGH HACEK;;01EE;;01EE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01F0'), U"\x006A\x030C") << desc_decomp_of(U'\x01F0') << "\n  Line: 01F0;LATIN SMALL LETTER J WITH CARON;Ll;0;L;006A 030C;;;;N;LATIN SMALL LETTER J HACEK;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01F4'), U"\x0047\x0301") << desc_decomp_of(U'\x01F4') << "\n  Line: 01F4;LATIN CAPITAL LETTER G WITH ACUTE;Lu;0;L;0047 0301;;;;N;;;;01F5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01F5'), U"\x0067\x0301") << desc_decomp_of(U'\x01F5') << "\n  Line: 01F5;LATIN SMALL LETTER G WITH ACUTE;Ll;0;L;0067 0301;;;;N;;;01F4;;01F4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01F8'), U"\x004E\x0300") << desc_decomp_of(U'\x01F8') << "\n  Line: 01F8;LATIN CAPITAL LETTER N WITH GRAVE;Lu;0;L;004E 0300;;;;N;;;;01F9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01F9'), U"\x006E\x0300") << desc_decomp_of(U'\x01F9') << "\n  Line: 01F9;LATIN SMALL LETTER N WITH GRAVE;Ll;0;L;006E 0300;;;;N;;;01F8;;01F8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01FA'), U"\x00C5\x0301") << desc_decomp_of(U'\x01FA') << "\n  Line: 01FA;LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE;Lu;0;L;00C5 0301;;;;N;;;;01FB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01FB'), U"\x00E5\x0301") << desc_decomp_of(U'\x01FB') << "\n  Line: 01FB;LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE;Ll;0;L;00E5 0301;;;;N;;;01FA;;01FA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01FC'), U"\x00C6\x0301") << desc_decomp_of(U'\x01FC') << "\n  Line: 01FC;LATIN CAPITAL LETTER AE WITH ACUTE;Lu;0;L;00C6 0301;;;;N;;;;01FD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01FD'), U"\x00E6\x0301") << desc_decomp_of(U'\x01FD') << "\n  Line: 01FD;LATIN SMALL LETTER AE WITH ACUTE;Ll;0;L;00E6 0301;;;;N;;;01FC;;01FC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01FE'), U"\x00D8\x0301") << desc_decomp_of(U'\x01FE') << "\n  Line: 01FE;LATIN CAPITAL LETTER O WITH STROKE AND ACUTE;Lu;0;L;00D8 0301;;;;N;;;;01FF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x01FF'), U"\x00F8\x0301") << desc_decomp_of(U'\x01FF') << "\n  Line: 01FF;LATIN SMALL LETTER O WITH STROKE AND ACUTE;Ll;0;L;00F8 0301;;;;N;;;01FE;;01FE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0200'), U"\x0041\x030F") << desc_decomp_of(U'\x0200') << "\n  Line: 0200;LATIN CAPITAL LETTER A WITH DOUBLE GRAVE;Lu;0;L;0041 030F;;;;N;;;;0201;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0201'), U"\x0061\x030F") << desc_decomp_of(U'\x0201') << "\n  Line: 0201;LATIN SMALL LETTER A WITH DOUBLE GRAVE;Ll;0;L;0061 030F;;;;N;;;0200;;0200";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0202'), U"\x0041\x0311") << desc_decomp_of(U'\x0202') << "\n  Line: 0202;LATIN CAPITAL LETTER A WITH INVERTED BREVE;Lu;0;L;0041 0311;;;;N;;;;0203;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0203'), U"\x0061\x0311") << desc_decomp_of(U'\x0203') << "\n  Line: 0203;LATIN SMALL LETTER A WITH INVERTED BREVE;Ll;0;L;0061 0311;;;;N;;;0202;;0202";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0204'), U"\x0045\x030F") << desc_decomp_of(U'\x0204') << "\n  Line: 0204;LATIN CAPITAL LETTER E WITH DOUBLE GRAVE;Lu;0;L;0045 030F;;;;N;;;;0205;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0205'), U"\x0065\x030F") << desc_decomp_of(U'\x0205') << "\n  Line: 0205;LATIN SMALL LETTER E WITH DOUBLE GRAVE;Ll;0;L;0065 030F;;;;N;;;0204;;0204";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0206'), U"\x0045\x0311") << desc_decomp_of(U'\x0206') << "\n  Line: 0206;LATIN CAPITAL LETTER E WITH INVERTED BREVE;Lu;0;L;0045 0311;;;;N;;;;0207;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0207'), U"\x0065\x0311") << desc_decomp_of(U'\x0207') << "\n  Line: 0207;LATIN SMALL LETTER E WITH INVERTED BREVE;Ll;0;L;0065 0311;;;;N;;;0206;;0206";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0208'), U"\x0049\x030F") << desc_decomp_of(U'\x0208') << "\n  Line: 0208;LATIN CAPITAL LETTER I WITH DOUBLE GRAVE;Lu;0;L;0049 030F;;;;N;;;;0209;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0209'), U"\x0069\x030F") << desc_decomp_of(U'\x0209') << "\n  Line: 0209;LATIN SMALL LETTER I WITH DOUBLE GRAVE;Ll;0;L;0069 030F;;;;N;;;0208;;0208";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x020A'), U"\x0049\x0311") << desc_decomp_of(U'\x020A') << "\n  Line: 020A;LATIN CAPITAL LETTER I WITH INVERTED BREVE;Lu;0;L;0049 0311;;;;N;;;;020B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x020B'), U"\x0069\x0311") << desc_decomp_of(U'\x020B') << "\n  Line: 020B;LATIN SMALL LETTER I WITH INVERTED BREVE;Ll;0;L;0069 0311;;;;N;;;020A;;020A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x020C'), U"\x004F\x030F") << desc_decomp_of(U'\x020C') << "\n  Line: 020C;LATIN CAPITAL LETTER O WITH DOUBLE GRAVE;Lu;0;L;004F 030F;;;;N;;;;020D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x020D'), U"\x006F\x030F") << desc_decomp_of(U'\x020D') << "\n  Line: 020D;LATIN SMALL LETTER O WITH DOUBLE GRAVE;Ll;0;L;006F 030F;;;;N;;;020C;;020C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x020E'), U"\x004F\x0311") << desc_decomp_of(U'\x020E') << "\n  Line: 020E;LATIN CAPITAL LETTER O WITH INVERTED BREVE;Lu;0;L;004F 0311;;;;N;;;;020F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x020F'), U"\x006F\x0311") << desc_decomp_of(U'\x020F') << "\n  Line: 020F;LATIN SMALL LETTER O WITH INVERTED BREVE;Ll;0;L;006F 0311;;;;N;;;020E;;020E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0210'), U"\x0052\x030F") << desc_decomp_of(U'\x0210') << "\n  Line: 0210;LATIN CAPITAL LETTER R WITH DOUBLE GRAVE;Lu;0;L;0052 030F;;;;N;;;;0211;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0211'), U"\x0072\x030F") << desc_decomp_of(U'\x0211') << "\n  Line: 0211;LATIN SMALL LETTER R WITH DOUBLE GRAVE;Ll;0;L;0072 030F;;;;N;;;0210;;0210";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0212'), U"\x0052\x0311") << desc_decomp_of(U'\x0212') << "\n  Line: 0212;LATIN CAPITAL LETTER R WITH INVERTED BREVE;Lu;0;L;0052 0311;;;;N;;;;0213;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0213'), U"\x0072\x0311") << desc_decomp_of(U'\x0213') << "\n  Line: 0213;LATIN SMALL LETTER R WITH INVERTED BREVE;Ll;0;L;0072 0311;;;;N;;;0212;;0212";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0214'), U"\x0055\x030F") << desc_decomp_of(U'\x0214') << "\n  Line: 0214;LATIN CAPITAL LETTER U WITH DOUBLE GRAVE;Lu;0;L;0055 030F;;;;N;;;;0215;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0215'), U"\x0075\x030F") << desc_decomp_of(U'\x0215') << "\n  Line: 0215;LATIN SMALL LETTER U WITH DOUBLE GRAVE;Ll;0;L;0075 030F;;;;N;;;0214;;0214";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0216'), U"\x0055\x0311") << desc_decomp_of(U'\x0216') << "\n  Line: 0216;LATIN CAPITAL LETTER U WITH INVERTED BREVE;Lu;0;L;0055 0311;;;;N;;;;0217;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0217'), U"\x0075\x0311") << desc_decomp_of(U'\x0217') << "\n  Line: 0217;LATIN SMALL LETTER U WITH INVERTED BREVE;Ll;0;L;0075 0311;;;;N;;;0216;;0216";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0218'), U"\x0053\x0326") << desc_decomp_of(U'\x0218') << "\n  Line: 0218;LATIN CAPITAL LETTER S WITH COMMA BELOW;Lu;0;L;0053 0326;;;;N;;;;0219;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0219'), U"\x0073\x0326") << desc_decomp_of(U'\x0219') << "\n  Line: 0219;LATIN SMALL LETTER S WITH COMMA BELOW;Ll;0;L;0073 0326;;;;N;;;0218;;0218";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x021A'), U"\x0054\x0326") << desc_decomp_of(U'\x021A') << "\n  Line: 021A;LATIN CAPITAL LETTER T WITH COMMA BELOW;Lu;0;L;0054 0326;;;;N;;;;021B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x021B'), U"\x0074\x0326") << desc_decomp_of(U'\x021B') << "\n  Line: 021B;LATIN SMALL LETTER T WITH COMMA BELOW;Ll;0;L;0074 0326;;;;N;;;021A;;021A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x021E'), U"\x0048\x030C") << desc_decomp_of(U'\x021E') << "\n  Line: 021E;LATIN CAPITAL LETTER H WITH CARON;Lu;0;L;0048 030C;;;;N;;;;021F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x021F'), U"\x0068\x030C") << desc_decomp_of(U'\x021F') << "\n  Line: 021F;LATIN SMALL LETTER H WITH CARON;Ll;0;L;0068 030C;;;;N;;;021E;;021E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0226'), U"\x0041\x0307") << desc_decomp_of(U'\x0226') << "\n  Line: 0226;LATIN CAPITAL LETTER A WITH DOT ABOVE;Lu;0;L;0041 0307;;;;N;;;;0227;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0227'), U"\x0061\x0307") << desc_decomp_of(U'\x0227') << "\n  Line: 0227;LATIN SMALL LETTER A WITH DOT ABOVE;Ll;0;L;0061 0307;;;;N;;;0226;;0226";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0228'), U"\x0045\x0327") << desc_decomp_of(U'\x0228') << "\n  Line: 0228;LATIN CAPITAL LETTER E WITH CEDILLA;Lu;0;L;0045 0327;;;;N;;;;0229;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0229'), U"\x0065\x0327") << desc_decomp_of(U'\x0229') << "\n  Line: 0229;LATIN SMALL LETTER E WITH CEDILLA;Ll;0;L;0065 0327;;;;N;;;0228;;0228";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x022A'), U"\x00D6\x0304") << desc_decomp_of(U'\x022A') << "\n  Line: 022A;LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON;Lu;0;L;00D6 0304;;;;N;;;;022B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x022B'), U"\x00F6\x0304") << desc_decomp_of(U'\x022B') << "\n  Line: 022B;LATIN SMALL LETTER O WITH DIAERESIS AND MACRON;Ll;0;L;00F6 0304;;;;N;;;022A;;022A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x022C'), U"\x00D5\x0304") << desc_decomp_of(U'\x022C') << "\n  Line: 022C;LATIN CAPITAL LETTER O WITH TILDE AND MACRON;Lu;0;L;00D5 0304;;;;N;;;;022D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x022D'), U"\x00F5\x0304") << desc_decomp_of(U'\x022D') << "\n  Line: 022D;LATIN SMALL LETTER O WITH TILDE AND MACRON;Ll;0;L;00F5 0304;;;;N;;;022C;;022C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x022E'), U"\x004F\x0307") << desc_decomp_of(U'\x022E') << "\n  Line: 022E;LATIN CAPITAL LETTER O WITH DOT ABOVE;Lu;0;L;004F 0307;;;;N;;;;022F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x022F'), U"\x006F\x0307") << desc_decomp_of(U'\x022F') << "\n  Line: 022F;LATIN SMALL LETTER O WITH DOT ABOVE;Ll;0;L;006F 0307;;;;N;;;022E;;022E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0230'), U"\x022E\x0304") << desc_decomp_of(U'\x0230') << "\n  Line: 0230;LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON;Lu;0;L;022E 0304;;;;N;;;;0231;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0231'), U"\x022F\x0304") << desc_decomp_of(U'\x0231') << "\n  Line: 0231;LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON;Ll;0;L;022F 0304;;;;N;;;0230;;0230";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0232'), U"\x0059\x0304") << desc_decomp_of(U'\x0232') << "\n  Line: 0232;LATIN CAPITAL LETTER Y WITH MACRON;Lu;0;L;0059 0304;;;;N;;;;0233;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0233'), U"\x0079\x0304") << desc_decomp_of(U'\x0233') << "\n  Line: 0233;LATIN SMALL LETTER Y WITH MACRON;Ll;0;L;0079 0304;;;;N;;;0232;;0232";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0340'), U"\x0300") << desc_decomp_of(U'\x0340') << "\n  Line: 0340;COMBINING GRAVE TONE MARK;Mn;230;NSM;0300;;;;N;NON-SPACING GRAVE TONE MARK;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0341'), U"\x0301") << desc_decomp_of(U'\x0341') << "\n  Line: 0341;COMBINING ACUTE TONE MARK;Mn;230;NSM;0301;;;;N;NON-SPACING ACUTE TONE MARK;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0343'), U"\x0313") << desc_decomp_of(U'\x0343') << "\n  Line: 0343;COMBINING GREEK KORONIS;Mn;230;NSM;0313;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0344'), U"\x0308\x0301") << desc_decomp_of(U'\x0344') << "\n  Line: 0344;COMBINING GREEK DIALYTIKA TONOS;Mn;230;NSM;0308 0301;;;;N;GREEK NON-SPACING DIAERESIS TONOS;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0374'), U"\x02B9") << desc_decomp_of(U'\x0374') << "\n  Line: 0374;GREEK NUMERAL SIGN;Lm;0;ON;02B9;;;;N;GREEK UPPER NUMERAL SIGN;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x037E'), U"\x003B") << desc_decomp_of(U'\x037E') << "\n  Line: 037E;GREEK QUESTION MARK;Po;0;ON;003B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0385'), U"\x00A8\x0301") << desc_decomp_of(U'\x0385') << "\n  Line: 0385;GREEK DIALYTIKA TONOS;Sk;0;ON;00A8 0301;;;;N;GREEK SPACING DIAERESIS TONOS;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0386'), U"\x0391\x0301") << desc_decomp_of(U'\x0386') << "\n  Line: 0386;GREEK CAPITAL LETTER ALPHA WITH TONOS;Lu;0;L;0391 0301;;;;N;GREEK CAPITAL LETTER ALPHA TONOS;;;03AC;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0387'), U"\x00B7") << desc_decomp_of(U'\x0387') << "\n  Line: 0387;GREEK ANO TELEIA;Po;0;ON;00B7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0388'), U"\x0395\x0301") << desc_decomp_of(U'\x0388') << "\n  Line: 0388;GREEK CAPITAL LETTER EPSILON WITH TONOS;Lu;0;L;0395 0301;;;;N;GREEK CAPITAL LETTER EPSILON TONOS;;;03AD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0389'), U"\x0397\x0301") << desc_decomp_of(U'\x0389') << "\n  Line: 0389;GREEK CAPITAL LETTER ETA WITH TONOS;Lu;0;L;0397 0301;;;;N;GREEK CAPITAL LETTER ETA TONOS;;;03AE;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x038A'), U"\x0399\x0301") << desc_decomp_of(U'\x038A') << "\n  Line: 038A;GREEK CAPITAL LETTER IOTA WITH TONOS;Lu;0;L;0399 0301;;;;N;GREEK CAPITAL LETTER IOTA TONOS;;;03AF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x038C'), U"\x039F\x0301") << desc_decomp_of(U'\x038C') << "\n  Line: 038C;GREEK CAPITAL LETTER OMICRON WITH TONOS;Lu;0;L;039F 0301;;;;N;GREEK CAPITAL LETTER OMICRON TONOS;;;03CC;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x038E'), U"\x03A5\x0301") << desc_decomp_of(U'\x038E') << "\n  Line: 038E;GREEK CAPITAL LETTER UPSILON WITH TONOS;Lu;0;L;03A5 0301;;;;N;GREEK CAPITAL LETTER UPSILON TONOS;;;03CD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x038F'), U"\x03A9\x0301") << desc_decomp_of(U'\x038F') << "\n  Line: 038F;GREEK CAPITAL LETTER OMEGA WITH TONOS;Lu;0;L;03A9 0301;;;;N;GREEK CAPITAL LETTER OMEGA TONOS;;;03CE;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0390'), U"\x03CA\x0301") << desc_decomp_of(U'\x0390') << "\n  Line: 0390;GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS;Ll;0;L;03CA 0301;;;;N;GREEK SMALL LETTER IOTA DIAERESIS TONOS;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03AA'), U"\x0399\x0308") << desc_decomp_of(U'\x03AA') << "\n  Line: 03AA;GREEK CAPITAL LETTER IOTA WITH DIALYTIKA;Lu;0;L;0399 0308;;;;N;GREEK CAPITAL LETTER IOTA DIAERESIS;;;03CA;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03AB'), U"\x03A5\x0308") << desc_decomp_of(U'\x03AB') << "\n  Line: 03AB;GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA;Lu;0;L;03A5 0308;;;;N;GREEK CAPITAL LETTER UPSILON DIAERESIS;;;03CB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03AC'), U"\x03B1\x0301") << desc_decomp_of(U'\x03AC') << "\n  Line: 03AC;GREEK SMALL LETTER ALPHA WITH TONOS;Ll;0;L;03B1 0301;;;;N;GREEK SMALL LETTER ALPHA TONOS;;0386;;0386";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03AD'), U"\x03B5\x0301") << desc_decomp_of(U'\x03AD') << "\n  Line: 03AD;GREEK SMALL LETTER EPSILON WITH TONOS;Ll;0;L;03B5 0301;;;;N;GREEK SMALL LETTER EPSILON TONOS;;0388;;0388";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03AE'), U"\x03B7\x0301") << desc_decomp_of(U'\x03AE') << "\n  Line: 03AE;GREEK SMALL LETTER ETA WITH TONOS;Ll;0;L;03B7 0301;;;;N;GREEK SMALL LETTER ETA TONOS;;0389;;0389";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03AF'), U"\x03B9\x0301") << desc_decomp_of(U'\x03AF') << "\n  Line: 03AF;GREEK SMALL LETTER IOTA WITH TONOS;Ll;0;L;03B9 0301;;;;N;GREEK SMALL LETTER IOTA TONOS;;038A;;038A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03B0'), U"\x03CB\x0301") << desc_decomp_of(U'\x03B0') << "\n  Line: 03B0;GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS;Ll;0;L;03CB 0301;;;;N;GREEK SMALL LETTER UPSILON DIAERESIS TONOS;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03CA'), U"\x03B9\x0308") << desc_decomp_of(U'\x03CA') << "\n  Line: 03CA;GREEK SMALL LETTER IOTA WITH DIALYTIKA;Ll;0;L;03B9 0308;;;;N;GREEK SMALL LETTER IOTA DIAERESIS;;03AA;;03AA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03CB'), U"\x03C5\x0308") << desc_decomp_of(U'\x03CB') << "\n  Line: 03CB;GREEK SMALL LETTER UPSILON WITH DIALYTIKA;Ll;0;L;03C5 0308;;;;N;GREEK SMALL LETTER UPSILON DIAERESIS;;03AB;;03AB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03CC'), U"\x03BF\x0301") << desc_decomp_of(U'\x03CC') << "\n  Line: 03CC;GREEK SMALL LETTER OMICRON WITH TONOS;Ll;0;L;03BF 0301;;;;N;GREEK SMALL LETTER OMICRON TONOS;;038C;;038C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03CD'), U"\x03C5\x0301") << desc_decomp_of(U'\x03CD') << "\n  Line: 03CD;GREEK SMALL LETTER UPSILON WITH TONOS;Ll;0;L;03C5 0301;;;;N;GREEK SMALL LETTER UPSILON TONOS;;038E;;038E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03CE'), U"\x03C9\x0301") << desc_decomp_of(U'\x03CE') << "\n  Line: 03CE;GREEK SMALL LETTER OMEGA WITH TONOS;Ll;0;L;03C9 0301;;;;N;GREEK SMALL LETTER OMEGA TONOS;;038F;;038F";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03D3'), U"\x03D2\x0301") << desc_decomp_of(U'\x03D3') << "\n  Line: 03D3;GREEK UPSILON WITH ACUTE AND HOOK SYMBOL;Lu;0;L;03D2 0301;;;;N;GREEK CAPITAL LETTER UPSILON HOOK TONOS;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x03D4'), U"\x03D2\x0308") << desc_decomp_of(U'\x03D4') << "\n  Line: 03D4;GREEK UPSILON WITH DIAERESIS AND HOOK SYMBOL;Lu;0;L;03D2 0308;;;;N;GREEK CAPITAL LETTER UPSILON HOOK DIAERESIS;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0400'), U"\x0415\x0300") << desc_decomp_of(U'\x0400') << "\n  Line: 0400;CYRILLIC CAPITAL LETTER IE WITH GRAVE;Lu;0;L;0415 0300;;;;N;;;;0450;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0401'), U"\x0415\x0308") << desc_decomp_of(U'\x0401') << "\n  Line: 0401;CYRILLIC CAPITAL LETTER IO;Lu;0;L;0415 0308;;;;N;;;;0451;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0403'), U"\x0413\x0301") << desc_decomp_of(U'\x0403') << "\n  Line: 0403;CYRILLIC CAPITAL LETTER GJE;Lu;0;L;0413 0301;;;;N;;;;0453;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0407'), U"\x0406\x0308") << desc_decomp_of(U'\x0407') << "\n  Line: 0407;CYRILLIC CAPITAL LETTER YI;Lu;0;L;0406 0308;;;;N;;;;0457;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x040C'), U"\x041A\x0301") << desc_decomp_of(U'\x040C') << "\n  Line: 040C;CYRILLIC CAPITAL LETTER KJE;Lu;0;L;041A 0301;;;;N;;;;045C;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x040D'), U"\x0418\x0300") << desc_decomp_of(U'\x040D') << "\n  Line: 040D;CYRILLIC CAPITAL LETTER I WITH GRAVE;Lu;0;L;0418 0300;;;;N;;;;045D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x040E'), U"\x0423\x0306") << desc_decomp_of(U'\x040E') << "\n  Line: 040E;CYRILLIC CAPITAL LETTER SHORT U;Lu;0;L;0423 0306;;;;N;;;;045E;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0419'), U"\x0418\x0306") << desc_decomp_of(U'\x0419') << "\n  Line: 0419;CYRILLIC CAPITAL LETTER SHORT I;Lu;0;L;0418 0306;;;;N;CYRILLIC CAPITAL LETTER SHORT II;;;0439;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0439'), U"\x0438\x0306") << desc_decomp_of(U'\x0439') << "\n  Line: 0439;CYRILLIC SMALL LETTER SHORT I;Ll;0;L;0438 0306;;;;N;CYRILLIC SMALL LETTER SHORT II;;0419;;0419";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0450'), U"\x0435\x0300") << desc_decomp_of(U'\x0450') << "\n  Line: 0450;CYRILLIC SMALL LETTER IE WITH GRAVE;Ll;0;L;0435 0300;;;;N;;;0400;;0400";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0451'), U"\x0435\x0308") << desc_decomp_of(U'\x0451') << "\n  Line: 0451;CYRILLIC SMALL LETTER IO;Ll;0;L;0435 0308;;;;N;;;0401;;0401";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0453'), U"\x0433\x0301") << desc_decomp_of(U'\x0453') << "\n  Line: 0453;CYRILLIC SMALL LETTER GJE;Ll;0;L;0433 0301;;;;N;;;0403;;0403";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0457'), U"\x0456\x0308") << desc_decomp_of(U'\x0457') << "\n  Line: 0457;CYRILLIC SMALL LETTER YI;Ll;0;L;0456 0308;;;;N;;;0407;;0407";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x045C'), U"\x043A\x0301") << desc_decomp_of(U'\x045C') << "\n  Line: 045C;CYRILLIC SMALL LETTER KJE;Ll;0;L;043A 0301;;;;N;;;040C;;040C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x045D'), U"\x0438\x0300") << desc_decomp_of(U'\x045D') << "\n  Line: 045D;CYRILLIC SMALL LETTER I WITH GRAVE;Ll;0;L;0438 0300;;;;N;;;040D;;040D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x045E'), U"\x0443\x0306") << desc_decomp_of(U'\x045E') << "\n  Line: 045E;CYRILLIC SMALL LETTER SHORT U;Ll;0;L;0443 0306;;;;N;;;040E;;040E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0476'), U"\x0474\x030F") << desc_decomp_of(U'\x0476') << "\n  Line: 0476;CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT;Lu;0;L;0474 030F;;;;N;CYRILLIC CAPITAL LETTER IZHITSA DOUBLE GRAVE;;;0477;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0477'), U"\x0475\x030F") << desc_decomp_of(U'\x0477') << "\n  Line: 0477;CYRILLIC SMALL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT;Ll;0;L;0475 030F;;;;N;CYRILLIC SMALL LETTER IZHITSA DOUBLE GRAVE;;0476;;0476";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04C1'), U"\x0416\x0306") << desc_decomp_of(U'\x04C1') << "\n  Line: 04C1;CYRILLIC CAPITAL LETTER ZHE WITH BREVE;Lu;0;L;0416 0306;;;;N;CYRILLIC CAPITAL LETTER SHORT ZHE;;;04C2;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04C2'), U"\x0436\x0306") << desc_decomp_of(U'\x04C2') << "\n  Line: 04C2;CYRILLIC SMALL LETTER ZHE WITH BREVE;Ll;0;L;0436 0306;;;;N;CYRILLIC SMALL LETTER SHORT ZHE;;04C1;;04C1";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04D0'), U"\x0410\x0306") << desc_decomp_of(U'\x04D0') << "\n  Line: 04D0;CYRILLIC CAPITAL LETTER A WITH BREVE;Lu;0;L;0410 0306;;;;N;;;;04D1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04D1'), U"\x0430\x0306") << desc_decomp_of(U'\x04D1') << "\n  Line: 04D1;CYRILLIC SMALL LETTER A WITH BREVE;Ll;0;L;0430 0306;;;;N;;;04D0;;04D0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04D2'), U"\x0410\x0308") << desc_decomp_of(U'\x04D2') << "\n  Line: 04D2;CYRILLIC CAPITAL LETTER A WITH DIAERESIS;Lu;0;L;0410 0308;;;;N;;;;04D3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04D3'), U"\x0430\x0308") << desc_decomp_of(U'\x04D3') << "\n  Line: 04D3;CYRILLIC SMALL LETTER A WITH DIAERESIS;Ll;0;L;0430 0308;;;;N;;;04D2;;04D2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04D6'), U"\x0415\x0306") << desc_decomp_of(U'\x04D6') << "\n  Line: 04D6;CYRILLIC CAPITAL LETTER IE WITH BREVE;Lu;0;L;0415 0306;;;;N;;;;04D7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04D7'), U"\x0435\x0306") << desc_decomp_of(U'\x04D7') << "\n  Line: 04D7;CYRILLIC SMALL LETTER IE WITH BREVE;Ll;0;L;0435 0306;;;;N;;;04D6;;04D6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04DA'), U"\x04D8\x0308") << desc_decomp_of(U'\x04DA') << "\n  Line: 04DA;CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS;Lu;0;L;04D8 0308;;;;N;;;;04DB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04DB'), U"\x04D9\x0308") << desc_decomp_of(U'\x04DB') << "\n  Line: 04DB;CYRILLIC SMALL LETTER SCHWA WITH DIAERESIS;Ll;0;L;04D9 0308;;;;N;;;04DA;;04DA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04DC'), U"\x0416\x0308") << desc_decomp_of(U'\x04DC') << "\n  Line: 04DC;CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS;Lu;0;L;0416 0308;;;;N;;;;04DD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04DD'), U"\x0436\x0308") << desc_decomp_of(U'\x04DD') << "\n  Line: 04DD;CYRILLIC SMALL LETTER ZHE WITH DIAERESIS;Ll;0;L;0436 0308;;;;N;;;04DC;;04DC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04DE'), U"\x0417\x0308") << desc_decomp_of(U'\x04DE') << "\n  Line: 04DE;CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS;Lu;0;L;0417 0308;;;;N;;;;04DF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04DF'), U"\x0437\x0308") << desc_decomp_of(U'\x04DF') << "\n  Line: 04DF;CYRILLIC SMALL LETTER ZE WITH DIAERESIS;Ll;0;L;0437 0308;;;;N;;;04DE;;04DE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04E2'), U"\x0418\x0304") << desc_decomp_of(U'\x04E2') << "\n  Line: 04E2;CYRILLIC CAPITAL LETTER I WITH MACRON;Lu;0;L;0418 0304;;;;N;;;;04E3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04E3'), U"\x0438\x0304") << desc_decomp_of(U'\x04E3') << "\n  Line: 04E3;CYRILLIC SMALL LETTER I WITH MACRON;Ll;0;L;0438 0304;;;;N;;;04E2;;04E2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04E4'), U"\x0418\x0308") << desc_decomp_of(U'\x04E4') << "\n  Line: 04E4;CYRILLIC CAPITAL LETTER I WITH DIAERESIS;Lu;0;L;0418 0308;;;;N;;;;04E5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04E5'), U"\x0438\x0308") << desc_decomp_of(U'\x04E5') << "\n  Line: 04E5;CYRILLIC SMALL LETTER I WITH DIAERESIS;Ll;0;L;0438 0308;;;;N;;;04E4;;04E4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04E6'), U"\x041E\x0308") << desc_decomp_of(U'\x04E6') << "\n  Line: 04E6;CYRILLIC CAPITAL LETTER O WITH DIAERESIS;Lu;0;L;041E 0308;;;;N;;;;04E7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04E7'), U"\x043E\x0308") << desc_decomp_of(U'\x04E7') << "\n  Line: 04E7;CYRILLIC SMALL LETTER O WITH DIAERESIS;Ll;0;L;043E 0308;;;;N;;;04E6;;04E6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04EA'), U"\x04E8\x0308") << desc_decomp_of(U'\x04EA') << "\n  Line: 04EA;CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS;Lu;0;L;04E8 0308;;;;N;;;;04EB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04EB'), U"\x04E9\x0308") << desc_decomp_of(U'\x04EB') << "\n  Line: 04EB;CYRILLIC SMALL LETTER BARRED O WITH DIAERESIS;Ll;0;L;04E9 0308;;;;N;;;04EA;;04EA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04EC'), U"\x042D\x0308") << desc_decomp_of(U'\x04EC') << "\n  Line: 04EC;CYRILLIC CAPITAL LETTER E WITH DIAERESIS;Lu;0;L;042D 0308;;;;N;;;;04ED;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04ED'), U"\x044D\x0308") << desc_decomp_of(U'\x04ED') << "\n  Line: 04ED;CYRILLIC SMALL LETTER E WITH DIAERESIS;Ll;0;L;044D 0308;;;;N;;;04EC;;04EC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04EE'), U"\x0423\x0304") << desc_decomp_of(U'\x04EE') << "\n  Line: 04EE;CYRILLIC CAPITAL LETTER U WITH MACRON;Lu;0;L;0423 0304;;;;N;;;;04EF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04EF'), U"\x0443\x0304") << desc_decomp_of(U'\x04EF') << "\n  Line: 04EF;CYRILLIC SMALL LETTER U WITH MACRON;Ll;0;L;0443 0304;;;;N;;;04EE;;04EE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04F0'), U"\x0423\x0308") << desc_decomp_of(U'\x04F0') << "\n  Line: 04F0;CYRILLIC CAPITAL LETTER U WITH DIAERESIS;Lu;0;L;0423 0308;;;;N;;;;04F1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04F1'), U"\x0443\x0308") << desc_decomp_of(U'\x04F1') << "\n  Line: 04F1;CYRILLIC SMALL LETTER U WITH DIAERESIS;Ll;0;L;0443 0308;;;;N;;;04F0;;04F0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04F2'), U"\x0423\x030B") << desc_decomp_of(U'\x04F2') << "\n  Line: 04F2;CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE;Lu;0;L;0423 030B;;;;N;;;;04F3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04F3'), U"\x0443\x030B") << desc_decomp_of(U'\x04F3') << "\n  Line: 04F3;CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE;Ll;0;L;0443 030B;;;;N;;;04F2;;04F2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04F4'), U"\x0427\x0308") << desc_decomp_of(U'\x04F4') << "\n  Line: 04F4;CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS;Lu;0;L;0427 0308;;;;N;;;;04F5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04F5'), U"\x0447\x0308") << desc_decomp_of(U'\x04F5') << "\n  Line: 04F5;CYRILLIC SMALL LETTER CHE WITH DIAERESIS;Ll;0;L;0447 0308;;;;N;;;04F4;;04F4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04F8'), U"\x042B\x0308") << desc_decomp_of(U'\x04F8') << "\n  Line: 04F8;CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS;Lu;0;L;042B 0308;;;;N;;;;04F9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x04F9'), U"\x044B\x0308") << desc_decomp_of(U'\x04F9') << "\n  Line: 04F9;CYRILLIC SMALL LETTER YERU WITH DIAERESIS;Ll;0;L;044B 0308;;;;N;;;04F8;;04F8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0622'), U"\x0627\x0653") << desc_decomp_of(U'\x0622') << "\n  Line: 0622;ARABIC LETTER ALEF WITH MADDA ABOVE;Lo;0;AL;0627 0653;;;;N;ARABIC LETTER MADDAH ON ALEF;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0623'), U"\x0627\x0654") << desc_decomp_of(U'\x0623') << "\n  Line: 0623;ARABIC LETTER ALEF WITH HAMZA ABOVE;Lo;0;AL;0627 0654;;;;N;ARABIC LETTER HAMZAH ON ALEF;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0624'), U"\x0648\x0654") << desc_decomp_of(U'\x0624') << "\n  Line: 0624;ARABIC LETTER WAW WITH HAMZA ABOVE;Lo;0;AL;0648 0654;;;;N;ARABIC LETTER HAMZAH ON WAW;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0625'), U"\x0627\x0655") << desc_decomp_of(U'\x0625') << "\n  Line: 0625;ARABIC LETTER ALEF WITH HAMZA BELOW;Lo;0;AL;0627 0655;;;;N;ARABIC LETTER HAMZAH UNDER ALEF;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0626'), U"\x064A\x0654") << desc_decomp_of(U'\x0626') << "\n  Line: 0626;ARABIC LETTER YEH WITH HAMZA ABOVE;Lo;0;AL;064A 0654;;;;N;ARABIC LETTER HAMZAH ON YA;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x06C0'), U"\x06D5\x0654") << desc_decomp_of(U'\x06C0') << "\n  Line: 06C0;ARABIC LETTER HEH WITH YEH ABOVE;Lo;0;AL;06D5 0654;;;;N;ARABIC LETTER HAMZAH ON HA;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x06C2'), U"\x06C1\x0654") << desc_decomp_of(U'\x06C2') << "\n  Line: 06C2;ARABIC LETTER HEH GOAL WITH HAMZA ABOVE;Lo;0;AL;06C1 0654;;;;N;ARABIC LETTER HAMZAH ON HA GOAL;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x06D3'), U"\x06D2\x0654") << desc_decomp_of(U'\x06D3') << "\n  Line: 06D3;ARABIC LETTER YEH BARREE WITH HAMZA ABOVE;Lo;0;AL;06D2 0654;;;;N;ARABIC LETTER HAMZAH ON YA BARREE;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0929'), U"\x0928\x093C") << desc_decomp_of(U'\x0929') << "\n  Line: 0929;DEVANAGARI LETTER NNNA;Lo;0;L;0928 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0931'), U"\x0930\x093C") << desc_decomp_of(U'\x0931') << "\n  Line: 0931;DEVANAGARI LETTER RRA;Lo;0;L;0930 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0934'), U"\x0933\x093C") << desc_decomp_of(U'\x0934') << "\n  Line: 0934;DEVANAGARI LETTER LLLA;Lo;0;L;0933 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0958'), U"\x0915\x093C") << desc_decomp_of(U'\x0958') << "\n  Line: 0958;DEVANAGARI LETTER QA;Lo;0;L;0915 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0959'), U"\x0916\x093C") << desc_decomp_of(U'\x0959') << "\n  Line: 0959;DEVANAGARI LETTER KHHA;Lo;0;L;0916 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x095A'), U"\x0917\x093C") << desc_decomp_of(U'\x095A') << "\n  Line: 095A;DEVANAGARI LETTER GHHA;Lo;0;L;0917 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x095B'), U"\x091C\x093C") << desc_decomp_of(U'\x095B') << "\n  Line: 095B;DEVANAGARI LETTER ZA;Lo;0;L;091C 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x095C'), U"\x0921\x093C") << desc_decomp_of(U'\x095C') << "\n  Line: 095C;DEVANAGARI LETTER DDDHA;Lo;0;L;0921 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x095D'), U"\x0922\x093C") << desc_decomp_of(U'\x095D') << "\n  Line: 095D;DEVANAGARI LETTER RHA;Lo;0;L;0922 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x095E'), U"\x092B\x093C") << desc_decomp_of(U'\x095E') << "\n  Line: 095E;DEVANAGARI LETTER FA;Lo;0;L;092B 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x095F'), U"\x092F\x093C") << desc_decomp_of(U'\x095F') << "\n  Line: 095F;DEVANAGARI LETTER YYA;Lo;0;L;092F 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x09CB'), U"\x09C7\x09BE") << desc_decomp_of(U'\x09CB') << "\n  Line: 09CB;BENGALI VOWEL SIGN O;Mc;0;L;09C7 09BE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x09CC'), U"\x09C7\x09D7") << desc_decomp_of(U'\x09CC') << "\n  Line: 09CC;BENGALI VOWEL SIGN AU;Mc;0;L;09C7 09D7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x09DC'), U"\x09A1\x09BC") << desc_decomp_of(U'\x09DC') << "\n  Line: 09DC;BENGALI LETTER RRA;Lo;0;L;09A1 09BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x09DD'), U"\x09A2\x09BC") << desc_decomp_of(U'\x09DD') << "\n  Line: 09DD;BENGALI LETTER RHA;Lo;0;L;09A2 09BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x09DF'), U"\x09AF\x09BC") << desc_decomp_of(U'\x09DF') << "\n  Line: 09DF;BENGALI LETTER YYA;Lo;0;L;09AF 09BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0A33'), U"\x0A32\x0A3C") << desc_decomp_of(U'\x0A33') << "\n  Line: 0A33;GURMUKHI LETTER LLA;Lo;0;L;0A32 0A3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0A36'), U"\x0A38\x0A3C") << desc_decomp_of(U'\x0A36') << "\n  Line: 0A36;GURMUKHI LETTER SHA;Lo;0;L;0A38 0A3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0A59'), U"\x0A16\x0A3C") << desc_decomp_of(U'\x0A59') << "\n  Line: 0A59;GURMUKHI LETTER KHHA;Lo;0;L;0A16 0A3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0A5A'), U"\x0A17\x0A3C") << desc_decomp_of(U'\x0A5A') << "\n  Line: 0A5A;GURMUKHI LETTER GHHA;Lo;0;L;0A17 0A3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0A5B'), U"\x0A1C\x0A3C") << desc_decomp_of(U'\x0A5B') << "\n  Line: 0A5B;GURMUKHI LETTER ZA;Lo;0;L;0A1C 0A3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0A5E'), U"\x0A2B\x0A3C") << desc_decomp_of(U'\x0A5E') << "\n  Line: 0A5E;GURMUKHI LETTER FA;Lo;0;L;0A2B 0A3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0B48'), U"\x0B47\x0B56") << desc_decomp_of(U'\x0B48') << "\n  Line: 0B48;ORIYA VOWEL SIGN AI;Mc;0;L;0B47 0B56;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0B4B'), U"\x0B47\x0B3E") << desc_decomp_of(U'\x0B4B') << "\n  Line: 0B4B;ORIYA VOWEL SIGN O;Mc;0;L;0B47 0B3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0B4C'), U"\x0B47\x0B57") << desc_decomp_of(U'\x0B4C') << "\n  Line: 0B4C;ORIYA VOWEL SIGN AU;Mc;0;L;0B47 0B57;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0B5C'), U"\x0B21\x0B3C") << desc_decomp_of(U'\x0B5C') << "\n  Line: 0B5C;ORIYA LETTER RRA;Lo;0;L;0B21 0B3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0B5D'), U"\x0B22\x0B3C") << desc_decomp_of(U'\x0B5D') << "\n  Line: 0B5D;ORIYA LETTER RHA;Lo;0;L;0B22 0B3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0B94'), U"\x0B92\x0BD7") << desc_decomp_of(U'\x0B94') << "\n  Line: 0B94;TAMIL LETTER AU;Lo;0;L;0B92 0BD7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0BCA'), U"\x0BC6\x0BBE") << desc_decomp_of(U'\x0BCA') << "\n  Line: 0BCA;TAMIL VOWEL SIGN O;Mc;0;L;0BC6 0BBE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0BCB'), U"\x0BC7\x0BBE") << desc_decomp_of(U'\x0BCB') << "\n  Line: 0BCB;TAMIL VOWEL SIGN OO;Mc;0;L;0BC7 0BBE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0BCC'), U"\x0BC6\x0BD7") << desc_decomp_of(U'\x0BCC') << "\n  Line: 0BCC;TAMIL VOWEL SIGN AU;Mc;0;L;0BC6 0BD7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0C48'), U"\x0C46\x0C56") << desc_decomp_of(U'\x0C48') << "\n  Line: 0C48;TELUGU VOWEL SIGN AI;Mn;0;NSM;0C46 0C56;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0CC0'), U"\x0CBF\x0CD5") << desc_decomp_of(U'\x0CC0') << "\n  Line: 0CC0;KANNADA VOWEL SIGN II;Mc;0;L;0CBF 0CD5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0CC7'), U"\x0CC6\x0CD5") << desc_decomp_of(U'\x0CC7') << "\n  Line: 0CC7;KANNADA VOWEL SIGN EE;Mc;0;L;0CC6 0CD5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0CC8'), U"\x0CC6\x0CD6") << desc_decomp_of(U'\x0CC8') << "\n  Line: 0CC8;KANNADA VOWEL SIGN AI;Mc;0;L;0CC6 0CD6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0CCA'), U"\x0CC6\x0CC2") << desc_decomp_of(U'\x0CCA') << "\n  Line: 0CCA;KANNADA VOWEL SIGN O;Mc;0;L;0CC6 0CC2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0CCB'), U"\x0CCA\x0CD5") << desc_decomp_of(U'\x0CCB') << "\n  Line: 0CCB;KANNADA VOWEL SIGN OO;Mc;0;L;0CCA 0CD5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0D4A'), U"\x0D46\x0D3E") << desc_decomp_of(U'\x0D4A') << "\n  Line: 0D4A;MALAYALAM VOWEL SIGN O;Mc;0;L;0D46 0D3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0D4B'), U"\x0D47\x0D3E") << desc_decomp_of(U'\x0D4B') << "\n  Line: 0D4B;MALAYALAM VOWEL SIGN OO;Mc;0;L;0D47 0D3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0D4C'), U"\x0D46\x0D57") << desc_decomp_of(U'\x0D4C') << "\n  Line: 0D4C;MALAYALAM VOWEL SIGN AU;Mc;0;L;0D46 0D57;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0DDA'), U"\x0DD9\x0DCA") << desc_decomp_of(U'\x0DDA') << "\n  Line: 0DDA;SINHALA VOWEL SIGN DIGA KOMBUVA;Mc;0;L;0DD9 0DCA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0DDC'), U"\x0DD9\x0DCF") << desc_decomp_of(U'\x0DDC') << "\n  Line: 0DDC;SINHALA VOWEL SIGN KOMBUVA HAA AELA-PILLA;Mc;0;L;0DD9 0DCF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0DDD'), U"\x0DDC\x0DCA") << desc_decomp_of(U'\x0DDD') << "\n  Line: 0DDD;SINHALA VOWEL SIGN KOMBUVA HAA DIGA AELA-PILLA;Mc;0;L;0DDC 0DCA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0DDE'), U"\x0DD9\x0DDF") << desc_decomp_of(U'\x0DDE') << "\n  Line: 0DDE;SINHALA VOWEL SIGN KOMBUVA HAA GAYANUKITTA;Mc;0;L;0DD9 0DDF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F43'), U"\x0F42\x0FB7") << desc_decomp_of(U'\x0F43') << "\n  Line: 0F43;TIBETAN LETTER GHA;Lo;0;L;0F42 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F4D'), U"\x0F4C\x0FB7") << desc_decomp_of(U'\x0F4D') << "\n  Line: 0F4D;TIBETAN LETTER DDHA;Lo;0;L;0F4C 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F52'), U"\x0F51\x0FB7") << desc_decomp_of(U'\x0F52') << "\n  Line: 0F52;TIBETAN LETTER DHA;Lo;0;L;0F51 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F57'), U"\x0F56\x0FB7") << desc_decomp_of(U'\x0F57') << "\n  Line: 0F57;TIBETAN LETTER BHA;Lo;0;L;0F56 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F5C'), U"\x0F5B\x0FB7") << desc_decomp_of(U'\x0F5C') << "\n  Line: 0F5C;TIBETAN LETTER DZHA;Lo;0;L;0F5B 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F69'), U"\x0F40\x0FB5") << desc_decomp_of(U'\x0F69') << "\n  Line: 0F69;TIBETAN LETTER KSSA;Lo;0;L;0F40 0FB5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F73'), U"\x0F71\x0F72") << desc_decomp_of(U'\x0F73') << "\n  Line: 0F73;TIBETAN VOWEL SIGN II;Mn;0;NSM;0F71 0F72;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F75'), U"\x0F71\x0F74") << desc_decomp_of(U'\x0F75') << "\n  Line: 0F75;TIBETAN VOWEL SIGN UU;Mn;0;NSM;0F71 0F74;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F76'), U"\x0FB2\x0F80") << desc_decomp_of(U'\x0F76') << "\n  Line: 0F76;TIBETAN VOWEL SIGN VOCALIC R;Mn;0;NSM;0FB2 0F80;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F78'), U"\x0FB3\x0F80") << desc_decomp_of(U'\x0F78') << "\n  Line: 0F78;TIBETAN VOWEL SIGN VOCALIC L;Mn;0;NSM;0FB3 0F80;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F81'), U"\x0F71\x0F80") << desc_decomp_of(U'\x0F81') << "\n  Line: 0F81;TIBETAN VOWEL SIGN REVERSED II;Mn;0;NSM;0F71 0F80;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F93'), U"\x0F92\x0FB7") << desc_decomp_of(U'\x0F93') << "\n  Line: 0F93;TIBETAN SUBJOINED LETTER GHA;Mn;0;NSM;0F92 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0F9D'), U"\x0F9C\x0FB7") << desc_decomp_of(U'\x0F9D') << "\n  Line: 0F9D;TIBETAN SUBJOINED LETTER DDHA;Mn;0;NSM;0F9C 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0FA2'), U"\x0FA1\x0FB7") << desc_decomp_of(U'\x0FA2') << "\n  Line: 0FA2;TIBETAN SUBJOINED LETTER DHA;Mn;0;NSM;0FA1 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0FA7'), U"\x0FA6\x0FB7") << desc_decomp_of(U'\x0FA7') << "\n  Line: 0FA7;TIBETAN SUBJOINED LETTER BHA;Mn;0;NSM;0FA6 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0FAC'), U"\x0FAB\x0FB7") << desc_decomp_of(U'\x0FAC') << "\n  Line: 0FAC;TIBETAN SUBJOINED LETTER DZHA;Mn;0;NSM;0FAB 0FB7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x0FB9'), U"\x0F90\x0FB5") << desc_decomp_of(U'\x0FB9') << "\n  Line: 0FB9;TIBETAN SUBJOINED LETTER KSSA;Mn;0;NSM;0F90 0FB5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1026'), U"\x1025\x102E") << desc_decomp_of(U'\x1026') << "\n  Line: 1026;MYANMAR LETTER UU;Lo;0;L;1025 102E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B06'), U"\x1B05\x1B35") << desc_decomp_of(U'\x1B06') << "\n  Line: 1B06;BALINESE LETTER AKARA TEDUNG;Lo;0;L;1B05 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B08'), U"\x1B07\x1B35") << desc_decomp_of(U'\x1B08') << "\n  Line: 1B08;BALINESE LETTER IKARA TEDUNG;Lo;0;L;1B07 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B0A'), U"\x1B09\x1B35") << desc_decomp_of(U'\x1B0A') << "\n  Line: 1B0A;BALINESE LETTER UKARA TEDUNG;Lo;0;L;1B09 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B0C'), U"\x1B0B\x1B35") << desc_decomp_of(U'\x1B0C') << "\n  Line: 1B0C;BALINESE LETTER RA REPA TEDUNG;Lo;0;L;1B0B 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B0E'), U"\x1B0D\x1B35") << desc_decomp_of(U'\x1B0E') << "\n  Line: 1B0E;BALINESE LETTER LA LENGA TEDUNG;Lo;0;L;1B0D 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B12'), U"\x1B11\x1B35") << desc_decomp_of(U'\x1B12') << "\n  Line: 1B12;BALINESE LETTER OKARA TEDUNG;Lo;0;L;1B11 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B3B'), U"\x1B3A\x1B35") << desc_decomp_of(U'\x1B3B') << "\n  Line: 1B3B;BALINESE VOWEL SIGN RA REPA TEDUNG;Mc;0;L;1B3A 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B3D'), U"\x1B3C\x1B35") << desc_decomp_of(U'\x1B3D') << "\n  Line: 1B3D;BALINESE VOWEL SIGN LA LENGA TEDUNG;Mc;0;L;1B3C 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B40'), U"\x1B3E\x1B35") << desc_decomp_of(U'\x1B40') << "\n  Line: 1B40;BALINESE VOWEL SIGN TALING TEDUNG;Mc;0;L;1B3E 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B41'), U"\x1B3F\x1B35") << desc_decomp_of(U'\x1B41') << "\n  Line: 1B41;BALINESE VOWEL SIGN TALING REPA TEDUNG;Mc;0;L;1B3F 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1B43'), U"\x1B42\x1B35") << desc_decomp_of(U'\x1B43') << "\n  Line: 1B43;BALINESE VOWEL SIGN PEPET TEDUNG;Mc;0;L;1B42 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E00'), U"\x0041\x0325") << desc_decomp_of(U'\x1E00') << "\n  Line: 1E00;LATIN CAPITAL LETTER A WITH RING BELOW;Lu;0;L;0041 0325;;;;N;;;;1E01;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E01'), U"\x0061\x0325") << desc_decomp_of(U'\x1E01') << "\n  Line: 1E01;LATIN SMALL LETTER A WITH RING BELOW;Ll;0;L;0061 0325;;;;N;;;1E00;;1E00";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E02'), U"\x0042\x0307") << desc_decomp_of(U'\x1E02') << "\n  Line: 1E02;LATIN CAPITAL LETTER B WITH DOT ABOVE;Lu;0;L;0042 0307;;;;N;;;;1E03;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E03'), U"\x0062\x0307") << desc_decomp_of(U'\x1E03') << "\n  Line: 1E03;LATIN SMALL LETTER B WITH DOT ABOVE;Ll;0;L;0062 0307;;;;N;;;1E02;;1E02";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E04'), U"\x0042\x0323") << desc_decomp_of(U'\x1E04') << "\n  Line: 1E04;LATIN CAPITAL LETTER B WITH DOT BELOW;Lu;0;L;0042 0323;;;;N;;;;1E05;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E05'), U"\x0062\x0323") << desc_decomp_of(U'\x1E05') << "\n  Line: 1E05;LATIN SMALL LETTER B WITH DOT BELOW;Ll;0;L;0062 0323;;;;N;;;1E04;;1E04";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E06'), U"\x0042\x0331") << desc_decomp_of(U'\x1E06') << "\n  Line: 1E06;LATIN CAPITAL LETTER B WITH LINE BELOW;Lu;0;L;0042 0331;;;;N;;;;1E07;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E07'), U"\x0062\x0331") << desc_decomp_of(U'\x1E07') << "\n  Line: 1E07;LATIN SMALL LETTER B WITH LINE BELOW;Ll;0;L;0062 0331;;;;N;;;1E06;;1E06";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E08'), U"\x00C7\x0301") << desc_decomp_of(U'\x1E08') << "\n  Line: 1E08;LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE;Lu;0;L;00C7 0301;;;;N;;;;1E09;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E09'), U"\x00E7\x0301") << desc_decomp_of(U'\x1E09') << "\n  Line: 1E09;LATIN SMALL LETTER C WITH CEDILLA AND ACUTE;Ll;0;L;00E7 0301;;;;N;;;1E08;;1E08";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E0A'), U"\x0044\x0307") << desc_decomp_of(U'\x1E0A') << "\n  Line: 1E0A;LATIN CAPITAL LETTER D WITH DOT ABOVE;Lu;0;L;0044 0307;;;;N;;;;1E0B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E0B'), U"\x0064\x0307") << desc_decomp_of(U'\x1E0B') << "\n  Line: 1E0B;LATIN SMALL LETTER D WITH DOT ABOVE;Ll;0;L;0064 0307;;;;N;;;1E0A;;1E0A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E0C'), U"\x0044\x0323") << desc_decomp_of(U'\x1E0C') << "\n  Line: 1E0C;LATIN CAPITAL LETTER D WITH DOT BELOW;Lu;0;L;0044 0323;;;;N;;;;1E0D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E0D'), U"\x0064\x0323") << desc_decomp_of(U'\x1E0D') << "\n  Line: 1E0D;LATIN SMALL LETTER D WITH DOT BELOW;Ll;0;L;0064 0323;;;;N;;;1E0C;;1E0C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E0E'), U"\x0044\x0331") << desc_decomp_of(U'\x1E0E') << "\n  Line: 1E0E;LATIN CAPITAL LETTER D WITH LINE BELOW;Lu;0;L;0044 0331;;;;N;;;;1E0F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E0F'), U"\x0064\x0331") << desc_decomp_of(U'\x1E0F') << "\n  Line: 1E0F;LATIN SMALL LETTER D WITH LINE BELOW;Ll;0;L;0064 0331;;;;N;;;1E0E;;1E0E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E10'), U"\x0044\x0327") << desc_decomp_of(U'\x1E10') << "\n  Line: 1E10;LATIN CAPITAL LETTER D WITH CEDILLA;Lu;0;L;0044 0327;;;;N;;;;1E11;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E11'), U"\x0064\x0327") << desc_decomp_of(U'\x1E11') << "\n  Line: 1E11;LATIN SMALL LETTER D WITH CEDILLA;Ll;0;L;0064 0327;;;;N;;;1E10;;1E10";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E12'), U"\x0044\x032D") << desc_decomp_of(U'\x1E12') << "\n  Line: 1E12;LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW;Lu;0;L;0044 032D;;;;N;;;;1E13;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E13'), U"\x0064\x032D") << desc_decomp_of(U'\x1E13') << "\n  Line: 1E13;LATIN SMALL LETTER D WITH CIRCUMFLEX BELOW;Ll;0;L;0064 032D;;;;N;;;1E12;;1E12";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E14'), U"\x0112\x0300") << desc_decomp_of(U'\x1E14') << "\n  Line: 1E14;LATIN CAPITAL LETTER E WITH MACRON AND GRAVE;Lu;0;L;0112 0300;;;;N;;;;1E15;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E15'), U"\x0113\x0300") << desc_decomp_of(U'\x1E15') << "\n  Line: 1E15;LATIN SMALL LETTER E WITH MACRON AND GRAVE;Ll;0;L;0113 0300;;;;N;;;1E14;;1E14";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E16'), U"\x0112\x0301") << desc_decomp_of(U'\x1E16') << "\n  Line: 1E16;LATIN CAPITAL LETTER E WITH MACRON AND ACUTE;Lu;0;L;0112 0301;;;;N;;;;1E17;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E17'), U"\x0113\x0301") << desc_decomp_of(U'\x1E17') << "\n  Line: 1E17;LATIN SMALL LETTER E WITH MACRON AND ACUTE;Ll;0;L;0113 0301;;;;N;;;1E16;;1E16";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E18'), U"\x0045\x032D") << desc_decomp_of(U'\x1E18') << "\n  Line: 1E18;LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW;Lu;0;L;0045 032D;;;;N;;;;1E19;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E19'), U"\x0065\x032D") << desc_decomp_of(U'\x1E19') << "\n  Line: 1E19;LATIN SMALL LETTER E WITH CIRCUMFLEX BELOW;Ll;0;L;0065 032D;;;;N;;;1E18;;1E18";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E1A'), U"\x0045\x0330") << desc_decomp_of(U'\x1E1A') << "\n  Line: 1E1A;LATIN CAPITAL LETTER E WITH TILDE BELOW;Lu;0;L;0045 0330;;;;N;;;;1E1B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E1B'), U"\x0065\x0330") << desc_decomp_of(U'\x1E1B') << "\n  Line: 1E1B;LATIN SMALL LETTER E WITH TILDE BELOW;Ll;0;L;0065 0330;;;;N;;;1E1A;;1E1A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E1C'), U"\x0228\x0306") << desc_decomp_of(U'\x1E1C') << "\n  Line: 1E1C;LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE;Lu;0;L;0228 0306;;;;N;;;;1E1D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E1D'), U"\x0229\x0306") << desc_decomp_of(U'\x1E1D') << "\n  Line: 1E1D;LATIN SMALL LETTER E WITH CEDILLA AND BREVE;Ll;0;L;0229 0306;;;;N;;;1E1C;;1E1C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E1E'), U"\x0046\x0307") << desc_decomp_of(U'\x1E1E') << "\n  Line: 1E1E;LATIN CAPITAL LETTER F WITH DOT ABOVE;Lu;0;L;0046 0307;;;;N;;;;1E1F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E1F'), U"\x0066\x0307") << desc_decomp_of(U'\x1E1F') << "\n  Line: 1E1F;LATIN SMALL LETTER F WITH DOT ABOVE;Ll;0;L;0066 0307;;;;N;;;1E1E;;1E1E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E20'), U"\x0047\x0304") << desc_decomp_of(U'\x1E20') << "\n  Line: 1E20;LATIN CAPITAL LETTER G WITH MACRON;Lu;0;L;0047 0304;;;;N;;;;1E21;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E21'), U"\x0067\x0304") << desc_decomp_of(U'\x1E21') << "\n  Line: 1E21;LATIN SMALL LETTER G WITH MACRON;Ll;0;L;0067 0304;;;;N;;;1E20;;1E20";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E22'), U"\x0048\x0307") << desc_decomp_of(U'\x1E22') << "\n  Line: 1E22;LATIN CAPITAL LETTER H WITH DOT ABOVE;Lu;0;L;0048 0307;;;;N;;;;1E23;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E23'), U"\x0068\x0307") << desc_decomp_of(U'\x1E23') << "\n  Line: 1E23;LATIN SMALL LETTER H WITH DOT ABOVE;Ll;0;L;0068 0307;;;;N;;;1E22;;1E22";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E24'), U"\x0048\x0323") << desc_decomp_of(U'\x1E24') << "\n  Line: 1E24;LATIN CAPITAL LETTER H WITH DOT BELOW;Lu;0;L;0048 0323;;;;N;;;;1E25;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E25'), U"\x0068\x0323") << desc_decomp_of(U'\x1E25') << "\n  Line: 1E25;LATIN SMALL LETTER H WITH DOT BELOW;Ll;0;L;0068 0323;;;;N;;;1E24;;1E24";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E26'), U"\x0048\x0308") << desc_decomp_of(U'\x1E26') << "\n  Line: 1E26;LATIN CAPITAL LETTER H WITH DIAERESIS;Lu;0;L;0048 0308;;;;N;;;;1E27;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E27'), U"\x0068\x0308") << desc_decomp_of(U'\x1E27') << "\n  Line: 1E27;LATIN SMALL LETTER H WITH DIAERESIS;Ll;0;L;0068 0308;;;;N;;;1E26;;1E26";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E28'), U"\x0048\x0327") << desc_decomp_of(U'\x1E28') << "\n  Line: 1E28;LATIN CAPITAL LETTER H WITH CEDILLA;Lu;0;L;0048 0327;;;;N;;;;1E29;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E29'), U"\x0068\x0327") << desc_decomp_of(U'\x1E29') << "\n  Line: 1E29;LATIN SMALL LETTER H WITH CEDILLA;Ll;0;L;0068 0327;;;;N;;;1E28;;1E28";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E2A'), U"\x0048\x032E") << desc_decomp_of(U'\x1E2A') << "\n  Line: 1E2A;LATIN CAPITAL LETTER H WITH BREVE BELOW;Lu;0;L;0048 032E;;;;N;;;;1E2B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E2B'), U"\x0068\x032E") << desc_decomp_of(U'\x1E2B') << "\n  Line: 1E2B;LATIN SMALL LETTER H WITH BREVE BELOW;Ll;0;L;0068 032E;;;;N;;;1E2A;;1E2A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E2C'), U"\x0049\x0330") << desc_decomp_of(U'\x1E2C') << "\n  Line: 1E2C;LATIN CAPITAL LETTER I WITH TILDE BELOW;Lu;0;L;0049 0330;;;;N;;;;1E2D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E2D'), U"\x0069\x0330") << desc_decomp_of(U'\x1E2D') << "\n  Line: 1E2D;LATIN SMALL LETTER I WITH TILDE BELOW;Ll;0;L;0069 0330;;;;N;;;1E2C;;1E2C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E2E'), U"\x00CF\x0301") << desc_decomp_of(U'\x1E2E') << "\n  Line: 1E2E;LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE;Lu;0;L;00CF 0301;;;;N;;;;1E2F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E2F'), U"\x00EF\x0301") << desc_decomp_of(U'\x1E2F') << "\n  Line: 1E2F;LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE;Ll;0;L;00EF 0301;;;;N;;;1E2E;;1E2E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E30'), U"\x004B\x0301") << desc_decomp_of(U'\x1E30') << "\n  Line: 1E30;LATIN CAPITAL LETTER K WITH ACUTE;Lu;0;L;004B 0301;;;;N;;;;1E31;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E31'), U"\x006B\x0301") << desc_decomp_of(U'\x1E31') << "\n  Line: 1E31;LATIN SMALL LETTER K WITH ACUTE;Ll;0;L;006B 0301;;;;N;;;1E30;;1E30";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E32'), U"\x004B\x0323") << desc_decomp_of(U'\x1E32') << "\n  Line: 1E32;LATIN CAPITAL LETTER K WITH DOT BELOW;Lu;0;L;004B 0323;;;;N;;;;1E33;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E33'), U"\x006B\x0323") << desc_decomp_of(U'\x1E33') << "\n  Line: 1E33;LATIN SMALL LETTER K WITH DOT BELOW;Ll;0;L;006B 0323;;;;N;;;1E32;;1E32";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E34'), U"\x004B\x0331") << desc_decomp_of(U'\x1E34') << "\n  Line: 1E34;LATIN CAPITAL LETTER K WITH LINE BELOW;Lu;0;L;004B 0331;;;;N;;;;1E35;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E35'), U"\x006B\x0331") << desc_decomp_of(U'\x1E35') << "\n  Line: 1E35;LATIN SMALL LETTER K WITH LINE BELOW;Ll;0;L;006B 0331;;;;N;;;1E34;;1E34";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E36'), U"\x004C\x0323") << desc_decomp_of(U'\x1E36') << "\n  Line: 1E36;LATIN CAPITAL LETTER L WITH DOT BELOW;Lu;0;L;004C 0323;;;;N;;;;1E37;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E37'), U"\x006C\x0323") << desc_decomp_of(U'\x1E37') << "\n  Line: 1E37;LATIN SMALL LETTER L WITH DOT BELOW;Ll;0;L;006C 0323;;;;N;;;1E36;;1E36";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E38'), U"\x1E36\x0304") << desc_decomp_of(U'\x1E38') << "\n  Line: 1E38;LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON;Lu;0;L;1E36 0304;;;;N;;;;1E39;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E39'), U"\x1E37\x0304") << desc_decomp_of(U'\x1E39') << "\n  Line: 1E39;LATIN SMALL LETTER L WITH DOT BELOW AND MACRON;Ll;0;L;1E37 0304;;;;N;;;1E38;;1E38";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E3A'), U"\x004C\x0331") << desc_decomp_of(U'\x1E3A') << "\n  Line: 1E3A;LATIN CAPITAL LETTER L WITH LINE BELOW;Lu;0;L;004C 0331;;;;N;;;;1E3B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E3B'), U"\x006C\x0331") << desc_decomp_of(U'\x1E3B') << "\n  Line: 1E3B;LATIN SMALL LETTER L WITH LINE BELOW;Ll;0;L;006C 0331;;;;N;;;1E3A;;1E3A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E3C'), U"\x004C\x032D") << desc_decomp_of(U'\x1E3C') << "\n  Line: 1E3C;LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW;Lu;0;L;004C 032D;;;;N;;;;1E3D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E3D'), U"\x006C\x032D") << desc_decomp_of(U'\x1E3D') << "\n  Line: 1E3D;LATIN SMALL LETTER L WITH CIRCUMFLEX BELOW;Ll;0;L;006C 032D;;;;N;;;1E3C;;1E3C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E3E'), U"\x004D\x0301") << desc_decomp_of(U'\x1E3E') << "\n  Line: 1E3E;LATIN CAPITAL LETTER M WITH ACUTE;Lu;0;L;004D 0301;;;;N;;;;1E3F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E3F'), U"\x006D\x0301") << desc_decomp_of(U'\x1E3F') << "\n  Line: 1E3F;LATIN SMALL LETTER M WITH ACUTE;Ll;0;L;006D 0301;;;;N;;;1E3E;;1E3E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E40'), U"\x004D\x0307") << desc_decomp_of(U'\x1E40') << "\n  Line: 1E40;LATIN CAPITAL LETTER M WITH DOT ABOVE;Lu;0;L;004D 0307;;;;N;;;;1E41;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E41'), U"\x006D\x0307") << desc_decomp_of(U'\x1E41') << "\n  Line: 1E41;LATIN SMALL LETTER M WITH DOT ABOVE;Ll;0;L;006D 0307;;;;N;;;1E40;;1E40";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E42'), U"\x004D\x0323") << desc_decomp_of(U'\x1E42') << "\n  Line: 1E42;LATIN CAPITAL LETTER M WITH DOT BELOW;Lu;0;L;004D 0323;;;;N;;;;1E43;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E43'), U"\x006D\x0323") << desc_decomp_of(U'\x1E43') << "\n  Line: 1E43;LATIN SMALL LETTER M WITH DOT BELOW;Ll;0;L;006D 0323;;;;N;;;1E42;;1E42";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E44'), U"\x004E\x0307") << desc_decomp_of(U'\x1E44') << "\n  Line: 1E44;LATIN CAPITAL LETTER N WITH DOT ABOVE;Lu;0;L;004E 0307;;;;N;;;;1E45;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E45'), U"\x006E\x0307") << desc_decomp_of(U'\x1E45') << "\n  Line: 1E45;LATIN SMALL LETTER N WITH DOT ABOVE;Ll;0;L;006E 0307;;;;N;;;1E44;;1E44";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E46'), U"\x004E\x0323") << desc_decomp_of(U'\x1E46') << "\n  Line: 1E46;LATIN CAPITAL LETTER N WITH DOT BELOW;Lu;0;L;004E 0323;;;;N;;;;1E47;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E47'), U"\x006E\x0323") << desc_decomp_of(U'\x1E47') << "\n  Line: 1E47;LATIN SMALL LETTER N WITH DOT BELOW;Ll;0;L;006E 0323;;;;N;;;1E46;;1E46";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E48'), U"\x004E\x0331") << desc_decomp_of(U'\x1E48') << "\n  Line: 1E48;LATIN CAPITAL LETTER N WITH LINE BELOW;Lu;0;L;004E 0331;;;;N;;;;1E49;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E49'), U"\x006E\x0331") << desc_decomp_of(U'\x1E49') << "\n  Line: 1E49;LATIN SMALL LETTER N WITH LINE BELOW;Ll;0;L;006E 0331;;;;N;;;1E48;;1E48";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E4A'), U"\x004E\x032D") << desc_decomp_of(U'\x1E4A') << "\n  Line: 1E4A;LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW;Lu;0;L;004E 032D;;;;N;;;;1E4B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E4B'), U"\x006E\x032D") << desc_decomp_of(U'\x1E4B') << "\n  Line: 1E4B;LATIN SMALL LETTER N WITH CIRCUMFLEX BELOW;Ll;0;L;006E 032D;;;;N;;;1E4A;;1E4A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E4C'), U"\x00D5\x0301") << desc_decomp_of(U'\x1E4C') << "\n  Line: 1E4C;LATIN CAPITAL LETTER O WITH TILDE AND ACUTE;Lu;0;L;00D5 0301;;;;N;;;;1E4D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E4D'), U"\x00F5\x0301") << desc_decomp_of(U'\x1E4D') << "\n  Line: 1E4D;LATIN SMALL LETTER O WITH TILDE AND ACUTE;Ll;0;L;00F5 0301;;;;N;;;1E4C;;1E4C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E4E'), U"\x00D5\x0308") << desc_decomp_of(U'\x1E4E') << "\n  Line: 1E4E;LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS;Lu;0;L;00D5 0308;;;;N;;;;1E4F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E4F'), U"\x00F5\x0308") << desc_decomp_of(U'\x1E4F') << "\n  Line: 1E4F;LATIN SMALL LETTER O WITH TILDE AND DIAERESIS;Ll;0;L;00F5 0308;;;;N;;;1E4E;;1E4E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E50'), U"\x014C\x0300") << desc_decomp_of(U'\x1E50') << "\n  Line: 1E50;LATIN CAPITAL LETTER O WITH MACRON AND GRAVE;Lu;0;L;014C 0300;;;;N;;;;1E51;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E51'), U"\x014D\x0300") << desc_decomp_of(U'\x1E51') << "\n  Line: 1E51;LATIN SMALL LETTER O WITH MACRON AND GRAVE;Ll;0;L;014D 0300;;;;N;;;1E50;;1E50";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E52'), U"\x014C\x0301") << desc_decomp_of(U'\x1E52') << "\n  Line: 1E52;LATIN CAPITAL LETTER O WITH MACRON AND ACUTE;Lu;0;L;014C 0301;;;;N;;;;1E53;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E53'), U"\x014D\x0301") << desc_decomp_of(U'\x1E53') << "\n  Line: 1E53;LATIN SMALL LETTER O WITH MACRON AND ACUTE;Ll;0;L;014D 0301;;;;N;;;1E52;;1E52";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E54'), U"\x0050\x0301") << desc_decomp_of(U'\x1E54') << "\n  Line: 1E54;LATIN CAPITAL LETTER P WITH ACUTE;Lu;0;L;0050 0301;;;;N;;;;1E55;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E55'), U"\x0070\x0301") << desc_decomp_of(U'\x1E55') << "\n  Line: 1E55;LATIN SMALL LETTER P WITH ACUTE;Ll;0;L;0070 0301;;;;N;;;1E54;;1E54";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E56'), U"\x0050\x0307") << desc_decomp_of(U'\x1E56') << "\n  Line: 1E56;LATIN CAPITAL LETTER P WITH DOT ABOVE;Lu;0;L;0050 0307;;;;N;;;;1E57;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E57'), U"\x0070\x0307") << desc_decomp_of(U'\x1E57') << "\n  Line: 1E57;LATIN SMALL LETTER P WITH DOT ABOVE;Ll;0;L;0070 0307;;;;N;;;1E56;;1E56";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E58'), U"\x0052\x0307") << desc_decomp_of(U'\x1E58') << "\n  Line: 1E58;LATIN CAPITAL LETTER R WITH DOT ABOVE;Lu;0;L;0052 0307;;;;N;;;;1E59;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E59'), U"\x0072\x0307") << desc_decomp_of(U'\x1E59') << "\n  Line: 1E59;LATIN SMALL LETTER R WITH DOT ABOVE;Ll;0;L;0072 0307;;;;N;;;1E58;;1E58";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E5A'), U"\x0052\x0323") << desc_decomp_of(U'\x1E5A') << "\n  Line: 1E5A;LATIN CAPITAL LETTER R WITH DOT BELOW;Lu;0;L;0052 0323;;;;N;;;;1E5B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E5B'), U"\x0072\x0323") << desc_decomp_of(U'\x1E5B') << "\n  Line: 1E5B;LATIN SMALL LETTER R WITH DOT BELOW;Ll;0;L;0072 0323;;;;N;;;1E5A;;1E5A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E5C'), U"\x1E5A\x0304") << desc_decomp_of(U'\x1E5C') << "\n  Line: 1E5C;LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON;Lu;0;L;1E5A 0304;;;;N;;;;1E5D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E5D'), U"\x1E5B\x0304") << desc_decomp_of(U'\x1E5D') << "\n  Line: 1E5D;LATIN SMALL LETTER R WITH DOT BELOW AND MACRON;Ll;0;L;1E5B 0304;;;;N;;;1E5C;;1E5C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E5E'), U"\x0052\x0331") << desc_decomp_of(U'\x1E5E') << "\n  Line: 1E5E;LATIN CAPITAL LETTER R WITH LINE BELOW;Lu;0;L;0052 0331;;;;N;;;;1E5F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E5F'), U"\x0072\x0331") << desc_decomp_of(U'\x1E5F') << "\n  Line: 1E5F;LATIN SMALL LETTER R WITH LINE BELOW;Ll;0;L;0072 0331;;;;N;;;1E5E;;1E5E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E60'), U"\x0053\x0307") << desc_decomp_of(U'\x1E60') << "\n  Line: 1E60;LATIN CAPITAL LETTER S WITH DOT ABOVE;Lu;0;L;0053 0307;;;;N;;;;1E61;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E61'), U"\x0073\x0307") << desc_decomp_of(U'\x1E61') << "\n  Line: 1E61;LATIN SMALL LETTER S WITH DOT ABOVE;Ll;0;L;0073 0307;;;;N;;;1E60;;1E60";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E62'), U"\x0053\x0323") << desc_decomp_of(U'\x1E62') << "\n  Line: 1E62;LATIN CAPITAL LETTER S WITH DOT BELOW;Lu;0;L;0053 0323;;;;N;;;;1E63;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E63'), U"\x0073\x0323") << desc_decomp_of(U'\x1E63') << "\n  Line: 1E63;LATIN SMALL LETTER S WITH DOT BELOW;Ll;0;L;0073 0323;;;;N;;;1E62;;1E62";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E64'), U"\x015A\x0307") << desc_decomp_of(U'\x1E64') << "\n  Line: 1E64;LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE;Lu;0;L;015A 0307;;;;N;;;;1E65;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E65'), U"\x015B\x0307") << desc_decomp_of(U'\x1E65') << "\n  Line: 1E65;LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE;Ll;0;L;015B 0307;;;;N;;;1E64;;1E64";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E66'), U"\x0160\x0307") << desc_decomp_of(U'\x1E66') << "\n  Line: 1E66;LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE;Lu;0;L;0160 0307;;;;N;;;;1E67;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E67'), U"\x0161\x0307") << desc_decomp_of(U'\x1E67') << "\n  Line: 1E67;LATIN SMALL LETTER S WITH CARON AND DOT ABOVE;Ll;0;L;0161 0307;;;;N;;;1E66;;1E66";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E68'), U"\x1E62\x0307") << desc_decomp_of(U'\x1E68') << "\n  Line: 1E68;LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE;Lu;0;L;1E62 0307;;;;N;;;;1E69;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E69'), U"\x1E63\x0307") << desc_decomp_of(U'\x1E69') << "\n  Line: 1E69;LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE;Ll;0;L;1E63 0307;;;;N;;;1E68;;1E68";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E6A'), U"\x0054\x0307") << desc_decomp_of(U'\x1E6A') << "\n  Line: 1E6A;LATIN CAPITAL LETTER T WITH DOT ABOVE;Lu;0;L;0054 0307;;;;N;;;;1E6B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E6B'), U"\x0074\x0307") << desc_decomp_of(U'\x1E6B') << "\n  Line: 1E6B;LATIN SMALL LETTER T WITH DOT ABOVE;Ll;0;L;0074 0307;;;;N;;;1E6A;;1E6A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E6C'), U"\x0054\x0323") << desc_decomp_of(U'\x1E6C') << "\n  Line: 1E6C;LATIN CAPITAL LETTER T WITH DOT BELOW;Lu;0;L;0054 0323;;;;N;;;;1E6D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E6D'), U"\x0074\x0323") << desc_decomp_of(U'\x1E6D') << "\n  Line: 1E6D;LATIN SMALL LETTER T WITH DOT BELOW;Ll;0;L;0074 0323;;;;N;;;1E6C;;1E6C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E6E'), U"\x0054\x0331") << desc_decomp_of(U'\x1E6E') << "\n  Line: 1E6E;LATIN CAPITAL LETTER T WITH LINE BELOW;Lu;0;L;0054 0331;;;;N;;;;1E6F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E6F'), U"\x0074\x0331") << desc_decomp_of(U'\x1E6F') << "\n  Line: 1E6F;LATIN SMALL LETTER T WITH LINE BELOW;Ll;0;L;0074 0331;;;;N;;;1E6E;;1E6E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E70'), U"\x0054\x032D") << desc_decomp_of(U'\x1E70') << "\n  Line: 1E70;LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW;Lu;0;L;0054 032D;;;;N;;;;1E71;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E71'), U"\x0074\x032D") << desc_decomp_of(U'\x1E71') << "\n  Line: 1E71;LATIN SMALL LETTER T WITH CIRCUMFLEX BELOW;Ll;0;L;0074 032D;;;;N;;;1E70;;1E70";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E72'), U"\x0055\x0324") << desc_decomp_of(U'\x1E72') << "\n  Line: 1E72;LATIN CAPITAL LETTER U WITH DIAERESIS BELOW;Lu;0;L;0055 0324;;;;N;;;;1E73;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E73'), U"\x0075\x0324") << desc_decomp_of(U'\x1E73') << "\n  Line: 1E73;LATIN SMALL LETTER U WITH DIAERESIS BELOW;Ll;0;L;0075 0324;;;;N;;;1E72;;1E72";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E74'), U"\x0055\x0330") << desc_decomp_of(U'\x1E74') << "\n  Line: 1E74;LATIN CAPITAL LETTER U WITH TILDE BELOW;Lu;0;L;0055 0330;;;;N;;;;1E75;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E75'), U"\x0075\x0330") << desc_decomp_of(U'\x1E75') << "\n  Line: 1E75;LATIN SMALL LETTER U WITH TILDE BELOW;Ll;0;L;0075 0330;;;;N;;;1E74;;1E74";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E76'), U"\x0055\x032D") << desc_decomp_of(U'\x1E76') << "\n  Line: 1E76;LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW;Lu;0;L;0055 032D;;;;N;;;;1E77;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E77'), U"\x0075\x032D") << desc_decomp_of(U'\x1E77') << "\n  Line: 1E77;LATIN SMALL LETTER U WITH CIRCUMFLEX BELOW;Ll;0;L;0075 032D;;;;N;;;1E76;;1E76";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E78'), U"\x0168\x0301") << desc_decomp_of(U'\x1E78') << "\n  Line: 1E78;LATIN CAPITAL LETTER U WITH TILDE AND ACUTE;Lu;0;L;0168 0301;;;;N;;;;1E79;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E79'), U"\x0169\x0301") << desc_decomp_of(U'\x1E79') << "\n  Line: 1E79;LATIN SMALL LETTER U WITH TILDE AND ACUTE;Ll;0;L;0169 0301;;;;N;;;1E78;;1E78";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E7A'), U"\x016A\x0308") << desc_decomp_of(U'\x1E7A') << "\n  Line: 1E7A;LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS;Lu;0;L;016A 0308;;;;N;;;;1E7B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E7B'), U"\x016B\x0308") << desc_decomp_of(U'\x1E7B') << "\n  Line: 1E7B;LATIN SMALL LETTER U WITH MACRON AND DIAERESIS;Ll;0;L;016B 0308;;;;N;;;1E7A;;1E7A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E7C'), U"\x0056\x0303") << desc_decomp_of(U'\x1E7C') << "\n  Line: 1E7C;LATIN CAPITAL LETTER V WITH TILDE;Lu;0;L;0056 0303;;;;N;;;;1E7D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E7D'), U"\x0076\x0303") << desc_decomp_of(U'\x1E7D') << "\n  Line: 1E7D;LATIN SMALL LETTER V WITH TILDE;Ll;0;L;0076 0303;;;;N;;;1E7C;;1E7C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E7E'), U"\x0056\x0323") << desc_decomp_of(U'\x1E7E') << "\n  Line: 1E7E;LATIN CAPITAL LETTER V WITH DOT BELOW;Lu;0;L;0056 0323;;;;N;;;;1E7F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E7F'), U"\x0076\x0323") << desc_decomp_of(U'\x1E7F') << "\n  Line: 1E7F;LATIN SMALL LETTER V WITH DOT BELOW;Ll;0;L;0076 0323;;;;N;;;1E7E;;1E7E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E80'), U"\x0057\x0300") << desc_decomp_of(U'\x1E80') << "\n  Line: 1E80;LATIN CAPITAL LETTER W WITH GRAVE;Lu;0;L;0057 0300;;;;N;;;;1E81;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E81'), U"\x0077\x0300") << desc_decomp_of(U'\x1E81') << "\n  Line: 1E81;LATIN SMALL LETTER W WITH GRAVE;Ll;0;L;0077 0300;;;;N;;;1E80;;1E80";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E82'), U"\x0057\x0301") << desc_decomp_of(U'\x1E82') << "\n  Line: 1E82;LATIN CAPITAL LETTER W WITH ACUTE;Lu;0;L;0057 0301;;;;N;;;;1E83;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E83'), U"\x0077\x0301") << desc_decomp_of(U'\x1E83') << "\n  Line: 1E83;LATIN SMALL LETTER W WITH ACUTE;Ll;0;L;0077 0301;;;;N;;;1E82;;1E82";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E84'), U"\x0057\x0308") << desc_decomp_of(U'\x1E84') << "\n  Line: 1E84;LATIN CAPITAL LETTER W WITH DIAERESIS;Lu;0;L;0057 0308;;;;N;;;;1E85;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E85'), U"\x0077\x0308") << desc_decomp_of(U'\x1E85') << "\n  Line: 1E85;LATIN SMALL LETTER W WITH DIAERESIS;Ll;0;L;0077 0308;;;;N;;;1E84;;1E84";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E86'), U"\x0057\x0307") << desc_decomp_of(U'\x1E86') << "\n  Line: 1E86;LATIN CAPITAL LETTER W WITH DOT ABOVE;Lu;0;L;0057 0307;;;;N;;;;1E87;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E87'), U"\x0077\x0307") << desc_decomp_of(U'\x1E87') << "\n  Line: 1E87;LATIN SMALL LETTER W WITH DOT ABOVE;Ll;0;L;0077 0307;;;;N;;;1E86;;1E86";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E88'), U"\x0057\x0323") << desc_decomp_of(U'\x1E88') << "\n  Line: 1E88;LATIN CAPITAL LETTER W WITH DOT BELOW;Lu;0;L;0057 0323;;;;N;;;;1E89;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E89'), U"\x0077\x0323") << desc_decomp_of(U'\x1E89') << "\n  Line: 1E89;LATIN SMALL LETTER W WITH DOT BELOW;Ll;0;L;0077 0323;;;;N;;;1E88;;1E88";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E8A'), U"\x0058\x0307") << desc_decomp_of(U'\x1E8A') << "\n  Line: 1E8A;LATIN CAPITAL LETTER X WITH DOT ABOVE;Lu;0;L;0058 0307;;;;N;;;;1E8B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E8B'), U"\x0078\x0307") << desc_decomp_of(U'\x1E8B') << "\n  Line: 1E8B;LATIN SMALL LETTER X WITH DOT ABOVE;Ll;0;L;0078 0307;;;;N;;;1E8A;;1E8A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E8C'), U"\x0058\x0308") << desc_decomp_of(U'\x1E8C') << "\n  Line: 1E8C;LATIN CAPITAL LETTER X WITH DIAERESIS;Lu;0;L;0058 0308;;;;N;;;;1E8D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E8D'), U"\x0078\x0308") << desc_decomp_of(U'\x1E8D') << "\n  Line: 1E8D;LATIN SMALL LETTER X WITH DIAERESIS;Ll;0;L;0078 0308;;;;N;;;1E8C;;1E8C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E8E'), U"\x0059\x0307") << desc_decomp_of(U'\x1E8E') << "\n  Line: 1E8E;LATIN CAPITAL LETTER Y WITH DOT ABOVE;Lu;0;L;0059 0307;;;;N;;;;1E8F;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E8F'), U"\x0079\x0307") << desc_decomp_of(U'\x1E8F') << "\n  Line: 1E8F;LATIN SMALL LETTER Y WITH DOT ABOVE;Ll;0;L;0079 0307;;;;N;;;1E8E;;1E8E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E90'), U"\x005A\x0302") << desc_decomp_of(U'\x1E90') << "\n  Line: 1E90;LATIN CAPITAL LETTER Z WITH CIRCUMFLEX;Lu;0;L;005A 0302;;;;N;;;;1E91;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E91'), U"\x007A\x0302") << desc_decomp_of(U'\x1E91') << "\n  Line: 1E91;LATIN SMALL LETTER Z WITH CIRCUMFLEX;Ll;0;L;007A 0302;;;;N;;;1E90;;1E90";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E92'), U"\x005A\x0323") << desc_decomp_of(U'\x1E92') << "\n  Line: 1E92;LATIN CAPITAL LETTER Z WITH DOT BELOW;Lu;0;L;005A 0323;;;;N;;;;1E93;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E93'), U"\x007A\x0323") << desc_decomp_of(U'\x1E93') << "\n  Line: 1E93;LATIN SMALL LETTER Z WITH DOT BELOW;Ll;0;L;007A 0323;;;;N;;;1E92;;1E92";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E94'), U"\x005A\x0331") << desc_decomp_of(U'\x1E94') << "\n  Line: 1E94;LATIN CAPITAL LETTER Z WITH LINE BELOW;Lu;0;L;005A 0331;;;;N;;;;1E95;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E95'), U"\x007A\x0331") << desc_decomp_of(U'\x1E95') << "\n  Line: 1E95;LATIN SMALL LETTER Z WITH LINE BELOW;Ll;0;L;007A 0331;;;;N;;;1E94;;1E94";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E96'), U"\x0068\x0331") << desc_decomp_of(U'\x1E96') << "\n  Line: 1E96;LATIN SMALL LETTER H WITH LINE BELOW;Ll;0;L;0068 0331;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E97'), U"\x0074\x0308") << desc_decomp_of(U'\x1E97') << "\n  Line: 1E97;LATIN SMALL LETTER T WITH DIAERESIS;Ll;0;L;0074 0308;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E98'), U"\x0077\x030A") << desc_decomp_of(U'\x1E98') << "\n  Line: 1E98;LATIN SMALL LETTER W WITH RING ABOVE;Ll;0;L;0077 030A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E99'), U"\x0079\x030A") << desc_decomp_of(U'\x1E99') << "\n  Line: 1E99;LATIN SMALL LETTER Y WITH RING ABOVE;Ll;0;L;0079 030A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E9B'), U"\x017F\x0307") << desc_decomp_of(U'\x1E9B') << "\n  Line: 1E9B;LATIN SMALL LETTER LONG S WITH DOT ABOVE;Ll;0;L;017F 0307;;;;N;;;1E60;;1E60";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA0'), U"\x0041\x0323") << desc_decomp_of(U'\x1EA0') << "\n  Line: 1EA0;LATIN CAPITAL LETTER A WITH DOT BELOW;Lu;0;L;0041 0323;;;;N;;;;1EA1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA1'), U"\x0061\x0323") << desc_decomp_of(U'\x1EA1') << "\n  Line: 1EA1;LATIN SMALL LETTER A WITH DOT BELOW;Ll;0;L;0061 0323;;;;N;;;1EA0;;1EA0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA2'), U"\x0041\x0309") << desc_decomp_of(U'\x1EA2') << "\n  Line: 1EA2;LATIN CAPITAL LETTER A WITH HOOK ABOVE;Lu;0;L;0041 0309;;;;N;;;;1EA3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA3'), U"\x0061\x0309") << desc_decomp_of(U'\x1EA3') << "\n  Line: 1EA3;LATIN SMALL LETTER A WITH HOOK ABOVE;Ll;0;L;0061 0309;;;;N;;;1EA2;;1EA2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA4'), U"\x00C2\x0301") << desc_decomp_of(U'\x1EA4') << "\n  Line: 1EA4;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE;Lu;0;L;00C2 0301;;;;N;;;;1EA5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA5'), U"\x00E2\x0301") << desc_decomp_of(U'\x1EA5') << "\n  Line: 1EA5;LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE;Ll;0;L;00E2 0301;;;;N;;;1EA4;;1EA4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA6'), U"\x00C2\x0300") << desc_decomp_of(U'\x1EA6') << "\n  Line: 1EA6;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE;Lu;0;L;00C2 0300;;;;N;;;;1EA7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA7'), U"\x00E2\x0300") << desc_decomp_of(U'\x1EA7') << "\n  Line: 1EA7;LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE;Ll;0;L;00E2 0300;;;;N;;;1EA6;;1EA6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA8'), U"\x00C2\x0309") << desc_decomp_of(U'\x1EA8') << "\n  Line: 1EA8;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE;Lu;0;L;00C2 0309;;;;N;;;;1EA9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EA9'), U"\x00E2\x0309") << desc_decomp_of(U'\x1EA9') << "\n  Line: 1EA9;LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE;Ll;0;L;00E2 0309;;;;N;;;1EA8;;1EA8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EAA'), U"\x00C2\x0303") << desc_decomp_of(U'\x1EAA') << "\n  Line: 1EAA;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE;Lu;0;L;00C2 0303;;;;N;;;;1EAB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EAB'), U"\x00E2\x0303") << desc_decomp_of(U'\x1EAB') << "\n  Line: 1EAB;LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE;Ll;0;L;00E2 0303;;;;N;;;1EAA;;1EAA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EAC'), U"\x1EA0\x0302") << desc_decomp_of(U'\x1EAC') << "\n  Line: 1EAC;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW;Lu;0;L;1EA0 0302;;;;N;;;;1EAD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EAD'), U"\x1EA1\x0302") << desc_decomp_of(U'\x1EAD') << "\n  Line: 1EAD;LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW;Ll;0;L;1EA1 0302;;;;N;;;1EAC;;1EAC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EAE'), U"\x0102\x0301") << desc_decomp_of(U'\x1EAE') << "\n  Line: 1EAE;LATIN CAPITAL LETTER A WITH BREVE AND ACUTE;Lu;0;L;0102 0301;;;;N;;;;1EAF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EAF'), U"\x0103\x0301") << desc_decomp_of(U'\x1EAF') << "\n  Line: 1EAF;LATIN SMALL LETTER A WITH BREVE AND ACUTE;Ll;0;L;0103 0301;;;;N;;;1EAE;;1EAE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB0'), U"\x0102\x0300") << desc_decomp_of(U'\x1EB0') << "\n  Line: 1EB0;LATIN CAPITAL LETTER A WITH BREVE AND GRAVE;Lu;0;L;0102 0300;;;;N;;;;1EB1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB1'), U"\x0103\x0300") << desc_decomp_of(U'\x1EB1') << "\n  Line: 1EB1;LATIN SMALL LETTER A WITH BREVE AND GRAVE;Ll;0;L;0103 0300;;;;N;;;1EB0;;1EB0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB2'), U"\x0102\x0309") << desc_decomp_of(U'\x1EB2') << "\n  Line: 1EB2;LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE;Lu;0;L;0102 0309;;;;N;;;;1EB3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB3'), U"\x0103\x0309") << desc_decomp_of(U'\x1EB3') << "\n  Line: 1EB3;LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE;Ll;0;L;0103 0309;;;;N;;;1EB2;;1EB2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB4'), U"\x0102\x0303") << desc_decomp_of(U'\x1EB4') << "\n  Line: 1EB4;LATIN CAPITAL LETTER A WITH BREVE AND TILDE;Lu;0;L;0102 0303;;;;N;;;;1EB5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB5'), U"\x0103\x0303") << desc_decomp_of(U'\x1EB5') << "\n  Line: 1EB5;LATIN SMALL LETTER A WITH BREVE AND TILDE;Ll;0;L;0103 0303;;;;N;;;1EB4;;1EB4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB6'), U"\x1EA0\x0306") << desc_decomp_of(U'\x1EB6') << "\n  Line: 1EB6;LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW;Lu;0;L;1EA0 0306;;;;N;;;;1EB7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB7'), U"\x1EA1\x0306") << desc_decomp_of(U'\x1EB7') << "\n  Line: 1EB7;LATIN SMALL LETTER A WITH BREVE AND DOT BELOW;Ll;0;L;1EA1 0306;;;;N;;;1EB6;;1EB6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB8'), U"\x0045\x0323") << desc_decomp_of(U'\x1EB8') << "\n  Line: 1EB8;LATIN CAPITAL LETTER E WITH DOT BELOW;Lu;0;L;0045 0323;;;;N;;;;1EB9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EB9'), U"\x0065\x0323") << desc_decomp_of(U'\x1EB9') << "\n  Line: 1EB9;LATIN SMALL LETTER E WITH DOT BELOW;Ll;0;L;0065 0323;;;;N;;;1EB8;;1EB8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EBA'), U"\x0045\x0309") << desc_decomp_of(U'\x1EBA') << "\n  Line: 1EBA;LATIN CAPITAL LETTER E WITH HOOK ABOVE;Lu;0;L;0045 0309;;;;N;;;;1EBB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EBB'), U"\x0065\x0309") << desc_decomp_of(U'\x1EBB') << "\n  Line: 1EBB;LATIN SMALL LETTER E WITH HOOK ABOVE;Ll;0;L;0065 0309;;;;N;;;1EBA;;1EBA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EBC'), U"\x0045\x0303") << desc_decomp_of(U'\x1EBC') << "\n  Line: 1EBC;LATIN CAPITAL LETTER E WITH TILDE;Lu;0;L;0045 0303;;;;N;;;;1EBD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EBD'), U"\x0065\x0303") << desc_decomp_of(U'\x1EBD') << "\n  Line: 1EBD;LATIN SMALL LETTER E WITH TILDE;Ll;0;L;0065 0303;;;;N;;;1EBC;;1EBC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EBE'), U"\x00CA\x0301") << desc_decomp_of(U'\x1EBE') << "\n  Line: 1EBE;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE;Lu;0;L;00CA 0301;;;;N;;;;1EBF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EBF'), U"\x00EA\x0301") << desc_decomp_of(U'\x1EBF') << "\n  Line: 1EBF;LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE;Ll;0;L;00EA 0301;;;;N;;;1EBE;;1EBE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC0'), U"\x00CA\x0300") << desc_decomp_of(U'\x1EC0') << "\n  Line: 1EC0;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE;Lu;0;L;00CA 0300;;;;N;;;;1EC1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC1'), U"\x00EA\x0300") << desc_decomp_of(U'\x1EC1') << "\n  Line: 1EC1;LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE;Ll;0;L;00EA 0300;;;;N;;;1EC0;;1EC0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC2'), U"\x00CA\x0309") << desc_decomp_of(U'\x1EC2') << "\n  Line: 1EC2;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE;Lu;0;L;00CA 0309;;;;N;;;;1EC3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC3'), U"\x00EA\x0309") << desc_decomp_of(U'\x1EC3') << "\n  Line: 1EC3;LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE;Ll;0;L;00EA 0309;;;;N;;;1EC2;;1EC2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC4'), U"\x00CA\x0303") << desc_decomp_of(U'\x1EC4') << "\n  Line: 1EC4;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE;Lu;0;L;00CA 0303;;;;N;;;;1EC5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC5'), U"\x00EA\x0303") << desc_decomp_of(U'\x1EC5') << "\n  Line: 1EC5;LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE;Ll;0;L;00EA 0303;;;;N;;;1EC4;;1EC4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC6'), U"\x1EB8\x0302") << desc_decomp_of(U'\x1EC6') << "\n  Line: 1EC6;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW;Lu;0;L;1EB8 0302;;;;N;;;;1EC7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC7'), U"\x1EB9\x0302") << desc_decomp_of(U'\x1EC7') << "\n  Line: 1EC7;LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW;Ll;0;L;1EB9 0302;;;;N;;;1EC6;;1EC6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC8'), U"\x0049\x0309") << desc_decomp_of(U'\x1EC8') << "\n  Line: 1EC8;LATIN CAPITAL LETTER I WITH HOOK ABOVE;Lu;0;L;0049 0309;;;;N;;;;1EC9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EC9'), U"\x0069\x0309") << desc_decomp_of(U'\x1EC9') << "\n  Line: 1EC9;LATIN SMALL LETTER I WITH HOOK ABOVE;Ll;0;L;0069 0309;;;;N;;;1EC8;;1EC8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ECA'), U"\x0049\x0323") << desc_decomp_of(U'\x1ECA') << "\n  Line: 1ECA;LATIN CAPITAL LETTER I WITH DOT BELOW;Lu;0;L;0049 0323;;;;N;;;;1ECB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ECB'), U"\x0069\x0323") << desc_decomp_of(U'\x1ECB') << "\n  Line: 1ECB;LATIN SMALL LETTER I WITH DOT BELOW;Ll;0;L;0069 0323;;;;N;;;1ECA;;1ECA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ECC'), U"\x004F\x0323") << desc_decomp_of(U'\x1ECC') << "\n  Line: 1ECC;LATIN CAPITAL LETTER O WITH DOT BELOW;Lu;0;L;004F 0323;;;;N;;;;1ECD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ECD'), U"\x006F\x0323") << desc_decomp_of(U'\x1ECD') << "\n  Line: 1ECD;LATIN SMALL LETTER O WITH DOT BELOW;Ll;0;L;006F 0323;;;;N;;;1ECC;;1ECC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ECE'), U"\x004F\x0309") << desc_decomp_of(U'\x1ECE') << "\n  Line: 1ECE;LATIN CAPITAL LETTER O WITH HOOK ABOVE;Lu;0;L;004F 0309;;;;N;;;;1ECF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ECF'), U"\x006F\x0309") << desc_decomp_of(U'\x1ECF') << "\n  Line: 1ECF;LATIN SMALL LETTER O WITH HOOK ABOVE;Ll;0;L;006F 0309;;;;N;;;1ECE;;1ECE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED0'), U"\x00D4\x0301") << desc_decomp_of(U'\x1ED0') << "\n  Line: 1ED0;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE;Lu;0;L;00D4 0301;;;;N;;;;1ED1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED1'), U"\x00F4\x0301") << desc_decomp_of(U'\x1ED1') << "\n  Line: 1ED1;LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE;Ll;0;L;00F4 0301;;;;N;;;1ED0;;1ED0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED2'), U"\x00D4\x0300") << desc_decomp_of(U'\x1ED2') << "\n  Line: 1ED2;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE;Lu;0;L;00D4 0300;;;;N;;;;1ED3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED3'), U"\x00F4\x0300") << desc_decomp_of(U'\x1ED3') << "\n  Line: 1ED3;LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE;Ll;0;L;00F4 0300;;;;N;;;1ED2;;1ED2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED4'), U"\x00D4\x0309") << desc_decomp_of(U'\x1ED4') << "\n  Line: 1ED4;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE;Lu;0;L;00D4 0309;;;;N;;;;1ED5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED5'), U"\x00F4\x0309") << desc_decomp_of(U'\x1ED5') << "\n  Line: 1ED5;LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE;Ll;0;L;00F4 0309;;;;N;;;1ED4;;1ED4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED6'), U"\x00D4\x0303") << desc_decomp_of(U'\x1ED6') << "\n  Line: 1ED6;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE;Lu;0;L;00D4 0303;;;;N;;;;1ED7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED7'), U"\x00F4\x0303") << desc_decomp_of(U'\x1ED7') << "\n  Line: 1ED7;LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE;Ll;0;L;00F4 0303;;;;N;;;1ED6;;1ED6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED8'), U"\x1ECC\x0302") << desc_decomp_of(U'\x1ED8') << "\n  Line: 1ED8;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW;Lu;0;L;1ECC 0302;;;;N;;;;1ED9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1ED9'), U"\x1ECD\x0302") << desc_decomp_of(U'\x1ED9') << "\n  Line: 1ED9;LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW;Ll;0;L;1ECD 0302;;;;N;;;1ED8;;1ED8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EDA'), U"\x01A0\x0301") << desc_decomp_of(U'\x1EDA') << "\n  Line: 1EDA;LATIN CAPITAL LETTER O WITH HORN AND ACUTE;Lu;0;L;01A0 0301;;;;N;;;;1EDB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EDB'), U"\x01A1\x0301") << desc_decomp_of(U'\x1EDB') << "\n  Line: 1EDB;LATIN SMALL LETTER O WITH HORN AND ACUTE;Ll;0;L;01A1 0301;;;;N;;;1EDA;;1EDA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EDC'), U"\x01A0\x0300") << desc_decomp_of(U'\x1EDC') << "\n  Line: 1EDC;LATIN CAPITAL LETTER O WITH HORN AND GRAVE;Lu;0;L;01A0 0300;;;;N;;;;1EDD;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EDD'), U"\x01A1\x0300") << desc_decomp_of(U'\x1EDD') << "\n  Line: 1EDD;LATIN SMALL LETTER O WITH HORN AND GRAVE;Ll;0;L;01A1 0300;;;;N;;;1EDC;;1EDC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EDE'), U"\x01A0\x0309") << desc_decomp_of(U'\x1EDE') << "\n  Line: 1EDE;LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE;Lu;0;L;01A0 0309;;;;N;;;;1EDF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EDF'), U"\x01A1\x0309") << desc_decomp_of(U'\x1EDF') << "\n  Line: 1EDF;LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE;Ll;0;L;01A1 0309;;;;N;;;1EDE;;1EDE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE0'), U"\x01A0\x0303") << desc_decomp_of(U'\x1EE0') << "\n  Line: 1EE0;LATIN CAPITAL LETTER O WITH HORN AND TILDE;Lu;0;L;01A0 0303;;;;N;;;;1EE1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE1'), U"\x01A1\x0303") << desc_decomp_of(U'\x1EE1') << "\n  Line: 1EE1;LATIN SMALL LETTER O WITH HORN AND TILDE;Ll;0;L;01A1 0303;;;;N;;;1EE0;;1EE0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE2'), U"\x01A0\x0323") << desc_decomp_of(U'\x1EE2') << "\n  Line: 1EE2;LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW;Lu;0;L;01A0 0323;;;;N;;;;1EE3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE3'), U"\x01A1\x0323") << desc_decomp_of(U'\x1EE3') << "\n  Line: 1EE3;LATIN SMALL LETTER O WITH HORN AND DOT BELOW;Ll;0;L;01A1 0323;;;;N;;;1EE2;;1EE2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE4'), U"\x0055\x0323") << desc_decomp_of(U'\x1EE4') << "\n  Line: 1EE4;LATIN CAPITAL LETTER U WITH DOT BELOW;Lu;0;L;0055 0323;;;;N;;;;1EE5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE5'), U"\x0075\x0323") << desc_decomp_of(U'\x1EE5') << "\n  Line: 1EE5;LATIN SMALL LETTER U WITH DOT BELOW;Ll;0;L;0075 0323;;;;N;;;1EE4;;1EE4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE6'), U"\x0055\x0309") << desc_decomp_of(U'\x1EE6') << "\n  Line: 1EE6;LATIN CAPITAL LETTER U WITH HOOK ABOVE;Lu;0;L;0055 0309;;;;N;;;;1EE7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE7'), U"\x0075\x0309") << desc_decomp_of(U'\x1EE7') << "\n  Line: 1EE7;LATIN SMALL LETTER U WITH HOOK ABOVE;Ll;0;L;0075 0309;;;;N;;;1EE6;;1EE6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE8'), U"\x01AF\x0301") << desc_decomp_of(U'\x1EE8') << "\n  Line: 1EE8;LATIN CAPITAL LETTER U WITH HORN AND ACUTE;Lu;0;L;01AF 0301;;;;N;;;;1EE9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EE9'), U"\x01B0\x0301") << desc_decomp_of(U'\x1EE9') << "\n  Line: 1EE9;LATIN SMALL LETTER U WITH HORN AND ACUTE;Ll;0;L;01B0 0301;;;;N;;;1EE8;;1EE8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EEA'), U"\x01AF\x0300") << desc_decomp_of(U'\x1EEA') << "\n  Line: 1EEA;LATIN CAPITAL LETTER U WITH HORN AND GRAVE;Lu;0;L;01AF 0300;;;;N;;;;1EEB;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EEB'), U"\x01B0\x0300") << desc_decomp_of(U'\x1EEB') << "\n  Line: 1EEB;LATIN SMALL LETTER U WITH HORN AND GRAVE;Ll;0;L;01B0 0300;;;;N;;;1EEA;;1EEA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EEC'), U"\x01AF\x0309") << desc_decomp_of(U'\x1EEC') << "\n  Line: 1EEC;LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE;Lu;0;L;01AF 0309;;;;N;;;;1EED;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EED'), U"\x01B0\x0309") << desc_decomp_of(U'\x1EED') << "\n  Line: 1EED;LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE;Ll;0;L;01B0 0309;;;;N;;;1EEC;;1EEC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EEE'), U"\x01AF\x0303") << desc_decomp_of(U'\x1EEE') << "\n  Line: 1EEE;LATIN CAPITAL LETTER U WITH HORN AND TILDE;Lu;0;L;01AF 0303;;;;N;;;;1EEF;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EEF'), U"\x01B0\x0303") << desc_decomp_of(U'\x1EEF') << "\n  Line: 1EEF;LATIN SMALL LETTER U WITH HORN AND TILDE;Ll;0;L;01B0 0303;;;;N;;;1EEE;;1EEE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF0'), U"\x01AF\x0323") << desc_decomp_of(U'\x1EF0') << "\n  Line: 1EF0;LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW;Lu;0;L;01AF 0323;;;;N;;;;1EF1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF1'), U"\x01B0\x0323") << desc_decomp_of(U'\x1EF1') << "\n  Line: 1EF1;LATIN SMALL LETTER U WITH HORN AND DOT BELOW;Ll;0;L;01B0 0323;;;;N;;;1EF0;;1EF0";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF2'), U"\x0059\x0300") << desc_decomp_of(U'\x1EF2') << "\n  Line: 1EF2;LATIN CAPITAL LETTER Y WITH GRAVE;Lu;0;L;0059 0300;;;;N;;;;1EF3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF3'), U"\x0079\x0300") << desc_decomp_of(U'\x1EF3') << "\n  Line: 1EF3;LATIN SMALL LETTER Y WITH GRAVE;Ll;0;L;0079 0300;;;;N;;;1EF2;;1EF2";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF4'), U"\x0059\x0323") << desc_decomp_of(U'\x1EF4') << "\n  Line: 1EF4;LATIN CAPITAL LETTER Y WITH DOT BELOW;Lu;0;L;0059 0323;;;;N;;;;1EF5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF5'), U"\x0079\x0323") << desc_decomp_of(U'\x1EF5') << "\n  Line: 1EF5;LATIN SMALL LETTER Y WITH DOT BELOW;Ll;0;L;0079 0323;;;;N;;;1EF4;;1EF4";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF6'), U"\x0059\x0309") << desc_decomp_of(U'\x1EF6') << "\n  Line: 1EF6;LATIN CAPITAL LETTER Y WITH HOOK ABOVE;Lu;0;L;0059 0309;;;;N;;;;1EF7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF7'), U"\x0079\x0309") << desc_decomp_of(U'\x1EF7') << "\n  Line: 1EF7;LATIN SMALL LETTER Y WITH HOOK ABOVE;Ll;0;L;0079 0309;;;;N;;;1EF6;;1EF6";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF8'), U"\x0059\x0303") << desc_decomp_of(U'\x1EF8') << "\n  Line: 1EF8;LATIN CAPITAL LETTER Y WITH TILDE;Lu;0;L;0059 0303;;;;N;;;;1EF9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1EF9'), U"\x0079\x0303") << desc_decomp_of(U'\x1EF9') << "\n  Line: 1EF9;LATIN SMALL LETTER Y WITH TILDE;Ll;0;L;0079 0303;;;;N;;;1EF8;;1EF8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F00'), U"\x03B1\x0313") << desc_decomp_of(U'\x1F00') << "\n  Line: 1F00;GREEK SMALL LETTER ALPHA WITH PSILI;Ll;0;L;03B1 0313;;;;N;;;1F08;;1F08";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F01'), U"\x03B1\x0314") << desc_decomp_of(U'\x1F01') << "\n  Line: 1F01;GREEK SMALL LETTER ALPHA WITH DASIA;Ll;0;L;03B1 0314;;;;N;;;1F09;;1F09";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F02'), U"\x1F00\x0300") << desc_decomp_of(U'\x1F02') << "\n  Line: 1F02;GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA;Ll;0;L;1F00 0300;;;;N;;;1F0A;;1F0A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F03'), U"\x1F01\x0300") << desc_decomp_of(U'\x1F03') << "\n  Line: 1F03;GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA;Ll;0;L;1F01 0300;;;;N;;;1F0B;;1F0B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F04'), U"\x1F00\x0301") << desc_decomp_of(U'\x1F04') << "\n  Line: 1F04;GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA;Ll;0;L;1F00 0301;;;;N;;;1F0C;;1F0C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F05'), U"\x1F01\x0301") << desc_decomp_of(U'\x1F05') << "\n  Line: 1F05;GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA;Ll;0;L;1F01 0301;;;;N;;;1F0D;;1F0D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F06'), U"\x1F00\x0342") << desc_decomp_of(U'\x1F06') << "\n  Line: 1F06;GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI;Ll;0;L;1F00 0342;;;;N;;;1F0E;;1F0E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F07'), U"\x1F01\x0342") << desc_decomp_of(U'\x1F07') << "\n  Line: 1F07;GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI;Ll;0;L;1F01 0342;;;;N;;;1F0F;;1F0F";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F08'), U"\x0391\x0313") << desc_decomp_of(U'\x1F08') << "\n  Line: 1F08;GREEK CAPITAL LETTER ALPHA WITH PSILI;Lu;0;L;0391 0313;;;;N;;;;1F00;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F09'), U"\x0391\x0314") << desc_decomp_of(U'\x1F09') << "\n  Line: 1F09;GREEK CAPITAL LETTER ALPHA WITH DASIA;Lu;0;L;0391 0314;;;;N;;;;1F01;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F0A'), U"\x1F08\x0300") << desc_decomp_of(U'\x1F0A') << "\n  Line: 1F0A;GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA;Lu;0;L;1F08 0300;;;;N;;;;1F02;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F0B'), U"\x1F09\x0300") << desc_decomp_of(U'\x1F0B') << "\n  Line: 1F0B;GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA;Lu;0;L;1F09 0300;;;;N;;;;1F03;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F0C'), U"\x1F08\x0301") << desc_decomp_of(U'\x1F0C') << "\n  Line: 1F0C;GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA;Lu;0;L;1F08 0301;;;;N;;;;1F04;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F0D'), U"\x1F09\x0301") << desc_decomp_of(U'\x1F0D') << "\n  Line: 1F0D;GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA;Lu;0;L;1F09 0301;;;;N;;;;1F05;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F0E'), U"\x1F08\x0342") << desc_decomp_of(U'\x1F0E') << "\n  Line: 1F0E;GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI;Lu;0;L;1F08 0342;;;;N;;;;1F06;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F0F'), U"\x1F09\x0342") << desc_decomp_of(U'\x1F0F') << "\n  Line: 1F0F;GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI;Lu;0;L;1F09 0342;;;;N;;;;1F07;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F10'), U"\x03B5\x0313") << desc_decomp_of(U'\x1F10') << "\n  Line: 1F10;GREEK SMALL LETTER EPSILON WITH PSILI;Ll;0;L;03B5 0313;;;;N;;;1F18;;1F18";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F11'), U"\x03B5\x0314") << desc_decomp_of(U'\x1F11') << "\n  Line: 1F11;GREEK SMALL LETTER EPSILON WITH DASIA;Ll;0;L;03B5 0314;;;;N;;;1F19;;1F19";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F12'), U"\x1F10\x0300") << desc_decomp_of(U'\x1F12') << "\n  Line: 1F12;GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA;Ll;0;L;1F10 0300;;;;N;;;1F1A;;1F1A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F13'), U"\x1F11\x0300") << desc_decomp_of(U'\x1F13') << "\n  Line: 1F13;GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA;Ll;0;L;1F11 0300;;;;N;;;1F1B;;1F1B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F14'), U"\x1F10\x0301") << desc_decomp_of(U'\x1F14') << "\n  Line: 1F14;GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA;Ll;0;L;1F10 0301;;;;N;;;1F1C;;1F1C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F15'), U"\x1F11\x0301") << desc_decomp_of(U'\x1F15') << "\n  Line: 1F15;GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA;Ll;0;L;1F11 0301;;;;N;;;1F1D;;1F1D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F18'), U"\x0395\x0313") << desc_decomp_of(U'\x1F18') << "\n  Line: 1F18;GREEK CAPITAL LETTER EPSILON WITH PSILI;Lu;0;L;0395 0313;;;;N;;;;1F10;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F19'), U"\x0395\x0314") << desc_decomp_of(U'\x1F19') << "\n  Line: 1F19;GREEK CAPITAL LETTER EPSILON WITH DASIA;Lu;0;L;0395 0314;;;;N;;;;1F11;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F1A'), U"\x1F18\x0300") << desc_decomp_of(U'\x1F1A') << "\n  Line: 1F1A;GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA;Lu;0;L;1F18 0300;;;;N;;;;1F12;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F1B'), U"\x1F19\x0300") << desc_decomp_of(U'\x1F1B') << "\n  Line: 1F1B;GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA;Lu;0;L;1F19 0300;;;;N;;;;1F13;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F1C'), U"\x1F18\x0301") << desc_decomp_of(U'\x1F1C') << "\n  Line: 1F1C;GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA;Lu;0;L;1F18 0301;;;;N;;;;1F14;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F1D'), U"\x1F19\x0301") << desc_decomp_of(U'\x1F1D') << "\n  Line: 1F1D;GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA;Lu;0;L;1F19 0301;;;;N;;;;1F15;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F20'), U"\x03B7\x0313") << desc_decomp_of(U'\x1F20') << "\n  Line: 1F20;GREEK SMALL LETTER ETA WITH PSILI;Ll;0;L;03B7 0313;;;;N;;;1F28;;1F28";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F21'), U"\x03B7\x0314") << desc_decomp_of(U'\x1F21') << "\n  Line: 1F21;GREEK SMALL LETTER ETA WITH DASIA;Ll;0;L;03B7 0314;;;;N;;;1F29;;1F29";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F22'), U"\x1F20\x0300") << desc_decomp_of(U'\x1F22') << "\n  Line: 1F22;GREEK SMALL LETTER ETA WITH PSILI AND VARIA;Ll;0;L;1F20 0300;;;;N;;;1F2A;;1F2A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F23'), U"\x1F21\x0300") << desc_decomp_of(U'\x1F23') << "\n  Line: 1F23;GREEK SMALL LETTER ETA WITH DASIA AND VARIA;Ll;0;L;1F21 0300;;;;N;;;1F2B;;1F2B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F24'), U"\x1F20\x0301") << desc_decomp_of(U'\x1F24') << "\n  Line: 1F24;GREEK SMALL LETTER ETA WITH PSILI AND OXIA;Ll;0;L;1F20 0301;;;;N;;;1F2C;;1F2C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F25'), U"\x1F21\x0301") << desc_decomp_of(U'\x1F25') << "\n  Line: 1F25;GREEK SMALL LETTER ETA WITH DASIA AND OXIA;Ll;0;L;1F21 0301;;;;N;;;1F2D;;1F2D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F26'), U"\x1F20\x0342") << desc_decomp_of(U'\x1F26') << "\n  Line: 1F26;GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI;Ll;0;L;1F20 0342;;;;N;;;1F2E;;1F2E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F27'), U"\x1F21\x0342") << desc_decomp_of(U'\x1F27') << "\n  Line: 1F27;GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI;Ll;0;L;1F21 0342;;;;N;;;1F2F;;1F2F";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F28'), U"\x0397\x0313") << desc_decomp_of(U'\x1F28') << "\n  Line: 1F28;GREEK CAPITAL LETTER ETA WITH PSILI;Lu;0;L;0397 0313;;;;N;;;;1F20;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F29'), U"\x0397\x0314") << desc_decomp_of(U'\x1F29') << "\n  Line: 1F29;GREEK CAPITAL LETTER ETA WITH DASIA;Lu;0;L;0397 0314;;;;N;;;;1F21;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F2A'), U"\x1F28\x0300") << desc_decomp_of(U'\x1F2A') << "\n  Line: 1F2A;GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA;Lu;0;L;1F28 0300;;;;N;;;;1F22;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F2B'), U"\x1F29\x0300") << desc_decomp_of(U'\x1F2B') << "\n  Line: 1F2B;GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA;Lu;0;L;1F29 0300;;;;N;;;;1F23;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F2C'), U"\x1F28\x0301") << desc_decomp_of(U'\x1F2C') << "\n  Line: 1F2C;GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA;Lu;0;L;1F28 0301;;;;N;;;;1F24;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F2D'), U"\x1F29\x0301") << desc_decomp_of(U'\x1F2D') << "\n  Line: 1F2D;GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA;Lu;0;L;1F29 0301;;;;N;;;;1F25;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F2E'), U"\x1F28\x0342") << desc_decomp_of(U'\x1F2E') << "\n  Line: 1F2E;GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI;Lu;0;L;1F28 0342;;;;N;;;;1F26;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F2F'), U"\x1F29\x0342") << desc_decomp_of(U'\x1F2F') << "\n  Line: 1F2F;GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI;Lu;0;L;1F29 0342;;;;N;;;;1F27;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F30'), U"\x03B9\x0313") << desc_decomp_of(U'\x1F30') << "\n  Line: 1F30;GREEK SMALL LETTER IOTA WITH PSILI;Ll;0;L;03B9 0313;;;;N;;;1F38;;1F38";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F31'), U"\x03B9\x0314") << desc_decomp_of(U'\x1F31') << "\n  Line: 1F31;GREEK SMALL LETTER IOTA WITH DASIA;Ll;0;L;03B9 0314;;;;N;;;1F39;;1F39";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F32'), U"\x1F30\x0300") << desc_decomp_of(U'\x1F32') << "\n  Line: 1F32;GREEK SMALL LETTER IOTA WITH PSILI AND VARIA;Ll;0;L;1F30 0300;;;;N;;;1F3A;;1F3A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F33'), U"\x1F31\x0300") << desc_decomp_of(U'\x1F33') << "\n  Line: 1F33;GREEK SMALL LETTER IOTA WITH DASIA AND VARIA;Ll;0;L;1F31 0300;;;;N;;;1F3B;;1F3B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F34'), U"\x1F30\x0301") << desc_decomp_of(U'\x1F34') << "\n  Line: 1F34;GREEK SMALL LETTER IOTA WITH PSILI AND OXIA;Ll;0;L;1F30 0301;;;;N;;;1F3C;;1F3C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F35'), U"\x1F31\x0301") << desc_decomp_of(U'\x1F35') << "\n  Line: 1F35;GREEK SMALL LETTER IOTA WITH DASIA AND OXIA;Ll;0;L;1F31 0301;;;;N;;;1F3D;;1F3D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F36'), U"\x1F30\x0342") << desc_decomp_of(U'\x1F36') << "\n  Line: 1F36;GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI;Ll;0;L;1F30 0342;;;;N;;;1F3E;;1F3E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F37'), U"\x1F31\x0342") << desc_decomp_of(U'\x1F37') << "\n  Line: 1F37;GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI;Ll;0;L;1F31 0342;;;;N;;;1F3F;;1F3F";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F38'), U"\x0399\x0313") << desc_decomp_of(U'\x1F38') << "\n  Line: 1F38;GREEK CAPITAL LETTER IOTA WITH PSILI;Lu;0;L;0399 0313;;;;N;;;;1F30;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F39'), U"\x0399\x0314") << desc_decomp_of(U'\x1F39') << "\n  Line: 1F39;GREEK CAPITAL LETTER IOTA WITH DASIA;Lu;0;L;0399 0314;;;;N;;;;1F31;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F3A'), U"\x1F38\x0300") << desc_decomp_of(U'\x1F3A') << "\n  Line: 1F3A;GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA;Lu;0;L;1F38 0300;;;;N;;;;1F32;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F3B'), U"\x1F39\x0300") << desc_decomp_of(U'\x1F3B') << "\n  Line: 1F3B;GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA;Lu;0;L;1F39 0300;;;;N;;;;1F33;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F3C'), U"\x1F38\x0301") << desc_decomp_of(U'\x1F3C') << "\n  Line: 1F3C;GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA;Lu;0;L;1F38 0301;;;;N;;;;1F34;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F3D'), U"\x1F39\x0301") << desc_decomp_of(U'\x1F3D') << "\n  Line: 1F3D;GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA;Lu;0;L;1F39 0301;;;;N;;;;1F35;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F3E'), U"\x1F38\x0342") << desc_decomp_of(U'\x1F3E') << "\n  Line: 1F3E;GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI;Lu;0;L;1F38 0342;;;;N;;;;1F36;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F3F'), U"\x1F39\x0342") << desc_decomp_of(U'\x1F3F') << "\n  Line: 1F3F;GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI;Lu;0;L;1F39 0342;;;;N;;;;1F37;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F40'), U"\x03BF\x0313") << desc_decomp_of(U'\x1F40') << "\n  Line: 1F40;GREEK SMALL LETTER OMICRON WITH PSILI;Ll;0;L;03BF 0313;;;;N;;;1F48;;1F48";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F41'), U"\x03BF\x0314") << desc_decomp_of(U'\x1F41') << "\n  Line: 1F41;GREEK SMALL LETTER OMICRON WITH DASIA;Ll;0;L;03BF 0314;;;;N;;;1F49;;1F49";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F42'), U"\x1F40\x0300") << desc_decomp_of(U'\x1F42') << "\n  Line: 1F42;GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA;Ll;0;L;1F40 0300;;;;N;;;1F4A;;1F4A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F43'), U"\x1F41\x0300") << desc_decomp_of(U'\x1F43') << "\n  Line: 1F43;GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA;Ll;0;L;1F41 0300;;;;N;;;1F4B;;1F4B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F44'), U"\x1F40\x0301") << desc_decomp_of(U'\x1F44') << "\n  Line: 1F44;GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA;Ll;0;L;1F40 0301;;;;N;;;1F4C;;1F4C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F45'), U"\x1F41\x0301") << desc_decomp_of(U'\x1F45') << "\n  Line: 1F45;GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA;Ll;0;L;1F41 0301;;;;N;;;1F4D;;1F4D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F48'), U"\x039F\x0313") << desc_decomp_of(U'\x1F48') << "\n  Line: 1F48;GREEK CAPITAL LETTER OMICRON WITH PSILI;Lu;0;L;039F 0313;;;;N;;;;1F40;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F49'), U"\x039F\x0314") << desc_decomp_of(U'\x1F49') << "\n  Line: 1F49;GREEK CAPITAL LETTER OMICRON WITH DASIA;Lu;0;L;039F 0314;;;;N;;;;1F41;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F4A'), U"\x1F48\x0300") << desc_decomp_of(U'\x1F4A') << "\n  Line: 1F4A;GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA;Lu;0;L;1F48 0300;;;;N;;;;1F42;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F4B'), U"\x1F49\x0300") << desc_decomp_of(U'\x1F4B') << "\n  Line: 1F4B;GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA;Lu;0;L;1F49 0300;;;;N;;;;1F43;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F4C'), U"\x1F48\x0301") << desc_decomp_of(U'\x1F4C') << "\n  Line: 1F4C;GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA;Lu;0;L;1F48 0301;;;;N;;;;1F44;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F4D'), U"\x1F49\x0301") << desc_decomp_of(U'\x1F4D') << "\n  Line: 1F4D;GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA;Lu;0;L;1F49 0301;;;;N;;;;1F45;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F50'), U"\x03C5\x0313") << desc_decomp_of(U'\x1F50') << "\n  Line: 1F50;GREEK SMALL LETTER UPSILON WITH PSILI;Ll;0;L;03C5 0313;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F51'), U"\x03C5\x0314") << desc_decomp_of(U'\x1F51') << "\n  Line: 1F51;GREEK SMALL LETTER UPSILON WITH DASIA;Ll;0;L;03C5 0314;;;;N;;;1F59;;1F59";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F52'), U"\x1F50\x0300") << desc_decomp_of(U'\x1F52') << "\n  Line: 1F52;GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA;Ll;0;L;1F50 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F53'), U"\x1F51\x0300") << desc_decomp_of(U'\x1F53') << "\n  Line: 1F53;GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA;Ll;0;L;1F51 0300;;;;N;;;1F5B;;1F5B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F54'), U"\x1F50\x0301") << desc_decomp_of(U'\x1F54') << "\n  Line: 1F54;GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA;Ll;0;L;1F50 0301;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F55'), U"\x1F51\x0301") << desc_decomp_of(U'\x1F55') << "\n  Line: 1F55;GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA;Ll;0;L;1F51 0301;;;;N;;;1F5D;;1F5D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F56'), U"\x1F50\x0342") << desc_decomp_of(U'\x1F56') << "\n  Line: 1F56;GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI;Ll;0;L;1F50 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F57'), U"\x1F51\x0342") << desc_decomp_of(U'\x1F57') << "\n  Line: 1F57;GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI;Ll;0;L;1F51 0342;;;;N;;;1F5F;;1F5F";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F59'), U"\x03A5\x0314") << desc_decomp_of(U'\x1F59') << "\n  Line: 1F59;GREEK CAPITAL LETTER UPSILON WITH DASIA;Lu;0;L;03A5 0314;;;;N;;;;1F51;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F5B'), U"\x1F59\x0300") << desc_decomp_of(U'\x1F5B') << "\n  Line: 1F5B;GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA;Lu;0;L;1F59 0300;;;;N;;;;1F53;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F5D'), U"\x1F59\x0301") << desc_decomp_of(U'\x1F5D') << "\n  Line: 1F5D;GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA;Lu;0;L;1F59 0301;;;;N;;;;1F55;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F5F'), U"\x1F59\x0342") << desc_decomp_of(U'\x1F5F') << "\n  Line: 1F5F;GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI;Lu;0;L;1F59 0342;;;;N;;;;1F57;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F60'), U"\x03C9\x0313") << desc_decomp_of(U'\x1F60') << "\n  Line: 1F60;GREEK SMALL LETTER OMEGA WITH PSILI;Ll;0;L;03C9 0313;;;;N;;;1F68;;1F68";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F61'), U"\x03C9\x0314") << desc_decomp_of(U'\x1F61') << "\n  Line: 1F61;GREEK SMALL LETTER OMEGA WITH DASIA;Ll;0;L;03C9 0314;;;;N;;;1F69;;1F69";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F62'), U"\x1F60\x0300") << desc_decomp_of(U'\x1F62') << "\n  Line: 1F62;GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA;Ll;0;L;1F60 0300;;;;N;;;1F6A;;1F6A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F63'), U"\x1F61\x0300") << desc_decomp_of(U'\x1F63') << "\n  Line: 1F63;GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA;Ll;0;L;1F61 0300;;;;N;;;1F6B;;1F6B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F64'), U"\x1F60\x0301") << desc_decomp_of(U'\x1F64') << "\n  Line: 1F64;GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA;Ll;0;L;1F60 0301;;;;N;;;1F6C;;1F6C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F65'), U"\x1F61\x0301") << desc_decomp_of(U'\x1F65') << "\n  Line: 1F65;GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA;Ll;0;L;1F61 0301;;;;N;;;1F6D;;1F6D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F66'), U"\x1F60\x0342") << desc_decomp_of(U'\x1F66') << "\n  Line: 1F66;GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI;Ll;0;L;1F60 0342;;;;N;;;1F6E;;1F6E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F67'), U"\x1F61\x0342") << desc_decomp_of(U'\x1F67') << "\n  Line: 1F67;GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI;Ll;0;L;1F61 0342;;;;N;;;1F6F;;1F6F";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F68'), U"\x03A9\x0313") << desc_decomp_of(U'\x1F68') << "\n  Line: 1F68;GREEK CAPITAL LETTER OMEGA WITH PSILI;Lu;0;L;03A9 0313;;;;N;;;;1F60;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F69'), U"\x03A9\x0314") << desc_decomp_of(U'\x1F69') << "\n  Line: 1F69;GREEK CAPITAL LETTER OMEGA WITH DASIA;Lu;0;L;03A9 0314;;;;N;;;;1F61;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F6A'), U"\x1F68\x0300") << desc_decomp_of(U'\x1F6A') << "\n  Line: 1F6A;GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA;Lu;0;L;1F68 0300;;;;N;;;;1F62;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F6B'), U"\x1F69\x0300") << desc_decomp_of(U'\x1F6B') << "\n  Line: 1F6B;GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA;Lu;0;L;1F69 0300;;;;N;;;;1F63;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F6C'), U"\x1F68\x0301") << desc_decomp_of(U'\x1F6C') << "\n  Line: 1F6C;GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA;Lu;0;L;1F68 0301;;;;N;;;;1F64;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F6D'), U"\x1F69\x0301") << desc_decomp_of(U'\x1F6D') << "\n  Line: 1F6D;GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA;Lu;0;L;1F69 0301;;;;N;;;;1F65;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F6E'), U"\x1F68\x0342") << desc_decomp_of(U'\x1F6E') << "\n  Line: 1F6E;GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI;Lu;0;L;1F68 0342;;;;N;;;;1F66;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F6F'), U"\x1F69\x0342") << desc_decomp_of(U'\x1F6F') << "\n  Line: 1F6F;GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI;Lu;0;L;1F69 0342;;;;N;;;;1F67;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F70'), U"\x03B1\x0300") << desc_decomp_of(U'\x1F70') << "\n  Line: 1F70;GREEK SMALL LETTER ALPHA WITH VARIA;Ll;0;L;03B1 0300;;;;N;;;1FBA;;1FBA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F71'), U"\x03AC") << desc_decomp_of(U'\x1F71') << "\n  Line: 1F71;GREEK SMALL LETTER ALPHA WITH OXIA;Ll;0;L;03AC;;;;N;;;1FBB;;1FBB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F72'), U"\x03B5\x0300") << desc_decomp_of(U'\x1F72') << "\n  Line: 1F72;GREEK SMALL LETTER EPSILON WITH VARIA;Ll;0;L;03B5 0300;;;;N;;;1FC8;;1FC8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F73'), U"\x03AD") << desc_decomp_of(U'\x1F73') << "\n  Line: 1F73;GREEK SMALL LETTER EPSILON WITH OXIA;Ll;0;L;03AD;;;;N;;;1FC9;;1FC9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F74'), U"\x03B7\x0300") << desc_decomp_of(U'\x1F74') << "\n  Line: 1F74;GREEK SMALL LETTER ETA WITH VARIA;Ll;0;L;03B7 0300;;;;N;;;1FCA;;1FCA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F75'), U"\x03AE") << desc_decomp_of(U'\x1F75') << "\n  Line: 1F75;GREEK SMALL LETTER ETA WITH OXIA;Ll;0;L;03AE;;;;N;;;1FCB;;1FCB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F76'), U"\x03B9\x0300") << desc_decomp_of(U'\x1F76') << "\n  Line: 1F76;GREEK SMALL LETTER IOTA WITH VARIA;Ll;0;L;03B9 0300;;;;N;;;1FDA;;1FDA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F77'), U"\x03AF") << desc_decomp_of(U'\x1F77') << "\n  Line: 1F77;GREEK SMALL LETTER IOTA WITH OXIA;Ll;0;L;03AF;;;;N;;;1FDB;;1FDB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F78'), U"\x03BF\x0300") << desc_decomp_of(U'\x1F78') << "\n  Line: 1F78;GREEK SMALL LETTER OMICRON WITH VARIA;Ll;0;L;03BF 0300;;;;N;;;1FF8;;1FF8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F79'), U"\x03CC") << desc_decomp_of(U'\x1F79') << "\n  Line: 1F79;GREEK SMALL LETTER OMICRON WITH OXIA;Ll;0;L;03CC;;;;N;;;1FF9;;1FF9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F7A'), U"\x03C5\x0300") << desc_decomp_of(U'\x1F7A') << "\n  Line: 1F7A;GREEK SMALL LETTER UPSILON WITH VARIA;Ll;0;L;03C5 0300;;;;N;;;1FEA;;1FEA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F7B'), U"\x03CD") << desc_decomp_of(U'\x1F7B') << "\n  Line: 1F7B;GREEK SMALL LETTER UPSILON WITH OXIA;Ll;0;L;03CD;;;;N;;;1FEB;;1FEB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F7C'), U"\x03C9\x0300") << desc_decomp_of(U'\x1F7C') << "\n  Line: 1F7C;GREEK SMALL LETTER OMEGA WITH VARIA;Ll;0;L;03C9 0300;;;;N;;;1FFA;;1FFA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F7D'), U"\x03CE") << desc_decomp_of(U'\x1F7D') << "\n  Line: 1F7D;GREEK SMALL LETTER OMEGA WITH OXIA;Ll;0;L;03CE;;;;N;;;1FFB;;1FFB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F80'), U"\x1F00\x0345") << desc_decomp_of(U'\x1F80') << "\n  Line: 1F80;GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI;Ll;0;L;1F00 0345;;;;N;;;1F88;;1F88";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F81'), U"\x1F01\x0345") << desc_decomp_of(U'\x1F81') << "\n  Line: 1F81;GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI;Ll;0;L;1F01 0345;;;;N;;;1F89;;1F89";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F82'), U"\x1F02\x0345") << desc_decomp_of(U'\x1F82') << "\n  Line: 1F82;GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F02 0345;;;;N;;;1F8A;;1F8A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F83'), U"\x1F03\x0345") << desc_decomp_of(U'\x1F83') << "\n  Line: 1F83;GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F03 0345;;;;N;;;1F8B;;1F8B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F84'), U"\x1F04\x0345") << desc_decomp_of(U'\x1F84') << "\n  Line: 1F84;GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F04 0345;;;;N;;;1F8C;;1F8C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F85'), U"\x1F05\x0345") << desc_decomp_of(U'\x1F85') << "\n  Line: 1F85;GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F05 0345;;;;N;;;1F8D;;1F8D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F86'), U"\x1F06\x0345") << desc_decomp_of(U'\x1F86') << "\n  Line: 1F86;GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F06 0345;;;;N;;;1F8E;;1F8E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F87'), U"\x1F07\x0345") << desc_decomp_of(U'\x1F87') << "\n  Line: 1F87;GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F07 0345;;;;N;;;1F8F;;1F8F";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F88'), U"\x1F08\x0345") << desc_decomp_of(U'\x1F88') << "\n  Line: 1F88;GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI;Lt;0;L;1F08 0345;;;;N;;;;1F80;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F89'), U"\x1F09\x0345") << desc_decomp_of(U'\x1F89') << "\n  Line: 1F89;GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI;Lt;0;L;1F09 0345;;;;N;;;;1F81;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F8A'), U"\x1F0A\x0345") << desc_decomp_of(U'\x1F8A') << "\n  Line: 1F8A;GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F0A 0345;;;;N;;;;1F82;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F8B'), U"\x1F0B\x0345") << desc_decomp_of(U'\x1F8B') << "\n  Line: 1F8B;GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F0B 0345;;;;N;;;;1F83;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F8C'), U"\x1F0C\x0345") << desc_decomp_of(U'\x1F8C') << "\n  Line: 1F8C;GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F0C 0345;;;;N;;;;1F84;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F8D'), U"\x1F0D\x0345") << desc_decomp_of(U'\x1F8D') << "\n  Line: 1F8D;GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F0D 0345;;;;N;;;;1F85;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F8E'), U"\x1F0E\x0345") << desc_decomp_of(U'\x1F8E') << "\n  Line: 1F8E;GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F0E 0345;;;;N;;;;1F86;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F8F'), U"\x1F0F\x0345") << desc_decomp_of(U'\x1F8F') << "\n  Line: 1F8F;GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F0F 0345;;;;N;;;;1F87;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F90'), U"\x1F20\x0345") << desc_decomp_of(U'\x1F90') << "\n  Line: 1F90;GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI;Ll;0;L;1F20 0345;;;;N;;;1F98;;1F98";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F91'), U"\x1F21\x0345") << desc_decomp_of(U'\x1F91') << "\n  Line: 1F91;GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI;Ll;0;L;1F21 0345;;;;N;;;1F99;;1F99";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F92'), U"\x1F22\x0345") << desc_decomp_of(U'\x1F92') << "\n  Line: 1F92;GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F22 0345;;;;N;;;1F9A;;1F9A";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F93'), U"\x1F23\x0345") << desc_decomp_of(U'\x1F93') << "\n  Line: 1F93;GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F23 0345;;;;N;;;1F9B;;1F9B";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F94'), U"\x1F24\x0345") << desc_decomp_of(U'\x1F94') << "\n  Line: 1F94;GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F24 0345;;;;N;;;1F9C;;1F9C";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F95'), U"\x1F25\x0345") << desc_decomp_of(U'\x1F95') << "\n  Line: 1F95;GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F25 0345;;;;N;;;1F9D;;1F9D";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F96'), U"\x1F26\x0345") << desc_decomp_of(U'\x1F96') << "\n  Line: 1F96;GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F26 0345;;;;N;;;1F9E;;1F9E";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F97'), U"\x1F27\x0345") << desc_decomp_of(U'\x1F97') << "\n  Line: 1F97;GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F27 0345;;;;N;;;1F9F;;1F9F";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F98'), U"\x1F28\x0345") << desc_decomp_of(U'\x1F98') << "\n  Line: 1F98;GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI;Lt;0;L;1F28 0345;;;;N;;;;1F90;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F99'), U"\x1F29\x0345") << desc_decomp_of(U'\x1F99') << "\n  Line: 1F99;GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI;Lt;0;L;1F29 0345;;;;N;;;;1F91;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F9A'), U"\x1F2A\x0345") << desc_decomp_of(U'\x1F9A') << "\n  Line: 1F9A;GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F2A 0345;;;;N;;;;1F92;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F9B'), U"\x1F2B\x0345") << desc_decomp_of(U'\x1F9B') << "\n  Line: 1F9B;GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F2B 0345;;;;N;;;;1F93;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F9C'), U"\x1F2C\x0345") << desc_decomp_of(U'\x1F9C') << "\n  Line: 1F9C;GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F2C 0345;;;;N;;;;1F94;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F9D'), U"\x1F2D\x0345") << desc_decomp_of(U'\x1F9D') << "\n  Line: 1F9D;GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F2D 0345;;;;N;;;;1F95;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F9E'), U"\x1F2E\x0345") << desc_decomp_of(U'\x1F9E') << "\n  Line: 1F9E;GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F2E 0345;;;;N;;;;1F96;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1F9F'), U"\x1F2F\x0345") << desc_decomp_of(U'\x1F9F') << "\n  Line: 1F9F;GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F2F 0345;;;;N;;;;1F97;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA0'), U"\x1F60\x0345") << desc_decomp_of(U'\x1FA0') << "\n  Line: 1FA0;GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI;Ll;0;L;1F60 0345;;;;N;;;1FA8;;1FA8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA1'), U"\x1F61\x0345") << desc_decomp_of(U'\x1FA1') << "\n  Line: 1FA1;GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI;Ll;0;L;1F61 0345;;;;N;;;1FA9;;1FA9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA2'), U"\x1F62\x0345") << desc_decomp_of(U'\x1FA2') << "\n  Line: 1FA2;GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F62 0345;;;;N;;;1FAA;;1FAA";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA3'), U"\x1F63\x0345") << desc_decomp_of(U'\x1FA3') << "\n  Line: 1FA3;GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F63 0345;;;;N;;;1FAB;;1FAB";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA4'), U"\x1F64\x0345") << desc_decomp_of(U'\x1FA4') << "\n  Line: 1FA4;GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F64 0345;;;;N;;;1FAC;;1FAC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA5'), U"\x1F65\x0345") << desc_decomp_of(U'\x1FA5') << "\n  Line: 1FA5;GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F65 0345;;;;N;;;1FAD;;1FAD";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA6'), U"\x1F66\x0345") << desc_decomp_of(U'\x1FA6') << "\n  Line: 1FA6;GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F66 0345;;;;N;;;1FAE;;1FAE";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA7'), U"\x1F67\x0345") << desc_decomp_of(U'\x1FA7') << "\n  Line: 1FA7;GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F67 0345;;;;N;;;1FAF;;1FAF";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA8'), U"\x1F68\x0345") << desc_decomp_of(U'\x1FA8') << "\n  Line: 1FA8;GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI;Lt;0;L;1F68 0345;;;;N;;;;1FA0;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FA9'), U"\x1F69\x0345") << desc_decomp_of(U'\x1FA9') << "\n  Line: 1FA9;GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI;Lt;0;L;1F69 0345;;;;N;;;;1FA1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FAA'), U"\x1F6A\x0345") << desc_decomp_of(U'\x1FAA') << "\n  Line: 1FAA;GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F6A 0345;;;;N;;;;1FA2;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FAB'), U"\x1F6B\x0345") << desc_decomp_of(U'\x1FAB') << "\n  Line: 1FAB;GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F6B 0345;;;;N;;;;1FA3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FAC'), U"\x1F6C\x0345") << desc_decomp_of(U'\x1FAC') << "\n  Line: 1FAC;GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F6C 0345;;;;N;;;;1FA4;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FAD'), U"\x1F6D\x0345") << desc_decomp_of(U'\x1FAD') << "\n  Line: 1FAD;GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F6D 0345;;;;N;;;;1FA5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FAE'), U"\x1F6E\x0345") << desc_decomp_of(U'\x1FAE') << "\n  Line: 1FAE;GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F6E 0345;;;;N;;;;1FA6;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FAF'), U"\x1F6F\x0345") << desc_decomp_of(U'\x1FAF') << "\n  Line: 1FAF;GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F6F 0345;;;;N;;;;1FA7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB0'), U"\x03B1\x0306") << desc_decomp_of(U'\x1FB0') << "\n  Line: 1FB0;GREEK SMALL LETTER ALPHA WITH VRACHY;Ll;0;L;03B1 0306;;;;N;;;1FB8;;1FB8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB1'), U"\x03B1\x0304") << desc_decomp_of(U'\x1FB1') << "\n  Line: 1FB1;GREEK SMALL LETTER ALPHA WITH MACRON;Ll;0;L;03B1 0304;;;;N;;;1FB9;;1FB9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB2'), U"\x1F70\x0345") << desc_decomp_of(U'\x1FB2') << "\n  Line: 1FB2;GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI;Ll;0;L;1F70 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB3'), U"\x03B1\x0345") << desc_decomp_of(U'\x1FB3') << "\n  Line: 1FB3;GREEK SMALL LETTER ALPHA WITH YPOGEGRAMMENI;Ll;0;L;03B1 0345;;;;N;;;1FBC;;1FBC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB4'), U"\x03AC\x0345") << desc_decomp_of(U'\x1FB4') << "\n  Line: 1FB4;GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI;Ll;0;L;03AC 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB6'), U"\x03B1\x0342") << desc_decomp_of(U'\x1FB6') << "\n  Line: 1FB6;GREEK SMALL LETTER ALPHA WITH PERISPOMENI;Ll;0;L;03B1 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB7'), U"\x1FB6\x0345") << desc_decomp_of(U'\x1FB7') << "\n  Line: 1FB7;GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1FB6 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB8'), U"\x0391\x0306") << desc_decomp_of(U'\x1FB8') << "\n  Line: 1FB8;GREEK CAPITAL LETTER ALPHA WITH VRACHY;Lu;0;L;0391 0306;;;;N;;;;1FB0;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FB9'), U"\x0391\x0304") << desc_decomp_of(U'\x1FB9') << "\n  Line: 1FB9;GREEK CAPITAL LETTER ALPHA WITH MACRON;Lu;0;L;0391 0304;;;;N;;;;1FB1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FBA'), U"\x0391\x0300") << desc_decomp_of(U'\x1FBA') << "\n  Line: 1FBA;GREEK CAPITAL LETTER ALPHA WITH VARIA;Lu;0;L;0391 0300;;;;N;;;;1F70;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FBB'), U"\x0386") << desc_decomp_of(U'\x1FBB') << "\n  Line: 1FBB;GREEK CAPITAL LETTER ALPHA WITH OXIA;Lu;0;L;0386;;;;N;;;;1F71;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FBC'), U"\x0391\x0345") << desc_decomp_of(U'\x1FBC') << "\n  Line: 1FBC;GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI;Lt;0;L;0391 0345;;;;N;;;;1FB3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FBE'), U"\x03B9") << desc_decomp_of(U'\x1FBE') << "\n  Line: 1FBE;GREEK PROSGEGRAMMENI;Ll;0;L;03B9;;;;N;;;0399;;0399";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FC1'), U"\x00A8\x0342") << desc_decomp_of(U'\x1FC1') << "\n  Line: 1FC1;GREEK DIALYTIKA AND PERISPOMENI;Sk;0;ON;00A8 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FC2'), U"\x1F74\x0345") << desc_decomp_of(U'\x1FC2') << "\n  Line: 1FC2;GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI;Ll;0;L;1F74 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FC3'), U"\x03B7\x0345") << desc_decomp_of(U'\x1FC3') << "\n  Line: 1FC3;GREEK SMALL LETTER ETA WITH YPOGEGRAMMENI;Ll;0;L;03B7 0345;;;;N;;;1FCC;;1FCC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FC4'), U"\x03AE\x0345") << desc_decomp_of(U'\x1FC4') << "\n  Line: 1FC4;GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI;Ll;0;L;03AE 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FC6'), U"\x03B7\x0342") << desc_decomp_of(U'\x1FC6') << "\n  Line: 1FC6;GREEK SMALL LETTER ETA WITH PERISPOMENI;Ll;0;L;03B7 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FC7'), U"\x1FC6\x0345") << desc_decomp_of(U'\x1FC7') << "\n  Line: 1FC7;GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1FC6 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FC8'), U"\x0395\x0300") << desc_decomp_of(U'\x1FC8') << "\n  Line: 1FC8;GREEK CAPITAL LETTER EPSILON WITH VARIA;Lu;0;L;0395 0300;;;;N;;;;1F72;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FC9'), U"\x0388") << desc_decomp_of(U'\x1FC9') << "\n  Line: 1FC9;GREEK CAPITAL LETTER EPSILON WITH OXIA;Lu;0;L;0388;;;;N;;;;1F73;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FCA'), U"\x0397\x0300") << desc_decomp_of(U'\x1FCA') << "\n  Line: 1FCA;GREEK CAPITAL LETTER ETA WITH VARIA;Lu;0;L;0397 0300;;;;N;;;;1F74;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FCB'), U"\x0389") << desc_decomp_of(U'\x1FCB') << "\n  Line: 1FCB;GREEK CAPITAL LETTER ETA WITH OXIA;Lu;0;L;0389;;;;N;;;;1F75;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FCC'), U"\x0397\x0345") << desc_decomp_of(U'\x1FCC') << "\n  Line: 1FCC;GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI;Lt;0;L;0397 0345;;;;N;;;;1FC3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FCD'), U"\x1FBF\x0300") << desc_decomp_of(U'\x1FCD') << "\n  Line: 1FCD;GREEK PSILI AND VARIA;Sk;0;ON;1FBF 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FCE'), U"\x1FBF\x0301") << desc_decomp_of(U'\x1FCE') << "\n  Line: 1FCE;GREEK PSILI AND OXIA;Sk;0;ON;1FBF 0301;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FCF'), U"\x1FBF\x0342") << desc_decomp_of(U'\x1FCF') << "\n  Line: 1FCF;GREEK PSILI AND PERISPOMENI;Sk;0;ON;1FBF 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FD0'), U"\x03B9\x0306") << desc_decomp_of(U'\x1FD0') << "\n  Line: 1FD0;GREEK SMALL LETTER IOTA WITH VRACHY;Ll;0;L;03B9 0306;;;;N;;;1FD8;;1FD8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FD1'), U"\x03B9\x0304") << desc_decomp_of(U'\x1FD1') << "\n  Line: 1FD1;GREEK SMALL LETTER IOTA WITH MACRON;Ll;0;L;03B9 0304;;;;N;;;1FD9;;1FD9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FD2'), U"\x03CA\x0300") << desc_decomp_of(U'\x1FD2') << "\n  Line: 1FD2;GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA;Ll;0;L;03CA 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FD3'), U"\x0390") << desc_decomp_of(U'\x1FD3') << "\n  Line: 1FD3;GREEK SMALL LETTER IOTA WITH DIALYTIKA AND OXIA;Ll;0;L;0390;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FD6'), U"\x03B9\x0342") << desc_decomp_of(U'\x1FD6') << "\n  Line: 1FD6;GREEK SMALL LETTER IOTA WITH PERISPOMENI;Ll;0;L;03B9 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FD7'), U"\x03CA\x0342") << desc_decomp_of(U'\x1FD7') << "\n  Line: 1FD7;GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI;Ll;0;L;03CA 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FD8'), U"\x0399\x0306") << desc_decomp_of(U'\x1FD8') << "\n  Line: 1FD8;GREEK CAPITAL LETTER IOTA WITH VRACHY;Lu;0;L;0399 0306;;;;N;;;;1FD0;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FD9'), U"\x0399\x0304") << desc_decomp_of(U'\x1FD9') << "\n  Line: 1FD9;GREEK CAPITAL LETTER IOTA WITH MACRON;Lu;0;L;0399 0304;;;;N;;;;1FD1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FDA'), U"\x0399\x0300") << desc_decomp_of(U'\x1FDA') << "\n  Line: 1FDA;GREEK CAPITAL LETTER IOTA WITH VARIA;Lu;0;L;0399 0300;;;;N;;;;1F76;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FDB'), U"\x038A") << desc_decomp_of(U'\x1FDB') << "\n  Line: 1FDB;GREEK CAPITAL LETTER IOTA WITH OXIA;Lu;0;L;038A;;;;N;;;;1F77;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FDD'), U"\x1FFE\x0300") << desc_decomp_of(U'\x1FDD') << "\n  Line: 1FDD;GREEK DASIA AND VARIA;Sk;0;ON;1FFE 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FDE'), U"\x1FFE\x0301") << desc_decomp_of(U'\x1FDE') << "\n  Line: 1FDE;GREEK DASIA AND OXIA;Sk;0;ON;1FFE 0301;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FDF'), U"\x1FFE\x0342") << desc_decomp_of(U'\x1FDF') << "\n  Line: 1FDF;GREEK DASIA AND PERISPOMENI;Sk;0;ON;1FFE 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE0'), U"\x03C5\x0306") << desc_decomp_of(U'\x1FE0') << "\n  Line: 1FE0;GREEK SMALL LETTER UPSILON WITH VRACHY;Ll;0;L;03C5 0306;;;;N;;;1FE8;;1FE8";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE1'), U"\x03C5\x0304") << desc_decomp_of(U'\x1FE1') << "\n  Line: 1FE1;GREEK SMALL LETTER UPSILON WITH MACRON;Ll;0;L;03C5 0304;;;;N;;;1FE9;;1FE9";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE2'), U"\x03CB\x0300") << desc_decomp_of(U'\x1FE2') << "\n  Line: 1FE2;GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA;Ll;0;L;03CB 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE3'), U"\x03B0") << desc_decomp_of(U'\x1FE3') << "\n  Line: 1FE3;GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND OXIA;Ll;0;L;03B0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE4'), U"\x03C1\x0313") << desc_decomp_of(U'\x1FE4') << "\n  Line: 1FE4;GREEK SMALL LETTER RHO WITH PSILI;Ll;0;L;03C1 0313;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE5'), U"\x03C1\x0314") << desc_decomp_of(U'\x1FE5') << "\n  Line: 1FE5;GREEK SMALL LETTER RHO WITH DASIA;Ll;0;L;03C1 0314;;;;N;;;1FEC;;1FEC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE6'), U"\x03C5\x0342") << desc_decomp_of(U'\x1FE6') << "\n  Line: 1FE6;GREEK SMALL LETTER UPSILON WITH PERISPOMENI;Ll;0;L;03C5 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE7'), U"\x03CB\x0342") << desc_decomp_of(U'\x1FE7') << "\n  Line: 1FE7;GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI;Ll;0;L;03CB 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE8'), U"\x03A5\x0306") << desc_decomp_of(U'\x1FE8') << "\n  Line: 1FE8;GREEK CAPITAL LETTER UPSILON WITH VRACHY;Lu;0;L;03A5 0306;;;;N;;;;1FE0;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FE9'), U"\x03A5\x0304") << desc_decomp_of(U'\x1FE9') << "\n  Line: 1FE9;GREEK CAPITAL LETTER UPSILON WITH MACRON;Lu;0;L;03A5 0304;;;;N;;;;1FE1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FEA'), U"\x03A5\x0300") << desc_decomp_of(U'\x1FEA') << "\n  Line: 1FEA;GREEK CAPITAL LETTER UPSILON WITH VARIA;Lu;0;L;03A5 0300;;;;N;;;;1F7A;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FEB'), U"\x038E") << desc_decomp_of(U'\x1FEB') << "\n  Line: 1FEB;GREEK CAPITAL LETTER UPSILON WITH OXIA;Lu;0;L;038E;;;;N;;;;1F7B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FEC'), U"\x03A1\x0314") << desc_decomp_of(U'\x1FEC') << "\n  Line: 1FEC;GREEK CAPITAL LETTER RHO WITH DASIA;Lu;0;L;03A1 0314;;;;N;;;;1FE5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FED'), U"\x00A8\x0300") << desc_decomp_of(U'\x1FED') << "\n  Line: 1FED;GREEK DIALYTIKA AND VARIA;Sk;0;ON;00A8 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FEE'), U"\x0385") << desc_decomp_of(U'\x1FEE') << "\n  Line: 1FEE;GREEK DIALYTIKA AND OXIA;Sk;0;ON;0385;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FEF'), U"\x0060") << desc_decomp_of(U'\x1FEF') << "\n  Line: 1FEF;GREEK VARIA;Sk;0;ON;0060;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FF2'), U"\x1F7C\x0345") << desc_decomp_of(U'\x1FF2') << "\n  Line: 1FF2;GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI;Ll;0;L;1F7C 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FF3'), U"\x03C9\x0345") << desc_decomp_of(U'\x1FF3') << "\n  Line: 1FF3;GREEK SMALL LETTER OMEGA WITH YPOGEGRAMMENI;Ll;0;L;03C9 0345;;;;N;;;1FFC;;1FFC";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FF4'), U"\x03CE\x0345") << desc_decomp_of(U'\x1FF4') << "\n  Line: 1FF4;GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI;Ll;0;L;03CE 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FF6'), U"\x03C9\x0342") << desc_decomp_of(U'\x1FF6') << "\n  Line: 1FF6;GREEK SMALL LETTER OMEGA WITH PERISPOMENI;Ll;0;L;03C9 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FF7'), U"\x1FF6\x0345") << desc_decomp_of(U'\x1FF7') << "\n  Line: 1FF7;GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1FF6 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FF8'), U"\x039F\x0300") << desc_decomp_of(U'\x1FF8') << "\n  Line: 1FF8;GREEK CAPITAL LETTER OMICRON WITH VARIA;Lu;0;L;039F 0300;;;;N;;;;1F78;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FF9'), U"\x038C") << desc_decomp_of(U'\x1FF9') << "\n  Line: 1FF9;GREEK CAPITAL LETTER OMICRON WITH OXIA;Lu;0;L;038C;;;;N;;;;1F79;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FFA'), U"\x03A9\x0300") << desc_decomp_of(U'\x1FFA') << "\n  Line: 1FFA;GREEK CAPITAL LETTER OMEGA WITH VARIA;Lu;0;L;03A9 0300;;;;N;;;;1F7C;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FFB'), U"\x038F") << desc_decomp_of(U'\x1FFB') << "\n  Line: 1FFB;GREEK CAPITAL LETTER OMEGA WITH OXIA;Lu;0;L;038F;;;;N;;;;1F7D;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FFC'), U"\x03A9\x0345") << desc_decomp_of(U'\x1FFC') << "\n  Line: 1FFC;GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI;Lt;0;L;03A9 0345;;;;N;;;;1FF3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1FFD'), U"\x00B4") << desc_decomp_of(U'\x1FFD') << "\n  Line: 1FFD;GREEK OXIA;Sk;0;ON;00B4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2000'), U"\x2002") << desc_decomp_of(U'\x2000') << "\n  Line: 2000;EN QUAD;Zs;0;WS;2002;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2001'), U"\x2003") << desc_decomp_of(U'\x2001') << "\n  Line: 2001;EM QUAD;Zs;0;WS;2003;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2126'), U"\x03A9") << desc_decomp_of(U'\x2126') << "\n  Line: 2126;OHM SIGN;Lu;0;L;03A9;;;;N;OHM;;;03C9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x212A'), U"\x004B") << desc_decomp_of(U'\x212A') << "\n  Line: 212A;KELVIN SIGN;Lu;0;L;004B;;;;N;DEGREES KELVIN;;;006B;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x212B'), U"\x00C5") << desc_decomp_of(U'\x212B') << "\n  Line: 212B;ANGSTROM SIGN;Lu;0;L;00C5;;;;N;ANGSTROM UNIT;;;00E5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x219A'), U"\x2190\x0338") << desc_decomp_of(U'\x219A') << "\n  Line: 219A;LEFTWARDS ARROW WITH STROKE;Sm;0;ON;2190 0338;;;;N;LEFT ARROW WITH STROKE;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x219B'), U"\x2192\x0338") << desc_decomp_of(U'\x219B') << "\n  Line: 219B;RIGHTWARDS ARROW WITH STROKE;Sm;0;ON;2192 0338;;;;N;RIGHT ARROW WITH STROKE;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x21AE'), U"\x2194\x0338") << desc_decomp_of(U'\x21AE') << "\n  Line: 21AE;LEFT RIGHT ARROW WITH STROKE;Sm;0;ON;2194 0338;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x21CD'), U"\x21D0\x0338") << desc_decomp_of(U'\x21CD') << "\n  Line: 21CD;LEFTWARDS DOUBLE ARROW WITH STROKE;So;0;ON;21D0 0338;;;;N;LEFT DOUBLE ARROW WITH STROKE;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x21CE'), U"\x21D4\x0338") << desc_decomp_of(U'\x21CE') << "\n  Line: 21CE;LEFT RIGHT DOUBLE ARROW WITH STROKE;Sm;0;ON;21D4 0338;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x21CF'), U"\x21D2\x0338") << desc_decomp_of(U'\x21CF') << "\n  Line: 21CF;RIGHTWARDS DOUBLE ARROW WITH STROKE;Sm;0;ON;21D2 0338;;;;N;RIGHT DOUBLE ARROW WITH STROKE;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2204'), U"\x2203\x0338") << desc_decomp_of(U'\x2204') << "\n  Line: 2204;THERE DOES NOT EXIST;Sm;0;ON;2203 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2209'), U"\x2208\x0338") << desc_decomp_of(U'\x2209') << "\n  Line: 2209;NOT AN ELEMENT OF;Sm;0;ON;2208 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x220C'), U"\x220B\x0338") << desc_decomp_of(U'\x220C') << "\n  Line: 220C;DOES NOT CONTAIN AS MEMBER;Sm;0;ON;220B 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2224'), U"\x2223\x0338") << desc_decomp_of(U'\x2224') << "\n  Line: 2224;DOES NOT DIVIDE;Sm;0;ON;2223 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2226'), U"\x2225\x0338") << desc_decomp_of(U'\x2226') << "\n  Line: 2226;NOT PARALLEL TO;Sm;0;ON;2225 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2241'), U"\x223C\x0338") << desc_decomp_of(U'\x2241') << "\n  Line: 2241;NOT TILDE;Sm;0;ON;223C 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2244'), U"\x2243\x0338") << desc_decomp_of(U'\x2244') << "\n  Line: 2244;NOT ASYMPTOTICALLY EQUAL TO;Sm;0;ON;2243 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2247'), U"\x2245\x0338") << desc_decomp_of(U'\x2247') << "\n  Line: 2247;NEITHER APPROXIMATELY NOR ACTUALLY EQUAL TO;Sm;0;ON;2245 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2249'), U"\x2248\x0338") << desc_decomp_of(U'\x2249') << "\n  Line: 2249;NOT ALMOST EQUAL TO;Sm;0;ON;2248 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2260'), U"\x003D\x0338") << desc_decomp_of(U'\x2260') << "\n  Line: 2260;NOT EQUAL TO;Sm;0;ON;003D 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2262'), U"\x2261\x0338") << desc_decomp_of(U'\x2262') << "\n  Line: 2262;NOT IDENTICAL TO;Sm;0;ON;2261 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x226D'), U"\x224D\x0338") << desc_decomp_of(U'\x226D') << "\n  Line: 226D;NOT EQUIVALENT TO;Sm;0;ON;224D 0338;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x226E'), U"\x003C\x0338") << desc_decomp_of(U'\x226E') << "\n  Line: 226E;NOT LESS-THAN;Sm;0;ON;003C 0338;;;;Y;NOT LESS THAN;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x226F'), U"\x003E\x0338") << desc_decomp_of(U'\x226F') << "\n  Line: 226F;NOT GREATER-THAN;Sm;0;ON;003E 0338;;;;Y;NOT GREATER THAN;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2270'), U"\x2264\x0338") << desc_decomp_of(U'\x2270') << "\n  Line: 2270;NEITHER LESS-THAN NOR EQUAL TO;Sm;0;ON;2264 0338;;;;Y;NEITHER LESS THAN NOR EQUAL TO;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2271'), U"\x2265\x0338") << desc_decomp_of(U'\x2271') << "\n  Line: 2271;NEITHER GREATER-THAN NOR EQUAL TO;Sm;0;ON;2265 0338;;;;Y;NEITHER GREATER THAN NOR EQUAL TO;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2274'), U"\x2272\x0338") << desc_decomp_of(U'\x2274') << "\n  Line: 2274;NEITHER LESS-THAN NOR EQUIVALENT TO;Sm;0;ON;2272 0338;;;;Y;NEITHER LESS THAN NOR EQUIVALENT TO;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2275'), U"\x2273\x0338") << desc_decomp_of(U'\x2275') << "\n  Line: 2275;NEITHER GREATER-THAN NOR EQUIVALENT TO;Sm;0;ON;2273 0338;;;;Y;NEITHER GREATER THAN NOR EQUIVALENT TO;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2278'), U"\x2276\x0338") << desc_decomp_of(U'\x2278') << "\n  Line: 2278;NEITHER LESS-THAN NOR GREATER-THAN;Sm;0;ON;2276 0338;;;;Y;NEITHER LESS THAN NOR GREATER THAN;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2279'), U"\x2277\x0338") << desc_decomp_of(U'\x2279') << "\n  Line: 2279;NEITHER GREATER-THAN NOR LESS-THAN;Sm;0;ON;2277 0338;;;;Y;NEITHER GREATER THAN NOR LESS THAN;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2280'), U"\x227A\x0338") << desc_decomp_of(U'\x2280') << "\n  Line: 2280;DOES NOT PRECEDE;Sm;0;ON;227A 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2281'), U"\x227B\x0338") << desc_decomp_of(U'\x2281') << "\n  Line: 2281;DOES NOT SUCCEED;Sm;0;ON;227B 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2284'), U"\x2282\x0338") << desc_decomp_of(U'\x2284') << "\n  Line: 2284;NOT A SUBSET OF;Sm;0;ON;2282 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2285'), U"\x2283\x0338") << desc_decomp_of(U'\x2285') << "\n  Line: 2285;NOT A SUPERSET OF;Sm;0;ON;2283 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2288'), U"\x2286\x0338") << desc_decomp_of(U'\x2288') << "\n  Line: 2288;NEITHER A SUBSET OF NOR EQUAL TO;Sm;0;ON;2286 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2289'), U"\x2287\x0338") << desc_decomp_of(U'\x2289') << "\n  Line: 2289;NEITHER A SUPERSET OF NOR EQUAL TO;Sm;0;ON;2287 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22AC'), U"\x22A2\x0338") << desc_decomp_of(U'\x22AC') << "\n  Line: 22AC;DOES NOT PROVE;Sm;0;ON;22A2 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22AD'), U"\x22A8\x0338") << desc_decomp_of(U'\x22AD') << "\n  Line: 22AD;NOT TRUE;Sm;0;ON;22A8 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22AE'), U"\x22A9\x0338") << desc_decomp_of(U'\x22AE') << "\n  Line: 22AE;DOES NOT FORCE;Sm;0;ON;22A9 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22AF'), U"\x22AB\x0338") << desc_decomp_of(U'\x22AF') << "\n  Line: 22AF;NEGATED DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE;Sm;0;ON;22AB 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22E0'), U"\x227C\x0338") << desc_decomp_of(U'\x22E0') << "\n  Line: 22E0;DOES NOT PRECEDE OR EQUAL;Sm;0;ON;227C 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22E1'), U"\x227D\x0338") << desc_decomp_of(U'\x22E1') << "\n  Line: 22E1;DOES NOT SUCCEED OR EQUAL;Sm;0;ON;227D 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22E2'), U"\x2291\x0338") << desc_decomp_of(U'\x22E2') << "\n  Line: 22E2;NOT SQUARE IMAGE OF OR EQUAL TO;Sm;0;ON;2291 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22E3'), U"\x2292\x0338") << desc_decomp_of(U'\x22E3') << "\n  Line: 22E3;NOT SQUARE ORIGINAL OF OR EQUAL TO;Sm;0;ON;2292 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22EA'), U"\x22B2\x0338") << desc_decomp_of(U'\x22EA') << "\n  Line: 22EA;NOT NORMAL SUBGROUP OF;Sm;0;ON;22B2 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22EB'), U"\x22B3\x0338") << desc_decomp_of(U'\x22EB') << "\n  Line: 22EB;DOES NOT CONTAIN AS NORMAL SUBGROUP;Sm;0;ON;22B3 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22EC'), U"\x22B4\x0338") << desc_decomp_of(U'\x22EC') << "\n  Line: 22EC;NOT NORMAL SUBGROUP OF OR EQUAL TO;Sm;0;ON;22B4 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x22ED'), U"\x22B5\x0338") << desc_decomp_of(U'\x22ED') << "\n  Line: 22ED;DOES NOT CONTAIN AS NORMAL SUBGROUP OR EQUAL;Sm;0;ON;22B5 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2329'), U"\x3008") << desc_decomp_of(U'\x2329') << "\n  Line: 2329;LEFT-POINTING ANGLE BRACKET;Ps;0;ON;3008;;;;Y;BRA;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x232A'), U"\x3009") << desc_decomp_of(U'\x232A') << "\n  Line: 232A;RIGHT-POINTING ANGLE BRACKET;Pe;0;ON;3009;;;;Y;KET;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2ADC'), U"\x2ADD\x0338") << desc_decomp_of(U'\x2ADC') << "\n  Line: 2ADC;FORKING;Sm;0;ON;2ADD 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x304C'), U"\x304B\x3099") << desc_decomp_of(U'\x304C') << "\n  Line: 304C;HIRAGANA LETTER GA;Lo;0;L;304B 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x304E'), U"\x304D\x3099") << desc_decomp_of(U'\x304E') << "\n  Line: 304E;HIRAGANA LETTER GI;Lo;0;L;304D 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3050'), U"\x304F\x3099") << desc_decomp_of(U'\x3050') << "\n  Line: 3050;HIRAGANA LETTER GU;Lo;0;L;304F 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3052'), U"\x3051\x3099") << desc_decomp_of(U'\x3052') << "\n  Line: 3052;HIRAGANA LETTER GE;Lo;0;L;3051 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3054'), U"\x3053\x3099") << desc_decomp_of(U'\x3054') << "\n  Line: 3054;HIRAGANA LETTER GO;Lo;0;L;3053 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3056'), U"\x3055\x3099") << desc_decomp_of(U'\x3056') << "\n  Line: 3056;HIRAGANA LETTER ZA;Lo;0;L;3055 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3058'), U"\x3057\x3099") << desc_decomp_of(U'\x3058') << "\n  Line: 3058;HIRAGANA LETTER ZI;Lo;0;L;3057 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x305A'), U"\x3059\x3099") << desc_decomp_of(U'\x305A') << "\n  Line: 305A;HIRAGANA LETTER ZU;Lo;0;L;3059 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x305C'), U"\x305B\x3099") << desc_decomp_of(U'\x305C') << "\n  Line: 305C;HIRAGANA LETTER ZE;Lo;0;L;305B 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x305E'), U"\x305D\x3099") << desc_decomp_of(U'\x305E') << "\n  Line: 305E;HIRAGANA LETTER ZO;Lo;0;L;305D 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3060'), U"\x305F\x3099") << desc_decomp_of(U'\x3060') << "\n  Line: 3060;HIRAGANA LETTER DA;Lo;0;L;305F 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3062'), U"\x3061\x3099") << desc_decomp_of(U'\x3062') << "\n  Line: 3062;HIRAGANA LETTER DI;Lo;0;L;3061 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3065'), U"\x3064\x3099") << desc_decomp_of(U'\x3065') << "\n  Line: 3065;HIRAGANA LETTER DU;Lo;0;L;3064 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3067'), U"\x3066\x3099") << desc_decomp_of(U'\x3067') << "\n  Line: 3067;HIRAGANA LETTER DE;Lo;0;L;3066 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3069'), U"\x3068\x3099") << desc_decomp_of(U'\x3069') << "\n  Line: 3069;HIRAGANA LETTER DO;Lo;0;L;3068 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3070'), U"\x306F\x3099") << desc_decomp_of(U'\x3070') << "\n  Line: 3070;HIRAGANA LETTER BA;Lo;0;L;306F 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3071'), U"\x306F\x309A") << desc_decomp_of(U'\x3071') << "\n  Line: 3071;HIRAGANA LETTER PA;Lo;0;L;306F 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3073'), U"\x3072\x3099") << desc_decomp_of(U'\x3073') << "\n  Line: 3073;HIRAGANA LETTER BI;Lo;0;L;3072 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3074'), U"\x3072\x309A") << desc_decomp_of(U'\x3074') << "\n  Line: 3074;HIRAGANA LETTER PI;Lo;0;L;3072 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3076'), U"\x3075\x3099") << desc_decomp_of(U'\x3076') << "\n  Line: 3076;HIRAGANA LETTER BU;Lo;0;L;3075 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3077'), U"\x3075\x309A") << desc_decomp_of(U'\x3077') << "\n  Line: 3077;HIRAGANA LETTER PU;Lo;0;L;3075 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3079'), U"\x3078\x3099") << desc_decomp_of(U'\x3079') << "\n  Line: 3079;HIRAGANA LETTER BE;Lo;0;L;3078 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x307A'), U"\x3078\x309A") << desc_decomp_of(U'\x307A') << "\n  Line: 307A;HIRAGANA LETTER PE;Lo;0;L;3078 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x307C'), U"\x307B\x3099") << desc_decomp_of(U'\x307C') << "\n  Line: 307C;HIRAGANA LETTER BO;Lo;0;L;307B 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x307D'), U"\x307B\x309A") << desc_decomp_of(U'\x307D') << "\n  Line: 307D;HIRAGANA LETTER PO;Lo;0;L;307B 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x3094'), U"\x3046\x3099") << desc_decomp_of(U'\x3094') << "\n  Line: 3094;HIRAGANA LETTER VU;Lo;0;L;3046 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x309E'), U"\x309D\x3099") << desc_decomp_of(U'\x309E') << "\n  Line: 309E;HIRAGANA VOICED ITERATION MARK;Lm;0;L;309D 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30AC'), U"\x30AB\x3099") << desc_decomp_of(U'\x30AC') << "\n  Line: 30AC;KATAKANA LETTER GA;Lo;0;L;30AB 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30AE'), U"\x30AD\x3099") << desc_decomp_of(U'\x30AE') << "\n  Line: 30AE;KATAKANA LETTER GI;Lo;0;L;30AD 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30B0'), U"\x30AF\x3099") << desc_decomp_of(U'\x30B0') << "\n  Line: 30B0;KATAKANA LETTER GU;Lo;0;L;30AF 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30B2'), U"\x30B1\x3099") << desc_decomp_of(U'\x30B2') << "\n  Line: 30B2;KATAKANA LETTER GE;Lo;0;L;30B1 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30B4'), U"\x30B3\x3099") << desc_decomp_of(U'\x30B4') << "\n  Line: 30B4;KATAKANA LETTER GO;Lo;0;L;30B3 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30B6'), U"\x30B5\x3099") << desc_decomp_of(U'\x30B6') << "\n  Line: 30B6;KATAKANA LETTER ZA;Lo;0;L;30B5 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30B8'), U"\x30B7\x3099") << desc_decomp_of(U'\x30B8') << "\n  Line: 30B8;KATAKANA LETTER ZI;Lo;0;L;30B7 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30BA'), U"\x30B9\x3099") << desc_decomp_of(U'\x30BA') << "\n  Line: 30BA;KATAKANA LETTER ZU;Lo;0;L;30B9 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30BC'), U"\x30BB\x3099") << desc_decomp_of(U'\x30BC') << "\n  Line: 30BC;KATAKANA LETTER ZE;Lo;0;L;30BB 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30BE'), U"\x30BD\x3099") << desc_decomp_of(U'\x30BE') << "\n  Line: 30BE;KATAKANA LETTER ZO;Lo;0;L;30BD 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30C0'), U"\x30BF\x3099") << desc_decomp_of(U'\x30C0') << "\n  Line: 30C0;KATAKANA LETTER DA;Lo;0;L;30BF 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30C2'), U"\x30C1\x3099") << desc_decomp_of(U'\x30C2') << "\n  Line: 30C2;KATAKANA LETTER DI;Lo;0;L;30C1 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30C5'), U"\x30C4\x3099") << desc_decomp_of(U'\x30C5') << "\n  Line: 30C5;KATAKANA LETTER DU;Lo;0;L;30C4 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30C7'), U"\x30C6\x3099") << desc_decomp_of(U'\x30C7') << "\n  Line: 30C7;KATAKANA LETTER DE;Lo;0;L;30C6 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30C9'), U"\x30C8\x3099") << desc_decomp_of(U'\x30C9') << "\n  Line: 30C9;KATAKANA LETTER DO;Lo;0;L;30C8 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30D0'), U"\x30CF\x3099") << desc_decomp_of(U'\x30D0') << "\n  Line: 30D0;KATAKANA LETTER BA;Lo;0;L;30CF 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30D1'), U"\x30CF\x309A") << desc_decomp_of(U'\x30D1') << "\n  Line: 30D1;KATAKANA LETTER PA;Lo;0;L;30CF 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30D3'), U"\x30D2\x3099") << desc_decomp_of(U'\x30D3') << "\n  Line: 30D3;KATAKANA LETTER BI;Lo;0;L;30D2 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30D4'), U"\x30D2\x309A") << desc_decomp_of(U'\x30D4') << "\n  Line: 30D4;KATAKANA LETTER PI;Lo;0;L;30D2 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30D6'), U"\x30D5\x3099") << desc_decomp_of(U'\x30D6') << "\n  Line: 30D6;KATAKANA LETTER BU;Lo;0;L;30D5 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30D7'), U"\x30D5\x309A") << desc_decomp_of(U'\x30D7') << "\n  Line: 30D7;KATAKANA LETTER PU;Lo;0;L;30D5 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30D9'), U"\x30D8\x3099") << desc_decomp_of(U'\x30D9') << "\n  Line: 30D9;KATAKANA LETTER BE;Lo;0;L;30D8 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30DA'), U"\x30D8\x309A") << desc_decomp_of(U'\x30DA') << "\n  Line: 30DA;KATAKANA LETTER PE;Lo;0;L;30D8 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30DC'), U"\x30DB\x3099") << desc_decomp_of(U'\x30DC') << "\n  Line: 30DC;KATAKANA LETTER BO;Lo;0;L;30DB 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30DD'), U"\x30DB\x309A") << desc_decomp_of(U'\x30DD') << "\n  Line: 30DD;KATAKANA LETTER PO;Lo;0;L;30DB 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30F4'), U"\x30A6\x3099") << desc_decomp_of(U'\x30F4') << "\n  Line: 30F4;KATAKANA LETTER VU;Lo;0;L;30A6 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30F7'), U"\x30EF\x3099") << desc_decomp_of(U'\x30F7') << "\n  Line: 30F7;KATAKANA LETTER VA;Lo;0;L;30EF 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30F8'), U"\x30F0\x3099") << desc_decomp_of(U'\x30F8') << "\n  Line: 30F8;KATAKANA LETTER VI;Lo;0;L;30F0 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30F9'), U"\x30F1\x3099") << desc_decomp_of(U'\x30F9') << "\n  Line: 30F9;KATAKANA LETTER VE;Lo;0;L;30F1 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30FA'), U"\x30F2\x3099") << desc_decomp_of(U'\x30FA') << "\n  Line: 30FA;KATAKANA LETTER VO;Lo;0;L;30F2 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x30FE'), U"\x30FD\x3099") << desc_decomp_of(U'\x30FE') << "\n  Line: 30FE;KATAKANA VOICED ITERATION MARK;Lm;0;L;30FD 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF900'), U"\x8C48") << desc_decomp_of(U'\xF900') << "\n  Line: F900;CJK COMPATIBILITY IDEOGRAPH-F900;Lo;0;L;8C48;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF901'), U"\x66F4") << desc_decomp_of(U'\xF901') << "\n  Line: F901;CJK COMPATIBILITY IDEOGRAPH-F901;Lo;0;L;66F4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF902'), U"\x8ECA") << desc_decomp_of(U'\xF902') << "\n  Line: F902;CJK COMPATIBILITY IDEOGRAPH-F902;Lo;0;L;8ECA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF903'), U"\x8CC8") << desc_decomp_of(U'\xF903') << "\n  Line: F903;CJK COMPATIBILITY IDEOGRAPH-F903;Lo;0;L;8CC8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF904'), U"\x6ED1") << desc_decomp_of(U'\xF904') << "\n  Line: F904;CJK COMPATIBILITY IDEOGRAPH-F904;Lo;0;L;6ED1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF905'), U"\x4E32") << desc_decomp_of(U'\xF905') << "\n  Line: F905;CJK COMPATIBILITY IDEOGRAPH-F905;Lo;0;L;4E32;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF906'), U"\x53E5") << desc_decomp_of(U'\xF906') << "\n  Line: F906;CJK COMPATIBILITY IDEOGRAPH-F906;Lo;0;L;53E5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF907'), U"\x9F9C") << desc_decomp_of(U'\xF907') << "\n  Line: F907;CJK COMPATIBILITY IDEOGRAPH-F907;Lo;0;L;9F9C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF908'), U"\x9F9C") << desc_decomp_of(U'\xF908') << "\n  Line: F908;CJK COMPATIBILITY IDEOGRAPH-F908;Lo;0;L;9F9C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF909'), U"\x5951") << desc_decomp_of(U'\xF909') << "\n  Line: F909;CJK COMPATIBILITY IDEOGRAPH-F909;Lo;0;L;5951;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF90A'), U"\x91D1") << desc_decomp_of(U'\xF90A') << "\n  Line: F90A;CJK COMPATIBILITY IDEOGRAPH-F90A;Lo;0;L;91D1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF90B'), U"\x5587") << desc_decomp_of(U'\xF90B') << "\n  Line: F90B;CJK COMPATIBILITY IDEOGRAPH-F90B;Lo;0;L;5587;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF90C'), U"\x5948") << desc_decomp_of(U'\xF90C') << "\n  Line: F90C;CJK COMPATIBILITY IDEOGRAPH-F90C;Lo;0;L;5948;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF90D'), U"\x61F6") << desc_decomp_of(U'\xF90D') << "\n  Line: F90D;CJK COMPATIBILITY IDEOGRAPH-F90D;Lo;0;L;61F6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF90E'), U"\x7669") << desc_decomp_of(U'\xF90E') << "\n  Line: F90E;CJK COMPATIBILITY IDEOGRAPH-F90E;Lo;0;L;7669;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF90F'), U"\x7F85") << desc_decomp_of(U'\xF90F') << "\n  Line: F90F;CJK COMPATIBILITY IDEOGRAPH-F90F;Lo;0;L;7F85;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF910'), U"\x863F") << desc_decomp_of(U'\xF910') << "\n  Line: F910;CJK COMPATIBILITY IDEOGRAPH-F910;Lo;0;L;863F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF911'), U"\x87BA") << desc_decomp_of(U'\xF911') << "\n  Line: F911;CJK COMPATIBILITY IDEOGRAPH-F911;Lo;0;L;87BA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF912'), U"\x88F8") << desc_decomp_of(U'\xF912') << "\n  Line: F912;CJK COMPATIBILITY IDEOGRAPH-F912;Lo;0;L;88F8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF913'), U"\x908F") << desc_decomp_of(U'\xF913') << "\n  Line: F913;CJK COMPATIBILITY IDEOGRAPH-F913;Lo;0;L;908F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF914'), U"\x6A02") << desc_decomp_of(U'\xF914') << "\n  Line: F914;CJK COMPATIBILITY IDEOGRAPH-F914;Lo;0;L;6A02;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF915'), U"\x6D1B") << desc_decomp_of(U'\xF915') << "\n  Line: F915;CJK COMPATIBILITY IDEOGRAPH-F915;Lo;0;L;6D1B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF916'), U"\x70D9") << desc_decomp_of(U'\xF916') << "\n  Line: F916;CJK COMPATIBILITY IDEOGRAPH-F916;Lo;0;L;70D9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF917'), U"\x73DE") << desc_decomp_of(U'\xF917') << "\n  Line: F917;CJK COMPATIBILITY IDEOGRAPH-F917;Lo;0;L;73DE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF918'), U"\x843D") << desc_decomp_of(U'\xF918') << "\n  Line: F918;CJK COMPATIBILITY IDEOGRAPH-F918;Lo;0;L;843D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF919'), U"\x916A") << desc_decomp_of(U'\xF919') << "\n  Line: F919;CJK COMPATIBILITY IDEOGRAPH-F919;Lo;0;L;916A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF91A'), U"\x99F1") << desc_decomp_of(U'\xF91A') << "\n  Line: F91A;CJK COMPATIBILITY IDEOGRAPH-F91A;Lo;0;L;99F1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF91B'), U"\x4E82") << desc_decomp_of(U'\xF91B') << "\n  Line: F91B;CJK COMPATIBILITY IDEOGRAPH-F91B;Lo;0;L;4E82;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF91C'), U"\x5375") << desc_decomp_of(U'\xF91C') << "\n  Line: F91C;CJK COMPATIBILITY IDEOGRAPH-F91C;Lo;0;L;5375;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF91D'), U"\x6B04") << desc_decomp_of(U'\xF91D') << "\n  Line: F91D;CJK COMPATIBILITY IDEOGRAPH-F91D;Lo;0;L;6B04;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF91E'), U"\x721B") << desc_decomp_of(U'\xF91E') << "\n  Line: F91E;CJK COMPATIBILITY IDEOGRAPH-F91E;Lo;0;L;721B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF91F'), U"\x862D") << desc_decomp_of(U'\xF91F') << "\n  Line: F91F;CJK COMPATIBILITY IDEOGRAPH-F91F;Lo;0;L;862D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF920'), U"\x9E1E") << desc_decomp_of(U'\xF920') << "\n  Line: F920;CJK COMPATIBILITY IDEOGRAPH-F920;Lo;0;L;9E1E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF921'), U"\x5D50") << desc_decomp_of(U'\xF921') << "\n  Line: F921;CJK COMPATIBILITY IDEOGRAPH-F921;Lo;0;L;5D50;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF922'), U"\x6FEB") << desc_decomp_of(U'\xF922') << "\n  Line: F922;CJK COMPATIBILITY IDEOGRAPH-F922;Lo;0;L;6FEB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF923'), U"\x85CD") << desc_decomp_of(U'\xF923') << "\n  Line: F923;CJK COMPATIBILITY IDEOGRAPH-F923;Lo;0;L;85CD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF924'), U"\x8964") << desc_decomp_of(U'\xF924') << "\n  Line: F924;CJK COMPATIBILITY IDEOGRAPH-F924;Lo;0;L;8964;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF925'), U"\x62C9") << desc_decomp_of(U'\xF925') << "\n  Line: F925;CJK COMPATIBILITY IDEOGRAPH-F925;Lo;0;L;62C9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF926'), U"\x81D8") << desc_decomp_of(U'\xF926') << "\n  Line: F926;CJK COMPATIBILITY IDEOGRAPH-F926;Lo;0;L;81D8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF927'), U"\x881F") << desc_decomp_of(U'\xF927') << "\n  Line: F927;CJK COMPATIBILITY IDEOGRAPH-F927;Lo;0;L;881F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF928'), U"\x5ECA") << desc_decomp_of(U'\xF928') << "\n  Line: F928;CJK COMPATIBILITY IDEOGRAPH-F928;Lo;0;L;5ECA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF929'), U"\x6717") << desc_decomp_of(U'\xF929') << "\n  Line: F929;CJK COMPATIBILITY IDEOGRAPH-F929;Lo;0;L;6717;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF92A'), U"\x6D6A") << desc_decomp_of(U'\xF92A') << "\n  Line: F92A;CJK COMPATIBILITY IDEOGRAPH-F92A;Lo;0;L;6D6A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF92B'), U"\x72FC") << desc_decomp_of(U'\xF92B') << "\n  Line: F92B;CJK COMPATIBILITY IDEOGRAPH-F92B;Lo;0;L;72FC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF92C'), U"\x90CE") << desc_decomp_of(U'\xF92C') << "\n  Line: F92C;CJK COMPATIBILITY IDEOGRAPH-F92C;Lo;0;L;90CE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF92D'), U"\x4F86") << desc_decomp_of(U'\xF92D') << "\n  Line: F92D;CJK COMPATIBILITY IDEOGRAPH-F92D;Lo;0;L;4F86;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF92E'), U"\x51B7") << desc_decomp_of(U'\xF92E') << "\n  Line: F92E;CJK COMPATIBILITY IDEOGRAPH-F92E;Lo;0;L;51B7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF92F'), U"\x52DE") << desc_decomp_of(U'\xF92F') << "\n  Line: F92F;CJK COMPATIBILITY IDEOGRAPH-F92F;Lo;0;L;52DE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF930'), U"\x64C4") << desc_decomp_of(U'\xF930') << "\n  Line: F930;CJK COMPATIBILITY IDEOGRAPH-F930;Lo;0;L;64C4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF931'), U"\x6AD3") << desc_decomp_of(U'\xF931') << "\n  Line: F931;CJK COMPATIBILITY IDEOGRAPH-F931;Lo;0;L;6AD3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF932'), U"\x7210") << desc_decomp_of(U'\xF932') << "\n  Line: F932;CJK COMPATIBILITY IDEOGRAPH-F932;Lo;0;L;7210;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF933'), U"\x76E7") << desc_decomp_of(U'\xF933') << "\n  Line: F933;CJK COMPATIBILITY IDEOGRAPH-F933;Lo;0;L;76E7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF934'), U"\x8001") << desc_decomp_of(U'\xF934') << "\n  Line: F934;CJK COMPATIBILITY IDEOGRAPH-F934;Lo;0;L;8001;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF935'), U"\x8606") << desc_decomp_of(U'\xF935') << "\n  Line: F935;CJK COMPATIBILITY IDEOGRAPH-F935;Lo;0;L;8606;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF936'), U"\x865C") << desc_decomp_of(U'\xF936') << "\n  Line: F936;CJK COMPATIBILITY IDEOGRAPH-F936;Lo;0;L;865C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF937'), U"\x8DEF") << desc_decomp_of(U'\xF937') << "\n  Line: F937;CJK COMPATIBILITY IDEOGRAPH-F937;Lo;0;L;8DEF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF938'), U"\x9732") << desc_decomp_of(U'\xF938') << "\n  Line: F938;CJK COMPATIBILITY IDEOGRAPH-F938;Lo;0;L;9732;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF939'), U"\x9B6F") << desc_decomp_of(U'\xF939') << "\n  Line: F939;CJK COMPATIBILITY IDEOGRAPH-F939;Lo;0;L;9B6F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF93A'), U"\x9DFA") << desc_decomp_of(U'\xF93A') << "\n  Line: F93A;CJK COMPATIBILITY IDEOGRAPH-F93A;Lo;0;L;9DFA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF93B'), U"\x788C") << desc_decomp_of(U'\xF93B') << "\n  Line: F93B;CJK COMPATIBILITY IDEOGRAPH-F93B;Lo;0;L;788C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF93C'), U"\x797F") << desc_decomp_of(U'\xF93C') << "\n  Line: F93C;CJK COMPATIBILITY IDEOGRAPH-F93C;Lo;0;L;797F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF93D'), U"\x7DA0") << desc_decomp_of(U'\xF93D') << "\n  Line: F93D;CJK COMPATIBILITY IDEOGRAPH-F93D;Lo;0;L;7DA0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF93E'), U"\x83C9") << desc_decomp_of(U'\xF93E') << "\n  Line: F93E;CJK COMPATIBILITY IDEOGRAPH-F93E;Lo;0;L;83C9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF93F'), U"\x9304") << desc_decomp_of(U'\xF93F') << "\n  Line: F93F;CJK COMPATIBILITY IDEOGRAPH-F93F;Lo;0;L;9304;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF940'), U"\x9E7F") << desc_decomp_of(U'\xF940') << "\n  Line: F940;CJK COMPATIBILITY IDEOGRAPH-F940;Lo;0;L;9E7F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF941'), U"\x8AD6") << desc_decomp_of(U'\xF941') << "\n  Line: F941;CJK COMPATIBILITY IDEOGRAPH-F941;Lo;0;L;8AD6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF942'), U"\x58DF") << desc_decomp_of(U'\xF942') << "\n  Line: F942;CJK COMPATIBILITY IDEOGRAPH-F942;Lo;0;L;58DF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF943'), U"\x5F04") << desc_decomp_of(U'\xF943') << "\n  Line: F943;CJK COMPATIBILITY IDEOGRAPH-F943;Lo;0;L;5F04;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF944'), U"\x7C60") << desc_decomp_of(U'\xF944') << "\n  Line: F944;CJK COMPATIBILITY IDEOGRAPH-F944;Lo;0;L;7C60;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF945'), U"\x807E") << desc_decomp_of(U'\xF945') << "\n  Line: F945;CJK COMPATIBILITY IDEOGRAPH-F945;Lo;0;L;807E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF946'), U"\x7262") << desc_decomp_of(U'\xF946') << "\n  Line: F946;CJK COMPATIBILITY IDEOGRAPH-F946;Lo;0;L;7262;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF947'), U"\x78CA") << desc_decomp_of(U'\xF947') << "\n  Line: F947;CJK COMPATIBILITY IDEOGRAPH-F947;Lo;0;L;78CA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF948'), U"\x8CC2") << desc_decomp_of(U'\xF948') << "\n  Line: F948;CJK COMPATIBILITY IDEOGRAPH-F948;Lo;0;L;8CC2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF949'), U"\x96F7") << desc_decomp_of(U'\xF949') << "\n  Line: F949;CJK COMPATIBILITY IDEOGRAPH-F949;Lo;0;L;96F7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF94A'), U"\x58D8") << desc_decomp_of(U'\xF94A') << "\n  Line: F94A;CJK COMPATIBILITY IDEOGRAPH-F94A;Lo;0;L;58D8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF94B'), U"\x5C62") << desc_decomp_of(U'\xF94B') << "\n  Line: F94B;CJK COMPATIBILITY IDEOGRAPH-F94B;Lo;0;L;5C62;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF94C'), U"\x6A13") << desc_decomp_of(U'\xF94C') << "\n  Line: F94C;CJK COMPATIBILITY IDEOGRAPH-F94C;Lo;0;L;6A13;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF94D'), U"\x6DDA") << desc_decomp_of(U'\xF94D') << "\n  Line: F94D;CJK COMPATIBILITY IDEOGRAPH-F94D;Lo;0;L;6DDA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF94E'), U"\x6F0F") << desc_decomp_of(U'\xF94E') << "\n  Line: F94E;CJK COMPATIBILITY IDEOGRAPH-F94E;Lo;0;L;6F0F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF94F'), U"\x7D2F") << desc_decomp_of(U'\xF94F') << "\n  Line: F94F;CJK COMPATIBILITY IDEOGRAPH-F94F;Lo;0;L;7D2F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF950'), U"\x7E37") << desc_decomp_of(U'\xF950') << "\n  Line: F950;CJK COMPATIBILITY IDEOGRAPH-F950;Lo;0;L;7E37;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF951'), U"\x964B") << desc_decomp_of(U'\xF951') << "\n  Line: F951;CJK COMPATIBILITY IDEOGRAPH-F951;Lo;0;L;964B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF952'), U"\x52D2") << desc_decomp_of(U'\xF952') << "\n  Line: F952;CJK COMPATIBILITY IDEOGRAPH-F952;Lo;0;L;52D2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF953'), U"\x808B") << desc_decomp_of(U'\xF953') << "\n  Line: F953;CJK COMPATIBILITY IDEOGRAPH-F953;Lo;0;L;808B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF954'), U"\x51DC") << desc_decomp_of(U'\xF954') << "\n  Line: F954;CJK COMPATIBILITY IDEOGRAPH-F954;Lo;0;L;51DC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF955'), U"\x51CC") << desc_decomp_of(U'\xF955') << "\n  Line: F955;CJK COMPATIBILITY IDEOGRAPH-F955;Lo;0;L;51CC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF956'), U"\x7A1C") << desc_decomp_of(U'\xF956') << "\n  Line: F956;CJK COMPATIBILITY IDEOGRAPH-F956;Lo;0;L;7A1C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF957'), U"\x7DBE") << desc_decomp_of(U'\xF957') << "\n  Line: F957;CJK COMPATIBILITY IDEOGRAPH-F957;Lo;0;L;7DBE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF958'), U"\x83F1") << desc_decomp_of(U'\xF958') << "\n  Line: F958;CJK COMPATIBILITY IDEOGRAPH-F958;Lo;0;L;83F1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF959'), U"\x9675") << desc_decomp_of(U'\xF959') << "\n  Line: F959;CJK COMPATIBILITY IDEOGRAPH-F959;Lo;0;L;9675;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF95A'), U"\x8B80") << desc_decomp_of(U'\xF95A') << "\n  Line: F95A;CJK COMPATIBILITY IDEOGRAPH-F95A;Lo;0;L;8B80;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF95B'), U"\x62CF") << desc_decomp_of(U'\xF95B') << "\n  Line: F95B;CJK COMPATIBILITY IDEOGRAPH-F95B;Lo;0;L;62CF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF95C'), U"\x6A02") << desc_decomp_of(U'\xF95C') << "\n  Line: F95C;CJK COMPATIBILITY IDEOGRAPH-F95C;Lo;0;L;6A02;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF95D'), U"\x8AFE") << desc_decomp_of(U'\xF95D') << "\n  Line: F95D;CJK COMPATIBILITY IDEOGRAPH-F95D;Lo;0;L;8AFE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF95E'), U"\x4E39") << desc_decomp_of(U'\xF95E') << "\n  Line: F95E;CJK COMPATIBILITY IDEOGRAPH-F95E;Lo;0;L;4E39;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF95F'), U"\x5BE7") << desc_decomp_of(U'\xF95F') << "\n  Line: F95F;CJK COMPATIBILITY IDEOGRAPH-F95F;Lo;0;L;5BE7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF960'), U"\x6012") << desc_decomp_of(U'\xF960') << "\n  Line: F960;CJK COMPATIBILITY IDEOGRAPH-F960;Lo;0;L;6012;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF961'), U"\x7387") << desc_decomp_of(U'\xF961') << "\n  Line: F961;CJK COMPATIBILITY IDEOGRAPH-F961;Lo;0;L;7387;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF962'), U"\x7570") << desc_decomp_of(U'\xF962') << "\n  Line: F962;CJK COMPATIBILITY IDEOGRAPH-F962;Lo;0;L;7570;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF963'), U"\x5317") << desc_decomp_of(U'\xF963') << "\n  Line: F963;CJK COMPATIBILITY IDEOGRAPH-F963;Lo;0;L;5317;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF964'), U"\x78FB") << desc_decomp_of(U'\xF964') << "\n  Line: F964;CJK COMPATIBILITY IDEOGRAPH-F964;Lo;0;L;78FB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF965'), U"\x4FBF") << desc_decomp_of(U'\xF965') << "\n  Line: F965;CJK COMPATIBILITY IDEOGRAPH-F965;Lo;0;L;4FBF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF966'), U"\x5FA9") << desc_decomp_of(U'\xF966') << "\n  Line: F966;CJK COMPATIBILITY IDEOGRAPH-F966;Lo;0;L;5FA9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF967'), U"\x4E0D") << desc_decomp_of(U'\xF967') << "\n  Line: F967;CJK COMPATIBILITY IDEOGRAPH-F967;Lo;0;L;4E0D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF968'), U"\x6CCC") << desc_decomp_of(U'\xF968') << "\n  Line: F968;CJK COMPATIBILITY IDEOGRAPH-F968;Lo;0;L;6CCC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF969'), U"\x6578") << desc_decomp_of(U'\xF969') << "\n  Line: F969;CJK COMPATIBILITY IDEOGRAPH-F969;Lo;0;L;6578;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF96A'), U"\x7D22") << desc_decomp_of(U'\xF96A') << "\n  Line: F96A;CJK COMPATIBILITY IDEOGRAPH-F96A;Lo;0;L;7D22;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF96B'), U"\x53C3") << desc_decomp_of(U'\xF96B') << "\n  Line: F96B;CJK COMPATIBILITY IDEOGRAPH-F96B;Lo;0;L;53C3;;;3;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF96C'), U"\x585E") << desc_decomp_of(U'\xF96C') << "\n  Line: F96C;CJK COMPATIBILITY IDEOGRAPH-F96C;Lo;0;L;585E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF96D'), U"\x7701") << desc_decomp_of(U'\xF96D') << "\n  Line: F96D;CJK COMPATIBILITY IDEOGRAPH-F96D;Lo;0;L;7701;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF96E'), U"\x8449") << desc_decomp_of(U'\xF96E') << "\n  Line: F96E;CJK COMPATIBILITY IDEOGRAPH-F96E;Lo;0;L;8449;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF96F'), U"\x8AAA") << desc_decomp_of(U'\xF96F') << "\n  Line: F96F;CJK COMPATIBILITY IDEOGRAPH-F96F;Lo;0;L;8AAA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF970'), U"\x6BBA") << desc_decomp_of(U'\xF970') << "\n  Line: F970;CJK COMPATIBILITY IDEOGRAPH-F970;Lo;0;L;6BBA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF971'), U"\x8FB0") << desc_decomp_of(U'\xF971') << "\n  Line: F971;CJK COMPATIBILITY IDEOGRAPH-F971;Lo;0;L;8FB0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF972'), U"\x6C88") << desc_decomp_of(U'\xF972') << "\n  Line: F972;CJK COMPATIBILITY IDEOGRAPH-F972;Lo;0;L;6C88;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF973'), U"\x62FE") << desc_decomp_of(U'\xF973') << "\n  Line: F973;CJK COMPATIBILITY IDEOGRAPH-F973;Lo;0;L;62FE;;;10;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF974'), U"\x82E5") << desc_decomp_of(U'\xF974') << "\n  Line: F974;CJK COMPATIBILITY IDEOGRAPH-F974;Lo;0;L;82E5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF975'), U"\x63A0") << desc_decomp_of(U'\xF975') << "\n  Line: F975;CJK COMPATIBILITY IDEOGRAPH-F975;Lo;0;L;63A0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF976'), U"\x7565") << desc_decomp_of(U'\xF976') << "\n  Line: F976;CJK COMPATIBILITY IDEOGRAPH-F976;Lo;0;L;7565;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF977'), U"\x4EAE") << desc_decomp_of(U'\xF977') << "\n  Line: F977;CJK COMPATIBILITY IDEOGRAPH-F977;Lo;0;L;4EAE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF978'), U"\x5169") << desc_decomp_of(U'\xF978') << "\n  Line: F978;CJK COMPATIBILITY IDEOGRAPH-F978;Lo;0;L;5169;;;2;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF979'), U"\x51C9") << desc_decomp_of(U'\xF979') << "\n  Line: F979;CJK COMPATIBILITY IDEOGRAPH-F979;Lo;0;L;51C9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF97A'), U"\x6881") << desc_decomp_of(U'\xF97A') << "\n  Line: F97A;CJK COMPATIBILITY IDEOGRAPH-F97A;Lo;0;L;6881;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF97B'), U"\x7CE7") << desc_decomp_of(U'\xF97B') << "\n  Line: F97B;CJK COMPATIBILITY IDEOGRAPH-F97B;Lo;0;L;7CE7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF97C'), U"\x826F") << desc_decomp_of(U'\xF97C') << "\n  Line: F97C;CJK COMPATIBILITY IDEOGRAPH-F97C;Lo;0;L;826F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF97D'), U"\x8AD2") << desc_decomp_of(U'\xF97D') << "\n  Line: F97D;CJK COMPATIBILITY IDEOGRAPH-F97D;Lo;0;L;8AD2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF97E'), U"\x91CF") << desc_decomp_of(U'\xF97E') << "\n  Line: F97E;CJK COMPATIBILITY IDEOGRAPH-F97E;Lo;0;L;91CF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF97F'), U"\x52F5") << desc_decomp_of(U'\xF97F') << "\n  Line: F97F;CJK COMPATIBILITY IDEOGRAPH-F97F;Lo;0;L;52F5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF980'), U"\x5442") << desc_decomp_of(U'\xF980') << "\n  Line: F980;CJK COMPATIBILITY IDEOGRAPH-F980;Lo;0;L;5442;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF981'), U"\x5973") << desc_decomp_of(U'\xF981') << "\n  Line: F981;CJK COMPATIBILITY IDEOGRAPH-F981;Lo;0;L;5973;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF982'), U"\x5EEC") << desc_decomp_of(U'\xF982') << "\n  Line: F982;CJK COMPATIBILITY IDEOGRAPH-F982;Lo;0;L;5EEC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF983'), U"\x65C5") << desc_decomp_of(U'\xF983') << "\n  Line: F983;CJK COMPATIBILITY IDEOGRAPH-F983;Lo;0;L;65C5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF984'), U"\x6FFE") << desc_decomp_of(U'\xF984') << "\n  Line: F984;CJK COMPATIBILITY IDEOGRAPH-F984;Lo;0;L;6FFE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF985'), U"\x792A") << desc_decomp_of(U'\xF985') << "\n  Line: F985;CJK COMPATIBILITY IDEOGRAPH-F985;Lo;0;L;792A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF986'), U"\x95AD") << desc_decomp_of(U'\xF986') << "\n  Line: F986;CJK COMPATIBILITY IDEOGRAPH-F986;Lo;0;L;95AD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF987'), U"\x9A6A") << desc_decomp_of(U'\xF987') << "\n  Line: F987;CJK COMPATIBILITY IDEOGRAPH-F987;Lo;0;L;9A6A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF988'), U"\x9E97") << desc_decomp_of(U'\xF988') << "\n  Line: F988;CJK COMPATIBILITY IDEOGRAPH-F988;Lo;0;L;9E97;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF989'), U"\x9ECE") << desc_decomp_of(U'\xF989') << "\n  Line: F989;CJK COMPATIBILITY IDEOGRAPH-F989;Lo;0;L;9ECE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF98A'), U"\x529B") << desc_decomp_of(U'\xF98A') << "\n  Line: F98A;CJK COMPATIBILITY IDEOGRAPH-F98A;Lo;0;L;529B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF98B'), U"\x66C6") << desc_decomp_of(U'\xF98B') << "\n  Line: F98B;CJK COMPATIBILITY IDEOGRAPH-F98B;Lo;0;L;66C6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF98C'), U"\x6B77") << desc_decomp_of(U'\xF98C') << "\n  Line: F98C;CJK COMPATIBILITY IDEOGRAPH-F98C;Lo;0;L;6B77;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF98D'), U"\x8F62") << desc_decomp_of(U'\xF98D') << "\n  Line: F98D;CJK COMPATIBILITY IDEOGRAPH-F98D;Lo;0;L;8F62;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF98E'), U"\x5E74") << desc_decomp_of(U'\xF98E') << "\n  Line: F98E;CJK COMPATIBILITY IDEOGRAPH-F98E;Lo;0;L;5E74;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF98F'), U"\x6190") << desc_decomp_of(U'\xF98F') << "\n  Line: F98F;CJK COMPATIBILITY IDEOGRAPH-F98F;Lo;0;L;6190;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF990'), U"\x6200") << desc_decomp_of(U'\xF990') << "\n  Line: F990;CJK COMPATIBILITY IDEOGRAPH-F990;Lo;0;L;6200;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF991'), U"\x649A") << desc_decomp_of(U'\xF991') << "\n  Line: F991;CJK COMPATIBILITY IDEOGRAPH-F991;Lo;0;L;649A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF992'), U"\x6F23") << desc_decomp_of(U'\xF992') << "\n  Line: F992;CJK COMPATIBILITY IDEOGRAPH-F992;Lo;0;L;6F23;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF993'), U"\x7149") << desc_decomp_of(U'\xF993') << "\n  Line: F993;CJK COMPATIBILITY IDEOGRAPH-F993;Lo;0;L;7149;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF994'), U"\x7489") << desc_decomp_of(U'\xF994') << "\n  Line: F994;CJK COMPATIBILITY IDEOGRAPH-F994;Lo;0;L;7489;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF995'), U"\x79CA") << desc_decomp_of(U'\xF995') << "\n  Line: F995;CJK COMPATIBILITY IDEOGRAPH-F995;Lo;0;L;79CA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF996'), U"\x7DF4") << desc_decomp_of(U'\xF996') << "\n  Line: F996;CJK COMPATIBILITY IDEOGRAPH-F996;Lo;0;L;7DF4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF997'), U"\x806F") << desc_decomp_of(U'\xF997') << "\n  Line: F997;CJK COMPATIBILITY IDEOGRAPH-F997;Lo;0;L;806F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF998'), U"\x8F26") << desc_decomp_of(U'\xF998') << "\n  Line: F998;CJK COMPATIBILITY IDEOGRAPH-F998;Lo;0;L;8F26;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF999'), U"\x84EE") << desc_decomp_of(U'\xF999') << "\n  Line: F999;CJK COMPATIBILITY IDEOGRAPH-F999;Lo;0;L;84EE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF99A'), U"\x9023") << desc_decomp_of(U'\xF99A') << "\n  Line: F99A;CJK COMPATIBILITY IDEOGRAPH-F99A;Lo;0;L;9023;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF99B'), U"\x934A") << desc_decomp_of(U'\xF99B') << "\n  Line: F99B;CJK COMPATIBILITY IDEOGRAPH-F99B;Lo;0;L;934A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF99C'), U"\x5217") << desc_decomp_of(U'\xF99C') << "\n  Line: F99C;CJK COMPATIBILITY IDEOGRAPH-F99C;Lo;0;L;5217;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF99D'), U"\x52A3") << desc_decomp_of(U'\xF99D') << "\n  Line: F99D;CJK COMPATIBILITY IDEOGRAPH-F99D;Lo;0;L;52A3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF99E'), U"\x54BD") << desc_decomp_of(U'\xF99E') << "\n  Line: F99E;CJK COMPATIBILITY IDEOGRAPH-F99E;Lo;0;L;54BD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF99F'), U"\x70C8") << desc_decomp_of(U'\xF99F') << "\n  Line: F99F;CJK COMPATIBILITY IDEOGRAPH-F99F;Lo;0;L;70C8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A0'), U"\x88C2") << desc_decomp_of(U'\xF9A0') << "\n  Line: F9A0;CJK COMPATIBILITY IDEOGRAPH-F9A0;Lo;0;L;88C2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A1'), U"\x8AAA") << desc_decomp_of(U'\xF9A1') << "\n  Line: F9A1;CJK COMPATIBILITY IDEOGRAPH-F9A1;Lo;0;L;8AAA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A2'), U"\x5EC9") << desc_decomp_of(U'\xF9A2') << "\n  Line: F9A2;CJK COMPATIBILITY IDEOGRAPH-F9A2;Lo;0;L;5EC9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A3'), U"\x5FF5") << desc_decomp_of(U'\xF9A3') << "\n  Line: F9A3;CJK COMPATIBILITY IDEOGRAPH-F9A3;Lo;0;L;5FF5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A4'), U"\x637B") << desc_decomp_of(U'\xF9A4') << "\n  Line: F9A4;CJK COMPATIBILITY IDEOGRAPH-F9A4;Lo;0;L;637B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A5'), U"\x6BAE") << desc_decomp_of(U'\xF9A5') << "\n  Line: F9A5;CJK COMPATIBILITY IDEOGRAPH-F9A5;Lo;0;L;6BAE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A6'), U"\x7C3E") << desc_decomp_of(U'\xF9A6') << "\n  Line: F9A6;CJK COMPATIBILITY IDEOGRAPH-F9A6;Lo;0;L;7C3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A7'), U"\x7375") << desc_decomp_of(U'\xF9A7') << "\n  Line: F9A7;CJK COMPATIBILITY IDEOGRAPH-F9A7;Lo;0;L;7375;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A8'), U"\x4EE4") << desc_decomp_of(U'\xF9A8') << "\n  Line: F9A8;CJK COMPATIBILITY IDEOGRAPH-F9A8;Lo;0;L;4EE4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9A9'), U"\x56F9") << desc_decomp_of(U'\xF9A9') << "\n  Line: F9A9;CJK COMPATIBILITY IDEOGRAPH-F9A9;Lo;0;L;56F9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9AA'), U"\x5BE7") << desc_decomp_of(U'\xF9AA') << "\n  Line: F9AA;CJK COMPATIBILITY IDEOGRAPH-F9AA;Lo;0;L;5BE7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9AB'), U"\x5DBA") << desc_decomp_of(U'\xF9AB') << "\n  Line: F9AB;CJK COMPATIBILITY IDEOGRAPH-F9AB;Lo;0;L;5DBA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9AC'), U"\x601C") << desc_decomp_of(U'\xF9AC') << "\n  Line: F9AC;CJK COMPATIBILITY IDEOGRAPH-F9AC;Lo;0;L;601C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9AD'), U"\x73B2") << desc_decomp_of(U'\xF9AD') << "\n  Line: F9AD;CJK COMPATIBILITY IDEOGRAPH-F9AD;Lo;0;L;73B2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9AE'), U"\x7469") << desc_decomp_of(U'\xF9AE') << "\n  Line: F9AE;CJK COMPATIBILITY IDEOGRAPH-F9AE;Lo;0;L;7469;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9AF'), U"\x7F9A") << desc_decomp_of(U'\xF9AF') << "\n  Line: F9AF;CJK COMPATIBILITY IDEOGRAPH-F9AF;Lo;0;L;7F9A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B0'), U"\x8046") << desc_decomp_of(U'\xF9B0') << "\n  Line: F9B0;CJK COMPATIBILITY IDEOGRAPH-F9B0;Lo;0;L;8046;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B1'), U"\x9234") << desc_decomp_of(U'\xF9B1') << "\n  Line: F9B1;CJK COMPATIBILITY IDEOGRAPH-F9B1;Lo;0;L;9234;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B2'), U"\x96F6") << desc_decomp_of(U'\xF9B2') << "\n  Line: F9B2;CJK COMPATIBILITY IDEOGRAPH-F9B2;Lo;0;L;96F6;;;0;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B3'), U"\x9748") << desc_decomp_of(U'\xF9B3') << "\n  Line: F9B3;CJK COMPATIBILITY IDEOGRAPH-F9B3;Lo;0;L;9748;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B4'), U"\x9818") << desc_decomp_of(U'\xF9B4') << "\n  Line: F9B4;CJK COMPATIBILITY IDEOGRAPH-F9B4;Lo;0;L;9818;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B5'), U"\x4F8B") << desc_decomp_of(U'\xF9B5') << "\n  Line: F9B5;CJK COMPATIBILITY IDEOGRAPH-F9B5;Lo;0;L;4F8B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B6'), U"\x79AE") << desc_decomp_of(U'\xF9B6') << "\n  Line: F9B6;CJK COMPATIBILITY IDEOGRAPH-F9B6;Lo;0;L;79AE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B7'), U"\x91B4") << desc_decomp_of(U'\xF9B7') << "\n  Line: F9B7;CJK COMPATIBILITY IDEOGRAPH-F9B7;Lo;0;L;91B4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B8'), U"\x96B8") << desc_decomp_of(U'\xF9B8') << "\n  Line: F9B8;CJK COMPATIBILITY IDEOGRAPH-F9B8;Lo;0;L;96B8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9B9'), U"\x60E1") << desc_decomp_of(U'\xF9B9') << "\n  Line: F9B9;CJK COMPATIBILITY IDEOGRAPH-F9B9;Lo;0;L;60E1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9BA'), U"\x4E86") << desc_decomp_of(U'\xF9BA') << "\n  Line: F9BA;CJK COMPATIBILITY IDEOGRAPH-F9BA;Lo;0;L;4E86;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9BB'), U"\x50DA") << desc_decomp_of(U'\xF9BB') << "\n  Line: F9BB;CJK COMPATIBILITY IDEOGRAPH-F9BB;Lo;0;L;50DA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9BC'), U"\x5BEE") << desc_decomp_of(U'\xF9BC') << "\n  Line: F9BC;CJK COMPATIBILITY IDEOGRAPH-F9BC;Lo;0;L;5BEE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9BD'), U"\x5C3F") << desc_decomp_of(U'\xF9BD') << "\n  Line: F9BD;CJK COMPATIBILITY IDEOGRAPH-F9BD;Lo;0;L;5C3F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9BE'), U"\x6599") << desc_decomp_of(U'\xF9BE') << "\n  Line: F9BE;CJK COMPATIBILITY IDEOGRAPH-F9BE;Lo;0;L;6599;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9BF'), U"\x6A02") << desc_decomp_of(U'\xF9BF') << "\n  Line: F9BF;CJK COMPATIBILITY IDEOGRAPH-F9BF;Lo;0;L;6A02;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C0'), U"\x71CE") << desc_decomp_of(U'\xF9C0') << "\n  Line: F9C0;CJK COMPATIBILITY IDEOGRAPH-F9C0;Lo;0;L;71CE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C1'), U"\x7642") << desc_decomp_of(U'\xF9C1') << "\n  Line: F9C1;CJK COMPATIBILITY IDEOGRAPH-F9C1;Lo;0;L;7642;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C2'), U"\x84FC") << desc_decomp_of(U'\xF9C2') << "\n  Line: F9C2;CJK COMPATIBILITY IDEOGRAPH-F9C2;Lo;0;L;84FC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C3'), U"\x907C") << desc_decomp_of(U'\xF9C3') << "\n  Line: F9C3;CJK COMPATIBILITY IDEOGRAPH-F9C3;Lo;0;L;907C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C4'), U"\x9F8D") << desc_decomp_of(U'\xF9C4') << "\n  Line: F9C4;CJK COMPATIBILITY IDEOGRAPH-F9C4;Lo;0;L;9F8D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C5'), U"\x6688") << desc_decomp_of(U'\xF9C5') << "\n  Line: F9C5;CJK COMPATIBILITY IDEOGRAPH-F9C5;Lo;0;L;6688;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C6'), U"\x962E") << desc_decomp_of(U'\xF9C6') << "\n  Line: F9C6;CJK COMPATIBILITY IDEOGRAPH-F9C6;Lo;0;L;962E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C7'), U"\x5289") << desc_decomp_of(U'\xF9C7') << "\n  Line: F9C7;CJK COMPATIBILITY IDEOGRAPH-F9C7;Lo;0;L;5289;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C8'), U"\x677B") << desc_decomp_of(U'\xF9C8') << "\n  Line: F9C8;CJK COMPATIBILITY IDEOGRAPH-F9C8;Lo;0;L;677B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9C9'), U"\x67F3") << desc_decomp_of(U'\xF9C9') << "\n  Line: F9C9;CJK COMPATIBILITY IDEOGRAPH-F9C9;Lo;0;L;67F3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9CA'), U"\x6D41") << desc_decomp_of(U'\xF9CA') << "\n  Line: F9CA;CJK COMPATIBILITY IDEOGRAPH-F9CA;Lo;0;L;6D41;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9CB'), U"\x6E9C") << desc_decomp_of(U'\xF9CB') << "\n  Line: F9CB;CJK COMPATIBILITY IDEOGRAPH-F9CB;Lo;0;L;6E9C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9CC'), U"\x7409") << desc_decomp_of(U'\xF9CC') << "\n  Line: F9CC;CJK COMPATIBILITY IDEOGRAPH-F9CC;Lo;0;L;7409;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9CD'), U"\x7559") << desc_decomp_of(U'\xF9CD') << "\n  Line: F9CD;CJK COMPATIBILITY IDEOGRAPH-F9CD;Lo;0;L;7559;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9CE'), U"\x786B") << desc_decomp_of(U'\xF9CE') << "\n  Line: F9CE;CJK COMPATIBILITY IDEOGRAPH-F9CE;Lo;0;L;786B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9CF'), U"\x7D10") << desc_decomp_of(U'\xF9CF') << "\n  Line: F9CF;CJK COMPATIBILITY IDEOGRAPH-F9CF;Lo;0;L;7D10;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D0'), U"\x985E") << desc_decomp_of(U'\xF9D0') << "\n  Line: F9D0;CJK COMPATIBILITY IDEOGRAPH-F9D0;Lo;0;L;985E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D1'), U"\x516D") << desc_decomp_of(U'\xF9D1') << "\n  Line: F9D1;CJK COMPATIBILITY IDEOGRAPH-F9D1;Lo;0;L;516D;;;6;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D2'), U"\x622E") << desc_decomp_of(U'\xF9D2') << "\n  Line: F9D2;CJK COMPATIBILITY IDEOGRAPH-F9D2;Lo;0;L;622E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D3'), U"\x9678") << desc_decomp_of(U'\xF9D3') << "\n  Line: F9D3;CJK COMPATIBILITY IDEOGRAPH-F9D3;Lo;0;L;9678;;;6;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D4'), U"\x502B") << desc_decomp_of(U'\xF9D4') << "\n  Line: F9D4;CJK COMPATIBILITY IDEOGRAPH-F9D4;Lo;0;L;502B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D5'), U"\x5D19") << desc_decomp_of(U'\xF9D5') << "\n  Line: F9D5;CJK COMPATIBILITY IDEOGRAPH-F9D5;Lo;0;L;5D19;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D6'), U"\x6DEA") << desc_decomp_of(U'\xF9D6') << "\n  Line: F9D6;CJK COMPATIBILITY IDEOGRAPH-F9D6;Lo;0;L;6DEA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D7'), U"\x8F2A") << desc_decomp_of(U'\xF9D7') << "\n  Line: F9D7;CJK COMPATIBILITY IDEOGRAPH-F9D7;Lo;0;L;8F2A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D8'), U"\x5F8B") << desc_decomp_of(U'\xF9D8') << "\n  Line: F9D8;CJK COMPATIBILITY IDEOGRAPH-F9D8;Lo;0;L;5F8B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9D9'), U"\x6144") << desc_decomp_of(U'\xF9D9') << "\n  Line: F9D9;CJK COMPATIBILITY IDEOGRAPH-F9D9;Lo;0;L;6144;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9DA'), U"\x6817") << desc_decomp_of(U'\xF9DA') << "\n  Line: F9DA;CJK COMPATIBILITY IDEOGRAPH-F9DA;Lo;0;L;6817;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9DB'), U"\x7387") << desc_decomp_of(U'\xF9DB') << "\n  Line: F9DB;CJK COMPATIBILITY IDEOGRAPH-F9DB;Lo;0;L;7387;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9DC'), U"\x9686") << desc_decomp_of(U'\xF9DC') << "\n  Line: F9DC;CJK COMPATIBILITY IDEOGRAPH-F9DC;Lo;0;L;9686;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9DD'), U"\x5229") << desc_decomp_of(U'\xF9DD') << "\n  Line: F9DD;CJK COMPATIBILITY IDEOGRAPH-F9DD;Lo;0;L;5229;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9DE'), U"\x540F") << desc_decomp_of(U'\xF9DE') << "\n  Line: F9DE;CJK COMPATIBILITY IDEOGRAPH-F9DE;Lo;0;L;540F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9DF'), U"\x5C65") << desc_decomp_of(U'\xF9DF') << "\n  Line: F9DF;CJK COMPATIBILITY IDEOGRAPH-F9DF;Lo;0;L;5C65;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E0'), U"\x6613") << desc_decomp_of(U'\xF9E0') << "\n  Line: F9E0;CJK COMPATIBILITY IDEOGRAPH-F9E0;Lo;0;L;6613;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E1'), U"\x674E") << desc_decomp_of(U'\xF9E1') << "\n  Line: F9E1;CJK COMPATIBILITY IDEOGRAPH-F9E1;Lo;0;L;674E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E2'), U"\x68A8") << desc_decomp_of(U'\xF9E2') << "\n  Line: F9E2;CJK COMPATIBILITY IDEOGRAPH-F9E2;Lo;0;L;68A8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E3'), U"\x6CE5") << desc_decomp_of(U'\xF9E3') << "\n  Line: F9E3;CJK COMPATIBILITY IDEOGRAPH-F9E3;Lo;0;L;6CE5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E4'), U"\x7406") << desc_decomp_of(U'\xF9E4') << "\n  Line: F9E4;CJK COMPATIBILITY IDEOGRAPH-F9E4;Lo;0;L;7406;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E5'), U"\x75E2") << desc_decomp_of(U'\xF9E5') << "\n  Line: F9E5;CJK COMPATIBILITY IDEOGRAPH-F9E5;Lo;0;L;75E2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E6'), U"\x7F79") << desc_decomp_of(U'\xF9E6') << "\n  Line: F9E6;CJK COMPATIBILITY IDEOGRAPH-F9E6;Lo;0;L;7F79;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E7'), U"\x88CF") << desc_decomp_of(U'\xF9E7') << "\n  Line: F9E7;CJK COMPATIBILITY IDEOGRAPH-F9E7;Lo;0;L;88CF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E8'), U"\x88E1") << desc_decomp_of(U'\xF9E8') << "\n  Line: F9E8;CJK COMPATIBILITY IDEOGRAPH-F9E8;Lo;0;L;88E1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9E9'), U"\x91CC") << desc_decomp_of(U'\xF9E9') << "\n  Line: F9E9;CJK COMPATIBILITY IDEOGRAPH-F9E9;Lo;0;L;91CC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9EA'), U"\x96E2") << desc_decomp_of(U'\xF9EA') << "\n  Line: F9EA;CJK COMPATIBILITY IDEOGRAPH-F9EA;Lo;0;L;96E2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9EB'), U"\x533F") << desc_decomp_of(U'\xF9EB') << "\n  Line: F9EB;CJK COMPATIBILITY IDEOGRAPH-F9EB;Lo;0;L;533F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9EC'), U"\x6EBA") << desc_decomp_of(U'\xF9EC') << "\n  Line: F9EC;CJK COMPATIBILITY IDEOGRAPH-F9EC;Lo;0;L;6EBA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9ED'), U"\x541D") << desc_decomp_of(U'\xF9ED') << "\n  Line: F9ED;CJK COMPATIBILITY IDEOGRAPH-F9ED;Lo;0;L;541D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9EE'), U"\x71D0") << desc_decomp_of(U'\xF9EE') << "\n  Line: F9EE;CJK COMPATIBILITY IDEOGRAPH-F9EE;Lo;0;L;71D0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9EF'), U"\x7498") << desc_decomp_of(U'\xF9EF') << "\n  Line: F9EF;CJK COMPATIBILITY IDEOGRAPH-F9EF;Lo;0;L;7498;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F0'), U"\x85FA") << desc_decomp_of(U'\xF9F0') << "\n  Line: F9F0;CJK COMPATIBILITY IDEOGRAPH-F9F0;Lo;0;L;85FA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F1'), U"\x96A3") << desc_decomp_of(U'\xF9F1') << "\n  Line: F9F1;CJK COMPATIBILITY IDEOGRAPH-F9F1;Lo;0;L;96A3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F2'), U"\x9C57") << desc_decomp_of(U'\xF9F2') << "\n  Line: F9F2;CJK COMPATIBILITY IDEOGRAPH-F9F2;Lo;0;L;9C57;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F3'), U"\x9E9F") << desc_decomp_of(U'\xF9F3') << "\n  Line: F9F3;CJK COMPATIBILITY IDEOGRAPH-F9F3;Lo;0;L;9E9F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F4'), U"\x6797") << desc_decomp_of(U'\xF9F4') << "\n  Line: F9F4;CJK COMPATIBILITY IDEOGRAPH-F9F4;Lo;0;L;6797;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F5'), U"\x6DCB") << desc_decomp_of(U'\xF9F5') << "\n  Line: F9F5;CJK COMPATIBILITY IDEOGRAPH-F9F5;Lo;0;L;6DCB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F6'), U"\x81E8") << desc_decomp_of(U'\xF9F6') << "\n  Line: F9F6;CJK COMPATIBILITY IDEOGRAPH-F9F6;Lo;0;L;81E8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F7'), U"\x7ACB") << desc_decomp_of(U'\xF9F7') << "\n  Line: F9F7;CJK COMPATIBILITY IDEOGRAPH-F9F7;Lo;0;L;7ACB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F8'), U"\x7B20") << desc_decomp_of(U'\xF9F8') << "\n  Line: F9F8;CJK COMPATIBILITY IDEOGRAPH-F9F8;Lo;0;L;7B20;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9F9'), U"\x7C92") << desc_decomp_of(U'\xF9F9') << "\n  Line: F9F9;CJK COMPATIBILITY IDEOGRAPH-F9F9;Lo;0;L;7C92;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9FA'), U"\x72C0") << desc_decomp_of(U'\xF9FA') << "\n  Line: F9FA;CJK COMPATIBILITY IDEOGRAPH-F9FA;Lo;0;L;72C0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9FB'), U"\x7099") << desc_decomp_of(U'\xF9FB') << "\n  Line: F9FB;CJK COMPATIBILITY IDEOGRAPH-F9FB;Lo;0;L;7099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9FC'), U"\x8B58") << desc_decomp_of(U'\xF9FC') << "\n  Line: F9FC;CJK COMPATIBILITY IDEOGRAPH-F9FC;Lo;0;L;8B58;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9FD'), U"\x4EC0") << desc_decomp_of(U'\xF9FD') << "\n  Line: F9FD;CJK COMPATIBILITY IDEOGRAPH-F9FD;Lo;0;L;4EC0;;;10;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9FE'), U"\x8336") << desc_decomp_of(U'\xF9FE') << "\n  Line: F9FE;CJK COMPATIBILITY IDEOGRAPH-F9FE;Lo;0;L;8336;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xF9FF'), U"\x523A") << desc_decomp_of(U'\xF9FF') << "\n  Line: F9FF;CJK COMPATIBILITY IDEOGRAPH-F9FF;Lo;0;L;523A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA00'), U"\x5207") << desc_decomp_of(U'\xFA00') << "\n  Line: FA00;CJK COMPATIBILITY IDEOGRAPH-FA00;Lo;0;L;5207;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA01'), U"\x5EA6") << desc_decomp_of(U'\xFA01') << "\n  Line: FA01;CJK COMPATIBILITY IDEOGRAPH-FA01;Lo;0;L;5EA6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA02'), U"\x62D3") << desc_decomp_of(U'\xFA02') << "\n  Line: FA02;CJK COMPATIBILITY IDEOGRAPH-FA02;Lo;0;L;62D3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA03'), U"\x7CD6") << desc_decomp_of(U'\xFA03') << "\n  Line: FA03;CJK COMPATIBILITY IDEOGRAPH-FA03;Lo;0;L;7CD6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA04'), U"\x5B85") << desc_decomp_of(U'\xFA04') << "\n  Line: FA04;CJK COMPATIBILITY IDEOGRAPH-FA04;Lo;0;L;5B85;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA05'), U"\x6D1E") << desc_decomp_of(U'\xFA05') << "\n  Line: FA05;CJK COMPATIBILITY IDEOGRAPH-FA05;Lo;0;L;6D1E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA06'), U"\x66B4") << desc_decomp_of(U'\xFA06') << "\n  Line: FA06;CJK COMPATIBILITY IDEOGRAPH-FA06;Lo;0;L;66B4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA07'), U"\x8F3B") << desc_decomp_of(U'\xFA07') << "\n  Line: FA07;CJK COMPATIBILITY IDEOGRAPH-FA07;Lo;0;L;8F3B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA08'), U"\x884C") << desc_decomp_of(U'\xFA08') << "\n  Line: FA08;CJK COMPATIBILITY IDEOGRAPH-FA08;Lo;0;L;884C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA09'), U"\x964D") << desc_decomp_of(U'\xFA09') << "\n  Line: FA09;CJK COMPATIBILITY IDEOGRAPH-FA09;Lo;0;L;964D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA0A'), U"\x898B") << desc_decomp_of(U'\xFA0A') << "\n  Line: FA0A;CJK COMPATIBILITY IDEOGRAPH-FA0A;Lo;0;L;898B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA0B'), U"\x5ED3") << desc_decomp_of(U'\xFA0B') << "\n  Line: FA0B;CJK COMPATIBILITY IDEOGRAPH-FA0B;Lo;0;L;5ED3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA0C'), U"\x5140") << desc_decomp_of(U'\xFA0C') << "\n  Line: FA0C;CJK COMPATIBILITY IDEOGRAPH-FA0C;Lo;0;L;5140;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA0D'), U"\x55C0") << desc_decomp_of(U'\xFA0D') << "\n  Line: FA0D;CJK COMPATIBILITY IDEOGRAPH-FA0D;Lo;0;L;55C0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA10'), U"\x585A") << desc_decomp_of(U'\xFA10') << "\n  Line: FA10;CJK COMPATIBILITY IDEOGRAPH-FA10;Lo;0;L;585A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA12'), U"\x6674") << desc_decomp_of(U'\xFA12') << "\n  Line: FA12;CJK COMPATIBILITY IDEOGRAPH-FA12;Lo;0;L;6674;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA15'), U"\x51DE") << desc_decomp_of(U'\xFA15') << "\n  Line: FA15;CJK COMPATIBILITY IDEOGRAPH-FA15;Lo;0;L;51DE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA16'), U"\x732A") << desc_decomp_of(U'\xFA16') << "\n  Line: FA16;CJK COMPATIBILITY IDEOGRAPH-FA16;Lo;0;L;732A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA17'), U"\x76CA") << desc_decomp_of(U'\xFA17') << "\n  Line: FA17;CJK COMPATIBILITY IDEOGRAPH-FA17;Lo;0;L;76CA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA18'), U"\x793C") << desc_decomp_of(U'\xFA18') << "\n  Line: FA18;CJK COMPATIBILITY IDEOGRAPH-FA18;Lo;0;L;793C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA19'), U"\x795E") << desc_decomp_of(U'\xFA19') << "\n  Line: FA19;CJK COMPATIBILITY IDEOGRAPH-FA19;Lo;0;L;795E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA1A'), U"\x7965") << desc_decomp_of(U'\xFA1A') << "\n  Line: FA1A;CJK COMPATIBILITY IDEOGRAPH-FA1A;Lo;0;L;7965;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA1B'), U"\x798F") << desc_decomp_of(U'\xFA1B') << "\n  Line: FA1B;CJK COMPATIBILITY IDEOGRAPH-FA1B;Lo;0;L;798F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA1C'), U"\x9756") << desc_decomp_of(U'\xFA1C') << "\n  Line: FA1C;CJK COMPATIBILITY IDEOGRAPH-FA1C;Lo;0;L;9756;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA1D'), U"\x7CBE") << desc_decomp_of(U'\xFA1D') << "\n  Line: FA1D;CJK COMPATIBILITY IDEOGRAPH-FA1D;Lo;0;L;7CBE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA1E'), U"\x7FBD") << desc_decomp_of(U'\xFA1E') << "\n  Line: FA1E;CJK COMPATIBILITY IDEOGRAPH-FA1E;Lo;0;L;7FBD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA20'), U"\x8612") << desc_decomp_of(U'\xFA20') << "\n  Line: FA20;CJK COMPATIBILITY IDEOGRAPH-FA20;Lo;0;L;8612;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA22'), U"\x8AF8") << desc_decomp_of(U'\xFA22') << "\n  Line: FA22;CJK COMPATIBILITY IDEOGRAPH-FA22;Lo;0;L;8AF8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA25'), U"\x9038") << desc_decomp_of(U'\xFA25') << "\n  Line: FA25;CJK COMPATIBILITY IDEOGRAPH-FA25;Lo;0;L;9038;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA26'), U"\x90FD") << desc_decomp_of(U'\xFA26') << "\n  Line: FA26;CJK COMPATIBILITY IDEOGRAPH-FA26;Lo;0;L;90FD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA2A'), U"\x98EF") << desc_decomp_of(U'\xFA2A') << "\n  Line: FA2A;CJK COMPATIBILITY IDEOGRAPH-FA2A;Lo;0;L;98EF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA2B'), U"\x98FC") << desc_decomp_of(U'\xFA2B') << "\n  Line: FA2B;CJK COMPATIBILITY IDEOGRAPH-FA2B;Lo;0;L;98FC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA2C'), U"\x9928") << desc_decomp_of(U'\xFA2C') << "\n  Line: FA2C;CJK COMPATIBILITY IDEOGRAPH-FA2C;Lo;0;L;9928;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA2D'), U"\x9DB4") << desc_decomp_of(U'\xFA2D') << "\n  Line: FA2D;CJK COMPATIBILITY IDEOGRAPH-FA2D;Lo;0;L;9DB4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA2E'), U"\x90DE") << desc_decomp_of(U'\xFA2E') << "\n  Line: FA2E;CJK COMPATIBILITY IDEOGRAPH-FA2E;Lo;0;L;90DE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA2F'), U"\x96B7") << desc_decomp_of(U'\xFA2F') << "\n  Line: FA2F;CJK COMPATIBILITY IDEOGRAPH-FA2F;Lo;0;L;96B7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA30'), U"\x4FAE") << desc_decomp_of(U'\xFA30') << "\n  Line: FA30;CJK COMPATIBILITY IDEOGRAPH-FA30;Lo;0;L;4FAE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA31'), U"\x50E7") << desc_decomp_of(U'\xFA31') << "\n  Line: FA31;CJK COMPATIBILITY IDEOGRAPH-FA31;Lo;0;L;50E7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA32'), U"\x514D") << desc_decomp_of(U'\xFA32') << "\n  Line: FA32;CJK COMPATIBILITY IDEOGRAPH-FA32;Lo;0;L;514D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA33'), U"\x52C9") << desc_decomp_of(U'\xFA33') << "\n  Line: FA33;CJK COMPATIBILITY IDEOGRAPH-FA33;Lo;0;L;52C9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA34'), U"\x52E4") << desc_decomp_of(U'\xFA34') << "\n  Line: FA34;CJK COMPATIBILITY IDEOGRAPH-FA34;Lo;0;L;52E4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA35'), U"\x5351") << desc_decomp_of(U'\xFA35') << "\n  Line: FA35;CJK COMPATIBILITY IDEOGRAPH-FA35;Lo;0;L;5351;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA36'), U"\x559D") << desc_decomp_of(U'\xFA36') << "\n  Line: FA36;CJK COMPATIBILITY IDEOGRAPH-FA36;Lo;0;L;559D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA37'), U"\x5606") << desc_decomp_of(U'\xFA37') << "\n  Line: FA37;CJK COMPATIBILITY IDEOGRAPH-FA37;Lo;0;L;5606;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA38'), U"\x5668") << desc_decomp_of(U'\xFA38') << "\n  Line: FA38;CJK COMPATIBILITY IDEOGRAPH-FA38;Lo;0;L;5668;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA39'), U"\x5840") << desc_decomp_of(U'\xFA39') << "\n  Line: FA39;CJK COMPATIBILITY IDEOGRAPH-FA39;Lo;0;L;5840;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA3A'), U"\x58A8") << desc_decomp_of(U'\xFA3A') << "\n  Line: FA3A;CJK COMPATIBILITY IDEOGRAPH-FA3A;Lo;0;L;58A8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA3B'), U"\x5C64") << desc_decomp_of(U'\xFA3B') << "\n  Line: FA3B;CJK COMPATIBILITY IDEOGRAPH-FA3B;Lo;0;L;5C64;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA3C'), U"\x5C6E") << desc_decomp_of(U'\xFA3C') << "\n  Line: FA3C;CJK COMPATIBILITY IDEOGRAPH-FA3C;Lo;0;L;5C6E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA3D'), U"\x6094") << desc_decomp_of(U'\xFA3D') << "\n  Line: FA3D;CJK COMPATIBILITY IDEOGRAPH-FA3D;Lo;0;L;6094;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA3E'), U"\x6168") << desc_decomp_of(U'\xFA3E') << "\n  Line: FA3E;CJK COMPATIBILITY IDEOGRAPH-FA3E;Lo;0;L;6168;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA3F'), U"\x618E") << desc_decomp_of(U'\xFA3F') << "\n  Line: FA3F;CJK COMPATIBILITY IDEOGRAPH-FA3F;Lo;0;L;618E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA40'), U"\x61F2") << desc_decomp_of(U'\xFA40') << "\n  Line: FA40;CJK COMPATIBILITY IDEOGRAPH-FA40;Lo;0;L;61F2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA41'), U"\x654F") << desc_decomp_of(U'\xFA41') << "\n  Line: FA41;CJK COMPATIBILITY IDEOGRAPH-FA41;Lo;0;L;654F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA42'), U"\x65E2") << desc_decomp_of(U'\xFA42') << "\n  Line: FA42;CJK COMPATIBILITY IDEOGRAPH-FA42;Lo;0;L;65E2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA43'), U"\x6691") << desc_decomp_of(U'\xFA43') << "\n  Line: FA43;CJK COMPATIBILITY IDEOGRAPH-FA43;Lo;0;L;6691;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA44'), U"\x6885") << desc_decomp_of(U'\xFA44') << "\n  Line: FA44;CJK COMPATIBILITY IDEOGRAPH-FA44;Lo;0;L;6885;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA45'), U"\x6D77") << desc_decomp_of(U'\xFA45') << "\n  Line: FA45;CJK COMPATIBILITY IDEOGRAPH-FA45;Lo;0;L;6D77;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA46'), U"\x6E1A") << desc_decomp_of(U'\xFA46') << "\n  Line: FA46;CJK COMPATIBILITY IDEOGRAPH-FA46;Lo;0;L;6E1A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA47'), U"\x6F22") << desc_decomp_of(U'\xFA47') << "\n  Line: FA47;CJK COMPATIBILITY IDEOGRAPH-FA47;Lo;0;L;6F22;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA48'), U"\x716E") << desc_decomp_of(U'\xFA48') << "\n  Line: FA48;CJK COMPATIBILITY IDEOGRAPH-FA48;Lo;0;L;716E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA49'), U"\x722B") << desc_decomp_of(U'\xFA49') << "\n  Line: FA49;CJK COMPATIBILITY IDEOGRAPH-FA49;Lo;0;L;722B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA4A'), U"\x7422") << desc_decomp_of(U'\xFA4A') << "\n  Line: FA4A;CJK COMPATIBILITY IDEOGRAPH-FA4A;Lo;0;L;7422;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA4B'), U"\x7891") << desc_decomp_of(U'\xFA4B') << "\n  Line: FA4B;CJK COMPATIBILITY IDEOGRAPH-FA4B;Lo;0;L;7891;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA4C'), U"\x793E") << desc_decomp_of(U'\xFA4C') << "\n  Line: FA4C;CJK COMPATIBILITY IDEOGRAPH-FA4C;Lo;0;L;793E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA4D'), U"\x7949") << desc_decomp_of(U'\xFA4D') << "\n  Line: FA4D;CJK COMPATIBILITY IDEOGRAPH-FA4D;Lo;0;L;7949;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA4E'), U"\x7948") << desc_decomp_of(U'\xFA4E') << "\n  Line: FA4E;CJK COMPATIBILITY IDEOGRAPH-FA4E;Lo;0;L;7948;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA4F'), U"\x7950") << desc_decomp_of(U'\xFA4F') << "\n  Line: FA4F;CJK COMPATIBILITY IDEOGRAPH-FA4F;Lo;0;L;7950;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA50'), U"\x7956") << desc_decomp_of(U'\xFA50') << "\n  Line: FA50;CJK COMPATIBILITY IDEOGRAPH-FA50;Lo;0;L;7956;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA51'), U"\x795D") << desc_decomp_of(U'\xFA51') << "\n  Line: FA51;CJK COMPATIBILITY IDEOGRAPH-FA51;Lo;0;L;795D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA52'), U"\x798D") << desc_decomp_of(U'\xFA52') << "\n  Line: FA52;CJK COMPATIBILITY IDEOGRAPH-FA52;Lo;0;L;798D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA53'), U"\x798E") << desc_decomp_of(U'\xFA53') << "\n  Line: FA53;CJK COMPATIBILITY IDEOGRAPH-FA53;Lo;0;L;798E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA54'), U"\x7A40") << desc_decomp_of(U'\xFA54') << "\n  Line: FA54;CJK COMPATIBILITY IDEOGRAPH-FA54;Lo;0;L;7A40;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA55'), U"\x7A81") << desc_decomp_of(U'\xFA55') << "\n  Line: FA55;CJK COMPATIBILITY IDEOGRAPH-FA55;Lo;0;L;7A81;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA56'), U"\x7BC0") << desc_decomp_of(U'\xFA56') << "\n  Line: FA56;CJK COMPATIBILITY IDEOGRAPH-FA56;Lo;0;L;7BC0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA57'), U"\x7DF4") << desc_decomp_of(U'\xFA57') << "\n  Line: FA57;CJK COMPATIBILITY IDEOGRAPH-FA57;Lo;0;L;7DF4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA58'), U"\x7E09") << desc_decomp_of(U'\xFA58') << "\n  Line: FA58;CJK COMPATIBILITY IDEOGRAPH-FA58;Lo;0;L;7E09;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA59'), U"\x7E41") << desc_decomp_of(U'\xFA59') << "\n  Line: FA59;CJK COMPATIBILITY IDEOGRAPH-FA59;Lo;0;L;7E41;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA5A'), U"\x7F72") << desc_decomp_of(U'\xFA5A') << "\n  Line: FA5A;CJK COMPATIBILITY IDEOGRAPH-FA5A;Lo;0;L;7F72;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA5B'), U"\x8005") << desc_decomp_of(U'\xFA5B') << "\n  Line: FA5B;CJK COMPATIBILITY IDEOGRAPH-FA5B;Lo;0;L;8005;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA5C'), U"\x81ED") << desc_decomp_of(U'\xFA5C') << "\n  Line: FA5C;CJK COMPATIBILITY IDEOGRAPH-FA5C;Lo;0;L;81ED;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA5D'), U"\x8279") << desc_decomp_of(U'\xFA5D') << "\n  Line: FA5D;CJK COMPATIBILITY IDEOGRAPH-FA5D;Lo;0;L;8279;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA5E'), U"\x8279") << desc_decomp_of(U'\xFA5E') << "\n  Line: FA5E;CJK COMPATIBILITY IDEOGRAPH-FA5E;Lo;0;L;8279;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA5F'), U"\x8457") << desc_decomp_of(U'\xFA5F') << "\n  Line: FA5F;CJK COMPATIBILITY IDEOGRAPH-FA5F;Lo;0;L;8457;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA60'), U"\x8910") << desc_decomp_of(U'\xFA60') << "\n  Line: FA60;CJK COMPATIBILITY IDEOGRAPH-FA60;Lo;0;L;8910;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA61'), U"\x8996") << desc_decomp_of(U'\xFA61') << "\n  Line: FA61;CJK COMPATIBILITY IDEOGRAPH-FA61;Lo;0;L;8996;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA62'), U"\x8B01") << desc_decomp_of(U'\xFA62') << "\n  Line: FA62;CJK COMPATIBILITY IDEOGRAPH-FA62;Lo;0;L;8B01;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA63'), U"\x8B39") << desc_decomp_of(U'\xFA63') << "\n  Line: FA63;CJK COMPATIBILITY IDEOGRAPH-FA63;Lo;0;L;8B39;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA64'), U"\x8CD3") << desc_decomp_of(U'\xFA64') << "\n  Line: FA64;CJK COMPATIBILITY IDEOGRAPH-FA64;Lo;0;L;8CD3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA65'), U"\x8D08") << desc_decomp_of(U'\xFA65') << "\n  Line: FA65;CJK COMPATIBILITY IDEOGRAPH-FA65;Lo;0;L;8D08;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA66'), U"\x8FB6") << desc_decomp_of(U'\xFA66') << "\n  Line: FA66;CJK COMPATIBILITY IDEOGRAPH-FA66;Lo;0;L;8FB6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA67'), U"\x9038") << desc_decomp_of(U'\xFA67') << "\n  Line: FA67;CJK COMPATIBILITY IDEOGRAPH-FA67;Lo;0;L;9038;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA68'), U"\x96E3") << desc_decomp_of(U'\xFA68') << "\n  Line: FA68;CJK COMPATIBILITY IDEOGRAPH-FA68;Lo;0;L;96E3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA69'), U"\x97FF") << desc_decomp_of(U'\xFA69') << "\n  Line: FA69;CJK COMPATIBILITY IDEOGRAPH-FA69;Lo;0;L;97FF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA6A'), U"\x983B") << desc_decomp_of(U'\xFA6A') << "\n  Line: FA6A;CJK COMPATIBILITY IDEOGRAPH-FA6A;Lo;0;L;983B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA6B'), U"\x6075") << desc_decomp_of(U'\xFA6B') << "\n  Line: FA6B;CJK COMPATIBILITY IDEOGRAPH-FA6B;Lo;0;L;6075;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA6C'), U"\x242EE") << desc_decomp_of(U'\xFA6C') << "\n  Line: FA6C;CJK COMPATIBILITY IDEOGRAPH-FA6C;Lo;0;L;242EE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA6D'), U"\x8218") << desc_decomp_of(U'\xFA6D') << "\n  Line: FA6D;CJK COMPATIBILITY IDEOGRAPH-FA6D;Lo;0;L;8218;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA70'), U"\x4E26") << desc_decomp_of(U'\xFA70') << "\n  Line: FA70;CJK COMPATIBILITY IDEOGRAPH-FA70;Lo;0;L;4E26;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA71'), U"\x51B5") << desc_decomp_of(U'\xFA71') << "\n  Line: FA71;CJK COMPATIBILITY IDEOGRAPH-FA71;Lo;0;L;51B5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA72'), U"\x5168") << desc_decomp_of(U'\xFA72') << "\n  Line: FA72;CJK COMPATIBILITY IDEOGRAPH-FA72;Lo;0;L;5168;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA73'), U"\x4F80") << desc_decomp_of(U'\xFA73') << "\n  Line: FA73;CJK COMPATIBILITY IDEOGRAPH-FA73;Lo;0;L;4F80;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA74'), U"\x5145") << desc_decomp_of(U'\xFA74') << "\n  Line: FA74;CJK COMPATIBILITY IDEOGRAPH-FA74;Lo;0;L;5145;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA75'), U"\x5180") << desc_decomp_of(U'\xFA75') << "\n  Line: FA75;CJK COMPATIBILITY IDEOGRAPH-FA75;Lo;0;L;5180;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA76'), U"\x52C7") << desc_decomp_of(U'\xFA76') << "\n  Line: FA76;CJK COMPATIBILITY IDEOGRAPH-FA76;Lo;0;L;52C7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA77'), U"\x52FA") << desc_decomp_of(U'\xFA77') << "\n  Line: FA77;CJK COMPATIBILITY IDEOGRAPH-FA77;Lo;0;L;52FA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA78'), U"\x559D") << desc_decomp_of(U'\xFA78') << "\n  Line: FA78;CJK COMPATIBILITY IDEOGRAPH-FA78;Lo;0;L;559D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA79'), U"\x5555") << desc_decomp_of(U'\xFA79') << "\n  Line: FA79;CJK COMPATIBILITY IDEOGRAPH-FA79;Lo;0;L;5555;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA7A'), U"\x5599") << desc_decomp_of(U'\xFA7A') << "\n  Line: FA7A;CJK COMPATIBILITY IDEOGRAPH-FA7A;Lo;0;L;5599;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA7B'), U"\x55E2") << desc_decomp_of(U'\xFA7B') << "\n  Line: FA7B;CJK COMPATIBILITY IDEOGRAPH-FA7B;Lo;0;L;55E2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA7C'), U"\x585A") << desc_decomp_of(U'\xFA7C') << "\n  Line: FA7C;CJK COMPATIBILITY IDEOGRAPH-FA7C;Lo;0;L;585A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA7D'), U"\x58B3") << desc_decomp_of(U'\xFA7D') << "\n  Line: FA7D;CJK COMPATIBILITY IDEOGRAPH-FA7D;Lo;0;L;58B3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA7E'), U"\x5944") << desc_decomp_of(U'\xFA7E') << "\n  Line: FA7E;CJK COMPATIBILITY IDEOGRAPH-FA7E;Lo;0;L;5944;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA7F'), U"\x5954") << desc_decomp_of(U'\xFA7F') << "\n  Line: FA7F;CJK COMPATIBILITY IDEOGRAPH-FA7F;Lo;0;L;5954;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA80'), U"\x5A62") << desc_decomp_of(U'\xFA80') << "\n  Line: FA80;CJK COMPATIBILITY IDEOGRAPH-FA80;Lo;0;L;5A62;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA81'), U"\x5B28") << desc_decomp_of(U'\xFA81') << "\n  Line: FA81;CJK COMPATIBILITY IDEOGRAPH-FA81;Lo;0;L;5B28;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA82'), U"\x5ED2") << desc_decomp_of(U'\xFA82') << "\n  Line: FA82;CJK COMPATIBILITY IDEOGRAPH-FA82;Lo;0;L;5ED2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA83'), U"\x5ED9") << desc_decomp_of(U'\xFA83') << "\n  Line: FA83;CJK COMPATIBILITY IDEOGRAPH-FA83;Lo;0;L;5ED9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA84'), U"\x5F69") << desc_decomp_of(U'\xFA84') << "\n  Line: FA84;CJK COMPATIBILITY IDEOGRAPH-FA84;Lo;0;L;5F69;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA85'), U"\x5FAD") << desc_decomp_of(U'\xFA85') << "\n  Line: FA85;CJK COMPATIBILITY IDEOGRAPH-FA85;Lo;0;L;5FAD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA86'), U"\x60D8") << desc_decomp_of(U'\xFA86') << "\n  Line: FA86;CJK COMPATIBILITY IDEOGRAPH-FA86;Lo;0;L;60D8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA87'), U"\x614E") << desc_decomp_of(U'\xFA87') << "\n  Line: FA87;CJK COMPATIBILITY IDEOGRAPH-FA87;Lo;0;L;614E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA88'), U"\x6108") << desc_decomp_of(U'\xFA88') << "\n  Line: FA88;CJK COMPATIBILITY IDEOGRAPH-FA88;Lo;0;L;6108;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA89'), U"\x618E") << desc_decomp_of(U'\xFA89') << "\n  Line: FA89;CJK COMPATIBILITY IDEOGRAPH-FA89;Lo;0;L;618E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA8A'), U"\x6160") << desc_decomp_of(U'\xFA8A') << "\n  Line: FA8A;CJK COMPATIBILITY IDEOGRAPH-FA8A;Lo;0;L;6160;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA8B'), U"\x61F2") << desc_decomp_of(U'\xFA8B') << "\n  Line: FA8B;CJK COMPATIBILITY IDEOGRAPH-FA8B;Lo;0;L;61F2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA8C'), U"\x6234") << desc_decomp_of(U'\xFA8C') << "\n  Line: FA8C;CJK COMPATIBILITY IDEOGRAPH-FA8C;Lo;0;L;6234;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA8D'), U"\x63C4") << desc_decomp_of(U'\xFA8D') << "\n  Line: FA8D;CJK COMPATIBILITY IDEOGRAPH-FA8D;Lo;0;L;63C4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA8E'), U"\x641C") << desc_decomp_of(U'\xFA8E') << "\n  Line: FA8E;CJK COMPATIBILITY IDEOGRAPH-FA8E;Lo;0;L;641C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA8F'), U"\x6452") << desc_decomp_of(U'\xFA8F') << "\n  Line: FA8F;CJK COMPATIBILITY IDEOGRAPH-FA8F;Lo;0;L;6452;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA90'), U"\x6556") << desc_decomp_of(U'\xFA90') << "\n  Line: FA90;CJK COMPATIBILITY IDEOGRAPH-FA90;Lo;0;L;6556;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA91'), U"\x6674") << desc_decomp_of(U'\xFA91') << "\n  Line: FA91;CJK COMPATIBILITY IDEOGRAPH-FA91;Lo;0;L;6674;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA92'), U"\x6717") << desc_decomp_of(U'\xFA92') << "\n  Line: FA92;CJK COMPATIBILITY IDEOGRAPH-FA92;Lo;0;L;6717;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA93'), U"\x671B") << desc_decomp_of(U'\xFA93') << "\n  Line: FA93;CJK COMPATIBILITY IDEOGRAPH-FA93;Lo;0;L;671B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA94'), U"\x6756") << desc_decomp_of(U'\xFA94') << "\n  Line: FA94;CJK COMPATIBILITY IDEOGRAPH-FA94;Lo;0;L;6756;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA95'), U"\x6B79") << desc_decomp_of(U'\xFA95') << "\n  Line: FA95;CJK COMPATIBILITY IDEOGRAPH-FA95;Lo;0;L;6B79;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA96'), U"\x6BBA") << desc_decomp_of(U'\xFA96') << "\n  Line: FA96;CJK COMPATIBILITY IDEOGRAPH-FA96;Lo;0;L;6BBA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA97'), U"\x6D41") << desc_decomp_of(U'\xFA97') << "\n  Line: FA97;CJK COMPATIBILITY IDEOGRAPH-FA97;Lo;0;L;6D41;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA98'), U"\x6EDB") << desc_decomp_of(U'\xFA98') << "\n  Line: FA98;CJK COMPATIBILITY IDEOGRAPH-FA98;Lo;0;L;6EDB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA99'), U"\x6ECB") << desc_decomp_of(U'\xFA99') << "\n  Line: FA99;CJK COMPATIBILITY IDEOGRAPH-FA99;Lo;0;L;6ECB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA9A'), U"\x6F22") << desc_decomp_of(U'\xFA9A') << "\n  Line: FA9A;CJK COMPATIBILITY IDEOGRAPH-FA9A;Lo;0;L;6F22;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA9B'), U"\x701E") << desc_decomp_of(U'\xFA9B') << "\n  Line: FA9B;CJK COMPATIBILITY IDEOGRAPH-FA9B;Lo;0;L;701E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA9C'), U"\x716E") << desc_decomp_of(U'\xFA9C') << "\n  Line: FA9C;CJK COMPATIBILITY IDEOGRAPH-FA9C;Lo;0;L;716E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA9D'), U"\x77A7") << desc_decomp_of(U'\xFA9D') << "\n  Line: FA9D;CJK COMPATIBILITY IDEOGRAPH-FA9D;Lo;0;L;77A7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA9E'), U"\x7235") << desc_decomp_of(U'\xFA9E') << "\n  Line: FA9E;CJK COMPATIBILITY IDEOGRAPH-FA9E;Lo;0;L;7235;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFA9F'), U"\x72AF") << desc_decomp_of(U'\xFA9F') << "\n  Line: FA9F;CJK COMPATIBILITY IDEOGRAPH-FA9F;Lo;0;L;72AF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA0'), U"\x732A") << desc_decomp_of(U'\xFAA0') << "\n  Line: FAA0;CJK COMPATIBILITY IDEOGRAPH-FAA0;Lo;0;L;732A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA1'), U"\x7471") << desc_decomp_of(U'\xFAA1') << "\n  Line: FAA1;CJK COMPATIBILITY IDEOGRAPH-FAA1;Lo;0;L;7471;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA2'), U"\x7506") << desc_decomp_of(U'\xFAA2') << "\n  Line: FAA2;CJK COMPATIBILITY IDEOGRAPH-FAA2;Lo;0;L;7506;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA3'), U"\x753B") << desc_decomp_of(U'\xFAA3') << "\n  Line: FAA3;CJK COMPATIBILITY IDEOGRAPH-FAA3;Lo;0;L;753B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA4'), U"\x761D") << desc_decomp_of(U'\xFAA4') << "\n  Line: FAA4;CJK COMPATIBILITY IDEOGRAPH-FAA4;Lo;0;L;761D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA5'), U"\x761F") << desc_decomp_of(U'\xFAA5') << "\n  Line: FAA5;CJK COMPATIBILITY IDEOGRAPH-FAA5;Lo;0;L;761F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA6'), U"\x76CA") << desc_decomp_of(U'\xFAA6') << "\n  Line: FAA6;CJK COMPATIBILITY IDEOGRAPH-FAA6;Lo;0;L;76CA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA7'), U"\x76DB") << desc_decomp_of(U'\xFAA7') << "\n  Line: FAA7;CJK COMPATIBILITY IDEOGRAPH-FAA7;Lo;0;L;76DB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA8'), U"\x76F4") << desc_decomp_of(U'\xFAA8') << "\n  Line: FAA8;CJK COMPATIBILITY IDEOGRAPH-FAA8;Lo;0;L;76F4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAA9'), U"\x774A") << desc_decomp_of(U'\xFAA9') << "\n  Line: FAA9;CJK COMPATIBILITY IDEOGRAPH-FAA9;Lo;0;L;774A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAAA'), U"\x7740") << desc_decomp_of(U'\xFAAA') << "\n  Line: FAAA;CJK COMPATIBILITY IDEOGRAPH-FAAA;Lo;0;L;7740;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAAB'), U"\x78CC") << desc_decomp_of(U'\xFAAB') << "\n  Line: FAAB;CJK COMPATIBILITY IDEOGRAPH-FAAB;Lo;0;L;78CC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAAC'), U"\x7AB1") << desc_decomp_of(U'\xFAAC') << "\n  Line: FAAC;CJK COMPATIBILITY IDEOGRAPH-FAAC;Lo;0;L;7AB1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAAD'), U"\x7BC0") << desc_decomp_of(U'\xFAAD') << "\n  Line: FAAD;CJK COMPATIBILITY IDEOGRAPH-FAAD;Lo;0;L;7BC0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAAE'), U"\x7C7B") << desc_decomp_of(U'\xFAAE') << "\n  Line: FAAE;CJK COMPATIBILITY IDEOGRAPH-FAAE;Lo;0;L;7C7B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAAF'), U"\x7D5B") << desc_decomp_of(U'\xFAAF') << "\n  Line: FAAF;CJK COMPATIBILITY IDEOGRAPH-FAAF;Lo;0;L;7D5B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB0'), U"\x7DF4") << desc_decomp_of(U'\xFAB0') << "\n  Line: FAB0;CJK COMPATIBILITY IDEOGRAPH-FAB0;Lo;0;L;7DF4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB1'), U"\x7F3E") << desc_decomp_of(U'\xFAB1') << "\n  Line: FAB1;CJK COMPATIBILITY IDEOGRAPH-FAB1;Lo;0;L;7F3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB2'), U"\x8005") << desc_decomp_of(U'\xFAB2') << "\n  Line: FAB2;CJK COMPATIBILITY IDEOGRAPH-FAB2;Lo;0;L;8005;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB3'), U"\x8352") << desc_decomp_of(U'\xFAB3') << "\n  Line: FAB3;CJK COMPATIBILITY IDEOGRAPH-FAB3;Lo;0;L;8352;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB4'), U"\x83EF") << desc_decomp_of(U'\xFAB4') << "\n  Line: FAB4;CJK COMPATIBILITY IDEOGRAPH-FAB4;Lo;0;L;83EF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB5'), U"\x8779") << desc_decomp_of(U'\xFAB5') << "\n  Line: FAB5;CJK COMPATIBILITY IDEOGRAPH-FAB5;Lo;0;L;8779;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB6'), U"\x8941") << desc_decomp_of(U'\xFAB6') << "\n  Line: FAB6;CJK COMPATIBILITY IDEOGRAPH-FAB6;Lo;0;L;8941;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB7'), U"\x8986") << desc_decomp_of(U'\xFAB7') << "\n  Line: FAB7;CJK COMPATIBILITY IDEOGRAPH-FAB7;Lo;0;L;8986;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB8'), U"\x8996") << desc_decomp_of(U'\xFAB8') << "\n  Line: FAB8;CJK COMPATIBILITY IDEOGRAPH-FAB8;Lo;0;L;8996;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAB9'), U"\x8ABF") << desc_decomp_of(U'\xFAB9') << "\n  Line: FAB9;CJK COMPATIBILITY IDEOGRAPH-FAB9;Lo;0;L;8ABF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFABA'), U"\x8AF8") << desc_decomp_of(U'\xFABA') << "\n  Line: FABA;CJK COMPATIBILITY IDEOGRAPH-FABA;Lo;0;L;8AF8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFABB'), U"\x8ACB") << desc_decomp_of(U'\xFABB') << "\n  Line: FABB;CJK COMPATIBILITY IDEOGRAPH-FABB;Lo;0;L;8ACB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFABC'), U"\x8B01") << desc_decomp_of(U'\xFABC') << "\n  Line: FABC;CJK COMPATIBILITY IDEOGRAPH-FABC;Lo;0;L;8B01;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFABD'), U"\x8AFE") << desc_decomp_of(U'\xFABD') << "\n  Line: FABD;CJK COMPATIBILITY IDEOGRAPH-FABD;Lo;0;L;8AFE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFABE'), U"\x8AED") << desc_decomp_of(U'\xFABE') << "\n  Line: FABE;CJK COMPATIBILITY IDEOGRAPH-FABE;Lo;0;L;8AED;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFABF'), U"\x8B39") << desc_decomp_of(U'\xFABF') << "\n  Line: FABF;CJK COMPATIBILITY IDEOGRAPH-FABF;Lo;0;L;8B39;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC0'), U"\x8B8A") << desc_decomp_of(U'\xFAC0') << "\n  Line: FAC0;CJK COMPATIBILITY IDEOGRAPH-FAC0;Lo;0;L;8B8A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC1'), U"\x8D08") << desc_decomp_of(U'\xFAC1') << "\n  Line: FAC1;CJK COMPATIBILITY IDEOGRAPH-FAC1;Lo;0;L;8D08;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC2'), U"\x8F38") << desc_decomp_of(U'\xFAC2') << "\n  Line: FAC2;CJK COMPATIBILITY IDEOGRAPH-FAC2;Lo;0;L;8F38;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC3'), U"\x9072") << desc_decomp_of(U'\xFAC3') << "\n  Line: FAC3;CJK COMPATIBILITY IDEOGRAPH-FAC3;Lo;0;L;9072;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC4'), U"\x9199") << desc_decomp_of(U'\xFAC4') << "\n  Line: FAC4;CJK COMPATIBILITY IDEOGRAPH-FAC4;Lo;0;L;9199;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC5'), U"\x9276") << desc_decomp_of(U'\xFAC5') << "\n  Line: FAC5;CJK COMPATIBILITY IDEOGRAPH-FAC5;Lo;0;L;9276;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC6'), U"\x967C") << desc_decomp_of(U'\xFAC6') << "\n  Line: FAC6;CJK COMPATIBILITY IDEOGRAPH-FAC6;Lo;0;L;967C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC7'), U"\x96E3") << desc_decomp_of(U'\xFAC7') << "\n  Line: FAC7;CJK COMPATIBILITY IDEOGRAPH-FAC7;Lo;0;L;96E3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC8'), U"\x9756") << desc_decomp_of(U'\xFAC8') << "\n  Line: FAC8;CJK COMPATIBILITY IDEOGRAPH-FAC8;Lo;0;L;9756;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAC9'), U"\x97DB") << desc_decomp_of(U'\xFAC9') << "\n  Line: FAC9;CJK COMPATIBILITY IDEOGRAPH-FAC9;Lo;0;L;97DB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFACA'), U"\x97FF") << desc_decomp_of(U'\xFACA') << "\n  Line: FACA;CJK COMPATIBILITY IDEOGRAPH-FACA;Lo;0;L;97FF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFACB'), U"\x980B") << desc_decomp_of(U'\xFACB') << "\n  Line: FACB;CJK COMPATIBILITY IDEOGRAPH-FACB;Lo;0;L;980B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFACC'), U"\x983B") << desc_decomp_of(U'\xFACC') << "\n  Line: FACC;CJK COMPATIBILITY IDEOGRAPH-FACC;Lo;0;L;983B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFACD'), U"\x9B12") << desc_decomp_of(U'\xFACD') << "\n  Line: FACD;CJK COMPATIBILITY IDEOGRAPH-FACD;Lo;0;L;9B12;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFACE'), U"\x9F9C") << desc_decomp_of(U'\xFACE') << "\n  Line: FACE;CJK COMPATIBILITY IDEOGRAPH-FACE;Lo;0;L;9F9C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFACF'), U"\x2284A") << desc_decomp_of(U'\xFACF') << "\n  Line: FACF;CJK COMPATIBILITY IDEOGRAPH-FACF;Lo;0;L;2284A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD0'), U"\x22844") << desc_decomp_of(U'\xFAD0') << "\n  Line: FAD0;CJK COMPATIBILITY IDEOGRAPH-FAD0;Lo;0;L;22844;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD1'), U"\x233D5") << desc_decomp_of(U'\xFAD1') << "\n  Line: FAD1;CJK COMPATIBILITY IDEOGRAPH-FAD1;Lo;0;L;233D5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD2'), U"\x3B9D") << desc_decomp_of(U'\xFAD2') << "\n  Line: FAD2;CJK COMPATIBILITY IDEOGRAPH-FAD2;Lo;0;L;3B9D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD3'), U"\x4018") << desc_decomp_of(U'\xFAD3') << "\n  Line: FAD3;CJK COMPATIBILITY IDEOGRAPH-FAD3;Lo;0;L;4018;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD4'), U"\x4039") << desc_decomp_of(U'\xFAD4') << "\n  Line: FAD4;CJK COMPATIBILITY IDEOGRAPH-FAD4;Lo;0;L;4039;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD5'), U"\x25249") << desc_decomp_of(U'\xFAD5') << "\n  Line: FAD5;CJK COMPATIBILITY IDEOGRAPH-FAD5;Lo;0;L;25249;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD6'), U"\x25CD0") << desc_decomp_of(U'\xFAD6') << "\n  Line: FAD6;CJK COMPATIBILITY IDEOGRAPH-FAD6;Lo;0;L;25CD0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD7'), U"\x27ED3") << desc_decomp_of(U'\xFAD7') << "\n  Line: FAD7;CJK COMPATIBILITY IDEOGRAPH-FAD7;Lo;0;L;27ED3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD8'), U"\x9F43") << desc_decomp_of(U'\xFAD8') << "\n  Line: FAD8;CJK COMPATIBILITY IDEOGRAPH-FAD8;Lo;0;L;9F43;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFAD9'), U"\x9F8E") << desc_decomp_of(U'\xFAD9') << "\n  Line: FAD9;CJK COMPATIBILITY IDEOGRAPH-FAD9;Lo;0;L;9F8E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB1D'), U"\x05D9\x05B4") << desc_decomp_of(U'\xFB1D') << "\n  Line: FB1D;HEBREW LETTER YOD WITH HIRIQ;Lo;0;R;05D9 05B4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB1F'), U"\x05F2\x05B7") << desc_decomp_of(U'\xFB1F') << "\n  Line: FB1F;HEBREW LIGATURE YIDDISH YOD YOD PATAH;Lo;0;R;05F2 05B7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB2A'), U"\x05E9\x05C1") << desc_decomp_of(U'\xFB2A') << "\n  Line: FB2A;HEBREW LETTER SHIN WITH SHIN DOT;Lo;0;R;05E9 05C1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB2B'), U"\x05E9\x05C2") << desc_decomp_of(U'\xFB2B') << "\n  Line: FB2B;HEBREW LETTER SHIN WITH SIN DOT;Lo;0;R;05E9 05C2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB2C'), U"\xFB49\x05C1") << desc_decomp_of(U'\xFB2C') << "\n  Line: FB2C;HEBREW LETTER SHIN WITH DAGESH AND SHIN DOT;Lo;0;R;FB49 05C1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB2D'), U"\xFB49\x05C2") << desc_decomp_of(U'\xFB2D') << "\n  Line: FB2D;HEBREW LETTER SHIN WITH DAGESH AND SIN DOT;Lo;0;R;FB49 05C2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB2E'), U"\x05D0\x05B7") << desc_decomp_of(U'\xFB2E') << "\n  Line: FB2E;HEBREW LETTER ALEF WITH PATAH;Lo;0;R;05D0 05B7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB2F'), U"\x05D0\x05B8") << desc_decomp_of(U'\xFB2F') << "\n  Line: FB2F;HEBREW LETTER ALEF WITH QAMATS;Lo;0;R;05D0 05B8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB30'), U"\x05D0\x05BC") << desc_decomp_of(U'\xFB30') << "\n  Line: FB30;HEBREW LETTER ALEF WITH MAPIQ;Lo;0;R;05D0 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB31'), U"\x05D1\x05BC") << desc_decomp_of(U'\xFB31') << "\n  Line: FB31;HEBREW LETTER BET WITH DAGESH;Lo;0;R;05D1 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB32'), U"\x05D2\x05BC") << desc_decomp_of(U'\xFB32') << "\n  Line: FB32;HEBREW LETTER GIMEL WITH DAGESH;Lo;0;R;05D2 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB33'), U"\x05D3\x05BC") << desc_decomp_of(U'\xFB33') << "\n  Line: FB33;HEBREW LETTER DALET WITH DAGESH;Lo;0;R;05D3 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB34'), U"\x05D4\x05BC") << desc_decomp_of(U'\xFB34') << "\n  Line: FB34;HEBREW LETTER HE WITH MAPIQ;Lo;0;R;05D4 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB35'), U"\x05D5\x05BC") << desc_decomp_of(U'\xFB35') << "\n  Line: FB35;HEBREW LETTER VAV WITH DAGESH;Lo;0;R;05D5 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB36'), U"\x05D6\x05BC") << desc_decomp_of(U'\xFB36') << "\n  Line: FB36;HEBREW LETTER ZAYIN WITH DAGESH;Lo;0;R;05D6 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB38'), U"\x05D8\x05BC") << desc_decomp_of(U'\xFB38') << "\n  Line: FB38;HEBREW LETTER TET WITH DAGESH;Lo;0;R;05D8 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB39'), U"\x05D9\x05BC") << desc_decomp_of(U'\xFB39') << "\n  Line: FB39;HEBREW LETTER YOD WITH DAGESH;Lo;0;R;05D9 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB3A'), U"\x05DA\x05BC") << desc_decomp_of(U'\xFB3A') << "\n  Line: FB3A;HEBREW LETTER FINAL KAF WITH DAGESH;Lo;0;R;05DA 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB3B'), U"\x05DB\x05BC") << desc_decomp_of(U'\xFB3B') << "\n  Line: FB3B;HEBREW LETTER KAF WITH DAGESH;Lo;0;R;05DB 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB3C'), U"\x05DC\x05BC") << desc_decomp_of(U'\xFB3C') << "\n  Line: FB3C;HEBREW LETTER LAMED WITH DAGESH;Lo;0;R;05DC 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB3E'), U"\x05DE\x05BC") << desc_decomp_of(U'\xFB3E') << "\n  Line: FB3E;HEBREW LETTER MEM WITH DAGESH;Lo;0;R;05DE 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB40'), U"\x05E0\x05BC") << desc_decomp_of(U'\xFB40') << "\n  Line: FB40;HEBREW LETTER NUN WITH DAGESH;Lo;0;R;05E0 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB41'), U"\x05E1\x05BC") << desc_decomp_of(U'\xFB41') << "\n  Line: FB41;HEBREW LETTER SAMEKH WITH DAGESH;Lo;0;R;05E1 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB43'), U"\x05E3\x05BC") << desc_decomp_of(U'\xFB43') << "\n  Line: FB43;HEBREW LETTER FINAL PE WITH DAGESH;Lo;0;R;05E3 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB44'), U"\x05E4\x05BC") << desc_decomp_of(U'\xFB44') << "\n  Line: FB44;HEBREW LETTER PE WITH DAGESH;Lo;0;R;05E4 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB46'), U"\x05E6\x05BC") << desc_decomp_of(U'\xFB46') << "\n  Line: FB46;HEBREW LETTER TSADI WITH DAGESH;Lo;0;R;05E6 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB47'), U"\x05E7\x05BC") << desc_decomp_of(U'\xFB47') << "\n  Line: FB47;HEBREW LETTER QOF WITH DAGESH;Lo;0;R;05E7 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB48'), U"\x05E8\x05BC") << desc_decomp_of(U'\xFB48') << "\n  Line: FB48;HEBREW LETTER RESH WITH DAGESH;Lo;0;R;05E8 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB49'), U"\x05E9\x05BC") << desc_decomp_of(U'\xFB49') << "\n  Line: FB49;HEBREW LETTER SHIN WITH DAGESH;Lo;0;R;05E9 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB4A'), U"\x05EA\x05BC") << desc_decomp_of(U'\xFB4A') << "\n  Line: FB4A;HEBREW LETTER TAV WITH DAGESH;Lo;0;R;05EA 05BC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB4B'), U"\x05D5\x05B9") << desc_decomp_of(U'\xFB4B') << "\n  Line: FB4B;HEBREW LETTER VAV WITH HOLAM;Lo;0;R;05D5 05B9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB4C'), U"\x05D1\x05BF") << desc_decomp_of(U'\xFB4C') << "\n  Line: FB4C;HEBREW LETTER BET WITH RAFE;Lo;0;R;05D1 05BF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB4D'), U"\x05DB\x05BF") << desc_decomp_of(U'\xFB4D') << "\n  Line: FB4D;HEBREW LETTER KAF WITH RAFE;Lo;0;R;05DB 05BF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\xFB4E'), U"\x05E4\x05BF") << desc_decomp_of(U'\xFB4E') << "\n  Line: FB4E;HEBREW LETTER PE WITH RAFE;Lo;0;R;05E4 05BF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1109A'), U"\x11099\x110BA") << desc_decomp_of(U'\x1109A') << "\n  Line: 1109A;KAITHI LETTER DDDHA;Lo;0;L;11099 110BA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1109C'), U"\x1109B\x110BA") << desc_decomp_of(U'\x1109C') << "\n  Line: 1109C;KAITHI LETTER RHA;Lo;0;L;1109B 110BA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x110AB'), U"\x110A5\x110BA") << desc_decomp_of(U'\x110AB') << "\n  Line: 110AB;KAITHI LETTER VA;Lo;0;L;110A5 110BA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1112E'), U"\x11131\x11127") << desc_decomp_of(U'\x1112E') << "\n  Line: 1112E;CHAKMA VOWEL SIGN O;Mn;0;NSM;11131 11127;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1112F'), U"\x11132\x11127") << desc_decomp_of(U'\x1112F') << "\n  Line: 1112F;CHAKMA VOWEL SIGN AU;Mn;0;NSM;11132 11127;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1134B'), U"\x11347\x1133E") << desc_decomp_of(U'\x1134B') << "\n  Line: 1134B;GRANTHA VOWEL SIGN OO;Mc;0;L;11347 1133E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1134C'), U"\x11347\x11357") << desc_decomp_of(U'\x1134C') << "\n  Line: 1134C;GRANTHA VOWEL SIGN AU;Mc;0;L;11347 11357;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x114BB'), U"\x114B9\x114BA") << desc_decomp_of(U'\x114BB') << "\n  Line: 114BB;TIRHUTA VOWEL SIGN AI;Mc;0;L;114B9 114BA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x114BC'), U"\x114B9\x114B0") << desc_decomp_of(U'\x114BC') << "\n  Line: 114BC;TIRHUTA VOWEL SIGN O;Mc;0;L;114B9 114B0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x114BE'), U"\x114B9\x114BD") << desc_decomp_of(U'\x114BE') << "\n  Line: 114BE;TIRHUTA VOWEL SIGN AU;Mc;0;L;114B9 114BD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x115BA'), U"\x115B8\x115AF") << desc_decomp_of(U'\x115BA') << "\n  Line: 115BA;SIDDHAM VOWEL SIGN O;Mc;0;L;115B8 115AF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x115BB'), U"\x115B9\x115AF") << desc_decomp_of(U'\x115BB') << "\n  Line: 115BB;SIDDHAM VOWEL SIGN AU;Mc;0;L;115B9 115AF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x11938'), U"\x11935\x11930") << desc_decomp_of(U'\x11938') << "\n  Line: 11938;DIVES AKURU VOWEL SIGN O;Mc;0;L;11935 11930;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D15E'), U"\x1D157\x1D165") << desc_decomp_of(U'\x1D15E') << "\n  Line: 1D15E;MUSICAL SYMBOL HALF NOTE;So;0;L;1D157 1D165;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D15F'), U"\x1D158\x1D165") << desc_decomp_of(U'\x1D15F') << "\n  Line: 1D15F;MUSICAL SYMBOL QUARTER NOTE;So;0;L;1D158 1D165;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D160'), U"\x1D15F\x1D16E") << desc_decomp_of(U'\x1D160') << "\n  Line: 1D160;MUSICAL SYMBOL EIGHTH NOTE;So;0;L;1D15F 1D16E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D161'), U"\x1D15F\x1D16F") << desc_decomp_of(U'\x1D161') << "\n  Line: 1D161;MUSICAL SYMBOL SIXTEENTH NOTE;So;0;L;1D15F 1D16F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D162'), U"\x1D15F\x1D170") << desc_decomp_of(U'\x1D162') << "\n  Line: 1D162;MUSICAL SYMBOL THIRTY-SECOND NOTE;So;0;L;1D15F 1D170;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D163'), U"\x1D15F\x1D171") << desc_decomp_of(U'\x1D163') << "\n  Line: 1D163;MUSICAL SYMBOL SIXTY-FOURTH NOTE;So;0;L;1D15F 1D171;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D164'), U"\x1D15F\x1D172") << desc_decomp_of(U'\x1D164') << "\n  Line: 1D164;MUSICAL SYMBOL ONE HUNDRED TWENTY-EIGHTH NOTE;So;0;L;1D15F 1D172;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D1BB'), U"\x1D1B9\x1D165") << desc_decomp_of(U'\x1D1BB') << "\n  Line: 1D1BB;MUSICAL SYMBOL MINIMA;So;0;L;1D1B9 1D165;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D1BC'), U"\x1D1BA\x1D165") << desc_decomp_of(U'\x1D1BC') << "\n  Line: 1D1BC;MUSICAL SYMBOL MINIMA BLACK;So;0;L;1D1BA 1D165;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D1BD'), U"\x1D1BB\x1D16E") << desc_decomp_of(U'\x1D1BD') << "\n  Line: 1D1BD;MUSICAL SYMBOL SEMIMINIMA WHITE;So;0;L;1D1BB 1D16E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D1BE'), U"\x1D1BC\x1D16E") << desc_decomp_of(U'\x1D1BE') << "\n  Line: 1D1BE;MUSICAL SYMBOL SEMIMINIMA BLACK;So;0;L;1D1BC 1D16E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D1BF'), U"\x1D1BB\x1D16F") << desc_decomp_of(U'\x1D1BF') << "\n  Line: 1D1BF;MUSICAL SYMBOL FUSA WHITE;So;0;L;1D1BB 1D16F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1D1C0'), U"\x1D1BC\x1D16F") << desc_decomp_of(U'\x1D1C0') << "\n  Line: 1D1C0;MUSICAL SYMBOL FUSA BLACK;So;0;L;1D1BC 1D16F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F800'), U"\x4E3D") << desc_decomp_of(U'\x2F800') << "\n  Line: 2F800;CJK COMPATIBILITY IDEOGRAPH-2F800;Lo;0;L;4E3D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F801'), U"\x4E38") << desc_decomp_of(U'\x2F801') << "\n  Line: 2F801;CJK COMPATIBILITY IDEOGRAPH-2F801;Lo;0;L;4E38;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F802'), U"\x4E41") << desc_decomp_of(U'\x2F802') << "\n  Line: 2F802;CJK COMPATIBILITY IDEOGRAPH-2F802;Lo;0;L;4E41;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F803'), U"\x20122") << desc_decomp_of(U'\x2F803') << "\n  Line: 2F803;CJK COMPATIBILITY IDEOGRAPH-2F803;Lo;0;L;20122;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F804'), U"\x4F60") << desc_decomp_of(U'\x2F804') << "\n  Line: 2F804;CJK COMPATIBILITY IDEOGRAPH-2F804;Lo;0;L;4F60;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F805'), U"\x4FAE") << desc_decomp_of(U'\x2F805') << "\n  Line: 2F805;CJK COMPATIBILITY IDEOGRAPH-2F805;Lo;0;L;4FAE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F806'), U"\x4FBB") << desc_decomp_of(U'\x2F806') << "\n  Line: 2F806;CJK COMPATIBILITY IDEOGRAPH-2F806;Lo;0;L;4FBB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F807'), U"\x5002") << desc_decomp_of(U'\x2F807') << "\n  Line: 2F807;CJK COMPATIBILITY IDEOGRAPH-2F807;Lo;0;L;5002;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F808'), U"\x507A") << desc_decomp_of(U'\x2F808') << "\n  Line: 2F808;CJK COMPATIBILITY IDEOGRAPH-2F808;Lo;0;L;507A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F809'), U"\x5099") << desc_decomp_of(U'\x2F809') << "\n  Line: 2F809;CJK COMPATIBILITY IDEOGRAPH-2F809;Lo;0;L;5099;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F80A'), U"\x50E7") << desc_decomp_of(U'\x2F80A') << "\n  Line: 2F80A;CJK COMPATIBILITY IDEOGRAPH-2F80A;Lo;0;L;50E7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F80B'), U"\x50CF") << desc_decomp_of(U'\x2F80B') << "\n  Line: 2F80B;CJK COMPATIBILITY IDEOGRAPH-2F80B;Lo;0;L;50CF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F80C'), U"\x349E") << desc_decomp_of(U'\x2F80C') << "\n  Line: 2F80C;CJK COMPATIBILITY IDEOGRAPH-2F80C;Lo;0;L;349E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F80D'), U"\x2063A") << desc_decomp_of(U'\x2F80D') << "\n  Line: 2F80D;CJK COMPATIBILITY IDEOGRAPH-2F80D;Lo;0;L;2063A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F80E'), U"\x514D") << desc_decomp_of(U'\x2F80E') << "\n  Line: 2F80E;CJK COMPATIBILITY IDEOGRAPH-2F80E;Lo;0;L;514D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F80F'), U"\x5154") << desc_decomp_of(U'\x2F80F') << "\n  Line: 2F80F;CJK COMPATIBILITY IDEOGRAPH-2F80F;Lo;0;L;5154;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F810'), U"\x5164") << desc_decomp_of(U'\x2F810') << "\n  Line: 2F810;CJK COMPATIBILITY IDEOGRAPH-2F810;Lo;0;L;5164;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F811'), U"\x5177") << desc_decomp_of(U'\x2F811') << "\n  Line: 2F811;CJK COMPATIBILITY IDEOGRAPH-2F811;Lo;0;L;5177;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F812'), U"\x2051C") << desc_decomp_of(U'\x2F812') << "\n  Line: 2F812;CJK COMPATIBILITY IDEOGRAPH-2F812;Lo;0;L;2051C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F813'), U"\x34B9") << desc_decomp_of(U'\x2F813') << "\n  Line: 2F813;CJK COMPATIBILITY IDEOGRAPH-2F813;Lo;0;L;34B9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F814'), U"\x5167") << desc_decomp_of(U'\x2F814') << "\n  Line: 2F814;CJK COMPATIBILITY IDEOGRAPH-2F814;Lo;0;L;5167;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F815'), U"\x518D") << desc_decomp_of(U'\x2F815') << "\n  Line: 2F815;CJK COMPATIBILITY IDEOGRAPH-2F815;Lo;0;L;518D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F816'), U"\x2054B") << desc_decomp_of(U'\x2F816') << "\n  Line: 2F816;CJK COMPATIBILITY IDEOGRAPH-2F816;Lo;0;L;2054B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F817'), U"\x5197") << desc_decomp_of(U'\x2F817') << "\n  Line: 2F817;CJK COMPATIBILITY IDEOGRAPH-2F817;Lo;0;L;5197;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F818'), U"\x51A4") << desc_decomp_of(U'\x2F818') << "\n  Line: 2F818;CJK COMPATIBILITY IDEOGRAPH-2F818;Lo;0;L;51A4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F819'), U"\x4ECC") << desc_decomp_of(U'\x2F819') << "\n  Line: 2F819;CJK COMPATIBILITY IDEOGRAPH-2F819;Lo;0;L;4ECC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F81A'), U"\x51AC") << desc_decomp_of(U'\x2F81A') << "\n  Line: 2F81A;CJK COMPATIBILITY IDEOGRAPH-2F81A;Lo;0;L;51AC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F81B'), U"\x51B5") << desc_decomp_of(U'\x2F81B') << "\n  Line: 2F81B;CJK COMPATIBILITY IDEOGRAPH-2F81B;Lo;0;L;51B5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F81C'), U"\x291DF") << desc_decomp_of(U'\x2F81C') << "\n  Line: 2F81C;CJK COMPATIBILITY IDEOGRAPH-2F81C;Lo;0;L;291DF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F81D'), U"\x51F5") << desc_decomp_of(U'\x2F81D') << "\n  Line: 2F81D;CJK COMPATIBILITY IDEOGRAPH-2F81D;Lo;0;L;51F5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F81E'), U"\x5203") << desc_decomp_of(U'\x2F81E') << "\n  Line: 2F81E;CJK COMPATIBILITY IDEOGRAPH-2F81E;Lo;0;L;5203;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F81F'), U"\x34DF") << desc_decomp_of(U'\x2F81F') << "\n  Line: 2F81F;CJK COMPATIBILITY IDEOGRAPH-2F81F;Lo;0;L;34DF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F820'), U"\x523B") << desc_decomp_of(U'\x2F820') << "\n  Line: 2F820;CJK COMPATIBILITY IDEOGRAPH-2F820;Lo;0;L;523B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F821'), U"\x5246") << desc_decomp_of(U'\x2F821') << "\n  Line: 2F821;CJK COMPATIBILITY IDEOGRAPH-2F821;Lo;0;L;5246;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F822'), U"\x5272") << desc_decomp_of(U'\x2F822') << "\n  Line: 2F822;CJK COMPATIBILITY IDEOGRAPH-2F822;Lo;0;L;5272;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F823'), U"\x5277") << desc_decomp_of(U'\x2F823') << "\n  Line: 2F823;CJK COMPATIBILITY IDEOGRAPH-2F823;Lo;0;L;5277;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F824'), U"\x3515") << desc_decomp_of(U'\x2F824') << "\n  Line: 2F824;CJK COMPATIBILITY IDEOGRAPH-2F824;Lo;0;L;3515;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F825'), U"\x52C7") << desc_decomp_of(U'\x2F825') << "\n  Line: 2F825;CJK COMPATIBILITY IDEOGRAPH-2F825;Lo;0;L;52C7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F826'), U"\x52C9") << desc_decomp_of(U'\x2F826') << "\n  Line: 2F826;CJK COMPATIBILITY IDEOGRAPH-2F826;Lo;0;L;52C9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F827'), U"\x52E4") << desc_decomp_of(U'\x2F827') << "\n  Line: 2F827;CJK COMPATIBILITY IDEOGRAPH-2F827;Lo;0;L;52E4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F828'), U"\x52FA") << desc_decomp_of(U'\x2F828') << "\n  Line: 2F828;CJK COMPATIBILITY IDEOGRAPH-2F828;Lo;0;L;52FA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F829'), U"\x5305") << desc_decomp_of(U'\x2F829') << "\n  Line: 2F829;CJK COMPATIBILITY IDEOGRAPH-2F829;Lo;0;L;5305;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F82A'), U"\x5306") << desc_decomp_of(U'\x2F82A') << "\n  Line: 2F82A;CJK COMPATIBILITY IDEOGRAPH-2F82A;Lo;0;L;5306;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F82B'), U"\x5317") << desc_decomp_of(U'\x2F82B') << "\n  Line: 2F82B;CJK COMPATIBILITY IDEOGRAPH-2F82B;Lo;0;L;5317;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F82C'), U"\x5349") << desc_decomp_of(U'\x2F82C') << "\n  Line: 2F82C;CJK COMPATIBILITY IDEOGRAPH-2F82C;Lo;0;L;5349;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F82D'), U"\x5351") << desc_decomp_of(U'\x2F82D') << "\n  Line: 2F82D;CJK COMPATIBILITY IDEOGRAPH-2F82D;Lo;0;L;5351;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F82E'), U"\x535A") << desc_decomp_of(U'\x2F82E') << "\n  Line: 2F82E;CJK COMPATIBILITY IDEOGRAPH-2F82E;Lo;0;L;535A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F82F'), U"\x5373") << desc_decomp_of(U'\x2F82F') << "\n  Line: 2F82F;CJK COMPATIBILITY IDEOGRAPH-2F82F;Lo;0;L;5373;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F830'), U"\x537D") << desc_decomp_of(U'\x2F830') << "\n  Line: 2F830;CJK COMPATIBILITY IDEOGRAPH-2F830;Lo;0;L;537D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F831'), U"\x537F") << desc_decomp_of(U'\x2F831') << "\n  Line: 2F831;CJK COMPATIBILITY IDEOGRAPH-2F831;Lo;0;L;537F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F832'), U"\x537F") << desc_decomp_of(U'\x2F832') << "\n  Line: 2F832;CJK COMPATIBILITY IDEOGRAPH-2F832;Lo;0;L;537F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F833'), U"\x537F") << desc_decomp_of(U'\x2F833') << "\n  Line: 2F833;CJK COMPATIBILITY IDEOGRAPH-2F833;Lo;0;L;537F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F834'), U"\x20A2C") << desc_decomp_of(U'\x2F834') << "\n  Line: 2F834;CJK COMPATIBILITY IDEOGRAPH-2F834;Lo;0;L;20A2C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F835'), U"\x7070") << desc_decomp_of(U'\x2F835') << "\n  Line: 2F835;CJK COMPATIBILITY IDEOGRAPH-2F835;Lo;0;L;7070;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F836'), U"\x53CA") << desc_decomp_of(U'\x2F836') << "\n  Line: 2F836;CJK COMPATIBILITY IDEOGRAPH-2F836;Lo;0;L;53CA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F837'), U"\x53DF") << desc_decomp_of(U'\x2F837') << "\n  Line: 2F837;CJK COMPATIBILITY IDEOGRAPH-2F837;Lo;0;L;53DF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F838'), U"\x20B63") << desc_decomp_of(U'\x2F838') << "\n  Line: 2F838;CJK COMPATIBILITY IDEOGRAPH-2F838;Lo;0;L;20B63;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F839'), U"\x53EB") << desc_decomp_of(U'\x2F839') << "\n  Line: 2F839;CJK COMPATIBILITY IDEOGRAPH-2F839;Lo;0;L;53EB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F83A'), U"\x53F1") << desc_decomp_of(U'\x2F83A') << "\n  Line: 2F83A;CJK COMPATIBILITY IDEOGRAPH-2F83A;Lo;0;L;53F1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F83B'), U"\x5406") << desc_decomp_of(U'\x2F83B') << "\n  Line: 2F83B;CJK COMPATIBILITY IDEOGRAPH-2F83B;Lo;0;L;5406;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F83C'), U"\x549E") << desc_decomp_of(U'\x2F83C') << "\n  Line: 2F83C;CJK COMPATIBILITY IDEOGRAPH-2F83C;Lo;0;L;549E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F83D'), U"\x5438") << desc_decomp_of(U'\x2F83D') << "\n  Line: 2F83D;CJK COMPATIBILITY IDEOGRAPH-2F83D;Lo;0;L;5438;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F83E'), U"\x5448") << desc_decomp_of(U'\x2F83E') << "\n  Line: 2F83E;CJK COMPATIBILITY IDEOGRAPH-2F83E;Lo;0;L;5448;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F83F'), U"\x5468") << desc_decomp_of(U'\x2F83F') << "\n  Line: 2F83F;CJK COMPATIBILITY IDEOGRAPH-2F83F;Lo;0;L;5468;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F840'), U"\x54A2") << desc_decomp_of(U'\x2F840') << "\n  Line: 2F840;CJK COMPATIBILITY IDEOGRAPH-2F840;Lo;0;L;54A2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F841'), U"\x54F6") << desc_decomp_of(U'\x2F841') << "\n  Line: 2F841;CJK COMPATIBILITY IDEOGRAPH-2F841;Lo;0;L;54F6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F842'), U"\x5510") << desc_decomp_of(U'\x2F842') << "\n  Line: 2F842;CJK COMPATIBILITY IDEOGRAPH-2F842;Lo;0;L;5510;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F843'), U"\x5553") << desc_decomp_of(U'\x2F843') << "\n  Line: 2F843;CJK COMPATIBILITY IDEOGRAPH-2F843;Lo;0;L;5553;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F844'), U"\x5563") << desc_decomp_of(U'\x2F844') << "\n  Line: 2F844;CJK COMPATIBILITY IDEOGRAPH-2F844;Lo;0;L;5563;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F845'), U"\x5584") << desc_decomp_of(U'\x2F845') << "\n  Line: 2F845;CJK COMPATIBILITY IDEOGRAPH-2F845;Lo;0;L;5584;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F846'), U"\x5584") << desc_decomp_of(U'\x2F846') << "\n  Line: 2F846;CJK COMPATIBILITY IDEOGRAPH-2F846;Lo;0;L;5584;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F847'), U"\x5599") << desc_decomp_of(U'\x2F847') << "\n  Line: 2F847;CJK COMPATIBILITY IDEOGRAPH-2F847;Lo;0;L;5599;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F848'), U"\x55AB") << desc_decomp_of(U'\x2F848') << "\n  Line: 2F848;CJK COMPATIBILITY IDEOGRAPH-2F848;Lo;0;L;55AB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F849'), U"\x55B3") << desc_decomp_of(U'\x2F849') << "\n  Line: 2F849;CJK COMPATIBILITY IDEOGRAPH-2F849;Lo;0;L;55B3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F84A'), U"\x55C2") << desc_decomp_of(U'\x2F84A') << "\n  Line: 2F84A;CJK COMPATIBILITY IDEOGRAPH-2F84A;Lo;0;L;55C2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F84B'), U"\x5716") << desc_decomp_of(U'\x2F84B') << "\n  Line: 2F84B;CJK COMPATIBILITY IDEOGRAPH-2F84B;Lo;0;L;5716;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F84C'), U"\x5606") << desc_decomp_of(U'\x2F84C') << "\n  Line: 2F84C;CJK COMPATIBILITY IDEOGRAPH-2F84C;Lo;0;L;5606;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F84D'), U"\x5717") << desc_decomp_of(U'\x2F84D') << "\n  Line: 2F84D;CJK COMPATIBILITY IDEOGRAPH-2F84D;Lo;0;L;5717;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F84E'), U"\x5651") << desc_decomp_of(U'\x2F84E') << "\n  Line: 2F84E;CJK COMPATIBILITY IDEOGRAPH-2F84E;Lo;0;L;5651;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F84F'), U"\x5674") << desc_decomp_of(U'\x2F84F') << "\n  Line: 2F84F;CJK COMPATIBILITY IDEOGRAPH-2F84F;Lo;0;L;5674;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F850'), U"\x5207") << desc_decomp_of(U'\x2F850') << "\n  Line: 2F850;CJK COMPATIBILITY IDEOGRAPH-2F850;Lo;0;L;5207;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F851'), U"\x58EE") << desc_decomp_of(U'\x2F851') << "\n  Line: 2F851;CJK COMPATIBILITY IDEOGRAPH-2F851;Lo;0;L;58EE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F852'), U"\x57CE") << desc_decomp_of(U'\x2F852') << "\n  Line: 2F852;CJK COMPATIBILITY IDEOGRAPH-2F852;Lo;0;L;57CE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F853'), U"\x57F4") << desc_decomp_of(U'\x2F853') << "\n  Line: 2F853;CJK COMPATIBILITY IDEOGRAPH-2F853;Lo;0;L;57F4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F854'), U"\x580D") << desc_decomp_of(U'\x2F854') << "\n  Line: 2F854;CJK COMPATIBILITY IDEOGRAPH-2F854;Lo;0;L;580D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F855'), U"\x578B") << desc_decomp_of(U'\x2F855') << "\n  Line: 2F855;CJK COMPATIBILITY IDEOGRAPH-2F855;Lo;0;L;578B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F856'), U"\x5832") << desc_decomp_of(U'\x2F856') << "\n  Line: 2F856;CJK COMPATIBILITY IDEOGRAPH-2F856;Lo;0;L;5832;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F857'), U"\x5831") << desc_decomp_of(U'\x2F857') << "\n  Line: 2F857;CJK COMPATIBILITY IDEOGRAPH-2F857;Lo;0;L;5831;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F858'), U"\x58AC") << desc_decomp_of(U'\x2F858') << "\n  Line: 2F858;CJK COMPATIBILITY IDEOGRAPH-2F858;Lo;0;L;58AC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F859'), U"\x214E4") << desc_decomp_of(U'\x2F859') << "\n  Line: 2F859;CJK COMPATIBILITY IDEOGRAPH-2F859;Lo;0;L;214E4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F85A'), U"\x58F2") << desc_decomp_of(U'\x2F85A') << "\n  Line: 2F85A;CJK COMPATIBILITY IDEOGRAPH-2F85A;Lo;0;L;58F2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F85B'), U"\x58F7") << desc_decomp_of(U'\x2F85B') << "\n  Line: 2F85B;CJK COMPATIBILITY IDEOGRAPH-2F85B;Lo;0;L;58F7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F85C'), U"\x5906") << desc_decomp_of(U'\x2F85C') << "\n  Line: 2F85C;CJK COMPATIBILITY IDEOGRAPH-2F85C;Lo;0;L;5906;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F85D'), U"\x591A") << desc_decomp_of(U'\x2F85D') << "\n  Line: 2F85D;CJK COMPATIBILITY IDEOGRAPH-2F85D;Lo;0;L;591A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F85E'), U"\x5922") << desc_decomp_of(U'\x2F85E') << "\n  Line: 2F85E;CJK COMPATIBILITY IDEOGRAPH-2F85E;Lo;0;L;5922;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F85F'), U"\x5962") << desc_decomp_of(U'\x2F85F') << "\n  Line: 2F85F;CJK COMPATIBILITY IDEOGRAPH-2F85F;Lo;0;L;5962;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F860'), U"\x216A8") << desc_decomp_of(U'\x2F860') << "\n  Line: 2F860;CJK COMPATIBILITY IDEOGRAPH-2F860;Lo;0;L;216A8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F861'), U"\x216EA") << desc_decomp_of(U'\x2F861') << "\n  Line: 2F861;CJK COMPATIBILITY IDEOGRAPH-2F861;Lo;0;L;216EA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F862'), U"\x59EC") << desc_decomp_of(U'\x2F862') << "\n  Line: 2F862;CJK COMPATIBILITY IDEOGRAPH-2F862;Lo;0;L;59EC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F863'), U"\x5A1B") << desc_decomp_of(U'\x2F863') << "\n  Line: 2F863;CJK COMPATIBILITY IDEOGRAPH-2F863;Lo;0;L;5A1B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F864'), U"\x5A27") << desc_decomp_of(U'\x2F864') << "\n  Line: 2F864;CJK COMPATIBILITY IDEOGRAPH-2F864;Lo;0;L;5A27;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F865'), U"\x59D8") << desc_decomp_of(U'\x2F865') << "\n  Line: 2F865;CJK COMPATIBILITY IDEOGRAPH-2F865;Lo;0;L;59D8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F866'), U"\x5A66") << desc_decomp_of(U'\x2F866') << "\n  Line: 2F866;CJK COMPATIBILITY IDEOGRAPH-2F866;Lo;0;L;5A66;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F867'), U"\x36EE") << desc_decomp_of(U'\x2F867') << "\n  Line: 2F867;CJK COMPATIBILITY IDEOGRAPH-2F867;Lo;0;L;36EE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F868'), U"\x36FC") << desc_decomp_of(U'\x2F868') << "\n  Line: 2F868;CJK COMPATIBILITY IDEOGRAPH-2F868;Lo;0;L;36FC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F869'), U"\x5B08") << desc_decomp_of(U'\x2F869') << "\n  Line: 2F869;CJK COMPATIBILITY IDEOGRAPH-2F869;Lo;0;L;5B08;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F86A'), U"\x5B3E") << desc_decomp_of(U'\x2F86A') << "\n  Line: 2F86A;CJK COMPATIBILITY IDEOGRAPH-2F86A;Lo;0;L;5B3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F86B'), U"\x5B3E") << desc_decomp_of(U'\x2F86B') << "\n  Line: 2F86B;CJK COMPATIBILITY IDEOGRAPH-2F86B;Lo;0;L;5B3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F86C'), U"\x219C8") << desc_decomp_of(U'\x2F86C') << "\n  Line: 2F86C;CJK COMPATIBILITY IDEOGRAPH-2F86C;Lo;0;L;219C8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F86D'), U"\x5BC3") << desc_decomp_of(U'\x2F86D') << "\n  Line: 2F86D;CJK COMPATIBILITY IDEOGRAPH-2F86D;Lo;0;L;5BC3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F86E'), U"\x5BD8") << desc_decomp_of(U'\x2F86E') << "\n  Line: 2F86E;CJK COMPATIBILITY IDEOGRAPH-2F86E;Lo;0;L;5BD8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F86F'), U"\x5BE7") << desc_decomp_of(U'\x2F86F') << "\n  Line: 2F86F;CJK COMPATIBILITY IDEOGRAPH-2F86F;Lo;0;L;5BE7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F870'), U"\x5BF3") << desc_decomp_of(U'\x2F870') << "\n  Line: 2F870;CJK COMPATIBILITY IDEOGRAPH-2F870;Lo;0;L;5BF3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F871'), U"\x21B18") << desc_decomp_of(U'\x2F871') << "\n  Line: 2F871;CJK COMPATIBILITY IDEOGRAPH-2F871;Lo;0;L;21B18;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F872'), U"\x5BFF") << desc_decomp_of(U'\x2F872') << "\n  Line: 2F872;CJK COMPATIBILITY IDEOGRAPH-2F872;Lo;0;L;5BFF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F873'), U"\x5C06") << desc_decomp_of(U'\x2F873') << "\n  Line: 2F873;CJK COMPATIBILITY IDEOGRAPH-2F873;Lo;0;L;5C06;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F874'), U"\x5F53") << desc_decomp_of(U'\x2F874') << "\n  Line: 2F874;CJK COMPATIBILITY IDEOGRAPH-2F874;Lo;0;L;5F53;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F875'), U"\x5C22") << desc_decomp_of(U'\x2F875') << "\n  Line: 2F875;CJK COMPATIBILITY IDEOGRAPH-2F875;Lo;0;L;5C22;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F876'), U"\x3781") << desc_decomp_of(U'\x2F876') << "\n  Line: 2F876;CJK COMPATIBILITY IDEOGRAPH-2F876;Lo;0;L;3781;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F877'), U"\x5C60") << desc_decomp_of(U'\x2F877') << "\n  Line: 2F877;CJK COMPATIBILITY IDEOGRAPH-2F877;Lo;0;L;5C60;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F878'), U"\x5C6E") << desc_decomp_of(U'\x2F878') << "\n  Line: 2F878;CJK COMPATIBILITY IDEOGRAPH-2F878;Lo;0;L;5C6E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F879'), U"\x5CC0") << desc_decomp_of(U'\x2F879') << "\n  Line: 2F879;CJK COMPATIBILITY IDEOGRAPH-2F879;Lo;0;L;5CC0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F87A'), U"\x5C8D") << desc_decomp_of(U'\x2F87A') << "\n  Line: 2F87A;CJK COMPATIBILITY IDEOGRAPH-2F87A;Lo;0;L;5C8D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F87B'), U"\x21DE4") << desc_decomp_of(U'\x2F87B') << "\n  Line: 2F87B;CJK COMPATIBILITY IDEOGRAPH-2F87B;Lo;0;L;21DE4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F87C'), U"\x5D43") << desc_decomp_of(U'\x2F87C') << "\n  Line: 2F87C;CJK COMPATIBILITY IDEOGRAPH-2F87C;Lo;0;L;5D43;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F87D'), U"\x21DE6") << desc_decomp_of(U'\x2F87D') << "\n  Line: 2F87D;CJK COMPATIBILITY IDEOGRAPH-2F87D;Lo;0;L;21DE6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F87E'), U"\x5D6E") << desc_decomp_of(U'\x2F87E') << "\n  Line: 2F87E;CJK COMPATIBILITY IDEOGRAPH-2F87E;Lo;0;L;5D6E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F87F'), U"\x5D6B") << desc_decomp_of(U'\x2F87F') << "\n  Line: 2F87F;CJK COMPATIBILITY IDEOGRAPH-2F87F;Lo;0;L;5D6B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F880'), U"\x5D7C") << desc_decomp_of(U'\x2F880') << "\n  Line: 2F880;CJK COMPATIBILITY IDEOGRAPH-2F880;Lo;0;L;5D7C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F881'), U"\x5DE1") << desc_decomp_of(U'\x2F881') << "\n  Line: 2F881;CJK COMPATIBILITY IDEOGRAPH-2F881;Lo;0;L;5DE1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F882'), U"\x5DE2") << desc_decomp_of(U'\x2F882') << "\n  Line: 2F882;CJK COMPATIBILITY IDEOGRAPH-2F882;Lo;0;L;5DE2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F883'), U"\x382F") << desc_decomp_of(U'\x2F883') << "\n  Line: 2F883;CJK COMPATIBILITY IDEOGRAPH-2F883;Lo;0;L;382F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F884'), U"\x5DFD") << desc_decomp_of(U'\x2F884') << "\n  Line: 2F884;CJK COMPATIBILITY IDEOGRAPH-2F884;Lo;0;L;5DFD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F885'), U"\x5E28") << desc_decomp_of(U'\x2F885') << "\n  Line: 2F885;CJK COMPATIBILITY IDEOGRAPH-2F885;Lo;0;L;5E28;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F886'), U"\x5E3D") << desc_decomp_of(U'\x2F886') << "\n  Line: 2F886;CJK COMPATIBILITY IDEOGRAPH-2F886;Lo;0;L;5E3D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F887'), U"\x5E69") << desc_decomp_of(U'\x2F887') << "\n  Line: 2F887;CJK COMPATIBILITY IDEOGRAPH-2F887;Lo;0;L;5E69;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F888'), U"\x3862") << desc_decomp_of(U'\x2F888') << "\n  Line: 2F888;CJK COMPATIBILITY IDEOGRAPH-2F888;Lo;0;L;3862;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F889'), U"\x22183") << desc_decomp_of(U'\x2F889') << "\n  Line: 2F889;CJK COMPATIBILITY IDEOGRAPH-2F889;Lo;0;L;22183;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F88A'), U"\x387C") << desc_decomp_of(U'\x2F88A') << "\n  Line: 2F88A;CJK COMPATIBILITY IDEOGRAPH-2F88A;Lo;0;L;387C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F88B'), U"\x5EB0") << desc_decomp_of(U'\x2F88B') << "\n  Line: 2F88B;CJK COMPATIBILITY IDEOGRAPH-2F88B;Lo;0;L;5EB0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F88C'), U"\x5EB3") << desc_decomp_of(U'\x2F88C') << "\n  Line: 2F88C;CJK COMPATIBILITY IDEOGRAPH-2F88C;Lo;0;L;5EB3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F88D'), U"\x5EB6") << desc_decomp_of(U'\x2F88D') << "\n  Line: 2F88D;CJK COMPATIBILITY IDEOGRAPH-2F88D;Lo;0;L;5EB6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F88E'), U"\x5ECA") << desc_decomp_of(U'\x2F88E') << "\n  Line: 2F88E;CJK COMPATIBILITY IDEOGRAPH-2F88E;Lo;0;L;5ECA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F88F'), U"\x2A392") << desc_decomp_of(U'\x2F88F') << "\n  Line: 2F88F;CJK COMPATIBILITY IDEOGRAPH-2F88F;Lo;0;L;2A392;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F890'), U"\x5EFE") << desc_decomp_of(U'\x2F890') << "\n  Line: 2F890;CJK COMPATIBILITY IDEOGRAPH-2F890;Lo;0;L;5EFE;;;9;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F891'), U"\x22331") << desc_decomp_of(U'\x2F891') << "\n  Line: 2F891;CJK COMPATIBILITY IDEOGRAPH-2F891;Lo;0;L;22331;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F892'), U"\x22331") << desc_decomp_of(U'\x2F892') << "\n  Line: 2F892;CJK COMPATIBILITY IDEOGRAPH-2F892;Lo;0;L;22331;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F893'), U"\x8201") << desc_decomp_of(U'\x2F893') << "\n  Line: 2F893;CJK COMPATIBILITY IDEOGRAPH-2F893;Lo;0;L;8201;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F894'), U"\x5F22") << desc_decomp_of(U'\x2F894') << "\n  Line: 2F894;CJK COMPATIBILITY IDEOGRAPH-2F894;Lo;0;L;5F22;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F895'), U"\x5F22") << desc_decomp_of(U'\x2F895') << "\n  Line: 2F895;CJK COMPATIBILITY IDEOGRAPH-2F895;Lo;0;L;5F22;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F896'), U"\x38C7") << desc_decomp_of(U'\x2F896') << "\n  Line: 2F896;CJK COMPATIBILITY IDEOGRAPH-2F896;Lo;0;L;38C7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F897'), U"\x232B8") << desc_decomp_of(U'\x2F897') << "\n  Line: 2F897;CJK COMPATIBILITY IDEOGRAPH-2F897;Lo;0;L;232B8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F898'), U"\x261DA") << desc_decomp_of(U'\x2F898') << "\n  Line: 2F898;CJK COMPATIBILITY IDEOGRAPH-2F898;Lo;0;L;261DA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F899'), U"\x5F62") << desc_decomp_of(U'\x2F899') << "\n  Line: 2F899;CJK COMPATIBILITY IDEOGRAPH-2F899;Lo;0;L;5F62;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F89A'), U"\x5F6B") << desc_decomp_of(U'\x2F89A') << "\n  Line: 2F89A;CJK COMPATIBILITY IDEOGRAPH-2F89A;Lo;0;L;5F6B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F89B'), U"\x38E3") << desc_decomp_of(U'\x2F89B') << "\n  Line: 2F89B;CJK COMPATIBILITY IDEOGRAPH-2F89B;Lo;0;L;38E3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F89C'), U"\x5F9A") << desc_decomp_of(U'\x2F89C') << "\n  Line: 2F89C;CJK COMPATIBILITY IDEOGRAPH-2F89C;Lo;0;L;5F9A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F89D'), U"\x5FCD") << desc_decomp_of(U'\x2F89D') << "\n  Line: 2F89D;CJK COMPATIBILITY IDEOGRAPH-2F89D;Lo;0;L;5FCD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F89E'), U"\x5FD7") << desc_decomp_of(U'\x2F89E') << "\n  Line: 2F89E;CJK COMPATIBILITY IDEOGRAPH-2F89E;Lo;0;L;5FD7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F89F'), U"\x5FF9") << desc_decomp_of(U'\x2F89F') << "\n  Line: 2F89F;CJK COMPATIBILITY IDEOGRAPH-2F89F;Lo;0;L;5FF9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A0'), U"\x6081") << desc_decomp_of(U'\x2F8A0') << "\n  Line: 2F8A0;CJK COMPATIBILITY IDEOGRAPH-2F8A0;Lo;0;L;6081;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A1'), U"\x393A") << desc_decomp_of(U'\x2F8A1') << "\n  Line: 2F8A1;CJK COMPATIBILITY IDEOGRAPH-2F8A1;Lo;0;L;393A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A2'), U"\x391C") << desc_decomp_of(U'\x2F8A2') << "\n  Line: 2F8A2;CJK COMPATIBILITY IDEOGRAPH-2F8A2;Lo;0;L;391C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A3'), U"\x6094") << desc_decomp_of(U'\x2F8A3') << "\n  Line: 2F8A3;CJK COMPATIBILITY IDEOGRAPH-2F8A3;Lo;0;L;6094;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A4'), U"\x226D4") << desc_decomp_of(U'\x2F8A4') << "\n  Line: 2F8A4;CJK COMPATIBILITY IDEOGRAPH-2F8A4;Lo;0;L;226D4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A5'), U"\x60C7") << desc_decomp_of(U'\x2F8A5') << "\n  Line: 2F8A5;CJK COMPATIBILITY IDEOGRAPH-2F8A5;Lo;0;L;60C7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A6'), U"\x6148") << desc_decomp_of(U'\x2F8A6') << "\n  Line: 2F8A6;CJK COMPATIBILITY IDEOGRAPH-2F8A6;Lo;0;L;6148;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A7'), U"\x614C") << desc_decomp_of(U'\x2F8A7') << "\n  Line: 2F8A7;CJK COMPATIBILITY IDEOGRAPH-2F8A7;Lo;0;L;614C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A8'), U"\x614E") << desc_decomp_of(U'\x2F8A8') << "\n  Line: 2F8A8;CJK COMPATIBILITY IDEOGRAPH-2F8A8;Lo;0;L;614E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8A9'), U"\x614C") << desc_decomp_of(U'\x2F8A9') << "\n  Line: 2F8A9;CJK COMPATIBILITY IDEOGRAPH-2F8A9;Lo;0;L;614C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8AA'), U"\x617A") << desc_decomp_of(U'\x2F8AA') << "\n  Line: 2F8AA;CJK COMPATIBILITY IDEOGRAPH-2F8AA;Lo;0;L;617A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8AB'), U"\x618E") << desc_decomp_of(U'\x2F8AB') << "\n  Line: 2F8AB;CJK COMPATIBILITY IDEOGRAPH-2F8AB;Lo;0;L;618E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8AC'), U"\x61B2") << desc_decomp_of(U'\x2F8AC') << "\n  Line: 2F8AC;CJK COMPATIBILITY IDEOGRAPH-2F8AC;Lo;0;L;61B2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8AD'), U"\x61A4") << desc_decomp_of(U'\x2F8AD') << "\n  Line: 2F8AD;CJK COMPATIBILITY IDEOGRAPH-2F8AD;Lo;0;L;61A4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8AE'), U"\x61AF") << desc_decomp_of(U'\x2F8AE') << "\n  Line: 2F8AE;CJK COMPATIBILITY IDEOGRAPH-2F8AE;Lo;0;L;61AF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8AF'), U"\x61DE") << desc_decomp_of(U'\x2F8AF') << "\n  Line: 2F8AF;CJK COMPATIBILITY IDEOGRAPH-2F8AF;Lo;0;L;61DE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B0'), U"\x61F2") << desc_decomp_of(U'\x2F8B0') << "\n  Line: 2F8B0;CJK COMPATIBILITY IDEOGRAPH-2F8B0;Lo;0;L;61F2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B1'), U"\x61F6") << desc_decomp_of(U'\x2F8B1') << "\n  Line: 2F8B1;CJK COMPATIBILITY IDEOGRAPH-2F8B1;Lo;0;L;61F6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B2'), U"\x6210") << desc_decomp_of(U'\x2F8B2') << "\n  Line: 2F8B2;CJK COMPATIBILITY IDEOGRAPH-2F8B2;Lo;0;L;6210;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B3'), U"\x621B") << desc_decomp_of(U'\x2F8B3') << "\n  Line: 2F8B3;CJK COMPATIBILITY IDEOGRAPH-2F8B3;Lo;0;L;621B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B4'), U"\x625D") << desc_decomp_of(U'\x2F8B4') << "\n  Line: 2F8B4;CJK COMPATIBILITY IDEOGRAPH-2F8B4;Lo;0;L;625D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B5'), U"\x62B1") << desc_decomp_of(U'\x2F8B5') << "\n  Line: 2F8B5;CJK COMPATIBILITY IDEOGRAPH-2F8B5;Lo;0;L;62B1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B6'), U"\x62D4") << desc_decomp_of(U'\x2F8B6') << "\n  Line: 2F8B6;CJK COMPATIBILITY IDEOGRAPH-2F8B6;Lo;0;L;62D4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B7'), U"\x6350") << desc_decomp_of(U'\x2F8B7') << "\n  Line: 2F8B7;CJK COMPATIBILITY IDEOGRAPH-2F8B7;Lo;0;L;6350;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B8'), U"\x22B0C") << desc_decomp_of(U'\x2F8B8') << "\n  Line: 2F8B8;CJK COMPATIBILITY IDEOGRAPH-2F8B8;Lo;0;L;22B0C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8B9'), U"\x633D") << desc_decomp_of(U'\x2F8B9') << "\n  Line: 2F8B9;CJK COMPATIBILITY IDEOGRAPH-2F8B9;Lo;0;L;633D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8BA'), U"\x62FC") << desc_decomp_of(U'\x2F8BA') << "\n  Line: 2F8BA;CJK COMPATIBILITY IDEOGRAPH-2F8BA;Lo;0;L;62FC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8BB'), U"\x6368") << desc_decomp_of(U'\x2F8BB') << "\n  Line: 2F8BB;CJK COMPATIBILITY IDEOGRAPH-2F8BB;Lo;0;L;6368;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8BC'), U"\x6383") << desc_decomp_of(U'\x2F8BC') << "\n  Line: 2F8BC;CJK COMPATIBILITY IDEOGRAPH-2F8BC;Lo;0;L;6383;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8BD'), U"\x63E4") << desc_decomp_of(U'\x2F8BD') << "\n  Line: 2F8BD;CJK COMPATIBILITY IDEOGRAPH-2F8BD;Lo;0;L;63E4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8BE'), U"\x22BF1") << desc_decomp_of(U'\x2F8BE') << "\n  Line: 2F8BE;CJK COMPATIBILITY IDEOGRAPH-2F8BE;Lo;0;L;22BF1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8BF'), U"\x6422") << desc_decomp_of(U'\x2F8BF') << "\n  Line: 2F8BF;CJK COMPATIBILITY IDEOGRAPH-2F8BF;Lo;0;L;6422;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C0'), U"\x63C5") << desc_decomp_of(U'\x2F8C0') << "\n  Line: 2F8C0;CJK COMPATIBILITY IDEOGRAPH-2F8C0;Lo;0;L;63C5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C1'), U"\x63A9") << desc_decomp_of(U'\x2F8C1') << "\n  Line: 2F8C1;CJK COMPATIBILITY IDEOGRAPH-2F8C1;Lo;0;L;63A9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C2'), U"\x3A2E") << desc_decomp_of(U'\x2F8C2') << "\n  Line: 2F8C2;CJK COMPATIBILITY IDEOGRAPH-2F8C2;Lo;0;L;3A2E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C3'), U"\x6469") << desc_decomp_of(U'\x2F8C3') << "\n  Line: 2F8C3;CJK COMPATIBILITY IDEOGRAPH-2F8C3;Lo;0;L;6469;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C4'), U"\x647E") << desc_decomp_of(U'\x2F8C4') << "\n  Line: 2F8C4;CJK COMPATIBILITY IDEOGRAPH-2F8C4;Lo;0;L;647E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C5'), U"\x649D") << desc_decomp_of(U'\x2F8C5') << "\n  Line: 2F8C5;CJK COMPATIBILITY IDEOGRAPH-2F8C5;Lo;0;L;649D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C6'), U"\x6477") << desc_decomp_of(U'\x2F8C6') << "\n  Line: 2F8C6;CJK COMPATIBILITY IDEOGRAPH-2F8C6;Lo;0;L;6477;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C7'), U"\x3A6C") << desc_decomp_of(U'\x2F8C7') << "\n  Line: 2F8C7;CJK COMPATIBILITY IDEOGRAPH-2F8C7;Lo;0;L;3A6C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C8'), U"\x654F") << desc_decomp_of(U'\x2F8C8') << "\n  Line: 2F8C8;CJK COMPATIBILITY IDEOGRAPH-2F8C8;Lo;0;L;654F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8C9'), U"\x656C") << desc_decomp_of(U'\x2F8C9') << "\n  Line: 2F8C9;CJK COMPATIBILITY IDEOGRAPH-2F8C9;Lo;0;L;656C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8CA'), U"\x2300A") << desc_decomp_of(U'\x2F8CA') << "\n  Line: 2F8CA;CJK COMPATIBILITY IDEOGRAPH-2F8CA;Lo;0;L;2300A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8CB'), U"\x65E3") << desc_decomp_of(U'\x2F8CB') << "\n  Line: 2F8CB;CJK COMPATIBILITY IDEOGRAPH-2F8CB;Lo;0;L;65E3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8CC'), U"\x66F8") << desc_decomp_of(U'\x2F8CC') << "\n  Line: 2F8CC;CJK COMPATIBILITY IDEOGRAPH-2F8CC;Lo;0;L;66F8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8CD'), U"\x6649") << desc_decomp_of(U'\x2F8CD') << "\n  Line: 2F8CD;CJK COMPATIBILITY IDEOGRAPH-2F8CD;Lo;0;L;6649;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8CE'), U"\x3B19") << desc_decomp_of(U'\x2F8CE') << "\n  Line: 2F8CE;CJK COMPATIBILITY IDEOGRAPH-2F8CE;Lo;0;L;3B19;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8CF'), U"\x6691") << desc_decomp_of(U'\x2F8CF') << "\n  Line: 2F8CF;CJK COMPATIBILITY IDEOGRAPH-2F8CF;Lo;0;L;6691;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D0'), U"\x3B08") << desc_decomp_of(U'\x2F8D0') << "\n  Line: 2F8D0;CJK COMPATIBILITY IDEOGRAPH-2F8D0;Lo;0;L;3B08;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D1'), U"\x3AE4") << desc_decomp_of(U'\x2F8D1') << "\n  Line: 2F8D1;CJK COMPATIBILITY IDEOGRAPH-2F8D1;Lo;0;L;3AE4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D2'), U"\x5192") << desc_decomp_of(U'\x2F8D2') << "\n  Line: 2F8D2;CJK COMPATIBILITY IDEOGRAPH-2F8D2;Lo;0;L;5192;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D3'), U"\x5195") << desc_decomp_of(U'\x2F8D3') << "\n  Line: 2F8D3;CJK COMPATIBILITY IDEOGRAPH-2F8D3;Lo;0;L;5195;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D4'), U"\x6700") << desc_decomp_of(U'\x2F8D4') << "\n  Line: 2F8D4;CJK COMPATIBILITY IDEOGRAPH-2F8D4;Lo;0;L;6700;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D5'), U"\x669C") << desc_decomp_of(U'\x2F8D5') << "\n  Line: 2F8D5;CJK COMPATIBILITY IDEOGRAPH-2F8D5;Lo;0;L;669C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D6'), U"\x80AD") << desc_decomp_of(U'\x2F8D6') << "\n  Line: 2F8D6;CJK COMPATIBILITY IDEOGRAPH-2F8D6;Lo;0;L;80AD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D7'), U"\x43D9") << desc_decomp_of(U'\x2F8D7') << "\n  Line: 2F8D7;CJK COMPATIBILITY IDEOGRAPH-2F8D7;Lo;0;L;43D9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D8'), U"\x6717") << desc_decomp_of(U'\x2F8D8') << "\n  Line: 2F8D8;CJK COMPATIBILITY IDEOGRAPH-2F8D8;Lo;0;L;6717;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8D9'), U"\x671B") << desc_decomp_of(U'\x2F8D9') << "\n  Line: 2F8D9;CJK COMPATIBILITY IDEOGRAPH-2F8D9;Lo;0;L;671B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8DA'), U"\x6721") << desc_decomp_of(U'\x2F8DA') << "\n  Line: 2F8DA;CJK COMPATIBILITY IDEOGRAPH-2F8DA;Lo;0;L;6721;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8DB'), U"\x675E") << desc_decomp_of(U'\x2F8DB') << "\n  Line: 2F8DB;CJK COMPATIBILITY IDEOGRAPH-2F8DB;Lo;0;L;675E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8DC'), U"\x6753") << desc_decomp_of(U'\x2F8DC') << "\n  Line: 2F8DC;CJK COMPATIBILITY IDEOGRAPH-2F8DC;Lo;0;L;6753;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8DD'), U"\x233C3") << desc_decomp_of(U'\x2F8DD') << "\n  Line: 2F8DD;CJK COMPATIBILITY IDEOGRAPH-2F8DD;Lo;0;L;233C3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8DE'), U"\x3B49") << desc_decomp_of(U'\x2F8DE') << "\n  Line: 2F8DE;CJK COMPATIBILITY IDEOGRAPH-2F8DE;Lo;0;L;3B49;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8DF'), U"\x67FA") << desc_decomp_of(U'\x2F8DF') << "\n  Line: 2F8DF;CJK COMPATIBILITY IDEOGRAPH-2F8DF;Lo;0;L;67FA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E0'), U"\x6785") << desc_decomp_of(U'\x2F8E0') << "\n  Line: 2F8E0;CJK COMPATIBILITY IDEOGRAPH-2F8E0;Lo;0;L;6785;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E1'), U"\x6852") << desc_decomp_of(U'\x2F8E1') << "\n  Line: 2F8E1;CJK COMPATIBILITY IDEOGRAPH-2F8E1;Lo;0;L;6852;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E2'), U"\x6885") << desc_decomp_of(U'\x2F8E2') << "\n  Line: 2F8E2;CJK COMPATIBILITY IDEOGRAPH-2F8E2;Lo;0;L;6885;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E3'), U"\x2346D") << desc_decomp_of(U'\x2F8E3') << "\n  Line: 2F8E3;CJK COMPATIBILITY IDEOGRAPH-2F8E3;Lo;0;L;2346D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E4'), U"\x688E") << desc_decomp_of(U'\x2F8E4') << "\n  Line: 2F8E4;CJK COMPATIBILITY IDEOGRAPH-2F8E4;Lo;0;L;688E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E5'), U"\x681F") << desc_decomp_of(U'\x2F8E5') << "\n  Line: 2F8E5;CJK COMPATIBILITY IDEOGRAPH-2F8E5;Lo;0;L;681F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E6'), U"\x6914") << desc_decomp_of(U'\x2F8E6') << "\n  Line: 2F8E6;CJK COMPATIBILITY IDEOGRAPH-2F8E6;Lo;0;L;6914;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E7'), U"\x3B9D") << desc_decomp_of(U'\x2F8E7') << "\n  Line: 2F8E7;CJK COMPATIBILITY IDEOGRAPH-2F8E7;Lo;0;L;3B9D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E8'), U"\x6942") << desc_decomp_of(U'\x2F8E8') << "\n  Line: 2F8E8;CJK COMPATIBILITY IDEOGRAPH-2F8E8;Lo;0;L;6942;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8E9'), U"\x69A3") << desc_decomp_of(U'\x2F8E9') << "\n  Line: 2F8E9;CJK COMPATIBILITY IDEOGRAPH-2F8E9;Lo;0;L;69A3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8EA'), U"\x69EA") << desc_decomp_of(U'\x2F8EA') << "\n  Line: 2F8EA;CJK COMPATIBILITY IDEOGRAPH-2F8EA;Lo;0;L;69EA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8EB'), U"\x6AA8") << desc_decomp_of(U'\x2F8EB') << "\n  Line: 2F8EB;CJK COMPATIBILITY IDEOGRAPH-2F8EB;Lo;0;L;6AA8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8EC'), U"\x236A3") << desc_decomp_of(U'\x2F8EC') << "\n  Line: 2F8EC;CJK COMPATIBILITY IDEOGRAPH-2F8EC;Lo;0;L;236A3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8ED'), U"\x6ADB") << desc_decomp_of(U'\x2F8ED') << "\n  Line: 2F8ED;CJK COMPATIBILITY IDEOGRAPH-2F8ED;Lo;0;L;6ADB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8EE'), U"\x3C18") << desc_decomp_of(U'\x2F8EE') << "\n  Line: 2F8EE;CJK COMPATIBILITY IDEOGRAPH-2F8EE;Lo;0;L;3C18;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8EF'), U"\x6B21") << desc_decomp_of(U'\x2F8EF') << "\n  Line: 2F8EF;CJK COMPATIBILITY IDEOGRAPH-2F8EF;Lo;0;L;6B21;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F0'), U"\x238A7") << desc_decomp_of(U'\x2F8F0') << "\n  Line: 2F8F0;CJK COMPATIBILITY IDEOGRAPH-2F8F0;Lo;0;L;238A7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F1'), U"\x6B54") << desc_decomp_of(U'\x2F8F1') << "\n  Line: 2F8F1;CJK COMPATIBILITY IDEOGRAPH-2F8F1;Lo;0;L;6B54;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F2'), U"\x3C4E") << desc_decomp_of(U'\x2F8F2') << "\n  Line: 2F8F2;CJK COMPATIBILITY IDEOGRAPH-2F8F2;Lo;0;L;3C4E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F3'), U"\x6B72") << desc_decomp_of(U'\x2F8F3') << "\n  Line: 2F8F3;CJK COMPATIBILITY IDEOGRAPH-2F8F3;Lo;0;L;6B72;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F4'), U"\x6B9F") << desc_decomp_of(U'\x2F8F4') << "\n  Line: 2F8F4;CJK COMPATIBILITY IDEOGRAPH-2F8F4;Lo;0;L;6B9F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F5'), U"\x6BBA") << desc_decomp_of(U'\x2F8F5') << "\n  Line: 2F8F5;CJK COMPATIBILITY IDEOGRAPH-2F8F5;Lo;0;L;6BBA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F6'), U"\x6BBB") << desc_decomp_of(U'\x2F8F6') << "\n  Line: 2F8F6;CJK COMPATIBILITY IDEOGRAPH-2F8F6;Lo;0;L;6BBB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F7'), U"\x23A8D") << desc_decomp_of(U'\x2F8F7') << "\n  Line: 2F8F7;CJK COMPATIBILITY IDEOGRAPH-2F8F7;Lo;0;L;23A8D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F8'), U"\x21D0B") << desc_decomp_of(U'\x2F8F8') << "\n  Line: 2F8F8;CJK COMPATIBILITY IDEOGRAPH-2F8F8;Lo;0;L;21D0B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8F9'), U"\x23AFA") << desc_decomp_of(U'\x2F8F9') << "\n  Line: 2F8F9;CJK COMPATIBILITY IDEOGRAPH-2F8F9;Lo;0;L;23AFA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8FA'), U"\x6C4E") << desc_decomp_of(U'\x2F8FA') << "\n  Line: 2F8FA;CJK COMPATIBILITY IDEOGRAPH-2F8FA;Lo;0;L;6C4E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8FB'), U"\x23CBC") << desc_decomp_of(U'\x2F8FB') << "\n  Line: 2F8FB;CJK COMPATIBILITY IDEOGRAPH-2F8FB;Lo;0;L;23CBC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8FC'), U"\x6CBF") << desc_decomp_of(U'\x2F8FC') << "\n  Line: 2F8FC;CJK COMPATIBILITY IDEOGRAPH-2F8FC;Lo;0;L;6CBF;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8FD'), U"\x6CCD") << desc_decomp_of(U'\x2F8FD') << "\n  Line: 2F8FD;CJK COMPATIBILITY IDEOGRAPH-2F8FD;Lo;0;L;6CCD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8FE'), U"\x6C67") << desc_decomp_of(U'\x2F8FE') << "\n  Line: 2F8FE;CJK COMPATIBILITY IDEOGRAPH-2F8FE;Lo;0;L;6C67;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F8FF'), U"\x6D16") << desc_decomp_of(U'\x2F8FF') << "\n  Line: 2F8FF;CJK COMPATIBILITY IDEOGRAPH-2F8FF;Lo;0;L;6D16;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F900'), U"\x6D3E") << desc_decomp_of(U'\x2F900') << "\n  Line: 2F900;CJK COMPATIBILITY IDEOGRAPH-2F900;Lo;0;L;6D3E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F901'), U"\x6D77") << desc_decomp_of(U'\x2F901') << "\n  Line: 2F901;CJK COMPATIBILITY IDEOGRAPH-2F901;Lo;0;L;6D77;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F902'), U"\x6D41") << desc_decomp_of(U'\x2F902') << "\n  Line: 2F902;CJK COMPATIBILITY IDEOGRAPH-2F902;Lo;0;L;6D41;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F903'), U"\x6D69") << desc_decomp_of(U'\x2F903') << "\n  Line: 2F903;CJK COMPATIBILITY IDEOGRAPH-2F903;Lo;0;L;6D69;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F904'), U"\x6D78") << desc_decomp_of(U'\x2F904') << "\n  Line: 2F904;CJK COMPATIBILITY IDEOGRAPH-2F904;Lo;0;L;6D78;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F905'), U"\x6D85") << desc_decomp_of(U'\x2F905') << "\n  Line: 2F905;CJK COMPATIBILITY IDEOGRAPH-2F905;Lo;0;L;6D85;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F906'), U"\x23D1E") << desc_decomp_of(U'\x2F906') << "\n  Line: 2F906;CJK COMPATIBILITY IDEOGRAPH-2F906;Lo;0;L;23D1E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F907'), U"\x6D34") << desc_decomp_of(U'\x2F907') << "\n  Line: 2F907;CJK COMPATIBILITY IDEOGRAPH-2F907;Lo;0;L;6D34;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F908'), U"\x6E2F") << desc_decomp_of(U'\x2F908') << "\n  Line: 2F908;CJK COMPATIBILITY IDEOGRAPH-2F908;Lo;0;L;6E2F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F909'), U"\x6E6E") << desc_decomp_of(U'\x2F909') << "\n  Line: 2F909;CJK COMPATIBILITY IDEOGRAPH-2F909;Lo;0;L;6E6E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F90A'), U"\x3D33") << desc_decomp_of(U'\x2F90A') << "\n  Line: 2F90A;CJK COMPATIBILITY IDEOGRAPH-2F90A;Lo;0;L;3D33;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F90B'), U"\x6ECB") << desc_decomp_of(U'\x2F90B') << "\n  Line: 2F90B;CJK COMPATIBILITY IDEOGRAPH-2F90B;Lo;0;L;6ECB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F90C'), U"\x6EC7") << desc_decomp_of(U'\x2F90C') << "\n  Line: 2F90C;CJK COMPATIBILITY IDEOGRAPH-2F90C;Lo;0;L;6EC7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F90D'), U"\x23ED1") << desc_decomp_of(U'\x2F90D') << "\n  Line: 2F90D;CJK COMPATIBILITY IDEOGRAPH-2F90D;Lo;0;L;23ED1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F90E'), U"\x6DF9") << desc_decomp_of(U'\x2F90E') << "\n  Line: 2F90E;CJK COMPATIBILITY IDEOGRAPH-2F90E;Lo;0;L;6DF9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F90F'), U"\x6F6E") << desc_decomp_of(U'\x2F90F') << "\n  Line: 2F90F;CJK COMPATIBILITY IDEOGRAPH-2F90F;Lo;0;L;6F6E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F910'), U"\x23F5E") << desc_decomp_of(U'\x2F910') << "\n  Line: 2F910;CJK COMPATIBILITY IDEOGRAPH-2F910;Lo;0;L;23F5E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F911'), U"\x23F8E") << desc_decomp_of(U'\x2F911') << "\n  Line: 2F911;CJK COMPATIBILITY IDEOGRAPH-2F911;Lo;0;L;23F8E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F912'), U"\x6FC6") << desc_decomp_of(U'\x2F912') << "\n  Line: 2F912;CJK COMPATIBILITY IDEOGRAPH-2F912;Lo;0;L;6FC6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F913'), U"\x7039") << desc_decomp_of(U'\x2F913') << "\n  Line: 2F913;CJK COMPATIBILITY IDEOGRAPH-2F913;Lo;0;L;7039;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F914'), U"\x701E") << desc_decomp_of(U'\x2F914') << "\n  Line: 2F914;CJK COMPATIBILITY IDEOGRAPH-2F914;Lo;0;L;701E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F915'), U"\x701B") << desc_decomp_of(U'\x2F915') << "\n  Line: 2F915;CJK COMPATIBILITY IDEOGRAPH-2F915;Lo;0;L;701B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F916'), U"\x3D96") << desc_decomp_of(U'\x2F916') << "\n  Line: 2F916;CJK COMPATIBILITY IDEOGRAPH-2F916;Lo;0;L;3D96;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F917'), U"\x704A") << desc_decomp_of(U'\x2F917') << "\n  Line: 2F917;CJK COMPATIBILITY IDEOGRAPH-2F917;Lo;0;L;704A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F918'), U"\x707D") << desc_decomp_of(U'\x2F918') << "\n  Line: 2F918;CJK COMPATIBILITY IDEOGRAPH-2F918;Lo;0;L;707D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F919'), U"\x7077") << desc_decomp_of(U'\x2F919') << "\n  Line: 2F919;CJK COMPATIBILITY IDEOGRAPH-2F919;Lo;0;L;7077;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F91A'), U"\x70AD") << desc_decomp_of(U'\x2F91A') << "\n  Line: 2F91A;CJK COMPATIBILITY IDEOGRAPH-2F91A;Lo;0;L;70AD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F91B'), U"\x20525") << desc_decomp_of(U'\x2F91B') << "\n  Line: 2F91B;CJK COMPATIBILITY IDEOGRAPH-2F91B;Lo;0;L;20525;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F91C'), U"\x7145") << desc_decomp_of(U'\x2F91C') << "\n  Line: 2F91C;CJK COMPATIBILITY IDEOGRAPH-2F91C;Lo;0;L;7145;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F91D'), U"\x24263") << desc_decomp_of(U'\x2F91D') << "\n  Line: 2F91D;CJK COMPATIBILITY IDEOGRAPH-2F91D;Lo;0;L;24263;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F91E'), U"\x719C") << desc_decomp_of(U'\x2F91E') << "\n  Line: 2F91E;CJK COMPATIBILITY IDEOGRAPH-2F91E;Lo;0;L;719C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F91F'), U"\x243AB") << desc_decomp_of(U'\x2F91F') << "\n  Line: 2F91F;CJK COMPATIBILITY IDEOGRAPH-2F91F;Lo;0;L;243AB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F920'), U"\x7228") << desc_decomp_of(U'\x2F920') << "\n  Line: 2F920;CJK COMPATIBILITY IDEOGRAPH-2F920;Lo;0;L;7228;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F921'), U"\x7235") << desc_decomp_of(U'\x2F921') << "\n  Line: 2F921;CJK COMPATIBILITY IDEOGRAPH-2F921;Lo;0;L;7235;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F922'), U"\x7250") << desc_decomp_of(U'\x2F922') << "\n  Line: 2F922;CJK COMPATIBILITY IDEOGRAPH-2F922;Lo;0;L;7250;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F923'), U"\x24608") << desc_decomp_of(U'\x2F923') << "\n  Line: 2F923;CJK COMPATIBILITY IDEOGRAPH-2F923;Lo;0;L;24608;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F924'), U"\x7280") << desc_decomp_of(U'\x2F924') << "\n  Line: 2F924;CJK COMPATIBILITY IDEOGRAPH-2F924;Lo;0;L;7280;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F925'), U"\x7295") << desc_decomp_of(U'\x2F925') << "\n  Line: 2F925;CJK COMPATIBILITY IDEOGRAPH-2F925;Lo;0;L;7295;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F926'), U"\x24735") << desc_decomp_of(U'\x2F926') << "\n  Line: 2F926;CJK COMPATIBILITY IDEOGRAPH-2F926;Lo;0;L;24735;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F927'), U"\x24814") << desc_decomp_of(U'\x2F927') << "\n  Line: 2F927;CJK COMPATIBILITY IDEOGRAPH-2F927;Lo;0;L;24814;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F928'), U"\x737A") << desc_decomp_of(U'\x2F928') << "\n  Line: 2F928;CJK COMPATIBILITY IDEOGRAPH-2F928;Lo;0;L;737A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F929'), U"\x738B") << desc_decomp_of(U'\x2F929') << "\n  Line: 2F929;CJK COMPATIBILITY IDEOGRAPH-2F929;Lo;0;L;738B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F92A'), U"\x3EAC") << desc_decomp_of(U'\x2F92A') << "\n  Line: 2F92A;CJK COMPATIBILITY IDEOGRAPH-2F92A;Lo;0;L;3EAC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F92B'), U"\x73A5") << desc_decomp_of(U'\x2F92B') << "\n  Line: 2F92B;CJK COMPATIBILITY IDEOGRAPH-2F92B;Lo;0;L;73A5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F92C'), U"\x3EB8") << desc_decomp_of(U'\x2F92C') << "\n  Line: 2F92C;CJK COMPATIBILITY IDEOGRAPH-2F92C;Lo;0;L;3EB8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F92D'), U"\x3EB8") << desc_decomp_of(U'\x2F92D') << "\n  Line: 2F92D;CJK COMPATIBILITY IDEOGRAPH-2F92D;Lo;0;L;3EB8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F92E'), U"\x7447") << desc_decomp_of(U'\x2F92E') << "\n  Line: 2F92E;CJK COMPATIBILITY IDEOGRAPH-2F92E;Lo;0;L;7447;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F92F'), U"\x745C") << desc_decomp_of(U'\x2F92F') << "\n  Line: 2F92F;CJK COMPATIBILITY IDEOGRAPH-2F92F;Lo;0;L;745C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F930'), U"\x7471") << desc_decomp_of(U'\x2F930') << "\n  Line: 2F930;CJK COMPATIBILITY IDEOGRAPH-2F930;Lo;0;L;7471;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F931'), U"\x7485") << desc_decomp_of(U'\x2F931') << "\n  Line: 2F931;CJK COMPATIBILITY IDEOGRAPH-2F931;Lo;0;L;7485;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F932'), U"\x74CA") << desc_decomp_of(U'\x2F932') << "\n  Line: 2F932;CJK COMPATIBILITY IDEOGRAPH-2F932;Lo;0;L;74CA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F933'), U"\x3F1B") << desc_decomp_of(U'\x2F933') << "\n  Line: 2F933;CJK COMPATIBILITY IDEOGRAPH-2F933;Lo;0;L;3F1B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F934'), U"\x7524") << desc_decomp_of(U'\x2F934') << "\n  Line: 2F934;CJK COMPATIBILITY IDEOGRAPH-2F934;Lo;0;L;7524;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F935'), U"\x24C36") << desc_decomp_of(U'\x2F935') << "\n  Line: 2F935;CJK COMPATIBILITY IDEOGRAPH-2F935;Lo;0;L;24C36;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F936'), U"\x753E") << desc_decomp_of(U'\x2F936') << "\n  Line: 2F936;CJK COMPATIBILITY IDEOGRAPH-2F936;Lo;0;L;753E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F937'), U"\x24C92") << desc_decomp_of(U'\x2F937') << "\n  Line: 2F937;CJK COMPATIBILITY IDEOGRAPH-2F937;Lo;0;L;24C92;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F938'), U"\x7570") << desc_decomp_of(U'\x2F938') << "\n  Line: 2F938;CJK COMPATIBILITY IDEOGRAPH-2F938;Lo;0;L;7570;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F939'), U"\x2219F") << desc_decomp_of(U'\x2F939') << "\n  Line: 2F939;CJK COMPATIBILITY IDEOGRAPH-2F939;Lo;0;L;2219F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F93A'), U"\x7610") << desc_decomp_of(U'\x2F93A') << "\n  Line: 2F93A;CJK COMPATIBILITY IDEOGRAPH-2F93A;Lo;0;L;7610;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F93B'), U"\x24FA1") << desc_decomp_of(U'\x2F93B') << "\n  Line: 2F93B;CJK COMPATIBILITY IDEOGRAPH-2F93B;Lo;0;L;24FA1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F93C'), U"\x24FB8") << desc_decomp_of(U'\x2F93C') << "\n  Line: 2F93C;CJK COMPATIBILITY IDEOGRAPH-2F93C;Lo;0;L;24FB8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F93D'), U"\x25044") << desc_decomp_of(U'\x2F93D') << "\n  Line: 2F93D;CJK COMPATIBILITY IDEOGRAPH-2F93D;Lo;0;L;25044;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F93E'), U"\x3FFC") << desc_decomp_of(U'\x2F93E') << "\n  Line: 2F93E;CJK COMPATIBILITY IDEOGRAPH-2F93E;Lo;0;L;3FFC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F93F'), U"\x4008") << desc_decomp_of(U'\x2F93F') << "\n  Line: 2F93F;CJK COMPATIBILITY IDEOGRAPH-2F93F;Lo;0;L;4008;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F940'), U"\x76F4") << desc_decomp_of(U'\x2F940') << "\n  Line: 2F940;CJK COMPATIBILITY IDEOGRAPH-2F940;Lo;0;L;76F4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F941'), U"\x250F3") << desc_decomp_of(U'\x2F941') << "\n  Line: 2F941;CJK COMPATIBILITY IDEOGRAPH-2F941;Lo;0;L;250F3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F942'), U"\x250F2") << desc_decomp_of(U'\x2F942') << "\n  Line: 2F942;CJK COMPATIBILITY IDEOGRAPH-2F942;Lo;0;L;250F2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F943'), U"\x25119") << desc_decomp_of(U'\x2F943') << "\n  Line: 2F943;CJK COMPATIBILITY IDEOGRAPH-2F943;Lo;0;L;25119;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F944'), U"\x25133") << desc_decomp_of(U'\x2F944') << "\n  Line: 2F944;CJK COMPATIBILITY IDEOGRAPH-2F944;Lo;0;L;25133;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F945'), U"\x771E") << desc_decomp_of(U'\x2F945') << "\n  Line: 2F945;CJK COMPATIBILITY IDEOGRAPH-2F945;Lo;0;L;771E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F946'), U"\x771F") << desc_decomp_of(U'\x2F946') << "\n  Line: 2F946;CJK COMPATIBILITY IDEOGRAPH-2F946;Lo;0;L;771F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F947'), U"\x771F") << desc_decomp_of(U'\x2F947') << "\n  Line: 2F947;CJK COMPATIBILITY IDEOGRAPH-2F947;Lo;0;L;771F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F948'), U"\x774A") << desc_decomp_of(U'\x2F948') << "\n  Line: 2F948;CJK COMPATIBILITY IDEOGRAPH-2F948;Lo;0;L;774A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F949'), U"\x4039") << desc_decomp_of(U'\x2F949') << "\n  Line: 2F949;CJK COMPATIBILITY IDEOGRAPH-2F949;Lo;0;L;4039;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F94A'), U"\x778B") << desc_decomp_of(U'\x2F94A') << "\n  Line: 2F94A;CJK COMPATIBILITY IDEOGRAPH-2F94A;Lo;0;L;778B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F94B'), U"\x4046") << desc_decomp_of(U'\x2F94B') << "\n  Line: 2F94B;CJK COMPATIBILITY IDEOGRAPH-2F94B;Lo;0;L;4046;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F94C'), U"\x4096") << desc_decomp_of(U'\x2F94C') << "\n  Line: 2F94C;CJK COMPATIBILITY IDEOGRAPH-2F94C;Lo;0;L;4096;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F94D'), U"\x2541D") << desc_decomp_of(U'\x2F94D') << "\n  Line: 2F94D;CJK COMPATIBILITY IDEOGRAPH-2F94D;Lo;0;L;2541D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F94E'), U"\x784E") << desc_decomp_of(U'\x2F94E') << "\n  Line: 2F94E;CJK COMPATIBILITY IDEOGRAPH-2F94E;Lo;0;L;784E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F94F'), U"\x788C") << desc_decomp_of(U'\x2F94F') << "\n  Line: 2F94F;CJK COMPATIBILITY IDEOGRAPH-2F94F;Lo;0;L;788C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F950'), U"\x78CC") << desc_decomp_of(U'\x2F950') << "\n  Line: 2F950;CJK COMPATIBILITY IDEOGRAPH-2F950;Lo;0;L;78CC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F951'), U"\x40E3") << desc_decomp_of(U'\x2F951') << "\n  Line: 2F951;CJK COMPATIBILITY IDEOGRAPH-2F951;Lo;0;L;40E3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F952'), U"\x25626") << desc_decomp_of(U'\x2F952') << "\n  Line: 2F952;CJK COMPATIBILITY IDEOGRAPH-2F952;Lo;0;L;25626;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F953'), U"\x7956") << desc_decomp_of(U'\x2F953') << "\n  Line: 2F953;CJK COMPATIBILITY IDEOGRAPH-2F953;Lo;0;L;7956;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F954'), U"\x2569A") << desc_decomp_of(U'\x2F954') << "\n  Line: 2F954;CJK COMPATIBILITY IDEOGRAPH-2F954;Lo;0;L;2569A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F955'), U"\x256C5") << desc_decomp_of(U'\x2F955') << "\n  Line: 2F955;CJK COMPATIBILITY IDEOGRAPH-2F955;Lo;0;L;256C5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F956'), U"\x798F") << desc_decomp_of(U'\x2F956') << "\n  Line: 2F956;CJK COMPATIBILITY IDEOGRAPH-2F956;Lo;0;L;798F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F957'), U"\x79EB") << desc_decomp_of(U'\x2F957') << "\n  Line: 2F957;CJK COMPATIBILITY IDEOGRAPH-2F957;Lo;0;L;79EB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F958'), U"\x412F") << desc_decomp_of(U'\x2F958') << "\n  Line: 2F958;CJK COMPATIBILITY IDEOGRAPH-2F958;Lo;0;L;412F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F959'), U"\x7A40") << desc_decomp_of(U'\x2F959') << "\n  Line: 2F959;CJK COMPATIBILITY IDEOGRAPH-2F959;Lo;0;L;7A40;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F95A'), U"\x7A4A") << desc_decomp_of(U'\x2F95A') << "\n  Line: 2F95A;CJK COMPATIBILITY IDEOGRAPH-2F95A;Lo;0;L;7A4A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F95B'), U"\x7A4F") << desc_decomp_of(U'\x2F95B') << "\n  Line: 2F95B;CJK COMPATIBILITY IDEOGRAPH-2F95B;Lo;0;L;7A4F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F95C'), U"\x2597C") << desc_decomp_of(U'\x2F95C') << "\n  Line: 2F95C;CJK COMPATIBILITY IDEOGRAPH-2F95C;Lo;0;L;2597C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F95D'), U"\x25AA7") << desc_decomp_of(U'\x2F95D') << "\n  Line: 2F95D;CJK COMPATIBILITY IDEOGRAPH-2F95D;Lo;0;L;25AA7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F95E'), U"\x25AA7") << desc_decomp_of(U'\x2F95E') << "\n  Line: 2F95E;CJK COMPATIBILITY IDEOGRAPH-2F95E;Lo;0;L;25AA7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F95F'), U"\x7AEE") << desc_decomp_of(U'\x2F95F') << "\n  Line: 2F95F;CJK COMPATIBILITY IDEOGRAPH-2F95F;Lo;0;L;7AEE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F960'), U"\x4202") << desc_decomp_of(U'\x2F960') << "\n  Line: 2F960;CJK COMPATIBILITY IDEOGRAPH-2F960;Lo;0;L;4202;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F961'), U"\x25BAB") << desc_decomp_of(U'\x2F961') << "\n  Line: 2F961;CJK COMPATIBILITY IDEOGRAPH-2F961;Lo;0;L;25BAB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F962'), U"\x7BC6") << desc_decomp_of(U'\x2F962') << "\n  Line: 2F962;CJK COMPATIBILITY IDEOGRAPH-2F962;Lo;0;L;7BC6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F963'), U"\x7BC9") << desc_decomp_of(U'\x2F963') << "\n  Line: 2F963;CJK COMPATIBILITY IDEOGRAPH-2F963;Lo;0;L;7BC9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F964'), U"\x4227") << desc_decomp_of(U'\x2F964') << "\n  Line: 2F964;CJK COMPATIBILITY IDEOGRAPH-2F964;Lo;0;L;4227;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F965'), U"\x25C80") << desc_decomp_of(U'\x2F965') << "\n  Line: 2F965;CJK COMPATIBILITY IDEOGRAPH-2F965;Lo;0;L;25C80;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F966'), U"\x7CD2") << desc_decomp_of(U'\x2F966') << "\n  Line: 2F966;CJK COMPATIBILITY IDEOGRAPH-2F966;Lo;0;L;7CD2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F967'), U"\x42A0") << desc_decomp_of(U'\x2F967') << "\n  Line: 2F967;CJK COMPATIBILITY IDEOGRAPH-2F967;Lo;0;L;42A0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F968'), U"\x7CE8") << desc_decomp_of(U'\x2F968') << "\n  Line: 2F968;CJK COMPATIBILITY IDEOGRAPH-2F968;Lo;0;L;7CE8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F969'), U"\x7CE3") << desc_decomp_of(U'\x2F969') << "\n  Line: 2F969;CJK COMPATIBILITY IDEOGRAPH-2F969;Lo;0;L;7CE3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F96A'), U"\x7D00") << desc_decomp_of(U'\x2F96A') << "\n  Line: 2F96A;CJK COMPATIBILITY IDEOGRAPH-2F96A;Lo;0;L;7D00;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F96B'), U"\x25F86") << desc_decomp_of(U'\x2F96B') << "\n  Line: 2F96B;CJK COMPATIBILITY IDEOGRAPH-2F96B;Lo;0;L;25F86;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F96C'), U"\x7D63") << desc_decomp_of(U'\x2F96C') << "\n  Line: 2F96C;CJK COMPATIBILITY IDEOGRAPH-2F96C;Lo;0;L;7D63;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F96D'), U"\x4301") << desc_decomp_of(U'\x2F96D') << "\n  Line: 2F96D;CJK COMPATIBILITY IDEOGRAPH-2F96D;Lo;0;L;4301;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F96E'), U"\x7DC7") << desc_decomp_of(U'\x2F96E') << "\n  Line: 2F96E;CJK COMPATIBILITY IDEOGRAPH-2F96E;Lo;0;L;7DC7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F96F'), U"\x7E02") << desc_decomp_of(U'\x2F96F') << "\n  Line: 2F96F;CJK COMPATIBILITY IDEOGRAPH-2F96F;Lo;0;L;7E02;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F970'), U"\x7E45") << desc_decomp_of(U'\x2F970') << "\n  Line: 2F970;CJK COMPATIBILITY IDEOGRAPH-2F970;Lo;0;L;7E45;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F971'), U"\x4334") << desc_decomp_of(U'\x2F971') << "\n  Line: 2F971;CJK COMPATIBILITY IDEOGRAPH-2F971;Lo;0;L;4334;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F972'), U"\x26228") << desc_decomp_of(U'\x2F972') << "\n  Line: 2F972;CJK COMPATIBILITY IDEOGRAPH-2F972;Lo;0;L;26228;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F973'), U"\x26247") << desc_decomp_of(U'\x2F973') << "\n  Line: 2F973;CJK COMPATIBILITY IDEOGRAPH-2F973;Lo;0;L;26247;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F974'), U"\x4359") << desc_decomp_of(U'\x2F974') << "\n  Line: 2F974;CJK COMPATIBILITY IDEOGRAPH-2F974;Lo;0;L;4359;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F975'), U"\x262D9") << desc_decomp_of(U'\x2F975') << "\n  Line: 2F975;CJK COMPATIBILITY IDEOGRAPH-2F975;Lo;0;L;262D9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F976'), U"\x7F7A") << desc_decomp_of(U'\x2F976') << "\n  Line: 2F976;CJK COMPATIBILITY IDEOGRAPH-2F976;Lo;0;L;7F7A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F977'), U"\x2633E") << desc_decomp_of(U'\x2F977') << "\n  Line: 2F977;CJK COMPATIBILITY IDEOGRAPH-2F977;Lo;0;L;2633E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F978'), U"\x7F95") << desc_decomp_of(U'\x2F978') << "\n  Line: 2F978;CJK COMPATIBILITY IDEOGRAPH-2F978;Lo;0;L;7F95;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F979'), U"\x7FFA") << desc_decomp_of(U'\x2F979') << "\n  Line: 2F979;CJK COMPATIBILITY IDEOGRAPH-2F979;Lo;0;L;7FFA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F97A'), U"\x8005") << desc_decomp_of(U'\x2F97A') << "\n  Line: 2F97A;CJK COMPATIBILITY IDEOGRAPH-2F97A;Lo;0;L;8005;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F97B'), U"\x264DA") << desc_decomp_of(U'\x2F97B') << "\n  Line: 2F97B;CJK COMPATIBILITY IDEOGRAPH-2F97B;Lo;0;L;264DA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F97C'), U"\x26523") << desc_decomp_of(U'\x2F97C') << "\n  Line: 2F97C;CJK COMPATIBILITY IDEOGRAPH-2F97C;Lo;0;L;26523;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F97D'), U"\x8060") << desc_decomp_of(U'\x2F97D') << "\n  Line: 2F97D;CJK COMPATIBILITY IDEOGRAPH-2F97D;Lo;0;L;8060;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F97E'), U"\x265A8") << desc_decomp_of(U'\x2F97E') << "\n  Line: 2F97E;CJK COMPATIBILITY IDEOGRAPH-2F97E;Lo;0;L;265A8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F97F'), U"\x8070") << desc_decomp_of(U'\x2F97F') << "\n  Line: 2F97F;CJK COMPATIBILITY IDEOGRAPH-2F97F;Lo;0;L;8070;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F980'), U"\x2335F") << desc_decomp_of(U'\x2F980') << "\n  Line: 2F980;CJK COMPATIBILITY IDEOGRAPH-2F980;Lo;0;L;2335F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F981'), U"\x43D5") << desc_decomp_of(U'\x2F981') << "\n  Line: 2F981;CJK COMPATIBILITY IDEOGRAPH-2F981;Lo;0;L;43D5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F982'), U"\x80B2") << desc_decomp_of(U'\x2F982') << "\n  Line: 2F982;CJK COMPATIBILITY IDEOGRAPH-2F982;Lo;0;L;80B2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F983'), U"\x8103") << desc_decomp_of(U'\x2F983') << "\n  Line: 2F983;CJK COMPATIBILITY IDEOGRAPH-2F983;Lo;0;L;8103;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F984'), U"\x440B") << desc_decomp_of(U'\x2F984') << "\n  Line: 2F984;CJK COMPATIBILITY IDEOGRAPH-2F984;Lo;0;L;440B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F985'), U"\x813E") << desc_decomp_of(U'\x2F985') << "\n  Line: 2F985;CJK COMPATIBILITY IDEOGRAPH-2F985;Lo;0;L;813E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F986'), U"\x5AB5") << desc_decomp_of(U'\x2F986') << "\n  Line: 2F986;CJK COMPATIBILITY IDEOGRAPH-2F986;Lo;0;L;5AB5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F987'), U"\x267A7") << desc_decomp_of(U'\x2F987') << "\n  Line: 2F987;CJK COMPATIBILITY IDEOGRAPH-2F987;Lo;0;L;267A7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F988'), U"\x267B5") << desc_decomp_of(U'\x2F988') << "\n  Line: 2F988;CJK COMPATIBILITY IDEOGRAPH-2F988;Lo;0;L;267B5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F989'), U"\x23393") << desc_decomp_of(U'\x2F989') << "\n  Line: 2F989;CJK COMPATIBILITY IDEOGRAPH-2F989;Lo;0;L;23393;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F98A'), U"\x2339C") << desc_decomp_of(U'\x2F98A') << "\n  Line: 2F98A;CJK COMPATIBILITY IDEOGRAPH-2F98A;Lo;0;L;2339C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F98B'), U"\x8201") << desc_decomp_of(U'\x2F98B') << "\n  Line: 2F98B;CJK COMPATIBILITY IDEOGRAPH-2F98B;Lo;0;L;8201;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F98C'), U"\x8204") << desc_decomp_of(U'\x2F98C') << "\n  Line: 2F98C;CJK COMPATIBILITY IDEOGRAPH-2F98C;Lo;0;L;8204;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F98D'), U"\x8F9E") << desc_decomp_of(U'\x2F98D') << "\n  Line: 2F98D;CJK COMPATIBILITY IDEOGRAPH-2F98D;Lo;0;L;8F9E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F98E'), U"\x446B") << desc_decomp_of(U'\x2F98E') << "\n  Line: 2F98E;CJK COMPATIBILITY IDEOGRAPH-2F98E;Lo;0;L;446B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F98F'), U"\x8291") << desc_decomp_of(U'\x2F98F') << "\n  Line: 2F98F;CJK COMPATIBILITY IDEOGRAPH-2F98F;Lo;0;L;8291;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F990'), U"\x828B") << desc_decomp_of(U'\x2F990') << "\n  Line: 2F990;CJK COMPATIBILITY IDEOGRAPH-2F990;Lo;0;L;828B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F991'), U"\x829D") << desc_decomp_of(U'\x2F991') << "\n  Line: 2F991;CJK COMPATIBILITY IDEOGRAPH-2F991;Lo;0;L;829D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F992'), U"\x52B3") << desc_decomp_of(U'\x2F992') << "\n  Line: 2F992;CJK COMPATIBILITY IDEOGRAPH-2F992;Lo;0;L;52B3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F993'), U"\x82B1") << desc_decomp_of(U'\x2F993') << "\n  Line: 2F993;CJK COMPATIBILITY IDEOGRAPH-2F993;Lo;0;L;82B1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F994'), U"\x82B3") << desc_decomp_of(U'\x2F994') << "\n  Line: 2F994;CJK COMPATIBILITY IDEOGRAPH-2F994;Lo;0;L;82B3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F995'), U"\x82BD") << desc_decomp_of(U'\x2F995') << "\n  Line: 2F995;CJK COMPATIBILITY IDEOGRAPH-2F995;Lo;0;L;82BD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F996'), U"\x82E6") << desc_decomp_of(U'\x2F996') << "\n  Line: 2F996;CJK COMPATIBILITY IDEOGRAPH-2F996;Lo;0;L;82E6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F997'), U"\x26B3C") << desc_decomp_of(U'\x2F997') << "\n  Line: 2F997;CJK COMPATIBILITY IDEOGRAPH-2F997;Lo;0;L;26B3C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F998'), U"\x82E5") << desc_decomp_of(U'\x2F998') << "\n  Line: 2F998;CJK COMPATIBILITY IDEOGRAPH-2F998;Lo;0;L;82E5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F999'), U"\x831D") << desc_decomp_of(U'\x2F999') << "\n  Line: 2F999;CJK COMPATIBILITY IDEOGRAPH-2F999;Lo;0;L;831D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F99A'), U"\x8363") << desc_decomp_of(U'\x2F99A') << "\n  Line: 2F99A;CJK COMPATIBILITY IDEOGRAPH-2F99A;Lo;0;L;8363;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F99B'), U"\x83AD") << desc_decomp_of(U'\x2F99B') << "\n  Line: 2F99B;CJK COMPATIBILITY IDEOGRAPH-2F99B;Lo;0;L;83AD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F99C'), U"\x8323") << desc_decomp_of(U'\x2F99C') << "\n  Line: 2F99C;CJK COMPATIBILITY IDEOGRAPH-2F99C;Lo;0;L;8323;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F99D'), U"\x83BD") << desc_decomp_of(U'\x2F99D') << "\n  Line: 2F99D;CJK COMPATIBILITY IDEOGRAPH-2F99D;Lo;0;L;83BD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F99E'), U"\x83E7") << desc_decomp_of(U'\x2F99E') << "\n  Line: 2F99E;CJK COMPATIBILITY IDEOGRAPH-2F99E;Lo;0;L;83E7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F99F'), U"\x8457") << desc_decomp_of(U'\x2F99F') << "\n  Line: 2F99F;CJK COMPATIBILITY IDEOGRAPH-2F99F;Lo;0;L;8457;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A0'), U"\x8353") << desc_decomp_of(U'\x2F9A0') << "\n  Line: 2F9A0;CJK COMPATIBILITY IDEOGRAPH-2F9A0;Lo;0;L;8353;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A1'), U"\x83CA") << desc_decomp_of(U'\x2F9A1') << "\n  Line: 2F9A1;CJK COMPATIBILITY IDEOGRAPH-2F9A1;Lo;0;L;83CA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A2'), U"\x83CC") << desc_decomp_of(U'\x2F9A2') << "\n  Line: 2F9A2;CJK COMPATIBILITY IDEOGRAPH-2F9A2;Lo;0;L;83CC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A3'), U"\x83DC") << desc_decomp_of(U'\x2F9A3') << "\n  Line: 2F9A3;CJK COMPATIBILITY IDEOGRAPH-2F9A3;Lo;0;L;83DC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A4'), U"\x26C36") << desc_decomp_of(U'\x2F9A4') << "\n  Line: 2F9A4;CJK COMPATIBILITY IDEOGRAPH-2F9A4;Lo;0;L;26C36;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A5'), U"\x26D6B") << desc_decomp_of(U'\x2F9A5') << "\n  Line: 2F9A5;CJK COMPATIBILITY IDEOGRAPH-2F9A5;Lo;0;L;26D6B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A6'), U"\x26CD5") << desc_decomp_of(U'\x2F9A6') << "\n  Line: 2F9A6;CJK COMPATIBILITY IDEOGRAPH-2F9A6;Lo;0;L;26CD5;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A7'), U"\x452B") << desc_decomp_of(U'\x2F9A7') << "\n  Line: 2F9A7;CJK COMPATIBILITY IDEOGRAPH-2F9A7;Lo;0;L;452B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A8'), U"\x84F1") << desc_decomp_of(U'\x2F9A8') << "\n  Line: 2F9A8;CJK COMPATIBILITY IDEOGRAPH-2F9A8;Lo;0;L;84F1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9A9'), U"\x84F3") << desc_decomp_of(U'\x2F9A9') << "\n  Line: 2F9A9;CJK COMPATIBILITY IDEOGRAPH-2F9A9;Lo;0;L;84F3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9AA'), U"\x8516") << desc_decomp_of(U'\x2F9AA') << "\n  Line: 2F9AA;CJK COMPATIBILITY IDEOGRAPH-2F9AA;Lo;0;L;8516;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9AB'), U"\x273CA") << desc_decomp_of(U'\x2F9AB') << "\n  Line: 2F9AB;CJK COMPATIBILITY IDEOGRAPH-2F9AB;Lo;0;L;273CA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9AC'), U"\x8564") << desc_decomp_of(U'\x2F9AC') << "\n  Line: 2F9AC;CJK COMPATIBILITY IDEOGRAPH-2F9AC;Lo;0;L;8564;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9AD'), U"\x26F2C") << desc_decomp_of(U'\x2F9AD') << "\n  Line: 2F9AD;CJK COMPATIBILITY IDEOGRAPH-2F9AD;Lo;0;L;26F2C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9AE'), U"\x455D") << desc_decomp_of(U'\x2F9AE') << "\n  Line: 2F9AE;CJK COMPATIBILITY IDEOGRAPH-2F9AE;Lo;0;L;455D;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9AF'), U"\x4561") << desc_decomp_of(U'\x2F9AF') << "\n  Line: 2F9AF;CJK COMPATIBILITY IDEOGRAPH-2F9AF;Lo;0;L;4561;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B0'), U"\x26FB1") << desc_decomp_of(U'\x2F9B0') << "\n  Line: 2F9B0;CJK COMPATIBILITY IDEOGRAPH-2F9B0;Lo;0;L;26FB1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B1'), U"\x270D2") << desc_decomp_of(U'\x2F9B1') << "\n  Line: 2F9B1;CJK COMPATIBILITY IDEOGRAPH-2F9B1;Lo;0;L;270D2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B2'), U"\x456B") << desc_decomp_of(U'\x2F9B2') << "\n  Line: 2F9B2;CJK COMPATIBILITY IDEOGRAPH-2F9B2;Lo;0;L;456B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B3'), U"\x8650") << desc_decomp_of(U'\x2F9B3') << "\n  Line: 2F9B3;CJK COMPATIBILITY IDEOGRAPH-2F9B3;Lo;0;L;8650;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B4'), U"\x865C") << desc_decomp_of(U'\x2F9B4') << "\n  Line: 2F9B4;CJK COMPATIBILITY IDEOGRAPH-2F9B4;Lo;0;L;865C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B5'), U"\x8667") << desc_decomp_of(U'\x2F9B5') << "\n  Line: 2F9B5;CJK COMPATIBILITY IDEOGRAPH-2F9B5;Lo;0;L;8667;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B6'), U"\x8669") << desc_decomp_of(U'\x2F9B6') << "\n  Line: 2F9B6;CJK COMPATIBILITY IDEOGRAPH-2F9B6;Lo;0;L;8669;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B7'), U"\x86A9") << desc_decomp_of(U'\x2F9B7') << "\n  Line: 2F9B7;CJK COMPATIBILITY IDEOGRAPH-2F9B7;Lo;0;L;86A9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B8'), U"\x8688") << desc_decomp_of(U'\x2F9B8') << "\n  Line: 2F9B8;CJK COMPATIBILITY IDEOGRAPH-2F9B8;Lo;0;L;8688;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9B9'), U"\x870E") << desc_decomp_of(U'\x2F9B9') << "\n  Line: 2F9B9;CJK COMPATIBILITY IDEOGRAPH-2F9B9;Lo;0;L;870E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9BA'), U"\x86E2") << desc_decomp_of(U'\x2F9BA') << "\n  Line: 2F9BA;CJK COMPATIBILITY IDEOGRAPH-2F9BA;Lo;0;L;86E2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9BB'), U"\x8779") << desc_decomp_of(U'\x2F9BB') << "\n  Line: 2F9BB;CJK COMPATIBILITY IDEOGRAPH-2F9BB;Lo;0;L;8779;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9BC'), U"\x8728") << desc_decomp_of(U'\x2F9BC') << "\n  Line: 2F9BC;CJK COMPATIBILITY IDEOGRAPH-2F9BC;Lo;0;L;8728;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9BD'), U"\x876B") << desc_decomp_of(U'\x2F9BD') << "\n  Line: 2F9BD;CJK COMPATIBILITY IDEOGRAPH-2F9BD;Lo;0;L;876B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9BE'), U"\x8786") << desc_decomp_of(U'\x2F9BE') << "\n  Line: 2F9BE;CJK COMPATIBILITY IDEOGRAPH-2F9BE;Lo;0;L;8786;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9BF'), U"\x45D7") << desc_decomp_of(U'\x2F9BF') << "\n  Line: 2F9BF;CJK COMPATIBILITY IDEOGRAPH-2F9BF;Lo;0;L;45D7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C0'), U"\x87E1") << desc_decomp_of(U'\x2F9C0') << "\n  Line: 2F9C0;CJK COMPATIBILITY IDEOGRAPH-2F9C0;Lo;0;L;87E1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C1'), U"\x8801") << desc_decomp_of(U'\x2F9C1') << "\n  Line: 2F9C1;CJK COMPATIBILITY IDEOGRAPH-2F9C1;Lo;0;L;8801;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C2'), U"\x45F9") << desc_decomp_of(U'\x2F9C2') << "\n  Line: 2F9C2;CJK COMPATIBILITY IDEOGRAPH-2F9C2;Lo;0;L;45F9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C3'), U"\x8860") << desc_decomp_of(U'\x2F9C3') << "\n  Line: 2F9C3;CJK COMPATIBILITY IDEOGRAPH-2F9C3;Lo;0;L;8860;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C4'), U"\x8863") << desc_decomp_of(U'\x2F9C4') << "\n  Line: 2F9C4;CJK COMPATIBILITY IDEOGRAPH-2F9C4;Lo;0;L;8863;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C5'), U"\x27667") << desc_decomp_of(U'\x2F9C5') << "\n  Line: 2F9C5;CJK COMPATIBILITY IDEOGRAPH-2F9C5;Lo;0;L;27667;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C6'), U"\x88D7") << desc_decomp_of(U'\x2F9C6') << "\n  Line: 2F9C6;CJK COMPATIBILITY IDEOGRAPH-2F9C6;Lo;0;L;88D7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C7'), U"\x88DE") << desc_decomp_of(U'\x2F9C7') << "\n  Line: 2F9C7;CJK COMPATIBILITY IDEOGRAPH-2F9C7;Lo;0;L;88DE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C8'), U"\x4635") << desc_decomp_of(U'\x2F9C8') << "\n  Line: 2F9C8;CJK COMPATIBILITY IDEOGRAPH-2F9C8;Lo;0;L;4635;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9C9'), U"\x88FA") << desc_decomp_of(U'\x2F9C9') << "\n  Line: 2F9C9;CJK COMPATIBILITY IDEOGRAPH-2F9C9;Lo;0;L;88FA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9CA'), U"\x34BB") << desc_decomp_of(U'\x2F9CA') << "\n  Line: 2F9CA;CJK COMPATIBILITY IDEOGRAPH-2F9CA;Lo;0;L;34BB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9CB'), U"\x278AE") << desc_decomp_of(U'\x2F9CB') << "\n  Line: 2F9CB;CJK COMPATIBILITY IDEOGRAPH-2F9CB;Lo;0;L;278AE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9CC'), U"\x27966") << desc_decomp_of(U'\x2F9CC') << "\n  Line: 2F9CC;CJK COMPATIBILITY IDEOGRAPH-2F9CC;Lo;0;L;27966;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9CD'), U"\x46BE") << desc_decomp_of(U'\x2F9CD') << "\n  Line: 2F9CD;CJK COMPATIBILITY IDEOGRAPH-2F9CD;Lo;0;L;46BE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9CE'), U"\x46C7") << desc_decomp_of(U'\x2F9CE') << "\n  Line: 2F9CE;CJK COMPATIBILITY IDEOGRAPH-2F9CE;Lo;0;L;46C7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9CF'), U"\x8AA0") << desc_decomp_of(U'\x2F9CF') << "\n  Line: 2F9CF;CJK COMPATIBILITY IDEOGRAPH-2F9CF;Lo;0;L;8AA0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D0'), U"\x8AED") << desc_decomp_of(U'\x2F9D0') << "\n  Line: 2F9D0;CJK COMPATIBILITY IDEOGRAPH-2F9D0;Lo;0;L;8AED;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D1'), U"\x8B8A") << desc_decomp_of(U'\x2F9D1') << "\n  Line: 2F9D1;CJK COMPATIBILITY IDEOGRAPH-2F9D1;Lo;0;L;8B8A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D2'), U"\x8C55") << desc_decomp_of(U'\x2F9D2') << "\n  Line: 2F9D2;CJK COMPATIBILITY IDEOGRAPH-2F9D2;Lo;0;L;8C55;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D3'), U"\x27CA8") << desc_decomp_of(U'\x2F9D3') << "\n  Line: 2F9D3;CJK COMPATIBILITY IDEOGRAPH-2F9D3;Lo;0;L;27CA8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D4'), U"\x8CAB") << desc_decomp_of(U'\x2F9D4') << "\n  Line: 2F9D4;CJK COMPATIBILITY IDEOGRAPH-2F9D4;Lo;0;L;8CAB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D5'), U"\x8CC1") << desc_decomp_of(U'\x2F9D5') << "\n  Line: 2F9D5;CJK COMPATIBILITY IDEOGRAPH-2F9D5;Lo;0;L;8CC1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D6'), U"\x8D1B") << desc_decomp_of(U'\x2F9D6') << "\n  Line: 2F9D6;CJK COMPATIBILITY IDEOGRAPH-2F9D6;Lo;0;L;8D1B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D7'), U"\x8D77") << desc_decomp_of(U'\x2F9D7') << "\n  Line: 2F9D7;CJK COMPATIBILITY IDEOGRAPH-2F9D7;Lo;0;L;8D77;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D8'), U"\x27F2F") << desc_decomp_of(U'\x2F9D8') << "\n  Line: 2F9D8;CJK COMPATIBILITY IDEOGRAPH-2F9D8;Lo;0;L;27F2F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9D9'), U"\x20804") << desc_decomp_of(U'\x2F9D9') << "\n  Line: 2F9D9;CJK COMPATIBILITY IDEOGRAPH-2F9D9;Lo;0;L;20804;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9DA'), U"\x8DCB") << desc_decomp_of(U'\x2F9DA') << "\n  Line: 2F9DA;CJK COMPATIBILITY IDEOGRAPH-2F9DA;Lo;0;L;8DCB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9DB'), U"\x8DBC") << desc_decomp_of(U'\x2F9DB') << "\n  Line: 2F9DB;CJK COMPATIBILITY IDEOGRAPH-2F9DB;Lo;0;L;8DBC;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9DC'), U"\x8DF0") << desc_decomp_of(U'\x2F9DC') << "\n  Line: 2F9DC;CJK COMPATIBILITY IDEOGRAPH-2F9DC;Lo;0;L;8DF0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9DD'), U"\x208DE") << desc_decomp_of(U'\x2F9DD') << "\n  Line: 2F9DD;CJK COMPATIBILITY IDEOGRAPH-2F9DD;Lo;0;L;208DE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9DE'), U"\x8ED4") << desc_decomp_of(U'\x2F9DE') << "\n  Line: 2F9DE;CJK COMPATIBILITY IDEOGRAPH-2F9DE;Lo;0;L;8ED4;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9DF'), U"\x8F38") << desc_decomp_of(U'\x2F9DF') << "\n  Line: 2F9DF;CJK COMPATIBILITY IDEOGRAPH-2F9DF;Lo;0;L;8F38;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E0'), U"\x285D2") << desc_decomp_of(U'\x2F9E0') << "\n  Line: 2F9E0;CJK COMPATIBILITY IDEOGRAPH-2F9E0;Lo;0;L;285D2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E1'), U"\x285ED") << desc_decomp_of(U'\x2F9E1') << "\n  Line: 2F9E1;CJK COMPATIBILITY IDEOGRAPH-2F9E1;Lo;0;L;285ED;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E2'), U"\x9094") << desc_decomp_of(U'\x2F9E2') << "\n  Line: 2F9E2;CJK COMPATIBILITY IDEOGRAPH-2F9E2;Lo;0;L;9094;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E3'), U"\x90F1") << desc_decomp_of(U'\x2F9E3') << "\n  Line: 2F9E3;CJK COMPATIBILITY IDEOGRAPH-2F9E3;Lo;0;L;90F1;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E4'), U"\x9111") << desc_decomp_of(U'\x2F9E4') << "\n  Line: 2F9E4;CJK COMPATIBILITY IDEOGRAPH-2F9E4;Lo;0;L;9111;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E5'), U"\x2872E") << desc_decomp_of(U'\x2F9E5') << "\n  Line: 2F9E5;CJK COMPATIBILITY IDEOGRAPH-2F9E5;Lo;0;L;2872E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E6'), U"\x911B") << desc_decomp_of(U'\x2F9E6') << "\n  Line: 2F9E6;CJK COMPATIBILITY IDEOGRAPH-2F9E6;Lo;0;L;911B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E7'), U"\x9238") << desc_decomp_of(U'\x2F9E7') << "\n  Line: 2F9E7;CJK COMPATIBILITY IDEOGRAPH-2F9E7;Lo;0;L;9238;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E8'), U"\x92D7") << desc_decomp_of(U'\x2F9E8') << "\n  Line: 2F9E8;CJK COMPATIBILITY IDEOGRAPH-2F9E8;Lo;0;L;92D7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9E9'), U"\x92D8") << desc_decomp_of(U'\x2F9E9') << "\n  Line: 2F9E9;CJK COMPATIBILITY IDEOGRAPH-2F9E9;Lo;0;L;92D8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9EA'), U"\x927C") << desc_decomp_of(U'\x2F9EA') << "\n  Line: 2F9EA;CJK COMPATIBILITY IDEOGRAPH-2F9EA;Lo;0;L;927C;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9EB'), U"\x93F9") << desc_decomp_of(U'\x2F9EB') << "\n  Line: 2F9EB;CJK COMPATIBILITY IDEOGRAPH-2F9EB;Lo;0;L;93F9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9EC'), U"\x9415") << desc_decomp_of(U'\x2F9EC') << "\n  Line: 2F9EC;CJK COMPATIBILITY IDEOGRAPH-2F9EC;Lo;0;L;9415;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9ED'), U"\x28BFA") << desc_decomp_of(U'\x2F9ED') << "\n  Line: 2F9ED;CJK COMPATIBILITY IDEOGRAPH-2F9ED;Lo;0;L;28BFA;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9EE'), U"\x958B") << desc_decomp_of(U'\x2F9EE') << "\n  Line: 2F9EE;CJK COMPATIBILITY IDEOGRAPH-2F9EE;Lo;0;L;958B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9EF'), U"\x4995") << desc_decomp_of(U'\x2F9EF') << "\n  Line: 2F9EF;CJK COMPATIBILITY IDEOGRAPH-2F9EF;Lo;0;L;4995;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F0'), U"\x95B7") << desc_decomp_of(U'\x2F9F0') << "\n  Line: 2F9F0;CJK COMPATIBILITY IDEOGRAPH-2F9F0;Lo;0;L;95B7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F1'), U"\x28D77") << desc_decomp_of(U'\x2F9F1') << "\n  Line: 2F9F1;CJK COMPATIBILITY IDEOGRAPH-2F9F1;Lo;0;L;28D77;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F2'), U"\x49E6") << desc_decomp_of(U'\x2F9F2') << "\n  Line: 2F9F2;CJK COMPATIBILITY IDEOGRAPH-2F9F2;Lo;0;L;49E6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F3'), U"\x96C3") << desc_decomp_of(U'\x2F9F3') << "\n  Line: 2F9F3;CJK COMPATIBILITY IDEOGRAPH-2F9F3;Lo;0;L;96C3;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F4'), U"\x5DB2") << desc_decomp_of(U'\x2F9F4') << "\n  Line: 2F9F4;CJK COMPATIBILITY IDEOGRAPH-2F9F4;Lo;0;L;5DB2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F5'), U"\x9723") << desc_decomp_of(U'\x2F9F5') << "\n  Line: 2F9F5;CJK COMPATIBILITY IDEOGRAPH-2F9F5;Lo;0;L;9723;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F6'), U"\x29145") << desc_decomp_of(U'\x2F9F6') << "\n  Line: 2F9F6;CJK COMPATIBILITY IDEOGRAPH-2F9F6;Lo;0;L;29145;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F7'), U"\x2921A") << desc_decomp_of(U'\x2F9F7') << "\n  Line: 2F9F7;CJK COMPATIBILITY IDEOGRAPH-2F9F7;Lo;0;L;2921A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F8'), U"\x4A6E") << desc_decomp_of(U'\x2F9F8') << "\n  Line: 2F9F8;CJK COMPATIBILITY IDEOGRAPH-2F9F8;Lo;0;L;4A6E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9F9'), U"\x4A76") << desc_decomp_of(U'\x2F9F9') << "\n  Line: 2F9F9;CJK COMPATIBILITY IDEOGRAPH-2F9F9;Lo;0;L;4A76;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9FA'), U"\x97E0") << desc_decomp_of(U'\x2F9FA') << "\n  Line: 2F9FA;CJK COMPATIBILITY IDEOGRAPH-2F9FA;Lo;0;L;97E0;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9FB'), U"\x2940A") << desc_decomp_of(U'\x2F9FB') << "\n  Line: 2F9FB;CJK COMPATIBILITY IDEOGRAPH-2F9FB;Lo;0;L;2940A;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9FC'), U"\x4AB2") << desc_decomp_of(U'\x2F9FC') << "\n  Line: 2F9FC;CJK COMPATIBILITY IDEOGRAPH-2F9FC;Lo;0;L;4AB2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9FD'), U"\x29496") << desc_decomp_of(U'\x2F9FD') << "\n  Line: 2F9FD;CJK COMPATIBILITY IDEOGRAPH-2F9FD;Lo;0;L;29496;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9FE'), U"\x980B") << desc_decomp_of(U'\x2F9FE') << "\n  Line: 2F9FE;CJK COMPATIBILITY IDEOGRAPH-2F9FE;Lo;0;L;980B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2F9FF'), U"\x980B") << desc_decomp_of(U'\x2F9FF') << "\n  Line: 2F9FF;CJK COMPATIBILITY IDEOGRAPH-2F9FF;Lo;0;L;980B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA00'), U"\x9829") << desc_decomp_of(U'\x2FA00') << "\n  Line: 2FA00;CJK COMPATIBILITY IDEOGRAPH-2FA00;Lo;0;L;9829;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA01'), U"\x295B6") << desc_decomp_of(U'\x2FA01') << "\n  Line: 2FA01;CJK COMPATIBILITY IDEOGRAPH-2FA01;Lo;0;L;295B6;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA02'), U"\x98E2") << desc_decomp_of(U'\x2FA02') << "\n  Line: 2FA02;CJK COMPATIBILITY IDEOGRAPH-2FA02;Lo;0;L;98E2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA03'), U"\x4B33") << desc_decomp_of(U'\x2FA03') << "\n  Line: 2FA03;CJK COMPATIBILITY IDEOGRAPH-2FA03;Lo;0;L;4B33;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA04'), U"\x9929") << desc_decomp_of(U'\x2FA04') << "\n  Line: 2FA04;CJK COMPATIBILITY IDEOGRAPH-2FA04;Lo;0;L;9929;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA05'), U"\x99A7") << desc_decomp_of(U'\x2FA05') << "\n  Line: 2FA05;CJK COMPATIBILITY IDEOGRAPH-2FA05;Lo;0;L;99A7;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA06'), U"\x99C2") << desc_decomp_of(U'\x2FA06') << "\n  Line: 2FA06;CJK COMPATIBILITY IDEOGRAPH-2FA06;Lo;0;L;99C2;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA07'), U"\x99FE") << desc_decomp_of(U'\x2FA07') << "\n  Line: 2FA07;CJK COMPATIBILITY IDEOGRAPH-2FA07;Lo;0;L;99FE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA08'), U"\x4BCE") << desc_decomp_of(U'\x2FA08') << "\n  Line: 2FA08;CJK COMPATIBILITY IDEOGRAPH-2FA08;Lo;0;L;4BCE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA09'), U"\x29B30") << desc_decomp_of(U'\x2FA09') << "\n  Line: 2FA09;CJK COMPATIBILITY IDEOGRAPH-2FA09;Lo;0;L;29B30;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA0A'), U"\x9B12") << desc_decomp_of(U'\x2FA0A') << "\n  Line: 2FA0A;CJK COMPATIBILITY IDEOGRAPH-2FA0A;Lo;0;L;9B12;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA0B'), U"\x9C40") << desc_decomp_of(U'\x2FA0B') << "\n  Line: 2FA0B;CJK COMPATIBILITY IDEOGRAPH-2FA0B;Lo;0;L;9C40;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA0C'), U"\x9CFD") << desc_decomp_of(U'\x2FA0C') << "\n  Line: 2FA0C;CJK COMPATIBILITY IDEOGRAPH-2FA0C;Lo;0;L;9CFD;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA0D'), U"\x4CCE") << desc_decomp_of(U'\x2FA0D') << "\n  Line: 2FA0D;CJK COMPATIBILITY IDEOGRAPH-2FA0D;Lo;0;L;4CCE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA0E'), U"\x4CED") << desc_decomp_of(U'\x2FA0E') << "\n  Line: 2FA0E;CJK COMPATIBILITY IDEOGRAPH-2FA0E;Lo;0;L;4CED;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA0F'), U"\x9D67") << desc_decomp_of(U'\x2FA0F') << "\n  Line: 2FA0F;CJK COMPATIBILITY IDEOGRAPH-2FA0F;Lo;0;L;9D67;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA10'), U"\x2A0CE") << desc_decomp_of(U'\x2FA10') << "\n  Line: 2FA10;CJK COMPATIBILITY IDEOGRAPH-2FA10;Lo;0;L;2A0CE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA11'), U"\x4CF8") << desc_decomp_of(U'\x2FA11') << "\n  Line: 2FA11;CJK COMPATIBILITY IDEOGRAPH-2FA11;Lo;0;L;4CF8;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA12'), U"\x2A105") << desc_decomp_of(U'\x2FA12') << "\n  Line: 2FA12;CJK COMPATIBILITY IDEOGRAPH-2FA12;Lo;0;L;2A105;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA13'), U"\x2A20E") << desc_decomp_of(U'\x2FA13') << "\n  Line: 2FA13;CJK COMPATIBILITY IDEOGRAPH-2FA13;Lo;0;L;2A20E;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA14'), U"\x2A291") << desc_decomp_of(U'\x2FA14') << "\n  Line: 2FA14;CJK COMPATIBILITY IDEOGRAPH-2FA14;Lo;0;L;2A291;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA15'), U"\x9EBB") << desc_decomp_of(U'\x2FA15') << "\n  Line: 2FA15;CJK COMPATIBILITY IDEOGRAPH-2FA15;Lo;0;L;9EBB;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA16'), U"\x4D56") << desc_decomp_of(U'\x2FA16') << "\n  Line: 2FA16;CJK COMPATIBILITY IDEOGRAPH-2FA16;Lo;0;L;4D56;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA17'), U"\x9EF9") << desc_decomp_of(U'\x2FA17') << "\n  Line: 2FA17;CJK COMPATIBILITY IDEOGRAPH-2FA17;Lo;0;L;9EF9;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA18'), U"\x9EFE") << desc_decomp_of(U'\x2FA18') << "\n  Line: 2FA18;CJK COMPATIBILITY IDEOGRAPH-2FA18;Lo;0;L;9EFE;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA19'), U"\x9F05") << desc_decomp_of(U'\x2FA19') << "\n  Line: 2FA19;CJK COMPATIBILITY IDEOGRAPH-2FA19;Lo;0;L;9F05;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1A'), U"\x9F0F") << desc_decomp_of(U'\x2FA1A') << "\n  Line: 2FA1A;CJK COMPATIBILITY IDEOGRAPH-2FA1A;Lo;0;L;9F0F;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1B'), U"\x9F16") << desc_decomp_of(U'\x2FA1B') << "\n  Line: 2FA1B;CJK COMPATIBILITY IDEOGRAPH-2FA1B;Lo;0;L;9F16;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1C'), U"\x9F3B") << desc_decomp_of(U'\x2FA1C') << "\n  Line: 2FA1C;CJK COMPATIBILITY IDEOGRAPH-2FA1C;Lo;0;L;9F3B;;;;N;;;;;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x2FA1D'), U"\x2A600") << desc_decomp_of(U'\x2FA1D') << "\n  Line: 2FA1D;CJK COMPATIBILITY IDEOGRAPH-2FA1D;Lo;0;L;2A600;;;;N;;;;;";
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

    // clang-format off
    // Get more examples with these commands:
    //  Canonical-only decompositions:
    //   awk 'BEGIN{FS=";"; OF=";"} !/^\s*#|</ {tst = " " $6; gsub(/\s+/, " ", tst); if (tst != " ") { gsub(/\s+/, "\\\x", tst); print "    test_decomp(U\"\\\x" $1 "\", U\"" tst "\");"; } }' UnicodeData.txt
    // clang-format on

    test_decomp(u32string{U"\0", 1}, u32string{U"\0", 1});
    test_decomp(U"\1", U"\1");
    test_decomp(U"\x00A2", U"\x00A2");
    test_decomp(U"\x10A2", U"\x10A2");

    test_decomp(U"\x00C0", U"\x0041\x0300");
    test_decomp(U"\x00C1", U"\x0041\x0301");
    test_decomp(U"\x00C2", U"\x0041\x0302");
    test_decomp(U"\x00C3", U"\x0041\x0303");
    test_decomp(U"\x00C4", U"\x0041\x0308");
    test_decomp(U"\x00C5", U"\x0041\x030A");
    test_decomp(U"\x00C7", U"\x0043\x0327");
    test_decomp(U"\x00C8", U"\x0045\x0300");
    test_decomp(U"\x00C9", U"\x0045\x0301");
    test_decomp(U"\x00CA", U"\x0045\x0302");
    test_decomp(U"\x00CB", U"\x0045\x0308");
    test_decomp(U"\x00CC", U"\x0049\x0300");
    test_decomp(U"\x00CD", U"\x0049\x0301");
    test_decomp(U"\x00CE", U"\x0049\x0302");
    test_decomp(U"\x00CF", U"\x0049\x0308");
    test_decomp(U"\x00D1", U"\x004E\x0303");
    test_decomp(U"\x00D2", U"\x004F\x0300");
    test_decomp(U"\x00D3", U"\x004F\x0301");
    test_decomp(U"\x00D4", U"\x004F\x0302");
    test_decomp(U"\x00D5", U"\x004F\x0303");
    test_decomp(U"\x00D6", U"\x004F\x0308");
    test_decomp(U"\x00D9", U"\x0055\x0300");
    test_decomp(U"\x00DA", U"\x0055\x0301");
    test_decomp(U"\x00DB", U"\x0055\x0302");
    test_decomp(U"\x00DC", U"\x0055\x0308");
    test_decomp(U"\x00DD", U"\x0059\x0301");
    test_decomp(U"\x00E0", U"\x0061\x0300");
    test_decomp(U"\x00E1", U"\x0061\x0301");
    test_decomp(U"\x00E2", U"\x0061\x0302");
    test_decomp(U"\x00E3", U"\x0061\x0303");
    test_decomp(U"\x00E4", U"\x0061\x0308");
    test_decomp(U"\x00E5", U"\x0061\x030A");
    test_decomp(U"\x00E7", U"\x0063\x0327");
    test_decomp(U"\x00E8", U"\x0065\x0300");
    test_decomp(U"\x00E9", U"\x0065\x0301");
    test_decomp(U"\x00EA", U"\x0065\x0302");
    test_decomp(U"\x00EB", U"\x0065\x0308");
    test_decomp(U"\x00EC", U"\x0069\x0300");
    test_decomp(U"\x00ED", U"\x0069\x0301");
    test_decomp(U"\x00EE", U"\x0069\x0302");
    test_decomp(U"\x00EF", U"\x0069\x0308");
    test_decomp(U"\x00F1", U"\x006E\x0303");
    test_decomp(U"\x00F2", U"\x006F\x0300");
    test_decomp(U"\x00F3", U"\x006F\x0301");
    test_decomp(U"\x00F4", U"\x006F\x0302");
    test_decomp(U"\x00F5", U"\x006F\x0303");
    test_decomp(U"\x00F6", U"\x006F\x0308");
    test_decomp(U"\x00F9", U"\x0075\x0300");
    test_decomp(U"\x00FA", U"\x0075\x0301");
    test_decomp(U"\x00FB", U"\x0075\x0302");
    test_decomp(U"\x00FC", U"\x0075\x0308");
    test_decomp(U"\x00FD", U"\x0079\x0301");
    test_decomp(U"\x00FF", U"\x0079\x0308");
    test_decomp(U"\x0100", U"\x0041\x0304");
    test_decomp(U"\x0101", U"\x0061\x0304");
    test_decomp(U"\x0102", U"\x0041\x0306");
    test_decomp(U"\x0103", U"\x0061\x0306");
    test_decomp(U"\x0104", U"\x0041\x0328");
    test_decomp(U"\x0105", U"\x0061\x0328");
    test_decomp(U"\x0106", U"\x0043\x0301");
    test_decomp(U"\x0107", U"\x0063\x0301");
    test_decomp(U"\x0108", U"\x0043\x0302");
    test_decomp(U"\x0109", U"\x0063\x0302");
    test_decomp(U"\x010A", U"\x0043\x0307");
    test_decomp(U"\x010B", U"\x0063\x0307");
    test_decomp(U"\x010C", U"\x0043\x030C");
    test_decomp(U"\x010D", U"\x0063\x030C");
    test_decomp(U"\x010E", U"\x0044\x030C");
    test_decomp(U"\x010F", U"\x0064\x030C");
    test_decomp(U"\x0112", U"\x0045\x0304");
    test_decomp(U"\x0113", U"\x0065\x0304");
    test_decomp(U"\x0114", U"\x0045\x0306");
    test_decomp(U"\x0115", U"\x0065\x0306");
    test_decomp(U"\x0116", U"\x0045\x0307");
    test_decomp(U"\x0117", U"\x0065\x0307");
    test_decomp(U"\x0118", U"\x0045\x0328");
    test_decomp(U"\x0119", U"\x0065\x0328");
    test_decomp(U"\x011A", U"\x0045\x030C");
    test_decomp(U"\x011B", U"\x0065\x030C");
    test_decomp(U"\x011C", U"\x0047\x0302");
    test_decomp(U"\x011D", U"\x0067\x0302");
    test_decomp(U"\x011E", U"\x0047\x0306");
    test_decomp(U"\x011F", U"\x0067\x0306");
    test_decomp(U"\x0120", U"\x0047\x0307");
    test_decomp(U"\x0121", U"\x0067\x0307");
    test_decomp(U"\x0122", U"\x0047\x0327");
    test_decomp(U"\x0123", U"\x0067\x0327");
    test_decomp(U"\x0124", U"\x0048\x0302");
    test_decomp(U"\x0125", U"\x0068\x0302");
    test_decomp(U"\x0128", U"\x0049\x0303");
    test_decomp(U"\x0129", U"\x0069\x0303");
    test_decomp(U"\x012A", U"\x0049\x0304");
    test_decomp(U"\x012B", U"\x0069\x0304");
    test_decomp(U"\x012C", U"\x0049\x0306");
    test_decomp(U"\x012D", U"\x0069\x0306");
    test_decomp(U"\x012E", U"\x0049\x0328");
    test_decomp(U"\x012F", U"\x0069\x0328");
    test_decomp(U"\x0130", U"\x0049\x0307");
    test_decomp(U"\x0134", U"\x004A\x0302");
    test_decomp(U"\x0135", U"\x006A\x0302");
    test_decomp(U"\x0136", U"\x004B\x0327");
    test_decomp(U"\x0137", U"\x006B\x0327");
    test_decomp(U"\x0139", U"\x004C\x0301");
    test_decomp(U"\x013A", U"\x006C\x0301");
    test_decomp(U"\x013B", U"\x004C\x0327");
    test_decomp(U"\x013C", U"\x006C\x0327");
    test_decomp(U"\x013D", U"\x004C\x030C");
    test_decomp(U"\x013E", U"\x006C\x030C");
    test_decomp(U"\x0143", U"\x004E\x0301");
    test_decomp(U"\x0144", U"\x006E\x0301");
    test_decomp(U"\x0145", U"\x004E\x0327");
    test_decomp(U"\x0146", U"\x006E\x0327");
    test_decomp(U"\x0147", U"\x004E\x030C");
    test_decomp(U"\x0148", U"\x006E\x030C");
    test_decomp(U"\x014C", U"\x004F\x0304");
    test_decomp(U"\x014D", U"\x006F\x0304");
    test_decomp(U"\x014E", U"\x004F\x0306");
    test_decomp(U"\x014F", U"\x006F\x0306");
    test_decomp(U"\x0150", U"\x004F\x030B");
    test_decomp(U"\x0151", U"\x006F\x030B");
    test_decomp(U"\x0154", U"\x0052\x0301");
    test_decomp(U"\x0155", U"\x0072\x0301");
    test_decomp(U"\x0156", U"\x0052\x0327");
    test_decomp(U"\x0157", U"\x0072\x0327");
    test_decomp(U"\x0158", U"\x0052\x030C");
    test_decomp(U"\x0159", U"\x0072\x030C");
    test_decomp(U"\x015A", U"\x0053\x0301");
    test_decomp(U"\x015B", U"\x0073\x0301");
    test_decomp(U"\x015C", U"\x0053\x0302");
    test_decomp(U"\x015D", U"\x0073\x0302");
    test_decomp(U"\x015E", U"\x0053\x0327");
    test_decomp(U"\x015F", U"\x0073\x0327");
    test_decomp(U"\x0160", U"\x0053\x030C");
    test_decomp(U"\x0161", U"\x0073\x030C");
    test_decomp(U"\x0162", U"\x0054\x0327");
    test_decomp(U"\x0163", U"\x0074\x0327");
    test_decomp(U"\x0164", U"\x0054\x030C");
    test_decomp(U"\x0165", U"\x0074\x030C");
    test_decomp(U"\x0168", U"\x0055\x0303");
    test_decomp(U"\x0169", U"\x0075\x0303");
    test_decomp(U"\x016A", U"\x0055\x0304");
    test_decomp(U"\x016B", U"\x0075\x0304");
    test_decomp(U"\x016C", U"\x0055\x0306");
    test_decomp(U"\x016D", U"\x0075\x0306");
    test_decomp(U"\x016E", U"\x0055\x030A");
    test_decomp(U"\x016F", U"\x0075\x030A");
    test_decomp(U"\x0170", U"\x0055\x030B");
    test_decomp(U"\x0171", U"\x0075\x030B");
    test_decomp(U"\x0172", U"\x0055\x0328");
    test_decomp(U"\x0173", U"\x0075\x0328");
    test_decomp(U"\x0174", U"\x0057\x0302");
    test_decomp(U"\x0175", U"\x0077\x0302");
    test_decomp(U"\x0176", U"\x0059\x0302");
    test_decomp(U"\x0177", U"\x0079\x0302");
    test_decomp(U"\x0178", U"\x0059\x0308");
    test_decomp(U"\x0179", U"\x005A\x0301");
    test_decomp(U"\x017A", U"\x007A\x0301");
    test_decomp(U"\x017B", U"\x005A\x0307");
    test_decomp(U"\x017C", U"\x007A\x0307");
    test_decomp(U"\x017D", U"\x005A\x030C");
    test_decomp(U"\x017E", U"\x007A\x030C");
    test_decomp(U"\x01A0", U"\x004F\x031B");
    test_decomp(U"\x01A1", U"\x006F\x031B");
    test_decomp(U"\x01AF", U"\x0055\x031B");
    test_decomp(U"\x01B0", U"\x0075\x031B");
    test_decomp(U"\x01CD", U"\x0041\x030C");
    test_decomp(U"\x01CE", U"\x0061\x030C");
    test_decomp(U"\x01CF", U"\x0049\x030C");
    test_decomp(U"\x01D0", U"\x0069\x030C");
    test_decomp(U"\x01D1", U"\x004F\x030C");
    test_decomp(U"\x01D2", U"\x006F\x030C");
    test_decomp(U"\x01D3", U"\x0055\x030C");
    test_decomp(U"\x01D4", U"\x0075\x030C");
    test_decomp(U"\x01D5", U"\x00DC\x0304");
    test_decomp(U"\x01D6", U"\x00FC\x0304");
    test_decomp(U"\x01D7", U"\x00DC\x0301");
    test_decomp(U"\x01D8", U"\x00FC\x0301");
    test_decomp(U"\x01D9", U"\x00DC\x030C");
    test_decomp(U"\x01DA", U"\x00FC\x030C");
    test_decomp(U"\x01DB", U"\x00DC\x0300");
    test_decomp(U"\x01DC", U"\x00FC\x0300");
    test_decomp(U"\x01DE", U"\x00C4\x0304");
    test_decomp(U"\x01DF", U"\x00E4\x0304");
    test_decomp(U"\x01E0", U"\x0226\x0304");
    test_decomp(U"\x01E1", U"\x0227\x0304");
    test_decomp(U"\x01E2", U"\x00C6\x0304");
    test_decomp(U"\x01E3", U"\x00E6\x0304");
    test_decomp(U"\x01E6", U"\x0047\x030C");
    test_decomp(U"\x01E7", U"\x0067\x030C");
    test_decomp(U"\x01E8", U"\x004B\x030C");
    test_decomp(U"\x01E9", U"\x006B\x030C");
    test_decomp(U"\x01EA", U"\x004F\x0328");
    test_decomp(U"\x01EB", U"\x006F\x0328");
    test_decomp(U"\x01EC", U"\x01EA\x0304");
    test_decomp(U"\x01ED", U"\x01EB\x0304");
    test_decomp(U"\x01EE", U"\x01B7\x030C");
    test_decomp(U"\x01EF", U"\x0292\x030C");
    test_decomp(U"\x01F0", U"\x006A\x030C");
    test_decomp(U"\x01F4", U"\x0047\x0301");
    test_decomp(U"\x01F5", U"\x0067\x0301");
    test_decomp(U"\x01F8", U"\x004E\x0300");
    test_decomp(U"\x01F9", U"\x006E\x0300");
    test_decomp(U"\x01FA", U"\x00C5\x0301");
    test_decomp(U"\x01FB", U"\x00E5\x0301");
    test_decomp(U"\x01FC", U"\x00C6\x0301");
    test_decomp(U"\x01FD", U"\x00E6\x0301");
    test_decomp(U"\x01FE", U"\x00D8\x0301");
    test_decomp(U"\x01FF", U"\x00F8\x0301");
    test_decomp(U"\x0200", U"\x0041\x030F");
    test_decomp(U"\x0201", U"\x0061\x030F");
    test_decomp(U"\x0202", U"\x0041\x0311");
    test_decomp(U"\x0203", U"\x0061\x0311");
    test_decomp(U"\x0204", U"\x0045\x030F");
    test_decomp(U"\x0205", U"\x0065\x030F");
    test_decomp(U"\x0206", U"\x0045\x0311");
    test_decomp(U"\x0207", U"\x0065\x0311");
    test_decomp(U"\x0208", U"\x0049\x030F");
    test_decomp(U"\x0209", U"\x0069\x030F");
    test_decomp(U"\x020A", U"\x0049\x0311");
    test_decomp(U"\x020B", U"\x0069\x0311");
    test_decomp(U"\x020C", U"\x004F\x030F");
    test_decomp(U"\x020D", U"\x006F\x030F");
    test_decomp(U"\x020E", U"\x004F\x0311");
    test_decomp(U"\x020F", U"\x006F\x0311");
    test_decomp(U"\x0210", U"\x0052\x030F");
    test_decomp(U"\x0211", U"\x0072\x030F");
    test_decomp(U"\x0212", U"\x0052\x0311");
    test_decomp(U"\x0213", U"\x0072\x0311");
    test_decomp(U"\x0214", U"\x0055\x030F");
    test_decomp(U"\x0215", U"\x0075\x030F");
    test_decomp(U"\x0216", U"\x0055\x0311");
    test_decomp(U"\x0217", U"\x0075\x0311");
    test_decomp(U"\x0218", U"\x0053\x0326");
    test_decomp(U"\x0219", U"\x0073\x0326");
    test_decomp(U"\x021A", U"\x0054\x0326");
    test_decomp(U"\x021B", U"\x0074\x0326");
    test_decomp(U"\x021E", U"\x0048\x030C");
    test_decomp(U"\x021F", U"\x0068\x030C");
    test_decomp(U"\x0226", U"\x0041\x0307");
    test_decomp(U"\x0227", U"\x0061\x0307");
    test_decomp(U"\x0228", U"\x0045\x0327");
    test_decomp(U"\x0229", U"\x0065\x0327");
    test_decomp(U"\x022A", U"\x00D6\x0304");
    test_decomp(U"\x022B", U"\x00F6\x0304");
    test_decomp(U"\x022C", U"\x00D5\x0304");
    test_decomp(U"\x022D", U"\x00F5\x0304");
    test_decomp(U"\x022E", U"\x004F\x0307");
    test_decomp(U"\x022F", U"\x006F\x0307");
    test_decomp(U"\x0230", U"\x022E\x0304");
    test_decomp(U"\x0231", U"\x022F\x0304");
    test_decomp(U"\x0232", U"\x0059\x0304");
    test_decomp(U"\x0233", U"\x0079\x0304");
    test_decomp(U"\x0340", U"\x0300");
    test_decomp(U"\x0341", U"\x0301");
    test_decomp(U"\x0343", U"\x0313");
    test_decomp(U"\x0344", U"\x0308\x0301");
    test_decomp(U"\x0374", U"\x02B9");
    test_decomp(U"\x037E", U"\x003B");
    test_decomp(U"\x0385", U"\x00A8\x0301");
    test_decomp(U"\x0386", U"\x0391\x0301");
    test_decomp(U"\x0387", U"\x00B7");
    test_decomp(U"\x0388", U"\x0395\x0301");
    test_decomp(U"\x0389", U"\x0397\x0301");
    test_decomp(U"\x038A", U"\x0399\x0301");
    test_decomp(U"\x038C", U"\x039F\x0301");
    test_decomp(U"\x038E", U"\x03A5\x0301");
    test_decomp(U"\x038F", U"\x03A9\x0301");
    test_decomp(U"\x0390", U"\x03CA\x0301");
    test_decomp(U"\x03AA", U"\x0399\x0308");
    test_decomp(U"\x03AB", U"\x03A5\x0308");
    test_decomp(U"\x03AC", U"\x03B1\x0301");
    test_decomp(U"\x03AD", U"\x03B5\x0301");
    test_decomp(U"\x03AE", U"\x03B7\x0301");
    test_decomp(U"\x03AF", U"\x03B9\x0301");
    test_decomp(U"\x03B0", U"\x03CB\x0301");
    test_decomp(U"\x03CA", U"\x03B9\x0308");
    test_decomp(U"\x03CB", U"\x03C5\x0308");
    test_decomp(U"\x03CC", U"\x03BF\x0301");
    test_decomp(U"\x03CD", U"\x03C5\x0301");
    test_decomp(U"\x03CE", U"\x03C9\x0301");
    test_decomp(U"\x03D3", U"\x03D2\x0301");
    test_decomp(U"\x03D4", U"\x03D2\x0308");
    test_decomp(U"\x0400", U"\x0415\x0300");
    test_decomp(U"\x0401", U"\x0415\x0308");
    test_decomp(U"\x0403", U"\x0413\x0301");
    test_decomp(U"\x0407", U"\x0406\x0308");
    test_decomp(U"\x040C", U"\x041A\x0301");
    test_decomp(U"\x040D", U"\x0418\x0300");
    test_decomp(U"\x040E", U"\x0423\x0306");
    test_decomp(U"\x0419", U"\x0418\x0306");
    test_decomp(U"\x0439", U"\x0438\x0306");
    test_decomp(U"\x0450", U"\x0435\x0300");
    test_decomp(U"\x0451", U"\x0435\x0308");
    test_decomp(U"\x0453", U"\x0433\x0301");
    test_decomp(U"\x0457", U"\x0456\x0308");
    test_decomp(U"\x045C", U"\x043A\x0301");
    test_decomp(U"\x045D", U"\x0438\x0300");
    test_decomp(U"\x045E", U"\x0443\x0306");
    test_decomp(U"\x0476", U"\x0474\x030F");
    test_decomp(U"\x0477", U"\x0475\x030F");
    test_decomp(U"\x04C1", U"\x0416\x0306");
    test_decomp(U"\x04C2", U"\x0436\x0306");
    test_decomp(U"\x04D0", U"\x0410\x0306");
    test_decomp(U"\x04D1", U"\x0430\x0306");
    test_decomp(U"\x04D2", U"\x0410\x0308");
    test_decomp(U"\x04D3", U"\x0430\x0308");
    test_decomp(U"\x04D6", U"\x0415\x0306");
    test_decomp(U"\x04D7", U"\x0435\x0306");
    test_decomp(U"\x04DA", U"\x04D8\x0308");
    test_decomp(U"\x04DB", U"\x04D9\x0308");
    test_decomp(U"\x04DC", U"\x0416\x0308");
    test_decomp(U"\x04DD", U"\x0436\x0308");
    test_decomp(U"\x04DE", U"\x0417\x0308");
    test_decomp(U"\x04DF", U"\x0437\x0308");
    test_decomp(U"\x04E2", U"\x0418\x0304");
    test_decomp(U"\x04E3", U"\x0438\x0304");
    test_decomp(U"\x04E4", U"\x0418\x0308");
    test_decomp(U"\x04E5", U"\x0438\x0308");
    test_decomp(U"\x04E6", U"\x041E\x0308");
    test_decomp(U"\x04E7", U"\x043E\x0308");
    test_decomp(U"\x04EA", U"\x04E8\x0308");
    test_decomp(U"\x04EB", U"\x04E9\x0308");
    test_decomp(U"\x04EC", U"\x042D\x0308");
    test_decomp(U"\x04ED", U"\x044D\x0308");
    test_decomp(U"\x04EE", U"\x0423\x0304");
    test_decomp(U"\x04EF", U"\x0443\x0304");
    test_decomp(U"\x04F0", U"\x0423\x0308");
    test_decomp(U"\x04F1", U"\x0443\x0308");
    test_decomp(U"\x04F2", U"\x0423\x030B");
    test_decomp(U"\x04F3", U"\x0443\x030B");
    test_decomp(U"\x04F4", U"\x0427\x0308");
    test_decomp(U"\x04F5", U"\x0447\x0308");
    test_decomp(U"\x04F8", U"\x042B\x0308");
    test_decomp(U"\x04F9", U"\x044B\x0308");
    test_decomp(U"\x0622", U"\x0627\x0653");
    test_decomp(U"\x0623", U"\x0627\x0654");
    test_decomp(U"\x0624", U"\x0648\x0654");
    test_decomp(U"\x0625", U"\x0627\x0655");
    test_decomp(U"\x0626", U"\x064A\x0654");
    test_decomp(U"\x06C0", U"\x06D5\x0654");
    test_decomp(U"\x06C2", U"\x06C1\x0654");
    test_decomp(U"\x06D3", U"\x06D2\x0654");
    test_decomp(U"\x0929", U"\x0928\x093C");
    test_decomp(U"\x0931", U"\x0930\x093C");
    test_decomp(U"\x0934", U"\x0933\x093C");
    test_decomp(U"\x0958", U"\x0915\x093C");
    test_decomp(U"\x0959", U"\x0916\x093C");
    test_decomp(U"\x095A", U"\x0917\x093C");
    test_decomp(U"\x095B", U"\x091C\x093C");
    test_decomp(U"\x095C", U"\x0921\x093C");
    test_decomp(U"\x095D", U"\x0922\x093C");
    test_decomp(U"\x095E", U"\x092B\x093C");
    test_decomp(U"\x095F", U"\x092F\x093C");
    test_decomp(U"\x09CB", U"\x09C7\x09BE");
    test_decomp(U"\x09CC", U"\x09C7\x09D7");
    test_decomp(U"\x09DC", U"\x09A1\x09BC");
    test_decomp(U"\x09DD", U"\x09A2\x09BC");
    test_decomp(U"\x09DF", U"\x09AF\x09BC");
    test_decomp(U"\x0A33", U"\x0A32\x0A3C");
    test_decomp(U"\x0A36", U"\x0A38\x0A3C");
    test_decomp(U"\x0A59", U"\x0A16\x0A3C");
    test_decomp(U"\x0A5A", U"\x0A17\x0A3C");
    test_decomp(U"\x0A5B", U"\x0A1C\x0A3C");
    test_decomp(U"\x0A5E", U"\x0A2B\x0A3C");
    test_decomp(U"\x0B48", U"\x0B47\x0B56");
    test_decomp(U"\x0B4B", U"\x0B47\x0B3E");
    test_decomp(U"\x0B4C", U"\x0B47\x0B57");
    test_decomp(U"\x0B5C", U"\x0B21\x0B3C");
    test_decomp(U"\x0B5D", U"\x0B22\x0B3C");
    test_decomp(U"\x0B94", U"\x0B92\x0BD7");
    test_decomp(U"\x0BCA", U"\x0BC6\x0BBE");
    test_decomp(U"\x0BCB", U"\x0BC7\x0BBE");
    test_decomp(U"\x0BCC", U"\x0BC6\x0BD7");
    test_decomp(U"\x0C48", U"\x0C46\x0C56");
    test_decomp(U"\x0CC0", U"\x0CBF\x0CD5");
    test_decomp(U"\x0CC7", U"\x0CC6\x0CD5");
    test_decomp(U"\x0CC8", U"\x0CC6\x0CD6");
    test_decomp(U"\x0CCA", U"\x0CC6\x0CC2");
    test_decomp(U"\x0CCB", U"\x0CCA\x0CD5");
    test_decomp(U"\x0D4A", U"\x0D46\x0D3E");
    test_decomp(U"\x0D4B", U"\x0D47\x0D3E");
    test_decomp(U"\x0D4C", U"\x0D46\x0D57");
    test_decomp(U"\x0DDA", U"\x0DD9\x0DCA");
    test_decomp(U"\x0DDC", U"\x0DD9\x0DCF");
    test_decomp(U"\x0DDD", U"\x0DDC\x0DCA");
    test_decomp(U"\x0DDE", U"\x0DD9\x0DDF");
    test_decomp(U"\x0F43", U"\x0F42\x0FB7");
    test_decomp(U"\x0F4D", U"\x0F4C\x0FB7");
    test_decomp(U"\x0F52", U"\x0F51\x0FB7");
    test_decomp(U"\x0F57", U"\x0F56\x0FB7");
    test_decomp(U"\x0F5C", U"\x0F5B\x0FB7");
    test_decomp(U"\x0F69", U"\x0F40\x0FB5");
    test_decomp(U"\x0F73", U"\x0F71\x0F72");
    test_decomp(U"\x0F75", U"\x0F71\x0F74");
    test_decomp(U"\x0F76", U"\x0FB2\x0F80");
    test_decomp(U"\x0F78", U"\x0FB3\x0F80");
    test_decomp(U"\x0F81", U"\x0F71\x0F80");
    test_decomp(U"\x0F93", U"\x0F92\x0FB7");
    test_decomp(U"\x0F9D", U"\x0F9C\x0FB7");
    test_decomp(U"\x0FA2", U"\x0FA1\x0FB7");
    test_decomp(U"\x0FA7", U"\x0FA6\x0FB7");
    test_decomp(U"\x0FAC", U"\x0FAB\x0FB7");
    test_decomp(U"\x0FB9", U"\x0F90\x0FB5");
    test_decomp(U"\x1026", U"\x1025\x102E");
    test_decomp(U"\x1B06", U"\x1B05\x1B35");
    test_decomp(U"\x1B08", U"\x1B07\x1B35");
    test_decomp(U"\x1B0A", U"\x1B09\x1B35");
    test_decomp(U"\x1B0C", U"\x1B0B\x1B35");
    test_decomp(U"\x1B0E", U"\x1B0D\x1B35");
    test_decomp(U"\x1B12", U"\x1B11\x1B35");
    test_decomp(U"\x1B3B", U"\x1B3A\x1B35");
    test_decomp(U"\x1B3D", U"\x1B3C\x1B35");
    test_decomp(U"\x1B40", U"\x1B3E\x1B35");
    test_decomp(U"\x1B41", U"\x1B3F\x1B35");
    test_decomp(U"\x1B43", U"\x1B42\x1B35");
    test_decomp(U"\x1E00", U"\x0041\x0325");
    test_decomp(U"\x1E01", U"\x0061\x0325");
    test_decomp(U"\x1E02", U"\x0042\x0307");
    test_decomp(U"\x1E03", U"\x0062\x0307");
    test_decomp(U"\x1E04", U"\x0042\x0323");
    test_decomp(U"\x1E05", U"\x0062\x0323");
    test_decomp(U"\x1E06", U"\x0042\x0331");
    test_decomp(U"\x1E07", U"\x0062\x0331");
    test_decomp(U"\x1E08", U"\x00C7\x0301");
    test_decomp(U"\x1E09", U"\x00E7\x0301");
    test_decomp(U"\x1E0A", U"\x0044\x0307");
    test_decomp(U"\x1E0B", U"\x0064\x0307");
    test_decomp(U"\x1E0C", U"\x0044\x0323");
    test_decomp(U"\x1E0D", U"\x0064\x0323");
    test_decomp(U"\x1E0E", U"\x0044\x0331");
    test_decomp(U"\x1E0F", U"\x0064\x0331");
    test_decomp(U"\x1E10", U"\x0044\x0327");
    test_decomp(U"\x1E11", U"\x0064\x0327");
    test_decomp(U"\x1E12", U"\x0044\x032D");
    test_decomp(U"\x1E13", U"\x0064\x032D");
    test_decomp(U"\x1E14", U"\x0112\x0300");
    test_decomp(U"\x1E15", U"\x0113\x0300");
    test_decomp(U"\x1E16", U"\x0112\x0301");
    test_decomp(U"\x1E17", U"\x0113\x0301");
    test_decomp(U"\x1E18", U"\x0045\x032D");
    test_decomp(U"\x1E19", U"\x0065\x032D");
    test_decomp(U"\x1E1A", U"\x0045\x0330");
    test_decomp(U"\x1E1B", U"\x0065\x0330");
    test_decomp(U"\x1E1C", U"\x0228\x0306");
    test_decomp(U"\x1E1D", U"\x0229\x0306");
    test_decomp(U"\x1E1E", U"\x0046\x0307");
    test_decomp(U"\x1E1F", U"\x0066\x0307");
    test_decomp(U"\x1E20", U"\x0047\x0304");
    test_decomp(U"\x1E21", U"\x0067\x0304");
    test_decomp(U"\x1E22", U"\x0048\x0307");
    test_decomp(U"\x1E23", U"\x0068\x0307");
    test_decomp(U"\x1E24", U"\x0048\x0323");
    test_decomp(U"\x1E25", U"\x0068\x0323");
    test_decomp(U"\x1E26", U"\x0048\x0308");
    test_decomp(U"\x1E27", U"\x0068\x0308");
    test_decomp(U"\x1E28", U"\x0048\x0327");
    test_decomp(U"\x1E29", U"\x0068\x0327");
    test_decomp(U"\x1E2A", U"\x0048\x032E");
    test_decomp(U"\x1E2B", U"\x0068\x032E");
    test_decomp(U"\x1E2C", U"\x0049\x0330");
    test_decomp(U"\x1E2D", U"\x0069\x0330");
    test_decomp(U"\x1E2E", U"\x00CF\x0301");
    test_decomp(U"\x1E2F", U"\x00EF\x0301");
    test_decomp(U"\x1E30", U"\x004B\x0301");
    test_decomp(U"\x1E31", U"\x006B\x0301");
    test_decomp(U"\x1E32", U"\x004B\x0323");
    test_decomp(U"\x1E33", U"\x006B\x0323");
    test_decomp(U"\x1E34", U"\x004B\x0331");
    test_decomp(U"\x1E35", U"\x006B\x0331");
    test_decomp(U"\x1E36", U"\x004C\x0323");
    test_decomp(U"\x1E37", U"\x006C\x0323");
    test_decomp(U"\x1E38", U"\x1E36\x0304");
    test_decomp(U"\x1E39", U"\x1E37\x0304");
    test_decomp(U"\x1E3A", U"\x004C\x0331");
    test_decomp(U"\x1E3B", U"\x006C\x0331");
    test_decomp(U"\x1E3C", U"\x004C\x032D");
    test_decomp(U"\x1E3D", U"\x006C\x032D");
    test_decomp(U"\x1E3E", U"\x004D\x0301");
    test_decomp(U"\x1E3F", U"\x006D\x0301");
    test_decomp(U"\x1E40", U"\x004D\x0307");
    test_decomp(U"\x1E41", U"\x006D\x0307");
    test_decomp(U"\x1E42", U"\x004D\x0323");
    test_decomp(U"\x1E43", U"\x006D\x0323");
    test_decomp(U"\x1E44", U"\x004E\x0307");
    test_decomp(U"\x1E45", U"\x006E\x0307");
    test_decomp(U"\x1E46", U"\x004E\x0323");
    test_decomp(U"\x1E47", U"\x006E\x0323");
    test_decomp(U"\x1E48", U"\x004E\x0331");
    test_decomp(U"\x1E49", U"\x006E\x0331");
    test_decomp(U"\x1E4A", U"\x004E\x032D");
    test_decomp(U"\x1E4B", U"\x006E\x032D");
    test_decomp(U"\x1E4C", U"\x00D5\x0301");
    test_decomp(U"\x1E4D", U"\x00F5\x0301");
    test_decomp(U"\x1E4E", U"\x00D5\x0308");
    test_decomp(U"\x1E4F", U"\x00F5\x0308");
    test_decomp(U"\x1E50", U"\x014C\x0300");
    test_decomp(U"\x1E51", U"\x014D\x0300");
    test_decomp(U"\x1E52", U"\x014C\x0301");
    test_decomp(U"\x1E53", U"\x014D\x0301");
    test_decomp(U"\x1E54", U"\x0050\x0301");
    test_decomp(U"\x1E55", U"\x0070\x0301");
    test_decomp(U"\x1E56", U"\x0050\x0307");
    test_decomp(U"\x1E57", U"\x0070\x0307");
    test_decomp(U"\x1E58", U"\x0052\x0307");
    test_decomp(U"\x1E59", U"\x0072\x0307");
    test_decomp(U"\x1E5A", U"\x0052\x0323");
    test_decomp(U"\x1E5B", U"\x0072\x0323");
    test_decomp(U"\x1E5C", U"\x1E5A\x0304");
    test_decomp(U"\x1E5D", U"\x1E5B\x0304");
    test_decomp(U"\x1E5E", U"\x0052\x0331");
    test_decomp(U"\x1E5F", U"\x0072\x0331");
    test_decomp(U"\x1E60", U"\x0053\x0307");
    test_decomp(U"\x1E61", U"\x0073\x0307");
    test_decomp(U"\x1E62", U"\x0053\x0323");
    test_decomp(U"\x1E63", U"\x0073\x0323");
    test_decomp(U"\x1E64", U"\x015A\x0307");
    test_decomp(U"\x1E65", U"\x015B\x0307");
    test_decomp(U"\x1E66", U"\x0160\x0307");
    test_decomp(U"\x1E67", U"\x0161\x0307");
    test_decomp(U"\x1E68", U"\x1E62\x0307");
    test_decomp(U"\x1E69", U"\x1E63\x0307");
    test_decomp(U"\x1E6A", U"\x0054\x0307");
    test_decomp(U"\x1E6B", U"\x0074\x0307");
    test_decomp(U"\x1E6C", U"\x0054\x0323");
    test_decomp(U"\x1E6D", U"\x0074\x0323");
    test_decomp(U"\x1E6E", U"\x0054\x0331");
    test_decomp(U"\x1E6F", U"\x0074\x0331");
    test_decomp(U"\x1E70", U"\x0054\x032D");
    test_decomp(U"\x1E71", U"\x0074\x032D");
    test_decomp(U"\x1E72", U"\x0055\x0324");
    test_decomp(U"\x1E73", U"\x0075\x0324");
    test_decomp(U"\x1E74", U"\x0055\x0330");
    test_decomp(U"\x1E75", U"\x0075\x0330");
    test_decomp(U"\x1E76", U"\x0055\x032D");
    test_decomp(U"\x1E77", U"\x0075\x032D");
    test_decomp(U"\x1E78", U"\x0168\x0301");
    test_decomp(U"\x1E79", U"\x0169\x0301");
    test_decomp(U"\x1E7A", U"\x016A\x0308");
    test_decomp(U"\x1E7B", U"\x016B\x0308");
    test_decomp(U"\x1E7C", U"\x0056\x0303");
    test_decomp(U"\x1E7D", U"\x0076\x0303");
    test_decomp(U"\x1E7E", U"\x0056\x0323");
    test_decomp(U"\x1E7F", U"\x0076\x0323");
    test_decomp(U"\x1E80", U"\x0057\x0300");
    test_decomp(U"\x1E81", U"\x0077\x0300");
    test_decomp(U"\x1E82", U"\x0057\x0301");
    test_decomp(U"\x1E83", U"\x0077\x0301");
    test_decomp(U"\x1E84", U"\x0057\x0308");
    test_decomp(U"\x1E85", U"\x0077\x0308");
    test_decomp(U"\x1E86", U"\x0057\x0307");
    test_decomp(U"\x1E87", U"\x0077\x0307");
    test_decomp(U"\x1E88", U"\x0057\x0323");
    test_decomp(U"\x1E89", U"\x0077\x0323");
    test_decomp(U"\x1E8A", U"\x0058\x0307");
    test_decomp(U"\x1E8B", U"\x0078\x0307");
    test_decomp(U"\x1E8C", U"\x0058\x0308");
    test_decomp(U"\x1E8D", U"\x0078\x0308");
    test_decomp(U"\x1E8E", U"\x0059\x0307");
    test_decomp(U"\x1E8F", U"\x0079\x0307");
    test_decomp(U"\x1E90", U"\x005A\x0302");
    test_decomp(U"\x1E91", U"\x007A\x0302");
    test_decomp(U"\x1E92", U"\x005A\x0323");
    test_decomp(U"\x1E93", U"\x007A\x0323");
    test_decomp(U"\x1E94", U"\x005A\x0331");
    test_decomp(U"\x1E95", U"\x007A\x0331");
    test_decomp(U"\x1E96", U"\x0068\x0331");
    test_decomp(U"\x1E97", U"\x0074\x0308");
    test_decomp(U"\x1E98", U"\x0077\x030A");
    test_decomp(U"\x1E99", U"\x0079\x030A");
    test_decomp(U"\x1E9B", U"\x017F\x0307");
    test_decomp(U"\x1EA0", U"\x0041\x0323");
    test_decomp(U"\x1EA1", U"\x0061\x0323");
    test_decomp(U"\x1EA2", U"\x0041\x0309");
    test_decomp(U"\x1EA3", U"\x0061\x0309");
    test_decomp(U"\x1EA4", U"\x00C2\x0301");
    test_decomp(U"\x1EA5", U"\x00E2\x0301");
    test_decomp(U"\x1EA6", U"\x00C2\x0300");
    test_decomp(U"\x1EA7", U"\x00E2\x0300");
    test_decomp(U"\x1EA8", U"\x00C2\x0309");
    test_decomp(U"\x1EA9", U"\x00E2\x0309");
    test_decomp(U"\x1EAA", U"\x00C2\x0303");
    test_decomp(U"\x1EAB", U"\x00E2\x0303");
    test_decomp(U"\x1EAC", U"\x1EA0\x0302");
    test_decomp(U"\x1EAD", U"\x1EA1\x0302");
    test_decomp(U"\x1EAE", U"\x0102\x0301");
    test_decomp(U"\x1EAF", U"\x0103\x0301");
    test_decomp(U"\x1EB0", U"\x0102\x0300");
    test_decomp(U"\x1EB1", U"\x0103\x0300");
    test_decomp(U"\x1EB2", U"\x0102\x0309");
    test_decomp(U"\x1EB3", U"\x0103\x0309");
    test_decomp(U"\x1EB4", U"\x0102\x0303");
    test_decomp(U"\x1EB5", U"\x0103\x0303");
    test_decomp(U"\x1EB6", U"\x1EA0\x0306");
    test_decomp(U"\x1EB7", U"\x1EA1\x0306");
    test_decomp(U"\x1EB8", U"\x0045\x0323");
    test_decomp(U"\x1EB9", U"\x0065\x0323");
    test_decomp(U"\x1EBA", U"\x0045\x0309");
    test_decomp(U"\x1EBB", U"\x0065\x0309");
    test_decomp(U"\x1EBC", U"\x0045\x0303");
    test_decomp(U"\x1EBD", U"\x0065\x0303");
    test_decomp(U"\x1EBE", U"\x00CA\x0301");
    test_decomp(U"\x1EBF", U"\x00EA\x0301");
    test_decomp(U"\x1EC0", U"\x00CA\x0300");
    test_decomp(U"\x1EC1", U"\x00EA\x0300");
    test_decomp(U"\x1EC2", U"\x00CA\x0309");
    test_decomp(U"\x1EC3", U"\x00EA\x0309");
    test_decomp(U"\x1EC4", U"\x00CA\x0303");
    test_decomp(U"\x1EC5", U"\x00EA\x0303");
    test_decomp(U"\x1EC6", U"\x1EB8\x0302");
    test_decomp(U"\x1EC7", U"\x1EB9\x0302");
    test_decomp(U"\x1EC8", U"\x0049\x0309");
    test_decomp(U"\x1EC9", U"\x0069\x0309");
    test_decomp(U"\x1ECA", U"\x0049\x0323");
    test_decomp(U"\x1ECB", U"\x0069\x0323");
    test_decomp(U"\x1ECC", U"\x004F\x0323");
    test_decomp(U"\x1ECD", U"\x006F\x0323");
    test_decomp(U"\x1ECE", U"\x004F\x0309");
    test_decomp(U"\x1ECF", U"\x006F\x0309");
    test_decomp(U"\x1ED0", U"\x00D4\x0301");
    test_decomp(U"\x1ED1", U"\x00F4\x0301");
    test_decomp(U"\x1ED2", U"\x00D4\x0300");
    test_decomp(U"\x1ED3", U"\x00F4\x0300");
    test_decomp(U"\x1ED4", U"\x00D4\x0309");
    test_decomp(U"\x1ED5", U"\x00F4\x0309");
    test_decomp(U"\x1ED6", U"\x00D4\x0303");
    test_decomp(U"\x1ED7", U"\x00F4\x0303");
    test_decomp(U"\x1ED8", U"\x1ECC\x0302");
    test_decomp(U"\x1ED9", U"\x1ECD\x0302");
    test_decomp(U"\x1EDA", U"\x01A0\x0301");
    test_decomp(U"\x1EDB", U"\x01A1\x0301");
    test_decomp(U"\x1EDC", U"\x01A0\x0300");
    test_decomp(U"\x1EDD", U"\x01A1\x0300");
    test_decomp(U"\x1EDE", U"\x01A0\x0309");
    test_decomp(U"\x1EDF", U"\x01A1\x0309");
    test_decomp(U"\x1EE0", U"\x01A0\x0303");
    test_decomp(U"\x1EE1", U"\x01A1\x0303");
    test_decomp(U"\x1EE2", U"\x01A0\x0323");
    test_decomp(U"\x1EE3", U"\x01A1\x0323");
    test_decomp(U"\x1EE4", U"\x0055\x0323");
    test_decomp(U"\x1EE5", U"\x0075\x0323");
    test_decomp(U"\x1EE6", U"\x0055\x0309");
    test_decomp(U"\x1EE7", U"\x0075\x0309");
    test_decomp(U"\x1EE8", U"\x01AF\x0301");
    test_decomp(U"\x1EE9", U"\x01B0\x0301");
    test_decomp(U"\x1EEA", U"\x01AF\x0300");
    test_decomp(U"\x1EEB", U"\x01B0\x0300");
    test_decomp(U"\x1EEC", U"\x01AF\x0309");
    test_decomp(U"\x1EED", U"\x01B0\x0309");
    test_decomp(U"\x1EEE", U"\x01AF\x0303");
    test_decomp(U"\x1EEF", U"\x01B0\x0303");
    test_decomp(U"\x1EF0", U"\x01AF\x0323");
    test_decomp(U"\x1EF1", U"\x01B0\x0323");
    test_decomp(U"\x1EF2", U"\x0059\x0300");
    test_decomp(U"\x1EF3", U"\x0079\x0300");
    test_decomp(U"\x1EF4", U"\x0059\x0323");
    test_decomp(U"\x1EF5", U"\x0079\x0323");
    test_decomp(U"\x1EF6", U"\x0059\x0309");
    test_decomp(U"\x1EF7", U"\x0079\x0309");
    test_decomp(U"\x1EF8", U"\x0059\x0303");
    test_decomp(U"\x1EF9", U"\x0079\x0303");
    test_decomp(U"\x1F00", U"\x03B1\x0313");
    test_decomp(U"\x1F01", U"\x03B1\x0314");
    test_decomp(U"\x1F02", U"\x1F00\x0300");
    test_decomp(U"\x1F03", U"\x1F01\x0300");
    test_decomp(U"\x1F04", U"\x1F00\x0301");
    test_decomp(U"\x1F05", U"\x1F01\x0301");
    test_decomp(U"\x1F06", U"\x1F00\x0342");
    test_decomp(U"\x1F07", U"\x1F01\x0342");
    test_decomp(U"\x1F08", U"\x0391\x0313");
    test_decomp(U"\x1F09", U"\x0391\x0314");
    test_decomp(U"\x1F0A", U"\x1F08\x0300");
    test_decomp(U"\x1F0B", U"\x1F09\x0300");
    test_decomp(U"\x1F0C", U"\x1F08\x0301");
    test_decomp(U"\x1F0D", U"\x1F09\x0301");
    test_decomp(U"\x1F0E", U"\x1F08\x0342");
    test_decomp(U"\x1F0F", U"\x1F09\x0342");
    test_decomp(U"\x1F10", U"\x03B5\x0313");
    test_decomp(U"\x1F11", U"\x03B5\x0314");
    test_decomp(U"\x1F12", U"\x1F10\x0300");
    test_decomp(U"\x1F13", U"\x1F11\x0300");
    test_decomp(U"\x1F14", U"\x1F10\x0301");
    test_decomp(U"\x1F15", U"\x1F11\x0301");
    test_decomp(U"\x1F18", U"\x0395\x0313");
    test_decomp(U"\x1F19", U"\x0395\x0314");
    test_decomp(U"\x1F1A", U"\x1F18\x0300");
    test_decomp(U"\x1F1B", U"\x1F19\x0300");
    test_decomp(U"\x1F1C", U"\x1F18\x0301");
    test_decomp(U"\x1F1D", U"\x1F19\x0301");
    test_decomp(U"\x1F20", U"\x03B7\x0313");
    test_decomp(U"\x1F21", U"\x03B7\x0314");
    test_decomp(U"\x1F22", U"\x1F20\x0300");
    test_decomp(U"\x1F23", U"\x1F21\x0300");
    test_decomp(U"\x1F24", U"\x1F20\x0301");
    test_decomp(U"\x1F25", U"\x1F21\x0301");
    test_decomp(U"\x1F26", U"\x1F20\x0342");
    test_decomp(U"\x1F27", U"\x1F21\x0342");
    test_decomp(U"\x1F28", U"\x0397\x0313");
    test_decomp(U"\x1F29", U"\x0397\x0314");
    test_decomp(U"\x1F2A", U"\x1F28\x0300");
    test_decomp(U"\x1F2B", U"\x1F29\x0300");
    test_decomp(U"\x1F2C", U"\x1F28\x0301");
    test_decomp(U"\x1F2D", U"\x1F29\x0301");
    test_decomp(U"\x1F2E", U"\x1F28\x0342");
    test_decomp(U"\x1F2F", U"\x1F29\x0342");
    test_decomp(U"\x1F30", U"\x03B9\x0313");
    test_decomp(U"\x1F31", U"\x03B9\x0314");
    test_decomp(U"\x1F32", U"\x1F30\x0300");
    test_decomp(U"\x1F33", U"\x1F31\x0300");
    test_decomp(U"\x1F34", U"\x1F30\x0301");
    test_decomp(U"\x1F35", U"\x1F31\x0301");
    test_decomp(U"\x1F36", U"\x1F30\x0342");
    test_decomp(U"\x1F37", U"\x1F31\x0342");
    test_decomp(U"\x1F38", U"\x0399\x0313");
    test_decomp(U"\x1F39", U"\x0399\x0314");
    test_decomp(U"\x1F3A", U"\x1F38\x0300");
    test_decomp(U"\x1F3B", U"\x1F39\x0300");
    test_decomp(U"\x1F3C", U"\x1F38\x0301");
    test_decomp(U"\x1F3D", U"\x1F39\x0301");
    test_decomp(U"\x1F3E", U"\x1F38\x0342");
    test_decomp(U"\x1F3F", U"\x1F39\x0342");
    test_decomp(U"\x1F40", U"\x03BF\x0313");
    test_decomp(U"\x1F41", U"\x03BF\x0314");
    test_decomp(U"\x1F42", U"\x1F40\x0300");
    test_decomp(U"\x1F43", U"\x1F41\x0300");
    test_decomp(U"\x1F44", U"\x1F40\x0301");
    test_decomp(U"\x1F45", U"\x1F41\x0301");
    test_decomp(U"\x1F48", U"\x039F\x0313");
    test_decomp(U"\x1F49", U"\x039F\x0314");
    test_decomp(U"\x1F4A", U"\x1F48\x0300");
    test_decomp(U"\x1F4B", U"\x1F49\x0300");
    test_decomp(U"\x1F4C", U"\x1F48\x0301");
    test_decomp(U"\x1F4D", U"\x1F49\x0301");
    test_decomp(U"\x1F50", U"\x03C5\x0313");
    test_decomp(U"\x1F51", U"\x03C5\x0314");
    test_decomp(U"\x1F52", U"\x1F50\x0300");
    test_decomp(U"\x1F53", U"\x1F51\x0300");
    test_decomp(U"\x1F54", U"\x1F50\x0301");
    test_decomp(U"\x1F55", U"\x1F51\x0301");
    test_decomp(U"\x1F56", U"\x1F50\x0342");
    test_decomp(U"\x1F57", U"\x1F51\x0342");
    test_decomp(U"\x1F59", U"\x03A5\x0314");
    test_decomp(U"\x1F5B", U"\x1F59\x0300");
    test_decomp(U"\x1F5D", U"\x1F59\x0301");
    test_decomp(U"\x1F5F", U"\x1F59\x0342");
    test_decomp(U"\x1F60", U"\x03C9\x0313");
    test_decomp(U"\x1F61", U"\x03C9\x0314");
    test_decomp(U"\x1F62", U"\x1F60\x0300");
    test_decomp(U"\x1F63", U"\x1F61\x0300");
    test_decomp(U"\x1F64", U"\x1F60\x0301");
    test_decomp(U"\x1F65", U"\x1F61\x0301");
    test_decomp(U"\x1F66", U"\x1F60\x0342");
    test_decomp(U"\x1F67", U"\x1F61\x0342");
    test_decomp(U"\x1F68", U"\x03A9\x0313");
    test_decomp(U"\x1F69", U"\x03A9\x0314");
    test_decomp(U"\x1F6A", U"\x1F68\x0300");
    test_decomp(U"\x1F6B", U"\x1F69\x0300");
    test_decomp(U"\x1F6C", U"\x1F68\x0301");
    test_decomp(U"\x1F6D", U"\x1F69\x0301");
    test_decomp(U"\x1F6E", U"\x1F68\x0342");
    test_decomp(U"\x1F6F", U"\x1F69\x0342");
    test_decomp(U"\x1F70", U"\x03B1\x0300");
    test_decomp(U"\x1F71", U"\x03AC");
    test_decomp(U"\x1F72", U"\x03B5\x0300");
    test_decomp(U"\x1F73", U"\x03AD");
    test_decomp(U"\x1F74", U"\x03B7\x0300");
    test_decomp(U"\x1F75", U"\x03AE");
    test_decomp(U"\x1F76", U"\x03B9\x0300");
    test_decomp(U"\x1F77", U"\x03AF");
    test_decomp(U"\x1F78", U"\x03BF\x0300");
    test_decomp(U"\x1F79", U"\x03CC");
    test_decomp(U"\x1F7A", U"\x03C5\x0300");
    test_decomp(U"\x1F7B", U"\x03CD");
    test_decomp(U"\x1F7C", U"\x03C9\x0300");
    test_decomp(U"\x1F7D", U"\x03CE");
    test_decomp(U"\x1F80", U"\x1F00\x0345");
    test_decomp(U"\x1F81", U"\x1F01\x0345");
    test_decomp(U"\x1F82", U"\x1F02\x0345");
    test_decomp(U"\x1F83", U"\x1F03\x0345");
    test_decomp(U"\x1F84", U"\x1F04\x0345");
    test_decomp(U"\x1F85", U"\x1F05\x0345");
    test_decomp(U"\x1F86", U"\x1F06\x0345");
    test_decomp(U"\x1F87", U"\x1F07\x0345");
    test_decomp(U"\x1F88", U"\x1F08\x0345");
    test_decomp(U"\x1F89", U"\x1F09\x0345");
    test_decomp(U"\x1F8A", U"\x1F0A\x0345");
    test_decomp(U"\x1F8B", U"\x1F0B\x0345");
    test_decomp(U"\x1F8C", U"\x1F0C\x0345");
    test_decomp(U"\x1F8D", U"\x1F0D\x0345");
    test_decomp(U"\x1F8E", U"\x1F0E\x0345");
    test_decomp(U"\x1F8F", U"\x1F0F\x0345");
    test_decomp(U"\x1F90", U"\x1F20\x0345");
    test_decomp(U"\x1F91", U"\x1F21\x0345");
    test_decomp(U"\x1F92", U"\x1F22\x0345");
    test_decomp(U"\x1F93", U"\x1F23\x0345");
    test_decomp(U"\x1F94", U"\x1F24\x0345");
    test_decomp(U"\x1F95", U"\x1F25\x0345");
    test_decomp(U"\x1F96", U"\x1F26\x0345");
    test_decomp(U"\x1F97", U"\x1F27\x0345");
    test_decomp(U"\x1F98", U"\x1F28\x0345");
    test_decomp(U"\x1F99", U"\x1F29\x0345");
    test_decomp(U"\x1F9A", U"\x1F2A\x0345");
    test_decomp(U"\x1F9B", U"\x1F2B\x0345");
    test_decomp(U"\x1F9C", U"\x1F2C\x0345");
    test_decomp(U"\x1F9D", U"\x1F2D\x0345");
    test_decomp(U"\x1F9E", U"\x1F2E\x0345");
    test_decomp(U"\x1F9F", U"\x1F2F\x0345");
    test_decomp(U"\x1FA0", U"\x1F60\x0345");
    test_decomp(U"\x1FA1", U"\x1F61\x0345");
    test_decomp(U"\x1FA2", U"\x1F62\x0345");
    test_decomp(U"\x1FA3", U"\x1F63\x0345");
    test_decomp(U"\x1FA4", U"\x1F64\x0345");
    test_decomp(U"\x1FA5", U"\x1F65\x0345");
    test_decomp(U"\x1FA6", U"\x1F66\x0345");
    test_decomp(U"\x1FA7", U"\x1F67\x0345");
    test_decomp(U"\x1FA8", U"\x1F68\x0345");
    test_decomp(U"\x1FA9", U"\x1F69\x0345");
    test_decomp(U"\x1FAA", U"\x1F6A\x0345");
    test_decomp(U"\x1FAB", U"\x1F6B\x0345");
    test_decomp(U"\x1FAC", U"\x1F6C\x0345");
    test_decomp(U"\x1FAD", U"\x1F6D\x0345");
    test_decomp(U"\x1FAE", U"\x1F6E\x0345");
    test_decomp(U"\x1FAF", U"\x1F6F\x0345");
    test_decomp(U"\x1FB0", U"\x03B1\x0306");
    test_decomp(U"\x1FB1", U"\x03B1\x0304");
    test_decomp(U"\x1FB2", U"\x1F70\x0345");
    test_decomp(U"\x1FB3", U"\x03B1\x0345");
    test_decomp(U"\x1FB4", U"\x03AC\x0345");
    test_decomp(U"\x1FB6", U"\x03B1\x0342");
    test_decomp(U"\x1FB7", U"\x1FB6\x0345");
    test_decomp(U"\x1FB8", U"\x0391\x0306");
    test_decomp(U"\x1FB9", U"\x0391\x0304");
    test_decomp(U"\x1FBA", U"\x0391\x0300");
    test_decomp(U"\x1FBB", U"\x0386");
    test_decomp(U"\x1FBC", U"\x0391\x0345");
    test_decomp(U"\x1FBE", U"\x03B9");
    test_decomp(U"\x1FC1", U"\x00A8\x0342");
    test_decomp(U"\x1FC2", U"\x1F74\x0345");
    test_decomp(U"\x1FC3", U"\x03B7\x0345");
    test_decomp(U"\x1FC4", U"\x03AE\x0345");
    test_decomp(U"\x1FC6", U"\x03B7\x0342");
    test_decomp(U"\x1FC7", U"\x1FC6\x0345");
    test_decomp(U"\x1FC8", U"\x0395\x0300");
    test_decomp(U"\x1FC9", U"\x0388");
    test_decomp(U"\x1FCA", U"\x0397\x0300");
    test_decomp(U"\x1FCB", U"\x0389");
    test_decomp(U"\x1FCC", U"\x0397\x0345");
    test_decomp(U"\x1FCD", U"\x1FBF\x0300");
    test_decomp(U"\x1FCE", U"\x1FBF\x0301");
    test_decomp(U"\x1FCF", U"\x1FBF\x0342");
    test_decomp(U"\x1FD0", U"\x03B9\x0306");
    test_decomp(U"\x1FD1", U"\x03B9\x0304");
    test_decomp(U"\x1FD2", U"\x03CA\x0300");
    test_decomp(U"\x1FD3", U"\x0390");
    test_decomp(U"\x1FD6", U"\x03B9\x0342");
    test_decomp(U"\x1FD7", U"\x03CA\x0342");
    test_decomp(U"\x1FD8", U"\x0399\x0306");
    test_decomp(U"\x1FD9", U"\x0399\x0304");
    test_decomp(U"\x1FDA", U"\x0399\x0300");
    test_decomp(U"\x1FDB", U"\x038A");
    test_decomp(U"\x1FDD", U"\x1FFE\x0300");
    test_decomp(U"\x1FDE", U"\x1FFE\x0301");
    test_decomp(U"\x1FDF", U"\x1FFE\x0342");
    test_decomp(U"\x1FE0", U"\x03C5\x0306");
    test_decomp(U"\x1FE1", U"\x03C5\x0304");
    test_decomp(U"\x1FE2", U"\x03CB\x0300");
    test_decomp(U"\x1FE3", U"\x03B0");
    test_decomp(U"\x1FE4", U"\x03C1\x0313");
    test_decomp(U"\x1FE5", U"\x03C1\x0314");
    test_decomp(U"\x1FE6", U"\x03C5\x0342");
    test_decomp(U"\x1FE7", U"\x03CB\x0342");
    test_decomp(U"\x1FE8", U"\x03A5\x0306");
    test_decomp(U"\x1FE9", U"\x03A5\x0304");
    test_decomp(U"\x1FEA", U"\x03A5\x0300");
    test_decomp(U"\x1FEB", U"\x038E");
    test_decomp(U"\x1FEC", U"\x03A1\x0314");
    test_decomp(U"\x1FED", U"\x00A8\x0300");
    test_decomp(U"\x1FEE", U"\x0385");
    test_decomp(U"\x1FEF", U"\x0060");
    test_decomp(U"\x1FF2", U"\x1F7C\x0345");
    test_decomp(U"\x1FF3", U"\x03C9\x0345");
    test_decomp(U"\x1FF4", U"\x03CE\x0345");
    test_decomp(U"\x1FF6", U"\x03C9\x0342");
    test_decomp(U"\x1FF7", U"\x1FF6\x0345");
    test_decomp(U"\x1FF8", U"\x039F\x0300");
    test_decomp(U"\x1FF9", U"\x038C");
    test_decomp(U"\x1FFA", U"\x03A9\x0300");
    test_decomp(U"\x1FFB", U"\x038F");
    test_decomp(U"\x1FFC", U"\x03A9\x0345");
    test_decomp(U"\x1FFD", U"\x00B4");
    test_decomp(U"\x2000", U"\x2002");
    test_decomp(U"\x2001", U"\x2003");
    test_decomp(U"\x2126", U"\x03A9");
    test_decomp(U"\x212A", U"\x004B");
    test_decomp(U"\x212B", U"\x00C5");
    test_decomp(U"\x219A", U"\x2190\x0338");
    test_decomp(U"\x219B", U"\x2192\x0338");
    test_decomp(U"\x21AE", U"\x2194\x0338");
    test_decomp(U"\x21CD", U"\x21D0\x0338");
    test_decomp(U"\x21CE", U"\x21D4\x0338");
    test_decomp(U"\x21CF", U"\x21D2\x0338");
    test_decomp(U"\x2204", U"\x2203\x0338");
    test_decomp(U"\x2209", U"\x2208\x0338");
    test_decomp(U"\x220C", U"\x220B\x0338");
    test_decomp(U"\x2224", U"\x2223\x0338");
    test_decomp(U"\x2226", U"\x2225\x0338");
    test_decomp(U"\x2241", U"\x223C\x0338");
    test_decomp(U"\x2244", U"\x2243\x0338");
    test_decomp(U"\x2247", U"\x2245\x0338");
    test_decomp(U"\x2249", U"\x2248\x0338");
    test_decomp(U"\x2260", U"\x003D\x0338");
    test_decomp(U"\x2262", U"\x2261\x0338");
    test_decomp(U"\x226D", U"\x224D\x0338");
    test_decomp(U"\x226E", U"\x003C\x0338");
    test_decomp(U"\x226F", U"\x003E\x0338");
    test_decomp(U"\x2270", U"\x2264\x0338");
    test_decomp(U"\x2271", U"\x2265\x0338");
    test_decomp(U"\x2274", U"\x2272\x0338");
    test_decomp(U"\x2275", U"\x2273\x0338");
    test_decomp(U"\x2278", U"\x2276\x0338");
    test_decomp(U"\x2279", U"\x2277\x0338");
    test_decomp(U"\x2280", U"\x227A\x0338");
    test_decomp(U"\x2281", U"\x227B\x0338");
    test_decomp(U"\x2284", U"\x2282\x0338");
    test_decomp(U"\x2285", U"\x2283\x0338");
    test_decomp(U"\x2288", U"\x2286\x0338");
    test_decomp(U"\x2289", U"\x2287\x0338");
    test_decomp(U"\x22AC", U"\x22A2\x0338");
    test_decomp(U"\x22AD", U"\x22A8\x0338");
    test_decomp(U"\x22AE", U"\x22A9\x0338");
    test_decomp(U"\x22AF", U"\x22AB\x0338");
    test_decomp(U"\x22E0", U"\x227C\x0338");
    test_decomp(U"\x22E1", U"\x227D\x0338");
    test_decomp(U"\x22E2", U"\x2291\x0338");
    test_decomp(U"\x22E3", U"\x2292\x0338");
    test_decomp(U"\x22EA", U"\x22B2\x0338");
    test_decomp(U"\x22EB", U"\x22B3\x0338");
    test_decomp(U"\x22EC", U"\x22B4\x0338");
    test_decomp(U"\x22ED", U"\x22B5\x0338");
    test_decomp(U"\x2329", U"\x3008");
    test_decomp(U"\x232A", U"\x3009");
    test_decomp(U"\x2ADC", U"\x2ADD\x0338");
    test_decomp(U"\x304C", U"\x304B\x3099");
    test_decomp(U"\x304E", U"\x304D\x3099");
    test_decomp(U"\x3050", U"\x304F\x3099");
    test_decomp(U"\x3052", U"\x3051\x3099");
    test_decomp(U"\x3054", U"\x3053\x3099");
    test_decomp(U"\x3056", U"\x3055\x3099");
    test_decomp(U"\x3058", U"\x3057\x3099");
    test_decomp(U"\x305A", U"\x3059\x3099");
    test_decomp(U"\x305C", U"\x305B\x3099");
    test_decomp(U"\x305E", U"\x305D\x3099");
    test_decomp(U"\x3060", U"\x305F\x3099");
    test_decomp(U"\x3062", U"\x3061\x3099");
    test_decomp(U"\x3065", U"\x3064\x3099");
    test_decomp(U"\x3067", U"\x3066\x3099");
    test_decomp(U"\x3069", U"\x3068\x3099");
    test_decomp(U"\x3070", U"\x306F\x3099");
    test_decomp(U"\x3071", U"\x306F\x309A");
    test_decomp(U"\x3073", U"\x3072\x3099");
    test_decomp(U"\x3074", U"\x3072\x309A");
    test_decomp(U"\x3076", U"\x3075\x3099");
    test_decomp(U"\x3077", U"\x3075\x309A");
    test_decomp(U"\x3079", U"\x3078\x3099");
    test_decomp(U"\x307A", U"\x3078\x309A");
    test_decomp(U"\x307C", U"\x307B\x3099");
    test_decomp(U"\x307D", U"\x307B\x309A");
    test_decomp(U"\x3094", U"\x3046\x3099");
    test_decomp(U"\x309E", U"\x309D\x3099");
    test_decomp(U"\x30AC", U"\x30AB\x3099");
    test_decomp(U"\x30AE", U"\x30AD\x3099");
    test_decomp(U"\x30B0", U"\x30AF\x3099");
    test_decomp(U"\x30B2", U"\x30B1\x3099");
    test_decomp(U"\x30B4", U"\x30B3\x3099");
    test_decomp(U"\x30B6", U"\x30B5\x3099");
    test_decomp(U"\x30B8", U"\x30B7\x3099");
    test_decomp(U"\x30BA", U"\x30B9\x3099");
    test_decomp(U"\x30BC", U"\x30BB\x3099");
    test_decomp(U"\x30BE", U"\x30BD\x3099");
    test_decomp(U"\x30C0", U"\x30BF\x3099");
    test_decomp(U"\x30C2", U"\x30C1\x3099");
    test_decomp(U"\x30C5", U"\x30C4\x3099");
    test_decomp(U"\x30C7", U"\x30C6\x3099");
    test_decomp(U"\x30C9", U"\x30C8\x3099");
    test_decomp(U"\x30D0", U"\x30CF\x3099");
    test_decomp(U"\x30D1", U"\x30CF\x309A");
    test_decomp(U"\x30D3", U"\x30D2\x3099");
    test_decomp(U"\x30D4", U"\x30D2\x309A");
    test_decomp(U"\x30D6", U"\x30D5\x3099");
    test_decomp(U"\x30D7", U"\x30D5\x309A");
    test_decomp(U"\x30D9", U"\x30D8\x3099");
    test_decomp(U"\x30DA", U"\x30D8\x309A");
    test_decomp(U"\x30DC", U"\x30DB\x3099");
    test_decomp(U"\x30DD", U"\x30DB\x309A");
    test_decomp(U"\x30F4", U"\x30A6\x3099");
    test_decomp(U"\x30F7", U"\x30EF\x3099");
    test_decomp(U"\x30F8", U"\x30F0\x3099");
    test_decomp(U"\x30F9", U"\x30F1\x3099");
    test_decomp(U"\x30FA", U"\x30F2\x3099");
    test_decomp(U"\x30FE", U"\x30FD\x3099");
    test_decomp(U"\xF900", U"\x8C48");
    test_decomp(U"\xF901", U"\x66F4");
    test_decomp(U"\xF902", U"\x8ECA");
    test_decomp(U"\xF903", U"\x8CC8");
    test_decomp(U"\xF904", U"\x6ED1");
    test_decomp(U"\xF905", U"\x4E32");
    test_decomp(U"\xF906", U"\x53E5");
    test_decomp(U"\xF907", U"\x9F9C");
    test_decomp(U"\xF908", U"\x9F9C");
    test_decomp(U"\xF909", U"\x5951");
    test_decomp(U"\xF90A", U"\x91D1");
    test_decomp(U"\xF90B", U"\x5587");
    test_decomp(U"\xF90C", U"\x5948");
    test_decomp(U"\xF90D", U"\x61F6");
    test_decomp(U"\xF90E", U"\x7669");
    test_decomp(U"\xF90F", U"\x7F85");
    test_decomp(U"\xF910", U"\x863F");
    test_decomp(U"\xF911", U"\x87BA");
    test_decomp(U"\xF912", U"\x88F8");
    test_decomp(U"\xF913", U"\x908F");
    test_decomp(U"\xF914", U"\x6A02");
    test_decomp(U"\xF915", U"\x6D1B");
    test_decomp(U"\xF916", U"\x70D9");
    test_decomp(U"\xF917", U"\x73DE");
    test_decomp(U"\xF918", U"\x843D");
    test_decomp(U"\xF919", U"\x916A");
    test_decomp(U"\xF91A", U"\x99F1");
    test_decomp(U"\xF91B", U"\x4E82");
    test_decomp(U"\xF91C", U"\x5375");
    test_decomp(U"\xF91D", U"\x6B04");
    test_decomp(U"\xF91E", U"\x721B");
    test_decomp(U"\xF91F", U"\x862D");
    test_decomp(U"\xF920", U"\x9E1E");
    test_decomp(U"\xF921", U"\x5D50");
    test_decomp(U"\xF922", U"\x6FEB");
    test_decomp(U"\xF923", U"\x85CD");
    test_decomp(U"\xF924", U"\x8964");
    test_decomp(U"\xF925", U"\x62C9");
    test_decomp(U"\xF926", U"\x81D8");
    test_decomp(U"\xF927", U"\x881F");
    test_decomp(U"\xF928", U"\x5ECA");
    test_decomp(U"\xF929", U"\x6717");
    test_decomp(U"\xF92A", U"\x6D6A");
    test_decomp(U"\xF92B", U"\x72FC");
    test_decomp(U"\xF92C", U"\x90CE");
    test_decomp(U"\xF92D", U"\x4F86");
    test_decomp(U"\xF92E", U"\x51B7");
    test_decomp(U"\xF92F", U"\x52DE");
    test_decomp(U"\xF930", U"\x64C4");
    test_decomp(U"\xF931", U"\x6AD3");
    test_decomp(U"\xF932", U"\x7210");
    test_decomp(U"\xF933", U"\x76E7");
    test_decomp(U"\xF934", U"\x8001");
    test_decomp(U"\xF935", U"\x8606");
    test_decomp(U"\xF936", U"\x865C");
    test_decomp(U"\xF937", U"\x8DEF");
    test_decomp(U"\xF938", U"\x9732");
    test_decomp(U"\xF939", U"\x9B6F");
    test_decomp(U"\xF93A", U"\x9DFA");
    test_decomp(U"\xF93B", U"\x788C");
    test_decomp(U"\xF93C", U"\x797F");
    test_decomp(U"\xF93D", U"\x7DA0");
    test_decomp(U"\xF93E", U"\x83C9");
    test_decomp(U"\xF93F", U"\x9304");
    test_decomp(U"\xF940", U"\x9E7F");
    test_decomp(U"\xF941", U"\x8AD6");
    test_decomp(U"\xF942", U"\x58DF");
    test_decomp(U"\xF943", U"\x5F04");
    test_decomp(U"\xF944", U"\x7C60");
    test_decomp(U"\xF945", U"\x807E");
    test_decomp(U"\xF946", U"\x7262");
    test_decomp(U"\xF947", U"\x78CA");
    test_decomp(U"\xF948", U"\x8CC2");
    test_decomp(U"\xF949", U"\x96F7");
    test_decomp(U"\xF94A", U"\x58D8");
    test_decomp(U"\xF94B", U"\x5C62");
    test_decomp(U"\xF94C", U"\x6A13");
    test_decomp(U"\xF94D", U"\x6DDA");
    test_decomp(U"\xF94E", U"\x6F0F");
    test_decomp(U"\xF94F", U"\x7D2F");
    test_decomp(U"\xF950", U"\x7E37");
    test_decomp(U"\xF951", U"\x964B");
    test_decomp(U"\xF952", U"\x52D2");
    test_decomp(U"\xF953", U"\x808B");
    test_decomp(U"\xF954", U"\x51DC");
    test_decomp(U"\xF955", U"\x51CC");
    test_decomp(U"\xF956", U"\x7A1C");
    test_decomp(U"\xF957", U"\x7DBE");
    test_decomp(U"\xF958", U"\x83F1");
    test_decomp(U"\xF959", U"\x9675");
    test_decomp(U"\xF95A", U"\x8B80");
    test_decomp(U"\xF95B", U"\x62CF");
    test_decomp(U"\xF95C", U"\x6A02");
    test_decomp(U"\xF95D", U"\x8AFE");
    test_decomp(U"\xF95E", U"\x4E39");
    test_decomp(U"\xF95F", U"\x5BE7");
    test_decomp(U"\xF960", U"\x6012");
    test_decomp(U"\xF961", U"\x7387");
    test_decomp(U"\xF962", U"\x7570");
    test_decomp(U"\xF963", U"\x5317");
    test_decomp(U"\xF964", U"\x78FB");
    test_decomp(U"\xF965", U"\x4FBF");
    test_decomp(U"\xF966", U"\x5FA9");
    test_decomp(U"\xF967", U"\x4E0D");
    test_decomp(U"\xF968", U"\x6CCC");
    test_decomp(U"\xF969", U"\x6578");
    test_decomp(U"\xF96A", U"\x7D22");
    test_decomp(U"\xF96B", U"\x53C3");
    test_decomp(U"\xF96C", U"\x585E");
    test_decomp(U"\xF96D", U"\x7701");
    test_decomp(U"\xF96E", U"\x8449");
    test_decomp(U"\xF96F", U"\x8AAA");
    test_decomp(U"\xF970", U"\x6BBA");
    test_decomp(U"\xF971", U"\x8FB0");
    test_decomp(U"\xF972", U"\x6C88");
    test_decomp(U"\xF973", U"\x62FE");
    test_decomp(U"\xF974", U"\x82E5");
    test_decomp(U"\xF975", U"\x63A0");
    test_decomp(U"\xF976", U"\x7565");
    test_decomp(U"\xF977", U"\x4EAE");
    test_decomp(U"\xF978", U"\x5169");
    test_decomp(U"\xF979", U"\x51C9");
    test_decomp(U"\xF97A", U"\x6881");
    test_decomp(U"\xF97B", U"\x7CE7");
    test_decomp(U"\xF97C", U"\x826F");
    test_decomp(U"\xF97D", U"\x8AD2");
    test_decomp(U"\xF97E", U"\x91CF");
    test_decomp(U"\xF97F", U"\x52F5");
    test_decomp(U"\xF980", U"\x5442");
    test_decomp(U"\xF981", U"\x5973");
    test_decomp(U"\xF982", U"\x5EEC");
    test_decomp(U"\xF983", U"\x65C5");
    test_decomp(U"\xF984", U"\x6FFE");
    test_decomp(U"\xF985", U"\x792A");
    test_decomp(U"\xF986", U"\x95AD");
    test_decomp(U"\xF987", U"\x9A6A");
    test_decomp(U"\xF988", U"\x9E97");
    test_decomp(U"\xF989", U"\x9ECE");
    test_decomp(U"\xF98A", U"\x529B");
    test_decomp(U"\xF98B", U"\x66C6");
    test_decomp(U"\xF98C", U"\x6B77");
    test_decomp(U"\xF98D", U"\x8F62");
    test_decomp(U"\xF98E", U"\x5E74");
    test_decomp(U"\xF98F", U"\x6190");
    test_decomp(U"\xF990", U"\x6200");
    test_decomp(U"\xF991", U"\x649A");
    test_decomp(U"\xF992", U"\x6F23");
    test_decomp(U"\xF993", U"\x7149");
    test_decomp(U"\xF994", U"\x7489");
    test_decomp(U"\xF995", U"\x79CA");
    test_decomp(U"\xF996", U"\x7DF4");
    test_decomp(U"\xF997", U"\x806F");
    test_decomp(U"\xF998", U"\x8F26");
    test_decomp(U"\xF999", U"\x84EE");
    test_decomp(U"\xF99A", U"\x9023");
    test_decomp(U"\xF99B", U"\x934A");
    test_decomp(U"\xF99C", U"\x5217");
    test_decomp(U"\xF99D", U"\x52A3");
    test_decomp(U"\xF99E", U"\x54BD");
    test_decomp(U"\xF99F", U"\x70C8");
    test_decomp(U"\xF9A0", U"\x88C2");
    test_decomp(U"\xF9A1", U"\x8AAA");
    test_decomp(U"\xF9A2", U"\x5EC9");
    test_decomp(U"\xF9A3", U"\x5FF5");
    test_decomp(U"\xF9A4", U"\x637B");
    test_decomp(U"\xF9A5", U"\x6BAE");
    test_decomp(U"\xF9A6", U"\x7C3E");
    test_decomp(U"\xF9A7", U"\x7375");
    test_decomp(U"\xF9A8", U"\x4EE4");
    test_decomp(U"\xF9A9", U"\x56F9");
    test_decomp(U"\xF9AA", U"\x5BE7");
    test_decomp(U"\xF9AB", U"\x5DBA");
    test_decomp(U"\xF9AC", U"\x601C");
    test_decomp(U"\xF9AD", U"\x73B2");
    test_decomp(U"\xF9AE", U"\x7469");
    test_decomp(U"\xF9AF", U"\x7F9A");
    test_decomp(U"\xF9B0", U"\x8046");
    test_decomp(U"\xF9B1", U"\x9234");
    test_decomp(U"\xF9B2", U"\x96F6");
    test_decomp(U"\xF9B3", U"\x9748");
    test_decomp(U"\xF9B4", U"\x9818");
    test_decomp(U"\xF9B5", U"\x4F8B");
    test_decomp(U"\xF9B6", U"\x79AE");
    test_decomp(U"\xF9B7", U"\x91B4");
    test_decomp(U"\xF9B8", U"\x96B8");
    test_decomp(U"\xF9B9", U"\x60E1");
    test_decomp(U"\xF9BA", U"\x4E86");
    test_decomp(U"\xF9BB", U"\x50DA");
    test_decomp(U"\xF9BC", U"\x5BEE");
    test_decomp(U"\xF9BD", U"\x5C3F");
    test_decomp(U"\xF9BE", U"\x6599");
    test_decomp(U"\xF9BF", U"\x6A02");
    test_decomp(U"\xF9C0", U"\x71CE");
    test_decomp(U"\xF9C1", U"\x7642");
    test_decomp(U"\xF9C2", U"\x84FC");
    test_decomp(U"\xF9C3", U"\x907C");
    test_decomp(U"\xF9C4", U"\x9F8D");
    test_decomp(U"\xF9C5", U"\x6688");
    test_decomp(U"\xF9C6", U"\x962E");
    test_decomp(U"\xF9C7", U"\x5289");
    test_decomp(U"\xF9C8", U"\x677B");
    test_decomp(U"\xF9C9", U"\x67F3");
    test_decomp(U"\xF9CA", U"\x6D41");
    test_decomp(U"\xF9CB", U"\x6E9C");
    test_decomp(U"\xF9CC", U"\x7409");
    test_decomp(U"\xF9CD", U"\x7559");
    test_decomp(U"\xF9CE", U"\x786B");
    test_decomp(U"\xF9CF", U"\x7D10");
    test_decomp(U"\xF9D0", U"\x985E");
    test_decomp(U"\xF9D1", U"\x516D");
    test_decomp(U"\xF9D2", U"\x622E");
    test_decomp(U"\xF9D3", U"\x9678");
    test_decomp(U"\xF9D4", U"\x502B");
    test_decomp(U"\xF9D5", U"\x5D19");
    test_decomp(U"\xF9D6", U"\x6DEA");
    test_decomp(U"\xF9D7", U"\x8F2A");
    test_decomp(U"\xF9D8", U"\x5F8B");
    test_decomp(U"\xF9D9", U"\x6144");
    test_decomp(U"\xF9DA", U"\x6817");
    test_decomp(U"\xF9DB", U"\x7387");
    test_decomp(U"\xF9DC", U"\x9686");
    test_decomp(U"\xF9DD", U"\x5229");
    test_decomp(U"\xF9DE", U"\x540F");
    test_decomp(U"\xF9DF", U"\x5C65");
    test_decomp(U"\xF9E0", U"\x6613");
    test_decomp(U"\xF9E1", U"\x674E");
    test_decomp(U"\xF9E2", U"\x68A8");
    test_decomp(U"\xF9E3", U"\x6CE5");
    test_decomp(U"\xF9E4", U"\x7406");
    test_decomp(U"\xF9E5", U"\x75E2");
    test_decomp(U"\xF9E6", U"\x7F79");
    test_decomp(U"\xF9E7", U"\x88CF");
    test_decomp(U"\xF9E8", U"\x88E1");
    test_decomp(U"\xF9E9", U"\x91CC");
    test_decomp(U"\xF9EA", U"\x96E2");
    test_decomp(U"\xF9EB", U"\x533F");
    test_decomp(U"\xF9EC", U"\x6EBA");
    test_decomp(U"\xF9ED", U"\x541D");
    test_decomp(U"\xF9EE", U"\x71D0");
    test_decomp(U"\xF9EF", U"\x7498");
    test_decomp(U"\xF9F0", U"\x85FA");
    test_decomp(U"\xF9F1", U"\x96A3");
    test_decomp(U"\xF9F2", U"\x9C57");
    test_decomp(U"\xF9F3", U"\x9E9F");
    test_decomp(U"\xF9F4", U"\x6797");
    test_decomp(U"\xF9F5", U"\x6DCB");
    test_decomp(U"\xF9F6", U"\x81E8");
    test_decomp(U"\xF9F7", U"\x7ACB");
    test_decomp(U"\xF9F8", U"\x7B20");
    test_decomp(U"\xF9F9", U"\x7C92");
    test_decomp(U"\xF9FA", U"\x72C0");
    test_decomp(U"\xF9FB", U"\x7099");
    test_decomp(U"\xF9FC", U"\x8B58");
    test_decomp(U"\xF9FD", U"\x4EC0");
    test_decomp(U"\xF9FE", U"\x8336");
    test_decomp(U"\xF9FF", U"\x523A");
    test_decomp(U"\xFA00", U"\x5207");
    test_decomp(U"\xFA01", U"\x5EA6");
    test_decomp(U"\xFA02", U"\x62D3");
    test_decomp(U"\xFA03", U"\x7CD6");
    test_decomp(U"\xFA04", U"\x5B85");
    test_decomp(U"\xFA05", U"\x6D1E");
    test_decomp(U"\xFA06", U"\x66B4");
    test_decomp(U"\xFA07", U"\x8F3B");
    test_decomp(U"\xFA08", U"\x884C");
    test_decomp(U"\xFA09", U"\x964D");
    test_decomp(U"\xFA0A", U"\x898B");
    test_decomp(U"\xFA0B", U"\x5ED3");
    test_decomp(U"\xFA0C", U"\x5140");
    test_decomp(U"\xFA0D", U"\x55C0");
    test_decomp(U"\xFA10", U"\x585A");
    test_decomp(U"\xFA12", U"\x6674");
    test_decomp(U"\xFA15", U"\x51DE");
    test_decomp(U"\xFA16", U"\x732A");
    test_decomp(U"\xFA17", U"\x76CA");
    test_decomp(U"\xFA18", U"\x793C");
    test_decomp(U"\xFA19", U"\x795E");
    test_decomp(U"\xFA1A", U"\x7965");
    test_decomp(U"\xFA1B", U"\x798F");
    test_decomp(U"\xFA1C", U"\x9756");
    test_decomp(U"\xFA1D", U"\x7CBE");
    test_decomp(U"\xFA1E", U"\x7FBD");
    test_decomp(U"\xFA20", U"\x8612");
    test_decomp(U"\xFA22", U"\x8AF8");
    test_decomp(U"\xFA25", U"\x9038");
    test_decomp(U"\xFA26", U"\x90FD");
    test_decomp(U"\xFA2A", U"\x98EF");
    test_decomp(U"\xFA2B", U"\x98FC");
    test_decomp(U"\xFA2C", U"\x9928");
    test_decomp(U"\xFA2D", U"\x9DB4");
    test_decomp(U"\xFA2E", U"\x90DE");
    test_decomp(U"\xFA2F", U"\x96B7");
    test_decomp(U"\xFA30", U"\x4FAE");
    test_decomp(U"\xFA31", U"\x50E7");
    test_decomp(U"\xFA32", U"\x514D");
    test_decomp(U"\xFA33", U"\x52C9");
    test_decomp(U"\xFA34", U"\x52E4");
    test_decomp(U"\xFA35", U"\x5351");
    test_decomp(U"\xFA36", U"\x559D");
    test_decomp(U"\xFA37", U"\x5606");
    test_decomp(U"\xFA38", U"\x5668");
    test_decomp(U"\xFA39", U"\x5840");
    test_decomp(U"\xFA3A", U"\x58A8");
    test_decomp(U"\xFA3B", U"\x5C64");
    test_decomp(U"\xFA3C", U"\x5C6E");
    test_decomp(U"\xFA3D", U"\x6094");
    test_decomp(U"\xFA3E", U"\x6168");
    test_decomp(U"\xFA3F", U"\x618E");
    test_decomp(U"\xFA40", U"\x61F2");
    test_decomp(U"\xFA41", U"\x654F");
    test_decomp(U"\xFA42", U"\x65E2");
    test_decomp(U"\xFA43", U"\x6691");
    test_decomp(U"\xFA44", U"\x6885");
    test_decomp(U"\xFA45", U"\x6D77");
    test_decomp(U"\xFA46", U"\x6E1A");
    test_decomp(U"\xFA47", U"\x6F22");
    test_decomp(U"\xFA48", U"\x716E");
    test_decomp(U"\xFA49", U"\x722B");
    test_decomp(U"\xFA4A", U"\x7422");
    test_decomp(U"\xFA4B", U"\x7891");
    test_decomp(U"\xFA4C", U"\x793E");
    test_decomp(U"\xFA4D", U"\x7949");
    test_decomp(U"\xFA4E", U"\x7948");
    test_decomp(U"\xFA4F", U"\x7950");
    test_decomp(U"\xFA50", U"\x7956");
    test_decomp(U"\xFA51", U"\x795D");
    test_decomp(U"\xFA52", U"\x798D");
    test_decomp(U"\xFA53", U"\x798E");
    test_decomp(U"\xFA54", U"\x7A40");
    test_decomp(U"\xFA55", U"\x7A81");
    test_decomp(U"\xFA56", U"\x7BC0");
    test_decomp(U"\xFA57", U"\x7DF4");
    test_decomp(U"\xFA58", U"\x7E09");
    test_decomp(U"\xFA59", U"\x7E41");
    test_decomp(U"\xFA5A", U"\x7F72");
    test_decomp(U"\xFA5B", U"\x8005");
    test_decomp(U"\xFA5C", U"\x81ED");
    test_decomp(U"\xFA5D", U"\x8279");
    test_decomp(U"\xFA5E", U"\x8279");
    test_decomp(U"\xFA5F", U"\x8457");
    test_decomp(U"\xFA60", U"\x8910");
    test_decomp(U"\xFA61", U"\x8996");
    test_decomp(U"\xFA62", U"\x8B01");
    test_decomp(U"\xFA63", U"\x8B39");
    test_decomp(U"\xFA64", U"\x8CD3");
    test_decomp(U"\xFA65", U"\x8D08");
    test_decomp(U"\xFA66", U"\x8FB6");
    test_decomp(U"\xFA67", U"\x9038");
    test_decomp(U"\xFA68", U"\x96E3");
    test_decomp(U"\xFA69", U"\x97FF");
    test_decomp(U"\xFA6A", U"\x983B");
    test_decomp(U"\xFA6B", U"\x6075");
    test_decomp(U"\xFA6C", U"\x242EE");
    test_decomp(U"\xFA6D", U"\x8218");
    test_decomp(U"\xFA70", U"\x4E26");
    test_decomp(U"\xFA71", U"\x51B5");
    test_decomp(U"\xFA72", U"\x5168");
    test_decomp(U"\xFA73", U"\x4F80");
    test_decomp(U"\xFA74", U"\x5145");
    test_decomp(U"\xFA75", U"\x5180");
    test_decomp(U"\xFA76", U"\x52C7");
    test_decomp(U"\xFA77", U"\x52FA");
    test_decomp(U"\xFA78", U"\x559D");
    test_decomp(U"\xFA79", U"\x5555");
    test_decomp(U"\xFA7A", U"\x5599");
    test_decomp(U"\xFA7B", U"\x55E2");
    test_decomp(U"\xFA7C", U"\x585A");
    test_decomp(U"\xFA7D", U"\x58B3");
    test_decomp(U"\xFA7E", U"\x5944");
    test_decomp(U"\xFA7F", U"\x5954");
    test_decomp(U"\xFA80", U"\x5A62");
    test_decomp(U"\xFA81", U"\x5B28");
    test_decomp(U"\xFA82", U"\x5ED2");
    test_decomp(U"\xFA83", U"\x5ED9");
    test_decomp(U"\xFA84", U"\x5F69");
    test_decomp(U"\xFA85", U"\x5FAD");
    test_decomp(U"\xFA86", U"\x60D8");
    test_decomp(U"\xFA87", U"\x614E");
    test_decomp(U"\xFA88", U"\x6108");
    test_decomp(U"\xFA89", U"\x618E");
    test_decomp(U"\xFA8A", U"\x6160");
    test_decomp(U"\xFA8B", U"\x61F2");
    test_decomp(U"\xFA8C", U"\x6234");
    test_decomp(U"\xFA8D", U"\x63C4");
    test_decomp(U"\xFA8E", U"\x641C");
    test_decomp(U"\xFA8F", U"\x6452");
    test_decomp(U"\xFA90", U"\x6556");
    test_decomp(U"\xFA91", U"\x6674");
    test_decomp(U"\xFA92", U"\x6717");
    test_decomp(U"\xFA93", U"\x671B");
    test_decomp(U"\xFA94", U"\x6756");
    test_decomp(U"\xFA95", U"\x6B79");
    test_decomp(U"\xFA96", U"\x6BBA");
    test_decomp(U"\xFA97", U"\x6D41");
    test_decomp(U"\xFA98", U"\x6EDB");
    test_decomp(U"\xFA99", U"\x6ECB");
    test_decomp(U"\xFA9A", U"\x6F22");
    test_decomp(U"\xFA9B", U"\x701E");
    test_decomp(U"\xFA9C", U"\x716E");
    test_decomp(U"\xFA9D", U"\x77A7");
    test_decomp(U"\xFA9E", U"\x7235");
    test_decomp(U"\xFA9F", U"\x72AF");
    test_decomp(U"\xFAA0", U"\x732A");
    test_decomp(U"\xFAA1", U"\x7471");
    test_decomp(U"\xFAA2", U"\x7506");
    test_decomp(U"\xFAA3", U"\x753B");
    test_decomp(U"\xFAA4", U"\x761D");
    test_decomp(U"\xFAA5", U"\x761F");
    test_decomp(U"\xFAA6", U"\x76CA");
    test_decomp(U"\xFAA7", U"\x76DB");
    test_decomp(U"\xFAA8", U"\x76F4");
    test_decomp(U"\xFAA9", U"\x774A");
    test_decomp(U"\xFAAA", U"\x7740");
    test_decomp(U"\xFAAB", U"\x78CC");
    test_decomp(U"\xFAAC", U"\x7AB1");
    test_decomp(U"\xFAAD", U"\x7BC0");
    test_decomp(U"\xFAAE", U"\x7C7B");
    test_decomp(U"\xFAAF", U"\x7D5B");
    test_decomp(U"\xFAB0", U"\x7DF4");
    test_decomp(U"\xFAB1", U"\x7F3E");
    test_decomp(U"\xFAB2", U"\x8005");
    test_decomp(U"\xFAB3", U"\x8352");
    test_decomp(U"\xFAB4", U"\x83EF");
    test_decomp(U"\xFAB5", U"\x8779");
    test_decomp(U"\xFAB6", U"\x8941");
    test_decomp(U"\xFAB7", U"\x8986");
    test_decomp(U"\xFAB8", U"\x8996");
    test_decomp(U"\xFAB9", U"\x8ABF");
    test_decomp(U"\xFABA", U"\x8AF8");
    test_decomp(U"\xFABB", U"\x8ACB");
    test_decomp(U"\xFABC", U"\x8B01");
    test_decomp(U"\xFABD", U"\x8AFE");
    test_decomp(U"\xFABE", U"\x8AED");
    test_decomp(U"\xFABF", U"\x8B39");
    test_decomp(U"\xFAC0", U"\x8B8A");
    test_decomp(U"\xFAC1", U"\x8D08");
    test_decomp(U"\xFAC2", U"\x8F38");
    test_decomp(U"\xFAC3", U"\x9072");
    test_decomp(U"\xFAC4", U"\x9199");
    test_decomp(U"\xFAC5", U"\x9276");
    test_decomp(U"\xFAC6", U"\x967C");
    test_decomp(U"\xFAC7", U"\x96E3");
    test_decomp(U"\xFAC8", U"\x9756");
    test_decomp(U"\xFAC9", U"\x97DB");
    test_decomp(U"\xFACA", U"\x97FF");
    test_decomp(U"\xFACB", U"\x980B");
    test_decomp(U"\xFACC", U"\x983B");
    test_decomp(U"\xFACD", U"\x9B12");
    test_decomp(U"\xFACE", U"\x9F9C");
    test_decomp(U"\xFACF", U"\x2284A");
    test_decomp(U"\xFAD0", U"\x22844");
    test_decomp(U"\xFAD1", U"\x233D5");
    test_decomp(U"\xFAD2", U"\x3B9D");
    test_decomp(U"\xFAD3", U"\x4018");
    test_decomp(U"\xFAD4", U"\x4039");
    test_decomp(U"\xFAD5", U"\x25249");
    test_decomp(U"\xFAD6", U"\x25CD0");
    test_decomp(U"\xFAD7", U"\x27ED3");
    test_decomp(U"\xFAD8", U"\x9F43");
    test_decomp(U"\xFAD9", U"\x9F8E");
    test_decomp(U"\xFB1D", U"\x05D9\x05B4");
    test_decomp(U"\xFB1F", U"\x05F2\x05B7");
    test_decomp(U"\xFB2A", U"\x05E9\x05C1");
    test_decomp(U"\xFB2B", U"\x05E9\x05C2");
    test_decomp(U"\xFB2C", U"\xFB49\x05C1");
    test_decomp(U"\xFB2D", U"\xFB49\x05C2");
    test_decomp(U"\xFB2E", U"\x05D0\x05B7");
    test_decomp(U"\xFB2F", U"\x05D0\x05B8");
    test_decomp(U"\xFB30", U"\x05D0\x05BC");
    test_decomp(U"\xFB31", U"\x05D1\x05BC");
    test_decomp(U"\xFB32", U"\x05D2\x05BC");
    test_decomp(U"\xFB33", U"\x05D3\x05BC");
    test_decomp(U"\xFB34", U"\x05D4\x05BC");
    test_decomp(U"\xFB35", U"\x05D5\x05BC");
    test_decomp(U"\xFB36", U"\x05D6\x05BC");
    test_decomp(U"\xFB38", U"\x05D8\x05BC");
    test_decomp(U"\xFB39", U"\x05D9\x05BC");
    test_decomp(U"\xFB3A", U"\x05DA\x05BC");
    test_decomp(U"\xFB3B", U"\x05DB\x05BC");
    test_decomp(U"\xFB3C", U"\x05DC\x05BC");
    test_decomp(U"\xFB3E", U"\x05DE\x05BC");
    test_decomp(U"\xFB40", U"\x05E0\x05BC");
    test_decomp(U"\xFB41", U"\x05E1\x05BC");
    test_decomp(U"\xFB43", U"\x05E3\x05BC");
    test_decomp(U"\xFB44", U"\x05E4\x05BC");
    test_decomp(U"\xFB46", U"\x05E6\x05BC");
    test_decomp(U"\xFB47", U"\x05E7\x05BC");
    test_decomp(U"\xFB48", U"\x05E8\x05BC");
    test_decomp(U"\xFB49", U"\x05E9\x05BC");
    test_decomp(U"\xFB4A", U"\x05EA\x05BC");
    test_decomp(U"\xFB4B", U"\x05D5\x05B9");
    test_decomp(U"\xFB4C", U"\x05D1\x05BF");
    test_decomp(U"\xFB4D", U"\x05DB\x05BF");
    test_decomp(U"\xFB4E", U"\x05E4\x05BF");
    test_decomp(U"\x1109A", U"\x11099\x110BA");
    test_decomp(U"\x1109C", U"\x1109B\x110BA");
    test_decomp(U"\x110AB", U"\x110A5\x110BA");
    test_decomp(U"\x1112E", U"\x11131\x11127");
    test_decomp(U"\x1112F", U"\x11132\x11127");
    test_decomp(U"\x1134B", U"\x11347\x1133E");
    test_decomp(U"\x1134C", U"\x11347\x11357");
    test_decomp(U"\x114BB", U"\x114B9\x114BA");
    test_decomp(U"\x114BC", U"\x114B9\x114B0");
    test_decomp(U"\x114BE", U"\x114B9\x114BD");
    test_decomp(U"\x115BA", U"\x115B8\x115AF");
    test_decomp(U"\x115BB", U"\x115B9\x115AF");
    test_decomp(U"\x11938", U"\x11935\x11930");
    test_decomp(U"\x1D15E", U"\x1D157\x1D165");
    test_decomp(U"\x1D15F", U"\x1D158\x1D165");
    test_decomp(U"\x1D160", U"\x1D15F\x1D16E");
    test_decomp(U"\x1D161", U"\x1D15F\x1D16F");
    test_decomp(U"\x1D162", U"\x1D15F\x1D170");
    test_decomp(U"\x1D163", U"\x1D15F\x1D171");
    test_decomp(U"\x1D164", U"\x1D15F\x1D172");
    test_decomp(U"\x1D1BB", U"\x1D1B9\x1D165");
    test_decomp(U"\x1D1BC", U"\x1D1BA\x1D165");
    test_decomp(U"\x1D1BD", U"\x1D1BB\x1D16E");
    test_decomp(U"\x1D1BE", U"\x1D1BC\x1D16E");
    test_decomp(U"\x1D1BF", U"\x1D1BB\x1D16F");
    test_decomp(U"\x1D1C0", U"\x1D1BC\x1D16F");
    test_decomp(U"\x2F800", U"\x4E3D");
    test_decomp(U"\x2F801", U"\x4E38");
    test_decomp(U"\x2F802", U"\x4E41");
    test_decomp(U"\x2F803", U"\x20122");
    test_decomp(U"\x2F804", U"\x4F60");
    test_decomp(U"\x2F805", U"\x4FAE");
    test_decomp(U"\x2F806", U"\x4FBB");
    test_decomp(U"\x2F807", U"\x5002");
    test_decomp(U"\x2F808", U"\x507A");
    test_decomp(U"\x2F809", U"\x5099");
    test_decomp(U"\x2F80A", U"\x50E7");
    test_decomp(U"\x2F80B", U"\x50CF");
    test_decomp(U"\x2F80C", U"\x349E");
    test_decomp(U"\x2F80D", U"\x2063A");
    test_decomp(U"\x2F80E", U"\x514D");
    test_decomp(U"\x2F80F", U"\x5154");
    test_decomp(U"\x2F810", U"\x5164");
    test_decomp(U"\x2F811", U"\x5177");
    test_decomp(U"\x2F812", U"\x2051C");
    test_decomp(U"\x2F813", U"\x34B9");
    test_decomp(U"\x2F814", U"\x5167");
    test_decomp(U"\x2F815", U"\x518D");
    test_decomp(U"\x2F816", U"\x2054B");
    test_decomp(U"\x2F817", U"\x5197");
    test_decomp(U"\x2F818", U"\x51A4");
    test_decomp(U"\x2F819", U"\x4ECC");
    test_decomp(U"\x2F81A", U"\x51AC");
    test_decomp(U"\x2F81B", U"\x51B5");
    test_decomp(U"\x2F81C", U"\x291DF");
    test_decomp(U"\x2F81D", U"\x51F5");
    test_decomp(U"\x2F81E", U"\x5203");
    test_decomp(U"\x2F81F", U"\x34DF");
    test_decomp(U"\x2F820", U"\x523B");
    test_decomp(U"\x2F821", U"\x5246");
    test_decomp(U"\x2F822", U"\x5272");
    test_decomp(U"\x2F823", U"\x5277");
    test_decomp(U"\x2F824", U"\x3515");
    test_decomp(U"\x2F825", U"\x52C7");
    test_decomp(U"\x2F826", U"\x52C9");
    test_decomp(U"\x2F827", U"\x52E4");
    test_decomp(U"\x2F828", U"\x52FA");
    test_decomp(U"\x2F829", U"\x5305");
    test_decomp(U"\x2F82A", U"\x5306");
    test_decomp(U"\x2F82B", U"\x5317");
    test_decomp(U"\x2F82C", U"\x5349");
    test_decomp(U"\x2F82D", U"\x5351");
    test_decomp(U"\x2F82E", U"\x535A");
    test_decomp(U"\x2F82F", U"\x5373");
    test_decomp(U"\x2F830", U"\x537D");
    test_decomp(U"\x2F831", U"\x537F");
    test_decomp(U"\x2F832", U"\x537F");
    test_decomp(U"\x2F833", U"\x537F");
    test_decomp(U"\x2F834", U"\x20A2C");
    test_decomp(U"\x2F835", U"\x7070");
    test_decomp(U"\x2F836", U"\x53CA");
    test_decomp(U"\x2F837", U"\x53DF");
    test_decomp(U"\x2F838", U"\x20B63");
    test_decomp(U"\x2F839", U"\x53EB");
    test_decomp(U"\x2F83A", U"\x53F1");
    test_decomp(U"\x2F83B", U"\x5406");
    test_decomp(U"\x2F83C", U"\x549E");
    test_decomp(U"\x2F83D", U"\x5438");
    test_decomp(U"\x2F83E", U"\x5448");
    test_decomp(U"\x2F83F", U"\x5468");
    test_decomp(U"\x2F840", U"\x54A2");
    test_decomp(U"\x2F841", U"\x54F6");
    test_decomp(U"\x2F842", U"\x5510");
    test_decomp(U"\x2F843", U"\x5553");
    test_decomp(U"\x2F844", U"\x5563");
    test_decomp(U"\x2F845", U"\x5584");
    test_decomp(U"\x2F846", U"\x5584");
    test_decomp(U"\x2F847", U"\x5599");
    test_decomp(U"\x2F848", U"\x55AB");
    test_decomp(U"\x2F849", U"\x55B3");
    test_decomp(U"\x2F84A", U"\x55C2");
    test_decomp(U"\x2F84B", U"\x5716");
    test_decomp(U"\x2F84C", U"\x5606");
    test_decomp(U"\x2F84D", U"\x5717");
    test_decomp(U"\x2F84E", U"\x5651");
    test_decomp(U"\x2F84F", U"\x5674");
    test_decomp(U"\x2F850", U"\x5207");
    test_decomp(U"\x2F851", U"\x58EE");
    test_decomp(U"\x2F852", U"\x57CE");
    test_decomp(U"\x2F853", U"\x57F4");
    test_decomp(U"\x2F854", U"\x580D");
    test_decomp(U"\x2F855", U"\x578B");
    test_decomp(U"\x2F856", U"\x5832");
    test_decomp(U"\x2F857", U"\x5831");
    test_decomp(U"\x2F858", U"\x58AC");
    test_decomp(U"\x2F859", U"\x214E4");
    test_decomp(U"\x2F85A", U"\x58F2");
    test_decomp(U"\x2F85B", U"\x58F7");
    test_decomp(U"\x2F85C", U"\x5906");
    test_decomp(U"\x2F85D", U"\x591A");
    test_decomp(U"\x2F85E", U"\x5922");
    test_decomp(U"\x2F85F", U"\x5962");
    test_decomp(U"\x2F860", U"\x216A8");
    test_decomp(U"\x2F861", U"\x216EA");
    test_decomp(U"\x2F862", U"\x59EC");
    test_decomp(U"\x2F863", U"\x5A1B");
    test_decomp(U"\x2F864", U"\x5A27");
    test_decomp(U"\x2F865", U"\x59D8");
    test_decomp(U"\x2F866", U"\x5A66");
    test_decomp(U"\x2F867", U"\x36EE");
    test_decomp(U"\x2F868", U"\x36FC");
    test_decomp(U"\x2F869", U"\x5B08");
    test_decomp(U"\x2F86A", U"\x5B3E");
    test_decomp(U"\x2F86B", U"\x5B3E");
    test_decomp(U"\x2F86C", U"\x219C8");
    test_decomp(U"\x2F86D", U"\x5BC3");
    test_decomp(U"\x2F86E", U"\x5BD8");
    test_decomp(U"\x2F86F", U"\x5BE7");
    test_decomp(U"\x2F870", U"\x5BF3");
    test_decomp(U"\x2F871", U"\x21B18");
    test_decomp(U"\x2F872", U"\x5BFF");
    test_decomp(U"\x2F873", U"\x5C06");
    test_decomp(U"\x2F874", U"\x5F53");
    test_decomp(U"\x2F875", U"\x5C22");
    test_decomp(U"\x2F876", U"\x3781");
    test_decomp(U"\x2F877", U"\x5C60");
    test_decomp(U"\x2F878", U"\x5C6E");
    test_decomp(U"\x2F879", U"\x5CC0");
    test_decomp(U"\x2F87A", U"\x5C8D");
    test_decomp(U"\x2F87B", U"\x21DE4");
    test_decomp(U"\x2F87C", U"\x5D43");
    test_decomp(U"\x2F87D", U"\x21DE6");
    test_decomp(U"\x2F87E", U"\x5D6E");
    test_decomp(U"\x2F87F", U"\x5D6B");
    test_decomp(U"\x2F880", U"\x5D7C");
    test_decomp(U"\x2F881", U"\x5DE1");
    test_decomp(U"\x2F882", U"\x5DE2");
    test_decomp(U"\x2F883", U"\x382F");
    test_decomp(U"\x2F884", U"\x5DFD");
    test_decomp(U"\x2F885", U"\x5E28");
    test_decomp(U"\x2F886", U"\x5E3D");
    test_decomp(U"\x2F887", U"\x5E69");
    test_decomp(U"\x2F888", U"\x3862");
    test_decomp(U"\x2F889", U"\x22183");
    test_decomp(U"\x2F88A", U"\x387C");
    test_decomp(U"\x2F88B", U"\x5EB0");
    test_decomp(U"\x2F88C", U"\x5EB3");
    test_decomp(U"\x2F88D", U"\x5EB6");
    test_decomp(U"\x2F88E", U"\x5ECA");
    test_decomp(U"\x2F88F", U"\x2A392");
    test_decomp(U"\x2F890", U"\x5EFE");
    test_decomp(U"\x2F891", U"\x22331");
    test_decomp(U"\x2F892", U"\x22331");
    test_decomp(U"\x2F893", U"\x8201");
    test_decomp(U"\x2F894", U"\x5F22");
    test_decomp(U"\x2F895", U"\x5F22");
    test_decomp(U"\x2F896", U"\x38C7");
    test_decomp(U"\x2F897", U"\x232B8");
    test_decomp(U"\x2F898", U"\x261DA");
    test_decomp(U"\x2F899", U"\x5F62");
    test_decomp(U"\x2F89A", U"\x5F6B");
    test_decomp(U"\x2F89B", U"\x38E3");
    test_decomp(U"\x2F89C", U"\x5F9A");
    test_decomp(U"\x2F89D", U"\x5FCD");
    test_decomp(U"\x2F89E", U"\x5FD7");
    test_decomp(U"\x2F89F", U"\x5FF9");
    test_decomp(U"\x2F8A0", U"\x6081");
    test_decomp(U"\x2F8A1", U"\x393A");
    test_decomp(U"\x2F8A2", U"\x391C");
    test_decomp(U"\x2F8A3", U"\x6094");
    test_decomp(U"\x2F8A4", U"\x226D4");
    test_decomp(U"\x2F8A5", U"\x60C7");
    test_decomp(U"\x2F8A6", U"\x6148");
    test_decomp(U"\x2F8A7", U"\x614C");
    test_decomp(U"\x2F8A8", U"\x614E");
    test_decomp(U"\x2F8A9", U"\x614C");
    test_decomp(U"\x2F8AA", U"\x617A");
    test_decomp(U"\x2F8AB", U"\x618E");
    test_decomp(U"\x2F8AC", U"\x61B2");
    test_decomp(U"\x2F8AD", U"\x61A4");
    test_decomp(U"\x2F8AE", U"\x61AF");
    test_decomp(U"\x2F8AF", U"\x61DE");
    test_decomp(U"\x2F8B0", U"\x61F2");
    test_decomp(U"\x2F8B1", U"\x61F6");
    test_decomp(U"\x2F8B2", U"\x6210");
    test_decomp(U"\x2F8B3", U"\x621B");
    test_decomp(U"\x2F8B4", U"\x625D");
    test_decomp(U"\x2F8B5", U"\x62B1");
    test_decomp(U"\x2F8B6", U"\x62D4");
    test_decomp(U"\x2F8B7", U"\x6350");
    test_decomp(U"\x2F8B8", U"\x22B0C");
    test_decomp(U"\x2F8B9", U"\x633D");
    test_decomp(U"\x2F8BA", U"\x62FC");
    test_decomp(U"\x2F8BB", U"\x6368");
    test_decomp(U"\x2F8BC", U"\x6383");
    test_decomp(U"\x2F8BD", U"\x63E4");
    test_decomp(U"\x2F8BE", U"\x22BF1");
    test_decomp(U"\x2F8BF", U"\x6422");
    test_decomp(U"\x2F8C0", U"\x63C5");
    test_decomp(U"\x2F8C1", U"\x63A9");
    test_decomp(U"\x2F8C2", U"\x3A2E");
    test_decomp(U"\x2F8C3", U"\x6469");
    test_decomp(U"\x2F8C4", U"\x647E");
    test_decomp(U"\x2F8C5", U"\x649D");
    test_decomp(U"\x2F8C6", U"\x6477");
    test_decomp(U"\x2F8C7", U"\x3A6C");
    test_decomp(U"\x2F8C8", U"\x654F");
    test_decomp(U"\x2F8C9", U"\x656C");
    test_decomp(U"\x2F8CA", U"\x2300A");
    test_decomp(U"\x2F8CB", U"\x65E3");
    test_decomp(U"\x2F8CC", U"\x66F8");
    test_decomp(U"\x2F8CD", U"\x6649");
    test_decomp(U"\x2F8CE", U"\x3B19");
    test_decomp(U"\x2F8CF", U"\x6691");
    test_decomp(U"\x2F8D0", U"\x3B08");
    test_decomp(U"\x2F8D1", U"\x3AE4");
    test_decomp(U"\x2F8D2", U"\x5192");
    test_decomp(U"\x2F8D3", U"\x5195");
    test_decomp(U"\x2F8D4", U"\x6700");
    test_decomp(U"\x2F8D5", U"\x669C");
    test_decomp(U"\x2F8D6", U"\x80AD");
    test_decomp(U"\x2F8D7", U"\x43D9");
    test_decomp(U"\x2F8D8", U"\x6717");
    test_decomp(U"\x2F8D9", U"\x671B");
    test_decomp(U"\x2F8DA", U"\x6721");
    test_decomp(U"\x2F8DB", U"\x675E");
    test_decomp(U"\x2F8DC", U"\x6753");
    test_decomp(U"\x2F8DD", U"\x233C3");
    test_decomp(U"\x2F8DE", U"\x3B49");
    test_decomp(U"\x2F8DF", U"\x67FA");
    test_decomp(U"\x2F8E0", U"\x6785");
    test_decomp(U"\x2F8E1", U"\x6852");
    test_decomp(U"\x2F8E2", U"\x6885");
    test_decomp(U"\x2F8E3", U"\x2346D");
    test_decomp(U"\x2F8E4", U"\x688E");
    test_decomp(U"\x2F8E5", U"\x681F");
    test_decomp(U"\x2F8E6", U"\x6914");
    test_decomp(U"\x2F8E7", U"\x3B9D");
    test_decomp(U"\x2F8E8", U"\x6942");
    test_decomp(U"\x2F8E9", U"\x69A3");
    test_decomp(U"\x2F8EA", U"\x69EA");
    test_decomp(U"\x2F8EB", U"\x6AA8");
    test_decomp(U"\x2F8EC", U"\x236A3");
    test_decomp(U"\x2F8ED", U"\x6ADB");
    test_decomp(U"\x2F8EE", U"\x3C18");
    test_decomp(U"\x2F8EF", U"\x6B21");
    test_decomp(U"\x2F8F0", U"\x238A7");
    test_decomp(U"\x2F8F1", U"\x6B54");
    test_decomp(U"\x2F8F2", U"\x3C4E");
    test_decomp(U"\x2F8F3", U"\x6B72");
    test_decomp(U"\x2F8F4", U"\x6B9F");
    test_decomp(U"\x2F8F5", U"\x6BBA");
    test_decomp(U"\x2F8F6", U"\x6BBB");
    test_decomp(U"\x2F8F7", U"\x23A8D");
    test_decomp(U"\x2F8F8", U"\x21D0B");
    test_decomp(U"\x2F8F9", U"\x23AFA");
    test_decomp(U"\x2F8FA", U"\x6C4E");
    test_decomp(U"\x2F8FB", U"\x23CBC");
    test_decomp(U"\x2F8FC", U"\x6CBF");
    test_decomp(U"\x2F8FD", U"\x6CCD");
    test_decomp(U"\x2F8FE", U"\x6C67");
    test_decomp(U"\x2F8FF", U"\x6D16");
    test_decomp(U"\x2F900", U"\x6D3E");
    test_decomp(U"\x2F901", U"\x6D77");
    test_decomp(U"\x2F902", U"\x6D41");
    test_decomp(U"\x2F903", U"\x6D69");
    test_decomp(U"\x2F904", U"\x6D78");
    test_decomp(U"\x2F905", U"\x6D85");
    test_decomp(U"\x2F906", U"\x23D1E");
    test_decomp(U"\x2F907", U"\x6D34");
    test_decomp(U"\x2F908", U"\x6E2F");
    test_decomp(U"\x2F909", U"\x6E6E");
    test_decomp(U"\x2F90A", U"\x3D33");
    test_decomp(U"\x2F90B", U"\x6ECB");
    test_decomp(U"\x2F90C", U"\x6EC7");
    test_decomp(U"\x2F90D", U"\x23ED1");
    test_decomp(U"\x2F90E", U"\x6DF9");
    test_decomp(U"\x2F90F", U"\x6F6E");
    test_decomp(U"\x2F910", U"\x23F5E");
    test_decomp(U"\x2F911", U"\x23F8E");
    test_decomp(U"\x2F912", U"\x6FC6");
    test_decomp(U"\x2F913", U"\x7039");
    test_decomp(U"\x2F914", U"\x701E");
    test_decomp(U"\x2F915", U"\x701B");
    test_decomp(U"\x2F916", U"\x3D96");
    test_decomp(U"\x2F917", U"\x704A");
    test_decomp(U"\x2F918", U"\x707D");
    test_decomp(U"\x2F919", U"\x7077");
    test_decomp(U"\x2F91A", U"\x70AD");
    test_decomp(U"\x2F91B", U"\x20525");
    test_decomp(U"\x2F91C", U"\x7145");
    test_decomp(U"\x2F91D", U"\x24263");
    test_decomp(U"\x2F91E", U"\x719C");
    test_decomp(U"\x2F91F", U"\x243AB");
    test_decomp(U"\x2F920", U"\x7228");
    test_decomp(U"\x2F921", U"\x7235");
    test_decomp(U"\x2F922", U"\x7250");
    test_decomp(U"\x2F923", U"\x24608");
    test_decomp(U"\x2F924", U"\x7280");
    test_decomp(U"\x2F925", U"\x7295");
    test_decomp(U"\x2F926", U"\x24735");
    test_decomp(U"\x2F927", U"\x24814");
    test_decomp(U"\x2F928", U"\x737A");
    test_decomp(U"\x2F929", U"\x738B");
    test_decomp(U"\x2F92A", U"\x3EAC");
    test_decomp(U"\x2F92B", U"\x73A5");
    test_decomp(U"\x2F92C", U"\x3EB8");
    test_decomp(U"\x2F92D", U"\x3EB8");
    test_decomp(U"\x2F92E", U"\x7447");
    test_decomp(U"\x2F92F", U"\x745C");
    test_decomp(U"\x2F930", U"\x7471");
    test_decomp(U"\x2F931", U"\x7485");
    test_decomp(U"\x2F932", U"\x74CA");
    test_decomp(U"\x2F933", U"\x3F1B");
    test_decomp(U"\x2F934", U"\x7524");
    test_decomp(U"\x2F935", U"\x24C36");
    test_decomp(U"\x2F936", U"\x753E");
    test_decomp(U"\x2F937", U"\x24C92");
    test_decomp(U"\x2F938", U"\x7570");
    test_decomp(U"\x2F939", U"\x2219F");
    test_decomp(U"\x2F93A", U"\x7610");
    test_decomp(U"\x2F93B", U"\x24FA1");
    test_decomp(U"\x2F93C", U"\x24FB8");
    test_decomp(U"\x2F93D", U"\x25044");
    test_decomp(U"\x2F93E", U"\x3FFC");
    test_decomp(U"\x2F93F", U"\x4008");
    test_decomp(U"\x2F940", U"\x76F4");
    test_decomp(U"\x2F941", U"\x250F3");
    test_decomp(U"\x2F942", U"\x250F2");
    test_decomp(U"\x2F943", U"\x25119");
    test_decomp(U"\x2F944", U"\x25133");
    test_decomp(U"\x2F945", U"\x771E");
    test_decomp(U"\x2F946", U"\x771F");
    test_decomp(U"\x2F947", U"\x771F");
    test_decomp(U"\x2F948", U"\x774A");
    test_decomp(U"\x2F949", U"\x4039");
    test_decomp(U"\x2F94A", U"\x778B");
    test_decomp(U"\x2F94B", U"\x4046");
    test_decomp(U"\x2F94C", U"\x4096");
    test_decomp(U"\x2F94D", U"\x2541D");
    test_decomp(U"\x2F94E", U"\x784E");
    test_decomp(U"\x2F94F", U"\x788C");
    test_decomp(U"\x2F950", U"\x78CC");
    test_decomp(U"\x2F951", U"\x40E3");
    test_decomp(U"\x2F952", U"\x25626");
    test_decomp(U"\x2F953", U"\x7956");
    test_decomp(U"\x2F954", U"\x2569A");
    test_decomp(U"\x2F955", U"\x256C5");
    test_decomp(U"\x2F956", U"\x798F");
    test_decomp(U"\x2F957", U"\x79EB");
    test_decomp(U"\x2F958", U"\x412F");
    test_decomp(U"\x2F959", U"\x7A40");
    test_decomp(U"\x2F95A", U"\x7A4A");
    test_decomp(U"\x2F95B", U"\x7A4F");
    test_decomp(U"\x2F95C", U"\x2597C");
    test_decomp(U"\x2F95D", U"\x25AA7");
    test_decomp(U"\x2F95E", U"\x25AA7");
    test_decomp(U"\x2F95F", U"\x7AEE");
    test_decomp(U"\x2F960", U"\x4202");
    test_decomp(U"\x2F961", U"\x25BAB");
    test_decomp(U"\x2F962", U"\x7BC6");
    test_decomp(U"\x2F963", U"\x7BC9");
    test_decomp(U"\x2F964", U"\x4227");
    test_decomp(U"\x2F965", U"\x25C80");
    test_decomp(U"\x2F966", U"\x7CD2");
    test_decomp(U"\x2F967", U"\x42A0");
    test_decomp(U"\x2F968", U"\x7CE8");
    test_decomp(U"\x2F969", U"\x7CE3");
    test_decomp(U"\x2F96A", U"\x7D00");
    test_decomp(U"\x2F96B", U"\x25F86");
    test_decomp(U"\x2F96C", U"\x7D63");
    test_decomp(U"\x2F96D", U"\x4301");
    test_decomp(U"\x2F96E", U"\x7DC7");
    test_decomp(U"\x2F96F", U"\x7E02");
    test_decomp(U"\x2F970", U"\x7E45");
    test_decomp(U"\x2F971", U"\x4334");
    test_decomp(U"\x2F972", U"\x26228");
    test_decomp(U"\x2F973", U"\x26247");
    test_decomp(U"\x2F974", U"\x4359");
    test_decomp(U"\x2F975", U"\x262D9");
    test_decomp(U"\x2F976", U"\x7F7A");
    test_decomp(U"\x2F977", U"\x2633E");
    test_decomp(U"\x2F978", U"\x7F95");
    test_decomp(U"\x2F979", U"\x7FFA");
    test_decomp(U"\x2F97A", U"\x8005");
    test_decomp(U"\x2F97B", U"\x264DA");
    test_decomp(U"\x2F97C", U"\x26523");
    test_decomp(U"\x2F97D", U"\x8060");
    test_decomp(U"\x2F97E", U"\x265A8");
    test_decomp(U"\x2F97F", U"\x8070");
    test_decomp(U"\x2F980", U"\x2335F");
    test_decomp(U"\x2F981", U"\x43D5");
    test_decomp(U"\x2F982", U"\x80B2");
    test_decomp(U"\x2F983", U"\x8103");
    test_decomp(U"\x2F984", U"\x440B");
    test_decomp(U"\x2F985", U"\x813E");
    test_decomp(U"\x2F986", U"\x5AB5");
    test_decomp(U"\x2F987", U"\x267A7");
    test_decomp(U"\x2F988", U"\x267B5");
    test_decomp(U"\x2F989", U"\x23393");
    test_decomp(U"\x2F98A", U"\x2339C");
    test_decomp(U"\x2F98B", U"\x8201");
    test_decomp(U"\x2F98C", U"\x8204");
    test_decomp(U"\x2F98D", U"\x8F9E");
    test_decomp(U"\x2F98E", U"\x446B");
    test_decomp(U"\x2F98F", U"\x8291");
    test_decomp(U"\x2F990", U"\x828B");
    test_decomp(U"\x2F991", U"\x829D");
    test_decomp(U"\x2F992", U"\x52B3");
    test_decomp(U"\x2F993", U"\x82B1");
    test_decomp(U"\x2F994", U"\x82B3");
    test_decomp(U"\x2F995", U"\x82BD");
    test_decomp(U"\x2F996", U"\x82E6");
    test_decomp(U"\x2F997", U"\x26B3C");
    test_decomp(U"\x2F998", U"\x82E5");
    test_decomp(U"\x2F999", U"\x831D");
    test_decomp(U"\x2F99A", U"\x8363");
    test_decomp(U"\x2F99B", U"\x83AD");
    test_decomp(U"\x2F99C", U"\x8323");
    test_decomp(U"\x2F99D", U"\x83BD");
    test_decomp(U"\x2F99E", U"\x83E7");
    test_decomp(U"\x2F99F", U"\x8457");
    test_decomp(U"\x2F9A0", U"\x8353");
    test_decomp(U"\x2F9A1", U"\x83CA");
    test_decomp(U"\x2F9A2", U"\x83CC");
    test_decomp(U"\x2F9A3", U"\x83DC");
    test_decomp(U"\x2F9A4", U"\x26C36");
    test_decomp(U"\x2F9A5", U"\x26D6B");
    test_decomp(U"\x2F9A6", U"\x26CD5");
    test_decomp(U"\x2F9A7", U"\x452B");
    test_decomp(U"\x2F9A8", U"\x84F1");
    test_decomp(U"\x2F9A9", U"\x84F3");
    test_decomp(U"\x2F9AA", U"\x8516");
    test_decomp(U"\x2F9AB", U"\x273CA");
    test_decomp(U"\x2F9AC", U"\x8564");
    test_decomp(U"\x2F9AD", U"\x26F2C");
    test_decomp(U"\x2F9AE", U"\x455D");
    test_decomp(U"\x2F9AF", U"\x4561");
    test_decomp(U"\x2F9B0", U"\x26FB1");
    test_decomp(U"\x2F9B1", U"\x270D2");
    test_decomp(U"\x2F9B2", U"\x456B");
    test_decomp(U"\x2F9B3", U"\x8650");
    test_decomp(U"\x2F9B4", U"\x865C");
    test_decomp(U"\x2F9B5", U"\x8667");
    test_decomp(U"\x2F9B6", U"\x8669");
    test_decomp(U"\x2F9B7", U"\x86A9");
    test_decomp(U"\x2F9B8", U"\x8688");
    test_decomp(U"\x2F9B9", U"\x870E");
    test_decomp(U"\x2F9BA", U"\x86E2");
    test_decomp(U"\x2F9BB", U"\x8779");
    test_decomp(U"\x2F9BC", U"\x8728");
    test_decomp(U"\x2F9BD", U"\x876B");
    test_decomp(U"\x2F9BE", U"\x8786");
    test_decomp(U"\x2F9BF", U"\x45D7");
    test_decomp(U"\x2F9C0", U"\x87E1");
    test_decomp(U"\x2F9C1", U"\x8801");
    test_decomp(U"\x2F9C2", U"\x45F9");
    test_decomp(U"\x2F9C3", U"\x8860");
    test_decomp(U"\x2F9C4", U"\x8863");
    test_decomp(U"\x2F9C5", U"\x27667");
    test_decomp(U"\x2F9C6", U"\x88D7");
    test_decomp(U"\x2F9C7", U"\x88DE");
    test_decomp(U"\x2F9C8", U"\x4635");
    test_decomp(U"\x2F9C9", U"\x88FA");
    test_decomp(U"\x2F9CA", U"\x34BB");
    test_decomp(U"\x2F9CB", U"\x278AE");
    test_decomp(U"\x2F9CC", U"\x27966");
    test_decomp(U"\x2F9CD", U"\x46BE");
    test_decomp(U"\x2F9CE", U"\x46C7");
    test_decomp(U"\x2F9CF", U"\x8AA0");
    test_decomp(U"\x2F9D0", U"\x8AED");
    test_decomp(U"\x2F9D1", U"\x8B8A");
    test_decomp(U"\x2F9D2", U"\x8C55");
    test_decomp(U"\x2F9D3", U"\x27CA8");
    test_decomp(U"\x2F9D4", U"\x8CAB");
    test_decomp(U"\x2F9D5", U"\x8CC1");
    test_decomp(U"\x2F9D6", U"\x8D1B");
    test_decomp(U"\x2F9D7", U"\x8D77");
    test_decomp(U"\x2F9D8", U"\x27F2F");
    test_decomp(U"\x2F9D9", U"\x20804");
    test_decomp(U"\x2F9DA", U"\x8DCB");
    test_decomp(U"\x2F9DB", U"\x8DBC");
    test_decomp(U"\x2F9DC", U"\x8DF0");
    test_decomp(U"\x2F9DD", U"\x208DE");
    test_decomp(U"\x2F9DE", U"\x8ED4");
    test_decomp(U"\x2F9DF", U"\x8F38");
    test_decomp(U"\x2F9E0", U"\x285D2");
    test_decomp(U"\x2F9E1", U"\x285ED");
    test_decomp(U"\x2F9E2", U"\x9094");
    test_decomp(U"\x2F9E3", U"\x90F1");
    test_decomp(U"\x2F9E4", U"\x9111");
    test_decomp(U"\x2F9E5", U"\x2872E");
    test_decomp(U"\x2F9E6", U"\x911B");
    test_decomp(U"\x2F9E7", U"\x9238");
    test_decomp(U"\x2F9E8", U"\x92D7");
    test_decomp(U"\x2F9E9", U"\x92D8");
    test_decomp(U"\x2F9EA", U"\x927C");
    test_decomp(U"\x2F9EB", U"\x93F9");
    test_decomp(U"\x2F9EC", U"\x9415");
    test_decomp(U"\x2F9ED", U"\x28BFA");
    test_decomp(U"\x2F9EE", U"\x958B");
    test_decomp(U"\x2F9EF", U"\x4995");
    test_decomp(U"\x2F9F0", U"\x95B7");
    test_decomp(U"\x2F9F1", U"\x28D77");
    test_decomp(U"\x2F9F2", U"\x49E6");
    test_decomp(U"\x2F9F3", U"\x96C3");
    test_decomp(U"\x2F9F4", U"\x5DB2");
    test_decomp(U"\x2F9F5", U"\x9723");
    test_decomp(U"\x2F9F6", U"\x29145");
    test_decomp(U"\x2F9F7", U"\x2921A");
    test_decomp(U"\x2F9F8", U"\x4A6E");
    test_decomp(U"\x2F9F9", U"\x4A76");
    test_decomp(U"\x2F9FA", U"\x97E0");
    test_decomp(U"\x2F9FB", U"\x2940A");
    test_decomp(U"\x2F9FC", U"\x4AB2");
    test_decomp(U"\x2F9FD", U"\x29496");
    test_decomp(U"\x2F9FE", U"\x980B");
    test_decomp(U"\x2F9FF", U"\x980B");
    test_decomp(U"\x2FA00", U"\x9829");
    test_decomp(U"\x2FA01", U"\x295B6");
    test_decomp(U"\x2FA02", U"\x98E2");
    test_decomp(U"\x2FA03", U"\x4B33");
    test_decomp(U"\x2FA04", U"\x9929");
    test_decomp(U"\x2FA05", U"\x99A7");
    test_decomp(U"\x2FA06", U"\x99C2");
    test_decomp(U"\x2FA07", U"\x99FE");
    test_decomp(U"\x2FA08", U"\x4BCE");
    test_decomp(U"\x2FA09", U"\x29B30");
    test_decomp(U"\x2FA0A", U"\x9B12");
    test_decomp(U"\x2FA0B", U"\x9C40");
    test_decomp(U"\x2FA0C", U"\x9CFD");
    test_decomp(U"\x2FA0D", U"\x4CCE");
    test_decomp(U"\x2FA0E", U"\x4CED");
    test_decomp(U"\x2FA0F", U"\x9D67");
    test_decomp(U"\x2FA10", U"\x2A0CE");
    test_decomp(U"\x2FA11", U"\x4CF8");
    test_decomp(U"\x2FA12", U"\x2A105");
    test_decomp(U"\x2FA13", U"\x2A20E");
    test_decomp(U"\x2FA14", U"\x2A291");
    test_decomp(U"\x2FA15", U"\x9EBB");
    test_decomp(U"\x2FA16", U"\x4D56");
    test_decomp(U"\x2FA17", U"\x9EF9");
    test_decomp(U"\x2FA18", U"\x9EFE");
    test_decomp(U"\x2FA19", U"\x9F05");
    test_decomp(U"\x2FA1A", U"\x9F0F");
    test_decomp(U"\x2FA1B", U"\x9F16");
    test_decomp(U"\x2FA1C", U"\x9F3B");
    test_decomp(U"\x2FA1D", U"\x2A600");
}

TEST(Unicode, DecomposeUTF32) {
    // clang-format off
    // Get more examples with these commands:
    //  Canonical-only decompositions:
    //   awk 'BEGIN{FS=";"; OF=";"} !/^\s*#|</ {tst = " " $6; gsub(/\s+/, " ", tst); if (tst != " ") { gsub(/\s+/, "\\\x", tst); print "    EXPECT_EQ(canonical_decomposed<u32string>(U\'\\\x" $1 "\'), U\"" tst "\") << desc_decomp_of(U\'\\\x" $1 "\') << \"\\\n  Line: " $0 "\";"; } }' UnicodeData.txt | sort -R | head
    // clang-format on

    EXPECT_EQ(canonical_decomposed<u32string>(U'\x1E0A'), U"\x44\x307") << desc_decomp_of(U'\0');

    EXPECT_EQ(utf8_to_utf32(utf32_to_utf8(U"\x29496")), U"\x29496");
    EXPECT_EQ(utf8_to_utf32(utf32_to_utf8(U"\x308")), U"\x308");

    EXPECT_EQ(canonical_decomposed<u32string>(U'\0'), (u32string{U"\0", 1})) << desc_decomp_of(U'\0');
    EXPECT_EQ(canonical_decomposed<u32string>(U'\1'), (U"\1")) << desc_decomp_of(U'\1');

    // clang-format off
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C0'), U"\x0041\x0300") << desc_decomp_of(U'\x00C0') << "\n  Line: 00C0;LATIN CAPITAL LETTER A WITH GRAVE;Lu;0;L;0041 0300;;;;N;LATIN CAPITAL LETTER A GRAVE;;;00E0;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C1'), U"\x0041\x0301") << desc_decomp_of(U'\x00C1') << "\n  Line: 00C1;LATIN CAPITAL LETTER A WITH ACUTE;Lu;0;L;0041 0301;;;;N;LATIN CAPITAL LETTER A ACUTE;;;00E1;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C2'), U"\x0041\x0302") << desc_decomp_of(U'\x00C2') << "\n  Line: 00C2;LATIN CAPITAL LETTER A WITH CIRCUMFLEX;Lu;0;L;0041 0302;;;;N;LATIN CAPITAL LETTER A CIRCUMFLEX;;;00E2;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C3'), U"\x0041\x0303") << desc_decomp_of(U'\x00C3') << "\n  Line: 00C3;LATIN CAPITAL LETTER A WITH TILDE;Lu;0;L;0041 0303;;;;N;LATIN CAPITAL LETTER A TILDE;;;00E3;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C4'), U"\x0041\x0308") << desc_decomp_of(U'\x00C4') << "\n  Line: 00C4;LATIN CAPITAL LETTER A WITH DIAERESIS;Lu;0;L;0041 0308;;;;N;LATIN CAPITAL LETTER A DIAERESIS;;;00E4;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C5'), U"\x0041\x030A") << desc_decomp_of(U'\x00C5') << "\n  Line: 00C5;LATIN CAPITAL LETTER A WITH RING ABOVE;Lu;0;L;0041 030A;;;;N;LATIN CAPITAL LETTER A RING;;;00E5;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C7'), U"\x0043\x0327") << desc_decomp_of(U'\x00C7') << "\n  Line: 00C7;LATIN CAPITAL LETTER C WITH CEDILLA;Lu;0;L;0043 0327;;;;N;LATIN CAPITAL LETTER C CEDILLA;;;00E7;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C8'), U"\x0045\x0300") << desc_decomp_of(U'\x00C8') << "\n  Line: 00C8;LATIN CAPITAL LETTER E WITH GRAVE;Lu;0;L;0045 0300;;;;N;LATIN CAPITAL LETTER E GRAVE;;;00E8;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00C9'), U"\x0045\x0301") << desc_decomp_of(U'\x00C9') << "\n  Line: 00C9;LATIN CAPITAL LETTER E WITH ACUTE;Lu;0;L;0045 0301;;;;N;LATIN CAPITAL LETTER E ACUTE;;;00E9;";
    EXPECT_EQ(canonical_decomposed<u32string>(U'\x00CA'), U"\x0045\x0302") << desc_decomp_of(U'\x00CA') << "\n  Line: 00CA;LATIN CAPITAL LETTER E WITH CIRCUMFLEX;Lu;0;L;0045 0302;;;;N;LATIN CAPITAL LETTER E CIRCUMFLEX;;;00EA;";
    // clang-format on
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
    // Print the excluded code points:
    // echo "($(cat DerivedNormalizationProps.txt | awk '/^[^#].*Full_Composition_Exclusion/{if (index($1, "..")) {
    //                                                         split($1, range, /\\.\\./)
    //                                                         start = strtonum("0x" range[1]);
    //                                                         end = strtonum("0x" range[2]);
    //                                                         for (i = start; i <= end; i++) {
    //                                                             printf("%04X|", i);
    //                                                         }
    //                                                     } else {
    //                                                         printf("%s|", $1);
    //                                                     }}')#)"
    //
    // which produces this:
    // (0340|0341|0343|0344|0374|037E|0387|0958|0959|095A|095B|095C|095D|095E|095F|09DC|09DD|09DF|0A33|0A36|0A59|0A5A|0A5B|0A5E|0B5C|0B5D|0F43|0F4D|0F52|0F57|0F5C|0F69|0F73|0F75|0F76|0F78|0F81|0F93|0F9D|0FA2|0FA7|0FAC|0FB9|1F71|1F73|1F75|1F77|1F79|1F7B|1F7D|1FBB|1FBE|1FC9|1FCB|1FD3|1FDB|1FE3|1FEB|1FEE|1FEF|1FF9|1FFB|1FFD|2000|2001|2126|212A|212B|2329|232A|2ADC|F900|F901|F902|F903|F904|F905|F906|F907|F908|F909|F90A|F90B|F90C|F90D|F90E|F90F|F910|F911|F912|F913|F914|F915|F916|F917|F918|F919|F91A|F91B|F91C|F91D|F91E|F91F|F920|F921|F922|F923|F924|F925|F926|F927|F928|F929|F92A|F92B|F92C|F92D|F92E|F92F|F930|F931|F932|F933|F934|F935|F936|F937|F938|F939|F93A|F93B|F93C|F93D|F93E|F93F|F940|F941|F942|F943|F944|F945|F946|F947|F948|F949|F94A|F94B|F94C|F94D|F94E|F94F|F950|F951|F952|F953|F954|F955|F956|F957|F958|F959|F95A|F95B|F95C|F95D|F95E|F95F|F960|F961|F962|F963|F964|F965|F966|F967|F968|F969|F96A|F96B|F96C|F96D|F96E|F96F|F970|F971|F972|F973|F974|F975|F976|F977|F978|F979|F97A|F97B|F97C|F97D|F97E|F97F|F980|F981|F982|F983|F984|F985|F986|F987|F988|F989|F98A|F98B|F98C|F98D|F98E|F98F|F990|F991|F992|F993|F994|F995|F996|F997|F998|F999|F99A|F99B|F99C|F99D|F99E|F99F|F9A0|F9A1|F9A2|F9A3|F9A4|F9A5|F9A6|F9A7|F9A8|F9A9|F9AA|F9AB|F9AC|F9AD|F9AE|F9AF|F9B0|F9B1|F9B2|F9B3|F9B4|F9B5|F9B6|F9B7|F9B8|F9B9|F9BA|F9BB|F9BC|F9BD|F9BE|F9BF|F9C0|F9C1|F9C2|F9C3|F9C4|F9C5|F9C6|F9C7|F9C8|F9C9|F9CA|F9CB|F9CC|F9CD|F9CE|F9CF|F9D0|F9D1|F9D2|F9D3|F9D4|F9D5|F9D6|F9D7|F9D8|F9D9|F9DA|F9DB|F9DC|F9DD|F9DE|F9DF|F9E0|F9E1|F9E2|F9E3|F9E4|F9E5|F9E6|F9E7|F9E8|F9E9|F9EA|F9EB|F9EC|F9ED|F9EE|F9EF|F9F0|F9F1|F9F2|F9F3|F9F4|F9F5|F9F6|F9F7|F9F8|F9F9|F9FA|F9FB|F9FC|F9FD|F9FE|F9FF|FA00|FA01|FA02|FA03|FA04|FA05|FA06|FA07|FA08|FA09|FA0A|FA0B|FA0C|FA0D|FA10|FA12|FA15|FA16|FA17|FA18|FA19|FA1A|FA1B|FA1C|FA1D|FA1E|FA20|FA22|FA25|FA26|FA2A|FA2B|FA2C|FA2D|FA2E|FA2F|FA30|FA31|FA32|FA33|FA34|FA35|FA36|FA37|FA38|FA39|FA3A|FA3B|FA3C|FA3D|FA3E|FA3F|FA40|FA41|FA42|FA43|FA44|FA45|FA46|FA47|FA48|FA49|FA4A|FA4B|FA4C|FA4D|FA4E|FA4F|FA50|FA51|FA52|FA53|FA54|FA55|FA56|FA57|FA58|FA59|FA5A|FA5B|FA5C|FA5D|FA5E|FA5F|FA60|FA61|FA62|FA63|FA64|FA65|FA66|FA67|FA68|FA69|FA6A|FA6B|FA6C|FA6D|FA70|FA71|FA72|FA73|FA74|FA75|FA76|FA77|FA78|FA79|FA7A|FA7B|FA7C|FA7D|FA7E|FA7F|FA80|FA81|FA82|FA83|FA84|FA85|FA86|FA87|FA88|FA89|FA8A|FA8B|FA8C|FA8D|FA8E|FA8F|FA90|FA91|FA92|FA93|FA94|FA95|FA96|FA97|FA98|FA99|FA9A|FA9B|FA9C|FA9D|FA9E|FA9F|FAA0|FAA1|FAA2|FAA3|FAA4|FAA5|FAA6|FAA7|FAA8|FAA9|FAAA|FAAB|FAAC|FAAD|FAAE|FAAF|FAB0|FAB1|FAB2|FAB3|FAB4|FAB5|FAB6|FAB7|FAB8|FAB9|FABA|FABB|FABC|FABD|FABE|FABF|FAC0|FAC1|FAC2|FAC3|FAC4|FAC5|FAC6|FAC7|FAC8|FAC9|FACA|FACB|FACC|FACD|FACE|FACF|FAD0|FAD1|FAD2|FAD3|FAD4|FAD5|FAD6|FAD7|FAD8|FAD9|FB1D|FB1F|FB2A|FB2B|FB2C|FB2D|FB2E|FB2F|FB30|FB31|FB32|FB33|FB34|FB35|FB36|FB38|FB39|FB3A|FB3B|FB3C|FB3E|FB40|FB41|FB43|FB44|FB46|FB47|FB48|FB49|FB4A|FB4B|FB4C|FB4D|FB4E|1D15E|1D15F|1D160|1D161|1D162|1D163|1D164|1D1BB|1D1BC|1D1BD|1D1BE|1D1BF|1D1C0|2F800|2F801|2F802|2F803|2F804|2F805|2F806|2F807|2F808|2F809|2F80A|2F80B|2F80C|2F80D|2F80E|2F80F|2F810|2F811|2F812|2F813|2F814|2F815|2F816|2F817|2F818|2F819|2F81A|2F81B|2F81C|2F81D|2F81E|2F81F|2F820|2F821|2F822|2F823|2F824|2F825|2F826|2F827|2F828|2F829|2F82A|2F82B|2F82C|2F82D|2F82E|2F82F|2F830|2F831|2F832|2F833|2F834|2F835|2F836|2F837|2F838|2F839|2F83A|2F83B|2F83C|2F83D|2F83E|2F83F|2F840|2F841|2F842|2F843|2F844|2F845|2F846|2F847|2F848|2F849|2F84A|2F84B|2F84C|2F84D|2F84E|2F84F|2F850|2F851|2F852|2F853|2F854|2F855|2F856|2F857|2F858|2F859|2F85A|2F85B|2F85C|2F85D|2F85E|2F85F|2F860|2F861|2F862|2F863|2F864|2F865|2F866|2F867|2F868|2F869|2F86A|2F86B|2F86C|2F86D|2F86E|2F86F|2F870|2F871|2F872|2F873|2F874|2F875|2F876|2F877|2F878|2F879|2F87A|2F87B|2F87C|2F87D|2F87E|2F87F|2F880|2F881|2F882|2F883|2F884|2F885|2F886|2F887|2F888|2F889|2F88A|2F88B|2F88C|2F88D|2F88E|2F88F|2F890|2F891|2F892|2F893|2F894|2F895|2F896|2F897|2F898|2F899|2F89A|2F89B|2F89C|2F89D|2F89E|2F89F|2F8A0|2F8A1|2F8A2|2F8A3|2F8A4|2F8A5|2F8A6|2F8A7|2F8A8|2F8A9|2F8AA|2F8AB|2F8AC|2F8AD|2F8AE|2F8AF|2F8B0|2F8B1|2F8B2|2F8B3|2F8B4|2F8B5|2F8B6|2F8B7|2F8B8|2F8B9|2F8BA|2F8BB|2F8BC|2F8BD|2F8BE|2F8BF|2F8C0|2F8C1|2F8C2|2F8C3|2F8C4|2F8C5|2F8C6|2F8C7|2F8C8|2F8C9|2F8CA|2F8CB|2F8CC|2F8CD|2F8CE|2F8CF|2F8D0|2F8D1|2F8D2|2F8D3|2F8D4|2F8D5|2F8D6|2F8D7|2F8D8|2F8D9|2F8DA|2F8DB|2F8DC|2F8DD|2F8DE|2F8DF|2F8E0|2F8E1|2F8E2|2F8E3|2F8E4|2F8E5|2F8E6|2F8E7|2F8E8|2F8E9|2F8EA|2F8EB|2F8EC|2F8ED|2F8EE|2F8EF|2F8F0|2F8F1|2F8F2|2F8F3|2F8F4|2F8F5|2F8F6|2F8F7|2F8F8|2F8F9|2F8FA|2F8FB|2F8FC|2F8FD|2F8FE|2F8FF|2F900|2F901|2F902|2F903|2F904|2F905|2F906|2F907|2F908|2F909|2F90A|2F90B|2F90C|2F90D|2F90E|2F90F|2F910|2F911|2F912|2F913|2F914|2F915|2F916|2F917|2F918|2F919|2F91A|2F91B|2F91C|2F91D|2F91E|2F91F|2F920|2F921|2F922|2F923|2F924|2F925|2F926|2F927|2F928|2F929|2F92A|2F92B|2F92C|2F92D|2F92E|2F92F|2F930|2F931|2F932|2F933|2F934|2F935|2F936|2F937|2F938|2F939|2F93A|2F93B|2F93C|2F93D|2F93E|2F93F|2F940|2F941|2F942|2F943|2F944|2F945|2F946|2F947|2F948|2F949|2F94A|2F94B|2F94C|2F94D|2F94E|2F94F|2F950|2F951|2F952|2F953|2F954|2F955|2F956|2F957|2F958|2F959|2F95A|2F95B|2F95C|2F95D|2F95E|2F95F|2F960|2F961|2F962|2F963|2F964|2F965|2F966|2F967|2F968|2F969|2F96A|2F96B|2F96C|2F96D|2F96E|2F96F|2F970|2F971|2F972|2F973|2F974|2F975|2F976|2F977|2F978|2F979|2F97A|2F97B|2F97C|2F97D|2F97E|2F97F|2F980|2F981|2F982|2F983|2F984|2F985|2F986|2F987|2F988|2F989|2F98A|2F98B|2F98C|2F98D|2F98E|2F98F|2F990|2F991|2F992|2F993|2F994|2F995|2F996|2F997|2F998|2F999|2F99A|2F99B|2F99C|2F99D|2F99E|2F99F|2F9A0|2F9A1|2F9A2|2F9A3|2F9A4|2F9A5|2F9A6|2F9A7|2F9A8|2F9A9|2F9AA|2F9AB|2F9AC|2F9AD|2F9AE|2F9AF|2F9B0|2F9B1|2F9B2|2F9B3|2F9B4|2F9B5|2F9B6|2F9B7|2F9B8|2F9B9|2F9BA|2F9BB|2F9BC|2F9BD|2F9BE|2F9BF|2F9C0|2F9C1|2F9C2|2F9C3|2F9C4|2F9C5|2F9C6|2F9C7|2F9C8|2F9C9|2F9CA|2F9CB|2F9CC|2F9CD|2F9CE|2F9CF|2F9D0|2F9D1|2F9D2|2F9D3|2F9D4|2F9D5|2F9D6|2F9D7|2F9D8|2F9D9|2F9DA|2F9DB|2F9DC|2F9DD|2F9DE|2F9DF|2F9E0|2F9E1|2F9E2|2F9E3|2F9E4|2F9E5|2F9E6|2F9E7|2F9E8|2F9E9|2F9EA|2F9EB|2F9EC|2F9ED|2F9EE|2F9EF|2F9F0|2F9F1|2F9F2|2F9F3|2F9F4|2F9F5|2F9F6|2F9F7|2F9F8|2F9F9|2F9FA|2F9FB|2F9FC|2F9FD|2F9FE|2F9FF|2FA00|2FA01|2FA02|2FA03|2FA04|2FA05|2FA06|2FA07|2FA08|2FA09|2FA0A|2FA0B|2FA0C|2FA0D|2FA0E|2FA0F|2FA10|2FA11|2FA12|2FA13|2FA14|2FA15|2FA16|2FA17|2FA18|2FA19|2FA1A|2FA1B|2FA1C|2FA1D|#)
    //
    // Print the code compositions themselves:
    // awk 'BEGIN{FS=";"; OF=""} !/^\s*#/ { $6 = " " $6; gsub(/\s+/, " ", $6); if ($6 != " " && $6 ~ /\S\s\S/ && !($6 ~ /</)) { $6 = gensub(/\s+/, ", 0x", 2, $6); gsub(/^\s/, "0x", $6); print "EXPECT_EQ(canonical_composed(" $6 "), 0x" $1 ");"; } }' UnicodeData.txt | sort -R | head
    //
    // Print only non-excluded compositions (merge of the both commands):
    // awk 'BEGIN{FS=";"; OF=""} !/^\s*#/ { tst = " " $6; gsub(/\s+/, " ", tst); if (tst != " " && tst ~ /\S\s\S/ && !(tst ~ /</)) { tst = gensub(/\s+/, ", 0x", 2, tst); gsub(/^\s/, "0x", tst); print "EXPECT_EQ(canonical_composed(" tst "), 0x" $1 ") << \"  Line: " $0 "\";"; } }' UnicodeData.txt | grep -vE "($(cat DerivedNormalizationProps.txt | awk
    // '/^[^#].*Full_Composition_Exclusion/{if (index($1, "..")) {
    //                                                             split($1, range, /\\.\\./)
    //                                                             start = strtonum("0x" range[1]);
    //                                                             end = strtonum("0x" range[2]);
    //                                                             for (i = start; i <= end; i++) {
    //                                                                 printf("%04X|", i);
    //                                                             }
    //                                                         } else {
    //                                                             printf("%s|", $1);
    //                                                         }}')#)"
    //
    // clang-format on
    using webpp::unicode::canonical_composed;

    // specials:
    EXPECT_EQ(canonical_composed(70'375, 43'456), webpp::unicode::replacement_char<char32_t>);

    // cat DerivedNormalizationProps.txt | grep 308 | grep 301
    // 0344          ; NFKC_CF; 0308 0301      # Mn       COMBINING GREEK DIALYTIKA TONOS
    // It's NFKC_CaseFold, not a canonical composed
    EXPECT_EQ(canonical_composed(0x308, 0x301), webpp::unicode::replacement_char<char32_t>);

    // clang-format off
    EXPECT_EQ(canonical_composed(0x0041, 0x0300), 0x00C0) << "  Line: 00C0;LATIN CAPITAL LETTER A WITH GRAVE;Lu;0;L;0041 0300;;;;N;LATIN CAPITAL LETTER A GRAVE;;;00E0;";
    EXPECT_EQ(canonical_composed(0x0041, 0x0301), 0x00C1) << "  Line: 00C1;LATIN CAPITAL LETTER A WITH ACUTE;Lu;0;L;0041 0301;;;;N;LATIN CAPITAL LETTER A ACUTE;;;00E1;";
    EXPECT_EQ(canonical_composed(0x0041, 0x0302), 0x00C2) << "  Line: 00C2;LATIN CAPITAL LETTER A WITH CIRCUMFLEX;Lu;0;L;0041 0302;;;;N;LATIN CAPITAL LETTER A CIRCUMFLEX;;;00E2;";
    EXPECT_EQ(canonical_composed(0x0041, 0x0303), 0x00C3) << "  Line: 00C3;LATIN CAPITAL LETTER A WITH TILDE;Lu;0;L;0041 0303;;;;N;LATIN CAPITAL LETTER A TILDE;;;00E3;";
    EXPECT_EQ(canonical_composed(0x0041, 0x0308), 0x00C4) << "  Line: 00C4;LATIN CAPITAL LETTER A WITH DIAERESIS;Lu;0;L;0041 0308;;;;N;LATIN CAPITAL LETTER A DIAERESIS;;;00E4;";
    EXPECT_EQ(canonical_composed(0x0041, 0x030A), 0x00C5) << "  Line: 00C5;LATIN CAPITAL LETTER A WITH RING ABOVE;Lu;0;L;0041 030A;;;;N;LATIN CAPITAL LETTER A RING;;;00E5;";
    EXPECT_EQ(canonical_composed(0x0043, 0x0327), 0x00C7) << "  Line: 00C7;LATIN CAPITAL LETTER C WITH CEDILLA;Lu;0;L;0043 0327;;;;N;LATIN CAPITAL LETTER C CEDILLA;;;00E7;";
    EXPECT_EQ(canonical_composed(0x0045, 0x0300), 0x00C8) << "  Line: 00C8;LATIN CAPITAL LETTER E WITH GRAVE;Lu;0;L;0045 0300;;;;N;LATIN CAPITAL LETTER E GRAVE;;;00E8;";
    EXPECT_EQ(canonical_composed(0x0045, 0x0301), 0x00C9) << "  Line: 00C9;LATIN CAPITAL LETTER E WITH ACUTE;Lu;0;L;0045 0301;;;;N;LATIN CAPITAL LETTER E ACUTE;;;00E9;";
    EXPECT_EQ(canonical_composed(0x0045, 0x0302), 0x00CA) << "  Line: 00CA;LATIN CAPITAL LETTER E WITH CIRCUMFLEX;Lu;0;L;0045 0302;;;;N;LATIN CAPITAL LETTER E CIRCUMFLEX;;;00EA;";
    EXPECT_EQ(canonical_composed(0x0045, 0x0308), 0x00CB) << "  Line: 00CB;LATIN CAPITAL LETTER E WITH DIAERESIS;Lu;0;L;0045 0308;;;;N;LATIN CAPITAL LETTER E DIAERESIS;;;00EB;";
    EXPECT_EQ(canonical_composed(0x0049, 0x0300), 0x00CC) << "  Line: 00CC;LATIN CAPITAL LETTER I WITH GRAVE;Lu;0;L;0049 0300;;;;N;LATIN CAPITAL LETTER I GRAVE;;;00EC;";
    EXPECT_EQ(canonical_composed(0x0049, 0x0301), 0x00CD) << "  Line: 00CD;LATIN CAPITAL LETTER I WITH ACUTE;Lu;0;L;0049 0301;;;;N;LATIN CAPITAL LETTER I ACUTE;;;00ED;";
    EXPECT_EQ(canonical_composed(0x0049, 0x0302), 0x00CE) << "  Line: 00CE;LATIN CAPITAL LETTER I WITH CIRCUMFLEX;Lu;0;L;0049 0302;;;;N;LATIN CAPITAL LETTER I CIRCUMFLEX;;;00EE;";
    EXPECT_EQ(canonical_composed(0x0049, 0x0308), 0x00CF) << "  Line: 00CF;LATIN CAPITAL LETTER I WITH DIAERESIS;Lu;0;L;0049 0308;;;;N;LATIN CAPITAL LETTER I DIAERESIS;;;00EF;";
    EXPECT_EQ(canonical_composed(0x004E, 0x0303), 0x00D1) << "  Line: 00D1;LATIN CAPITAL LETTER N WITH TILDE;Lu;0;L;004E 0303;;;;N;LATIN CAPITAL LETTER N TILDE;;;00F1;";
    EXPECT_EQ(canonical_composed(0x004F, 0x0300), 0x00D2) << "  Line: 00D2;LATIN CAPITAL LETTER O WITH GRAVE;Lu;0;L;004F 0300;;;;N;LATIN CAPITAL LETTER O GRAVE;;;00F2;";
    EXPECT_EQ(canonical_composed(0x004F, 0x0301), 0x00D3) << "  Line: 00D3;LATIN CAPITAL LETTER O WITH ACUTE;Lu;0;L;004F 0301;;;;N;LATIN CAPITAL LETTER O ACUTE;;;00F3;";
    EXPECT_EQ(canonical_composed(0x004F, 0x0302), 0x00D4) << "  Line: 00D4;LATIN CAPITAL LETTER O WITH CIRCUMFLEX;Lu;0;L;004F 0302;;;;N;LATIN CAPITAL LETTER O CIRCUMFLEX;;;00F4;";
    EXPECT_EQ(canonical_composed(0x004F, 0x0303), 0x00D5) << "  Line: 00D5;LATIN CAPITAL LETTER O WITH TILDE;Lu;0;L;004F 0303;;;;N;LATIN CAPITAL LETTER O TILDE;;;00F5;";
    EXPECT_EQ(canonical_composed(0x004F, 0x0308), 0x00D6) << "  Line: 00D6;LATIN CAPITAL LETTER O WITH DIAERESIS;Lu;0;L;004F 0308;;;;N;LATIN CAPITAL LETTER O DIAERESIS;;;00F6;";
    EXPECT_EQ(canonical_composed(0x0055, 0x0300), 0x00D9) << "  Line: 00D9;LATIN CAPITAL LETTER U WITH GRAVE;Lu;0;L;0055 0300;;;;N;LATIN CAPITAL LETTER U GRAVE;;;00F9;";
    EXPECT_EQ(canonical_composed(0x0055, 0x0301), 0x00DA) << "  Line: 00DA;LATIN CAPITAL LETTER U WITH ACUTE;Lu;0;L;0055 0301;;;;N;LATIN CAPITAL LETTER U ACUTE;;;00FA;";
    EXPECT_EQ(canonical_composed(0x0055, 0x0302), 0x00DB) << "  Line: 00DB;LATIN CAPITAL LETTER U WITH CIRCUMFLEX;Lu;0;L;0055 0302;;;;N;LATIN CAPITAL LETTER U CIRCUMFLEX;;;00FB;";
    EXPECT_EQ(canonical_composed(0x0055, 0x0308), 0x00DC) << "  Line: 00DC;LATIN CAPITAL LETTER U WITH DIAERESIS;Lu;0;L;0055 0308;;;;N;LATIN CAPITAL LETTER U DIAERESIS;;;00FC;";
    EXPECT_EQ(canonical_composed(0x0059, 0x0301), 0x00DD) << "  Line: 00DD;LATIN CAPITAL LETTER Y WITH ACUTE;Lu;0;L;0059 0301;;;;N;LATIN CAPITAL LETTER Y ACUTE;;;00FD;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0300), 0x00E0) << "  Line: 00E0;LATIN SMALL LETTER A WITH GRAVE;Ll;0;L;0061 0300;;;;N;LATIN SMALL LETTER A GRAVE;;00C0;;00C0";
    EXPECT_EQ(canonical_composed(0x0061, 0x0301), 0x00E1) << "  Line: 00E1;LATIN SMALL LETTER A WITH ACUTE;Ll;0;L;0061 0301;;;;N;LATIN SMALL LETTER A ACUTE;;00C1;;00C1";
    EXPECT_EQ(canonical_composed(0x0061, 0x0302), 0x00E2) << "  Line: 00E2;LATIN SMALL LETTER A WITH CIRCUMFLEX;Ll;0;L;0061 0302;;;;N;LATIN SMALL LETTER A CIRCUMFLEX;;00C2;;00C2";
    EXPECT_EQ(canonical_composed(0x0061, 0x0303), 0x00E3) << "  Line: 00E3;LATIN SMALL LETTER A WITH TILDE;Ll;0;L;0061 0303;;;;N;LATIN SMALL LETTER A TILDE;;00C3;;00C3";
    EXPECT_EQ(canonical_composed(0x0061, 0x0308), 0x00E4) << "  Line: 00E4;LATIN SMALL LETTER A WITH DIAERESIS;Ll;0;L;0061 0308;;;;N;LATIN SMALL LETTER A DIAERESIS;;00C4;;00C4";
    EXPECT_EQ(canonical_composed(0x0061, 0x030A), 0x00E5) << "  Line: 00E5;LATIN SMALL LETTER A WITH RING ABOVE;Ll;0;L;0061 030A;;;;N;LATIN SMALL LETTER A RING;;00C5;;00C5";
    EXPECT_EQ(canonical_composed(0x0063, 0x0327), 0x00E7) << "  Line: 00E7;LATIN SMALL LETTER C WITH CEDILLA;Ll;0;L;0063 0327;;;;N;LATIN SMALL LETTER C CEDILLA;;00C7;;00C7";
    EXPECT_EQ(canonical_composed(0x0065, 0x0300), 0x00E8) << "  Line: 00E8;LATIN SMALL LETTER E WITH GRAVE;Ll;0;L;0065 0300;;;;N;LATIN SMALL LETTER E GRAVE;;00C8;;00C8";
    EXPECT_EQ(canonical_composed(0x0065, 0x0301), 0x00E9) << "  Line: 00E9;LATIN SMALL LETTER E WITH ACUTE;Ll;0;L;0065 0301;;;;N;LATIN SMALL LETTER E ACUTE;;00C9;;00C9";
    EXPECT_EQ(canonical_composed(0x0065, 0x0302), 0x00EA) << "  Line: 00EA;LATIN SMALL LETTER E WITH CIRCUMFLEX;Ll;0;L;0065 0302;;;;N;LATIN SMALL LETTER E CIRCUMFLEX;;00CA;;00CA";
    EXPECT_EQ(canonical_composed(0x0065, 0x0308), 0x00EB) << "  Line: 00EB;LATIN SMALL LETTER E WITH DIAERESIS;Ll;0;L;0065 0308;;;;N;LATIN SMALL LETTER E DIAERESIS;;00CB;;00CB";
    EXPECT_EQ(canonical_composed(0x0069, 0x0300), 0x00EC) << "  Line: 00EC;LATIN SMALL LETTER I WITH GRAVE;Ll;0;L;0069 0300;;;;N;LATIN SMALL LETTER I GRAVE;;00CC;;00CC";
    EXPECT_EQ(canonical_composed(0x0069, 0x0301), 0x00ED) << "  Line: 00ED;LATIN SMALL LETTER I WITH ACUTE;Ll;0;L;0069 0301;;;;N;LATIN SMALL LETTER I ACUTE;;00CD;;00CD";
    EXPECT_EQ(canonical_composed(0x0069, 0x0302), 0x00EE) << "  Line: 00EE;LATIN SMALL LETTER I WITH CIRCUMFLEX;Ll;0;L;0069 0302;;;;N;LATIN SMALL LETTER I CIRCUMFLEX;;00CE;;00CE";
    EXPECT_EQ(canonical_composed(0x0069, 0x0308), 0x00EF) << "  Line: 00EF;LATIN SMALL LETTER I WITH DIAERESIS;Ll;0;L;0069 0308;;;;N;LATIN SMALL LETTER I DIAERESIS;;00CF;;00CF";
    EXPECT_EQ(canonical_composed(0x006E, 0x0303), 0x00F1) << "  Line: 00F1;LATIN SMALL LETTER N WITH TILDE;Ll;0;L;006E 0303;;;;N;LATIN SMALL LETTER N TILDE;;00D1;;00D1";
    EXPECT_EQ(canonical_composed(0x006F, 0x0300), 0x00F2) << "  Line: 00F2;LATIN SMALL LETTER O WITH GRAVE;Ll;0;L;006F 0300;;;;N;LATIN SMALL LETTER O GRAVE;;00D2;;00D2";
    EXPECT_EQ(canonical_composed(0x006F, 0x0301), 0x00F3) << "  Line: 00F3;LATIN SMALL LETTER O WITH ACUTE;Ll;0;L;006F 0301;;;;N;LATIN SMALL LETTER O ACUTE;;00D3;;00D3";
    EXPECT_EQ(canonical_composed(0x006F, 0x0302), 0x00F4) << "  Line: 00F4;LATIN SMALL LETTER O WITH CIRCUMFLEX;Ll;0;L;006F 0302;;;;N;LATIN SMALL LETTER O CIRCUMFLEX;;00D4;;00D4";
    EXPECT_EQ(canonical_composed(0x006F, 0x0303), 0x00F5) << "  Line: 00F5;LATIN SMALL LETTER O WITH TILDE;Ll;0;L;006F 0303;;;;N;LATIN SMALL LETTER O TILDE;;00D5;;00D5";
    EXPECT_EQ(canonical_composed(0x006F, 0x0308), 0x00F6) << "  Line: 00F6;LATIN SMALL LETTER O WITH DIAERESIS;Ll;0;L;006F 0308;;;;N;LATIN SMALL LETTER O DIAERESIS;;00D6;;00D6";
    EXPECT_EQ(canonical_composed(0x0075, 0x0300), 0x00F9) << "  Line: 00F9;LATIN SMALL LETTER U WITH GRAVE;Ll;0;L;0075 0300;;;;N;LATIN SMALL LETTER U GRAVE;;00D9;;00D9";
    EXPECT_EQ(canonical_composed(0x0075, 0x0301), 0x00FA) << "  Line: 00FA;LATIN SMALL LETTER U WITH ACUTE;Ll;0;L;0075 0301;;;;N;LATIN SMALL LETTER U ACUTE;;00DA;;00DA";
    EXPECT_EQ(canonical_composed(0x0075, 0x0302), 0x00FB) << "  Line: 00FB;LATIN SMALL LETTER U WITH CIRCUMFLEX;Ll;0;L;0075 0302;;;;N;LATIN SMALL LETTER U CIRCUMFLEX;;00DB;;00DB";
    EXPECT_EQ(canonical_composed(0x0075, 0x0308), 0x00FC) << "  Line: 00FC;LATIN SMALL LETTER U WITH DIAERESIS;Ll;0;L;0075 0308;;;;N;LATIN SMALL LETTER U DIAERESIS;;00DC;;00DC";
    EXPECT_EQ(canonical_composed(0x0079, 0x0301), 0x00FD) << "  Line: 00FD;LATIN SMALL LETTER Y WITH ACUTE;Ll;0;L;0079 0301;;;;N;LATIN SMALL LETTER Y ACUTE;;00DD;;00DD";
    EXPECT_EQ(canonical_composed(0x0079, 0x0308), 0x00FF) << "  Line: 00FF;LATIN SMALL LETTER Y WITH DIAERESIS;Ll;0;L;0079 0308;;;;N;LATIN SMALL LETTER Y DIAERESIS;;0178;;0178";
    EXPECT_EQ(canonical_composed(0x0041, 0x0304), 0x0100) << "  Line: 0100;LATIN CAPITAL LETTER A WITH MACRON;Lu;0;L;0041 0304;;;;N;LATIN CAPITAL LETTER A MACRON;;;0101;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0304), 0x0101) << "  Line: 0101;LATIN SMALL LETTER A WITH MACRON;Ll;0;L;0061 0304;;;;N;LATIN SMALL LETTER A MACRON;;0100;;0100";
    EXPECT_EQ(canonical_composed(0x0041, 0x0306), 0x0102) << "  Line: 0102;LATIN CAPITAL LETTER A WITH BREVE;Lu;0;L;0041 0306;;;;N;LATIN CAPITAL LETTER A BREVE;;;0103;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0306), 0x0103) << "  Line: 0103;LATIN SMALL LETTER A WITH BREVE;Ll;0;L;0061 0306;;;;N;LATIN SMALL LETTER A BREVE;;0102;;0102";
    EXPECT_EQ(canonical_composed(0x0041, 0x0328), 0x0104) << "  Line: 0104;LATIN CAPITAL LETTER A WITH OGONEK;Lu;0;L;0041 0328;;;;N;LATIN CAPITAL LETTER A OGONEK;;;0105;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0328), 0x0105) << "  Line: 0105;LATIN SMALL LETTER A WITH OGONEK;Ll;0;L;0061 0328;;;;N;LATIN SMALL LETTER A OGONEK;;0104;;0104";
    EXPECT_EQ(canonical_composed(0x0043, 0x0301), 0x0106) << "  Line: 0106;LATIN CAPITAL LETTER C WITH ACUTE;Lu;0;L;0043 0301;;;;N;LATIN CAPITAL LETTER C ACUTE;;;0107;";
    EXPECT_EQ(canonical_composed(0x0063, 0x0301), 0x0107) << "  Line: 0107;LATIN SMALL LETTER C WITH ACUTE;Ll;0;L;0063 0301;;;;N;LATIN SMALL LETTER C ACUTE;;0106;;0106";
    EXPECT_EQ(canonical_composed(0x0043, 0x0302), 0x0108) << "  Line: 0108;LATIN CAPITAL LETTER C WITH CIRCUMFLEX;Lu;0;L;0043 0302;;;;N;LATIN CAPITAL LETTER C CIRCUMFLEX;;;0109;";
    EXPECT_EQ(canonical_composed(0x0063, 0x0302), 0x0109) << "  Line: 0109;LATIN SMALL LETTER C WITH CIRCUMFLEX;Ll;0;L;0063 0302;;;;N;LATIN SMALL LETTER C CIRCUMFLEX;;0108;;0108";
    EXPECT_EQ(canonical_composed(0x0043, 0x0307), 0x010A) << "  Line: 010A;LATIN CAPITAL LETTER C WITH DOT ABOVE;Lu;0;L;0043 0307;;;;N;LATIN CAPITAL LETTER C DOT;;;010B;";
    EXPECT_EQ(canonical_composed(0x0063, 0x0307), 0x010B) << "  Line: 010B;LATIN SMALL LETTER C WITH DOT ABOVE;Ll;0;L;0063 0307;;;;N;LATIN SMALL LETTER C DOT;;010A;;010A";
    EXPECT_EQ(canonical_composed(0x0043, 0x030C), 0x010C) << "  Line: 010C;LATIN CAPITAL LETTER C WITH CARON;Lu;0;L;0043 030C;;;;N;LATIN CAPITAL LETTER C HACEK;;;010D;";
    EXPECT_EQ(canonical_composed(0x0063, 0x030C), 0x010D) << "  Line: 010D;LATIN SMALL LETTER C WITH CARON;Ll;0;L;0063 030C;;;;N;LATIN SMALL LETTER C HACEK;;010C;;010C";
    EXPECT_EQ(canonical_composed(0x0044, 0x030C), 0x010E) << "  Line: 010E;LATIN CAPITAL LETTER D WITH CARON;Lu;0;L;0044 030C;;;;N;LATIN CAPITAL LETTER D HACEK;;;010F;";
    EXPECT_EQ(canonical_composed(0x0064, 0x030C), 0x010F) << "  Line: 010F;LATIN SMALL LETTER D WITH CARON;Ll;0;L;0064 030C;;;;N;LATIN SMALL LETTER D HACEK;;010E;;010E";
    EXPECT_EQ(canonical_composed(0x0045, 0x0304), 0x0112) << "  Line: 0112;LATIN CAPITAL LETTER E WITH MACRON;Lu;0;L;0045 0304;;;;N;LATIN CAPITAL LETTER E MACRON;;;0113;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0304), 0x0113) << "  Line: 0113;LATIN SMALL LETTER E WITH MACRON;Ll;0;L;0065 0304;;;;N;LATIN SMALL LETTER E MACRON;;0112;;0112";
    EXPECT_EQ(canonical_composed(0x0045, 0x0306), 0x0114) << "  Line: 0114;LATIN CAPITAL LETTER E WITH BREVE;Lu;0;L;0045 0306;;;;N;LATIN CAPITAL LETTER E BREVE;;;0115;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0306), 0x0115) << "  Line: 0115;LATIN SMALL LETTER E WITH BREVE;Ll;0;L;0065 0306;;;;N;LATIN SMALL LETTER E BREVE;;0114;;0114";
    EXPECT_EQ(canonical_composed(0x0045, 0x0307), 0x0116) << "  Line: 0116;LATIN CAPITAL LETTER E WITH DOT ABOVE;Lu;0;L;0045 0307;;;;N;LATIN CAPITAL LETTER E DOT;;;0117;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0307), 0x0117) << "  Line: 0117;LATIN SMALL LETTER E WITH DOT ABOVE;Ll;0;L;0065 0307;;;;N;LATIN SMALL LETTER E DOT;;0116;;0116";
    EXPECT_EQ(canonical_composed(0x0045, 0x0328), 0x0118) << "  Line: 0118;LATIN CAPITAL LETTER E WITH OGONEK;Lu;0;L;0045 0328;;;;N;LATIN CAPITAL LETTER E OGONEK;;;0119;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0328), 0x0119) << "  Line: 0119;LATIN SMALL LETTER E WITH OGONEK;Ll;0;L;0065 0328;;;;N;LATIN SMALL LETTER E OGONEK;;0118;;0118";
    EXPECT_EQ(canonical_composed(0x0045, 0x030C), 0x011A) << "  Line: 011A;LATIN CAPITAL LETTER E WITH CARON;Lu;0;L;0045 030C;;;;N;LATIN CAPITAL LETTER E HACEK;;;011B;";
    EXPECT_EQ(canonical_composed(0x0065, 0x030C), 0x011B) << "  Line: 011B;LATIN SMALL LETTER E WITH CARON;Ll;0;L;0065 030C;;;;N;LATIN SMALL LETTER E HACEK;;011A;;011A";
    EXPECT_EQ(canonical_composed(0x0047, 0x0302), 0x011C) << "  Line: 011C;LATIN CAPITAL LETTER G WITH CIRCUMFLEX;Lu;0;L;0047 0302;;;;N;LATIN CAPITAL LETTER G CIRCUMFLEX;;;011D;";
    EXPECT_EQ(canonical_composed(0x0067, 0x0302), 0x011D) << "  Line: 011D;LATIN SMALL LETTER G WITH CIRCUMFLEX;Ll;0;L;0067 0302;;;;N;LATIN SMALL LETTER G CIRCUMFLEX;;011C;;011C";
    EXPECT_EQ(canonical_composed(0x0047, 0x0306), 0x011E) << "  Line: 011E;LATIN CAPITAL LETTER G WITH BREVE;Lu;0;L;0047 0306;;;;N;LATIN CAPITAL LETTER G BREVE;;;011F;";
    EXPECT_EQ(canonical_composed(0x0067, 0x0306), 0x011F) << "  Line: 011F;LATIN SMALL LETTER G WITH BREVE;Ll;0;L;0067 0306;;;;N;LATIN SMALL LETTER G BREVE;;011E;;011E";
    EXPECT_EQ(canonical_composed(0x0047, 0x0307), 0x0120) << "  Line: 0120;LATIN CAPITAL LETTER G WITH DOT ABOVE;Lu;0;L;0047 0307;;;;N;LATIN CAPITAL LETTER G DOT;;;0121;";
    EXPECT_EQ(canonical_composed(0x0067, 0x0307), 0x0121) << "  Line: 0121;LATIN SMALL LETTER G WITH DOT ABOVE;Ll;0;L;0067 0307;;;;N;LATIN SMALL LETTER G DOT;;0120;;0120";
    EXPECT_EQ(canonical_composed(0x0047, 0x0327), 0x0122) << "  Line: 0122;LATIN CAPITAL LETTER G WITH CEDILLA;Lu;0;L;0047 0327;;;;N;LATIN CAPITAL LETTER G CEDILLA;;;0123;";
    EXPECT_EQ(canonical_composed(0x0067, 0x0327), 0x0123) << "  Line: 0123;LATIN SMALL LETTER G WITH CEDILLA;Ll;0;L;0067 0327;;;;N;LATIN SMALL LETTER G CEDILLA;;0122;;0122";
    EXPECT_EQ(canonical_composed(0x0048, 0x0302), 0x0124) << "  Line: 0124;LATIN CAPITAL LETTER H WITH CIRCUMFLEX;Lu;0;L;0048 0302;;;;N;LATIN CAPITAL LETTER H CIRCUMFLEX;;;0125;";
    EXPECT_EQ(canonical_composed(0x0068, 0x0302), 0x0125) << "  Line: 0125;LATIN SMALL LETTER H WITH CIRCUMFLEX;Ll;0;L;0068 0302;;;;N;LATIN SMALL LETTER H CIRCUMFLEX;;0124;;0124";
    EXPECT_EQ(canonical_composed(0x0049, 0x0303), 0x0128) << "  Line: 0128;LATIN CAPITAL LETTER I WITH TILDE;Lu;0;L;0049 0303;;;;N;LATIN CAPITAL LETTER I TILDE;;;0129;";
    EXPECT_EQ(canonical_composed(0x0069, 0x0303), 0x0129) << "  Line: 0129;LATIN SMALL LETTER I WITH TILDE;Ll;0;L;0069 0303;;;;N;LATIN SMALL LETTER I TILDE;;0128;;0128";
    EXPECT_EQ(canonical_composed(0x0049, 0x0304), 0x012A) << "  Line: 012A;LATIN CAPITAL LETTER I WITH MACRON;Lu;0;L;0049 0304;;;;N;LATIN CAPITAL LETTER I MACRON;;;012B;";
    EXPECT_EQ(canonical_composed(0x0069, 0x0304), 0x012B) << "  Line: 012B;LATIN SMALL LETTER I WITH MACRON;Ll;0;L;0069 0304;;;;N;LATIN SMALL LETTER I MACRON;;012A;;012A";
    EXPECT_EQ(canonical_composed(0x0049, 0x0306), 0x012C) << "  Line: 012C;LATIN CAPITAL LETTER I WITH BREVE;Lu;0;L;0049 0306;;;;N;LATIN CAPITAL LETTER I BREVE;;;012D;";
    EXPECT_EQ(canonical_composed(0x0069, 0x0306), 0x012D) << "  Line: 012D;LATIN SMALL LETTER I WITH BREVE;Ll;0;L;0069 0306;;;;N;LATIN SMALL LETTER I BREVE;;012C;;012C";
    EXPECT_EQ(canonical_composed(0x0049, 0x0328), 0x012E) << "  Line: 012E;LATIN CAPITAL LETTER I WITH OGONEK;Lu;0;L;0049 0328;;;;N;LATIN CAPITAL LETTER I OGONEK;;;012F;";
    EXPECT_EQ(canonical_composed(0x0069, 0x0328), 0x012F) << "  Line: 012F;LATIN SMALL LETTER I WITH OGONEK;Ll;0;L;0069 0328;;;;N;LATIN SMALL LETTER I OGONEK;;012E;;012E";
    EXPECT_EQ(canonical_composed(0x0049, 0x0307), 0x0130) << "  Line: 0130;LATIN CAPITAL LETTER I WITH DOT ABOVE;Lu;0;L;0049 0307;;;;N;LATIN CAPITAL LETTER I DOT;;;0069;";
    EXPECT_EQ(canonical_composed(0x004A, 0x0302), 0x0134) << "  Line: 0134;LATIN CAPITAL LETTER J WITH CIRCUMFLEX;Lu;0;L;004A 0302;;;;N;LATIN CAPITAL LETTER J CIRCUMFLEX;;;0135;";
    EXPECT_EQ(canonical_composed(0x006A, 0x0302), 0x0135) << "  Line: 0135;LATIN SMALL LETTER J WITH CIRCUMFLEX;Ll;0;L;006A 0302;;;;N;LATIN SMALL LETTER J CIRCUMFLEX;;0134;;0134";
    EXPECT_EQ(canonical_composed(0x004B, 0x0327), 0x0136) << "  Line: 0136;LATIN CAPITAL LETTER K WITH CEDILLA;Lu;0;L;004B 0327;;;;N;LATIN CAPITAL LETTER K CEDILLA;;;0137;";
    EXPECT_EQ(canonical_composed(0x006B, 0x0327), 0x0137) << "  Line: 0137;LATIN SMALL LETTER K WITH CEDILLA;Ll;0;L;006B 0327;;;;N;LATIN SMALL LETTER K CEDILLA;;0136;;0136";
    EXPECT_EQ(canonical_composed(0x004C, 0x0301), 0x0139) << "  Line: 0139;LATIN CAPITAL LETTER L WITH ACUTE;Lu;0;L;004C 0301;;;;N;LATIN CAPITAL LETTER L ACUTE;;;013A;";
    EXPECT_EQ(canonical_composed(0x006C, 0x0301), 0x013A) << "  Line: 013A;LATIN SMALL LETTER L WITH ACUTE;Ll;0;L;006C 0301;;;;N;LATIN SMALL LETTER L ACUTE;;0139;;0139";
    EXPECT_EQ(canonical_composed(0x004C, 0x0327), 0x013B) << "  Line: 013B;LATIN CAPITAL LETTER L WITH CEDILLA;Lu;0;L;004C 0327;;;;N;LATIN CAPITAL LETTER L CEDILLA;;;013C;";
    EXPECT_EQ(canonical_composed(0x006C, 0x0327), 0x013C) << "  Line: 013C;LATIN SMALL LETTER L WITH CEDILLA;Ll;0;L;006C 0327;;;;N;LATIN SMALL LETTER L CEDILLA;;013B;;013B";
    EXPECT_EQ(canonical_composed(0x004C, 0x030C), 0x013D) << "  Line: 013D;LATIN CAPITAL LETTER L WITH CARON;Lu;0;L;004C 030C;;;;N;LATIN CAPITAL LETTER L HACEK;;;013E;";
    EXPECT_EQ(canonical_composed(0x006C, 0x030C), 0x013E) << "  Line: 013E;LATIN SMALL LETTER L WITH CARON;Ll;0;L;006C 030C;;;;N;LATIN SMALL LETTER L HACEK;;013D;;013D";
    EXPECT_EQ(canonical_composed(0x004E, 0x0301), 0x0143) << "  Line: 0143;LATIN CAPITAL LETTER N WITH ACUTE;Lu;0;L;004E 0301;;;;N;LATIN CAPITAL LETTER N ACUTE;;;0144;";
    EXPECT_EQ(canonical_composed(0x006E, 0x0301), 0x0144) << "  Line: 0144;LATIN SMALL LETTER N WITH ACUTE;Ll;0;L;006E 0301;;;;N;LATIN SMALL LETTER N ACUTE;;0143;;0143";
    EXPECT_EQ(canonical_composed(0x004E, 0x0327), 0x0145) << "  Line: 0145;LATIN CAPITAL LETTER N WITH CEDILLA;Lu;0;L;004E 0327;;;;N;LATIN CAPITAL LETTER N CEDILLA;;;0146;";
    EXPECT_EQ(canonical_composed(0x006E, 0x0327), 0x0146) << "  Line: 0146;LATIN SMALL LETTER N WITH CEDILLA;Ll;0;L;006E 0327;;;;N;LATIN SMALL LETTER N CEDILLA;;0145;;0145";
    EXPECT_EQ(canonical_composed(0x004E, 0x030C), 0x0147) << "  Line: 0147;LATIN CAPITAL LETTER N WITH CARON;Lu;0;L;004E 030C;;;;N;LATIN CAPITAL LETTER N HACEK;;;0148;";
    EXPECT_EQ(canonical_composed(0x006E, 0x030C), 0x0148) << "  Line: 0148;LATIN SMALL LETTER N WITH CARON;Ll;0;L;006E 030C;;;;N;LATIN SMALL LETTER N HACEK;;0147;;0147";
    EXPECT_EQ(canonical_composed(0x004F, 0x0304), 0x014C) << "  Line: 014C;LATIN CAPITAL LETTER O WITH MACRON;Lu;0;L;004F 0304;;;;N;LATIN CAPITAL LETTER O MACRON;;;014D;";
    EXPECT_EQ(canonical_composed(0x006F, 0x0304), 0x014D) << "  Line: 014D;LATIN SMALL LETTER O WITH MACRON;Ll;0;L;006F 0304;;;;N;LATIN SMALL LETTER O MACRON;;014C;;014C";
    EXPECT_EQ(canonical_composed(0x004F, 0x0306), 0x014E) << "  Line: 014E;LATIN CAPITAL LETTER O WITH BREVE;Lu;0;L;004F 0306;;;;N;LATIN CAPITAL LETTER O BREVE;;;014F;";
    EXPECT_EQ(canonical_composed(0x006F, 0x0306), 0x014F) << "  Line: 014F;LATIN SMALL LETTER O WITH BREVE;Ll;0;L;006F 0306;;;;N;LATIN SMALL LETTER O BREVE;;014E;;014E";
    EXPECT_EQ(canonical_composed(0x004F, 0x030B), 0x0150) << "  Line: 0150;LATIN CAPITAL LETTER O WITH DOUBLE ACUTE;Lu;0;L;004F 030B;;;;N;LATIN CAPITAL LETTER O DOUBLE ACUTE;;;0151;";
    EXPECT_EQ(canonical_composed(0x006F, 0x030B), 0x0151) << "  Line: 0151;LATIN SMALL LETTER O WITH DOUBLE ACUTE;Ll;0;L;006F 030B;;;;N;LATIN SMALL LETTER O DOUBLE ACUTE;;0150;;0150";
    EXPECT_EQ(canonical_composed(0x0052, 0x0301), 0x0154) << "  Line: 0154;LATIN CAPITAL LETTER R WITH ACUTE;Lu;0;L;0052 0301;;;;N;LATIN CAPITAL LETTER R ACUTE;;;0155;";
    EXPECT_EQ(canonical_composed(0x0072, 0x0301), 0x0155) << "  Line: 0155;LATIN SMALL LETTER R WITH ACUTE;Ll;0;L;0072 0301;;;;N;LATIN SMALL LETTER R ACUTE;;0154;;0154";
    EXPECT_EQ(canonical_composed(0x0052, 0x0327), 0x0156) << "  Line: 0156;LATIN CAPITAL LETTER R WITH CEDILLA;Lu;0;L;0052 0327;;;;N;LATIN CAPITAL LETTER R CEDILLA;;;0157;";
    EXPECT_EQ(canonical_composed(0x0072, 0x0327), 0x0157) << "  Line: 0157;LATIN SMALL LETTER R WITH CEDILLA;Ll;0;L;0072 0327;;;;N;LATIN SMALL LETTER R CEDILLA;;0156;;0156";
    EXPECT_EQ(canonical_composed(0x0052, 0x030C), 0x0158) << "  Line: 0158;LATIN CAPITAL LETTER R WITH CARON;Lu;0;L;0052 030C;;;;N;LATIN CAPITAL LETTER R HACEK;;;0159;";
    EXPECT_EQ(canonical_composed(0x0072, 0x030C), 0x0159) << "  Line: 0159;LATIN SMALL LETTER R WITH CARON;Ll;0;L;0072 030C;;;;N;LATIN SMALL LETTER R HACEK;;0158;;0158";
    EXPECT_EQ(canonical_composed(0x0053, 0x0301), 0x015A) << "  Line: 015A;LATIN CAPITAL LETTER S WITH ACUTE;Lu;0;L;0053 0301;;;;N;LATIN CAPITAL LETTER S ACUTE;;;015B;";
    EXPECT_EQ(canonical_composed(0x0073, 0x0301), 0x015B) << "  Line: 015B;LATIN SMALL LETTER S WITH ACUTE;Ll;0;L;0073 0301;;;;N;LATIN SMALL LETTER S ACUTE;;015A;;015A";
    EXPECT_EQ(canonical_composed(0x0053, 0x0302), 0x015C) << "  Line: 015C;LATIN CAPITAL LETTER S WITH CIRCUMFLEX;Lu;0;L;0053 0302;;;;N;LATIN CAPITAL LETTER S CIRCUMFLEX;;;015D;";
    EXPECT_EQ(canonical_composed(0x0073, 0x0302), 0x015D) << "  Line: 015D;LATIN SMALL LETTER S WITH CIRCUMFLEX;Ll;0;L;0073 0302;;;;N;LATIN SMALL LETTER S CIRCUMFLEX;;015C;;015C";
    EXPECT_EQ(canonical_composed(0x0053, 0x0327), 0x015E) << "  Line: 015E;LATIN CAPITAL LETTER S WITH CEDILLA;Lu;0;L;0053 0327;;;;N;LATIN CAPITAL LETTER S CEDILLA;;;015F;";
    EXPECT_EQ(canonical_composed(0x0073, 0x0327), 0x015F) << "  Line: 015F;LATIN SMALL LETTER S WITH CEDILLA;Ll;0;L;0073 0327;;;;N;LATIN SMALL LETTER S CEDILLA;;015E;;015E";
    EXPECT_EQ(canonical_composed(0x0053, 0x030C), 0x0160) << "  Line: 0160;LATIN CAPITAL LETTER S WITH CARON;Lu;0;L;0053 030C;;;;N;LATIN CAPITAL LETTER S HACEK;;;0161;";
    EXPECT_EQ(canonical_composed(0x0073, 0x030C), 0x0161) << "  Line: 0161;LATIN SMALL LETTER S WITH CARON;Ll;0;L;0073 030C;;;;N;LATIN SMALL LETTER S HACEK;;0160;;0160";
    EXPECT_EQ(canonical_composed(0x0054, 0x0327), 0x0162) << "  Line: 0162;LATIN CAPITAL LETTER T WITH CEDILLA;Lu;0;L;0054 0327;;;;N;LATIN CAPITAL LETTER T CEDILLA;;;0163;";
    EXPECT_EQ(canonical_composed(0x0074, 0x0327), 0x0163) << "  Line: 0163;LATIN SMALL LETTER T WITH CEDILLA;Ll;0;L;0074 0327;;;;N;LATIN SMALL LETTER T CEDILLA;;0162;;0162";
    EXPECT_EQ(canonical_composed(0x0054, 0x030C), 0x0164) << "  Line: 0164;LATIN CAPITAL LETTER T WITH CARON;Lu;0;L;0054 030C;;;;N;LATIN CAPITAL LETTER T HACEK;;;0165;";
    EXPECT_EQ(canonical_composed(0x0074, 0x030C), 0x0165) << "  Line: 0165;LATIN SMALL LETTER T WITH CARON;Ll;0;L;0074 030C;;;;N;LATIN SMALL LETTER T HACEK;;0164;;0164";
    EXPECT_EQ(canonical_composed(0x0055, 0x0303), 0x0168) << "  Line: 0168;LATIN CAPITAL LETTER U WITH TILDE;Lu;0;L;0055 0303;;;;N;LATIN CAPITAL LETTER U TILDE;;;0169;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0303), 0x0169) << "  Line: 0169;LATIN SMALL LETTER U WITH TILDE;Ll;0;L;0075 0303;;;;N;LATIN SMALL LETTER U TILDE;;0168;;0168";
    EXPECT_EQ(canonical_composed(0x0055, 0x0304), 0x016A) << "  Line: 016A;LATIN CAPITAL LETTER U WITH MACRON;Lu;0;L;0055 0304;;;;N;LATIN CAPITAL LETTER U MACRON;;;016B;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0304), 0x016B) << "  Line: 016B;LATIN SMALL LETTER U WITH MACRON;Ll;0;L;0075 0304;;;;N;LATIN SMALL LETTER U MACRON;;016A;;016A";
    EXPECT_EQ(canonical_composed(0x0055, 0x0306), 0x016C) << "  Line: 016C;LATIN CAPITAL LETTER U WITH BREVE;Lu;0;L;0055 0306;;;;N;LATIN CAPITAL LETTER U BREVE;;;016D;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0306), 0x016D) << "  Line: 016D;LATIN SMALL LETTER U WITH BREVE;Ll;0;L;0075 0306;;;;N;LATIN SMALL LETTER U BREVE;;016C;;016C";
    EXPECT_EQ(canonical_composed(0x0055, 0x030A), 0x016E) << "  Line: 016E;LATIN CAPITAL LETTER U WITH RING ABOVE;Lu;0;L;0055 030A;;;;N;LATIN CAPITAL LETTER U RING;;;016F;";
    EXPECT_EQ(canonical_composed(0x0075, 0x030A), 0x016F) << "  Line: 016F;LATIN SMALL LETTER U WITH RING ABOVE;Ll;0;L;0075 030A;;;;N;LATIN SMALL LETTER U RING;;016E;;016E";
    EXPECT_EQ(canonical_composed(0x0055, 0x030B), 0x0170) << "  Line: 0170;LATIN CAPITAL LETTER U WITH DOUBLE ACUTE;Lu;0;L;0055 030B;;;;N;LATIN CAPITAL LETTER U DOUBLE ACUTE;;;0171;";
    EXPECT_EQ(canonical_composed(0x0075, 0x030B), 0x0171) << "  Line: 0171;LATIN SMALL LETTER U WITH DOUBLE ACUTE;Ll;0;L;0075 030B;;;;N;LATIN SMALL LETTER U DOUBLE ACUTE;;0170;;0170";
    EXPECT_EQ(canonical_composed(0x0055, 0x0328), 0x0172) << "  Line: 0172;LATIN CAPITAL LETTER U WITH OGONEK;Lu;0;L;0055 0328;;;;N;LATIN CAPITAL LETTER U OGONEK;;;0173;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0328), 0x0173) << "  Line: 0173;LATIN SMALL LETTER U WITH OGONEK;Ll;0;L;0075 0328;;;;N;LATIN SMALL LETTER U OGONEK;;0172;;0172";
    EXPECT_EQ(canonical_composed(0x0057, 0x0302), 0x0174) << "  Line: 0174;LATIN CAPITAL LETTER W WITH CIRCUMFLEX;Lu;0;L;0057 0302;;;;N;LATIN CAPITAL LETTER W CIRCUMFLEX;;;0175;";
    EXPECT_EQ(canonical_composed(0x0077, 0x0302), 0x0175) << "  Line: 0175;LATIN SMALL LETTER W WITH CIRCUMFLEX;Ll;0;L;0077 0302;;;;N;LATIN SMALL LETTER W CIRCUMFLEX;;0174;;0174";
    EXPECT_EQ(canonical_composed(0x0059, 0x0302), 0x0176) << "  Line: 0176;LATIN CAPITAL LETTER Y WITH CIRCUMFLEX;Lu;0;L;0059 0302;;;;N;LATIN CAPITAL LETTER Y CIRCUMFLEX;;;0177;";
    EXPECT_EQ(canonical_composed(0x0079, 0x0302), 0x0177) << "  Line: 0177;LATIN SMALL LETTER Y WITH CIRCUMFLEX;Ll;0;L;0079 0302;;;;N;LATIN SMALL LETTER Y CIRCUMFLEX;;0176;;0176";
    EXPECT_EQ(canonical_composed(0x0059, 0x0308), 0x0178) << "  Line: 0178;LATIN CAPITAL LETTER Y WITH DIAERESIS;Lu;0;L;0059 0308;;;;N;LATIN CAPITAL LETTER Y DIAERESIS;;;00FF;";
    EXPECT_EQ(canonical_composed(0x005A, 0x0301), 0x0179) << "  Line: 0179;LATIN CAPITAL LETTER Z WITH ACUTE;Lu;0;L;005A 0301;;;;N;LATIN CAPITAL LETTER Z ACUTE;;;017A;";
    EXPECT_EQ(canonical_composed(0x007A, 0x0301), 0x017A) << "  Line: 017A;LATIN SMALL LETTER Z WITH ACUTE;Ll;0;L;007A 0301;;;;N;LATIN SMALL LETTER Z ACUTE;;0179;;0179";
    EXPECT_EQ(canonical_composed(0x005A, 0x0307), 0x017B) << "  Line: 017B;LATIN CAPITAL LETTER Z WITH DOT ABOVE;Lu;0;L;005A 0307;;;;N;LATIN CAPITAL LETTER Z DOT;;;017C;";
    EXPECT_EQ(canonical_composed(0x007A, 0x0307), 0x017C) << "  Line: 017C;LATIN SMALL LETTER Z WITH DOT ABOVE;Ll;0;L;007A 0307;;;;N;LATIN SMALL LETTER Z DOT;;017B;;017B";
    EXPECT_EQ(canonical_composed(0x005A, 0x030C), 0x017D) << "  Line: 017D;LATIN CAPITAL LETTER Z WITH CARON;Lu;0;L;005A 030C;;;;N;LATIN CAPITAL LETTER Z HACEK;;;017E;";
    EXPECT_EQ(canonical_composed(0x007A, 0x030C), 0x017E) << "  Line: 017E;LATIN SMALL LETTER Z WITH CARON;Ll;0;L;007A 030C;;;;N;LATIN SMALL LETTER Z HACEK;;017D;;017D";
    EXPECT_EQ(canonical_composed(0x004F, 0x031B), 0x01A0) << "  Line: 01A0;LATIN CAPITAL LETTER O WITH HORN;Lu;0;L;004F 031B;;;;N;LATIN CAPITAL LETTER O HORN;;;01A1;";
    EXPECT_EQ(canonical_composed(0x006F, 0x031B), 0x01A1) << "  Line: 01A1;LATIN SMALL LETTER O WITH HORN;Ll;0;L;006F 031B;;;;N;LATIN SMALL LETTER O HORN;;01A0;;01A0";
    EXPECT_EQ(canonical_composed(0x0055, 0x031B), 0x01AF) << "  Line: 01AF;LATIN CAPITAL LETTER U WITH HORN;Lu;0;L;0055 031B;;;;N;LATIN CAPITAL LETTER U HORN;;;01B0;";
    EXPECT_EQ(canonical_composed(0x0075, 0x031B), 0x01B0) << "  Line: 01B0;LATIN SMALL LETTER U WITH HORN;Ll;0;L;0075 031B;;;;N;LATIN SMALL LETTER U HORN;;01AF;;01AF";
    EXPECT_EQ(canonical_composed(0x0041, 0x030C), 0x01CD) << "  Line: 01CD;LATIN CAPITAL LETTER A WITH CARON;Lu;0;L;0041 030C;;;;N;LATIN CAPITAL LETTER A HACEK;;;01CE;";
    EXPECT_EQ(canonical_composed(0x0061, 0x030C), 0x01CE) << "  Line: 01CE;LATIN SMALL LETTER A WITH CARON;Ll;0;L;0061 030C;;;;N;LATIN SMALL LETTER A HACEK;;01CD;;01CD";
    EXPECT_EQ(canonical_composed(0x0049, 0x030C), 0x01CF) << "  Line: 01CF;LATIN CAPITAL LETTER I WITH CARON;Lu;0;L;0049 030C;;;;N;LATIN CAPITAL LETTER I HACEK;;;01D0;";
    EXPECT_EQ(canonical_composed(0x0069, 0x030C), 0x01D0) << "  Line: 01D0;LATIN SMALL LETTER I WITH CARON;Ll;0;L;0069 030C;;;;N;LATIN SMALL LETTER I HACEK;;01CF;;01CF";
    EXPECT_EQ(canonical_composed(0x004F, 0x030C), 0x01D1) << "  Line: 01D1;LATIN CAPITAL LETTER O WITH CARON;Lu;0;L;004F 030C;;;;N;LATIN CAPITAL LETTER O HACEK;;;01D2;";
    EXPECT_EQ(canonical_composed(0x006F, 0x030C), 0x01D2) << "  Line: 01D2;LATIN SMALL LETTER O WITH CARON;Ll;0;L;006F 030C;;;;N;LATIN SMALL LETTER O HACEK;;01D1;;01D1";
    EXPECT_EQ(canonical_composed(0x0055, 0x030C), 0x01D3) << "  Line: 01D3;LATIN CAPITAL LETTER U WITH CARON;Lu;0;L;0055 030C;;;;N;LATIN CAPITAL LETTER U HACEK;;;01D4;";
    EXPECT_EQ(canonical_composed(0x0075, 0x030C), 0x01D4) << "  Line: 01D4;LATIN SMALL LETTER U WITH CARON;Ll;0;L;0075 030C;;;;N;LATIN SMALL LETTER U HACEK;;01D3;;01D3";
    EXPECT_EQ(canonical_composed(0x00DC, 0x0304), 0x01D5) << "  Line: 01D5;LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON;Lu;0;L;00DC 0304;;;;N;LATIN CAPITAL LETTER U DIAERESIS MACRON;;;01D6;";
    EXPECT_EQ(canonical_composed(0x00FC, 0x0304), 0x01D6) << "  Line: 01D6;LATIN SMALL LETTER U WITH DIAERESIS AND MACRON;Ll;0;L;00FC 0304;;;;N;LATIN SMALL LETTER U DIAERESIS MACRON;;01D5;;01D5";
    EXPECT_EQ(canonical_composed(0x00DC, 0x0301), 0x01D7) << "  Line: 01D7;LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE;Lu;0;L;00DC 0301;;;;N;LATIN CAPITAL LETTER U DIAERESIS ACUTE;;;01D8;";
    EXPECT_EQ(canonical_composed(0x00FC, 0x0301), 0x01D8) << "  Line: 01D8;LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE;Ll;0;L;00FC 0301;;;;N;LATIN SMALL LETTER U DIAERESIS ACUTE;;01D7;;01D7";
    EXPECT_EQ(canonical_composed(0x00DC, 0x030C), 0x01D9) << "  Line: 01D9;LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON;Lu;0;L;00DC 030C;;;;N;LATIN CAPITAL LETTER U DIAERESIS HACEK;;;01DA;";
    EXPECT_EQ(canonical_composed(0x00FC, 0x030C), 0x01DA) << "  Line: 01DA;LATIN SMALL LETTER U WITH DIAERESIS AND CARON;Ll;0;L;00FC 030C;;;;N;LATIN SMALL LETTER U DIAERESIS HACEK;;01D9;;01D9";
    EXPECT_EQ(canonical_composed(0x00DC, 0x0300), 0x01DB) << "  Line: 01DB;LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE;Lu;0;L;00DC 0300;;;;N;LATIN CAPITAL LETTER U DIAERESIS GRAVE;;;01DC;";
    EXPECT_EQ(canonical_composed(0x00FC, 0x0300), 0x01DC) << "  Line: 01DC;LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE;Ll;0;L;00FC 0300;;;;N;LATIN SMALL LETTER U DIAERESIS GRAVE;;01DB;;01DB";
    EXPECT_EQ(canonical_composed(0x00C4, 0x0304), 0x01DE) << "  Line: 01DE;LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON;Lu;0;L;00C4 0304;;;;N;LATIN CAPITAL LETTER A DIAERESIS MACRON;;;01DF;";
    EXPECT_EQ(canonical_composed(0x00E4, 0x0304), 0x01DF) << "  Line: 01DF;LATIN SMALL LETTER A WITH DIAERESIS AND MACRON;Ll;0;L;00E4 0304;;;;N;LATIN SMALL LETTER A DIAERESIS MACRON;;01DE;;01DE";
    EXPECT_EQ(canonical_composed(0x0226, 0x0304), 0x01E0) << "  Line: 01E0;LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON;Lu;0;L;0226 0304;;;;N;LATIN CAPITAL LETTER A DOT MACRON;;;01E1;";
    EXPECT_EQ(canonical_composed(0x0227, 0x0304), 0x01E1) << "  Line: 01E1;LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON;Ll;0;L;0227 0304;;;;N;LATIN SMALL LETTER A DOT MACRON;;01E0;;01E0";
    EXPECT_EQ(canonical_composed(0x00C6, 0x0304), 0x01E2) << "  Line: 01E2;LATIN CAPITAL LETTER AE WITH MACRON;Lu;0;L;00C6 0304;;;;N;LATIN CAPITAL LETTER A E MACRON;;;01E3;";
    EXPECT_EQ(canonical_composed(0x00E6, 0x0304), 0x01E3) << "  Line: 01E3;LATIN SMALL LETTER AE WITH MACRON;Ll;0;L;00E6 0304;;;;N;LATIN SMALL LETTER A E MACRON;;01E2;;01E2";
    EXPECT_EQ(canonical_composed(0x0047, 0x030C), 0x01E6) << "  Line: 01E6;LATIN CAPITAL LETTER G WITH CARON;Lu;0;L;0047 030C;;;;N;LATIN CAPITAL LETTER G HACEK;;;01E7;";
    EXPECT_EQ(canonical_composed(0x0067, 0x030C), 0x01E7) << "  Line: 01E7;LATIN SMALL LETTER G WITH CARON;Ll;0;L;0067 030C;;;;N;LATIN SMALL LETTER G HACEK;;01E6;;01E6";
    EXPECT_EQ(canonical_composed(0x004B, 0x030C), 0x01E8) << "  Line: 01E8;LATIN CAPITAL LETTER K WITH CARON;Lu;0;L;004B 030C;;;;N;LATIN CAPITAL LETTER K HACEK;;;01E9;";
    EXPECT_EQ(canonical_composed(0x006B, 0x030C), 0x01E9) << "  Line: 01E9;LATIN SMALL LETTER K WITH CARON;Ll;0;L;006B 030C;;;;N;LATIN SMALL LETTER K HACEK;;01E8;;01E8";
    EXPECT_EQ(canonical_composed(0x004F, 0x0328), 0x01EA) << "  Line: 01EA;LATIN CAPITAL LETTER O WITH OGONEK;Lu;0;L;004F 0328;;;;N;LATIN CAPITAL LETTER O OGONEK;;;01EB;";
    EXPECT_EQ(canonical_composed(0x006F, 0x0328), 0x01EB) << "  Line: 01EB;LATIN SMALL LETTER O WITH OGONEK;Ll;0;L;006F 0328;;;;N;LATIN SMALL LETTER O OGONEK;;01EA;;01EA";
    EXPECT_EQ(canonical_composed(0x01EA, 0x0304), 0x01EC) << "  Line: 01EC;LATIN CAPITAL LETTER O WITH OGONEK AND MACRON;Lu;0;L;01EA 0304;;;;N;LATIN CAPITAL LETTER O OGONEK MACRON;;;01ED;";
    EXPECT_EQ(canonical_composed(0x01EB, 0x0304), 0x01ED) << "  Line: 01ED;LATIN SMALL LETTER O WITH OGONEK AND MACRON;Ll;0;L;01EB 0304;;;;N;LATIN SMALL LETTER O OGONEK MACRON;;01EC;;01EC";
    EXPECT_EQ(canonical_composed(0x01B7, 0x030C), 0x01EE) << "  Line: 01EE;LATIN CAPITAL LETTER EZH WITH CARON;Lu;0;L;01B7 030C;;;;N;LATIN CAPITAL LETTER YOGH HACEK;;;01EF;";
    EXPECT_EQ(canonical_composed(0x0292, 0x030C), 0x01EF) << "  Line: 01EF;LATIN SMALL LETTER EZH WITH CARON;Ll;0;L;0292 030C;;;;N;LATIN SMALL LETTER YOGH HACEK;;01EE;;01EE";
    EXPECT_EQ(canonical_composed(0x006A, 0x030C), 0x01F0) << "  Line: 01F0;LATIN SMALL LETTER J WITH CARON;Ll;0;L;006A 030C;;;;N;LATIN SMALL LETTER J HACEK;;;;";
    EXPECT_EQ(canonical_composed(0x0047, 0x0301), 0x01F4) << "  Line: 01F4;LATIN CAPITAL LETTER G WITH ACUTE;Lu;0;L;0047 0301;;;;N;;;;01F5;";
    EXPECT_EQ(canonical_composed(0x0067, 0x0301), 0x01F5) << "  Line: 01F5;LATIN SMALL LETTER G WITH ACUTE;Ll;0;L;0067 0301;;;;N;;;01F4;;01F4";
    EXPECT_EQ(canonical_composed(0x004E, 0x0300), 0x01F8) << "  Line: 01F8;LATIN CAPITAL LETTER N WITH GRAVE;Lu;0;L;004E 0300;;;;N;;;;01F9;";
    EXPECT_EQ(canonical_composed(0x006E, 0x0300), 0x01F9) << "  Line: 01F9;LATIN SMALL LETTER N WITH GRAVE;Ll;0;L;006E 0300;;;;N;;;01F8;;01F8";
    EXPECT_EQ(canonical_composed(0x00C5, 0x0301), 0x01FA) << "  Line: 01FA;LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE;Lu;0;L;00C5 0301;;;;N;;;;01FB;";
    EXPECT_EQ(canonical_composed(0x00E5, 0x0301), 0x01FB) << "  Line: 01FB;LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE;Ll;0;L;00E5 0301;;;;N;;;01FA;;01FA";
    EXPECT_EQ(canonical_composed(0x00C6, 0x0301), 0x01FC) << "  Line: 01FC;LATIN CAPITAL LETTER AE WITH ACUTE;Lu;0;L;00C6 0301;;;;N;;;;01FD;";
    EXPECT_EQ(canonical_composed(0x00E6, 0x0301), 0x01FD) << "  Line: 01FD;LATIN SMALL LETTER AE WITH ACUTE;Ll;0;L;00E6 0301;;;;N;;;01FC;;01FC";
    EXPECT_EQ(canonical_composed(0x00D8, 0x0301), 0x01FE) << "  Line: 01FE;LATIN CAPITAL LETTER O WITH STROKE AND ACUTE;Lu;0;L;00D8 0301;;;;N;;;;01FF;";
    EXPECT_EQ(canonical_composed(0x00F8, 0x0301), 0x01FF) << "  Line: 01FF;LATIN SMALL LETTER O WITH STROKE AND ACUTE;Ll;0;L;00F8 0301;;;;N;;;01FE;;01FE";
    EXPECT_EQ(canonical_composed(0x0041, 0x030F), 0x0200) << "  Line: 0200;LATIN CAPITAL LETTER A WITH DOUBLE GRAVE;Lu;0;L;0041 030F;;;;N;;;;0201;";
    EXPECT_EQ(canonical_composed(0x0061, 0x030F), 0x0201) << "  Line: 0201;LATIN SMALL LETTER A WITH DOUBLE GRAVE;Ll;0;L;0061 030F;;;;N;;;0200;;0200";
    EXPECT_EQ(canonical_composed(0x0041, 0x0311), 0x0202) << "  Line: 0202;LATIN CAPITAL LETTER A WITH INVERTED BREVE;Lu;0;L;0041 0311;;;;N;;;;0203;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0311), 0x0203) << "  Line: 0203;LATIN SMALL LETTER A WITH INVERTED BREVE;Ll;0;L;0061 0311;;;;N;;;0202;;0202";
    EXPECT_EQ(canonical_composed(0x0045, 0x030F), 0x0204) << "  Line: 0204;LATIN CAPITAL LETTER E WITH DOUBLE GRAVE;Lu;0;L;0045 030F;;;;N;;;;0205;";
    EXPECT_EQ(canonical_composed(0x0065, 0x030F), 0x0205) << "  Line: 0205;LATIN SMALL LETTER E WITH DOUBLE GRAVE;Ll;0;L;0065 030F;;;;N;;;0204;;0204";
    EXPECT_EQ(canonical_composed(0x0045, 0x0311), 0x0206) << "  Line: 0206;LATIN CAPITAL LETTER E WITH INVERTED BREVE;Lu;0;L;0045 0311;;;;N;;;;0207;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0311), 0x0207) << "  Line: 0207;LATIN SMALL LETTER E WITH INVERTED BREVE;Ll;0;L;0065 0311;;;;N;;;0206;;0206";
    EXPECT_EQ(canonical_composed(0x0049, 0x030F), 0x0208) << "  Line: 0208;LATIN CAPITAL LETTER I WITH DOUBLE GRAVE;Lu;0;L;0049 030F;;;;N;;;;0209;";
    EXPECT_EQ(canonical_composed(0x0069, 0x030F), 0x0209) << "  Line: 0209;LATIN SMALL LETTER I WITH DOUBLE GRAVE;Ll;0;L;0069 030F;;;;N;;;0208;;0208";
    EXPECT_EQ(canonical_composed(0x0049, 0x0311), 0x020A) << "  Line: 020A;LATIN CAPITAL LETTER I WITH INVERTED BREVE;Lu;0;L;0049 0311;;;;N;;;;020B;";
    EXPECT_EQ(canonical_composed(0x0069, 0x0311), 0x020B) << "  Line: 020B;LATIN SMALL LETTER I WITH INVERTED BREVE;Ll;0;L;0069 0311;;;;N;;;020A;;020A";
    EXPECT_EQ(canonical_composed(0x004F, 0x030F), 0x020C) << "  Line: 020C;LATIN CAPITAL LETTER O WITH DOUBLE GRAVE;Lu;0;L;004F 030F;;;;N;;;;020D;";
    EXPECT_EQ(canonical_composed(0x006F, 0x030F), 0x020D) << "  Line: 020D;LATIN SMALL LETTER O WITH DOUBLE GRAVE;Ll;0;L;006F 030F;;;;N;;;020C;;020C";
    EXPECT_EQ(canonical_composed(0x004F, 0x0311), 0x020E) << "  Line: 020E;LATIN CAPITAL LETTER O WITH INVERTED BREVE;Lu;0;L;004F 0311;;;;N;;;;020F;";
    EXPECT_EQ(canonical_composed(0x006F, 0x0311), 0x020F) << "  Line: 020F;LATIN SMALL LETTER O WITH INVERTED BREVE;Ll;0;L;006F 0311;;;;N;;;020E;;020E";
    EXPECT_EQ(canonical_composed(0x0052, 0x030F), 0x0210) << "  Line: 0210;LATIN CAPITAL LETTER R WITH DOUBLE GRAVE;Lu;0;L;0052 030F;;;;N;;;;0211;";
    EXPECT_EQ(canonical_composed(0x0072, 0x030F), 0x0211) << "  Line: 0211;LATIN SMALL LETTER R WITH DOUBLE GRAVE;Ll;0;L;0072 030F;;;;N;;;0210;;0210";
    EXPECT_EQ(canonical_composed(0x0052, 0x0311), 0x0212) << "  Line: 0212;LATIN CAPITAL LETTER R WITH INVERTED BREVE;Lu;0;L;0052 0311;;;;N;;;;0213;";
    EXPECT_EQ(canonical_composed(0x0072, 0x0311), 0x0213) << "  Line: 0213;LATIN SMALL LETTER R WITH INVERTED BREVE;Ll;0;L;0072 0311;;;;N;;;0212;;0212";
    EXPECT_EQ(canonical_composed(0x0055, 0x030F), 0x0214) << "  Line: 0214;LATIN CAPITAL LETTER U WITH DOUBLE GRAVE;Lu;0;L;0055 030F;;;;N;;;;0215;";
    EXPECT_EQ(canonical_composed(0x0075, 0x030F), 0x0215) << "  Line: 0215;LATIN SMALL LETTER U WITH DOUBLE GRAVE;Ll;0;L;0075 030F;;;;N;;;0214;;0214";
    EXPECT_EQ(canonical_composed(0x0055, 0x0311), 0x0216) << "  Line: 0216;LATIN CAPITAL LETTER U WITH INVERTED BREVE;Lu;0;L;0055 0311;;;;N;;;;0217;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0311), 0x0217) << "  Line: 0217;LATIN SMALL LETTER U WITH INVERTED BREVE;Ll;0;L;0075 0311;;;;N;;;0216;;0216";
    EXPECT_EQ(canonical_composed(0x0053, 0x0326), 0x0218) << "  Line: 0218;LATIN CAPITAL LETTER S WITH COMMA BELOW;Lu;0;L;0053 0326;;;;N;;;;0219;";
    EXPECT_EQ(canonical_composed(0x0073, 0x0326), 0x0219) << "  Line: 0219;LATIN SMALL LETTER S WITH COMMA BELOW;Ll;0;L;0073 0326;;;;N;;;0218;;0218";
    EXPECT_EQ(canonical_composed(0x0054, 0x0326), 0x021A) << "  Line: 021A;LATIN CAPITAL LETTER T WITH COMMA BELOW;Lu;0;L;0054 0326;;;;N;;;;021B;";
    EXPECT_EQ(canonical_composed(0x0074, 0x0326), 0x021B) << "  Line: 021B;LATIN SMALL LETTER T WITH COMMA BELOW;Ll;0;L;0074 0326;;;;N;;;021A;;021A";
    EXPECT_EQ(canonical_composed(0x0048, 0x030C), 0x021E) << "  Line: 021E;LATIN CAPITAL LETTER H WITH CARON;Lu;0;L;0048 030C;;;;N;;;;021F;";
    EXPECT_EQ(canonical_composed(0x0068, 0x030C), 0x021F) << "  Line: 021F;LATIN SMALL LETTER H WITH CARON;Ll;0;L;0068 030C;;;;N;;;021E;;021E";
    EXPECT_EQ(canonical_composed(0x0041, 0x0307), 0x0226) << "  Line: 0226;LATIN CAPITAL LETTER A WITH DOT ABOVE;Lu;0;L;0041 0307;;;;N;;;;0227;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0307), 0x0227) << "  Line: 0227;LATIN SMALL LETTER A WITH DOT ABOVE;Ll;0;L;0061 0307;;;;N;;;0226;;0226";
    EXPECT_EQ(canonical_composed(0x0045, 0x0327), 0x0228) << "  Line: 0228;LATIN CAPITAL LETTER E WITH CEDILLA;Lu;0;L;0045 0327;;;;N;;;;0229;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0327), 0x0229) << "  Line: 0229;LATIN SMALL LETTER E WITH CEDILLA;Ll;0;L;0065 0327;;;;N;;;0228;;0228";
    EXPECT_EQ(canonical_composed(0x00D6, 0x0304), 0x022A) << "  Line: 022A;LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON;Lu;0;L;00D6 0304;;;;N;;;;022B;";
    EXPECT_EQ(canonical_composed(0x00F6, 0x0304), 0x022B) << "  Line: 022B;LATIN SMALL LETTER O WITH DIAERESIS AND MACRON;Ll;0;L;00F6 0304;;;;N;;;022A;;022A";
    EXPECT_EQ(canonical_composed(0x00D5, 0x0304), 0x022C) << "  Line: 022C;LATIN CAPITAL LETTER O WITH TILDE AND MACRON;Lu;0;L;00D5 0304;;;;N;;;;022D;";
    EXPECT_EQ(canonical_composed(0x00F5, 0x0304), 0x022D) << "  Line: 022D;LATIN SMALL LETTER O WITH TILDE AND MACRON;Ll;0;L;00F5 0304;;;;N;;;022C;;022C";
    EXPECT_EQ(canonical_composed(0x004F, 0x0307), 0x022E) << "  Line: 022E;LATIN CAPITAL LETTER O WITH DOT ABOVE;Lu;0;L;004F 0307;;;;N;;;;022F;";
    EXPECT_EQ(canonical_composed(0x006F, 0x0307), 0x022F) << "  Line: 022F;LATIN SMALL LETTER O WITH DOT ABOVE;Ll;0;L;006F 0307;;;;N;;;022E;;022E";
    EXPECT_EQ(canonical_composed(0x022E, 0x0304), 0x0230) << "  Line: 0230;LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON;Lu;0;L;022E 0304;;;;N;;;;0231;";
    EXPECT_EQ(canonical_composed(0x022F, 0x0304), 0x0231) << "  Line: 0231;LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON;Ll;0;L;022F 0304;;;;N;;;0230;;0230";
    EXPECT_EQ(canonical_composed(0x0059, 0x0304), 0x0232) << "  Line: 0232;LATIN CAPITAL LETTER Y WITH MACRON;Lu;0;L;0059 0304;;;;N;;;;0233;";
    EXPECT_EQ(canonical_composed(0x0079, 0x0304), 0x0233) << "  Line: 0233;LATIN SMALL LETTER Y WITH MACRON;Ll;0;L;0079 0304;;;;N;;;0232;;0232";
    EXPECT_EQ(canonical_composed(0x00A8, 0x0301), 0x0385) << "  Line: 0385;GREEK DIALYTIKA TONOS;Sk;0;ON;00A8 0301;;;;N;GREEK SPACING DIAERESIS TONOS;;;;";
    EXPECT_EQ(canonical_composed(0x0391, 0x0301), 0x0386) << "  Line: 0386;GREEK CAPITAL LETTER ALPHA WITH TONOS;Lu;0;L;0391 0301;;;;N;GREEK CAPITAL LETTER ALPHA TONOS;;;03AC;";
    EXPECT_EQ(canonical_composed(0x0395, 0x0301), 0x0388) << "  Line: 0388;GREEK CAPITAL LETTER EPSILON WITH TONOS;Lu;0;L;0395 0301;;;;N;GREEK CAPITAL LETTER EPSILON TONOS;;;03AD;";
    EXPECT_EQ(canonical_composed(0x0397, 0x0301), 0x0389) << "  Line: 0389;GREEK CAPITAL LETTER ETA WITH TONOS;Lu;0;L;0397 0301;;;;N;GREEK CAPITAL LETTER ETA TONOS;;;03AE;";
    EXPECT_EQ(canonical_composed(0x0399, 0x0301), 0x038A) << "  Line: 038A;GREEK CAPITAL LETTER IOTA WITH TONOS;Lu;0;L;0399 0301;;;;N;GREEK CAPITAL LETTER IOTA TONOS;;;03AF;";
    EXPECT_EQ(canonical_composed(0x039F, 0x0301), 0x038C) << "  Line: 038C;GREEK CAPITAL LETTER OMICRON WITH TONOS;Lu;0;L;039F 0301;;;;N;GREEK CAPITAL LETTER OMICRON TONOS;;;03CC;";
    EXPECT_EQ(canonical_composed(0x03A5, 0x0301), 0x038E) << "  Line: 038E;GREEK CAPITAL LETTER UPSILON WITH TONOS;Lu;0;L;03A5 0301;;;;N;GREEK CAPITAL LETTER UPSILON TONOS;;;03CD;";
    EXPECT_EQ(canonical_composed(0x03A9, 0x0301), 0x038F) << "  Line: 038F;GREEK CAPITAL LETTER OMEGA WITH TONOS;Lu;0;L;03A9 0301;;;;N;GREEK CAPITAL LETTER OMEGA TONOS;;;03CE;";
    EXPECT_EQ(canonical_composed(0x03CA, 0x0301), 0x0390) << "  Line: 0390;GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS;Ll;0;L;03CA 0301;;;;N;GREEK SMALL LETTER IOTA DIAERESIS TONOS;;;;";
    EXPECT_EQ(canonical_composed(0x0399, 0x0308), 0x03AA) << "  Line: 03AA;GREEK CAPITAL LETTER IOTA WITH DIALYTIKA;Lu;0;L;0399 0308;;;;N;GREEK CAPITAL LETTER IOTA DIAERESIS;;;03CA;";
    EXPECT_EQ(canonical_composed(0x03A5, 0x0308), 0x03AB) << "  Line: 03AB;GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA;Lu;0;L;03A5 0308;;;;N;GREEK CAPITAL LETTER UPSILON DIAERESIS;;;03CB;";
    EXPECT_EQ(canonical_composed(0x03B1, 0x0301), 0x03AC) << "  Line: 03AC;GREEK SMALL LETTER ALPHA WITH TONOS;Ll;0;L;03B1 0301;;;;N;GREEK SMALL LETTER ALPHA TONOS;;0386;;0386";
    EXPECT_EQ(canonical_composed(0x03B5, 0x0301), 0x03AD) << "  Line: 03AD;GREEK SMALL LETTER EPSILON WITH TONOS;Ll;0;L;03B5 0301;;;;N;GREEK SMALL LETTER EPSILON TONOS;;0388;;0388";
    EXPECT_EQ(canonical_composed(0x03B7, 0x0301), 0x03AE) << "  Line: 03AE;GREEK SMALL LETTER ETA WITH TONOS;Ll;0;L;03B7 0301;;;;N;GREEK SMALL LETTER ETA TONOS;;0389;;0389";
    EXPECT_EQ(canonical_composed(0x03B9, 0x0301), 0x03AF) << "  Line: 03AF;GREEK SMALL LETTER IOTA WITH TONOS;Ll;0;L;03B9 0301;;;;N;GREEK SMALL LETTER IOTA TONOS;;038A;;038A";
    EXPECT_EQ(canonical_composed(0x03CB, 0x0301), 0x03B0) << "  Line: 03B0;GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS;Ll;0;L;03CB 0301;;;;N;GREEK SMALL LETTER UPSILON DIAERESIS TONOS;;;;";
    EXPECT_EQ(canonical_composed(0x03B9, 0x0308), 0x03CA) << "  Line: 03CA;GREEK SMALL LETTER IOTA WITH DIALYTIKA;Ll;0;L;03B9 0308;;;;N;GREEK SMALL LETTER IOTA DIAERESIS;;03AA;;03AA";
    EXPECT_EQ(canonical_composed(0x03C5, 0x0308), 0x03CB) << "  Line: 03CB;GREEK SMALL LETTER UPSILON WITH DIALYTIKA;Ll;0;L;03C5 0308;;;;N;GREEK SMALL LETTER UPSILON DIAERESIS;;03AB;;03AB";
    EXPECT_EQ(canonical_composed(0x03BF, 0x0301), 0x03CC) << "  Line: 03CC;GREEK SMALL LETTER OMICRON WITH TONOS;Ll;0;L;03BF 0301;;;;N;GREEK SMALL LETTER OMICRON TONOS;;038C;;038C";
    EXPECT_EQ(canonical_composed(0x03C5, 0x0301), 0x03CD) << "  Line: 03CD;GREEK SMALL LETTER UPSILON WITH TONOS;Ll;0;L;03C5 0301;;;;N;GREEK SMALL LETTER UPSILON TONOS;;038E;;038E";
    EXPECT_EQ(canonical_composed(0x03C9, 0x0301), 0x03CE) << "  Line: 03CE;GREEK SMALL LETTER OMEGA WITH TONOS;Ll;0;L;03C9 0301;;;;N;GREEK SMALL LETTER OMEGA TONOS;;038F;;038F";
    EXPECT_EQ(canonical_composed(0x03D2, 0x0301), 0x03D3) << "  Line: 03D3;GREEK UPSILON WITH ACUTE AND HOOK SYMBOL;Lu;0;L;03D2 0301;;;;N;GREEK CAPITAL LETTER UPSILON HOOK TONOS;;;;";
    EXPECT_EQ(canonical_composed(0x03D2, 0x0308), 0x03D4) << "  Line: 03D4;GREEK UPSILON WITH DIAERESIS AND HOOK SYMBOL;Lu;0;L;03D2 0308;;;;N;GREEK CAPITAL LETTER UPSILON HOOK DIAERESIS;;;;";
    EXPECT_EQ(canonical_composed(0x0415, 0x0300), 0x0400) << "  Line: 0400;CYRILLIC CAPITAL LETTER IE WITH GRAVE;Lu;0;L;0415 0300;;;;N;;;;0450;";
    EXPECT_EQ(canonical_composed(0x0415, 0x0308), 0x0401) << "  Line: 0401;CYRILLIC CAPITAL LETTER IO;Lu;0;L;0415 0308;;;;N;;;;0451;";
    EXPECT_EQ(canonical_composed(0x0413, 0x0301), 0x0403) << "  Line: 0403;CYRILLIC CAPITAL LETTER GJE;Lu;0;L;0413 0301;;;;N;;;;0453;";
    EXPECT_EQ(canonical_composed(0x0406, 0x0308), 0x0407) << "  Line: 0407;CYRILLIC CAPITAL LETTER YI;Lu;0;L;0406 0308;;;;N;;;;0457;";
    EXPECT_EQ(canonical_composed(0x041A, 0x0301), 0x040C) << "  Line: 040C;CYRILLIC CAPITAL LETTER KJE;Lu;0;L;041A 0301;;;;N;;;;045C;";
    EXPECT_EQ(canonical_composed(0x0418, 0x0300), 0x040D) << "  Line: 040D;CYRILLIC CAPITAL LETTER I WITH GRAVE;Lu;0;L;0418 0300;;;;N;;;;045D;";
    EXPECT_EQ(canonical_composed(0x0423, 0x0306), 0x040E) << "  Line: 040E;CYRILLIC CAPITAL LETTER SHORT U;Lu;0;L;0423 0306;;;;N;;;;045E;";
    EXPECT_EQ(canonical_composed(0x0418, 0x0306), 0x0419) << "  Line: 0419;CYRILLIC CAPITAL LETTER SHORT I;Lu;0;L;0418 0306;;;;N;CYRILLIC CAPITAL LETTER SHORT II;;;0439;";
    EXPECT_EQ(canonical_composed(0x0438, 0x0306), 0x0439) << "  Line: 0439;CYRILLIC SMALL LETTER SHORT I;Ll;0;L;0438 0306;;;;N;CYRILLIC SMALL LETTER SHORT II;;0419;;0419";
    EXPECT_EQ(canonical_composed(0x0435, 0x0300), 0x0450) << "  Line: 0450;CYRILLIC SMALL LETTER IE WITH GRAVE;Ll;0;L;0435 0300;;;;N;;;0400;;0400";
    EXPECT_EQ(canonical_composed(0x0435, 0x0308), 0x0451) << "  Line: 0451;CYRILLIC SMALL LETTER IO;Ll;0;L;0435 0308;;;;N;;;0401;;0401";
    EXPECT_EQ(canonical_composed(0x0433, 0x0301), 0x0453) << "  Line: 0453;CYRILLIC SMALL LETTER GJE;Ll;0;L;0433 0301;;;;N;;;0403;;0403";
    EXPECT_EQ(canonical_composed(0x0456, 0x0308), 0x0457) << "  Line: 0457;CYRILLIC SMALL LETTER YI;Ll;0;L;0456 0308;;;;N;;;0407;;0407";
    EXPECT_EQ(canonical_composed(0x043A, 0x0301), 0x045C) << "  Line: 045C;CYRILLIC SMALL LETTER KJE;Ll;0;L;043A 0301;;;;N;;;040C;;040C";
    EXPECT_EQ(canonical_composed(0x0438, 0x0300), 0x045D) << "  Line: 045D;CYRILLIC SMALL LETTER I WITH GRAVE;Ll;0;L;0438 0300;;;;N;;;040D;;040D";
    EXPECT_EQ(canonical_composed(0x0443, 0x0306), 0x045E) << "  Line: 045E;CYRILLIC SMALL LETTER SHORT U;Ll;0;L;0443 0306;;;;N;;;040E;;040E";
    EXPECT_EQ(canonical_composed(0x0474, 0x030F), 0x0476) << "  Line: 0476;CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT;Lu;0;L;0474 030F;;;;N;CYRILLIC CAPITAL LETTER IZHITSA DOUBLE GRAVE;;;0477;";
    EXPECT_EQ(canonical_composed(0x0475, 0x030F), 0x0477) << "  Line: 0477;CYRILLIC SMALL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT;Ll;0;L;0475 030F;;;;N;CYRILLIC SMALL LETTER IZHITSA DOUBLE GRAVE;;0476;;0476";
    EXPECT_EQ(canonical_composed(0x0416, 0x0306), 0x04C1) << "  Line: 04C1;CYRILLIC CAPITAL LETTER ZHE WITH BREVE;Lu;0;L;0416 0306;;;;N;CYRILLIC CAPITAL LETTER SHORT ZHE;;;04C2;";
    EXPECT_EQ(canonical_composed(0x0436, 0x0306), 0x04C2) << "  Line: 04C2;CYRILLIC SMALL LETTER ZHE WITH BREVE;Ll;0;L;0436 0306;;;;N;CYRILLIC SMALL LETTER SHORT ZHE;;04C1;;04C1";
    EXPECT_EQ(canonical_composed(0x0410, 0x0306), 0x04D0) << "  Line: 04D0;CYRILLIC CAPITAL LETTER A WITH BREVE;Lu;0;L;0410 0306;;;;N;;;;04D1;";
    EXPECT_EQ(canonical_composed(0x0430, 0x0306), 0x04D1) << "  Line: 04D1;CYRILLIC SMALL LETTER A WITH BREVE;Ll;0;L;0430 0306;;;;N;;;04D0;;04D0";
    EXPECT_EQ(canonical_composed(0x0410, 0x0308), 0x04D2) << "  Line: 04D2;CYRILLIC CAPITAL LETTER A WITH DIAERESIS;Lu;0;L;0410 0308;;;;N;;;;04D3;";
    EXPECT_EQ(canonical_composed(0x0430, 0x0308), 0x04D3) << "  Line: 04D3;CYRILLIC SMALL LETTER A WITH DIAERESIS;Ll;0;L;0430 0308;;;;N;;;04D2;;04D2";
    EXPECT_EQ(canonical_composed(0x0415, 0x0306), 0x04D6) << "  Line: 04D6;CYRILLIC CAPITAL LETTER IE WITH BREVE;Lu;0;L;0415 0306;;;;N;;;;04D7;";
    EXPECT_EQ(canonical_composed(0x0435, 0x0306), 0x04D7) << "  Line: 04D7;CYRILLIC SMALL LETTER IE WITH BREVE;Ll;0;L;0435 0306;;;;N;;;04D6;;04D6";
    EXPECT_EQ(canonical_composed(0x04D8, 0x0308), 0x04DA) << "  Line: 04DA;CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS;Lu;0;L;04D8 0308;;;;N;;;;04DB;";
    EXPECT_EQ(canonical_composed(0x04D9, 0x0308), 0x04DB) << "  Line: 04DB;CYRILLIC SMALL LETTER SCHWA WITH DIAERESIS;Ll;0;L;04D9 0308;;;;N;;;04DA;;04DA";
    EXPECT_EQ(canonical_composed(0x0416, 0x0308), 0x04DC) << "  Line: 04DC;CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS;Lu;0;L;0416 0308;;;;N;;;;04DD;";
    EXPECT_EQ(canonical_composed(0x0436, 0x0308), 0x04DD) << "  Line: 04DD;CYRILLIC SMALL LETTER ZHE WITH DIAERESIS;Ll;0;L;0436 0308;;;;N;;;04DC;;04DC";
    EXPECT_EQ(canonical_composed(0x0417, 0x0308), 0x04DE) << "  Line: 04DE;CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS;Lu;0;L;0417 0308;;;;N;;;;04DF;";
    EXPECT_EQ(canonical_composed(0x0437, 0x0308), 0x04DF) << "  Line: 04DF;CYRILLIC SMALL LETTER ZE WITH DIAERESIS;Ll;0;L;0437 0308;;;;N;;;04DE;;04DE";
    EXPECT_EQ(canonical_composed(0x0418, 0x0304), 0x04E2) << "  Line: 04E2;CYRILLIC CAPITAL LETTER I WITH MACRON;Lu;0;L;0418 0304;;;;N;;;;04E3;";
    EXPECT_EQ(canonical_composed(0x0438, 0x0304), 0x04E3) << "  Line: 04E3;CYRILLIC SMALL LETTER I WITH MACRON;Ll;0;L;0438 0304;;;;N;;;04E2;;04E2";
    EXPECT_EQ(canonical_composed(0x0418, 0x0308), 0x04E4) << "  Line: 04E4;CYRILLIC CAPITAL LETTER I WITH DIAERESIS;Lu;0;L;0418 0308;;;;N;;;;04E5;";
    EXPECT_EQ(canonical_composed(0x0438, 0x0308), 0x04E5) << "  Line: 04E5;CYRILLIC SMALL LETTER I WITH DIAERESIS;Ll;0;L;0438 0308;;;;N;;;04E4;;04E4";
    EXPECT_EQ(canonical_composed(0x041E, 0x0308), 0x04E6) << "  Line: 04E6;CYRILLIC CAPITAL LETTER O WITH DIAERESIS;Lu;0;L;041E 0308;;;;N;;;;04E7;";
    EXPECT_EQ(canonical_composed(0x043E, 0x0308), 0x04E7) << "  Line: 04E7;CYRILLIC SMALL LETTER O WITH DIAERESIS;Ll;0;L;043E 0308;;;;N;;;04E6;;04E6";
    EXPECT_EQ(canonical_composed(0x04E8, 0x0308), 0x04EA) << "  Line: 04EA;CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS;Lu;0;L;04E8 0308;;;;N;;;;04EB;";
    EXPECT_EQ(canonical_composed(0x04E9, 0x0308), 0x04EB) << "  Line: 04EB;CYRILLIC SMALL LETTER BARRED O WITH DIAERESIS;Ll;0;L;04E9 0308;;;;N;;;04EA;;04EA";
    EXPECT_EQ(canonical_composed(0x042D, 0x0308), 0x04EC) << "  Line: 04EC;CYRILLIC CAPITAL LETTER E WITH DIAERESIS;Lu;0;L;042D 0308;;;;N;;;;04ED;";
    EXPECT_EQ(canonical_composed(0x044D, 0x0308), 0x04ED) << "  Line: 04ED;CYRILLIC SMALL LETTER E WITH DIAERESIS;Ll;0;L;044D 0308;;;;N;;;04EC;;04EC";
    EXPECT_EQ(canonical_composed(0x0423, 0x0304), 0x04EE) << "  Line: 04EE;CYRILLIC CAPITAL LETTER U WITH MACRON;Lu;0;L;0423 0304;;;;N;;;;04EF;";
    EXPECT_EQ(canonical_composed(0x0443, 0x0304), 0x04EF) << "  Line: 04EF;CYRILLIC SMALL LETTER U WITH MACRON;Ll;0;L;0443 0304;;;;N;;;04EE;;04EE";
    EXPECT_EQ(canonical_composed(0x0423, 0x0308), 0x04F0) << "  Line: 04F0;CYRILLIC CAPITAL LETTER U WITH DIAERESIS;Lu;0;L;0423 0308;;;;N;;;;04F1;";
    EXPECT_EQ(canonical_composed(0x0443, 0x0308), 0x04F1) << "  Line: 04F1;CYRILLIC SMALL LETTER U WITH DIAERESIS;Ll;0;L;0443 0308;;;;N;;;04F0;;04F0";
    EXPECT_EQ(canonical_composed(0x0423, 0x030B), 0x04F2) << "  Line: 04F2;CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE;Lu;0;L;0423 030B;;;;N;;;;04F3;";
    EXPECT_EQ(canonical_composed(0x0443, 0x030B), 0x04F3) << "  Line: 04F3;CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE;Ll;0;L;0443 030B;;;;N;;;04F2;;04F2";
    EXPECT_EQ(canonical_composed(0x0427, 0x0308), 0x04F4) << "  Line: 04F4;CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS;Lu;0;L;0427 0308;;;;N;;;;04F5;";
    EXPECT_EQ(canonical_composed(0x0447, 0x0308), 0x04F5) << "  Line: 04F5;CYRILLIC SMALL LETTER CHE WITH DIAERESIS;Ll;0;L;0447 0308;;;;N;;;04F4;;04F4";
    EXPECT_EQ(canonical_composed(0x042B, 0x0308), 0x04F8) << "  Line: 04F8;CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS;Lu;0;L;042B 0308;;;;N;;;;04F9;";
    EXPECT_EQ(canonical_composed(0x044B, 0x0308), 0x04F9) << "  Line: 04F9;CYRILLIC SMALL LETTER YERU WITH DIAERESIS;Ll;0;L;044B 0308;;;;N;;;04F8;;04F8";
    EXPECT_EQ(canonical_composed(0x0627, 0x0653), 0x0622) << "  Line: 0622;ARABIC LETTER ALEF WITH MADDA ABOVE;Lo;0;AL;0627 0653;;;;N;ARABIC LETTER MADDAH ON ALEF;;;;";
    EXPECT_EQ(canonical_composed(0x0627, 0x0654), 0x0623) << "  Line: 0623;ARABIC LETTER ALEF WITH HAMZA ABOVE;Lo;0;AL;0627 0654;;;;N;ARABIC LETTER HAMZAH ON ALEF;;;;";
    EXPECT_EQ(canonical_composed(0x0648, 0x0654), 0x0624) << "  Line: 0624;ARABIC LETTER WAW WITH HAMZA ABOVE;Lo;0;AL;0648 0654;;;;N;ARABIC LETTER HAMZAH ON WAW;;;;";
    EXPECT_EQ(canonical_composed(0x0627, 0x0655), 0x0625) << "  Line: 0625;ARABIC LETTER ALEF WITH HAMZA BELOW;Lo;0;AL;0627 0655;;;;N;ARABIC LETTER HAMZAH UNDER ALEF;;;;";
    EXPECT_EQ(canonical_composed(0x064A, 0x0654), 0x0626) << "  Line: 0626;ARABIC LETTER YEH WITH HAMZA ABOVE;Lo;0;AL;064A 0654;;;;N;ARABIC LETTER HAMZAH ON YA;;;;";
    EXPECT_EQ(canonical_composed(0x06D5, 0x0654), 0x06C0) << "  Line: 06C0;ARABIC LETTER HEH WITH YEH ABOVE;Lo;0;AL;06D5 0654;;;;N;ARABIC LETTER HAMZAH ON HA;;;;";
    EXPECT_EQ(canonical_composed(0x06C1, 0x0654), 0x06C2) << "  Line: 06C2;ARABIC LETTER HEH GOAL WITH HAMZA ABOVE;Lo;0;AL;06C1 0654;;;;N;ARABIC LETTER HAMZAH ON HA GOAL;;;;";
    EXPECT_EQ(canonical_composed(0x06D2, 0x0654), 0x06D3) << "  Line: 06D3;ARABIC LETTER YEH BARREE WITH HAMZA ABOVE;Lo;0;AL;06D2 0654;;;;N;ARABIC LETTER HAMZAH ON YA BARREE;;;;";
    EXPECT_EQ(canonical_composed(0x0928, 0x093C), 0x0929) << "  Line: 0929;DEVANAGARI LETTER NNNA;Lo;0;L;0928 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0930, 0x093C), 0x0931) << "  Line: 0931;DEVANAGARI LETTER RRA;Lo;0;L;0930 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0933, 0x093C), 0x0934) << "  Line: 0934;DEVANAGARI LETTER LLLA;Lo;0;L;0933 093C;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x09C7, 0x09BE), 0x09CB) << "  Line: 09CB;BENGALI VOWEL SIGN O;Mc;0;L;09C7 09BE;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x09C7, 0x09D7), 0x09CC) << "  Line: 09CC;BENGALI VOWEL SIGN AU;Mc;0;L;09C7 09D7;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0B47, 0x0B56), 0x0B48) << "  Line: 0B48;ORIYA VOWEL SIGN AI;Mc;0;L;0B47 0B56;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0B47, 0x0B3E), 0x0B4B) << "  Line: 0B4B;ORIYA VOWEL SIGN O;Mc;0;L;0B47 0B3E;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0B47, 0x0B57), 0x0B4C) << "  Line: 0B4C;ORIYA VOWEL SIGN AU;Mc;0;L;0B47 0B57;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0B92, 0x0BD7), 0x0B94) << "  Line: 0B94;TAMIL LETTER AU;Lo;0;L;0B92 0BD7;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0BC6, 0x0BBE), 0x0BCA) << "  Line: 0BCA;TAMIL VOWEL SIGN O;Mc;0;L;0BC6 0BBE;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0BC7, 0x0BBE), 0x0BCB) << "  Line: 0BCB;TAMIL VOWEL SIGN OO;Mc;0;L;0BC7 0BBE;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0BC6, 0x0BD7), 0x0BCC) << "  Line: 0BCC;TAMIL VOWEL SIGN AU;Mc;0;L;0BC6 0BD7;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0C46, 0x0C56), 0x0C48) << "  Line: 0C48;TELUGU VOWEL SIGN AI;Mn;0;NSM;0C46 0C56;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0CBF, 0x0CD5), 0x0CC0) << "  Line: 0CC0;KANNADA VOWEL SIGN II;Mc;0;L;0CBF 0CD5;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0CC6, 0x0CD5), 0x0CC7) << "  Line: 0CC7;KANNADA VOWEL SIGN EE;Mc;0;L;0CC6 0CD5;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0CC6, 0x0CD6), 0x0CC8) << "  Line: 0CC8;KANNADA VOWEL SIGN AI;Mc;0;L;0CC6 0CD6;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0CC6, 0x0CC2), 0x0CCA) << "  Line: 0CCA;KANNADA VOWEL SIGN O;Mc;0;L;0CC6 0CC2;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0CCA, 0x0CD5), 0x0CCB) << "  Line: 0CCB;KANNADA VOWEL SIGN OO;Mc;0;L;0CCA 0CD5;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0D46, 0x0D3E), 0x0D4A) << "  Line: 0D4A;MALAYALAM VOWEL SIGN O;Mc;0;L;0D46 0D3E;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0D47, 0x0D3E), 0x0D4B) << "  Line: 0D4B;MALAYALAM VOWEL SIGN OO;Mc;0;L;0D47 0D3E;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0D46, 0x0D57), 0x0D4C) << "  Line: 0D4C;MALAYALAM VOWEL SIGN AU;Mc;0;L;0D46 0D57;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0DD9, 0x0DCA), 0x0DDA) << "  Line: 0DDA;SINHALA VOWEL SIGN DIGA KOMBUVA;Mc;0;L;0DD9 0DCA;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0DD9, 0x0DCF), 0x0DDC) << "  Line: 0DDC;SINHALA VOWEL SIGN KOMBUVA HAA AELA-PILLA;Mc;0;L;0DD9 0DCF;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0DDC, 0x0DCA), 0x0DDD) << "  Line: 0DDD;SINHALA VOWEL SIGN KOMBUVA HAA DIGA AELA-PILLA;Mc;0;L;0DDC 0DCA;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0DD9, 0x0DDF), 0x0DDE) << "  Line: 0DDE;SINHALA VOWEL SIGN KOMBUVA HAA GAYANUKITTA;Mc;0;L;0DD9 0DDF;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1025, 0x102E), 0x1026) << "  Line: 1026;MYANMAR LETTER UU;Lo;0;L;1025 102E;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B05, 0x1B35), 0x1B06) << "  Line: 1B06;BALINESE LETTER AKARA TEDUNG;Lo;0;L;1B05 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B07, 0x1B35), 0x1B08) << "  Line: 1B08;BALINESE LETTER IKARA TEDUNG;Lo;0;L;1B07 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B09, 0x1B35), 0x1B0A) << "  Line: 1B0A;BALINESE LETTER UKARA TEDUNG;Lo;0;L;1B09 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B0B, 0x1B35), 0x1B0C) << "  Line: 1B0C;BALINESE LETTER RA REPA TEDUNG;Lo;0;L;1B0B 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B0D, 0x1B35), 0x1B0E) << "  Line: 1B0E;BALINESE LETTER LA LENGA TEDUNG;Lo;0;L;1B0D 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B11, 0x1B35), 0x1B12) << "  Line: 1B12;BALINESE LETTER OKARA TEDUNG;Lo;0;L;1B11 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B3A, 0x1B35), 0x1B3B) << "  Line: 1B3B;BALINESE VOWEL SIGN RA REPA TEDUNG;Mc;0;L;1B3A 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B3C, 0x1B35), 0x1B3D) << "  Line: 1B3D;BALINESE VOWEL SIGN LA LENGA TEDUNG;Mc;0;L;1B3C 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B3E, 0x1B35), 0x1B40) << "  Line: 1B40;BALINESE VOWEL SIGN TALING TEDUNG;Mc;0;L;1B3E 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B3F, 0x1B35), 0x1B41) << "  Line: 1B41;BALINESE VOWEL SIGN TALING REPA TEDUNG;Mc;0;L;1B3F 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1B42, 0x1B35), 0x1B43) << "  Line: 1B43;BALINESE VOWEL SIGN PEPET TEDUNG;Mc;0;L;1B42 1B35;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0041, 0x0325), 0x1E00) << "  Line: 1E00;LATIN CAPITAL LETTER A WITH RING BELOW;Lu;0;L;0041 0325;;;;N;;;;1E01;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0325), 0x1E01) << "  Line: 1E01;LATIN SMALL LETTER A WITH RING BELOW;Ll;0;L;0061 0325;;;;N;;;1E00;;1E00";
    EXPECT_EQ(canonical_composed(0x0042, 0x0307), 0x1E02) << "  Line: 1E02;LATIN CAPITAL LETTER B WITH DOT ABOVE;Lu;0;L;0042 0307;;;;N;;;;1E03;";
    EXPECT_EQ(canonical_composed(0x0062, 0x0307), 0x1E03) << "  Line: 1E03;LATIN SMALL LETTER B WITH DOT ABOVE;Ll;0;L;0062 0307;;;;N;;;1E02;;1E02";
    EXPECT_EQ(canonical_composed(0x0042, 0x0323), 0x1E04) << "  Line: 1E04;LATIN CAPITAL LETTER B WITH DOT BELOW;Lu;0;L;0042 0323;;;;N;;;;1E05;";
    EXPECT_EQ(canonical_composed(0x0062, 0x0323), 0x1E05) << "  Line: 1E05;LATIN SMALL LETTER B WITH DOT BELOW;Ll;0;L;0062 0323;;;;N;;;1E04;;1E04";
    EXPECT_EQ(canonical_composed(0x0042, 0x0331), 0x1E06) << "  Line: 1E06;LATIN CAPITAL LETTER B WITH LINE BELOW;Lu;0;L;0042 0331;;;;N;;;;1E07;";
    EXPECT_EQ(canonical_composed(0x0062, 0x0331), 0x1E07) << "  Line: 1E07;LATIN SMALL LETTER B WITH LINE BELOW;Ll;0;L;0062 0331;;;;N;;;1E06;;1E06";
    EXPECT_EQ(canonical_composed(0x00C7, 0x0301), 0x1E08) << "  Line: 1E08;LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE;Lu;0;L;00C7 0301;;;;N;;;;1E09;";
    EXPECT_EQ(canonical_composed(0x00E7, 0x0301), 0x1E09) << "  Line: 1E09;LATIN SMALL LETTER C WITH CEDILLA AND ACUTE;Ll;0;L;00E7 0301;;;;N;;;1E08;;1E08";
    EXPECT_EQ(canonical_composed(0x0044, 0x0307), 0x1E0A) << "  Line: 1E0A;LATIN CAPITAL LETTER D WITH DOT ABOVE;Lu;0;L;0044 0307;;;;N;;;;1E0B;";
    EXPECT_EQ(canonical_composed(0x0064, 0x0307), 0x1E0B) << "  Line: 1E0B;LATIN SMALL LETTER D WITH DOT ABOVE;Ll;0;L;0064 0307;;;;N;;;1E0A;;1E0A";
    EXPECT_EQ(canonical_composed(0x0044, 0x0323), 0x1E0C) << "  Line: 1E0C;LATIN CAPITAL LETTER D WITH DOT BELOW;Lu;0;L;0044 0323;;;;N;;;;1E0D;";
    EXPECT_EQ(canonical_composed(0x0064, 0x0323), 0x1E0D) << "  Line: 1E0D;LATIN SMALL LETTER D WITH DOT BELOW;Ll;0;L;0064 0323;;;;N;;;1E0C;;1E0C";
    EXPECT_EQ(canonical_composed(0x0044, 0x0331), 0x1E0E) << "  Line: 1E0E;LATIN CAPITAL LETTER D WITH LINE BELOW;Lu;0;L;0044 0331;;;;N;;;;1E0F;";
    EXPECT_EQ(canonical_composed(0x0064, 0x0331), 0x1E0F) << "  Line: 1E0F;LATIN SMALL LETTER D WITH LINE BELOW;Ll;0;L;0064 0331;;;;N;;;1E0E;;1E0E";
    EXPECT_EQ(canonical_composed(0x0044, 0x0327), 0x1E10) << "  Line: 1E10;LATIN CAPITAL LETTER D WITH CEDILLA;Lu;0;L;0044 0327;;;;N;;;;1E11;";
    EXPECT_EQ(canonical_composed(0x0064, 0x0327), 0x1E11) << "  Line: 1E11;LATIN SMALL LETTER D WITH CEDILLA;Ll;0;L;0064 0327;;;;N;;;1E10;;1E10";
    EXPECT_EQ(canonical_composed(0x0044, 0x032D), 0x1E12) << "  Line: 1E12;LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW;Lu;0;L;0044 032D;;;;N;;;;1E13;";
    EXPECT_EQ(canonical_composed(0x0064, 0x032D), 0x1E13) << "  Line: 1E13;LATIN SMALL LETTER D WITH CIRCUMFLEX BELOW;Ll;0;L;0064 032D;;;;N;;;1E12;;1E12";
    EXPECT_EQ(canonical_composed(0x0112, 0x0300), 0x1E14) << "  Line: 1E14;LATIN CAPITAL LETTER E WITH MACRON AND GRAVE;Lu;0;L;0112 0300;;;;N;;;;1E15;";
    EXPECT_EQ(canonical_composed(0x0113, 0x0300), 0x1E15) << "  Line: 1E15;LATIN SMALL LETTER E WITH MACRON AND GRAVE;Ll;0;L;0113 0300;;;;N;;;1E14;;1E14";
    EXPECT_EQ(canonical_composed(0x0112, 0x0301), 0x1E16) << "  Line: 1E16;LATIN CAPITAL LETTER E WITH MACRON AND ACUTE;Lu;0;L;0112 0301;;;;N;;;;1E17;";
    EXPECT_EQ(canonical_composed(0x0113, 0x0301), 0x1E17) << "  Line: 1E17;LATIN SMALL LETTER E WITH MACRON AND ACUTE;Ll;0;L;0113 0301;;;;N;;;1E16;;1E16";
    EXPECT_EQ(canonical_composed(0x0045, 0x032D), 0x1E18) << "  Line: 1E18;LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW;Lu;0;L;0045 032D;;;;N;;;;1E19;";
    EXPECT_EQ(canonical_composed(0x0065, 0x032D), 0x1E19) << "  Line: 1E19;LATIN SMALL LETTER E WITH CIRCUMFLEX BELOW;Ll;0;L;0065 032D;;;;N;;;1E18;;1E18";
    EXPECT_EQ(canonical_composed(0x0045, 0x0330), 0x1E1A) << "  Line: 1E1A;LATIN CAPITAL LETTER E WITH TILDE BELOW;Lu;0;L;0045 0330;;;;N;;;;1E1B;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0330), 0x1E1B) << "  Line: 1E1B;LATIN SMALL LETTER E WITH TILDE BELOW;Ll;0;L;0065 0330;;;;N;;;1E1A;;1E1A";
    EXPECT_EQ(canonical_composed(0x0228, 0x0306), 0x1E1C) << "  Line: 1E1C;LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE;Lu;0;L;0228 0306;;;;N;;;;1E1D;";
    EXPECT_EQ(canonical_composed(0x0229, 0x0306), 0x1E1D) << "  Line: 1E1D;LATIN SMALL LETTER E WITH CEDILLA AND BREVE;Ll;0;L;0229 0306;;;;N;;;1E1C;;1E1C";
    EXPECT_EQ(canonical_composed(0x0046, 0x0307), 0x1E1E) << "  Line: 1E1E;LATIN CAPITAL LETTER F WITH DOT ABOVE;Lu;0;L;0046 0307;;;;N;;;;1E1F;";
    EXPECT_EQ(canonical_composed(0x0066, 0x0307), 0x1E1F) << "  Line: 1E1F;LATIN SMALL LETTER F WITH DOT ABOVE;Ll;0;L;0066 0307;;;;N;;;1E1E;;1E1E";
    EXPECT_EQ(canonical_composed(0x0047, 0x0304), 0x1E20) << "  Line: 1E20;LATIN CAPITAL LETTER G WITH MACRON;Lu;0;L;0047 0304;;;;N;;;;1E21;";
    EXPECT_EQ(canonical_composed(0x0067, 0x0304), 0x1E21) << "  Line: 1E21;LATIN SMALL LETTER G WITH MACRON;Ll;0;L;0067 0304;;;;N;;;1E20;;1E20";
    EXPECT_EQ(canonical_composed(0x0048, 0x0307), 0x1E22) << "  Line: 1E22;LATIN CAPITAL LETTER H WITH DOT ABOVE;Lu;0;L;0048 0307;;;;N;;;;1E23;";
    EXPECT_EQ(canonical_composed(0x0068, 0x0307), 0x1E23) << "  Line: 1E23;LATIN SMALL LETTER H WITH DOT ABOVE;Ll;0;L;0068 0307;;;;N;;;1E22;;1E22";
    EXPECT_EQ(canonical_composed(0x0048, 0x0323), 0x1E24) << "  Line: 1E24;LATIN CAPITAL LETTER H WITH DOT BELOW;Lu;0;L;0048 0323;;;;N;;;;1E25;";
    EXPECT_EQ(canonical_composed(0x0068, 0x0323), 0x1E25) << "  Line: 1E25;LATIN SMALL LETTER H WITH DOT BELOW;Ll;0;L;0068 0323;;;;N;;;1E24;;1E24";
    EXPECT_EQ(canonical_composed(0x0048, 0x0308), 0x1E26) << "  Line: 1E26;LATIN CAPITAL LETTER H WITH DIAERESIS;Lu;0;L;0048 0308;;;;N;;;;1E27;";
    EXPECT_EQ(canonical_composed(0x0068, 0x0308), 0x1E27) << "  Line: 1E27;LATIN SMALL LETTER H WITH DIAERESIS;Ll;0;L;0068 0308;;;;N;;;1E26;;1E26";
    EXPECT_EQ(canonical_composed(0x0048, 0x0327), 0x1E28) << "  Line: 1E28;LATIN CAPITAL LETTER H WITH CEDILLA;Lu;0;L;0048 0327;;;;N;;;;1E29;";
    EXPECT_EQ(canonical_composed(0x0068, 0x0327), 0x1E29) << "  Line: 1E29;LATIN SMALL LETTER H WITH CEDILLA;Ll;0;L;0068 0327;;;;N;;;1E28;;1E28";
    EXPECT_EQ(canonical_composed(0x0048, 0x032E), 0x1E2A) << "  Line: 1E2A;LATIN CAPITAL LETTER H WITH BREVE BELOW;Lu;0;L;0048 032E;;;;N;;;;1E2B;";
    EXPECT_EQ(canonical_composed(0x0068, 0x032E), 0x1E2B) << "  Line: 1E2B;LATIN SMALL LETTER H WITH BREVE BELOW;Ll;0;L;0068 032E;;;;N;;;1E2A;;1E2A";
    EXPECT_EQ(canonical_composed(0x0049, 0x0330), 0x1E2C) << "  Line: 1E2C;LATIN CAPITAL LETTER I WITH TILDE BELOW;Lu;0;L;0049 0330;;;;N;;;;1E2D;";
    EXPECT_EQ(canonical_composed(0x0069, 0x0330), 0x1E2D) << "  Line: 1E2D;LATIN SMALL LETTER I WITH TILDE BELOW;Ll;0;L;0069 0330;;;;N;;;1E2C;;1E2C";
    EXPECT_EQ(canonical_composed(0x00CF, 0x0301), 0x1E2E) << "  Line: 1E2E;LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE;Lu;0;L;00CF 0301;;;;N;;;;1E2F;";
    EXPECT_EQ(canonical_composed(0x00EF, 0x0301), 0x1E2F) << "  Line: 1E2F;LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE;Ll;0;L;00EF 0301;;;;N;;;1E2E;;1E2E";
    EXPECT_EQ(canonical_composed(0x004B, 0x0301), 0x1E30) << "  Line: 1E30;LATIN CAPITAL LETTER K WITH ACUTE;Lu;0;L;004B 0301;;;;N;;;;1E31;";
    EXPECT_EQ(canonical_composed(0x006B, 0x0301), 0x1E31) << "  Line: 1E31;LATIN SMALL LETTER K WITH ACUTE;Ll;0;L;006B 0301;;;;N;;;1E30;;1E30";
    EXPECT_EQ(canonical_composed(0x004B, 0x0323), 0x1E32) << "  Line: 1E32;LATIN CAPITAL LETTER K WITH DOT BELOW;Lu;0;L;004B 0323;;;;N;;;;1E33;";
    EXPECT_EQ(canonical_composed(0x006B, 0x0323), 0x1E33) << "  Line: 1E33;LATIN SMALL LETTER K WITH DOT BELOW;Ll;0;L;006B 0323;;;;N;;;1E32;;1E32";
    EXPECT_EQ(canonical_composed(0x004B, 0x0331), 0x1E34) << "  Line: 1E34;LATIN CAPITAL LETTER K WITH LINE BELOW;Lu;0;L;004B 0331;;;;N;;;;1E35;";
    EXPECT_EQ(canonical_composed(0x006B, 0x0331), 0x1E35) << "  Line: 1E35;LATIN SMALL LETTER K WITH LINE BELOW;Ll;0;L;006B 0331;;;;N;;;1E34;;1E34";
    EXPECT_EQ(canonical_composed(0x004C, 0x0323), 0x1E36) << "  Line: 1E36;LATIN CAPITAL LETTER L WITH DOT BELOW;Lu;0;L;004C 0323;;;;N;;;;1E37;";
    EXPECT_EQ(canonical_composed(0x006C, 0x0323), 0x1E37) << "  Line: 1E37;LATIN SMALL LETTER L WITH DOT BELOW;Ll;0;L;006C 0323;;;;N;;;1E36;;1E36";
    EXPECT_EQ(canonical_composed(0x1E36, 0x0304), 0x1E38) << "  Line: 1E38;LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON;Lu;0;L;1E36 0304;;;;N;;;;1E39;";
    EXPECT_EQ(canonical_composed(0x1E37, 0x0304), 0x1E39) << "  Line: 1E39;LATIN SMALL LETTER L WITH DOT BELOW AND MACRON;Ll;0;L;1E37 0304;;;;N;;;1E38;;1E38";
    EXPECT_EQ(canonical_composed(0x004C, 0x0331), 0x1E3A) << "  Line: 1E3A;LATIN CAPITAL LETTER L WITH LINE BELOW;Lu;0;L;004C 0331;;;;N;;;;1E3B;";
    EXPECT_EQ(canonical_composed(0x006C, 0x0331), 0x1E3B) << "  Line: 1E3B;LATIN SMALL LETTER L WITH LINE BELOW;Ll;0;L;006C 0331;;;;N;;;1E3A;;1E3A";
    EXPECT_EQ(canonical_composed(0x004C, 0x032D), 0x1E3C) << "  Line: 1E3C;LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW;Lu;0;L;004C 032D;;;;N;;;;1E3D;";
    EXPECT_EQ(canonical_composed(0x006C, 0x032D), 0x1E3D) << "  Line: 1E3D;LATIN SMALL LETTER L WITH CIRCUMFLEX BELOW;Ll;0;L;006C 032D;;;;N;;;1E3C;;1E3C";
    EXPECT_EQ(canonical_composed(0x004D, 0x0301), 0x1E3E) << "  Line: 1E3E;LATIN CAPITAL LETTER M WITH ACUTE;Lu;0;L;004D 0301;;;;N;;;;1E3F;";
    EXPECT_EQ(canonical_composed(0x006D, 0x0301), 0x1E3F) << "  Line: 1E3F;LATIN SMALL LETTER M WITH ACUTE;Ll;0;L;006D 0301;;;;N;;;1E3E;;1E3E";
    EXPECT_EQ(canonical_composed(0x004D, 0x0307), 0x1E40) << "  Line: 1E40;LATIN CAPITAL LETTER M WITH DOT ABOVE;Lu;0;L;004D 0307;;;;N;;;;1E41;";
    EXPECT_EQ(canonical_composed(0x006D, 0x0307), 0x1E41) << "  Line: 1E41;LATIN SMALL LETTER M WITH DOT ABOVE;Ll;0;L;006D 0307;;;;N;;;1E40;;1E40";
    EXPECT_EQ(canonical_composed(0x004D, 0x0323), 0x1E42) << "  Line: 1E42;LATIN CAPITAL LETTER M WITH DOT BELOW;Lu;0;L;004D 0323;;;;N;;;;1E43;";
    EXPECT_EQ(canonical_composed(0x006D, 0x0323), 0x1E43) << "  Line: 1E43;LATIN SMALL LETTER M WITH DOT BELOW;Ll;0;L;006D 0323;;;;N;;;1E42;;1E42";
    EXPECT_EQ(canonical_composed(0x004E, 0x0307), 0x1E44) << "  Line: 1E44;LATIN CAPITAL LETTER N WITH DOT ABOVE;Lu;0;L;004E 0307;;;;N;;;;1E45;";
    EXPECT_EQ(canonical_composed(0x006E, 0x0307), 0x1E45) << "  Line: 1E45;LATIN SMALL LETTER N WITH DOT ABOVE;Ll;0;L;006E 0307;;;;N;;;1E44;;1E44";
    EXPECT_EQ(canonical_composed(0x004E, 0x0323), 0x1E46) << "  Line: 1E46;LATIN CAPITAL LETTER N WITH DOT BELOW;Lu;0;L;004E 0323;;;;N;;;;1E47;";
    EXPECT_EQ(canonical_composed(0x006E, 0x0323), 0x1E47) << "  Line: 1E47;LATIN SMALL LETTER N WITH DOT BELOW;Ll;0;L;006E 0323;;;;N;;;1E46;;1E46";
    EXPECT_EQ(canonical_composed(0x004E, 0x0331), 0x1E48) << "  Line: 1E48;LATIN CAPITAL LETTER N WITH LINE BELOW;Lu;0;L;004E 0331;;;;N;;;;1E49;";
    EXPECT_EQ(canonical_composed(0x006E, 0x0331), 0x1E49) << "  Line: 1E49;LATIN SMALL LETTER N WITH LINE BELOW;Ll;0;L;006E 0331;;;;N;;;1E48;;1E48";
    EXPECT_EQ(canonical_composed(0x004E, 0x032D), 0x1E4A) << "  Line: 1E4A;LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW;Lu;0;L;004E 032D;;;;N;;;;1E4B;";
    EXPECT_EQ(canonical_composed(0x006E, 0x032D), 0x1E4B) << "  Line: 1E4B;LATIN SMALL LETTER N WITH CIRCUMFLEX BELOW;Ll;0;L;006E 032D;;;;N;;;1E4A;;1E4A";
    EXPECT_EQ(canonical_composed(0x00D5, 0x0301), 0x1E4C) << "  Line: 1E4C;LATIN CAPITAL LETTER O WITH TILDE AND ACUTE;Lu;0;L;00D5 0301;;;;N;;;;1E4D;";
    EXPECT_EQ(canonical_composed(0x00F5, 0x0301), 0x1E4D) << "  Line: 1E4D;LATIN SMALL LETTER O WITH TILDE AND ACUTE;Ll;0;L;00F5 0301;;;;N;;;1E4C;;1E4C";
    EXPECT_EQ(canonical_composed(0x00D5, 0x0308), 0x1E4E) << "  Line: 1E4E;LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS;Lu;0;L;00D5 0308;;;;N;;;;1E4F;";
    EXPECT_EQ(canonical_composed(0x00F5, 0x0308), 0x1E4F) << "  Line: 1E4F;LATIN SMALL LETTER O WITH TILDE AND DIAERESIS;Ll;0;L;00F5 0308;;;;N;;;1E4E;;1E4E";
    EXPECT_EQ(canonical_composed(0x014C, 0x0300), 0x1E50) << "  Line: 1E50;LATIN CAPITAL LETTER O WITH MACRON AND GRAVE;Lu;0;L;014C 0300;;;;N;;;;1E51;";
    EXPECT_EQ(canonical_composed(0x014D, 0x0300), 0x1E51) << "  Line: 1E51;LATIN SMALL LETTER O WITH MACRON AND GRAVE;Ll;0;L;014D 0300;;;;N;;;1E50;;1E50";
    EXPECT_EQ(canonical_composed(0x014C, 0x0301), 0x1E52) << "  Line: 1E52;LATIN CAPITAL LETTER O WITH MACRON AND ACUTE;Lu;0;L;014C 0301;;;;N;;;;1E53;";
    EXPECT_EQ(canonical_composed(0x014D, 0x0301), 0x1E53) << "  Line: 1E53;LATIN SMALL LETTER O WITH MACRON AND ACUTE;Ll;0;L;014D 0301;;;;N;;;1E52;;1E52";
    EXPECT_EQ(canonical_composed(0x0050, 0x0301), 0x1E54) << "  Line: 1E54;LATIN CAPITAL LETTER P WITH ACUTE;Lu;0;L;0050 0301;;;;N;;;;1E55;";
    EXPECT_EQ(canonical_composed(0x0070, 0x0301), 0x1E55) << "  Line: 1E55;LATIN SMALL LETTER P WITH ACUTE;Ll;0;L;0070 0301;;;;N;;;1E54;;1E54";
    EXPECT_EQ(canonical_composed(0x0050, 0x0307), 0x1E56) << "  Line: 1E56;LATIN CAPITAL LETTER P WITH DOT ABOVE;Lu;0;L;0050 0307;;;;N;;;;1E57;";
    EXPECT_EQ(canonical_composed(0x0070, 0x0307), 0x1E57) << "  Line: 1E57;LATIN SMALL LETTER P WITH DOT ABOVE;Ll;0;L;0070 0307;;;;N;;;1E56;;1E56";
    EXPECT_EQ(canonical_composed(0x0052, 0x0307), 0x1E58) << "  Line: 1E58;LATIN CAPITAL LETTER R WITH DOT ABOVE;Lu;0;L;0052 0307;;;;N;;;;1E59;";
    EXPECT_EQ(canonical_composed(0x0072, 0x0307), 0x1E59) << "  Line: 1E59;LATIN SMALL LETTER R WITH DOT ABOVE;Ll;0;L;0072 0307;;;;N;;;1E58;;1E58";
    EXPECT_EQ(canonical_composed(0x0052, 0x0323), 0x1E5A) << "  Line: 1E5A;LATIN CAPITAL LETTER R WITH DOT BELOW;Lu;0;L;0052 0323;;;;N;;;;1E5B;";
    EXPECT_EQ(canonical_composed(0x0072, 0x0323), 0x1E5B) << "  Line: 1E5B;LATIN SMALL LETTER R WITH DOT BELOW;Ll;0;L;0072 0323;;;;N;;;1E5A;;1E5A";
    EXPECT_EQ(canonical_composed(0x1E5A, 0x0304), 0x1E5C) << "  Line: 1E5C;LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON;Lu;0;L;1E5A 0304;;;;N;;;;1E5D;";
    EXPECT_EQ(canonical_composed(0x1E5B, 0x0304), 0x1E5D) << "  Line: 1E5D;LATIN SMALL LETTER R WITH DOT BELOW AND MACRON;Ll;0;L;1E5B 0304;;;;N;;;1E5C;;1E5C";
    EXPECT_EQ(canonical_composed(0x0052, 0x0331), 0x1E5E) << "  Line: 1E5E;LATIN CAPITAL LETTER R WITH LINE BELOW;Lu;0;L;0052 0331;;;;N;;;;1E5F;";
    EXPECT_EQ(canonical_composed(0x0072, 0x0331), 0x1E5F) << "  Line: 1E5F;LATIN SMALL LETTER R WITH LINE BELOW;Ll;0;L;0072 0331;;;;N;;;1E5E;;1E5E";
    EXPECT_EQ(canonical_composed(0x0053, 0x0307), 0x1E60) << "  Line: 1E60;LATIN CAPITAL LETTER S WITH DOT ABOVE;Lu;0;L;0053 0307;;;;N;;;;1E61;";
    EXPECT_EQ(canonical_composed(0x0073, 0x0307), 0x1E61) << "  Line: 1E61;LATIN SMALL LETTER S WITH DOT ABOVE;Ll;0;L;0073 0307;;;;N;;;1E60;;1E60";
    EXPECT_EQ(canonical_composed(0x0053, 0x0323), 0x1E62) << "  Line: 1E62;LATIN CAPITAL LETTER S WITH DOT BELOW;Lu;0;L;0053 0323;;;;N;;;;1E63;";
    EXPECT_EQ(canonical_composed(0x0073, 0x0323), 0x1E63) << "  Line: 1E63;LATIN SMALL LETTER S WITH DOT BELOW;Ll;0;L;0073 0323;;;;N;;;1E62;;1E62";
    EXPECT_EQ(canonical_composed(0x015A, 0x0307), 0x1E64) << "  Line: 1E64;LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE;Lu;0;L;015A 0307;;;;N;;;;1E65;";
    EXPECT_EQ(canonical_composed(0x015B, 0x0307), 0x1E65) << "  Line: 1E65;LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE;Ll;0;L;015B 0307;;;;N;;;1E64;;1E64";
    EXPECT_EQ(canonical_composed(0x0160, 0x0307), 0x1E66) << "  Line: 1E66;LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE;Lu;0;L;0160 0307;;;;N;;;;1E67;";
    EXPECT_EQ(canonical_composed(0x0161, 0x0307), 0x1E67) << "  Line: 1E67;LATIN SMALL LETTER S WITH CARON AND DOT ABOVE;Ll;0;L;0161 0307;;;;N;;;1E66;;1E66";
    EXPECT_EQ(canonical_composed(0x1E62, 0x0307), 0x1E68) << "  Line: 1E68;LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE;Lu;0;L;1E62 0307;;;;N;;;;1E69;";
    EXPECT_EQ(canonical_composed(0x1E63, 0x0307), 0x1E69) << "  Line: 1E69;LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE;Ll;0;L;1E63 0307;;;;N;;;1E68;;1E68";
    EXPECT_EQ(canonical_composed(0x0054, 0x0307), 0x1E6A) << "  Line: 1E6A;LATIN CAPITAL LETTER T WITH DOT ABOVE;Lu;0;L;0054 0307;;;;N;;;;1E6B;";
    EXPECT_EQ(canonical_composed(0x0074, 0x0307), 0x1E6B) << "  Line: 1E6B;LATIN SMALL LETTER T WITH DOT ABOVE;Ll;0;L;0074 0307;;;;N;;;1E6A;;1E6A";
    EXPECT_EQ(canonical_composed(0x0054, 0x0323), 0x1E6C) << "  Line: 1E6C;LATIN CAPITAL LETTER T WITH DOT BELOW;Lu;0;L;0054 0323;;;;N;;;;1E6D;";
    EXPECT_EQ(canonical_composed(0x0074, 0x0323), 0x1E6D) << "  Line: 1E6D;LATIN SMALL LETTER T WITH DOT BELOW;Ll;0;L;0074 0323;;;;N;;;1E6C;;1E6C";
    EXPECT_EQ(canonical_composed(0x0054, 0x0331), 0x1E6E) << "  Line: 1E6E;LATIN CAPITAL LETTER T WITH LINE BELOW;Lu;0;L;0054 0331;;;;N;;;;1E6F;";
    EXPECT_EQ(canonical_composed(0x0074, 0x0331), 0x1E6F) << "  Line: 1E6F;LATIN SMALL LETTER T WITH LINE BELOW;Ll;0;L;0074 0331;;;;N;;;1E6E;;1E6E";
    EXPECT_EQ(canonical_composed(0x0054, 0x032D), 0x1E70) << "  Line: 1E70;LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW;Lu;0;L;0054 032D;;;;N;;;;1E71;";
    EXPECT_EQ(canonical_composed(0x0074, 0x032D), 0x1E71) << "  Line: 1E71;LATIN SMALL LETTER T WITH CIRCUMFLEX BELOW;Ll;0;L;0074 032D;;;;N;;;1E70;;1E70";
    EXPECT_EQ(canonical_composed(0x0055, 0x0324), 0x1E72) << "  Line: 1E72;LATIN CAPITAL LETTER U WITH DIAERESIS BELOW;Lu;0;L;0055 0324;;;;N;;;;1E73;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0324), 0x1E73) << "  Line: 1E73;LATIN SMALL LETTER U WITH DIAERESIS BELOW;Ll;0;L;0075 0324;;;;N;;;1E72;;1E72";
    EXPECT_EQ(canonical_composed(0x0055, 0x0330), 0x1E74) << "  Line: 1E74;LATIN CAPITAL LETTER U WITH TILDE BELOW;Lu;0;L;0055 0330;;;;N;;;;1E75;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0330), 0x1E75) << "  Line: 1E75;LATIN SMALL LETTER U WITH TILDE BELOW;Ll;0;L;0075 0330;;;;N;;;1E74;;1E74";
    EXPECT_EQ(canonical_composed(0x0055, 0x032D), 0x1E76) << "  Line: 1E76;LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW;Lu;0;L;0055 032D;;;;N;;;;1E77;";
    EXPECT_EQ(canonical_composed(0x0075, 0x032D), 0x1E77) << "  Line: 1E77;LATIN SMALL LETTER U WITH CIRCUMFLEX BELOW;Ll;0;L;0075 032D;;;;N;;;1E76;;1E76";
    EXPECT_EQ(canonical_composed(0x0168, 0x0301), 0x1E78) << "  Line: 1E78;LATIN CAPITAL LETTER U WITH TILDE AND ACUTE;Lu;0;L;0168 0301;;;;N;;;;1E79;";
    EXPECT_EQ(canonical_composed(0x0169, 0x0301), 0x1E79) << "  Line: 1E79;LATIN SMALL LETTER U WITH TILDE AND ACUTE;Ll;0;L;0169 0301;;;;N;;;1E78;;1E78";
    EXPECT_EQ(canonical_composed(0x016A, 0x0308), 0x1E7A) << "  Line: 1E7A;LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS;Lu;0;L;016A 0308;;;;N;;;;1E7B;";
    EXPECT_EQ(canonical_composed(0x016B, 0x0308), 0x1E7B) << "  Line: 1E7B;LATIN SMALL LETTER U WITH MACRON AND DIAERESIS;Ll;0;L;016B 0308;;;;N;;;1E7A;;1E7A";
    EXPECT_EQ(canonical_composed(0x0056, 0x0303), 0x1E7C) << "  Line: 1E7C;LATIN CAPITAL LETTER V WITH TILDE;Lu;0;L;0056 0303;;;;N;;;;1E7D;";
    EXPECT_EQ(canonical_composed(0x0076, 0x0303), 0x1E7D) << "  Line: 1E7D;LATIN SMALL LETTER V WITH TILDE;Ll;0;L;0076 0303;;;;N;;;1E7C;;1E7C";
    EXPECT_EQ(canonical_composed(0x0056, 0x0323), 0x1E7E) << "  Line: 1E7E;LATIN CAPITAL LETTER V WITH DOT BELOW;Lu;0;L;0056 0323;;;;N;;;;1E7F;";
    EXPECT_EQ(canonical_composed(0x0076, 0x0323), 0x1E7F) << "  Line: 1E7F;LATIN SMALL LETTER V WITH DOT BELOW;Ll;0;L;0076 0323;;;;N;;;1E7E;;1E7E";
    EXPECT_EQ(canonical_composed(0x0057, 0x0300), 0x1E80) << "  Line: 1E80;LATIN CAPITAL LETTER W WITH GRAVE;Lu;0;L;0057 0300;;;;N;;;;1E81;";
    EXPECT_EQ(canonical_composed(0x0077, 0x0300), 0x1E81) << "  Line: 1E81;LATIN SMALL LETTER W WITH GRAVE;Ll;0;L;0077 0300;;;;N;;;1E80;;1E80";
    EXPECT_EQ(canonical_composed(0x0057, 0x0301), 0x1E82) << "  Line: 1E82;LATIN CAPITAL LETTER W WITH ACUTE;Lu;0;L;0057 0301;;;;N;;;;1E83;";
    EXPECT_EQ(canonical_composed(0x0077, 0x0301), 0x1E83) << "  Line: 1E83;LATIN SMALL LETTER W WITH ACUTE;Ll;0;L;0077 0301;;;;N;;;1E82;;1E82";
    EXPECT_EQ(canonical_composed(0x0057, 0x0308), 0x1E84) << "  Line: 1E84;LATIN CAPITAL LETTER W WITH DIAERESIS;Lu;0;L;0057 0308;;;;N;;;;1E85;";
    EXPECT_EQ(canonical_composed(0x0077, 0x0308), 0x1E85) << "  Line: 1E85;LATIN SMALL LETTER W WITH DIAERESIS;Ll;0;L;0077 0308;;;;N;;;1E84;;1E84";
    EXPECT_EQ(canonical_composed(0x0057, 0x0307), 0x1E86) << "  Line: 1E86;LATIN CAPITAL LETTER W WITH DOT ABOVE;Lu;0;L;0057 0307;;;;N;;;;1E87;";
    EXPECT_EQ(canonical_composed(0x0077, 0x0307), 0x1E87) << "  Line: 1E87;LATIN SMALL LETTER W WITH DOT ABOVE;Ll;0;L;0077 0307;;;;N;;;1E86;;1E86";
    EXPECT_EQ(canonical_composed(0x0057, 0x0323), 0x1E88) << "  Line: 1E88;LATIN CAPITAL LETTER W WITH DOT BELOW;Lu;0;L;0057 0323;;;;N;;;;1E89;";
    EXPECT_EQ(canonical_composed(0x0077, 0x0323), 0x1E89) << "  Line: 1E89;LATIN SMALL LETTER W WITH DOT BELOW;Ll;0;L;0077 0323;;;;N;;;1E88;;1E88";
    EXPECT_EQ(canonical_composed(0x0058, 0x0307), 0x1E8A) << "  Line: 1E8A;LATIN CAPITAL LETTER X WITH DOT ABOVE;Lu;0;L;0058 0307;;;;N;;;;1E8B;";
    EXPECT_EQ(canonical_composed(0x0078, 0x0307), 0x1E8B) << "  Line: 1E8B;LATIN SMALL LETTER X WITH DOT ABOVE;Ll;0;L;0078 0307;;;;N;;;1E8A;;1E8A";
    EXPECT_EQ(canonical_composed(0x0058, 0x0308), 0x1E8C) << "  Line: 1E8C;LATIN CAPITAL LETTER X WITH DIAERESIS;Lu;0;L;0058 0308;;;;N;;;;1E8D;";
    EXPECT_EQ(canonical_composed(0x0078, 0x0308), 0x1E8D) << "  Line: 1E8D;LATIN SMALL LETTER X WITH DIAERESIS;Ll;0;L;0078 0308;;;;N;;;1E8C;;1E8C";
    EXPECT_EQ(canonical_composed(0x0059, 0x0307), 0x1E8E) << "  Line: 1E8E;LATIN CAPITAL LETTER Y WITH DOT ABOVE;Lu;0;L;0059 0307;;;;N;;;;1E8F;";
    EXPECT_EQ(canonical_composed(0x0079, 0x0307), 0x1E8F) << "  Line: 1E8F;LATIN SMALL LETTER Y WITH DOT ABOVE;Ll;0;L;0079 0307;;;;N;;;1E8E;;1E8E";
    EXPECT_EQ(canonical_composed(0x005A, 0x0302), 0x1E90) << "  Line: 1E90;LATIN CAPITAL LETTER Z WITH CIRCUMFLEX;Lu;0;L;005A 0302;;;;N;;;;1E91;";
    EXPECT_EQ(canonical_composed(0x007A, 0x0302), 0x1E91) << "  Line: 1E91;LATIN SMALL LETTER Z WITH CIRCUMFLEX;Ll;0;L;007A 0302;;;;N;;;1E90;;1E90";
    EXPECT_EQ(canonical_composed(0x005A, 0x0323), 0x1E92) << "  Line: 1E92;LATIN CAPITAL LETTER Z WITH DOT BELOW;Lu;0;L;005A 0323;;;;N;;;;1E93;";
    EXPECT_EQ(canonical_composed(0x007A, 0x0323), 0x1E93) << "  Line: 1E93;LATIN SMALL LETTER Z WITH DOT BELOW;Ll;0;L;007A 0323;;;;N;;;1E92;;1E92";
    EXPECT_EQ(canonical_composed(0x005A, 0x0331), 0x1E94) << "  Line: 1E94;LATIN CAPITAL LETTER Z WITH LINE BELOW;Lu;0;L;005A 0331;;;;N;;;;1E95;";
    EXPECT_EQ(canonical_composed(0x007A, 0x0331), 0x1E95) << "  Line: 1E95;LATIN SMALL LETTER Z WITH LINE BELOW;Ll;0;L;007A 0331;;;;N;;;1E94;;1E94";
    EXPECT_EQ(canonical_composed(0x0068, 0x0331), 0x1E96) << "  Line: 1E96;LATIN SMALL LETTER H WITH LINE BELOW;Ll;0;L;0068 0331;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0074, 0x0308), 0x1E97) << "  Line: 1E97;LATIN SMALL LETTER T WITH DIAERESIS;Ll;0;L;0074 0308;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0077, 0x030A), 0x1E98) << "  Line: 1E98;LATIN SMALL LETTER W WITH RING ABOVE;Ll;0;L;0077 030A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0079, 0x030A), 0x1E99) << "  Line: 1E99;LATIN SMALL LETTER Y WITH RING ABOVE;Ll;0;L;0079 030A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x017F, 0x0307), 0x1E9B) << "  Line: 1E9B;LATIN SMALL LETTER LONG S WITH DOT ABOVE;Ll;0;L;017F 0307;;;;N;;;1E60;;1E60";
    EXPECT_EQ(canonical_composed(0x0041, 0x0323), 0x1EA0) << "  Line: 1EA0;LATIN CAPITAL LETTER A WITH DOT BELOW;Lu;0;L;0041 0323;;;;N;;;;1EA1;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0323), 0x1EA1) << "  Line: 1EA1;LATIN SMALL LETTER A WITH DOT BELOW;Ll;0;L;0061 0323;;;;N;;;1EA0;;1EA0";
    EXPECT_EQ(canonical_composed(0x0041, 0x0309), 0x1EA2) << "  Line: 1EA2;LATIN CAPITAL LETTER A WITH HOOK ABOVE;Lu;0;L;0041 0309;;;;N;;;;1EA3;";
    EXPECT_EQ(canonical_composed(0x0061, 0x0309), 0x1EA3) << "  Line: 1EA3;LATIN SMALL LETTER A WITH HOOK ABOVE;Ll;0;L;0061 0309;;;;N;;;1EA2;;1EA2";
    EXPECT_EQ(canonical_composed(0x00C2, 0x0301), 0x1EA4) << "  Line: 1EA4;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE;Lu;0;L;00C2 0301;;;;N;;;;1EA5;";
    EXPECT_EQ(canonical_composed(0x00E2, 0x0301), 0x1EA5) << "  Line: 1EA5;LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE;Ll;0;L;00E2 0301;;;;N;;;1EA4;;1EA4";
    EXPECT_EQ(canonical_composed(0x00C2, 0x0300), 0x1EA6) << "  Line: 1EA6;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE;Lu;0;L;00C2 0300;;;;N;;;;1EA7;";
    EXPECT_EQ(canonical_composed(0x00E2, 0x0300), 0x1EA7) << "  Line: 1EA7;LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE;Ll;0;L;00E2 0300;;;;N;;;1EA6;;1EA6";
    EXPECT_EQ(canonical_composed(0x00C2, 0x0309), 0x1EA8) << "  Line: 1EA8;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE;Lu;0;L;00C2 0309;;;;N;;;;1EA9;";
    EXPECT_EQ(canonical_composed(0x00E2, 0x0309), 0x1EA9) << "  Line: 1EA9;LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE;Ll;0;L;00E2 0309;;;;N;;;1EA8;;1EA8";
    EXPECT_EQ(canonical_composed(0x00C2, 0x0303), 0x1EAA) << "  Line: 1EAA;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE;Lu;0;L;00C2 0303;;;;N;;;;1EAB;";
    EXPECT_EQ(canonical_composed(0x00E2, 0x0303), 0x1EAB) << "  Line: 1EAB;LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE;Ll;0;L;00E2 0303;;;;N;;;1EAA;;1EAA";
    EXPECT_EQ(canonical_composed(0x1EA0, 0x0302), 0x1EAC) << "  Line: 1EAC;LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW;Lu;0;L;1EA0 0302;;;;N;;;;1EAD;";
    EXPECT_EQ(canonical_composed(0x1EA1, 0x0302), 0x1EAD) << "  Line: 1EAD;LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW;Ll;0;L;1EA1 0302;;;;N;;;1EAC;;1EAC";
    EXPECT_EQ(canonical_composed(0x0102, 0x0301), 0x1EAE) << "  Line: 1EAE;LATIN CAPITAL LETTER A WITH BREVE AND ACUTE;Lu;0;L;0102 0301;;;;N;;;;1EAF;";
    EXPECT_EQ(canonical_composed(0x0103, 0x0301), 0x1EAF) << "  Line: 1EAF;LATIN SMALL LETTER A WITH BREVE AND ACUTE;Ll;0;L;0103 0301;;;;N;;;1EAE;;1EAE";
    EXPECT_EQ(canonical_composed(0x0102, 0x0300), 0x1EB0) << "  Line: 1EB0;LATIN CAPITAL LETTER A WITH BREVE AND GRAVE;Lu;0;L;0102 0300;;;;N;;;;1EB1;";
    EXPECT_EQ(canonical_composed(0x0103, 0x0300), 0x1EB1) << "  Line: 1EB1;LATIN SMALL LETTER A WITH BREVE AND GRAVE;Ll;0;L;0103 0300;;;;N;;;1EB0;;1EB0";
    EXPECT_EQ(canonical_composed(0x0102, 0x0309), 0x1EB2) << "  Line: 1EB2;LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE;Lu;0;L;0102 0309;;;;N;;;;1EB3;";
    EXPECT_EQ(canonical_composed(0x0103, 0x0309), 0x1EB3) << "  Line: 1EB3;LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE;Ll;0;L;0103 0309;;;;N;;;1EB2;;1EB2";
    EXPECT_EQ(canonical_composed(0x0102, 0x0303), 0x1EB4) << "  Line: 1EB4;LATIN CAPITAL LETTER A WITH BREVE AND TILDE;Lu;0;L;0102 0303;;;;N;;;;1EB5;";
    EXPECT_EQ(canonical_composed(0x0103, 0x0303), 0x1EB5) << "  Line: 1EB5;LATIN SMALL LETTER A WITH BREVE AND TILDE;Ll;0;L;0103 0303;;;;N;;;1EB4;;1EB4";
    EXPECT_EQ(canonical_composed(0x1EA0, 0x0306), 0x1EB6) << "  Line: 1EB6;LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW;Lu;0;L;1EA0 0306;;;;N;;;;1EB7;";
    EXPECT_EQ(canonical_composed(0x1EA1, 0x0306), 0x1EB7) << "  Line: 1EB7;LATIN SMALL LETTER A WITH BREVE AND DOT BELOW;Ll;0;L;1EA1 0306;;;;N;;;1EB6;;1EB6";
    EXPECT_EQ(canonical_composed(0x0045, 0x0323), 0x1EB8) << "  Line: 1EB8;LATIN CAPITAL LETTER E WITH DOT BELOW;Lu;0;L;0045 0323;;;;N;;;;1EB9;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0323), 0x1EB9) << "  Line: 1EB9;LATIN SMALL LETTER E WITH DOT BELOW;Ll;0;L;0065 0323;;;;N;;;1EB8;;1EB8";
    EXPECT_EQ(canonical_composed(0x0045, 0x0309), 0x1EBA) << "  Line: 1EBA;LATIN CAPITAL LETTER E WITH HOOK ABOVE;Lu;0;L;0045 0309;;;;N;;;;1EBB;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0309), 0x1EBB) << "  Line: 1EBB;LATIN SMALL LETTER E WITH HOOK ABOVE;Ll;0;L;0065 0309;;;;N;;;1EBA;;1EBA";
    EXPECT_EQ(canonical_composed(0x0045, 0x0303), 0x1EBC) << "  Line: 1EBC;LATIN CAPITAL LETTER E WITH TILDE;Lu;0;L;0045 0303;;;;N;;;;1EBD;";
    EXPECT_EQ(canonical_composed(0x0065, 0x0303), 0x1EBD) << "  Line: 1EBD;LATIN SMALL LETTER E WITH TILDE;Ll;0;L;0065 0303;;;;N;;;1EBC;;1EBC";
    EXPECT_EQ(canonical_composed(0x00CA, 0x0301), 0x1EBE) << "  Line: 1EBE;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE;Lu;0;L;00CA 0301;;;;N;;;;1EBF;";
    EXPECT_EQ(canonical_composed(0x00EA, 0x0301), 0x1EBF) << "  Line: 1EBF;LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE;Ll;0;L;00EA 0301;;;;N;;;1EBE;;1EBE";
    EXPECT_EQ(canonical_composed(0x00CA, 0x0300), 0x1EC0) << "  Line: 1EC0;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE;Lu;0;L;00CA 0300;;;;N;;;;1EC1;";
    EXPECT_EQ(canonical_composed(0x00EA, 0x0300), 0x1EC1) << "  Line: 1EC1;LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE;Ll;0;L;00EA 0300;;;;N;;;1EC0;;1EC0";
    EXPECT_EQ(canonical_composed(0x00CA, 0x0309), 0x1EC2) << "  Line: 1EC2;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE;Lu;0;L;00CA 0309;;;;N;;;;1EC3;";
    EXPECT_EQ(canonical_composed(0x00EA, 0x0309), 0x1EC3) << "  Line: 1EC3;LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE;Ll;0;L;00EA 0309;;;;N;;;1EC2;;1EC2";
    EXPECT_EQ(canonical_composed(0x00CA, 0x0303), 0x1EC4) << "  Line: 1EC4;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE;Lu;0;L;00CA 0303;;;;N;;;;1EC5;";
    EXPECT_EQ(canonical_composed(0x00EA, 0x0303), 0x1EC5) << "  Line: 1EC5;LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE;Ll;0;L;00EA 0303;;;;N;;;1EC4;;1EC4";
    EXPECT_EQ(canonical_composed(0x1EB8, 0x0302), 0x1EC6) << "  Line: 1EC6;LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW;Lu;0;L;1EB8 0302;;;;N;;;;1EC7;";
    EXPECT_EQ(canonical_composed(0x1EB9, 0x0302), 0x1EC7) << "  Line: 1EC7;LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW;Ll;0;L;1EB9 0302;;;;N;;;1EC6;;1EC6";
    EXPECT_EQ(canonical_composed(0x0049, 0x0309), 0x1EC8) << "  Line: 1EC8;LATIN CAPITAL LETTER I WITH HOOK ABOVE;Lu;0;L;0049 0309;;;;N;;;;1EC9;";
    EXPECT_EQ(canonical_composed(0x0069, 0x0309), 0x1EC9) << "  Line: 1EC9;LATIN SMALL LETTER I WITH HOOK ABOVE;Ll;0;L;0069 0309;;;;N;;;1EC8;;1EC8";
    EXPECT_EQ(canonical_composed(0x0049, 0x0323), 0x1ECA) << "  Line: 1ECA;LATIN CAPITAL LETTER I WITH DOT BELOW;Lu;0;L;0049 0323;;;;N;;;;1ECB;";
    EXPECT_EQ(canonical_composed(0x0069, 0x0323), 0x1ECB) << "  Line: 1ECB;LATIN SMALL LETTER I WITH DOT BELOW;Ll;0;L;0069 0323;;;;N;;;1ECA;;1ECA";
    EXPECT_EQ(canonical_composed(0x004F, 0x0323), 0x1ECC) << "  Line: 1ECC;LATIN CAPITAL LETTER O WITH DOT BELOW;Lu;0;L;004F 0323;;;;N;;;;1ECD;";
    EXPECT_EQ(canonical_composed(0x006F, 0x0323), 0x1ECD) << "  Line: 1ECD;LATIN SMALL LETTER O WITH DOT BELOW;Ll;0;L;006F 0323;;;;N;;;1ECC;;1ECC";
    EXPECT_EQ(canonical_composed(0x004F, 0x0309), 0x1ECE) << "  Line: 1ECE;LATIN CAPITAL LETTER O WITH HOOK ABOVE;Lu;0;L;004F 0309;;;;N;;;;1ECF;";
    EXPECT_EQ(canonical_composed(0x006F, 0x0309), 0x1ECF) << "  Line: 1ECF;LATIN SMALL LETTER O WITH HOOK ABOVE;Ll;0;L;006F 0309;;;;N;;;1ECE;;1ECE";
    EXPECT_EQ(canonical_composed(0x00D4, 0x0301), 0x1ED0) << "  Line: 1ED0;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE;Lu;0;L;00D4 0301;;;;N;;;;1ED1;";
    EXPECT_EQ(canonical_composed(0x00F4, 0x0301), 0x1ED1) << "  Line: 1ED1;LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE;Ll;0;L;00F4 0301;;;;N;;;1ED0;;1ED0";
    EXPECT_EQ(canonical_composed(0x00D4, 0x0300), 0x1ED2) << "  Line: 1ED2;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE;Lu;0;L;00D4 0300;;;;N;;;;1ED3;";
    EXPECT_EQ(canonical_composed(0x00F4, 0x0300), 0x1ED3) << "  Line: 1ED3;LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE;Ll;0;L;00F4 0300;;;;N;;;1ED2;;1ED2";
    EXPECT_EQ(canonical_composed(0x00D4, 0x0309), 0x1ED4) << "  Line: 1ED4;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE;Lu;0;L;00D4 0309;;;;N;;;;1ED5;";
    EXPECT_EQ(canonical_composed(0x00F4, 0x0309), 0x1ED5) << "  Line: 1ED5;LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE;Ll;0;L;00F4 0309;;;;N;;;1ED4;;1ED4";
    EXPECT_EQ(canonical_composed(0x00D4, 0x0303), 0x1ED6) << "  Line: 1ED6;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE;Lu;0;L;00D4 0303;;;;N;;;;1ED7;";
    EXPECT_EQ(canonical_composed(0x00F4, 0x0303), 0x1ED7) << "  Line: 1ED7;LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE;Ll;0;L;00F4 0303;;;;N;;;1ED6;;1ED6";
    EXPECT_EQ(canonical_composed(0x1ECC, 0x0302), 0x1ED8) << "  Line: 1ED8;LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW;Lu;0;L;1ECC 0302;;;;N;;;;1ED9;";
    EXPECT_EQ(canonical_composed(0x1ECD, 0x0302), 0x1ED9) << "  Line: 1ED9;LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW;Ll;0;L;1ECD 0302;;;;N;;;1ED8;;1ED8";
    EXPECT_EQ(canonical_composed(0x01A0, 0x0301), 0x1EDA) << "  Line: 1EDA;LATIN CAPITAL LETTER O WITH HORN AND ACUTE;Lu;0;L;01A0 0301;;;;N;;;;1EDB;";
    EXPECT_EQ(canonical_composed(0x01A1, 0x0301), 0x1EDB) << "  Line: 1EDB;LATIN SMALL LETTER O WITH HORN AND ACUTE;Ll;0;L;01A1 0301;;;;N;;;1EDA;;1EDA";
    EXPECT_EQ(canonical_composed(0x01A0, 0x0300), 0x1EDC) << "  Line: 1EDC;LATIN CAPITAL LETTER O WITH HORN AND GRAVE;Lu;0;L;01A0 0300;;;;N;;;;1EDD;";
    EXPECT_EQ(canonical_composed(0x01A1, 0x0300), 0x1EDD) << "  Line: 1EDD;LATIN SMALL LETTER O WITH HORN AND GRAVE;Ll;0;L;01A1 0300;;;;N;;;1EDC;;1EDC";
    EXPECT_EQ(canonical_composed(0x01A0, 0x0309), 0x1EDE) << "  Line: 1EDE;LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE;Lu;0;L;01A0 0309;;;;N;;;;1EDF;";
    EXPECT_EQ(canonical_composed(0x01A1, 0x0309), 0x1EDF) << "  Line: 1EDF;LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE;Ll;0;L;01A1 0309;;;;N;;;1EDE;;1EDE";
    EXPECT_EQ(canonical_composed(0x01A0, 0x0303), 0x1EE0) << "  Line: 1EE0;LATIN CAPITAL LETTER O WITH HORN AND TILDE;Lu;0;L;01A0 0303;;;;N;;;;1EE1;";
    EXPECT_EQ(canonical_composed(0x01A1, 0x0303), 0x1EE1) << "  Line: 1EE1;LATIN SMALL LETTER O WITH HORN AND TILDE;Ll;0;L;01A1 0303;;;;N;;;1EE0;;1EE0";
    EXPECT_EQ(canonical_composed(0x01A0, 0x0323), 0x1EE2) << "  Line: 1EE2;LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW;Lu;0;L;01A0 0323;;;;N;;;;1EE3;";
    EXPECT_EQ(canonical_composed(0x01A1, 0x0323), 0x1EE3) << "  Line: 1EE3;LATIN SMALL LETTER O WITH HORN AND DOT BELOW;Ll;0;L;01A1 0323;;;;N;;;1EE2;;1EE2";
    EXPECT_EQ(canonical_composed(0x0055, 0x0323), 0x1EE4) << "  Line: 1EE4;LATIN CAPITAL LETTER U WITH DOT BELOW;Lu;0;L;0055 0323;;;;N;;;;1EE5;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0323), 0x1EE5) << "  Line: 1EE5;LATIN SMALL LETTER U WITH DOT BELOW;Ll;0;L;0075 0323;;;;N;;;1EE4;;1EE4";
    EXPECT_EQ(canonical_composed(0x0055, 0x0309), 0x1EE6) << "  Line: 1EE6;LATIN CAPITAL LETTER U WITH HOOK ABOVE;Lu;0;L;0055 0309;;;;N;;;;1EE7;";
    EXPECT_EQ(canonical_composed(0x0075, 0x0309), 0x1EE7) << "  Line: 1EE7;LATIN SMALL LETTER U WITH HOOK ABOVE;Ll;0;L;0075 0309;;;;N;;;1EE6;;1EE6";
    EXPECT_EQ(canonical_composed(0x01AF, 0x0301), 0x1EE8) << "  Line: 1EE8;LATIN CAPITAL LETTER U WITH HORN AND ACUTE;Lu;0;L;01AF 0301;;;;N;;;;1EE9;";
    EXPECT_EQ(canonical_composed(0x01B0, 0x0301), 0x1EE9) << "  Line: 1EE9;LATIN SMALL LETTER U WITH HORN AND ACUTE;Ll;0;L;01B0 0301;;;;N;;;1EE8;;1EE8";
    EXPECT_EQ(canonical_composed(0x01AF, 0x0300), 0x1EEA) << "  Line: 1EEA;LATIN CAPITAL LETTER U WITH HORN AND GRAVE;Lu;0;L;01AF 0300;;;;N;;;;1EEB;";
    EXPECT_EQ(canonical_composed(0x01B0, 0x0300), 0x1EEB) << "  Line: 1EEB;LATIN SMALL LETTER U WITH HORN AND GRAVE;Ll;0;L;01B0 0300;;;;N;;;1EEA;;1EEA";
    EXPECT_EQ(canonical_composed(0x01AF, 0x0309), 0x1EEC) << "  Line: 1EEC;LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE;Lu;0;L;01AF 0309;;;;N;;;;1EED;";
    EXPECT_EQ(canonical_composed(0x01B0, 0x0309), 0x1EED) << "  Line: 1EED;LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE;Ll;0;L;01B0 0309;;;;N;;;1EEC;;1EEC";
    EXPECT_EQ(canonical_composed(0x01AF, 0x0303), 0x1EEE) << "  Line: 1EEE;LATIN CAPITAL LETTER U WITH HORN AND TILDE;Lu;0;L;01AF 0303;;;;N;;;;1EEF;";
    EXPECT_EQ(canonical_composed(0x01B0, 0x0303), 0x1EEF) << "  Line: 1EEF;LATIN SMALL LETTER U WITH HORN AND TILDE;Ll;0;L;01B0 0303;;;;N;;;1EEE;;1EEE";
    EXPECT_EQ(canonical_composed(0x01AF, 0x0323), 0x1EF0) << "  Line: 1EF0;LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW;Lu;0;L;01AF 0323;;;;N;;;;1EF1;";
    EXPECT_EQ(canonical_composed(0x01B0, 0x0323), 0x1EF1) << "  Line: 1EF1;LATIN SMALL LETTER U WITH HORN AND DOT BELOW;Ll;0;L;01B0 0323;;;;N;;;1EF0;;1EF0";
    EXPECT_EQ(canonical_composed(0x0059, 0x0300), 0x1EF2) << "  Line: 1EF2;LATIN CAPITAL LETTER Y WITH GRAVE;Lu;0;L;0059 0300;;;;N;;;;1EF3;";
    EXPECT_EQ(canonical_composed(0x0079, 0x0300), 0x1EF3) << "  Line: 1EF3;LATIN SMALL LETTER Y WITH GRAVE;Ll;0;L;0079 0300;;;;N;;;1EF2;;1EF2";
    EXPECT_EQ(canonical_composed(0x0059, 0x0323), 0x1EF4) << "  Line: 1EF4;LATIN CAPITAL LETTER Y WITH DOT BELOW;Lu;0;L;0059 0323;;;;N;;;;1EF5;";
    EXPECT_EQ(canonical_composed(0x0079, 0x0323), 0x1EF5) << "  Line: 1EF5;LATIN SMALL LETTER Y WITH DOT BELOW;Ll;0;L;0079 0323;;;;N;;;1EF4;;1EF4";
    EXPECT_EQ(canonical_composed(0x0059, 0x0309), 0x1EF6) << "  Line: 1EF6;LATIN CAPITAL LETTER Y WITH HOOK ABOVE;Lu;0;L;0059 0309;;;;N;;;;1EF7;";
    EXPECT_EQ(canonical_composed(0x0079, 0x0309), 0x1EF7) << "  Line: 1EF7;LATIN SMALL LETTER Y WITH HOOK ABOVE;Ll;0;L;0079 0309;;;;N;;;1EF6;;1EF6";
    EXPECT_EQ(canonical_composed(0x0059, 0x0303), 0x1EF8) << "  Line: 1EF8;LATIN CAPITAL LETTER Y WITH TILDE;Lu;0;L;0059 0303;;;;N;;;;1EF9;";
    EXPECT_EQ(canonical_composed(0x0079, 0x0303), 0x1EF9) << "  Line: 1EF9;LATIN SMALL LETTER Y WITH TILDE;Ll;0;L;0079 0303;;;;N;;;1EF8;;1EF8";
    EXPECT_EQ(canonical_composed(0x03B1, 0x0313), 0x1F00) << "  Line: 1F00;GREEK SMALL LETTER ALPHA WITH PSILI;Ll;0;L;03B1 0313;;;;N;;;1F08;;1F08";
    EXPECT_EQ(canonical_composed(0x03B1, 0x0314), 0x1F01) << "  Line: 1F01;GREEK SMALL LETTER ALPHA WITH DASIA;Ll;0;L;03B1 0314;;;;N;;;1F09;;1F09";
    EXPECT_EQ(canonical_composed(0x1F00, 0x0300), 0x1F02) << "  Line: 1F02;GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA;Ll;0;L;1F00 0300;;;;N;;;1F0A;;1F0A";
    EXPECT_EQ(canonical_composed(0x1F01, 0x0300), 0x1F03) << "  Line: 1F03;GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA;Ll;0;L;1F01 0300;;;;N;;;1F0B;;1F0B";
    EXPECT_EQ(canonical_composed(0x1F00, 0x0301), 0x1F04) << "  Line: 1F04;GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA;Ll;0;L;1F00 0301;;;;N;;;1F0C;;1F0C";
    EXPECT_EQ(canonical_composed(0x1F01, 0x0301), 0x1F05) << "  Line: 1F05;GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA;Ll;0;L;1F01 0301;;;;N;;;1F0D;;1F0D";
    EXPECT_EQ(canonical_composed(0x1F00, 0x0342), 0x1F06) << "  Line: 1F06;GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI;Ll;0;L;1F00 0342;;;;N;;;1F0E;;1F0E";
    EXPECT_EQ(canonical_composed(0x1F01, 0x0342), 0x1F07) << "  Line: 1F07;GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI;Ll;0;L;1F01 0342;;;;N;;;1F0F;;1F0F";
    EXPECT_EQ(canonical_composed(0x0391, 0x0313), 0x1F08) << "  Line: 1F08;GREEK CAPITAL LETTER ALPHA WITH PSILI;Lu;0;L;0391 0313;;;;N;;;;1F00;";
    EXPECT_EQ(canonical_composed(0x0391, 0x0314), 0x1F09) << "  Line: 1F09;GREEK CAPITAL LETTER ALPHA WITH DASIA;Lu;0;L;0391 0314;;;;N;;;;1F01;";
    EXPECT_EQ(canonical_composed(0x1F08, 0x0300), 0x1F0A) << "  Line: 1F0A;GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA;Lu;0;L;1F08 0300;;;;N;;;;1F02;";
    EXPECT_EQ(canonical_composed(0x1F09, 0x0300), 0x1F0B) << "  Line: 1F0B;GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA;Lu;0;L;1F09 0300;;;;N;;;;1F03;";
    EXPECT_EQ(canonical_composed(0x1F08, 0x0301), 0x1F0C) << "  Line: 1F0C;GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA;Lu;0;L;1F08 0301;;;;N;;;;1F04;";
    EXPECT_EQ(canonical_composed(0x1F09, 0x0301), 0x1F0D) << "  Line: 1F0D;GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA;Lu;0;L;1F09 0301;;;;N;;;;1F05;";
    EXPECT_EQ(canonical_composed(0x1F08, 0x0342), 0x1F0E) << "  Line: 1F0E;GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI;Lu;0;L;1F08 0342;;;;N;;;;1F06;";
    EXPECT_EQ(canonical_composed(0x1F09, 0x0342), 0x1F0F) << "  Line: 1F0F;GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI;Lu;0;L;1F09 0342;;;;N;;;;1F07;";
    EXPECT_EQ(canonical_composed(0x03B5, 0x0313), 0x1F10) << "  Line: 1F10;GREEK SMALL LETTER EPSILON WITH PSILI;Ll;0;L;03B5 0313;;;;N;;;1F18;;1F18";
    EXPECT_EQ(canonical_composed(0x03B5, 0x0314), 0x1F11) << "  Line: 1F11;GREEK SMALL LETTER EPSILON WITH DASIA;Ll;0;L;03B5 0314;;;;N;;;1F19;;1F19";
    EXPECT_EQ(canonical_composed(0x1F10, 0x0300), 0x1F12) << "  Line: 1F12;GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA;Ll;0;L;1F10 0300;;;;N;;;1F1A;;1F1A";
    EXPECT_EQ(canonical_composed(0x1F11, 0x0300), 0x1F13) << "  Line: 1F13;GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA;Ll;0;L;1F11 0300;;;;N;;;1F1B;;1F1B";
    EXPECT_EQ(canonical_composed(0x1F10, 0x0301), 0x1F14) << "  Line: 1F14;GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA;Ll;0;L;1F10 0301;;;;N;;;1F1C;;1F1C";
    EXPECT_EQ(canonical_composed(0x1F11, 0x0301), 0x1F15) << "  Line: 1F15;GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA;Ll;0;L;1F11 0301;;;;N;;;1F1D;;1F1D";
    EXPECT_EQ(canonical_composed(0x0395, 0x0313), 0x1F18) << "  Line: 1F18;GREEK CAPITAL LETTER EPSILON WITH PSILI;Lu;0;L;0395 0313;;;;N;;;;1F10;";
    EXPECT_EQ(canonical_composed(0x0395, 0x0314), 0x1F19) << "  Line: 1F19;GREEK CAPITAL LETTER EPSILON WITH DASIA;Lu;0;L;0395 0314;;;;N;;;;1F11;";
    EXPECT_EQ(canonical_composed(0x1F18, 0x0300), 0x1F1A) << "  Line: 1F1A;GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA;Lu;0;L;1F18 0300;;;;N;;;;1F12;";
    EXPECT_EQ(canonical_composed(0x1F19, 0x0300), 0x1F1B) << "  Line: 1F1B;GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA;Lu;0;L;1F19 0300;;;;N;;;;1F13;";
    EXPECT_EQ(canonical_composed(0x1F18, 0x0301), 0x1F1C) << "  Line: 1F1C;GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA;Lu;0;L;1F18 0301;;;;N;;;;1F14;";
    EXPECT_EQ(canonical_composed(0x1F19, 0x0301), 0x1F1D) << "  Line: 1F1D;GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA;Lu;0;L;1F19 0301;;;;N;;;;1F15;";
    EXPECT_EQ(canonical_composed(0x03B7, 0x0313), 0x1F20) << "  Line: 1F20;GREEK SMALL LETTER ETA WITH PSILI;Ll;0;L;03B7 0313;;;;N;;;1F28;;1F28";
    EXPECT_EQ(canonical_composed(0x03B7, 0x0314), 0x1F21) << "  Line: 1F21;GREEK SMALL LETTER ETA WITH DASIA;Ll;0;L;03B7 0314;;;;N;;;1F29;;1F29";
    EXPECT_EQ(canonical_composed(0x1F20, 0x0300), 0x1F22) << "  Line: 1F22;GREEK SMALL LETTER ETA WITH PSILI AND VARIA;Ll;0;L;1F20 0300;;;;N;;;1F2A;;1F2A";
    EXPECT_EQ(canonical_composed(0x1F21, 0x0300), 0x1F23) << "  Line: 1F23;GREEK SMALL LETTER ETA WITH DASIA AND VARIA;Ll;0;L;1F21 0300;;;;N;;;1F2B;;1F2B";
    EXPECT_EQ(canonical_composed(0x1F20, 0x0301), 0x1F24) << "  Line: 1F24;GREEK SMALL LETTER ETA WITH PSILI AND OXIA;Ll;0;L;1F20 0301;;;;N;;;1F2C;;1F2C";
    EXPECT_EQ(canonical_composed(0x1F21, 0x0301), 0x1F25) << "  Line: 1F25;GREEK SMALL LETTER ETA WITH DASIA AND OXIA;Ll;0;L;1F21 0301;;;;N;;;1F2D;;1F2D";
    EXPECT_EQ(canonical_composed(0x1F20, 0x0342), 0x1F26) << "  Line: 1F26;GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI;Ll;0;L;1F20 0342;;;;N;;;1F2E;;1F2E";
    EXPECT_EQ(canonical_composed(0x1F21, 0x0342), 0x1F27) << "  Line: 1F27;GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI;Ll;0;L;1F21 0342;;;;N;;;1F2F;;1F2F";
    EXPECT_EQ(canonical_composed(0x0397, 0x0313), 0x1F28) << "  Line: 1F28;GREEK CAPITAL LETTER ETA WITH PSILI;Lu;0;L;0397 0313;;;;N;;;;1F20;";
    EXPECT_EQ(canonical_composed(0x0397, 0x0314), 0x1F29) << "  Line: 1F29;GREEK CAPITAL LETTER ETA WITH DASIA;Lu;0;L;0397 0314;;;;N;;;;1F21;";
    EXPECT_EQ(canonical_composed(0x1F28, 0x0300), 0x1F2A) << "  Line: 1F2A;GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA;Lu;0;L;1F28 0300;;;;N;;;;1F22;";
    EXPECT_EQ(canonical_composed(0x1F29, 0x0300), 0x1F2B) << "  Line: 1F2B;GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA;Lu;0;L;1F29 0300;;;;N;;;;1F23;";
    EXPECT_EQ(canonical_composed(0x1F28, 0x0301), 0x1F2C) << "  Line: 1F2C;GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA;Lu;0;L;1F28 0301;;;;N;;;;1F24;";
    EXPECT_EQ(canonical_composed(0x1F29, 0x0301), 0x1F2D) << "  Line: 1F2D;GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA;Lu;0;L;1F29 0301;;;;N;;;;1F25;";
    EXPECT_EQ(canonical_composed(0x1F28, 0x0342), 0x1F2E) << "  Line: 1F2E;GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI;Lu;0;L;1F28 0342;;;;N;;;;1F26;";
    EXPECT_EQ(canonical_composed(0x1F29, 0x0342), 0x1F2F) << "  Line: 1F2F;GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI;Lu;0;L;1F29 0342;;;;N;;;;1F27;";
    EXPECT_EQ(canonical_composed(0x03B9, 0x0313), 0x1F30) << "  Line: 1F30;GREEK SMALL LETTER IOTA WITH PSILI;Ll;0;L;03B9 0313;;;;N;;;1F38;;1F38";
    EXPECT_EQ(canonical_composed(0x03B9, 0x0314), 0x1F31) << "  Line: 1F31;GREEK SMALL LETTER IOTA WITH DASIA;Ll;0;L;03B9 0314;;;;N;;;1F39;;1F39";
    EXPECT_EQ(canonical_composed(0x1F30, 0x0300), 0x1F32) << "  Line: 1F32;GREEK SMALL LETTER IOTA WITH PSILI AND VARIA;Ll;0;L;1F30 0300;;;;N;;;1F3A;;1F3A";
    EXPECT_EQ(canonical_composed(0x1F31, 0x0300), 0x1F33) << "  Line: 1F33;GREEK SMALL LETTER IOTA WITH DASIA AND VARIA;Ll;0;L;1F31 0300;;;;N;;;1F3B;;1F3B";
    EXPECT_EQ(canonical_composed(0x1F30, 0x0301), 0x1F34) << "  Line: 1F34;GREEK SMALL LETTER IOTA WITH PSILI AND OXIA;Ll;0;L;1F30 0301;;;;N;;;1F3C;;1F3C";
    EXPECT_EQ(canonical_composed(0x1F31, 0x0301), 0x1F35) << "  Line: 1F35;GREEK SMALL LETTER IOTA WITH DASIA AND OXIA;Ll;0;L;1F31 0301;;;;N;;;1F3D;;1F3D";
    EXPECT_EQ(canonical_composed(0x1F30, 0x0342), 0x1F36) << "  Line: 1F36;GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI;Ll;0;L;1F30 0342;;;;N;;;1F3E;;1F3E";
    EXPECT_EQ(canonical_composed(0x1F31, 0x0342), 0x1F37) << "  Line: 1F37;GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI;Ll;0;L;1F31 0342;;;;N;;;1F3F;;1F3F";
    EXPECT_EQ(canonical_composed(0x0399, 0x0313), 0x1F38) << "  Line: 1F38;GREEK CAPITAL LETTER IOTA WITH PSILI;Lu;0;L;0399 0313;;;;N;;;;1F30;";
    EXPECT_EQ(canonical_composed(0x0399, 0x0314), 0x1F39) << "  Line: 1F39;GREEK CAPITAL LETTER IOTA WITH DASIA;Lu;0;L;0399 0314;;;;N;;;;1F31;";
    EXPECT_EQ(canonical_composed(0x1F38, 0x0300), 0x1F3A) << "  Line: 1F3A;GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA;Lu;0;L;1F38 0300;;;;N;;;;1F32;";
    EXPECT_EQ(canonical_composed(0x1F39, 0x0300), 0x1F3B) << "  Line: 1F3B;GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA;Lu;0;L;1F39 0300;;;;N;;;;1F33;";
    EXPECT_EQ(canonical_composed(0x1F38, 0x0301), 0x1F3C) << "  Line: 1F3C;GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA;Lu;0;L;1F38 0301;;;;N;;;;1F34;";
    EXPECT_EQ(canonical_composed(0x1F39, 0x0301), 0x1F3D) << "  Line: 1F3D;GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA;Lu;0;L;1F39 0301;;;;N;;;;1F35;";
    EXPECT_EQ(canonical_composed(0x1F38, 0x0342), 0x1F3E) << "  Line: 1F3E;GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI;Lu;0;L;1F38 0342;;;;N;;;;1F36;";
    EXPECT_EQ(canonical_composed(0x1F39, 0x0342), 0x1F3F) << "  Line: 1F3F;GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI;Lu;0;L;1F39 0342;;;;N;;;;1F37;";
    EXPECT_EQ(canonical_composed(0x03BF, 0x0313), 0x1F40) << "  Line: 1F40;GREEK SMALL LETTER OMICRON WITH PSILI;Ll;0;L;03BF 0313;;;;N;;;1F48;;1F48";
    EXPECT_EQ(canonical_composed(0x03BF, 0x0314), 0x1F41) << "  Line: 1F41;GREEK SMALL LETTER OMICRON WITH DASIA;Ll;0;L;03BF 0314;;;;N;;;1F49;;1F49";
    EXPECT_EQ(canonical_composed(0x1F40, 0x0300), 0x1F42) << "  Line: 1F42;GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA;Ll;0;L;1F40 0300;;;;N;;;1F4A;;1F4A";
    EXPECT_EQ(canonical_composed(0x1F41, 0x0300), 0x1F43) << "  Line: 1F43;GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA;Ll;0;L;1F41 0300;;;;N;;;1F4B;;1F4B";
    EXPECT_EQ(canonical_composed(0x1F40, 0x0301), 0x1F44) << "  Line: 1F44;GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA;Ll;0;L;1F40 0301;;;;N;;;1F4C;;1F4C";
    EXPECT_EQ(canonical_composed(0x1F41, 0x0301), 0x1F45) << "  Line: 1F45;GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA;Ll;0;L;1F41 0301;;;;N;;;1F4D;;1F4D";
    EXPECT_EQ(canonical_composed(0x039F, 0x0313), 0x1F48) << "  Line: 1F48;GREEK CAPITAL LETTER OMICRON WITH PSILI;Lu;0;L;039F 0313;;;;N;;;;1F40;";
    EXPECT_EQ(canonical_composed(0x039F, 0x0314), 0x1F49) << "  Line: 1F49;GREEK CAPITAL LETTER OMICRON WITH DASIA;Lu;0;L;039F 0314;;;;N;;;;1F41;";
    EXPECT_EQ(canonical_composed(0x1F48, 0x0300), 0x1F4A) << "  Line: 1F4A;GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA;Lu;0;L;1F48 0300;;;;N;;;;1F42;";
    EXPECT_EQ(canonical_composed(0x1F49, 0x0300), 0x1F4B) << "  Line: 1F4B;GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA;Lu;0;L;1F49 0300;;;;N;;;;1F43;";
    EXPECT_EQ(canonical_composed(0x1F48, 0x0301), 0x1F4C) << "  Line: 1F4C;GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA;Lu;0;L;1F48 0301;;;;N;;;;1F44;";
    EXPECT_EQ(canonical_composed(0x1F49, 0x0301), 0x1F4D) << "  Line: 1F4D;GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA;Lu;0;L;1F49 0301;;;;N;;;;1F45;";
    EXPECT_EQ(canonical_composed(0x03C5, 0x0313), 0x1F50) << "  Line: 1F50;GREEK SMALL LETTER UPSILON WITH PSILI;Ll;0;L;03C5 0313;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03C5, 0x0314), 0x1F51) << "  Line: 1F51;GREEK SMALL LETTER UPSILON WITH DASIA;Ll;0;L;03C5 0314;;;;N;;;1F59;;1F59";
    EXPECT_EQ(canonical_composed(0x1F50, 0x0300), 0x1F52) << "  Line: 1F52;GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA;Ll;0;L;1F50 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1F51, 0x0300), 0x1F53) << "  Line: 1F53;GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA;Ll;0;L;1F51 0300;;;;N;;;1F5B;;1F5B";
    EXPECT_EQ(canonical_composed(0x1F50, 0x0301), 0x1F54) << "  Line: 1F54;GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA;Ll;0;L;1F50 0301;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1F51, 0x0301), 0x1F55) << "  Line: 1F55;GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA;Ll;0;L;1F51 0301;;;;N;;;1F5D;;1F5D";
    EXPECT_EQ(canonical_composed(0x1F50, 0x0342), 0x1F56) << "  Line: 1F56;GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI;Ll;0;L;1F50 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1F51, 0x0342), 0x1F57) << "  Line: 1F57;GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI;Ll;0;L;1F51 0342;;;;N;;;1F5F;;1F5F";
    EXPECT_EQ(canonical_composed(0x03A5, 0x0314), 0x1F59) << "  Line: 1F59;GREEK CAPITAL LETTER UPSILON WITH DASIA;Lu;0;L;03A5 0314;;;;N;;;;1F51;";
    EXPECT_EQ(canonical_composed(0x1F59, 0x0300), 0x1F5B) << "  Line: 1F5B;GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA;Lu;0;L;1F59 0300;;;;N;;;;1F53;";
    EXPECT_EQ(canonical_composed(0x1F59, 0x0301), 0x1F5D) << "  Line: 1F5D;GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA;Lu;0;L;1F59 0301;;;;N;;;;1F55;";
    EXPECT_EQ(canonical_composed(0x1F59, 0x0342), 0x1F5F) << "  Line: 1F5F;GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI;Lu;0;L;1F59 0342;;;;N;;;;1F57;";
    EXPECT_EQ(canonical_composed(0x03C9, 0x0313), 0x1F60) << "  Line: 1F60;GREEK SMALL LETTER OMEGA WITH PSILI;Ll;0;L;03C9 0313;;;;N;;;1F68;;1F68";
    EXPECT_EQ(canonical_composed(0x03C9, 0x0314), 0x1F61) << "  Line: 1F61;GREEK SMALL LETTER OMEGA WITH DASIA;Ll;0;L;03C9 0314;;;;N;;;1F69;;1F69";
    EXPECT_EQ(canonical_composed(0x1F60, 0x0300), 0x1F62) << "  Line: 1F62;GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA;Ll;0;L;1F60 0300;;;;N;;;1F6A;;1F6A";
    EXPECT_EQ(canonical_composed(0x1F61, 0x0300), 0x1F63) << "  Line: 1F63;GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA;Ll;0;L;1F61 0300;;;;N;;;1F6B;;1F6B";
    EXPECT_EQ(canonical_composed(0x1F60, 0x0301), 0x1F64) << "  Line: 1F64;GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA;Ll;0;L;1F60 0301;;;;N;;;1F6C;;1F6C";
    EXPECT_EQ(canonical_composed(0x1F61, 0x0301), 0x1F65) << "  Line: 1F65;GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA;Ll;0;L;1F61 0301;;;;N;;;1F6D;;1F6D";
    EXPECT_EQ(canonical_composed(0x1F60, 0x0342), 0x1F66) << "  Line: 1F66;GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI;Ll;0;L;1F60 0342;;;;N;;;1F6E;;1F6E";
    EXPECT_EQ(canonical_composed(0x1F61, 0x0342), 0x1F67) << "  Line: 1F67;GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI;Ll;0;L;1F61 0342;;;;N;;;1F6F;;1F6F";
    EXPECT_EQ(canonical_composed(0x03A9, 0x0313), 0x1F68) << "  Line: 1F68;GREEK CAPITAL LETTER OMEGA WITH PSILI;Lu;0;L;03A9 0313;;;;N;;;;1F60;";
    EXPECT_EQ(canonical_composed(0x03A9, 0x0314), 0x1F69) << "  Line: 1F69;GREEK CAPITAL LETTER OMEGA WITH DASIA;Lu;0;L;03A9 0314;;;;N;;;;1F61;";
    EXPECT_EQ(canonical_composed(0x1F68, 0x0300), 0x1F6A) << "  Line: 1F6A;GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA;Lu;0;L;1F68 0300;;;;N;;;;1F62;";
    EXPECT_EQ(canonical_composed(0x1F69, 0x0300), 0x1F6B) << "  Line: 1F6B;GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA;Lu;0;L;1F69 0300;;;;N;;;;1F63;";
    EXPECT_EQ(canonical_composed(0x1F68, 0x0301), 0x1F6C) << "  Line: 1F6C;GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA;Lu;0;L;1F68 0301;;;;N;;;;1F64;";
    EXPECT_EQ(canonical_composed(0x1F69, 0x0301), 0x1F6D) << "  Line: 1F6D;GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA;Lu;0;L;1F69 0301;;;;N;;;;1F65;";
    EXPECT_EQ(canonical_composed(0x1F68, 0x0342), 0x1F6E) << "  Line: 1F6E;GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI;Lu;0;L;1F68 0342;;;;N;;;;1F66;";
    EXPECT_EQ(canonical_composed(0x1F69, 0x0342), 0x1F6F) << "  Line: 1F6F;GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI;Lu;0;L;1F69 0342;;;;N;;;;1F67;";
    EXPECT_EQ(canonical_composed(0x03B1, 0x0300), 0x1F70) << "  Line: 1F70;GREEK SMALL LETTER ALPHA WITH VARIA;Ll;0;L;03B1 0300;;;;N;;;1FBA;;1FBA";
    EXPECT_EQ(canonical_composed(0x03B5, 0x0300), 0x1F72) << "  Line: 1F72;GREEK SMALL LETTER EPSILON WITH VARIA;Ll;0;L;03B5 0300;;;;N;;;1FC8;;1FC8";
    EXPECT_EQ(canonical_composed(0x03B7, 0x0300), 0x1F74) << "  Line: 1F74;GREEK SMALL LETTER ETA WITH VARIA;Ll;0;L;03B7 0300;;;;N;;;1FCA;;1FCA";
    EXPECT_EQ(canonical_composed(0x03B9, 0x0300), 0x1F76) << "  Line: 1F76;GREEK SMALL LETTER IOTA WITH VARIA;Ll;0;L;03B9 0300;;;;N;;;1FDA;;1FDA";
    EXPECT_EQ(canonical_composed(0x03BF, 0x0300), 0x1F78) << "  Line: 1F78;GREEK SMALL LETTER OMICRON WITH VARIA;Ll;0;L;03BF 0300;;;;N;;;1FF8;;1FF8";
    EXPECT_EQ(canonical_composed(0x03C5, 0x0300), 0x1F7A) << "  Line: 1F7A;GREEK SMALL LETTER UPSILON WITH VARIA;Ll;0;L;03C5 0300;;;;N;;;1FEA;;1FEA";
    EXPECT_EQ(canonical_composed(0x03C9, 0x0300), 0x1F7C) << "  Line: 1F7C;GREEK SMALL LETTER OMEGA WITH VARIA;Ll;0;L;03C9 0300;;;;N;;;1FFA;;1FFA";
    EXPECT_EQ(canonical_composed(0x1F00, 0x0345), 0x1F80) << "  Line: 1F80;GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI;Ll;0;L;1F00 0345;;;;N;;;1F88;;1F88";
    EXPECT_EQ(canonical_composed(0x1F01, 0x0345), 0x1F81) << "  Line: 1F81;GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI;Ll;0;L;1F01 0345;;;;N;;;1F89;;1F89";
    EXPECT_EQ(canonical_composed(0x1F02, 0x0345), 0x1F82) << "  Line: 1F82;GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F02 0345;;;;N;;;1F8A;;1F8A";
    EXPECT_EQ(canonical_composed(0x1F03, 0x0345), 0x1F83) << "  Line: 1F83;GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F03 0345;;;;N;;;1F8B;;1F8B";
    EXPECT_EQ(canonical_composed(0x1F04, 0x0345), 0x1F84) << "  Line: 1F84;GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F04 0345;;;;N;;;1F8C;;1F8C";
    EXPECT_EQ(canonical_composed(0x1F05, 0x0345), 0x1F85) << "  Line: 1F85;GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F05 0345;;;;N;;;1F8D;;1F8D";
    EXPECT_EQ(canonical_composed(0x1F06, 0x0345), 0x1F86) << "  Line: 1F86;GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F06 0345;;;;N;;;1F8E;;1F8E";
    EXPECT_EQ(canonical_composed(0x1F07, 0x0345), 0x1F87) << "  Line: 1F87;GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F07 0345;;;;N;;;1F8F;;1F8F";
    EXPECT_EQ(canonical_composed(0x1F08, 0x0345), 0x1F88) << "  Line: 1F88;GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI;Lt;0;L;1F08 0345;;;;N;;;;1F80;";
    EXPECT_EQ(canonical_composed(0x1F09, 0x0345), 0x1F89) << "  Line: 1F89;GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI;Lt;0;L;1F09 0345;;;;N;;;;1F81;";
    EXPECT_EQ(canonical_composed(0x1F0A, 0x0345), 0x1F8A) << "  Line: 1F8A;GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F0A 0345;;;;N;;;;1F82;";
    EXPECT_EQ(canonical_composed(0x1F0B, 0x0345), 0x1F8B) << "  Line: 1F8B;GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F0B 0345;;;;N;;;;1F83;";
    EXPECT_EQ(canonical_composed(0x1F0C, 0x0345), 0x1F8C) << "  Line: 1F8C;GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F0C 0345;;;;N;;;;1F84;";
    EXPECT_EQ(canonical_composed(0x1F0D, 0x0345), 0x1F8D) << "  Line: 1F8D;GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F0D 0345;;;;N;;;;1F85;";
    EXPECT_EQ(canonical_composed(0x1F0E, 0x0345), 0x1F8E) << "  Line: 1F8E;GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F0E 0345;;;;N;;;;1F86;";
    EXPECT_EQ(canonical_composed(0x1F0F, 0x0345), 0x1F8F) << "  Line: 1F8F;GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F0F 0345;;;;N;;;;1F87;";
    EXPECT_EQ(canonical_composed(0x1F20, 0x0345), 0x1F90) << "  Line: 1F90;GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI;Ll;0;L;1F20 0345;;;;N;;;1F98;;1F98";
    EXPECT_EQ(canonical_composed(0x1F21, 0x0345), 0x1F91) << "  Line: 1F91;GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI;Ll;0;L;1F21 0345;;;;N;;;1F99;;1F99";
    EXPECT_EQ(canonical_composed(0x1F22, 0x0345), 0x1F92) << "  Line: 1F92;GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F22 0345;;;;N;;;1F9A;;1F9A";
    EXPECT_EQ(canonical_composed(0x1F23, 0x0345), 0x1F93) << "  Line: 1F93;GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F23 0345;;;;N;;;1F9B;;1F9B";
    EXPECT_EQ(canonical_composed(0x1F24, 0x0345), 0x1F94) << "  Line: 1F94;GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F24 0345;;;;N;;;1F9C;;1F9C";
    EXPECT_EQ(canonical_composed(0x1F25, 0x0345), 0x1F95) << "  Line: 1F95;GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F25 0345;;;;N;;;1F9D;;1F9D";
    EXPECT_EQ(canonical_composed(0x1F26, 0x0345), 0x1F96) << "  Line: 1F96;GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F26 0345;;;;N;;;1F9E;;1F9E";
    EXPECT_EQ(canonical_composed(0x1F27, 0x0345), 0x1F97) << "  Line: 1F97;GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F27 0345;;;;N;;;1F9F;;1F9F";
    EXPECT_EQ(canonical_composed(0x1F28, 0x0345), 0x1F98) << "  Line: 1F98;GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI;Lt;0;L;1F28 0345;;;;N;;;;1F90;";
    EXPECT_EQ(canonical_composed(0x1F29, 0x0345), 0x1F99) << "  Line: 1F99;GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI;Lt;0;L;1F29 0345;;;;N;;;;1F91;";
    EXPECT_EQ(canonical_composed(0x1F2A, 0x0345), 0x1F9A) << "  Line: 1F9A;GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F2A 0345;;;;N;;;;1F92;";
    EXPECT_EQ(canonical_composed(0x1F2B, 0x0345), 0x1F9B) << "  Line: 1F9B;GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F2B 0345;;;;N;;;;1F93;";
    EXPECT_EQ(canonical_composed(0x1F2C, 0x0345), 0x1F9C) << "  Line: 1F9C;GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F2C 0345;;;;N;;;;1F94;";
    EXPECT_EQ(canonical_composed(0x1F2D, 0x0345), 0x1F9D) << "  Line: 1F9D;GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F2D 0345;;;;N;;;;1F95;";
    EXPECT_EQ(canonical_composed(0x1F2E, 0x0345), 0x1F9E) << "  Line: 1F9E;GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F2E 0345;;;;N;;;;1F96;";
    EXPECT_EQ(canonical_composed(0x1F2F, 0x0345), 0x1F9F) << "  Line: 1F9F;GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F2F 0345;;;;N;;;;1F97;";
    EXPECT_EQ(canonical_composed(0x1F60, 0x0345), 0x1FA0) << "  Line: 1FA0;GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI;Ll;0;L;1F60 0345;;;;N;;;1FA8;;1FA8";
    EXPECT_EQ(canonical_composed(0x1F61, 0x0345), 0x1FA1) << "  Line: 1FA1;GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI;Ll;0;L;1F61 0345;;;;N;;;1FA9;;1FA9";
    EXPECT_EQ(canonical_composed(0x1F62, 0x0345), 0x1FA2) << "  Line: 1FA2;GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F62 0345;;;;N;;;1FAA;;1FAA";
    EXPECT_EQ(canonical_composed(0x1F63, 0x0345), 0x1FA3) << "  Line: 1FA3;GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI;Ll;0;L;1F63 0345;;;;N;;;1FAB;;1FAB";
    EXPECT_EQ(canonical_composed(0x1F64, 0x0345), 0x1FA4) << "  Line: 1FA4;GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F64 0345;;;;N;;;1FAC;;1FAC";
    EXPECT_EQ(canonical_composed(0x1F65, 0x0345), 0x1FA5) << "  Line: 1FA5;GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI;Ll;0;L;1F65 0345;;;;N;;;1FAD;;1FAD";
    EXPECT_EQ(canonical_composed(0x1F66, 0x0345), 0x1FA6) << "  Line: 1FA6;GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F66 0345;;;;N;;;1FAE;;1FAE";
    EXPECT_EQ(canonical_composed(0x1F67, 0x0345), 0x1FA7) << "  Line: 1FA7;GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1F67 0345;;;;N;;;1FAF;;1FAF";
    EXPECT_EQ(canonical_composed(0x1F68, 0x0345), 0x1FA8) << "  Line: 1FA8;GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI;Lt;0;L;1F68 0345;;;;N;;;;1FA0;";
    EXPECT_EQ(canonical_composed(0x1F69, 0x0345), 0x1FA9) << "  Line: 1FA9;GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI;Lt;0;L;1F69 0345;;;;N;;;;1FA1;";
    EXPECT_EQ(canonical_composed(0x1F6A, 0x0345), 0x1FAA) << "  Line: 1FAA;GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F6A 0345;;;;N;;;;1FA2;";
    EXPECT_EQ(canonical_composed(0x1F6B, 0x0345), 0x1FAB) << "  Line: 1FAB;GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI;Lt;0;L;1F6B 0345;;;;N;;;;1FA3;";
    EXPECT_EQ(canonical_composed(0x1F6C, 0x0345), 0x1FAC) << "  Line: 1FAC;GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F6C 0345;;;;N;;;;1FA4;";
    EXPECT_EQ(canonical_composed(0x1F6D, 0x0345), 0x1FAD) << "  Line: 1FAD;GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI;Lt;0;L;1F6D 0345;;;;N;;;;1FA5;";
    EXPECT_EQ(canonical_composed(0x1F6E, 0x0345), 0x1FAE) << "  Line: 1FAE;GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F6E 0345;;;;N;;;;1FA6;";
    EXPECT_EQ(canonical_composed(0x1F6F, 0x0345), 0x1FAF) << "  Line: 1FAF;GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI;Lt;0;L;1F6F 0345;;;;N;;;;1FA7;";
    EXPECT_EQ(canonical_composed(0x03B1, 0x0306), 0x1FB0) << "  Line: 1FB0;GREEK SMALL LETTER ALPHA WITH VRACHY;Ll;0;L;03B1 0306;;;;N;;;1FB8;;1FB8";
    EXPECT_EQ(canonical_composed(0x03B1, 0x0304), 0x1FB1) << "  Line: 1FB1;GREEK SMALL LETTER ALPHA WITH MACRON;Ll;0;L;03B1 0304;;;;N;;;1FB9;;1FB9";
    EXPECT_EQ(canonical_composed(0x1F70, 0x0345), 0x1FB2) << "  Line: 1FB2;GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI;Ll;0;L;1F70 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03B1, 0x0345), 0x1FB3) << "  Line: 1FB3;GREEK SMALL LETTER ALPHA WITH YPOGEGRAMMENI;Ll;0;L;03B1 0345;;;;N;;;1FBC;;1FBC";
    EXPECT_EQ(canonical_composed(0x03AC, 0x0345), 0x1FB4) << "  Line: 1FB4;GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI;Ll;0;L;03AC 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03B1, 0x0342), 0x1FB6) << "  Line: 1FB6;GREEK SMALL LETTER ALPHA WITH PERISPOMENI;Ll;0;L;03B1 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1FB6, 0x0345), 0x1FB7) << "  Line: 1FB7;GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1FB6 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0391, 0x0306), 0x1FB8) << "  Line: 1FB8;GREEK CAPITAL LETTER ALPHA WITH VRACHY;Lu;0;L;0391 0306;;;;N;;;;1FB0;";
    EXPECT_EQ(canonical_composed(0x0391, 0x0304), 0x1FB9) << "  Line: 1FB9;GREEK CAPITAL LETTER ALPHA WITH MACRON;Lu;0;L;0391 0304;;;;N;;;;1FB1;";
    EXPECT_EQ(canonical_composed(0x0391, 0x0300), 0x1FBA) << "  Line: 1FBA;GREEK CAPITAL LETTER ALPHA WITH VARIA;Lu;0;L;0391 0300;;;;N;;;;1F70;";
    EXPECT_EQ(canonical_composed(0x0391, 0x0345), 0x1FBC) << "  Line: 1FBC;GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI;Lt;0;L;0391 0345;;;;N;;;;1FB3;";
    EXPECT_EQ(canonical_composed(0x00A8, 0x0342), 0x1FC1) << "  Line: 1FC1;GREEK DIALYTIKA AND PERISPOMENI;Sk;0;ON;00A8 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1F74, 0x0345), 0x1FC2) << "  Line: 1FC2;GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI;Ll;0;L;1F74 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03B7, 0x0345), 0x1FC3) << "  Line: 1FC3;GREEK SMALL LETTER ETA WITH YPOGEGRAMMENI;Ll;0;L;03B7 0345;;;;N;;;1FCC;;1FCC";
    EXPECT_EQ(canonical_composed(0x03AE, 0x0345), 0x1FC4) << "  Line: 1FC4;GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI;Ll;0;L;03AE 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03B7, 0x0342), 0x1FC6) << "  Line: 1FC6;GREEK SMALL LETTER ETA WITH PERISPOMENI;Ll;0;L;03B7 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1FC6, 0x0345), 0x1FC7) << "  Line: 1FC7;GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1FC6 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0395, 0x0300), 0x1FC8) << "  Line: 1FC8;GREEK CAPITAL LETTER EPSILON WITH VARIA;Lu;0;L;0395 0300;;;;N;;;;1F72;";
    EXPECT_EQ(canonical_composed(0x0397, 0x0300), 0x1FCA) << "  Line: 1FCA;GREEK CAPITAL LETTER ETA WITH VARIA;Lu;0;L;0397 0300;;;;N;;;;1F74;";
    EXPECT_EQ(canonical_composed(0x0397, 0x0345), 0x1FCC) << "  Line: 1FCC;GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI;Lt;0;L;0397 0345;;;;N;;;;1FC3;";
    EXPECT_EQ(canonical_composed(0x1FBF, 0x0300), 0x1FCD) << "  Line: 1FCD;GREEK PSILI AND VARIA;Sk;0;ON;1FBF 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1FBF, 0x0301), 0x1FCE) << "  Line: 1FCE;GREEK PSILI AND OXIA;Sk;0;ON;1FBF 0301;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1FBF, 0x0342), 0x1FCF) << "  Line: 1FCF;GREEK PSILI AND PERISPOMENI;Sk;0;ON;1FBF 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03B9, 0x0306), 0x1FD0) << "  Line: 1FD0;GREEK SMALL LETTER IOTA WITH VRACHY;Ll;0;L;03B9 0306;;;;N;;;1FD8;;1FD8";
    EXPECT_EQ(canonical_composed(0x03B9, 0x0304), 0x1FD1) << "  Line: 1FD1;GREEK SMALL LETTER IOTA WITH MACRON;Ll;0;L;03B9 0304;;;;N;;;1FD9;;1FD9";
    EXPECT_EQ(canonical_composed(0x03CA, 0x0300), 0x1FD2) << "  Line: 1FD2;GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA;Ll;0;L;03CA 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03B9, 0x0342), 0x1FD6) << "  Line: 1FD6;GREEK SMALL LETTER IOTA WITH PERISPOMENI;Ll;0;L;03B9 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03CA, 0x0342), 0x1FD7) << "  Line: 1FD7;GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI;Ll;0;L;03CA 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x0399, 0x0306), 0x1FD8) << "  Line: 1FD8;GREEK CAPITAL LETTER IOTA WITH VRACHY;Lu;0;L;0399 0306;;;;N;;;;1FD0;";
    EXPECT_EQ(canonical_composed(0x0399, 0x0304), 0x1FD9) << "  Line: 1FD9;GREEK CAPITAL LETTER IOTA WITH MACRON;Lu;0;L;0399 0304;;;;N;;;;1FD1;";
    EXPECT_EQ(canonical_composed(0x0399, 0x0300), 0x1FDA) << "  Line: 1FDA;GREEK CAPITAL LETTER IOTA WITH VARIA;Lu;0;L;0399 0300;;;;N;;;;1F76;";
    EXPECT_EQ(canonical_composed(0x1FFE, 0x0300), 0x1FDD) << "  Line: 1FDD;GREEK DASIA AND VARIA;Sk;0;ON;1FFE 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1FFE, 0x0301), 0x1FDE) << "  Line: 1FDE;GREEK DASIA AND OXIA;Sk;0;ON;1FFE 0301;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1FFE, 0x0342), 0x1FDF) << "  Line: 1FDF;GREEK DASIA AND PERISPOMENI;Sk;0;ON;1FFE 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03C5, 0x0306), 0x1FE0) << "  Line: 1FE0;GREEK SMALL LETTER UPSILON WITH VRACHY;Ll;0;L;03C5 0306;;;;N;;;1FE8;;1FE8";
    EXPECT_EQ(canonical_composed(0x03C5, 0x0304), 0x1FE1) << "  Line: 1FE1;GREEK SMALL LETTER UPSILON WITH MACRON;Ll;0;L;03C5 0304;;;;N;;;1FE9;;1FE9";
    EXPECT_EQ(canonical_composed(0x03CB, 0x0300), 0x1FE2) << "  Line: 1FE2;GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA;Ll;0;L;03CB 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03C1, 0x0313), 0x1FE4) << "  Line: 1FE4;GREEK SMALL LETTER RHO WITH PSILI;Ll;0;L;03C1 0313;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03C1, 0x0314), 0x1FE5) << "  Line: 1FE5;GREEK SMALL LETTER RHO WITH DASIA;Ll;0;L;03C1 0314;;;;N;;;1FEC;;1FEC";
    EXPECT_EQ(canonical_composed(0x03C5, 0x0342), 0x1FE6) << "  Line: 1FE6;GREEK SMALL LETTER UPSILON WITH PERISPOMENI;Ll;0;L;03C5 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03CB, 0x0342), 0x1FE7) << "  Line: 1FE7;GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI;Ll;0;L;03CB 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03A5, 0x0306), 0x1FE8) << "  Line: 1FE8;GREEK CAPITAL LETTER UPSILON WITH VRACHY;Lu;0;L;03A5 0306;;;;N;;;;1FE0;";
    EXPECT_EQ(canonical_composed(0x03A5, 0x0304), 0x1FE9) << "  Line: 1FE9;GREEK CAPITAL LETTER UPSILON WITH MACRON;Lu;0;L;03A5 0304;;;;N;;;;1FE1;";
    EXPECT_EQ(canonical_composed(0x03A5, 0x0300), 0x1FEA) << "  Line: 1FEA;GREEK CAPITAL LETTER UPSILON WITH VARIA;Lu;0;L;03A5 0300;;;;N;;;;1F7A;";
    EXPECT_EQ(canonical_composed(0x03A1, 0x0314), 0x1FEC) << "  Line: 1FEC;GREEK CAPITAL LETTER RHO WITH DASIA;Lu;0;L;03A1 0314;;;;N;;;;1FE5;";
    EXPECT_EQ(canonical_composed(0x00A8, 0x0300), 0x1FED) << "  Line: 1FED;GREEK DIALYTIKA AND VARIA;Sk;0;ON;00A8 0300;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1F7C, 0x0345), 0x1FF2) << "  Line: 1FF2;GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI;Ll;0;L;1F7C 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03C9, 0x0345), 0x1FF3) << "  Line: 1FF3;GREEK SMALL LETTER OMEGA WITH YPOGEGRAMMENI;Ll;0;L;03C9 0345;;;;N;;;1FFC;;1FFC";
    EXPECT_EQ(canonical_composed(0x03CE, 0x0345), 0x1FF4) << "  Line: 1FF4;GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI;Ll;0;L;03CE 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x03C9, 0x0342), 0x1FF6) << "  Line: 1FF6;GREEK SMALL LETTER OMEGA WITH PERISPOMENI;Ll;0;L;03C9 0342;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1FF6, 0x0345), 0x1FF7) << "  Line: 1FF7;GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI;Ll;0;L;1FF6 0345;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x039F, 0x0300), 0x1FF8) << "  Line: 1FF8;GREEK CAPITAL LETTER OMICRON WITH VARIA;Lu;0;L;039F 0300;;;;N;;;;1F78;";
    EXPECT_EQ(canonical_composed(0x03A9, 0x0300), 0x1FFA) << "  Line: 1FFA;GREEK CAPITAL LETTER OMEGA WITH VARIA;Lu;0;L;03A9 0300;;;;N;;;;1F7C;";
    EXPECT_EQ(canonical_composed(0x03A9, 0x0345), 0x1FFC) << "  Line: 1FFC;GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI;Lt;0;L;03A9 0345;;;;N;;;;1FF3;";
    EXPECT_EQ(canonical_composed(0x2190, 0x0338), 0x219A) << "  Line: 219A;LEFTWARDS ARROW WITH STROKE;Sm;0;ON;2190 0338;;;;N;LEFT ARROW WITH STROKE;;;;";
    EXPECT_EQ(canonical_composed(0x2192, 0x0338), 0x219B) << "  Line: 219B;RIGHTWARDS ARROW WITH STROKE;Sm;0;ON;2192 0338;;;;N;RIGHT ARROW WITH STROKE;;;;";
    EXPECT_EQ(canonical_composed(0x2194, 0x0338), 0x21AE) << "  Line: 21AE;LEFT RIGHT ARROW WITH STROKE;Sm;0;ON;2194 0338;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x21D0, 0x0338), 0x21CD) << "  Line: 21CD;LEFTWARDS DOUBLE ARROW WITH STROKE;So;0;ON;21D0 0338;;;;N;LEFT DOUBLE ARROW WITH STROKE;;;;";
    EXPECT_EQ(canonical_composed(0x21D4, 0x0338), 0x21CE) << "  Line: 21CE;LEFT RIGHT DOUBLE ARROW WITH STROKE;Sm;0;ON;21D4 0338;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x21D2, 0x0338), 0x21CF) << "  Line: 21CF;RIGHTWARDS DOUBLE ARROW WITH STROKE;Sm;0;ON;21D2 0338;;;;N;RIGHT DOUBLE ARROW WITH STROKE;;;;";
    EXPECT_EQ(canonical_composed(0x2203, 0x0338), 0x2204) << "  Line: 2204;THERE DOES NOT EXIST;Sm;0;ON;2203 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2208, 0x0338), 0x2209) << "  Line: 2209;NOT AN ELEMENT OF;Sm;0;ON;2208 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x220B, 0x0338), 0x220C) << "  Line: 220C;DOES NOT CONTAIN AS MEMBER;Sm;0;ON;220B 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2223, 0x0338), 0x2224) << "  Line: 2224;DOES NOT DIVIDE;Sm;0;ON;2223 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2225, 0x0338), 0x2226) << "  Line: 2226;NOT PARALLEL TO;Sm;0;ON;2225 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x223C, 0x0338), 0x2241) << "  Line: 2241;NOT TILDE;Sm;0;ON;223C 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2243, 0x0338), 0x2244) << "  Line: 2244;NOT ASYMPTOTICALLY EQUAL TO;Sm;0;ON;2243 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2245, 0x0338), 0x2247) << "  Line: 2247;NEITHER APPROXIMATELY NOR ACTUALLY EQUAL TO;Sm;0;ON;2245 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2248, 0x0338), 0x2249) << "  Line: 2249;NOT ALMOST EQUAL TO;Sm;0;ON;2248 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x003D, 0x0338), 0x2260) << "  Line: 2260;NOT EQUAL TO;Sm;0;ON;003D 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2261, 0x0338), 0x2262) << "  Line: 2262;NOT IDENTICAL TO;Sm;0;ON;2261 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x224D, 0x0338), 0x226D) << "  Line: 226D;NOT EQUIVALENT TO;Sm;0;ON;224D 0338;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x003C, 0x0338), 0x226E) << "  Line: 226E;NOT LESS-THAN;Sm;0;ON;003C 0338;;;;Y;NOT LESS THAN;;;;";
    EXPECT_EQ(canonical_composed(0x003E, 0x0338), 0x226F) << "  Line: 226F;NOT GREATER-THAN;Sm;0;ON;003E 0338;;;;Y;NOT GREATER THAN;;;;";
    EXPECT_EQ(canonical_composed(0x2264, 0x0338), 0x2270) << "  Line: 2270;NEITHER LESS-THAN NOR EQUAL TO;Sm;0;ON;2264 0338;;;;Y;NEITHER LESS THAN NOR EQUAL TO;;;;";
    EXPECT_EQ(canonical_composed(0x2265, 0x0338), 0x2271) << "  Line: 2271;NEITHER GREATER-THAN NOR EQUAL TO;Sm;0;ON;2265 0338;;;;Y;NEITHER GREATER THAN NOR EQUAL TO;;;;";
    EXPECT_EQ(canonical_composed(0x2272, 0x0338), 0x2274) << "  Line: 2274;NEITHER LESS-THAN NOR EQUIVALENT TO;Sm;0;ON;2272 0338;;;;Y;NEITHER LESS THAN NOR EQUIVALENT TO;;;;";
    EXPECT_EQ(canonical_composed(0x2273, 0x0338), 0x2275) << "  Line: 2275;NEITHER GREATER-THAN NOR EQUIVALENT TO;Sm;0;ON;2273 0338;;;;Y;NEITHER GREATER THAN NOR EQUIVALENT TO;;;;";
    EXPECT_EQ(canonical_composed(0x2276, 0x0338), 0x2278) << "  Line: 2278;NEITHER LESS-THAN NOR GREATER-THAN;Sm;0;ON;2276 0338;;;;Y;NEITHER LESS THAN NOR GREATER THAN;;;;";
    EXPECT_EQ(canonical_composed(0x2277, 0x0338), 0x2279) << "  Line: 2279;NEITHER GREATER-THAN NOR LESS-THAN;Sm;0;ON;2277 0338;;;;Y;NEITHER GREATER THAN NOR LESS THAN;;;;";
    EXPECT_EQ(canonical_composed(0x227A, 0x0338), 0x2280) << "  Line: 2280;DOES NOT PRECEDE;Sm;0;ON;227A 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x227B, 0x0338), 0x2281) << "  Line: 2281;DOES NOT SUCCEED;Sm;0;ON;227B 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2282, 0x0338), 0x2284) << "  Line: 2284;NOT A SUBSET OF;Sm;0;ON;2282 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2283, 0x0338), 0x2285) << "  Line: 2285;NOT A SUPERSET OF;Sm;0;ON;2283 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2286, 0x0338), 0x2288) << "  Line: 2288;NEITHER A SUBSET OF NOR EQUAL TO;Sm;0;ON;2286 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2287, 0x0338), 0x2289) << "  Line: 2289;NEITHER A SUPERSET OF NOR EQUAL TO;Sm;0;ON;2287 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x22A2, 0x0338), 0x22AC) << "  Line: 22AC;DOES NOT PROVE;Sm;0;ON;22A2 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x22A8, 0x0338), 0x22AD) << "  Line: 22AD;NOT TRUE;Sm;0;ON;22A8 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x22A9, 0x0338), 0x22AE) << "  Line: 22AE;DOES NOT FORCE;Sm;0;ON;22A9 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x22AB, 0x0338), 0x22AF) << "  Line: 22AF;NEGATED DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE;Sm;0;ON;22AB 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x227C, 0x0338), 0x22E0) << "  Line: 22E0;DOES NOT PRECEDE OR EQUAL;Sm;0;ON;227C 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x227D, 0x0338), 0x22E1) << "  Line: 22E1;DOES NOT SUCCEED OR EQUAL;Sm;0;ON;227D 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2291, 0x0338), 0x22E2) << "  Line: 22E2;NOT SQUARE IMAGE OF OR EQUAL TO;Sm;0;ON;2291 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x2292, 0x0338), 0x22E3) << "  Line: 22E3;NOT SQUARE ORIGINAL OF OR EQUAL TO;Sm;0;ON;2292 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x22B2, 0x0338), 0x22EA) << "  Line: 22EA;NOT NORMAL SUBGROUP OF;Sm;0;ON;22B2 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x22B3, 0x0338), 0x22EB) << "  Line: 22EB;DOES NOT CONTAIN AS NORMAL SUBGROUP;Sm;0;ON;22B3 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x22B4, 0x0338), 0x22EC) << "  Line: 22EC;NOT NORMAL SUBGROUP OF OR EQUAL TO;Sm;0;ON;22B4 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x22B5, 0x0338), 0x22ED) << "  Line: 22ED;DOES NOT CONTAIN AS NORMAL SUBGROUP OR EQUAL;Sm;0;ON;22B5 0338;;;;Y;;;;;";
    EXPECT_EQ(canonical_composed(0x304B, 0x3099), 0x304C) << "  Line: 304C;HIRAGANA LETTER GA;Lo;0;L;304B 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x304D, 0x3099), 0x304E) << "  Line: 304E;HIRAGANA LETTER GI;Lo;0;L;304D 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x304F, 0x3099), 0x3050) << "  Line: 3050;HIRAGANA LETTER GU;Lo;0;L;304F 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3051, 0x3099), 0x3052) << "  Line: 3052;HIRAGANA LETTER GE;Lo;0;L;3051 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3053, 0x3099), 0x3054) << "  Line: 3054;HIRAGANA LETTER GO;Lo;0;L;3053 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3055, 0x3099), 0x3056) << "  Line: 3056;HIRAGANA LETTER ZA;Lo;0;L;3055 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3057, 0x3099), 0x3058) << "  Line: 3058;HIRAGANA LETTER ZI;Lo;0;L;3057 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3059, 0x3099), 0x305A) << "  Line: 305A;HIRAGANA LETTER ZU;Lo;0;L;3059 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x305B, 0x3099), 0x305C) << "  Line: 305C;HIRAGANA LETTER ZE;Lo;0;L;305B 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x305D, 0x3099), 0x305E) << "  Line: 305E;HIRAGANA LETTER ZO;Lo;0;L;305D 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x305F, 0x3099), 0x3060) << "  Line: 3060;HIRAGANA LETTER DA;Lo;0;L;305F 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3061, 0x3099), 0x3062) << "  Line: 3062;HIRAGANA LETTER DI;Lo;0;L;3061 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3064, 0x3099), 0x3065) << "  Line: 3065;HIRAGANA LETTER DU;Lo;0;L;3064 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3066, 0x3099), 0x3067) << "  Line: 3067;HIRAGANA LETTER DE;Lo;0;L;3066 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3068, 0x3099), 0x3069) << "  Line: 3069;HIRAGANA LETTER DO;Lo;0;L;3068 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x306F, 0x3099), 0x3070) << "  Line: 3070;HIRAGANA LETTER BA;Lo;0;L;306F 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x306F, 0x309A), 0x3071) << "  Line: 3071;HIRAGANA LETTER PA;Lo;0;L;306F 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3072, 0x3099), 0x3073) << "  Line: 3073;HIRAGANA LETTER BI;Lo;0;L;3072 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3072, 0x309A), 0x3074) << "  Line: 3074;HIRAGANA LETTER PI;Lo;0;L;3072 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3075, 0x3099), 0x3076) << "  Line: 3076;HIRAGANA LETTER BU;Lo;0;L;3075 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3075, 0x309A), 0x3077) << "  Line: 3077;HIRAGANA LETTER PU;Lo;0;L;3075 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3078, 0x3099), 0x3079) << "  Line: 3079;HIRAGANA LETTER BE;Lo;0;L;3078 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3078, 0x309A), 0x307A) << "  Line: 307A;HIRAGANA LETTER PE;Lo;0;L;3078 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x307B, 0x3099), 0x307C) << "  Line: 307C;HIRAGANA LETTER BO;Lo;0;L;307B 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x307B, 0x309A), 0x307D) << "  Line: 307D;HIRAGANA LETTER PO;Lo;0;L;307B 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x3046, 0x3099), 0x3094) << "  Line: 3094;HIRAGANA LETTER VU;Lo;0;L;3046 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x309D, 0x3099), 0x309E) << "  Line: 309E;HIRAGANA VOICED ITERATION MARK;Lm;0;L;309D 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30AB, 0x3099), 0x30AC) << "  Line: 30AC;KATAKANA LETTER GA;Lo;0;L;30AB 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30AD, 0x3099), 0x30AE) << "  Line: 30AE;KATAKANA LETTER GI;Lo;0;L;30AD 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30AF, 0x3099), 0x30B0) << "  Line: 30B0;KATAKANA LETTER GU;Lo;0;L;30AF 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30B1, 0x3099), 0x30B2) << "  Line: 30B2;KATAKANA LETTER GE;Lo;0;L;30B1 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30B3, 0x3099), 0x30B4) << "  Line: 30B4;KATAKANA LETTER GO;Lo;0;L;30B3 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30B5, 0x3099), 0x30B6) << "  Line: 30B6;KATAKANA LETTER ZA;Lo;0;L;30B5 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30B7, 0x3099), 0x30B8) << "  Line: 30B8;KATAKANA LETTER ZI;Lo;0;L;30B7 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30B9, 0x3099), 0x30BA) << "  Line: 30BA;KATAKANA LETTER ZU;Lo;0;L;30B9 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30BB, 0x3099), 0x30BC) << "  Line: 30BC;KATAKANA LETTER ZE;Lo;0;L;30BB 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30BD, 0x3099), 0x30BE) << "  Line: 30BE;KATAKANA LETTER ZO;Lo;0;L;30BD 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30BF, 0x3099), 0x30C0) << "  Line: 30C0;KATAKANA LETTER DA;Lo;0;L;30BF 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30C1, 0x3099), 0x30C2) << "  Line: 30C2;KATAKANA LETTER DI;Lo;0;L;30C1 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30C4, 0x3099), 0x30C5) << "  Line: 30C5;KATAKANA LETTER DU;Lo;0;L;30C4 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30C6, 0x3099), 0x30C7) << "  Line: 30C7;KATAKANA LETTER DE;Lo;0;L;30C6 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30C8, 0x3099), 0x30C9) << "  Line: 30C9;KATAKANA LETTER DO;Lo;0;L;30C8 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30CF, 0x3099), 0x30D0) << "  Line: 30D0;KATAKANA LETTER BA;Lo;0;L;30CF 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30CF, 0x309A), 0x30D1) << "  Line: 30D1;KATAKANA LETTER PA;Lo;0;L;30CF 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30D2, 0x3099), 0x30D3) << "  Line: 30D3;KATAKANA LETTER BI;Lo;0;L;30D2 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30D2, 0x309A), 0x30D4) << "  Line: 30D4;KATAKANA LETTER PI;Lo;0;L;30D2 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30D5, 0x3099), 0x30D6) << "  Line: 30D6;KATAKANA LETTER BU;Lo;0;L;30D5 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30D5, 0x309A), 0x30D7) << "  Line: 30D7;KATAKANA LETTER PU;Lo;0;L;30D5 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30D8, 0x3099), 0x30D9) << "  Line: 30D9;KATAKANA LETTER BE;Lo;0;L;30D8 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30D8, 0x309A), 0x30DA) << "  Line: 30DA;KATAKANA LETTER PE;Lo;0;L;30D8 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30DB, 0x3099), 0x30DC) << "  Line: 30DC;KATAKANA LETTER BO;Lo;0;L;30DB 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30DB, 0x309A), 0x30DD) << "  Line: 30DD;KATAKANA LETTER PO;Lo;0;L;30DB 309A;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30A6, 0x3099), 0x30F4) << "  Line: 30F4;KATAKANA LETTER VU;Lo;0;L;30A6 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30EF, 0x3099), 0x30F7) << "  Line: 30F7;KATAKANA LETTER VA;Lo;0;L;30EF 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30F0, 0x3099), 0x30F8) << "  Line: 30F8;KATAKANA LETTER VI;Lo;0;L;30F0 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30F1, 0x3099), 0x30F9) << "  Line: 30F9;KATAKANA LETTER VE;Lo;0;L;30F1 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30F2, 0x3099), 0x30FA) << "  Line: 30FA;KATAKANA LETTER VO;Lo;0;L;30F2 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x30FD, 0x3099), 0x30FE) << "  Line: 30FE;KATAKANA VOICED ITERATION MARK;Lm;0;L;30FD 3099;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x11099, 0x110BA), 0x1109A) << "  Line: 1109A;KAITHI LETTER DDDHA;Lo;0;L;11099 110BA;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x1109B, 0x110BA), 0x1109C) << "  Line: 1109C;KAITHI LETTER RHA;Lo;0;L;1109B 110BA;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x110A5, 0x110BA), 0x110AB) << "  Line: 110AB;KAITHI LETTER VA;Lo;0;L;110A5 110BA;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x11131, 0x11127), 0x1112E) << "  Line: 1112E;CHAKMA VOWEL SIGN O;Mn;0;NSM;11131 11127;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x11132, 0x11127), 0x1112F) << "  Line: 1112F;CHAKMA VOWEL SIGN AU;Mn;0;NSM;11132 11127;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x11347, 0x1133E), 0x1134B) << "  Line: 1134B;GRANTHA VOWEL SIGN OO;Mc;0;L;11347 1133E;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x11347, 0x11357), 0x1134C) << "  Line: 1134C;GRANTHA VOWEL SIGN AU;Mc;0;L;11347 11357;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x114B9, 0x114BA), 0x114BB) << "  Line: 114BB;TIRHUTA VOWEL SIGN AI;Mc;0;L;114B9 114BA;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x114B9, 0x114B0), 0x114BC) << "  Line: 114BC;TIRHUTA VOWEL SIGN O;Mc;0;L;114B9 114B0;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x114B9, 0x114BD), 0x114BE) << "  Line: 114BE;TIRHUTA VOWEL SIGN AU;Mc;0;L;114B9 114BD;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x115B8, 0x115AF), 0x115BA) << "  Line: 115BA;SIDDHAM VOWEL SIGN O;Mc;0;L;115B8 115AF;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x115B9, 0x115AF), 0x115BB) << "  Line: 115BB;SIDDHAM VOWEL SIGN AU;Mc;0;L;115B9 115AF;;;;N;;;;;";
    EXPECT_EQ(canonical_composed(0x11935, 0x11930), 0x11938) << "  Line: 11938;DIVES AKURU VOWEL SIGN O;Mc;0;L;11935 11930;;;;N;;;;;";
    // clang-format on
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

    int test_index = 0;

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

    std::string u32ToString(std::u8string const& hexString) {
        std::ostringstream oss;
        for (auto const codePoint : hexString) {
            oss << "\\x" << std::hex << static_cast<std::uint32_t>(codePoint);
        }
        return oss.str();
    }

    std::string u32ToString(std::string const& hexString) {
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
          << "\n  NFC Answer: " << u32ToString(nfc) << "\n  NFD Answer: " << u32ToString(nfd)
          << "\n  index: " << test_index;
        EXPECT_EQ(toNFC(str), toNFC(toNFD(str)))
          << "  NFD: " << u32ToString(toNFD(str)) << "\n  Source: " << u32ToString(str) << "\n  NFC Answer: "
          << u32ToString(nfc) << "\n  NFD Answer: " << u32ToString(nfd) << "\n  index: " << test_index;

        // toNFD
        EXPECT_EQ(toNFD(str), toNFD(toNFC(str)))
          << "  NFC: " << u32ToString(toNFC(str)) << "\n  Source: " << u32ToString(str) << "\n  NFC Answer: "
          << u32ToString(nfc) << "\n  NFD Answer: " << u32ToString(nfd) << "\n  index: " << test_index;
        EXPECT_EQ(toNFD(str), toNFD(toNFD(str)))
          << "  Src: " << u32ToString(str) << "\n  NFD Layer 1: " << u32ToString(toNFD(str))
          << "\n  NFC Answer: " << u32ToString(nfc) << "\n  NFD Answer: " << u32ToString(nfd)
          << "\n  index: " << test_index;

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

    std::string report_composition_list(std::u32string_view str) {
        std::ostringstream    report;
        std::vector<char32_t> comps;
        for (auto const lhs : str) {
            for (auto const rhs : str) {
                auto const comp = webpp::unicode::canonical_composed(lhs, rhs);
                if (comp == webpp::unicode::replacement_char<char32_t>) {
                    continue;
                }
                report << "\n  Compositions: " << std::hex << std::uppercase << "0x"
                       << static_cast<std::uint32_t>(lhs) << " + 0x" << static_cast<std::uint32_t>(rhs)
                       << " = 0x" << static_cast<std::uint32_t>(comp) << " (CCC: " << std::dec
                       << static_cast<int>(ccc_of(lhs)) << " + " << static_cast<int>(ccc_of(rhs)) << " = "
                       << static_cast<int>(ccc_of(comp)) << ")";
                comps.emplace_back(comp);
            }
        }
        for (auto const lhs : comps) {
            for (auto const rhs : str) {
                auto const comp = webpp::unicode::canonical_composed(lhs, rhs);
                if (comp == webpp::unicode::replacement_char<char32_t>) {
                    continue;
                }
                report << "\n  Compositions: " << std::hex << std::uppercase << "0x"
                       << static_cast<std::uint32_t>(lhs) << " + 0x" << static_cast<std::uint32_t>(rhs)
                       << " = 0x" << static_cast<std::uint32_t>(comp) << " (CCC: " << std::dec
                       << static_cast<int>(ccc_of(lhs)) << " + " << static_cast<int>(ccc_of(rhs)) << " = "
                       << static_cast<int>(ccc_of(comp)) << ")";
            }
        }
        return report.str();
    }
} // namespace

TEST(Unicode, NormalizationTests) {
    using std::u32string;
    using std::u32string_view;
    using std::u8string;
    using std::u8string_view;
    using webpp::unicode::canonical_composed;
    using webpp::unicode::canonical_decomposed;
    using webpp::unicode::toNFC;
    using webpp::unicode::toNFD;
    // in the table from https://www.unicode.org/reports/tr15/#Design_Goals


    // NOLINTBEGIN(*-easily-swappable-parameters)
    auto const check =
      [&](u32string const&                  source,
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
            << "\n  NFC: " << u32ToString(nfc) << "\n  line: " << line << "\n  index: " << test_index
            << "\n  Decomposed: " << u32ToString(canonical_decomposed<std::u32string>(source));

          if constexpr (enable_utf8_composition_tests) {
              EXPECT_EQ(nfd8, toNFD(source8))
                << "  Source: " << u32ToString(source) << "  Source: " << u32ToString(source)
                << "\n  NFD: " << u32ToString(nfd) << "\n  NFC: " << u32ToString(nfc) << "\n  line: " << line
                << "\n  index: " << test_index
                << "\n  Decomposed: " << u32ToString(canonical_decomposed<std::u32string>(source));
          }

          EXPECT_EQ(nfc, toNFC(source))
            << "  Source: " << u32ToString(source) << "\n  NFD: " << u32ToString(nfd)
            << "\n  NFC: " << u32ToString(nfc) << "\n  line: " << line
            << "\n  Calculated NFD: " << u32ToString(toNFD(source)) << "\n  index: " << test_index
            << "\n  Decomposed: " << u32ToString(canonical_decomposed<std::u32string>(source))
            << report_composition_list(source);

          if constexpr (enable_utf8_composition_tests) {
              EXPECT_EQ(nfc8, toNFC(source8))
                << "  Source: " << u32ToString(source) << "  Source: " << u32ToString(source)
                << "\n  NFD: " << u32ToString(nfd) << "\n  NFC: " << u32ToString(nfc) << "\n  line: " << line
                << "\n  index: " << test_index
                << "\n  Decomposed: " << u32ToString(canonical_decomposed<std::u32string>(source));
          }

          check_idempotent(source, nfc, nfd);

          if constexpr (enable_utf8_composition_tests) {
              check_idempotent(source8, nfc, nfd);
          }

          ++test_index;
      };

    // special cases:
    EXPECT_EQ(canonical_composed<char32_t>(0xE0, 0x302), webpp::unicode::replacement_char<char32_t>);
    EXPECT_EQ(toNFC<std::u32string>(U"\x61\x5ae\x300\x302\x315\x62"), U"\xe0\x5ae\x302\x315\x62");
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
