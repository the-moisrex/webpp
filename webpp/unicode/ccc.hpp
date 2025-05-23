// Created by moisrex on 4/3/25.

#ifndef WEBPP_UNICODE_CCC_HPP
#define WEBPP_UNICODE_CCC_HPP

#include "./details/ccc_tables.hpp"
#include "./unicode.hpp"

namespace webpp::unicode {

    /**
     * Get CCC (Canonical Combining Class) and QC (Quick Check) info of the inputted Code Point
     */
    template <UTF CharT = char32_t>
    [[nodiscard]] static constexpr stl::uint16_t qc_ccc_of(CharT const code_point) noexcept {
        using details::ccc_index;
        using details::ccc_indices;
        using details::ccc_values;
        using details::trailing_zero_cccs;

        // The CCC of anything bigger than this number is zero because zero is the default by Unicode standard
        if (code_point >= static_cast<CharT>(trailing_zero_cccs)) [[unlikely]] {
            return 0;
        }

        // Look at the ccc_index table for how this works:
        auto const code = ccc_indices[static_cast<stl::uint32_t>(code_point) >> ccc_index::chunk_shift];

        // calculating the position of the value in the ccc_values table:
        return ccc_values[code.get_position(code_point)];
    }

    /// Canonical Combining Class
    template <UTF CharT = char32_t>
    [[nodiscard]] static constexpr stl::uint8_t ccc_of(CharT const code_point) noexcept {
        // NOLINTNEXTLINE(*-magic-numbers)
        return static_cast<stl::uint8_t>(qc_ccc_of(code_point) & 0xFFU);
    }

    /// Canonical Combining Class
    template <stl::input_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr stl::uint8_t ccc_of(Iter const pos) noexcept {
        return ccc_of(unchecked::next_code_point_copy(pos));
    }

    /**
     * A starter code point in Unicode is a base character that can be combined with combining characters to
     * form a grapheme cluster. Specifically:
     *
     * A starter is a code point that is not a combining character. It is a base character, spacing mark,
     * enclosing mark, or format control character that can start a grapheme cluster[1].
     *
     * Combining characters are code points that are typically rendered by applying them to the preceding
     * character. They include nonspacing marks, spacing marks, and enclosing marks[1].
     *
     * A grapheme cluster is a sequence of one or more Unicode code points that represent a single
     * grapheme-like unit. It consists of a base character (starter) followed by zero or more combining
     * characters[1].
     *
     * So in summary, a starter code point is a base character that can start a grapheme cluster and be
     * combined with combining characters to represent a single written unit. It is the first code point in a
     * grapheme cluster.
     */
    template <stl::integral CharT = char32_t>
    [[nodiscard]] static constexpr bool is_starter(CharT const code_point) noexcept {
        return ccc_of(code_point) == 0;
    }

    template <stl::input_iterator Iter = char8_t const*>
    [[nodiscard]] static constexpr bool is_starter(Iter const pos) noexcept {
        return ccc_of(unchecked::next_code_point_copy(pos)) == 0;
    }

    /**
     * Function to check if a combining character sequence is blocked based on combining classes
     * See Section 3.11, D115 of Version 15.1.0 of the Unicode Standard.
     *
     * In Unicode, a "blocked" combining character sequence is one where a base character is followed by
     * one or more combining characters, but the sequence is not a valid representation of a single
     * abstract character. This can happen when the combining characters have a combining class that
     * prevents them from being applied to the base character.
     *
     * For example, the sequence <a, combining_grave, combining_acute> is blocked because the combining
     * acute accent cannot be applied to the base character after the combining grave accent has already
     * been applied. The combining classes of the combining characters determine the order in which they
     * can be applied to the base character.
     *
     * Attention: this function does not check validity of the iterator itself.
     */
    template <stl::input_iterator Iter>
    [[nodiscard]] static constexpr bool is_blocked(Iter const inp) noexcept {
        return ccc_of(*inp) >= ccc_of(*stl::next(inp));
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_CCC_HPP
