// Created by moisrex on 8/30/25.

#ifndef WEBPP_UNICODE_QUICK_CHECK_HPP
#define WEBPP_UNICODE_QUICK_CHECK_HPP

#include "./ccc.hpp"

namespace webpp::unicode {

    /**
     * Normalization form constants to describe which normalization algorithm should be performed.
     *
     * Also see:
     * - Unicode Standard, §2.12 Equivalent Sequences
     * - Unicode Standard, §3.11 Normalization Forms
     * - https://unicode.org/reports/tr15/
     */
    enum struct norm_form : stl::uint8_t {
        gibberish, // no form at all
        NFC,       // NFC: Normalization Form C (Preferred by W3C, Linux, and others)
        NFD,       // NFD: Normalization Form D
        NFKC,      // Normalization Form KC
        NFKD,      // Normalization Form KD
    };


    /**
     * QC (Quick Check) states
     */
    enum struct quick_check_state : stl::uint8_t {
        // If you plan to change these, make sure to change them in the tables as well

        YES   = 0b0U,  // Default
        NO    = 0b11U, // 'No' suppresses 'Maybe'
        MAYBE = 0b10U,

        simplify_mask = 0b11,

        NFD_NO    = NO | 0b100U,
        NFC_NO    = NO | 0b1'1000U, // 'No' must hide 'Maybe'
        NFC_MAYBE = MAYBE | 0b1'0000U,
        NFKD_NO   = NO | 0b10'0000U,
        NFKC_NO   = NO | 0b100'0000U,
    };

    /**
     * Given a QC value, this function will simplify it to Yes, No, Maybe
     */
    template <norm_form Form = norm_form::NFC>
    [[nodiscard]] static constexpr quick_check_state qc_of(
      stl::underlying_type_t<quick_check_state> const code) noexcept {
        using stl::to_underlying;
        using enum quick_check_state;
        using enum norm_form;

        // if they're separately included, this function needs to be modified.
        static_assert(details::embed_quick_check_tables, "Quick Check values are not embedded.");
        static_assert(NFD != Form || !details::exclude_NFD,
                      "Data required for QuickCheck is not included in the source code.");
        static_assert(NFKD != Form || !details::exclude_kompatibility,
                      "Data required for QuickCheck is not included in the source code.");
        static_assert(NFKC != Form || !details::exclude_kompatibility,
                      "Data required for QuickCheck is not included in the source code.");
        if constexpr (NFC == Form) {
            return static_cast<quick_check_state>(code & to_underlying(NFC_NO) & to_underlying(simplify_mask));
        } else if constexpr (NFD == Form) {
            return static_cast<quick_check_state>(code & to_underlying(NFD_NO) & to_underlying(simplify_mask));
        } else if constexpr (NFKD == Form) {
            return static_cast<quick_check_state>(code & to_underlying(NFKD_NO) & to_underlying(simplify_mask));
        } else if constexpr (NFKC == Form) {
            return static_cast<quick_check_state>(code & to_underlying(NFKC_NO) & to_underlying(simplify_mask));
        } else {
            return NO;
        }
    }

    /**
     * https://www.unicode.org/reports/tr15/tr15-54.html#Detecting_Normalization_Forms
     */
    template <norm_form Form = norm_form::NFC, stl::forward_iterator Iter>
    [[nodiscard]] static constexpr quick_check_state quick_check(Iter spos, Iter const send) noexcept {
        using stl::to_underlying;
        using enum quick_check_state;
        using enum checked::error_handling;

        stl::uint8_t prev_ccc = 0;
        auto         result   = to_underlying(YES);
        while (spos != send) {
            auto const code_point = checked::next_code_point<return_negated>(spos, send);
            if (static_cast<stl::int32_t>(code_point) < 0) [[unlikely]] {
                return NO;
            }
            auto const info    = qc_ccc_of(code_point);
            auto const ccc     = static_cast<stl::uint8_t>(info & 0xFFU);
            auto const qc_val  = static_cast<stl::uint8_t>(info >> 8U);
            result            |= to_underlying(qc_of<Form>(qc_val));
            if ((prev_ccc > ccc && ccc != 0) || result == to_underlying(NO)) [[unlikely]] {
                return NO;
            }
            prev_ccc = ccc;
        }
        return static_cast<quick_check_state>(result);
    }

    /**
     * This runs a quick check on the input and stops at the first starter code point that after that there's a MAYBE.
     */
    template <norm_form Form = norm_form::NFC, stl::forward_iterator Iter>
    [[nodiscard]] static constexpr quick_check_state quick_check_till_maybe(Iter& spos, Iter const send) noexcept {
        using stl::to_underlying;
        using enum quick_check_state;
        using enum checked::error_handling;

        stl::uint8_t prev_ccc = 0;
        auto         result   = to_underlying(YES);
        Iter         starter  = spos;
        Iter         prev     = spos;
        for (; spos != send; prev = spos) {
            auto const code_point = checked::next_code_point<return_negated>(spos, send);
            if (static_cast<stl::int32_t>(code_point) < 0) [[unlikely]] {
                result = to_underlying(NO);
                break;
            }
            auto const info    = qc_ccc_of(code_point);
            auto const ccc     = static_cast<stl::uint8_t>(info & 0xFFU);
            auto const qc_val  = static_cast<stl::uint8_t>(info >> 8U);
            result            |= to_underlying(qc_of<Form>(qc_val));

            if (result != to_underlying(YES)) [[unlikely]] {
                break;
            }

            // constantly keep track of the starter code point
            if (ccc == 0) {
                starter = prev;
            } else if (prev_ccc > ccc) [[unlikely]] {
                result = to_underlying(NO);
                break;
            }
            prev_ccc = ccc;
        }
        if (result == to_underlying(MAYBE)) {
            spos = starter; // restoring the lastest starter code point
        } else if (result == to_underlying(NO)) {
            spos = prev;
        }
        return static_cast<quick_check_state>(result);
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_QUICK_CHECK_HPP
