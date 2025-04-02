
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_joiners_tables.mjs
 *   Unicode UCD Database Creation Date: 2024-08-25
 *   This file's generation date:        Wed, 02 Apr 2025 02:25:20 GMT
 *   Unicode Version:                    16.0.0
 *   Total Table sizes in this file:
 *       - in bits:       18704
 *       - in bytes:      2338 B
 *       - in KibiBytes:  2.28 KiB
 *   Some other implementations' total table size was 1.55 KiB;
 *   So I have wasted 0.73 KiB.
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
 *
 * Attention:
 *   These 'Joining_Type's are excluded from these tables and have been
 *   replaced with 'Non_Joining':
 *       Transparent, Join_Causing
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
    static constexpr auto trailing_zero_joiners = 0x1E950UL;

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
        explicit(false) consteval joiners_index(std::uint16_t const value) noexcept
          : pos{static_cast<std::uint16_t>(value)} {}

        [[nodiscard]] constexpr std::uint16_t value() const noexcept {
            return static_cast<std::uint16_t>(pos);
        }

        static constexpr std::uint16_t chunk_mask  = 0xFU;
        static constexpr std::size_t   chunk_size  = 16U;
        static constexpr std::uint8_t  chunk_shift = 4U;

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

    struct alignas(std::uint64_t) joiners_breakpoint_type {
        std::uint16_t starting;
        std::uint16_t ending;
        std::uint16_t offset;
    };

    /**
     * You can choose between the indices' table using these breakpoints:
     *
     * Table size in KibiBytes:  1.34 KiB
     */
    static constexpr std::array<joiners_breakpoint_type, 171U> joiners_breakpoints{
      joiners_breakpoint_type{   .starting = 97,   .ending = 141,    .offset = 97}, // Section 1
      {  .starting = 383,   .ending = 395,   .offset = 339}, // Section 2
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      { .starting = 2691,  .ending = 2696,  .offset = 2635}, // Section 3
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      { .starting = 4267,  .ending = 4349,  .offset = 4206}, // Section 4
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      { .starting = 7823,  .ending = 7829,  .offset = 7680}, // Section 5
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {    .starting = 0,     .ending = 0,     .offset = 0}, // Section Invalid
      {.starting = 43529, .ending = 43530, .offset = 43380}, // Section 6
    };

    static constexpr joiners_index joiners_common_position{0U};
    static constexpr std::uint16_t joiners_breakpoint_shift{8U};


    /**
     * JOINERS Index Table (combined 6 sections)
     *
     * Joiners: Joining_Type
     *
     * Each value contains 1 numbers hidden inside:
     *     [16bits = pos]
     *
     * Table size:
     *   - in bits:       2400
     *   - in bytes:      300 B
     *   - in KibiBytes:  0.29 KiB
     */
    static constexpr std::array<joiners_index, 150ULL> joiners_indices{

      // Section [97, 141) size containing 44 values:
      //   - in bits:       704
      //   - in bytes:      88 B
      //   - in KibiBytes:  0.09 KiB
      0,
      16,
      31,
      47,
      0,
      58,
      74,
      82,
      90,
      100,
      100,
      116,
      132,
      138,
      154,
      0,
      170,
      185,
      0,
      201,
      215,
      227,
      242,
      0,
      0,
      0,
      0,
      258,
      100,
      268,
      0,
      0,
      0,
      0,
      0,
      284,
      296,
      312,
      328,
      341,
      0,
      357,
      372,
      382,

      // Section [383, 395) size containing 12 values:
      //   - in bits:       192
      //   - in bytes:      24 B
      //   - in KibiBytes:  0.02 KiB
      0,
      391,
      0,
      100,
      100,
      100,
      100,
      100,
      382,
      261,
      100,
      407,

      // Section [2691, 2696) size containing 5 values:
      //   - in bits:       80
      //   - in bytes:      10 B
      //   - in KibiBytes:  0.01 KiB
      0,
      100,
      100,
      100,
      423,

      // Section [4267, 4349) size containing 82 values:
      //   - in bits:       1312
      //   - in bytes:      164 B
      //   - in KibiBytes:  0.16 KiB
      0,
      439,
      453,
      468,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      482,
      498,
      505,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      521,
      100,
      535,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      549,
      0,
      0,
      0,
      0,
      0,
      0,
      565,
      576,
      591,
      0,
      607,
      56,
      0,
      0,
      622,
      638,

      // Section [7823, 7829) size containing 6 values:
      //   - in bits:       96
      //   - in bytes:      12 B
      //   - in KibiBytes:  0.01 KiB
      0,
      100,
      100,
      100,
      100,
      654,

      // Section [43529, 43530) size containing 1 values:
      //   - in bits:       16
      //   - in bytes:      2 B
      //   - in KibiBytes:  0.00 KiB
      0,

    };




    /**
     * JOINERS Values Table
     *
     * Joiners: Joining_Type
     *
     * Table size:
     *   - in bits:       5360
     *   - in bytes:      670 B
     *   - in KibiBytes:  0.65 KiB
     */
    static constexpr std::array<std::uint8_t, 670ULL> joiners_values{

      // Start of 0x0-0x610, 0x650, 0x700, 0x730, 0x780-0x7b0, 0x7f0-0x830, 0x890, 0x8d0-0x17f0, 0x1810,
      // 0x18b0-0xa830, 0xa880-0x10ab0, 0x10af0-0x10b70, 0x10bb0-0x10cf0, 0x10d30-0x10eb0, 0x10ed0-0x10f20,
      // 0x10f60, 0x10f90-0x10fa0, 0x10fd0-0x1e8f0:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x620:
      3, 0, 1, 1, 1, 1, 3, 1, 3, 1, 3, 3, 3, 3, 3,

      // Start of 0x630:
      1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x640:
      0, 3, 3, 3, 3, 3, 3, 3, 1,

      // Start of 0x10f80:
      3, 3,

      // Start of 0x660:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3,

      // Start of 0x670:
      0, 1, 1, 1, 0, 1, 1, 1,

      // Start of 0x680:
      3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x690:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x6a0-0x6b0, 0x7d0, 0x1820-0x1860, 0x1890, 0xa840-0xa860, 0x10d10, 0x1e900-0x1e930:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x6c0:
      1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1,

      // Start of 0x6d0:
      3, 3, 1, 1, 0, 1,

      // Start of 0x6e0:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,

      // Start of 0x6f0:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 3,

      // Start of 0x710:
      1, 0, 3, 3, 3, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1,

      // Start of 0x720:
      3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 1, 3, 1, 3, 3, 1,

      // Start of 0x740:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,

      // Start of 0x750:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1,

      // Start of 0x760:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 3, 3,

      // Start of 0x770:
      3, 1, 3, 1, 1, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3,

      // Start of 0x7c0:
      0, 0, 0,

      // Start of 0x1880:
      0, 0, 0, 0, 0, 0, 0,

      // Start of 0x7e0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0,

      // Start of 0x840:
      1, 3, 3, 3, 3, 3, 1, 1, 3, 1, 3, 3,

      // Start of 0x850:
      3, 3, 3, 3, 1, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x860:
      3, 0, 3, 3, 3, 3, 0, 1, 3, 1, 1, 0, 0, 0, 0, 0,

      // Start of 0x870:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x880:
      1, 1, 1, 0, 0, 0, 3, 0, 0, 3, 3, 3, 3, 3, 1, 0,

      // Start of 0x8a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 0, 1,

      // Start of 0x8b0:
      3, 1, 1, 3, 3, 3, 3, 3, 3, 1,

      // Start of 0x8c0, 0x1870:
      3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1800:
      0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x18a0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 0, 0, 0, 0,

      // Start of 0xa870:
      3, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x10ac0:
      3, 3, 3, 3, 3, 1, 0, 1, 0, 1, 1, 0, 0, 2,

      // Start of 0x10ad0:
      1, 1, 1, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 1, 3,

      // Start of 0x10ae0:
      3, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3,

      // Start of 0x10b80:
      3, 1, 3, 1, 1, 1, 3, 3, 3, 1, 3, 3, 1, 3, 1, 1,

      // Start of 0x10b90:
      3, 1, 0, 0, 0, 0, 0,

      // Start of 0x10ba0:
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 3, 0,

      // Start of 0x10d00:
      2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10d20:
      3, 3, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x10ec0:
      0, 0, 1, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x10f30:
      3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10f40:
      3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x10f50:
      0, 3, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      // Start of 0x10f70:
      3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x10fb0:
      3, 0, 3, 3, 1, 1, 1, 0, 3, 1, 1, 3, 3, 1, 3, 3,

      // Start of 0x10fc0:
      0, 3, 1, 1, 3, 0, 0, 0, 0, 1, 3, 2, 0, 0, 0, 0,

      // Start of 0x1e940:
      3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};





} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_JOINERS_TABLES_HPP
