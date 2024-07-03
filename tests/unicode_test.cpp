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
    auto const remaining_pos    = static_cast<stl::size_t>(code_point) & ccc_index::chunk_mask;
    // auto const        helper           = ccc_index[code_point_range];
    // auto const        mask             = static_cast<stl::uint8_t>(helper);
    // auto const        shift            = static_cast<stl::uint8_t>(helper >> 8);
    // auto const        index            = helper >> 16U;
    // auto const        masked_pos       = mask & code_point_index;
    // stl::size_t const index_pos        = index + static_cast<stl::size_t>(masked_pos);

    // auto const code_point_index = static_cast<stl::uint8_t>(code_point);
    auto const code             = ccc_indices.at(code_point_range);
    // calculating the position of te value in the ccc_values table:
    stl::size_t const index_pos = code.get_position(code_point);
    auto              res       = ccc_values.at(index_pos);

    stl::string           around       = "[..., ";
    constexpr stl::size_t details_span = 4ULL;
    for (stl::size_t pos = static_cast<stl::size_t>(stl::max<stl::int64_t>(index_pos - details_span, 0LL));
         pos != stl::min<stl::size_t>(index_pos + details_span, ccc_values.size());
         ++pos)
    {
        if (pos == index_pos) {
            around += "[";
        }
        around += stl::to_string(pos);
        around += "=";
        around += stl::to_string(ccc_values.at(pos));
        if (pos == index_pos) {
            around += "]";
        }
        around += ", ";
    }
    around += "...]";
    return fmt::format(
      R"data(code: {}
mask: {}
index: {}
remaining pos: {}
masked pos: {}
actual pos: {} = {} + {}
result: {}
{}
)data",
      code.value(),
      code.mask,
      code.pos,
      remaining_pos,
      code.masked(remaining_pos),
      code.get_position(code_point),
      code.pos,
      remaining_pos,
      res,
      around);
}

template <typename CharT = char32_t>
[[nodiscard]] static constexpr stl::string desc_decomp_of(CharT const code_point) noexcept {
    using webpp::unicode::details::decomp_index;
    using webpp::unicode::details::decomp_indices;
    using webpp::unicode::details::decomp_values;
    using webpp::unicode::details::trailing_mapped_deomps;
    if (code_point >= static_cast<CharT>(trailing_mapped_deomps)) [[unlikely]] {
        return "Definite Zero";
    }
    auto const code_point_range = static_cast<stl::size_t>(code_point) >> decomp_index::chunk_shift;
    auto const remaining_pos    = static_cast<stl::size_t>(code_point) & decomp_index::chunk_mask;

    try {
        auto const        code      = decomp_indices.at(code_point_range);
        stl::size_t const index_pos = code.get_position(code_point);
        auto              res       = decomp_values.at(index_pos);

        return fmt::format(
          R"data(code: {}
pos: {}
max length: {}
remaining pos: {}
actual pos: {} = {} + {}
result: {}
)data",
          code.value(),
          code.pos,
          code.max_length,
          remaining_pos,
          code.get_position(code_point),
          code.pos,
          remaining_pos,
          res);
    } catch (std::out_of_range const&) {
        return fmt::format("Out of range");
    }
}

TEST(Unicode, getCcc) {
    // clang-format off
    // Command to get samples:
    // awk 'BEGIN{FS=";"; OF=""} !/^\s*#/ { if ($4 != 0) { print "EXPECT_EQ(unicode::ccc_of(0x" $1 "), " $4 ") << desc_ccc_of(0x" $1 ");"; } }' UnicodeData.txt | sort -R | head
    // clang-format on
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

    EXPECT_EQ(unicode::ccc_of(0x08D8), 230) << desc_ccc_of(0x08D8);
    EXPECT_EQ(unicode::ccc_of(0x1'11C0), 9) << desc_ccc_of(0x1'11C0);
    EXPECT_EQ(unicode::ccc_of(0x1'0A3F), 9) << desc_ccc_of(0x1'0A3F);
    EXPECT_EQ(unicode::ccc_of(0x081E), 230) << desc_ccc_of(0x081E);
    EXPECT_EQ(unicode::ccc_of(0x193B), 220) << desc_ccc_of(0x193B);
    EXPECT_EQ(unicode::ccc_of(0x034C), 230) << desc_ccc_of(0x034C);
    EXPECT_EQ(unicode::ccc_of(0x06DB), 230) << desc_ccc_of(0x06DB);
    EXPECT_EQ(unicode::ccc_of(0x0325), 220) << desc_ccc_of(0x0325);
    EXPECT_EQ(unicode::ccc_of(0x09BC), 7) << desc_ccc_of(0x09BC);
    EXPECT_EQ(unicode::ccc_of(0x2CF0), 230) << desc_ccc_of(0x2CF0);

    EXPECT_EQ(unicode::ccc_of(0x1'E8D4), 220) << desc_ccc_of(0x1'E8D4);
    EXPECT_EQ(unicode::ccc_of(0x1'E8D5), 220) << desc_ccc_of(0x1'E8D5);
    EXPECT_EQ(unicode::ccc_of(0x1'E8D6), 220) << desc_ccc_of(0x1'E8D6);
    EXPECT_EQ(unicode::ccc_of(0x1'E944), 230) << desc_ccc_of(0x1'E944);
    EXPECT_EQ(unicode::ccc_of(0x1'E945), 230) << desc_ccc_of(0x1'E945);
    EXPECT_EQ(unicode::ccc_of(0x1'E946), 230) << desc_ccc_of(0x1'E946);
    EXPECT_EQ(unicode::ccc_of(0x1'E947), 230) << desc_ccc_of(0x1'E947);
    EXPECT_EQ(unicode::ccc_of(0x1'E948), 230) << desc_ccc_of(0x1'E948);
    EXPECT_EQ(unicode::ccc_of(0x1'E949), 230) << desc_ccc_of(0x1'E949);
    EXPECT_EQ(unicode::ccc_of(0x1'E94A), 7) << desc_ccc_of(0x1'E94A);

    EXPECT_EQ(unicode::ccc_of(0xE'01EA), 0) << desc_ccc_of(0xE'01EA);
    EXPECT_EQ(unicode::ccc_of(0xE'01EB), 0) << desc_ccc_of(0xE'01EB);
    EXPECT_EQ(unicode::ccc_of(0xE'01EC), 0) << desc_ccc_of(0xE'01EC);
    EXPECT_EQ(unicode::ccc_of(0xE'01ED), 0) << desc_ccc_of(0xE'01ED);
    EXPECT_EQ(unicode::ccc_of(0xE'01EE), 0) << desc_ccc_of(0xE'01EE);
    EXPECT_EQ(unicode::ccc_of(0xE'01EF), 0) << desc_ccc_of(0xE'01EF);
    EXPECT_EQ(unicode::ccc_of(0xF'0000), 0) << desc_ccc_of(0xF'0000);
    EXPECT_EQ(unicode::ccc_of(0xF'FFFD), 0) << desc_ccc_of(0xF'FFFD);
    EXPECT_EQ(unicode::ccc_of(0x10'0000), 0) << desc_ccc_of(0x10'0000);
    EXPECT_EQ(unicode::ccc_of(0x10'FFFD), 0) << desc_ccc_of(0x10'FFFD);

    EXPECT_EQ(unicode::ccc_of(0x0000), 0) << desc_ccc_of(0x0000);
    EXPECT_EQ(unicode::ccc_of(0x0001), 0) << desc_ccc_of(0x0001);
    EXPECT_EQ(unicode::ccc_of(0x0002), 0) << desc_ccc_of(0x0002);
    EXPECT_EQ(unicode::ccc_of(0x0003), 0) << desc_ccc_of(0x0003);
    EXPECT_EQ(unicode::ccc_of(0x0004), 0) << desc_ccc_of(0x0004);
    EXPECT_EQ(unicode::ccc_of(0x0005), 0) << desc_ccc_of(0x0005);
    EXPECT_EQ(unicode::ccc_of(0x0006), 0) << desc_ccc_of(0x0006);
    EXPECT_EQ(unicode::ccc_of(0x0007), 0) << desc_ccc_of(0x0007);
    EXPECT_EQ(unicode::ccc_of(0x0008), 0) << desc_ccc_of(0x0008);
    EXPECT_EQ(unicode::ccc_of(0x0009), 0) << desc_ccc_of(0x0009);

    EXPECT_EQ(unicode::ccc_of(0x2DFC), 230) << desc_ccc_of(0x2DFC);
    EXPECT_EQ(unicode::ccc_of(0x05AA), 220) << desc_ccc_of(0x05AA);
    EXPECT_EQ(unicode::ccc_of(0x1BF3), 9) << desc_ccc_of(0x1BF3);
    EXPECT_EQ(unicode::ccc_of(0x0485), 230) << desc_ccc_of(0x0485);
    EXPECT_EQ(unicode::ccc_of(0x1'D171), 216) << desc_ccc_of(0x1'D171);
    EXPECT_EQ(unicode::ccc_of(0x1'14C2), 9) << desc_ccc_of(0x1'14C2);
    EXPECT_EQ(unicode::ccc_of(0x1'E945), 230) << desc_ccc_of(0x1'E945);
    EXPECT_EQ(unicode::ccc_of(0x065A), 230) << desc_ccc_of(0x065A);
    EXPECT_EQ(unicode::ccc_of(0x0615), 230) << desc_ccc_of(0x0615);
    EXPECT_EQ(unicode::ccc_of(0x1'E011), 230) << desc_ccc_of(0x1'E011);

    EXPECT_EQ(unicode::ccc_of(0x1'E00A), 230) << desc_ccc_of(0x1'E00A);
    EXPECT_EQ(unicode::ccc_of(0x081C), 230) << desc_ccc_of(0x081C);
    EXPECT_EQ(unicode::ccc_of(0x1'133B), 7) << desc_ccc_of(0x1'133B);
    EXPECT_EQ(unicode::ccc_of(0x1'D243), 230) << desc_ccc_of(0x1'D243);
    EXPECT_EQ(unicode::ccc_of(0x1DF8), 228) << desc_ccc_of(0x1DF8);
    EXPECT_EQ(unicode::ccc_of(0x08D2), 220) << desc_ccc_of(0x08D2);
    EXPECT_EQ(unicode::ccc_of(0x073F), 230) << desc_ccc_of(0x073F);
    EXPECT_EQ(unicode::ccc_of(0x2CF1), 230) << desc_ccc_of(0x2CF1);
    EXPECT_EQ(unicode::ccc_of(0x08D6), 230) << desc_ccc_of(0x08D6);
    EXPECT_EQ(unicode::ccc_of(0x0328), 202) << desc_ccc_of(0x0328);
}

std::u8string utf32_to_utf8(std::u32string const& utf32_str) {
    std::u8string utf8_str;
    utf8_str.reserve(utf32_str.length() * 4); // Estimate maximum size of UTF-8 string

    for (char32_t const code_point : utf32_str) {
        if (!unicode::checked::append(utf8_str, code_point)) {
            throw stl::invalid_argument("Invalid code point");
        }
    }

    return utf8_str;
}

// Use this command to get the decomposed and its mapped values:
// awk 'BEGIN{FS=";"; OF=""} !/^\s*#/{gsub(/<[^>]*>/, "", $6); if($6 != "") print $1 ": " $6}' UnicodeData.txt
TEST(Unicode, Decompose) {
    // clang-format off
    // Get more examples with this command:
    // awk 'BEGIN{FS=";"; OF=""} !/^\s*#/ {gsub(/<[^>]*>/, "", $6); $6 = " " $6; gsub(/\s+/, " ", $6); if ($6 != " ") { gsub(/\s+/, "\\\x", $6); print "EXPECT_EQ(unicode::decomposed<stl::u8string>(U\'\\\x" $1 "\'), utf32_to_utf8(U\"" $6 "\")) << desc_decomp_of(U\'\\\x" $1 "\');"; } }' UnicodeData.txt | sort -R | head
    // clang-format on

    static_assert(stl::same_as<char8_t, istl::appendable_value_type_t<stl::u8string*>>, "invalid value type");

    // start
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\0'), utf32_to_utf8(stl::u32string{U"\0", 1}))
      << desc_decomp_of(U'\0');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\1'), utf32_to_utf8(U"\1")) << desc_decomp_of(U'\1');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x009F'), utf32_to_utf8(U"\x009F"))
      << desc_decomp_of(U'\x009F');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00A0'), utf32_to_utf8(U"\x0020"))
      << desc_decomp_of(U'\x00A0');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00A8'), utf32_to_utf8(U"\x0020\x0308"))
      << desc_decomp_of(U'\x00A8');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00AA'), utf32_to_utf8(U"\x0061"))
      << desc_decomp_of(U'\x00AA');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00AF'), utf32_to_utf8(U"\x0020\x0304"))
      << desc_decomp_of(U'\x00AF');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00B2'), utf32_to_utf8(U"\x0032"))
      << desc_decomp_of(U'\x00B2');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00B3'), utf32_to_utf8(U"\x0033"))
      << desc_decomp_of(U'\x00B3');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00B4'), utf32_to_utf8(U"\x0020\x0301"))
      << desc_decomp_of(U'\x00B4');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00B5'), utf32_to_utf8(U"\x03BC"))
      << desc_decomp_of(U'\x00B5');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00B8'), utf32_to_utf8(U"\x0020\x0327"))
      << desc_decomp_of(U'\x00B8');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x00B9'), utf32_to_utf8(U"\x0031"))
      << desc_decomp_of(U'\x00B9');


    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA16'), utf32_to_utf8(U"\x4D56"))
      << desc_decomp_of(U'\x2FA16');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x3359'), utf32_to_utf8(U"\x0031\x70B9"))
      << desc_decomp_of(U'\x3359');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xF9B1'), utf32_to_utf8(U"\x9234"))
      << desc_decomp_of(U'\xF9B1');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1EEAB'), utf32_to_utf8(U"\x0644"))
      << desc_decomp_of(U'\x1EEAB');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2F9FD'), utf32_to_utf8(U"\x29496"))
      << desc_decomp_of(U'\x2F9FD');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFCA7'), utf32_to_utf8(U"\x062C\x062D"))
      << desc_decomp_of(U'\xFCA7');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1EE09'), utf32_to_utf8(U"\x064A"))
      << desc_decomp_of(U'\x1EE09');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xF9F2'), utf32_to_utf8(U"\x9C57"))
      << desc_decomp_of(U'\xF9F2');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1D41E'), utf32_to_utf8(U"\x0065"))
      << desc_decomp_of(U'\x1D41E');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1E06D'), utf32_to_utf8(U"\x04B1"))
      << desc_decomp_of(U'\x1E06D');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x249E'), utf32_to_utf8(U"\x0028\x0063\x0029"))
      << desc_decomp_of(U'\x249E');

    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1EE3B'), utf32_to_utf8(U"\x063A"))
      << desc_decomp_of(U'\x1EE3B');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2F8FE'), utf32_to_utf8(U"\x6C67"))
      << desc_decomp_of(U'\x2F8FE');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x0130'), utf32_to_utf8(U"\x0049\x0307"))
      << desc_decomp_of(U'\x0130');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x01DC'), utf32_to_utf8(U"\x00FC\x0300"))
      << desc_decomp_of(U'\x01DC');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFF8B'), utf32_to_utf8(U"\x30D2"))
      << desc_decomp_of(U'\xFF8B');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x32D1'), utf32_to_utf8(U"\x30A4"))
      << desc_decomp_of(U'\x32D1');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x3193'), utf32_to_utf8(U"\x4E8C"))
      << desc_decomp_of(U'\x3193');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x02B8'), utf32_to_utf8(U"\x0079"))
      << desc_decomp_of(U'\x02B8');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2076'), utf32_to_utf8(U"\x0036"))
      << desc_decomp_of(U'\x2076');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1D4D8'), utf32_to_utf8(U"\x0049"))
      << desc_decomp_of(U'\x1D4D8');

    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x216C'), utf32_to_utf8(U"\x004C"))
      << desc_decomp_of(U'\x216C');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFE94'), utf32_to_utf8(U"\x0629"))
      << desc_decomp_of(U'\xFE94');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFCE9'), utf32_to_utf8(U"\x0634\x0645"))
      << desc_decomp_of(U'\xFCE9');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x0F9D'), utf32_to_utf8(U"\x0F9C\x0FB7"))
      << desc_decomp_of(U'\x0F9D');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x038C'), utf32_to_utf8(U"\x039F\x0301"))
      << desc_decomp_of(U'\x038C');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x0F77'), utf32_to_utf8(U"\x0FB2\x0F81"))
      << desc_decomp_of(U'\x0F77');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFC0A'), utf32_to_utf8(U"\x0628\x064A"))
      << desc_decomp_of(U'\xFC0A');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFB4E'), utf32_to_utf8(U"\x05E4\x05BF"))
      << desc_decomp_of(U'\xFB4E');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2F996'), utf32_to_utf8(U"\x82E6"))
      << desc_decomp_of(U'\x2F996');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2F859'), utf32_to_utf8(U"\x214E4"))
      << desc_decomp_of(U'\x2F859');

    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA14'), utf32_to_utf8(U"\x2A291"))
      << desc_decomp_of(U'\x2FA14');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA15'), utf32_to_utf8(U"\x9EBB"))
      << desc_decomp_of(U'\x2FA15');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA16'), utf32_to_utf8(U"\x4D56"))
      << desc_decomp_of(U'\x2FA16');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA17'), utf32_to_utf8(U"\x9EF9"))
      << desc_decomp_of(U'\x2FA17');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA18'), utf32_to_utf8(U"\x9EFE"))
      << desc_decomp_of(U'\x2FA18');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA19'), utf32_to_utf8(U"\x9F05"))
      << desc_decomp_of(U'\x2FA19');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA1A'), utf32_to_utf8(U"\x9F0F"))
      << desc_decomp_of(U'\x2FA1A');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA1B'), utf32_to_utf8(U"\x9F16"))
      << desc_decomp_of(U'\x2FA1B');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA1C'), utf32_to_utf8(U"\x9F3B"))
      << desc_decomp_of(U'\x2FA1C');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FA1D'), utf32_to_utf8(U"\x2A600"))
      << desc_decomp_of(U'\x2FA1D');

    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFFC5'), utf32_to_utf8(U"\x3152"))
      << desc_decomp_of(U'\xFFC5');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x03AD'), utf32_to_utf8(U"\x03B5\x0301"))
      << desc_decomp_of(U'\x03AD');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1D731'), utf32_to_utf8(U"\x03A6"))
      << desc_decomp_of(U'\x1D731');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFCF9'), utf32_to_utf8(U"\x063A\x0649"))
      << desc_decomp_of(U'\xFCF9');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x32C3'), utf32_to_utf8(U"\x0034\x6708"))
      << desc_decomp_of(U'\x32C3');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1FE1'), utf32_to_utf8(U"\x03C5\x0304"))
      << desc_decomp_of(U'\x1FE1');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1D53'), utf32_to_utf8(U"\x0254"))
      << desc_decomp_of(U'\x1D53');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFC49'), utf32_to_utf8(U"\x0645\x0649"))
      << desc_decomp_of(U'\xFC49');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1E067'), utf32_to_utf8(U"\x0491"))
      << desc_decomp_of(U'\x1E067');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFD3A'), utf32_to_utf8(U"\x0637\x0645"))
      << desc_decomp_of(U'\xFD3A');

    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x3160'), utf32_to_utf8(U"\x1172"))
      << desc_decomp_of(U'\x3160');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x2FCB'), utf32_to_utf8(U"\x9EF9"))
      << desc_decomp_of(U'\x2FCB');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x33E4'), utf32_to_utf8(U"\x0035\x65E5"))
      << desc_decomp_of(U'\x33E4');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1F62'), utf32_to_utf8(U"\x1F60\x0300"))
      << desc_decomp_of(U'\x1F62');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFFB5'), utf32_to_utf8(U"\x3145"))
      << desc_decomp_of(U'\xFFB5');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x0453'), utf32_to_utf8(U"\x0433\x0301"))
      << desc_decomp_of(U'\x0453');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x305A'), utf32_to_utf8(U"\x3059\x3099"))
      << desc_decomp_of(U'\x305A');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\xFFBA'), utf32_to_utf8(U"\x314A"))
      << desc_decomp_of(U'\xFFBA');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x1D5CE'), utf32_to_utf8(U"\x0075"))
      << desc_decomp_of(U'\x1D5CE');
    EXPECT_EQ(unicode::decomposed<stl::u8string>(U'\x0FA2'), utf32_to_utf8(U"\x0FA1\x0FB7"))
      << desc_decomp_of(U'\x0FA2');
}

// NOLINTEND(*-magic-numbers)
