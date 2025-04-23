// Created by moisrex on 4/6/25.

#ifndef WEBPP_UNICODE_GENERAL_CATEGORY_HPP
#define WEBPP_UNICODE_GENERAL_CATEGORY_HPP

#include "./details/gc_tables.hpp"
#include "./unicode.hpp"

namespace webpp::unicode {

    /**
     * Get the General_Category property of the specified code point.
     * Attention: this may not return the correct result of our code point tables exclude the
     *            full GC values in order to reduce table size.
     *            Checkout ./details/gc_tables.hpp file's comments for the list of excluded categories if any.
     */
    template <UTF CharT = char32_t>
    [[nodiscard]] static constexpr general_category general_category_of(CharT const code_point) noexcept {
        using enum general_category;
        using details::gc_index;
        using details::gc_indices;
        using details::gc_values;

        // NOLINTBEGIN(*-pro-bounds-constant-array-index)
        auto const chunk         = code_point >> gc_index::chunk_shift;
        auto const section_index = static_cast<stl::uint16_t>(chunk >> details::gc_breakpoint_shift);
        if (chunk >= static_cast<CharT>(details::gc_last_breakpoint)) [[unlikely]] {
            return Unassigned;
        }
        auto const [starting, ending, offset] = details::gc_breakpoints[section_index];

        gc_index const pos = chunk < starting || chunk >= ending
                               ? details::gc_common_pos
                               : gc_indices[static_cast<stl::uint16_t>(chunk - offset)];

        return static_cast<general_category>(gc_values[pos.get_position(code_point)]);
        // NOLINTEND(*-pro-bounds-constant-array-index)
    }

    /**
     * Check if the specified code point is in the specified category or not;
     * Attention: this is not an exact match; if you give a single-letter category, and the code point
     *            belongs to a 2-letter sub-category, it'll match.
     */
    template <UTF CharT = char32_t>
    [[nodiscard]] static constexpr bool is_general_category_of(CharT const            code_point,
                                                               general_category const cat) noexcept {
        using enum general_category;
        constexpr stl::uint8_t mask   = 31U; // 32 - 1
        auto const             gc_val = general_category_of(code_point);

        // If you specify the "single-letter" category, we'd make sure the subsequence 2-letter ones are a
        // match as well.
        //   "Mark" can be "Spacing_Mark", "Enclosing_Mark", or "Enclosing_Mark"
        if ((static_cast<stl::uint8_t>(cat) & mask) == 0) {
            return (static_cast<stl::uint8_t>(gc_val) & static_cast<stl::uint8_t>(~mask)) ==
                   static_cast<stl::uint8_t>(cat);
        }
        return gc_val == cat;
    }
} // namespace webpp::unicode

#endif // WEBPP_UNICODE_GENERAL_CATEGORY_HPP
