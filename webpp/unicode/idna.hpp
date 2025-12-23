// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "./checked.hpp"
#include "./details/idna_mapping_tables.hpp"

namespace webpp::unicode::idna {

    struct idna_options { // NOLINT(*-struct-pack-align)

        // ===================================================================
        // Options following the WHATWG URL Standard (Domain to ASCII)
        // https://url.spec.whatwg.org/#concept-domain-to-ascii
        // ===================================================================

        // Controls strictness level (beStrict flag in the spec).
        // false for special schemes (http, https, ws, wss, ftp, file)
        // true  for all others (e.g., when registering domains)
        // Default: false — optimized for typical web URL parsing use case
        bool CheckHyphens = false;
        // Disallows:
        // - Hyphens in 3rd and 4th positions (e.g., "xn--")
        // - Leading, trailing, or consecutive hyphens in labels

        bool UseSTD3ASCIIRules = false;
        // When true: only allows ASCII letters (a–z), digits (0–9), and hyphen
        // When false: more permissive (typical for browsers)

        bool VerifyDnsLength = false;
        // Enforces:
        // - Total domain length: 1..253
        // - Each label length: 1..63
        // Usually disabled in browsers, enabled in registrars

        // ===================================================================
        // Always-enabled checks (not affected by beStrict)
        // ===================================================================

        bool CheckACE = true;
        // Ensures proper ACE prefix ("xn--") handling and forbids it in invalid positions

        bool CheckBidi = true;
        // Enforces bidirectional (RTL/LTR) domain label rules per RFC 5893

        bool CheckJoiners = true;
        // Disallows zero-width joiners/non-joiners and other ContextJ code points
        // that could be used for spoofing

        bool CheckInvalidPunycode = true;
        // If true, would accept malformed Punycode (not allowed by spec)
        // Always false — spec requires failure on invalid Punycode

        bool CheckNFC = true;
        // Enforces Unicode NFC normalization and rejects non-NFC input
        // Required by UTS #46 and URL standard

        // ===================================================================
        // Additional UTS #46 validity checks (non-transitional mode)
        // ===================================================================

        bool CheckDotInclusions = true;
        // Prevents confusing dot-like characters (e.g., fullwidth period)
        // that could masquerade as label separators

        bool CheckMappingRequired = true;
        // UTS #46 Validity Criterion #7:
        // Characters marked "disallowed_STD3_mapped" or "deviation" must be mapped
        // (e.g., ß → ss, ẞ → SS) — otherwise invalid in non-transitional mode

        bool CheckCombiningMarkAtLabelStart = true;
        // UTS #46 rule: a label must not begin with a combining mark

        bool CheckDecodeAndValidateLabels = true;
        // For A-labels (xn--...), decode Punycode and validate resulting U-label
        // Required for security (prevents encoded invalid/invisible domains)

        // Transitional_Processing is intentionally not supported
        // (Deprecated in UTS #46 since 2016; removed from modern IDNA)
        // bool Transitional_Processing = false;
    };

    static constexpr idna_options strict_idna_options{
      .CheckHyphens      = true,
      .UseSTD3ASCIIRules = true,
      .VerifyDnsLength   = true,

      .CheckACE             = true,
      .CheckBidi            = true,
      .CheckJoiners         = true,
      .CheckInvalidPunycode = true,
      .CheckNFC             = true,

      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
      .CheckDecodeAndValidateLabels   = true,
    };

    /// Relaxed options
    /// Warning: Using these set of options will open you to security vulnerabilities
    static constexpr idna_options loose_idna_options{
      .CheckHyphens      = false,
      .UseSTD3ASCIIRules = false,
      .VerifyDnsLength   = false,

      .CheckACE             = false,
      .CheckBidi            = false, // Browsers typically disable full Bidi checks for compatibility
      .CheckJoiners         = false, // Often relaxed in practice
      .CheckInvalidPunycode = false, // Some parsers are more forgiving
      .CheckNFC             = false, // Many browsers skip NFC check on input

      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false, // Warning: this is not secure
    };

    [[nodiscard]] static constexpr idna_options idna_flags(stl::uint16_t const flags) noexcept {
        static constexpr auto to_bool = [](std::uint16_t const value) constexpr noexcept -> bool {
            return static_cast<bool>(value & 0b1U);
        };
        return idna_options{
          .CheckHyphens      = to_bool(flags >> 11U),
          .UseSTD3ASCIIRules = to_bool(flags >> 10U),
          .VerifyDnsLength   = to_bool(flags >> 9U),

          .CheckACE             = to_bool(flags >> 8U),
          .CheckBidi            = to_bool(flags >> 7U),
          .CheckJoiners         = to_bool(flags >> 6U),
          .CheckInvalidPunycode = to_bool(flags >> 5U),
          .CheckNFC             = to_bool(flags >> 4U),

          .CheckDotInclusions             = to_bool(flags >> 3U),
          .CheckMappingRequired           = to_bool(flags >> 2U),
          .CheckCombiningMarkAtLabelStart = to_bool(flags >> 1U),
          .CheckDecodeAndValidateLabels   = to_bool(flags >> 0U),
        };
    }

    [[nodiscard]] static constexpr stl::uint16_t idna_flags(idna_options const options) noexcept {
        static constexpr auto to_option = [](bool const value) constexpr noexcept -> std::uint32_t {
            return value ? 0b1U : 0b0U;
        };
        return static_cast<stl::uint16_t>(
          to_option(options.CheckHyphens) << 11U |                  //
          to_option(options.UseSTD3ASCIIRules) << 10U |             //
          to_option(options.VerifyDnsLength) << 9U |                //

          to_option(options.CheckACE) << 8U |                       //
          to_option(options.CheckBidi) << 7U |                      //
          to_option(options.CheckJoiners) << 6U |                   //
          to_option(options.CheckInvalidPunycode) << 5U |           //
          to_option(options.CheckNFC) << 4U |                       //

          to_option(options.CheckDotInclusions) << 3U |             //
          to_option(options.CheckMappingRequired) << 2U |           //
          to_option(options.CheckCombiningMarkAtLabelStart) << 1U | //
          to_option(options.CheckDecodeAndValidateLabels) << 0U);
    }

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

        // NOLINTBEGIN(*-array-index, *-avoid-nested-conditional-operator, *-unchecked-container-access)
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
        // NOLINTEND(*-array-index, *-avoid-nested-conditional-operator, *-unchecked-container-access)
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

            default: {
                // Mapped or ignored:
                auto const* ptr = std::next(idna_mappings.begin(), pos);
                if constexpr (UTF8String<OutStrT>) {
                    for (; *ptr != u8'\0'; ++ptr) { // NOLINT(*-arithmetic)
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
        using enum err_policy;
        auto const* beg        = stl::begin(src);
        auto const* end        = stl::end(src);
        auto const  code_point = checked::next_code_point<return_negated>(beg, end);

        // ignored code points are mapped to nothing, so no special code is needed
        switch (auto const pos = status_of(code_point)) {
            case disallowed: return end;
            case valid: return beg;
            default: return stl::next(details::idna_mappings.begin(), pos); // mapped
        }
    }

    /**
     * Check if a string requires any type of IDNA mapping.
     */
    template <stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool requires_idna_mapping(Iter pos, Iter const end) noexcept {
        using enum err_policy;
        using checked::next_code_point;
        using details::valid;
        while (pos != end) {
            auto const code_point = next_code_point<return_negated>(pos, end);
            auto const map_pos    = status_of(code_point);
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
        using enum err_policy;
        using inp_char_type = stl::iter_value_t<Iter>;
        using out_char_type = istl::appendable_value_type_t<OutStrT>;

        if constexpr (stl::same_as<Iter, OutStrT>) {
            // no inplace mapping
            assert(!(out >= beg && out < end));
        }

        auto is_valid = true;
        for (Iter pos = beg; pos != end;) {
            Iter const cp_beg     = pos;
            auto const code_point = next_code_point<return_negated>(pos, end);
            auto const map_pos    = status_of(code_point);

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

} // namespace webpp::unicode::idna

#endif // WEBPP_URI_IDNA_MAPPINGS_HPP
