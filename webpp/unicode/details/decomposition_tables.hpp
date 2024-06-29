
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_decomposition_tables.mjs
 *   Unicode UCD Database Creation Date: 2023-08-28
 *   This file's generation date:        Sat, 29 Jun 2024 23:04:27 GMT
 *   Unicode Version:                    15.1.0
 *   Total Table sizes in this file:
 *       - in bits:       184952
 *       - in bytes:      23119 B
 *       - in KibiBytes:  23 KiB
 *   Some other implementations' total table size was 73.4 KiB;
 *   So I saved 51 KiB and a better a locality.
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

#include <array>
#include <cstdint>
#include <string_view>

namespace webpp::unicode::details {


    /**
     * In "decomposition_index" table, any code point bigger than this number will be "non-mapped" (it's
     * mapped to the input code point by standard); so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_mapped_deomps = 0x2FA1EUL;

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


        /// This is the position that should be looked for in the values table.
        std::uint16_t pos;

        /// Length of the UTF-8 Encoded Decomposition Code Points.
        /// This value is the max length of each mappings; there should be additional empty values added
        /// in between the values of the values table in order to make sure we can easily find the needed
        /// mappings for all the code points without searching for them.
        char8_t max_length;

        /**
         * [16bits = pos] + [8bits = max_length]
         */
        explicit(false) consteval decomp_index(std::uint32_t const value) noexcept
          : pos{static_cast<std::uint16_t>(value >> pos_shift)},
            max_length{static_cast<char8_t>(value)} {}

        [[nodiscard]] constexpr std::uint32_t value() const noexcept {
            return static_cast<std::uint32_t>(max_length) | (static_cast<std::uint32_t>(pos) << pos_shift);
        }

        static constexpr std::uint16_t chunk_mask  = 0xFFU;
        static constexpr std::size_t   chunk_size  = 256U;
        static constexpr std::uint8_t  chunk_shift = 8U;



        /// Maximum value of "max_length" in the whole values table.
        /// It's the amount of mapped UTF-8 "bytes" (not code points).
        /// Hope this can enable some optimizations.
        static constexpr auto max_max_length = 32UL;

        /**
         * Get the final position of the second table.
         * This does not apply the shift or get the value of the second table for you; this only applies tha
         * mask.
         */
        [[nodiscard]] constexpr std::uint16_t get_position(auto const request_position) const noexcept {
#if __cplusplus >= 202302L // C++23
            [[assume(max_length <= max_max_length)]];
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
     *   - in bits:       24384
     *   - in bytes:      3048 B
     *   - in KibiBytes:  3 KiB
     */
    static constexpr std::array<decomp_index, 762ULL> decomp_indices{
      4,       65284,   205315,  253956,  296453,  353284,  354308,  0,       0,       366595,  375810,
      374274,  375810,  375298,  157186,  373250,  383235,  0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       383746,  0,       389378,  435716,  680196,  916744,
      1032197, 1145093, 1198851, 1200388, 0,       0,       0,       0,       0,       1327108, 0,
      555265,  1281,    1281,    1332227, 1491462, 1588483, 1646857, 2172680, 0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       1281,    2641666, 0,       0,       0,       6657,    0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       2643459, 2836995, 2993668, 3195910,
      3499040, 3778564, 3968771, 0,       0,       0,       0,       0,       0,       0,       4092674,
      0,       0,       0,       0,       0,       0,       0,       0,       4120580, 4123654, 0,
      4126726, 107522,  107522,  0,       0,       0,       4129286, 0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       942338,  0,       0,       4130817, 4193281, 4215811, 4286211, 0,
      0,       0,       0,       0,       0,       0,       0,       4470531, 0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       4510466,
      0,       0,       4582663, 4680201, 0,       0,       0,       0,       0,       0,       0,
      0,       3972609, 0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       4738051, 4928259};

    /**
     * DECOMP Values Table
     *
     * Decomposition Index
     *
     * Table size:
     *   - in bits:       160568
     *   - in bytes:      20071 B
     *   - in KibiBytes:  20 KiB
     */
    static constexpr std::basic_string_view<char8_t> decomp_values{

      // Start of 0x0, 0x700-0x800, 0x1100-0x1a00, 0x1c00, 0x2500-0x2900, 0x2b00, 0x3400-0xa500,
      // 0xa800-0xaa00, 0xac00-0xf800, 0x10000-0x10600, 0x10800-0x10f00, 0x11200, 0x11600-0x11800,
      // 0x11a00-0x1d000, 0x1d200-0x1d300, 0x1d800-0x1df00, 0x1e100-0x1ed00, 0x1ef00-0x1f000, 0x1f300-0x1fa00,
      // 0x1fc00-0x2f700:
      u8"\24\24\xc4\xb4\75"

      // Start of 0x2d00-0x2e00, 0xa600:
      u8"\0\0\24\xc4\xb0\40\0\0\41\0\0\24\xc4\xad\0\0\0\24\xc5\x87\37"

      // Start of 0xab00:
      u8"\0\0\0\0\0\37\xdf\xbc\42\37\xdf\xbc\40\41\xdf\xbc\42\51\xc4\xac\0\51\xc4\xad\0\51\xc4\xae\0\51\xc4"
      u8"\xaf\0\51\xc4\xb4\0\51\0\0\0\53\xc5\x87\0\55\xc4\xac\0\55\xc4\xad\0\55\xc4\xae\0\55\xc4\xb4\0\61\xc4"
      u8"\xac\0\61\xc4\xad\0\61\xc4\xae\0\61\xc4\xb4\0\0\xc4\xaf\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xae\0\0\xc4"
      u8"\xaf\0\0\xc4\xb4\0\67\xc4\xac\0\67\xc4\xad\0\67\xc4\xae\0\67\xc4\xb4\0\73\xc4\xad\0\75\xc4\xac\0\75"
      u8"\xc4\xad\0\75\xc4\xae\0\75\xc4\xaf\0\75\xc4\xb4\0\75\0\0\0\77\xc5\x87\0\x41\xc4\xac\0\x41\xc4\xad\0"
      u8"\x41\xc4\xae\0\x41\xc4\xb4\0\x45\xc4\xac\0\x45\xc4\xad\0\x45\xc4\xae\0\x45\xc4\xb4\0\0\xc4\xaf\0\0"
      u8"\xc4\xac\0\0\xc4\xad\0\0\xc4\xae\0\0\xc4\xaf\0\0\xc4\xb4\0\x4b\xc4\xac\0\x4b\xc4\xad\0\x4b\xc4\xae\0"
      u8"\x4b\xc4\xb4\0\x4f\xc4\xad\0\x4f\xc4\xb4\0"

      // Start of 0x100:
      u8"\51\xc4\xb0\75\xc4\xb0\51\xc4\xb2\75\xc4\xb2\51\xc5\x88\75\xc5\x88\53\xc4\xad\77\xc4\xad\53\xc4\xae"
      u8"\77\xc4\xae\53\xc4\xb3\77\xc4\xb3\53\0\0\77\0\0\54\0\0\x40\0\0\55\xc4\xb0\x41\xc4\xb0\55\xc4\xb2\x41"
      u8"\xc4\xb2\55\xc4\xb3\x41\xc4\xb3\55\xc5\x88\x41\xc5\x88\55\0\0\x41\0\0\57\xc4\xae\x43\xc4\xae\57\xc4"
      u8"\xb2\x43\xc4\xb2\57\xc4\xb3\x43\xc4\xb3\57\xc5\x87\x43\xc5\x87\60\xc4\xae\x44\xc4\xae\61\xc4\xaf\x45"
      u8"\xc4\xaf\61\xc4\xb0\x45\xc4\xb0\61\xc4\xb2\x45\xc4\xb2\61\xc5\x88\x45\xc5\x88\61\xc4\xb3\61\0\0\x45"
      u8"\0\0\0\xc4\xae\0\xc4\xae\0\xc5\x87\0\xc5\x87\0\xc4\xad\0\xc4\xad\0\xc5\x87\0\xc5\x87"

      // Start of 0x11400-0x11500:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\xc4\xad\0\xc4\xad\0\xc5\x87\0\xc5\x87\0\0\0\0\0\0\0\0\0\0\xc4\xb0\0\xc4"
      u8"\xb0\0\xc4\xb2\0\xc4\xb2\0\0\0\0\0\0\64\xc4\xad\x48\xc4\xad\64\xc5\x87\x48\xc5\x87\64\0\0\x48\0\0\65"
      u8"\xc4\xad\x49\xc4\xad\65\xc4\xae\x49\xc4\xae\65\xc5\x87\x49\xc5\x87\65\0\0\x49\0\0\66\xc5\x87\x4a\xc5"
      u8"\x87\66\0\0\x4a\0\0\67\xc4\xaf\x4b\xc4\xaf\67\xc4\xb0\x4b\xc4\xb0\67\xc4\xb2\x4b\xc4\xb2\67\0\0\x4b"
      u8"\0\0\67\0\0\x4b\0\0\67\xc5\x88\x4b\xc5\x88\71\xc4\xae\x4d\xc4\xae\73\xc4\xae\x4f\xc4\xae\73\xc4\xb4"
      u8"\0\xc4\xad\0\xc4\xad\0\xc4\xb3\0\xc4\xb3\0\0\0\0\0\0\x49\0\0\0\0\0\0\0\0\67\0\0\x4b\0\0\54\0\0\54\0"

      // Start of 0xe00:
      u8"\0\x40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\51\0\0\75\0\0\61\0\0\x45\0\0\0\0\0\0\0\0\67\0\0\x4b\0"
      u8"\0\0\xc4\xb0\0\xc4\xb0\0\xc4\xad\0\xc4\xad\0\0\0\0\0\0\0\xc4\xac\0\xc4\xac\0\xc4\xb0\0\xc4\xb0\xc3"
      u8"\xa2\xc4\xb0\xc3\xa3\xc4\xb0\0\xc4\xb0\0\0\xc4\xb0\0\57\0\0\0\x43\0\0\0\0\0\0\0\0\0\0\0\0\xc5\x88\0"
      u8"\0\xc5\x88\0\0\xc4\xb0\0\0\xc4\xb0\0\0\0\0\0\xc4\xa4\0\0\0\0\0\0\54\0\0\0\54\0\0\0\x40\0\0\0\57\xc4"
      u8"\xad\0\x43\xc4\xad\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4"
      u8"\xad\0\0\xc4\xad\0"

      // Start of 0x200:
      u8"\51\0\75\0\51\xc4\xb7\75\xc4\xb7\55\0\0\x41\0\0\55\xc4\xb7\x41\xc4\xb7\61\0\0\x45\0\0\61\xc4\xb7\x45"
      u8"\xc4\xb7\0\0\0\0\0\0\0\xc4\xb7\0\xc4\xb7\64\0\0\x48\0\0\64\xc4\xb7\x48\xc4\xb7\67\0\0\x4b\0\0\67\xc4"
      u8"\xb7\x4b\xc4\xb7\65\xc5\x86\x49\xc5\x86\66\xc5\x86\x4a\xc5\x86\60\0\0\x44\0\0\51\xc4\xb3\75\xc4\xb3"
      u8"\55\xc5\x87\x41\xc5\x87\0\xc4\xb0\0\xc4\xb0\0\xc4\xb0\0\xc4\xb0\0\xc4\xb3\0\xc4\xb3\0\xc4\xb0\0\xc4"
      u8"\xb0\73\xc4\xb0\x4f\xc4\xb0\x44\0\0\xc4\x8a\0\0\0\0\x48\0\0\xc4\x97\0\0\0\0\xc4\x99\0\x4d\0\0\x4f\0"
      u8"\0\24\xc4\xb2\24\xc4\xb3\24\0\0\24\xc5\x88\24\xc4\xaf\24\0\0\xc4\x87\0\0\0\0\x49\0\0\x4e\0\0\xc4\xa7"
      u8"\0"

      // Start of 0x300:
      u8"\xc4\xac\xc4\xad\xc4\xb9\xc4\xb4\xc4\xad\0\0\0\0\24\xc5\x99\0\0\0\0\0\24\xc4\xad\0\0\xc4\xad\0\xc6"
      u8"\x87\xc4\xad\0\0\0\0\xc6\x8b\xc4\xad\xc6\x8d\xc4\xad\xc6\x8f\xc4\xad\0\xc4\xad\0\0\xc4\xad\0\0\xc4"
      u8"\xad\0\0\xc4\xad\0\xc6\x8f\xc4\xb4\0\xc4\xb4\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4"
      u8"\xad\0\0\xc4\xb4\0\0\xc4\xb4\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xad\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc4\xad"
      u8"\0\0\xc4\xb4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc6\x8e\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x400:
      u8"\xc6\x9f\xc4\xac\xc6\x9f\xc4\xb4\xc6\x9d\xc4\xad\xc6\x96\xc4\xb4\0\xc4\xad\0\xc6\xa2\xc4\xac\xc6\xa7"
      u8"\xc4\xb2\xc6\xa2\xc4\xb2\xc6\xb6\xc4\xb2\xc6\xb3\xc4\xac\xc6\xb3\xc4\xb4\xc6\xb1\xc4\xad\xc7\x88\xc4"
      u8"\xb4\0\xc4\xad\0\xc6\xb6\xc4\xac\xc6\xbb\xc4\xb2\xc7\x9a\0\0\xc7\x9b\0\0\xc6\xa0\xc4\xb2\xc6\xb4\xc4"
      u8"\xb2\xc6\x9a\xc4\xb2\xc6\xae\xc4\xb2\xc6\x9a\xc4\xb4\xc6\xae\xc4\xb4\xc6\x9f\xc4\xb2\xc6\xb3\xc4\xb2"
      u8"\0\xc4\xb4\0\0\xc4\xb4\0\xc6\xa0\xc4\xb4\xc6\xb4\xc4\xb4\xc6\xa1\xc4\xb4\xc6\xb5\xc4\xb4\xc6\xa2\xc4"
      u8"\xb0\xc6\xb6\xc4\xb0\xc6\xa2\xc4\xb4\xc6\xb6\xc4\xb4\0\xc4\xb4\0\0\xc4\xb4\0\xe8\x90\x80\xc4\xb4\xe2"
      u8"\xa0\x80\xc4\xb4\0\xc4\xb4\0\0\0\xc4\xb4\0\0\xc6\xa7\xc4\xb0\0\xc6\xbb\xc4\xb0\0\xc6\xa7\xc4\xb4\0"
      u8"\xc6\xbb\xc4\xb4\0\xc6\xa7\0\0\0\xc6\xbb\0\0\0\xc6\xab\xc4\xb4\0\xc6\xbf\xc4\xb4\0\0\xc4\xb4\0\0\0"
      u8"\xc4\xb4\0\0"

      // Start of 0x500:
      u8"\xc8\xb5\xc9\x86"

      // Start of 0x600:
      u8"\xc9\xb3\xca\x8d\xc9\xb3\xca\x8e\xca\x88\xca\x8e\xc9\xb3\xca\x8f\0\xca\x8e\0\xc9\xb3\xca\xa2\xca\x88"
      u8"\xca\xa2\0\xca\xa2\0\0\xca\xa2\0\0\xca\x8e\0\0\xca\x8e\0\0\xca\x8e\0"

      // Start of 0x900:
      u8"\xce\xa0\0\xce\xa2\0\xce\xa5\0\xce\x93\0\xce\x94\0\xce\x95\0\0\0\0\xce\x99\0\xce\x9a"

      // Start of 0xf00:
      u8"\0\0\0\0"

      // Start of 0xb00:
      u8"\0\0\0\0"

      // Start of 0xd00:
      u8"\0\0"

      // Start of 0xa00, 0xc00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1000:
      u8"\xd0\x81"

      // Start of 0x1b00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1d00:
      u8"\51\0\52\54\55\0\57\60\61\0\0\0\0\0\0\xc3\x9e\62\0\64\0\66\0\67\0\71\0\75\0\xc3\xba\xc3\xbb\0\0\76\0"
      u8"\x40\0\x41\0\xc4\x83\0\0\0\0\x43\0\0\0\0\0\0\0\0\0\xc3\xbe\0\0\0\0\x46\0\x4a\0\x4b\0\0\0\0\0\x4c\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\x45\0\x48\0\x4b\0\x4c\0\0\0\0\0\0\0\0\0\0\0\0\0\xc3\xbc\77\0\xc3\xbf\0\0\0\0"
      u8"\x42\0\0\0\xc4\x85\xc4\x89\xc4\x8c\xc4\x8d\0\0\0\0\0\0\0\0\0\0\0\0\xc4\x8f\xc4\x8e\xc4\x90\xc4\x91"
      u8"\xc4\x92\xc4\x93\xc4\x96\xc4\x9a\xc4\x9b\0\0\xc4\xa1\0\0\0\0\0\0\0\0\0\0\xc4\xa2\xc4\xa3\xc4\xa4\0\0"

      // Start of 0x1e00:
      u8"\51\xc5\x85\75\xc5\x85\52\xc4\xb3\76\xc4\xb3\52\xc5\x83\76\xc5\x83\52\xc5\x8b\76\xc5\x8b\0\xc4\xad\0"
      u8"\xc4\xad\54\xc4\xb3\x40\xc4\xb3\54\xc5\x83\x40\xc5\x83\54\xc5\x8b\x40\xc5\x8b\54\xc5\x87\x40\xc5\x87"
      u8"\54\0\0\x40\0\0\x70\xc4\xac\x71\xc4\xac\x70\xc4\xad\x71\xc4\xad\55\0\0\x41\0\0\55\xc5\x8a\x41\xc5"
      u8"\x8a\xc3\xa4\xc4\xb2\xc3\xa5\xc4\xb2\56\xc4\xb3\0\x42\xc4\xb3\0\57\xc4\xb0\0\x43\xc4\xb0\0\60\xc4"
      u8"\xb3\0\x44\xc4\xb3\0\60\xc5\x83\0\x44\xc5\x83\0\60\xc4\xb4\0\x44\xc4\xb4\0\60\xc5\x87\0\x44\xc5\x87"
      u8"\0\60\0\0\0\x44\0\0\0\61\xc5\x8a\0\x45\xc5\x8a\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xad\0\0"
      u8"\xc5\x83\0\0\xc5\x83\0\0\xc5\x8b\0\0\xc5\x8b\0\0\xc5\x83\0\0\xc5\x83\0\0\xc4\xb0\0\0\xc4\xb0\0\0\xc5"
      u8"\x8b\0\0\xc5\x8b\0\0\0\0\0\0\0\0\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xb3\0\0\xc4\xb3\0\0\xc5\x83\0\0\xc5"
      u8"\x83\0\0\xc4\xb3\0\0\xc4\xb3\0\0\xc5\x83\0\0\xc5\x83\0\0\xc5\x8b\0\0\xc5\x8b\0\0\0\0\0\0\0\0\0\0\xc4"
      u8"\xad\0\0\xc4\xad\0\0\xc4\xb4\0\0\xc4\xb4\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\62\xc4"
      u8"\xad\0\x46\xc4\xad\0\62\xc4\xb3\0\x46\xc4\xb3\0\64\xc4\xb3\0\x48\xc4\xb3\0\64\xc5\x83\0\x48\xc5\x83"
      u8"\0\0\xc4\xb0\0\0\xc4\xb0\0\64\xc5\x8b\0\x48\xc5\x8b\0\65\xc4\xb3\0\x49\xc4\xb3\0\65\xc5\x83\0\x49"
      u8"\xc5\x83\0\0\xc4\xb3\0\0\xc4\xb3\0\xc2\xa0\xc4\xb3\xc2\xa1\xc4\xb3\0\xc4\xb3\0\0\xc4\xb3\0\66\xc4"
      u8"\xb3\0\x4a\xc4\xb3\0\66\xc5\x83\0\x4a\xc5\x83\0\66\xc5\x8b\0\x4a\xc5\x8b\0\66\0\0\0\x4a\0\0\0\67\xc5"
      u8"\x84\0\x4b\xc5\x84\0\67\xc5\x8a\0\x4b\xc5\x8a\0\67\0\0\0\x4b\0\0\0\xc2\xa8\xc4\xad\xc2\xa9\xc4\xad\0"
      u8"\xc4\xb4\0\0\xc4\xb4"

      // Start of 0x2c00:
      u8"\0\70\xc4\xaf\0\x4c\xc4\xaf\0\70\xc5\x83\0\x4c\xc5\x83\0\71\xc4\xac\0\x4d\xc4\xac\0\71\xc4\xad\0\x4d"
      u8"\xc4\xad\0\71\xc4\xb4\0\x4d\xc4\xb4\0\71\xc4\xb3\0\x4d\xc4\xb3\0\71\xc5\x83\0\x4d\xc5\x83\0\72\xc4"
      u8"\xb3\0\x4e\xc4\xb3\0\72\xc4\xb4\0\x4e\xc4\xb4\0\73\xc4\xb3\0\x4f\xc4\xb3\0\0\xc4\xae\0\0\xc4\xae\0\0"
      u8"\xc5\x83\0\0\xc5\x83\0\0\xc5\x8b\0\0\xc5\x8b\0\x44\xc5\x8b\0\x4a\xc4\xb4\0\x4d\0\0\0\x4f\0\0\0\75\0"
      u8"\0\0\0\xc4\xb3\0\51\xc5\x83\0\75\xc5\x83\0\51\xc4\xb5\0\75\xc4\xb5\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4"
      u8"\xac\0\0\xc4\xac\0\0\xc4\xb5\0\0\xc4\xb5\0\0\xc4\xaf\0\0\xc4\xaf\0\0\xc4\xae\0\0\xc4\xae\0\x66\xc4"
      u8"\xad\0\x67\xc4\xad\0\x66\xc4\xac\0\x67\xc4\xac\0\x66\xc4\xb5\0\x67\xc4\xb5\0\x66\xc4\xaf\0\x67\xc4"
      u8"\xaf\0\0\xc4\xb2\0\0\xc4\xb2\0\55\xc5\x83\0\x41\xc5\x83\0\55\xc4\xb5\0\x41\xc4\xb5\0\55\xc4\xaf\0"
      u8"\x41\xc4\xaf\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xb5\0\0\xc4\xb5\0\0\xc4\xaf\0\0"
      u8"\xc4\xaf\0\0\xc4\xae\0\0\xc4\xae\0\61\xc4\xb5\0\x45\xc4\xb5\0\61\xc5\x83\0\x45\xc5\x83\0\0\xc5\x83\0"
      u8"\0\xc5\x83\0\0\xc4\xb5\0\0\xc4\xb5\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xb5\0\0"
      u8"\xc4\xb5\0\0\xc4\xaf\0\0\xc4\xaf\0\0\xc4\xae\0\0\xc4\xae\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xac\0\0\xc4"
      u8"\xac\0\0\xc4\xb5\0\0\xc4\xb5\0\0\xc4\xaf\0\0\xc4\xaf\0\0\xc5\x83\0\0\xc5\x83\0\67\xc5\x83\0\x4b\xc5"
      u8"\x83\0\67\xc4\xb5\0\x4b\xc4\xb5\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xb5\0\0\xc4"
      u8"\xb5\0\0\xc4\xaf\0\0\xc4\xaf\0\0\xc5\x83\0\0\xc5\x83\0\73\xc4\xac\0\x4f\xc4\xac\0\73\xc5\x83\0\x4f"
      u8"\xc5\x83\0\73\xc4\xb5\0\x4f\xc4\xb5\0\73\xc4\xaf\0\x4f\xc4\xaf"

      // Start of 0x1f00:
      u8"\0\xc4\xb9\0\xc4\xba\0\xc4\xac\0\xc4\xac\0\xc4\xad\0\xc4\xad\0\xc5\x96\0\xc5\x96\xc6\x87\xc4\xb9\xc6"
      u8"\x87\xc4\xba\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\0\xc5\x96\0\0\xc5\x96\0\0\xc4\xb9\0\0"
      u8"\xc4\xba\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\xc6\x8b\xc4\xb9\xc6\x8b\xc4\xba\0\xc4\xac"
      u8"\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xb9\0\0\xc4\xba\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0"
      u8"\xc4\xad\0\0\xc5\x96\0\0\xc5\x96\0\xc6\x8d\xc4\xb9\xc6\x8d\xc4\xba\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad"
      u8"\0\0\xc4\xad\0\0\xc5\x96\0\0\xc5\x96\0\0\xc4\xb9\0\0\xc4\xba\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0"
      u8"\xc4\xad\0\0\xc5\x96\0\0\xc5\x96\0\xc6\x8f\xc4\xb9\xc6\x8f\xc4\xba\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad"
      u8"\0\0\xc4\xad\0\0\xc5\x96\0\0\xc5\x96\0\0\xc4\xb9\0\0\xc4\xba\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0"
      u8"\xc4\xad\0\0\xc4\xb9\0\0\xc4\xba\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\0\xc4\xb9\0\0\xc4"
      u8"\xba\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\0\xc5\x96\0\0\xc5\x96\0\0\xc4\xba\0\0\xc4\xac"
      u8"\0\0\xc4\xad\0\0\xc5\x96\0\0\xc4\xb9\0\0\xc4\xba\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\0"
      u8"\xc5\x96\0\0\xc5\x96\0\0\xc4\xb9\0\0\xc4\xba\0\0\xc4\xac\0\0\xc4\xac\0\0\xc4\xad\0\0\xc4\xad\0\0\xc5"
      u8"\x96\0\0\xc5\x96\0\0\xc4\xac\0\0\0\0\0\0\xc4\xac\0\0\0\0\0\0\xc4\xac\0\0\0\0\0\0\xc4\xac\0\0\0\0\0\0"
      u8"\xc4\xac\0\0\0\0\0\0\xc4\xac\0\0\0\0\0\0\xc4\xac\0\0\0\0\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5"
      u8"\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99"
      u8"\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0"
      u8"\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5"
      u8"\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99"
      u8"\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0"
      u8"\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc4\xb2\0\0\xc4\xb0\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5"
      u8"\x96\0\0\xc5\x99\0\xc6\x87\xc4\xb2\xc6\x87\xc4\xb0\xc6\x87\xc4\xac\xc6\x82\0\0\xc6\x87\xc5\x99\24"
      u8"\xc4\xb9\0\0\0\0\0\24\xc4\xb9\0\24\xc5\x96\0\0\xc5\x96\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5\x99\0\0\xc5"
      u8"\x96\0\0\xc5\x99\0\xc6\x8b\xc4\xac\xc6\x84\0\0\xc6\x8d\xc4\xac\xc6\x85\0\0\xc6\x8d\xc5\x99\0\xc4\xac"
      u8"\0\0\xc4\xad\0\0\xc5\x96\0\0\xc4\xb2\0\0\xc4\xb0\0\0\xc4\xac\0\xc6\x86\0\0\0\xc5\x96\0\0\xc5\x96\0"
      u8"\xc6\x8f\xc4\xb2\xc6\x8f\xc4\xb0\xc6\x8f\xc4\xac\0\0\0\0\0\xc4\xac\0\0\xc4\xad\0\0\xc5\x96\0\0\xc4"
      u8"\xb2\0\0\xc4\xb0\0\0\xc4\xac\0\0\0\0\0\0\xc4\xb9\0\0\xc4\xba\0\0\xc5\x96\0\0\xc5\x96\0\0\xc4\xb2\0\0"
      u8"\xc4\xb0\0\0\xc4\xac\0\0\0\0\0\0\xc4\xba\0\0\xc4\xac\0\xc6\x81\0\0\74\0\0\0\0\xc5\x99\0\0\xc5\x99\0"
      u8"\0\xc5\x99\0\0\xc5\x96\0\0\xc5\x99\0\0\xc4\xac\0\0\0\0\0\0\xc4\xac\0\0\0\0\0\0\xc5\x99\0\0\0\0\0\24"
      u8"\xc4\xba\0"

      // Start of 0x2000:
      u8"\xdf\x92\xdf\x93\24\0\24\0\24\0\24\0\24\0\24\0\24\0\24\0\24\0\xdf\x9a\24\xc5\x8d\0\0\0\0\0\0\0\0\0"
      u8"\24\0\0\xdf\xb0\xdf\xb0\xdf\xb0\xdf\xb0\xdf\xb0\xdf\xb3\xdf\xb3\0\0\xdf\xb3\xdf\xb3\xdf\xb3\25\25\0"
      u8"\0\0\0\24\xc4\xb1\0\0\0\0\0\0\0\0\0\0\25\0\0\0\0\25\0\0\0\0\0\xdf\xb0\xdf\xb0\xdf\xb0\xdf\xb0\24"

      // Start of 0x1d100:
      u8"\0\0\0\0\0\0\0\36\0\0\0\0\0\0\0\x45\0\0\0\0\0\0\0\42\0\0\0\0\0\0\0\43\0\0\0\0\0\0\0\44\0\0\0\0\0\0\0"
      u8"\45\0\0\0\0\0\0\0\46\0\0\0\0\0\0\0\47\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\xa4\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\34\0\0\0\0\0\0\0\35\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\36\0\0\0\0\0\0\0\37\0\0\0\0\0\0\0\40\0\0\0"
      u8"\0\0\0\0\41\0\0\0\0\0\0\0\42\0\0\0\0\0\0\0\43\0\0\0\0\0\0\0\44\0\0\0\0\0\0\0\45\0\0\0\0\0\0\0\46\0\0"
      u8"\0\0\0\0\0\47\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\xa4\0\0\0\0\0\0\0\0\0\0\0\0\0\34\0\0\0\0\0\0\0"
      u8"\35\0\0\0\0\0\0\0\75\0\0\0\0\0\0\0\x41\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x4e\0\0\0\0\0\0\0\xc4\x83\0\0\0"
      u8"\0\0\0\x44\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x46\0\0\0\0"
      u8"\0\0\0\x49\0\0\0\0\0\0\0\x4a\0\0\0\0\0\0\0\64\x49\0\0\0\0\0\0"

      // Start of 0x2100:
      u8"\75\0\77\75\0\x49\53\0\0\0\53\0\77\0\0\77\0\x4b\xc2\xbe\0\0\56\0\x43\0\0\60\0\0\60\0\0\60\0\0\x44\0"
      u8"\0\x7f\0\0\61\0\0\61\0\0\0\0\0\0\0\0\0\0\0\0\0\0\62\0\0\63\0\0\64\0\0\64\0\0\64\0\0\65\0\0\66\55\0"
      u8"\66\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\52\0\0\53\0\0\x41\0\0\55\0\0\56\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\x45\0\0\56\51\72\0\0\0\0\0\0\xc6\x89\0\0\0\0\xe0\xa2\xa3\54\0\0\x40\0\0\x41\0\0\x45\0\0\0"
      u8"\0\0\37\xdf\xbc\45\37\xdf\xbc\47\37\xdf\xbc\37\36\37\xdf\xbc\41\0\40\xdf\xbc\41\0\37\xdf\xbc\43\0\40"
      u8"\xdf\xbc\43\0\41\xdf\xbc\43\0\42\xdf\xbc\43\0\37\xdf\xbc\44\0\43\xdf\xbc\44\0\37\xdf\xbc\46\0\41\xdf"
      u8"\xbc\46\0\43\xdf\xbc\46\0\45\xdf\xbc\46\0\37\xdf\xbc\0\0\61\0\0\0\0\61\61\0\0\0\61\61\61\0\0\61\70\0"
      u8"\0\0\70\0\0\0\0\70\61\0\0\0\70\61\61\0\0\70\61\61\61\0\61\72\0\0\0\72\0\0\0\0\72\61\0\0\0\72\61\61\0"
      u8"\0\0\0\0\0\0\53\0\0\0\0\54\0\0\0\0\0\0\0\0\0\x45\0\0\0\0\x45\x45\0\0\0\x45\x45\x45\0\0\x45\x4c\0\0\0"
      u8"\x4c\0\0\0\0\x4c\x45\0\0\0\x4c\x45\x45\0\0\x4c\x45\x45\x45\0\x45\x4e\0\0\0\x4e\0\0\0\0\x4e\x45\0\0\0"
      u8"\x4e\x45\x45\0\0\0\0\0\0\0\77\0\0\0\0\x40\0\0\0\0\0\0\0\0\0\36\xdf\xbc\41\0\xe0\xa2\x8e\xc5\x92\xe0"
      u8"\xa2\x90\xc5\x92\xe0\xa2\x92\xc5\x92\0\xc5\x92\0\0\0\xc5\x92\0\0\0\xc5\x92\0\0"

      // Start of 0x2200:
      u8"\xe0\xa2\x9b\xc5\x92\xe0\xa2\xa0\xc5\x92\0\xc5\x92\0\0\xe0\xa2\xaf\xc5\x92\xe0\xa2\xb1\xc5\x92\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc5\x92\0\0\xe0\xa3\x83\xc5\x92\xe0\xa3\x85\xc5\x92\xe0\xa3\x88"
      u8"\xc5\x92\0\xc5\x92\0\0\xe0\xa3\x95\xc5\x92\0\xc5\x92\0\0\0\xc5\x92\0\0\0\xc5\x92\0\0\xe0\xa3\x98\xc5"
      u8"\x92\xe0\xa3\x99\xc5\x92\xe0\xa3\xa0\xc5\x92\xe0\xa3\xa1\xc5\x92\xe0\xa3\xa4\xc5\x92\xe0\xa3\xa5\xc5"
      u8"\x92\0\xc5\x92\0\0\0\xc5\x92\0\0\xe0\xa3\xaa\xc5\x92\xe0\xa3\xab\xc5\x92\xe0\xa3\xae\xc5\x92\xe0\xa3"
      u8"\xaf\xc5\x92\0\xc5\x92\0\0\0\xc5\x92\0\0\0\xc5\x92\0\0\0\xc5\x92\0\0\0\xc5\x92\0\0\0\xc5\x92\0\0\xe0"
      u8"\xa3\xb3\xc5\x92\xe0\xa3\xb4\xc5\x92\0\xc5\x92\0\0\0\xc5\x92\0\0\0\xc5\x92\0\0\0\xc5\x92\0\0"

      // Start of 0x2300:
      u8"\xe0\xaf\x80\xe0\xaf\x81"

      // Start of 0x2400:
      u8"\37\40\41\42\43\44\45\46\47\37\36\37\37\37\40\37\41\37\42\37\43\37\44\37\45\37\46\37\47\40\36\34\37"
      u8"\35\34\40\35\34\41\35\34\42\35\34\43\35\34\44\35\34\45\35\34\46\35\34\47\35\34\37\36\35\34\37\37\35"
      u8"\34\37\40\35\34\37\41\35\34\37\42\35\34\37\43\35\34\37\44\35\34\37\45\35\34\37\46\35\34\37\47\35\34"
      u8"\40\36\35\37\0\0\0\40\0\0\0\41\0\0\0\42\0\0\0\43\0\0\0\44\0\0\0\45\0\0\0\46\0\0\0\47\0\0\0\37\36\0\0"
      u8"\37\37\0\0\37\40\0\0\37\41\0\0\37\42\0\0\37\43\0\0\37\44\0\0\37\45\0\0\37\46\0\0\37\47\0\0\40\36\0\0"
      u8"\34\75\35\0\34\76\35\0\34\77\35\0\34\x40\35\0\34\x41\35\0\34\x42\35\0\34\x43\35\0\34\x44\35\0\34\x45"
      u8"\35\0\34\0\35\0\34\0\35\0\34\0\35\0\34\0\35\0\34\0\35\0\34\0\35\0\34\x46\35\0\34\x47\35\0\34\x48\35"
      u8"\0\34\x49\35\0\34\x4a\35\0\34\x4b\35\0\34\x4c\35\0\34\x4d\35\0\34\x4e\35\0\34\x4f\35\0\34\0\35\0\51"
      u8"\0\0\0\52\0\0\0\53\0\0\0\54\0\0\0\55\0\0\0\56\0\0\0\57\0\0\0\60\0\0\0\61\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\62\0\0\0\63\0\0\0\64\0\0\0\65\0\0\0\66\0\0\0\67\0\0\0\70\0\0\0\71\0\0\0"
      u8"\72\0\0\0\73\0\0\0\0\0\0\0\75\0\0\0\76\0\0\0\77\0\0\0\x40\0\0\0\x41\0\0\0\x42\0\0\0\x43\0\0\0\x44\0"
      u8"\0\0\x45\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x46\0\0\0\x47\0\0\0\x48\0\0\0\x49\0\0"
      u8"\0\x4a\0\0\0\x4b\0\0\0\x4c\0\0\0\x4d\0\0\0\x4e\0\0\0\x4f\0\0\0\0\0\0\0\36"

      // Start of 0x2a00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc5\x92\0"

      // Start of 0x2f00:
      u8"\4\0\0\0\0\0\0\0\0\0\xe1\x90\xad\0\0\0\xe1\x90\xbe\xe1\x91\x8c\0\0\0\63\0\0\0\0\0\xe1\x91\x90\0\0\0"
      u8"\0\0\0\xe1\x93\x83\0\0\0\xe1\x93\x9a\xe1\x93\x9d\0\0\0\xe1\x93\xb9\xe1\x94\x86\0\0\0\0\0\0\xec\xbc"
      u8"\x88\0\0\0\0\0\0\0\0\0\xe1\x9c\x8e\0\0\0\xe1\x9c\x9b\xe1\x9c\xa7\xe1\x9d\x95\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\xe1\xa1\x80\xe1\xa1\x9c\0\0\0\0\0\0\xe1\xa6\x86\xe1\xa6\xbb\xe1\xa7\x85\0\0\0\0\0\0"
      u8"\x41\0\0\xe2\xba\xa0\0\0\0\xe1\xa8\xb4\xe1\xa9\x88\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\xe1\xb1\x84\0\0\0\0\0\0\xe1\xb1\x8f\xe1\xb1\x9b\0\0\0\0\0\0\xe1\xb3\x98\xe1"
      u8"\xb3\x9d\0\0\0\xe2\x9a\x80\xe1\xb5\x9e\0\0\0\xe1\xb5\xa8\xe1\xb5\xaa\0\0\0\xe1\xb6\xa8\xe1\xb7\xbc\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb8\x94\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\xe1\xbd\x81\0\0\0\xe1\xbd\x8c\xe1\xbd\xa1\0\0\0\xe1\xbe\x99\xe3\xb1\xa8\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x81\x90\xe2\x81\x96\0\0\0\0\0\0\xe2\x8a\x88\0\0\0\xe2\x8a\x9f\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe2\x8e\x83\xe2\x8e\xbd\0\0\0\0\0\0\0\0\0\xe2\x95\xa9\xe2\x95\xac\0\0\0\0\0\0\0\0\0\xe6\x80\x80\xe2"
      u8"\x98\x97\0\0\0\xe2\x98\xa2\xe2\x98\xa9\0\0\0\0\0\0\0\0\0\xe2\x99\x89\0\0\0\0\0\0\0\0\0\xe2\x9c\x8c"
      u8"\xe2\x9c\x8f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x3000:
      u8"\24\xe0\xaf\x84\xe1\x93\x9d\xe1\x93\xa0\xe1\x93\xa1\0\xe0\xb0\x9b\0\xe0\xb0\x9b\0\xe0\xb0\x9b\xe0"
      u8"\xaf\xab\xe0\xb0\x9b\xe0\xaf\xad\xe0\xb0\x9b\xe0\xaf\xaf\xe0\xb0\x9b\xe0\xaf\xb1\xe0\xb0\x9b\xe0\xaf"
      u8"\xb3\xe0\xb0\x9b\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\xe0\xaf\xb5\xe0\xb0\x9b\xe0"
      u8"\xaf\xb8\xe0\xb0\x9b\xe0\xaf\xba\xe0\xb0\x9b\xe0\xaf\xbc\xe0\xb0\x9b\0\xe0\xb0\x9b\0\0\0\0\0\0\0\0"
      u8"\xe0\xb0\x80\xe0\xb0\x9b\xe0\xb0\x80\0\0\0\xe0\xb0\x83\xe0\xb0\x9b\xe0\xb0\x83\0\0\0\xe0\xb0\x86\xe0"
      u8"\xb0\x9b\xe0\xb0\x86\0\0\0\0\xe0\xb0\x9b\0\0\0\0\0\0\0\0\xe0\xaf\xa6\xe0\xb0\x9b\24\xe0\xb0\x9b\0\0"
      u8"\24\0\0\0\0\0\0\xe0\xb0\x9b\0\0\xe0\xb0\x90\0\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b"
      u8"\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0"
      u8"\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0"
      u8"\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\0\0\0\0\0\0\xe0\xb0\x9b\0\0\0\0\0\0\0\0\0\xe0\xb0\x9b\0\0\0"
      u8"\0\0\0\0\0\0\xe0\xb0\x9b\0\0\0\0\0\0\0\0\0\xe0\xb0\x9b\0\0\0\0\0\0\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0"
      u8"\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\xe0\xb0\x9b\0\0\0\0\0\0\0\0"

      // Start of 0x3100:
      u8"\xd1\x8c\xd1\x8d\0\0\xd1\x8e\0\0\0\0\xd1\x8f\xd1\x90\xd1\x91\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd1\x92\xd1"
      u8"\x93\xd1\x94\xd1\xa1\xd1\x95\0\0\0\0\0\0\0\0\0\0\0\0\xd1\x96\xd1\x97\xd1\x98\xd2\x89\xd2\x8a\xd2\x8b"
      u8"\xd2\x8c\xd2\x8d\xd2\x8e\xd2\x8f\xd2\x90\xd2\x91\0\0\0\0\0\0\0\0\0\0\0\0\xd2\x92\xd2\x93\xd2\x94\xd2"
      u8"\x95\xd2\x96\xd2\x97\xd2\x88\xd1\x9a\xd1\x9b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd1\xa0"
      u8"\xd1\xa2\xd1\xa3\xd1\xa7\xd1\xa9\0\0\0\0\0\0\0\0\0\0\xd1\xac\xd1\xb0\xd1\xb4\xd1\xbb\0\0\0\0\0\0\xd2"
      u8"\x85\xd2\x86\xd2\x87\xd2\xa0\xd2\xa1\xd2\xa4\xd2\xa7\xd2\xa8\xd2\xaa\0\0\0\0\4\0\0\0\xe2\xa0\x80\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xe1\xb5\xac\0\0\0\0\0\0\50\0\0\xe1\x9c\xa9\xe1\x99\xa2\0\0\0"

      // Start of 0x3200:
      u8"\34\xd1\x8c\35\34\xd1\x8e\35\34\xd1\x8f\35\34\xd1\x91\35\34\xd1\x92\35\34\xd1\x93\35\34\xd1\x95\35"
      u8"\34\0\35\0\34\0\35\0\34\0\35\0\34\0\35\0\34\xd1\x96\35\34\xd1\x97\35\34\xd1\x98\35\34\xd1\x8c\xd2"
      u8"\x89\35\34\xd1\x8e\xd2\x89\35\34\xd1\x8f\xd2\x89\35\34\xd1\x91\xd2\x89\35\34\xd1\x92\xd2\x89\35\34"
      u8"\xd1\x93\xd2\x89\35\34\xd1\x95\xd2\x89\35\34\0\xd2\x89\35\0\34\0\xd2\x89\35\0\34\0\xd2\x89\35\0\34\0"
      u8"\xd2\x89\35\0\34\xd1\x96\xd2\x89\35\34\xd1\x97\xd2\x89\35\34\xd1\x98\xd2\x89\35\34\0\0\35\0\0\34\0"
      u8"\xd2\x91\0\xd2\x8d\0\35\34\0\xd2\x91\xd1\x98\0\35\0\34\4\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\xe2"
      u8"\xa0\x80\35\0\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\xe0\xbe\xa0\35\0"
      u8"\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\xe1\x93\x9d\35\0\0\0\0\34\xe1\xa8\xb4\35\0\0\0\0"
      u8"\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\xe1\xa9\x88\35\0\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0"
      u8"\0\0\0\34\xe2\xba\xa0\35\0\0\0\0\34\0\35\0\0\0\0\0\0\34\xe1\xa8\xb5\35\0\0\0\0\34\0\35\0\0\0\0\0\0"
      u8"\34\0\35\0\0\0\0\0\0\34\xe1\xb1\xaf\35\0\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0"
      u8"\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\xe2\xa3\xa0\35\0\0\0\0\34\0\35"
      u8"\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\34\xe1\x93\xaa\35\0\0\0\0\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0"
      u8"\34\0\35\0\0\0\0\0\0\34\0\35\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xa6\xbb\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\62\66\55\0\0\0\0\0\0\40\37\0\0\0\0\0\0\0\40\40\0\0\0\0\0\0\0\40\41\0\0\0\0\0\0\0"
      u8"\40\42\0\0\0\0\0\0\0\40\43\0\0\0\0\0\0\0\40\44\0\0\0\0\0\0\0\40\45\0\0\0\0\0\0\0\40\46\0\0\0\0\0\0\0"
      u8"\40\47\0\0\0\0\0\0\0\41\36\0\0\0\0\0\0\0\41\37\0\0\0\0\0\0\0\41\40\0\0\0\0\0\0\0\41\41\0\0\0\0\0\0\0"
      u8"\41\42\0\0\0\0\0\0\0\41\43\0\0\0\0\0\0\0\xd1\x8c\0\0\0\0\0\0\0\xd1\x8e\0\0\0\0\0\0\0\xd1\x8f\0\0\0\0"
      u8"\0\0\0\xd1\x91\0\0\0\0\0\0\0\xd1\x92\0\0\0\0\0\0\0\xd1\x93\0\0\0\0\0\0\0\xd1\x95\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd1\x96\0\0\0\0\0\0\0\xd1\x97\0\0"
      u8"\0\0\0\0\0\xd1\x98\0\0\0\0\0\0\0\xd1\x8c\xd2\x89\0\0\0\0\0\xd1\x8e\xd2\x89\0\0\0\0\0\xd1\x8f\xd2\x89"
      u8"\0\0\0\0\0\xd1\x91\xd2\x89\0\0\0\0\0\xd1\x92\xd2\x89\0\0\0\0\0\xd1\x93\xd2\x89\0\0\0\0\0\xd1\x95\xd2"
      u8"\x89\0\0\0\0\0\0\xd2\x89\0\0\0\0\0\0\0\xd2\x89\0\0\0\0\0\0\0\xd2\x89\0\0\0\0\0\0\0\xd2\x89\0\0\0\0\0"
      u8"\0\xd1\x96\xd2\x89\0\0\0\0\0\xd1\x97\xd2\x89\0\0\0\0\0\xd1\x98\xd2\x89\0\0\0\0\0\0\xd2\x89\0\xd1\x8c"
      u8"\xd2\x91\0\0\0\0\xd2\x96\0\0\0\0\0\0\0\0\0\0\0\0\0\4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\xa0\x80\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbe\xa0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x93\x9d\0\0\0\0\0\0\xe1\xa8\xb4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xe1\xa9\x88\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\xba\xa0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\xe1\xa8\xb5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb1\xaf\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb5\xb1\0\0\0\0\0\0\xe1\x9d"
      u8"\x95\0\0\0\0\0\0\xe2\x8d\xad\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x93\xba\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe2\x99\x8d\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x91\x8f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\xa3\xa0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x93\xaa"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\41\44\0\0\0\0\0\0\0\41\45\0\0\0\0\0\0\0\41\46\0\0\0\0\0\0\0\41\47\0\0"
      u8"\0\0\0\0\0\42\36\0\0\0\0\0\0\0\42\37\0\0\0\0\0\0\0\42\40\0\0\0\0\0\0\0\42\41\0\0\0\0\0\0\0\42\42\0\0"
      u8"\0\0\0\0\0\42\43\0\0\0\0\0\0\0\42\44\0\0\0\0\0\0\0\42\45\0\0\0\0\0\0\0\42\46\0\0\0\0\0\0\0\42\47\0\0"
      u8"\0\0\0\0\0\43\36\0\0\0\0\0\0\0\37\xe1\xa8\xb4\0\0\0\0\0\40\xe1\xa8\xb4\0\0\0\0\0\41\xe1\xa8\xb4\0\0"
      u8"\0\0\0\42\xe1\xa8\xb4\0\0\0\0\0\43\xe1\xa8\xb4\0\0\0\0\0\44\xe1\xa8\xb4\0\0\0\0\0\45\xe1\xa8\xb4\0\0"
      u8"\0\0\0\46\xe1\xa8\xb4\0\0\0\0\0\47\xe1\xa8\xb4\0\0\0\0\0\37\36\xe1\xa8\xb4\0\0\0\0\37\37\xe1\xa8\xb4"
      u8"\0\0\0\0\37\40\xe1\xa8\xb4\0\0\0\0\60\x43\0\0\0\0\0\0\0\x41\x48\x43\0\0\0\0\0\0\x41\70\0\0\0\0\0\0\0"
      u8"\0\66\54\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\36"
      u8"\0\0\0\0\0\0\0\0\xc4\xac\0\0\0\0\0\0\0\xe0\xae\xb8\0\0\0\0\0\0\xe9\x8f\xa0\0\0\0\0\0\0\xec\x8e\x80\0"
      u8"\0\0\0\0\0\xe5\xb8\x80\0\0\0\0\0\0\xea\xb0\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x3300:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\36\0\0\0"
      u8"\0\0\0\0\0\xea\xb0\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xec\x9b\x80"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xea\xb0\x80\36\0\0\xc4\xac\0\0\0\0\0\0\0\0\0\0\0\xea\xb0\x80\36\0"
      u8"\0\0\xea\xb0\x80\36\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\36\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xea\xb0\x80\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xea\xb0\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xea\x8c\x80\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc4\xac\0\0\0\0\0\xc4\xac\0\0\0\0\0\xc4\xac\0\0\0\0\0\xe9\x8f\xa0\0\0"
      u8"\0\0\xe9\x8f\xa0\0\0\0\0\xec\x8e\x80\0\0\0\0\0\0\0\0\0\0\0\0\xea\xb0\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\36\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\36\0\0\0\0\0\0\37\0\0\0\0\0\0\40\0\0\0\0\0\0\41\0"
      u8"\0\0\0\0\0\42\0\0\0\0\0\0\43\0\0\0\0\0\0\44\0\0\0\0\0\0\45\0\0\0\0\0\0\46\0\0\0\0\0\0\47\0\0\0\0\0\0"
      u8"\37\36\0\0\0\0\0\37\37\0\0\0\0\0\37\40\0\0\0\0\0\37\41\0\0\0\0\0\37\42\0\0\0\0\0\37\43\0\0\0\0\0\37"
      u8"\44\0\0\0\0\0\37\45\0\0\0\0\0\37\46\0\0\0\0\0\37\47\0\0\0\0\0\40\36\0\0\0\0\0\40\37\0\0\0\0\0\40\40"
      u8"\0\0\0\0\0\40\41\0\0\0\0\0\40\42\0\0\0\0\0\x44\62\75\0\0\0\0\x40\75\0\0\0\0\0\51\67\0\0\0\0\0\76\75"
      u8"\x48\0\0\0\0\0\70\0\0\0\0\0\x46\77\0\0\0\0\0\x40\0\0\0\0\0\0\x40\0\0\0\0\0\0\x40\0\0\0\0\0\0\61\67\0"
      u8"\0\0\0\0\0\xe1\xa1\x82\0\0\0\0\0\0\0\0\0\0\xe1\x9c\xa7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x46\51\0"
      u8"\0\0\0\0\0\51\0\0\0\0\0\0\51\0\0\0\0\0\0\51\0\0\0\0\0\0\51\0\0\0\0\0\0\52\0\0\0\0\0\0\52\0\0\0\0\0"
      u8"\57\52\0\0\0\0\0\77\75\0\0\0\0\0\0\77\75\0\0\0\0\x46\56\0\0\0\0\0\0\56\0\0\0\0\0\0\56\0\0\0\0\0\0"
      u8"\x43\0\0\0\0\0\0\x43\0\0\0\0\0\0\x43\0\0\0\0\0\60\0\0\0\0\0\0\0\60\0\0\0\0\0\0\60\0\0\0\0\0\57\60\0"
      u8"\0\0\0\0\66\60\0\0\0\0\0\0\xe0\xa1\x81\0\0\0\0\xe0\xa1\x81\0\0\0\x40\xe0\xa1\x81\0\0\0\0\xe0\xa1\x81"
      u8"\0\0\0\x42\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\77\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\77\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\77\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\xe0\xa2\xa7\x49\0\0\0\xe0\xa2\xa7\x49\0\0\62\75\0\0\0\0\0\0\62\75\0\0\0\0\0\62\75\0\0\0\0"
      u8"\57\62\75\0\0\0\0\x48\75\x40\0\0\0\0\x48\75\x40\xe0\xa2\xa7\x49\x48\75\x40\xe0\xa2\xa7\x49\0\x46\x49"
      u8"\0\0\0\0\0\0\0\x49\0\0\0\0\0\0\0\x49\0\0\0\0\0\0\0\x49\0\0\0\0\0\0\x46\70\0\0\0\0\0\0\0\70\0\0\0\0\0"
      u8"\0\0\70\0\0\0\0\0\0\0\70\0\0\0\0\0\0\0\70\0\0\0\0\0\0\0\70\0\0\0\0\0\0\x46\71\0\0\0\0\0\0\0\71\0\0\0"
      u8"\0\0\0\0\71\0\0\0\0\0\0\0\71\0\0\0\0\0\0\0\71\0\0\0\0\0\0\0\71\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\75\0\0\0\0\0\0\0\52\x47\0\0\0\0\0\0\77\77\0\0\0\0\0\0\77\x40\0\0\0\0\0\0\53\xe0\xa2\xa7\0\x43"
      u8"\0\0\53\0\0\0\0\0\0\0\x40\52\0\0\0\0\0\0\57\x4f\0\0\0\0\0\0\x44\75\0\0\0\0\0\0\60\62\0\0\0\0\0\0\x45"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x4a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\x43\0\0\0\0\0\0\x4e\0\0\0\0\0\0\0\76\0\0\0\0\0\0\0\x45\0\0\0\0\0\0\0\0\0\0\0\0\0\0\62\60\0\0\0\0\0"
      u8"\0\x46\0\0\0\0\0\0\0\62\62\0\0\0\0\0\0\62\64\0\0\0\0\0\0\x49\x48\0\0\0\0\0\0\65\x4c\0\0\0\0\0\0\71"
      u8"\76\0\0\0\0\0\0\70\xe0\xa2\xa7\0\0\0\0\51\xe0\xa2\xa7\0\0\0\0\37\xe2\xba\xa0\0\0\0\0\40\xe2\xba\xa0"
      u8"\0\0\0\0\41\xe2\xba\xa0\0\0\0\0\42\xe2\xba\xa0\0\0\0\0\43\xe2\xba\xa0\0\0\0\0\44\xe2\xba\xa0\0\0\0\0"
      u8"\45\xe2\xba\xa0\0\0\0\0\46\xe2\xba\xa0\0\0\0\0\47\xe2\xba\xa0\0\0\0\0\37\36\xe2\xba\xa0\0\0\0\37\37"
      u8"\xe2\xba\xa0\0\0\0\37\40\xe2\xba\xa0\0\0\0\37\41\xe2\xba\xa0\0\0\0\37\42\xe2\xba\xa0\0\0\0\37\43\xe2"
      u8"\xba\xa0\0\0\0\37\44\xe2\xba\xa0\0\0\0\37\45\xe2\xba\xa0\0\0\0\37\46\xe2\xba\xa0\0\0\0\37\47\xe2\xba"
      u8"\xa0\0\0\0\40\36\xe2\xba\xa0\0\0\0\40\37\xe2\xba\xa0\0\0\0\40\40\xe2\xba\xa0\0\0\0\40\41\xe2\xba\xa0"
      u8"\0\0\0\40\42\xe2\xba\xa0\0\0\0\40\43\xe2\xba\xa0\0\0\0\40\44\xe2\xba\xa0\0\0\0\40\45\xe2\xba\xa0\0\0"
      u8"\0\40\46\xe2\xba\xa0\0\0\0\40\47\xe2\xba\xa0\0\0\0\41\36\xe2\xba\xa0\0\0\0\41\37\xe2\xba\xa0\0\0\0"
      u8"\x43\75\0\0\0\0\0"

      // Start of 0xa700:
      u8"\0\53\56\63\x7e\xc2\x99"

      // Start of 0xf900:
      u8"\0\0\0\0\0\0\xef\xbf\xbd\0\0\0\0\0\0\xe1\x9c\xbf\0\0\0\xe1\x97\x93\xe1\x9c\xbc\0\0\0\xe1\xb7\xb5\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x93\xbf\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x8c\x84\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xa8\xbd\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb0\xaa\xea\xb8\x80\xe1\xbd\x81\xe2\x86\x9e\0\0\0\0\0\0\xe2\x98\x84\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x91\x98\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb1\x9e\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe2\x97\x8b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x9d\xbc\xe1\xb3\x9b\xe1\xb6\x92\xe1\x93\x85\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xa6\xb2\0\0\0\0\0\0\0\0\0\xe1\xb8\x95\xe2\x84\x81\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\xe1\xbd\x80\0\0\0\xe1\xb6\x8d\0\0\0\xe1\x90\xb1\0\0\0\xe1\xab\xa1\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\xe1\x95\x82\xe1\x9d\x95\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xe1\xa0\xae\xe1\xa0\xb8\0\0\0\0\0\0\xe1\xaf\xad\xe1\xb5\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe2\x8c\xbf\0\0\0\xe1\x91\xa1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb3"
      u8"\x8f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb4\xad\0\0\0\xe1\xbd\xae\xe2\x90\x92\0\0\0\xe2\x98\x94"
      u8"\xe2\x99\x9a\0\0\0\0\0\0\0\0\0\0\0\0\xc9\x98\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xa7\x87\0\0\0\0\0\0\xe1"
      u8"\xb7\x9a\0\0\0\0\0\0\0\0\0\xe1\xa8\xa0\0\0\0\xe1\x92\xa9\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb3\xb1\xe1\xb6"
      u8"\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x97\x8e\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xa0\x80\xe1\xaa\xa1"
      u8"\xe1\xb3\x9b\xe2\x97\x96\xe1\x91\xad\0\0\0\0\0\0\xe1\xa7\x95\0\0\0\0\0\0\0\0\0\xe1\xb3\xae\xe1\xb5"
      u8"\x8c\0\0\0\0\0\0\xcd\xb0\0\0\0\0\xe2\x96\x80\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb5\x8a\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\xe1\xaa\x8d\0\0\0\xe3\x84\x80\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xae\xbb\0\0\0\0\0\0\xe2\x82\x90\0\0\0"

      // Start of 0xfa00:
      u8"\xe1\x91\x97\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x90\x94\0\0\0\0\0"
      u8"\0\xe1\xa8\x92\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xbc\x9d\0\0\0\xe2\x98\x9c\0\0\0\0\0\0\xe2\x86\xa4\0"
      u8"\0\0\xe2\x8d\x8e\0\0\0\0\0\0\0\0\0\xe2\x9b\x88\0\0\0\0\0\0\0\0\0\0\0\0\xe6\x94\x80\0\0\0\0\0\0\xee"
      u8"\xbd\x80\xe1\x93\xa7\0\0\0\xe1\x97\xa6\xe1\x98\xa4\xe1\x9b\x90\0\0\0\0\0\0\0\0\0\xe1\x9f\x8e\xe1\xa0"
      u8"\x98\0\0\0\0\0\0\0\0\0\xe1\xa5\xa4\xe1\xa8\xa3\xe1\xab\xa5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb3\xbe"
      u8"\xe1\xbb\x93\0\0\0\xe1\xbc\x8d\xe1\xbc\x8c\xe1\xbc\x8e\xe1\xbc\x94\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xe8\x98\x80\0\0\0\0\0\0\xe1\xbd\x85\0\0\0\xe2\x81\x97\xe2\x81\x97\xe2\x84\x89\xe2\x8b\x8e"
      u8"\xe2\x8c\xa4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x8d\x8e\xe7\x9c\x80\0\0\0\0\0\0\xe1\x9e\xbb\0\0\0\xe2"
      u8"\x80\x9a\0\0\0\0\0\0\xe1\x90\xb0\0\0\0\xe1\x90\x99\xe1\x90\xbc\0\0\0\0\0\0\0\0\0\xe1\x96\xb3\xe1\x97"
      u8"\x9f\xe1\x95\xbc\0\0\0\0\0\0\xe1\x9c\xb8\xe1\x9d\x82\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe1\x9f\x9c\0\0\0\xe1\xa0\x90\0\0\0\xe1\xa1\x9a\0\0\0\0\0\0\xe1\xa4\xb4\xe1\xa6\x9c\xe1\xa8\x92\xe1"
      u8"\xa8\xbd\0\0\0\xe1\xa9\xa4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb1\x83\0\0\0\0"
      u8"\0\0\xe1\xb4\xaf\xe1\xb5\x92\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb8\xbc\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xbd\x85\xe2\x82\xa0\0\0\0\xe2\x89\x8b\xe2\x8b\xad\xe2\x8c\x9a\xe2\x8c"
      u8"\xa4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x8d\xb0\xe2\x8f\xaf\xe2\x90\xbc"
      u8"\0\0\0\xe7\x9c\x80\xe2\x98\x9c\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe5\xa4\xbc\0\0\0\0\0\0\xe0"
      u8"\xbe\xb2\xe0\xbf\x87\xe6\x8a\xa1\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xfb00:
      u8"\x42\x42\x42\x45\x42\0\x42\x42\x45\x42\x42\0\0\x4a\0\x49\x4a\0\xc8\xbe\xc9\x80\xc8\xbe\xc8\xb5\xc8"
      u8"\xbe\0\0\0\xc9\x80\0\xc8\xbe\0\0\0\0\0\0\0\0\0\0\xc7\xb4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xe6\x94\x80\0\0\0\0\0\0\0\0\0\xef\x88\x80\0\xef\x88\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\5\0\0\0\62\0\0\0\xe1\x8e\x88\0\xec\x8d\x90\0\xe4\xad\x80\0\xef\x82\x80\0\xe6\x94\x80\0\xef\x88"
      u8"\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xec\x8d\x90\0\0\0\0\0\xca\x9f\0\0\xca\x9f\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\xa8\0\0\xca\xa8\0\0\xca\xa8\0\0\xca\xa8\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\xa7\0\0\xca\xa7\0\0\xca\xa7\0\0\xca"
      u8"\xa7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\xac\0\0\xca\xac\0\0\xca"
      u8"\xac\0\0\xca\xac\0\0\xca\xab\0\0\xca\xab\0\0\xca\xab\0\0\xca\xab\0\0\xca\xae\0\0\xca\xae\0\0\xca\xae"
      u8"\0\0\xca\xae\0\0\xca\xaf\0\0\xca\xaf\0\0\xca\xaf\0\0\xca\xaf\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xca\xb0\0\0\xca\xb0\0\0\xca\xba\0\0\xca\xba\0\0\xca\xb3\0\0\xca\xb3\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\xa5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\x89\0\0\xca\x89\0\0\xc9\xb2\xc9\xb3\xc9\xb2\xc9\xb3"
      u8"\xc9\xb2\0\0\xc9\xb2\0\0\xc9\xb2\xca\x88\xc9\xb2\xca\x88\xc9\xb2\0\0\xc9\xb2\0\0\xc9\xb2\0\0\xc9\xb2"
      u8"\0\0\xc9\xb2\0\0\xc9\xb2\0\0\xc9\xb2\0\0\xc9\xb2\0\0\xc9\xb2\0\0\xc9\xb2\xca\x89\xc9\xb2\xca\x89\xc9"
      u8"\xb2\xca\x89\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xfc00:
      u8"\xc9\xb2\0\xc9\xb2\0\xc9\xb2\xca\x85\xc9\xb2\xca\x89\xc9\xb2\0\0\xc9\xb4\0\0\xc9\xb4\0\0\xc9\xb4\0\0"
      u8"\xc9\xb4\xca\x85\xc9\xb4\xca\x89\xc9\xb4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\x85\0\0\xca\x89\0\0\0\0\0"
      u8"\0\0\0\0\0\xca\x85\0\0\xca\x89\0\0\0\0\0\0\0\0\0\0\xca\x85\0\0\0\0\0\0\xca\x85\0\0\0\0\0\0\0\0\0\0"
      u8"\xca\x85\0\xc9\xb9\0\0\xc9\xb9\0\0\xc9\xb9\0\0\xc9\xb9\xca\x85\xc9\xbb\0\0\xc9\xbb\xca\x85\xc9\xbc\0"
      u8"\0\xc9\xbc\0\0\xc9\xbc\0\0\xc9\xbc\xca\x85\xc9\xbd\0\0\xc9\xbd\xca\x85\xc9\xbe\xca\x85\xc9\xbf\0\0"
      u8"\xc9\xbf\xca\x85\0\0\0\0\0\xca\x85\0\xca\x81\0\0\xca\x81\0\0\xca\x81\0\0\xca\x81\xca\x85\xca\x81\xca"
      u8"\x89\xca\x81\0\0\xca\x82\0\0\xca\x82\xca\x85\xca\x82\xca\x89\xca\x82\0\0\xca\x83\xc9\xb3\xca\x83\0\0"
      u8"\xca\x83\0\0\xca\x83\0\0\xca\x83\xca\x84\xca\x83\xca\x85\xca\x83\xca\x89\xca\x83\0\0\xca\x84\0\0\xca"
      u8"\x84\0\0\xca\x84\0\0\xca\x84\xca\x85\xca\x84\xca\x89\xca\x84\0\0\xca\x85\0\0\xca\x85\0\0\xca\x85\0\0"
      u8"\xca\x85\xca\x85\xca\x85\xca\x89\xca\x85\0\0\xca\x86\0\0\xca\x86\0\0\xca\x86\0\0\xca\x86\xca\x85\xca"
      u8"\x86\xca\x89\xca\x86\0\0\xca\x87\0\0\xca\x87\xca\x85\xca\x87\xca\x89\xca\x87\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\xca\x85\0\0\xca\x89\0\0\0\0\0\xc9\xb6\xca\x9e\xc9\xb7\xca\x9e\xca\x89\xca\x9e\24\0\xca\x8b\24"
      u8"\0\xca\x8b\24\0\xca\x8b\24\0\xca\x8b\24\xca\x8a\xca\x8b\24\xca\x8b\xca\x9e\xc9\xb2\xc9\xb7\0\xc9\xb2"
      u8"\xc9\xb8\0\xc9\xb2\xca\x85\0\xc9\xb2\xca\x86\0\xc9\xb2\xca\x89\0\xc9\xb2\0\0\0\xc9\xb4\xc9\xb7\0\xc9"
      u8"\xb4\xc9\xb8\0\xc9\xb4\xca\x85\0\xc9\xb4\xca\x86\0\xc9\xb4\xca\x89\0\xc9\xb4\0\0\0\0\xc9\xb7\0\0\0"
      u8"\xc9\xb8\0\0\0\xca\x85\0\0\0\xca\x86\0\0\0\xca\x89\0\0\0\0\0\0\0\0\xc9\xb7\0\0\0\xc9\xb8\0\0\0\xca"
      u8"\x85\0\0\0\xca\x86\0\0\0\xca\x89\0\0\0\0\0\0\0\xca\x81\xca\x89\0\xca\x81\0\0\0\xca\x82\xca\x89\0\xca"
      u8"\x82\0\0\0\xca\x83\xc9\xb3\0\xca\x83\xca\x84\0\xca\x83\xca\x85\0\xca\x83\xca\x89\0\xca\x83\0\0\0\xca"
      u8"\x84\xca\x85\0\xca\x84\xca\x89\0\xca\x84\0\0\0\xca\x85\xc9\xb3\0\xca\x85\xca\x85\0\xca\x86\xc9\xb7\0"
      u8"\xca\x86\xc9\xb8\0\xca\x86\xca\x85\0\xca\x86\xca\x86\0\xca\x86\xca\x89\0\xca\x86\0\0\0\xca\x89\xca"
      u8"\x9e\0\0\xc9\xb7\0\0\0\xc9\xb8\0\0\0\xca\x85\0\0\0\xca\x86\0\0\0\xca\x89\0\0\0\0\0\0\0\xc9\xb2\0\0\0"
      u8"\xc9\xb2\0\0\0\xc9\xb2\0\0\0\xc9\xb2\xca\x85\0\xc9\xb2\xca\x87\0\xc9\xb4\0\0\0\xc9\xb4\0\0\0\xc9\xb4"
      u8"\0\0\0\xc9\xb4\xca\x85\0\xc9\xb4\xca\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\x85\0\0\0\xca\x87\0\0"
      u8"\0\xca\x85\0\0\0\0\0\0\0\0\xca\x85\0\0\0\0\0\0\0\0\xca\x85\0\0\0\0\0\0\0\0\xca\x85\0\0\xc9\xb9\0\0\0"
      u8"\xc9\xb9\0\0\0\xc9\xb9\0\0\0\xc9\xb9\xca\x85\0\xc9\xbb\0\0\0\xc9\xbb\0\0\0\xc9\xbb\xca\x85\0\xc9\xbc"
      u8"\0\0\0\xc9\xbc\0\0\0\xc9\xbc\0\0\0\xc9\xbc\xca\x85\0\xc9\xbd\0\0\0\xc9\xbe\xca\x85\0\xc9\xbf\0\0\0"
      u8"\xc9\xbf\xca\x85\0\0\0\0\0\0\0\xca\x85\0\0\xca\x81\0\0\0\xca\x81\0\0\0\xca\x81\0\0\0\xca\x81\xca\x85"
      u8"\0\xca\x82\0\0\0\xca\x82\xca\x85\0\xca\x83\0\0\0\xca\x83\0\0\0\xca\x83\0\0\0\xca\x83\xca\x84\0\xca"
      u8"\x83\xca\x85\0\xca\x84\0\0\0\xca\x84\0\0\0\xca\x84\0\0\0\xca\x84\xca\x85\0\xca\x84\xca\x87\0\xca\x85"
      u8"\0\0\0\xca\x85\0\0\0\xca\x85\0\0\0\xca\x85\xca\x85\0\xca\x86\0\0\0\xca\x86\0\0\0\xca\x86\0\0\0\xca"
      u8"\x86\xca\x85\0\xca\x86\xca\x87\0\xca\x87\0\0\0\xca\x87\xca\x85\0\xca\x87\xca\x9e\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\xca\x85\0\0\0\xca\x87\0\0\xc9\xb2\xca\x85\0\xc9\xb2\xca\x87\0\xc9\xb4\xca\x85\0\xc9"
      u8"\xb4\xca\x87\0\0\xca\x85\0\0\0\xca\x87\0\0\0\xca\x85\0\0\0\xca\x87\0\0\xc9\xb9\xca\x85\0\xc9\xb9\xca"
      u8"\x87\0\xc9\xba\xca\x85\0\xc9\xba\xca\x87\0\xca\x83\xca\x84\0\xca\x83\xca\x85\0\xca\x84\xca\x85\0\xca"
      u8"\x86\xca\x85\0\xca\x86\xca\x87\0\0\xca\x85\0\0\0\xca\x87\0\0\xca\x80\0\xca\x8b\xca\x80\0\xca\x8b\xca"
      u8"\x80\xca\x8a\xca\x8b\xc9\xbd\xca\x89\0\0\xc9\xbd\0\0\0\0\xc9\xbf\xca\x89\0\0\xc9\xbf\0\0\0\0\0\xca"
      u8"\x89\0\0\0\0\0\0\0\0\0\xc9\xb9\xca\x89\0\0\xc9\xb9\0\0\0\0\xc9\xba\xca\x89\0\0\xc9\xba\0\0"

      // Start of 0xfd00:
      u8"\0\0\0\xca\x89\0\0\0\0\xca\x89\0\0\0\xc9\xbb\xca\x89\xc9\xbb\0\0\xc9\xbc\xca\x89\xc9\xbc\0\0\xc9\xba"
      u8"\0\0\xc9\xba\0\0\xc9\xba\0\0\xc9\xba\xca\x85\xc9\xba\xc9\xb7\xc9\xb9\xc9\xb7\xc9\xbb\xc9\xb7\xc9\xbc"
      u8"\xc9\xb7\xc9\xbd\xca\x89\xc9\xbd\0\0\xc9\xbf\xca\x89\xc9\xbf\0\0\0\xca\x89\0\0\0\0\0\xc9\xb9\xca\x89"
      u8"\xc9\xb9\0\0\xc9\xba\xca\x89\xc9\xba\0\0\0\xca\x89\0\0\0\0\0\0\xca\x89\0\0\0\0\0\0\xca\x89\0\0\0\0\0"
      u8"\xc9\xbb\xca\x89\xc9\xbb\0\0\xc9\xbc\xca\x89\xc9\xbc\0\0\xc9\xba\0\0\xc9\xba\0\0\xc9\xba\0\0\xc9\xba"
      u8"\xca\x85\xc9\xba\xc9\xb7\xc9\xb9\xc9\xb7\xc9\xbb\xc9\xb7\xc9\xbc\xc9\xb7\xc9\xba\0\0\xc9\xba\0\0\xc9"
      u8"\xba\0\0\xc9\xba\xca\x85\xc9\xb9\xca\x87\xc9\xba\xca\x87\xc9\xbd\xca\x85\xc9\xb9\0\0\xc9\xb9\0\0\xc9"
      u8"\xb9\0\0\xc9\xba\0\0\xc9\xba\0\0\xc9\xba\0\0\xc9\xbd\xca\x85\xc9\xbe\xca\x85\xc9\xb3\0\0\xc9\xb3\0\0"
      u8"\0\0\xca\x85\0\0\0\0\0\0\0\0\0\0\xca\x85\0\0\xca\x85\0\xca\x85\0\0\xca\x85\0\0\xca\x85\0\0\xca\x85\0"
      u8"\0\xca\x85\0\0\xca\x85\0\0\xca\x85\xca\x89\xc9\xb9\0\0\0\xc9\xb9\0\0\0\xc9\xb9\0\xca\x89\xc9\xb9\xca"
      u8"\x85\0\xc9\xb9\xca\x85\0\xc9\xb9\xca\x85\0\xc9\xb9\xca\x85\xca\x85\xc9\xb9\xca\x85\xca\x85\xc9\xbb\0"
      u8"\0\0\0\xc9\xbb\0\0\0\0\xc9\xbb\xca\x85\xca\x85\xc9\xba\0\xca\x85\0\xc9\xba\0\xca\x85\0\xc9\xba\0\0\0"
      u8"\0\xc9\xba\xca\x85\0\0\xc9\xba\xca\x85\0\0\xc9\xba\xca\x85\xca\x85\xc9\xba\xca\x85\xca\x85\xc9\xbc\0"
      u8"\xca\x89\0\xc9\xbc\0\xca\x85\0\xc9\xbc\0\xca\x85\0\xc9\xbd\xca\x85\0\0\xc9\xbd\xca\x85\0\0\xc9\xbd"
      u8"\xca\x85\xca\x85\xc9\xbd\xca\x85\0\0\xc9\xbf\0\xca\x85\0\xc9\xbf\xca\x85\xca\x85\xc9\xbf\xca\x85\xca"
      u8"\x85\xc9\xbf\xca\x85\xca\x89\0\xca\x85\xca\x85\0\0\xca\x85\0\0\0\0\xca\x85\xca\x89\0\xca\x81\0\xca"
      u8"\x85\0\xca\x81\0\xca\x85\0\xca\x82\xca\x85\0\0\xca\x82\xca\x85\xca\x85\xca\x84\0\xca\x85\0\xca\x84\0"
      u8"\0\0\0\xca\x84\0\xca\x89\0\xca\x84\0\0\0\0\xca\x84\0\0\0\0\xca\x84\0\xca\x85\0\xca\x84\0\xca\x85\0"
      u8"\xca\x84\xca\x85\0\0\xca\x84\xca\x85\0\0\xca\x85\0\0\0\0\xca\x85\0\xca\x85\0\xca\x85\0\0\0\0\xca\x85"
      u8"\0\0\0\0\xca\x85\0\xca\x85\0\xca\x85\0\0\0\0\xca\x85\0\xca\x85\0\xca\x85\0\0\0\0\xca\x87\xca\x85\0\0"
      u8"\xca\x87\xca\x85\xca\x85\xca\x86\0\xca\x85\0\xca\x86\0\xca\x89\0\xca\x86\0\xca\x85\0\xca\x86\0\xca"
      u8"\x85\0\xca\x86\0\xca\x89\0\xca\x86\xca\x85\0\0\xca\x86\xca\x85\xca\x89\0\xca\x85\xca\x85\0\0\xca\x85"
      u8"\xca\x85\0\xc9\xb4\0\0\0\0\0\0\0\0\0\0\0\0\xca\x89\0\0\0\0\0\0\0\0\0\0\xca\x89\0\0\0\xca\x85\0\0\0\0"
      u8"\xca\x85\xca\x89\0\0\xca\x85\0\0\0\0\0\xca\x89\0\0\0\xca\x85\xca\x89\0\xc9\xb9\0\xca\x89\0\xc9\xbb\0"
      u8"\0\0\0\xc9\xba\0\0\0\0\xc9\xbc\0\0\0\0\xca\x84\0\0\0\0\xca\x84\xca\x85\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xca\x85\0\0\0\xca\x85\xca\x85\0\0\xca\x82\xca\x85\0\0\xca\x86\0\0\0\0\xca\x82\xca\x85\0\0\xca\x84\0"
      u8"\xca\x85\0\xc9\xbf\xca\x85\0\0\xca\x83\xca\x85\0\0\xca\x86\0\0\0\0\xca\x85\0\0\0\0\xca\x84\0\xca\x85"
      u8"\0\xca\x83\xca\x85\xca\x85\xca\x84\0\xca\x85\0\xca\x86\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\x85\0\0\0"
      u8"\0\xca\x81\xca\x85\0\0\xc9\xb4\0\0\0\0\xca\x83\xca\x85\xca\x85\xc9\xbf\0\xca\x85\0\xc9\xbb\xca\x85"
      u8"\xca\x85\xc9\xb9\0\0\0\0\xca\x86\0\0\0\0\xc9\xbb\xca\x84\0\0\xca\x82\xca\x84\0\0\xc9\xb3\xca\x84\xca"
      u8"\x84\xca\x87\xc9\xb3\xca\x83\xc9\xb4\xc9\xb7\xca\x85\0\xca\x85\0\0\0\xc9\xbb\xca\x84\xc9\xbf\xca\x85"
      u8"\xc9\xb7\xc9\xb9\xca\x88\xca\x84\xc9\xbf\xca\x84\0\xca\x87\0\xca\x88\xc9\xb9\xca\x84\xca\x85\xc9\xbb"
      u8"\xca\x84\xca\x89\0\0\xc9\xbb\xca\x84\xca\x89\24\xc9\xb3\xca\x84\xca\x84\xca\x87\24\xc9\xbf\xca\x84\0"
      u8"\xca\x87\24\xca\x88\xc9\xb9\xca\x84\xca\x85\0\xca\x84\24\0\xca\x84\xc9\xb3\xca\x84\xca\x87\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc9\xb7\0\xc9\xb3\xca\x84\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0"

      // Start of 0xfe00:
      u8"\0\xe0\xae\xb9\xe0\xae\xba\0\0\0\0\0\0\25\0\0\0\0\0\xe0\xaf\x88\xe0\xaf\x89\xdf\xaa\0\xdf\xa9\0\xdf"
      u8"\x9e\0\xdf\x9d\0\0\0\0\0\0\0\34\0\0\35\0\0\0\0\0\0\0\0\xe0\xaf\x86\xe0\xaf\x87\xe0\xaf\x82\xe0\xaf"
      u8"\x83\0\0\0\0\0\0\xe0\xaf\x80\xe0\xaf\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xae\xb9\0\0\0\0\0\0\0\0\0\0\0\0\25\0\0\xdf\x9e\0\34\0\0\35\0\0\0\0\0\0"
      u8"\0\0\xe0\xaf\x86\xe0\xaf\x87\27\0\0\32\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\30\0\0\31\0\0"
      u8"\50\0\0\24\0\0\xca\x80\0\24\0\0\24\0\0\24\0\0\xca\x80\0\24\0\0\xca\x80\0\24\xca\x8a\xca\x80\xca\x8a"
      u8"\24\xca\x8b\0\xca\x80\xca\x8b\24\xca\x8c\0\xca\x80\xca\x8c\xc9\xad\0\0\xc9\xae\0\0\xc9\xae\0\0\xc9"
      u8"\xaf\0\0\xc9\xaf\0\0\xc9\xb0\0\0\xc9\xb0\0\0\xc9\xb1\0\0\xc9\xb1\0\0\xc9\xb2\0\0\xc9\xb2\0\0\xc9\xb2"
      u8"\0\0\xc9\xb2\0\0\xc9\xb3\0\0\xc9\xb3\0\0\xc9\xb4\0\0\xc9\xb4\0\0\xc9\xb4\0\0\xc9\xb4\0\0\xc9\xb5\0\0"
      u8"\xc9\xb5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc9\xb6\0\0"
      u8"\xc9\xb6\0\0\xc9\xb7\0\0\xc9\xb7\0\0\xc9\xb8\0\0\xc9\xb8\0\0\xc9\xb9\0\0\xc9\xb9\0\0\xc9\xb9\0\0\xc9"
      u8"\xb9\0\0\xc9\xba\0\0\xc9\xba\0\0\xc9\xba\0\0\xc9\xba\0\0\xc9\xbb\0\0\xc9\xbb\0\0\xc9\xbb\0\0\xc9\xbb"
      u8"\0\0\xc9\xbc\0\0\xc9\xbc\0\0\xc9\xbc\0\0\xc9\xbc\0\0\xc9\xbd\0\0\xc9\xbd\0\0\xc9\xbd\0\0\xc9\xbd\0\0"
      u8"\xc9\xbe\0\0\xc9\xbe\0\0\xc9\xbe\0\0\xc9\xbe\0\0\xc9\xbf\0\0\xc9\xbf\0\0\xc9\xbf\0\0\xc9\xbf\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\x81\0\0\xca\x81\0\0\xca\x81\0\0\xca\x81\0\0\xca\x82\0\0\xca\x82\0\0"
      u8"\xca\x82\0\0\xca\x82\0\0\xca\x83\0\0\xca\x83\0\0\xca\x83\0\0\xca\x83\0\0\xca\x84\0\0\xca\x84\0\0\xca"
      u8"\x84\0\0\xca\x84\0\0\xca\x85\0\0\xca\x85\0\0\xca\x85\0\0\xca\x85\0\0\xca\x86\0\0\xca\x86\0\0\xca\x86"
      u8"\0\0\xca\x86\0\0\xca\x87\0\0\xca\x87\0\0\xca\x87\0\0\xca\x87\0\0\xca\x88\0\0\xca\x88\0\0\xca\x89\0\0"
      u8"\xca\x89\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\x84\xc9\xae\xca\x84\xc9\xae\xca\x84\xc9\xaf\xca\x84"
      u8"\xc9\xaf\xca\x84\xc9\xb1\xca\x84\xc9\xb1\xca\x84\xc9\xb3\xca\x84\xc9\xb3"

      // Start of 0xff00:
      u8"\25\26\27\30\31\32\33\34\35\0\0\0\0\0\0"

      // Start of 0x1fb00:
      u8"\36\37\40\41\42\43\44\45\46\47\0\0\0\0\0\0\50\51\52\53\54\55\56\57\60\61\0\0\0\0\0\0\62\63\64\65\66"
      u8"\67\70\71\72\73\0\0\0\0\0\0\74\75\76\77\x40\x41\x42\x43\x44\x45\0\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b"
      u8"\x4c\x4d\x4e\x4f\0\0\0\0\0\xe0\xae\xa9\xe0\xae\xaa\xe0\xae\xba\0\0\0\0\0\0\xe0\xae\xb9\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe7\x94\xb0\xec\x9b\x80\xea\x8c\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\36\0\0\xc4\xac\0\xe0\xae\xb8"
      u8"\xe9\x8f\xa0\xec\x8e\x80\xe5\xb8\x80\xea\xb0\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xb0\x9b\0\0"
      u8"\0\xe0\xb1\x9c\xe0\xb0\xbb\xe0\xb0\xbc\xe0\xb0\xbd\xe0\xb0\xbe\xe0\xb0\xbf\xe0\xb1\x80\xe0\xb1\x81"
      u8"\xe0\xb1\x82\xe0\xb1\x83\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xb1\x84\xe0\xb1\x85\xe0\xb1\x86\xe0"
      u8"\xb1\x87\xe0\xb1\x88\xe0\xb1\x89\xe0\xb1\x8a\xe0\xb1\x8b\xe0\xb1\x8c\xe0\xb1\x8d\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\xe0\xb1\x8e\xe0\xb1\x8f\xe0\xb1\x90\xe0\xb1\x91\xe0\xb1\x92\xe0\xb1\x93\xe0\xb1\x94"
      u8"\xe0\xb1\x95\xe0\xb1\x96\xe0\xb1\x97\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xb1\x98\xe0\xb1\x99\xe0"
      u8"\xb1\x9a\xe0\xb1\x9b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa7\x86\xe0\xa2\x8e\xe0\xa2\x8f"
      u8"\xe0\xa2\x90\xe0\xa2\x91\0\0\0"

      // Start of 0x10700:
      u8"\0\0\0\xc4\xab\xc3\xbd\0\0\0\0\0\0\0\0\xc4\x80\xc4\x81\0\0\xc4\x82\0\0\0\0\xc4\x88\xc4\x86\xc4\x84\0"
      u8"\0\x7f\0\0\0\xc4\x8b\xc4\x9c\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc4\x94\xc4\x95\x47\0\0\0\0\0\0"
      u8"\0\0\0\xc4\x98\0\0\0\0\0\0\0\0\xc4\xa0\0\0\0\0\0\0\0\0\xc4\xaa\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x11000:
      u8"\xe2\xad\x9b\0\0\0\0\0\0\0\0\0"

      // Start of 0x11100:
      u8"\xe2\xad\xbb\xe2\xad\xb7\xe2\xad\xbc\xe2\xad\xb7"

      // Start of 0x11300:
      u8"\xe2\xb1\x93\0\xe2\xb1\x93\xe2\xb1\x9d"

      // Start of 0x11900:
      u8"\xe2\xba\x9f\xe2\xba\x9a"

      // Start of 0x1d400:
      u8"\51\52\53\54\55\56\57\60\61\0\0\0\0\0\0\62\63\64\65\66\67\70\71\72\73\0\75\76\77\x40\x41\x42\x43\x44"
      u8"\x45\0\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\0\51\52\53\54\55\56\57\60\61\0\0\0\0\0\0\62"
      u8"\63\64\65\66\67\70\71\72\73\0\75\76\77\x40\x41\x42\x43\x45\0\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c"
      u8"\x4d\x4e\x4f\0\51\52\53\54\55\56\57\60\61\0\0\0\0\0\0\62\63\64\65\66\67\70\71\72\73\0\75\76\77\x40"
      u8"\x41\x42\x43\x44\x45\0\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\0\51\53\54\57\0\0\0\0\62\63"
      u8"\65\66\67\70\71\72\73\0\75\76\77\x40\x42\x44\x45\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\0"
      u8"\51\52\53\54\55\56\57\60\61\0\0\0\0\0\0\62\63\64\65\66\67\70\71\72\73\0\75\76\77\x40\x41\x42\x43\x44"
      u8"\x45\0\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c"

      // Start of 0x1d500:
      u8"\x4d\x4e\x4f\0\51\52\54\55\56\57\0\0\0\0\0\0\62\63\65\66\67\70\71\72\73\75\76\77\x40\x41\x42\x43\x44"
      u8"\x45\0\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\0\51\52\54\55\56\57\61\0\0\0\0\0\65\66\67"
      u8"\70\71\72\73\75\76\77\x40\x41\x42\x43\x44\x45\0\0\0\0\0\0\x46\x47\x48"

      // Start of 0x1d600:
      u8"\x49\x4a\x4b\x4c\x4d\x4e\x4f\0\51\52\53\54\55\56\57\60\61\0\0\0\0\0\0\62\63\64\65\66\67\70\71\72\73"
      u8"\0\75\76\77\x40\x41\x42\x43\x44\x45\0\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\0\51\52\53"
      u8"\54\55\56\57\60\61\0\0\0\0\0\0\62\63\64\65\66\67\70\71\72\73\0\75\76\77\x40\x41\x42\x43\x44\x45\0\0"
      u8"\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\0\51\52\53\54\55\56\57\60\61\0\0\0\0\0\0\62\63\64"
      u8"\65\66\67\70\71\72\73\0\75\76\77\x40\x41\x42\x43\x44\x45\0\0\0\0\0\0\x46\x47\x48\x49\x4a\x4b\x4c\x4d"
      u8"\x4e\x4f\0\xc2\x83\xc3\xad\xc6\x87\xc6\x88\xc6\x89\xc6\x8a\xc6\x8b\xc6\x8c\xc6\x8d\xc6\x8e\xc6\x8f\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\x9f\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1d700:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\x9a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc6\x87\0"
      u8"\xc6\x88\0\xc6\x89\0\xc6\x8a\0\xc6\x8b\0\xc6\x8c\0\xc6\x8d\0\xc6\x8e\0\xc6\x8f\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\x9f\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\x9a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc6\x87\0\xc6"
      u8"\x88\0\xc6\x89\0\xc6\x8a\0\xc6\x8b\0\xc6\x8c\0\xc6\x8d\0\xc6\x8e\0\xc6\x8f\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\x9f\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\x9a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc6\x87\0\xc6\x88"
      u8"\0\xc6\x89\0\xc6\x8a\0\xc6\x8b\0\xc6\x8c\0\xc6\x8d\0\xc6\x8e\0\xc6\x8f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\x9f\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa2\x9a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\36\0\0\37"
      u8"\0\0\40\0\0\41\0\0\42\0\0\43\0\0\44\0\0\45\0\0\46\0\0\47\0\0\36\0\0\37\0\0\40\0\0\41\0\0\42\0\0\43\0"
      u8"\0\44\0\0\45\0\0\46\0\0\47\0\0\36\0\0\37\0\0\40\0\0\41\0\0\42\0\0\43\0\0\44\0\0\45\0\0\46\0\0\47\0\0"
      u8"\36\0\0\37\0\0\40\0\0\41\0\0\42\0\0\43\0\0\44\0\0\45\0\0\46\0\0\47\0\0\36\0\0\37\0\0\40\0\0\41\0\0"
      u8"\42\0\0\43\0\0\44\0\0\45\0\0\46\0\0\47\0\0"

      // Start of 0x1e000:
      u8"\xc6\xae\xc6\xaf\xc6\xb0\xc6\xb1\xc6\xb2\xc6\xb3\xc6\xb4\xc6\xb5\xc6\xb6\0\0\0\0\0\0\0\0\0\0\xc6\xb8"
      u8"\xc6\xb9\xc6\xba\xc6\xbb\xc6\xbc\xc6\xbd\xc6\xbe\xc6\xbf\xc7\x80\0\0\0\0\0\0\0\0\0\0\xc7\x88\xc7\x8a"
      u8"\xe2\xa0\x80\0\0\0\0\0\0\xc6\xae\0\xc6\xaf\0\xc6\xb0\0\xc6\xb1\0\xc6\xb2\0\xc6\xb3\0\xc6\xb4\0\xc6"
      u8"\xb5\0\xc6\xb6\0\0\0\0\0\0\0\0\0\0\0\0\0\xc6\xb9\0\xc6\xbb\0\xc6\xbc\0\xc6\xbd\0\xc6\xbe\0\xc6\xbf\0"
      u8"\xc7\x80\0\0\0\0\0\0\0\xc7\xab\0\xc7\x88\0\xc7\x87\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1ee00:
      u8"\xc9\xb3\xc9\xb4\0\0\0\0\xca\x88\xc9\xb8\0\0\xc9\xbd\0\0\xca\x83\xca\x84\xca\x85\xca\x86\xc9\xb9\xc9"
      u8"\xbf\xca\x81\xc9\xbb\xca\x82\xc9\xb7\xc9\xba\0\0\0\0\0\0\xc9\xb6\xc9\xbc\xc9\xbe\0\0\0\0\0\0\0\0\0\0"
      u8"\xc9\xb4\0\0\xca\x87\0\0\0\0\xca\x83\xca\x84\xca\x85\xca\x86\xc9\xb9\xc9\xbf\xca\x81\xc9\xbb\xca\x82"
      u8"\xc9\xba\0\0\0\0\0\0\xc9\xbc\0\0\0\0\0\0\0\0\xca\x84\xca\x86\xc9\xb9\xc9\xbf\xc9\xbb\xca\x82\xc9\xba"
      u8"\0\0\xc9\xbc\0\0\0\0\0\0\xc9\xb4\0\0\xca\x87\0\0\xc9\xbd\0\0\xca\x83\xca\x85\xca\x86\xc9\xb9\xc9\xbf"
      u8"\xca\x81\xc9\xbb\xca\x82\xc9\xba\0\0\0\0\0\0\xc9\xbc\xc9\xbe\0\0\0\0\0\0\xc9\xb3\xc9\xb4\0\0\0\0\xca"
      u8"\x87\xca\x88\xc9\xb8\0\0\xc9\xbd\0\0\xca\x84\xca\x85\xca\x86\xc9\xb9\xc9\xbf\xca\x81\xc9\xbb\xca\x82"
      u8"\xc9\xb7\xc9\xba\0\0\0\0\0\0\xc9\xb6\xc9\xbc\xc9\xbe\0\0\xc9\xb4\0\0\0\0\xca\x88\xc9\xb8\0\0\xc9\xbd"
      u8"\0\0\xca\x84\xca\x85\xca\x86\xc9\xb9\xc9\xbf\xca\x81\xc9\xbb\xca\x82\xc9\xb7\xc9\xba\0\0\0\0\0\0\xc9"
      u8"\xb6\xc9\xbc\xc9\xbe\0\0"

      // Start of 0x1f100:
      u8"\36\0\36\0\37\0\40\0\41\0\42\0\43\0\44\0\45\0\46\0\47\0\34\51\35\34\52\35\34\53\35\34\54\35\34\55\35"
      u8"\34\56\35\34\57\35\34\60\35\34\61\35\34\0\35\34\0\35\34\0\35\34\0\35\34\0\35\34\0\35\34\62\35\34\63"
      u8"\35\34\64\35\34\65\35\34\66\35\34\67\35\34\70\35\34\71\35\34\72\35\34\73\35\34\0\35\xe0\xaf\x86\65"
      u8"\xe0\xaf\x87\53\0\0\0\0\0\0\64\0\0\0\0\0\0\53\54\0\0\0\0\0\71\0\0\0\0\0\0\51\0\0\0\0\0\0\52\0\0\0\0"
      u8"\0\0\53\0\0\0\0\0\0\54\0\0\0\0\0\0\55\0\0\0\0\0\0\56\0\0\0\0\0\0\57\0\0\0\0\0\0\60\0\0\0\0\0\0\61\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\62\0"
      u8"\0\0\0\0\0\63\0\0\0\0\0\0\64\0\0\0\0\0\0\65\0\0\0\0\0\0\66\0\0\0\0\0\0\67\0\0\0\0\0\0\70\0\0\0\0\0\0"
      u8"\71\0\0\0\0\0\0\72\0\0\0\0\0\0\73\0\0\0\0\0\0\0\0\0\0\0\0\0\60\70\0\0\0\0\0\0\70\0\0\0\0\0\65\54\0\0"
      u8"\0\0\0\65\65\0\0\0\0\0\62\62\70\0\0\0\0\71\53\0\0\0\0\0\0\53\0\0\0\0\0\0\54\0\0\0\0\0\0\64\0\0\0\0\0"
      u8"\54"

      // Start of 0x1f200:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe5\xae\xa8\xe1\x9c\xa9\0\0\0\xe1\xa7\x9c\xe1\xaf\x91\xe1\xa7"
      u8"\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x94\xbf\0\0\0\xe1\xa2\x97\xe1\xa3\x93"
      u8"\4\0\0\xe2\xa0\x80\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xa2\xa3\0\0\0\xe1\xa1\xad\xe1\xbc\xad\0\0\0\xe1\x94"
      u8"\xa0\0\0\0\xe1\xa8\xb5\xe1\xa8\xb4\xe1\xb5\xad\xe1\x92\x98\0\0\0\0\0\0\xe0\xaf\x86\0\xe0\xaf\x87\xe0"
      u8"\xaf\x86\xe2\xa0\x80\xe0\xaf\x87\xe0\xaf\x86\0\xe0\xaf\x87\0\0\xe0\xaf\x86\0\xe0\xaf\x87\0\0\xe0\xaf"
      u8"\x86\0\xe0\xaf\x87\0\0\xe0\xaf\x86\xe1\xa1\xad\xe0\xaf\x87\xe0\xaf\x86\0\xe0\xaf\x87\0\0\xe0\xaf\x86"
      u8"\0\xe0\xaf\x87\0\0\xe0\xaf\x86\xe1\xa6\x9d\xe0\xaf\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2f800:
      u8"\0\0\0\xe4\xba\x9a\0\0\0\0\0\0\0\0\0\xe1\x8e\x8a\0\0\0\xe1\x8f\xab\xe6\x94\x80\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\xe1\x90\xa2\xe1\x90\xac\xe1\x90\xb9\0\0\0\0\0\0\xe1\x90\xaf\0\0\0\0\0\0\xe1\x91\x8d\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\xe1\x91\x93\0\0\0\0\0\0\xe1\x91\xbe\xe1\x92\x98\xe1\x92\x9d\xe0\xb6\xbb\0\0\0"
      u8"\0\0\0\xee\xbd\x80\0\0\0\xe1\x92\xb9\xe1\x92\xba\xe1\x93\x85\xe1\x93\xa5\xe1\x93\xa7\0\0\0\xe1\x93"
      u8"\xbd\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xae\x9e\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x94\x9e\0\0\0\xe1"
      u8"\x94\xbe\xe1\x95\x88\xe1\x95\x9c\0\0\0\0\0\0\xe1\x96\x86\xe1\x96\xb1\xe1\x96\xbb\xe1\x97\x90\xe1\x97"
      u8"\x90\xe1\x97\x9f\0\0\0\0\0\0\0\0\0\xe1\x99\x94\xe1\x97\xa6\xe1\x99\x95\xe1\x98\x93\xe1\x98\xaa\xe1"
      u8"\x91\x97\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x9b\x88\xe1\x9b\x87\0\0\0\xea\x9d\xa0\0\0\0\0\0\0\xe1\x9c"
      u8"\x92\0\0\0\xe1\x9c\xa2\xe1\x9d\x8a\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbb\x85\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbc\x96\xe5\x9a"
      u8"\xa7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe5\x9c\xbb\xe5\x9c\xbb\xe2\x80\x89\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\xe9\x91\xb0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x9f\x81\0\0\0\0\0\0\xe1\x9f\x8e\0\0"
      u8"\0\0\0\0\xe1\xa0\x84\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xa1\x82\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\xe1\xa3\x8e\0\0\0\0\0\0\0\0\0\xe1\xa3\xa0\xe1\xa3\xaf\xe9\xb3\xb0\0\0\0\xe1"
      u8"\xa4\x96\0\0\0\0\0\0\0\0\0\xe1\xa5\x85\0\0\0\0\0\0\xe1\xa5\x8d\0\0\0\0\0\0\0\0\0\0\0\0\xef\xb7\xa8\0"
      u8"\0\0\xe1\xa7\xb9\0\0\0\xe1\xa8\xa3\0\0\0\0\0\0\xe1\x91\x88\xe1\x91\x8b\xe1\xa8\xac\0\0\0\0\0\0\0\0\0"
      u8"\xe1\xa8\xbd\0\0\0\xe1\xa9\x81\0\0\0\xe1\xa9\xa1\0\0\0\0\0\0\0\0\0\xe1\xaa\x81\xe1\xab\x84\xe1\xab"
      u8"\xa5\0\0\0\0\0\0\0\0\0\xe1\xac\x82\0\0\0\xe1\xac\x9e\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2f900:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xad\xbf\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xae\xa5\0\0\0"
      u8"\xe5\x80\xad\xe1\xaf\xa9\xe5\xbb\x87\0\0\0\0\0\0\xe1\xb0\xbc\xe1\xb1\x83\xe1\xb1\x92\xe6\x80\xa0\xe1"
      u8"\xb1\xb0\xe1\xb1\xbf\xe6\x82\x9f\xe6\x83\xae\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xb4\x97\0\0\0"
      u8"\xe1\xb4\xaf\xe1\xb4\xbd\0\0\0\0\0\0\xe1\xb5\xa4\0\0\0\0\0\0\0\0\0\xe1\xb6\x92\0\0\0\xe1\xb6\xba\0\0"
      u8"\0\0\0\0\xe6\x87\x94\0\0\0\xe0\xbe\xa8\0\0\0\0\0\0\0\0\0\xe6\x88\x9f\xe6\x88\xad\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\xe0\xbf\x87\0\0\0\xe0\xbf\x8e\xe1\x80\x80\0\0\0\0\0\0\0\0\0\0\0\0\xe9\xb1\x80\xe6\x90\x9a\xe1"
      u8"\xbc\x94\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x81\xaa\0\0\0\0"
      u8"\0\0\0\0\0\xe1\x82\x83\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x83\x8d\0\0\0\xca\xbc\0\0"
      u8"\0\0\xe1\x83\xae\xe6\x99\xb4\xe6\x9a\x87\xe1\x84\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\xbd\x85\0\0\0"
      u8"\xe6\x9e\x9b\xe1\xbd\xbc\0\0\0\xe1\xbe\x86\0\0\0\0\0\0\0\0\0\xe1\xbe\xa7\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\xe5\xad\xa1\0\0\0\xe2\x80\x89\xe2\x80\x8c\0\0\0\0\0\0\xe2\x81\xa3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\xe3\xa2\x80\0\0\0\xe1\xbd\x80\0\0\0\xe2\x82\xab\0\0\0\xe2\x82\x83\0\0\0\xec\xae\x80\xe2\x84\x89"
      u8"\xe2\x82\xa1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x85\x84\0\0\0\xe2\x85\xb4\0\0"
      u8"\0\0\0\0\xe1\x87\x91\0\0\0\0\0\0\0\0\0\xe2\x87\x8a\0\0\0\xe2\x87\x9b\xe2\x87\x9d\0\0\0\xe2\x87\xb0\0"
      u8"\0\0\xe2\x86\x98\xe2\x89\x8b\xe2\x88\x98\0\0\0\xe2\x89\x92\0\0\0\xcd\xa6\0\xe2\x89\xa1\0\0\0\xe2\x8a"
      u8"\x9c\xe2\x8a\x9f\xe6\xb0\x93\0\0\0\0\0\0\xe1\x88\x9b\0\0\0\0\0\0\0\0\0\xe6\xb4\xbe\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe5\x85\x84\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\xe2\x8e\x86\0\0\0\xe2\x8e\x97\0\0\0\0\0\0\xe2\x90\x96\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x93"
      u8"\x87\0\0\0\0\0\0\xe1\x8e\x83\0\0\0\0\0\0\xea\xb9\x80\0\0\0\0\0\0\xe2\x97\xbb\xe7\x87\x99\0\0\0\0\0\0"
      u8"\0\0\0\x61\0\0\0\0\0\0\0\0\xe7\x8c\xb8\0\0\0\0\0\0\xe2\x99\xa5\0\0\0\xe2\x99\x88\0\0\0\xe2\x9b\x89\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
      // done.
      20071UL // String Length
    };

    template <typename CharT = char8_t, typename CPType>
    [[nodiscard]] static constexpr CharT const* decomp_ptr(
      decomp_index const code, CPType const code_point) noexcept {
        if constexpr (stl::same_as<CharT, char8_t>) {
            return decomp_values.data() + code.get_position(code_point);
        } else {
            return reinterpret_cast<CharT const*>(decomp_values.data()) + code.get_position(code_point);
        }
    }

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_DECOMPOSITION_TABLES_HPP
