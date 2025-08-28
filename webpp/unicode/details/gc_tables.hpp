
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_general_categories_tables.mjs
 *   Unicode UCD Database Creation Date: 2024-08-25
 *   This file's generation date:        Thu, 28 Aug 2025 17:21:22 GMT
 *   Unicode Version:                    16.0.0
 *   Total Table sizes in this file:
 *       - in bits:       77960
 *       - in bytes:      9745 B
 *       - in KibiBytes:  9.52 KiB
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
 *   These General Categories have been excluded from this file because we've decided
 *   these we don't need them for now:
 *       Other,
 *       Control,
 *       Format,
 *       Private_Use,
 *       Surrogate,
 *       Letter,
 *       Cased_Letter,
 *       Lowercase_Letter,
 *       Modifier_Letter,
 *       Other_Letter,
 *       Titlecase_Letter,
 *       Uppercase_Letter,
 *       Number,
 *       Decimal_Number,
 *       Letter_Number,
 *       Other_Number,
 *       Punctuation,
 *       Connector_Punctuation,
 *       Dash_Punctuation,
 *       Close_Punctuation,
 *       Final_Punctuation,
 *       Initial_Punctuation,
 *       Other_Punctuation,
 *       Open_Punctuation,
 *       Symbol,
 *       Currency_Symbol,
 *       Modifier_Symbol,
 *       Math_Symbol,
 *       Other_Symbol,
 *       Separator,
 *       Line_Separator,
 *       Paragraph_Separator,
 *       Space_Separator
 *
 *
 */

#ifndef WEBPP_UNICODE_GC_TABLES_HPP
#define WEBPP_UNICODE_GC_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode {

    // NOLINTBEGIN(*-missing-parentheses)
    /// Giving aliases to known values:
    /// Can be re-generating using:
    /// curl --silent https://www.unicode.org/Public/UCD/latest/ucd/PropertyValueAliases.txt | grep -oE '[^#]+' | awk
    /// 'BEGIN{FS=";"; i=0; print "enum struct general_category : std::uint8_t {"; } {gsub(/ */, "", $1); gsub(/ */, "",
    /// $3); gsub(/ */, "", $2); if ($1 == "gc") {print "  " $3 " = " i ",\n  " $2 " = " i ",\n"; i=i+1; }} END { print
    /// "};"; }'

    enum struct general_category : std::uint8_t {
        Other                 = 32U,
        C                     = 1U * 32U + 0U,
        Control               = 33U,
        Cc                    = 1U * 32U + 1U,
        Format                = 34U,
        Cf                    = 1U * 32U + 2U,
        Unassigned            = 35U,
        Cn                    = 1U * 32U + 3U,
        Private_Use           = 36U,
        Co                    = 1U * 32U + 4U,
        Surrogate             = 37U,
        Cs                    = 1U * 32U + 5U,
        Letter                = 64U,
        L                     = 2U * 32U + 0U,
        Cased_Letter          = 65U,
        LC                    = 2U * 32U + 1U,
        Lowercase_Letter      = 66U,
        Ll                    = 2U * 32U + 2U,
        Modifier_Letter       = 67U,
        Lm                    = 2U * 32U + 3U,
        Other_Letter          = 68U,
        Lo                    = 2U * 32U + 4U,
        Titlecase_Letter      = 69U,
        Lt                    = 2U * 32U + 5U,
        Uppercase_Letter      = 70U,
        Lu                    = 2U * 32U + 6U,
        Mark                  = 96U,
        M                     = 3U * 32U + 0U,
        Spacing_Mark          = 97U,
        Mc                    = 3U * 32U + 1U,
        Enclosing_Mark        = 98U,
        Me                    = 3U * 32U + 2U,
        Nonspacing_Mark       = 99U,
        Mn                    = 3U * 32U + 3U,
        Number                = 128U,
        N                     = 4U * 32U + 0U,
        Decimal_Number        = 129U,
        Nd                    = 4U * 32U + 1U,
        Letter_Number         = 130U,
        Nl                    = 4U * 32U + 2U,
        Other_Number          = 131U,
        No                    = 4U * 32U + 3U,
        Punctuation           = 160U,
        P                     = 5U * 32U + 0U,
        Connector_Punctuation = 161U,
        Pc                    = 5U * 32U + 1U,
        Dash_Punctuation      = 162U,
        Pd                    = 5U * 32U + 2U,
        Close_Punctuation     = 163U,
        Pe                    = 5U * 32U + 3U,
        Final_Punctuation     = 164U,
        Pf                    = 5U * 32U + 4U,
        Initial_Punctuation   = 165U,
        Pi                    = 5U * 32U + 5U,
        Other_Punctuation     = 166U,
        Po                    = 5U * 32U + 6U,
        Open_Punctuation      = 167U,
        Ps                    = 5U * 32U + 7U,
        Symbol                = 192U,
        S                     = 6U * 32U + 0U,
        Currency_Symbol       = 193U,
        Sc                    = 6U * 32U + 1U,
        Modifier_Symbol       = 194U,
        Sk                    = 6U * 32U + 2U,
        Math_Symbol           = 195U,
        Sm                    = 6U * 32U + 3U,
        Other_Symbol          = 196U,
        So                    = 6U * 32U + 4U,
        Separator             = 224U,
        Z                     = 7U * 32U + 0U,
        Line_Separator        = 225U,
        Zl                    = 7U * 32U + 1U,
        Paragraph_Separator   = 226U,
        Zp                    = 7U * 32U + 2U,
        Space_Separator       = 227U,
        Zs                    = 7U * 32U + 3U,
    };

    // NOLINTEND(*-missing-parentheses)

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
        explicit(false) consteval gc_index(std::uint16_t const value) noexcept :
          pos{static_cast<std::uint16_t>(value)} {}

        [[nodiscard]] constexpr std::uint16_t value() const noexcept {
            return pos;
        }

        static constexpr std::uint16_t chunk_mask  = 0x1FU;
        static constexpr std::size_t   chunk_size  = 32U;
        static constexpr std::uint8_t  chunk_shift = 5U;

        /**
         * Get the final position of the second table.
         * This does not apply the shift or get the value of the second table for you; this only applies that mask.
         */
        [[nodiscard]] constexpr std::uint16_t get_position(auto const request_position) const noexcept {
            auto const remaining_pos = static_cast<std::uint16_t>(request_position & chunk_mask);
            return pos + remaining_pos;
        }
    };

    struct gc_breakpoint_type {
        std::uint16_t starting;
        std::uint16_t ending;
        std::uint16_t offset;
    };

    /**
     * You can choose between the indices' table using these breakpoints:
     *
     * Table size in KibiBytes:  2.67 KiB
     */
    static constexpr std::array<gc_breakpoint_type, 57U> gc_breakpoints{
      gc_breakpoint_type{    .starting = 0,   .ending = 389,     .offset = 0}, // Section 1
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 1330,  .ending = 1376,   .offset = 941}, // Section 2
      { .starting = 2007,  .ending = 2467,  .offset = 1572}, // Section 3
      { .starting = 2007,  .ending = 2467,  .offset = 1572}, // Section 3
      { .starting = 2823,  .ending = 2944,  .offset = 1928}, // Section 4
      { .starting = 3555,  .ending = 3915,  .offset = 2539}, // Section 5
      { .starting = 3555,  .ending = 3915,  .offset = 2539}, // Section 5
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
      {.starting = 28679, .ending = 28688, .offset = 27303}, // Section 6
    };

    static constexpr std::uint16_t gc_last_breakpoint{28688U};
    static constexpr std::uint16_t gc_breakpoint_shift{9U};
    static constexpr gc_index      gc_common_pos{0U}; // this is the last common value position


    /**
     * gc Index Table (combined 6 sections)
     *
     * GC: General Category
     * These are the indices that are used to find which values from "gc_values" table correspond to a Unicode Code
     * Point.
     *
     * Each value is a [16bits = pos]
     *
     * Table size:
     *   - in bits:       22160
     *   - in bytes:      2770 B
     *   - in KibiBytes:  2.71 KiB
     */
    static constexpr std::array<gc_index, 1385ULL> gc_indices{
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 32, 48, 0, 0, 0, 0, 0, 0, 0, 0,
      77, 0, 0, 0, 0, 0, 0, 0, 15, 109, 139, 0, 155, 0, 21, 182, 0, 0, 199, 223, 238, 16, 270, 0, 0, 165, 0, 291, 321,
      349, 363, 0, 9, 0, 22, 395, 424, 430, 461, 491, 522, 526, 557, 587, 618, 526, 650, 668, 618, 526, 700, 730, 522,
      762, 792, 491, 822, 825, 857, 0, 889, 894, 925, 491, 522, 762, 957, 987, 1019, 1024, 1055, 491, 522, 0, 1079,
      1111, 0, 1131, 1158, 0, 0, 1173, 1202, 0, 1217, 1243, 0, 1275, 1307, 35, 818, 0, 0, 1339, 1370, 1401, 1431, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1461, 1483, 714, 714, 0, 1504, 1534, 0, 1564, 0, 0, 0, 488, 189, 0, 0, 0, 1596,
      0, 0, 0, 0, 0, 0, 1624, 0, 1652, 1684, 0, 1716, 1747, 0, 423, 1762, 1792, 1813, 1845, 1876, 0, 1902, 0, 1930, 0,
      0, 0, 0, 1954, 1985, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      2011, 2042, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2059, 0, 0, 0, 1, 0, 0, 0, 32, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2081, 0, 0, 2097,
      // End of Section #1 [0, 389) containing 389 values (0.76 KiB). Skipping 941 values.

      0, 2124, 2, 0, 0, 716, 0, 0, 0, 0, 0, 0, 0, 0, 2154, 2183, 0, 0, 2215, 2227, 2255, 2287, 0, 2312, 2337, 0, 424,
      2357, 2388, 819, 0, 2411, 2440, 2454, 0, 2484, 823, 2516, 0, 0, 0, 0, 0, 0, 0, 2545,
      // End of Section #2 [1330, 1376) containing 46 values (0.09 KiB). Skipping 631 values.

      0, 2559, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2560, 0, 0, 0, 0, 0, 0, 0, 2589, 0, 0, 0, 2599, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2630, 364, 0, 0, 0, 0, 0, 488, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 2638, 0, 2661, 0, 0, 0, 0, 0, 0, 0, 0, 0, 721, 0, 4, 0, 0, 165, 0, 2691, 0, 0, 0, 2723, 8, 2755, 2771, 1845,
      2803, 822, 0, 2835, 2860, 2210, 2570, 1845, 2881, 2912, 0, 0, 2932, 823, 0, 0, 0, 1, 2964, 1019, 1024, 2996, 3026,
      0, 3047, 3078, 3109, 0, 3120, 3152, 0, 0, 3183, 3214, 0, 0, 0, 0, 0, 0, 3231, 3262, 0, 0, 3292, 2589, 0, 0, 3324,
      0, 0, 3348, 3380, 0, 0, 0, 0, 0, 0, 0, 3400, 0, 0, 0, 0, 0, 0, 0, 3427, 2555, 0, 0, 0, 3458, 3490, 3521, 3534,
      3565, 0, 3593, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3619, 0, 0, 14, 3650, 0, 0, 0, 3673, 3704, 0, 3726, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 3750, 3782, 3794, 3156, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3826,
      // End of Section #3 [2007, 2467) containing 460 values (0.90 KiB). Skipping 356 values.

      0, 2, 3858, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 2654, 0, 2739, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 3875, 3892, 3916, 0, 0, 3944,
      // End of Section #4 [2823, 2944) containing 121 values (0.24 KiB). Skipping 611 values.

      0, 3962, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32,
      3994, 2755, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4026, 4055, 2683, 0, 0, 0, 0, 2833, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 4087, 32, 4114, 194, 4145, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4177, 4207, 0, 0,
      183, 0, 0, 0, 0, 2739, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 184, 0, 2681, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      2681, 0, 0, 0, 0, 0, 0, 0, 718, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2739, 0, 0, 0,
      1206,
      // End of Section #5 [3555, 3915) containing 360 values (0.70 KiB). Skipping 24764 values.

      0, 32, 32, 32, 32, 32, 32, 32, 48,
      // End of Section #6 [28679, 28688) containing 9 values (0.02 KiB).

      // done
    };




    /**
     * GC Values Table
     *
     * GC: General Category
     * These values are calculated and individually represent actual GC values, but they have no
     * valid order by themselves, and they only make sense if they're being used in conjunction with
     * the "gc_indices" table.
     *
     * Table size:
     *   - in bits:       33912
     *   - in bytes:      4239 B
     *   - in KibiBytes:  4.14 KiB
     */
    static constexpr std::array<std::uint8_t, 4239ULL> gc_values{

      // Start of 0x0-0x2e0, 0x380-0x460, 0x4a0-0x560, 0x5e0, 0x620, 0x680-0x6a0, 0x760-0x780, 0x7c0, 0x860, 0x8a0,
      // 0xbe0, 0xda0, 0xe00, 0xe60-0xe80, 0xee0, 0xf40, 0xfe0-0x1000, 0x10a0-0x1320, 0x1360-0x16e0, 0x1780, 0x17e0,
      // 0x1820-0x1860, 0x18c0-0x1900, 0x1940-0x19e0, 0x1a20, 0x1a80, 0x1ae0, 0x1bc0, 0x1c00, 0x1c40-0x1ca0,
      // 0x1d00-0x1da0, 0x1e00-0x20a0, 0x2100-0x2cc0, 0x2d00-0x2d40, 0x2d80-0x2dc0, 0x2e00-0x3000, 0x3040-0x3060,
      // 0x30a0-0xa640, 0xa6a0-0xa6c0, 0xa700-0xa7e0, 0xa840-0xa860, 0xa900, 0xa960, 0xaa00, 0xaa80, 0xab00-0xabc0,
      // 0xac00-0xfae0, 0xfb20-0xfde0, 0xfe40-0x101c0, 0x10200-0x102c0, 0x10300-0x10340, 0x10380-0x109e0,
      // 0x10a40-0x10ac0, 0x10b00-0x10d00, 0x10d40, 0x10d80-0x10e80, 0x10ec0, 0x10f00-0x10f20, 0x10f60, 0x10fa0-0x10fe0,
      // 0x110e0, 0x111e0-0x11200, 0x11260-0x112a0, 0x11380, 0x11400, 0x11460-0x11480, 0x114e0-0x11580, 0x115e0-0x11600,
      // 0x11660-0x11680, 0x116c0-0x116e0, 0x11740-0x11800, 0x11840-0x11900, 0x11960-0x119a0, 0x11a60, 0x11aa0-0x11c00,
      // 0x11c40-0x11c60, 0x11cc0-0x11d00, 0x11d60, 0x11da0-0x11ec0, 0x11f60-0x13420, 0x13460-0x160e0, 0x16140-0x16ac0,
      // 0x16b00, 0x16b40-0x16f20, 0x16fa0-0x16fc0, 0x17000-0x1bc60, 0x1bca0-0x1cee0, 0x1cf60-0x1d140, 0x1d1c0-0x1d220,
      // 0x1d260-0x1d9e0, 0x1dac0-0x1dfe0, 0x1e040-0x1e060, 0x1e0a0-0x1e100, 0x1e140-0x1e280, 0x1e2c0, 0x1e300-0x1e4c0,
      // 0x1e500-0x1e5c0, 0x1e600-0x1e8a0, 0x1e8e0-0x1e920, 0x1e960-0xe00e0:
      35,

      // Start of 0x2d60, 0x112c0:
      35,

      // Start of 0xa680, 0x16100:
      35,

      // Start of 0x1340:
      35,

      // Start of 0x10ee0:
      35, 35, 35, 35,

      // Start of 0x11020:
      35,

      // Start of 0x880:
      35, 35, 35, 35, 35,

      // Start of 0x11c80:
      35,

      // Start of 0x580:
      35,

      // Start of 0x720:
      35, 35, 35, 35, 35,

      // Start of 0x640:
      35,

      // Start of 0x8c0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x300-0x340, 0x1dc0-0x1de0, 0x2de0, 0x1cf00, 0x1da00, 0x1da40, 0xe0100-0xe01c0:
      99, 99, 99,

      // Start of 0xfa0:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0x360, 0xfe00-0xfe20, 0xe01e0:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35,

      // Start of 0x480:
      35, 35, 35, 99, 99, 99, 99, 99, 98, 98, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x5a0:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
      99, 99,

      // Start of 0x5c0:
      35, 99, 99, 35, 99, 99, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x600:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x7a0, 0x10f40:
      35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0x660:
      35,

      // Start of 0x1e080:
      35,

      // Start of 0x1e2a0:
      35, 35, 35, 35, 35,

      // Start of 0x18a0:
      35, 35, 35, 35, 35,

      // Start of 0x1da80:
      35, 35, 35, 35, 99,

      // Start of 0x6c0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99,

      // Start of 0x6e0:
      99, 99, 99, 99, 99, 35, 35, 99, 99, 35, 99, 99, 99, 99, 35,

      // Start of 0x700:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x740:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x7e0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 99,

      // Start of 0x800:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 35, 99,

      // Start of 0x820:
      99, 99, 99, 99, 35, 99, 99, 99, 35, 99, 99, 99, 99, 99,

      // Start of 0x840:
      35,

      // Start of 0x10a20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 35,
      35, 35, 35,

      // Start of 0x8e0:
      99, 99, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0x1b00:
      99,

      // Start of 0x900, 0xa980:
      99, 99, 99, 97, 35, 35,

      // Start of 0x920:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 97,
      99, 35, 97,

      // Start of 0x940:
      97, 99, 99, 99, 99, 99, 99, 99, 99, 97, 97, 97, 97, 99, 97, 97, 35, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35,

      // Start of 0x1880, 0x10ae0:
      35, 35, 35,

      // Start of 0x960, 0xb60, 0xc60, 0xd60:
      35, 35, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35,

      // Start of 0x980, 0xb00, 0xc80, 0xd80:
      35, 99, 97, 97,

      // Start of 0x9a0, 0xa20, 0xaa0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      99, 35, 97,

      // Start of 0x9c0:
      97, 99, 99, 99, 99, 35, 35, 97, 97, 35, 35, 97, 97, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 35, 35, 35, 35,
      35, 35,

      // Start of 0x9e0:
      35, 35, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 99,

      // Start of 0xa00, 0xa80:
      35, 99, 99, 97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0xa40:
      97, 99, 99, 35, 35, 35, 35, 99, 99, 35, 35, 99, 99, 99, 35, 35, 35, 99,

      // Start of 0xa60:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 35, 35, 35, 99, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0xac0:
      97, 99, 99, 99, 99, 99, 35, 99, 99, 97, 35, 97, 97, 99,

      // Start of 0x1740-0x1760:
      35, 35,

      // Start of 0xa6e0:
      35, 35,

      // Start of 0x1e5e0:
      35, 35, 35,

      // Start of 0x10ea0:
      35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0xae0:
      35, 35, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99,
      99, 99, 99, 99,

      // Start of 0xb20, 0xca0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      99, 35,

      // Start of 0xb40:
      97, 99, 99, 99, 99, 35, 35, 97, 97, 35, 35, 97, 97, 99, 35, 35, 35, 35, 35, 35, 35, 99, 99, 97, 35, 35,

      // Start of 0xfc0:
      35,

      // Start of 0xa9e0:
      35, 35, 35,

      // Start of 0xb80, 0x110c0:
      35,

      // Start of 0xaac0, 0x11240:
      35, 99,

      // Start of 0xba0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 97, 97,

      // Start of 0xbc0:
      99, 97, 97, 35, 35, 35, 97, 97, 97, 35, 97, 97, 97, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0xc00:
      99, 97, 97, 97, 99,

      // Start of 0xc20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      99, 35, 99,

      // Start of 0xc40:
      99, 97, 97, 97, 97, 35, 99, 99, 99, 35, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 99, 99, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0xcc0:
      97, 97, 97, 97, 97, 35, 99, 97, 97, 35, 97, 97, 99, 99, 35, 35, 35, 35, 35, 35, 35, 97, 97, 35, 35, 35, 35, 35,
      35, 35,

      // Start of 0xce0:
      35, 35, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0xd00, 0x11300:
      99, 99, 97, 97, 35,

      // Start of 0xd20, 0x11320:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99,
      99, 35, 97,

      // Start of 0xd40:
      97, 99, 99, 99, 99, 35, 97, 97, 97, 35, 97, 97, 97, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97,

      // Start of 0xdc0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 35, 99, 35, 97, 97, 97, 97,
      97, 97, 97, 97,

      // Start of 0xde0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97,

      // Start of 0xe20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35, 35, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0xe40:
      35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0xea0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99,
      99,

      // Start of 0xec0:
      35, 35, 35, 35,

      // Start of 0x1e940:
      35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0xf00:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99,

      // Start of 0xf20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35, 99, 35, 99, 35, 35,
      35, 35, 97, 97,

      // Start of 0xf60:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 97,

      // Start of 0xf80:
      99, 99, 99, 99, 99, 35, 99, 99, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 99, 99, 99,
      99, 99, 99, 99,

      // Start of 0x1020:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 99, 99, 99, 99, 97, 99, 99, 99, 99, 99, 99, 97, 99, 99, 97,
      97, 99, 99,

      // Start of 0x1040:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 99, 99, 35, 35,
      35, 35, 99,

      // Start of 0x1060:
      99, 35, 97, 97, 97, 35, 35, 97, 97, 97, 97, 97, 97, 97, 35, 35, 35, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35,
      35, 35,

      // Start of 0x1080:
      35, 35, 99, 97, 97, 99, 99, 97, 97, 97, 97, 97, 97, 99, 35, 97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97,
      97, 99,

      // Start of 0x1700:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 97,

      // Start of 0x1720:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 97,

      // Start of 0x17a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 97, 99, 99, 99, 99, 99,
      99, 99,

      // Start of 0x17c0:
      97, 97, 97, 97, 97, 97, 99, 97, 97, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 99,

      // Start of 0x1800:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x1920:
      99, 99, 99, 97, 97, 97, 97, 99, 99, 97, 97, 97, 35, 35, 35, 35, 97, 97, 99, 97, 97, 97, 97, 97, 97, 99, 99, 99,

      // Start of 0x1a00:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 97, 97, 99,

      // Start of 0x1a40:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 99, 97, 99, 99, 99, 99,
      99, 99, 99, 35,

      // Start of 0x1a60:
      99, 97, 99, 97, 97, 99, 99, 99, 99, 99, 99, 99, 99, 97, 97, 97, 97, 97, 97, 99, 99, 99, 99, 99, 99, 99, 99, 99,
      99, 35, 35, 99,

      // Start of 0x1aa0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 98,

      // Start of 0x1ac0:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0x1b20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 97, 99, 99, 99, 99, 99, 97,
      99, 97,

      // Start of 0x1b40:
      97, 97, 99, 97, 97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x1b60:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x1b80, 0x11080, 0x11180:
      99, 99, 97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35,

      // Start of 0x1ba0:
      35, 97, 99, 99, 99, 99, 97, 97, 99, 99, 97, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x1be0:
      35, 35, 35, 35, 35, 35, 99, 97, 99, 99, 97, 97, 97, 99, 97, 99, 99, 99, 97, 97, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x1c20:
      35, 35, 35, 35, 97, 97, 97, 97, 97, 97, 97, 97, 99, 99, 99, 99, 99, 99, 99, 99, 97, 97, 99, 99,

      // Start of 0x1cc0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 35, 99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99,

      // Start of 0x1ce0:
      99, 97, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 99, 35, 35, 35, 35, 35, 35, 99, 35, 35, 97, 99, 99,

      // Start of 0x20c0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
      99, 98, 98,

      // Start of 0x20e0:
      98, 99, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0x2ce0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 35, 35, 35, 35,

      // Start of 0x3020:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 97, 97,

      // Start of 0x3080:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99,

      // Start of 0xa660:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 98, 98, 98, 35, 99, 99, 99, 99, 99, 99, 99, 99,
      99, 99,

      // Start of 0xa800:
      35, 35, 99, 35, 35, 35, 99, 35, 35, 35, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35,

      // Start of 0xa820:
      35, 35, 35, 97, 97, 99, 99, 97, 35, 35, 35, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11140:
      35, 35, 35, 35, 35,

      // Start of 0xa880:
      97, 97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0xa8a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 97, 97, 97, 97, 97,

      // Start of 0xa8c0:
      97, 97, 97, 97, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0xa8e0:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0xa920:
      35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0xa940:
      35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 97, 97,

      // Start of 0xa9a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 97, 97, 99, 99, 99, 99, 97, 97,
      99, 99, 97,

      // Start of 0xa9c0:
      97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0xaa20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 97, 97, 99, 99, 97, 97, 99, 99, 35, 35, 35, 35, 35,
      35,

      // Start of 0xaa40:
      35, 35, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 99, 97,

      // Start of 0xaa60:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97,
      99, 97,

      // Start of 0xaaa0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35, 99, 99, 99, 35, 35, 99, 99, 35, 35, 35,
      35, 35, 99, 99,

      // Start of 0xaae0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 99, 99, 97, 97, 35, 35, 35, 35, 35, 97, 99, 35, 35, 35, 35, 35,
      35,

      // Start of 0xabe0:
      35, 35, 35, 97, 97, 99, 97, 97, 99, 97,

      // Start of 0x11940:
      97, 35, 97, 99,

      // Start of 0xfb00:
      35,

      // Start of 0x101e0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11160:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x102e0, 0x11640:
      99, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x10360:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 35,
      35, 35, 35,

      // Start of 0x10a00:
      35, 99, 99, 99, 35, 99, 99, 35,

      // Start of 0x10d20:
      35, 35, 35, 35, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x16ae0:
      35, 35, 35, 35, 35, 35, 35,

      // Start of 0x10d60:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35,

      // Start of 0x1e2e0, 0x1e4e0:
      35, 35,

      // Start of 0x1d1a0:
      35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x10f80:
      35, 35, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x11000:
      97, 99, 97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x16b20, 0x1e120, 0x1e8c0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11040, 0x1cf40:
      99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11060:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35, 35, 99, 99, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 99,

      // Start of 0x110a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 99, 97, 97, 99, 99, 35,
      35, 35,

      // Start of 0x1d240:
      35, 35,

      // Start of 0x11100:
      99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11120:
      35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 97, 99, 99, 99, 99, 99, 99, 99, 99,

      // Start of 0x111a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 99, 99, 99,
      99, 99, 99,

      // Start of 0x111c0:
      97, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 35, 97, 99, 35, 35, 35, 35,

      // Start of 0x11220:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 97, 97, 99, 97, 99, 99, 35, 35, 35, 35,
      35, 35, 99, 35,

      // Start of 0x112e0:
      97, 97, 97, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x11340:
      99, 97, 97, 97, 97, 35, 35, 97, 97, 35, 35, 97, 97, 97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 35, 35, 35, 35,
      35, 35,

      // Start of 0x11360:
      35, 35, 97, 97, 35, 35, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 99, 99, 99, 99, 99,

      // Start of 0x113a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99,
      99, 99, 99,

      // Start of 0x113c0:
      99, 35, 97, 35, 35, 97, 35, 97, 97, 97, 97, 35, 97, 97, 99, 97, 99, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35,

      // Start of 0x113e0:
      35, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11420:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 99,
      99, 99, 99, 99,

      // Start of 0x11440:
      97, 97, 99, 99,

      // Start of 0x11f40:
      99, 97, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99,

      // Start of 0x114a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 99, 99, 99, 97, 99, 97,
      97, 97, 97,

      // Start of 0x114c0:
      99, 97, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x115a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 99, 35, 35, 97, 97, 97, 97,
      99, 99, 97,

      // Start of 0x115c0:
      99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      99, 99,

      // Start of 0x11620:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 99, 99, 99, 99, 99, 97,
      97, 99, 97, 99,

      // Start of 0x116a0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 97, 99, 97, 97, 99, 99, 99, 99, 99, 99, 97, 99,

      // Start of 0x11700:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 99, 97, 99,

      // Start of 0x11720:
      97, 97, 99, 99, 99, 99, 97, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11820:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 99, 99, 99, 99, 99, 99, 97, 99, 99,

      // Start of 0x11920:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 97, 97, 97, 35, 97, 97, 35, 35, 99,
      99, 97, 99,

      // Start of 0x119c0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 99, 99, 99, 99, 35, 35, 99, 99,
      97, 97, 97, 97,

      // Start of 0x119e0:
      99, 35, 35, 35, 97, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35,

      // Start of 0x11a00:
      35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35,

      // Start of 0x11a20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 97, 35, 99,
      99, 99, 99,

      // Start of 0x11a40:
      35, 35, 35, 35, 35, 35, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 97, 97, 99, 99, 99,

      // Start of 0x11a80:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 97, 99, 99,

      // Start of 0x11c20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 99, 99, 99, 99, 99, 99, 99, 35, 99, 99, 99, 99,
      99, 99, 97,

      // Start of 0x11ca0:
      99, 99, 99, 99, 99, 99, 99, 99, 35, 97, 99, 99, 99, 99, 99, 99, 99, 97, 99, 99, 97, 99, 99,

      // Start of 0x11d20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 35, 35, 35, 99, 35,
      99, 99, 35,

      // Start of 0x11d40:
      99, 99, 99, 99, 99, 99, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11d80:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 97, 97, 97, 35, 99, 99, 35, 97, 97, 99, 97, 99,

      // Start of 0x11ee0:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 99, 97, 97, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x11f00:
      99, 99, 35, 97, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x11f20:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97, 99, 99, 99, 99, 99, 35,
      35, 35, 97, 97,

      // Start of 0x13440:
      99, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x16120:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 97, 97, 97, 99, 99, 99, 35,

      // Start of 0x16f40:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35,

      // Start of 0x16f60:
      97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,

      // Start of 0x16f80:
      97, 97, 97, 97, 97, 97, 97, 97, 35, 35, 35, 35, 35, 35, 35, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35,

      // Start of 0x16fe0:
      35, 35, 35, 35, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 97, 97,

      // Start of 0x1bc80:
      35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 99, 99, 35,

      // Start of 0x1cf20:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99,

      // Start of 0x1d160:
      35, 35, 35, 35, 35, 97, 97, 99, 99, 99, 35, 35, 35, 97, 97, 97, 97, 97, 97, 35, 35, 35, 35, 35, 35, 35, 35, 99,
      99,

      // Start of 0x1d180:
      99, 99, 99, 35, 35, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x1da20:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35,

      // Start of 0x1da60:
      99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 99, 35, 35, 35, 35, 35, 35,
      35, 35, 35,

      // Start of 0x1daa0:
      35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,

      // Start of 0x1e000:
      99, 99, 99, 99, 99, 99, 99, 35, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 35, 35, 99,
      99, 99,

      // Start of 0x1e020:
      99, 99, 35, 99, 99, 35, 99, 99, 99, 99, 99, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
      35, 35, 35, 35,
      // done
    };




} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_GC_TABLES_HPP
