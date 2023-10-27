// Created by moisrex on 12/26/20.

#ifndef WEBPP_UNICODE_HPP
#define WEBPP_UNICODE_HPP

#include "../std/type_traits.hpp"


// NOLINTBEGIN(*-magic-numbers)
namespace webpp::unicode {

    template <typename T>
    concept UTF8 = (sizeof(T) == sizeof(char8_t));

    template <typename T>
    concept UTF16 = (sizeof(T) == sizeof(char16_t));

    template <typename T>
    concept WChar = (sizeof(T) >= sizeof(char32_t));


    // Leading (high) surrogates: 0xd800 - 0xdbff
    // Trailing (low) surrogates: 0xdc00 - 0xdfff
    template <typename u16 = char16_t>
    static constexpr u16 lead_surrogate_min = 0xd800;

    template <typename u16 = char16_t>
    static constexpr u16 lead_surrogate_max = 0xdbff;

    template <typename u16 = char16_t>
    static constexpr u16 trail_surrogate_min = 0xdc00;

    template <typename u16 = char16_t>
    static constexpr u16 trail_surrogate_max = 0xdfff;

    template <typename u16 = char16_t>
    static constexpr u16 lead_offset = lead_surrogate_min<u16> - (0x10000 >> 10);

    template <typename u32 = char32_t>
    static constexpr u32 surrogate_offset =
      0x10000 - (lead_surrogate_min<u32> << 10) - trail_surrogate_min<u32>;

    // Max valid value for a unicode code point
    template <typename u32 = char32_t>
    static constexpr u32 code_point_max = 0x0010ffffu;

    template <typename u8 = char8_t, typename octet_type>
    static constexpr u8 mask8(octet_type oc) noexcept {
        return static_cast<u8>(0xff & oc);
    }

    template <typename u16 = char16_t, typename u16_type>
    static constexpr u16 mask16(u16_type oc) noexcept {
        return static_cast<u16>(0xffff & oc);
    }

    template <typename octet_type>
    static constexpr bool is_trail(octet_type oc) noexcept {
        return ((mask8(oc) >> 6) == 0x2);
    }

    template <typename u16>
    static constexpr bool is_lead_surrogate(u16 cp) noexcept {
        return (cp >= lead_surrogate_min<u16> && cp <= lead_surrogate_max<u16>);
    }

    template <typename u16>
    static constexpr bool is_trail_surrogate(u16 cp) noexcept {
        return (cp >= trail_surrogate_min<u16> && cp <= trail_surrogate_max<u16>);
    }

    template <typename u16>
    static constexpr bool is_surrogate(u16 cp) noexcept {
        return (cp >= lead_surrogate_min<u16> && cp <= trail_surrogate_max<u16>);
    }

    /*
     * Check whether a Unicode (5.2) char is in a valid range.
     *
     * The first check comes from the Unicode guarantee to never encode
     * a point above 0x0010ffff, since UTF-16 couldn't represent it.
     *
     * The second check covers surrogate pairs (category Cs).
     */
    template <typename u32>
    static constexpr bool is_code_point_valid(u32 cp) noexcept {
        return (cp) < 0x110000 && ((cp & 0xFFFFF800) != 0xD800);
        // alternative implementation:
        // return (cp <= code_point_max<u32> && !is_surrogate(cp));
    }


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
                auto const next_val = p + 1u;
                val |= *next_val;
                return val;
            }
            return static_cast<code_point_type>(val); // this is the only char
        } else if constexpr (is_utf8) {
            constexpr char_type shift_bit_count = sizeof(char8_t) * 8u;
            if ((val & 0x80u) == 0) {
                // we have one char
                return static_cast<code_point_type>(val);
            } else if ((val & 0xE0u) == 0xC0u) {
                // we have 2 chars
                val <<= shift_bit_count;
                auto const next_val = p + 1u;
                val |= *next_val;
                return val;
            } else if ((val & 0xF0u) == 0xE0u) {
                // we have 3 chars
                val <<= shift_bit_count;
                auto next_val = p + 1u;
                val |= *next_val;
                val <<= shift_bit_count;
                ++next_val;
                val |= *next_val;
                return val;
            } else if ((val & 0xF8u) == 0xF0u) {
                // we have 4 chars
                val <<= shift_bit_count;
                auto next_val = p + 1u;
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
        requires(stl::is_integral_v<value_type>)
    static constexpr auto count_bytes(value_type value) noexcept {
        if constexpr (UTF16<value_type>) {
            if ((value & 0xFC00u) == 0xD800u)
                return 2;
            return 1;
        } else if constexpr (UTF8<value_type>) {
            return value < 0x80
                     ? 1
                     : (value < 0x800
                          ? 2
                          : (value < 0x10000 ? 3 : (value < 0x200000 ? 4 : (value < 0x4000000 ? 5 : 6))));
            // alternative implementation:
            // if ((value & 0x80u) == 0) {
            //     return 1;
            // } else if ((value & 0xE0u) == 0xC0u) {
            //     return 2;
            // } else if ((value & 0xF0u) == 0xE0u) {
            //     return 3;
            // } else if ((value & 0xF8u) == 0xF0u) {
            //     return 4;
            // }
            // return 1;
        } else {
            return 1;
        }
    }



    namespace unchecked {

        namespace details {

            // from glib/gutf8.c
            // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
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
            // NOLINTEND(cppcoreguidelines-avoid-c-arrays)
        } // namespace details



        template <typename CharT = char8_t>
        static constexpr void next_char(CharT*& p) noexcept {
            if constexpr (UTF8<CharT>) {
                // alternative implementation:
                // for (++p; (*p & 0xc0) == 0x80; ++p) ;
                p += details::utf8_skip<CharT>[*p];
            } else if constexpr (UTF16<CharT>) {
                ++p;
                if (!(*p < 0xDC00 || *p > 0xDFFF))
                    ++p;
            } else {
                ++p;
            }
        }

        template <typename CharT = char8_t>
        static constexpr CharT* next_char_copy(CharT* p) noexcept {
            next_char<CharT>(p);
            return p;
        }

        /**
         * Go to the beginning of the previous character.
         * This function does not check if previous character exists or not or even if its a valid character.
         */
        template <typename CharT = char8_t>
        static constexpr void prev_char(CharT*& p) noexcept {
            if constexpr (UTF8<CharT>) {
                --p;
                if ((*p & 0xc0) != 0x80)
                    return;
                --p;
                if ((*p & 0xc0) != 0x80)
                    return;
                --p;
                if ((*p & 0xc0) != 0x80)
                    return;
                --p;
                if ((*p & 0xc0) != 0x80)
                    return;
                --p;
                if ((*p & 0xc0) != 0x80)
                    return;
                --p;
                if ((*p & 0xc0) != 0x80)
                    return;
            } else if constexpr (UTF16<CharT>) {
                --p;
                if (!(*p < 0xDC00 || *p > 0xDFFF))
                    --p;
            } else {
                --p;
            }
        }

        template <typename CharT = char8_t>
        static constexpr CharT* prev_char_copy(CharT* p) noexcept {
            prev_char<CharT>(p);
            return p;
        }




        template <typename CharT = char8_t>
        static constexpr stl::size_t count(CharT const* p, stl::size_t max) noexcept {
            if constexpr (UTF8<CharT> || UTF16<CharT>) {
                stl::size_t  len   = 0;
                const CharT* start = p;
                if (max == 0 || !*p)
                    return 0;

                next_char(p);

                while (p - start < max && *p) {
                    ++len;
                    next_char(p);
                }

                /* only do the last len increment if we got a complete
                 * char (don't count partial chars)
                 */
                if (p - start <= max)
                    ++len;
                return len;
            } else {
                // todo
            }
        }

        template <typename CharT = char8_t>
        static constexpr stl::size_t count(CharT const* start, CharT const* end) noexcept {
            if constexpr (UTF8<CharT> || UTF16<CharT>) {
                // todo
            } else {
                return end - start;
            }
        }

        // There's a better way to count 32bit unicode if you know the start and the end.
        template <typename CharT = char8_t>
        static constexpr stl::size_t count(CharT const* p) noexcept {
            stl::size_t len = 0;
            for (; *p; next_char(p))
                ++len;
            return len;
        }

        template <typename Ptr, typename CharT = char32_t>
            requires(!stl::is_const_v<Ptr>)
        static constexpr void append(Ptr& result, CharT cp) noexcept {
            using char_type = stl::remove_cvref_t<decltype(*result)>;
            if constexpr (UTF8<char_type>) {
                if (cp < 0x80) // one octet
                    *(result++) = static_cast<char_type>(cp);
                else if (cp < 0x800) { // two octets
                    *(result++) = static_cast<char_type>((cp >> 6) | 0xc0);
                    *(result++) = static_cast<char_type>((cp & 0x3f) | 0x80);
                } else if (cp < 0x10000) { // three octets
                    *(result++) = static_cast<char_type>((cp >> 12) | 0xe0);
                    *(result++) = static_cast<char_type>(((cp >> 6) & 0x3f) | 0x80);
                    *(result++) = static_cast<char_type>((cp & 0x3f) | 0x80);
                } else { // four octets
                    *(result++) = static_cast<char_type>((cp >> 18) | 0xf0);
                    *(result++) = static_cast<char_type>(((cp >> 12) & 0x3f) | 0x80);
                    *(result++) = static_cast<char_type>(((cp >> 6) & 0x3f) | 0x80);
                    *(result++) = static_cast<char_type>((cp & 0x3f) | 0x80);
                }
            } else if constexpr (UTF16<char_type>) {
                // todo
            } else { // for char32_t or others
                *(result++) = static_cast<char_type>(cp);
            }
        }

    } // namespace unchecked

    namespace checked {

        template <typename Ptr, typename CharT = char32_t>
            requires(!stl::is_const_v<Ptr>)
        static constexpr bool append(Ptr& result, CharT cp) noexcept {
            if (!is_code_point_valid(cp))
                return false;
            unchecked::append<Ptr, CharT>(result, cp);
            return true;
        }

    } // namespace checked

} // namespace webpp::unicode

// NOLINTEND(*-magic-numbers)

#endif // WEBPP_UNICODE_HPP
