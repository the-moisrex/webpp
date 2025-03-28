#ifndef WEBPP_UNICODE_BIDI_HPP
#define WEBPP_UNICODE_BIDI_HPP

#include "./details/bidi_tables.hpp"
#include "./unicode.hpp"

namespace webpp::unicode {

    /// https://www.unicode.org/reports/tr9/#Table_Bidirectional_Character_Types
    /// https://www.rfc-editor.org/rfc/rfc5893.html#section-1.4
    enum struct direction : stl::uint8_t {
        NONE = 0,

        // ---------- Strong -----------------------------------------------
        L,  // Left to right - most letters in LTR scripts
        R,  // Right to left - most letters in non-Arabic RTL scripts
        AL, // Arabic letters - most letters in the Arabic script

        // ----------  Weak  -----------------------------------------------
        EN,  // European Number (0-9, and Extended Arabic-Indic numbers)
        ES,  // European Number Separator (+ and -)
        ET,  // European Number Terminator (currency symbols, the hash sign, the percent sign and so on)
        AN,  // Arabic Number; this encompasses the Arabic-Indic numbers, but not the Extended Arabic-Indics
        CS,  // Common Number Separator (. , / : et al)
        NSM, // Nonspacing Mark - most combining accents
        BN,  // Boundary Neutral - control characters (ZWNJ, ZWJ, and others)

        // ---------- Neutral -----------------------------------------------
        B,  // Paragraph Separator
        S,  // Segment Separator
        WS, // Whitespace, including the SPACE character
        ON, // Other Neutrals, including @, &, parentheses, MIDDLE DOT

        // ---------- Explicit Formatting -----------------------------------
        // these are "directional control characters" and are not used in IDNA labels.
        LRE, // Left-to-Right Embedding
        LRO, // Left-to-Right Override
        RLE, // Right-to-Left Embedding
        RLO, // Right-to-Left Override
        PDF, // Pop Directional Format
        LRI, // Left-to-Right Isolate
        RLI, // Right-to-Left Isolate
        FSI, // First Strong Isolate
        PDI, // Pop Directional Isolate
    };

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

        if (code_point >= details::trailing_zero_bidis) [[unlikely]] {
            return NONE;
        }

        auto const chunk                      = code_point >> bidi_index::chunk_shift;
        auto const section_index              = chunk >> details::breakpoint_shift;
        auto const [starting, ending, offset] = details::breakpoints[section_index];
        bidi_index const pos =
          chunk < starting || chunk >= ending ? common_position : bidi_indices[chunk - offset];

        return static_cast<direction>(bidi_values[pos.get_position(code_point)]);
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_BIDI_HPP
