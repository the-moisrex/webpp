// Created by moisrex on 4/6/25.

#ifndef WEBPP_UNICODE_GENERAL_CATEGORY_HPP
#define WEBPP_UNICODE_GENERAL_CATEGORY_HPP

#include "./details/gc_tables.hpp"
#include "./unicode.hpp"

namespace webpp::unicode {

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
                               ? details::gc_common_position
                               : gc_indices[static_cast<stl::uint16_t>(chunk - offset)];

        return static_cast<general_category>(gc_values[pos.get_position(code_point)]);
        // NOLINTEND(*-pro-bounds-constant-array-index)
    }

    template <UTF CharT = char32_t>
    [[nodiscard]] static constexpr bool is_general_category_of(CharT const            code_point,
                                                               general_category const cat) noexcept {
        using enum general_category;
        auto const gc_val = general_category_of(code_point);
        switch (cat) {
            case Mark:
                return gc_val == Enclosing_Mark || gc_val == Nonspacing_Mark || gc_val == Spacing_Mark ||
                       gc_val == Mark;
            default: break;
        }
        return gc_val == cat;
    }
} // namespace webpp::unicode

#endif // WEBPP_UNICODE_GENERAL_CATEGORY_HPP
