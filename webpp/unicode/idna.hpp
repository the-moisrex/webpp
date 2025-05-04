// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

#include "../std/expected.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
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
        invalid_code_point = stl::to_underlying(punycode_status::bad_input),
        punycode_overflow  = stl::to_underlying(punycode_status::overflow),

        // More errors:
        empty_domain_label       = 0b1U << 3U,
        too_long_label           = 0b1U << 4U, // the subdomain is more than 63
        too_long_domain          = 0b1U << 5U, // the whole domain is more than 255
        failed_validity_criteria = 0b1U << 6U, // the label failed the validity criteria requirements.
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
     * The ToASCII operation takes a sequence of Unicode code points that
     * make up one label and transforms it into a sequence of code points in
     * the ASCII range (0..7F).  If ToASCII succeeds, the original sequence
     * and the resulting sequence are equivalent labels.
     *  - From RFC 3490
     *
     *         RFC: https://www.rfc-editor.org/rfc/rfc3490.html#section-4.1
     *     UTS #46: https://www.unicode.org/reports/tr46/#ToASCII
     *  Steps From: https://www.unicode.org/reports/tr46/#Processing
     */
    template <idna_options Options = {}, stl::random_access_iterator Iter, stl::random_access_iterator OIter>
    [[nodiscard]] static constexpr to_ascii_status_type
    to_ascii(Iter spos, Iter const send, OIter& out) noexcept {
        using enum to_ascii_status;
        using istl::iter_append;
        using stl::to_underlying;
        using unicode::normalization_form;


        // Normalization is guaranteed to not require more space than 3 times the input.
        // If VerifyDnsLength is needed, IDNA Mapping will require no more than 254 max size
        // Otherwise, the max size is essentially unlimited or limited by integer overflows.
        auto const src_length = send - spos;
        auto       status     = to_underlying(valid);
        auto const obeg       = out;

        // If output is in between the input, it's a disaster waiting to happen.
        assert(!(out > spos && out < send));

        // 1. Processing
        // https://www.unicode.org/reports/tr46/#Processing

        // 1.1 Map (and/or copy to output)
        if (!idna::map(spos, send, out)) [[unlikely]] {
            // Disallowed code point was found
            status |= to_underlying(invalid_code_point);
        }

        // 1.2. Normalize inplace
        {
            auto       pos  = istl::appendable_next(out, obeg);
            auto const oend = istl::appendable_end(out);
            normalize<normalization_form::NFC>(pos, oend, out);
        }

        // 1.3. Break: Break the string into labels at U+002E (.) FULL STOP
        using flag_type                                 = stl::uint_fast8_t;
        webpp_static_constexpr flag_type x_flag         = 0b1U;
        webpp_static_constexpr flag_type n_flag         = 0b10U;
        webpp_static_constexpr flag_type dash_flag      = 0b100U;
        webpp_static_constexpr flag_type non_ascii_flag = 0b1000U;
        webpp_static_constexpr flag_type ascii_flag     = 0b1'0000U;
        webpp_static_constexpr flag_type dot_flag       = 0b100'0000U | ascii_flag;
        webpp_static_constexpr flag_type ace_flag   = x_flag | n_flag | dash_flag | ascii_flag; // ACE prefix
        webpp_static_constexpr auto      clean_flag = static_cast<flag_type>(~dot_flag);
        webpp_static_constexpr auto      interesting_characters = categorize<flag_type, 256U>(
          cat{.set = ".", .value = dot_flag},
          cat{.set = "xX", .value = x_flag},
          cat{.set = "nN", .value = n_flag},
          cat{.set = "-", .value = dash_flag},
          cat{.set = NON_ASCII_CODE_UNITS, .value = non_ascii_flag},
          cat{.set = ALL_ASCII<char8_t>, .value = ascii_flag});

        stl::size_t accum_length = 0;
        auto        pos          = istl::appendable_next(out, obeg);
        auto const  oend         = istl::appendable_end(out);
        for (; spos != send; ++spos) {
            auto const lpos = spos; // start of label

            // find the label:
            flag_type const flag = or_all_if<flag_type>(
              interesting_characters,
              spos,
              send,
              [](flag_type const cur_flags) constexpr noexcept -> bool {
                  return cur_flags >= dot_flag; // we found a dot
              });

            auto const label_length = spos - lpos;

            // 1.4. Convert/Validate. For each label in the domain_name string:
            switch (flag & clean_flag) {
                [[unlikely]] case 0:
                    // If the label is empty, or ..., record that there was an error.
                    status |= to_underlying(empty_domain_label);
                    break;
                case ace_flag:
                    if (label_length >= 4 && lpos[0] == 'x' && lpos[1] == 'n' && lpos[2] == '-' &&
                        lpos[3] == '-')
                    {
                        // Found xn--.
                        // 1.4.1. If the label contains any non-ASCII code point (i.e., a Code Point greater
                        // than U+007F), record that there was an error, and continue with the next label.
                        if ((flag & non_ascii_flag) != 0) [[unlikely]] {
                            status |= to_underlying(invalid_code_point);
                            continue;
                        }

                        // 1.4.2. Attempt to convert the rest of the label to Unicode according to Punycode
                        // [RFC3492]. If that conversion fails and if not IgnoreInvalidPunycode, record that
                        // there was an error, and continue with the next label. Otherwise, replace the
                        // original label in the string by the results of the conversion.
                        // todo: output is not correct
                        auto const pun_status = punycode_decode(lpos, spos, out);
                        if (pun_status != punycode_status::success) [[unlikely]] {
                            status |= to_underlying(pun_status);
                        }

                        // 1.4.3. If the label is empty, or if the label contains only ASCII code points,
                        // record that there was an error.
                        // todo

                        // 1.4.4. If the label is empty, or if the label contains only ASCII code points,
                        // record that there was an error.
                        // todo
                    }
                    [[fallthrough]];
                [[likely]] default:
                    // 1.4.4. Verify that the label meets the validity criteria in Section 4.1, Validity
                    // Criteria. If any of the validity criteria are not satisfied, record that there was
                    // an error.
                    if (!is_label_valid(lpos, spos)) [[unlikely]] {
                        status |= to_underlying(failed_validity_criteria);
                    }
                    break;
            }

            accum_length |= label_length;

            // 3. Punycode
            // Convert each label with non-ASCII characters into Punycode [RFC3492], and prefix by “xn--”.
            // This may record an error.
            if ((flag & non_ascii_flag) != 0) {
                iter_append(out, 'x', 'n', '-', '-'); // prepend ACE prefix
                [[maybe_unused]] auto const p_status = punycode_encode(lpos, spos, out);
                if constexpr (!Options.IgnoreInvalidPunycode) {
                    status |= to_underlying(p_status);
                }
            }


            // 6. Join the labels using U+002E FULL STOP as a separator and return the result
            if ((flag & dot_flag) == dot_flag && flag != dot_flag) { // every label except the last label
                iter_append(out, '.');
            }
        }



        // 4. VerifyDnsLength
        if constexpr (Options.VerifyDnsLength) {
            // no need to bailout early
            constexpr auto max_label   = 63U;
            constexpr auto max_domain  = 253U;
            status                     |= accum_length > max_label ? to_underlying(too_long_label) : status;
            if (out.size() > max_domain && (out.size() != max_domain + 1 || out.back() != '.')) [[unlikely]] {
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
        return status;
    }

    template <idna_options Options = {}, stl::random_access_iterator Iter, istl::String StrT = stl::u8string>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(Iter spos, Iter const send, StrT& out) {
        if constexpr (Options.VerifyDnsLength) {
            using char_type                        = istl::char_type_of_t<StrT>;
            constexpr auto       max_domain_length = 254U;
            to_ascii_status_type status            = 0;
            istl::resize_and_overwrite(
              out,
              max_domain_length,
              [&](char_type* buf, [[maybe_unused]] stl::size_t max_len) constexpr noexcept {
                  auto const beg = buf;
                  status         = unsafe_to_ascii<Options>(spos, send, buf);
                  return static_cast<stl::size_t>(buf - beg);
              });
            return status;
        } else {
            return unsafe_to_ascii<Options>(spos, send, out);
        }
    }

    template <idna_options Options = {}, istl::StringViewifiable StrVT, istl::String StrT = stl::u8string>
    [[nodiscard]] static constexpr to_ascii_status_type to_ascii(StrVT&& src, StrT& out) {
        auto const src_v = istl::string_viewify(stl::forward<StrVT>(src));
        return to_ascii<Options>(src_v.begin(), src_v.end(), out);
    }

    template <idna_options   Options = {},
              istl::CharType CharT   = char,
              istl::String   OutStrT = stl::basic_string<CharT>,
              typename... Args>
    [[nodiscard]] static constexpr stl::expected<OutStrT, to_ascii_status_type> to_ascii(
      stl::basic_string_view<CharT> src,
      Args&&... args) {
        OutStrT    out{stl::forward<Args>(args)...};
        auto const status = to_ascii<Options>(src.begin(), src.end(), out);
        if (status == to_ascii_status::valid) {
            return out;
        }
        return status;
    }

    [[nodiscard]] static constexpr bool operator==(to_ascii_status_type const lhs,
                                                   to_ascii_status const      rhs) noexcept {
        return lhs == static_cast<to_ascii_status_type>(rhs);
    }

    template <istl::String OutStrT>
    [[nodiscard]] static constexpr bool operator==(stl::expected<OutStrT, to_ascii_status_type> const lhs,
                                                   to_ascii_status const rhs) noexcept {
        to_ascii_status_type const status =
          lhs.has_value() ? stl::to_underlying(to_ascii_status::valid) : lhs.error();
        return status == static_cast<to_ascii_status_type>(rhs);
    }

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
