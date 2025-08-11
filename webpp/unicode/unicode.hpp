// Created by moisrex on 12/26/20.

#ifndef WEBPP_UNICODE_HPP
#define WEBPP_UNICODE_HPP

#include "../common/meta.hpp"
#include "../std/iterator.hpp"
#include "../std/string.hpp"
#include "../std/type_traits.hpp"
#include "../std/utility.hpp"
#include "./unicode_concepts.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>

// NOLINTBEGIN(*-magic-numbers)
namespace webpp::unicode {


    // Leading (high) surrogates: 0xd800 - 0xdbff
    // Trailing (low) surrogates: 0xdc00 - 0xdfff
    template <typename u16 = char16_t>
    static constexpr u16 lead_surrogate_min = 0xD800;

    template <typename u16 = char16_t>
    static constexpr u16 lead_surrogate_max = 0xDBFF;

    template <typename u16 = char16_t>
    static constexpr u16 trail_surrogate_min = 0xDC00;

    template <typename u16 = char16_t>
    static constexpr u16 trail_surrogate_max = 0xDFFF;

    template <typename u16 = char16_t>
    static constexpr u16 lead_offset = lead_surrogate_min<u16> - (0x1'0000U >> 10U);

    template <UTF32 u32 = char32_t>
    static constexpr u32 surrogate_offset = 0x1'0000 - (lead_surrogate_min<u32> << 10) - trail_surrogate_min<u32>;

    /**
     * This character is used to replace an unknown, unrecognized, or unrepresentable character in a text.
     * It serves as a placeholder when a character cannot be displayed or interpreted correctly, often
     * due to encoding issues or when a character is not available in the current character set.
     *
     * Characteristics of U+FFFD
     *   - Block: U+FFFD is located in the "Specials" block of Unicode.
     *   - Purpose: It is commonly used to indicate that a character is missing or has been replaced due to an
     *              error in text processing.
     *   - Representation: In various encoding systems, U+FFFD is represented as:
     *                     UTF-8: EF BF BD
     *                     UTF-16: FF FD
     *                     UTF-32: 00 00 FF FD
     *   - Display: The glyph for U+FFFD is typically displayed as a replacement symbol,
     *              such as a question mark or a square, depending on the rendering system and context.
     */
    template <UTF32 u32 = char32_t>
    static constexpr u32 replacement_char = 0x0000'FFFD;

    /// Basic Multilingual Plane (BMP)
    template <UTF T = char32_t>
    static constexpr T max_bmp = 0x0000'FFFF;

    template <UTF32 u32 = char32_t>
    static constexpr u32 max_utf16 = 0x0010'FFFF;

    template <UTF32 u32 = char32_t>
    static constexpr u32 max_utf32 = 0x7FFF'FFFF;

    /// Max valid value for a Unicode code point
    template <UTF32 u32 = char32_t>
    static constexpr u32 max_legal_utf32 = 0x0010'FFFF;

    static constexpr int  half_shift = 10; // used for shifting by 10 bits
    static constexpr auto half_base  = 0x001'0000UL;
    static constexpr auto half_mask  = 0x3FFUL;

    /// Match the max length of two strings based on their character type
    template <UTF InCharT = char32_t, UTF OutCharT = InCharT>
    [[nodiscard]] static constexpr stl::size_t adjust_utf_output_size(stl::size_t inp_size) noexcept {
        webpp_static_constexpr auto inp = sizeof(InCharT);
        webpp_static_constexpr auto out = sizeof(OutCharT);

        // If the input and output character types are different, this will take those into account as well
        if constexpr (inp < out) {
            inp_size /= out - inp;
        } else if constexpr (inp > out) {
            inp_size *= inp - out;
        }
        return inp_size; // output size
    }

    /// utf8_leading_code_units[N] gives you the start of code unit that is required to
    /// be followed by N other code units.
    template <UTF8 T = char8_t>
    static constexpr stl::array<stl::make_unsigned_t<T>, 9UL> utf8_leading_code_units{
      0,           // should be invalid
      0b0,         // length: 1 unit
      0b1100'0000, // length: 2 units
      0b1110'0000, // length: 3 units
      0b1111'0000, // length: 4 units
      0b1111'1000, // length: 5 units
      0b1111'1100, // length: 6 units
      0,           // Cannot happen
      0,           // Cannot happen
    };

    /// utf16_leading_code_units[N] gives you the start of Code Unit that is required to
    /// be followed by N other code units.
    template <UTF16 T = char16_t>
    static constexpr stl::array<T, 3UL> utf16_leading_code_units{
      0,                     // should not be possible
      0b0,                   // length: 1 unit
      0b1101'1000'0000'0000, // length: 2 units
    };

    /// Automatically deduce the type
    template <typename T>
    static constexpr auto utf_leading_code_units = []() consteval {
        if constexpr (UTF8<T>) {
            return utf8_leading_code_units<T>;
        } else if constexpr (UTF16<T>) {
            return utf16_leading_code_units<T>;
        } else {
            static_assert_false(T, "UTF-32 Does not make sense; you might have a bug.");
            return 1;
        }
    }();

    template <typename CharT = char8_t>
    [[nodiscard]] static constexpr bool is_ascii(CharT const code_point) noexcept {
        if constexpr (stl::is_unsigned_v<CharT>) {
            return code_point < 0x80;
        } else {
            return code_point >= 0 && code_point < 0x80;
        }
    }

    template <stl::forward_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr bool is_ascii(Iter spos, Iter send) noexcept {
        // TODO: add SWAR optimization
        for (; spos != send; ++spos) {
            if (!is_ascii(*spos)) {
                return false;
            }
        }
        return true;
    }

    template <typename u8 = char8_t, typename octet_type>
    [[nodiscard]] static constexpr u8 mask8(octet_type oct) noexcept {
        return static_cast<u8>(0xFF & oct);
    }

    template <typename u16 = char16_t, typename u16_type>
    [[nodiscard]] static constexpr u16 mask16(u16_type oct) noexcept {
        return static_cast<u16>(0xFFFF & oct);
    }

    template <typename octet_type>
    [[nodiscard]] static constexpr bool is_trail(octet_type oct) noexcept {
        return (mask8(oct) >> 6) == 0x2;
    }

    template <typename u16>
    [[nodiscard]] static constexpr bool is_lead_surrogate(u16 code_point) noexcept {
        return code_point >= lead_surrogate_min<u16> && code_point <= lead_surrogate_max<u16>;
    }

    template <typename u16>
    [[nodiscard]] static constexpr bool is_trail_surrogate(u16 code_point) noexcept {
        return code_point >= trail_surrogate_min<u16> && code_point <= trail_surrogate_max<u16>;
    }

    template <typename u16>
    [[nodiscard]] static constexpr bool is_surrogate(u16 code_point) noexcept {
        return code_point >= lead_surrogate_min<u16> && code_point <= trail_surrogate_max<u16>;
    }

    template <UTF32 CharT = char32_t>
    [[nodiscard]] static bool is_bmp(CharT const code_point) noexcept {
        return code_point >= 0 && code_point <= 0xFFFFU;
    }

    /**
     * This is used to determine if a given Unicode code point is a supplementary character. Supplementary
     * characters are those with code points in the range from U+10000 to U+10FFFF, which are beyond the Basic
     * Multilingual Plane (BMP) that covers U+0000 to U+FFFF.
     */
    template <UTF32 CharT = char32_t>
    [[nodiscard]] static bool is_supplementary(CharT codePoint) noexcept {
        return codePoint > 0xFFFFU && codePoint <= 0x10'FFFFU;
    }

    /**
     * Check if it's a start of a Code Unit
     */
    template <typename T>
    [[nodiscard]] static constexpr bool is_code_unit_start(T unit) noexcept {
        if constexpr (UTF8<T>) {
            // byte 2, 3, and 4 all start with 0b10xx'xxxx
            return (static_cast<stl::uint8_t>(unit) & 0b1100'0000U) != 0b1000'0000U;
        } else if constexpr (UTF16<T>) {
            return (static_cast<std::uint16_t>(unit) & 0xFC00U) != 0xDC00U;
        } else {
            return true;
        }
    }

    /**
     * Check if the code point is in range
     */
    template <UTF32 u32>
    [[nodiscard]] static constexpr bool is_in_range(u32 code_point) noexcept {
        return code_point <= max_legal_utf32<u32>;
    }

    /**
     * Check whether a Unicode code point is in a valid range.
     *
     * The first check comes from the Unicode guarantee to never encode
     * a point above 0x0010ffff, since UTF-16 couldn't represent it.
     *
     * The second check covers surrogate pairs (category Cs).
     */
    template <UTF32 u32>
    [[nodiscard]] static constexpr bool is_code_point_valid(u32 code_point) noexcept {
        using uu32 = stl::make_unsigned_t<u32>;
        return code_point >= 0 && code_point <= max_legal_utf32<u32> &&
               ((static_cast<uu32>(code_point) & 0xFFFF'F800U) != lead_surrogate_min<uu32>);
        // alternative implementation:
        // return (cp <= max_legal_utf32<u32> && !is_surrogate(cp));
    }

    namespace details {

        // from glib/gutf8.c
        // NOLINTBEGIN(*-avoid-c-arrays)
        static constexpr stl::array<stl::uint8_t, 256> utf8_skip{
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
          2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1};
        // NOLINTEND(*-avoid-c-arrays)
    } // namespace details

    template <UTF CharT, stl::integral SizeT = stl::size_t>
    [[nodiscard]] static constexpr SizeT required_length_of(CharT const code_unit) noexcept {
        if constexpr (UTF16<CharT>) {
            if ((code_unit & 0xFC00U) == 0xD800U) {
                return 2U;
            }
            return 1U;
        } else if constexpr (UTF8<CharT>) {
            // alternative implementation:
            // return value < 0x80
            //          ? 1
            //          : (value < 0x800
            //               ? 2
            //               : (value < 0x1'0000 ? 3 : (value < 0x20'0000 ? 4 : (value < 0x400'0000 ? 5 :
            //               6))));

            // impl 2:
            // if ((value & 0x80U) == 0) {
            //     return 1;
            // }
            // if ((value & 0xE0U) == 0xC0U) {
            //     return 2;
            // }
            // if ((value & 0xF0U) == 0xE0U) {
            //     return 3;
            // }
            // if ((value & 0xF8U) == 0xF0U) {
            //     return 4;
            // }
            // return 1;

            // impl 3:
            return static_cast<SizeT>(details::utf8_skip[static_cast<unsigned char>(code_unit)]);
        } else {
            return 1U;
        }
    }

    /// utf8_length_from_utf32
    template <stl::integral SizeT = stl::size_t, UTF32 CharT = char32_t>
    [[nodiscard]] static constexpr SizeT utf8_length_from_utf32(CharT const code_point) noexcept {
        // todo: check if this function can be optimized by making it branch-less
        if (code_point < 0x80U) [[likely]] {
            return 1U;
        }
        if (code_point < 0x800U) {
            return 2U;
        }
        if (code_point < 0x1'0000U) {
            if (is_surrogate(code_point)) [[unlikely]] {
                // Surrogates are invalid UTF-32 characters.
                return 0U;
            }
            return 3U;
        }
        // Max Code Point for Unicode is 0x0010FFFF.
        if (code_point <= max_legal_utf32<CharT>) {
            return 4U;
        }

        [[unlikely]] {
            // Invalid UTF-32 character.
            return 0;
        }
    }

    /// utf16_length_from_utf32
    template <stl::integral SizeT = stl::size_t, UTF32 CharT = char32_t>
    [[nodiscard]] static constexpr SizeT utf16_length_from_utf32(CharT const code_point) noexcept {
        if (code_point > 0xFFFFU) {
            return 2U;
        }
        return 1U;
    }

    /// utf_length_from_utf32
    template <typename T, stl::integral SizeT = stl::size_t, UTF32 CharT = char32_t>
    [[nodiscard]] static constexpr SizeT utf_length_from_utf32(CharT const code_point) noexcept {
        if constexpr (UTF32<T>) {
            return 1;
        } else if constexpr (UTF16<T>) {
            return utf16_length_from_utf32<SizeT>(code_point);
        } else if constexpr (UTF8<T>) {
            return utf8_length_from_utf32<SizeT>(code_point);
        } else {
            static_assert_false(T, "Invalid return type specified.");
            return 0;
        }
    }

    template <UTF8 T, stl::integral SizeT = stl::size_t, UTF16 CharT = char32_t>
    [[nodiscard]] static constexpr SizeT utf16_length_from_utf8(CharT const code_unit) noexcept {
        SizeT length = 0U;

        // -65 is 0b10111111, anything larger in two-complement's should start a new code point
        if (static_cast<stl::int8_t>(code_unit) > static_cast<stl::int8_t>(0b1011'1111)) {
            ++length;
        }
        if (static_cast<T>(code_unit) >= 240) {
            ++length;
        }
        return length;
    }

    template <UTF8 T, stl::integral SizeT = stl::size_t, UTF16 CharT = char32_t>
    [[nodiscard]] static constexpr SizeT utf8_length_from_utf16(CharT const code_unit) noexcept {
        if (is_surrogate(code_unit)) {
            // surrogate pairs are always 4 bytes.
            return static_cast<SizeT>(4);
        }
        return utf8_length_from_utf32<SizeT>(static_cast<char32_t>(code_unit));
    }

    /// Count the required length to store this code unit
    /// Attention: this is a code unit, not a code point
    template <typename T, stl::integral SizeT = stl::size_t, UTF CharT = char32_t>
    [[nodiscard]] static constexpr SizeT utf_length_from(CharT const code_unit) noexcept {
        if constexpr (UTF32<T>) {
            return 1;
        } else if constexpr (UTF16<T>) {
            if constexpr (UTF32<CharT>) {
                return utf16_length_from_utf32<SizeT>(code_unit);
            } else if constexpr (UTF8<CharT>) {
                return utf16_length_from_utf8<SizeT>(code_unit);
            } else {
                return required_length_of<SizeT>(code_unit); // both are UTF-16
            }
        } else if constexpr (UTF8<T>) {
            if constexpr (UTF32<CharT>) {
                return utf8_length_from_utf32<SizeT>(code_unit);
            } else if constexpr (UTF16<CharT>) {
                return utf8_length_from_utf16<SizeT>(code_unit);
            } else {
                return required_length_of<SizeT>(code_unit); // both are UTF-8
            }
        } else {
            static_assert_false(T, "Invalid return type specified.");
            return 0;
        }
    }

    namespace unchecked {

        // todo: check out the glib/gutf8.c implementation
        template <stl::forward_iterator Iter = char8_t const*, UTF32 CodePointType = char32_t>
        [[nodiscard]] static constexpr CodePointType next_code_point(Iter& pos) noexcept {
            using code_point_type    = CodePointType;
            using char_type          = stl::iter_value_t<Iter>;
            using unsigned_char_type = stl::make_unsigned_t<char_type>;

            // double casting to make sure negative values can't come out of it
            auto val = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
            if constexpr (UTF16<char_type>) {
                if ((val & 0xFC00U) == 0xD800U) {
                    // we have two chars
                    val  &= 0x3FFU;
                    val <<= 10U;
                    val  |= static_cast<code_point_type>(*pos++) & 0x3FFU;
                    val  += 0x1'0000U;
                    return val;
                }
                return val; // this is the only char
            } else if constexpr (UTF8<char_type>) {
                if ((val & 0b1000'0000U) == 0) {
                    // we have one char
                    return val;
                }
                if ((val & 0b1110'0000U) == 0b1100'0000U) {
                    // we have 2 chars
                    val  &= 0b0001'1111U;
                    val <<= 6U;
                    val  |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                    return val;
                }
                if ((val & 0b1111'0000U) == 0b1110'0000U) {
                    // we have 3 chars
                    val  &= 0b0000'1111U;
                    val <<= 12U;
                    val  |= (static_cast<code_point_type>(*pos++) & 0b0011'1111U) << 6U;
                    val  |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                    return val;
                }
                if ((val & 0b1111'1000U) == 0b1111'0000U) {
                    // we have 4 chars
                    val  &= 0b0000'0111U;
                    val <<= 18U;
                    val  |= (static_cast<code_point_type>(*pos++) & 0b0011'1111U) << 12U;
                    val  |= (static_cast<code_point_type>(*pos++) & 0b0011'1111U) << 6U;
                    val  |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                    return val;
                }
                return val; // return this one anyway
            } else {
                return val;
            }
        }

        template <stl::bidirectional_iterator Iter = char8_t const*,
                  typename EIter                   = Iter,
                  UTF32 CodePointType              = char32_t>
            requires stl::sentinel_for<EIter, Iter>
        [[nodiscard]] static constexpr CodePointType next_code_point(Iter& pos, EIter end) noexcept {
            using code_point_type    = CodePointType;
            using char_type          = stl::iter_value_t<Iter>;
            using unsigned_char_type = stl::make_unsigned_t<char_type>;

            if (pos == end) {
                return static_cast<code_point_type>(0);
            }

            // double casting to make sure negative values can't come out of it
            auto val = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
            if (pos == end) {
                return val;
            }
            if constexpr (UTF16<char_type>) {
                if ((val & 0xFC00U) == 0xD800U) {
                    // we have two chars
                    val  &= 0x3FFU;
                    val <<= 10U;
                    val  |= static_cast<code_point_type>(*pos++) & 0x3FFU;
                    val  += 0x1'0000U;
                    return val;
                }
                return val; // this is the only char
            } else if constexpr (UTF8<char_type>) {
                if ((val & 0b1000'0000U) == 0) {
                    // we have one char
                    return val;
                }
                if ((val & 0b1110'0000U) == 0b1100'0000U) {
                    // we have 2 chars
                    val  &= 0b0001'1111U;
                    val <<= 6U;
                    val  |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                    return val;
                }
                if ((val & 0b1111'0000U) == 0b1110'0000U) {
                    // we have 3 chars
                    val  &= 0b0000'1111U;
                    val <<= 12U;
                    val  |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 6U;
                    if (++pos == end) {
                        return *stl::prev(--pos); // bad code point found, return the first code unit
                    }
                    val |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                    return val;
                }
                if ((val & 0b1111'1000U) == 0b1111'0000U) {
                    // we have 4 chars
                    val  &= 0b0000'0111U;
                    val <<= 18U;
                    val  |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 12U;
                    if (++pos == end) {
                        return *stl::prev(--pos); // bad code point found, return the first code unit
                    }
                    val |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 6U;
                    if (++pos == end) {
                        stl::advance(pos, -2);
                        return *stl::prev(pos); // bad code point found, return the first code unit
                    }
                    val |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                    return val;
                }
                return val; // return this one anyway
            } else {
                return val;
            }
        }

        template <stl::forward_iterator Iter = char8_t const*, typename CodePointType = char32_t>
        [[nodiscard]] static constexpr CodePointType next_code_point_copy(Iter pos) noexcept {
            return next_code_point<Iter, CodePointType>(pos);
        }

        template <stl::bidirectional_iterator Iter = char8_t const*,
                  typename EIter                   = Iter,
                  typename CodePointType           = char32_t>
            requires stl::sentinel_for<EIter, Iter>
        [[nodiscard]] static constexpr CodePointType next_code_point_copy(Iter pos, EIter end) noexcept {
            return next_code_point<Iter, EIter, CodePointType>(pos, end);
        }

        template <stl::bidirectional_iterator Iter = char8_t const*, UTF32 CodePointType = char32_t>
        [[nodiscard]] static constexpr CodePointType prev_code_point(Iter& pos) noexcept {
            using code_point_type = CodePointType;
            using char_type       = stl::iter_value_t<Iter>;

            auto val = static_cast<code_point_type>(*--pos);
            if constexpr (UTF16<char_type>) {
                // UTF-16 Encoding
                // byte1            | byte2            |
                // 0xxxxxxxxxxxxxxx |                  |
                // 110110xxxxxxxxxx | 110111xxxxxxxxxx |

                if ((val & 0xFC00U) != 0b1101'1100'0000'0000U) {
                    return val; // 1 byte
                }

                // not it has to be 2 bytes, let's fix the Unicode residuals first:
                val &= 0x3FFU;
                val += 0x1'0000U;

                // now let's add the second byte:
                val |= (static_cast<code_point_type>(*--pos) & 0x3FFU) << 10U;

                return val;
            } else if constexpr (UTF8<char_type>) {
                // byte1    | byte2    |  byte3   | byte4    |
                // 0xxxxxxx |          |          |          |
                // 110xxxxx | 10xxxxxx |          |          |
                // 1110xxxx | 10xxxxxx | 10xxxxxx |          |
                // 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
                if (val >> 6U != 0b10) [[likely]] {
                    return val; // 1 byte
                }

                // let's clean up the first byte's Unicode residuals
                val &= 0b0011'1111U;

                if (*--pos >> 6U != 0b10) {
                    val |= (static_cast<code_point_type>(*pos) & 0b0001'1111U) << 6U; // byte 2
                    return val;                                                       // 2 bytes
                }

                // now there have to be 3 or 4 bytes, let's add byte 2 first:
                val |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 6U; // byte 2

                // checking byte 3:
                if (*--pos >> 6U != 0b10) {
                    val |= (static_cast<code_point_type>(*pos) & 0b0000'1111U) << 12U; // byte 3
                    return val;                                                        // 3 bytes
                }

                // now we have to have 4 bytes, let's add byte 3 first:
                val |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 12U; // byte 3

                // checking byte 4
                if (*--pos >> 6U != 0b10) [[likely]] {
                    val |= (static_cast<code_point_type>(*pos) & 0b0000'0111U) << 18U; // byte 4
                    return val;                                                        // 3 bytes
                }

                // we had to have 4 bytes, seems like a broken code point, let's add the 4th byte first:
                val |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 18U;

                // invalid code point found, let's just return whatever we have now:
                return val;
            } else {
                // UTF-32 is trivial
                return val;
            }
        }

        template <stl::bidirectional_iterator Iter = char8_t const*, UTF32 CodePointType = char32_t>
        [[nodiscard]] static constexpr CodePointType prev_code_point_copy(Iter pos) noexcept {
            return prev_code_point<Iter, CodePointType>(pos);
        }

        template <istl::Appendable Iter = char8_t*, stl::forward_iterator Iter2 = Iter>
        static constexpr stl::size_t copy_next_into(Iter& ito, Iter2& from) noexcept(istl::NothrowAppendable<Iter>) {
            using char_type     = istl::appendable_value_type_t<Iter>;
            using src_char_type = stl::iter_value_t<stl::remove_cvref_t<Iter2>>;
            static_assert(sizeof(char_type) == sizeof(src_char_type), "Character types need to have the same size.");
            if constexpr (UTF8<char_type>) {
                auto const size = static_cast<stl::size_t>(details::utf8_skip[*from]);
                webpp_assume(size <= 6);
                for (stl::size_t index = 0U; index != size; ++index) {
                    istl::iter_append(ito, *from++);
                }
                return size;
            } else if constexpr (UTF16<char_type>) {
                istl::iter_append(ito, *from++);
                if (!(*ito < trail_surrogate_min<char_type> || *ito > trail_surrogate_max<char_type>) ) {
                    istl::iter_append(ito, *from++);
                    return 2U;
                }
                return 1U;
            } else {
                istl::iter_append(ito, *from++);
                return 1U;
            }
        }

        template <istl::Appendable Iter = char8_t*, stl::forward_iterator Iter2 = Iter>
        static constexpr stl::size_t copy_next_into(Iter& ito, Iter2 const& from)
          noexcept(istl::NothrowAppendable<Iter>) {
            Iter2 from_cpy = from;
            return copy_next_into(ito, from_cpy);
        }

        template <stl::forward_iterator Iter = char8_t*>
        static constexpr void next_char(Iter& pos) noexcept {
            using char_type = stl::iter_value_t<Iter>;
            if constexpr (UTF8<char_type>) {
                // alternative implementation:
                // for (++p; (*p & 0xc0) == 0x80; ++p) ;
                using unsigned_type  = stl::make_unsigned_t<char_type>;
                pos                  += details::utf8_skip[static_cast<unsigned_type>(*pos)];
            } else if constexpr (UTF16<char_type>) {
                ++pos;
                if (!(*pos < trail_surrogate_min<char_type> || *pos > trail_surrogate_max<char_type>) ) {
                    ++pos;
                }
            } else {
                ++pos;
            }
        }

        template <stl::forward_iterator Iter = char8_t*>
        [[nodiscard]] static constexpr bool next_char(Iter& pos, Iter const& end) noexcept {
            using char_type       = stl::iter_value_t<Iter>;
            using difference_type = stl::iter_difference_t<Iter>;
            if (pos == end) {
                return false;
            }
            if constexpr (UTF8<char_type>) {
                // alternative implementation:
                // for (++p; (*p & 0xc0) == 0x80; ++p) ;
                using unsigned_type = stl::make_unsigned_t<char_type>;
                auto const len = static_cast<difference_type>(details::utf8_skip[static_cast<unsigned_type>(*pos)]);
                if (end - pos < len) {
                    ++pos;
                    return false;
                }
                stl::advance(pos, len);
            } else if constexpr (UTF16<char_type>) {
                ++pos;
                if (!(*pos < trail_surrogate_min<char_type> || *pos > trail_surrogate_max<char_type>) ) {
                    if (pos == end) {
                        return false;
                    }
                    ++pos;
                }
            } else {
                ++pos;
            }
            return true;
        }

        template <stl::forward_iterator Iter = char8_t*>
        static constexpr void next_char(Iter& pos, stl::size_t count) noexcept {
            using char_type = stl::iter_value_t<Iter>;
            using diff_type = stl::iter_difference_t<Iter>;
            if constexpr (UTF8<char_type> || UTF16<char_type>) {
                while (count != 0) {
                    next_char(pos);
                    --count;
                }
            } else {
                pos += static_cast<diff_type>(count);
            }
        }

        template <stl::forward_iterator Iter = char8_t*>
        static constexpr Iter next_char_copy(Iter pos) noexcept {
            next_char<Iter>(pos);
            return pos;
        }

        template <stl::forward_iterator Iter = char8_t*>
        static constexpr Iter next_char_copy(Iter pos, stl::size_t const count) noexcept {
            next_char<Iter>(pos, count);
            return pos;
        }

        /**
         * Go to the beginning of the previous character.
         * This function does not check if a previous character exists or not or even
         * if it's a valid character.
         */
        template <stl::bidirectional_iterator Iter = char8_t const*>
        static constexpr void prev_char(Iter& pos) noexcept {
            using char_type = stl::iter_value_t<Iter>;
            if constexpr (UTF8<char_type>) {
                --pos;
                if ((*pos & 0xc0) != 0x80) {
                    return;
                }
                --pos;
                if ((*pos & 0xc0) != 0x80) {
                    return;
                }
                --pos;
                if ((*pos & 0xc0) != 0x80) {
                    return;
                }
                --pos;
                if ((*pos & 0xc0) != 0x80) {
                    return;
                }
                --pos;
                if ((*pos & 0xc0) != 0x80) {
                    return;
                }
                --pos;
            } else if constexpr (UTF16<char_type>) {
                --pos;
                if (!(*pos < trail_surrogate_min<char_type> || *pos > trail_surrogate_max<char_type>) ) {
                    --pos;
                }
            } else {
                --pos;
            }
        }

        template <stl::bidirectional_iterator Iter = char8_t const*>
        static constexpr void prev_char(Iter& pos, stl::size_t count) noexcept {
            using char_type = stl::iter_value_t<Iter>;
            if constexpr (UTF8<char_type> || UTF16<char_type>) {
                while (count != 0) {
                    prev_char(pos);
                    --count;
                }
            } else {
                pos -= count;
            }
        }

        template <stl::bidirectional_iterator Iter = char8_t const*>
        [[nodiscard]] static constexpr Iter prev_char_copy(Iter pos) noexcept {
            prev_char<Iter>(pos);
            return pos;
        }

        template <stl::bidirectional_iterator Iter = char8_t const*>
        [[nodiscard]] static constexpr Iter prev_char_copy(Iter pos, stl::size_t const count) noexcept {
            prev_char<Iter>(pos, count);
            return pos;
        }

        /// Unicode-aware std::iter_swap
        template <stl::indirectly_swappable Iter = char8_t*>
            requires(stl::random_access_iterator<Iter>)
        static constexpr void swap_code_points(Iter lhs, Iter rhs)
          noexcept(stl::is_nothrow_swappable_v<stl::iter_value_t<Iter>>) {
            using stl::swap;
            using char_type = stl::iter_value_t<Iter>;
            using diff_type = stl::iter_difference_t<Iter>;

            if constexpr (UTF8<char_type> || UTF16<char_type>) {
                if (lhs > rhs) {
                    swap(lhs, rhs);
                }
                auto const lhs_length = required_length_of<char_type, diff_type>(*lhs);
                auto const rhs_length = required_length_of<char_type, diff_type>(*rhs);

                [[maybe_unused]] webpp_static_constexpr auto max_len = UTF8<char_type> ? 6U : 2U;
                webpp_assume(lhs_length >= 0 && lhs_length <= max_len);
                webpp_assume(rhs_length >= 0 && rhs_length <= max_len);

                // [X|X|X|X| | |X|X| ]
                //  -------     ---
                //     |         |
                //     `--> lhs  `---> rhs
                //
                //  1. swap code points
                //  2. rotate
                stl::swap_ranges(lhs, lhs + rhs_length, rhs);
                if (lhs_length == rhs_length) {
                    return;
                }
                auto middle = (lhs_length > rhs_length ? lhs : rhs) + lhs_length;
                stl::rotate(lhs + rhs_length, middle, rhs + rhs_length);
            } else {
                // utf-32 swap is trivial
                stl::iter_swap(lhs, rhs);
            }
        }

        /**
         * Append a Code Point to a string
         * "out" can be an iterator/pointer or a string
         */
        template <istl::Appendable StrT,
                  stl::integral    SizeT = istl::size_type_of_t<StrT>,
                  stl::integral    CharT = char32_t>
        static constexpr SizeT append(StrT& out, CharT code_point) noexcept(istl::NothrowAppendable<StrT>) {
            using istl::iter_append;

            using char_type = istl::appendable_value_type_t<StrT>;
            using uchar_t   = stl::make_unsigned_t<CharT>;
            if constexpr (UTF32<CharT>) {
                auto const ccp = static_cast<stl::uint32_t>(code_point);
                if constexpr (UTF8<char_type>) {
                    if (ccp < 0x80U) { // one octet
                        iter_append(out, code_point);
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
                    if (ccp <= max_bmp<stl::uint32_t>) {
                        iter_append(out, code_point); // normal case
                        return 1U;
                    }
                    iter_append(out, 0xD7C0U + (static_cast<uchar_t>(ccp) >> 10U));
                    iter_append(out, 0xDC00U + (static_cast<uchar_t>(ccp) & 0x3FFU));
                    return 2U;
                } else { // for char32_t or others
                    iter_append(out, code_point);
                    return 1U;
                }
            } else if constexpr (UTF16<CharT>) {
                if constexpr (UTF8<char_type>) {
                    if (code_point & 0xFF80U) {
                        if (code_point & 0xF800U) {
                            // UCS-2 = U+0800 - U+FFFF -> UTF-8 (3 bytes)
                            iter_append(out, 0xE0U | (code_point >> 12U));
                            iter_append(out, 0x80U | ((code_point >> 6U) & 0x3FU));
                            iter_append(out, 0x80U | (code_point & 0x3FU));
                            return 3U;
                        }

                        // UCS-2 = U+0080 - U+07FF -> UTF-8 (2 bytes)
                        iter_append(out, 0xC0U | (code_point >> 6U));
                        iter_append(out, 0x80U | (code_point & 0x3FU));
                        return 2U;
                    }

                    // UCS-2 = U+0000 - U+007F -> UTF-8 (1 byte)
                    iter_append(out, code_point);
                    return 1U;
                } else {
                    // UTF-16 or UTF-32 are simple casts
                    iter_append(out, code_point);
                    return 1U;
                }
            } else if constexpr (UTF8<CharT>) {
                // converting UTF-8 to upper is a simple cast
                iter_append(out, code_point);
                return 1U;
            } else {
                static_assert_false(CharT, "Invalid code point type");
                return 0U; // just to get rid of a warning
            }
        }

        template <istl::Appendable      StrT,
                  stl::integral         SizeT = istl::size_type_of_t<StrT>,
                  stl::forward_iterator Iter  = char32_t const*>
        static constexpr SizeT append(StrT& out, Iter& src) noexcept(istl::NothrowAppendable<StrT>) {
            using out_char_type = istl::appendable_value_type_t<StrT>;
            using src_char_type = stl::iter_value_t<Iter>;
            if constexpr (UTF32<src_char_type>) {
                return append<StrT, SizeT>(out, *src++);
            } else if constexpr (sizeof(src_char_type) == sizeof(out_char_type)) {
                return unchecked::copy_next_into(out, src);
            } else {
                return append<StrT, SizeT>(out, next_code_point(src));
            }
        }

        template <istl::Appendable      StrT,
                  stl::integral         SizeT = istl::size_type_of_t<StrT>,
                  stl::forward_iterator Iter  = char32_t const*>
        static constexpr SizeT append(StrT& out, Iter const& src) noexcept(istl::NothrowAppendable<StrT>) {
            using out_char_type = istl::appendable_value_type_t<StrT>;
            using src_char_type = stl::iter_value_t<Iter>;
            if constexpr (UTF32<src_char_type>) {
                return append<StrT, SizeT>(out, *src);
            } else if constexpr (sizeof(src_char_type) == sizeof(out_char_type)) {
                return unchecked::copy_next_into(out, src);
            } else {
                return append<StrT, SizeT>(out, next_code_point(src));
            }
        }

        template <istl::Appendable      StrT,
                  stl::integral         SizeT = istl::size_type_of_t<StrT>,
                  stl::forward_iterator Iter  = char32_t const*,
                  typename EIter              = Iter>
            requires stl::sentinel_for<EIter, Iter>
        static constexpr SizeT append(StrT& out, Iter& src, EIter end) noexcept(istl::NothrowAppendable<StrT>) {
            using out_char_type = istl::char_traits_type_of_t<StrT>;
            using src_char_type = stl::iter_value_t<Iter>;
            if constexpr (sizeof(src_char_type) >= sizeof(out_char_type)) {
                // no need to convert to UTF32 then convert to whatever
                return append<StrT, SizeT>(out, *src++);
            } else {
                return append<StrT, SizeT>(out, next_code_point(src, end));
            }
        }

        template <istl::AppendableStorage StrT  = std::array<char8_t, 4UL>,
                  stl::integral           CharT = char32_t,
                  typename... Args>
        [[nodiscard]] static constexpr StrT to(CharT const code_point, Args&&... args)
          noexcept(istl::NothrowAppendable<StrT>) {
            StrT str{stl::forward<Args>(args)...};
            auto iter = istl::appendable_iter_of(str);
            append(iter, code_point);
            return str;
        }

    } // namespace unchecked

    namespace checked {

        template <typename Ptr, stl::integral CharT = char32_t>
        [[nodiscard("Use unicode::unchecked::append if the input codepoint is always valid.")]] static constexpr bool
        append(Ptr& out, CharT code_point) noexcept {
            if (!is_code_point_valid(code_point)) {
                return false;
            }
            unchecked::append<Ptr, CharT>(out, code_point);
            return true;
        }

        enum struct error_handling : stl::uint8_t {
            return_replacement_char = 0,
            return_unchanged        = 1,
            return_negated          = 2,
            return_zero_char        = 3,
            return_max_utf32        = 4,
        };

        template <error_handling              ErrorHandling = error_handling::return_unchanged,
                  UTF32                       CodePointType = char32_t,
                  stl::bidirectional_iterator Iter          = char8_t const*,
                  typename EIter                            = char32_t const*>
            requires(stl::sentinel_for<EIter, Iter>)
        [[nodiscard]] static constexpr CodePointType next_code_point(Iter& pos, EIter const& end) noexcept {
            using enum error_handling;
            using code_point_type    = CodePointType;
            using char_type          = stl::iter_value_t<Iter>;
            using unsigned_char_type = stl::make_unsigned_t<char_type>;
            using difference_type    = stl::iter_difference_t<Iter>;

            if (pos == end) {
                ++pos;
                return static_cast<code_point_type>(0); // return \0 if we're at the end already
            }

            auto const cu1        = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
            auto       code_point = cu1;

            // We're in a constexpr land, we can't use goto.
            for (;;) {
                // double casting to make sure negative values can't come out of it
                if constexpr (UTF32<char_type>) {
                    return cu1;
                } else if constexpr (UTF16<char_type>) {
                    bool const requires_2_units = (cu1 & 0xFC00) == 0xD800;
                    bool       error            = false;
                    if (requires_2_units) {
                        if (pos == end) [[unlikely]] {
                            break;
                        }
                        auto const cu2   = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        error           |= (cu1 & 0xFC00) != 0xD800;
                        error           |= (cu2 & 0xFC00) != 0xDC00;
                        code_point      &= 0x3FF;
                        code_point     <<= 10U;
                        code_point      |= cu2 & 0x3FF;
                        code_point      += 0x1'0000;
                    }
                    if (error || is_surrogate(code_point)) [[unlikely]] {
                        if (requires_2_units) {
                            --pos;
                        }
                        code_point = cu1;
                        break;
                    }
                    return code_point;
                } else if constexpr (UTF8<char_type>) {
                    auto const len = required_length_of<char_type, difference_type>(static_cast<char_type>(cu1));
                    if constexpr (stl::random_access_iterator<Iter>) {
                        if (end - pos < len - 1) [[unlikely]] {
                            break;
                        }
                    }
                    switch (len) {
                        case 1:
                            if ((cu1 & 0b1000'0000) != 0) [[unlikely]] {
                                break;
                            }
                            return cu1;
                        case 2: {
                            if constexpr (!stl::random_access_iterator<Iter>) {
                                if (pos == end) [[unlikely]] {
                                    break;
                                }
                            }
                            auto const cu2     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                            bool       error   = (cu1 & 0b1110'0000) != 0b1100'0000;
                            error             |= (cu2 & 0b1100'0000) != 0b1000'0000;
                            code_point        &= 0b0001'1111;
                            code_point       <<= 6U;
                            code_point        |= cu2 & 0b0011'1111;
                            if (error || code_point < 0x80 || 0x7FF < code_point) [[unlikely]] {
                                --pos;
                                code_point = cu1;
                                break;
                            }
                            return code_point;
                        }
                        case 3: {
                            if constexpr (!stl::random_access_iterator<Iter>) {
                                if (pos == end) [[unlikely]] {
                                    break;
                                }
                            }
                            auto const cu2 = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                            if constexpr (!stl::random_access_iterator<Iter>) {
                                if (pos == end) [[unlikely]] {
                                    break;
                                }
                            }
                            auto const cu3     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                            bool       error   = (cu1 & 0b1111'0000) != 0b1110'0000;
                            error             |= (cu2 & 0b1100'0000) != 0b1000'0000;
                            error             |= (cu3 & 0b1100'0000) != 0b1000'0000;
                            code_point        &= 0b0000'1111;
                            code_point       <<= 12U;
                            code_point        |= (cu2 & 0b0011'1111) << 6U;
                            code_point        |= cu3 & 0b0011'1111;
                            if (error || code_point < 0x800 || 0xFFFF < code_point ||
                                (0xD7FF < code_point && code_point < 0xE000)) [[unlikely]]
                            {
                                stl::advance(pos, -2);
                                code_point = cu1;
                                break;
                            }
                            return code_point;
                        }
                        case 4: {
                            if constexpr (!stl::random_access_iterator<Iter>) {
                                if (pos == end) [[unlikely]] {
                                    break;
                                }
                            }
                            auto const cu2 = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                            if constexpr (!stl::random_access_iterator<Iter>) {
                                if (pos == end) [[unlikely]] {
                                    break;
                                }
                            }
                            auto const cu3 = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                            if constexpr (!stl::random_access_iterator<Iter>) {
                                if (pos == end) [[unlikely]] {
                                    break;
                                }
                            }
                            auto const cu4     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                            bool       error   = (cu1 & 0b1111'0000) != 0b1111'0000;
                            error             |= (cu2 & 0b1100'0000) != 0b1000'0000;
                            error             |= (cu3 & 0b1100'0000) != 0b1000'0000;
                            error             |= (cu4 & 0b1100'0000) != 0b1000'0000;
                            code_point        &= 0b0000'0111;
                            code_point       <<= 18U;
                            code_point        |= (cu2 & 0b0011'1111) << 12U;
                            code_point        |= (cu3 & 0b0011'1111) << 6U;
                            code_point        |= cu4 & 0b0011'1111;
                            if (error || code_point <= 0xFFFF || 0x10'FFFF < code_point) [[unlikely]] {
                                stl::advance(pos, -3);
                                code_point = cu1;
                                break;
                            }
                            return code_point;
                        }
                        default: break;
                    }
                    break;

                } else {
                    static_assert_false(char_type, "Invalid code unit type.");
                    return cu1;
                }
            }

            // handle errors:
            if constexpr (ErrorHandling == return_replacement_char) {
                return replacement_char<code_point_type>;
            } else if constexpr (ErrorHandling == return_max_utf32) {
                return max_utf32<code_point_type>;
            } else if constexpr (ErrorHandling == return_negated) {
                // static_assert(stl::is_signed_v<code_point_type>,
                //             "The code point type should support negative values if you want us to return"
                //             "negative values as errors.");
                return code_point > 0 ? -code_point : code_point;
            } else if constexpr (ErrorHandling == return_zero_char) {
                return static_cast<code_point_type>(0);
            } else {
                return code_point;
            }
        }

        template <error_handling              ErrorHandling = error_handling::return_unchanged,
                  UTF32                       CodePointType = char32_t,
                  stl::bidirectional_iterator Iter          = char8_t const*,
                  typename EIter                            = Iter>
            requires stl::sentinel_for<EIter, Iter>
        [[nodiscard]] static constexpr CodePointType next_code_point_copy(Iter pos, EIter const& end) noexcept {
            return next_code_point<ErrorHandling, CodePointType, Iter, EIter>(pos, end);
        }

        template <stl::bidirectional_iterator Iter = char8_t*, typename EIter = Iter>
            requires stl::sentinel_for<EIter, Iter>
        static constexpr bool next_char(Iter& pos, EIter const& end) noexcept {
            using enum error_handling;
            // todo: is there a way to optimize this?
            static_cast<void>(next_code_point<return_negated, stl::int32_t, Iter, EIter>(pos, end));
            return pos != end;
        }

        namespace details {
            static constexpr stl::array<stl::uint8_t, 256> utf8_magic_lengths{
              // std::array<uint8_t, 256> lens;
              // lens.fill(0);
              // for (uint8_t cp = 0; cp != 0xFF; ++cp) {
              //     lens[(cp & ~0b11) | 0b00]               = 1;
              //     lens[(cp & ~0b11) | 0b01]               = 1;
              //     lens[(cp & ~0b1111) | 0b1110]           = 2;
              //     lens[(cp & ~0b11'1111) | 0b11'1010]     = 3;
              //     lens[(cp & ~0b1111'1111) | 0b1110'1010] = 4;
              // }
              1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0,
              1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 0, 1, 1, 2, 0,
              1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0,
              1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 0, 1, 1, 2, 0,
              1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0,
              1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 0, 1, 1, 2, 0,
              1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0,
              1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 4, 0, 1, 1, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 0, 1, 1, 2,
            };
        } // namespace details

        template <error_handling              ErrorHandling = error_handling::return_unchanged,
                  UTF32                       CodePointType = char32_t,
                  stl::bidirectional_iterator Iter          = char8_t const*,
                  typename EIter                            = Iter>
            requires stl::sentinel_for<EIter, Iter>
        [[nodiscard]] static constexpr CodePointType prev_code_point(Iter& pos, EIter const& beg) noexcept {
            using enum error_handling;
            using code_point_type    = CodePointType;
            using char_type          = stl::iter_value_t<Iter>;
            using unsigned_char_type = stl::make_unsigned_t<char_type>;

            if (pos == beg) {
                --pos;
                return static_cast<code_point_type>(0);
            }

            // Last Code Unit of the code point:
            auto const cu_last    = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
            auto       code_point = cu_last;

            for (;;) {
                if constexpr (UTF16<char_type>) {
                    // UTF-16 Encoding
                    // byte1            | byte2            |
                    // 0xxxxxxxxxxxxxxx |                  |
                    // 110110xxxxxxxxxx | 110111xxxxxxxxxx |
                    bool       error            = false;
                    auto const cu2              = cu_last;
                    bool const requires_2_units = (cu2 & 0xFC00) == 0xDC00;
                    if (requires_2_units) {
                        if (pos == beg) [[unlikely]] {
                            break;
                        }
                        auto const cu1  = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
                        error          |= (cu1 & 0xFC00) != 0xD800;
                        // error       |= (cu2 & 0xFC00) != 0xDC00;
                        code_point     &= 0x3FF;
                        code_point     |= (cu1 & 0x3FF) << 10U;
                        code_point     += 0x1'0000;
                    }

                    if (error || is_surrogate(code_point)) [[unlikely]] {
                        if (requires_2_units) {
                            ++pos;
                        }
                        code_point = cu_last;
                        break;
                    }
                    return code_point;
                } else if constexpr (UTF8<char_type>) {
                    // byte1    | byte2    |  byte3   | byte4    |
                    // 0xxxxxxx |          |          |          |
                    // 110xxxxx | 10xxxxxx |          |          |
                    // 1110xxxx | 10xxxxxx | 10xxxxxx |          |
                    // 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |

                    if (code_point >> 6U != 0b10) [[likely]] {
                        return code_point; // 1 byte
                    }

                    auto const        cu4 = cu_last;
                    code_point_type   cu1, cu2, cu3; // NOLINT(*-isolate-declaration)
                    stl::uint_fast8_t magic_code = 0;
                    stl::uint8_t      length;        // NOLINT(*-init-variables)
                    if constexpr (!stl::random_access_iterator<Iter>) {
                        // To make support for bidirectional iterators that are not random iterators
                        auto const prebeg = stl::prev(beg);
                        stl::int8_t bytes_checked = 4;
                        for (;;) {
                            magic_code |= (cu4 & 0b1100'0000) >> 6U;
                            if (pos == prebeg) {
                                bytes_checked = 1;
                                break;
                            }
                            cu3         = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
                            magic_code |= (cu3 & 0b1100'0000) >> 4U;
                            if (pos == prebeg) {
                                bytes_checked = 2;
                                break;
                            }
                            cu2         = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
                            magic_code |= (cu2 & 0b1100'0000) >> 2U;
                            if (pos == prebeg) {
                                bytes_checked = 3;
                                break;
                            }
                            cu1         = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
                            magic_code |= cu1 & 0b1100'0000;
                            break;
                        }

                        // NOLINTNEXTLINE(*-pro-bounds-constant-array-index)
                        length = details::utf8_magic_lengths[magic_code];

                        stl::advance(pos, bytes_checked - length);
                    } else if (pos - beg >= 3) {
                        cu3 = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
                        cu2 = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
                        cu1 = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));

                        magic_code |= (cu4 & 0b1100'0000) >> 6U;
                        magic_code |= (cu3 & 0b1100'0000) >> 4U;
                        magic_code |= (cu2 & 0b1100'0000) >> 2U;
                        magic_code |= static_cast<stl::uint_fast8_t>(cu1 & 0b1100'0000);

                        // NOLINTNEXTLINE(*-pro-bounds-constant-array-index)
                        length = details::utf8_magic_lengths[magic_code];

                        stl::advance(pos, 4 - length);
                    } else {
                        stl::int8_t bytes_checked = 3;
                        auto const prebeg = stl::prev(beg);
                        for (;;) {
                            magic_code |= (cu4 & 0b1100'0000) >> 6U;
                            if (pos == prebeg) {
                                bytes_checked = 1;
                                break;
                            }
                            cu3         = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
                            magic_code |= (cu3 & 0b1100'0000) >> 4U;
                            if (pos == prebeg) {
                                bytes_checked = 2;
                                break;
                            }
                            cu2         = static_cast<code_point_type>(static_cast<unsigned_char_type>(*--pos));
                            magic_code |= (cu2 & 0b1100'0000) >> 2U;
                            break;
                        }
                        // NOLINTNEXTLINE(*-pro-bounds-constant-array-index)
                        length = details::utf8_magic_lengths[magic_code];

                        stl::advance(pos, bytes_checked - length);
                    }

                    switch (length) {
                        [[unlikely]] case 0: {
                            code_point = cu_last;
                            break;
                        }
                        case 1: {
                            return code_point;
                        }
                        case 2: {
                            code_point &= 0b0011'1111U;
                            code_point |= (cu3 & 0b0001'1111) << 6U;
                            if ((cu3 & 0b1110'0000) != 0b1100'0000 || code_point < 0x80 || 0x7FF < code_point)
                              [[unlikely]]
                            {
                                ++pos;
                                code_point = cu_last;
                                break;
                            }
                            return code_point;
                        }
                        case 3: {
                            code_point &= 0b0011'1111U;
                            code_point |= (cu3 & 0b0011'1111) << 6U;
                            code_point |= (cu2 & 0b0000'1111) << 12U;
                            if ((cu2 & 0b1111'0000) != 0b1110'0000 || code_point < 0x800 || 0xFFFF < code_point ||
                                (0xD7FF < code_point && code_point < 0xE000)) [[unlikely]]
                            {
                                stl::advance(pos, 2);
                                code_point = cu_last;
                                break;
                            }
                            return code_point;
                        }
                        case 4: {
                            code_point &= 0b0011'1111U;
                            code_point |= (cu3 & 0b0011'1111) << 6U;
                            code_point |= (cu2 & 0b0011'1111) << 12U;
                            code_point |= (cu1 & 0b0000'0111) << 18U;
                            if ((cu1 & 0b1111'0000) != 0b1111'0000 || code_point <= 0xFFFF || 0x10'FFFF < code_point)
                              [[unlikely]]
                            {
                                stl::advance(pos, 3);
                                code_point = cu_last;
                                break;
                            }
                            return code_point;
                        }
                        default: stl::unreachable();
                    }
                } else {
                    // UTF-32 is trivial
                    return code_point;
                }
                break;
            }

            // handle errors:
            if constexpr (ErrorHandling == return_replacement_char) {
                return replacement_char<code_point_type>;
            } else if constexpr (ErrorHandling == return_max_utf32) {
                return max_utf32<code_point_type>;
            } else if constexpr (ErrorHandling == return_negated) {
                // static_assert(stl::is_signed_v<code_point_type>,
                //             "The code point type should support negative values if you want us to return"
                //             "negative values as errors.");
                return code_point > 0 ? -code_point : code_point;
            } else if constexpr (ErrorHandling == return_zero_char) {
                return static_cast<code_point_type>(0);
            } else {
                return code_point;
            }
        }

        template <error_handling              ErrorHandling = error_handling::return_unchanged,
                  UTF32                       CodePointType = char32_t,
                  stl::bidirectional_iterator Iter          = char8_t const*,
                  typename EIter                            = Iter>
            requires stl::sentinel_for<EIter, Iter>
        [[nodiscard]] static constexpr CodePointType prev_code_point_copy(Iter pos, EIter const& beg) noexcept {
            return prev_code_point<ErrorHandling, CodePointType, Iter, EIter>(pos, beg);
        }

        /// Length of Code Units in current Code Point:
        ///   Safely check the length of the current code point that the iterator is pointing to even if
        ///   the values are not a valid code point (upon which we return the length of 1).
        template <stl::integral               SizeT = stl::size_t,
                  stl::bidirectional_iterator Iter  = char8_t const*,
                  typename EIter                    = Iter>
            requires stl::sentinel_for<EIter, Iter>
        [[nodiscard]] static constexpr SizeT code_point_length(Iter pos, EIter const& end) noexcept {
            using value_type = stl::iter_value_t<Iter>;
            if (pos == end) {
                return 0;
            }

            if constexpr (UTF32<value_type>) {
                return 1;
            } else if constexpr (UTF16<value_type>) {
                // UTF-16 Encoding
                // byte1            | byte2            |
                // 0xxxxxxxxxxxxxxx |                  |
                // 110110xxxxxxxxxx | 110111xxxxxxxxxx |
                auto const cu1 = *pos++;
                if (pos == end) [[unlikely]] {
                    return 1; // it doesn't matter if it's valid or not
                }
                auto const cu2         = *pos;
                auto const clamped_CUs = (cu1 & 0xFC00) | (cu2 >> 10U);
                return clamped_CUs == 0xD837 ? 2 : 1;
            } else if constexpr (UTF8<value_type>) {
                // todo: find a better way of doing this:
                auto const beg = pos;
                static_cast<void>(next_char<Iter>(pos, end));
                return static_cast<SizeT>(pos - beg);
            } else {
                static_assert_false(value_type, "Invalid iterator.");
                return 0;
            }
        }

        template <stl::bidirectional_iterator OIterT = stl::u8string::iterator>
        static constexpr void advance(OIterT& out, OIterT const oend, stl::size_t index) noexcept {
            assert(static_cast<stl::size_t>(oend - out) >= index);
            if constexpr (UTF32<stl::iter_value_t<OIterT>>) {
                stl::advance(out, index);
            } else {
                for (; index != 0; --index) {
                    static_cast<void>(next_code_point(out, oend));
                }
            }
        }

        template <istl::String StrT = stl::u8string, UTF CharT = char32_t>
        static constexpr void insert(StrT& out, stl::size_t index, CharT val) {
            using out_char_type = istl::char_type_of_t<StrT>;
            if constexpr (UTF32<out_char_type>) {
                out.insert(out.begin() + index, static_cast<out_char_type>(val));
            } else {
                auto       pos  = out.begin();
                auto const pend = out.end();
                assert(out.size() >= index);
                advance(pos, pend, index);
                if constexpr (sizeof(out_char_type) == sizeof(CharT)) {
                    out.insert(pos, static_cast<out_char_type>(val));
                } else {
                    auto const arr = unchecked::to<stl::array<out_char_type, 4U>>(val);
                    out.insert(pos, arr.begin(), arr.begin() + utf_length_from<out_char_type>(val));
                }
            }
        }

        /// Attention: this function will remove the last Code Point
        template <stl::random_access_iterator OIterT = stl::u8string::iterator, UTF CharT = char32_t>
        static constexpr void insert(OIterT& out, OIterT const oend, stl::size_t index, CharT val) noexcept {
            using out_char_type = stl::iter_value_t<OIterT>;
            using diff_type     = stl::iter_difference_t<OIterT>;
            advance(out, oend, index);
            if constexpr (UTF32<CharT> && UTF32<out_char_type>) {
                stl::copy_n(out, oend - out - 1, stl::next(out));
            } else {
                auto const len = utf_length_from<out_char_type, diff_type>(val);
                stl::copy_n(out, oend - out - len, stl::next(out, len));
            }
            unchecked::append(out, val);
        }

        template <stl::random_access_iterator OIterT = stl::u8string::iterator, UTF CharT = char32_t>
        static constexpr void insert_at(OIterT out, OIterT const oend, stl::size_t index, CharT val) noexcept {
            insert(out, oend, index, val);
        }

        /**
         * UTF-32 Bidirectional Iterator
         */
        template <stl::bidirectional_iterator Iter,
                  typename EIter               = stl::default_sentinel_t,
                  error_handling ErrorHandling = error_handling::return_unchanged>
            requires std::sentinel_for<EIter, Iter>
        struct [[nodiscard]] utf32_bidi_iter {
            using difference_type   = stl::iter_difference_t<Iter>;
            using value_type        = char32_t;
            using traits            = stl::iterator_traits<Iter>;
            using pointer           = typename traits::pointer;
            using reference         = value_type&;
            using const_reference   = value_type const&;
            using iterator_category = stl::bidirectional_iterator_tag;
            using iterator_concept  = stl::bidirectional_iterator_tag;

          private:
            [[no_unique_address]] istl::begin_iterator<Iter> beg{};
            [[no_unique_address]] Iter                       lpos{}; // lower-bound of the Unicode code point
            [[no_unique_address]] Iter                       upos{}; // upper-bound of the Unicode code point
            [[no_unique_address]] EIter                      send{};
            value_type                                       code_point{};

          public:
            explicit constexpr utf32_bidi_iter(Iter inp_pos, EIter inp_end) noexcept
              : beg{istl::begin_sentinel(inp_pos)},
                lpos{inp_pos},
                upos{inp_pos},
                send{inp_end} {
                using enum error_handling;
                if (upos == send) {
                    return;
                }
                code_point = checked::next_code_point<ErrorHandling, value_type>(upos, send);
            }

            constexpr utf32_bidi_iter()                                      = default;
            constexpr utf32_bidi_iter(utf32_bidi_iter const&)                = default;
            constexpr utf32_bidi_iter(utf32_bidi_iter&&) noexcept            = default;
            constexpr utf32_bidi_iter& operator=(utf32_bidi_iter const&)     = default;
            constexpr utf32_bidi_iter& operator=(utf32_bidi_iter&&) noexcept = default;
            constexpr ~utf32_bidi_iter() noexcept                            = default;

            constexpr utf32_bidi_iter& operator++() noexcept {
                using enum error_handling;
                lpos       = upos;
                code_point = checked::next_code_point<ErrorHandling, value_type>(upos, send);
                return *this;
            }

            constexpr utf32_bidi_iter& operator--() noexcept {
                using enum error_handling;
                upos       = lpos;
                code_point = checked::prev_code_point<ErrorHandling, value_type>(lpos, beg);
                return *this;
            }

            constexpr const_reference operator*() const noexcept {
                return code_point;
            }

            [[nodiscard]] constexpr utf32_bidi_iter operator--(int) noexcept {
                auto const res = utf32_bidi_iter{*this};
                operator--();
                return res;
            }

            [[nodiscard]] constexpr utf32_bidi_iter operator++(int) noexcept {
                auto const res = utf32_bidi_iter{*this};
                operator++();
                return res;
            }

            [[nodiscard]] constexpr bool operator==(utf32_bidi_iter const& other) const noexcept {
                return lpos == other.lpos;
            }

            [[nodiscard]] constexpr bool operator==(EIter const&) const noexcept {
                return at_end();
            }

            [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
                requires(!stl::convertible_to<EIter, stl::default_sentinel_t>)
            {
                return at_end();
            }

            [[nodiscard]] constexpr bool operator==(istl::begin_sentinel_t) const noexcept {
                return lpos == beg;
            }

            [[nodiscard]] constexpr bool at_end() const noexcept {
                return lpos == send;
            }
        };

        /**
         * UTF-32 Specialization of bidirectional UTF iterator wrapper.
         */
        template <stl::bidirectional_iterator Iter, typename EIter, error_handling ErrorHandling>
            requires(UTF32<stl::iter_value_t<Iter>> && stl::sentinel_for<EIter, Iter>)
        struct [[nodiscard]] utf32_bidi_iter<Iter, EIter, ErrorHandling> {
            using difference_type   = stl::iter_difference_t<Iter>;
            using value_type        = stl::iter_value_t<Iter>;
            using traits            = stl::iterator_traits<Iter>;
            using pointer           = typename traits::pointer;
            using reference         = value_type&;
            using const_reference   = value_type const&;
            using iterator_category = stl::bidirectional_iterator_tag;
            using iterator_concept  = stl::bidirectional_iterator_tag;

          private:
            // todo: do we need beg in this specialization?
            [[no_unique_address]] istl::begin_iterator<Iter> beg{};
            [[no_unique_address]] Iter                       pos{};
            [[no_unique_address]] EIter                      send{};

          public:
            explicit constexpr utf32_bidi_iter(Iter inp_pos, EIter inp_end) noexcept
              : beg{istl::begin_sentinel(inp_pos)},
                pos{inp_pos},
                send{inp_end} {}

            constexpr utf32_bidi_iter()                                      = default;
            constexpr utf32_bidi_iter(utf32_bidi_iter const&)                = default;
            constexpr utf32_bidi_iter(utf32_bidi_iter&&) noexcept            = default;
            constexpr utf32_bidi_iter& operator=(utf32_bidi_iter const&)     = default;
            constexpr utf32_bidi_iter& operator=(utf32_bidi_iter&&) noexcept = default;
            constexpr ~utf32_bidi_iter() noexcept                            = default;

            constexpr utf32_bidi_iter& operator++() noexcept {
                using enum error_handling;
                ++pos;
                return *this;
            }

            constexpr utf32_bidi_iter& operator--() noexcept {
                using enum error_handling;
                --pos;
                return *this;
            }

            constexpr const_reference operator*() const noexcept {
                return *pos;
            }

            [[nodiscard]] constexpr utf32_bidi_iter operator--(int) noexcept {
                auto const res = utf32_bidi_iter{*this};
                operator--();
                return res;
            }

            [[nodiscard]] constexpr utf32_bidi_iter operator++(int) noexcept {
                auto const res = utf32_bidi_iter{*this};
                operator++();
                return res;
            }

            [[nodiscard]] constexpr bool operator==(utf32_bidi_iter const& other) const noexcept {
                return pos == other.pos;
            }

            [[nodiscard]] constexpr bool operator==(EIter const&) const noexcept {
                return at_end();
            }

            [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
                requires(!stl::convertible_to<EIter, stl::default_sentinel_t>)
            {
                return at_end();
            }

            [[nodiscard]] constexpr bool operator==(istl::begin_sentinel_t) const noexcept {
                return pos == beg;
            }

            [[nodiscard]] constexpr bool at_end() const noexcept {
                return pos == send;
            }

            [[nodiscard]] constexpr bool at_start() const noexcept {
                return pos == beg;
            }
        };

        /**
         * UTF-32 Forward Iterator Wrapper.
         * Input Iterator may be UTF-8 or UTF-16.
         */
        template <stl::forward_iterator Iter,
                  typename EIter               = stl::default_sentinel_t,
                  error_handling ErrorHandling = error_handling::return_unchanged>
            requires stl::sentinel_for<EIter, Iter>
        struct [[nodiscard]] utf32_forward_iter {
            using difference_type   = stl::iter_difference_t<Iter>;
            using value_type        = char32_t;
            using traits            = stl::iterator_traits<Iter>;
            using pointer           = typename traits::pointer;
            using reference         = value_type&;
            using const_reference   = value_type const&;
            using iterator_category = stl::forward_iterator_tag;
            using iterator_concept  = stl::forward_iterator_tag;

          private:
            [[no_unique_address]] Iter  cur{};
            [[no_unique_address]] Iter  pos{};
            [[no_unique_address]] EIter send{};
            value_type                  code_point{};

          public:
            explicit constexpr utf32_forward_iter(Iter inp_pos, EIter inp_end) noexcept
              : cur{inp_pos},
                pos{inp_pos},
                send{inp_end} {
                using enum error_handling;
                if (pos == send) {
                    return;
                }
                code_point = checked::next_code_point<ErrorHandling, value_type>(pos, send);
            }

            constexpr utf32_forward_iter()                                         = default;
            constexpr utf32_forward_iter(utf32_forward_iter const&)                = default;
            constexpr utf32_forward_iter(utf32_forward_iter&&) noexcept            = default;
            constexpr utf32_forward_iter& operator=(utf32_forward_iter const&)     = default;
            constexpr utf32_forward_iter& operator=(utf32_forward_iter&&) noexcept = default;
            constexpr ~utf32_forward_iter() noexcept                               = default;

            constexpr utf32_forward_iter& operator++() noexcept {
                using enum error_handling;
                cur        = pos;
                code_point = checked::next_code_point<ErrorHandling, value_type>(pos, send);
                return *this;
            }

            constexpr const_reference operator*() const noexcept {
                return code_point;
            }

            [[nodiscard]] constexpr utf32_forward_iter operator++(int) noexcept {
                auto const res = utf32_forward_iter{*this};
                operator++();
                return res;
            }

            [[nodiscard]] constexpr bool operator==(utf32_forward_iter const& other) const noexcept {
                return cur == other.cur;
            }

            [[nodiscard]] constexpr bool operator==(EIter const&) const noexcept {
                return at_end();
            }

            [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
                requires(!stl::convertible_to<EIter, stl::default_sentinel_t>)
            {
                return at_end();
            }

            [[nodiscard]] constexpr bool at_end() const noexcept {
                return cur == send;
            }
        };

        /**
         * UTF-32 Specialization of the above UTF forward iterator
         */
        template <stl::forward_iterator Iter, typename EIter, error_handling ErrorHandling>
            requires(UTF32<stl::iter_value_t<Iter>> && stl::sentinel_for<EIter, Iter>)
        struct [[nodiscard]] utf32_forward_iter<Iter, EIter, ErrorHandling> {
            using difference_type   = stl::iter_difference_t<Iter>;
            using value_type        = stl::iter_value_t<Iter>;
            using traits            = stl::iterator_traits<Iter>;
            using pointer           = typename traits::pointer;
            using reference         = value_type&;
            using const_reference   = value_type const&;
            using iterator_category = stl::forward_iterator_tag;
            using iterator_concept  = stl::forward_iterator_tag;

          private:
            [[no_unique_address]] Iter  pos{};
            [[no_unique_address]] EIter send{};

          public:
            explicit constexpr utf32_forward_iter(Iter inp_pos, EIter inp_end) noexcept : pos{inp_pos}, send{inp_end} {}

            constexpr utf32_forward_iter()                                         = default;
            constexpr utf32_forward_iter(utf32_forward_iter const&)                = default;
            constexpr utf32_forward_iter(utf32_forward_iter&&) noexcept            = default;
            constexpr utf32_forward_iter& operator=(utf32_forward_iter const&)     = default;
            constexpr utf32_forward_iter& operator=(utf32_forward_iter&&) noexcept = default;
            constexpr ~utf32_forward_iter() noexcept                               = default;

            constexpr utf32_forward_iter& operator++() noexcept {
                using enum error_handling;
                ++pos;
                return *this;
            }

            constexpr const_reference operator*() const noexcept {
                return *pos;
            }

            [[nodiscard]] constexpr utf32_forward_iter operator++(int) noexcept {
                auto const res = utf32_forward_iter{*this};
                operator++();
                return res;
            }

            [[nodiscard]] constexpr bool operator==(utf32_forward_iter const& other) const noexcept {
                return pos == other.pos;
            }

            [[nodiscard]] constexpr bool operator==(EIter const&) const noexcept {
                return at_end();
            }

            [[nodiscard]] constexpr bool operator==(stl::default_sentinel_t) const noexcept
                requires(!stl::convertible_to<EIter, stl::default_sentinel_t>)
            {
                return at_end();
            }

            [[nodiscard]] constexpr bool at_end() const noexcept {
                return pos == send;
            }
        };


    } // namespace checked

} // namespace webpp::unicode

// NOLINTEND(*-magic-numbers)

#endif // WEBPP_UNICODE_HPP
