// Created by moisrex on 12/26/20.

#ifndef WEBPP_UNICODE_HPP
#define WEBPP_UNICODE_HPP

#include "../common/meta.hpp"
#include "../std/iterator.hpp"
#include "../std/string_concepts.hpp"
#include "../std/type_traits.hpp"
#include "./unicode_concepts.hpp"

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

    template <typename u32 = char32_t>
    static constexpr u32 surrogate_offset =
      0x1'0000 - (lead_surrogate_min<u32> << 10) - trail_surrogate_min<u32>;

    template <typename u32 = char32_t>
    static constexpr u32 replacement_char = 0x0000'FFFD;

    /// Basic Multilingual Plane (BMP)
    template <typename u32 = char32_t>
    static constexpr u32 max_bmp = 0x0000'FFFF;

    template <typename u32 = char32_t>
    static constexpr u32 max_utf16 = 0x0010'FFFF;

    template <typename u32 = char32_t>
    static constexpr u32 max_utf32 = 0x7FFF'FFFF;

    /// Max valid value for a Unicode code point
    template <typename u32 = char32_t>
    static constexpr u32 max_legal_utf32 = 0x0010'FFFF;

    static constexpr int  half_shift = 10; // used for shifting by 10 bits
    static constexpr auto half_base  = 0x001'0000UL;
    static constexpr auto half_mask  = 0x3FFUL;

    template <typename u8 = char8_t, typename octet_type>
    [[nodiscard]] static constexpr u8 mask8(octet_type oct) noexcept {
        return static_cast<u8>(0xff & oct);
    }

    template <typename u16 = char16_t, typename u16_type>
    [[nodiscard]] static constexpr u16 mask16(u16_type oct) noexcept {
        return static_cast<u16>(0xffff & oct);
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

    /*
     * Check whether a Unicode (5.2) char is in a valid range.
     *
     * The first check comes from the Unicode guarantee to never encode
     * a point above 0x0010ffff, since UTF-16 couldn't represent it.
     *
     * The second check covers surrogate pairs (category Cs).
     */
    template <typename u32>
    [[nodiscard]] static constexpr bool is_code_point_valid(u32 code_point) noexcept {
        using uu32 = stl::make_unsigned_t<u32>;
        return code_point < static_cast<u32>(0x11'0000U) &&
               ((static_cast<uu32>(code_point) & 0xFFFF'F800U) != lead_surrogate_min<uu32>);
        // alternative implementation:
        // return (cp <= max_legal_utf32<u32> && !is_surrogate(cp));
    }

    // todo: check out the glib/gutf8.c implementation
    template <stl::forward_iterator Iter = char8_t const*, UTF32 CodePointType = char32_t>
    [[nodiscard]] static constexpr CodePointType next_code_point(Iter& pos) noexcept {
        using code_point_type = CodePointType;
        using char_type       = typename stl::iterator_traits<Iter>::value_type;

        auto val = static_cast<code_point_type>(*pos++);
        if constexpr (UTF16<char_type>) {
            if ((val & 0xFC00U) == 0xD800U) {
                // we have two chars
                val <<= sizeof(char16_t) * 8U;
                val  |= static_cast<code_point_type>(*pos++);
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
                val  <<= 18U;
                val  |= (static_cast<code_point_type>(*pos++) & 0b0011'1111U) << 12U;
                val   |= (static_cast<code_point_type>(*pos++) & 0b0011'1111U) << 6U;
                val   |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                return val;
            }
            return val; // return this one anyway
        } else {
            return val;
        }
    }

    template <stl::forward_iterator Iter          = char8_t const*,
              stl::forward_iterator EIter         = Iter,
              UTF32                 CodePointType = char32_t>
    [[nodiscard]] static constexpr CodePointType next_code_point(Iter& pos, EIter end) noexcept {
        using code_point_type = CodePointType;
        using char_type       = typename stl::iterator_traits<Iter>::value_type;

        if (pos == end) {
            return static_cast<code_point_type>(0);
        }

        auto val = static_cast<code_point_type>(*pos++);
        if (pos == end) {
            return val;
        }
        if constexpr (UTF16<char_type>) {
            if ((val & 0xFC00U) == 0xD800U) {
                // we have two chars
                val <<= sizeof(char16_t) * 8U;
                val  |= static_cast<code_point_type>(*pos++);
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
                if (++pos != end) {
                    val |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                }
                return val;
            }
            if ((val & 0b1111'1000U) == 0b1111'0000U) {
                // we have 4 chars
                val  &= 0b0000'0111U;
                val  <<= 18U;
                val  |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 12U;
                if (++pos != end) {
                    val |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 6U;
                    if (++pos != end) {
                        val |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                    }
                }
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

    template <stl::forward_iterator Iter  = char8_t const*,
              stl::forward_iterator EIter = Iter,
              typename CodePointType      = char32_t>
    [[nodiscard]] static constexpr CodePointType next_code_point_copy(Iter pos, EIter end) noexcept {
        return next_code_point<Iter, EIter, CodePointType>(pos, end);
    }

    template <stl::bidirectional_iterator Iter = char8_t const*, UTF32 CodePointType = char32_t>
    [[nodiscard]] static constexpr CodePointType prev_code_point(Iter& pos) noexcept {
        using code_point_type = CodePointType;
        using char_type       = typename stl::iterator_traits<Iter>::value_type;

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
            val &= 0b0000'0011'1111'1111U;

            // now let's add the second byte:
            val |= (static_cast<code_point_type>(*pos) & 0b0000'0011'1111'1111U) << 10U;

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

    namespace details {

        // from glib/gutf8.c
        // NOLINTBEGIN(*-avoid-c-arrays)
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
        // NOLINTEND(*-avoid-c-arrays)
    } // namespace details

    template <typename value_type>
        requires(stl::is_integral_v<value_type>)
    [[nodiscard]] static constexpr auto count_bytes(value_type value) noexcept {
        if constexpr (UTF16<value_type>) {
            if ((value & 0xFC00U) == 0xD800U) {
                return 2;
            }
            return 1;
        } else if constexpr (UTF8<value_type>) {
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
            return details::utf8_skip<value_type>[value];
        } else {
            return 1;
        }
    }

    namespace unchecked {

        template <stl::forward_iterator Iter = char8_t*>
        static constexpr void next_char(Iter& pos) noexcept {
            using char_type = typename stl::iterator_traits<Iter>::value_type;
            if constexpr (UTF8<char_type>) {
                // alternative implementation:
                // for (++p; (*p & 0xc0) == 0x80; ++p) ;
                pos += details::utf8_skip<char_type>[*pos];
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
        static constexpr void next_char(Iter& pos, stl::size_t count) noexcept {
            using char_type = typename stl::iterator_traits<Iter>::value_type;
            if constexpr (UTF8<char_type> || UTF16<char_type>) {
                while (count != 0) {
                    next_char(pos);
                    --count;
                }
            } else {
                pos += count;
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
         * This function does not check if previous character exists or not or even if it's a valid character.
         */
        template <stl::bidirectional_iterator Iter = char8_t const*>
        static constexpr void prev_char(Iter& pos) noexcept {
            using char_type = typename stl::iterator_traits<Iter>::value_type;
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
            using char_type = typename stl::iterator_traits<Iter>::value_type;
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
        static constexpr void swap_code_points(Iter lhs, Iter rhs) noexcept(
          stl::is_nothrow_swappable_v<typename stl::iterator_traits<Iter>::value_type>) {
            using stl::swap;
            using char_type = typename stl::iterator_traits<Iter>::value_type;

            if constexpr (UTF8<char_type> || UTF16<char_type>) {
                if (lhs > rhs) {
                    swap(lhs, rhs);
                }
                auto const lhs_length = count_bytes(*lhs);
                auto const rhs_length = count_bytes(*rhs);
                if constexpr (UTF8<char_type>) {
                    webpp_assume(lhs_length >= 0 && lhs_length <= 6U);
                    webpp_assume(rhs_length >= 0 && rhs_length <= 6U);
                } else {
                    // utf-16
                    webpp_assume(lhs_length >= 0 && lhs_length <= 2U);
                    webpp_assume(rhs_length >= 0 && rhs_length <= 2U);
                }

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

        template <typename CharT = char8_t>
        [[nodiscard]] static constexpr stl::size_t count(CharT const* pos, stl::size_t max) noexcept {
            if constexpr (UTF8<CharT> || UTF16<CharT>) {
                stl::size_t  len   = 0;
                CharT const* start = pos;
                if (max == 0 || !*pos) {
                    return 0;
                }

                next_char(pos);

                while (pos - start < max && *pos) {
                    ++len;
                    next_char(pos);
                }

                /* only do the last len increment if we got a complete
                 * char (don't count partial chars)
                 */
                if (pos - start <= max) {
                    ++len;
                }
                return len;
            } else {
                // todo
            }
        }

        template <typename CharT = char8_t>
        [[nodiscard]] static constexpr stl::size_t count(CharT const* start, CharT const* end) noexcept {
            if constexpr (UTF8<CharT> || UTF16<CharT>) {
                // todo
            } else {
                return end - start;
            }
        }

        // There's a better way to count 32bit Unicode if you know the start and the end.
        template <typename CharT = char8_t>
        [[nodiscard]] static constexpr stl::size_t count(CharT const* pos) noexcept {
            stl::size_t len = 0;
            for (; *pos; next_char(pos)) {
                ++len;
            }
            return len;
        }

        /**
         * Append a code point to a string
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
                    if (ccp <= max_bmp<char_type>) {
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
                    if (code_point & 0xff80) {
                        if (code_point & 0xf800) {
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
            using src_char_type = typename stl::iterator_traits<Iter>::value_type;
            if constexpr (sizeof(src_char_type) >= sizeof(out_char_type)) {
                // no need to convert to UTF32 then convert to whatever
                return append<StrT, SizeT>(out, *src++);
            } else {
                return append<StrT, SizeT>(out, next_code_point(src));
            }
        }

        template <istl::Appendable      StrT,
                  stl::integral         SizeT = istl::size_type_of_t<StrT>,
                  stl::forward_iterator Iter  = char32_t const*,
                  stl::forward_iterator EIter = Iter>
        static constexpr SizeT append(StrT& out, Iter& src, EIter end) noexcept(
          istl::NothrowAppendable<StrT>) {
            using out_char_type = istl::char_traits_type_of<StrT>;
            using src_char_type = typename stl::iterator_traits<Iter>::value_type;
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
        [[nodiscard]] static constexpr StrT to(CharT const code_point, Args&&... args) noexcept(
          istl::NothrowAppendable<StrT>) {
            StrT str{stl::forward<Args>(args)...};
            auto iter = istl::appendable_iter_of(str);
            append(iter, code_point);
            return str;
        }

    } // namespace unchecked

    namespace checked {

        template <typename Ptr, stl::integral CharT = char32_t>
        [[nodiscard(
          "Use unicode::unchecked::append if the input codepoint is always valid.")]] static constexpr bool
        append(Ptr& out, CharT code_point) noexcept {
            if (!is_code_point_valid(code_point)) {
                return false;
            }
            unchecked::append<Ptr, CharT>(out, code_point);
            return true;
        }

    } // namespace checked

} // namespace webpp::unicode

// NOLINTEND(*-magic-numbers)

#endif // WEBPP_UNICODE_HPP
