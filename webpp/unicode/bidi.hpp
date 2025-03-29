#ifndef WEBPP_UNICODE_BIDI_HPP
#define WEBPP_UNICODE_BIDI_HPP

#include "./details/bidi_tables.hpp"
#include "./unicode.hpp"
#include "details/idna_mapping_tables.hpp"

#include <climits>

namespace webpp::unicode {

    using direction_type = stl::uint8_t;

    /// Note: if you ever needed to change these values, you need to change it in the bidi.mjs file as well
    ///       and regenerate the bidi tables.
    /// https://www.unicode.org/reports/tr9/#Table_Bidirectional_Character_Types
    /// https://www.rfc-editor.org/rfc/rfc5893.html#section-1.4
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
        using stl::to_underlying;
        return ((0b1U << to_underlying(dirs)) | ... | 0b0U);
    }

    [[nodiscard]] constexpr stl::string_view to_string(direction const dir) noexcept {
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
    template <UTF32 CPType>
    [[nodiscard]] static constexpr direction direction_of(CPType const code_point) noexcept {
        using enum direction;
        using details::bidi_index;
        using details::bidi_indices;
        using details::bidi_values;
        using details::common_position;

        // NOLINTBEGIN(*-pro-bounds-constant-array-index)
        if (code_point >= details::trailing_zero_bidis) [[unlikely]] {
            return NONE;
        }

        auto const chunk                      = code_point >> bidi_index::chunk_shift;
        auto const section_index              = chunk >> details::breakpoint_shift;
        auto const [starting, ending, offset] = details::breakpoints[section_index];
        bidi_index const pos =
          chunk < starting || chunk >= ending ? common_position : bidi_indices[chunk - offset];

        return static_cast<direction>(bidi_values[pos.get_position(code_point)]);
        // NOLINTEND(*-pro-bounds-constant-array-index)
    }

    /**
     * An RTL label is a label that contains at least one character of type R, AL, or AN.
     * From RFC: https://www.rfc-editor.org/rfc/rfc5893#section-1.4
     */
    template <stl::forward_iterator IterT>
    [[nodiscard]] static constexpr bool is_rtl_label(IterT pos, IterT endp) noexcept {
        using enum direction;

        stl::uint32_t directions = 0U;
        for (; pos != endp; ++pos) {
            directions |= 0b1U << direction_of(*pos);
        }
        return (directions & bidi_mask(R, AL, AN)) != 0U;
    }

    /**
     * Check if we're in compliant with the Bidi Rule
     * RFC: https://www.rfc-editor.org/rfc/rfc5893#section-2
     */
    template <stl::random_access_iterator IterT>
    [[nodiscard]] static constexpr bool validate_bidi_rule(IterT const beg, IterT const endp) noexcept {
        using stl::to_underlying;
        using enum direction;
        using enum checked::error_handling;

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

        auto          pos      = beg;
        char32_t      last_cp  = 0;
        auto const    first_cp = checked::next_code_point<return_zero_char>(pos, endp);
        auto const    first    = to_underlying(direction_of(first_cp));
        stl::uint32_t accum    = 0b1U << to_underlying(direction_of(first_cp));
        if (first_cp == 0) {
            return true;
        }
        for (;;) {
            last_cp = checked::next_code_point<return_zero_char>(pos, endp);
            if (last_cp == 0) {
                break;
            }
            accum |= 0b1U << to_underlying(direction_of(last_cp));
        }

        // A "Bidi domain name" is a domain name that contains at least one RTL label.
        if ((accum & bidi_mask(R, AL, AN)) == 0U) [[likely]] { // it's all LTR
            return true;
        }

        stl::uint32_t const last  = 0b1U << to_underlying(direction_of(last_cp));
        bool                valid = true;

        // we don't need to check other things, the first rule will make sure it's not valid otherwise
        bool const is_rtl = (first != to_underlying(L));

        // 1. First character must be L, R, or AL:
        valid &= (first & bidi_mask(L, R, AL)) != 0;

        if (!is_rtl) [[likely]] {
            // 5. Checking LTR allowed characters
            valid &= (accum & ~bidi_mask(L, EN, ES, CS, ET, ON, BN, NSM)) == 0;

            // 6. It ends with (semi-regex): (L|EN)NSM*
            if ((last & bidi_mask(L, EN)) != 0) [[unlikely]] {
                // For Example, Every Dhivehi word ends with a combining mark (NSM)
                for (;;) {
                    last_cp = checked::prev_code_point<return_zero_char>(pos, endp);
                    if (last_cp == 0) [[unlikely]] {
                        valid = false;
                        break;
                    }
                    if (direction_of(*pos) != NSM) {
                        break;
                    }
                }
                valid &= (to_underlying(direction_of(*pos)) & bidi_mask(L, EN)) != 0;
            }

        } else {
            // 2. Checking RTL allowed characters
            valid &= (accum & ~bidi_mask(R, AL, AN, EN, ES, CS, ET, ON, BN, NSM)) == 0;

            // 3. It ends with (semi-regex): (R|AL|EN|AN)NSM*
            if ((last & bidi_mask(R, AL, EN, AN)) != 0) {
                // For Example, Every Dhivehi word ends with a combining mark (NSM)
                for (;;) {
                    last_cp = checked::prev_code_point<return_zero_char>(pos, endp);
                    if (last_cp == 0) [[unlikely]] {
                        valid = false;
                        break;
                    }
                    if (direction_of(*pos) != NSM) {
                        break;
                    }
                }
                valid &= (to_underlying(direction_of(*pos)) & bidi_mask(R, AL, EN, AN)) != 0;
            }

            // 4. AN and EN should not be present together
            webpp_static_constexpr auto AN_EN  = bidi_mask(AN, EN);
            valid                             &= (accum & AN_EN) != AN_EN;
        }

        // We don't need early bailouts in this function since the happy path goes through all the checks and
        // still this value would be true, if this value is false, something is wrong, and we don't care about
        // optimizing the sad path.
        return valid;
    }



} // namespace webpp::unicode

#endif // WEBPP_UNICODE_BIDI_HPP
