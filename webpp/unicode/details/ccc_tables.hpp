
/**
 * Attention: Auto-generated file, don't modify.
 *
 *   Auto generated from:                generate_ccc_tables.mjs
 *   Unicode UCD Database Creation Date: 2023-08-28
 *   This file's generation date:        Sun, 02 Jun 2024 05:21:43 GMT
 *   Unicode Version:                    15.1.0
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
 */

#ifndef WEBPP_UNICODE_CCC_TABLES_HPP
#define WEBPP_UNICODE_CCC_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {




    /**
     * CCC (Index Table)
     * Canonical Combining Class
     */
    struct alignas(std::uint32_t) CCC_index {
        /// This is the position that should be looked for in the values table.
        std::uint16_t pos;

        /// This is used to mask the 'remaining position' of the values table;
        /// meaning, instead of getting the values_table[0x12'34], we would get values_table[0x12'00].
        /// The mask does not apply to the whole index, but only to the remaining index.
        std::uint8_t mask;

        /**
         * [16bits = pos] + [8bits = mask]
         */
        explicit(false) consteval CCC_index(std::uint32_t const value) noexcept
          : pos{static_cast<std::uint16_t>(value >> 8U)},
            mask{static_cast<std::uint8_t>(value)} {}

        [[nodiscard]] constexpr std::uint32_t value() const noexcept {
            return static_cast<std::uint32_t>(mask) | (static_cast<std::uint32_t>(pos) << 8U);
        }

        static constexpr std::uint16_t chunk_mask  = 0xFFU;
        static constexpr std::size_t   chunk_size  = 256U;
        static constexpr std::uint8_t  chunk_shift = 8U;

        /**
         * Apply the mask to the position specified in the input.
         * Attention: input is the remaining part of the position, meaning
         *    remaining_pos = (code_point & 255); // 255 refers to chunk_mask
         */
        [[nodiscard]] constexpr std::uint16_t masked(auto const remaining_pos) const noexcept {
            return static_cast<std::uint16_t>(mask) & static_cast<std::uint16_t>(remaining_pos);
        }

        /**
         * Get the final position of the second table.
         * This does not apply the shift or get the value of the second table for you; this only applies tha
         * mask.
         */
        [[nodiscard]] constexpr std::uint16_t get_position(auto const request_position) const noexcept {
            auto const range                = static_cast<std::uint16_t>(request_position) >> chunk_shift;
            auto const remaining_pos        = static_cast<std::uint16_t>(request_position) & chunk_mask;
            auto const masked_remaining_pos = masked(remaining_pos);
            return range + masked_remaining_pos;
        }
    };

    /**
     * CCC Index Table
     *
     * CCC: Canonical Combining Class
     * These are the indices that are used to find which values from "ccc_values" table correspond to a
     * unicode code point.
     *
     * Each value contains 3 numbers:
     *     [16bits: pos] + [8bits: shift] + [8bits: mask]
     *
     *   - pos:   it's the index that points to the `ccc_values` table.
     *            it's the starting point of a (at most) 256 length CCC values.
     *
     *   - shift: add this value to the CCC value, after you applied the mask and found
     *            the actual position of the CCC value.
     *
     *   - mask:  apply this mask (with an & operator), to this: pos + (code_point % 256)
     *            which means, in order to get the CCC value of a `code_point`, you need to do this:
     *            @code
     *                auto code  = ccc_index[code_point >> 8]
     *                auto shift = (code >> 8) & 0xFF;
     *                auto mask  = code & 0xFF;
     *                auto pos   = (code >> 16) & 0xFF;
     *                auto ccc   = ccc_values[pos + ((code_point % 256) & mask)] + shift;
     *            @endcode
     *
     * Table size:
     *   - in bits:       15648
     *   - in bytes:      1956 B
     *   - in KibiBytes:  2 KiB
     */
    static constexpr std::array<CCC_index, 489ULL> ccc_indices{
      254,   0,     0,     510,   766,   1022,  1278,  1534,  1790,  2046,  2302,  2558,  2814,  3070, 3326,
      3582,  3838,  0,     0,     4094,  0,     0,     0,     4350,  4606,  4862,  5118,  5374,  5630, 5886,
      0,     0,     6142,  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    6398,
      6654,  0,     0,     6910,  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     7166,  0,     7422,  7678,  7934,  8190,  0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     8446,  0,     0,    8702,
      0,     0,     8958,  9214,  9470,  0,     0,     0,     0,     0,     0,     9726,  0,     0,    7678,
      10238, 10494, 10750, 11006, 11262, 11518, 11774, 12030, 12286, 12542, 12798, 13054, 13310, 0,    13566,
      13822, 0,     14078, 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     14334, 14590, 0,     0,     0,     11262, 0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     15102, 0,     0,     0,     0,    0,
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      15358, 15614, 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,    0,
      15870, 16126, 16382, 0,     16638, 0,     0,     0,     16894};


    /**
     * CCC Table
     *
     * CCC: Canonical Combining Class
     * These values are calculated and individually represent actual CCC values, but they have no
     * valid order by themselves, and they only make sense if they're being used in conjunction with
     * the "ccc_indices" table.
     *
     * Table size:
     *   - in bits:       536
     *   - in bytes:      67 B
     *   - in KibiBytes:  1 KiB
     */
    static constexpr std::array<std::uint8_t, 67ULL> ccc_values{

      // Start of 0x0, 0x100, 0x200, 0x1100, 0x1200, 0x1400, 0x1500, 0x1600, 0x1e00, 0x1f00, 0x2100, 0x2200,
      // 0x2300, 0x2400, 0x2500, 0x2600, 0x2700, 0x2800, 0x2900, 0x2a00, 0x2b00, 0x2e00, 0x2f00, 0x3100,
      // 0x3200, 0x3300, 0x3400, 0x3500, 0x3600, 0x3700, 0x3800, 0x3900, 0x3a00, 0x3b00, 0x3c00, 0x3d00,
      // 0x3e00, 0x3f00, 0x4000, 0x4100, 0x4200, 0x4300, 0x4400, 0x4500, 0x4600, 0x4700, 0x4800, 0x4900,
      // 0x4a00, 0x4b00, 0x4c00, 0x4d00, 0x4e00, 0x4f00, 0x5000, 0x5100, 0x5200, 0x5300, 0x5400, 0x5500,
      // 0x5600, 0x5700, 0x5800, 0x5900, 0x5a00, 0x5b00, 0x5c00, 0x5d00, 0x5e00, 0x5f00, 0x6000, 0x6100,
      // 0x6200, 0x6300, 0x6400, 0x6500, 0x6600, 0x6700, 0x6800, 0x6900, 0x6a00, 0x6b00, 0x6c00, 0x6d00,
      // 0x6e00, 0x6f00, 0x7000, 0x7100, 0x7200, 0x7300, 0x7400, 0x7500, 0x7600, 0x7700, 0x7800, 0x7900,
      // 0x7a00, 0x7b00, 0x7c00, 0x7d00, 0x7e00, 0x7f00, 0x8000, 0x8100, 0x8200, 0x8300, 0x8400, 0x8500,
      // 0x8600, 0x8700, 0x8800, 0x8900, 0x8a00, 0x8b00, 0x8c00, 0x8d00, 0x8e00, 0x8f00, 0x9000, 0x9100,
      // 0x9200, 0x9300, 0x9400, 0x9500, 0x9600, 0x9700, 0x9800, 0x9900, 0x9a00, 0x9b00, 0x9c00, 0x9d00,
      // 0x9e00, 0x9f00, 0xa000, 0xa100, 0xa200, 0xa300, 0xa400, 0xa500, 0xa700, 0xac00, 0xad00, 0xae00,
      // 0xaf00, 0xb000, 0xb100, 0xb200, 0xb300, 0xb400, 0xb500, 0xb600, 0xb700, 0xb800, 0xb900, 0xba00,
      // 0xbb00, 0xbc00, 0xbd00, 0xbe00, 0xbf00, 0xc000, 0xc100, 0xc200, 0xc300, 0xc400, 0xc500, 0xc600,
      // 0xc700, 0xc800, 0xc900, 0xca00, 0xcb00, 0xcc00, 0xcd00, 0xce00, 0xcf00, 0xd000, 0xd100, 0xd200,
      // 0xd300, 0xd400, 0xd500, 0xd600, 0xd700, 0xd800, 0xd900, 0xda00, 0xdb00, 0xdc00, 0xdd00, 0xde00,
      // 0xdf00, 0xe000, 0xe100, 0xe200, 0xe300, 0xe400, 0xe500, 0xe600, 0xe700, 0xe800, 0xe900, 0xea00,
      // 0xeb00, 0xec00, 0xed00, 0xee00, 0xef00, 0xf000, 0xf100, 0xf200, 0xf300, 0xf400, 0xf500, 0xf600,
      // 0xf700, 0xf800, 0xf900, 0xfa00, 0xfc00, 0xfd00, 0xff00, 0x10000, 0x10400, 0x10500, 0x10600, 0x10700,
      // 0x10800, 0x10900, 0x10b00, 0x10c00, 0x11b00, 0x11e00, 0x12000, 0x12100, 0x12200, 0x12300, 0x12400,
      // 0x12500, 0x12600, 0x12700, 0x12800, 0x12900, 0x12a00, 0x12b00, 0x12c00, 0x12d00, 0x12e00, 0x12f00,
      // 0x13000, 0x13100, 0x13200, 0x13300, 0x13400, 0x13500, 0x13600, 0x13700, 0x13800, 0x13900, 0x13a00,
      // 0x13b00, 0x13c00, 0x13d00, 0x13e00, 0x13f00, 0x14000, 0x14100, 0x14200, 0x14300, 0x14400, 0x14500,
      // 0x14600, 0x14700, 0x14800, 0x14900, 0x14a00, 0x14b00, 0x14c00, 0x14d00, 0x14e00, 0x14f00, 0x15000,
      // 0x15100, 0x15200, 0x15300, 0x15400, 0x15500, 0x15600, 0x15700, 0x15800, 0x15900, 0x15a00, 0x15b00,
      // 0x15c00, 0x15d00, 0x15e00, 0x15f00, 0x16000, 0x16100, 0x16200, 0x16300, 0x16400, 0x16500, 0x16600,
      // 0x16700, 0x16800, 0x16900, 0x16c00, 0x16d00, 0x16e00, 0x17000, 0x17100, 0x17200, 0x17300, 0x17400,
      // 0x17500, 0x17600, 0x17700, 0x17800, 0x17900, 0x17a00, 0x17b00, 0x17c00, 0x17d00, 0x17e00, 0x17f00,
      // 0x18000, 0x18100, 0x18200, 0x18300, 0x18400, 0x18500, 0x18600, 0x18700, 0x18800, 0x18900, 0x18a00,
      // 0x18b00, 0x18c00, 0x18d00, 0x18e00, 0x18f00, 0x19000, 0x19100, 0x19200, 0x19300, 0x19400, 0x19500,
      // 0x19600, 0x19700, 0x19800, 0x19900, 0x19a00, 0x19b00, 0x19c00, 0x19d00, 0x19e00, 0x19f00, 0x1a000,
      // 0x1a100, 0x1a200, 0x1a300, 0x1a400, 0x1a500, 0x1a600, 0x1a700, 0x1a800, 0x1a900, 0x1aa00, 0x1ab00,
      // 0x1ac00, 0x1ad00, 0x1ae00, 0x1af00, 0x1b000, 0x1b100, 0x1b200, 0x1b300, 0x1b400, 0x1b500, 0x1b600,
      // 0x1b700, 0x1b800, 0x1b900, 0x1ba00, 0x1bb00, 0x1bd00, 0x1be00, 0x1bf00, 0x1c000, 0x1c100, 0x1c200,
      // 0x1c300, 0x1c400, 0x1c500, 0x1c600, 0x1c700, 0x1c800, 0x1c900, 0x1ca00, 0x1cb00, 0x1cc00, 0x1cd00,
      // 0x1ce00, 0x1cf00, 0x1d000, 0x1d300, 0x1d400, 0x1d500, 0x1d600, 0x1d700, 0x1d800, 0x1d900, 0x1da00,
      // 0x1db00, 0x1dc00, 0x1dd00, 0x1de00, 0x1df00, 0x1e300, 0x1e500, 0x1e600, 0x1e700:
      0,
      // Start of 0x300:
      230,
      // Start of 0x400:
      0,
      // Start of 0x500:
      0,
      // Start of 0x600:
      0,
      // Start of 0x700:
      0,
      // Start of 0x800:
      0,
      // Start of 0x900:
      0,
      // Start of 0xa00:
      0,
      // Start of 0xb00:
      0,
      // Start of 0xc00:
      0,
      // Start of 0xd00:
      0,
      // Start of 0xe00:
      0,
      // Start of 0xf00:
      0,
      // Start of 0x1000:
      0,
      // Start of 0x1300:
      0,
      // Start of 0x1700:
      0,
      // Start of 0x1800:
      0,
      // Start of 0x1900:
      0,
      // Start of 0x1a00:
      0,
      // Start of 0x1b00:
      0,
      // Start of 0x1c00:
      0,
      // Start of 0x1d00:
      0,
      // Start of 0x2000:
      0,
      // Start of 0x2c00:
      0,
      // Start of 0x2d00:
      0,
      // Start of 0x3000:
      0,
      // Start of 0xa600:
      0,
      // Start of 0xa800:
      9,
      // Start of 0xa900, 0x10d00:
      0,
      // Start of 0xaa00:
      0,
      // Start of 0xab00:
      0,
      // Start of 0xfb00:
      0,
      // Start of 0xfe00:
      0,
      // Start of 0x10100:
      0,
      // Start of 0x10200:
      0,
      // Start of 0x10300:
      0,
      // Start of 0x10a00:
      0, 0,
      // Start of 0x10e00:
      0,
      // Start of 0x10f00:
      0,
      // Start of 0x11000:
      0,
      // Start of 0x11100:
      230,
      // Start of 0x11200, 0x16f00:
      0,
      // Start of 0x11300:
      0,
      // Start of 0x11400:
      0,
      // Start of 0x11500:
      0,
      // Start of 0x11600:
      0,
      // Start of 0x11700:
      0,
      // Start of 0x11800:
      0,
      // Start of 0x11900:
      0,
      // Start of 0x11a00:
      0,
      // Start of 0x11c00:
      0,
      // Start of 0x11d00:
      0,
      // Start of 0x11f00:
      0,
      // Start of 0x16a00:
      0,
      // Start of 0x16b00:
      0, 0,
      // Start of 0x1bc00:
      0,
      // Start of 0x1d100:
      0,
      // Start of 0x1d200:
      0,
      // Start of 0x1e000:
      230,
      // Start of 0x1e100:
      0,
      // Start of 0x1e200:
      0,
      // Start of 0x1e400:
      0,
      // Start of 0x1e800:
      0, 0};

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_CCC_TABLES_HPP
