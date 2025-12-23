// Created by moisrex on 4/6/25.

#ifndef WEBPP_UNICODE_GENERAL_CATEGORY_HPP
#define WEBPP_UNICODE_GENERAL_CATEGORY_HPP

#include "../std/utility.hpp"
#include "./details/gc_tables.hpp"

#include <cassert>

namespace webpp::unicode {

    [[nodiscard]] static constexpr stl::underlying_type_t<general_category> operator+(
      general_category const gcat) noexcept {
        return stl::to_underlying(gcat);
    }

    /**
     * Get the General_Category property of the specified code point.
     * Attention: this may not return the correct result of our code point tables exclude the
     *            full GC values in order to reduce table size.
     *            Checkout ./details/gc_tables.hpp file's comments for the list of excluded categories if any.
     */
    [[nodiscard]] static constexpr general_category general_category_of(char32_t const code_point) noexcept {
        using details::gc_index;
        using details::gc_indices;
        using details::gc_values;

        auto const chunk         = code_point >> gc_index::chunk_shift;
        auto const section_index = static_cast<stl::uint16_t>(chunk >> details::gc_breakpoint_shift);
        if (chunk >= static_cast<char32_t>(details::gc_last_breakpoint)) [[unlikely]] {
            return general_category::Unassigned;
        }
        assert(section_index < details::gc_breakpoints.size());
        auto const [starting, ending, offset] = details::gc_breakpoints.at(section_index);

        gc_index const pos =
          chunk < starting || chunk >= ending ? details::gc_common_pos : gc_indices.at(chunk - offset);

        auto const gc_pos = pos.get_position(code_point);
        assert(gc_pos < gc_values.size());
        return static_cast<general_category>(gc_values.at(gc_pos));
    }

    /**
     * Check if the specified code point is in the specified category or not;
     * Attention: this is not an exact match; if you give a single-letter category, and the code point
     *            belongs to a 2-letter sub-category, it'll match.
     *
     * See: https://www.unicode.org/reports/tr44/#General_Category_Values
     */
    [[nodiscard]] static constexpr bool is_general_category_of(char32_t const         code_point,
                                                               general_category const cat) noexcept {
        constexpr stl::uint8_t mask   = 32U - 1U;
        auto const             gc_val = general_category_of(code_point);

        // If you specify the "single-letter" category, we'd make sure the subsequence 2-letter ones are a
        // match as well.
        //   "Mark" can be "Spacing_Mark", "Enclosing_Mark", or "Enclosing_Mark"
        if ((+cat & mask) == 0) {
            return (+gc_val & static_cast<stl::uint8_t>(~mask)) == +cat;
        }
        return gc_val == cat;
    }
} // namespace webpp::unicode

#endif // WEBPP_UNICODE_GENERAL_CATEGORY_HPP
