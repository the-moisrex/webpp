// Created by moisrex on 2/29/24.

#ifndef URI_HELPERS_HPP
#define URI_HELPERS_HPP

#include "../../std/string_view.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/iequals.hpp"

namespace webpp::uri {

    /**
     * Almost same as iequals, but it'll ignore `\r`, `\n`, and `\t` characters
     */
    template <ascii::char_case_side   Side             = ascii::char_case_side::both_unknown,
              CharSet auto            IgnoreCharacters = charset{'\r', '\t', '\n'},
              istl::StringViewifiable LT,
              istl::StringViewifiable RT>
    [[nodiscard]] static constexpr bool iiequals(LT&& lhs, RT&& rhs) noexcept {
        if constexpr (IgnoreCharacters.empty()) {
            return ascii::iequals<Side>(stl::forward<LT>(lhs), stl::forward<RT>(rhs));
        } else {
            auto lhs_it = stl::begin(lhs);
            auto rhs_it = stl::begin(rhs);

            while (lhs_it != stl::end(lhs) && rhs_it != stl::end(rhs)) {
                if (ascii::iequals<Side>(*lhs_it, *rhs_it)) {
                    ++lhs_it;
                    ++rhs_it;
                    continue;
                }

                if (IgnoreCharacters.contains(*lhs_it)) {
                    ++lhs_it;
                    continue;
                }

                if (IgnoreCharacters.contains(*rhs_it)) {
                    ++rhs_it;
                    continue;
                }
                return false;
            }

            // If both strings have been traversed completely, they are equal.
            // Otherwise, if there are remaining characters in either string,
            // they are not equal.
            return lhs_it == stl::end(lhs) && rhs_it == stl::end(rhs);
        }
    }

    /// first lowered
    template <CharSet auto IgnoreCharacters = charset{'\r', '\t', '\n'}, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_fl(LT&& lhs, RT&& rhs) noexcept {
        return iiequals<ascii::char_case_side::first_lowered, IgnoreCharacters>(
          stl::forward<LT>(lhs),
          stl::forward<RT>(rhs));
    }

    /// second lowered
    template <CharSet auto IgnoreCharacters = charset{'\r', '\t', '\n'}, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_sl(LT&& lhs, RT&& rhs) noexcept {
        return iiequals<ascii::char_case_side::second_lowered, IgnoreCharacters>(
          stl::forward<LT>(lhs),
          stl::forward<RT>(rhs));
    }

    /// first uppered
    template <CharSet auto IgnoreCharacters = charset{'\r', '\t', '\n'}, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_fu(LT&& lhs, RT&& rhs) noexcept {
        return iiequals<ascii::char_case_side::first_uppered, IgnoreCharacters>(
          stl::forward<LT>(lhs),
          stl::forward<RT>(rhs));
    }

    /// second uppered
    template <CharSet auto IgnoreCharacters = charset{'\r', '\t', '\n'}, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_su(LT&& lhs, RT&& rhs) noexcept {
        return iiequals<ascii::char_case_side::second_uppered, IgnoreCharacters>(
          stl::forward<LT>(lhs),
          stl::forward<RT>(rhs));
    }

} // namespace webpp::uri

#endif // URI_HELPERS_HPP
