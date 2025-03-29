
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_bidi_tables.mjs
 *   Unicode UCD Database Creation Date: 2024-08-25
 *   This file's generation date:        Sat, 29 Mar 2025 03:01:54 GMT
 *   Unicode Version:                    16.0.0
 *   Total Table sizes in this file:
 *       - in bits:       79904
 *       - in bytes:      9988 B
 *       - in KibiBytes:  9.75 KiB
 *   Some other implementations' total table size was 16.98 KiB;
 *   So I have saved 7.23 KiB.
 *   Some other implementations use binary search, which is not be the fastest solution.
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 *   Derived Bidi Classes from UCD Database Code Points:
 *       https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedBidiClass.txt
 *       https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt
 *   UCD README file (used to check the version and creation date):
 *       https://www.unicode.org/Public/UCD/latest/ucd/ReadMe.txt
 */

#ifndef WEBPP_UNICODE_BIDI_TABLES_HPP
#define WEBPP_UNICODE_BIDI_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {


    /**
     * In "bidi_index" table, any code point bigger than this number will have "None" as its Bidi value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_bidis = 0x110000UL;

    /**
     * Bidi (Index Table)
     * Bidirectional Class
     */
    struct alignas(std::uint16_t) bidi_index {
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
        explicit(false) consteval bidi_index(std::uint16_t const value) noexcept
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

    struct alignas(std::uint64_t) breakpoint_type {
        std::uint16_t starting;
        std::uint16_t ending;
        std::uint16_t offset;
    };

    /**
     * You can choose between the indices' table using these breakpoints:
     *
     * Table size in KibiBytes:  0.53 KiB
     */
    static constexpr std::array<breakpoint_type, 68U> breakpoints{
      {

       // Section 1:
        {.starting = 0, .ending = 416, .offset = 0},

       // Section 2:
        {.starting = 621, .ending = 624, .offset = 205},

       // Section 3:
        {.starting = 1315, .ending = 1376, .offset = 896},

       // Section 4:
        {.starting = 2007, .ending = 2467, .offset = 1527},

       // Section 5:
        {.starting = 2007, .ending = 2467, .offset = 1527},

       // Section 6:
        {.starting = 2823, .ending = 2944, .offset = 1883},

       // Section 7:
        {.starting = 3555, .ending = 4096, .offset = 2494},

       // Section 8:
        {.starting = 3555, .ending = 4096, .offset = 2494},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 9:
        {.starting = 6142, .ending = 6144, .offset = 4540},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 10:
        {.starting = 8190, .ending = 8192, .offset = 6586},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 11:
        {.starting = 10238, .ending = 10240, .offset = 8632},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 12:
        {.starting = 12286, .ending = 12288, .offset = 10678},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 13:
        {.starting = 14334, .ending = 14336, .offset = 12724},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 14:
        {.starting = 16382, .ending = 16384, .offset = 14770},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 15:
        {.starting = 18430, .ending = 18432, .offset = 16816},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 16:
        {.starting = 20478, .ending = 20480, .offset = 18862},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 17:
        {.starting = 22526, .ending = 22528, .offset = 20908},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 18:
        {.starting = 24574, .ending = 24576, .offset = 22954},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 19:
        {.starting = 26622, .ending = 26624, .offset = 25000},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 20:
        {.starting = 28670, .ending = 28800, .offset = 27046},

       // Section 21:
        {.starting = 28670, .ending = 28800, .offset = 27046},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 22:
        {.starting = 30718, .ending = 30720, .offset = 28964},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 23:
        {.starting = 32766, .ending = 32768, .offset = 31010},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section Invalid:
        {.starting = 0, .ending = 0, .offset = 0},

       // Section 24:
        {.starting = 34814, .ending = 34816, .offset = 33056}}
    };

    static constexpr bidi_index    common_position{213U};
    static constexpr std::uint16_t breakpoint_shift{9U};


    /**
     * BIDI Index Table (combined 21 sections)
     *
     * Bidi: Bidirectional Class
     *
     * Each value contains 1 numbers hidden inside:
     *     [16bits = pos]
     *
     * Table size:
     *   - in bits:       28160
     *   - in bytes:      3520 B
     *   - in KibiBytes:  3.44 KiB
     */
    static constexpr std::array<bidi_index, 1760ULL> bidi_indices{

      // Section [0, 416) size containing 416 values:
      //   - in bits:       6656
      //   - in bytes:      832 B
      //   - in KibiBytes:  0.81 KiB
      0,
      32,
      63,
      94,
      125,
      157,
      189,
      189,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      220,
      250,
      282,
      314,
      314,
      314,
      330,
      361,
      213,
      213,
      190,
      213,
      213,
      213,
      213,
      390,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      412,
      429,
      459,
      467,
      499,
      526,
      547,
      579,
      526,
      526,
      596,
      627,
      653,
      542,
      515,
      526,
      526,
      679,
      467,
      711,
      741,
      769,
      783,
      526,
      815,
      526,
      847,
      877,
      906,
      912,
      943,
      973,
      1004,
      977,
      1035,
      1065,
      1096,
      977,
      1127,
      1145,
      1096,
      977,
      1176,
      1206,
      1004,
      1238,
      1268,
      973,
      1003,
      213,
      1290,
      1304,
      1336,
      1341,
      1372,
      1391,
      1004,
      977,
      1422,
      973,
      1434,
      1101,
      1035,
      973,
      1004,
      213,
      1456,
      213,
      213,
      1479,
      1511,
      213,
      213,
      1526,
      385,
      213,
      1555,
      1581,
      213,
      1611,
      1637,
      317,
      999,
      213,
      213,
      1669,
      1104,
      1699,
      1729,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      1007,
      213,
      1759,
      213,
      213,
      213,
      212,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      1791,
      213,
      213,
      213,
      1820,
      1049,
      1049,
      1049,
      213,
      1841,
      1871,
      1759,
      1903,
      213,
      213,
      213,
      970,
      996,
      213,
      213,
      213,
      1935,
      1967,
      213,
      213,
      213,
      215,
      1999,
      2031,
      213,
      2059,
      2089,
      213,
      2121,
      894,
      213,
      905,
      2153,
      1003,
      2182,
      1434,
      2212,
      213,
      2238,
      213,
      2258,
      213,
      213,
      213,
      213,
      2282,
      2313,
      213,
      213,
      213,
      213,
      213,
      213,
      314,
      314,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      2339,
      2370,
      2386,
      2418,
      2442,
      2470,
      2502,
      2534,
      2566,
      2582,
      892,
      2614,
      2644,
      2676,
      213,
      2708,
      1999,
      1999,
      1999,
      2724,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      2009,
      213,
      2756,
      2783,
      1999,
      1999,
      1999,
      1999,
      2021,
      2020,
      1999,
      2807,
      213,
      213,
      1989,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      2839,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      2852,
      2874,
      1999,
      1999,
      1999,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      2906,
      213,
      213,
      213,
      2938,
      213,
      213,
      213,
      314,
      1999,
      1999,
      2001,
      213,
      2970,
      1999,
      1999,
      2011,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      2009,
      1983,
      3002,
      3033,
      213,
      213,
      3065,
      212,
      213,
      3094,
      213,
      213,
      213,
      213,
      213,
      213,
      1999,
      3126,
      216,
      213,
      1983,
      3142,
      213,
      1982,
      3173,
      213,
      213,
      213,
      213,
      3189,
      213,
      213,
      215,
      214,

      // Section [621, 624) size containing 3 values:
      //   - in bits:       48
      //   - in bytes:      6 B
      //   - in KibiBytes:  0.01 KiB
      213,
      1999,
      1999,

      // Section [1315, 1376) size containing 61 values:
      //   - in bits:       976
      //   - in bytes:      122 B
      //   - in KibiBytes:  0.12 KiB
      213,
      1983,
      1999,
      2749,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      3216,
      213,
      213,
      3233,
      1006,
      213,
      213,
      1051,
      1999,
      2936,
      213,
      213,
      204,
      213,
      213,
      213,
      3265,
      3292,
      213,
      3192,
      213,
      213,
      971,
      3324,
      213,
      3349,
      3374,
      213,
      906,
      3392,
      213,
      1000,
      213,
      3422,
      3451,
      977,
      213,
      3467,
      1004,
      3499,
      213,
      213,
      213,
      3522,
      213,
      213,
      213,
      3549,

      // Section [2007, 2467) size containing 460 values:
      //   - in bits:       7360
      //   - in bytes:      920 B
      //   - in KibiBytes:  0.90 KiB
      213,
      3563,
      3594,
      3610,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      3626,
      3656,
      526,
      526,
      526,
      3673,
      3690,
      3722,
      3754,
      3770,
      3802,
      526,
      526,
      526,
      3818,
      3850,
      63,
      63,
      2750,
      213,
      213,
      213,
      3882,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      211,
      213,
      1999,
      1999,
      3914,
      212,
      213,
      1437,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      3946,
      213,
      213,
      213,
      3974,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      4006,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      4038,
      4054,
      467,
      467,
      467,
      467,
      467,
      4086,
      467,
      4093,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      526,
      4125,
      4157,
      4180,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      4212,
      467,
      4243,
      526,
      530,
      467,
      3610,
      679,
      4275,
      4305,
      467,
      467,
      467,
      1004,
      4337,
      4362,
      4388,
      1434,
      4420,
      1003,
      213,
      906,
      4447,
      213,
      986,
      1434,
      4468,
      4499,
      213,
      213,
      4516,
      1004,
      213,
      213,
      213,
      2938,
      4547,
      1434,
      1101,
      4557,
      4583,
      213,
      4562,
      4603,
      1096,
      213,
      4337,
      4633,
      213,
      213,
      4664,
      1095,
      213,
      213,
      213,
      213,
      213,
      213,
      4696,
      4727,
      213,
      213,
      4757,
      4557,
      2018,
      213,
      4789,
      213,
      213,
      909,
      4819,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      4836,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      4863,
      1002,
      213,
      213,
      213,
      4894,
      4557,
      4925,
      4938,
      4969,
      213,
      4997,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      5023,
      213,
      213,
      5053,
      5077,
      213,
      213,
      213,
      5100,
      5131,
      213,
      5147,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      1560,
      1434,
      3974,
      4637,
      213,
      213,
      213,
      5171,
      5202,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      5234,

      // Section [2823, 2944) size containing 121 values:
      //   - in bits:       1936
      //   - in bytes:      242 B
      //   - in KibiBytes:  0.24 KiB
      213,
      1006,
      5266,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      5282,
      213,
      377,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      990,
      213,
      5303,
      213,
      213,
      5333,

      // Section [3555, 4096) size containing 541 values:
      //   - in bits:       8656
      //   - in bytes:      1082 B
      //   - in KibiBytes:  1.06 KiB
      213,
      1099,
      5365,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      2009,
      5381,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      2011,
      213,
      213,
      314,
      5413,
      902,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      5445,
      5474,
      5496,
      213,
      5519,
      1999,
      1999,
      5551,
      213,
      213,
      213,
      213,
      213,
      1999,
      1999,
      2008,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      5582,
      3094,
      191,
      191,
      197,
      197,
      203,
      203,
      5606,
      5620,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      314,
      5652,
      314,
      5679,
      5696,
      5722,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      5754,
      5784,
      213,
      213,
      990,
      213,
      213,
      213,
      213,
      377,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      991,
      213,
      5804,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      5494,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      1565,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      467,
      467,
      467,
      467,
      467,
      467,
      5836,
      467,
      467,
      467,
      5848,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      3610,
      526,
      526,
      467,
      467,
      526,
      526,
      4275,
      467,
      467,
      467,
      467,
      467,
      526,
      526,
      526,
      526,
      526,
      526,
      526,
      5880,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      467,
      1999,
      5912,
      1999,
      1999,
      2011,
      2857,
      5944,
      2009,
      5976,
      197,
      213,
      5998,
      213,
      199,
      213,
      213,
      213,
      213,
      213,
      2750,
      213,
      213,
      213,
      213,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      6030,
      3914,
      1999,
      1999,
      1999,
      6031,
      1999,
      1999,
      2005,
      6058,
      5912,
      1999,
      6090,
      1999,
      6108,
      6126,
      2936,
      213,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      1999,
      2011,
      6158,
      6177,
      1999,
      6202,
      6233,
      1999,
      1999,
      1999,
      1999,
      6265,
      1999,
      1999,
      6285,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      213,
      6311,

      // Section [6142, 6144) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [8190, 8192) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [10238, 10240) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [12286, 12288) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [14334, 14336) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [16382, 16384) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [18430, 18432) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [20478, 20480) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [22526, 22528) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [24574, 24576) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [26622, 26624) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [28670, 28800) size containing 130 values:
      //   - in bits:       2080
      //   - in bytes:      260 B
      //   - in KibiBytes:  0.25 KiB
      213,
      6342,
      6371,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6404,
      314,
      314,
      314,
      314,
      314,
      314,
      314,
      6436,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6403,
      6404,

      // Section [30718, 30720) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [32766, 32768) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

      // Section [34814, 34816) size containing 2 values:
      //   - in bits:       32
      //   - in bytes:      4 B
      //   - in KibiBytes:  0.00 KiB
      213,
      6311,

    };




    /**
     * BIDI Values Table
     *
     * Bidi: Bidirectional Class
     *
     * Table size:
     *   - in bits:       51744
     *   - in bytes:      6468 B
     *   - in KibiBytes:  6.32 KiB
     */
    static constexpr std::array<std::uint8_t, 6468ULL> bidi_values{

      // Start of 0x0:
      10, 10, 10, 10, 10, 10, 10, 10, 10, 12, 11, 12, 13, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
      10, 10, 11, 11, 11, 12,

      // Start of 0x20:
      13, 14, 14, 6, 6, 6, 14, 14, 14, 14, 14, 5, 8, 5, 8, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 14, 14, 14, 14,

      // Start of 0x40, 0xff20-0xff40:
      14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14,

      // Start of 0x60:
      14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14,

      // Start of 0x80:
      10, 10, 10, 10, 10, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
      10, 10, 10, 10, 10, 10,

      // Start of 0xa0:
      8, 14, 6, 6, 6, 6, 14, 14, 14, 14, 1, 14, 14, 10, 14, 14, 6, 6, 4, 4, 14, 1, 14, 14, 14, 4, 1, 14, 14,
      14, 14, 14,

      // Start of 0xc0-0xe0:
      1,

      // Start of 0x3e0:
      1,

      // Start of 0x1d700-0x1d720:
      1, 1, 1, 1, 1, 1,

      // Start of 0x1d740-0x1d760, 0x1f120:
      1, 1,

      // Start of 0x1f1a0:
      1, 1, 1, 1,

      // Start of 0x1d780-0x1d7a0:
      1,

      // Start of 0xa780:
      1, 1, 1, 1, 1, 1, 1,

      // Start of 0x10100:
      1,

      // Start of 0x1400, 0x30a0, 0x101a0:
      14,

      // Start of 0x100-0x280, 0x3a0-0x3c0, 0x400-0x460, 0x4a0-0x560, 0xba0, 0xda0, 0xde0-0xe00, 0xe60-0xe80,
      // 0xee0, 0xf40, 0xfe0-0x1000, 0x10a0-0x1320, 0x1360, 0x13a0-0x13e0, 0x1420-0x1660, 0x16a0-0x16e0,
      // 0x1780, 0x1820-0x1860, 0x18c0-0x1900, 0x1960-0x19a0, 0x1a20, 0x1a80, 0x1ae0, 0x1bc0, 0x1c00,
      // 0x1c40-0x1ca0, 0x1d00-0x1da0, 0x1e00-0x1f80, 0x2160, 0x2340, 0x24a0-0x24c0, 0x2800-0x28e0,
      // 0x2c00-0x2cc0, 0x2d00-0x2d40, 0x2d80-0x2dc0, 0x2e60, 0x3040-0x3060, 0x30c0, 0x3100-0x31a0, 0x3220,
      // 0x3280, 0x32e0-0x3340, 0x3380-0x33a0, 0x3400-0x4da0, 0x4e00-0xa460, 0xa4e0-0xa5e0, 0xa620-0xa640,
      // 0xa6a0-0xa6c0, 0xa740-0xa760, 0xa7a0-0xa7e0, 0xa840, 0xa880-0xa8a0, 0xa900, 0xa960, 0xa9c0, 0xaa00,
      // 0xaa80, 0xab00-0xab40, 0xab80-0xabc0, 0xac00-0xfae0, 0xff80-0xffc0, 0x10000-0x100e0, 0x10120,
      // 0x101c0, 0x10200-0x102c0, 0x10300-0x10340, 0x10380-0x107e0, 0x110e0, 0x11140, 0x111e0-0x11200,
      // 0x11260-0x112a0, 0x11380, 0x11400, 0x11460-0x11480, 0x114e0-0x11580, 0x115e0-0x11600, 0x11680,
      // 0x116c0-0x116e0, 0x11740-0x11800, 0x11840-0x11900, 0x11960-0x119a0, 0x11a60, 0x11aa0-0x11c00,
      // 0x11c40-0x11c60, 0x11cc0-0x11d00, 0x11d60, 0x11da0-0x11ec0, 0x11f60-0x11fa0, 0x12000-0x13420,
      // 0x13460-0x160e0, 0x16140-0x16ac0, 0x16b00, 0x16b40-0x16f20, 0x16f60, 0x16fa0-0x16fc0,
      // 0x17000-0x1bc60, 0x1bcc0-0x1cbe0, 0x1cec0-0x1cee0, 0x1cf60-0x1d140, 0x1d1c0, 0x1d260-0x1d2e0,
      // 0x1d360-0x1d6a0, 0x1d800-0x1d9e0, 0x1dac0-0x1dfe0, 0x1e040-0x1e060, 0x1e0a0-0x1e100, 0x1e140-0x1e280,
      // 0x1e2c0, 0x1e300-0x1e4c0, 0x1e500-0x1e5c0, 0x1e600-0x1e7e0, 0x1f140, 0x1f180, 0x1f1c0-0x1f240,
      // 0x1f280-0x1f2e0, 0x1f8e0, 0x1fc00-0x1ffc0, 0x20000-0x2ffc0, 0x30000-0x3ffc0, 0x40000-0x4ffc0,
      // 0x50000-0x5ffc0, 0x60000-0x6ffc0, 0x70000-0x7ffc0, 0x80000-0x8ffc0, 0x90000-0x9ffc0, 0xa0000-0xaffc0,
      // 0xb0000-0xbffc0, 0xc0000-0xcffc0, 0xd0000-0xdffc0, 0xe1000-0xeffc0, 0xf0000-0xfffc0,
      // 0x100000-0x10ffc0:
      1,

      // Start of 0x33e0:
      1,

      // Start of 0x19c0, 0x33c0:
      1,

      // Start of 0x3200:
      1, 1, 1, 1,

      // Start of 0x2a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 1, 1, 1,

      // Start of 0x2c0:
      1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14,

      // Start of 0x2e0:
      1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14,

      // Start of 0x300-0x340, 0x1dc0-0x1de0, 0x2de0, 0x1cf00, 0x1da00, 0x1da40, 0xe0100-0xe01c0:
      9, 9, 9,

      // Start of 0xfa0:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x360:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 14,

      // Start of 0x380:
      1, 1, 1, 1, 14, 14, 1, 14, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x16b20, 0x1e120:
      1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xec0:
      1, 1, 1, 1, 1,

      // Start of 0x480:
      1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x580:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 1, 1, 14, 14, 6, 2,

      // Start of 0x5a0:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x5c0:
      2, 9, 9, 2, 9, 9, 2, 9,

      // Start of 0x5e0, 0x7c0, 0x10800-0x108e0, 0x10920-0x109e0, 0x10a40-0x10ac0, 0x10b00, 0x10b40-0x10ce0,
      // 0x10d80-0x10e40, 0x10e80, 0x10f00, 0x10fa0-0x10fe0, 0x1e800-0x1e8a0, 0x1e8e0-0x1e920,
      // 0x1e960-0x1ec40, 0x1ecc0-0x1ece0, 0x1ed60-0x1ede0, 0x1ef00-0x1efe0:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0x600:
      7, 7, 7, 7, 7, 7, 14, 14, 3, 6, 6, 3, 8, 3, 14, 14,

      // Start of 0x740:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x620, 0x680-0x6a0, 0x760-0x780, 0x860, 0x8a0, 0xfb60-0xfd00, 0xfd60-0xfda0, 0xfe80-0xfec0,
      // 0x10d00, 0x10ec0, 0x1ec80-0x1eca0, 0x1ed00-0x1ed20, 0x1ee00-0x1eec0:
      3, 3, 3, 3,

      // Start of 0x10ee0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x720:
      3, 3, 3, 3, 3,

      // Start of 0x640:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x660:
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 3, 3, 3, 9,

      // Start of 0x6c0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 9, 9, 9, 9, 9, 9, 7, 14,

      // Start of 0x6e0:
      9, 9, 9, 9, 9, 3, 3, 9, 9, 14, 9, 9, 9, 9, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,

      // Start of 0x700:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x7a0, 0x10f40:
      3, 3, 3, 3, 3, 3, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x7e0:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 9, 9, 9, 9, 9, 9, 9, 2, 2, 14, 14, 14, 14, 2, 2, 2, 9,

      // Start of 0x800:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 9, 9, 2, 9,

      // Start of 0x820:
      9, 9, 9, 9, 2, 9, 9, 9, 2, 9, 9, 9, 9, 9,

      // Start of 0x840:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 9, 2, 2, 2, 2,

      // Start of 0x880:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 7, 3, 3, 3, 3, 3, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x8c0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x8e0:
      9, 9, 7, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x20e0:
      9, 9,

      // Start of 0x1ac0:
      9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x1cf40:
      9, 9, 9,

      // Start of 0x1b00:
      9,

      // Start of 0x900, 0xa980, 0x11100:
      9, 9, 9,

      // Start of 0x11700:
      1, 1, 1,

      // Start of 0x920:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 9, 1, 1,

      // Start of 0x940:
      1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 9, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1,

      // Start of 0x1880:
      1,

      // Start of 0xa8c0:
      1, 1,

      // Start of 0x960, 0xb60, 0xce0, 0xd60:
      1, 1, 9, 9,

      // Start of 0x9a0, 0xa20, 0xaa0, 0xca0, 0xaa60:
      1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x11160:
      1, 1, 1, 1,

      // Start of 0x16f40, 0x1e080:
      1,

      // Start of 0x1e2a0:
      1, 1, 1, 1, 1,

      // Start of 0x18a0:
      1, 1, 1,

      // Start of 0xfc0:
      1,

      // Start of 0xa9e0:
      1, 1,

      // Start of 0x11940:
      1,

      // Start of 0xb80, 0x1b40, 0x110c0:
      1,

      // Start of 0x980, 0xb00, 0xc80, 0xd80, 0xaac0, 0x11000, 0x11240:
      1, 9,

      // Start of 0xa680, 0x16100:
      1,

      // Start of 0x1340:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x9c0, 0xd40:
      1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0x1720-0x1760:
      1, 1,

      // Start of 0xa6e0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x9e0:
      1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 6, 1, 1, 1, 1, 1, 1, 1, 6, 1, 1,

      // Start of 0x114c0:
      9,

      // Start of 0xa00, 0xa80, 0x113e0:
      1, 9, 9,

      // Start of 0x1bc80:
      1, 1,

      // Start of 0xd20, 0x11320:
      1, 1, 1,

      // Start of 0x1040:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xa40:
      1, 9, 9, 1, 1, 1, 1, 9, 9, 1, 1, 9, 9, 9, 1, 1, 1, 9,

      // Start of 0xa60:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xac0:
      1, 9, 9, 9, 9, 9, 1, 9, 9, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xae0:
      1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9,

      // Start of 0xb20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1,

      // Start of 0xb40:
      1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0xbc0:
      9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0xbe0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 6, 14, 1, 1, 1, 1, 1,

      // Start of 0xc00:
      9, 1, 1, 1, 9,

      // Start of 0xc20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 9,

      // Start of 0xc40:
      9, 1, 1, 1, 1, 1, 9, 9, 9, 1, 9, 9, 9, 9, 1, 1, 1, 1, 1,

      // Start of 0xc60:
      1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14,

      // Start of 0xcc0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xd00, 0x1b80, 0x11080, 0x11180, 0x11300, 0x11f00:
      9, 9, 1,

      // Start of 0x101e0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xdc0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 9,

      // Start of 0xe20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 6,

      // Start of 0xe40:
      1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0xea0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0xf00:
      1, 1, 1, 1, 1,

      // Start of 0x11ee0:
      1, 1, 1, 1, 1,

      // Start of 0x1e5e0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9,

      // Start of 0xf20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 9, 1, 9, 14, 14, 14, 14,

      // Start of 0xf60:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0xf80:
      9, 9, 9, 9, 9, 1, 9, 9, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x1020:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 9, 9, 9, 9, 9, 9, 1, 9, 9, 1, 1, 9,

      // Start of 0x1060:
      9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1080:
      1, 1, 9, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0x1380, 0x17e0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1,
      1,

      // Start of 0x1680:
      13, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14,

      // Start of 0x1700:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9,

      // Start of 0x17a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x17c0:
      1, 1, 1, 1, 1, 1, 9, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 6, 1, 9, 1, 1,

      // Start of 0x1800:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 9, 9, 9, 10, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1,

      // Start of 0x1920:
      9, 9, 9, 1, 1, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 1, 1, 1,

      // Start of 0x1940:
      14, 1, 1, 1, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x32a0:
      1,

      // Start of 0x2fe0, 0x3240, 0xa480:
      1, 1, 1, 1, 1, 1,

      // Start of 0x24e0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x19e0, 0x21a0-0x21e0, 0x2220-0x2300, 0x23a0-0x2400, 0x2460, 0x2500-0x2680, 0x26c0-0x27e0,
      // 0x2900-0x2b40, 0x2ba0-0x2be0, 0x2e00-0x2e20, 0x2ea0-0x2ec0, 0x2f00-0x2fa0, 0x31c0, 0x4dc0-0x4de0,
      // 0xa4a0, 0xa700, 0x10140-0x10160, 0x1cc00-0x1cca0, 0x1cd00-0x1ce80, 0x1d200-0x1d220, 0x1d300-0x1d320,
      // 0x1f000, 0x1f040-0x1f060, 0x1f300-0x1f6a0, 0x1f700-0x1f740, 0x1f780-0x1f7a0, 0x1f820, 0x1f860,
      // 0x1f900-0x1fa20, 0x1faa0, 0x1fb00-0x1fb60, 0x1fba0-0x1fbc0:
      14, 14,

      // Start of 0x2e40:
      14, 14, 14, 14,

      // Start of 0x1f7c0:
      14, 14, 14,

      // Start of 0x1d340:
      14,

      // Start of 0x2320, 0x2fc0, 0x1ccc0, 0x1f0e0:
      14, 14,

      // Start of 0x2ee0, 0x1cea0, 0x1f080, 0x1fa40:
      14, 14, 14, 14, 14, 14, 14,

      // Start of 0x11660:
      14, 14,

      // Start of 0x2440:
      14,

      // Start of 0x2420:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,

      // Start of 0x1a00:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 9,

      // Start of 0x1a40:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 9, 9, 9, 9, 9, 9,

      // Start of 0x1a60:
      9, 1, 9, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 9,

      // Start of 0x1aa0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x1b20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 9, 9, 9, 9, 9, 1, 9,

      // Start of 0x1b60:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1ba0:
      1, 1, 9, 9, 9, 9, 1, 1, 9, 9, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1be0:
      1, 1, 1, 1, 1, 1, 9, 1, 9, 9, 1, 1, 1, 9, 1, 9, 9, 9, 1, 1,

      // Start of 0x1c20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9,

      // Start of 0x1cc0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x1ce0:
      9, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 9, 9,

      // Start of 0x1fa0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 1,

      // Start of 0x1fc0:
      14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14,

      // Start of 0x1fe0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 1,

      // Start of 0x2000:
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 10, 10, 10, 1, 2, 14, 14, 14, 14, 14, 14, 14, 14,

      // Start of 0x2020:
      14, 14, 14, 14, 14, 14, 14, 14, 13, 11, 15, 17, 19, 16, 18, 8, 6, 6, 6, 6, 6, 14, 14, 14, 14, 14, 14,
      14,

      // Start of 0x2040:
      14, 14, 14, 14, 8, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 13,

      // Start of 0x2060:
      10, 10, 10, 10, 10, 1, 20, 21, 22, 23, 10, 10, 10, 10, 10, 10, 4, 1, 1, 1, 4, 4, 4, 4, 4, 4, 5, 5, 14,
      14, 14, 1,

      // Start of 0x2080:
      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x20a0:
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,

      // Start of 0x20c0:
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x2100:
      14, 14, 1, 14, 14, 14, 14, 1, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 1, 14, 14, 14, 1, 1, 1, 1, 1,

      // Start of 0x2120:
      14, 14, 14, 14, 1, 14, 1, 14, 1, 14, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 1, 1, 1, 1,

      // Start of 0x2140:
      14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 14, 14, 14, 14, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14,

      // Start of 0x2180:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 1, 1, 1, 1,

      // Start of 0x2200:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 5, 6, 14, 14, 14, 14, 14,

      // Start of 0xa4c0:
      14,

      // Start of 0xff60, 0x1f260:
      14, 14, 14, 14, 14, 14,

      // Start of 0x2360:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x2380:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 14, 14,

      // Start of 0x2480:
      14, 14, 14, 14, 14, 14, 14, 14, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1,

      // Start of 0x26a0:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1,

      // Start of 0x2b60:
      14, 14, 14, 14, 14,

      // Start of 0x1f0a0:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1,

      // Start of 0x2b80:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 14, 14, 14,
      14, 14, 14, 14, 14, 14,

      // Start of 0x2ce0:
      1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14,

      // Start of 0xa720, 0x1f8c0:
      14, 14,

      // Start of 0x2d60, 0x112c0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0x2e80:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      1, 14, 14, 14, 14, 14,

      // Start of 0x3000:
      13, 14, 14, 14, 14, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14,

      // Start of 0x3020:
      14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 14, 1, 1, 1, 1, 1, 14, 14, 1, 1, 1, 1, 1, 14, 14, 14,

      // Start of 0x3080:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 14, 14,

      // Start of 0x30e0, 0x1d6e0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 1, 1, 1, 1,

      // Start of 0x31e0:
      14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14,

      // Start of 0x3260:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14,

      // Start of 0x32c0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14,

      // Start of 0x3360:
      1, 1, 1,

      // Start of 0xa860:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14,

      // Start of 0xa600:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 1,

      // Start of 0xa660:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 14, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 14, 14,

      // Start of 0xa800:
      1, 1, 9, 1, 1, 1, 9, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xa820:
      1, 1, 1, 1, 1, 9, 9, 1, 14, 14, 14, 14, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 6, 1, 1, 1, 1, 1, 1,

      // Start of 0xa8e0:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xa920:
      1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xa940:
      1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0xa9a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 9, 9, 9, 9, 1, 1, 9, 9,

      // Start of 0xaa20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1,

      // Start of 0xaa40:
      1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1,

      // Start of 0xaaa0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 9, 9, 9, 1, 1, 9, 9, 1, 1, 1, 1, 1, 9, 9,

      // Start of 0xaae0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0xab60:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xabe0:
      1, 1, 1, 1, 1, 9, 1, 1, 9, 1, 1, 1, 1, 9,

      // Start of 0xfb00:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 9,

      // Start of 0xfb20:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 2,

      // Start of 0xfb40, 0x10f20, 0x1ec60:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0xfd20:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0xfd40:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 3,

      // Start of 0xfdc0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 10,

      // Start of 0xfde0:
      10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      14, 14, 14,

      // Start of 0xfe00:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1,
      1,

      // Start of 0xfe20:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0xfe40:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 8, 14, 8, 1, 14, 8, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 6,

      // Start of 0xfe60:
      14, 14, 5, 5, 14, 14, 14, 1, 14, 6, 6, 14, 1, 1, 1, 1,

      // Start of 0xfee0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10,

      // Start of 0xff00:
      1, 14, 14, 6, 6, 6, 14, 14, 14, 14, 14, 5, 8, 5, 8, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 14, 14, 14, 14,
      14,

      // Start of 0xffe0:
      6, 6, 14, 14, 14, 6, 6, 1, 14, 14, 14, 14, 14, 14, 14, 1, 10, 10, 10, 10, 10, 10, 10, 10, 1, 14, 14, 14,
      14, 14, 10, 1,

      // Start of 0x10180, 0x1f6e0:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 1, 1, 1,

      // Start of 0x102e0:
      9, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,

      // Start of 0x10360, 0x11f20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1,

      // Start of 0x10900:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 14,

      // Start of 0x10a00:
      2, 9, 9, 9, 2, 9, 9, 2, 2, 2, 2, 2, 9, 9, 9, 9,

      // Start of 0x10a20:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 9, 2, 2, 2, 2, 9,

      // Start of 0x10ae0:
      2, 2, 2, 2, 2, 9, 9,

      // Start of 0x10b20:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 14, 14, 14, 14, 14, 14, 14,

      // Start of 0x10d20:
      3, 3, 3, 3, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3,

      // Start of 0x10d40:
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0x10d60:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 9, 9, 9, 14, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0x10e60:
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,

      // Start of 0x10ea0:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0x10f60, 0x1ed40:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0x10f80:
      2, 2, 9, 9, 9, 9, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0x11020, 0x11420:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0x11040:
      9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14,

      // Start of 0x11060:
      14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0x110a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 9, 9,

      // Start of 0x11120:
      1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 1, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x111a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x111c0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 9, 1,

      // Start of 0x11220:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 1, 9, 1, 9, 9, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0x112e0:
      1, 1, 1, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x11340, 0x11640, 0x119e0:
      9, 1, 1, 1, 1,

      // Start of 0x113a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x11360:
      1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 9, 9, 9,

      // Start of 0x113c0:
      9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 9, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x11440:
      1, 1, 9, 9,

      // Start of 0x11f40:
      9, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9,

      // Start of 0x114a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 1, 9, 1, 1, 1, 1, 9,

      // Start of 0x115a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 9, 9, 1,

      // Start of 0x115c0:
      9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9,

      // Start of 0x11620:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 9, 1, 9,

      // Start of 0x116a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 9, 1, 1, 9, 9, 9, 9, 9, 9, 1, 9, 1, 1, 1, 1, 1, 1,

      // Start of 0x11720:
      1, 1, 9, 9, 9, 9, 1, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1,

      // Start of 0x11820:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 9, 9,

      // Start of 0x11920:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 9,

      // Start of 0x119c0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 9, 9, 1, 1, 1,

      // Start of 0x11a00:
      1, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9, 1, 1,

      // Start of 0x11a20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9, 9, 9,

      // Start of 0x11a40:
      1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9, 9,

      // Start of 0x11a80:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 9, 9,

      // Start of 0x11c20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 9, 9, 9, 9, 9, 9,

      // Start of 0x11c80:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9,

      // Start of 0x11ca0:
      9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 9, 9, 1, 9, 9,

      // Start of 0x11d20:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 1, 9, 9, 1,

      // Start of 0x11d40:
      9, 9, 9, 9, 9, 9, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x11d80:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 1, 9, 1, 9,

      // Start of 0x11fc0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 6, 6,

      // Start of 0x11fe0:
      6, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1,

      // Start of 0x13440:
      9, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x16120:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 9, 9,

      // Start of 0x16ae0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9,

      // Start of 0x16f80:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x16fe0:
      1, 1, 14, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1bca0:
      10, 10, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1cce0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1,

      // Start of 0x1cf20:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,

      // Start of 0x1d160:
      1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 10, 10, 10, 10, 10, 10, 9, 9,

      // Start of 0x1d180:
      9, 9, 9, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1e4e0:
      1, 1,

      // Start of 0x1d1a0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1d1e0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1d240:
      14, 14, 9, 9, 9, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1d6c0:
      1, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1d7c0:
      1, 1, 1, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1d7e0:
      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,

      // Start of 0x1da20:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1,

      // Start of 0x1da60:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1,

      // Start of 0x1da80:
      1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1daa0:
      1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1e000:
      9, 9, 9, 9, 9, 9, 9, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9, 9,

      // Start of 0x1e020:
      9, 9, 1, 9, 9, 1, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1e2e0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6,

      // Start of 0x1e8c0:
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0x1e940:
      2, 2, 2, 2, 9, 9, 9, 9, 9, 9, 9, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

      // Start of 0x1eee0:
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

      // Start of 0x1f020, 0x1f800:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14,

      // Start of 0x1f0c0:
      1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14,

      // Start of 0x1f100:
      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1,

      // Start of 0x1f160:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0x1f6c0:
      14,

      // Start of 0x1f760:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1,

      // Start of 0x1f7e0:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1,

      // Start of 0x1f840:
      14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14,

      // Start of 0x1f880:
      14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 1, 14, 14,

      // Start of 0x1f8a0:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 1, 1, 1, 1,

      // Start of 0x1fa60:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 14, 14, 14,

      // Start of 0x1fa80:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,

      // Start of 0x1fac0:
      14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 1, 1,

      // Start of 0x1fae0:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1, 1, 1, 1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 1, 1,
      1, 1, 1, 1,

      // Start of 0x1fb80:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 1,

      // Start of 0x1fbe0:
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,

      // Start of 0x1ffe0, 0x2ffe0, 0x3ffe0, 0x4ffe0, 0x5ffe0, 0x6ffe0, 0x7ffe0, 0x8ffe0, 0x9ffe0, 0xaffe0,
      // 0xbffe0, 0xcffe0, 0xeffe0, 0xfffe0, 0x10ffe0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10,

      // Start of 0xdffe0:
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

      // Start of 0xe0000:
      1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
      10, 10, 10, 10, 10, 1,

      // Start of 0xe0020-0xe00c0, 0xe0200-0xe0fc0:
      10,

      // Start of 0xe00e0, 0xe0fe0:
      10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
      10, 10, 10, 10, 10, 1,

      // Start of 0xe01e0:
      9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
      10, 10};




} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_BIDI_TABLES_HPP
