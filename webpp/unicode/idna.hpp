// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

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
    template <UTF32 CharT = char32_t>
    [[nodiscard]] static constexpr stl::uint16_t status_of(CharT const code_point) noexcept {
        using details::disallowed;
        using details::idna_index;
        using details::not_mapped;

        // NOLINTBEGIN(*-pro-bounds-constant-array-index, *-avoid-nested-conditional-operator)
        auto const chunk         = code_point >> idna_index::chunk_shift;
        auto const section_index = static_cast<stl::uint16_t>(chunk >> details::idna_breakpoint_shift);

        if (code_point < 0 || section_index >= details::idna_last_breakpoint) [[unlikely]] {
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

            constexpr auto pack_size =
              sizeof(typename decltype(details::idna_mappings_bools)::value_type) * CHAR_BIT;

            // the bits in the integer are stored in reverse order, so we don't have to do additional
            // calculations to get the bit that we need.
            auto const          bpos       = pos / pack_size;
            auto const          remaining  = pos % pack_size;
            stl::uint16_t const status_bit = 0b1U & (details::idna_mappings_bools[bpos] >> remaining);

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
    template <UTF32 CharT = char32_t, istl::Appendable OutStrT = stl::u8string>
    static constexpr bool map(CharT const code_point, OutStrT& out)
      noexcept(istl::NothrowAppendable<OutStrT>) {
        using details::disallowed;
        using details::idna_mappings;
        using details::valid;
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
                        unchecked::append(out, *ptr);
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

    template <istl::String OutStrT = stl::u8string, UTF32 CharT = char32_t, typename... Args>
    static constexpr OutStrT mapped(CharT const code_point, Args&&... args) {
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
     * Mapping Step of the IDNA Processing
     * UTS #46: https://www.unicode.org/reports/tr46/#ProcessingStepMap
     */
    template <istl::Appendable OutStrT, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool map(Iter beg, Iter end, OutStrT& out)
      noexcept(istl::NothrowAppendable<OutStrT>) {
        using enum checked::error_handling;
        using checked::next_code_point;

        auto is_valid = true;
        for (auto pos = beg;;) {
            auto const code_point = next_code_point<return_negated_char, char32_t, Iter>(pos, end);
            if (code_point == 0) {
                break;
            }

            // Disallowed: Leave the code point unchanged in the string. Note: The Convert/Validate step below
            //             checks for disallowed characters, after mapping and normalization.
            is_valid &= map(code_point, out);
        }
        return is_valid;
    }

    template <istl::Appendable OutStrT, istl::StringViewifiable InpStrT>
    [[nodiscard]] static constexpr bool map(InpStrT&& src, OutStrT& out)
      noexcept(istl::NothrowAppendable<OutStrT>) {
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
        invalid_code_point  = stl::to_underlying(punycode_status::bad_input),
        punycode_overflow   = stl::to_underlying(punycode_status::overflow),
        ascii_only_punycode = 0b1U << 3U,
        empty_punycode      = 0b1U << 4U,

        // More errors:
        empty_domain_label       = 0b1U << 5U,
        too_long_label           = 0b1U << 6U, // the subdomain is more than 63
        too_long_domain          = 0b1U << 7U, // the whole domain is more than 253 without last dot
        failed_validity_criteria = 0b1U << 8U, // the label failed the validity criteria requirements.
    };

    struct idna_options {                      // NOLINT(*-struct-pack-align)
        bool CheckHyphens            = false;
        bool CheckBidi               = true;
        bool CheckJoiners            = true;
        bool UseSTD3ASCIIRules       = false;
        bool Transitional_Processing = false;
        bool VerifyDnsLength         = false;
        bool IgnoreInvalidPunycode   = false;

        // Skipped Steps:
        bool CheckNFC           = false;
        bool CheckDotInclusions = false;
        bool CheckStatusValues  = false; // rule 7 of the Validity Criteria
    };

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
                    auto const before_cp = checked::prev_code_point<return_unchanged>(pos, send);

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
                            is_valid &= true;
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
                            is_valid &= true;
                            break;
                        }
                    }
                    if (!is_valid) [[unlikely]] {
                        return false;
                    }
                    break;
                }

                    // This may occur in Indic scripts in a consonant-conjunct context (immediately following
                    // a virama), to control required display of such conjuncts.
                case U'\x200D': { // ZERO WIDTH JOINER
                    auto       pos       = spos;
                    auto const before_cp = checked::prev_code_point<return_unchanged>(pos, send);
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
    template <idna_options Options = {}, stl::random_access_iterator Iter>
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
            clean            = static_cast<flag_type>(~dot | ascii),
            messy_code_units = dot | non_ascii | ace,
            all              = 0b1111'1111U, // all possibilities
        };

        // array<flag_types, 256>
        static constexpr auto interesting_characters = categorize<256U>(
          cat{.set = ".", .value = flag_types::dot},
          cat{.set = "xX", .value = flag_types::x},
          cat{.set = "nN", .value = flag_types::n},
          cat{.set = "-", .value = flag_types::dash},
          cat{.set = NON_ASCII_CODE_UNITS, .value = flag_types::non_ascii},
          cat{.set = ALL_ASCII<char8_t>, .value = flag_types::ascii});


        stl::size_t max_size = 0; // not adjusted to the output size if the input and output's character types
                                  // are different.

        [[nodiscard]] static constexpr stl::uint8_t best_factor_of(UTF32 auto const code_point) noexcept {
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
        template <idna_options Options = {}, stl::random_access_iterator Iter>
        [[nodiscard]] constexpr flag_type operator()(Iter spos, Iter send) noexcept {
            using enum flag_types;
            using details::idna_default_max_len_factor;
            using stl::to_underlying;

            auto const cur_len = send - spos;
            flag_type  flags   = 0U;
            max_size           = static_cast<stl::size_t>(cur_len * idna_default_max_len_factor);

            while (spos != send) {
                flag_type const flag = or_all_if<flag_type>(
                  interesting_characters,
                  spos,
                  send,
                  [](flag_type const cur_flag) constexpr noexcept {
                      return (cur_flag & to_underlying(non_ascii)) != 0;
                  });

                flags |= flag;


                switch (flag & to_underlying(clean)) {
                    // we optimize this function for ascii-only strings
                    [[unlikely]] case to_underlying(non_ascii): {
                        auto const code_point = checked::next_code_point(spos, send);

                        // Update the max size
                        max_size += best_factor_of(code_point) - idna_default_max_len_factor;
                        break;
                    }
                }
            }

            // requires_mapping = (flags & to_underlying(non_ascii)) != 0;

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
    template <idna_options Options = {}, stl::random_access_iterator Iter, stl::random_access_iterator OIter>
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
        using unicode::normalization_form;
        using flag_type = to_ascii_info::flag_type;


        // Normalization is guaranteed to not require more space than 3 times the input.
        // If VerifyDnsLength is needed, IDNA Mapping will require no more than 254 max size
        // Otherwise, the max size is essentially unlimited or limited by integer overflows.

        auto const src_length          = iend - ipos;
        auto       status              = to_underlying(valid);
        auto const out_beg             = out;
        bool const all_ascii           = (flags & to_underlying(non_ascii)) == 0;
        bool const might_have_punycode = (flags & to_underlying(ace)) != 0;
        bool const all_lower_ascii     = (flags & to_underlying(ascii_upper)) == 0;
        auto       spos                = out;
        auto       send                = stl::next(spos, src_length); // init
        auto const oend                = out + out_len;

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
        } else if (all_ascii) {
            // 1.1 ASCII Map (and/or copy to output)
            ascii::lower_to(ipos, iend, out);
            stl::advance(out, src_length);
        } else {
            // 1.1 Map (and/or copy to output)
            if (!idna::map(ipos, iend, out)) [[unlikely]] {
                // Disallowed code point was found
                status |= to_underlying(invalid_code_point);
            }

            // 1.2. Normalize inplace
            {
                auto pos = out;
                normalize<normalization_form::NFC>(pos, send, out);
                send = pos; // the new end
            }
        }

        // 1.3. Break: Break the string into labels at U+002E (.) FULL STOP
        stl::uint16_t accum_length = 0;
        while (spos != send) {
            auto const lcbeg = spos;
            auto       lbeg  = spos; // start of label

            // find the label:
            flag_type const flag = or_all_if<flag_type>(
              to_ascii_info::interesting_characters,
              spos,
              send,
              [](flag_type const cur_flags) constexpr noexcept -> bool {
                  return cur_flags >= to_underlying(dot); // we found a dot
              });

            auto const lcend        = spos;
            auto       lend         = spos;
            auto const label_length = lend - lbeg;

            // 1.4. Convert/Validate. For each label in the domain_name string:
            switch (flag & to_underlying(clean)) {
                [[unlikely]] case 0:
                    // If the label is empty, or ..., record that there was an error.
                    status |= to_underlying(empty_domain_label);
                    out     = out_beg;
                    *out    = '\0';
                    return status;
                case to_underlying(ace):
                    if (label_length >= 4 && lbeg[0] == 'x' && lbeg[1] == 'n' && lbeg[2] == '-' &&
                        lbeg[3] == '-')
                    {
                        // Found xn--.
                        // 1.4.1. If the label contains any non-ASCII code point (i.e., a Code Point greater
                        // than U+007F), record that there was an error, and continue with the next label.
                        if ((flag & to_underlying(non_ascii)) != 0) [[unlikely]] {
                            status |= to_underlying(invalid_code_point);
                            continue;
                        }

                        // 1.4.2. Attempt to convert the rest of the label to Unicode according to Punycode
                        // [RFC3492]. If that conversion fails and if not IgnoreInvalidPunycode, record that
                        // there was an error, and continue with the next label. Otherwise, replace the
                        // original label in the string by the results of the conversion.
                        lend                     = send;
                        lbeg                     = send;
                        auto const pun_status    = punycode_decode(lcbeg, lcend, lend);
                        auto const new_label_len = lend - send;
                        if constexpr (!Options.IgnoreInvalidPunycode) {
                            if (pun_status != punycode_status::success) [[unlikely]] {
                                // restore the original label:
                                status       |= to_underlying(pun_status);
                                accum_length |= static_cast<stl::uint16_t>(lend - lbeg);
                                continue;
                            }
                        }

                        // 1.4.3. If the label is empty, or if the label contains only ASCII code points,
                        // record that there was an error.
                        if (new_label_len == 0) [[unlikely]] {
                            status |= to_underlying(empty_punycode);
                            out     = out_beg;
                            *out    = '\0';
                            return status;
                        }
                        if (is_ascii(lbeg, out)) [[unlikely]] {
                            status |= to_underlying(ascii_only_punycode);
                            out     = out_beg;
                            *out    = '\0';
                            return status;
                        }
                    }
                    [[fallthrough]];
                [[likely]] default:
                    // 1.4.4. Verify that the label meets the validity criteria in Section 4.1, Validity
                    // Criteria. If any of the validity criteria are not satisfied, record that there was
                    // an error.
                    if (!is_label_valid(lbeg, lend)) [[unlikely]] {
                        status |= to_underlying(failed_validity_criteria);
                        out     = out_beg;
                        *out    = '\0';
                        return status;
                    }
                    break;
            }

            // don't worry about length being longer than uint16_t, it'll require it to be more than the max
            // size for that to happen.
            accum_length |= static_cast<stl::uint16_t>(lend - lbeg);

            // 3. Punycode
            // Converts each label with non-ASCII characters into Punycode [RFC3492], and prefixes by “xn--”.
            // This may record an error.
            if ((flag & to_underlying(non_ascii)) != 0) {
                out                                  = stl::max(send, lend); // temp storage
                auto const                  tmp_beg  = out;
                [[maybe_unused]] auto const p_status = punycode_encode(lbeg, lend, out);
                assert(out <= oend);

                auto const label_len = out - tmp_beg + 4U;
                {
                    // Create space for the new label
                    auto const move_amount = label_len - label_length;
                    stl::copy_backward(lcend, send, stl::next(send, move_amount));
                    stl::advance(spos, move_amount);
                    stl::advance(send, move_amount);
                }
                {
                    // write the new label
                    iter_append(lbeg, 'x', 'n', '-', '-'); // prepend ACE prefix
                    stl::copy(tmp_beg, out, lbeg);
                    out = stl::next(lbeg, label_len);
                }
                if constexpr (!Options.IgnoreInvalidPunycode) {
                    if (p_status != punycode_status::success) [[unlikely]] {
                        status |= to_underlying(p_status);
                        // todo: Clearing the output is not needed?
                        *out    = '\0';
                        return status;
                    }
                }
            }


            // 6. Join the labels using U+002E FULL STOP as a separator and return the result
        }



        // 4. VerifyDnsLength
        if constexpr (Options.VerifyDnsLength) {
            // No need to bailout early
            constexpr auto max_label   = 63U;
            constexpr auto max_domain  = 253U;
            status                     |= accum_length > max_label ? to_underlying(too_long_label) : status;
            if (out.size() > max_domain && (out.size() != max_domain + 1 || out.back() != '.')) [[unlikely]] {
                status |= to_underlying(too_long_domain);
                out     = out_beg;
                *out    = '\0';
                return status;
            }
        }


        // Implementations are advised to apply additional tests to these labels, such as those described in
        // Unicode Technical Report #36, Unicode Security Considerations [UTR36] and Unicode Technical
        // Standard #39, Unicode Security Mechanisms [UTS39], and take appropriate actions. For example, a
        // label with mixed scripts or confusables may be called out in the UI. Note that the use of Punycode
        // to signal problems may be counter-productive, as described in [UTR36].

        // 5. If an error was recorded in steps 1-4, then the operation has failed and a failure value is
        // returned. No DNS lookup should be done.
        *out = '\0';
        return status;
    }

    template <idna_options Options = {}, stl::random_access_iterator Iter, istl::String StrT = stl::u8string>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(Iter spos, Iter const send, StrT& out) {
        using input_char_type       = stl::iter_value_t<Iter>;
        using output_char_type      = istl::char_type_of_t<StrT>;
        to_ascii_status_type status = 0;
        to_ascii_info        info;
        auto const           flags = info(spos, send);
        istl::resize_and_overwrite(
          out,
          adjust_utf_output_size<input_char_type, output_char_type>(info.max_size),
          [&, flags](output_char_type* buf, stl::size_t const max_len) constexpr noexcept {
              auto const beg = buf;
              status         = to_ascii<Options>(spos, send, buf, max_len, flags);
              auto const len = static_cast<stl::size_t>(buf - beg);
              assert(len <= max_len); // let's not rely on -D_GLIBCXX_ASSERTS or -D_GLIBCXX_DEBUG
              return len;
          });
        return status;
    }

    template <idna_options Options = {}, istl::StringViewifiable StrVT, istl::String StrT = stl::u8string>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(StrVT&& src, StrT& out) {
        auto const src_v = istl::string_viewify(stl::forward<StrVT>(src));
        return to_ascii<Options>(src_v.begin(), src_v.end(), out);
    }

    /**
     * @returns empty string if error occurred.
     */
    template <istl::String            StrT    = stl::u8string,
              idna_options            Options = {},
              istl::StringViewifiable StrVT,
              typename... Args>
        requires(stl::is_constructible_v<StrT, Args...> && !istl::cvref_as<StrT, Args...>)
    [[nodiscard]] static constexpr StrT to_ascii(StrVT&& src, Args&&... args) {
        using stl::to_underlying;

        StrT                        out{stl::forward<Args>(args)...};
        auto const                  src_v  = istl::string_viewify(stl::forward<StrVT>(src));
        [[maybe_unused]] auto const status = to_ascii<Options>(src_v.begin(), src_v.end(), out);
        return out;
    }

    // template <idna_options   Options = {},
    //           istl::CharType CharT   = char,
    //           istl::String   OutStrT = stl::basic_string<CharT>,
    //           typename... Args>
    // [[nodiscard]] static constexpr stl::expected<OutStrT, to_ascii_status_type> to_ascii(
    //   stl::basic_string_view<CharT> src,
    //   Args&&... args) {
    //     OutStrT    out{stl::forward<Args>(args)...};
    //     auto const status = to_ascii<Options>(src.begin(), src.end(), out);
    //     if (status == to_ascii_status::valid) {
    //         return out;
    //     }
    //     return status;
    // }

    [[nodiscard]] static constexpr bool operator==(to_ascii_status_type const lhs,
                                                   to_ascii_status const      rhs) noexcept {
        return lhs == static_cast<to_ascii_status_type>(rhs);
    }

    // template <istl::String OutStrT>
    // [[nodiscard]] static constexpr bool operator==(stl::expected<OutStrT, to_ascii_status_type> const lhs,
    //                                                to_ascii_status const rhs) noexcept {
    //     to_ascii_status_type const status =
    //       lhs.has_value() ? stl::to_underlying(to_ascii_status::valid) : lhs.error();
    //     return status == static_cast<to_ascii_status_type>(rhs);
    // }

    [[nodiscard]] static constexpr bool operator!=(to_ascii_status_type const lhs,
                                                   to_ascii_status const      rhs) noexcept {
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
