#ifndef WEBPP_UNICODE_IDNA_VALIDITY_CRITERIA_HPP
#define WEBPP_UNICODE_IDNA_VALIDITY_CRITERIA_HPP

#include "../std/iterator.hpp"
#include "../std/string.hpp"
#include "./bidi.hpp"
#include "./checked.hpp"
#include "./general_category.hpp"
#include "./idna.hpp"
#include "./joiners.hpp"
#include "./normalization.hpp"

#include <bit>
#include <cstdint>

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
     *
     * If you ever needed to update this, make sure to update the `to_ascii_status` as well.
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
        V7                       = 0b1U << 7U,
        requires_mapping_failure = V7,

        // 8. ContextJ Rules Failure
        V8             = 0b1U << 8U,
        joiner_failure = V8,

        // 9. Bidi Failure
        V9           = 0b1U << 9U,
        bidi_failure = V9,

        // 10. Empty Label (... must be satisfied for a non-empty label)
        V10         = 0b1U << 10U,
        empty_label = V10,

        // Flags:
        bidi_domain_name = 0b1U << 11U, // it's a flag, and not a status
    };

    using validity_flag_type = stl::uint8_t;

    // NOLINTBEGIN(*-signed-bitwise)
    enum struct validity_flags : validity_flag_type {
        // ASCII and Non-ASCII:
        non_ascii   = 0b1000U,
        ascii_upper = 0b1'0000U,
        dot         = 0b10'0000U, // Assume dot is not ASCII

        // xn-- (Called ACE Prefix):
        x    = 0b1U,
        n    = 0b10U,
        dash = 0b100U,
        ace  = x | n | dash, // ACE prefix

        // Misc:
        all         = 0b1111'1111U, // all possibilities
        valid_label = all & ~dot,
    };

    // NOLINTEND(*-signed-bitwise)


    /// Shortcut for `std::to_underlying(status)`
    [[nodiscard]] static constexpr validity_flag_type operator+(validity_flags const flags) noexcept {
        return stl::to_underlying(flags);
    }

    /// Check if `all_flags` has at least of of the `flags` in it
    template <typename... T>
        requires((stl::convertible_to<T, validity_flags> && ...))
    [[nodiscard]] static constexpr bool has_flag(validity_flag_type const all_flags, T const... flags) noexcept {
        return (all_flags & static_cast<validity_flag_type>((+flags | ...))) != 0U;
    }

    [[nodiscard]] static constexpr stl::string_view to_string(validity_criteria_status const status) noexcept {
        using enum validity_criteria_status;
        switch (status) {
            // Valid:
            case valid: return {"Valid"};

            // Errors:
            case V1: return {"NFC Failure"};
            case V2: return {"Hyphen character used in the 3rd and 4th position"};
            case V3: return {"Hyphen character found in the beginning or ending of the label"};
            case V4: return {"ACE prefix (xn--) found at the beginning of the label"};
            case V5: return {"Label has a dot in it."};
            case V6: return {"Label starts with a Unicode combining mark"};
            case V7: return {"The label requires mapping some code points"};
            case V8: return {"Failure in ContextJ Rules"};
            case V9: return {"Failure in Bidi Rules"};
            case V10: return {"Empty Label"};

            // Flags:
            case bidi_domain_name:
                return {"Domain is bidirectional"};
            [[unlikely]] default:
                break;
        }
        return {"<unknown-validity-criteria-status>"};
    }

    [[nodiscard]] static constexpr validity_criteria_status_type operator+(
      validity_criteria_status const status) noexcept {
        return stl::to_underlying(status);
    }

    [[nodiscard]] static consteval validity_criteria_status_type operator~(
      validity_criteria_status const status) noexcept {
        auto const bit_len = static_cast<stl::uint8_t>(stl::countr_zero(+status));
        return static_cast<validity_criteria_status_type>(0b1U << bit_len);
    }

    /**
     * Check if the status is valid (ignoring the information flags while at it)
     */
    [[nodiscard]] static constexpr bool is_valid(validity_criteria_status_type const status) noexcept {
        using enum validity_criteria_status;
        constexpr auto not_bidi = static_cast<validity_criteria_status_type>(~+bidi_domain_name);
        return (status & not_bidi) == +valid;
    }

    /**
     * Check if the status code, has the flag you specify.
     */
    [[nodiscard]] static constexpr bool has_flag(validity_criteria_status_type const status,
                                                 validity_criteria_status const      flag) noexcept {
        return (status & +flag) != 0;
    }

    /**
     * Is Domain Label (not the whole domain, but each label of the domain) Valid?
     * Valid Criteria: https://www.unicode.org/reports/tr46/#Validity_Criteria
     *
     * Preconditions:
     *  - The label must be in Unicode Normalization Form C.
     *  - The label must not contain a U+002E (.) FULL STOP.
     *  - Each code point in the label must only have certain Status values according to
     *    Section 5, IDNA Mapping Table
     *
     * Starting with Unicode 16.0, UseSTD3ASCIIRules=true is handled only in the Validity Criteria
     *
     * Attention: If CheckBidi is given, the caller MUST check for "Bidi Domain Name", and
     *            ignore `bidi_failure` if it's in the results.
     */
    template <idna_options Options = idna_options{}, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr validity_criteria_status_type label_validity_status(
      Iter                     spos,
      Iter                     send,
      validity_flag_type const flags = +validity_flags::valid_label) noexcept {
        // 1. The label must be in Unicode Normalization Form NFC.
        // 2. If CheckHyphens, the label must not contain a U+002D HYPHEN-MINUS character in both the third
        //    and fourth positions.
        // 3. If CheckHyphens, the label must neither begin nor end with a U+002D HYPHEN-MINUS character.
        // 4. If not CheckHyphens, the label must not begin with “xn--”.
        // 5. The label must not contain a U+002E (.) FULL STOP.
        // 6. The label must not begin with a combining mark, that is: General_Category=Mark.
        // 7. Each code point in the label must only have certain Status values according to IDNA Mapping Table
        //    And, if UseSTD3ASCIIRules=true, each ASCII code point must be a lowercase letter (a–z), a digit, or a
        //    hyphen,
        // 8. If CheckJoiners, the label must satisfy the ContextJ rules from Appendix A, in
        //    The Unicode Code Points and Internationalized Domain Names for Applications (IDNA) [IDNA2008].
        // 9. If CheckBidi, and if the domain name is a Bidi domain name,
        //    then the label must satisfy all six of the numbered conditions
        //    in [IDNA2008] RFC 5893, Section 2.
        //    https://www.rfc-editor.org/rfc/rfc5893#section-2

        using checked::utf32_forward_iter;
        using unicode::details::isNFC_until_next_starter;
        using unicode::details::validate_zero_with_joiner;
        using unicode::details::validate_zero_with_non_joiner;
        using enum err_policy;
        using enum validity_criteria_status;
        using enum validity_flags;

        validity_criteria_status_type status = ~valid;
        auto const                    length = stl::distance(spos, send);


        // 2,3,4. Check hyphens
        if (has_flag(flags, dash)) {
            if constexpr (Options.CheckHyphens && Options.CheckDecodeAndValidateLabels) {
                switch (length) {
                    [[likely]] default:
                    case 4: {
                        utf32_forward_iter pos{spos, send};
                        char32_t const     cp1 = *pos++;
                        ++pos;
                        char32_t const cp3      = *pos++;
                        char32_t const cp4      = *pos;
                        auto const     cp_back  = *stl::prev(send); // spos + length - 1
                        // the label must not contain a U+002D HYPHEN-MINUS in both the third and fourth positions
                        status                 |= cp3 == '-' && cp4 == '-' ? ~hyphen_34 : ~valid; // 3rd and 4th
                        // the label must neither begin nor end with a U+002D HYPHEN-MINUS character.
                        status |= cp1 == '-' || cp_back == '-' ? ~hyphen_around : ~valid; // first or last
                        break;
                    }
                    case 3: {
                        auto const cp1      = *spos;
                        auto const cp_back  = *stl::prev(send);
                        status             |= cp1 == '-' || cp_back == '-' ? ~hyphen_around : ~valid; // first or last
                        break;
                    }
                    case 2: {
                        // don't need to use utf32 iterator for 2 chars
                        auto const cp1  = *spos;
                        auto const cp2  = *stl::next(spos);
                        status         |= cp1 == '-' || cp2 == '-' ? ~hyphen_around : ~valid; // first or last
                        break;
                    }
                    case 1:
                        status |= *spos == '-' ? ~hyphen_around : ~valid; // first or last
                        break;
                    case 0: break;
                }
            } else if constexpr (Options.CheckACE && Options.CheckDecodeAndValidateLabels) {
                Iter pos = spos;

                status |=
                  // NOLINTNEXTLINE(*-inc-dec-in-conditions)
                  has_flag(flags, ace) && length >= 4 && *pos++ == 'x' && *pos++ == 'n' && *pos++ == '-' && *pos == '-'
                    ? ~ace_found
                    : ~valid;
            }
        }

        auto const first_cp     = checked::next_code_point_copy<return_replacement>(spos, send);
        bool const has_unicode  = has_flag(flags, non_ascii);
        bool const check_bidi   = Options.CheckBidi && (has_unicode || charmap_half{DIGIT<char>}.contains(first_cp));
        bool const has_mappable = has_flag(flags, non_ascii, ascii_upper);

        // 5. Check if it includes any dots
        status |= Options.CheckDotInclusions && has_flag(flags, dot) ? ~dot_found : ~valid;

        // Length check
        status |= Options.VerifyDnsLength && length == 0 ? ~empty_label : ~valid;

        if (!has_unicode) [[likely]] {
            if constexpr (Options.CheckBidi) {
                using enum direction;
                bool const valid_first  = (direction_mask_of(first_cp) & bidi_mask(L, R, AL)) != 0;
                status                 |= check_bidi && !valid_first ? ~bidi_failure : ~valid;
            }
            status |=
              !Options.UseSTD3ASCIIRules || ASCII_STD3_RULES.contains(spos, send) ? ~valid : ~requires_mapping_failure;
            return status;
        }

        // 6. The label must not start with a combining mark
        // no need to check the length, it'll return 0, which is not GC, so it's fine.
        status |= Options.CheckCombiningMarkAtLabelStart && is_general_category_of(first_cp, general_category::Mark)
                    ? ~combining_mark_at_start
                    : ~valid;

        [[maybe_unused]] Iter const   sbeg     = spos;
        [[maybe_unused]] Iter         starter  = spos;
        [[maybe_unused]] Iter         prev     = spos;
        [[maybe_unused]] stl::uint8_t prev_ccc = 0;
        [[maybe_unused]] auto         result   = +quick_check_state::YES;
        [[maybe_unused]] bidi_info    b_info{};
        char32_t                      last_cp = 0;

        // 9. (partially) initialize bidi information
        if (check_bidi && spos != send) [[likely]] {
            unicode::details::bidi_info_first(b_info, first_cp);
        }

        for (Iter pos = spos; pos != send;) {
            // return replacement character because an invalid code point is not NFC failure
            char32_t const code_point = checked::next_code_point<return_replacement>(pos, send);

            // 1. Check if it's in NFC form
            // We do have isNFC function, but we are already iterating through the string,
            // so we might as well do it here.
            // This is almost the implementation of QuickCheck:
            if (Options.CheckNFC && result != +quick_check_state::NO) {
                for (;;) {
                    auto const info    = qc_ccc_of(code_point);
                    auto const ccc     = static_cast<stl::uint8_t>(info & 0xFFU);
                    auto const qc_val  = static_cast<stl::uint8_t>(info >> 8U);
                    result            |= +qc_of<norm_form::NFC>(qc_val);

                    if (result != +quick_check_state::YES) [[unlikely]] {
                        status |= result == +quick_check_state::NO ? ~nfc_failure : ~valid;
                        break;
                    }

                    if (ccc == 0) {
                        starter = prev;
                    } else if (prev_ccc > ccc) [[unlikely]] {
                        status |= ~nfc_failure;
                        break;
                    }
                    prev_ccc = ccc;
                    break;
                }

                if (result == +quick_check_state::MAYBE && !isNFC_until_next_starter(starter, send)) [[unlikely]] {
                    result |= +quick_check_state::NO;
                    status |= ~nfc_failure;
                }

                prev = pos;
            }


            // 7. Checking Status values
            if constexpr (Options.CheckMappingRequired) {
                // https://www.unicode.org/reports/tr46/#Deviations
                // Deviations are considered valid in IDNA2008 and UTS #46.
                // - For Transitional Processing (deprecated)
                // - For Nontransitional Processing, each value must be either valid or deviation.
                // - In addition,
                status |= has_mappable && status_of(code_point) != details::valid ? ~requires_mapping_failure : ~valid;

                // if UseSTD3ASCIIRules=true and the code point is an ASCII code point (U+0000..U+007F), then it
                // must be a lowercase letter (a-z), a digit (0-9), or a hyphen-minus (U+002D). (Note: This excludes
                // uppercase ASCII A-Z which are mapped in UTS #46 and disallowed in IDNA2008.)
                status |= !Options.UseSTD3ASCIIRules || !is_ascii(code_point) || ASCII_STD3_RULES.contains(code_point)
                            ? ~valid
                            : ~requires_mapping_failure;
            }



            // 8. and 5. Check joiners
            // read validate_context_joiners for details on how this works
            switch (code_point) {
                case U'\x200C': // ZERO WIDTH NON-JOINER
                    status |= Options.CheckJoiners && !validate_zero_with_non_joiner(sbeg, pos, send, last_cp)
                                ? ~joiner_failure
                                : ~valid;
                    break;
                case U'\x200D': // ZERO WIDTH JOINER
                    status |= Options.CheckJoiners && !validate_zero_with_joiner(last_cp) ? ~joiner_failure : ~valid;
                    break;
                [[likely]] default:
                    break;
            }


            // 9. Check bidi rule (get the information)
            if (check_bidi) {
                unicode::details::bidi_info_step(b_info, code_point);
            }

            last_cp = code_point;
        }


        // 9. Check bidi rule
        if (check_bidi) {
            // The documentation asks us to "If CheckBidi, and if the domain name is a 'Bidi domain name'",
            // but we don't yet know if the full domain is a bidi domain or not. It's on the caller to
            // check the status code for bidi_failures.
            status |= !validate_bidi_rule(b_info) ? ~bidi_failure : ~valid;
            status |= is_bidi_domain_name(b_info) ? ~bidi_domain_name : ~valid;
        }

        return status;
    }

    template <idna_options Options = idna_options{}, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool is_label_valid(Iter const& spos, Iter const& send) noexcept {
        return is_valid(label_validity_status<Options>(spos, send));
    }

    template <idna_options Options = idna_options{}, istl::StringViewifiable StrT>
    [[nodiscard]] static constexpr bool is_label_valid(StrT&& inp_str) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        return is_label_valid<Options>(str.begin(), str.end());
    }

    /**
     * Check Validity Criteria for the whole domain
     */
    template <idna_options Options = idna_options{}, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool is_domain_valid(Iter spos, Iter const& send) noexcept {
        using enum validity_criteria_status;

        // todo: use sentinels to optimize the double passing
        validity_criteria_status_type status = +valid;
        for (Iter beg = spos; spos != send;) {
            if (*spos != '.') [[likely]] {
                ++spos;
                continue;
            }
            status |= label_validity_status<Options>(beg, spos);
            beg     = ++spos;
        }

        // Remove the bidi_failure if the domain is a bidi domain name:
        if (!has_flag(status, bidi_domain_name) && has_flag(status, bidi_failure)) {
            status &= +bidi_failure;
        }

        return status;
    }


} // namespace webpp::unicode::idna

#endif // WEBPP_UNICODE_IDNA_VALIDITY_CRITERIA_HPP
