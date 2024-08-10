
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_decomposition_tables.mjs
 *   Unicode UCD Database Creation Date: 2023-08-28
 *   This file's generation date:        Fri, 09 Aug 2024 02:46:41 GMT
 *   Unicode Version:                    15.1.0
 *   Total Table sizes in this file:
 *       - in bits:       469184
 *       - in bytes:      58648 B
 *       - in KibiBytes:  58 KiB
 *   Some other implementations' total table size was 73.4 KiB;
 *   So I saved 17 KiB and a better a locality.
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 *   UCD Database Code Points (used the get the CCC values and what not):
 *       https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt
 *   UCD README file (used to check the version and creation date):
 *       https://www.unicode.org/Public/UCD/latest/ucd/ReadMe.txt
 *   Decomposition Mapping syntax used in the UCD Database:
 *       https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
 */

#ifndef WEBPP_UNICODE_DECOMPOSITION_TABLES_HPP
#define WEBPP_UNICODE_DECOMPOSITION_TABLES_HPP

#include "../../utils/bits.hpp"

#include <array>
#include <cstdint>
#include <string_view>

namespace webpp::unicode::details {


    /**
     * In "decomposition_index" table, any code point bigger than this number will be "non-mapped" (it's
     * mapped to the input code point by standard); so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_mapped_deomps = 0x2FA40UL;

    /**
     * Decomp (Index Table)
     * Decomposition Code Points
     */
    struct alignas(std::uint32_t) decomp_index {
        /// The shifts required to extract the values out of a std::uint32_t; you can use masks as well:
        static constexpr std::uint8_t pos_shift        = 8U;
        static constexpr std::uint8_t max_length_shift = 0U;

        /// The masks required to extracting the values out of a std::uint32_t; you can use shifts as well:
        static constexpr std::uint32_t pos_mask        = 0xFFFF00U;
        static constexpr std::uint32_t max_length_mask = 0xFFU;

        // NOLINTBEGIN(*-non-private-member-variables-in-classes)

        /// This is the position that should be looked for in the values table.
        std::uint16_t pos = 0;

        /// Length of the UTF-8 Encoded Decomposition Code Points.
        /// This value is the max length of each mapping; there should be additional empty values added
        /// in between the values of the values table in order to make sure we can easily find the needed
        /// mappings for all the code points without searching for them.
        char8_t max_length = 1;

        // NOLINTEND(*-non-private-member-variables-in-classes)

        /**
         * [16bits = pos] + [8bits = max_length]
         */
        explicit(false) consteval decomp_index(std::uint32_t const value) noexcept
          : pos{static_cast<std::uint16_t>(value >> pos_shift)},
            max_length{static_cast<char8_t>((value & max_length_mask))} {}

        // NOLINTNEXTLINE(*-easily-swappable-parameters)
        explicit consteval decomp_index(std::uint16_t const inp_pos, char8_t const inp_max_length) noexcept
          : pos{inp_pos},
            max_length{inp_max_length} {}

        [[nodiscard]] constexpr std::uint32_t value() const noexcept {
            return static_cast<std::uint32_t>(max_length) | (static_cast<std::uint32_t>(pos) << pos_shift);
        }

        static constexpr std::uint16_t chunk_mask  = 0x3FU;
        static constexpr std::size_t   chunk_size  = 64U;
        static constexpr std::uint8_t  chunk_shift = 6U;

        /// Get an invalid mapping (that shows the code point is not being mapped at all)
        /// This means the code point is mapped to itself
        [[nodiscard]] static consteval decomp_index not_mapped() noexcept {
            // it can be identified by max_length == 0
            return decomp_index{0, 0};
        }

        /// Maximum value of "max_length" in the whole values table.
        /// It's the amount of mapped UTF-8 "bytes" (not code points).
        /// Hope this can enable some optimizations.
        static constexpr auto max_utf8_mapped_length = 33UL;

        /// Maximum values of UTF-16 code points mapped
        static constexpr auto max_utf16_mapped_length = 18UL;

        /// Maximum values of code points mapped (UTF-32)
        static constexpr auto max_utf32_mapped_length = 18UL;

        /**
         * Get the final position of the second table.
         * This does not apply the shift or get the value of the second table for you; this only applies tha
         * mask.
         */
        [[nodiscard]] constexpr std::uint16_t get_position(auto const request_position) const noexcept {
#if __cplusplus >= 202302L // C++23
            [[assume(max_length <= max_utf8_mapped_length)]];
#endif
            std::uint16_t const remaining_pos = static_cast<std::uint16_t>(request_position) & chunk_mask;
            return pos + static_cast<std::uint16_t>(remaining_pos * max_length);
        }
    };

    /**
     * DECOMP Index Table
     *
     * Decomposition Index
     *
     * Each value contains 2 numbers hidden inside:
     *     [16bits = pos] + [8bits = max_length]
     *
     * Table size:
     *   - in bits:       97568
     *   - in bytes:      12196 B
     *   - in KibiBytes:  12 KiB
     */
    static constexpr std::array<decomp_index, 3049ULL> decomp_indices{
      0,        0,        5,        81923,    131075,   180227,   228867,   273924,   339460,   0,
      392450,   421379,   0,        470532,   534276,   589572,   655108,   714500,   0,        779012,
      0,        0,        838404,   0,        869124,   909060,   0,        974596,   0,        0,
      0,        0,        0,        0,        0,        0,        995078,   1076486,  0,        1157894,
      1207046,  1291526,  0,        0,        0,        1377542,  1445126,  1528070,  0,        1614086,
      0,        1712390,  0,        1795334,  0,        1853702,  1901318,  0,        1981190,  2061062,
      2150147,  2194694,  2291462,  0,        2380550,  0,        0,        2440451,  0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        2487302,  2585606,
      0,        0,        0,        0,        0,        0,        2661378,  2694147,  2737667,  0,
      2786821,  2868741,  2950661,  3032581,  3114501,  3196421,  3278341,  3360261,  3442185,  3585804,
      3782403,  0,        3831555,  3880710,  3977733,  4043013,  4115721,  4261893,  4343813,  4405253,
      4464131,  0,        0,        0,        0,        4497156,  4562692,  4628225,  0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        4639244,  4795907,
      0,        4838149,  0,        0,        0,        0,        0,        4904705,  0,        0,
      0,        4920579,  0,        0,        0,        0,        4957443,  4982019,  5031171,  5080323,
      5129475,  5178627,  5227779,  5273094,  5368326,  5466630,  5564931,  5614083,  5663235,  0,
      5712401,  5990927,  6236675,  6285830,  6384146,  6679055,  6924809,  7072262,  0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        7169794,  0,        0,        7185155,
      0,        7222786,  0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        7252483,  0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        7301635,  7350787,  7399939,  7449091,
      7498243,  7547396,  7612931,  7662084,  7727621,  7809540,  7875074,  7900676,  7966212,  8031749,
      8113668,  8179206,  8277508,  8340998,  8439302,  8537633,  9065987,  9115140,  9180674,  9212932,
      9278209,  9294595,  9343747,  9392131,  0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        9440260,  0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      9500680,  0,        9590792,  0,        0,        0,        0,        0,        0,        0,
      0,        9699336,  0,        0,        0,        0,        9725960,  0,        0,        0,
      9854984,  0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        9977864,  0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        10094600, 10170376, 10301448, 0,        0,        0,        0,        0,        0,
      0,        0,        10432513, 10448897, 10465281, 10481665, 10498049, 10514433, 10437633, 10520577,
      10523649, 10433537, 10540034, 10572803, 10577411, 10582019, 10586627, 10635779, 0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      10684418, 10717187, 0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        10766338, 10798082, 10830850, 0,
      0,        0,        0,        0,        0,        0,        0,        0,        10863623, 10978307,
      11019266, 0,        11052038, 11150345, 0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        11285505, 0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      0,        0,        0,        0,        0,        0,        0,        0,        0,        0,
      11301892, 11367428, 11432964, 11498500, 11564036, 11629572, 11695108, 11760644, 11826180};

    /**
     * DECOMP Values Table
     *
     * Decomposition Index
     *
     * Table size:
     *   - in bits:       371616
     *   - in bytes:      46452 B
     *   - in KibiBytes:  46 KiB
     */
    static constexpr std::basic_string_view<char8_t> decomp_values{

      // Start of 0x0-0x80, 0x240, 0x300, 0x480, 0x500-0x540, 0x5c0, 0x680, 0x700-0x8c0, 0x980, 0xa80-0xb00,
      // 0xc00, 0xc80, 0xd00, 0xd80, 0xe40, 0xfc0, 0x1040-0x1080, 0x1100-0x1ac0, 0x1b80-0x1cc0, 0x1dc0,
      // 0x20c0, 0x2340-0x2400, 0x2500-0x29c0, 0x2a80, 0x2b00-0x2c00, 0x2c80-0x2d00, 0x2d80-0x2e40, 0x31c0,
      // 0x3400-0xa640, 0xa6c0-0xa700, 0xa780, 0xa800-0xab00, 0xab80-0xf8c0, 0x10000-0x10740, 0x107c0-0x11040,
      // 0x110c0, 0x11140-0x11300, 0x11380-0x11440, 0x114c0-0x11540, 0x115c0-0x118c0, 0x11940-0x1d100,
      // 0x1d200-0x1d3c0, 0x1d800-0x1dfc0, 0x1e080-0x1edc0, 0x1eec0-0x1f0c0, 0x1f1c0, 0x1f280-0x1fb80,
      // 0x1fc00-0x2f7c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\40\xcc\x88\0\0\0\0\0\0\0\x61\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\40\xcc\x84\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\62\0\0\0\0\63\0\0\0\0\40\xcc\x81\0\0\xce\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\40\xcc\xa7\0\0"
      u8"\61\0\0\0\0\x6f\0\0\0\0\0\0\0\0\0\61\xe2\x81\x84\64\61\xe2\x81\x84\62\63\xe2\x81\x84\64\0\0\0\0\0"

      // Start of 0xc0:
      u8"\x41\xcc\x80\x41\xcc\x81\x41\xcc\x82\x41\xcc\x83\x41\xcc\x88\x41\xcc\x8a\0\0\0\x43\xcc\xa7\x45\xcc"
      u8"\x80\x45\xcc\x81\x45\xcc\x82\x45\xcc\x88\x49\xcc\x80\x49\xcc\x81\x49\xcc\x82\x49\xcc\x88\0\0\0\x4e"
      u8"\xcc\x83\x4f\xcc\x80\x4f\xcc\x81\x4f\xcc\x82\x4f\xcc\x83\x4f\xcc\x88\0\0\0\0\0\0\x55\xcc\x80\x55\xcc"
      u8"\x81\x55\xcc\x82\x55\xcc\x88\x59\xcc\x81\0\0\0\0\0\0\x61\xcc\x80\x61\xcc\x81\x61\xcc\x82\x61\xcc\x83"
      u8"\x61\xcc\x88\x61\xcc\x8a\0\0\0\x63\xcc\xa7\x65\xcc\x80\x65\xcc\x81\x65\xcc\x82\x65\xcc\x88\x69\xcc"
      u8"\x80\x69\xcc\x81\x69\xcc\x82\x69\xcc\x88\0\0\0\x6e\xcc\x83\x6f\xcc\x80\x6f\xcc\x81\x6f\xcc\x82\x6f"
      u8"\xcc\x83\x6f\xcc\x88\0\0\0\0\0\0\x75\xcc\x80\x75\xcc\x81\x75\xcc\x82\x75\xcc\x88\x79\xcc\x81\0\0\0"
      u8"\x79\xcc\x88"

      // Start of 0x100:
      u8"\x41\xcc\x84\x61\xcc\x84\x41\xcc\x86\x61\xcc\x86\x41\xcc\xa8\x61\xcc\xa8\x43\xcc\x81\x63\xcc\x81\x43"
      u8"\xcc\x82\x63\xcc\x82\x43\xcc\x87\x63\xcc\x87\x43\xcc\x8c\x63\xcc\x8c\x44\xcc\x8c\x64\xcc\x8c\0\0\0\0"
      u8"\0\0\x45\xcc\x84\x65\xcc\x84\x45\xcc\x86\x65\xcc\x86\x45\xcc\x87\x65\xcc\x87\x45\xcc\xa8\x65\xcc\xa8"
      u8"\x45\xcc\x8c\x65\xcc\x8c\x47\xcc\x82\x67\xcc\x82\x47\xcc\x86\x67\xcc\x86\x47\xcc\x87\x67\xcc\x87\x47"
      u8"\xcc\xa7\x67\xcc\xa7\x48\xcc\x82\x68\xcc\x82\0\0\0\0\0\0\x49\xcc\x83\x69\xcc\x83\x49\xcc\x84\x69\xcc"
      u8"\x84\x49\xcc\x86\x69\xcc\x86\x49\xcc\xa8\x69\xcc\xa8\x49\xcc\x87\0\0\0\x49\x4a\0\x69\x6a\0\x4a\xcc"
      u8"\x82\x6a\xcc\x82\x4b\xcc\xa7\x6b\xcc\xa7\0\0\0\x4c\xcc\x81\x6c\xcc\x81\x4c\xcc\xa7\x6c\xcc\xa7\x4c"
      u8"\xcc\x8c\x6c\xcc\x8c\x4c\xc2\xb7"

      // Start of 0x140:
      u8"\x6c\xc2\xb7\0\0\0\0\0\0\x4e\xcc\x81\x6e\xcc\x81\x4e\xcc\xa7\x6e\xcc\xa7\x4e\xcc\x8c\x6e\xcc\x8c\xca"
      u8"\xbc\x6e\0\0\0\0\0\0\x4f\xcc\x84\x6f\xcc\x84\x4f\xcc\x86\x6f\xcc\x86\x4f\xcc\x8b\x6f\xcc\x8b\0\0\0\0"
      u8"\0\0\x52\xcc\x81\x72\xcc\x81\x52\xcc\xa7\x72\xcc\xa7\x52\xcc\x8c\x72\xcc\x8c\x53\xcc\x81\x73\xcc\x81"
      u8"\x53\xcc\x82\x73\xcc\x82\x53\xcc\xa7\x73\xcc\xa7\x53\xcc\x8c\x73\xcc\x8c\x54\xcc\xa7\x74\xcc\xa7\x54"
      u8"\xcc\x8c\x74\xcc\x8c\0\0\0\0\0\0\x55\xcc\x83\x75\xcc\x83\x55\xcc\x84\x75\xcc\x84\x55\xcc\x86\x75\xcc"
      u8"\x86\x55\xcc\x8a\x75\xcc\x8a\x55\xcc\x8b\x75\xcc\x8b\x55\xcc\xa8\x75\xcc\xa8\x57\xcc\x82\x77\xcc\x82"
      u8"\x59\xcc\x82\x79\xcc\x82\x59\xcc\x88\x5a\xcc\x81\x7a\xcc\x81\x5a\xcc\x87\x7a\xcc\x87\x5a\xcc\x8c\x7a"
      u8"\xcc\x8c\x73"

      // Start of 0x180:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x4f\xcc"
      u8"\x9b\x6f\xcc\x9b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x55"
      u8"\xcc\x9b\x75\xcc\x9b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x44\xc5\xbd\0\x44\xc5\xbe\0\x64\xc5\xbe\0\x4c\x4a\0\0\x4c\x6a\0\0"
      u8"\x6c\x6a\0\0\x4e\x4a\0\0\x4e\x6a\0\0\x6e\x6a\0\0\x41\xcc\x8c\0\x61\xcc\x8c\0\x49\xcc\x8c\0\x69\xcc"
      u8"\x8c\0\x4f\xcc\x8c\0\x6f\xcc\x8c\0\x55\xcc\x8c\0\x75\xcc\x8c\0\xc3\x9c\xcc\x84\xc3\xbc\xcc\x84\xc3"
      u8"\x9c\xcc\x81\xc3\xbc\xcc\x81\xc3\x9c\xcc\x8c\xc3\xbc\xcc\x8c\xc3\x9c\xcc\x80\xc3\xbc\xcc\x80\0\0\0\0"
      u8"\xc3\x84\xcc\x84\xc3\xa4\xcc\x84\xc8\xa6\xcc\x84\xc8\xa7\xcc\x84\xc3\x86\xcc\x84\xc3\xa6\xcc\x84\0\0"
      u8"\0\0\0\0\0\0\x47\xcc\x8c\0\x67\xcc\x8c\0\x4b\xcc\x8c\0\x6b\xcc\x8c\0\x4f\xcc\xa8\0\x6f\xcc\xa8\0\xc7"
      u8"\xaa\xcc\x84\xc7\xab\xcc\x84\xc6\xb7\xcc\x8c\xca\x92\xcc\x8c\x6a\xcc\x8c\0\x44\x5a\0\0\x44\x7a\0\0"
      u8"\x64\x7a\0\0\x47\xcc\x81\0\x67\xcc\x81\0\0\0\0\0\0\0\0\0\x4e\xcc\x80\0\x6e\xcc\x80\0\xc3\x85\xcc\x81"
      u8"\xc3\xa5\xcc\x81\xc3\x86\xcc\x81\xc3\xa6\xcc\x81\xc3\x98\xcc\x81\xc3\xb8\xcc\x81"

      // Start of 0x200:
      u8"\x41\xcc\x8f\0\x61\xcc\x8f\0\x41\xcc\x91\0\x61\xcc\x91\0\x45\xcc\x8f\0\x65\xcc\x8f\0\x45\xcc\x91\0"
      u8"\x65\xcc\x91\0\x49\xcc\x8f\0\x69\xcc\x8f\0\x49\xcc\x91\0\x69\xcc\x91\0\x4f\xcc\x8f\0\x6f\xcc\x8f\0"
      u8"\x4f\xcc\x91\0\x6f\xcc\x91\0\x52\xcc\x8f\0\x72\xcc\x8f\0\x52\xcc\x91\0\x72\xcc\x91\0\x55\xcc\x8f\0"
      u8"\x75\xcc\x8f\0\x55\xcc\x91\0\x75\xcc\x91\0\x53\xcc\xa6\0\x73\xcc\xa6\0\x54\xcc\xa6\0\x74\xcc\xa6\0\0"
      u8"\0\0\0\0\0\0\0\x48\xcc\x8c\0\x68\xcc\x8c\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x41\xcc"
      u8"\x87\0\x61\xcc\x87\0\x45\xcc\xa7\0\x65\xcc\xa7\0\xc3\x96\xcc\x84\xc3\xb6\xcc\x84\xc3\x95\xcc\x84\xc3"
      u8"\xb5\xcc\x84\x4f\xcc\x87\0\x6f\xcc\x87\0\xc8\xae\xcc\x84\xc8\xaf\xcc\x84\x59\xcc\x84\0\x79\xcc\x84"

      // Start of 0x280:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x68\0"
      u8"\xc9\xa6\x6a\0\x72\0\xc9\xb9\xc9\xbb\xca\x81\x77\0\x79"

      // Start of 0x2c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\40\xcc\x86\40\xcc\x87\40\xcc\x8a\40\xcc\xa8\40\xcc\x83"
      u8"\40\xcc\x8b\0\0\0\0\0\0\xc9\xa3\0\x6c\0\0\x73\0\0\x78\0\0\xca\x95\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x340:
      u8"\xcc\x80\0\0\xcc\x81\0\0\0\0\0\0\xcc\x93\0\0\xcc\x88\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\xb9\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\40\xcd\x85"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\73"

      // Start of 0x380:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\40\xcc\x81\0\xc2\xa8\xcc\x81\xce\x91\xcc\x81\xc2\xb7\0\0\xce\x95"
      u8"\xcc\x81\xce\x97\xcc\x81\xce\x99\xcc\x81\0\0\0\0\xce\x9f\xcc\x81\0\0\0\0\xce\xa5\xcc\x81\xce\xa9\xcc"
      u8"\x81\xcf\x8a\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xce\x99\xcc\x88\xce\xa5\xcc\x88\xce\xb1\xcc\x81\xce\xb5\xcc\x81\xce\xb7\xcc\x81"
      u8"\xce\xb9\xcc\x81\xcf\x8b\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x3c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xce\xb9\xcc\x88\xcf"
      u8"\x85\xcc\x88\xce\xbf\xcc\x81\xcf\x85\xcc\x81\xcf\x89\xcc\x81\0\0\0\0\xce\xb2\0\0\xce\xb8\0\0\xce\xa5"
      u8"\0\0\xcf\x92\xcc\x81\xcf\x92\xcc\x88\xcf\x86\0\0\xcf\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xce\xba\0\0\xcf\x81\0\0\xcf\x82\0\0\0\0"
      u8"\0\0\xce\x98\0\0\xce\xb5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xce\xa3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0"

      // Start of 0x400:
      u8"\xd0\x95\xcc\x80\xd0\x95\xcc\x88\0\0\0\0\xd0\x93\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x86\xcc\x88\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x9a\xcc\x81\xd0\x98\xcc\x80\xd0\xa3\xcc\x86\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x98\xcc\x86\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xd0\xb8\xcc\x86"

      // Start of 0x440:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\xb5\xcc\x80\xd0\xb5\xcc\x88\0\0\0\0\xd0\xb3\xcc\x81\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xd1\x96\xcc\x88\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\xba\xcc\x81\xd0\xb8\xcc\x80\xd1\x83\xcc"
      u8"\x86\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd1\xb4\xcc"
      u8"\x8f\xd1\xb5\xcc\x8f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x4c0:
      u8"\0\0\0\0\xd0\x96\xcc\x86\xd0\xb6\xcc\x86\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x90\xcc\x86\xd0\xb0\xcc\x86\xd0\x90\xcc\x88\xd0\xb0"
      u8"\xcc\x88\0\0\0\0\0\0\0\0\xd0\x95\xcc\x86\xd0\xb5\xcc\x86\0\0\0\0\0\0\0\0\xd3\x98\xcc\x88\xd3\x99\xcc"
      u8"\x88\xd0\x96\xcc\x88\xd0\xb6\xcc\x88\xd0\x97\xcc\x88\xd0\xb7\xcc\x88\0\0\0\0\0\0\0\0\xd0\x98\xcc\x84"
      u8"\xd0\xb8\xcc\x84\xd0\x98\xcc\x88\xd0\xb8\xcc\x88\xd0\x9e\xcc\x88\xd0\xbe\xcc\x88\0\0\0\0\0\0\0\0\xd3"
      u8"\xa8\xcc\x88\xd3\xa9\xcc\x88\xd0\xad\xcc\x88\xd1\x8d\xcc\x88\xd0\xa3\xcc\x84\xd1\x83\xcc\x84\xd0\xa3"
      u8"\xcc\x88\xd1\x83\xcc\x88\xd0\xa3\xcc\x8b\xd1\x83\xcc\x8b\xd0\xa7\xcc\x88\xd1\x87\xcc\x88\0\0\0\0\0\0"
      u8"\0\0\xd0\xab\xcc\x88\xd1\x8b\xcc\x88"

      // Start of 0x580:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd5\xa5\xd6\x82\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x600:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xa7\xd9\x93\xd8\xa7\xd9"
      u8"\x94\xd9\x88\xd9\x94\xd8\xa7\xd9\x95\xd9\x8a\xd9\x94"

      // Start of 0x640:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xa7\xd9\xb4\xd9\x88\xd9\xb4\xdb\x87\xd9\xb4\xd9\x8a\xd9\xb4\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x6c0:
      u8"\xdb\x95\xd9\x94\0\0\0\0\xdb\x81\xd9\x94\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xdb\x92\xd9\x94"

      // Start of 0x900:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa4"
      u8"\xa8\xe0\xa4\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe0\xa4\xb0\xe0\xa4\xbc\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa4\xb3\xe0\xa4\xbc"

      // Start of 0x940:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa4\x95"
      u8"\xe0\xa4\xbc\xe0\xa4\x96\xe0\xa4\xbc\xe0\xa4\x97\xe0\xa4\xbc\xe0\xa4\x9c\xe0\xa4\xbc\xe0\xa4\xa1\xe0"
      u8"\xa4\xbc\xe0\xa4\xa2\xe0\xa4\xbc\xe0\xa4\xab\xe0\xa4\xbc\xe0\xa4\xaf\xe0\xa4\xbc\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x9c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa7\x87\xe0\xa6\xbe\xe0\xa7\x87\xe0\xa7\x97\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa6\xa1\xe0\xa6\xbc\xe0\xa6\xa2\xe0"
      u8"\xa6\xbc\0\0\0\0\0\0\xe0\xa6\xaf\xe0\xa6\xbc"

      // Start of 0xa00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe0\xa8\xb2\xe0\xa8\xbc\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa8\xb8\xe0\xa8\xbc"

      // Start of 0xa40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe0\xa8\x96\xe0\xa8\xbc\xe0\xa8\x97\xe0\xa8\xbc\xe0\xa8\x9c\xe0\xa8\xbc\0\0\0\0\0\0\0\0\0\0\0\0\xe0"
      u8"\xa8\xab\xe0\xa8\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xb40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0"
      u8"\xad\x87\xe0\xad\x96\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xad\x87\xe0\xac\xbe\xe0\xad\x87\xe0\xad\x97\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xac\xa1\xe0\xac\xbc\xe0"
      u8"\xac\xa2\xe0\xac\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xb80:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xae\x92\xe0\xaf\x97\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xbc0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xe0\xaf\x86\xe0\xae\xbe\xe0\xaf\x87\xe0\xae\xbe\xe0\xaf\x86\xe0\xaf\x97\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0"

      // Start of 0xc40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0"
      u8"\xb1\x86\xe0\xb1\x96\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xcc0:
      u8"\xe0\xb2\xbf\xe0\xb3\x95\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0"
      u8"\xb3\x86\xe0\xb3\x95\xe0\xb3\x86\xe0\xb3\x96\0\0\0\0\0\0\xe0\xb3\x86\xe0\xb3\x82\xe0\xb3\x8a\xe0\xb3"
      u8"\x95\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0"

      // Start of 0xd40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xe0\xb5\x86\xe0\xb4\xbe\xe0\xb5\x87\xe0\xb4\xbe\xe0\xb5\x86\xe0\xb5\x97\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xdc0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe0\xb7\x99\xe0\xb7\x8a\0\0\0\0\0\0\xe0\xb7\x99\xe0\xb7\x8f\xe0\xb7\x9c\xe0\xb7\x8a\xe0"
      u8"\xb7\x99\xe0\xb7\x9f"

      // Start of 0xe00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe0\xb9\x8d\xe0\xb8\xb2"

      // Start of 0xe80:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe0\xbb\x8d\xe0\xba\xb2"

      // Start of 0xec0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xba\xab\xe0\xba\x99\xe0\xba\xab\xe0\xba\xa1\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xf00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbc\x8b\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xf40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x82\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x8c\xe0\xbe\xb7\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x91\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe0\xbd\x96\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x9b\xe0"
      u8"\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x80\xe0\xbe\xb5\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\xb1\xe0"
      u8"\xbd\xb2\0\0\0\0\0\0\xe0\xbd\xb1\xe0\xbd\xb4\xe0\xbe\xb2\xe0\xbe\x80\xe0\xbe\xb2\xe0\xbe\x81\xe0\xbe"
      u8"\xb3\xe0\xbe\x80\xe0\xbe\xb3\xe0\xbe\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xf80:
      u8"\0\0\0\0\0\0\xe0\xbd\xb1\xe0\xbe\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbe\x92\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbe\x9c\xe0\xbe\xb7\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbe\xa1\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\xe0\xbe\xa6\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbe\xab"
      u8"\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbe\x90\xe0\xbe\xb5"

      // Start of 0x1000:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x80\xa5\xe1\x80\xae"

      // Start of 0x10c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x83\x9c"

      // Start of 0x1b00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xac\x85\xe1\xac\xb5\0\0"
      u8"\0\0\0\0\xe1\xac\x87\xe1\xac\xb5\0\0\0\0\0\0\xe1\xac\x89\xe1\xac\xb5\0\0\0\0\0\0\xe1\xac\x8b\xe1\xac"
      u8"\xb5\0\0\0\0\0\0\xe1\xac\x8d\xe1\xac\xb5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xac\x91\xe1\xac\xb5"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xac\xba\xe1\xac"
      u8"\xb5\0\0\0\0\0\0\xe1\xac\xbc\xe1\xac\xb5\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1b40:
      u8"\xe1\xac\xbe\xe1\xac\xb5\xe1\xac\xbf\xe1\xac\xb5\0\0\0\0\0\0\xe1\xad\x82\xe1\xac\xb5\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1d00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x41\0\xc3\x86\x42\0\0\0"
      u8"\x44\0\x45\0\xc6\x8e\x47\0\x48\0\x49\0\x4a\0\x4b\0\x4c\0\x4d\0\x4e\0\0\0\x4f\0\xc8\xa2\x50\0\x52\0"

      // Start of 0x1d40:
      u8"\x54\0\0\x55\0\0\x57\0\0\x61\0\0\xc9\x90\0\xc9\x91\0\xe1\xb4\x82\x62\0\0\x64\0\0\x65\0\0\xc9\x99\0"
      u8"\xc9\x9b\0\xc9\x9c\0\x67\0\0\0\0\0\x6b\0\0\x6d\0\0\xc5\x8b\0\x6f\0\0\xc9\x94\0\xe1\xb4\x96\xe1\xb4"
      u8"\x97\x70\0\0\x74\0\0\x75\0\0\xe1\xb4\x9d\xc9\xaf\0\x76\0\0\xe1\xb4\xa5\xce\xb2\0\xce\xb3\0\xce\xb4\0"
      u8"\xcf\x86\0\xcf\x87\0\x69\0\0\x72\0\0\x75\0\0\x76\0\0\xce\xb2\0\xce\xb3\0\xcf\x81\0\xcf\x86\0\xcf\x87"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\xbd"

      // Start of 0x1d80:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc9\x92\0\x63\0\0\xc9\x95\0\xc3\xb0\0"
      u8"\xc9\x9c\0\x66\0\0\xc9\x9f\0\xc9\xa1\0\xc9\xa5\0\xc9\xa8\0\xc9\xa9\0\xc9\xaa\0\xe1\xb5\xbb\xca\x9d\0"
      u8"\xc9\xad\0\xe1\xb6\x85\xca\x9f\0\xc9\xb1\0\xc9\xb0\0\xc9\xb2\0\xc9\xb3\0\xc9\xb4\0\xc9\xb5\0\xc9\xb8"
      u8"\0\xca\x82\0\xca\x83\0\xc6\xab\0\xca\x89\0\xca\x8a\0\xe1\xb4\x9c\xca\x8b\0\xca\x8c\0\x7a\0\0\xca\x90"
      u8"\0\xca\x91\0\xca\x92\0\xce\xb8\0"

      // Start of 0x1e00:
      u8"\x41\xcc\xa5\0\0\x61\xcc\xa5\0\0\x42\xcc\x87\0\0\x62\xcc\x87\0\0\x42\xcc\xa3\0\0\x62\xcc\xa3\0\0\x42"
      u8"\xcc\xb1\0\0\x62\xcc\xb1\0\0\xc3\x87\xcc\x81\0\xc3\xa7\xcc\x81\0\x44\xcc\x87\0\0\x64\xcc\x87\0\0\x44"
      u8"\xcc\xa3\0\0\x64\xcc\xa3\0\0\x44\xcc\xb1\0\0\x64\xcc\xb1\0\0\x44\xcc\xa7\0\0\x64\xcc\xa7\0\0\x44\xcc"
      u8"\xad\0\0\x64\xcc\xad\0\0\xc4\x92\xcc\x80\0\xc4\x93\xcc\x80\0\xc4\x92\xcc\x81\0\xc4\x93\xcc\x81\0\x45"
      u8"\xcc\xad\0\0\x65\xcc\xad\0\0\x45\xcc\xb0\0\0\x65\xcc\xb0\0\0\xc8\xa8\xcc\x86\0\xc8\xa9\xcc\x86\0\x46"
      u8"\xcc\x87\0\0\x66\xcc\x87\0\0\x47\xcc\x84\0\0\x67\xcc\x84\0\0\x48\xcc\x87\0\0\x68\xcc\x87\0\0\x48\xcc"
      u8"\xa3\0\0\x68\xcc\xa3\0\0\x48\xcc\x88\0\0\x68\xcc\x88\0\0\x48\xcc\xa7\0\0\x68\xcc\xa7\0\0\x48\xcc\xae"
      u8"\0\0\x68\xcc\xae\0\0\x49\xcc\xb0\0\0\x69\xcc\xb0\0\0\xc3\x8f\xcc\x81\0\xc3\xaf\xcc\x81\0\x4b\xcc\x81"
      u8"\0\0\x6b\xcc\x81\0\0\x4b\xcc\xa3\0\0\x6b\xcc\xa3\0\0\x4b\xcc\xb1\0\0\x6b\xcc\xb1\0\0\x4c\xcc\xa3\0\0"
      u8"\x6c\xcc\xa3\0\0\xe1\xb8\xb6\xcc\x84\xe1\xb8\xb7\xcc\x84\x4c\xcc\xb1\0\0\x6c\xcc\xb1\0\0\x4c\xcc\xad"
      u8"\0\0\x6c\xcc\xad\0\0\x4d\xcc\x81\0\0\x6d\xcc\x81\0\0"

      // Start of 0x1e40:
      u8"\x4d\xcc\x87\0\0\x6d\xcc\x87\0\0\x4d\xcc\xa3\0\0\x6d\xcc\xa3\0\0\x4e\xcc\x87\0\0\x6e\xcc\x87\0\0\x4e"
      u8"\xcc\xa3\0\0\x6e\xcc\xa3\0\0\x4e\xcc\xb1\0\0\x6e\xcc\xb1\0\0\x4e\xcc\xad\0\0\x6e\xcc\xad\0\0\xc3\x95"
      u8"\xcc\x81\0\xc3\xb5\xcc\x81\0\xc3\x95\xcc\x88\0\xc3\xb5\xcc\x88\0\xc5\x8c\xcc\x80\0\xc5\x8d\xcc\x80\0"
      u8"\xc5\x8c\xcc\x81\0\xc5\x8d\xcc\x81\0\x50\xcc\x81\0\0\x70\xcc\x81\0\0\x50\xcc\x87\0\0\x70\xcc\x87\0\0"
      u8"\x52\xcc\x87\0\0\x72\xcc\x87\0\0\x52\xcc\xa3\0\0\x72\xcc\xa3\0\0\xe1\xb9\x9a\xcc\x84\xe1\xb9\x9b\xcc"
      u8"\x84\x52\xcc\xb1\0\0\x72\xcc\xb1\0\0\x53\xcc\x87\0\0\x73\xcc\x87\0\0\x53\xcc\xa3\0\0\x73\xcc\xa3\0\0"
      u8"\xc5\x9a\xcc\x87\0\xc5\x9b\xcc\x87\0\xc5\xa0\xcc\x87\0\xc5\xa1\xcc\x87\0\xe1\xb9\xa2\xcc\x87\xe1\xb9"
      u8"\xa3\xcc\x87\x54\xcc\x87\0\0\x74\xcc\x87\0\0\x54\xcc\xa3\0\0\x74\xcc\xa3\0\0\x54\xcc\xb1\0\0\x74\xcc"
      u8"\xb1\0\0\x54\xcc\xad\0\0\x74\xcc\xad\0\0\x55\xcc\xa4\0\0\x75\xcc\xa4\0\0\x55\xcc\xb0\0\0\x75\xcc\xb0"
      u8"\0\0\x55\xcc\xad\0\0\x75\xcc\xad\0\0\xc5\xa8\xcc\x81\0\xc5\xa9\xcc\x81\0\xc5\xaa\xcc\x88\0\xc5\xab"
      u8"\xcc\x88\0\x56\xcc\x83\0\0\x76\xcc\x83\0\0\x56\xcc\xa3\0\0\x76\xcc\xa3\0\0"

      // Start of 0x1e80:
      u8"\x57\xcc\x80\0\0\x77\xcc\x80\0\0\x57\xcc\x81\0\0\x77\xcc\x81\0\0\x57\xcc\x88\0\0\x77\xcc\x88\0\0\x57"
      u8"\xcc\x87\0\0\x77\xcc\x87\0\0\x57\xcc\xa3\0\0\x77\xcc\xa3\0\0\x58\xcc\x87\0\0\x78\xcc\x87\0\0\x58\xcc"
      u8"\x88\0\0\x78\xcc\x88\0\0\x59\xcc\x87\0\0\x79\xcc\x87\0\0\x5a\xcc\x82\0\0\x7a\xcc\x82\0\0\x5a\xcc\xa3"
      u8"\0\0\x7a\xcc\xa3\0\0\x5a\xcc\xb1\0\0\x7a\xcc\xb1\0\0\x68\xcc\xb1\0\0\x74\xcc\x88\0\0\x77\xcc\x8a\0\0"
      u8"\x79\xcc\x8a\0\0\x61\xca\xbe\0\0\xc5\xbf\xcc\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x41\xcc"
      u8"\xa3\0\0\x61\xcc\xa3\0\0\x41\xcc\x89\0\0\x61\xcc\x89\0\0\xc3\x82\xcc\x81\0\xc3\xa2\xcc\x81\0\xc3\x82"
      u8"\xcc\x80\0\xc3\xa2\xcc\x80\0\xc3\x82\xcc\x89\0\xc3\xa2\xcc\x89\0\xc3\x82\xcc\x83\0\xc3\xa2\xcc\x83\0"
      u8"\xe1\xba\xa0\xcc\x82\xe1\xba\xa1\xcc\x82\xc4\x82\xcc\x81\0\xc4\x83\xcc\x81\0\xc4\x82\xcc\x80\0\xc4"
      u8"\x83\xcc\x80\0\xc4\x82\xcc\x89\0\xc4\x83\xcc\x89\0\xc4\x82\xcc\x83\0\xc4\x83\xcc\x83\0\xe1\xba\xa0"
      u8"\xcc\x86\xe1\xba\xa1\xcc\x86\x45\xcc\xa3\0\0\x65\xcc\xa3\0\0\x45\xcc\x89\0\0\x65\xcc\x89\0\0\x45\xcc"
      u8"\x83\0\0\x65\xcc\x83\0\0\xc3\x8a\xcc\x81\0\xc3\xaa\xcc\x81\0"

      // Start of 0x1ec0:
      u8"\xc3\x8a\xcc\x80\0\xc3\xaa\xcc\x80\0\xc3\x8a\xcc\x89\0\xc3\xaa\xcc\x89\0\xc3\x8a\xcc\x83\0\xc3\xaa"
      u8"\xcc\x83\0\xe1\xba\xb8\xcc\x82\xe1\xba\xb9\xcc\x82\x49\xcc\x89\0\0\x69\xcc\x89\0\0\x49\xcc\xa3\0\0"
      u8"\x69\xcc\xa3\0\0\x4f\xcc\xa3\0\0\x6f\xcc\xa3\0\0\x4f\xcc\x89\0\0\x6f\xcc\x89\0\0\xc3\x94\xcc\x81\0"
      u8"\xc3\xb4\xcc\x81\0\xc3\x94\xcc\x80\0\xc3\xb4\xcc\x80\0\xc3\x94\xcc\x89\0\xc3\xb4\xcc\x89\0\xc3\x94"
      u8"\xcc\x83\0\xc3\xb4\xcc\x83\0\xe1\xbb\x8c\xcc\x82\xe1\xbb\x8d\xcc\x82\xc6\xa0\xcc\x81\0\xc6\xa1\xcc"
      u8"\x81\0\xc6\xa0\xcc\x80\0\xc6\xa1\xcc\x80\0\xc6\xa0\xcc\x89\0\xc6\xa1\xcc\x89\0\xc6\xa0\xcc\x83\0\xc6"
      u8"\xa1\xcc\x83\0\xc6\xa0\xcc\xa3\0\xc6\xa1\xcc\xa3\0\x55\xcc\xa3\0\0\x75\xcc\xa3\0\0\x55\xcc\x89\0\0"
      u8"\x75\xcc\x89\0\0\xc6\xaf\xcc\x81\0\xc6\xb0\xcc\x81\0\xc6\xaf\xcc\x80\0\xc6\xb0\xcc\x80\0\xc6\xaf\xcc"
      u8"\x89\0\xc6\xb0\xcc\x89\0\xc6\xaf\xcc\x83\0\xc6\xb0\xcc\x83\0\xc6\xaf\xcc\xa3\0\xc6\xb0\xcc\xa3\0\x59"
      u8"\xcc\x80\0\0\x79\xcc\x80\0\0\x59\xcc\xa3\0\0\x79\xcc\xa3\0\0\x59\xcc\x89\0\0\x79\xcc\x89\0\0\x59\xcc"
      u8"\x83\0\0\x79\xcc\x83\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1f00:
      u8"\xce\xb1\xcc\x93\0\xce\xb1\xcc\x94\0\xe1\xbc\x80\xcc\x80\xe1\xbc\x81\xcc\x80\xe1\xbc\x80\xcc\x81\xe1"
      u8"\xbc\x81\xcc\x81\xe1\xbc\x80\xcd\x82\xe1\xbc\x81\xcd\x82\xce\x91\xcc\x93\0\xce\x91\xcc\x94\0\xe1\xbc"
      u8"\x88\xcc\x80\xe1\xbc\x89\xcc\x80\xe1\xbc\x88\xcc\x81\xe1\xbc\x89\xcc\x81\xe1\xbc\x88\xcd\x82\xe1\xbc"
      u8"\x89\xcd\x82\xce\xb5\xcc\x93\0\xce\xb5\xcc\x94\0\xe1\xbc\x90\xcc\x80\xe1\xbc\x91\xcc\x80\xe1\xbc\x90"
      u8"\xcc\x81\xe1\xbc\x91\xcc\x81\0\0\0\0\0\0\0\0\0\0\xce\x95\xcc\x93\0\xce\x95\xcc\x94\0\xe1\xbc\x98\xcc"
      u8"\x80\xe1\xbc\x99\xcc\x80\xe1\xbc\x98\xcc\x81\xe1\xbc\x99\xcc\x81\0\0\0\0\0\0\0\0\0\0\xce\xb7\xcc\x93"
      u8"\0\xce\xb7\xcc\x94\0\xe1\xbc\xa0\xcc\x80\xe1\xbc\xa1\xcc\x80\xe1\xbc\xa0\xcc\x81\xe1\xbc\xa1\xcc\x81"
      u8"\xe1\xbc\xa0\xcd\x82\xe1\xbc\xa1\xcd\x82\xce\x97\xcc\x93\0\xce\x97\xcc\x94\0\xe1\xbc\xa8\xcc\x80\xe1"
      u8"\xbc\xa9\xcc\x80\xe1\xbc\xa8\xcc\x81\xe1\xbc\xa9\xcc\x81\xe1\xbc\xa8\xcd\x82\xe1\xbc\xa9\xcd\x82\xce"
      u8"\xb9\xcc\x93\0\xce\xb9\xcc\x94\0\xe1\xbc\xb0\xcc\x80\xe1\xbc\xb1\xcc\x80\xe1\xbc\xb0\xcc\x81\xe1\xbc"
      u8"\xb1\xcc\x81\xe1\xbc\xb0\xcd\x82\xe1\xbc\xb1\xcd\x82\xce\x99\xcc\x93\0\xce\x99\xcc\x94\0\xe1\xbc\xb8"
      u8"\xcc\x80\xe1\xbc\xb9\xcc\x80\xe1\xbc\xb8\xcc\x81\xe1\xbc\xb9\xcc\x81\xe1\xbc\xb8\xcd\x82\xe1\xbc\xb9"
      u8"\xcd\x82"

      // Start of 0x1f40:
      u8"\xce\xbf\xcc\x93\0\xce\xbf\xcc\x94\0\xe1\xbd\x80\xcc\x80\xe1\xbd\x81\xcc\x80\xe1\xbd\x80\xcc\x81\xe1"
      u8"\xbd\x81\xcc\x81\0\0\0\0\0\0\0\0\0\0\xce\x9f\xcc\x93\0\xce\x9f\xcc\x94\0\xe1\xbd\x88\xcc\x80\xe1\xbd"
      u8"\x89\xcc\x80\xe1\xbd\x88\xcc\x81\xe1\xbd\x89\xcc\x81\0\0\0\0\0\0\0\0\0\0\xcf\x85\xcc\x93\0\xcf\x85"
      u8"\xcc\x94\0\xe1\xbd\x90\xcc\x80\xe1\xbd\x91\xcc\x80\xe1\xbd\x90\xcc\x81\xe1\xbd\x91\xcc\x81\xe1\xbd"
      u8"\x90\xcd\x82\xe1\xbd\x91\xcd\x82\0\0\0\0\0\xce\xa5\xcc\x94\0\0\0\0\0\0\xe1\xbd\x99\xcc\x80\0\0\0\0\0"
      u8"\xe1\xbd\x99\xcc\x81\0\0\0\0\0\xe1\xbd\x99\xcd\x82\xcf\x89\xcc\x93\0\xcf\x89\xcc\x94\0\xe1\xbd\xa0"
      u8"\xcc\x80\xe1\xbd\xa1\xcc\x80\xe1\xbd\xa0\xcc\x81\xe1\xbd\xa1\xcc\x81\xe1\xbd\xa0\xcd\x82\xe1\xbd\xa1"
      u8"\xcd\x82\xce\xa9\xcc\x93\0\xce\xa9\xcc\x94\0\xe1\xbd\xa8\xcc\x80\xe1\xbd\xa9\xcc\x80\xe1\xbd\xa8\xcc"
      u8"\x81\xe1\xbd\xa9\xcc\x81\xe1\xbd\xa8\xcd\x82\xe1\xbd\xa9\xcd\x82\xce\xb1\xcc\x80\0\xce\xac\0\0\0\xce"
      u8"\xb5\xcc\x80\0\xce\xad\0\0\0\xce\xb7\xcc\x80\0\xce\xae\0\0\0\xce\xb9\xcc\x80\0\xce\xaf\0\0\0\xce\xbf"
      u8"\xcc\x80\0\xcf\x8c\0\0\0\xcf\x85\xcc\x80\0\xcf\x8d\0\0\0\xcf\x89\xcc\x80\0\xcf\x8e\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0"

      // Start of 0x1f80:
      u8"\xe1\xbc\x80\xcd\x85\xe1\xbc\x81\xcd\x85\xe1\xbc\x82\xcd\x85\xe1\xbc\x83\xcd\x85\xe1\xbc\x84\xcd\x85"
      u8"\xe1\xbc\x85\xcd\x85\xe1\xbc\x86\xcd\x85\xe1\xbc\x87\xcd\x85\xe1\xbc\x88\xcd\x85\xe1\xbc\x89\xcd\x85"
      u8"\xe1\xbc\x8a\xcd\x85\xe1\xbc\x8b\xcd\x85\xe1\xbc\x8c\xcd\x85\xe1\xbc\x8d\xcd\x85\xe1\xbc\x8e\xcd\x85"
      u8"\xe1\xbc\x8f\xcd\x85\xe1\xbc\xa0\xcd\x85\xe1\xbc\xa1\xcd\x85\xe1\xbc\xa2\xcd\x85\xe1\xbc\xa3\xcd\x85"
      u8"\xe1\xbc\xa4\xcd\x85\xe1\xbc\xa5\xcd\x85\xe1\xbc\xa6\xcd\x85\xe1\xbc\xa7\xcd\x85\xe1\xbc\xa8\xcd\x85"
      u8"\xe1\xbc\xa9\xcd\x85\xe1\xbc\xaa\xcd\x85\xe1\xbc\xab\xcd\x85\xe1\xbc\xac\xcd\x85\xe1\xbc\xad\xcd\x85"
      u8"\xe1\xbc\xae\xcd\x85\xe1\xbc\xaf\xcd\x85\xe1\xbd\xa0\xcd\x85\xe1\xbd\xa1\xcd\x85\xe1\xbd\xa2\xcd\x85"
      u8"\xe1\xbd\xa3\xcd\x85\xe1\xbd\xa4\xcd\x85\xe1\xbd\xa5\xcd\x85\xe1\xbd\xa6\xcd\x85\xe1\xbd\xa7\xcd\x85"
      u8"\xe1\xbd\xa8\xcd\x85\xe1\xbd\xa9\xcd\x85\xe1\xbd\xaa\xcd\x85\xe1\xbd\xab\xcd\x85\xe1\xbd\xac\xcd\x85"
      u8"\xe1\xbd\xad\xcd\x85\xe1\xbd\xae\xcd\x85\xe1\xbd\xaf\xcd\x85\xce\xb1\xcc\x86\0\xce\xb1\xcc\x84\0\xe1"
      u8"\xbd\xb0\xcd\x85\xce\xb1\xcd\x85\0\xce\xac\xcd\x85\0\0\0\0\0\0\xce\xb1\xcd\x82\0\xe1\xbe\xb6\xcd\x85"
      u8"\xce\x91\xcc\x86\0\xce\x91\xcc\x84\0\xce\x91\xcc\x80\0\xce\x86\0\0\0\xce\x91\xcd\x85\0\40\xcc\x93\0"
      u8"\0\xce\xb9\0\0\0\40\xcc\x93\0\0"

      // Start of 0x1fc0:
      u8"\40\xcd\x82\0\0\xc2\xa8\xcd\x82\0\xe1\xbd\xb4\xcd\x85\xce\xb7\xcd\x85\0\xce\xae\xcd\x85\0\0\0\0\0\0"
      u8"\xce\xb7\xcd\x82\0\xe1\xbf\x86\xcd\x85\xce\x95\xcc\x80\0\xce\x88\0\0\0\xce\x97\xcc\x80\0\xce\x89\0\0"
      u8"\0\xce\x97\xcd\x85\0\xe1\xbe\xbf\xcc\x80\xe1\xbe\xbf\xcc\x81\xe1\xbe\xbf\xcd\x82\xce\xb9\xcc\x86\0"
      u8"\xce\xb9\xcc\x84\0\xcf\x8a\xcc\x80\0\xce\x90\0\0\0\0\0\0\0\0\0\0\0\0\0\xce\xb9\xcd\x82\0\xcf\x8a\xcd"
      u8"\x82\0\xce\x99\xcc\x86\0\xce\x99\xcc\x84\0\xce\x99\xcc\x80\0\xce\x8a\0\0\0\0\0\0\0\0\xe1\xbf\xbe\xcc"
      u8"\x80\xe1\xbf\xbe\xcc\x81\xe1\xbf\xbe\xcd\x82\xcf\x85\xcc\x86\0\xcf\x85\xcc\x84\0\xcf\x8b\xcc\x80\0"
      u8"\xce\xb0\0\0\0\xcf\x81\xcc\x93\0\xcf\x81\xcc\x94\0\xcf\x85\xcd\x82\0\xcf\x8b\xcd\x82\0\xce\xa5\xcc"
      u8"\x86\0\xce\xa5\xcc\x84\0\xce\xa5\xcc\x80\0\xce\x8e\0\0\0\xce\xa1\xcc\x94\0\xc2\xa8\xcc\x80\0\xce\x85"
      u8"\0\0\0\x60\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xbd\xbc\xcd\x85\xcf\x89\xcd\x85\0\xcf\x8e\xcd\x85\0\0\0\0"
      u8"\0\0\xcf\x89\xcd\x82\0\xe1\xbf\xb6\xcd\x85\xce\x9f\xcc\x80\0\xce\x8c\0\0\0\xce\xa9\xcc\x80\0\xce\x8f"
      u8"\0\0\0\xce\xa9\xcd\x85\0\xc2\xb4\0\0\0\40\xcc\x94\0\0\0\0\0\0\0"

      // Start of 0x2000:
      u8"\xe2\x80\x82\0\0\0\0\0\0\xe2\x80\x83\0\0\0\0\0\0\40\0\0\0\0\0\0\0\0\40\0\0\0\0\0\0\0\0\40\0\0\0\0\0"
      u8"\0\0\0\40\0\0\0\0\0\0\0\0\40\0\0\0\0\0\0\0\0\40\0\0\0\0\0\0\0\0\40\0\0\0\0\0\0\0\0\40\0\0\0\0\0\0\0"
      u8"\0\40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x80\x90\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\40\xcc\xb3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\56\0\0\0\0\0\0"
      u8"\0\0\56\56\0\0\0\0\0\0\0\56\56\56\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\40\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x80\xb2\xe2\x80\xb2\0\0\0\xe2"
      u8"\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\0\0\0\0\0\0\0\0\0\xe2\x80\xb5\xe2\x80\xb5\0\0\0\xe2\x80\xb5\xe2\x80"
      u8"\xb5\xe2\x80\xb5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\41\41\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\40\xcc\x85"

      // Start of 0x2040:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\77\77\0\0\0\0\0\0\0\0\0\0\77\41"
      u8"\0\0\0\0\0\0\0\0\0\0\41\77\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\xe2"
      u8"\x80\xb2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\40\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\60\0\0\0\0\0"
      u8"\0\0\0\0\0\0\x69\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\64\0\0\0\0\0"
      u8"\0\0\0\0\0\0\65\0\0\0\0\0\0\0\0\0\0\0\66\0\0\0\0\0\0\0\0\0\0\0\67\0\0\0\0\0\0\0\0\0\0\0\70\0\0\0\0\0"
      u8"\0\0\0\0\0\0\71\0\0\0\0\0\0\0\0\0\0\0\53\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\x92\0\0\0\0\0\0\0\0\0\75\0\0"
      u8"\0\0\0\0\0\0\0\0\0\50\0\0\0\0\0\0\0\0\0\0\0\51\0\0\0\0\0\0\0\0\0\0\0\x6e\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2080:
      u8"\60\0\0\61\0\0\62\0\0\63\0\0\64\0\0\65\0\0\66\0\0\67\0\0\70\0\0\71\0\0\53\0\0\xe2\x88\x92\75\0\0\50"
      u8"\0\0\51\0\0\0\0\0\x61\0\0\x65\0\0\x6f\0\0\x78\0\0\xc9\x99\0\x68\0\0\x6b\0\0\x6c\0\0\x6d\0\0\x6e\0\0"
      u8"\x70\0\0\x73\0\0\x74\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x52\x73\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2100:
      u8"\x61\57\x63\x61\57\x73\x43\0\0\xc2\xb0\x43\0\0\0\x63\57\x6f\x63\57\x75\xc6\x90\0\0\0\0\xc2\xb0\x46"
      u8"\x67\0\0\x48\0\0\x48\0\0\x48\0\0\x68\0\0\xc4\xa7\0\x49\0\0\x49\0\0\x4c\0\0\x6c\0\0\0\0\0\x4e\0\0\x4e"
      u8"\x6f\0\0\0\0\0\0\0\x50\0\0\x51\0\0\x52\0\0\x52\0\0\x52\0\0\0\0\0\0\0\0\x53\x4d\0\x54\x45\x4c\x54\x4d"
      u8"\0\0\0\0\x5a\0\0\0\0\0\xce\xa9\0\0\0\0\x5a\0\0\0\0\0\x4b\0\0\xc3\x85\0\x42\0\0\x43\0\0\0\0\0\x65\0\0"
      u8"\x45\0\0\x46\0\0\0\0\0\x4d\0\0\x6f\0\0\xd7\x90\0\xd7\x91\0\xd7\x92\0\xd7\x93\0\x69\0\0\0\0\0\x46\x41"
      u8"\x58\xcf\x80\0\xce\xb3\0\xce\x93\0\xce\xa0\0"

      // Start of 0x2140:
      u8"\xe2\x88\x91\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x44\0\0\0\0\0\x64\0\0\0\0\0\x65\0"
      u8"\0\0\0\0\x69\0\0\0\0\0\x6a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\61\xe2\x81\x84\67\0\61\xe2\x81\x84\71\0\61\xe2\x81\x84\61\60\61\xe2\x81\x84\63\0\62\xe2\x81"
      u8"\x84\63\0\61\xe2\x81\x84\65\0\62\xe2\x81\x84\65\0\63\xe2\x81\x84\65\0\64\xe2\x81\x84\65\0\61\xe2\x81"
      u8"\x84\66\0\65\xe2\x81\x84\66\0\61\xe2\x81\x84\70\0\63\xe2\x81\x84\70\0\65\xe2\x81\x84\70\0\67\xe2\x81"
      u8"\x84\70\0\61\xe2\x81\x84\0\0\x49\0\0\0\0\0\x49\x49\0\0\0\0\x49\x49\x49\0\0\0\x49\x56\0\0\0\0\x56\0\0"
      u8"\0\0\0\x56\x49\0\0\0\0\x56\x49\x49\0\0\0\x56\x49\x49\x49\0\0\x49\x58\0\0\0\0\x58\0\0\0\0\0\x58\x49\0"
      u8"\0\0\0\x58\x49\x49\0\0\0\x4c\0\0\0\0\0\x43\0\0\0\0\0\x44\0\0\0\0\0\x4d\0\0\0\0\0\x69\0\0\0\0\0\x69"
      u8"\x69\0\0\0\0\x69\x69\x69\0\0\0\x69\x76\0\0\0\0\x76\0\0\0\0\0\x76\x69\0\0\0\0\x76\x69\x69\0\0\0\x76"
      u8"\x69\x69\x69\0\0\x69\x78\0\0\0\0\x78\0\0\0\0\0\x78\x69\0\0\0\0\x78\x69\x69\0\0\0\x6c\0\0\0\0\0\x63\0"
      u8"\0\0\0\0\x64\0\0\0\0\0\x6d"

      // Start of 0x2180:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\60\xe2"
      u8"\x81\x84\63\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x86\x90\xcc\xb8\xe2\x86"
      u8"\x92\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x86"
      u8"\x94\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x21c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x87\x90\xcc\xb8\xe2\x87\x94\xcc\xb8\xe2\x87\x92\xcc\xb8\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2200:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\x83\xcc\xb8\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\x88\xcc\xb8\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\x8b\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\xa3\xcc\xb8\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\xa5\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\xab\xe2\x88\xab\0\0\0\xe2\x88\xab\xe2\x88\xab\xe2"
      u8"\x88\xab\0\0\0\0\0\0\0\0\0\xe2\x88\xae\xe2\x88\xae\0\0\0\xe2\x88\xae\xe2\x88\xae\xe2\x88\xae\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2240:
      u8"\0\0\0\0\0\xe2\x88\xbc\xcc\xb8\0\0\0\0\0\0\0\0\0\0\xe2\x89\x83\xcc\xb8\0\0\0\0\0\0\0\0\0\0\xe2\x89"
      u8"\x85\xcc\xb8\0\0\0\0\0\xe2\x89\x88\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\75\xcc\xb8\0\0\0\0\0\0\0\xe2\x89\xa1"
      u8"\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xe2\x89\x8d\xcc\xb8\74\xcc\xb8\0\0\76\xcc\xb8\0\0\xe2\x89\xa4\xcc\xb8\xe2\x89\xa5\xcc\xb8\0"
      u8"\0\0\0\0\0\0\0\0\0\xe2\x89\xb2\xcc\xb8\xe2\x89\xb3\xcc\xb8\0\0\0\0\0\0\0\0\0\0\xe2\x89\xb6\xcc\xb8"
      u8"\xe2\x89\xb7\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2280:
      u8"\xe2\x89\xba\xcc\xb8\xe2\x89\xbb\xcc\xb8\0\0\0\0\0\0\0\0\0\0\xe2\x8a\x82\xcc\xb8\xe2\x8a\x83\xcc\xb8"
      u8"\0\0\0\0\0\0\0\0\0\0\xe2\x8a\x86\xcc\xb8\xe2\x8a\x87\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe2\x8a\xa2\xcc\xb8\xe2\x8a\xa8\xcc\xb8\xe2\x8a\xa9\xcc\xb8\xe2\x8a\xab\xcc\xb8"

      // Start of 0x22c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xe2\x89\xbc\xcc\xb8\xe2\x89\xbd\xcc\xb8\xe2\x8a\x91\xcc\xb8\xe2\x8a\x92\xcc\xb8"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x8a\xb2\xcc\xb8\xe2\x8a\xb3\xcc\xb8"
      u8"\xe2\x8a\xb4\xcc\xb8\xe2\x8a\xb5\xcc\xb8"

      // Start of 0x2300:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x80\x88\xe3\x80\x89"

      // Start of 0x2440:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\61\0\0\0\62\0\0\0\63\0\0\0\64\0\0\0\65\0\0"
      u8"\0\66\0\0\0\67\0\0\0\70\0\0\0\71\0\0\0\61\60\0\0\61\61\0\0\61\62\0\0\61\63\0\0\61\64\0\0\61\65\0\0"
      u8"\61\66\0\0\61\67\0\0\61\70\0\0\61\71\0\0\62\60\0\0\50\61\51\0\50\62\51\0\50\63\51\0\50\64\51\0\50\65"
      u8"\51\0\50\66\51\0\50\67\51\0\50\70\51\0\50\71\51\0\50\61\60\51\50\61\61\51\50\61\62\51"

      // Start of 0x2480:
      u8"\50\61\63\51\50\61\64\51\50\61\65\51\50\61\66\51\50\61\67\51\50\61\70\51\50\61\71\51\50\62\60\51\61"
      u8"\56\0\0\62\56\0\0\63\56\0\0\64\56\0\0\65\56\0\0\66\56\0\0\67\56\0\0\70\56\0\0\71\56\0\0\61\60\56\0"
      u8"\61\61\56\0\61\62\56\0\61\63\56\0\61\64\56\0\61\65\56\0\61\66\56\0\61\67\56\0\61\70\56\0\61\71\56\0"
      u8"\62\60\56\0\50\x61\51\0\50\x62\51\0\50\x63\51\0\50\x64\51\0\50\x65\51\0\50\x66\51\0\50\x67\51\0\50"
      u8"\x68\51\0\50\x69\51\0\50\x6a\51\0\50\x6b\51\0\50\x6c\51\0\50\x6d\51\0\50\x6e\51\0\50\x6f\51\0\50\x70"
      u8"\51\0\50\x71\51\0\50\x72\51\0\50\x73\51\0\50\x74\51\0\50\x75\51\0\50\x76\51\0\50\x77\51\0\50\x78\51"
      u8"\0\50\x79\51\0\50\x7a\51\0\x41\0\0\0\x42\0\0\0\x43\0\0\0\x44\0\0\0\x45\0\0\0\x46\0\0\0\x47\0\0\0\x48"
      u8"\0\0\0\x49\0\0\0\x4a\0\0\0"

      // Start of 0x24c0:
      u8"\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x61\x62\x63\x64\x65\x66\x67\x68\x69"
      u8"\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\60"

      // Start of 0x2a00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\xab"
      u8"\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2a40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\72\72\75\75\75\0\75\75\75"

      // Start of 0x2ac0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\xab\x9d\xcc\xb8"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2c40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\x6a\x56"

      // Start of 0x2d40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\xb5\xa1"

      // Start of 0x2e80:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe6\xaf\x8d"

      // Start of 0x2ec0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\xe9\xbe\x9f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2f00:
      u8"\xe4\xb8\x80\xe4\xb8\xa8\xe4\xb8\xb6\xe4\xb8\xbf\xe4\xb9\x99\xe4\xba\x85\xe4\xba\x8c\xe4\xba\xa0\xe4"
      u8"\xba\xba\xe5\x84\xbf\xe5\x85\xa5\xe5\x85\xab\xe5\x86\x82\xe5\x86\x96\xe5\x86\xab\xe5\x87\xa0\xe5\x87"
      u8"\xb5\xe5\x88\x80\xe5\x8a\x9b\xe5\x8b\xb9\xe5\x8c\x95\xe5\x8c\x9a\xe5\x8c\xb8\xe5\x8d\x81\xe5\x8d\x9c"
      u8"\xe5\x8d\xa9\xe5\x8e\x82\xe5\x8e\xb6\xe5\x8f\x88\xe5\x8f\xa3\xe5\x9b\x97\xe5\x9c\x9f\xe5\xa3\xab\xe5"
      u8"\xa4\x82\xe5\xa4\x8a\xe5\xa4\x95\xe5\xa4\xa7\xe5\xa5\xb3\xe5\xad\x90\xe5\xae\x80\xe5\xaf\xb8\xe5\xb0"
      u8"\x8f\xe5\xb0\xa2\xe5\xb0\xb8\xe5\xb1\xae\xe5\xb1\xb1\xe5\xb7\x9b\xe5\xb7\xa5\xe5\xb7\xb1\xe5\xb7\xbe"
      u8"\xe5\xb9\xb2\xe5\xb9\xba\xe5\xb9\xbf\xe5\xbb\xb4\xe5\xbb\xbe\xe5\xbc\x8b\xe5\xbc\x93\xe5\xbd\x90\xe5"
      u8"\xbd\xa1\xe5\xbd\xb3\xe5\xbf\x83\xe6\x88\x88\xe6\x88\xb6\xe6\x89\x8b"

      // Start of 0x2f40:
      u8"\xe6\x94\xaf\xe6\x94\xb4\xe6\x96\x87\xe6\x96\x97\xe6\x96\xa4\xe6\x96\xb9\xe6\x97\xa0\xe6\x97\xa5\xe6"
      u8"\x9b\xb0\xe6\x9c\x88\xe6\x9c\xa8\xe6\xac\xa0\xe6\xad\xa2\xe6\xad\xb9\xe6\xae\xb3\xe6\xaf\x8b\xe6\xaf"
      u8"\x94\xe6\xaf\x9b\xe6\xb0\x8f\xe6\xb0\x94\xe6\xb0\xb4\xe7\x81\xab\xe7\x88\xaa\xe7\x88\xb6\xe7\x88\xbb"
      u8"\xe7\x88\xbf\xe7\x89\x87\xe7\x89\x99\xe7\x89\x9b\xe7\x8a\xac\xe7\x8e\x84\xe7\x8e\x89\xe7\x93\x9c\xe7"
      u8"\x93\xa6\xe7\x94\x98\xe7\x94\x9f\xe7\x94\xa8\xe7\x94\xb0\xe7\x96\x8b\xe7\x96\x92\xe7\x99\xb6\xe7\x99"
      u8"\xbd\xe7\x9a\xae\xe7\x9a\xbf\xe7\x9b\xae\xe7\x9f\x9b\xe7\x9f\xa2\xe7\x9f\xb3\xe7\xa4\xba\xe7\xa6\xb8"
      u8"\xe7\xa6\xbe\xe7\xa9\xb4\xe7\xab\x8b\xe7\xab\xb9\xe7\xb1\xb3\xe7\xb3\xb8\xe7\xbc\xb6\xe7\xbd\x91\xe7"
      u8"\xbe\x8a\xe7\xbe\xbd\xe8\x80\x81\xe8\x80\x8c\xe8\x80\x92\xe8\x80\xb3"

      // Start of 0x2f80:
      u8"\xe8\x81\xbf\xe8\x82\x89\xe8\x87\xa3\xe8\x87\xaa\xe8\x87\xb3\xe8\x87\xbc\xe8\x88\x8c\xe8\x88\x9b\xe8"
      u8"\x88\x9f\xe8\x89\xae\xe8\x89\xb2\xe8\x89\xb8\xe8\x99\x8d\xe8\x99\xab\xe8\xa1\x80\xe8\xa1\x8c\xe8\xa1"
      u8"\xa3\xe8\xa5\xbe\xe8\xa6\x8b\xe8\xa7\x92\xe8\xa8\x80\xe8\xb0\xb7\xe8\xb1\x86\xe8\xb1\x95\xe8\xb1\xb8"
      u8"\xe8\xb2\x9d\xe8\xb5\xa4\xe8\xb5\xb0\xe8\xb6\xb3\xe8\xba\xab\xe8\xbb\x8a\xe8\xbe\x9b\xe8\xbe\xb0\xe8"
      u8"\xbe\xb5\xe9\x82\x91\xe9\x85\x89\xe9\x87\x86\xe9\x87\x8c\xe9\x87\x91\xe9\x95\xb7\xe9\x96\x80\xe9\x98"
      u8"\x9c\xe9\x9a\xb6\xe9\x9a\xb9\xe9\x9b\xa8\xe9\x9d\x91\xe9\x9d\x9e\xe9\x9d\xa2\xe9\x9d\xa9\xe9\x9f\x8b"
      u8"\xe9\x9f\xad\xe9\x9f\xb3\xe9\xa0\x81\xe9\xa2\xa8\xe9\xa3\x9b\xe9\xa3\x9f\xe9\xa6\x96\xe9\xa6\x99\xe9"
      u8"\xa6\xac\xe9\xaa\xa8\xe9\xab\x98\xe9\xab\x9f\xe9\xac\xa5\xe9\xac\xaf"

      // Start of 0x2fc0:
      u8"\xe9\xac\xb2\xe9\xac\xbc\xe9\xad\x9a\xe9\xb3\xa5\xe9\xb9\xb5\xe9\xb9\xbf\xe9\xba\xa5\xe9\xba\xbb\xe9"
      u8"\xbb\x83\xe9\xbb\x8d\xe9\xbb\x91\xe9\xbb\xb9\xe9\xbb\xbd\xe9\xbc\x8e\xe9\xbc\x93\xe9\xbc\xa0\xe9\xbc"
      u8"\xbb\xe9\xbd\x8a\xe9\xbd\x92\xe9\xbe\x8d\xe9\xbe\x9c\xe9\xbe\xa0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0"

      // Start of 0x3000:
      u8"\40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x80\x92\0\0\0\xe5\x8d\x81\xe5\x8d\x84\xe5\x8d\x85"

      // Start of 0x3040:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x81\x8b\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\x8d\xe3\x82"
      u8"\x99\0\0\0\0\0\0\xe3\x81\x8f\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\x91\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\x93"
      u8"\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\x95\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\x97\xe3\x82\x99\0\0\0\0\0\0\xe3"
      u8"\x81\x99\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\x9b\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\x9d\xe3\x82\x99\0\0\0\0"
      u8"\0\0\xe3\x81\x9f\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\xa1\xe3\x82\x99\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x81\xa4"
      u8"\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\xa6\xe3\x82\x99\0\0\0\0\0\0\xe3\x81\xa8\xe3\x82\x99\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x81\xaf\xe3\x82\x99\xe3\x81\xaf\xe3\x82"
      u8"\x9a\0\0\0\0\0\0\xe3\x81\xb2\xe3\x82\x99\xe3\x81\xb2\xe3\x82\x9a\0\0\0\0\0\0\xe3\x81\xb5\xe3\x82\x99"
      u8"\xe3\x81\xb5\xe3\x82\x9a\0\0\0\0\0\0\xe3\x81\xb8\xe3\x82\x99\xe3\x81\xb8\xe3\x82\x9a\0\0\0\0\0\0\xe3"
      u8"\x81\xbb\xe3\x82\x99\xe3\x81\xbb\xe3\x82\x9a"

      // Start of 0x3080:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x81\x86\xe3\x82\x99\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\40\xe3\x82\x99\0\0\40\xe3\x82\x9a\0\0\0\0\0\0\0\0\xe3\x82\x9d"
      u8"\xe3\x82\x99\xe3\x82\x88\xe3\x82\x8a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x82\xab\xe3\x82"
      u8"\x99\0\0\0\0\0\0\xe3\x82\xad\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xaf\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xb1"
      u8"\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xb3\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xb5\xe3\x82\x99\0\0\0\0\0\0\xe3"
      u8"\x82\xb7\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xb9\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xbb\xe3\x82\x99\0\0\0\0"
      u8"\0\0\xe3\x82\xbd\xe3\x82\x99\0\0\0\0\0\0"

      // Start of 0x30c0:
      u8"\xe3\x82\xbf\xe3\x82\x99\0\0\0\0\0\0\xe3\x83\x81\xe3\x82\x99\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x83\x84\xe3"
      u8"\x82\x99\0\0\0\0\0\0\xe3\x83\x86\xe3\x82\x99\0\0\0\0\0\0\xe3\x83\x88\xe3\x82\x99\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x83\x8f\xe3\x82\x99\xe3\x83\x8f\xe3\x82\x9a"
      u8"\0\0\0\0\0\0\xe3\x83\x92\xe3\x82\x99\xe3\x83\x92\xe3\x82\x9a\0\0\0\0\0\0\xe3\x83\x95\xe3\x82\x99\xe3"
      u8"\x83\x95\xe3\x82\x9a\0\0\0\0\0\0\xe3\x83\x98\xe3\x82\x99\xe3\x83\x98\xe3\x82\x9a\0\0\0\0\0\0\xe3\x83"
      u8"\x9b\xe3\x82\x99\xe3\x83\x9b\xe3\x82\x9a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\xe3\x82\xa6\xe3\x82\x99\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x83\xaf\xe3\x82\x99\xe3\x83\xb0\xe3\x82\x99"
      u8"\xe3\x83\xb1\xe3\x82\x99\xe3\x83\xb2\xe3\x82\x99\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x83\xbd\xe3"
      u8"\x82\x99\xe3\x82\xb3\xe3\x83\x88"

      // Start of 0x3100:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1"
      u8"\x84\x80\xe1\x84\x81\xe1\x86\xaa\xe1\x84\x82\xe1\x86\xac\xe1\x86\xad\xe1\x84\x83\xe1\x84\x84\xe1\x84"
      u8"\x85\xe1\x86\xb0\xe1\x86\xb1\xe1\x86\xb2\xe1\x86\xb3\xe1\x86\xb4\xe1\x86\xb5"

      // Start of 0x3140:
      u8"\xe1\x84\x9a\xe1\x84\x86\xe1\x84\x87\xe1\x84\x88\xe1\x84\xa1\xe1\x84\x89\xe1\x84\x8a\xe1\x84\x8b\xe1"
      u8"\x84\x8c\xe1\x84\x8d\xe1\x84\x8e\xe1\x84\x8f\xe1\x84\x90\xe1\x84\x91\xe1\x84\x92\xe1\x85\xa1\xe1\x85"
      u8"\xa2\xe1\x85\xa3\xe1\x85\xa4\xe1\x85\xa5\xe1\x85\xa6\xe1\x85\xa7\xe1\x85\xa8\xe1\x85\xa9\xe1\x85\xaa"
      u8"\xe1\x85\xab\xe1\x85\xac\xe1\x85\xad\xe1\x85\xae\xe1\x85\xaf\xe1\x85\xb0\xe1\x85\xb1\xe1\x85\xb2\xe1"
      u8"\x85\xb3\xe1\x85\xb4\xe1\x85\xb5\xe1\x85\xa0\xe1\x84\x94\xe1\x84\x95\xe1\x87\x87\xe1\x87\x88\xe1\x87"
      u8"\x8c\xe1\x87\x8e\xe1\x87\x93\xe1\x87\x97\xe1\x87\x99\xe1\x84\x9c\xe1\x87\x9d\xe1\x87\x9f\xe1\x84\x9d"
      u8"\xe1\x84\x9e\xe1\x84\xa0\xe1\x84\xa2\xe1\x84\xa3\xe1\x84\xa7\xe1\x84\xa9\xe1\x84\xab\xe1\x84\xac\xe1"
      u8"\x84\xad\xe1\x84\xae\xe1\x84\xaf\xe1\x84\xb2\xe1\x84\xb6\xe1\x85\x80"

      // Start of 0x3180:
      u8"\xe1\x85\x87\xe1\x85\x8c\xe1\x87\xb1\xe1\x87\xb2\xe1\x85\x97\xe1\x85\x98\xe1\x85\x99\xe1\x86\x84\xe1"
      u8"\x86\x85\xe1\x86\x88\xe1\x86\x91\xe1\x86\x92\xe1\x86\x94\xe1\x86\x9e\xe1\x86\xa1\0\0\0\0\0\0\0\0\0"
      u8"\xe4\xb8\x80\xe4\xba\x8c\xe4\xb8\x89\xe5\x9b\x9b\xe4\xb8\x8a\xe4\xb8\xad\xe4\xb8\x8b\xe7\x94\xb2\xe4"
      u8"\xb9\x99\xe4\xb8\x99\xe4\xb8\x81\xe5\xa4\xa9\xe5\x9c\xb0\xe4\xba\xba\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x3200:
      u8"\50\xe1\x84\x80\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x82\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x83"
      u8"\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x85\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x86\51\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\50\xe1\x84\x87\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x89\51\0\0\0\0\0\0\0\0\0\0\0\0\50"
      u8"\xe1\x84\x8b\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x8c\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x8e\51"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x8f\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x90\51\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\50\xe1\x84\x91\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1\x84\x92\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe1"
      u8"\x84\x80\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x82\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84"
      u8"\x83\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x85\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x86"
      u8"\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x87\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x89\xe1"
      u8"\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x8b\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x8c\xe1\x85"
      u8"\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x8e\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x8f\xe1\x85\xa1"
      u8"\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x90\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x91\xe1\x85\xa1\51"
      u8"\0\0\0\0\0\0\0\0\0\50\xe1\x84\x92\xe1\x85\xa1\51\0\0\0\0\0\0\0\0\0\50\xe1\x84\x8c\xe1\x85\xae\51\0\0"
      u8"\0\0\0\0\0\0\0\50\xe1\x84\x8b\xe1\x85\xa9\xe1\x84\x8c\xe1\x85\xa5\xe1\x86\xab\51\50\xe1\x84\x8b\xe1"
      u8"\x85\xa9\xe1\x84\x92\xe1\x85\xae\51\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\50\xe4\xb8\x80\51\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\50\xe4\xba\x8c\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe4\xb8\x89\51\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\50\xe5\x9b\x9b\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe4\xba\x94\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe5\x85"
      u8"\xad\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe4\xb8\x83\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe5\x85\xab\51\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\50\xe4\xb9\x9d\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe5\x8d\x81\51\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\50\xe6\x9c\x88\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe7\x81\xab\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe6\xb0\xb4"
      u8"\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe6\x9c\xa8\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe9\x87\x91\51\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\50\xe5\x9c\x9f\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe6\x97\xa5\51\0\0\0\0\0\0\0\0\0\0\0\0\50"
      u8"\xe6\xa0\xaa\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe6\x9c\x89\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe7\xa4\xbe\51"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\50\xe5\x90\x8d\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe7\x89\xb9\51\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\50\xe8\xb2\xa1\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe7\xa5\x9d\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe5"
      u8"\x8a\xb4\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe4\xbb\xa3\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe5\x91\xbc\51\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\50\xe5\xad\xa6\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe7\x9b\xa3\51\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\50\xe4\xbc\x81\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe8\xb3\x87\51\0\0\0\0\0\0\0\0\0\0\0\0\50\xe5\x8d"
      u8"\x94\51\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x3240:
      u8"\50\xe7\xa5\xad\51\0\0\0\0\0\0\0\0\0\0\50\xe4\xbc\x91\51\0\0\0\0\0\0\0\0\0\0\50\xe8\x87\xaa\51\0\0\0"
      u8"\0\0\0\0\0\0\0\50\xe8\x87\xb3\51\0\0\0\0\0\0\0\0\0\0\xe5\x95\x8f\0\0\0\0\0\0\0\0\0\0\0\0\xe5\xb9\xbc"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe6\x96\x87\0\0\0\0\0\0\0\0\0\0\0\0\xe7\xae\x8f\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x50\x54\x45\0\0\0\0\0\0\0\0\0\0\0\0\62\61\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\62\62\0\0\0\0\0\0\0\0\0\0\0\0\0\62\63\0\0\0\0\0\0\0\0\0\0\0\0\0\62\64\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\62\65\0\0\0\0\0\0\0\0\0\0\0\0\0\62\66\0\0\0\0\0\0\0\0\0\0\0\0\0\62\67\0\0\0\0\0\0\0\0\0\0\0\0\0\62"
      u8"\70\0\0\0\0\0\0\0\0\0\0\0\0\0\62\71\0\0\0\0\0\0\0\0\0\0\0\0\0\63\60\0\0\0\0\0\0\0\0\0\0\0\0\0\63\61"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\63\62\0\0\0\0\0\0\0\0\0\0\0\0\0\63\63\0\0\0\0\0\0\0\0\0\0\0\0\0\63\64\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\63\65\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x80\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84"
      u8"\x82\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x83\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x85\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe1\x84\x86\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x87\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x89\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xe1\x84\x8b\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x8c\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x8e\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\xe1\x84\x8f\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x90\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x91"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x92\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x84\x80\xe1\x85\xa1\0\0\0\0\0\0\0\0"
      u8"\0\xe1\x84\x82\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x83\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x85"
      u8"\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x86\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x87\xe1\x85\xa1\0\0"
      u8"\0\0\0\0\0\0\0\xe1\x84\x89\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x8b\xe1\x85\xa1\0\0\0\0\0\0\0\0\0"
      u8"\xe1\x84\x8c\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x8e\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x8f\xe1"
      u8"\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x90\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x91\xe1\x85\xa1\0\0\0\0"
      u8"\0\0\0\0\0\xe1\x84\x92\xe1\x85\xa1\0\0\0\0\0\0\0\0\0\xe1\x84\x8e\xe1\x85\xa1\xe1\x86\xb7\xe1\x84\x80"
      u8"\xe1\x85\xa9\xe1\x84\x8c\xe1\x85\xae\xe1\x84\x8b\xe1\x85\xb4\0\0\0\xe1\x84\x8b\xe1\x85\xae\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x3280:
      u8"\xe4\xb8\x80\xe4\xba\x8c\xe4\xb8\x89\xe5\x9b\x9b\xe4\xba\x94\xe5\x85\xad\xe4\xb8\x83\xe5\x85\xab\xe4"
      u8"\xb9\x9d\xe5\x8d\x81\xe6\x9c\x88\xe7\x81\xab\xe6\xb0\xb4\xe6\x9c\xa8\xe9\x87\x91\xe5\x9c\x9f\xe6\x97"
      u8"\xa5\xe6\xa0\xaa\xe6\x9c\x89\xe7\xa4\xbe\xe5\x90\x8d\xe7\x89\xb9\xe8\xb2\xa1\xe7\xa5\x9d\xe5\x8a\xb4"
      u8"\xe7\xa7\x98\xe7\x94\xb7\xe5\xa5\xb3\xe9\x81\xa9\xe5\x84\xaa\xe5\x8d\xb0\xe6\xb3\xa8\xe9\xa0\x85\xe4"
      u8"\xbc\x91\xe5\x86\x99\xe6\xad\xa3\xe4\xb8\x8a\xe4\xb8\xad\xe4\xb8\x8b\xe5\xb7\xa6\xe5\x8f\xb3\xe5\x8c"
      u8"\xbb\xe5\xae\x97\xe5\xad\xa6\xe7\x9b\xa3\xe4\xbc\x81\xe8\xb3\x87\xe5\x8d\x94\xe5\xa4\x9c\63\66\0\63"
      u8"\67\0\63\70\0\63\71\0\64\60\0\64\61\0\64\62\0\64\63\0\64\64\0\64\65\0\64\66\0\64\67\0\64\70\0\64\71"
      u8"\0\65\60\0"

      // Start of 0x32c0:
      u8"\61\xe6\x9c\x88\0\0\62\xe6\x9c\x88\0\0\63\xe6\x9c\x88\0\0\64\xe6\x9c\x88\0\0\65\xe6\x9c\x88\0\0\66"
      u8"\xe6\x9c\x88\0\0\67\xe6\x9c\x88\0\0\70\xe6\x9c\x88\0\0\71\xe6\x9c\x88\0\0\61\60\xe6\x9c\x88\0\61\61"
      u8"\xe6\x9c\x88\0\61\62\xe6\x9c\x88\0\x48\x67\0\0\0\0\x65\x72\x67\0\0\0\x65\x56\0\0\0\0\x4c\x54\x44\0\0"
      u8"\0\xe3\x82\xa2\0\0\0\xe3\x82\xa4\0\0\0\xe3\x82\xa6\0\0\0\xe3\x82\xa8\0\0\0\xe3\x82\xaa\0\0\0\xe3\x82"
      u8"\xab\0\0\0\xe3\x82\xad\0\0\0\xe3\x82\xaf\0\0\0\xe3\x82\xb1\0\0\0\xe3\x82\xb3\0\0\0\xe3\x82\xb5\0\0\0"
      u8"\xe3\x82\xb7\0\0\0\xe3\x82\xb9\0\0\0\xe3\x82\xbb\0\0\0\xe3\x82\xbd\0\0\0\xe3\x82\xbf\0\0\0\xe3\x83"
      u8"\x81\0\0\0\xe3\x83\x84\0\0\0\xe3\x83\x86\0\0\0\xe3\x83\x88\0\0\0\xe3\x83\x8a\0\0\0\xe3\x83\x8b\0\0\0"
      u8"\xe3\x83\x8c\0\0\0\xe3\x83\x8d\0\0\0\xe3\x83\x8e\0\0\0\xe3\x83\x8f\0\0\0\xe3\x83\x92\0\0\0\xe3\x83"
      u8"\x95\0\0\0\xe3\x83\x98\0\0\0\xe3\x83\x9b\0\0\0\xe3\x83\x9e\0\0\0\xe3\x83\x9f\0\0\0\xe3\x83\xa0\0\0\0"
      u8"\xe3\x83\xa1\0\0\0\xe3\x83\xa2\0\0\0\xe3\x83\xa4\0\0\0\xe3\x83\xa6\0\0\0\xe3\x83\xa8\0\0\0\xe3\x83"
      u8"\xa9\0\0\0\xe3\x83\xaa\0\0\0\xe3\x83\xab\0\0\0\xe3\x83\xac\0\0\0\xe3\x83\xad\0\0\0\xe3\x83\xaf\0\0\0"
      u8"\xe3\x83\xb0\0\0\0\xe3\x83\xb1\0\0\0\xe3\x83\xb2\0\0\0\xe4\xbb\xa4\xe5\x92\x8c"

      // Start of 0x3300:
      u8"\xe3\x82\xa2\xe3\x83\x91\xe3\x83\xbc\xe3\x83\x88\0\0\0\0\0\0\xe3\x82\xa2\xe3\x83\xab\xe3\x83\x95\xe3"
      u8"\x82\xa1\0\0\0\0\0\0\xe3\x82\xa2\xe3\x83\xb3\xe3\x83\x9a\xe3\x82\xa2\0\0\0\0\0\0\xe3\x82\xa2\xe3\x83"
      u8"\xbc\xe3\x83\xab\0\0\0\0\0\0\0\0\0\xe3\x82\xa4\xe3\x83\x8b\xe3\x83\xb3\xe3\x82\xb0\0\0\0\0\0\0\xe3"
      u8"\x82\xa4\xe3\x83\xb3\xe3\x83\x81\0\0\0\0\0\0\0\0\0\xe3\x82\xa6\xe3\x82\xa9\xe3\x83\xb3\0\0\0\0\0\0\0"
      u8"\0\0\xe3\x82\xa8\xe3\x82\xb9\xe3\x82\xaf\xe3\x83\xbc\xe3\x83\x89\0\0\0\xe3\x82\xa8\xe3\x83\xbc\xe3"
      u8"\x82\xab\xe3\x83\xbc\0\0\0\0\0\0\xe3\x82\xaa\xe3\x83\xb3\xe3\x82\xb9\0\0\0\0\0\0\0\0\0\xe3\x82\xaa"
      u8"\xe3\x83\xbc\xe3\x83\xa0\0\0\0\0\0\0\0\0\0\xe3\x82\xab\xe3\x82\xa4\xe3\x83\xaa\0\0\0\0\0\0\0\0\0\xe3"
      u8"\x82\xab\xe3\x83\xa9\xe3\x83\x83\xe3\x83\x88\0\0\0\0\0\0\xe3\x82\xab\xe3\x83\xad\xe3\x83\xaa\xe3\x83"
      u8"\xbc\0\0\0\0\0\0\xe3\x82\xac\xe3\x83\xad\xe3\x83\xb3\0\0\0\0\0\0\0\0\0\xe3\x82\xac\xe3\x83\xb3\xe3"
      u8"\x83\x9e\0\0\0\0\0\0\0\0\0\xe3\x82\xae\xe3\x82\xac\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x82\xae\xe3\x83\x8b"
      u8"\xe3\x83\xbc\0\0\0\0\0\0\0\0\0\xe3\x82\xad\xe3\x83\xa5\xe3\x83\xaa\xe3\x83\xbc\0\0\0\0\0\0\xe3\x82"
      u8"\xae\xe3\x83\xab\xe3\x83\x80\xe3\x83\xbc\0\0\0\0\0\0\xe3\x82\xad\xe3\x83\xad\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\0\0\0\xe3\x82\xad\xe3\x83\xad\xe3\x83"
      u8"\xa1\xe3\x83\xbc\xe3\x83\x88\xe3\x83\xab\xe3\x82\xad\xe3\x83\xad\xe3\x83\xaf\xe3\x83\x83\xe3\x83\x88"
      u8"\0\0\0\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\0\0\0\0\0\0\0\0\0\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\xe3"
      u8"\x83\x88\xe3\x83\xb3\0\0\0\xe3\x82\xaf\xe3\x83\xab\xe3\x82\xbc\xe3\x82\xa4\xe3\x83\xad\0\0\0\xe3\x82"
      u8"\xaf\xe3\x83\xad\xe3\x83\xbc\xe3\x83\x8d\0\0\0\0\0\0\xe3\x82\xb1\xe3\x83\xbc\xe3\x82\xb9\0\0\0\0\0\0"
      u8"\0\0\0\xe3\x82\xb3\xe3\x83\xab\xe3\x83\x8a\0\0\0\0\0\0\0\0\0\xe3\x82\xb3\xe3\x83\xbc\xe3\x83\x9d\0\0"
      u8"\0\0\0\0\0\0\0\xe3\x82\xb5\xe3\x82\xa4\xe3\x82\xaf\xe3\x83\xab\0\0\0\0\0\0\xe3\x82\xb5\xe3\x83\xb3"
      u8"\xe3\x83\x81\xe3\x83\xbc\xe3\x83\xa0\0\0\0\xe3\x82\xb7\xe3\x83\xaa\xe3\x83\xb3\xe3\x82\xb0\0\0\0\0\0"
      u8"\0\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x81\0\0\0\0\0\0\0\0\0\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x88\0\0\0\0"
      u8"\0\0\0\0\0\xe3\x83\x80\xe3\x83\xbc\xe3\x82\xb9\0\0\0\0\0\0\0\0\0\xe3\x83\x87\xe3\x82\xb7\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe3\x83\x89\xe3\x83\xab\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x83\x88\xe3\x83\xb3\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xe3\x83\x8a\xe3\x83\x8e\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x83\x8e\xe3\x83\x83\xe3\x83\x88\0\0\0\0"
      u8"\0\0\0\0\0\xe3\x83\x8f\xe3\x82\xa4\xe3\x83\x84\0\0\0\0\0\0\0\0\0\xe3\x83\x91\xe3\x83\xbc\xe3\x82\xbb"
      u8"\xe3\x83\xb3\xe3\x83\x88\0\0\0\xe3\x83\x91\xe3\x83\xbc\xe3\x83\x84\0\0\0\0\0\0\0\0\0\xe3\x83\x90\xe3"
      u8"\x83\xbc\xe3\x83\xac\xe3\x83\xab\0\0\0\0\0\0\xe3\x83\x94\xe3\x82\xa2\xe3\x82\xb9\xe3\x83\x88\xe3\x83"
      u8"\xab\0\0\0\xe3\x83\x94\xe3\x82\xaf\xe3\x83\xab\0\0\0\0\0\0\0\0\0\xe3\x83\x94\xe3\x82\xb3\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe3\x83\x93\xe3\x83\xab\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x83\x95\xe3\x82\xa1\xe3\x83\xa9\xe3"
      u8"\x83\x83\xe3\x83\x89\0\0\0\xe3\x83\x95\xe3\x82\xa3\xe3\x83\xbc\xe3\x83\x88\0\0\0\0\0\0\xe3\x83\x96"
      u8"\xe3\x83\x83\xe3\x82\xb7\xe3\x82\xa7\xe3\x83\xab\0\0\0\xe3\x83\x95\xe3\x83\xa9\xe3\x83\xb3\0\0\0\0\0"
      u8"\0\0\0\0\xe3\x83\x98\xe3\x82\xaf\xe3\x82\xbf\xe3\x83\xbc\xe3\x83\xab\0\0\0\xe3\x83\x9a\xe3\x82\xbd\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\xe3\x83\x9a\xe3\x83\x8b\xe3\x83\x92\0\0\0\0\0\0\0\0\0\xe3\x83\x98\xe3\x83\xab"
      u8"\xe3\x83\x84\0\0\0\0\0\0\0\0\0\xe3\x83\x9a\xe3\x83\xb3\xe3\x82\xb9\0\0\0\0\0\0\0\0\0\xe3\x83\x9a\xe3"
      u8"\x83\xbc\xe3\x82\xb8\0\0\0\0\0\0\0\0\0\xe3\x83\x99\xe3\x83\xbc\xe3\x82\xbf\0\0\0\0\0\0\0\0\0\xe3\x83"
      u8"\x9d\xe3\x82\xa4\xe3\x83\xb3\xe3\x83\x88\0\0\0\0\0\0\xe3\x83\x9c\xe3\x83\xab\xe3\x83\x88\0\0\0\0\0\0"
      u8"\0\0\0\xe3\x83\x9b\xe3\x83\xb3\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x3340:
      u8"\xe3\x83\x9d\xe3\x83\xb3\xe3\x83\x89\0\0\0\0\0\0\xe3\x83\x9b\xe3\x83\xbc\xe3\x83\xab\0\0\0\0\0\0\xe3"
      u8"\x83\x9b\xe3\x83\xbc\xe3\x83\xb3\0\0\0\0\0\0\xe3\x83\x9e\xe3\x82\xa4\xe3\x82\xaf\xe3\x83\xad\0\0\0"
      u8"\xe3\x83\x9e\xe3\x82\xa4\xe3\x83\xab\0\0\0\0\0\0\xe3\x83\x9e\xe3\x83\x83\xe3\x83\x8f\0\0\0\0\0\0\xe3"
      u8"\x83\x9e\xe3\x83\xab\xe3\x82\xaf\0\0\0\0\0\0\xe3\x83\x9e\xe3\x83\xb3\xe3\x82\xb7\xe3\x83\xa7\xe3\x83"
      u8"\xb3\xe3\x83\x9f\xe3\x82\xaf\xe3\x83\xad\xe3\x83\xb3\0\0\0\xe3\x83\x9f\xe3\x83\xaa\0\0\0\0\0\0\0\0\0"
      u8"\xe3\x83\x9f\xe3\x83\xaa\xe3\x83\x90\xe3\x83\xbc\xe3\x83\xab\xe3\x83\xa1\xe3\x82\xac\0\0\0\0\0\0\0\0"
      u8"\0\xe3\x83\xa1\xe3\x82\xac\xe3\x83\x88\xe3\x83\xb3\0\0\0\xe3\x83\xa1\xe3\x83\xbc\xe3\x83\x88\xe3\x83"
      u8"\xab\0\0\0\xe3\x83\xa4\xe3\x83\xbc\xe3\x83\x89\0\0\0\0\0\0\xe3\x83\xa4\xe3\x83\xbc\xe3\x83\xab\0\0\0"
      u8"\0\0\0\xe3\x83\xa6\xe3\x82\xa2\xe3\x83\xb3\0\0\0\0\0\0\xe3\x83\xaa\xe3\x83\x83\xe3\x83\x88\xe3\x83"
      u8"\xab\0\0\0\xe3\x83\xaa\xe3\x83\xa9\0\0\0\0\0\0\0\0\0\xe3\x83\xab\xe3\x83\x94\xe3\x83\xbc\0\0\0\0\0\0"
      u8"\xe3\x83\xab\xe3\x83\xbc\xe3\x83\x96\xe3\x83\xab\0\0\0\xe3\x83\xac\xe3\x83\xa0\0\0\0\0\0\0\0\0\0\xe3"
      u8"\x83\xac\xe3\x83\xb3\xe3\x83\x88\xe3\x82\xb2\xe3\x83\xb3\xe3\x83\xaf\xe3\x83\x83\xe3\x83\x88\0\0\0\0"
      u8"\0\0\60\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\0\61\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\0\62\xe7\x82\xb9\0\0\0"
      u8"\0\0\0\0\0\0\0\0\63\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\0\64\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\0\65\xe7"
      u8"\x82\xb9\0\0\0\0\0\0\0\0\0\0\0\66\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\0\67\xe7\x82\xb9\0\0\0\0\0\0\0\0\0"
      u8"\0\0\70\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\0\71\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\0\61\60\xe7\x82\xb9\0\0"
      u8"\0\0\0\0\0\0\0\0\61\61\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\61\62\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\61\63"
      u8"\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\61\64\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\61\65\xe7\x82\xb9\0\0\0\0\0\0"
      u8"\0\0\0\0\61\66\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\61\67\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\61\70\xe7\x82"
      u8"\xb9\0\0\0\0\0\0\0\0\0\0\61\71\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\62\60\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0"
      u8"\62\61\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\62\62\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\62\63\xe7\x82\xb9\0\0\0"
      u8"\0\0\0\0\0\0\0\62\64\xe7\x82\xb9\0\0\0\0\0\0\0\0\0\0\x68\x50\x61\0\0\0\0\0\0\0\0\0\0\0\0\x64\x61\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\x41\x55\0\0\0\0\0\0\0\0\0\0\0\0\0\x62\x61\x72\0\0\0\0\0\0\0\0\0\0\0\0\x6f\x56"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\x70\x63\0\0\0\0\0\0\0\0\0\0\0\0\0\x64\x6d\0\0\0\0\0\0\0\0\0\0\0\0\0\x64"
      u8"\x6d\xc2\xb2\0\0\0\0\0\0\0\0\0\0\0\x64\x6d\xc2\xb3\0\0\0\0\0\0\0\0\0\0\0\x49\x55\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\xe5\xb9\xb3\xe6\x88\x90\0\0\0\0\0\0\0\0\0\xe6\x98\xad\xe5\x92\x8c\0\0\0\0\0\0\0\0\0\xe5\xa4"
      u8"\xa7\xe6\xad\xa3\0\0\0\0\0\0\0\0\0\xe6\x98\x8e\xe6\xb2\xbb\0\0\0\0\0\0\0\0\0\xe6\xa0\xaa\xe5\xbc\x8f"
      u8"\xe4\xbc\x9a\xe7\xa4\xbe\0\0\0"

      // Start of 0x3380:
      u8"\x70\x41\0\0\0\0\0\0\0\x6e\x41\0\0\0\0\0\0\0\xce\xbc\x41\0\0\0\0\0\0\x6d\x41\0\0\0\0\0\0\0\x6b\x41\0"
      u8"\0\0\0\0\0\0\x4b\x42\0\0\0\0\0\0\0\x4d\x42\0\0\0\0\0\0\0\x47\x42\0\0\0\0\0\0\0\x63\x61\x6c\0\0\0\0\0"
      u8"\0\x6b\x63\x61\x6c\0\0\0\0\0\x70\x46\0\0\0\0\0\0\0\x6e\x46\0\0\0\0\0\0\0\xce\xbc\x46\0\0\0\0\0\0\xce"
      u8"\xbc\x67\0\0\0\0\0\0\x6d\x67\0\0\0\0\0\0\0\x6b\x67\0\0\0\0\0\0\0\x48\x7a\0\0\0\0\0\0\0\x6b\x48\x7a\0"
      u8"\0\0\0\0\0\x4d\x48\x7a\0\0\0\0\0\0\x47\x48\x7a\0\0\0\0\0\0\x54\x48\x7a\0\0\0\0\0\0\xce\xbc\xe2\x84"
      u8"\x93\0\0\0\0\x6d\xe2\x84\x93\0\0\0\0\0\x64\xe2\x84\x93\0\0\0\0\0\x6b\xe2\x84\x93\0\0\0\0\0\x66\x6d\0"
      u8"\0\0\0\0\0\0\x6e\x6d\0\0\0\0\0\0\0\xce\xbc\x6d\0\0\0\0\0\0\x6d\x6d\0\0\0\0\0\0\0\x63\x6d\0\0\0\0\0\0"
      u8"\0\x6b\x6d\0\0\0\0\0\0\0\x6d\x6d\xc2\xb2\0\0\0\0\0\x63\x6d\xc2\xb2\0\0\0\0\0\x6d\xc2\xb2\0\0\0\0\0\0"
      u8"\x6b\x6d\xc2\xb2\0\0\0\0\0\x6d\x6d\xc2\xb3\0\0\0\0\0\x63\x6d\xc2\xb3\0\0\0\0\0\x6d\xc2\xb3\0\0\0\0\0"
      u8"\0\x6b\x6d\xc2\xb3\0\0\0\0\0\x6d\xe2\x88\x95\x73\0\0\0\0\x6d\xe2\x88\x95\x73\xc2\xb2\0\0\x50\x61\0\0"
      u8"\0\0\0\0\0\x6b\x50\x61\0\0\0\0\0\0\x4d\x50\x61\0\0\0\0\0\0\x47\x50\x61\0\0\0\0\0\0\x72\x61\x64\0\0\0"
      u8"\0\0\0\x72\x61\x64\xe2\x88\x95\x73\0\0\x72\x61\x64\xe2\x88\x95\x73\xc2\xb2\x70\x73\0\0\0\0\0\0\0\x6e"
      u8"\x73\0\0\0\0\0\0\0\xce\xbc\x73\0\0\0\0\0\0\x6d\x73\0\0\0\0\0\0\0\x70\x56\0\0\0\0\0\0\0\x6e\x56\0\0\0"
      u8"\0\0\0\0\xce\xbc\x56\0\0\0\0\0\0\x6d\x56\0\0\0\0\0\0\0\x6b\x56\0\0\0\0\0\0\0\x4d\x56\0\0\0\0\0\0\0"
      u8"\x70\x57\0\0\0\0\0\0\0\x6e\x57\0\0\0\0\0\0\0\xce\xbc\x57\0\0\0\0\0\0\x6d\x57\0\0\0\0\0\0\0\x6b\x57\0"
      u8"\0\0\0\0\0\0\x4d\x57\0\0\0\0\0\0\0"

      // Start of 0x33c0:
      u8"\x6b\xce\xa9\0\0\0\x4d\xce\xa9\0\0\0\x61\56\x6d\56\0\0\x42\x71\0\0\0\0\x63\x63\0\0\0\0\x63\x64\0\0\0"
      u8"\0\x43\xe2\x88\x95\x6b\x67\x43\x6f\56\0\0\0\x64\x42\0\0\0\0\x47\x79\0\0\0\0\x68\x61\0\0\0\0\x48\x50"
      u8"\0\0\0\0\x69\x6e\0\0\0\0\x4b\x4b\0\0\0\0\x4b\x4d\0\0\0\0\x6b\x74\0\0\0\0\x6c\x6d\0\0\0\0\x6c\x6e\0\0"
      u8"\0\0\x6c\x6f\x67\0\0\0\x6c\x78\0\0\0\0\x6d\x62\0\0\0\0\x6d\x69\x6c\0\0\0\x6d\x6f\x6c\0\0\0\x50\x48\0"
      u8"\0\0\0\x70\56\x6d\56\0\0\x50\x50\x4d\0\0\0\x50\x52\0\0\0\0\x73\x72\0\0\0\0\x53\x76\0\0\0\0\x57\x62\0"
      u8"\0\0\0\x56\xe2\x88\x95\x6d\0\x41\xe2\x88\x95\x6d\0\61\xe6\x97\xa5\0\0\62\xe6\x97\xa5\0\0\63\xe6\x97"
      u8"\xa5\0\0\64\xe6\x97\xa5\0\0\65\xe6\x97\xa5\0\0\66\xe6\x97\xa5\0\0\67\xe6\x97\xa5\0\0\70\xe6\x97\xa5"
      u8"\0\0\71\xe6\x97\xa5\0\0\61\60\xe6\x97\xa5\0\61\61\xe6\x97\xa5\0\61\62\xe6\x97\xa5\0\61\63\xe6\x97"
      u8"\xa5\0\61\64\xe6\x97\xa5\0\61\65\xe6\x97\xa5\0\61\66\xe6\x97\xa5\0\61\67\xe6\x97\xa5\0\61\70\xe6\x97"
      u8"\xa5\0\61\71\xe6\x97\xa5\0\62\60\xe6\x97\xa5\0\62\61\xe6\x97\xa5\0\62\62\xe6\x97\xa5\0\62\63\xe6\x97"
      u8"\xa5\0\62\64\xe6\x97\xa5\0\62\65\xe6\x97\xa5\0\62\66\xe6\x97\xa5\0\62\67\xe6\x97\xa5\0\62\70\xe6\x97"
      u8"\xa5\0\62\71\xe6\x97\xa5\0\63\60\xe6\x97\xa5\0\63\61\xe6\x97\xa5\0\x67\x61\x6c"

      // Start of 0xa680:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xd1\x8a\xd1\x8c"

      // Start of 0xa740:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xea\x9d\xaf"

      // Start of 0xa7c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\x43\0\x46\0\x51\0\0\0\0\0\0\0\xc4\xa6\xc5\x93"

      // Start of 0xab40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xea\x9c\xa7\xea\xac\xb7\xc9\xab"
      u8"\0\xea\xad\x92\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\x8d\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0"

      // Start of 0xf900:
      u8"\xe8\xb1\x88\xe6\x9b\xb4\xe8\xbb\x8a\xe8\xb3\x88\xe6\xbb\x91\xe4\xb8\xb2\xe5\x8f\xa5\xe9\xbe\x9c\xe9"
      u8"\xbe\x9c\xe5\xa5\x91\xe9\x87\x91\xe5\x96\x87\xe5\xa5\x88\xe6\x87\xb6\xe7\x99\xa9\xe7\xbe\x85\xe8\x98"
      u8"\xbf\xe8\x9e\xba\xe8\xa3\xb8\xe9\x82\x8f\xe6\xa8\x82\xe6\xb4\x9b\xe7\x83\x99\xe7\x8f\x9e\xe8\x90\xbd"
      u8"\xe9\x85\xaa\xe9\xa7\xb1\xe4\xba\x82\xe5\x8d\xb5\xe6\xac\x84\xe7\x88\x9b\xe8\x98\xad\xe9\xb8\x9e\xe5"
      u8"\xb5\x90\xe6\xbf\xab\xe8\x97\x8d\xe8\xa5\xa4\xe6\x8b\x89\xe8\x87\x98\xe8\xa0\x9f\xe5\xbb\x8a\xe6\x9c"
      u8"\x97\xe6\xb5\xaa\xe7\x8b\xbc\xe9\x83\x8e\xe4\xbe\x86\xe5\x86\xb7\xe5\x8b\x9e\xe6\x93\x84\xe6\xab\x93"
      u8"\xe7\x88\x90\xe7\x9b\xa7\xe8\x80\x81\xe8\x98\x86\xe8\x99\x9c\xe8\xb7\xaf\xe9\x9c\xb2\xe9\xad\xaf\xe9"
      u8"\xb7\xba\xe7\xa2\x8c\xe7\xa5\xbf\xe7\xb6\xa0\xe8\x8f\x89\xe9\x8c\x84"

      // Start of 0xf940:
      u8"\xe9\xb9\xbf\xe8\xab\x96\xe5\xa3\x9f\xe5\xbc\x84\xe7\xb1\xa0\xe8\x81\xbe\xe7\x89\xa2\xe7\xa3\x8a\xe8"
      u8"\xb3\x82\xe9\x9b\xb7\xe5\xa3\x98\xe5\xb1\xa2\xe6\xa8\x93\xe6\xb7\x9a\xe6\xbc\x8f\xe7\xb4\xaf\xe7\xb8"
      u8"\xb7\xe9\x99\x8b\xe5\x8b\x92\xe8\x82\x8b\xe5\x87\x9c\xe5\x87\x8c\xe7\xa8\x9c\xe7\xb6\xbe\xe8\x8f\xb1"
      u8"\xe9\x99\xb5\xe8\xae\x80\xe6\x8b\x8f\xe6\xa8\x82\xe8\xab\xbe\xe4\xb8\xb9\xe5\xaf\xa7\xe6\x80\x92\xe7"
      u8"\x8e\x87\xe7\x95\xb0\xe5\x8c\x97\xe7\xa3\xbb\xe4\xbe\xbf\xe5\xbe\xa9\xe4\xb8\x8d\xe6\xb3\x8c\xe6\x95"
      u8"\xb8\xe7\xb4\xa2\xe5\x8f\x83\xe5\xa1\x9e\xe7\x9c\x81\xe8\x91\x89\xe8\xaa\xaa\xe6\xae\xba\xe8\xbe\xb0"
      u8"\xe6\xb2\x88\xe6\x8b\xbe\xe8\x8b\xa5\xe6\x8e\xa0\xe7\x95\xa5\xe4\xba\xae\xe5\x85\xa9\xe5\x87\x89\xe6"
      u8"\xa2\x81\xe7\xb3\xa7\xe8\x89\xaf\xe8\xab\x92\xe9\x87\x8f\xe5\x8b\xb5"

      // Start of 0xf980:
      u8"\xe5\x91\x82\xe5\xa5\xb3\xe5\xbb\xac\xe6\x97\x85\xe6\xbf\xbe\xe7\xa4\xaa\xe9\x96\xad\xe9\xa9\xaa\xe9"
      u8"\xba\x97\xe9\xbb\x8e\xe5\x8a\x9b\xe6\x9b\x86\xe6\xad\xb7\xe8\xbd\xa2\xe5\xb9\xb4\xe6\x86\x90\xe6\x88"
      u8"\x80\xe6\x92\x9a\xe6\xbc\xa3\xe7\x85\x89\xe7\x92\x89\xe7\xa7\x8a\xe7\xb7\xb4\xe8\x81\xaf\xe8\xbc\xa6"
      u8"\xe8\x93\xae\xe9\x80\xa3\xe9\x8d\x8a\xe5\x88\x97\xe5\x8a\xa3\xe5\x92\xbd\xe7\x83\x88\xe8\xa3\x82\xe8"
      u8"\xaa\xaa\xe5\xbb\x89\xe5\xbf\xb5\xe6\x8d\xbb\xe6\xae\xae\xe7\xb0\xbe\xe7\x8d\xb5\xe4\xbb\xa4\xe5\x9b"
      u8"\xb9\xe5\xaf\xa7\xe5\xb6\xba\xe6\x80\x9c\xe7\x8e\xb2\xe7\x91\xa9\xe7\xbe\x9a\xe8\x81\x86\xe9\x88\xb4"
      u8"\xe9\x9b\xb6\xe9\x9d\x88\xe9\xa0\x98\xe4\xbe\x8b\xe7\xa6\xae\xe9\x86\xb4\xe9\x9a\xb8\xe6\x83\xa1\xe4"
      u8"\xba\x86\xe5\x83\x9a\xe5\xaf\xae\xe5\xb0\xbf\xe6\x96\x99\xe6\xa8\x82"

      // Start of 0xf9c0:
      u8"\xe7\x87\x8e\xe7\x99\x82\xe8\x93\xbc\xe9\x81\xbc\xe9\xbe\x8d\xe6\x9a\x88\xe9\x98\xae\xe5\x8a\x89\xe6"
      u8"\x9d\xbb\xe6\x9f\xb3\xe6\xb5\x81\xe6\xba\x9c\xe7\x90\x89\xe7\x95\x99\xe7\xa1\xab\xe7\xb4\x90\xe9\xa1"
      u8"\x9e\xe5\x85\xad\xe6\x88\xae\xe9\x99\xb8\xe5\x80\xab\xe5\xb4\x99\xe6\xb7\xaa\xe8\xbc\xaa\xe5\xbe\x8b"
      u8"\xe6\x85\x84\xe6\xa0\x97\xe7\x8e\x87\xe9\x9a\x86\xe5\x88\xa9\xe5\x90\x8f\xe5\xb1\xa5\xe6\x98\x93\xe6"
      u8"\x9d\x8e\xe6\xa2\xa8\xe6\xb3\xa5\xe7\x90\x86\xe7\x97\xa2\xe7\xbd\xb9\xe8\xa3\x8f\xe8\xa3\xa1\xe9\x87"
      u8"\x8c\xe9\x9b\xa2\xe5\x8c\xbf\xe6\xba\xba\xe5\x90\x9d\xe7\x87\x90\xe7\x92\x98\xe8\x97\xba\xe9\x9a\xa3"
      u8"\xe9\xb1\x97\xe9\xba\x9f\xe6\x9e\x97\xe6\xb7\x8b\xe8\x87\xa8\xe7\xab\x8b\xe7\xac\xa0\xe7\xb2\x92\xe7"
      u8"\x8b\x80\xe7\x82\x99\xe8\xad\x98\xe4\xbb\x80\xe8\x8c\xb6\xe5\x88\xba"

      // Start of 0xfa00:
      u8"\xe5\x88\x87\xe5\xba\xa6\xe6\x8b\x93\xe7\xb3\x96\xe5\xae\x85\xe6\xb4\x9e\xe6\x9a\xb4\xe8\xbc\xbb\xe8"
      u8"\xa1\x8c\xe9\x99\x8d\xe8\xa6\x8b\xe5\xbb\x93\xe5\x85\x80\xe5\x97\x80\0\0\0\0\0\0\xe5\xa1\x9a\0\0\0"
      u8"\xe6\x99\xb4\0\0\0\0\0\0\xe5\x87\x9e\xe7\x8c\xaa\xe7\x9b\x8a\xe7\xa4\xbc\xe7\xa5\x9e\xe7\xa5\xa5\xe7"
      u8"\xa6\x8f\xe9\x9d\x96\xe7\xb2\xbe\xe7\xbe\xbd\0\0\0\xe8\x98\x92\0\0\0\xe8\xab\xb8\0\0\0\0\0\0\xe9\x80"
      u8"\xb8\xe9\x83\xbd\0\0\0\0\0\0\0\0\0\xe9\xa3\xaf\xe9\xa3\xbc\xe9\xa4\xa8\xe9\xb6\xb4\xe9\x83\x9e\xe9"
      u8"\x9a\xb7\xe4\xbe\xae\xe5\x83\xa7\xe5\x85\x8d\xe5\x8b\x89\xe5\x8b\xa4\xe5\x8d\x91\xe5\x96\x9d\xe5\x98"
      u8"\x86\xe5\x99\xa8\xe5\xa1\x80\xe5\xa2\xa8\xe5\xb1\xa4\xe5\xb1\xae\xe6\x82\x94\xe6\x85\xa8\xe6\x86\x8e"

      // Start of 0xfa40:
      u8"\xe6\x87\xb2\0\xe6\x95\x8f\0\xe6\x97\xa2\0\xe6\x9a\x91\0\xe6\xa2\x85\0\xe6\xb5\xb7\0\xe6\xb8\x9a\0"
      u8"\xe6\xbc\xa2\0\xe7\x85\xae\0\xe7\x88\xab\0\xe7\x90\xa2\0\xe7\xa2\x91\0\xe7\xa4\xbe\0\xe7\xa5\x89\0"
      u8"\xe7\xa5\x88\0\xe7\xa5\x90\0\xe7\xa5\x96\0\xe7\xa5\x9d\0\xe7\xa6\x8d\0\xe7\xa6\x8e\0\xe7\xa9\x80\0"
      u8"\xe7\xaa\x81\0\xe7\xaf\x80\0\xe7\xb7\xb4\0\xe7\xb8\x89\0\xe7\xb9\x81\0\xe7\xbd\xb2\0\xe8\x80\x85\0"
      u8"\xe8\x87\xad\0\xe8\x89\xb9\0\xe8\x89\xb9\0\xe8\x91\x97\0\xe8\xa4\x90\0\xe8\xa6\x96\0\xe8\xac\x81\0"
      u8"\xe8\xac\xb9\0\xe8\xb3\x93\0\xe8\xb4\x88\0\xe8\xbe\xb6\0\xe9\x80\xb8\0\xe9\x9b\xa3\0\xe9\x9f\xbf\0"
      u8"\xe9\xa0\xbb\0\xe6\x81\xb5\0\xf0\xa4\x8b\xae\xe8\x88\x98\0\0\0\0\0\0\0\0\0\xe4\xb8\xa6\0\xe5\x86\xb5"
      u8"\0\xe5\x85\xa8\0\xe4\xbe\x80\0\xe5\x85\x85\0\xe5\x86\x80\0\xe5\x8b\x87\0\xe5\x8b\xba\0\xe5\x96\x9d\0"
      u8"\xe5\x95\x95\0\xe5\x96\x99\0\xe5\x97\xa2\0\xe5\xa1\x9a\0\xe5\xa2\xb3\0\xe5\xa5\x84\0\xe5\xa5\x94\0"

      // Start of 0xfa80:
      u8"\xe5\xa9\xa2\xe5\xac\xa8\xe5\xbb\x92\xe5\xbb\x99\xe5\xbd\xa9\xe5\xbe\xad\xe6\x83\x98\xe6\x85\x8e\xe6"
      u8"\x84\x88\xe6\x86\x8e\xe6\x85\xa0\xe6\x87\xb2\xe6\x88\xb4\xe6\x8f\x84\xe6\x90\x9c\xe6\x91\x92\xe6\x95"
      u8"\x96\xe6\x99\xb4\xe6\x9c\x97\xe6\x9c\x9b\xe6\x9d\x96\xe6\xad\xb9\xe6\xae\xba\xe6\xb5\x81\xe6\xbb\x9b"
      u8"\xe6\xbb\x8b\xe6\xbc\xa2\xe7\x80\x9e\xe7\x85\xae\xe7\x9e\xa7\xe7\x88\xb5\xe7\x8a\xaf\xe7\x8c\xaa\xe7"
      u8"\x91\xb1\xe7\x94\x86\xe7\x94\xbb\xe7\x98\x9d\xe7\x98\x9f\xe7\x9b\x8a\xe7\x9b\x9b\xe7\x9b\xb4\xe7\x9d"
      u8"\x8a\xe7\x9d\x80\xe7\xa3\x8c\xe7\xaa\xb1\xe7\xaf\x80\xe7\xb1\xbb\xe7\xb5\x9b\xe7\xb7\xb4\xe7\xbc\xbe"
      u8"\xe8\x80\x85\xe8\x8d\x92\xe8\x8f\xaf\xe8\x9d\xb9\xe8\xa5\x81\xe8\xa6\x86\xe8\xa6\x96\xe8\xaa\xbf\xe8"
      u8"\xab\xb8\xe8\xab\x8b\xe8\xac\x81\xe8\xab\xbe\xe8\xab\xad\xe8\xac\xb9"

      // Start of 0xfac0:
      u8"\xe8\xae\x8a\0\xe8\xb4\x88\0\xe8\xbc\xb8\0\xe9\x81\xb2\0\xe9\x86\x99\0\xe9\x89\xb6\0\xe9\x99\xbc\0"
      u8"\xe9\x9b\xa3\0\xe9\x9d\x96\0\xe9\x9f\x9b\0\xe9\x9f\xbf\0\xe9\xa0\x8b\0\xe9\xa0\xbb\0\xe9\xac\x92\0"
      u8"\xe9\xbe\x9c\0\xf0\xa2\xa1\x8a\xf0\xa2\xa1\x84\xf0\xa3\x8f\x95\xe3\xae\x9d\0\xe4\x80\x98\0\xe4\x80"
      u8"\xb9\0\xf0\xa5\x89\x89\xf0\xa5\xb3\x90\xf0\xa7\xbb\x93\xe9\xbd\x83\0\xe9\xbe\x8e\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xfb00:
      u8"\x66\x66\0\0\0\x66\x69\0\0\0\x66\x6c\0\0\0\x66\x66\x69\0\0\x66\x66\x6c\0\0\xc5\xbf\x74\0\0\x73\x74\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xd5\xb4\xd5\xb6\0\xd5\xb4\xd5\xa5\0\xd5\xb4\xd5\xab\0\xd5\xbe\xd5\xb6\0\xd5"
      u8"\xb4\xd5\xad\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd7\x99\xd6\xb4\0\0\0\0\0\0\xd7\xb2"
      u8"\xd6\xb7\0\xd7\xa2\0\0\0\xd7\x90\0\0\0\xd7\x93\0\0\0\xd7\x94\0\0\0\xd7\x9b\0\0\0\xd7\x9c\0\0\0\xd7"
      u8"\x9d\0\0\0\xd7\xa8\0\0\0\xd7\xaa\0\0\0\53\0\0\0\0\xd7\xa9\xd7\x81\0\xd7\xa9\xd7\x82\0\xef\xad\x89"
      u8"\xd7\x81\xef\xad\x89\xd7\x82\xd7\x90\xd6\xb7\0\xd7\x90\xd6\xb8\0\xd7\x90\xd6\xbc\0\xd7\x91\xd6\xbc\0"
      u8"\xd7\x92\xd6\xbc\0\xd7\x93\xd6\xbc\0\xd7\x94\xd6\xbc\0\xd7\x95\xd6\xbc\0\xd7\x96\xd6\xbc\0\0\0\0\0\0"
      u8"\xd7\x98\xd6\xbc\0\xd7\x99\xd6\xbc\0\xd7\x9a\xd6\xbc\0\xd7\x9b\xd6\xbc\0\xd7\x9c\xd6\xbc\0\0\0\0\0\0"
      u8"\xd7\x9e\xd6\xbc\0\0\0\0\0\0"

      // Start of 0xfb40:
      u8"\xd7\xa0\xd6\xbc\xd7\xa1\xd6\xbc\0\0\0\0\xd7\xa3\xd6\xbc\xd7\xa4\xd6\xbc\0\0\0\0\xd7\xa6\xd6\xbc\xd7"
      u8"\xa7\xd6\xbc\xd7\xa8\xd6\xbc\xd7\xa9\xd6\xbc\xd7\xaa\xd6\xbc\xd7\x95\xd6\xb9\xd7\x91\xd6\xbf\xd7\x9b"
      u8"\xd6\xbf\xd7\xa4\xd6\xbf\xd7\x90\xd7\x9c\xd9\xb1\0\0\xd9\xb1\0\0\xd9\xbb\0\0\xd9\xbb\0\0\xd9\xbb\0\0"
      u8"\xd9\xbb\0\0\xd9\xbe\0\0\xd9\xbe\0\0\xd9\xbe\0\0\xd9\xbe\0\0\xda\x80\0\0\xda\x80\0\0\xda\x80\0\0\xda"
      u8"\x80\0\0\xd9\xba\0\0\xd9\xba\0\0\xd9\xba\0\0\xd9\xba\0\0\xd9\xbf\0\0\xd9\xbf\0\0\xd9\xbf\0\0\xd9\xbf"
      u8"\0\0\xd9\xb9\0\0\xd9\xb9\0\0\xd9\xb9\0\0\xd9\xb9\0\0\xda\xa4\0\0\xda\xa4\0\0\xda\xa4\0\0\xda\xa4\0\0"
      u8"\xda\xa6\0\0\xda\xa6\0\0\xda\xa6\0\0\xda\xa6\0\0\xda\x84\0\0\xda\x84\0\0\xda\x84\0\0\xda\x84\0\0\xda"
      u8"\x83\0\0\xda\x83\0\0\xda\x83\0\0\xda\x83\0\0\xda\x86\0\0\xda\x86\0\0\xda\x86\0\0\xda\x86\0\0\xda\x87"
      u8"\0\0\xda\x87\0\0"

      // Start of 0xfb80:
      u8"\xda\x87\xda\x87\xda\x8d\xda\x8d\xda\x8c\xda\x8c\xda\x8e\xda\x8e\xda\x88\xda\x88\xda\x98\xda\x98\xda"
      u8"\x91\xda\x91\xda\xa9\xda\xa9\xda\xa9\xda\xa9\xda\xaf\xda\xaf\xda\xaf\xda\xaf\xda\xb3\xda\xb3\xda\xb3"
      u8"\xda\xb3\xda\xb1\xda\xb1\xda\xb1\xda\xb1\xda\xba\xda\xba\xda\xbb\xda\xbb\xda\xbb\xda\xbb\xdb\x80\xdb"
      u8"\x80\xdb\x81\xdb\x81\xdb\x81\xdb\x81\xda\xbe\xda\xbe\xda\xbe\xda\xbe\xdb\x92\xdb\x92\xdb\x93\xdb\x93"

      // Start of 0xfbc0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xda\xad\0\0\xda\xad\0\0\xda\xad\0\0\xda\xad\0\0"
      u8"\xdb\x87\0\0\xdb\x87\0\0\xdb\x86\0\0\xdb\x86\0\0\xdb\x88\0\0\xdb\x88\0\0\xd9\xb7\0\0\xdb\x8b\0\0\xdb"
      u8"\x8b\0\0\xdb\x85\0\0\xdb\x85\0\0\xdb\x89\0\0\xdb\x89\0\0\xdb\x90\0\0\xdb\x90\0\0\xdb\x90\0\0\xdb\x90"
      u8"\0\0\xd9\x89\0\0\xd9\x89\0\0\xd8\xa6\xd8\xa7\xd8\xa6\xd8\xa7\xd8\xa6\xdb\x95\xd8\xa6\xdb\x95\xd8\xa6"
      u8"\xd9\x88\xd8\xa6\xd9\x88\xd8\xa6\xdb\x87\xd8\xa6\xdb\x87\xd8\xa6\xdb\x86\xd8\xa6\xdb\x86\xd8\xa6\xdb"
      u8"\x88\xd8\xa6\xdb\x88\xd8\xa6\xdb\x90\xd8\xa6\xdb\x90\xd8\xa6\xdb\x90\xd8\xa6\xd9\x89\xd8\xa6\xd9\x89"
      u8"\xd8\xa6\xd9\x89\xdb\x8c\0\0\xdb\x8c\0\0\xdb\x8c\0\0\xdb\x8c\0\0"

      // Start of 0xfc00:
      u8"\xd8\xa6\xd8\xac\xd8\xa6\xd8\xad\xd8\xa6\xd9\x85\xd8\xa6\xd9\x89\xd8\xa6\xd9\x8a\xd8\xa8\xd8\xac\xd8"
      u8"\xa8\xd8\xad\xd8\xa8\xd8\xae\xd8\xa8\xd9\x85\xd8\xa8\xd9\x89\xd8\xa8\xd9\x8a\xd8\xaa\xd8\xac\xd8\xaa"
      u8"\xd8\xad\xd8\xaa\xd8\xae\xd8\xaa\xd9\x85\xd8\xaa\xd9\x89\xd8\xaa\xd9\x8a\xd8\xab\xd8\xac\xd8\xab\xd9"
      u8"\x85\xd8\xab\xd9\x89\xd8\xab\xd9\x8a\xd8\xac\xd8\xad\xd8\xac\xd9\x85\xd8\xad\xd8\xac\xd8\xad\xd9\x85"
      u8"\xd8\xae\xd8\xac\xd8\xae\xd8\xad\xd8\xae\xd9\x85\xd8\xb3\xd8\xac\xd8\xb3\xd8\xad\xd8\xb3\xd8\xae\xd8"
      u8"\xb3\xd9\x85\xd8\xb5\xd8\xad\xd8\xb5\xd9\x85\xd8\xb6\xd8\xac\xd8\xb6\xd8\xad\xd8\xb6\xd8\xae\xd8\xb6"
      u8"\xd9\x85\xd8\xb7\xd8\xad\xd8\xb7\xd9\x85\xd8\xb8\xd9\x85\xd8\xb9\xd8\xac\xd8\xb9\xd9\x85\xd8\xba\xd8"
      u8"\xac\xd8\xba\xd9\x85\xd9\x81\xd8\xac\xd9\x81\xd8\xad\xd9\x81\xd8\xae\xd9\x81\xd9\x85\xd9\x81\xd9\x89"
      u8"\xd9\x81\xd9\x8a\xd9\x82\xd8\xad\xd9\x82\xd9\x85\xd9\x82\xd9\x89\xd9\x82\xd9\x8a\xd9\x83\xd8\xa7\xd9"
      u8"\x83\xd8\xac\xd9\x83\xd8\xad\xd9\x83\xd8\xae\xd9\x83\xd9\x84\xd9\x83\xd9\x85\xd9\x83\xd9\x89\xd9\x83"
      u8"\xd9\x8a\xd9\x84\xd8\xac"

      // Start of 0xfc40:
      u8"\xd9\x84\xd8\xad\0\xd9\x84\xd8\xae\0\xd9\x84\xd9\x85\0\xd9\x84\xd9\x89\0\xd9\x84\xd9\x8a\0\xd9\x85"
      u8"\xd8\xac\0\xd9\x85\xd8\xad\0\xd9\x85\xd8\xae\0\xd9\x85\xd9\x85\0\xd9\x85\xd9\x89\0\xd9\x85\xd9\x8a\0"
      u8"\xd9\x86\xd8\xac\0\xd9\x86\xd8\xad\0\xd9\x86\xd8\xae\0\xd9\x86\xd9\x85\0\xd9\x86\xd9\x89\0\xd9\x86"
      u8"\xd9\x8a\0\xd9\x87\xd8\xac\0\xd9\x87\xd9\x85\0\xd9\x87\xd9\x89\0\xd9\x87\xd9\x8a\0\xd9\x8a\xd8\xac\0"
      u8"\xd9\x8a\xd8\xad\0\xd9\x8a\xd8\xae\0\xd9\x8a\xd9\x85\0\xd9\x8a\xd9\x89\0\xd9\x8a\xd9\x8a\0\xd8\xb0"
      u8"\xd9\xb0\0\xd8\xb1\xd9\xb0\0\xd9\x89\xd9\xb0\0\40\xd9\x8c\xd9\x91\40\xd9\x8d\xd9\x91\40\xd9\x8e\xd9"
      u8"\x91\40\xd9\x8f\xd9\x91\40\xd9\x90\xd9\x91\40\xd9\x91\xd9\xb0\xd8\xa6\xd8\xb1\0\xd8\xa6\xd8\xb2\0"
      u8"\xd8\xa6\xd9\x85\0\xd8\xa6\xd9\x86\0\xd8\xa6\xd9\x89\0\xd8\xa6\xd9\x8a\0\xd8\xa8\xd8\xb1\0\xd8\xa8"
      u8"\xd8\xb2\0\xd8\xa8\xd9\x85\0\xd8\xa8\xd9\x86\0\xd8\xa8\xd9\x89\0\xd8\xa8\xd9\x8a\0\xd8\xaa\xd8\xb1\0"
      u8"\xd8\xaa\xd8\xb2\0\xd8\xaa\xd9\x85\0\xd8\xaa\xd9\x86\0\xd8\xaa\xd9\x89\0\xd8\xaa\xd9\x8a\0\xd8\xab"
      u8"\xd8\xb1\0\xd8\xab\xd8\xb2\0\xd8\xab\xd9\x85\0\xd8\xab\xd9\x86\0\xd8\xab\xd9\x89\0\xd8\xab\xd9\x8a\0"
      u8"\xd9\x81\xd9\x89\0\xd9\x81\xd9\x8a\0\xd9\x82\xd9\x89\0\xd9\x82\xd9\x8a\0"

      // Start of 0xfc80:
      u8"\xd9\x83\xd8\xa7\xd9\x83\xd9\x84\xd9\x83\xd9\x85\xd9\x83\xd9\x89\xd9\x83\xd9\x8a\xd9\x84\xd9\x85\xd9"
      u8"\x84\xd9\x89\xd9\x84\xd9\x8a\xd9\x85\xd8\xa7\xd9\x85\xd9\x85\xd9\x86\xd8\xb1\xd9\x86\xd8\xb2\xd9\x86"
      u8"\xd9\x85\xd9\x86\xd9\x86\xd9\x86\xd9\x89\xd9\x86\xd9\x8a\xd9\x89\xd9\xb0\xd9\x8a\xd8\xb1\xd9\x8a\xd8"
      u8"\xb2\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86\xd9\x8a\xd9\x89\xd9\x8a\xd9\x8a\xd8\xa6\xd8\xac\xd8\xa6\xd8\xad"
      u8"\xd8\xa6\xd8\xae\xd8\xa6\xd9\x85\xd8\xa6\xd9\x87\xd8\xa8\xd8\xac\xd8\xa8\xd8\xad\xd8\xa8\xd8\xae\xd8"
      u8"\xa8\xd9\x85\xd8\xa8\xd9\x87\xd8\xaa\xd8\xac\xd8\xaa\xd8\xad\xd8\xaa\xd8\xae\xd8\xaa\xd9\x85\xd8\xaa"
      u8"\xd9\x87\xd8\xab\xd9\x85\xd8\xac\xd8\xad\xd8\xac\xd9\x85\xd8\xad\xd8\xac\xd8\xad\xd9\x85\xd8\xae\xd8"
      u8"\xac\xd8\xae\xd9\x85\xd8\xb3\xd8\xac\xd8\xb3\xd8\xad\xd8\xb3\xd8\xae\xd8\xb3\xd9\x85\xd8\xb5\xd8\xad"
      u8"\xd8\xb5\xd8\xae\xd8\xb5\xd9\x85\xd8\xb6\xd8\xac\xd8\xb6\xd8\xad\xd8\xb6\xd8\xae\xd8\xb6\xd9\x85\xd8"
      u8"\xb7\xd8\xad\xd8\xb8\xd9\x85\xd8\xb9\xd8\xac\xd8\xb9\xd9\x85\xd8\xba\xd8\xac\xd8\xba\xd9\x85\xd9\x81"
      u8"\xd8\xac\xd9\x81\xd8\xad"

      // Start of 0xfcc0:
      u8"\xd9\x81\xd8\xae\0\0\xd9\x81\xd9\x85\0\0\xd9\x82\xd8\xad\0\0\xd9\x82\xd9\x85\0\0\xd9\x83\xd8\xac\0\0"
      u8"\xd9\x83\xd8\xad\0\0\xd9\x83\xd8\xae\0\0\xd9\x83\xd9\x84\0\0\xd9\x83\xd9\x85\0\0\xd9\x84\xd8\xac\0\0"
      u8"\xd9\x84\xd8\xad\0\0\xd9\x84\xd8\xae\0\0\xd9\x84\xd9\x85\0\0\xd9\x84\xd9\x87\0\0\xd9\x85\xd8\xac\0\0"
      u8"\xd9\x85\xd8\xad\0\0\xd9\x85\xd8\xae\0\0\xd9\x85\xd9\x85\0\0\xd9\x86\xd8\xac\0\0\xd9\x86\xd8\xad\0\0"
      u8"\xd9\x86\xd8\xae\0\0\xd9\x86\xd9\x85\0\0\xd9\x86\xd9\x87\0\0\xd9\x87\xd8\xac\0\0\xd9\x87\xd9\x85\0\0"
      u8"\xd9\x87\xd9\xb0\0\0\xd9\x8a\xd8\xac\0\0\xd9\x8a\xd8\xad\0\0\xd9\x8a\xd8\xae\0\0\xd9\x8a\xd9\x85\0\0"
      u8"\xd9\x8a\xd9\x87\0\0\xd8\xa6\xd9\x85\0\0\xd8\xa6\xd9\x87\0\0\xd8\xa8\xd9\x85\0\0\xd8\xa8\xd9\x87\0\0"
      u8"\xd8\xaa\xd9\x85\0\0\xd8\xaa\xd9\x87\0\0\xd8\xab\xd9\x85\0\0\xd8\xab\xd9\x87\0\0\xd8\xb3\xd9\x85\0\0"
      u8"\xd8\xb3\xd9\x87\0\0\xd8\xb4\xd9\x85\0\0\xd8\xb4\xd9\x87\0\0\xd9\x83\xd9\x84\0\0\xd9\x83\xd9\x85\0\0"
      u8"\xd9\x84\xd9\x85\0\0\xd9\x86\xd9\x85\0\0\xd9\x86\xd9\x87\0\0\xd9\x8a\xd9\x85\0\0\xd9\x8a\xd9\x87\0\0"
      u8"\xd9\x80\xd9\x8e\xd9\x91\xd9\x80\xd9\x8f\xd9\x91\xd9\x80\xd9\x90\xd9\x91\xd8\xb7\xd9\x89\0\0\xd8\xb7"
      u8"\xd9\x8a\0\0\xd8\xb9\xd9\x89\0\0\xd8\xb9\xd9\x8a\0\0\xd8\xba\xd9\x89\0\0\xd8\xba\xd9\x8a\0\0\xd8\xb3"
      u8"\xd9\x89\0\0\xd8\xb3\xd9\x8a\0\0\xd8\xb4\xd9\x89\0\0\xd8\xb4\xd9\x8a\0\0\xd8\xad\xd9\x89\0\0"

      // Start of 0xfd00:
      u8"\xd8\xad\xd9\x8a\xd8\xac\xd9\x89\xd8\xac\xd9\x8a\xd8\xae\xd9\x89\xd8\xae\xd9\x8a\xd8\xb5\xd9\x89\xd8"
      u8"\xb5\xd9\x8a\xd8\xb6\xd9\x89\xd8\xb6\xd9\x8a\xd8\xb4\xd8\xac\xd8\xb4\xd8\xad\xd8\xb4\xd8\xae\xd8\xb4"
      u8"\xd9\x85\xd8\xb4\xd8\xb1\xd8\xb3\xd8\xb1\xd8\xb5\xd8\xb1\xd8\xb6\xd8\xb1\xd8\xb7\xd9\x89\xd8\xb7\xd9"
      u8"\x8a\xd8\xb9\xd9\x89\xd8\xb9\xd9\x8a\xd8\xba\xd9\x89\xd8\xba\xd9\x8a\xd8\xb3\xd9\x89\xd8\xb3\xd9\x8a"
      u8"\xd8\xb4\xd9\x89\xd8\xb4\xd9\x8a\xd8\xad\xd9\x89\xd8\xad\xd9\x8a\xd8\xac\xd9\x89\xd8\xac\xd9\x8a\xd8"
      u8"\xae\xd9\x89\xd8\xae\xd9\x8a\xd8\xb5\xd9\x89\xd8\xb5\xd9\x8a\xd8\xb6\xd9\x89\xd8\xb6\xd9\x8a\xd8\xb4"
      u8"\xd8\xac\xd8\xb4\xd8\xad\xd8\xb4\xd8\xae\xd8\xb4\xd9\x85\xd8\xb4\xd8\xb1\xd8\xb3\xd8\xb1\xd8\xb5\xd8"
      u8"\xb1\xd8\xb6\xd8\xb1\xd8\xb4\xd8\xac\xd8\xb4\xd8\xad\xd8\xb4\xd8\xae\xd8\xb4\xd9\x85\xd8\xb3\xd9\x87"
      u8"\xd8\xb4\xd9\x87\xd8\xb7\xd9\x85\xd8\xb3\xd8\xac\xd8\xb3\xd8\xad\xd8\xb3\xd8\xae\xd8\xb4\xd8\xac\xd8"
      u8"\xb4\xd8\xad\xd8\xb4\xd8\xae\xd8\xb7\xd9\x85\xd8\xb8\xd9\x85\xd8\xa7\xd9\x8b\xd8\xa7\xd9\x8b"

      // Start of 0xfd40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xaa"
      u8"\xd8\xac\xd9\x85\xd8\xaa\xd8\xad\xd8\xac\xd8\xaa\xd8\xad\xd8\xac\xd8\xaa\xd8\xad\xd9\x85\xd8\xaa\xd8"
      u8"\xae\xd9\x85\xd8\xaa\xd9\x85\xd8\xac\xd8\xaa\xd9\x85\xd8\xad\xd8\xaa\xd9\x85\xd8\xae\xd8\xac\xd9\x85"
      u8"\xd8\xad\xd8\xac\xd9\x85\xd8\xad\xd8\xad\xd9\x85\xd9\x8a\xd8\xad\xd9\x85\xd9\x89\xd8\xb3\xd8\xad\xd8"
      u8"\xac\xd8\xb3\xd8\xac\xd8\xad\xd8\xb3\xd8\xac\xd9\x89\xd8\xb3\xd9\x85\xd8\xad\xd8\xb3\xd9\x85\xd8\xad"
      u8"\xd8\xb3\xd9\x85\xd8\xac\xd8\xb3\xd9\x85\xd9\x85\xd8\xb3\xd9\x85\xd9\x85\xd8\xb5\xd8\xad\xd8\xad\xd8"
      u8"\xb5\xd8\xad\xd8\xad\xd8\xb5\xd9\x85\xd9\x85\xd8\xb4\xd8\xad\xd9\x85\xd8\xb4\xd8\xad\xd9\x85\xd8\xb4"
      u8"\xd8\xac\xd9\x8a\xd8\xb4\xd9\x85\xd8\xae\xd8\xb4\xd9\x85\xd8\xae\xd8\xb4\xd9\x85\xd9\x85\xd8\xb4\xd9"
      u8"\x85\xd9\x85\xd8\xb6\xd8\xad\xd9\x89\xd8\xb6\xd8\xae\xd9\x85\xd8\xb6\xd8\xae\xd9\x85\xd8\xb7\xd9\x85"
      u8"\xd8\xad\xd8\xb7\xd9\x85\xd8\xad\xd8\xb7\xd9\x85\xd9\x85\xd8\xb7\xd9\x85\xd9\x8a\xd8\xb9\xd8\xac\xd9"
      u8"\x85\xd8\xb9\xd9\x85\xd9\x85\xd8\xb9\xd9\x85\xd9\x85\xd8\xb9\xd9\x85\xd9\x89\xd8\xba\xd9\x85\xd9\x85"
      u8"\xd8\xba\xd9\x85\xd9\x8a\xd8\xba\xd9\x85\xd9\x89\xd9\x81\xd8\xae\xd9\x85\xd9\x81\xd8\xae\xd9\x85\xd9"
      u8"\x82\xd9\x85\xd8\xad\xd9\x82\xd9\x85\xd9\x85"

      // Start of 0xfd80:
      u8"\xd9\x84\xd8\xad\xd9\x85\xd9\x84\xd8\xad\xd9\x8a\xd9\x84\xd8\xad\xd9\x89\xd9\x84\xd8\xac\xd8\xac\xd9"
      u8"\x84\xd8\xac\xd8\xac\xd9\x84\xd8\xae\xd9\x85\xd9\x84\xd8\xae\xd9\x85\xd9\x84\xd9\x85\xd8\xad\xd9\x84"
      u8"\xd9\x85\xd8\xad\xd9\x85\xd8\xad\xd8\xac\xd9\x85\xd8\xad\xd9\x85\xd9\x85\xd8\xad\xd9\x8a\xd9\x85\xd8"
      u8"\xac\xd8\xad\xd9\x85\xd8\xac\xd9\x85\xd9\x85\xd8\xae\xd8\xac\xd9\x85\xd8\xae\xd9\x85\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xd9\x85\xd8\xac\xd8\xae\xd9\x87\xd9\x85\xd8\xac\xd9\x87\xd9\x85\xd9\x85\xd9\x86\xd8\xad\xd9"
      u8"\x85\xd9\x86\xd8\xad\xd9\x89\xd9\x86\xd8\xac\xd9\x85\xd9\x86\xd8\xac\xd9\x85\xd9\x86\xd8\xac\xd9\x89"
      u8"\xd9\x86\xd9\x85\xd9\x8a\xd9\x86\xd9\x85\xd9\x89\xd9\x8a\xd9\x85\xd9\x85\xd9\x8a\xd9\x85\xd9\x85\xd8"
      u8"\xa8\xd8\xae\xd9\x8a\xd8\xaa\xd8\xac\xd9\x8a\xd8\xaa\xd8\xac\xd9\x89\xd8\xaa\xd8\xae\xd9\x8a\xd8\xaa"
      u8"\xd8\xae\xd9\x89\xd8\xaa\xd9\x85\xd9\x8a\xd8\xaa\xd9\x85\xd9\x89\xd8\xac\xd9\x85\xd9\x8a\xd8\xac\xd8"
      u8"\xad\xd9\x89\xd8\xac\xd9\x85\xd9\x89\xd8\xb3\xd8\xae\xd9\x89\xd8\xb5\xd8\xad\xd9\x8a\xd8\xb4\xd8\xad"
      u8"\xd9\x8a\xd8\xb6\xd8\xad\xd9\x8a\xd9\x84\xd8\xac\xd9\x8a\xd9\x84\xd9\x85\xd9\x8a\xd9\x8a\xd8\xad\xd9"
      u8"\x8a\xd9\x8a\xd8\xac\xd9\x8a\xd9\x8a\xd9\x85\xd9\x8a\xd9\x85\xd9\x85\xd9\x8a\xd9\x82\xd9\x85\xd9\x8a"
      u8"\xd9\x86\xd8\xad\xd9\x8a\xd9\x82\xd9\x85\xd8\xad\xd9\x84\xd8\xad\xd9\x85\xd8\xb9\xd9\x85\xd9\x8a\xd9"
      u8"\x83\xd9\x85\xd9\x8a\xd9\x86\xd8\xac\xd8\xad\xd9\x85\xd8\xae\xd9\x8a\xd9\x84\xd8\xac\xd9\x85\xd9\x83"
      u8"\xd9\x85\xd9\x85\xd9\x84\xd8\xac\xd9\x85\xd9\x86\xd8\xac\xd8\xad\xd8\xac\xd8\xad\xd9\x8a\xd8\xad\xd8"
      u8"\xac\xd9\x8a"

      // Start of 0xfdc0:
      u8"\xd9\x85\xd8\xac\xd9\x8a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd9\x81\xd9\x85\xd9"
      u8"\x8a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xa8\xd8\xad\xd9\x8a\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd9\x83\xd9\x85\xd9\x85\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\xd8\xb9\xd8\xac\xd9\x85\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xb5"
      u8"\xd9\x85\xd9\x85\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xb3\xd8\xae\xd9\x8a\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd9\x86\xd8\xac\xd9\x8a\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xb5\xd9\x84\xdb\x92\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd9\x82\xd9\x84\xdb\x92\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\xd8\xa7\xd9\x84\xd9\x84\xd9\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\xd8\xa7\xd9\x83\xd8\xa8\xd8\xb1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd9\x85\xd8"
      u8"\xad\xd9\x85\xd8\xaf\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xb5\xd9\x84\xd8\xb9\xd9"
      u8"\x85\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xb1\xd8\xb3\xd9\x88\xd9\x84\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\xd9\x88\xd8\xb3\xd9\x84\xd9\x85\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xd8\xb5\xd9\x84\xd9\x89\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xb5\xd9\x84\xd9"
      u8"\x89\40\xd8\xa7\xd9\x84\xd9\x84\xd9\x87\40\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\40\xd9\x88\xd8\xb3\xd9"
      u8"\x84\xd9\x85\xd8\xac\xd9\x84\40\xd8\xac\xd9\x84\xd8\xa7\xd9\x84\xd9\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xd8\xb1\xdb\x8c\xd8\xa7\xd9\x84\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xfe00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\54"
      u8"\0\0\xe3\x80\x81\xe3\x80\x82\72\0\0\73\0\0\41\0\0\77\0\0\xe3\x80\x96\xe3\x80\x97\xe2\x80\xa6\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x80\xa5\xe2\x80\x94\xe2\x80\x93\x5f\0\0\x5f\0\0\50\0\0\51\0\0\x7b\0\0"
      u8"\x7d\0\0\xe3\x80\x94\xe3\x80\x95\xe3\x80\x90\xe3\x80\x91\xe3\x80\x8a\xe3\x80\x8b\xe3\x80\x88"

      // Start of 0xfe40:
      u8"\xe3\x80\x89\0\xe3\x80\x8c\0\xe3\x80\x8d\0\xe3\x80\x8e\0\xe3\x80\x8f\0\0\0\0\0\0\0\0\0\x5b\0\0\0\x5d"
      u8"\0\0\0\xe2\x80\xbe\0\xe2\x80\xbe\0\xe2\x80\xbe\0\xe2\x80\xbe\0\x5f\0\0\0\x5f\0\0\0\x5f\0\0\0\54\0\0"
      u8"\0\xe3\x80\x81\0\56\0\0\0\0\0\0\0\73\0\0\0\72\0\0\0\77\0\0\0\41\0\0\0\xe2\x80\x94\0\50\0\0\0\51\0\0"
      u8"\0\x7b\0\0\0\x7d\0\0\0\xe3\x80\x94\0\xe3\x80\x95\0\43\0\0\0\46\0\0\0\52\0\0\0\53\0\0\0\55\0\0\0\74\0"
      u8"\0\0\76\0\0\0\75\0\0\0\0\0\0\0\x5c\0\0\0\44\0\0\0\45\0\0\0\x40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\40\xd9\x8b\0\xd9\x80\xd9\x8b\40\xd9\x8c\0\0\0\0\0\40\xd9\x8d\0\0\0\0\0\40\xd9\x8e\0\xd9\x80\xd9\x8e"
      u8"\40\xd9\x8f\0\xd9\x80\xd9\x8f\40\xd9\x90\0\xd9\x80\xd9\x90\40\xd9\x91\0\xd9\x80\xd9\x91\40\xd9\x92\0"
      u8"\xd9\x80\xd9\x92"

      // Start of 0xfe80:
      u8"\xd8\xa1\xd8\xa2\xd8\xa2\xd8\xa3\xd8\xa3\xd8\xa4\xd8\xa4\xd8\xa5\xd8\xa5\xd8\xa6\xd8\xa6\xd8\xa6\xd8"
      u8"\xa6\xd8\xa7\xd8\xa7\xd8\xa8\xd8\xa8\xd8\xa8\xd8\xa8\xd8\xa9\xd8\xa9\xd8\xaa\xd8\xaa\xd8\xaa\xd8\xaa"
      u8"\xd8\xab\xd8\xab\xd8\xab\xd8\xab\xd8\xac\xd8\xac\xd8\xac\xd8\xac\xd8\xad\xd8\xad\xd8\xad\xd8\xad\xd8"
      u8"\xae\xd8\xae\xd8\xae\xd8\xae\xd8\xaf\xd8\xaf\xd8\xb0\xd8\xb0\xd8\xb1\xd8\xb1\xd8\xb2\xd8\xb2\xd8\xb3"
      u8"\xd8\xb3\xd8\xb3\xd8\xb3\xd8\xb4\xd8\xb4\xd8\xb4\xd8\xb4\xd8\xb5\xd8\xb5\xd8\xb5\xd8\xb5\xd8\xb6\xd8"
      u8"\xb6"

      // Start of 0xfec0:
      u8"\xd8\xb6\0\0\xd8\xb7\0\0\xd8\xb7\0\0\xd8\xb7\0\0\xd8\xb7\0\0\xd8\xb8\0\0\xd8\xb8\0\0\xd8\xb8\0\0\xd8"
      u8"\xb8\0\0\xd8\xb9\0\0\xd8\xb9\0\0\xd8\xb9\0\0\xd8\xb9\0\0\xd8\xba\0\0\xd8\xba\0\0\xd8\xba\0\0\xd8\xba"
      u8"\0\0\xd9\x81\0\0\xd9\x81\0\0\xd9\x81\0\0\xd9\x81\0\0\xd9\x82\0\0\xd9\x82\0\0\xd9\x82\0\0\xd9\x82\0\0"
      u8"\xd9\x83\0\0\xd9\x83\0\0\xd9\x83\0\0\xd9\x83\0\0\xd9\x84\0\0\xd9\x84\0\0\xd9\x84\0\0\xd9\x84\0\0\xd9"
      u8"\x85\0\0\xd9\x85\0\0\xd9\x85\0\0\xd9\x85\0\0\xd9\x86\0\0\xd9\x86\0\0\xd9\x86\0\0\xd9\x86\0\0\xd9\x87"
      u8"\0\0\xd9\x87\0\0\xd9\x87\0\0\xd9\x87\0\0\xd9\x88\0\0\xd9\x88\0\0\xd9\x89\0\0\xd9\x89\0\0\xd9\x8a\0\0"
      u8"\xd9\x8a\0\0\xd9\x8a\0\0\xd9\x8a\0\0\xd9\x84\xd8\xa2\xd9\x84\xd8\xa2\xd9\x84\xd8\xa3\xd9\x84\xd8\xa3"
      u8"\xd9\x84\xd8\xa5\xd9\x84\xd8\xa5\xd9\x84\xd8\xa7\xd9\x84\xd8\xa7\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xff00:
      u8"\0\41\42\43\44\45\46\47\50\51\52\53\54\55\56\57\60\61\62\63\64\65\66\67\70\71\72\73\74\75\76\77\x40"
      u8"\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59"
      u8"\x5a\x5b\x5c\x5d\x5e\x5f"

      // Start of 0xff40:
      u8"\x60\0\0\x61\0\0\x62\0\0\x63\0\0\x64\0\0\x65\0\0\x66\0\0\x67\0\0\x68\0\0\x69\0\0\x6a\0\0\x6b\0\0\x6c"
      u8"\0\0\x6d\0\0\x6e\0\0\x6f\0\0\x70\0\0\x71\0\0\x72\0\0\x73\0\0\x74\0\0\x75\0\0\x76\0\0\x77\0\0\x78\0\0"
      u8"\x79\0\0\x7a\0\0\x7b\0\0\x7c\0\0\x7d\0\0\x7e\0\0\xe2\xa6\x85\xe2\xa6\x86\xe3\x80\x82\xe3\x80\x8c\xe3"
      u8"\x80\x8d\xe3\x80\x81\xe3\x83\xbb\xe3\x83\xb2\xe3\x82\xa1\xe3\x82\xa3\xe3\x82\xa5\xe3\x82\xa7\xe3\x82"
      u8"\xa9\xe3\x83\xa3\xe3\x83\xa5\xe3\x83\xa7\xe3\x83\x83\xe3\x83\xbc\xe3\x82\xa2\xe3\x82\xa4\xe3\x82\xa6"
      u8"\xe3\x82\xa8\xe3\x82\xaa\xe3\x82\xab\xe3\x82\xad\xe3\x82\xaf\xe3\x82\xb1\xe3\x82\xb3\xe3\x82\xb5\xe3"
      u8"\x82\xb7\xe3\x82\xb9\xe3\x82\xbb\xe3\x82\xbd"

      // Start of 0xff80:
      u8"\xe3\x82\xbf\xe3\x83\x81\xe3\x83\x84\xe3\x83\x86\xe3\x83\x88\xe3\x83\x8a\xe3\x83\x8b\xe3\x83\x8c\xe3"
      u8"\x83\x8d\xe3\x83\x8e\xe3\x83\x8f\xe3\x83\x92\xe3\x83\x95\xe3\x83\x98\xe3\x83\x9b\xe3\x83\x9e\xe3\x83"
      u8"\x9f\xe3\x83\xa0\xe3\x83\xa1\xe3\x83\xa2\xe3\x83\xa4\xe3\x83\xa6\xe3\x83\xa8\xe3\x83\xa9\xe3\x83\xaa"
      u8"\xe3\x83\xab\xe3\x83\xac\xe3\x83\xad\xe3\x83\xaf\xe3\x83\xb3\xe3\x82\x99\xe3\x82\x9a\xe3\x85\xa4\xe3"
      u8"\x84\xb1\xe3\x84\xb2\xe3\x84\xb3\xe3\x84\xb4\xe3\x84\xb5\xe3\x84\xb6\xe3\x84\xb7\xe3\x84\xb8\xe3\x84"
      u8"\xb9\xe3\x84\xba\xe3\x84\xbb\xe3\x84\xbc\xe3\x84\xbd\xe3\x84\xbe\xe3\x84\xbf\xe3\x85\x80\xe3\x85\x81"
      u8"\xe3\x85\x82\xe3\x85\x83\xe3\x85\x84\xe3\x85\x85\xe3\x85\x86\xe3\x85\x87\xe3\x85\x88\xe3\x85\x89\xe3"
      u8"\x85\x8a\xe3\x85\x8b\xe3\x85\x8c\xe3\x85\x8d\xe3\x85\x8e"

      // Start of 0xffc0:
      u8"\0\0\0\0\0\0\xe3\x85\x8f\xe3\x85\x90\xe3\x85\x91\xe3\x85\x92\xe3\x85\x93\xe3\x85\x94\0\0\0\0\0\0\xe3"
      u8"\x85\x95\xe3\x85\x96\xe3\x85\x97\xe3\x85\x98\xe3\x85\x99\xe3\x85\x9a\0\0\0\0\0\0\xe3\x85\x9b\xe3\x85"
      u8"\x9c\xe3\x85\x9d\xe3\x85\x9e\xe3\x85\x9f\xe3\x85\xa0\0\0\0\0\0\0\xe3\x85\xa1\xe3\x85\xa2\xe3\x85\xa3"
      u8"\0\0\0\0\0\0\0\0\0\xc2\xa2\0\xc2\xa3\0\xc2\xac\0\xc2\xaf\0\xc2\xa6\0\xc2\xa5\0\xe2\x82\xa9\0\0\0\xe2"
      u8"\x94\x82\xe2\x86\x90\xe2\x86\x91\xe2\x86\x92\xe2\x86\x93\xe2\x96\xa0\xe2\x97\x8b\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x10780:
      u8"\0\0\0\0\xcb\x90\0\0\xcb\x91\0\0\xc3\xa6\0\0\xca\x99\0\0\xc9\x93\0\0\0\0\0\0\xca\xa3\0\0\xea\xad\xa6"
      u8"\0\xca\xa5\0\0\xca\xa4\0\0\xc9\x96\0\0\xc9\x97\0\0\xe1\xb6\x91\0\xc9\x98\0\0\xc9\x9e\0\0\xca\xa9\0\0"
      u8"\xc9\xa4\0\0\xc9\xa2\0\0\xc9\xa0\0\0\xca\x9b\0\0\xc4\xa7\0\0\xca\x9c\0\0\xc9\xa7\0\0\xca\x84\0\0\xca"
      u8"\xaa\0\0\xca\xab\0\0\xc9\xac\0\0\xf0\x9d\xbc\x84\xea\x9e\x8e\0\xc9\xae\0\0\xf0\x9d\xbc\x85\xca\x8e\0"
      u8"\0\xf0\x9d\xbc\x86\xc3\xb8\0\0\xc9\xb6\0\0\xc9\xb7\0\0\x71\0\0\0\xc9\xba\0\0\xf0\x9d\xbc\x88\xc9\xbd"
      u8"\0\0\xc9\xbe\0\0\xca\x80\0\0\xca\xa8\0\0\xca\xa6\0\0\xea\xad\xa7\0\xca\xa7\0\0\xca\x88\0\0\xe2\xb1"
      u8"\xb1\0\0\0\0\0\xca\x8f\0\0\xca\xa1\0\0\xca\xa2\0\0\xca\x98\0\0\xc7\x80\0\0\xc7\x81\0\0\xc7\x82\0\0"
      u8"\xf0\x9d\xbc\x8a\xf0\x9d\xbc\x9e"

      // Start of 0x11080:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\xf0\x91\x82\x99\xf0\x91\x82\xba\0\0\0\0\0\0\0\0\xf0\x91\x82\x9b\xf0\x91\x82\xba\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xf0\x91\x82\xa5\xf0\x91\x82\xba"

      // Start of 0x11100:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0\x91\x84\xb1\xf0\x91\x84\xa7\xf0\x91\x84\xb2\xf0\x91\x84\xa7"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x11340:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0\x91\x8d\x87\xf0\x91"
      u8"\x8c\xbe\xf0\x91\x8d\x87\xf0\x91\x8d\x97"

      // Start of 0x11480:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0\x91\x92\xb9\xf0\x91\x92\xba\xf0\x91\x92\xb9\xf0\x91"
      u8"\x92\xb0\0\0\0\0\0\0\0\0\xf0\x91\x92\xb9\xf0\x91\x92\xbd"

      // Start of 0x11580:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0\x91\x96\xb8\xf0\x91\x96\xaf\xf0\x91\x96\xb9\xf0\x91\x96\xaf"

      // Start of 0x11900:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0"
      u8"\x91\xa4\xb5\xf0\x91\xa4\xb0"

      // Start of 0x1d140:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0\x9d\x85\x97\xf0"
      u8"\x9d\x85\xa5\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\x9f\xf0\x9d\x85\xae\xf0\x9d\x85\x9f\xf0\x9d"
      u8"\x85\xaf\xf0\x9d\x85\x9f\xf0\x9d\x85\xb0\xf0\x9d\x85\x9f\xf0\x9d\x85\xb1\xf0\x9d\x85\x9f\xf0\x9d\x85"
      u8"\xb2"

      // Start of 0x1d180:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x86\xba\xf0\x9d"
      u8"\x85\xa5\xf0\x9d\x86\xbb\xf0\x9d\x85\xae\xf0\x9d\x86\xbc\xf0\x9d\x85\xae\xf0\x9d\x86\xbb\xf0\x9d\x85"
      u8"\xaf"

      // Start of 0x1d1c0:
      u8"\xf0\x9d\x86\xbc\xf0\x9d\x85\xaf\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1d400:
      u8"\x41\x42\x43\x44"

      // Start of 0x1d640:
      u8"\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54"

      // Start of 0x1d580:
      u8"\x55\x56\x57\x58\x59\x5a\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73"
      u8"\x74\x75\x76\x77\x78\x79\x7a\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c"

      // Start of 0x1d440:
      u8"\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x61\x62\x63\x64\x65\x66\x67\0\x69\x6a\x6b"
      u8"\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a"
      u8"\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58"

      // Start of 0x1d480:
      u8"\x59\x5a\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77"
      u8"\x78\x79\x7a\x41\0\x43\x44\0\0\x47\0\0\x4a\x4b\0\0\x4e\x4f\x50\x51\0\x53\x54\x55\x56\x57\x58\x59\x5a"
      u8"\x61\x62\x63\x64\0\x66\0\x68\x69\x6a"

      // Start of 0x1d4c0:
      u8"\x6b\x6c\x6d\x6e\0\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x41\x42\x43\x44\x45\x46\x47\x48\x49"
      u8"\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x61\x62\x63\x64\x65\x66\x67\x68"
      u8"\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76"

      // Start of 0x1d500:
      u8"\x77\x78\x79\x7a\x41\x42\0\x44\x45\x46\x47\0\0\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\0\x53\x54\x55\x56\x57"
      u8"\x58\x59\0\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76"
      u8"\x77\x78\x79\x7a\x41\x42\0\x44\x45\x46\x47\0"

      // Start of 0x1d540:
      u8"\x49\x4a\x4b\x4c\x4d\0\x4f\0\0\0\x53\x54\x55\x56\x57\x58\x59\0\x61\x62\x63\x64\x65\x66"

      // Start of 0x1d5c0:
      u8"\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72"

      // Start of 0x1d600:
      u8"\x73\x74\x75\x76\x77\x78\x79\x7a\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51"
      u8"\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70"
      u8"\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x41\x42\x43\x44"

      // Start of 0x1d680:
      u8"\x51\0\x52\0\x53\0\x54\0\x55\0\x56\0\x57\0\x58\0\x59\0\x5a\0\x61\0\x62\0\x63\0\x64\0\x65\0\x66\0\x67"
      u8"\0\x68\0\x69\0\x6a\0\x6b\0\x6c\0\x6d\0\x6e\0\x6f\0\x70\0\x71\0\x72\0\x73\0\x74\0\x75\0\x76\0\x77\0"
      u8"\x78\0\x79\0\x7a\0\xc4\xb1\xc8\xb7\0\0\0\0\xce\x91\xce\x92\xce\x93\xce\x94\xce\x95\xce\x96\xce\x97"
      u8"\xce\x98\xce\x99\xce\x9a\xce\x9b\xce\x9c\xce\x9d\xce\x9e\xce\x9f\xce\xa0\xce\xa1\xcf\xb4\xce\xa3\xce"
      u8"\xa4\xce\xa5\xce\xa6\xce\xa7\xce\xa8"

      // Start of 0x1d6c0:
      u8"\xce\xa9\0\xe2\x88\x87\xce\xb1\0\xce\xb2\0\xce\xb3\0\xce\xb4\0"

      // Start of 0x1d700:
      u8"\xce\xb5\0\xce\xb6\0\xce\xb7\0\xce\xb8\0\xce\xb9\0\xce\xba\0"

      // Start of 0x1d740:
      u8"\xce\xbb\0\xce\xbc\0\xce\xbd\0\xce\xbe\0\xce\xbf\0\xcf\x80\0"

      // Start of 0x1d780:
      u8"\xcf\x81\0\xcf\x82\0\xcf\x83\0\xcf\x84\0\xcf\x85\0\xcf\x86\0\xcf\x87\0\xcf\x88\0\xcf\x89\0\xe2\x88"
      u8"\x82\xcf\xb5\0\xcf\x91\0\xcf\xb0\0\xcf\x95\0\xcf\xb1\0\xcf\x96\0\xce\x91\0\xce\x92\0\xce\x93\0\xce"
      u8"\x94\0\xce\x95\0\xce\x96\0\xce\x97\0\xce\x98\0\xce\x99\0\xce\x9a\0\xce\x9b\0\xce\x9c\0\xce\x9d\0\xce"
      u8"\x9e\0\xce\x9f\0\xce\xa0\0\xce\xa1\0\xcf\xb4\0\xce\xa3\0\xce\xa4\0\xce\xa5\0\xce\xa6\0\xce\xa7\0\xce"
      u8"\xa8\0\xce\xa9\0\xe2\x88\x87\xce\xb1\0\xce\xb2\0\xce\xb3\0\xce\xb4\0\xce\xb5\0\xce\xb6\0\xce\xb7\0"
      u8"\xce\xb8\0\xce\xb9\0\xce\xba\0\xce\xbb\0\xce\xbc\0\xce\xbd\0\xce\xbe\0\xce\xbf\0\xcf\x80\0\xcf\x81\0"
      u8"\xcf\x82\0\xcf\x83\0\xcf\x84\0\xcf\x85\0\xcf\x86\0"

      // Start of 0x1d7c0:
      u8"\xcf\x87\0\xcf\x88\0\xcf\x89\0\xe2\x88\x82\xcf\xb5\0\xcf\x91\0\xcf\xb0\0\xcf\x95\0\xcf\xb1\0\xcf\x96"
      u8"\0\xcf\x9c\0\xcf\x9d\0\0\0\0\0\0\0\60\0\0\61\0\0\62\0\0\63\0\0\64\0\0\65\0\0\66\0\0\67\0\0\70\0\0\71"
      u8"\0\0\60\0\0\61\0\0\62\0\0\63\0\0\64\0\0\65\0\0\66\0\0\67\0\0\70\0\0\71\0\0\60\0\0\61\0\0\62\0\0\63\0"
      u8"\0\64\0\0\65\0\0\66\0\0\67\0\0\70\0\0\71\0\0\60\0\0\61\0\0\62\0\0\63\0\0\64\0\0\65\0\0\66\0\0\67\0\0"
      u8"\70\0\0\71\0\0\60\0\0\61\0\0\62\0\0\63\0\0\64\0\0\65\0\0\66\0\0\67\0\0\70\0\0\71"

      // Start of 0x1e000:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\xb0"
      u8"\xd0\xb1\xd0\xb2\xd0\xb3\xd0\xb4\xd0\xb5\xd0\xb6\xd0\xb7\xd0\xb8\xd0\xba\xd0\xbb\xd0\xbc\xd0\xbe\xd0"
      u8"\xbf\xd1\x80\xd1\x81"

      // Start of 0x1e040:
      u8"\xd1\x82\0\xd1\x83\0\xd1\x84\0\xd1\x85\0\xd1\x86\0\xd1\x87\0\xd1\x88\0\xd1\x8b\0\xd1\x8d\0\xd1\x8e\0"
      u8"\xea\x9a\x89\xd3\x99\0\xd1\x96\0\xd1\x98\0\xd3\xa9\0\xd2\xaf\0\xd3\x8f\0\xd0\xb0\0\xd0\xb1\0\xd0\xb2"
      u8"\0\xd0\xb3\0\xd0\xb4\0\xd0\xb5\0\xd0\xb6\0\xd0\xb7\0\xd0\xb8\0\xd0\xba\0\xd0\xbb\0\xd0\xbe\0\xd0\xbf"
      u8"\0\xd1\x81\0\xd1\x83\0\xd1\x84\0\xd1\x85\0\xd1\x86\0\xd1\x87\0\xd1\x88\0\xd1\x8a\0\xd1\x8b\0\xd2\x91"
      u8"\0\xd1\x96\0\xd1\x95\0\xd1\x9f\0\xd2\xab\0\xea\x99\x91\xd2\xb1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1ee00:
      u8"\xd8\xa7\xd8\xa8\xd8\xac\xd8\xaf\0\0\xd9\x88\xd8\xb2\xd8\xad\xd8\xb7\xd9\x8a\xd9\x83\xd9\x84\xd9\x85"
      u8"\xd9\x86\xd8\xb3\xd8\xb9\xd9\x81\xd8\xb5\xd9\x82\xd8\xb1\xd8\xb4\xd8\xaa\xd8\xab\xd8\xae\xd8\xb0\xd8"
      u8"\xb6\xd8\xb8\xd8\xba\xd9\xae\xda\xba\xda\xa1\xd9\xaf\0\0\xd8\xa8\xd8\xac\0\0\xd9\x87\0\0\0\0\xd8\xad"
      u8"\0\0\xd9\x8a\xd9\x83\xd9\x84\xd9\x85\xd9\x86\xd8\xb3\xd8\xb9\xd9\x81\xd8\xb5\xd9\x82\0\0\xd8\xb4\xd8"
      u8"\xaa\xd8\xab\xd8\xae\0\0\xd8\xb6\0\0\xd8\xba\0\0\0\0"

      // Start of 0x1ee40:
      u8"\0\0\0\0\xd8\xac\0\0\0\0\0\0\0\0\xd8\xad\0\0\xd9\x8a\0\0\xd9\x84\0\0\xd9\x86\xd8\xb3\xd8\xb9\0\0\xd8"
      u8"\xb5\xd9\x82\0\0\xd8\xb4\0\0\0\0\xd8\xae\0\0\xd8\xb6\0\0\xd8\xba\0\0\xda\xba\0\0\xd9\xaf\0\0\xd8\xa8"
      u8"\xd8\xac\0\0\xd9\x87\0\0\0\0\xd8\xad\xd8\xb7\xd9\x8a\xd9\x83\0\0\xd9\x85\xd9\x86\xd8\xb3\xd8\xb9\xd9"
      u8"\x81\xd8\xb5\xd9\x82\0\0\xd8\xb4\xd8\xaa\xd8\xab\xd8\xae\0\0\xd8\xb6\xd8\xb8\xd8\xba\xd9\xae\0\0\xda"
      u8"\xa1\0\0"

      // Start of 0x1ee80:
      u8"\xd8\xa7\xd8\xa8\xd8\xac\xd8\xaf\xd9\x87\xd9\x88\xd8\xb2\xd8\xad\xd8\xb7\xd9\x8a\0\0\xd9\x84\xd9\x85"
      u8"\xd9\x86\xd8\xb3\xd8\xb9\xd9\x81\xd8\xb5\xd9\x82\xd8\xb1\xd8\xb4\xd8\xaa\xd8\xab\xd8\xae\xd8\xb0\xd8"
      u8"\xb6\xd8\xb8\xd8\xba\0\0\0\0\0\0\0\0\0\0\xd8\xa8\xd8\xac\xd8\xaf\0\0\xd9\x88\xd8\xb2\xd8\xad\xd8\xb7"
      u8"\xd9\x8a\0\0\xd9\x84\xd9\x85\xd9\x86\xd8\xb3\xd8\xb9\xd9\x81\xd8\xb5\xd9\x82\xd8\xb1\xd8\xb4\xd8\xaa"
      u8"\xd8\xab\xd8\xae\xd8\xb0\xd8\xb6\xd8\xb8\xd8\xba\0\0\0\0\0\0\0\0"

      // Start of 0x1f100:
      u8"\60\56\0\0\0\0\0\60\54\0\0\0\0\0\61\54\0\0\0\0\0\62\54\0\0\0\0\0\63\54\0\0\0\0\0\64\54\0\0\0\0\0\65"
      u8"\54\0\0\0\0\0\66\54\0\0\0\0\0\67\54\0\0\0\0\0\70\54\0\0\0\0\0\71\54\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\50\x41\51\0\0\0\0\50\x42\51\0\0\0\0\50\x43\51\0\0\0"
      u8"\0\50\x44\51\0\0\0\0\50\x45\51\0\0\0\0\50\x46\51\0\0\0\0\50\x47\51\0\0\0\0\50\x48\51\0\0\0\0\50\x49"
      u8"\51\0\0\0\0\50\x4a\51\0\0\0\0\50\x4b\51\0\0\0\0\50\x4c\51\0\0\0\0\50\x4d\51\0\0\0\0\50\x4e\51\0\0\0"
      u8"\0\50\x4f\51\0\0\0\0\50\x50\51\0\0\0\0\50\x51\51\0\0\0\0\50\x52\51\0\0\0\0\50\x53\51\0\0\0\0\50\x54"
      u8"\51\0\0\0\0\50\x55\51\0\0\0\0\50\x56\51\0\0\0\0\50\x57\51\0\0\0\0\50\x58\51\0\0\0\0\50\x59\51\0\0\0"
      u8"\0\50\x5a\51\0\0\0\0\xe3\x80\x94\x53\xe3\x80\x95\x43\0\0\0\0\0\0\x52\0\0\0\0\0\0\x43\x44\0\0\0\0\0"
      u8"\x57\x5a\0\0\0\0\0\0\0\0\0\0\0\0\x41\0\0\0\0\0\0\x42\0\0\0\0\0\0\x43\0\0\0\0\0\0\x44\0\0\0\0\0\0\x45"
      u8"\0\0\0\0\0\0\x46\0\0\0\0\0\0\x47\0\0\0\0\0\0\x48\0\0\0\0\0\0\x49\0\0\0\0\0\0\x4a\0\0\0\0\0\0\x4b\0\0"
      u8"\0\0\0\0\x4c\0\0\0\0\0\0\x4d\0\0\0\0\0\0\x4e\0\0\0\0\0\0\x4f\0\0\0\0\0\0\x50\0\0\0\0\0\0"

      // Start of 0x1f140:
      u8"\x51\0\0\x52\0\0\x53\0\0\x54\0\0\x55\0\0\x56\0\0\x57\0\0\x58\0\0\x59\0\0\x5a\0\0\x48\x56\0\x4d\x56\0"
      u8"\x53\x44\0\x53\x53\0\x50\x50\x56\x57\x43\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\x4d\x43\0\x4d\x44\0\x4d\x52\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1f180:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x44\x4a\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1f200:
      u8"\xe3\x81\xbb\xe3\x81\x8b\xe3\x82\xb3\xe3\x82\xb3\xe3\x82\xb5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\xe6\x89\x8b\0\0\0\xe5\xad\x97\0\0\0\xe5\x8f\x8c\0\0\0\xe3\x83\x87\0\0\0\xe4"
      u8"\xba\x8c\0\0\0\xe5\xa4\x9a\0\0\0\xe8\xa7\xa3\0\0\0\xe5\xa4\xa9\0\0\0\xe4\xba\xa4\0\0\0\xe6\x98\xa0\0"
      u8"\0\0\xe7\x84\xa1\0\0\0\xe6\x96\x99\0\0\0\xe5\x89\x8d\0\0\0\xe5\xbe\x8c\0\0\0\xe5\x86\x8d\0\0\0\xe6"
      u8"\x96\xb0\0\0\0\xe5\x88\x9d\0\0\0\xe7\xb5\x82\0\0\0\xe7\x94\x9f\0\0\0\xe8\xb2\xa9\0\0\0\xe5\xa3\xb0\0"
      u8"\0\0\xe5\x90\xb9\0\0\0\xe6\xbc\x94\0\0\0\xe6\x8a\x95\0\0\0\xe6\x8d\x95\0\0\0\xe4\xb8\x80\0\0\0\xe4"
      u8"\xb8\x89\0\0\0\xe9\x81\x8a\0\0\0\xe5\xb7\xa6\0\0\0\xe4\xb8\xad\0\0\0\xe5\x8f\xb3\0\0\0\xe6\x8c\x87\0"
      u8"\0\0\xe8\xb5\xb0\0\0\0\xe6\x89\x93\0\0\0\xe7\xa6\x81\0\0\0\xe7\xa9\xba\0\0\0\xe5\x90\x88\0\0\0\xe6"
      u8"\xba\x80\0\0\0\xe6\x9c\x89\0\0\0\xe6\x9c\x88\0\0\0\xe7\x94\xb3\0\0\0\xe5\x89\xb2\0\0\0\xe5\x96\xb6\0"
      u8"\0\0\xe9\x85\x8d\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1f240:
      u8"\xe3\x80\x94\xe6\x9c\xac\xe3\x80\x95\xe3\x80\x94\xe4\xb8\x89\xe3\x80\x95\xe3\x80\x94\xe4\xba\x8c\xe3"
      u8"\x80\x95\xe3\x80\x94\xe5\xae\x89\xe3\x80\x95\xe3\x80\x94\xe7\x82\xb9\xe3\x80\x95\xe3\x80\x94\xe6\x89"
      u8"\x93\xe3\x80\x95\xe3\x80\x94\xe7\x9b\x97\xe3\x80\x95\xe3\x80\x94\xe5\x8b\x9d\xe3\x80\x95\xe3\x80\x94"
      u8"\xe6\x95\x97\xe3\x80\x95\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe5\xbe\x97\0\0\0\0\0\0\xe5\x8f\xaf\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1fbc0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\60"
      u8"\61\62\63\64\65\66\67\70\71\0\0\0\0\0\0"

      // Start of 0x2f800:
      u8"\xe4\xb8\xbd\0\xe4\xb8\xb8\0\xe4\xb9\x81\0\xf0\xa0\x84\xa2\xe4\xbd\xa0\0\xe4\xbe\xae\0\xe4\xbe\xbb\0"
      u8"\xe5\x80\x82\0\xe5\x81\xba\0\xe5\x82\x99\0\xe5\x83\xa7\0\xe5\x83\x8f\0\xe3\x92\x9e\0\xf0\xa0\x98\xba"
      u8"\xe5\x85\x8d\0\xe5\x85\x94\0\xe5\x85\xa4\0\xe5\x85\xb7\0\xf0\xa0\x94\x9c\xe3\x92\xb9\0\xe5\x85\xa7\0"
      u8"\xe5\x86\x8d\0\xf0\xa0\x95\x8b\xe5\x86\x97\0\xe5\x86\xa4\0\xe4\xbb\x8c\0\xe5\x86\xac\0\xe5\x86\xb5\0"
      u8"\xf0\xa9\x87\x9f\xe5\x87\xb5\0\xe5\x88\x83\0\xe3\x93\x9f\0\xe5\x88\xbb\0\xe5\x89\x86\0\xe5\x89\xb2\0"
      u8"\xe5\x89\xb7\0\xe3\x94\x95\0\xe5\x8b\x87\0\xe5\x8b\x89\0\xe5\x8b\xa4\0\xe5\x8b\xba\0\xe5\x8c\x85\0"
      u8"\xe5\x8c\x86\0\xe5\x8c\x97\0\xe5\x8d\x89\0\xe5\x8d\x91\0\xe5\x8d\x9a\0\xe5\x8d\xb3\0\xe5\x8d\xbd\0"
      u8"\xe5\x8d\xbf\0\xe5\x8d\xbf\0\xe5\x8d\xbf\0\xf0\xa0\xa8\xac\xe7\x81\xb0\0\xe5\x8f\x8a\0\xe5\x8f\x9f\0"
      u8"\xf0\xa0\xad\xa3\xe5\x8f\xab\0\xe5\x8f\xb1\0\xe5\x90\x86\0\xe5\x92\x9e\0\xe5\x90\xb8\0\xe5\x91\x88\0"
      u8"\xe5\x91\xa8\0"

      // Start of 0x2f840:
      u8"\xe5\x92\xa2\0\xe5\x93\xb6\0\xe5\x94\x90\0\xe5\x95\x93\0\xe5\x95\xa3\0\xe5\x96\x84\0\xe5\x96\x84\0"
      u8"\xe5\x96\x99\0\xe5\x96\xab\0\xe5\x96\xb3\0\xe5\x97\x82\0\xe5\x9c\x96\0\xe5\x98\x86\0\xe5\x9c\x97\0"
      u8"\xe5\x99\x91\0\xe5\x99\xb4\0\xe5\x88\x87\0\xe5\xa3\xae\0\xe5\x9f\x8e\0\xe5\x9f\xb4\0\xe5\xa0\x8d\0"
      u8"\xe5\x9e\x8b\0\xe5\xa0\xb2\0\xe5\xa0\xb1\0\xe5\xa2\xac\0\xf0\xa1\x93\xa4\xe5\xa3\xb2\0\xe5\xa3\xb7\0"
      u8"\xe5\xa4\x86\0\xe5\xa4\x9a\0\xe5\xa4\xa2\0\xe5\xa5\xa2\0\xf0\xa1\x9a\xa8\xf0\xa1\x9b\xaa\xe5\xa7\xac"
      u8"\0\xe5\xa8\x9b\0\xe5\xa8\xa7\0\xe5\xa7\x98\0\xe5\xa9\xa6\0\xe3\x9b\xae\0\xe3\x9b\xbc\0\xe5\xac\x88\0"
      u8"\xe5\xac\xbe\0\xe5\xac\xbe\0\xf0\xa1\xa7\x88\xe5\xaf\x83\0\xe5\xaf\x98\0\xe5\xaf\xa7\0\xe5\xaf\xb3\0"
      u8"\xf0\xa1\xac\x98\xe5\xaf\xbf\0\xe5\xb0\x86\0\xe5\xbd\x93\0\xe5\xb0\xa2\0\xe3\x9e\x81\0\xe5\xb1\xa0\0"
      u8"\xe5\xb1\xae\0\xe5\xb3\x80\0\xe5\xb2\x8d\0\xf0\xa1\xb7\xa4\xe5\xb5\x83\0\xf0\xa1\xb7\xa6\xe5\xb5\xae"
      u8"\0\xe5\xb5\xab\0"

      // Start of 0x2f880:
      u8"\xe5\xb5\xbc\0\xe5\xb7\xa1\0\xe5\xb7\xa2\0\xe3\xa0\xaf\0\xe5\xb7\xbd\0\xe5\xb8\xa8\0\xe5\xb8\xbd\0"
      u8"\xe5\xb9\xa9\0\xe3\xa1\xa2\0\xf0\xa2\x86\x83\xe3\xa1\xbc\0\xe5\xba\xb0\0\xe5\xba\xb3\0\xe5\xba\xb6\0"
      u8"\xe5\xbb\x8a\0\xf0\xaa\x8e\x92\xe5\xbb\xbe\0\xf0\xa2\x8c\xb1\xf0\xa2\x8c\xb1\xe8\x88\x81\0\xe5\xbc"
      u8"\xa2\0\xe5\xbc\xa2\0\xe3\xa3\x87\0\xf0\xa3\x8a\xb8\xf0\xa6\x87\x9a\xe5\xbd\xa2\0\xe5\xbd\xab\0\xe3"
      u8"\xa3\xa3\0\xe5\xbe\x9a\0\xe5\xbf\x8d\0\xe5\xbf\x97\0\xe5\xbf\xb9\0\xe6\x82\x81\0\xe3\xa4\xba\0\xe3"
      u8"\xa4\x9c\0\xe6\x82\x94\0\xf0\xa2\x9b\x94\xe6\x83\x87\0\xe6\x85\x88\0\xe6\x85\x8c\0\xe6\x85\x8e\0\xe6"
      u8"\x85\x8c\0\xe6\x85\xba\0\xe6\x86\x8e\0\xe6\x86\xb2\0\xe6\x86\xa4\0\xe6\x86\xaf\0\xe6\x87\x9e\0\xe6"
      u8"\x87\xb2\0\xe6\x87\xb6\0\xe6\x88\x90\0\xe6\x88\x9b\0\xe6\x89\x9d\0\xe6\x8a\xb1\0\xe6\x8b\x94\0\xe6"
      u8"\x8d\x90\0\xf0\xa2\xac\x8c\xe6\x8c\xbd\0\xe6\x8b\xbc\0\xe6\x8d\xa8\0\xe6\x8e\x83\0\xe6\x8f\xa4\0\xf0"
      u8"\xa2\xaf\xb1\xe6\x90\xa2\0"

      // Start of 0x2f8c0:
      u8"\xe6\x8f\x85\0\xe6\x8e\xa9\0\xe3\xa8\xae\0\xe6\x91\xa9\0\xe6\x91\xbe\0\xe6\x92\x9d\0\xe6\x91\xb7\0"
      u8"\xe3\xa9\xac\0\xe6\x95\x8f\0\xe6\x95\xac\0\xf0\xa3\x80\x8a\xe6\x97\xa3\0\xe6\x9b\xb8\0\xe6\x99\x89\0"
      u8"\xe3\xac\x99\0\xe6\x9a\x91\0\xe3\xac\x88\0\xe3\xab\xa4\0\xe5\x86\x92\0\xe5\x86\x95\0\xe6\x9c\x80\0"
      u8"\xe6\x9a\x9c\0\xe8\x82\xad\0\xe4\x8f\x99\0\xe6\x9c\x97\0\xe6\x9c\x9b\0\xe6\x9c\xa1\0\xe6\x9d\x9e\0"
      u8"\xe6\x9d\x93\0\xf0\xa3\x8f\x83\xe3\xad\x89\0\xe6\x9f\xba\0\xe6\x9e\x85\0\xe6\xa1\x92\0\xe6\xa2\x85\0"
      u8"\xf0\xa3\x91\xad\xe6\xa2\x8e\0\xe6\xa0\x9f\0\xe6\xa4\x94\0\xe3\xae\x9d\0\xe6\xa5\x82\0\xe6\xa6\xa3\0"
      u8"\xe6\xa7\xaa\0\xe6\xaa\xa8\0\xf0\xa3\x9a\xa3\xe6\xab\x9b\0\xe3\xb0\x98\0\xe6\xac\xa1\0\xf0\xa3\xa2"
      u8"\xa7\xe6\xad\x94\0\xe3\xb1\x8e\0\xe6\xad\xb2\0\xe6\xae\x9f\0\xe6\xae\xba\0\xe6\xae\xbb\0\xf0\xa3\xaa"
      u8"\x8d\xf0\xa1\xb4\x8b\xf0\xa3\xab\xba\xe6\xb1\x8e\0\xf0\xa3\xb2\xbc\xe6\xb2\xbf\0\xe6\xb3\x8d\0\xe6"
      u8"\xb1\xa7\0\xe6\xb4\x96\0"

      // Start of 0x2f900:
      u8"\xe6\xb4\xbe\0\xe6\xb5\xb7\0\xe6\xb5\x81\0\xe6\xb5\xa9\0\xe6\xb5\xb8\0\xe6\xb6\x85\0\xf0\xa3\xb4\x9e"
      u8"\xe6\xb4\xb4\0\xe6\xb8\xaf\0\xe6\xb9\xae\0\xe3\xb4\xb3\0\xe6\xbb\x8b\0\xe6\xbb\x87\0\xf0\xa3\xbb\x91"
      u8"\xe6\xb7\xb9\0\xe6\xbd\xae\0\xf0\xa3\xbd\x9e\xf0\xa3\xbe\x8e\xe6\xbf\x86\0\xe7\x80\xb9\0\xe7\x80\x9e"
      u8"\0\xe7\x80\x9b\0\xe3\xb6\x96\0\xe7\x81\x8a\0\xe7\x81\xbd\0\xe7\x81\xb7\0\xe7\x82\xad\0\xf0\xa0\x94"
      u8"\xa5\xe7\x85\x85\0\xf0\xa4\x89\xa3\xe7\x86\x9c\0\xf0\xa4\x8e\xab\xe7\x88\xa8\0\xe7\x88\xb5\0\xe7\x89"
      u8"\x90\0\xf0\xa4\x98\x88\xe7\x8a\x80\0\xe7\x8a\x95\0\xf0\xa4\x9c\xb5\xf0\xa4\xa0\x94\xe7\x8d\xba\0\xe7"
      u8"\x8e\x8b\0\xe3\xba\xac\0\xe7\x8e\xa5\0\xe3\xba\xb8\0\xe3\xba\xb8\0\xe7\x91\x87\0\xe7\x91\x9c\0\xe7"
      u8"\x91\xb1\0\xe7\x92\x85\0\xe7\x93\x8a\0\xe3\xbc\x9b\0\xe7\x94\xa4\0\xf0\xa4\xb0\xb6\xe7\x94\xbe\0\xf0"
      u8"\xa4\xb2\x92\xe7\x95\xb0\0\xf0\xa2\x86\x9f\xe7\x98\x90\0\xf0\xa4\xbe\xa1\xf0\xa4\xbe\xb8\xf0\xa5\x81"
      u8"\x84\xe3\xbf\xbc\0\xe4\x80\x88\0"

      // Start of 0x2f940:
      u8"\xe7\x9b\xb4\0\xf0\xa5\x83\xb3\xf0\xa5\x83\xb2\xf0\xa5\x84\x99\xf0\xa5\x84\xb3\xe7\x9c\x9e\0\xe7\x9c"
      u8"\x9f\0\xe7\x9c\x9f\0\xe7\x9d\x8a\0\xe4\x80\xb9\0\xe7\x9e\x8b\0\xe4\x81\x86\0\xe4\x82\x96\0\xf0\xa5"
      u8"\x90\x9d\xe7\xa1\x8e\0\xe7\xa2\x8c\0\xe7\xa3\x8c\0\xe4\x83\xa3\0\xf0\xa5\x98\xa6\xe7\xa5\x96\0\xf0"
      u8"\xa5\x9a\x9a\xf0\xa5\x9b\x85\xe7\xa6\x8f\0\xe7\xa7\xab\0\xe4\x84\xaf\0\xe7\xa9\x80\0\xe7\xa9\x8a\0"
      u8"\xe7\xa9\x8f\0\xf0\xa5\xa5\xbc\xf0\xa5\xaa\xa7\xf0\xa5\xaa\xa7\xe7\xab\xae\0\xe4\x88\x82\0\xf0\xa5"
      u8"\xae\xab\xe7\xaf\x86\0\xe7\xaf\x89\0\xe4\x88\xa7\0\xf0\xa5\xb2\x80\xe7\xb3\x92\0\xe4\x8a\xa0\0\xe7"
      u8"\xb3\xa8\0\xe7\xb3\xa3\0\xe7\xb4\x80\0\xf0\xa5\xbe\x86\xe7\xb5\xa3\0\xe4\x8c\x81\0\xe7\xb7\x87\0\xe7"
      u8"\xb8\x82\0\xe7\xb9\x85\0\xe4\x8c\xb4\0\xf0\xa6\x88\xa8\xf0\xa6\x89\x87\xe4\x8d\x99\0\xf0\xa6\x8b\x99"
      u8"\xe7\xbd\xba\0\xf0\xa6\x8c\xbe\xe7\xbe\x95\0\xe7\xbf\xba\0\xe8\x80\x85\0\xf0\xa6\x93\x9a\xf0\xa6\x94"
      u8"\xa3\xe8\x81\xa0\0\xf0\xa6\x96\xa8\xe8\x81\xb0\0"

      // Start of 0x2f980:
      u8"\xf0\xa3\x8d\x9f\xe4\x8f\x95\0\xe8\x82\xb2\0\xe8\x84\x83\0\xe4\x90\x8b\0\xe8\x84\xbe\0\xe5\xaa\xb5\0"
      u8"\xf0\xa6\x9e\xa7\xf0\xa6\x9e\xb5\xf0\xa3\x8e\x93\xf0\xa3\x8e\x9c\xe8\x88\x81\0\xe8\x88\x84\0\xe8\xbe"
      u8"\x9e\0\xe4\x91\xab\0\xe8\x8a\x91\0\xe8\x8a\x8b\0\xe8\x8a\x9d\0\xe5\x8a\xb3\0\xe8\x8a\xb1\0\xe8\x8a"
      u8"\xb3\0\xe8\x8a\xbd\0\xe8\x8b\xa6\0\xf0\xa6\xac\xbc\xe8\x8b\xa5\0\xe8\x8c\x9d\0\xe8\x8d\xa3\0\xe8\x8e"
      u8"\xad\0\xe8\x8c\xa3\0\xe8\x8e\xbd\0\xe8\x8f\xa7\0\xe8\x91\x97\0\xe8\x8d\x93\0\xe8\x8f\x8a\0\xe8\x8f"
      u8"\x8c\0\xe8\x8f\x9c\0\xf0\xa6\xb0\xb6\xf0\xa6\xb5\xab\xf0\xa6\xb3\x95\xe4\x94\xab\0\xe8\x93\xb1\0\xe8"
      u8"\x93\xb3\0\xe8\x94\x96\0\xf0\xa7\x8f\x8a\xe8\x95\xa4\0\xf0\xa6\xbc\xac\xe4\x95\x9d\0\xe4\x95\xa1\0"
      u8"\xf0\xa6\xbe\xb1\xf0\xa7\x83\x92\xe4\x95\xab\0\xe8\x99\x90\0\xe8\x99\x9c\0\xe8\x99\xa7\0\xe8\x99\xa9"
      u8"\0\xe8\x9a\xa9\0\xe8\x9a\x88\0\xe8\x9c\x8e\0\xe8\x9b\xa2\0\xe8\x9d\xb9\0\xe8\x9c\xa8\0\xe8\x9d\xab\0"
      u8"\xe8\x9e\x86\0\xe4\x97\x97\0"

      // Start of 0x2f9c0:
      u8"\xe8\x9f\xa1\0\xe8\xa0\x81\0\xe4\x97\xb9\0\xe8\xa1\xa0\0\xe8\xa1\xa3\0\xf0\xa7\x99\xa7\xe8\xa3\x97\0"
      u8"\xe8\xa3\x9e\0\xe4\x98\xb5\0\xe8\xa3\xba\0\xe3\x92\xbb\0\xf0\xa7\xa2\xae\xf0\xa7\xa5\xa6\xe4\x9a\xbe"
      u8"\0\xe4\x9b\x87\0\xe8\xaa\xa0\0\xe8\xab\xad\0\xe8\xae\x8a\0\xe8\xb1\x95\0\xf0\xa7\xb2\xa8\xe8\xb2\xab"
      u8"\0\xe8\xb3\x81\0\xe8\xb4\x9b\0\xe8\xb5\xb7\0\xf0\xa7\xbc\xaf\xf0\xa0\xa0\x84\xe8\xb7\x8b\0\xe8\xb6"
      u8"\xbc\0\xe8\xb7\xb0\0\xf0\xa0\xa3\x9e\xe8\xbb\x94\0\xe8\xbc\xb8\0\xf0\xa8\x97\x92\xf0\xa8\x97\xad\xe9"
      u8"\x82\x94\0\xe9\x83\xb1\0\xe9\x84\x91\0\xf0\xa8\x9c\xae\xe9\x84\x9b\0\xe9\x88\xb8\0\xe9\x8b\x97\0\xe9"
      u8"\x8b\x98\0\xe9\x89\xbc\0\xe9\x8f\xb9\0\xe9\x90\x95\0\xf0\xa8\xaf\xba\xe9\x96\x8b\0\xe4\xa6\x95\0\xe9"
      u8"\x96\xb7\0\xf0\xa8\xb5\xb7\xe4\xa7\xa6\0\xe9\x9b\x83\0\xe5\xb6\xb2\0\xe9\x9c\xa3\0\xf0\xa9\x85\x85"
      u8"\xf0\xa9\x88\x9a\xe4\xa9\xae\0\xe4\xa9\xb6\0\xe9\x9f\xa0\0\xf0\xa9\x90\x8a\xe4\xaa\xb2\0\xf0\xa9\x92"
      u8"\x96\xe9\xa0\x8b\0\xe9\xa0\x8b\0"

      // Start of 0x2fa00:
      u8"\xe9\xa0\xa9\0\xf0\xa9\x96\xb6\xe9\xa3\xa2\0\xe4\xac\xb3\0\xe9\xa4\xa9\0\xe9\xa6\xa7\0\xe9\xa7\x82\0"
      u8"\xe9\xa7\xbe\0\xe4\xaf\x8e\0\xf0\xa9\xac\xb0\xe9\xac\x92\0\xe9\xb1\x80\0\xe9\xb3\xbd\0\xe4\xb3\x8e\0"
      u8"\xe4\xb3\xad\0\xe9\xb5\xa7\0\xf0\xaa\x83\x8e\xe4\xb3\xb8\0\xf0\xaa\x84\x85\xf0\xaa\x88\x8e\xf0\xaa"
      u8"\x8a\x91\xe9\xba\xbb\0\xe4\xb5\x96\0\xe9\xbb\xb9\0\xe9\xbb\xbe\0\xe9\xbc\x85\0\xe9\xbc\x8f\0\xe9\xbc"
      u8"\x96\0\xe9\xbc\xbb\0\xf0\xaa\x98\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0",
      // done.
      46452UL // String Length
    };

    template <typename CharT = char8_t, typename CPType>
        requires(sizeof(CharT) == sizeof(char8_t))
    [[nodiscard]] static constexpr CharT const* decomp_ptr(
      decomp_index const code, CPType const code_point) noexcept {
        if constexpr (stl::same_as<CharT, char8_t>) {
            return decomp_values.data() + code.get_position(code_point);
        } else {
            // Legally we can't cast a "char const*" to "char8_t const*",
            // but we can cast a "char8_t const*" to "char const*"; this is a very weird C++ behavior, that's
            // why we chose u8-based strings in the values table above instead of traditional values.
            // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
            return reinterpret_cast<CharT const*>(decomp_values.data()) + code.get_position(code_point);
        }
    }

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_DECOMPOSITION_TABLES_HPP
