// Created by moisrex on 12/22/25.

#ifndef WEBPP_UNICODE_ERROR_HANDLING_HPP
#define WEBPP_UNICODE_ERROR_HANDLING_HPP

#include "./unchecked.hpp"
#include "./unicode.hpp"

#include <limits>

namespace webpp::unicode {

    /**
     * Error Handling solution for how to handle invalid Unicode Code Points or invalid UTF encodings
     */
    enum struct [[nodiscard]] err_policy : stl::uint8_t {
        // Return U+FFFD which is called the replacement character
        // This is the standard way
        return_replacement,

        // Return whatever we find without any change (which may be a valid code but different Code Point).
        // This does nothing thus have the best performance
        leave_broken,

        // Return a negative integer value
        return_negated,

        // Return null character \0
        return_null_char,

        // Return 0x7FFFFFFF which is the maximum possible UTF-32 possible value
        return_max_utf32,

        // Return 0x10FFFF which is the maximum legal Unicode Code Point
        return_max_legal_utf32,

        // Return a negated and yet recoverable version of the invalid Code Point or invalidly encoded Code Point to its
        // original state
        return_recoverable,
    };

} // namespace webpp::unicode

namespace webpp::unicode::checked {

    /**
     * Handle Unicode UTF failures.
     *
     * @ref recover_error
     * @ref append_recovering
     */
    template <err_policy ErrPolicy, UTF CharT = char32_t>
    [[nodiscard]] static constexpr char32_t to_error(CharT const code_point) noexcept {
        using enum err_policy;
        if constexpr (ErrPolicy == return_replacement) {
            return replacement_char;
        } else if constexpr (ErrPolicy == return_max_utf32) {
            return max_utf32;
        } else if constexpr (ErrPolicy == return_max_legal_utf32) {
            return max_legal_utf32;
        } else if constexpr (ErrPolicy == return_negated) {
            return to_negative(code_point);
        } else if constexpr (ErrPolicy == return_null_char) {
            return U'\0';
        } else if constexpr (ErrPolicy == return_recoverable) {
            static constexpr stl::uint32_t negated                  = 0b1U << 31U;
            static constexpr stl::uint32_t mask                     = 0b111U << 29U;
            static constexpr stl::uint32_t utf8_identifier          = negated | (3U << 29U);
            static constexpr stl::uint32_t utf16_identifier         = negated | (2U << 29U);
            static constexpr stl::uint32_t utf32_identifier         = negated | (1U << 29U);
            // this identifier is for numbers that are bigger than we can handle
            static constexpr stl::uint32_t utf32_reverse_identifier = negated | (0U << 29U);

            auto icp = static_cast<std::uint32_t>(code_point);
            if constexpr (UTF8<CharT>) {
                assert((icp & mask) == 0);
                icp |= utf8_identifier;
            } else if (UTF16<CharT>) {
                assert((icp & mask) == 0);
                icp |= utf16_identifier;
            } else {
                if ((icp & mask) != 0) [[unlikely]] {
                    // The number is too big, but we know it's at least how big it is, so we just subtract that much
                    // from the number and later in recovery, we do the add this much back to recover the original.
                    icp -= ~mask;
                    icp |= utf32_reverse_identifier;
                } else {
                    icp |= utf32_identifier;
                }
            }
            return static_cast<char32_t>(icp);
        } else {
            return code_point;
        }
    }

    /**
     * Recover the Code Point which has gone through the `err_policy::return_recoverable` error system
     * @ref to_error
     * @ref append_recovering
     */
    template <UTF CharT>
    [[nodiscard]] static constexpr auto recover_error(char32_t const code_point) noexcept {
        static constexpr stl::uint32_t negated                  = 0b1U << 31U;
        static constexpr stl::uint32_t mask                     = 0b111U << 29U;
        static constexpr stl::uint32_t utf8_identifier          = negated | (3U << 29U);
        static constexpr stl::uint32_t utf16_identifier         = negated | (2U << 29U);
        static constexpr stl::uint32_t utf32_identifier         = negated | (1U << 29U);
        static constexpr stl::uint32_t utf32_reverse_identifier = negated | (0U << 29U);

        assert(static_cast<stl::int32_t>(code_point) < 0);
        auto       ucp       = static_cast<stl::uint32_t>(code_point);
        auto const ucp_mask  = ucp & mask;
        ucp                 &= ~mask;
        if constexpr (UTF8<CharT> || UTF16<CharT>) {
            stl::array<CharT, UTF8<CharT> ? 4U : 2U> units{};
            auto                                     ptr = units.data();
            switch (ucp_mask) {
                case utf8_identifier:
                    // Probably improperly encoded Code Point
                    assert(ucp <= std::numeric_limits<char8_t>::max());
                    units[0] = static_cast<CharT>(ucp);
                    break;
                case utf16_identifier:
                    assert(ucp <= std::numeric_limits<char16_t>::max());
                    unchecked::append(ptr, static_cast<char16_t>(ucp));
                    break;
                case utf32_identifier:
                    // Input is UTF32, but the output is UTF-8/16
                    unchecked::append(ptr, static_cast<char32_t>(ucp));
                    break;
                case utf32_reverse_identifier:
                    // Recover what we subtracted from the number in the encoding process
                    ucp += ~mask;
                    unchecked::append(ptr, static_cast<char32_t>(ucp));
                    break;
                default: stl::unreachable();
            }
            return units;
        } else {
            switch (ucp_mask) {
                case utf8_identifier:
                    // Probably improperly encoded Code Point
                    assert(ucp <= stl::numeric_limits<char8_t>::max());
                    return static_cast<CharT>(ucp);
                case utf16_identifier:
                    assert(ucp <= stl::numeric_limits<char16_t>::max());
                    return static_cast<CharT>(ucp);
                case utf32_identifier: return static_cast<CharT>(ucp);
                case utf32_reverse_identifier:
                    // Recover what we subtracted from the number in the encoding process
                    ucp += ~mask;
                    return static_cast<CharT>(ucp);
                default: stl::unreachable();
            }
        }
    }

    /**
     * Append the code point to the output, but also recover bad code points as well that have been encoded using
     * the `err_policy::return_recoverable` failure handling system.
     * @ref recover_error
     * @ref to_error
     */
    template <istl::Appendable Ptr>
    static constexpr stl::size_t append_recovering(Ptr& out, char32_t code_point) noexcept {
        using istl::iter_append;
        using char_type = istl::appendable_value_type_t<Ptr>;

        static constexpr stl::uint32_t mask = 0b111U << 29U;
        if ((static_cast<std::uint32_t>(code_point) & mask) != 0U) [[unlikely]] {
            // Append the error code points
            stl::size_t count = 0U;
            if constexpr (UTF8<char_type> || UTF16<char_type>) {
                auto const code_units = recover_error<char_type>(code_point);
                for (auto const unit : code_units) {
                    if (unit == 0) {
                        break;
                    }
                    iter_append(out, unit);
                    ++count;
                }
            } else {
                // UTF-32
                iter_append(out, recover_error<char_type>(code_point));
                ++count;
            }
            return count;
        }
        return unchecked::append(out, code_point);
    }

    template <err_policy Policy, istl::Appendable Ptr>
    static constexpr stl::size_t append(Ptr& out, char32_t code_point) noexcept {
        if constexpr (err_policy::return_recoverable == Policy) {
            return append_recovering(out, code_point);
        } else {
            return unchecked::append(out, code_point);
        }
    }
} // namespace webpp::unicode::checked

#endif // WEBPP_UNICODE_ERROR_HANDLING_HPP
