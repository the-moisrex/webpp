// Created by moisrex on 4/1/25.

#ifndef WEBPP_UNICODE_JOINERS_HPP
#define WEBPP_UNICODE_JOINERS_HPP

#include "../std/string_view.hpp"
#include "./ccc.hpp"
#include "./details/joiners_tables.hpp"

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
    [[nodiscard]] static constexpr joiner_type joiner_type_of(char32_t const code_point) noexcept {
        using enum joiner_type;
        using details::joiners_index;
        using details::joiners_indices;
        using details::joiners_values;


        auto const chunk         = code_point >> joiners_index::chunk_shift;
        auto const section_index = static_cast<stl::uint16_t>(chunk >> details::joiners_breakpoint_shift);
        if (chunk >= static_cast<char32_t>(details::joiners_last_breakpoint)) [[unlikely]] {
            return non_joining;
        }
        assert(section_index < details::joiners_breakpoints.size());
        auto const [starting, ending, offset] = details::joiners_breakpoints.at(section_index);
        joiners_index const pos =
          chunk < starting || chunk >= ending ? details::joiners_common_pos : joiners_indices.at(chunk - offset);

        auto const joiner_pos = pos.get_position(code_point);
        assert(joiner_pos < joiners_values.size());
        return static_cast<joiner_type>(joiners_values.at(joiner_pos));
    }

    namespace details {
        /// ZERO WIDTH NON-JOINER
        /// spos points to the next code point
        template <stl::random_access_iterator Iter>
        [[nodiscard]] static constexpr bool validate_zero_with_non_joiner(
          Iter const&    sbeg,
          Iter const&    spos,
          Iter const&    send,
          char32_t const last_code_point) noexcept {
            using enum err_policy;
            using enum joiner_type;


            // ccc_of(0) is not gonna be Virama, so we don't need to check for it
            if (is_ccc_of(last_code_point, ccc_props::Virama)) {
                return true;
            }

            // If RegExpMatch((L|D)(T)*\u200C(T)*(R|D)) Then True;
            Iter pos    = spos;
            stl::ignore = checked::prev_code_point(pos, sbeg);
            for (;;) {
                if (pos == sbeg) [[unlikely]] {
                    return false;
                }
                switch (joiner_type_of(checked::prev_code_point<return_negated>(pos, sbeg))) {
                    case transparent: continue;
                    case left_joining:
                    case dual_joining:
                        break;
                    [[unlikely]] default:
                        return false;
                }
                break;
            }

            pos = spos;
            for (;;) {
                if (pos == send) [[unlikely]] {
                    return false;
                }
                switch (joiner_type_of(checked::next_code_point<return_negated>(pos, send))) {
                    case transparent: continue;
                    case right_joining:
                    case dual_joining:
                        break;
                    [[unlikely]] default:
                        return false;
                }
                break;
            }
            [[likely]] { return true; }
        }

        /// ZERO WIDTH NON-JOINER
        [[nodiscard]] static constexpr bool validate_zero_with_joiner(char32_t last_code_point) noexcept {
            // ccc_of(0) is not gonna be Virama, so we don't need to check
            return is_ccc_of(last_code_point, ccc_props::Virama);
        }

    } // namespace details

    /**
     * Check if joiner code points are correct.
     * Attention: this function does only the lookup part of the appendix, and not the full check.
     * RFC: https://www.rfc-editor.org/rfc/rfc5892.html#appendix-A
     */
    template <stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool validate_context_joiners(Iter const& sbeg, Iter const& send) noexcept {
        using enum err_policy;
        using enum joiner_type;

        Iter     spos       = sbeg;
        char32_t code_point = 0;
        for (char32_t last_cp = 0; spos != send; last_cp = code_point) {
            code_point = checked::next_code_point<return_negated>(spos, send);
            switch (code_point) {
                    // This may occur in a formally cursive script (such as Arabic) in a context where it
                    // breaks a cursive connection as required for orthographic rules, as in the Persian
                    // language, for example. It also may occur in Indic scripts in a consonant-conjunct
                    // context (immediately following a virama), to control required display of such
                    // conjuncts.
                case U'\x200C': { // ZERO WIDTH NON-JOINER
                    if (!details::validate_zero_with_non_joiner(sbeg, spos, send, last_cp)) [[unlikely]] {
                        return false;
                    }
                    break;
                }

                    // This may occur in Indic scripts in a consonant-conjunct context (immediately following
                    // a virama), to control the required display of such conjuncts.
                case U'\x200D': { // ZERO WIDTH JOINER
                    if (!details::validate_zero_with_joiner(last_cp)) [[unlikely]] {
                        return false;
                    }
                    break;
                }

                // Other Appendix rules don't apply since their "Lookup" is false which means we don't need to
                // check those rules during DNS lookup.
                default: break;
            }
        }
        return true;
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_JOINERS_HPP
