// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "./details/idna_mapping_tables.hpp"
#include "./normalization.hpp"
#include "./unicode.hpp"
#include "bidi.hpp"

#include <cassert>
#include <climits>
#include <cstdint>
#include <iterator>

namespace webpp::unicode::idna {
    template <UTF32 CharT = char32_t>
    [[nodiscard]] static constexpr stl::uint16_t status_of(CharT const code_point) noexcept {
        using details::batch_bit_count;
        using details::batch_mask;
        using details::disallowed;
        using details::idna_ref_blocks;
        using details::idna_ref_bools;
        using details::not_mapped;

        // NOLINTBEGIN(*-pro-bounds-constant-array-index)
        stl::uint16_t ref; // NOLINT(*-init-variables)
        if (code_point <= static_cast<CharT>(details::breakpoint_start)) [[likely]] {
            ref = details::idna_refs[static_cast<stl::uint16_t>(code_point >> batch_bit_count)];
        } else [[unlikely]] {
            if (code_point >= static_cast<CharT>(details::last_disallowed)) {
                return disallowed;
            }
            if (code_point >= static_cast<CharT>(details::breakpoint_end)) {
                auto const pos = static_cast<stl::uint16_t>(
                  (code_point - static_cast<CharT>(details::breakpoint_end)) >> batch_bit_count);
                ref = details::idna_refs_extra[pos];
            } else {
                return disallowed;
            }
        }

        stl::uint16_t const clean_ref = ref & static_cast<stl::uint16_t>(~details::table_pick_mask);
        auto const          ref_ptr   = static_cast<stl::uint16_t>(clean_ref + (code_point & batch_mask));
        if (clean_ref != ref) {
            // looking at the idna_ref_bools table

            constexpr auto pack_size = sizeof(typename decltype(idna_ref_bools)::value_type) * CHAR_BIT;
            auto const     pos       = ref_ptr / pack_size;

            // the bits in the integer are stored in reverse order, so we don't have to do additional
            // calculations to get the bit that we need.
            auto const          remaining  = (ref_ptr % pack_size);
            stl::uint16_t const status_bit = 0b1U & (idna_ref_bools[pos] >> remaining);

            // if it's 1, it'll become valid, otherwise it'll stay disallowed
            return disallowed | status_bit;
        }

        // we don't need to use ref_ptr, but we've already calculated it anyway:
        return idna_ref_blocks[ref_ptr];
        // NOLINTEND(*-pro-bounds-constant-array-index)
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
            case disallowed: return false; // it's not allowed
            case valid:
                // todo: you can optimize this, we don't have to re-convert the code point
                unchecked::append(out, code_point);
                return true;

            default: { // mapped
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

        for (auto pos = beg; pos != end;) {
            auto const code_point = next_code_point<return_negated_char, char32_t, Iter>(pos, end);
            if (static_cast<stl::int32_t>(code_point) < 0) [[unlikely]] {
                unchecked::append(out, -code_point);
                continue;
            }

            if (!map(code_point, out)) {
                return false;
            }
        }
        return true;
    }

    template <istl::Appendable OutStrT, istl::StringViewifiable InpStrT>
    [[nodiscard]] static constexpr bool map(InpStrT&& src, OutStrT& out)
      noexcept(istl::NothrowAppendable<OutStrT>) {
        auto const src_view = istl::string_viewify(stl::forward<InpStrT>(src));
        using iterator      = typename decltype(src_view)::iterator;
        return map<OutStrT, iterator>(stl::begin(src_view), stl::end(src_view), out);
    }

    enum struct to_ascii_status : stl::uint32_t {
        valid              = 0,
        invalid_code_point = 1,
        empty_domain_label = 2,
    };

    struct idna_options { // NOLINT(*-struct-pack-align)
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
    };

    /**
     * Is Domain Label Valid.
     * Valid Criteria: https://www.unicode.org/reports/tr46/#Validity_Criteria
     *
     * Preconditions:
     *  - The label must be in Unicode Normalization Form C.
     *  - The label must not contain a U+002E (.) FULL STOP.
     *  - Each code point in the label must only have certain Status values according to
     *    Section 5, IDNA Mapping Table
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

        // 5. Check if includes any dots (SKIPPED by default)
        if constexpr (Options.CheckDotInclusions) {
            // we don't need to check for UTF encodings, nor we need early bailout since that would mean we'd
            // be optimizing for the failure path as opposed to optimizing for the happy path
            for (auto pos = spos; pos != send; ++pos) {
                valid &= *pos != '.';
            }
        }

        // 8. Check joiners
        if constexpr (Options.CheckJoiners) {
            // todo
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
     *  - from RFC 3490
     *
     *         RFC: https://www.rfc-editor.org/rfc/rfc3490.html#section-4.1
     *     UTS #46: https://www.unicode.org/reports/tr46/#ToASCII
     *  Steps From: https://www.unicode.org/reports/tr46/#Processing
     */
    template <idna_options Options = {}, istl::Appendable StrT = stl::string, typename Iter>
    static constexpr to_ascii_status to_ascii(Iter spos, Iter send, StrT& out)
      noexcept(istl::NothrowAppendable<StrT>) {
        using enum to_ascii_status;
        using unicode::normalization_form;

        // 1. Processing
        // https://www.unicode.org/reports/tr46/#Processing

        // 1.1 Map
        if (!idna::map(spos, send, out)) {
            // todo: is this error code the correct error?
            return invalid_code_point;
        }

        // 1.2. Normalize
        normalize<normalization_form::NFC>(out);

        // 1.3. Break: Break the string into labels at U+002E (.) FULL STOP
        using flag_type                                         = stl::uint8_t;
        webpp_static_constexpr flag_type dot_flag               = 0b100'0000U;
        webpp_static_constexpr flag_type x_flag                 = 0b1U;
        webpp_static_constexpr flag_type n_flag                 = 0b10U;
        webpp_static_constexpr flag_type dash_flag              = 0b100U;
        webpp_static_constexpr flag_type ascii_flag             = 0b1000U;
        webpp_static_constexpr flag_type xnd_flag               = x_flag | n_flag | dash_flag | ascii_flag;
        webpp_static_constexpr flag_type clean_flag             = static_cast<flag_type>(~dot_flag);
        webpp_static_constexpr auto      interesting_characters = categorize<flag_type, 256U>(
          cat{.set = ".", .value = dot_flag},
          cat{.set = "xX", .value = x_flag},
          cat{.set = "nN", .value = n_flag},
          cat{.set = "-", .value = dash_flag},
          cat{.set = ALL_ASCII<char8_t>, .value = ascii_flag});

        auto status = valid;
        for (; spos != send; ++spos) {
            auto const lpos = spos; // start of label

            // find the label:
            flag_type const flag = or_all_if<flag_type>(
              interesting_characters,
              spos,
              send,
              [](flag_type const res) constexpr noexcept -> bool {
                  return res >= dot_flag; // we found a dot
              });

            switch (flag & clean_flag) {
                [[unlikely]] case 0:
                    // If the label is empty, or ..., record that there was an error.
                    return empty_domain_label;
                case xnd_flag:
                    if (
                      lpos - lpos > 4 && lpos[0] == 'x' && lpos[1] == 'n' && lpos[2] == '-' && lpos[3] == '-')
                    {
                        // found xn--
                        // If the label contains any non-ASCII code point (i.e., a code point greater than
                        // U+007F), record that there was an error, and continue with the next label.
                        if ((flag & ascii_flag) != ascii_flag) [[unlikely]] {
                            status = invalid_code_point;
                            continue;
                        }
                    }
                    [[fallthrough]];
                [[likely]] default:
                    break;
            }

            if ((flag & dot_flag) == dot_flag) { // every label except the last label
                istl::iter_append(out, '.');
            }
        }
        if (status != valid) [[unlikely]] {
            return status;
        }

        // 1.4. Convert/Validate

        // 2. Punycode
        // Convert each label with non-ASCII characters into Punycode [RFC3492], and prefix by “xn--”.
        // This may record an error.


        // 4. VerifyDnsLength (not implemented)
        // todo: implement it


        // Implementations are advised to apply additional tests to these labels, such as those described in
        // Unicode Technical Report #36, Unicode Security Considerations [UTR36] and Unicode Technical
        // Standard #39, Unicode Security Mechanisms [UTS39], and take appropriate actions. For example, a
        // label with mixed scripts or confusables may be called out in the UI. Note that the use of Punycode
        // to signal problems may be counter-productive, as described in [UTR36].

        return valid;
    }


} // namespace webpp::unicode::idna

#endif // WEBPP_URI_IDNA_MAPPINGS_HPP
