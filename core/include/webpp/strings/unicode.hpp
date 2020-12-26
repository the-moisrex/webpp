// Created by moisrex on 12/26/20.

#ifndef WEBPP_UNICODE_HPP
#define WEBPP_UNICODE_HPP

#include "../std/type_traits.hpp"


namespace webpp::unicode {

    template <typename T>
    concept UTF8 = (sizeof(T) == sizeof(char8_t));

    template <typename T>
    concept UTF16 = (sizeof(T) == sizeof(char16_t));

    template <typename T>
    concept WChar = (sizeof(T) >= sizeof(char32_t));


    // todo: check out the glib/gutf8.c implementation
    template <typename CharT = char8_t, typename CodePointType = char32_t>
    static constexpr CodePointType code_point(CharT const* const p) noexcept {
        using code_point_type   = CodePointType;
        using char_type         = CharT;
        constexpr auto is_utf8  = UTF8<char_type>;
        constexpr auto is_utf16 = UTF16<char_type>;

        auto val = *p;
        if constexpr (is_utf16) {
            if ((val & 0xFC00u) == 0xD800u) {
                // we have two chars
                val <<= sizeof(char16_t) * 8u;
                auto const next_val = p + 1;
                val |= *next_val;
                return val;
            }
            return static_cast<code_point_type>(val); // this is the only char
        } else if constexpr (is_utf8) {
            constexpr auto shift_bit_count = sizeof(char8_t) * 8u;
            if ((val & 0x80u) == 0) {
                // we have one char
                return static_cast<code_point_type>(val);
            } else if ((val & 0xE0u) == 0xC0u) {
                // we have 2 chars
                val <<= shift_bit_count;
                auto const next_val = p + 1;
                val |= *next_val;
                return val;
            } else if ((val & 0xF0u) == 0xE0u) {
                // we have 3 chars
                val <<= shift_bit_count;
                auto next_val = p + 1;
                val |= *next_val;
                val <<= shift_bit_count;
                ++next_val;
                val |= *next_val;
                return val;
            } else if ((val & 0xF8u) == 0xF0u) {
                // we have 4 chars
                val <<= shift_bit_count;
                auto next_val = p + 1;
                val |= *next_val;
                val <<= shift_bit_count;
                ++next_val;
                val |= *next_val;
                val <<= shift_bit_count;
                ++next_val;
                val |= *next_val;
                return val;
            }
            return val; // return this one anyway
        } else {
            return val;
        }
    }


    template <typename value_type>
    requires(stl::is_integral_v<value_type>) static constexpr stl::size_t
      count_bytes(value_type value) noexcept {
        if constexpr (sizeof(value_type) == sizeof(char16_t)) {
            if ((value & 0xFC00u) == 0xD800u)
                return 2;
            return 1;
        } else if constexpr (sizeof(value_type) == sizeof(char8_t)) {
            if ((value & 0x80u) == 0) {
                return 1;
            } else if ((value & 0xE0u) == 0xC0u) {
                return 2;
            } else if ((value & 0xF0u) == 0xE0u) {
                return 3;
            } else if ((value & 0xF8u) == 0xF0u) {
                return 4;
            }
            return 1;
        } else {
            return 1;
        }
    }



    namespace details {

        // from glib/gutf8.c
        template <typename CharT = char8_t>
        static constexpr CharT utf8_skip[256] = {
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
          3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1};


    } // namespace details

    template <typename T = char8_t>
    requires(!stl::is_const_v<T>) static inline void advance(T*& p) noexcept {
        if constexpr (UTF8<T>) {
            p += details::utf8_skip<T>[*p];
        } else if constexpr (UTF16<T>) {
            // todo
        } else {
            ++p;
        }
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_HPP
