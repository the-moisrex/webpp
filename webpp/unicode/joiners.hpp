// Created by moisrex on 4/1/25.

#ifndef WEBPP_UNICODE_JOINERS_HPP
#define WEBPP_UNICODE_JOINERS_HPP

#include "../std/string_view.hpp"
#include "./details/joiners_tables.hpp"
#include "./unicode.hpp"

namespace webpp::unicode {

    /**
     * Values are extracted from:
     *   https://www.unicode.org/reports/tr44/#ArabicShaping.txt
     *   https://www.unicode.org/reports/tr44/#Joining_Type
     */
    enum struct joiner_type : stl::uint8_t {
        non_joining   = 0, // U
        right_joining = 1, // R
        left_joining  = 2, // L
        dual_joining  = 3, // D
        join_causing  = 4, // C
        transparent   = 5, // T
    };

    [[nodiscard]] static constexpr stl::string_view to_string(joiner_type const type) noexcept {
        using enum joiner_type;
        switch (type) {
            case non_joining: return {"Non_Joining"};
            case right_joining: return {"Right_Joining"};
            case left_joining: return {"Left_Joining"};
            case dual_joining: return {"Dual_Joining"};
            case join_causing: return {"Join_Causing"};
            case transparent: return {"Transparent"};
        }
        return {"<unknown joiner>"};
    }

    /**
     * This function returns the Joining_Type of the input code point.
     * They are extracted from DerivedJoiningType.txt
     */
    template <UTF32 CharT = char32_t>
    [[nodiscard]] static constexpr joiner_type joiner_type_of(CharT const code_point) noexcept {
        using enum joiner_type;
        using details::joiners_index;
        using details::joiners_indices;
        using details::joiners_values;

        // NOLINTBEGIN(*-pro-bounds-constant-array-index)

        auto const chunk         = code_point >> joiners_index::chunk_shift;
        auto const section_index = static_cast<stl::uint16_t>(chunk >> details::joiners_breakpoint_shift);
        if (chunk >= static_cast<CharT>(details::joiners_last_breakpoint)) [[unlikely]] {
            return non_joining;
        }
        auto const [starting, ending, offset] = details::joiners_breakpoints[section_index];
        joiners_index const pos =
          chunk < starting || chunk >= ending
            ? details::joiners_common_position
            : joiners_indices[static_cast<stl::uint16_t>(chunk - offset)];

        return static_cast<joiner_type>(joiners_values[pos.get_position(code_point)]);
        // NOLINTEND(*-pro-bounds-constant-array-index)
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_JOINERS_HPP
