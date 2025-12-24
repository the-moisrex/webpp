// Created by moisrex on 12/22/25.

#ifndef WEBPP_UNICODE_CHECKED_HPP
#define WEBPP_UNICODE_CHECKED_HPP

#include "../std/string.hpp"
#include "./error_handling.hpp"

#include <algorithm> // todo: try removing this, it's a heavy header to include

// NOLINTBEGIN(*-magic-numbers)
namespace webpp::unicode::checked {

    template <typename Ptr, UTF CharT = char32_t>
    [[nodiscard("Use unicode::unchecked::append if the input codepoint is always valid.")]] static constexpr bool
    append(Ptr& out, CharT code_point) noexcept {
        if (!is_code_point_valid(static_cast<char32_t>(code_point))) {
            return false;
        }
        unchecked::append<Ptr, CharT>(out, code_point);
        return true;
    }

    template <err_policy ErrPolicy, UTF CharT = char32_t>
    [[nodiscard]] static constexpr char32_t validate_code_point(CharT const code_point) noexcept {
        auto const ccp = static_cast<char32_t>(code_point);
        return is_code_point_valid(ccp) ? ccp : to_error<ErrPolicy, CharT>(code_point);
    }

    /**
     * Next Unicode Code Point (Converted to UTF-32)
     */
    template <err_policy            ErrPolicy = err_policy::leave_broken,
              stl::forward_iterator Iter      = char8_t const*,
              typename EIter                  = char32_t const*>
        requires(stl::sentinel_for<EIter, Iter>)
    [[nodiscard]] static constexpr char32_t next_code_point(Iter& pos, EIter const& end) noexcept {
        using cp_type            = std::uint32_t;
        using char_type          = stl::iter_value_t<Iter>;
        using unsigned_char_type = stl::make_unsigned_t<char_type>;
        using difference_type    = stl::iter_difference_t<Iter>;

        static_assert(stl::bidirectional_iterator<Iter> || UTF32<char_type>, "Impl requirement is not met.");

        if (pos == end) {
            return U'\0'; // return \0 if we're at the end already
        }

        // We cast to unsigned in order to prevent converting for example negative `char` to still negative `char32_t`
        auto const code_unit  = static_cast<unsigned_char_type>(*pos++);
        auto const cu1        = static_cast<cp_type>(code_unit);
        auto       code_point = cu1;

        // We're in a constexpr land, we can't use goto.
        for (;;) {
            // double casting to make sure negative values can't come out of it
            if constexpr (UTF32<char_type>) {
                if constexpr (ErrPolicy != err_policy::leave_broken) {
                    if (!is_code_point_valid(code_point)) [[unlikely]] {
                        break;
                    }
                }
                return static_cast<char32_t>(code_point);
            } else if constexpr (UTF16<char_type>) {
                bool const is_leading_surrogate = (cu1 & 0xFC00U) == 0xD800U;
                bool       error                = false;
                if (is_leading_surrogate) {
                    if (pos == end) [[unlikely]] {
                        break;
                    }
                    auto const cu2    = static_cast<cp_type>(static_cast<unsigned_char_type>(*pos++));
                    error            |= (cu1 & 0xFC00U) != 0xD800U;
                    error            |= (cu2 & 0xFC00U) != 0xDC00U;
                    auto const lead   = cu1 & 0x3FFU;
                    auto const trail  = cu2 & 0x3FFU;
                    code_point        = (lead << 10U) + trail + 0x1'0000U;
                }
                if (error || is_surrogate(code_point)) [[unlikely]] {
                    if (is_leading_surrogate) {
                        --pos;
                    }
                    break;
                }
                return static_cast<char32_t>(code_point);
            } else if constexpr (UTF8<char_type>) {
                auto const len = required_length_of<char_type, difference_type>(static_cast<char_type>(cu1));
                if constexpr (stl::random_access_iterator<Iter>) {
                    if (end - pos < len - 1) [[unlikely]] {
                        break;
                    }
                }
                switch (len) {
                    [[likely]] case 1:
                        if ((cu1 & 0b1000'0000U) != 0) [[unlikely]] {
                            break;
                        }
                        return static_cast<char32_t>(cu1);
                    case 2: {
                        if constexpr (!stl::random_access_iterator<Iter>) {
                            if (pos == end) [[unlikely]] {
                                break;
                            }
                        }
                        auto const cu2     = static_cast<cp_type>(static_cast<unsigned_char_type>(*pos++));
                        bool       error   = (cu1 & 0b1110'0000U) != 0b1100'0000U;
                        error             |= (cu2 & 0b1100'0000U) != 0b1000'0000U;
                        code_point        &= 0b0001'1111U;
                        code_point       <<= 6U;
                        code_point        |= cu2 & 0b0011'1111U;
                        if (error || code_point < 0x80 || 0x7FF < code_point) [[unlikely]] {
                            --pos;
                            break;
                        }
                        return static_cast<char32_t>(code_point);
                    }
                    case 3: {
                        if constexpr (!stl::random_access_iterator<Iter>) {
                            if (pos == end) [[unlikely]] {
                                break;
                            }
                        }
                        auto const cu2 = static_cast<cp_type>(static_cast<unsigned_char_type>(*pos++));
                        if constexpr (!stl::random_access_iterator<Iter>) {
                            if (pos == end) [[unlikely]] {
                                break;
                            }
                        }
                        auto const cu3     = static_cast<cp_type>(static_cast<unsigned_char_type>(*pos++));
                        bool       error   = (cu1 & 0b1111'0000U) != 0b1110'0000;
                        error             |= (cu2 & 0b1100'0000U) != 0b1000'0000;
                        error             |= (cu3 & 0b1100'0000U) != 0b1000'0000;
                        code_point        &= 0b0000'1111U;
                        code_point       <<= 12U;
                        code_point        |= (cu2 & 0b0011'1111U) << 6U;
                        code_point        |= cu3 & 0b0011'1111U;
                        if (error || code_point < 0x800 || 0xFFFF < code_point ||
                            (0xD7FF < code_point && code_point < 0xE000)) [[unlikely]]
                        {
                            stl::advance(pos, -2);
                            break;
                        }
                        return static_cast<char32_t>(code_point);
                    }
                    case 4: {
                        if constexpr (!stl::random_access_iterator<Iter>) {
                            if (pos == end) [[unlikely]] {
                                break;
                            }
                        }
                        auto const cu2 = static_cast<cp_type>(static_cast<unsigned_char_type>(*pos++));
                        if constexpr (!stl::random_access_iterator<Iter>) {
                            if (pos == end) [[unlikely]] {
                                break;
                            }
                        }
                        auto const cu3 = static_cast<cp_type>(static_cast<unsigned_char_type>(*pos++));
                        if constexpr (!stl::random_access_iterator<Iter>) {
                            if (pos == end) [[unlikely]] {
                                break;
                            }
                        }
                        auto const cu4     = static_cast<cp_type>(static_cast<unsigned_char_type>(*pos++));
                        bool       error   = (cu1 & 0b1111'0000U) != 0b1111'0000;
                        error             |= (cu2 & 0b1100'0000U) != 0b1000'0000;
                        error             |= (cu3 & 0b1100'0000U) != 0b1000'0000;
                        error             |= (cu4 & 0b1100'0000U) != 0b1000'0000;
                        code_point        &= 0b0000'0111U;
                        code_point       <<= 18U;
                        code_point        |= (cu2 & 0b0011'1111U) << 12U;
                        code_point        |= (cu3 & 0b0011'1111U) << 6U;
                        code_point        |= cu4 & 0b0011'1111U;
                        if (error || code_point <= 0xFFFF || 0x10'FFFF < code_point) [[unlikely]] {
                            stl::advance(pos, -3);
                            break;
                        }
                        return static_cast<char32_t>(code_point);
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
        [[unlikely]] { return to_error<ErrPolicy>(code_unit); }
    }

    template <err_policy            ErrPolicy = err_policy::leave_broken,
              stl::forward_iterator Iter      = char8_t const*,
              typename EIter                  = Iter>
        requires stl::sentinel_for<EIter, Iter>
    [[nodiscard]] static constexpr char32_t next_code_point_copy(Iter pos, EIter const& end) noexcept {
        return next_code_point<ErrPolicy, Iter, EIter>(pos, end);
    }

    template <stl::forward_iterator Iter = char8_t*, typename EIter = Iter>
        requires stl::sentinel_for<EIter, Iter>
    static constexpr bool next_char(Iter& pos, EIter const& end) noexcept {
        using enum err_policy;
        // todo: is there a way to optimize this?
        static_cast<void>(next_code_point(pos, end));
        return pos != end;
    }

    namespace details {
        static constexpr stl::array<stl::uint8_t, 256> utf8_magic_lengths{
          // std::array<uint8_t, 256> lens;
          // lens.fill(1);
          // for (uint8_t cp = 0; cp != 0xFF; ++cp) {
          //     lens[(cp & ~0b11) | 0b00]               = 1;
          //     lens[(cp & ~0b11) | 0b01]               = 1;
          //     lens[(cp & ~0b1111) | 0b1110]           = 2;
          //     lens[(cp & ~0b11'1111) | 0b11'1010]     = 3;
          //     lens[(cp & ~0b1111'1111) | 0b1110'1010] = 4;
          // }
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 3, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 2,
        };
    } // namespace details

    template <err_policy                  ErrPolicy = err_policy::leave_broken,
              stl::bidirectional_iterator Iter      = char8_t const*,
              typename EIter                        = Iter>
        requires stl::sentinel_for<EIter, Iter>
    [[nodiscard]] static constexpr char32_t prev_code_point(Iter& pos, EIter const& beg) noexcept {
        using cp_type            = std::uint32_t;
        using char_type          = stl::iter_value_t<Iter>;
        using unsigned_char_type = stl::make_unsigned_t<char_type>;

        if (pos == beg) {
            return static_cast<cp_type>(static_cast<unsigned_char_type>(*pos));
        }

        // Last Code Unit of the code point:
        auto const code_unit  = static_cast<unsigned_char_type>(*--pos);
        auto const cu_last    = static_cast<cp_type>(code_unit);
        auto       code_point = cu_last;

        for (;;) {
            if constexpr (UTF16<char_type>) {
                // UTF-16 Encoding
                // byte1            | byte2            |
                // 0xxxxxxxxxxxxxxx |                  |
                // 110110xxxxxxxxxx | 110111xxxxxxxxxx |
                bool       error              = false;
                auto const trail              = cu_last; // This is the trailing surrogate we encountered
                bool const is_trail_surrogate = (trail & 0xFC00U) == 0xDC00;
                if (is_trail_surrogate) {
                    if (pos == beg) [[unlikely]] {
                        break;
                    }
                    auto const lead         = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));
                    error                  |= (lead & 0xFC00U) != 0xD800;
                    error                  |= (trail & 0xFC00U) != 0xDC00;
                    auto const lead_value   = lead & 0x3FFU;
                    auto const trail_value  = trail & 0x3FFU;
                    code_point              = (lead_value << 10U) + trail_value + 0x1'0000;
                }

                if (error || is_surrogate(code_point)) [[unlikely]] {
                    if (is_trail_surrogate) {
                        ++pos;
                    }
                    break;
                }
                return static_cast<char32_t>(code_point);
            } else if constexpr (UTF8<char_type>) {
                // byte1    | byte2    |  byte3   | byte4    |
                // 0xxxxxxx |          |          |          |
                // 110xxxxx | 10xxxxxx |          |          |
                // 1110xxxx | 10xxxxxx | 10xxxxxx |          |
                // 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |

                if (code_point >> 6U != 0b10) [[likely]] {
                    if (code_point >> 6U == 0b11) [[unlikely]] {
                        break;                                // invalid code point requires throwing error correctly
                    }
                    return static_cast<char32_t>(code_point); // 1 byte
                }

                auto const        cu4        = cu_last;
                cp_type           cu1        = 0;
                cp_type           cu2        = 0;
                cp_type           cu3        = 0;
                stl::uint_fast8_t magic_code = 0U;
                stl::uint8_t      length     = 0U;
                if constexpr (!stl::random_access_iterator<Iter>) {
                    // To make support for bidirectional iterators that are not random iterators
                    stl::uint8_t units = 4;
                    for (;;) {
                        magic_code |= (cu4 & 0b1100'0000U) >> 6U;
                        if (pos == beg) {
                            units = 1;
                            break;
                        }
                        cu3         = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));
                        magic_code |= (cu3 & 0b1100'0000U) >> 4U;
                        if (pos == beg) {
                            units = 2;
                            break;
                        }
                        cu2         = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));
                        magic_code |= (cu2 & 0b1100'0000U) >> 2U;
                        if (pos == beg) {
                            units = 3;
                            break;
                        }
                        cu1         = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));
                        magic_code |= cu1 & 0b1100'0000U;
                        break;
                    }

                    assert(magic_code < details::utf8_magic_lengths.size());
                    length = details::utf8_magic_lengths.at(magic_code);

                    stl::advance(pos, units - length);
                } else if (pos - beg >= 3) {
                    cu3 = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));
                    cu2 = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));
                    cu1 = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));

                    magic_code |= (cu4 & 0b1100'0000U) >> 6U;
                    magic_code |= (cu3 & 0b1100'0000U) >> 4U;
                    magic_code |= (cu2 & 0b1100'0000U) >> 2U;
                    magic_code |= static_cast<stl::uint_fast8_t>(cu1 & 0b1100'0000U);

                    assert(magic_code < details::utf8_magic_lengths.size());
                    length = details::utf8_magic_lengths.at(magic_code);

                    stl::advance(pos, 4 - length);
                } else {
                    stl::uint8_t units = 3;
                    for (;;) {
                        magic_code |= (cu4 & 0b1100'0000U) >> 6U;
                        if (pos == beg) {
                            units = 1;
                            break;
                        }
                        cu3         = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));
                        magic_code |= (cu3 & 0b1100'0000U) >> 4U;
                        if (pos == beg) {
                            units = 2;
                            break;
                        }
                        cu2         = static_cast<cp_type>(static_cast<unsigned_char_type>(*--pos));
                        magic_code |= (cu2 & 0b1100'0000U) >> 2U;
                        break;
                    }
                    assert(magic_code < details::utf8_magic_lengths.size());
                    length = details::utf8_magic_lengths.at(magic_code);

                    stl::advance(pos, units - length);
                }

                switch (length) {
                    [[unlikely]] case 0:
                        break;
                    case 1:
                        if (code_point >= 128U) [[unlikely]] {
                            break;
                        }
                        return code_point;
                        return static_cast<char32_t>(code_point);
                    case 2: {
                        code_point &= 0b0011'1111U;
                        code_point |= (cu3 & 0b0001'1111U) << 6U;
                        if ((cu3 & 0b1110'0000U) != 0b1100'0000 || code_point < 0x80 || 0x7FF < code_point) [[unlikely]]
                        {
                            ++pos;
                            break;
                        }
                        return static_cast<char32_t>(code_point);
                    }
                    case 3: {
                        code_point &= 0b0011'1111U;
                        code_point |= (cu3 & 0b0011'1111U) << 6U;
                        code_point |= (cu2 & 0b0000'1111U) << 12U;
                        if ((cu2 & 0b1111'0000U) != 0b1110'0000 || code_point < 0x800 || 0xFFFF < code_point ||
                            (0xD7FF < code_point && code_point < 0xE000)) [[unlikely]]
                        {
                            stl::advance(pos, 2);
                            break;
                        }
                        return static_cast<char32_t>(code_point);
                    }
                    case 4: {
                        code_point &= 0b0011'1111U;
                        code_point |= (cu3 & 0b0011'1111U) << 6U;
                        code_point |= (cu2 & 0b0011'1111U) << 12U;
                        code_point |= (cu1 & 0b0000'0111U) << 18U;
                        if ((cu1 & 0b1111'0000U) != 0b1111'0000 || code_point <= 0xFFFF || 0x10'FFFF < code_point)
                          [[unlikely]]
                        {
                            stl::advance(pos, 3);
                            break;
                        }
                        return static_cast<char32_t>(code_point);
                    }
                    default: stl::unreachable();
                }
            } else {
                // UTF-32 is trivial
                if constexpr (ErrPolicy != err_policy::leave_broken) {
                    if (!is_code_point_valid(code_point)) [[unlikely]] {
                        break;
                    }
                }
                return static_cast<char32_t>(code_point);
            }
            break;
        }

        // handle errors:
        [[unlikely]] { return to_error<ErrPolicy>(code_unit); }
    }

    template <err_policy                  ErrPolicy = err_policy::leave_broken,
              stl::bidirectional_iterator Iter      = char8_t const*,
              typename EIter                        = Iter>
        requires stl::sentinel_for<EIter, Iter>
    [[nodiscard]] static constexpr char32_t prev_code_point_copy(Iter pos, EIter const& beg) noexcept {
        return prev_code_point<ErrPolicy, Iter, EIter>(pos, beg);
    }

    /// Length of Code Units in current Code Point:
    ///   Safely check the length of the current code point that the iterator is pointing to even if
    ///   the values are not a valid code point (upon which we return the length of 1).
    template <stl::bidirectional_iterator Iter = char8_t const*, typename EIter = Iter>
        requires stl::sentinel_for<EIter, Iter>
    [[nodiscard]] static constexpr stl::size_t code_point_length(Iter pos, EIter const& end) noexcept {
        using value_type = stl::iter_value_t<Iter>;
        if (pos == end) {
            return 0U;
        }

        if constexpr (UTF32<value_type>) {
            return 1U;
        } else if constexpr (UTF16<value_type>) {
            // UTF-16 Encoding
            // byte1            | byte2            |
            // 0xxxxxxxxxxxxxxx |                  |
            // 110110xxxxxxxxxx | 110111xxxxxxxxxx |
            auto const cu1 = *pos++;
            if (pos == end) [[unlikely]] {
                return 1U; // it doesn't matter if it's valid or not
            }
            auto const cu2         = *pos;
            auto const clamped_CUs = (cu1 & 0xFC00) | (cu2 >> 10U);
            return clamped_CUs == 0xD837 ? 2 : 1;
        } else if constexpr (UTF8<value_type>) {
            // todo: find a better way of doing this:
            auto const beg = pos;
            static_cast<void>(next_char<Iter>(pos, end));
            return static_cast<stl::size_t>(pos - beg);
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
    static constexpr void insert(StrT& out, stl::size_t const index, CharT const val) {
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
    /// Returns the number of Code Units appended.
    template <stl::random_access_iterator OIterT = stl::u8string::iterator, UTF CharT = char32_t>
    static constexpr stl::size_t insert(OIterT& out, OIterT const oend, stl::size_t index, CharT val) noexcept {
        using out_char_type = stl::iter_value_t<OIterT>;
        using diff_type     = stl::iter_difference_t<OIterT>;

        // move 'out' to the insertion position
        advance(out, oend, index);

        // determine how many code units we must make room for
        auto const len = static_cast<diff_type>(utf_length_from<out_char_type>(val));

        // shift the tail right by 'len' units using a backward copy to avoid overlap corruption
        // copy_backward(src_first, src_last, dest_last) copies [src_first, src_last) to
        // the range ending at dest_last (so dest_last - (src_last - src_first) .. dest_last)
        // We want to move [out, oend) to start at out + len, so destination end is oend + len.
        stl::copy_backward(out, oend, stl::next(oend, len));

        // Now 'out' still points at insertion position; write the code point there.
        return unchecked::append(out, val);
    }

    /// Attention: this function will remove the last Code Point
    /// Returns the number of Code Units appended.
    template <stl::random_access_iterator OIterT = stl::u8string::iterator, UTF CharT = char32_t>
    static constexpr stl::size_t insert_at(OIterT out, OIterT const oend, stl::size_t index, CharT val) noexcept {
        return insert(out, oend, index, val);
    }

    /// Attention: this function moves things pass the specified output length
    /// Returns the number of Code Units appended.
    template <stl::random_access_iterator OIterT = stl::u8string::iterator, UTF CharT = char32_t>
    static constexpr stl::size_t
    insert_between_at(OIterT out, stl::size_t const out_len, stl::size_t const index, CharT const val) noexcept {
        using out_char_type = stl::iter_value_t<OIterT>;
        using diff_type     = stl::iter_difference_t<OIterT>;

        // determine how many code units we must make room for
        auto const cp_len = static_cast<diff_type>(utf_length_from<out_char_type>(val));
        assert(cp_len >= 0 && cp_len <= 4);
        auto const oend = stl::next(out, static_cast<diff_type>(out_len) + cp_len);

        advance(out, oend, index);
        stl::copy_backward(out, oend, stl::next(oend, cp_len));

        // Now 'out' still points at insertion position; write the code point there.
        return unchecked::append(out, val);
    }
} // namespace webpp::unicode::checked

// NOLINTEND(*-magic-numbers)

#endif // WEBPP_UNICODE_CHECKED_HPP
