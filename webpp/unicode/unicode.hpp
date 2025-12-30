// Created by moisrex on 12/26/20.

#ifndef WEBPP_UNICODE_HPP
#define WEBPP_UNICODE_HPP

#include "../common/meta.hpp"
#include "../std/type_traits.hpp"
#include "./unicode_concepts.hpp"

#include <array>
#include <cassert>

// NOLINTBEGIN(*-magic-numbers)
namespace webpp::unicode {

    /// Match the max length of two strings based on their character type
    /// For example 1 UTF-32 code point at max will require 4 UTF-8 code point
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
    static constexpr stl::array<stl::make_unsigned_t<char8_t>, 9UL> utf8_leading_code_units{
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
    static constexpr stl::array<char16_t, 3UL> utf16_leading_code_units{
      0,                     // should not be possible
      0b0,                   // length: 1 unit
      0b1101'1000'0000'0000, // length: 2 units
    };

    /// Automatically deduce the type
    template <typename T>
    static constexpr auto utf_leading_code_units = []() consteval {
        if constexpr (UTF8<T>) {
            return utf8_leading_code_units;
        } else if constexpr (UTF16<T>) {
            return utf16_leading_code_units;
        } else {
            static_assert_false(T, "UTF-32 Does not make sense; you might have a bug.");
            return 1;
        }
    }();

    template <UTF CharT = char8_t>
    [[nodiscard]] static constexpr bool is_ascii(CharT const code_point) noexcept {
        return (static_cast<CharT>(~0x7FU) & code_point) == 0;
        // return code_point >= 0 && code_point < 0x80;
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

    template <typename octet_type>
    [[nodiscard]] static constexpr char8_t mask8(octet_type const oct) noexcept {
        return static_cast<char8_t>(0xFF & oct);
    }

    template <typename u16_type>
    [[nodiscard]] static constexpr char16_t mask16(u16_type const oct) noexcept {
        return static_cast<char16_t>(0xFFFF & oct);
    }

    template <typename octet_type>
    [[nodiscard]] static constexpr bool is_trail(octet_type const oct) noexcept {
        return mask8(oct) >> 6 == 0x2;
    }

    template <typename CharT>
        requires(UTF32<CharT> || UTF16<CharT>)
    [[nodiscard]] static constexpr bool is_lead_surrogate(CharT const code_point) noexcept {
        return code_point >= static_cast<CharT>(lead_surrogate_min) &&
               code_point <= static_cast<CharT>(lead_surrogate_max);
    }

    template <typename CharT>
        requires(UTF32<CharT> || UTF16<CharT>)
    [[nodiscard]] static constexpr bool is_trail_surrogate(CharT const code_point) noexcept {
        return code_point >= static_cast<CharT>(trail_surrogate_min) &&
               code_point <= static_cast<CharT>(trail_surrogate_max);
    }

    /**
     * Check if it's a surrogate Code Point or not.
     * This is templated because static casting char32_t to char16_t may change the value.
     */
    template <typename CharT>
        requires(UTF32<CharT> || UTF16<CharT>)
    [[nodiscard]] static constexpr bool is_surrogate(CharT const code_point) noexcept {
        return code_point >= static_cast<CharT>(lead_surrogate_min) &&
               code_point <= static_cast<CharT>(trail_surrogate_max);
    }

    [[nodiscard]] static constexpr bool is_bmp(char32_t const code_point) noexcept {
        return static_cast<stl::int32_t>(code_point) >= 0 && code_point <= 0xFFFF;
    }

    /**
     * This is used to determine if a given Unicode code point is a supplementary character. Supplementary
     * characters are those with code points in the range from U+10000 to U+10FFFF, which are beyond the Basic
     * Multilingual Plane (BMP) that covers U+0000 to U+FFFF.
     */
    [[nodiscard]] static constexpr bool is_supplementary(char32_t const codePoint) noexcept {
        return codePoint > 0xFFFF && codePoint <= 0x10'FFFF;
    }

    /**
     * Check if it's a start of a Code Unit
     */
    template <typename T>
    [[nodiscard]] static constexpr bool is_code_unit_start(T unit) noexcept {
        if constexpr (UTF8<T>) {
            // byte 2, 3, and 4 all start with `0b10xx xxxx`
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
    [[nodiscard]] static constexpr bool is_in_range(char32_t const code_point) noexcept {
        return code_point <= max_legal_utf32;
    }

    /**
     * Check whether a Unicode code point is in a valid range.
     *
     * The first check comes from the Unicode guarantee to never encode
     * a point above 0x0010ffff, since UTF-16 couldn't represent it.
     *
     * The second check covers surrogate pairs (category Cs).
     */
    [[nodiscard]] static constexpr bool is_code_point_valid(char32_t const code_point) noexcept {
        using uu32 = stl::make_unsigned_t<char32_t>;
        return static_cast<stl::int32_t>(code_point) >= 0 && code_point <= max_legal_utf32 &&
               ((static_cast<uu32>(code_point) & 0xFFFF'F800U) != static_cast<uu32>(lead_surrogate_min));
        // alternative implementation:
        // return (cp <= max_legal_utf32<u32> && !is_surrogate(cp));
    }

    [[nodiscard]] static constexpr char32_t negate(char32_t const code_point) noexcept {
        return static_cast<char32_t>(-static_cast<stl::int32_t>(code_point));
    }

    /// Convert to a negative number if it's not already
    [[nodiscard]] static constexpr char32_t to_negative(char32_t const code_point) noexcept {
        auto const icp = static_cast<stl::int32_t>(code_point);
        return icp <= 0 ? code_point : static_cast<char32_t>(-icp);
    }

    /// Convert to a positive number if it's not already
    [[nodiscard]] static constexpr char32_t to_positive(char32_t const code_point) noexcept {
        auto const icp = static_cast<stl::int32_t>(code_point);
        return icp >= 0 ? code_point : static_cast<char32_t>(-icp);
    }

    [[nodiscard]] static constexpr bool is_negative(char32_t const code_point) noexcept {
        auto const icp = static_cast<stl::int32_t>(code_point);
        return icp < 0;
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
            auto const pos = static_cast<unsigned char>(code_unit);
            assert(pos < details::utf8_skip.size());
            return static_cast<SizeT>(details::utf8_skip.at(pos));
        } else {
            return 1U;
        }
    }

    /// utf8_length_from_utf32
    [[nodiscard]] static constexpr stl::size_t utf8_length_from_utf32(char32_t const code_point) noexcept {
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
        if (code_point <= max_legal_utf32) {
            return 4U;
        }

        [[unlikely]] {
            // Invalid UTF-32 character.
            return 0;
        }
    }

    /// utf16_length_from_utf32
    [[nodiscard]] static constexpr stl::size_t utf16_length_from_utf32(char32_t const code_point) noexcept {
        if (code_point > 0xFFFFU) {
            return 2U;
        }
        return 1U;
    }

    /// utf_length_from_utf32
    template <typename T>
    [[nodiscard]] static constexpr stl::size_t utf_length_from_utf32(char32_t const code_point) noexcept {
        if constexpr (UTF32<T>) {
            return 1U;
        } else if constexpr (UTF16<T>) {
            return utf16_length_from_utf32(code_point);
        } else if constexpr (UTF8<T>) {
            return utf8_length_from_utf32(code_point);
        } else {
            static_assert_false(T, "Invalid return type specified.");
            return 0U;
        }
    }

    template <UTF8 T>
    [[nodiscard]] static constexpr stl::size_t utf16_length_from_utf8(char32_t const code_unit) noexcept {
        stl::size_t length = 0U;

        // -65 is 0b10111111, anything larger in two-complement's should start a new code point
        if (static_cast<stl::int8_t>(code_unit) > static_cast<stl::int8_t>(0b1011'1111)) {
            ++length;
        }
        if (static_cast<T>(code_unit) >= 240) {
            ++length;
        }
        return length;
    }

    [[nodiscard]] static constexpr stl::size_t utf8_length_from_utf16(char16_t const code_unit) noexcept {
        if (is_surrogate(code_unit)) {
            // surrogate pairs are always 4 bytes.
            return 4U;
        }
        return utf8_length_from_utf32(static_cast<char32_t>(code_unit));
    }

    /// Count the required length to store this code unit
    /// Attention: this is a code unit, not a code point
    template <typename T, UTF CharT = char32_t>
    [[nodiscard]] static constexpr stl::size_t utf_length_from(CharT const code_unit) noexcept {
        if constexpr (UTF32<T>) {
            return 1U;
        } else if constexpr (UTF16<T>) {
            if constexpr (UTF32<CharT>) {
                return utf16_length_from_utf32(code_unit);
            } else if constexpr (UTF8<CharT>) {
                return utf16_length_from_utf8(code_unit);
            } else {
                return required_length_of(code_unit); // both are UTF-16
            }
        } else if constexpr (UTF8<T>) {
            if constexpr (UTF32<CharT>) {
                return utf8_length_from_utf32(code_unit);
            } else if constexpr (UTF16<CharT>) {
                return utf8_length_from_utf16(code_unit);
            } else {
                return required_length_of(code_unit); // both are UTF-8
            }
        } else {
            static_assert_false(T, "Invalid return type specified.");
            return 0U;
        }
    }

} // namespace webpp::unicode

// NOLINTEND(*-magic-numbers)

#endif // WEBPP_UNICODE_HPP
