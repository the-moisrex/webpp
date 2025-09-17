// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

#include "../std/expected.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "../strings/to_case.hpp"
#include "./bidi.hpp"
#include "./details/idna_mapping_tables.hpp"
#include "./general_category.hpp"
#include "./joiners.hpp"
#include "./normalization.hpp"
#include "./punycodes.hpp"
#include "./unicode.hpp"

#include <cassert>
#include <climits>
#include <cstdint>
#include <iterator>
#include <limits>

namespace webpp::unicode::idna {

    /// https://www.unicode.org/reports/tr46/#Validity_Criteria
    static constexpr charmap<256U> ASCII_STD3_RULES{LOWER_ALPHA<char>, DIGIT<char>, charset('-')};
    static constexpr auto          NON_ASCII_CODE_UNITS = bitmap_range<0x80U, 0xFFU, 256>();

    /**
     * Get the status of the specified code point in regarding their IDNA status.
     * The result is:
     *   - VALID
     *   - DISALLOWED
     *   - Mapped: if it's not valid/disallowed, the return value is the starting position of
     *             the mapped value in the idna_mappings table.
     */
    [[nodiscard]] static constexpr stl::uint16_t status_of(char32_t const code_point) noexcept {
        using details::disallowed;
        using details::idna_index;
        using details::not_mapped;

        // NOLINTBEGIN(*-pro-bounds-constant-array-index, *-avoid-nested-conditional-operator)
        auto const chunk         = code_point >> idna_index::chunk_shift;
        auto const section_index = static_cast<stl::uint16_t>(chunk >> details::idna_breakpoint_shift);

        if (static_cast<stl::int32_t>(code_point) < 0 || section_index >= details::idna_last_breakpoint) [[unlikely]] {
            return disallowed;
        }

        auto const [starting, ending, offset, common_value] = details::idna_breakpoints[section_index];
        stl::uint16_t const index =
          chunk >= ending    ? common_value
          : chunk < starting ? details::idna_breakpoints[section_index - 1].common_value
                             : details::idna_mapping_ref[static_cast<stl::uint16_t>(chunk - offset)];

        auto const remaining_pos = static_cast<stl::uint16_t>(code_point & idna_index::chunk_mask);
        auto const pos           = static_cast<stl::uint16_t>((index & idna_index::pos_mask) + remaining_pos);
        if ((index & idna_index::use_second_table_mask) != 0) {
            // looking at the boolean-only table (which includes only VALID/DISALLOWED states)

            constexpr auto pack_size = sizeof(typename decltype(details::idna_mappings_bools)::value_type) * CHAR_BIT;

            // the bits in the integer are stored in reverse order, so we don't have to do additional
            // calculations to get the bit that we need.
            auto const          bpos       = pos / pack_size;
            auto const          remaining  = pos % pack_size;
            stl::uint16_t const status_bit = 0b1U & details::idna_mappings_bools[bpos] >> remaining;

            // if it's 1, it'll become valid, otherwise it'll stay disallowed
            return disallowed | status_bit;
        }

        return details::idna_mapping_blocks[pos];
        // NOLINTEND(*-pro-bounds-constant-array-index, *-avoid-nested-conditional-operator)
    }

    /**
     * Perform the mapping for a single character
     * @returns false if the code point is not allowed to be in a URL
     */
    template <istl::Appendable OutStrT = stl::u8string>
    static constexpr bool map(char32_t const code_point, OutStrT& out) noexcept(istl::NothrowAppendable<OutStrT>) {
        using details::disallowed;
        using details::idna_mappings;
        using details::valid;
        using istl::iter_append;
        auto const pos = status_of(code_point);

        // ignored code points are mapped to nothing, so no special code is needed
        switch (pos) {
            case disallowed:
                // Disallowed: Leave the code point unchanged in the string.
                // Note: The Convert/Validate step below checks for disallowed characters,
                //       after mapping and normalization.
                unchecked::append(out, code_point);
                return false; // it's not allowed
            case valid:       // or deviation
                // todo: you can optimize this, we don't have to re-convert the code point
                unchecked::append(out, code_point);
                return true;

            default: { // mapped or ignored
                auto ptr = idna_mappings.begin() + pos;
                if constexpr (UTF8String<OutStrT>) {
                    for (; *ptr != u8'\0'; ++ptr) {
                        iter_append(out, *ptr);
                    }
                } else {
                    while (*ptr != u8'\0') {
                        auto const cur_cp = unchecked::next_code_point(ptr);
                        unchecked::append(out, cur_cp);
                    }
                }
                return true;
            }
        }
    }

    template <istl::String OutStrT = stl::u8string, typename... Args>
    static constexpr OutStrT mapped(char32_t const code_point, Args&&... args) {
        OutStrT out{stl::forward<Args>(args)...};
        map(code_point, out);
        return out;
    }

    using idna_mappings_string_type = decltype(details::idna_mappings);

    [[nodiscard]] static constexpr idna_mappings_string_type::iterator mapped_begin(
      idna_mappings_string_type const& src) noexcept {
        using details::disallowed;
        using details::valid;
        auto const* beg        = stl::begin(src);
        auto const* end        = stl::end(src);
        auto const  code_point = checked::next_code_point(beg, end);

        // ignored code points are mapped to nothing, so no special code is needed
        switch (auto const pos = status_of(code_point)) {
            case disallowed: return end;
            case valid: return beg;
            default: return details::idna_mappings.begin() + pos; // mapped
        }
    }

    /**
     * Check if a string requires any type of IDNA mapping.
     */
    template <stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool requires_idna_mapping(Iter pos, Iter const end) noexcept {
        using enum checked::error_handling;
        using checked::next_code_point;
        using details::valid;
        for (;;) {
            auto const code_point = next_code_point<return_negated, Iter>(pos, end);
            if (code_point == 0) {
                break;
            }
            auto const map_pos = status_of(code_point);
            if (map_pos != valid) {
                return true;
            }
        }
        [[likely]] { return false; }
    }

    /**
     * Mapping Step of the IDNA Processing
     * UTS #46: https://www.unicode.org/reports/tr46/#ProcessingStepMap
     */
    template <istl::Appendable OutStrT, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool map(Iter beg, Iter end, OutStrT& out)
      noexcept(istl::NothrowAppendable<OutStrT>) {
        using checked::next_code_point;
        using details::disallowed;
        using details::idna_mappings;
        using details::valid;
        using istl::iter_append;
        using istl::iter_append_range;
        using enum checked::error_handling;
        using inp_char_type = stl::iter_value_t<Iter>;
        using out_char_type = istl::appendable_value_type_t<OutStrT>;

        if constexpr (stl::same_as<Iter, OutStrT>) {
            // no inplace mapping
            assert(!(out >= beg && out < end));
        }

        auto is_valid = true;
        for (Iter pos = beg;;) {
            Iter const cp_beg     = pos;
            auto const code_point = next_code_point<return_negated, Iter>(pos, end);
            if (code_point == 0) {
                break;
            }
            if (code_point < 0) [[unlikely]] {
                is_valid = false;
                iter_append_range(out, cp_beg, pos);
                continue;
            }

            auto const map_pos = status_of(code_point);

            // ignored code points are mapped to nothing, so no special code is needed
            switch (map_pos) {
                case disallowed:
                case valid: // Or deviation
                    // Disallowed: Leave the code point unchanged in the string. Note: The Convert/Validate
                    //             step below checks for disallowed characters, after mapping
                    //             and normalization.
                    if constexpr (sizeof(inp_char_type) == sizeof(out_char_type)) {
                        iter_append_range(out, cp_beg, pos);
                    } else {
                        unchecked::append(out, code_point);
                    }
                    is_valid &= map_pos == valid;
                    break;

                default: { // mapped or ignored
                    auto ptr = idna_mappings.begin() + map_pos;
                    if constexpr (UTF8<out_char_type>) {
                        for (; *ptr != u8'\0'; ++ptr) {
                            iter_append(out, *ptr);
                        }
                    } else {
                        while (*ptr != u8'\0') {
                            auto const cur_cp = unchecked::next_code_point(ptr);
                            unchecked::append(out, cur_cp);
                        }
                    }
                    break;
                }
            }
        }
        return is_valid;
    }

    template <istl::Appendable OutStrT, istl::StringViewifiable InpStrT>
    [[nodiscard]] static constexpr bool map(InpStrT&& src, OutStrT& out) noexcept(istl::NothrowAppendable<OutStrT>) {
        auto const src_view = istl::string_viewify(stl::forward<InpStrT>(src));
        using iterator      = typename decltype(src_view)::iterator;
        return map<OutStrT, iterator>(stl::begin(src_view), stl::end(src_view), out);
    }

    using to_ascii_status_type = stl::underlying_type_t<punycode_status>;
    /**
     * ToASCII status values.
     * Attention: ToASCII function may return a combination of these errors
     */
    enum struct to_ascii_status : to_ascii_status_type {
        valid = 0,

        // Punycode errors:
        invalid_code_point             = stl::to_underlying(punycode_status::bad_input),
        punycode_overflow              = stl::to_underlying(punycode_status::overflow),
        ascii_only_punycode            = 0b1U << 3U,
        empty_punycode                 = 0b1U << 4U,
        non_normalized_punycode        = 0b1U << 5U,
        punycode_requires_idna_mapping = 0b1U << 6U,


        // More errors:
        empty_domain_label       = 0b1U << 7U,
        too_long_label           = 0b1U << 8U,  // the subdomain is more than 63
        too_long_domain          = 0b1U << 9U,  // the whole domain is more than 253 without the last dot
        failed_validity_criteria = 0b1U << 10U, // the label failed the validity criteria requirements.
    };

    struct idna_options {                       // NOLINT(*-struct-pack-align)
        bool CheckHyphens          = false;
        bool CheckBidi             = true;
        bool CheckJoiners          = true;
        bool UseSTD3ASCIIRules     = false;
        bool VerifyDnsLength       = false;
        bool IgnoreInvalidPunycode = false;

        // we don't support Transitional Processing since it's been deprecated.
        // bool Transitional_Processing = false;

        // Skipped Steps:
        bool CheckNFC           = false;
        bool CheckDotInclusions = false;
        bool CheckStatusValues  = false; // rule 7 of the Validity Criteria
    };

    [[nodiscard]] static constexpr stl::string_view to_string(to_ascii_status const status) noexcept {
        using enum to_ascii_status;
        switch (status) {
            case valid: return {"valid"};
            case invalid_code_point: return {"Bad input for punycode was given."};
            case punycode_overflow: return {"Punycode overflow."};
            case ascii_only_punycode: return {"The ASCII-Only label was unnecessarily encoded into punycode."};
            case empty_punycode: return {"Empty punycode-encoded label was found."};
            case non_normalized_punycode: return {"The punycode-encoded label was not in NFC form."};
            case punycode_requires_idna_mapping: return {"The punycode-encoded label requires IDNA mapping."};
            case empty_domain_label: return {"Empty domain labels are not valid."};
            case too_long_label: return {"Label was too long."};
            case too_long_domain:
                return {"The Domain was too long."};
            [[unlikely]] default:
                break;
        }
        return {"<unknown-to-ascii-status>"};
    }

    /**
     * Check if joiner code points are correct.
     * Attention: this function does only the lookup part of the appendix, and not the full check.
     * RFC: https://www.rfc-editor.org/rfc/rfc5892.html#appendix-A
     */
    template <stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool validate_context_joiners(Iter sbeg, Iter send) noexcept {
        using enum checked::error_handling;
        using enum joiner_type;

        auto     spos       = sbeg;
        char32_t code_point = 0;
        for (;;) {
            code_point = checked::next_code_point<return_unchanged>(spos, send);
            if (code_point == 0) {
                break;
            }
            switch (code_point) {
                    // This may occur in a formally cursive script (such as Arabic) in a context where it
                    // breaks a cursive connection as required for orthographic rules, as in the Persian
                    // language, for example. It also may occur in Indic scripts in a consonant-conjunct
                    // context (immediately following a virama), to control required display of such
                    // conjuncts.
                case U'\x200C': {
                    // ZERO WIDTH NON-JOINER
                    bool       is_valid  = false;
                    auto       pos       = spos;
                    auto const before_cp = checked::prev_code_point<return_unchanged>(pos, sbeg);

                    // ccc_of(0) is not gonna be Virama, so we don't need to check for it
                    if (is_ccc_of(before_cp, ccc_props::Virama)) {
                        continue;
                    }

                    for (;;) {
                        auto const cur_cp = checked::prev_code_point<return_unchanged>(pos, sbeg);
                        if (cur_cp == 0) {
                            break;
                        }
                        auto const joining_type = joiner_type_of(cur_cp);
                        if (joining_type == transparent) {
                            continue;
                        }
                        if (joining_type == left_joining || joining_type == dual_joining) {
                            is_valid = true;
                            break;
                        }
                    }

                    // let's not early bailout on the failure path:
                    // if (!is_ok) [[unlikely]] {
                    //     return false;
                    // }

                    pos         = spos;
                    auto cur_cp = checked::next_code_point<return_unchanged>(pos, send);
                    for (; cur_cp != 0; cur_cp = checked::next_code_point<return_unchanged>(pos, send)) {
                        auto const joining_type = joiner_type_of(cur_cp);
                        if (joining_type == transparent) {
                            continue;
                        }
                        if (joining_type == right_joining || joining_type == dual_joining) {
                            is_valid = true;
                            break;
                        }
                    }
                    if (!is_valid) [[unlikely]] {
                        return false;
                    }
                    break;
                }

                    // This may occur in Indic scripts in a consonant-conjunct context (immediately following
                    // a virama), to control the required display of such conjuncts.
                case U'\x200D': { // ZERO WIDTH JOINER
                    auto       pos       = spos;
                    auto const before_cp = checked::prev_code_point<return_unchanged>(pos, sbeg);
                    // ccc_of(0) is not gonna be Virama, so we don't need to check
                    if (!is_ccc_of(before_cp, ccc_props::Virama)) {
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
    [[nodiscard]] static constexpr bool is_label_valid(Iter spos, Iter send) noexcept {
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

        // 2-4. Check hyphens (default is false)
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
            auto const cur_cp = checked::next_code_point_copy<return_unchanged>(spos, send);

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
            for (;;) {
                auto const cur_cp = checked::next_code_point<return_unchanged>(pos, send);
                if (cur_cp == 0) {
                    break;
                }
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
            valid &= validate_bidi_rule(spos, send);
        }

        return valid;
    }

    template <idna_options Options = idna_options{}, istl::StringViewifiable StrT>
    [[nodiscard]] static constexpr bool is_label_valid(StrT&& inp_str) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        return is_label_valid<Options>(str.begin(), str.end());
    }

    /**
     * This class helps you get information about your string before you allocate enough storage for toASCII
     * algorithm.
     */
    struct to_ascii_info {
        using flag_type = stl::uint_fast8_t;
        enum struct flag_types : flag_type {
            // ASCII and Non-ASCII:
            non_ascii   = 0b1000U,
            ascii       = 0b1'0000U,
            ascii_upper = 0b10'0000U | ascii,
            dot         = 0b100'0000U | ascii,

            // xn-- (Called ACE Prefix):
            x    = 0b1U | ascii,
            n    = 0b10U | ascii,
            dash = 0b100U | ascii,
            ace  = x | n | dash, // ACE prefix

            // Misc:
            clean         = static_cast<flag_type>(~dot | ascii),
            length_police = (dot | non_ascii) & ~ascii,
            all           = 0b1111'1111U, // all possibilities
        };

        // array<flag_types, 256>
        static constexpr auto interesting_characters = categorize<256U>(
          cat{.set = ".", .value = flag_types::dot},
          cat{.set = "xX", .value = flag_types::x},
          cat{.set = "nN", .value = flag_types::n},
          cat{.set = "-", .value = flag_types::dash},
          cat{.set = NON_ASCII_CODE_UNITS, .value = flag_types::non_ascii},
          cat{.set = ALL_ASCII<char8_t>, .value = flag_types::ascii},
          cat{.set = UPPER_ALPHA<char8_t>, .value = flag_types::ascii_upper});


        stl::size_t max_size = 0; // not adjusted to the output size if the input and output's character types
                                  // are different.

        [[nodiscard]] static constexpr stl::uint8_t best_factor_of(char32_t const code_point) noexcept {
            constexpr stl::uint32_t split = 24U;
            constexpr stl::uint32_t mask  = (0b1U << split) - 1U;
            auto const              inf   = details::idna_max_len_factors[code_point % details::idna_rem];
            if ((inf & mask) == code_point) [[unlikely]] {
                return static_cast<stl::uint8_t>(inf >> split);
            }
            return details::idna_default_max_len_factor;
        }

        /**
         * @returns maximum required storage length for conversion; zero if no need for conversion.
         */
        template <UTF OutCharT, stl::random_access_iterator Iter>
        [[nodiscard]] constexpr flag_type operator()(Iter spos, Iter send) noexcept {
            using enum flag_types;
            using enum checked::error_handling;
            using details::idna_default_max_len_factor;
            using stl::to_underlying;
            using inp_char_type = stl::iter_value_t<Iter>;

            auto const cur_len = adjust_utf_output_size<inp_char_type, OutCharT>(static_cast<stl::size_t>(send - spos));
            flag_type  flags   = 0U;
            stl::size_t biggest_label = 0U;
            auto        lbeg          = spos;

            max_size  = cur_len;
            max_size *= static_cast<stl::size_t>(idna_default_max_len_factor);

            // We can't rely on finding dots and using them as label lengths since this is before IDNA Mapping
            // takes place and here, the dots may be in Unicode. But, if the dots are in Unicode, then we
            // consider the whole string as one big label.
            while (spos != send) {
                flag_type const flag = or_all_if<flag_type>(
                  interesting_characters,
                  spos,
                  send,
                  [](flag_type const cur_flag) constexpr noexcept {
                      return (cur_flag & to_underlying(length_police)) != 0;
                  });

                flags |= flag;

                if ((flag & to_underlying(dot)) == to_underlying(dot)) {
                    biggest_label = stl::max<stl::size_t>(biggest_label, static_cast<stl::size_t>(spos - lbeg));
                    lbeg          = spos;
                } else if ((flag & to_underlying(non_ascii)) != 0) {
                    auto const code_point = checked::next_code_point<return_unchanged>(spos, send);

                    // Update the max size
                    auto map_count  = adjust_utf_output_size<char32_t, OutCharT>(best_factor_of(code_point));
                    map_count      *= 4; // For punycode: each code point at max may turn into N ascii chars
                    max_size       += map_count;
                    max_size       -= idna_default_max_len_factor; // remove the default max len factor
                    max_size       += 4;                           // each label can have an ACE Prefix (xn--)
                }
            }

            biggest_label = stl::max<stl::size_t>(biggest_label, static_cast<stl::size_t>(spos - lbeg));

            // We only care about the biggest label because punycode conversions happen on each label, and the
            // biggest label would become the maximum required length for processing.
            max_size += biggest_label * 3U; // max punycode

            // We're not going to apply this since the toASCII function itself may encounter undefined
            // behaviors when we don't reserve enough storage for it, and we don't want to make that algorithm
            // slower.
            //
            // if constexpr (Options.VerifyDnsLength) {
            //     // The length of the domain name, excluding the root label and its dot, is from 1 to 253.
            //     max_size = stl::max<stl::size_t>(max_size, 254U); // NOLINT(*-magic-numbers)
            // }

            return flags;
        }
    };

    /**
     * The ToASCII operation takes a sequence of Unicode code points that
     * make up one label and transforms it into a sequence of code points in
     * the ASCII range (0..7F).  If ToASCII succeeds, the original sequence
     * and the resulting sequence are equivalent labels.
     *  - From RFC 3490
     *
     *         RFC: https://www.rfc-editor.org/rfc/rfc3490.html#section-4.1
     *     UTS #46: https://www.unicode.org/reports/tr46/#ToASCII
     *  Steps From: https://www.unicode.org/reports/tr46/#Processing
     *    Used by:  https://url.spec.whatwg.org/#idna
     */
    template <idna_options                Options = idna_options{},
              stl::random_access_iterator Iter,
              stl::random_access_iterator OIter>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(
      Iter                           ipos,
      Iter const                     iend,
      OIter&                         out,
      stl::size_t                    out_len,
      to_ascii_info::flag_type const flags = stl::to_underlying(to_ascii_info::flag_types::all)) noexcept {
        using enum to_ascii_status;
        using enum to_ascii_info::flag_types;
        using istl::iter_append;
        using stl::to_underlying;
        using unicode::norm_form;
        using flag_type = to_ascii_info::flag_type;
        using diff_type = stl::iter_difference_t<OIter>;


        // Normalization is guaranteed to not require more space than 3 times the input.
        // If VerifyDnsLength is needed, IDNA Mapping will require no more than 254 max size
        // Otherwise, the max size is essentially unlimited or limited by integer overflows.

        auto const  src_length          = iend - ipos;
        auto        status              = to_underlying(valid);
        OIter const out_beg             = out;
        bool const  all_ascii           = (flags & to_underlying(non_ascii)) == 0;
        bool const  might_have_punycode = (flags & to_underlying(ace)) != 0;
        bool const  all_lower_ascii     = (flags & to_underlying(ascii_upper)) == to_underlying(ascii);
        OIter       spos                = out;
        auto        send                = stl::next(spos, src_length); // init
        auto const  oend                = stl::next(out, static_cast<diff_type>(out_len));

        // If output is in between the input, it's a disaster waiting to happen.
        if constexpr (stl::same_as<Iter, OIter>) {
            assert(!(out >= ipos && out < iend));
        }
        assert(src_length < stl::numeric_limits<stl::uint32_t>::max());
        assert(out_len < stl::numeric_limits<stl::uint32_t>::max());

        // 1. Processing
        // https://www.unicode.org/reports/tr46/#Processing
        if (all_lower_ascii) {
            stl::copy(ipos, iend, out);
            stl::advance(out, src_length);
            if (!might_have_punycode) {
                return status;
            }
        } else if (all_ascii) {
            // 1.1 ASCII Map (and/or copy to output)
            ascii::lower_to(ipos, iend, out);
            stl::advance(out, src_length);
            if (!might_have_punycode) {
                return status;
            }
        } else {
            // 1.1 Map (and/or copy to output)
            if (!idna::map(ipos, iend, out)) [[unlikely]] {
                // Disallowed code point was found
                status |= to_underlying(invalid_code_point);
            }

            // 1.2. Normalize inplace
            {
                send = spos;
                normalize<norm_form::NFC>(out_beg, out, send); // inplace normalization
            }
            assert(out <= oend);                               // we ran out of space.
        }

        // 1.3. Break: Break the string into labels at U+002E (.) FULL STOP
        stl::uint16_t accum_length = 0;
        while (spos != send) {
            auto const lcbeg = spos;
            OIter      lbeg  = spos; // start of label

            // find the label:
            flag_type flag = or_all_if<flag_type>(
              to_ascii_info::interesting_characters,
              spos,
              send,
              [](flag_type const cur_flags) constexpr noexcept -> bool {
                  return cur_flags >= to_underlying(dot); // we found a dot
              });

            bool const contains_dot     = (flag & to_underlying(dot)) == to_underlying(dot);
            auto const lcend            = contains_dot ? stl::prev(spos) : spos;
            OIter      lend             = lcend;
            auto const src_label_length = lend - lbeg;

            // 1.4. Convert/Validate. For each label in the domain_name string:
            switch (flag & to_underlying(clean)) {
                [[unlikely]] case 0:
                    // If the label is empty, or ..., record that there was an error.
                    status |= to_underlying(empty_domain_label);
                    break;
                case to_underlying(ace):
                    if (src_label_length >= 4 && lbeg[0] == 'x' && lbeg[1] == 'n' && lbeg[2] == '-' && lbeg[3] == '-') {
                        // Found xn--.
                        // 1.4.1. If the label contains any non-ASCII code point (i.e., a Code Point greater
                        // than U+007F), record that there was an error, and continue with the next label.
                        if ((flag & to_underlying(non_ascii)) != 0) [[unlikely]] {
                            status |= to_underlying(invalid_code_point);
                            continue;
                        }

                        // Decode Punycode
                        // 1.4.2. Attempt to convert the rest of the label to Unicode according to Punycode
                        // [RFC3492]. If that conversion fails and if not IgnoreInvalidPunycode, record that
                        // there was an error, and continue with the next label. Otherwise, replace the
                        // original label in the string by the results of the conversion.

                        // Give enough room for re-conversion
                        // No need to take xn-- into account, it's already in 'src length'.
                        // todo: optimize this to use UTF-32 storage since it's completely temporary
                        auto const max_punycode_len = src_label_length * (4 - 1);
                        lend                        = stl::next(send, max_punycode_len);
                        lbeg                        = lend;
                        auto const pun_status       = punycode_decode(stl::next(lcbeg, 4), lcend, lend);
                        auto const new_label_len    = lend - send;
                        assert(lend <= oend);
                        if constexpr (!Options.IgnoreInvalidPunycode) {
                            if (pun_status != punycode_status::success) [[unlikely]] {
                                // restore the original label:
                                status       |= to_underlying(pun_status);
                                accum_length |= static_cast<stl::uint16_t>(lend - lbeg);
                                continue;
                            }
                        }
                        flag |= to_underlying(non_ascii); // make sure to re-convert it back to punycode

                        // 1.4.3. If the label is empty, or if the label contains only ASCII code points,
                        // record that there was an error.
                        if (new_label_len == 0) [[unlikely]] {
                            status |= to_underlying(empty_punycode);
                        }
                        if (is_ascii(lbeg, lend)) [[unlikely]] {
                            status |= to_underlying(ascii_only_punycode);
                        }

                        if (!is_normalized<norm_form::NFC>(lbeg, lend)) [[unlikely]] {
                            status |= to_underlying(non_normalized_punycode);
                        }

                        if (requires_idna_mapping(lbeg, lend)) [[unlikely]] {
                            status |= to_underlying(punycode_requires_idna_mapping);
                        }
                    }
                    [[fallthrough]];
                [[likely]] default:
                    // 1.4.4. Verify that the label meets the validity criteria in Section 4.1, Validity
                    // Criteria. If any of the validity criteria are not satisfied, record that there was
                    // an error.
                    if (!is_label_valid<Options>(lbeg, lend)) [[unlikely]] {
                        status |= to_underlying(failed_validity_criteria);
                    }
                    break;
            }

            // don't worry about length being longer than uint16_t, it'll require it to be more than the max
            // size for that to happen.
            accum_length |= static_cast<stl::uint16_t>(lend - lbeg);

            // 3. Encode Punycode
            // Converts each label with non-ASCII characters into Punycode [RFC3492], and prefixes by “xn--”.
            // This may record an error.
            if ((flag & to_underlying(non_ascii)) != 0) {
                out                = send;
                auto const tmp_beg = out;
                iter_append(out, 'x', 'n', '-', '-');
                [[maybe_unused]] auto const p_status = punycode_encode(lbeg, lend, out);

                // We ran out of space
                assert(out <= oend);

                // Move the new generated label to its rightful place:
                lbeg = lcbeg;
                stl::rotate(lcend, tmp_beg, out);
                stl::shift_left(lbeg, out, src_label_length);
                stl::advance(out, -src_label_length);

                if constexpr (!Options.IgnoreInvalidPunycode) {
                    if (p_status != punycode_status::success) [[unlikely]] {
                        status |= to_underlying(p_status);
                    }
                }
            }


            // 6. Join the labels using U+002E FULL STOP as a separator and return the result
            // if (contains_dot) {
            //     *lcend = '.';
            // }
        }



        // 4. VerifyDnsLength
        if constexpr (Options.VerifyDnsLength) {
            // No need to bailout early
            constexpr auto max_label   = 63U;
            constexpr auto max_domain  = 253U;
            auto const     cur_out_len = out - out_beg;
            if (accum_length > max_label) [[unlikely]] {
                status |= to_underlying(too_long_label);
            }
            if (cur_out_len > max_domain && (cur_out_len != max_domain + 1 || *stl::prev(out) != '.')) [[unlikely]] {
                status |= to_underlying(too_long_domain);
            }
        }


        // Implementations are advised to apply additional tests to these labels, such as those described in
        // Unicode Technical Report #36, Unicode Security Considerations [UTR36] and Unicode Technical
        // Standard #39, Unicode Security Mechanisms [UTS39], and take appropriate actions. For example, a
        // label with mixed scripts or confusables may be called out in the UI. Note that the use of Punycode
        // to signal problems may be counter-productive, as described in [UTR36].

        // 5. If an error was recorded in steps 1-4, then the operation has failed and a failure value is
        // returned. No DNS lookup should be done.
        if (status != to_underlying(valid)) [[unlikely]] {
            out = out_beg;
        }
        *out = '\0';
        return status;
    }

    template <idna_options                Options = idna_options{},
              stl::random_access_iterator Iter,
              istl::String                StrT = stl::u8string>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(Iter spos, Iter const send, StrT& out) {
        using output_char_type      = istl::char_type_of_t<StrT>;
        to_ascii_status_type status = 0;
        to_ascii_info        info;
        auto const           flags = info.operator()<output_char_type>(spos, send);
        istl::resize_and_overwrite(
          out,
          info.max_size,
          [&, flags](output_char_type* buf, stl::size_t const max_len) constexpr noexcept {
              auto const beg = buf;
              status         = to_ascii<Options>(spos, send, buf, max_len, flags);
              auto const len = static_cast<stl::size_t>(buf - beg);
              assert(len <= max_len); // let's not rely on -D_GLIBCXX_ASSERTS or -D_GLIBCXX_DEBUG
              return len;
          });
        return status;
    }

    template <idna_options Options = idna_options{}, istl::StringViewifiable StrVT, istl::String StrT = stl::u8string>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(StrVT&& src, StrT& out) {
        auto const src_v = istl::string_viewify(stl::forward<StrVT>(src));
        return to_ascii<Options>(src_v.begin(), src_v.end(), out);
    }

    template <istl::String            OutStrT = stl::u8string,
              idna_options            Options = idna_options{},
              istl::StringViewifiable StrT,
              typename... Args>
    [[nodiscard]] static constexpr stl::expected<OutStrT, to_ascii_status_type> to_ascii(StrT&& src, Args&&... args) {
        auto const str = istl::string_viewify(stl::forward<StrT>(src));
        OutStrT    out{stl::forward<Args>(args)...};
        auto const status = to_ascii<Options>(str.begin(), str.end(), out);
        if (status == to_ascii_status::valid) {
            return out;
        }
        return stl::unexpected{status};
    }

    [[nodiscard]] static constexpr bool operator==(to_ascii_status_type const lhs, to_ascii_status const rhs) noexcept {
        return lhs == static_cast<to_ascii_status_type>(rhs);
    }

    template <istl::String OutStrT>
    [[nodiscard]] static constexpr bool operator==(stl::expected<OutStrT, to_ascii_status_type> const& lhs,
                                                   to_ascii_status const                               rhs) noexcept {
        to_ascii_status_type const status = lhs.has_value() ? stl::to_underlying(to_ascii_status::valid) : lhs.error();
        return status == static_cast<to_ascii_status_type>(rhs);
    }

    template <istl::String OutStrT, istl::StringViewifiable StrV>
    [[nodiscard]] static constexpr bool operator==(stl::expected<OutStrT, to_ascii_status_type> const& lhs,
                                                   StrV&&                                              rhs) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrV>(rhs));
        if (lhs.has_value()) {
            return lhs.value() == str;
        }
        return false;
    }

    [[nodiscard]] static constexpr bool operator!=(to_ascii_status_type const lhs, to_ascii_status const rhs) noexcept {
        return lhs != static_cast<to_ascii_status_type>(rhs);
    }

    [[nodiscard]] static constexpr to_ascii_status_type operator&(
      to_ascii_status_type const lhs,
      to_ascii_status const      rhs) noexcept {
        return lhs & static_cast<to_ascii_status_type>(rhs);
    }

    [[nodiscard]] static constexpr to_ascii_status_type operator&(
      to_ascii_status const      lhs,
      to_ascii_status_type const rhs) noexcept {
        return static_cast<to_ascii_status_type>(lhs) & rhs;
    }


} // namespace webpp::unicode::idna

#endif // WEBPP_URI_IDNA_MAPPINGS_HPP
