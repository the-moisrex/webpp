// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "./details/idna_mapping_tables.hpp"
#include "./normalization.hpp"
#include "./unicode.hpp"

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
            ref = details::idna_refs[code_point >> batch_bit_count];
        } else [[unlikely]] {
            if (code_point >= static_cast<CharT>(details::last_disallowed)) {
                return disallowed;
            }
            if (code_point >= static_cast<CharT>(details::breakpoint_end)) {
                auto const pos =
                  (code_point - static_cast<CharT>(details::breakpoint_end)) >> batch_bit_count;
                ref = details::idna_refs_extra[pos];
            } else {
                return disallowed;
            }
        }

        stl::uint16_t const clean_ref = ref & static_cast<stl::uint16_t>(~details::table_pick_mask);
        auto const          ref_ptr   = clean_ref + (code_point & batch_mask);
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
    template <UTF32 CharT = char32_t, istl::String OutStrT = stl::u8string>
    static constexpr bool map(CharT const code_point, OutStrT& out) {
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
    template <istl::String OutStrT, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool map(Iter beg, Iter end, OutStrT& out) {
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

    template <istl::String OutStrT, istl::StringViewifiable InpStrT>
    [[nodiscard]] static constexpr bool map(InpStrT&& src, OutStrT& out) {
        auto const src_view = istl::string_viewify(stl::forward<InpStrT>(src));
        using iterator      = typename decltype(src_view)::iterator;
        return map<OutStrT, iterator>(stl::begin(src_view), stl::end(src_view), out);
    }


    enum struct to_ascii_status : stl::uint32_t {
        valid              = 0,
        invalid_code_point = 1,
    };

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
     *
     * We do not implement the whole thing yet, these are the parameters that URL parsing requires:
     *  - CheckHyphens set to false,
     *  - CheckBidi set to true,
     *  - CheckJoiners set to true,
     *  - UseSTD3ASCIIRules set to false,
     *  - Transitional_Processing set to false,
     *  - VerifyDnsLength set to false,
     *  - IgnoreInvalidPunycode set to false.
     */
    template <istl::String StrT = stl::string, typename Iter>
    static constexpr to_ascii_status to_ascii(Iter spos, Iter send, StrT& out) {
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
