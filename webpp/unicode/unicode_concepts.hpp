// Created by moisrex on 4/14/24.

#ifndef WEBPP_UNICODE_UNICODE_CONCEPTS_HPP
#define WEBPP_UNICODE_UNICODE_CONCEPTS_HPP

#include "../std/concepts.hpp"

namespace webpp::unicode {

    template <typename T>
    concept UTFCharType = stl::integral<T>;

    template <typename T>
    concept UTF8 = UTFCharType<T> && sizeof(T) == sizeof(char8_t);

    template <typename T>
    concept UTF16 = UTFCharType<T> && sizeof(T) == sizeof(char16_t);

    template <typename T>
    concept WChar = UTFCharType<T> && sizeof(T) >= sizeof(wchar_t);

    template <typename T>
    concept UTF32 = UTFCharType<T> && sizeof(T) >= sizeof(char32_t);

    template <typename T>
    concept UTF = UTF8<T> || UTF16<T> || UTF32<T>;

    template <typename T>
    concept UTF8String = UTF8<typename T::value_type>;

    template <typename T>
    concept UTF16String = UTF16<typename T::value_type>;

    template <typename T>
    concept UTF32String = UTF32<typename T::value_type>;


    // Leading (high) surrogates: 0xd800 - 0xdbff
    // Trailing (low) surrogates: 0xdc00 - 0xdfff
    static constexpr char16_t lead_surrogate_min  = 0xD800;
    static constexpr char16_t lead_surrogate_max  = 0xDBFF;
    static constexpr char16_t trail_surrogate_min = 0xDC00;
    static constexpr char16_t trail_surrogate_max = 0xDFFF;
    static constexpr char16_t lead_offset         = lead_surrogate_min - (0x1'0000U >> 10U);
    static constexpr char32_t surrogate_offset =
      0x1'0000 - (static_cast<char32_t>(lead_surrogate_min) << 10U) - static_cast<char32_t>(trail_surrogate_min);

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
    static constexpr char32_t replacement_char = 0x0000'FFFD;

    /// Basic Multilingual Plane (BMP)
    static constexpr char32_t max_bmp = 0x0000'FFFF;

    static constexpr char32_t max_utf16 = 0x0010'FFFF;
    static constexpr char32_t max_utf32 = 0x7FFF'FFFF;

    /// Max valid value for a Unicode code point
    static constexpr char32_t max_legal_utf32 = 0x0010'FFFF;

    static constexpr int  half_shift = 10; // used for shifting by 10 bits
    static constexpr auto half_base  = 0x001'0000UL;
    static constexpr auto half_mask  = 0x3FFUL;

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_UNICODE_CONCEPTS_HPP
