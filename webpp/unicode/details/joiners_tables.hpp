
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_joiners_tables.mjs
 *   Unicode UCD Database Creation Date: 2024-08-25
 *   This file's generation date:        Wed, 23 Apr 2025 20:08:55 GMT
 *   Unicode Version:                    16.0.0
 *   Total Table sizes in this file:
 *       - in bits:       93928
 *       - in bytes:      11741 B
 *       - in KibiBytes:  11.47 KiB
 *   Some other implementations' total table size was 1.55 KiB;
 *   So I have wasted 9.92 KiB.
 *   Some other implementations use binary search, which is not be the fastest solution.
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 *   Arabic Shaping code point lists:
 *       https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedJoiningType.txt
 *   UCD README file (used to check the version and creation date):
 *       https://www.unicode.org/Public/UCD/latest/ucd/ReadMe.txt
 *
 */

#ifndef WEBPP_UNICODE_JOINERS_TABLES_HPP
#define WEBPP_UNICODE_JOINERS_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {


    /**
     * In "joiners_index" table, any code point bigger than this number will have "Non_Joining" as its Joiners
     * value; so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_joiners = 0xE0200UL;

    /**
     * Joiners (Index Table)
     * Joining_Type
     */
    struct alignas(std::uint16_t) joiners_index {
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
        explicit(false) consteval joiners_index(std::uint16_t const value) noexcept :
          pos{static_cast<std::uint16_t>(value)} {}

        [[nodiscard]] constexpr std::uint16_t value() const noexcept {
            return static_cast<std::uint16_t>(static_cast<std::uint16_t>(pos));
        }

        static constexpr std::uint16_t chunk_mask  = 0xFU;
        static constexpr std::size_t   chunk_size  = 16U;
        static constexpr std::uint8_t  chunk_shift = 4U;

        /**
         * Get the final position of the second table.
         * This does not apply the shift or get the value of the second table for you; this only applies that
         * mask.
         */
        [[nodiscard]] constexpr std::uint16_t get_position(auto const request_position) const noexcept {
            auto const remaining_pos = static_cast<std::uint16_t>(request_position & chunk_mask);
            return pos + remaining_pos;
        }
    };

    struct joiners_breakpoint_type {
        std::uint16_t starting;
        std::uint16_t ending;
        std::uint16_t offset;
    };

    /**
     * You can choose between the indices' table using these breakpoints:
     *
     * Table size in KibiBytes:  5.30 KiB
     */
    static constexpr std::array<joiners_breakpoint_type, 113U> joiners_breakpoints{
      joiners_breakpoint_type{    .starting = 0,   .ending = 778,     .offset = 0}, // Section 1
      {    .starting = 0,   .ending = 778,     .offset = 0}, // Section 1
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 2661,  .ending = 2751,  .offset = 1883}, // Section 2
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 4016,  .ending = 4598,  .offset = 3148}, // Section 3
      { .starting = 4016,  .ending = 4598,  .offset = 3148}, // Section 3
      { .starting = 4930,  .ending = 4934,  .offset = 3480}, // Section 4
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 5648,  .ending = 5887,  .offset = 4194}, // Section 5
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Optimized Away
      { .starting = 7112,  .ending = 7115,  .offset = 5419}, // Section 6
      { .starting = 7407,  .ending = 7829,  .offset = 5711}, // Section 7
      { .starting = 7407,  .ending = 7829,  .offset = 5711}, // Section 7
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
      {.starting = 57343, .ending = 57375, .offset = 55225}, // Section 8
      {.starting = 57343, .ending = 57375, .offset = 55225}, // Section 8
    };

    static constexpr std::uint16_t joiners_last_breakpoint{0xE01FU};
    static constexpr std::uint16_t joiners_breakpoint_shift{9U};
    static constexpr joiners_index joiners_common_pos{0U}; // this is the last common value position


    /**
     * joiners Index Table (combined 8 sections)
     *
     * Joiners: Joining_Type
     *
     * Each value is a [16bits = pos]
     *
     * Table size:
     *   - in bits:       34400
     *   - in bytes:      4300 B
     *   - in KibiBytes:  4.20 KiB
     */
    static constexpr std::array<joiners_index, 2150ULL> joiners_indices{
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 19, 19, 19, 19, 19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 19, 51, 65, 0, 0, 0, 0, 81, 97,
      112, 128, 19, 144, 160, 168, 176, 186, 186, 202, 218, 226, 242, 1, 258, 273, 19, 289, 303, 315, 330, 0,
      0, 45, 346, 352, 186, 362, 374, 0, 388, 400, 0, 416, 428, 444, 460, 473, 74, 489, 504, 514, 19, 528, 19,
      141, 0, 0, 544, 559, 574, 588, 0, 603, 0, 0, 592, 618, 0, 588, 2, 633, 0, 0, 592, 648, 603, 0, 660, 633,
      0, 0, 592, 675, 0, 588, 689, 603, 0, 0, 4, 618, 585, 588, 0, 602, 0, 0, 0, 591, 0, 0, 0, 704, 0, 0, 709,
      724, 585, 588, 0, 603, 0, 0, 4, 738, 0, 588, 0, 142, 0, 0, 638, 618, 0, 588, 0, 603, 0, 0, 0, 594, 752,
      0, 0, 0, 0, 0, 571, 761, 0, 0, 0, 0, 0, 0, 776, 789, 0, 0, 0, 0, 582, 0, 804, 0, 0, 0, 50, 820, 833, 19,
      22, 598, 0, 0, 0, 0, 0, 606, 848, 0, 641, 346, 863, 877, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 606, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 891, 0, 588, 0, 588, 0, 588, 0, 0, 0, 903, 10,
      913, 0, 0, 927, 0, 186, 186, 186, 186, 186, 943, 954, 186, 961, 0, 0, 0, 0, 0, 0, 0, 977, 991, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1003, 0, 0, 0, 1015, 1029, 1042, 0, 0, 0, 19, 20, 0, 0, 0, 140, 0, 0,
      1058, 602, 0, 1071, 140, 142, 0, 1087, 0, 0, 0, 1101, 142, 0, 0, 607, 1116, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      527, 1132, 1141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 19, 19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1151, 0, 666, 0, 0, 0, 1165, 0, 0, 0, 0, 0,
      0, 19, 19, 346, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 142, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
      0, 0, 0, 19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 609, 0, 0, 0, 0, 0, 0, 1181,
      // End of Section #1 [0, 778) containing 778 values (1.52 KiB). Skipping 1883 values.

      0, 1, 1197, 0, 605, 0, 0, 0, 0, 0, 142, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1211, 0, 1223,
      0, 186, 186, 186, 1239, 0, 0, 0, 0, 586, 0, 19, 634, 0, 0, 1249, 0, 74, 142, 0, 0, 141, 0, 0, 1263, 0,
      0, 599, 0, 0, 0, 690, 1274, 1287, 0, 0, 592, 0, 0, 0, 1303, 603, 0, 637, 598, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1319,
      // End of Section #2 [2661, 2751) containing 90 values (0.18 KiB). Skipping 1265 values.

      0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1333, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 346, 0, 0, 0, 0, 0, 0, 0, 0, 1345,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1360, 0, 0, 1376, 0, 0, 0, 0, 0, 0, 0, 0, 1392, 1406, 1421, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1435, 1451, 1458,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1474, 186, 1488, 0, 0, 0, 1073, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 638, 0, 1502, 0, 0, 607, 0, 0, 0, 1518, 1529, 1544, 0,
      1560, 1574, 0, 0, 1590, 1606, 0, 0, 0, 603, 0, 0, 73, 537, 0, 0, 1622, 142, 0, 0, 1634, 602, 0, 0, 0,
      141, 0, 1645, 139, 0, 0, 0, 601, 142, 0, 0, 1661, 1628, 0, 0, 0, 0, 0, 1, 1674, 603, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 1689, 0, 142, 0, 0, 638, 346, 0, 791, 139, 0, 0, 0, 1071, 1699, 1713, 633, 0, 0, 0, 0, 73, 752,
      2, 0, 0, 0, 0, 0, 1726, 1741, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1755, 346, 637, 0, 0, 0, 0, 0,
      1771, 346, 0, 0, 0, 0, 0, 1702, 1786, 0, 0, 0, 0, 0, 1700, 1800, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 1816, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1827, 601, 0, 0, 0, 0, 0, 0, 0, 0, 1633,
      346, 0, 1842, 0, 0, 1855, 597, 1870, 0, 0, 689, 1817, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 842, 0, 0, 0, 0, 0, 17, 1886, 1901, 0, 0, 0, 0, 0, 0, 0, 1916, 1786, 0, 0, 0, 0,
      1931, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 587, 142, 0, 0, 1345, 1713, 594,
      // End of Section #3 [4016, 4598) containing 582 values (1.14 KiB). Skipping 332 values.

      0, 19, 44, 538,
      // End of Section #4 [4930, 4934) containing 4 values (0.01 KiB). Skipping 714 values.

      0, 605, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 139, 0, 0, 0, 537, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 141, 0, 0, 0, 0, 600,
      // End of Section #5 [5648, 5887) containing 239 values (0.47 KiB). Skipping 1225 values.

      0, 636, 140,
      // End of Section #6 [7112, 7115) containing 3 values (0.01 KiB). Skipping 292 values.

      0, 19, 19, 21, 19, 537, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 1335, 48, 1947, 0, 609, 0, 0, 0, 0, 0, 0, 0, 0, 0, 891, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 19, 19, 19, 1952, 19, 19, 22, 599, 600, 1071, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1963, 1971, 1985, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 537, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 607, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 607, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      605, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 537, 0, 0, 186, 186, 186, 186, 2001,
      // End of Section #7 [7407, 7829) containing 422 values (0.82 KiB). Skipping 49514 values.

      0, 603, 0, 19, 19, 19, 19, 19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
      19, 19, 19, 19,
      // End of Section #8 [57343, 57375) containing 32 values (0.06 KiB).

      // done
    };




    /**
     * JOINERS Values Table
     *
     * Joiners: Joining_Type Values
     *
     * Table size:
     *   - in bits:       16136
     *   - in bytes:      2017 B
     *   - in KibiBytes:  1.97 KiB
     */
    static constexpr std::array<std::uint8_t, 2017ULL> joiners_values{

      // Start of 0x0-0x90, 0xb0-0x2f0, 0x370-0x470, 0x490-0x580, 0x5d0-0x600, 0x780-0x790, 0x800, 0x830,
      // 0x910-0x920, 0x970, 0x990-0x9a0, 0x9d0, 0xa10-0xa20, 0xa60, 0xa90-0xaa0, 0xad0, 0xb10-0xb20, 0xb70,
      // 0xb90-0xbb0, 0xbd0-0xbf0, 0xc10-0xc20, 0xc70, 0xc90-0xca0, 0xcd0, 0xcf0, 0xd10-0xd20, 0xd50, 0xd70,
      // 0xd90-0xdb0, 0xde0-0xe20, 0xe50-0xea0, 0xed0-0xf00, 0xf20, 0xf40-0xf60, 0xfd0-0x1010, 0x1040,
      // 0x10a0-0x1340, 0x1360-0x1700, 0x1720, 0x1740, 0x1760, 0x1780-0x17a0, 0x17e0-0x17f0, 0x1810,
      // 0x18b0-0x1910, 0x1940-0x1a00, 0x1a20-0x1a40, 0x1a80-0x1aa0, 0x1ad0-0x1af0, 0x1b10-0x1b20, 0x1b50,
      // 0x1b90, 0x1bb0-0x1bd0, 0x1c00-0x1c10, 0x1c40-0x1cc0, 0x1d00-0x1db0, 0x1e00-0x1ff0, 0x2010,
      // 0x2030-0x2050, 0x2070-0x20c0, 0x2100-0x2cd0, 0x2d00-0x2d60, 0x2d80-0x2dd0, 0x2e00-0x3010,
      // 0x3030-0x3080, 0x30a0-0xa650, 0xa680, 0xa6a0-0xa6e0, 0xa700-0xa7f0, 0xa810, 0xa830, 0xa880-0xa8b0,
      // 0xa8d0, 0xa900-0xa910, 0xa930, 0xa960-0xa970, 0xa990-0xa9a0, 0xa9c0-0xa9d0, 0xa9f0-0xaa10,
      // 0xaa50-0xaa60, 0xaa80-0xaaa0, 0xaad0, 0xab00-0xabd0, 0xabf0-0xfb00, 0xfb20-0xfdf0, 0xfe10,
      // 0xfe30-0xfee0, 0xff00-0xffe0, 0x10000-0x101e0, 0x10200-0x102d0, 0x102f0-0x10360, 0x10380-0x109f0,
      // 0x10a10-0x10a20, 0x10a40-0x10ab0, 0x10af0-0x10b70, 0x10bb0-0x10cf0, 0x10d30-0x10d50, 0x10d70-0x10e90,
      // 0x10eb0, 0x10ed0-0x10ee0, 0x10f00-0x10f20, 0x10f60, 0x10f90-0x10fa0, 0x10fd0-0x10ff0,
      // 0x11010-0x11020, 0x11050-0x11060, 0x11090-0x110a0, 0x110d0-0x110f0, 0x11110, 0x11140-0x11160,
      // 0x11190-0x111a0, 0x111d0-0x11210, 0x11250-0x112c0, 0x112f0, 0x11310-0x11320, 0x11350,
      // 0x11380-0x113a0, 0x113f0-0x11420, 0x11460-0x114a0, 0x114d0-0x115a0, 0x115e0-0x11620, 0x11650-0x11690,
      // 0x116c0-0x11700, 0x11730-0x11810, 0x11840-0x11920, 0x11950-0x119c0, 0x119f0, 0x11a10-0x11a20,
      // 0x11a60-0x11a70, 0x11aa0-0x11c20, 0x11c40-0x11c80, 0x11cc0-0x11d20, 0x11d50-0x11d80, 0x11da0-0x11ee0,
      // 0x11f10-0x11f20, 0x11f60-0x13420, 0x13460-0x16100, 0x16130-0x16ae0, 0x16b00-0x16b20, 0x16b40-0x16f30,
      // 0x16f50-0x16f70, 0x16fa0-0x16fd0, 0x16ff0-0x1bc80, 0x1bcb0-0x1cef0, 0x1cf50-0x1d150, 0x1d190,
      // 0x1d1b0-0x1d230, 0x1d250-0x1d9f0, 0x1dab0-0x1dff0, 0x1e030-0x1e070, 0x1e090-0x1e120, 0x1e140-0x1e290,
      // 0x1e2b0-0x1e2d0, 0x1e2f0-0x1e4d0, 0x1e4f0-0x1e5d0, 0x1e5f0-0x1e8c0, 0x1e8e0-0x1e8f0, 0x1e950-0xdfff0,
      // 0xe0010, 0xe0080-0xe00f0, 0xe01f0:
      0,

      // Start of 0x700, 0x2ce0, 0x2d70, 0xa660, 0xfef0, 0x11220, 0x112d0, 0x11820, 0x16f40, 0x16f80, 0x1e080:
      0,

      // Start of 0x9f0, 0xfb10, 0x11450, 0x1e2a0:
      0,

      // Start of 0xa0, 0x1090, 0x101f0:
      0,

      // Start of 0xb30, 0xcb0:
      0, 0, 0, 0, 0, 0,

      // Start of 0x17c0:
      0, 0, 0, 0, 0, 0, 5,

      // Start of 0x11c90:
      0,

      // Start of 0x590, 0x1daa0:
      0,

      // Start of 0x300-0x360, 0x5a0, 0x650, 0x730, 0x8d0, 0x8f0, 0xfa0, 0x1ab0, 0x1dc0-0x1df0, 0x20d0-0x20e0,
      // 0x2de0-0x2df0, 0xa8e0, 0xfe00, 0xfe20, 0x13430, 0x1cf00-0x1cf10, 0x1cf30, 0x1da00-0x1da20,
      // 0x1da40-0x1da50, 0xe0020-0xe0070, 0xe0100-0xe01e0:
      5,

      // Start of 0x1ac0:
      5,

      // Start of 0x1cf20:
      5,

      // Start of 0xfb0, 0x1da60:
      5, 5, 5,

      // Start of 0x16120:
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0x480:
      0, 0, 0, 5, 5, 5, 5, 5, 5,

      // Start of 0x13440:
      5,

      // Start of 0x7a0:
      0, 0, 0,

      // Start of 0x1d170:
      0, 0,

      // Start of 0xf70:
      0,

      // Start of 0x5b0:
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0x5c0:
      0, 5, 5, 0, 5, 5, 0, 5,

      // Start of 0x11030, 0x11430:
      0,

      // Start of 0x890, 0xa940:
      0, 0, 0, 0, 0, 0, 0,

      // Start of 0x610:
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 5, 0, 0, 0,

      // Start of 0x620:
      3, 0, 1, 1, 1, 1, 3, 1, 3, 1, 3, 3, 3, 3, 3,

      // Start of 0x630:
      1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x640:
      4, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3,

      // Start of 0x11130, 0x11370, 0x16af0:
      5,

      // Start of 0x1b00, 0x1b70, 0x1bca0:
      5,

      // Start of 0x900, 0xa980, 0x11100, 0x16f90:
      5,

      // Start of 0xd00, 0x1b80, 0x1bf0, 0x2cf0, 0xa6f0, 0xa950, 0x11080, 0x11180, 0x11300, 0x11f00:
      5, 5,

      // Start of 0x660:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3,

      // Start of 0x670:
      5, 1, 1, 1, 0, 1, 1, 1,

      // Start of 0x680:
      3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x690:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x6a0-0x6b0, 0x7d0, 0x1820-0x1860, 0x1890, 0xa840-0xa860, 0x10d10, 0x1e900-0x1e930:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x6c0:
      1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1,

      // Start of 0x6d0:
      3, 3, 1, 1, 0, 1, 5, 5,

      // Start of 0x6e0:
      5, 5, 5, 5, 5, 0, 0, 5, 5, 0, 5, 5, 5, 5, 1, 1,

      // Start of 0x6f0:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 3,

      // Start of 0x710:
      1, 5, 3, 3, 3, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1,

      // Start of 0x720:
      3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 1, 3, 1, 3, 3, 1,

      // Start of 0x740:
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 1,

      // Start of 0x750:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1,

      // Start of 0x760:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 3, 3,

      // Start of 0x770:
      3, 1, 3, 1, 1, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3,

      // Start of 0x7b0, 0x1060, 0x20f0, 0x102e0, 0x11340, 0x115c0, 0x11640, 0x119e0:
      5, 0, 0, 0, 0, 0,

      // Start of 0x7c0:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x7e0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5,

      // Start of 0x7f0:
      5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 4, 0, 0, 5,

      // Start of 0x810:
      0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5,

      // Start of 0x820:
      5, 5, 5, 5, 0, 5, 5, 5, 0, 5, 5, 5, 5, 5, 0, 0,

      // Start of 0x840:
      1, 3, 3, 3, 3, 3, 1, 1, 3, 1, 3, 3,

      // Start of 0x850:
      3, 3, 3, 3, 1, 3, 1, 1, 1, 5, 5, 5, 0, 0, 0, 0,

      // Start of 0x860:
      3, 0, 3, 3, 3, 3, 0, 1, 3, 1, 1, 0, 0, 0, 0, 0,

      // Start of 0x870:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x880:
      1, 1, 1, 4, 4, 4, 3, 0, 0, 3, 3, 3, 3, 3, 1, 0,

      // Start of 0x8a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 0, 1,

      // Start of 0x8b0:
      3, 1, 1, 3, 3, 3, 3, 3, 3, 1,

      // Start of 0x8c0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 5, 5, 5,

      // Start of 0x1cd0:
      5,

      // Start of 0x8e0:
      5, 5, 0, 5, 5, 5, 5, 5, 5,

      // Start of 0x11040, 0x16b30, 0x1cf40, 0x1e130, 0x1e8d0:
      5,

      // Start of 0x13450:
      5, 5, 5, 5, 5, 5,

      // Start of 0x930:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5, 0, 0,

      // Start of 0x940:
      0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0,

      // Start of 0xe30:
      0, 5, 0,

      // Start of 0x950:
      0, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0xf10:
      0, 0, 0,

      // Start of 0xb50, 0xc50:
      0,

      // Start of 0xa8c0:
      0,

      // Start of 0x11ef0:
      0,

      // Start of 0x960, 0x9e0, 0xae0, 0xb60, 0xc60, 0xce0, 0xd60, 0x1730, 0x1750, 0x1770:
      0, 0, 5,

      // Start of 0xbc0:
      5,

      // Start of 0x9b0, 0xa30, 0xab0, 0xaa70:
      0, 0,

      // Start of 0xdc0, 0x11f50:
      0, 0, 0,

      // Start of 0x11a40:
      0,

      // Start of 0xfc0, 0xaaf0:
      0,

      // Start of 0xa9e0, 0x1da70:
      0,

      // Start of 0x16fe0, 0x1da80:
      0,

      // Start of 0x11170, 0x11940:
      0,

      // Start of 0xb80, 0x1b40, 0x110c0:
      0,

      // Start of 0x980, 0xa50, 0xb00, 0xc80, 0xd80, 0xaac0, 0x11000, 0x11240, 0xe0000:
      0, 5,

      // Start of 0xa690, 0x16110, 0x1e5e0:
      0,

      // Start of 0x1020, 0x1350:
      0,

      // Start of 0x1c20, 0x10ef0, 0x1e2e0, 0x1e4e0:
      0, 0,

      // Start of 0x3020, 0x1d1a0:
      0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x9c0, 0xb40, 0xd40:
      0, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0,

      // Start of 0xa00, 0xa80, 0x113e0:
      0,

      // Start of 0xa8f0:
      5, 5,

      // Start of 0x1bc90:
      0,

      // Start of 0xaae0, 0x115d0:
      0,

      // Start of 0xd30, 0x10ea0, 0x11330:
      0, 0, 0,

      // Start of 0x1050:
      0, 0, 0, 0, 0, 0, 0,

      // Start of 0xa40:
      0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5,

      // Start of 0xa70:
      5, 5, 0, 0, 0, 5,

      // Start of 0x2020:
      0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0xac0:
      0, 5, 5, 5, 5, 5, 0, 5, 5, 0, 0, 0, 0, 5,

      // Start of 0xaf0, 0x11a80:
      0,

      // Start of 0xaa20:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5,

      // Start of 0xc00:
      5, 0, 0, 0, 5,

      // Start of 0xc30:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5,

      // Start of 0xc40:
      5, 0, 0, 0, 0, 0, 5, 5, 5, 0, 5, 5, 5, 5,

      // Start of 0xcc0:
      0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 5, 5,

      // Start of 0xdd0, 0x11440:
      0, 0, 5, 5, 5, 0, 5, 0, 0,

      // Start of 0xe40:
      0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0xeb0:
      0, 5, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0xec0:
      0, 0,

      // Start of 0x11360:
      0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0xf30:
      0, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 0, 0, 0, 0, 0,

      // Start of 0xf80:
      5, 5, 5, 5, 5, 0, 5, 5, 0, 0, 0, 0, 0,

      // Start of 0xf90:
      5, 5, 5, 5, 5, 5, 5, 5, 0,

      // Start of 0x11c30:
      5, 5, 5, 5, 5, 5,

      // Start of 0x1030:
      5, 0, 5, 5, 5, 5, 5, 5, 0, 5, 5, 0, 0, 5, 5,

      // Start of 0x1070:
      0, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x1080:
      0, 0, 5, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5,

      // Start of 0x1710, 0x1d240:
      0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x17b0:
      0, 0, 0, 0, 5, 5, 0, 5, 5, 5,

      // Start of 0x17d0:
      5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,

      // Start of 0x1800:
      0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 4, 5, 5, 5, 0, 5,

      // Start of 0x1870:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,

      // Start of 0x1880:
      0, 0, 0, 0, 0, 5, 5,

      // Start of 0x18a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 3, 0, 0, 0, 0, 0,

      // Start of 0x1920:
      5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0,

      // Start of 0x1930:
      0, 0, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5,

      // Start of 0x1a10:
      0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 5,

      // Start of 0x1a50:
      0, 0, 0, 0, 0, 0, 5, 0, 5, 5, 5, 5, 5, 5,

      // Start of 0x1a60:
      5, 0, 5, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0x1a70:
      0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 5,

      // Start of 0x1b30:
      0, 0, 0, 0, 5, 0, 5, 5, 5, 5, 5, 0, 5,

      // Start of 0x1b60, 0x113b0, 0x1da90:
      0, 0,

      // Start of 0x10d60:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5,

      // Start of 0x1ba0:
      0, 0, 5, 5, 5, 5, 0, 0, 5, 5, 0, 5, 5, 5,

      // Start of 0x1be0:
      0, 0, 0, 0, 0, 0, 5, 0, 5, 5, 0, 0, 0, 5, 0,

      // Start of 0x1c30:
      5, 5, 5, 5, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x1ce0:
      5, 0, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0x1cf0:
      0, 0, 0, 0, 5, 0, 0, 0, 5, 5,

      // Start of 0x2000:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 4,

      // Start of 0x2060:
      5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5,

      // Start of 0x3090:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0,

      // Start of 0xa670:
      5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0xa800:
      0, 0, 5, 0, 0, 0, 5, 0, 0, 0, 0, 5,

      // Start of 0xa820:
      0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 5, 0, 0, 0,

      // Start of 0xa870:
      3, 3, 2, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0xa920:
      0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0xa9b0:
      0, 0, 0, 5, 0, 0, 5, 5, 5, 5, 0,

      // Start of 0xaa30:
      0, 5, 5, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0,

      // Start of 0xaa40:
      0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0,

      // Start of 0xaab0:
      5, 0, 5, 5, 5, 0, 0, 5, 5, 0, 0, 0, 0, 0, 5, 5,

      // Start of 0xabe0:
      0, 0, 0, 0, 0, 5, 0, 0, 5, 0, 0, 0, 0, 5,

      // Start of 0xfff0:
      0, 0,

      // Start of 0x1d160:
      0, 0, 0, 0, 0, 0, 0, 5, 5, 5,

      // Start of 0x10370, 0x11f30:
      0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0,

      // Start of 0x10a00:
      0, 5, 5, 5, 0, 5, 5, 0, 0, 0, 0, 0, 5, 5, 5, 5,

      // Start of 0x10a30:
      0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 5,

      // Start of 0x10ac0:
      3, 3, 3, 3, 3, 1, 0, 1, 0, 1, 1, 0, 0, 2,

      // Start of 0x10ad0:
      1, 1, 1, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 1, 3,

      // Start of 0x10ae0:
      3, 1, 0, 0, 1, 5, 5, 0, 0, 0, 0, 3, 3, 3,

      // Start of 0x10b80:
      3, 1, 3, 1, 1, 1, 3, 3, 3, 1, 3, 3, 1, 3, 1, 1,

      // Start of 0x10b90:
      3, 1, 0, 0, 0, 0, 0,

      // Start of 0x10ba0:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 3, 0,

      // Start of 0x10d00:
      2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10d20:
      3, 3, 1, 3, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0,

      // Start of 0x10ec0:
      0, 0, 1, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x10f30:
      3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10f40:
      3, 3, 3, 3, 3, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0x10f50:
      5, 3, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x10f70:
      3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10f80:
      3, 3, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x10fb0:
      3, 0, 3, 3, 1, 1, 1, 0, 3, 1, 1, 3, 3, 1, 3, 3,

      // Start of 0x10fc0:
      0, 3, 1, 1, 3, 0, 0, 0, 0, 1, 3, 2, 0, 0, 0, 0,

      // Start of 0x11070:
      5, 0, 0, 5, 5, 0,

      // Start of 0x111c0:
      0, 0, 0, 0, 0,

      // Start of 0x119d0:
      0,

      // Start of 0x110b0:
      0, 0, 0, 5, 5, 5, 5, 0, 0, 5, 5,

      // Start of 0x11120:
      0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 5, 5, 5,

      // Start of 0x111b0:
      0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0x11230:
      5, 5, 0, 0, 5, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5,

      // Start of 0x112e0:
      0, 0, 0, 5, 5, 5, 5, 5, 5, 5,

      // Start of 0x113c0:
      5,

      // Start of 0x11710:
      0, 0,

      // Start of 0x116a0:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x113d0, 0x11f40:
      5, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x114b0:
      0, 0, 0, 5, 5, 5, 5, 5, 5, 0, 5, 0, 0, 0, 0,

      // Start of 0x114c0:
      5, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x115b0:
      0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 5,

      // Start of 0x11630:
      0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 5, 0,

      // Start of 0x116b0, 0x11d40:
      5, 5, 5, 5, 5, 5, 0, 5, 0, 0, 0, 0, 0, 0,

      // Start of 0x11720:
      0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0,

      // Start of 0x11830:
      5,

      // Start of 0x11a90:
      5, 5, 5, 5, 5, 5, 5, 0, 5, 5,

      // Start of 0x11930:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 5,

      // Start of 0x11a00:
      0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0,

      // Start of 0x11a30:
      0, 0, 0, 5, 5, 5, 5, 5, 5, 0, 0, 5, 5, 5, 5,

      // Start of 0x11a50:
      0, 5, 5, 5, 5, 5, 5, 0, 0, 5, 5, 5, 0, 0, 0, 0,

      // Start of 0x11ca0:
      5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 5, 5, 5, 5, 5,

      // Start of 0x11cb0:
      5, 0, 5, 5, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x11d30:
      0, 5, 5, 5, 5, 5, 5, 0, 0, 0, 5, 0, 5, 5, 0,

      // Start of 0x11d90:
      5, 5, 0, 0, 0, 5, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x1d180:
      5, 5, 5, 0, 0,

      // Start of 0x1da30:
      5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0,

      // Start of 0x1e000:
      5, 5, 5, 5, 5, 5, 5, 0,

      // Start of 0x1e010:
      5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 5, 5, 5,

      // Start of 0x1e020:
      5, 5, 0, 5, 5, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0,

      // Start of 0x1e940:
      3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0,
      // done
    };





} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_JOINERS_TABLES_HPP
