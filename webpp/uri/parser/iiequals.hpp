// Created by moisrex on 2/29/24.

#ifndef URI_IIEQUALS_HPP
#define URI_IIEQUALS_HPP

#include "../../std/string_view.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/iequals.hpp"

namespace webpp::uri {

    static constexpr auto empty_charset = charset<char, 0>{};

    /**
     * Almost same as iequals, but it'll ignore `\r`, `\n`, and `\t` characters
     */
    template <CharSet auto            IgnoreCharacters = empty_charset,
              ascii::char_case_side   Side             = ascii::char_case_side::both_unknown,
              istl::StringViewifiable LT,
              istl::StringViewifiable RT>
    [[nodiscard]] static constexpr bool iiequals(LT&& inp_lhs, RT&& inp_rhs) noexcept {
        if constexpr (IgnoreCharacters.empty()) {
            return ascii::iequals<Side>(stl::forward<LT>(inp_lhs), stl::forward<RT>(inp_rhs));
        } else {
            auto const lhs = istl::string_viewify(stl::forward<LT>(inp_lhs));
            auto const rhs = istl::string_viewify(stl::forward<RT>(inp_rhs));

            auto lhs_it = stl::begin(lhs);
            auto rhs_it = stl::begin(rhs);

            using char_type = stl::iter_value_t<decltype(lhs_it)>;

            for (;;) {
                if (lhs_it == stl::end(lhs)) {
                    // rhs, now has to be all ignored, otherwise it's not equal
                    for (; rhs_it != stl::end(rhs); ++rhs_it) {
                        if (!IgnoreCharacters.contains(*rhs_it)) {
                            return false;
                        }
                    }
                    return true;
                }

                if (rhs_it == stl::end(rhs)) {
                    // rhs, now has to be all ignored, otherwise it's not equal
                    for (; lhs_it != stl::end(lhs); ++lhs_it) {
                        if (!IgnoreCharacters.contains(*lhs_it)) {
                            return false;
                        }
                    }
                    return true;
                }

                if (ascii::iequals<Side>(*lhs_it, static_cast<char_type>(*rhs_it))) {
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
        }
    }

    /// first lowered
    template <CharSet auto IgnoreCharacters = empty_charset, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_fl(LT&& lhs, RT&& rhs) noexcept {
        return iiequals<IgnoreCharacters, ascii::char_case_side::first_lowered>(
          stl::forward<LT>(lhs),
          stl::forward<RT>(rhs));
    }

    /// second lowered
    template <CharSet auto IgnoreCharacters = empty_charset, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_sl(LT&& lhs, RT&& rhs) noexcept {
        return iiequals<IgnoreCharacters, ascii::char_case_side::second_lowered>(
          stl::forward<LT>(lhs),
          stl::forward<RT>(rhs));
    }

    /// first uppered
    template <CharSet auto IgnoreCharacters = empty_charset, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_fu(LT&& lhs, RT&& rhs) noexcept {
        return iiequals<IgnoreCharacters, ascii::char_case_side::first_uppered>(
          stl::forward<LT>(lhs),
          stl::forward<RT>(rhs));
    }

    /// second uppered
    template <CharSet auto IgnoreCharacters = empty_charset, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_su(LT&& lhs, RT&& rhs) noexcept {
        return iiequals<IgnoreCharacters, ascii::char_case_side::second_uppered>(
          stl::forward<LT>(lhs),
          stl::forward<RT>(rhs));
    }

    /// Auto Lowered
    template <CharSet auto IgnoreCharacters = empty_charset, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_auto_lowered(LT&& lhs, RT&& rhs) noexcept {
        constexpr auto lowered =
          istl::ModifiableString<LT> && istl::ModifiableString<RT> ? ascii::char_case_side::both_lowered
          : istl::ModifiableString<LT>                             ? ascii::char_case_side::first_lowered
                                                                   : ascii::char_case_side::second_lowered;
        return iiequals<IgnoreCharacters, lowered>(stl::forward<LT>(lhs), stl::forward<RT>(rhs));
    }

    /// Auto Upper
    template <CharSet auto IgnoreCharacters = empty_charset, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_auto_upper(LT&& lhs, RT&& rhs) noexcept {
        constexpr auto uppered =
          istl::ModifiableString<LT> && istl::ModifiableString<RT> ? ascii::char_case_side::both_uppered
          : istl::ModifiableString<LT>                             ? ascii::char_case_side::first_uppered
                                                                   : ascii::char_case_side::second_uppered;
        return iiequals<IgnoreCharacters, uppered>(stl::forward<LT>(lhs), stl::forward<RT>(rhs));
    }

    /// Auto First Lowered
    template <CharSet auto IgnoreCharacters = empty_charset, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_afl(LT&& lhs, RT&& rhs) noexcept {
        constexpr auto lowered =
          istl::ModifiableString<LT> ? ascii::char_case_side::first_lowered : ascii::char_case_side::both_unknown;
        return iiequals<IgnoreCharacters, lowered>(stl::forward<LT>(lhs), stl::forward<RT>(rhs));
    }

    /// Auto First Upper
    template <CharSet auto IgnoreCharacters = empty_charset, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals_afu(LT&& lhs, RT&& rhs) noexcept {
        constexpr auto uppered =
          istl::ModifiableString<LT> ? ascii::char_case_side::first_uppered : ascii::char_case_side::both_unknown;
        return iiequals<IgnoreCharacters, uppered>(stl::forward<LT>(lhs), stl::forward<RT>(rhs));
    }
} // namespace webpp::uri

#endif // URI_IIEQUALS_HPP
