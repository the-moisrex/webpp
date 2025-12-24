// Created by moisrex on 12/22/25.

#ifndef WEBPP_UNICODE_UNCHECKED_HPP
#define WEBPP_UNICODE_UNCHECKED_HPP

#include "../common/meta.hpp"
#include "../std/iterator.hpp"
#include "./unicode.hpp"

// NOLINTBEGIN(*-magic-numbers)
namespace webpp::unicode::unchecked {

    // todo: check out the glib/gutf8.c implementation
    template <stl::forward_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr char32_t next_code_point(Iter& pos) noexcept {
        using code_point_type    = char32_t;
        using char_type          = stl::iter_value_t<Iter>;
        using unsigned_char_type = stl::make_unsigned_t<char_type>;

        // double casting to make sure negative values can't come out of it
        auto val = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
        if constexpr (UTF16<char_type>) {
            // Check if it's a leading surrogate (0xD800-0xDBFF)
            if ((val & 0xFC00U) == 0xD800U) {
                // we have two code units (surrogate pair)
                auto const lead  = val & 0x3FFU;
                auto const trail = static_cast<code_point_type>(*pos++) & 0x3FFU;
                val              = (lead << 10U) + trail + 0x1'0000U;
                return val;
            }
            return val; // this is a single code unit
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

    template <stl::forward_iterator Iter = char8_t const*, typename EIter = Iter>
        requires stl::sentinel_for<EIter, Iter>
    [[nodiscard]] static constexpr char32_t next_code_point(Iter& pos, EIter end) noexcept {
        using code_point_type    = char32_t;
        using char_type          = stl::iter_value_t<Iter>;
        using unsigned_char_type = stl::make_unsigned_t<char_type>;

        static_assert(stl::bidirectional_iterator<Iter> || UTF32<char_type>, "Impl requirements are not met.");

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

    template <stl::forward_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr char32_t next_code_point_copy(Iter pos) noexcept {
        return next_code_point<Iter>(pos);
    }

    template <stl::forward_iterator Iter = char8_t const*, typename EIter = Iter>
        requires stl::sentinel_for<EIter, Iter>
    [[nodiscard]] static constexpr char32_t next_code_point_copy(Iter pos, EIter end) noexcept {
        return next_code_point<Iter, EIter>(pos, end);
    }

    template <stl::bidirectional_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr char32_t prev_code_point(Iter& pos) noexcept {
        using code_point_type = char32_t;
        using char_type       = stl::iter_value_t<Iter>;

        auto val = static_cast<code_point_type>(*--pos);
        if constexpr (UTF16<char_type>) {
            // UTF-16 Encoding
            // byte1            | byte2            |
            // 0xxxxxxxxxxxxxxx |                  |
            // 110110xxxxxxxxxx | 110111xxxxxxxxxx |

            // When going backwards, we encounter the trailing surrogate first
            // If it's not a trailing surrogate, then it's a single code unit
            if ((val & 0xFC00U) != 0xDC00U) {
                return val; // 1 code unit
            }

            // It's a trailing surrogate, so we need to get the leading surrogate
            auto const trail = val & 0x3FFU;

            // Move to the previous code unit (the leading surrogate)
            auto const lead = static_cast<code_point_type>(*--pos) & 0x3FFU;

            // Reconstruct the full code point
            val = (lead << 10U) + trail + 0x1'0000U;

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

    template <stl::bidirectional_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr char32_t prev_code_point_copy(Iter pos) noexcept {
        return prev_code_point<Iter>(pos);
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
            if (is_trail_surrogate(*ito)) {
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
    static constexpr stl::size_t copy_next_into(Iter& ito, Iter2 const& from) noexcept(istl::NothrowAppendable<Iter>) {
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
            pos                 += details::utf8_skip[static_cast<unsigned_type>(*pos)];
        } else if constexpr (UTF16<char_type>) {
            ++pos;
            if (is_trail_surrogate(*pos)) {
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
            auto const len      = static_cast<difference_type>(details::utf8_skip[static_cast<unsigned_type>(*pos)]);
            if (end - pos < len) {
                ++pos;
                return false;
            }
            stl::advance(pos, len);
        } else if constexpr (UTF16<char_type>) {
            ++pos;
            if (is_trail_surrogate(*pos)) {
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
            if (is_trail_surrogate(*pos)) {
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
    template <istl::Appendable StrT, UTF CharT = char32_t>
    static constexpr stl::size_t append(StrT& out, CharT code_point) noexcept(istl::NothrowAppendable<StrT>) {
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
                if (ccp <= static_cast<stl::uint32_t>(max_bmp)) {
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
                        iter_append(out, 0xE0U | code_point >> 12U);
                        iter_append(out, 0x80U | ((code_point >> 6U) & 0x3FU));
                        iter_append(out, 0x80U | (code_point & 0x3FU));
                        return 3U;
                    }

                    // UCS-2 = U+0080 - U+07FF -> UTF-8 (2 bytes)
                    iter_append(out, 0xC0U | code_point >> 6U);
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

    template <istl::Appendable StrT, stl::forward_iterator Iter = char32_t const*>
    static constexpr stl::size_t append(StrT& out, Iter& src) noexcept(istl::NothrowAppendable<StrT>) {
        using out_char_type = istl::appendable_value_type_t<StrT>;
        using src_char_type = stl::iter_value_t<Iter>;
        if constexpr (UTF32<src_char_type>) {
            return append(out, *src++);
        } else if constexpr (sizeof(src_char_type) == sizeof(out_char_type)) {
            return unchecked::copy_next_into(out, src);
        } else {
            return append(out, next_code_point(src));
        }
    }

    template <istl::Appendable StrT, stl::forward_iterator Iter = char32_t const*>
    static constexpr stl::size_t append(StrT& out, Iter const& src) noexcept(istl::NothrowAppendable<StrT>) {
        using out_char_type = istl::appendable_value_type_t<StrT>;
        using src_char_type = stl::iter_value_t<Iter>;
        if constexpr (UTF32<src_char_type>) {
            return append<StrT>(out, *src);
        } else if constexpr (sizeof(src_char_type) == sizeof(out_char_type)) {
            return unchecked::copy_next_into(out, src);
        } else {
            return append<StrT>(out, next_code_point(src));
        }
    }

    template <istl::Appendable StrT, stl::forward_iterator Iter = char32_t const*, typename EIter = Iter>
        requires stl::sentinel_for<EIter, Iter>
    static constexpr stl::size_t append(StrT& out, Iter& src, EIter end) noexcept(istl::NothrowAppendable<StrT>) {
        using out_char_type = istl::char_traits_type_of_t<StrT>;
        using src_char_type = stl::iter_value_t<Iter>;
        if constexpr (sizeof(src_char_type) >= sizeof(out_char_type)) {
            // no need to convert to UTF32 then convert to whatever
            return append<StrT>(out, *src++);
        } else {
            return append<StrT>(out, next_code_point(src, end));
        }
    }

    template <istl::AppendableStorage StrT = std::array<char8_t, 4UL>, UTF CharT = char32_t, typename... Args>
    [[nodiscard]] static constexpr StrT to(CharT const code_point, Args&&... args)
      noexcept(istl::NothrowAppendable<StrT>) {
        StrT str{stl::forward<Args>(args)...};
        auto iter = istl::appendable_iter_of(str);
        append(iter, code_point);
        return str;
    }
} // namespace webpp::unicode::unchecked

// NOLINTEND(*-magic-numbers)

#endif // WEBPP_UNICODE_UNCHECKED_HPP
