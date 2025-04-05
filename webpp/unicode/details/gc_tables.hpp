
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_general_categories_tables.mjs
 *   Unicode UCD Database Creation Date: 2024-08-25
 *   This file's generation date:        Sat, 05 Apr 2025 14:50:53 GMT
 *   Unicode Version:                    16.0.0
 *   Total Table sizes in this file:
 *       - in bits:       60648
 *       - in bytes:      7581 B
 *       - in KibiBytes:  7.40 KiB
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 *   UCD Database Code Points (used the get the GC values and what not):
 *       https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt
 *   UCD README file (used to check the version and creation date):
 *       https://www.unicode.org/Public/UCD/latest/ucd/ReadMe.txt
 *   Known Properties' Values are taken from:
 *       https://www.unicode.org/Public/UCD/latest/ucd/PropertyValueAliases.txt
 *
 *
 * Attention:
 *   These General Categories have been excluded from this file:
 *    Control, Format, Private_Use, Surrogate, Letter, Cased_Letter, Lowercase_Letter, Modifier_Letter,
 * Other_Letter, Titlecase_Letter, Uppercase_Letter, Number, Decimal_Number, Letter_Number, Other_Number,
 * Punctuation, Connector_Punctuation, Dash_Punctuation, Close_Punctuation, Final_Punctuation,
 * Initial_Punctuation, Other_Punctuation, Open_Punctuation, Symbol, Currency_Symbol, Modifier_Symbol,
 * Math_Symbol, Other_Symbol, Separator, Line_Separator, Paragraph_Separator, Space_Separator
 *
 */

#ifndef WEBPP_UNICODE_GC_TABLES_HPP
#define WEBPP_UNICODE_GC_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode {

    /// Giving aliases to known values:
    /// Can be re-generating using:
    /// curl --silent https://www.unicode.org/Public/UCD/latest/ucd/PropertyValueAliases.txt | grep -oE
    /// '[^#]+' | awk 'BEGIN{FS=";"; i=0; print "enum struct general_category : std::uint8_t {"; } {gsub(/ */,
    /// "", $1); gsub(/ */, "", $3); gsub(/ */, "", $2); if ($1 == "gc") {print "  " $3 " = " i ",\n  " $2 " =
    /// " i ",\n"; i=i+1; }} END { print "};"; }'

    enum struct general_category : std::uint8_t {
        Other                 = 0U,
        C                     = 0U,
        Control               = 1U,
        Cc                    = 1U,
        Format                = 2U,
        Cf                    = 2U,
        Unassigned            = 3U,
        Cn                    = 3U,
        Private_Use           = 4U,
        Co                    = 4U,
        Surrogate             = 5U,
        Cs                    = 5U,
        Letter                = 6U,
        L                     = 6U,
        Cased_Letter          = 7U,
        LC                    = 7U,
        Lowercase_Letter      = 8U,
        Ll                    = 8U,
        Modifier_Letter       = 9U,
        Lm                    = 9U,
        Other_Letter          = 10U,
        Lo                    = 10U,
        Titlecase_Letter      = 11U,
        Lt                    = 11U,
        Uppercase_Letter      = 12U,
        Lu                    = 12U,
        Mark                  = 13U,
        M                     = 13U,
        Spacing_Mark          = 14U,
        Mc                    = 14U,
        Enclosing_Mark        = 15U,
        Me                    = 15U,
        Nonspacing_Mark       = 16U,
        Mn                    = 16U,
        Number                = 17U,
        N                     = 17U,
        Decimal_Number        = 18U,
        Nd                    = 18U,
        Letter_Number         = 19U,
        Nl                    = 19U,
        Other_Number          = 20U,
        No                    = 20U,
        Punctuation           = 21U,
        P                     = 21U,
        Connector_Punctuation = 22U,
        Pc                    = 22U,
        Dash_Punctuation      = 23U,
        Pd                    = 23U,
        Close_Punctuation     = 24U,
        Pe                    = 24U,
        Final_Punctuation     = 25U,
        Pf                    = 25U,
        Initial_Punctuation   = 26U,
        Pi                    = 26U,
        Other_Punctuation     = 27U,
        Po                    = 27U,
        Open_Punctuation      = 28U,
        Ps                    = 28U,
        Symbol                = 29U,
        S                     = 29U,
        Currency_Symbol       = 30U,
        Sc                    = 30U,
        Modifier_Symbol       = 31U,
        Sk                    = 31U,
        Math_Symbol           = 32U,
        Sm                    = 32U,
        Other_Symbol          = 33U,
        So                    = 33U,
        Separator             = 34U,
        Z                     = 34U,
        Line_Separator        = 35U,
        Zl                    = 35U,
        Paragraph_Separator   = 36U,
        Zp                    = 36U,
        Space_Separator       = 37U,
        Zs                    = 37U,
    };


} // namespace webpp::unicode

namespace webpp::unicode::details {

    /**
     * In "gc_index" table, any code point bigger than this number will have "zero" as its GC value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_unassigned_gc = 0xE0200UL;

    /**
     * Gc (Index Table)
     * General Category
     */
    struct alignas(std::uint16_t) gc_index {
        /// The shifts required to extract the values out of a std::uint16_t; you can use masks as well:
        static constexpr std::uint8_t pos_shift = 0U;

        /// The masks required to extracting the values out of a std::uint16_t; you can use shifts as well:
        static constexpr std::uint16_t pos_mask = 0xFFFFU;

        // NOLINTBEGIN(*-non-private-member-variables-in-classes)

        /// This is the position that should be looked for in the values table.
        std::uint16_t pos = 0;

        // NOLINTEND(*-non-private-member-variables-in-classes)

        /**
         * [16bits = pos]
         */
        explicit(false) consteval gc_index(std::uint16_t const value) noexcept
          : pos{static_cast<std::uint16_t>(value)} {}

        [[nodiscard]] constexpr std::uint16_t value() const noexcept {
            return static_cast<std::uint16_t>(pos);
        }

        static constexpr std::uint16_t chunk_mask  = 0x1FU;
        static constexpr std::size_t   chunk_size  = 32U;
        static constexpr std::uint8_t  chunk_shift = 5U;

        /**
         * Get the final position of the second table.
         * This does not apply the shift or get the value of the second table for you; this only applies tha
         * mask.
         */
        [[nodiscard]] constexpr std::uint16_t get_position(auto const request_position) const noexcept {
            auto const remaining_pos = static_cast<std::uint16_t>(request_position & chunk_mask);
            return pos + remaining_pos;
        }
    };

    struct alignas(std::uint64_t) gc_breakpoint_type {
        std::uint16_t starting;
        std::uint16_t ending;
        std::uint16_t offset;
    };

    /**
     * You can choose between the indices' table using these breakpoints:
     *
     * Table size in KibiBytes:  0.88 KiB
     */
    static constexpr std::array<gc_breakpoint_type, 113U> gc_breakpoints{
      gc_breakpoint_type{    .starting = 0,   .ending = 389,     .offset = 0}, // Section 1
      {    .starting = 0,   .ending = 389,     .offset = 0}, // Section 1
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 1330,  .ending = 1376,   .offset = 941}, // Section 2
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 2007,  .ending = 2299,  .offset = 1572}, // Section 3
      { .starting = 2007,  .ending = 2299,  .offset = 1572}, // Section 3
      { .starting = 2465,  .ending = 2467,  .offset = 1738}, // Section 4
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 2823,  .ending = 2944,  .offset = 2094}, // Section 5
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 3555,  .ending = 3915,  .offset = 2705}, // Section 6
      { .starting = 3555,  .ending = 3915,  .offset = 2705}, // Section 6
      { .starting = 3555,  .ending = 3915,  .offset = 2705}, // Section 6
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {.starting = 28679, .ending = 28688, .offset = 27469}, // Section 7
    };

    static constexpr std::uint16_t gc_last_breakpoint{0x7010U};
    static constexpr gc_index      gc_common_position{0U};
    static constexpr std::uint16_t gc_breakpoint_shift{8U};


    /**
     * GC Index Table (combined 7 sections)
     *
     * GC: General Category
     * These are the indices that are used to find which values from "gc_values" table correspond to a Unicode
     * Code Point.
     *
     * Each value contains 1 numbers hidden inside:
     *     [16bits = pos]
     *
     * Table size:
     *   - in bits:       19504
     *   - in bytes:      2438 B
     *   - in KibiBytes:  2.38 KiB
     */
    static constexpr std::array<gc_index, 1219ULL> gc_indices{

      // Section #1 [0, 389) size containing 389 values:
      //   - in bits:       6224
      //   - in bytes:      778 B
      //   - in KibiBytes:  0.76 KiB
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      32,
      32,
      32,
      48,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      77,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      15,
      109,
      139,
      0,
      155,
      0,
      21,
      182,
      0,
      0,
      199,
      223,
      238,
      16,
      270,
      0,
      0,
      165,
      0,
      291,
      321,
      349,
      363,
      0,
      9,
      0,
      22,
      395,
      424,
      430,
      461,
      491,
      522,
      526,
      557,
      587,
      618,
      526,
      650,
      668,
      618,
      526,
      700,
      730,
      522,
      762,
      792,
      491,
      822,
      825,
      857,
      0,
      889,
      894,
      925,
      491,
      522,
      762,
      957,
      987,
      1019,
      1024,
      1055,
      491,
      522,
      0,
      1079,
      1111,
      0,
      1131,
      1158,
      0,
      0,
      1173,
      1202,
      0,
      1217,
      1243,
      0,
      1275,
      1307,
      35,
      818,
      0,
      0,
      1339,
      1370,
      1401,
      1431,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      3,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      1461,
      1483,
      714,
      714,
      0,
      1504,
      1534,
      0,
      1564,
      0,
      0,
      0,
      488,
      189,
      0,
      0,
      0,
      1596,
      0,
      0,
      0,
      0,
      0,
      0,
      1624,
      0,
      1652,
      1684,
      0,
      1716,
      1747,
      0,
      423,
      1762,
      1792,
      1813,
      1845,
      1876,
      0,
      1902,
      0,
      1930,
      0,
      0,
      0,
      0,
      1954,
      1985,
      0,
      0,
      0,
      0,
      0,
      0,
      32,
      32,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2011,
      2042,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2059,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      32,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2081,
      0,
      0,
      2097,

      // Section #2 [1330, 1376) size containing 46 values:
      //   - in bits:       736
      //   - in bytes:      92 B
      //   - in KibiBytes:  0.09 KiB
      0,
      2124,
      2,
      0,
      0,
      716,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2154,
      2183,
      0,
      0,
      2215,
      2227,
      2255,
      2287,
      0,
      2312,
      2337,
      0,
      424,
      2357,
      2388,
      819,
      0,
      2411,
      2440,
      2454,
      0,
      2484,
      823,
      2516,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2545,

      // Section #3 [2007, 2299) size containing 292 values:
      //   - in bits:       4672
      //   - in bytes:      584 B
      //   - in KibiBytes:  0.57 KiB
      0,
      2559,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      48,
      48,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2560,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2589,
      0,
      0,
      0,
      2599,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2630,
      364,
      0,
      0,
      0,
      0,
      0,
      488,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2638,
      0,
      2661,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      721,
      0,
      4,
      0,
      0,
      165,
      0,
      2691,
      0,
      0,
      0,
      2723,
      8,
      2755,
      2771,
      1845,
      2803,
      822,
      0,
      2835,
      2860,
      2210,
      2570,
      1845,
      2881,
      2912,
      0,
      0,
      2932,
      823,
      0,
      0,
      0,
      1,
      2964,
      1019,
      1024,
      2996,
      3026,
      0,
      3047,
      3078,
      3109,
      0,
      3120,
      3152,
      0,
      0,
      3183,
      3214,
      0,
      0,
      0,
      0,
      0,
      0,
      3231,
      3262,
      0,
      0,
      3292,
      2589,
      0,
      0,
      3324,
      0,
      0,
      3348,
      3380,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      3400,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      3427,
      2555,
      0,
      0,
      0,
      3458,
      3490,
      3521,
      3534,
      3565,
      0,
      3593,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      3619,
      0,
      0,
      14,
      3650,
      0,
      0,
      0,
      3673,
      3704,
      0,
      3726,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      3750,
      3782,
      3794,
      3156,

      // Section #4 [2465, 2467) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      0,
      3826,

      // Section #5 [2823, 2944) size containing 121 values:
      //   - in bits:       1936
      //   - in bytes:      242 B
      //   - in KibiBytes:  0.24 KiB
      0,
      2,
      3858,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2654,
      0,
      2739,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      3875,
      3892,
      3916,
      0,
      0,
      3944,

      // Section #6 [3555, 3915) size containing 360 values:
      //   - in bits:       5760
      //   - in bytes:      720 B
      //   - in KibiBytes:  0.70 KiB
      0,
      3962,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      32,
      3994,
      2755,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      4026,
      4055,
      2683,
      0,
      0,
      0,
      0,
      2833,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      32,
      4087,
      32,
      4114,
      194,
      4145,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      4177,
      4207,
      0,
      0,
      183,
      0,
      0,
      0,
      0,
      2739,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      184,
      0,
      2681,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2681,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      718,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      2739,
      0,
      0,
      0,
      1206,

      // Section #7 [28679, 28688) size containing 9 values:
      //   - in bits:       144
      //   - in bytes:      18 B
      //   - in KibiBytes:  0.02 KiB
      0,
      32,
      32,
      32,
      32,
      32,
      32,
      32,
      48,

    };




    /**
     * GC Values Table
     *
     * GC: General Category
     * These are the indices that are used to find which values from "gc_values" table correspond to a Unicode
     * Code Point.
     *
     * Table size:
     *   - in bits:       33912
     *   - in bytes:      4239 B
     *   - in KibiBytes:  4.14 KiB
     */
    static constexpr std::array<std::uint8_t, 4239ULL> gc_values{

      // Start of 0x0-0x2e0, 0x380-0x460, 0x4a0-0x560, 0x5e0, 0x620, 0x680-0x6a0, 0x760-0x780, 0x7c0, 0x860,
      // 0x8a0, 0xbe0, 0xda0, 0xe00, 0xe60-0xe80, 0xee0, 0xf40, 0xfe0-0x1000, 0x10a0-0x1320, 0x1360-0x16e0,
      // 0x1780, 0x17e0, 0x1820-0x1860, 0x18c0-0x1900, 0x1940-0x19e0, 0x1a20, 0x1a80, 0x1ae0, 0x1bc0, 0x1c00,
      // 0x1c40-0x1ca0, 0x1d00-0x1da0, 0x1e00-0x20a0, 0x2100-0x2cc0, 0x2d00-0x2d40, 0x2d80-0x2dc0,
      // 0x2e00-0x3000, 0x3040-0x3060, 0x30a0-0xa640, 0xa6a0-0xa6c0, 0xa700-0xa7e0, 0xa840-0xa860, 0xa900,
      // 0xa960, 0xaa00, 0xaa80, 0xab00-0xabc0, 0xac00-0xfae0, 0xfb20-0xfde0, 0xfe40-0x101c0, 0x10200-0x102c0,
      // 0x10300-0x10340, 0x10380-0x109e0, 0x10a40-0x10ac0, 0x10b00-0x10d00, 0x10d40, 0x10d80-0x10e80,
      // 0x10ec0, 0x10f00-0x10f20, 0x10f60, 0x10fa0-0x10fe0, 0x110e0, 0x111e0-0x11200, 0x11260-0x112a0,
      // 0x11380, 0x11400, 0x11460-0x11480, 0x114e0-0x11580, 0x115e0-0x11600, 0x11660-0x11680,
      // 0x116c0-0x116e0, 0x11740-0x11800, 0x11840-0x11900, 0x11960-0x119a0, 0x11a60, 0x11aa0-0x11c00,
      // 0x11c40-0x11c60, 0x11cc0-0x11d00, 0x11d60, 0x11da0-0x11ec0, 0x11f60-0x13420, 0x13460-0x160e0,
      // 0x16140-0x16ac0, 0x16b00, 0x16b40-0x16f20, 0x16fa0-0x16fc0, 0x17000-0x1bc60, 0x1bca0-0x1cee0,
      // 0x1cf60-0x1d140, 0x1d1c0-0x1d220, 0x1d260-0x1d9e0, 0x1dac0-0x1dfe0, 0x1e040-0x1e060, 0x1e0a0-0x1e100,
      // 0x1e140-0x1e280, 0x1e2c0, 0x1e300-0x1e4c0, 0x1e500-0x1e5c0, 0x1e600-0x1e8a0, 0x1e8e0-0x1e920,
      // 0x1e960-0xe00e0:
      3,

      // Start of 0x2d60, 0x112c0:
      3,

      // Start of 0xa680, 0x16100:
      3,

      // Start of 0x1340:
      3,

      // Start of 0x10ee0:
      3, 3, 3, 3,

      // Start of 0x11020:
      3,

      // Start of 0x880:
      3, 3, 3, 3, 3,

      // Start of 0x11c80:
      3,

      // Start of 0x580:
      3,

      // Start of 0x720:
      3, 3, 3, 3, 3,

      // Start of 0x640:
      3,

      // Start of 0x8c0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x300-0x340, 0x1dc0-0x1de0, 0x2de0, 0x1cf00, 0x1da00, 0x1da40, 0xe0100-0xe01c0:
      16, 16, 16,

      // Start of 0xfa0:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0x360, 0xfe00-0xfe20, 0xe01e0:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x480:
      3, 3, 3, 16, 16, 16, 16, 16, 15, 15, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x5a0:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
      16, 16, 16, 16,

      // Start of 0x5c0:
      3, 16, 16, 3, 16, 16, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x600:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x7a0, 0x10f40:
      3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0x660:
      3,

      // Start of 0x1e080:
      3,

      // Start of 0x1e2a0:
      3, 3, 3, 3, 3,

      // Start of 0x18a0:
      3, 3, 3, 3, 3,

      // Start of 0x1da80:
      3, 3, 3, 3, 16,

      // Start of 0x6c0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16,

      // Start of 0x6e0:
      16, 16, 16, 16, 16, 3, 3, 16, 16, 3, 16, 16, 16, 16, 3,

      // Start of 0x700:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x740:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x7e0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16,

      // Start of 0x800:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 3, 16,

      // Start of 0x820:
      16, 16, 16, 16, 3, 16, 16, 16, 3, 16, 16, 16, 16, 16,

      // Start of 0x840:
      3,

      // Start of 0x10a20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 3, 3, 3, 3,

      // Start of 0x8e0:
      16, 16, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
      16, 16,

      // Start of 0x1b00:
      16,

      // Start of 0x900, 0xa980:
      16, 16, 16, 14, 3, 3,

      // Start of 0x920:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 14, 16, 3, 14,

      // Start of 0x940:
      14, 16, 16, 16, 16, 16, 16, 16, 16, 14, 14, 14, 14, 16, 14, 14, 3, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3,

      // Start of 0x1880, 0x10ae0:
      3, 3, 3,

      // Start of 0x960, 0xb60, 0xc60, 0xd60:
      3, 3, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x980, 0xb00, 0xc80, 0xd80:
      3, 16, 14, 14,

      // Start of 0x9a0, 0xa20, 0xaa0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 14,

      // Start of 0x9c0:
      14, 16, 16, 16, 16, 3, 3, 14, 14, 3, 3, 14, 14, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 3, 3, 3, 3, 3, 3,

      // Start of 0x9e0:
      3, 3, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16,

      // Start of 0xa00, 0xa80:
      3, 16, 16, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xa40:
      14, 16, 16, 3, 3, 3, 3, 16, 16, 3, 3, 16, 16, 16, 3, 3, 3, 16,

      // Start of 0xa60:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xac0:
      14, 16, 16, 16, 16, 16, 3, 16, 16, 14, 3, 14, 14, 16,

      // Start of 0x1740-0x1760:
      3, 3,

      // Start of 0xa6e0:
      3, 3,

      // Start of 0x1e5e0:
      3, 3, 3,

      // Start of 0x10ea0:
      3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xae0:
      3, 3, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16,

      // Start of 0xb20, 0xca0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3,

      // Start of 0xb40:
      14, 16, 16, 16, 16, 3, 3, 14, 14, 3, 3, 14, 14, 16, 3, 3, 3, 3, 3, 3, 3, 16, 16, 14, 3, 3,

      // Start of 0xfc0:
      3,

      // Start of 0xa9e0:
      3, 3, 3,

      // Start of 0xb80, 0x110c0:
      3,

      // Start of 0xaac0, 0x11240:
      3, 16,

      // Start of 0xba0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14,

      // Start of 0xbc0:
      16, 14, 14, 3, 3, 3, 14, 14, 14, 3, 14, 14, 14, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 3, 3, 3, 3, 3, 3, 3,
      3,

      // Start of 0xc00:
      16, 14, 14, 14, 16,

      // Start of 0xc20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 16,

      // Start of 0xc40:
      16, 14, 14, 14, 14, 3, 16, 16, 16, 3, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 16, 16, 3, 3, 3, 3, 3, 3, 3,
      3, 3,

      // Start of 0xcc0:
      14, 14, 14, 14, 14, 3, 16, 14, 14, 3, 14, 14, 16, 16, 3, 3, 3, 3, 3, 3, 3, 14, 14, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xce0:
      3, 3, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xd00, 0x11300:
      16, 16, 14, 14, 3,

      // Start of 0xd20, 0x11320:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 3, 14,

      // Start of 0xd40:
      14, 16, 16, 16, 16, 3, 14, 14, 14, 3, 14, 14, 14, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14,

      // Start of 0xdc0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 3, 16, 3, 14, 14, 14, 14, 14, 14,
      14, 14,

      // Start of 0xde0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14,

      // Start of 0xe20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 3, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0xe40:
      3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0xea0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0xec0:
      3, 3, 3, 3,

      // Start of 0x1e940:
      3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0xf00:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16,

      // Start of 0xf20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 16, 3, 16, 3, 3, 3, 3, 14, 14,

      // Start of 0xf60:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
      16, 14,

      // Start of 0xf80:
      16, 16, 16, 16, 16, 3, 16, 16, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 16, 16, 16,
      16, 16, 16, 16,

      // Start of 0x1020:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 14, 16, 16, 14, 14,
      16, 16,

      // Start of 0x1040:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 16, 16, 3, 3, 3, 3, 16,

      // Start of 0x1060:
      16, 3, 14, 14, 14, 3, 3, 14, 14, 14, 14, 14, 14, 14, 3, 3, 3, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1080:
      3, 3, 16, 14, 14, 16, 16, 14, 14, 14, 14, 14, 14, 16, 3, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14,
      16,

      // Start of 0x1700:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 14,

      // Start of 0x1720:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 14,

      // Start of 0x17a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0x17c0:
      14, 14, 14, 14, 14, 14, 16, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 16,

      // Start of 0x1800:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1920:
      16, 16, 16, 14, 14, 14, 14, 16, 16, 14, 14, 14, 3, 3, 3, 3, 14, 14, 16, 14, 14, 14, 14, 14, 14, 16, 16,
      16,

      // Start of 0x1a00:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 14, 14, 16,

      // Start of 0x1a40:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 16, 14, 16, 16, 16, 16, 16, 16, 16,
      3,

      // Start of 0x1a60:
      16, 14, 16, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 14, 14, 14, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16,
      16, 16, 16, 3, 3, 16,

      // Start of 0x1aa0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
      15,

      // Start of 0x1ac0:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0x1b20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 14, 16, 16, 16, 16, 16, 14, 16, 14,

      // Start of 0x1b40:
      14, 14, 16, 14, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1b60:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1b80, 0x11080, 0x11180:
      16, 16, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1ba0:
      3, 14, 16, 16, 16, 16, 14, 14, 16, 16, 14, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1be0:
      3, 3, 3, 3, 3, 3, 16, 14, 16, 16, 14, 14, 14, 16, 14, 16, 16, 16, 14, 14, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1c20:
      3, 3, 3, 3, 14, 14, 14, 14, 14, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 14, 14, 16, 16,

      // Start of 0x1cc0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
      16,

      // Start of 0x1ce0:
      16, 14, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 16, 3, 3, 14, 16, 16,

      // Start of 0x20c0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 15,
      15,

      // Start of 0x20e0:
      15, 16, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0x2ce0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 3, 3, 3, 3,

      // Start of 0x3020:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 14, 14,

      // Start of 0x3080:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16,

      // Start of 0xa660:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 15, 15, 15, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0xa800:
      3, 3, 16, 3, 3, 3, 16, 3, 3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xa820:
      3, 3, 3, 14, 14, 16, 16, 14, 3, 3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11140:
      3, 3, 3, 3, 3,

      // Start of 0xa880:
      14, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xa8a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 14, 14, 14, 14, 14,

      // Start of 0xa8c0:
      14, 14, 14, 14, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xa8e0:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xa920:
      3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xa940:
      3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 14,

      // Start of 0xa9a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 14, 14, 16, 16, 16, 16, 14, 14, 16, 16, 14,

      // Start of 0xa9c0:
      14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xaa20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 14, 14, 16, 16, 14, 14, 16, 16, 3, 3, 3, 3, 3, 3,

      // Start of 0xaa40:
      3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 16, 14,

      // Start of 0xaa60:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 16, 14,

      // Start of 0xaaa0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 16, 16, 16, 3, 3, 16, 16, 3, 3, 3, 3, 3, 16, 16,

      // Start of 0xaae0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 16, 16, 14, 14, 3, 3, 3, 3, 3, 14, 16, 3, 3, 3, 3, 3, 3,

      // Start of 0xabe0:
      3, 3, 3, 14, 14, 16, 14, 14, 16, 14,

      // Start of 0x11940:
      14, 3, 14, 16,

      // Start of 0xfb00:
      3,

      // Start of 0x101e0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11160:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x102e0, 0x11640:
      16, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10360:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 3, 3, 3, 3,

      // Start of 0x10a00:
      3, 16, 16, 16, 3, 16, 16, 3,

      // Start of 0x10d20:
      3, 3, 3, 3, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x16ae0:
      3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10d60:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3,

      // Start of 0x1e2e0, 0x1e4e0:
      3, 3,

      // Start of 0x1d1a0:
      3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10f80:
      3, 3, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11000:
      14, 16, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x16b20, 0x1e120, 0x1e8c0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11040, 0x1cf40:
      16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11060:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 3, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16,

      // Start of 0x110a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16, 14, 14, 16, 16, 3, 3, 3,

      // Start of 0x1d240:
      3, 3,

      // Start of 0x11100:
      16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11120:
      3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0x111a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16,

      // Start of 0x111c0:
      14, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 3, 14, 16, 3, 3, 3, 3,

      // Start of 0x11220:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 14, 14, 16, 14, 16, 16, 3, 3, 3, 3, 3, 3,
      16, 3,

      // Start of 0x112e0:
      14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3,

      // Start of 0x11340:
      16, 14, 14, 14, 14, 3, 3, 14, 14, 3, 3, 14, 14, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 3, 3, 3, 3, 3, 3,

      // Start of 0x11360:
      3, 3, 14, 14, 3, 3, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 16, 16, 16, 16, 16,

      // Start of 0x113a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16,

      // Start of 0x113c0:
      16, 3, 14, 3, 3, 14, 3, 14, 14, 14, 14, 3, 14, 14, 16, 14, 16, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3,

      // Start of 0x113e0:
      3, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11420:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16,
      16,

      // Start of 0x11440:
      14, 14, 16, 16,

      // Start of 0x11f40:
      16, 14, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16,

      // Start of 0x114a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16, 16, 16, 14, 16, 14, 14, 14,
      14,

      // Start of 0x114c0:
      16, 14, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x115a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16, 3, 3, 14, 14, 14, 14, 16, 16,
      14,

      // Start of 0x115c0:
      16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16,

      // Start of 0x11620:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 14, 14, 16,
      14, 16,

      // Start of 0x116a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 14, 16, 14, 14, 16, 16, 16, 16, 16, 16, 14, 16,

      // Start of 0x11700:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 14, 16,

      // Start of 0x11720:
      14, 14, 16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11820:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 16, 16,

      // Start of 0x11920:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 14, 14, 14, 3, 14, 14, 3, 3, 16, 16, 14, 16,

      // Start of 0x119c0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 16, 16, 16, 16, 3, 3, 16, 16, 14, 14, 14,
      14,

      // Start of 0x119e0:
      16, 3, 3, 3, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11a00:
      3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3,

      // Start of 0x11a20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 14, 3, 16, 16, 16, 16,

      // Start of 0x11a40:
      3, 3, 3, 3, 3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 14, 14, 16, 16, 16,

      // Start of 0x11a80:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 16, 16,

      // Start of 0x11c20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 16, 16, 16, 16, 16, 16, 16, 3, 16, 16, 16, 16, 16, 16,
      14,

      // Start of 0x11ca0:
      16, 16, 16, 16, 16, 16, 16, 16, 3, 14, 16, 16, 16, 16, 16, 16, 16, 14, 16, 16, 14, 16, 16,

      // Start of 0x11d20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 3, 3, 3, 16, 3, 16, 16, 3,

      // Start of 0x11d40:
      16, 16, 16, 16, 16, 16, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11d80:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 14, 14, 14, 3, 16, 16, 3, 14, 14, 16, 14, 16,

      // Start of 0x11ee0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 14, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11f00:
      16, 16, 3, 14, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x11f20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 16, 16, 16, 16, 16, 3, 3, 3, 14, 14,

      // Start of 0x13440:
      16, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3,

      // Start of 0x16120:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 14, 14, 16, 16, 16, 3,

      // Start of 0x16f40:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3,

      // Start of 0x16f60:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,

      // Start of 0x16f80:
      14, 14, 14, 14, 14, 14, 14, 14, 3, 3, 3, 3, 3, 3, 3, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x16fe0:
      3, 3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14,

      // Start of 0x1bc80:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16, 3,

      // Start of 0x1cf20:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
      16, 16, 16, 16, 16, 16,

      // Start of 0x1d160:
      3, 3, 3, 3, 3, 14, 14, 16, 16, 16, 3, 3, 3, 14, 14, 14, 14, 14, 14, 3, 3, 3, 3, 3, 3, 3, 3, 16, 16,

      // Start of 0x1d180:
      16, 16, 16, 3, 3, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3,

      // Start of 0x1da20:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3,

      // Start of 0x1da60:
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 16, 3, 3, 3, 3, 3, 3, 3, 3,
      3,

      // Start of 0x1daa0:
      3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3,

      // Start of 0x1e000:
      16, 16, 16, 16, 16, 16, 16, 3, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3,
      16, 16, 16,

      // Start of 0x1e020:
      16, 16, 3, 16, 16, 3, 16, 16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3};




} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_GC_TABLES_HPP
