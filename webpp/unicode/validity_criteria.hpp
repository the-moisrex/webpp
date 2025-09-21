#ifndef WEBPP_UNICODE_IDNA_VALIDITY_CRITERIA_HPP
#define WEBPP_UNICODE_IDNA_VALIDITY_CRITERIA_HPP

#include "../std/iterator.hpp"
#include "../std/string.hpp"
#include "./bidi.hpp"
#include "./general_category.hpp"
#include "./idna.hpp"
#include "./joiners.hpp"
#include "./normalization.hpp"
#include "./unicode.hpp"

namespace webpp::unicode::idna {

    using validity_criteria_status_type = stl::uint16_t;

    /**
     * From IdnaTestV2.txt:
     *   A status in toUnicode, toAsciiN or toAsciiT is indicated by a value in square brackets,
     *   such as "[B5, B6]". In such a case, the contents is a list of status codes based on the step
     *   numbers in UTS #46 and IDNA2008, with the following formats.
     *
     *     Pn for Section 4 Processing step n
     *     Vn for 4.1 Validity Criteria step n
     *     U1 for UseSTD3ASCIIRules
     *     An for 4.2 ToASCII step n
     *     Bn for Bidi (in IDNA2008)
     *     Cn for ContextJ (in IDNA2008)
     *     Xn for toUnicode issues (see below)
     *
     *   Implementations that allow values of particular input flags to be false would ignore
     *   the corresponding status codes listed in the table below when testing for errors.
     *
     *   VerifyDnsLength:   A4_1, A4_2
     *   CheckHyphens:      V2, V3
     *   CheckJoiners:      Cn
     *   CheckBidi:         Bn
     *   UseSTD3ASCIIRules: U1
     */
    enum struct validity_criteria_status : validity_criteria_status_type {
        valid = 0,

        // 1. Must be NFC
        V1          = 0b1U << 1U,
        nfc_failure = V1,

        // 2. Hyphen at 3rd and 4th
        V2        = 0b1U << 2U,
        hyphen_34 = V2,

        // 3. Hyphen at beginning or at the end
        V3            = 0b1U << 3U,
        hyphen_around = V3,

        // 4. xn-- (ACE prefix)
        V4        = 0b1U << 4U,
        ace_found = V4,

        // 5. Labels don't have dots in them
        V5        = 0b1U << 5U,
        dot_found = V5,

        // 6. No combining marks allowed at the start
        V6                      = 0b1U << 6U,
        combining_mark_at_start = V6,

        // 7. Status values failure
        V7                    = 0b1U << 7U,
        status_values_failure = V7,

        // 8. ContextJ Rules Failure
        V8             = 0b1U << 8U,
        joiner_failure = V8,

        // 9. Bidi Failure
        V9           = 0b1U << 9U,
        bidi_failure = V9,
    };

    [[nodiscard]] static constexpr stl::string_view to_string(validity_criteria_status const status) noexcept {
        using enum validity_criteria_status;
        switch (status) {
            default: break;
        }
        return "<unknown-validity-criteria-status>";
    }

    /**
     * Is Domain Label Valid?
     * Valid Criteria: https://www.unicode.org/reports/tr46/#Validity_Criteria
     *
     * Preconditions:
     *  - The label must be in Unicode Normalization Form C.
     *  - The label must not contain a U+002E (.) FULL STOP.
     *  - Each code point in the label must only have certain Status values according to
     *    Section 5, IDNA Mapping Table
     *
     * Starting with Unicode 16.0, UseSTD3ASCIIRules=true is handled only in the Validity Criteria
     */
    template <idna_options Options = idna_options{}, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr validity_criteria_status_type is_label_valid(Iter spos, Iter send) noexcept {
        // 1. SKIPPED: The label must be in Unicode Normalization Form NFC.
        // 2. If CheckHyphens, the label must not contain a U+002D HYPHEN-MINUS character in both the third
        //    and fourth positions.
        // 3. If CheckHyphens, the label must neither begin nor end with a U+002D HYPHEN-MINUS character.
        // 4. If not CheckHyphens, the label must not begin with “xn--”.
        // 5. SKIPPED: The label must not contain a U+002E (.) FULL STOP.
        // 6. The label must not begin with a combining mark, that is: General_Category=Mark.
        // 7. SKIPPED: ...
        // 8. If CheckJoiners, the label must satisfy the ContextJ rules from Appendix A, in
        //    The Unicode Code Points and Internationalized Domain Names for Applications (IDNA) [IDNA2008].
        // 9. If CheckBidi, and if the domain name is a Bidi domain name,
        //    then the label must satisfy all six of the numbered conditions
        //    in [IDNA2008] RFC 5893, Section 2.
        //    https://www.rfc-editor.org/rfc/rfc5893#section-2


        using enum checked::error_handling;

        bool       valid  = true;
        auto const length = send - spos;
        // if (length == 0) {
        //     return true;
        // }


        // 1. Check if it's in NFC form (SKIPPED by default)
        if constexpr (Options.CheckNFC) {
            valid &= isNFC(spos, send);
        }

        // 2,3,4. Check hyphens (default is false)
        if constexpr (Options.CheckHyphens) {
            switch (length) {
                [[likely]] default:
                case 4:
                    valid &= *stl::next(spos, 3) != '-';          // forth
                    [[fallthrough]];
                case 3:
                    valid &= *stl::next(spos, 2) != '-';          // third
                    [[fallthrough]];
                case 2:
                    valid &= *stl::next(spos, length - 1) != '-'; // last
                    [[fallthrough]];
                case 1:
                    valid &= *spos != '-';                        // first
                    [[fallthrough]];
                case 0: break;
            }
        } else {
            auto pos = spos;

            // NOLINTNEXTLINE(*-inc-dec-in-conditions)
            valid &= length < 4 || *pos++ != 'x' || *pos++ != 'n' || *pos++ != '-' || *pos != '-';
        }

        // 5. Check if it includes any dots (SKIPPED by default)
        if constexpr (Options.CheckDotInclusions) {
            // we don't need to check for UTF encodings, nor we need early bailout since that would mean we'd
            // be optimizing for the failure path as opposed to optimizing for the happy path
            for (auto pos = spos; pos != send; ++pos) {
                valid &= *pos != '.';
            }
        }

        // 6. The label must not start with a combining mark
        {
            auto const cur_cp = checked::next_code_point_copy<return_negated>(spos, send);

            // no need to check the length, it'll return 0, which is not GC, so it's fine.
            valid &= !is_general_category_of(cur_cp, general_category::Mark);
        }

        // 7. Checking Status values (SKIPPED by default)
        if constexpr (Options.CheckStatusValues) {
            // - For Transitional Processing (deprecated)
            // - For Nontransitional Processing, each value must be either valid or deviation.
            // - In addition, if UseSTD3ASCIIRules=true and the code point is an ASCII code point
            //   (U+0000..U+007F), then it must be a lowercase letter (a-z), a digit (0-9), or a hyphen-minus
            //   (U+002D). (Note: This excludes uppercase ASCII A-Z which are mapped in UTS #46 and disallowed
            //   in IDNA2008.)
            auto pos = spos;
            while (pos != send) {
                auto const cur_cp = checked::next_code_point<return_negated>(pos, send);
                auto const status = status_of(cur_cp);

                // https://www.unicode.org/reports/tr46/#Deviations
                // Deviations are considered valid in IDNA2008 and UTS #46.
                valid &= status == details::valid;

                if constexpr (Options.UseSTD3ASCIIRules) {
                    valid &= !is_ascii(cur_cp) || ASCII_STD3_RULES.contains(cur_cp);
                }
            }
        }

        // 8. Check joiners
        if constexpr (Options.CheckJoiners) {
            valid &= validate_context_joiners(spos, send);
        }

        // 9. Check bidi rule
        if constexpr (Options.CheckBidi) {
            // If CheckBidi, and if the domain name is a "Bidi domain name":
            valid &= validate_bidi_rule(spos, send);
        }

        return valid;
    }

    template <idna_options Options = idna_options{}, istl::StringViewifiable StrT>
    [[nodiscard]] static constexpr bool is_label_valid(StrT&& inp_str) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        return is_label_valid<Options>(str.begin(), str.end());
    }


} // namespace webpp::unicode::idna

#endif // WEBPP_UNICODE_IDNA_VALIDITY_CRITERIA_HPP
