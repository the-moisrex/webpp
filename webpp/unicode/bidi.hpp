#ifndef WEBPP_UNICODE_BIDI_HPP
#define WEBPP_UNICODE_BIDI_HPP

#include "./details/bidi_tables.hpp"
#include "./details/idna_mapping_tables.hpp"
#include "./unicode.hpp"

#include <climits>

namespace webpp::unicode {

    using direction_type = stl::uint8_t;

    /// Note:
    ///   if you ever needed to change these values, you need to change it in the bidi.mjs file as well
    ///   and regenerate the bidi tables.
    /// You can get some of these values using this command:
    ///   cat PropertyValueAliases.txt | awk 'BEGIN{FS=";"} { if ($1 ~ /^\s*bc\s*$/) print $0 }'
    /// More info:
    ///   https://www.unicode.org/reports/tr9/#Table_Bidirectional_Character_Types
    ///   https://www.rfc-editor.org/rfc/rfc5893.html#section-1.4
    enum struct direction : direction_type {
        NONE = 0,

        // ---------- Strong -----------------------------------------------
        L  = 1U, // Left to right - most letters in LTR scripts
        R  = 2U, // Right to left - most letters in non-Arabic RTL scripts
        AL = 3U, // Arabic letters - most letters in the Arabic script

        // ----------  Weak  -----------------------------------------------
        EN  = 4U,  // European Number (0-9, and Extended Arabic-Indic numbers)
        ES  = 5U,  // European Number Separator (+ and -)
        ET  = 6U,  // European Number Terminator (currency symbols, the hash sign, the percent sign and so on)
        AN  = 7U,  // Arabic Number; this encompasses Arabic-Indics, but not Extended Arabic-Indics
        CS  = 8U,  // Common Number Separator (. , / : et al)
        NSM = 9U,  // NonSpacing Mark - most combining accents
        BN  = 10U, // Boundary Neutral - control characters (ZWNJ, ZWJ, and others)

        // ---------- Neutral -----------------------------------------------
        B  = 11U, // Paragraph Separator
        S  = 12U, // Segment Separator
        WS = 13U, // Whitespace, including the SPACE character
        ON = 14U, // Other Neutrals, including @, &, parentheses, MIDDLE DOT

        // ---------- Explicit Formatting -----------------------------------
        // these are "directional control characters" and are not used in IDNA labels.
        LRE = 15U, // Left-to-Right Embedding
        LRO = 16U, // Left-to-Right Override
        RLE = 17U, // Right-to-Left Embedding
        RLO = 18U, // Right-to-Left Override
        PDF = 19U, // Pop Directional Format
        LRI = 20U, // Left-to-Right Isolate
        RLI = 21U, // Right-to-Left Isolate
        FSI = 22U, // First Strong Isolate
        PDI = 23U, // Pop Directional Isolate
    };

    template <typename... T>
    [[nodiscard]] consteval stl::uint32_t bidi_mask(T const... dirs) noexcept {
        return ((0b1U << stl::to_underlying(dirs)) | ... | 0b0U);
    }

    [[nodiscard]] static constexpr stl::string_view to_string(direction const dir) noexcept {
        using enum direction;
        switch (dir) {
            case NONE: return "None";

            // ---------- Strong -----------------------------------------------
            case L: return "Left To Right"; // most letters in LTR scripts
            case R: return "Right To left"; // most letters in non-Arabic RTL scripts
            case AL:
                return "Arabic Letters";    // most letters in the Arabic script

            // ----------  Weak  -----------------------------------------------
            case EN: return "European Number";            // (0-9, and Extended Arabic-Indic numbers)
            case ES: return "European number Separator";  // (+ and -)
            case ET: return "European number Terminator"; // (currency symbols, the hash sign, ...)
            case AN: return "Arabic Number";
            case CS: return "Common number Separator";    // Common Number Separator (. , / : et al)
            case NSM: return "NonSpacing Mark";           // most combining accents
            case BN:
                return "Boundary Neutral";                // control characters (ZWNJ, ZWJ, ...)

            // ---------- Neutral -----------------------------------------------
            case B: return "Paragraph Separator"; // Paragraph Separator
            case S: return "Segment Separator";   // Segment Separator
            case WS: return "WhiteSpace";         // including the SPACE character
            case ON:
                return "Other Neutrals";          // including @, &, parentheses, MIDDLE DOT

            // ---------- Explicit Formatting -----------------------------------
            // these are "directional control characters" and are not used in IDNA labels.
            case LRE: return "Left-to-Right Embedding";
            case LRO: return "Left-to-Right Override";
            case RLE: return "Right-to-Left Embedding";
            case RLO: return "Right-to-Left Override";
            case PDF: return "Pop Directional Format";
            case LRI: return "Left-to-Right Isolate";
            case RLI: return "Right-to-Left Isolate";
            case FSI: return "First Strong Isolate";
            case PDI: return "Pop Directional Isolate";
            default: break;
        }
        return "<unknown direction>";
    }

    [[nodiscard]] constexpr stl::string_view to_abbr(direction const dir) noexcept {
        using enum direction;
        switch (dir) {
            case NONE: return "None";

            case L: return "L";
            case R: return "R";
            case AL: return "AL";

            case EN: return "EN";
            case ES: return "ES";
            case ET: return "ET";
            case AN: return "AN";
            case CS: return "CS";
            case NSM: return "NSM";
            case BN: return "BN";

            case B: return "B";
            case S: return "S";
            case WS: return "WS";
            case ON: return "ON";

            case LRE: return "LRE";
            case LRO: return "LRO";
            case RLE: return "RLE";
            case RLO: return "RLO";
            case PDF: return "PDF";
            case LRI: return "LRI";
            case RLI: return "RLI";
            case FSI: return "FSI";
            case PDI: return "PDI";
            default: break;
        }
        return "<unknown dir>";
    }

    /// Get the direction of the specified code point
    [[nodiscard]] static constexpr direction direction_of(char32_t const code_point) noexcept {
        using enum direction;
        using details::bidi_common_pos;
        using details::bidi_index;
        using details::bidi_indices;
        using details::bidi_values;

        // NOLINTBEGIN(*-pro-bounds-constant-array-index)
        if (static_cast<stl::int32_t>(code_point) < 0 || code_point >= details::trailing_zero_bidis) [[unlikely]] {
            return NONE;
        }

        auto const chunk                      = code_point >> bidi_index::chunk_shift;
        auto const section_index              = static_cast<stl::uint16_t>(chunk >> details::bidi_breakpoint_shift);
        auto const [starting, ending, offset] = details::bidi_breakpoints[section_index];
        bidi_index const pos =
          chunk < starting || chunk >= ending
            ? bidi_common_pos
            : bidi_indices[static_cast<stl::uint16_t>(chunk - offset)];

        return static_cast<direction>(bidi_values[pos.get_position(code_point)]);
        // NOLINTEND(*-pro-bounds-constant-array-index)
    }

    [[nodiscard]] static constexpr stl::uint32_t direction_mask_of(direction const dir) noexcept {
        return 0b1U << stl::to_underlying(dir);
    }

    [[nodiscard]] static constexpr stl::uint32_t direction_mask_of(char32_t const code_point) noexcept {
        return direction_mask_of(direction_of(code_point));
    }

    struct bidi_info {
        // These are the result of direction_mask_of(...) function:
        stl::uint32_t accum        = 0;
        stl::uint32_t first        = 0;
        stl::uint32_t last_non_nsm = 0;
    };

    namespace details {
        /// Run this while you're looping through a range to fill the bidi_info
        static constexpr void bidi_info_step(bidi_info& info, char32_t code_point) noexcept {
            using enum direction;

            auto const last  = direction_mask_of(code_point);
            info.accum      |= last;
            if (last != direction_mask_of(NSM)) {
                info.last_non_nsm = last;
            }
        }

        static constexpr void bidi_info_first(bidi_info& info, char32_t code_point) noexcept {
            using enum direction;

            auto const first = direction_mask_of(code_point);
            info.first       = first;
            info.accum       = first;
            if (first != direction_mask_of(NSM)) {
                info.last_non_nsm = first;
            }
        }
    } // namespace details

    /**
     * Generate a bidi_info which is required to check if the specified range is compliant with the Bidi Rules.
     */
    template <stl::random_access_iterator IterT>
    [[nodiscard]] static constexpr bidi_info get_bidi_info(IterT const& beg, IterT const& endp) noexcept {
        using enum direction;
        using enum err_policy;
        using char_type = stl::iter_value_t<IterT>;

        // Handle empty strings
        if (beg == endp) [[unlikely]] {
            return bidi_info{};
        }

        auto       pos      = beg;
        auto const first_cp = checked::next_code_point<return_replacement_char>(pos, endp);
        char32_t   last_cp  = first_cp;
        bidi_info  info{
           .first = direction_mask_of(first_cp),
        };
        info.accum = info.first;

        if constexpr (UTF32<char_type>) {
            if (pos != endp) {
                for (;; ++pos) {
                    info.accum |= direction_mask_of(*pos);
                    if (pos == endp) {
                        break;
                    }
                }
                last_cp = *--pos;
            }
        } else {
            while (pos != endp) {
                last_cp     = checked::next_code_point<return_replacement_char>(pos, endp);
                info.accum |= direction_mask_of(last_cp);
            }
        }


        info.last_non_nsm = direction_mask_of(last_cp);
        while (info.last_non_nsm == direction_mask_of(NSM) && pos != beg) {
            info.last_non_nsm = direction_mask_of(checked::prev_code_point<return_replacement_char>(pos, beg));
        }

        return info;
    }

    /**
     * Check if we're in compliant with the Bidi Rule
     * RFC: https://www.rfc-editor.org/rfc/rfc5893#section-2
     */
    [[nodiscard]] static constexpr bool validate_bidi_rule(bidi_info const& info) noexcept {
        using enum direction;

        // The following rule, consisting of six conditions, applies to labels in Bidi domain names.
        // All the conditions must be satisfied for the rule to be satisfied.
        //
        // 1.  The first character must be a character with Bidi property L, R,
        //     or AL.  If it has the R or AL property, it is an RTL label; if it
        //     has the L property, it is an LTR label.
        //
        // 2.  In an RTL label, only characters with the Bidi properties R, AL,
        //     AN, EN, ES, CS, ET, ON, BN, or NSM are allowed.
        //
        // 3.  In an RTL label, the end of the label must be a character with
        //     Bidi property R, AL, EN, or AN, followed by zero or more
        //     characters with Bidi property NSM.
        //
        // 4.  In an RTL label, if an EN is present, no AN may be present, and
        //     vice versa.
        //
        // 5.  In an LTR label, only characters with the Bidi properties L, EN,
        //     ES, CS, ET, ON, BN, or NSM are allowed.
        //
        // 6.  In an LTR label, the end of the label must be a character with
        //     Bidi property L or EN, followed by zero or more characters with
        //     Bidi property NSM.

        // Handle empty strings:
        if (info.accum == 0) [[unlikely]] {
            return true;
        }

        // We don't need to check other things, the first rule will make sure it's not valid otherwise
        bool const is_rtl = info.first != direction_mask_of(L);

        // 1. The first character must be L, R, or AL:
        bool valid = (info.first & bidi_mask(L, R, AL)) != 0;

        if (!is_rtl) {
            // 5. Checking LTR allowed characters
            valid &= (info.accum & ~bidi_mask(L, EN, ES, CS, ET, ON, BN, NSM)) == 0;

            // 6. It ends with (semi-regex): (L|EN)NSM*
            valid &= (info.last_non_nsm & bidi_mask(L, EN)) != 0;

        } else {
            // 2. Checking RTL allowed characters
            valid &= (info.accum & ~bidi_mask(R, AL, AN, EN, ES, CS, ET, ON, BN, NSM)) == 0;

            // 3. It ends with (semi-regex): (R|AL|EN|AN)NSM*
            // For Example, Every Dhivehi word ends with a combining mark (NSM)
            valid &= (info.last_non_nsm & bidi_mask(R, AL, EN, AN)) != 0;

            // 4. AN and EN should not be present together
            valid &= (info.accum & bidi_mask(AN, EN)) != bidi_mask(AN, EN);
        }

        // We don't need early bailouts in this function since the happy path goes through all the checks and
        // still this value would be true, if this value is false, something is wrong, and we don't care about
        // optimizing the sad path.
        return valid;
    }

    /**
     * A "Bidi domain name" is a domain name that contains at least one RTL label.
     * A Bidi domain name is a domain name containing at least one character with Bidi_Class R, AL, or AN.
     * See [IDNA2008] RFC 5893, Section 1.4.
     */
    [[nodiscard]] static constexpr bool is_bidi_domain_name(bidi_info const& info) noexcept {
        using enum direction;
        return (info.accum & bidi_mask(R, AL, AN)) != 0U;
    }

    /**
     * Check if we're in compliant with the Bidi Rule
     * RFC: https://www.rfc-editor.org/rfc/rfc5893#section-2
     */
    template <stl::random_access_iterator IterT>
    [[nodiscard]] static constexpr bool validate_bidi_rule(IterT const beg, IterT const endp) noexcept {
        return validate_bidi_rule(get_bidi_info(beg, endp));
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_BIDI_HPP
