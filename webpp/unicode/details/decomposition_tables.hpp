
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_decomposition_tables.mjs
 *   Unicode UCD Database Creation Date: 2023-08-28
 *   This file's generation date:        Wed, 02 Oct 2024 20:53:38 GMT
 *   Unicode Version:                    15.1.0
 *   Total Table sizes in this file:
 *       - in bits:       308712
 *       - in bytes:      38589 B
 *       - in KibiBytes:  38 KiB
 *   Some other implementations' total table size was 73.4 KiB;
 *   So I saved 36 KiB and a better a locality.
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
        static constexpr auto max_utf8_mapped_length = 0UL;

        /// Maximum values of UTF-16 code points mapped
        static constexpr auto max_utf16_mapped_length = 4UL;

        /// Maximum values of code points mapped (UTF-32)
        static constexpr auto max_utf32_mapped_length = 4UL;

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
      0,       0,       0,       3,       49155,   97539,   145923,  191749,  273669,  0,       0,
      0,       0,       355588,  419846,  515588,  581124,  642564,  0,       707844,  0,       0,
      0,       0,       767236,  0,       0,       832772,  0,       0,       0,       0,       0,
      0,       0,       0,       887814,  979974,  0,       1075462, 1160710, 1252614, 0,       0,
      0,       1348870, 1442054, 1537798, 0,       1634054, 0,       1732361, 0,       1877254, 0,
      1968905, 0,       0,       0,       0,       0,       2115590, 2213638, 0,       2302726, 0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       2399494, 2497798,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       2596101,
      2678021, 2759941, 2841861, 2923782, 3022086, 3120392, 3251206, 3349507, 0,       0,       0,
      3388931, 0,       3431429, 3510021, 3590917, 3672581, 3754501, 3828229, 3899651, 0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       3941637, 0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       4020486, 4115718, 4214022, 0,       0,
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
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       4312323, 4361475, 4410627, 4459779, 4508931,
      4558084, 4623619, 4672772, 4730886, 4829188, 0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       4888072, 0,       5007368, 0,       0,       0,       0,       0,       0,       0,
      0,       5135624, 0,       0,       0,       0,       5251592, 0,       0,       0,       5380616,
      0,       0,       0,       0,       0,       0,       0,       0,       0,       0,       0,
      0,       0,       5503496, 0,       0,       0,       0,       0,       0,       0,       0,
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
      0,       0,       5626892, 5808396, 6005004, 0,       0,       0,       0,       0,       0,
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
      0,       0,       0,       0,       6201604, 6267140, 6332676, 6398212, 6463748, 6529284, 6594820,
      6660356, 6725892};

    /**
     * DECOMP Values Table
     *
     * Decomposition Index
     *
     * Table size:
     *   - in bits:       211144
     *   - in bytes:      26393 B
     *   - in KibiBytes:  26 KiB
     */
    static constexpr std::basic_string_view<char8_t> decomp_values{

      // Start of 0x0-0xc0, 0x240-0x300, 0x480, 0x500-0x5c0, 0x640-0x680, 0x700-0x8c0, 0x980, 0xa80-0xb00,
      // 0xc00, 0xc80, 0xd00, 0xd80, 0xe00-0xf00, 0xfc0, 0x1040-0x1ac0, 0x1b80-0x1dc0, 0x2040-0x20c0, 0x2140,
      // 0x2340-0x2a80, 0x2b00-0x3000, 0x3100-0xf8c0, 0xfb80-0x11040, 0x110c0, 0x11140-0x11300,
      // 0x11380-0x11440, 0x114c0-0x11540, 0x115c0-0x118c0, 0x11940-0x1d100, 0x1d200-0x2f7c0:
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
      u8"\x84\x49\xcc\x86\x69\xcc\x86\x49\xcc\xa8\x69\xcc\xa8\x49\xcc\x87\0\0\0\0\0\0\0\0\0\x4a\xcc\x82\x6a"
      u8"\xcc\x82\x4b\xcc\xa7\x6b\xcc\xa7\0\0\0\x4c\xcc\x81\x6c\xcc\x81\x4c\xcc\xa7\x6c\xcc\xa7\x4c\xcc\x8c"
      u8"\x6c\xcc\x8c"

      // Start of 0x140:
      u8"\0\0\0\0\0\0\0\0\0\x4e\xcc\x81\x6e\xcc\x81\x4e\xcc\xa7\x6e\xcc\xa7\x4e\xcc\x8c\x6e\xcc\x8c\0\0\0\0\0"
      u8"\0\0\0\0\x4f\xcc\x84\x6f\xcc\x84\x4f\xcc\x86\x6f\xcc\x86\x4f\xcc\x8b\x6f\xcc\x8b\0\0\0\0\0\0\x52\xcc"
      u8"\x81\x72\xcc\x81\x52\xcc\xa7\x72\xcc\xa7\x52\xcc\x8c\x72\xcc\x8c\x53\xcc\x81\x73\xcc\x81\x53\xcc\x82"
      u8"\x73\xcc\x82\x53\xcc\xa7\x73\xcc\xa7\x53\xcc\x8c\x73\xcc\x8c\x54\xcc\xa7\x74\xcc\xa7\x54\xcc\x8c\x74"
      u8"\xcc\x8c\0\0\0\0\0\0\x55\xcc\x83\x75\xcc\x83\x55\xcc\x84\x75\xcc\x84\x55\xcc\x86\x75\xcc\x86\x55\xcc"
      u8"\x8a\x75\xcc\x8a\x55\xcc\x8b\x75\xcc\x8b\x55\xcc\xa8\x75\xcc\xa8\x57\xcc\x82\x77\xcc\x82\x59\xcc\x82"
      u8"\x79\xcc\x82\x59\xcc\x88\x5a\xcc\x81\x7a\xcc\x81\x5a\xcc\x87\x7a\xcc\x87\x5a\xcc\x8c\x7a\xcc\x8c"

      // Start of 0x180:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x4f\xcc"
      u8"\x9b\x6f\xcc\x9b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x55"
      u8"\xcc\x9b\x75\xcc\x9b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x41\xcc\x8c\0\0\x61\xcc\x8c\0\0\x49\xcc\x8c\0\0\x69\xcc\x8c\0\0\x4f"
      u8"\xcc\x8c\0\0\x6f\xcc\x8c\0\0\x55\xcc\x8c\0\0\x75\xcc\x8c\0\0\x55\xcc\x88\xcc\x84\x75\xcc\x88\xcc\x84"
      u8"\x55\xcc\x88\xcc\x81\x75\xcc\x88\xcc\x81\x55\xcc\x88\xcc\x8c\x75\xcc\x88\xcc\x8c\x55\xcc\x88\xcc\x80"
      u8"\x75\xcc\x88\xcc\x80\0\0\0\0\0\x41\xcc\x88\xcc\x84\x61\xcc\x88\xcc\x84\x41\xcc\x87\xcc\x84\x61\xcc"
      u8"\x87\xcc\x84\xc3\x86\xcc\x84\0\xc3\xa6\xcc\x84\0\0\0\0\0\0\0\0\0\0\0\x47\xcc\x8c\0\0\x67\xcc\x8c\0\0"
      u8"\x4b\xcc\x8c\0\0\x6b\xcc\x8c\0\0\x4f\xcc\xa8\0\0\x6f\xcc\xa8\0\0\x4f\xcc\xa8\xcc\x84\x6f\xcc\xa8\xcc"
      u8"\x84\xc6\xb7\xcc\x8c\0\xca\x92\xcc\x8c\0\x6a\xcc\x8c\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x47\xcc\x81\0"
      u8"\0\x67\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\x4e\xcc\x80\0\0\x6e\xcc\x80\0\0\x41\xcc\x8a\xcc\x81\x61\xcc"
      u8"\x8a\xcc\x81\xc3\x86\xcc\x81\0\xc3\xa6\xcc\x81\0\xc3\x98\xcc\x81\0\xc3\xb8\xcc\x81\0"

      // Start of 0x200:
      u8"\x41\xcc\x8f\0\0\x61\xcc\x8f\0\0\x41\xcc\x91\0\0\x61\xcc\x91\0\0\x45\xcc\x8f\0\0\x65\xcc\x8f\0\0\x45"
      u8"\xcc\x91\0\0\x65\xcc\x91\0\0\x49\xcc\x8f\0\0\x69\xcc\x8f\0\0\x49\xcc\x91\0\0\x69\xcc\x91\0\0\x4f\xcc"
      u8"\x8f\0\0\x6f\xcc\x8f\0\0\x4f\xcc\x91\0\0\x6f\xcc\x91\0\0\x52\xcc\x8f\0\0\x72\xcc\x8f\0\0\x52\xcc\x91"
      u8"\0\0\x72\xcc\x91\0\0\x55\xcc\x8f\0\0\x75\xcc\x8f\0\0\x55\xcc\x91\0\0\x75\xcc\x91\0\0\x53\xcc\xa6\0\0"
      u8"\x73\xcc\xa6\0\0\x54\xcc\xa6\0\0\x74\xcc\xa6\0\0\0\0\0\0\0\0\0\0\0\0\x48\xcc\x8c\0\0\x68\xcc\x8c\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x41\xcc\x87\0\0\x61\xcc\x87\0\0\x45\xcc"
      u8"\xa7\0\0\x65\xcc\xa7\0\0\x4f\xcc\x88\xcc\x84\x6f\xcc\x88\xcc\x84\x4f\xcc\x83\xcc\x84\x6f\xcc\x83\xcc"
      u8"\x84\x4f\xcc\x87\0\0\x6f\xcc\x87\0\0\x4f\xcc\x87\xcc\x84\x6f\xcc\x87\xcc\x84\x59\xcc\x84\0\0\x79\xcc"
      u8"\x84\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x340:
      u8"\xcc\x80\0\0\xcc\x81\0\0\0\0\0\0\xcc\x93\0\0\xcc\x88\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xca\xb9\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\73\0\0"

      // Start of 0x380:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc2\xa8\xcc\x81\0\0\xce\x91\xcc\x81\0\0"
      u8"\xc2\xb7\0\0\0\0\xce\x95\xcc\x81\0\0\xce\x97\xcc\x81\0\0\xce\x99\xcc\x81\0\0\0\0\0\0\0\0\xce\x9f\xcc"
      u8"\x81\0\0\0\0\0\0\0\0\xce\xa5\xcc\x81\0\0\xce\xa9\xcc\x81\0\0\xce\xb9\xcc\x88\xcc\x81\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xce\x99\xcc\x88"
      u8"\0\0\xce\xa5\xcc\x88\0\0\xce\xb1\xcc\x81\0\0\xce\xb5\xcc\x81\0\0\xce\xb7\xcc\x81\0\0\xce\xb9\xcc\x81"
      u8"\0\0\xcf\x85\xcc\x88\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x3c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xce\xb9\xcc\x88\xcf"
      u8"\x85\xcc\x88\xce\xbf\xcc\x81\xcf\x85\xcc\x81\xcf\x89\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xcf\x92"
      u8"\xcc\x81\xcf\x92\xcc\x88\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x400:
      u8"\xd0\x95\xcc\x80\xd0\x95\xcc\x88\0\0\0\0\xd0\x93\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x86\xcc\x88\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x9a\xcc\x81\xd0\x98\xcc\x80\xd0\xa3\xcc\x86\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x98\xcc\x86\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xd0\xb8\xcc\x86\0\0\0\0\0\0\0\0"

      // Start of 0x440:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\xb5\xcc\x80\xd0\xb5\xcc\x88\0\0\0\0\xd0\xb3\xcc\x81\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\xd1\x96\xcc\x88\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\xba\xcc\x81\xd0\xb8\xcc\x80\xd1\x83\xcc"
      u8"\x86\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd1\xb4\xcc"
      u8"\x8f\xd1\xb5\xcc\x8f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x4c0:
      u8"\0\0\0\0\xd0\x96\xcc\x86\xd0\xb6\xcc\x86\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x90\xcc\x86\xd0\xb0\xcc\x86\xd0\x90\xcc\x88\xd0\xb0"
      u8"\xcc\x88\0\0\0\0\0\0\0\0\xd0\x95\xcc\x86\xd0\xb5\xcc\x86\0\0\0\0\0\0\0\0\xd3\x98\xcc\x88\xd3\x99\xcc"
      u8"\x88\xd0\x96\xcc\x88\xd0\xb6\xcc\x88\xd0\x97\xcc\x88\xd0\xb7\xcc\x88\0\0\0\0\0\0\0\0\xd0\x98\xcc\x84"
      u8"\xd0\xb8\xcc\x84\xd0\x98\xcc\x88\xd0\xb8\xcc\x88\xd0\x9e\xcc\x88\xd0\xbe\xcc\x88\0\0\0\0\0\0\0\0\xd3"
      u8"\xa8\xcc\x88\xd3\xa9\xcc\x88\xd0\xad\xcc\x88\xd1\x8d\xcc\x88\xd0\xa3\xcc\x84\xd1\x83\xcc\x84\xd0\xa3"
      u8"\xcc\x88\xd1\x83\xcc\x88\xd0\xa3\xcc\x8b\xd1\x83\xcc\x8b\xd0\xa7\xcc\x88\xd1\x87\xcc\x88\0\0\0\0\0\0"
      u8"\0\0\xd0\xab\xcc\x88\xd1\x8b\xcc\x88"

      // Start of 0x600:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd8\xa7\xd9\x93\xd8\xa7\xd9"
      u8"\x94\xd9\x88\xd9\x94\xd8\xa7\xd9\x95\xd9\x8a\xd9\x94\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x6c0:
      u8"\xdb\x95\xd9\x94\0\0\0\0\xdb\x81\xd9\x94\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xdb\x92\xd9\x94\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x900:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa4"
      u8"\xa8\xe0\xa4\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe0\xa4\xb0\xe0\xa4\xbc\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa4\xb3\xe0\xa4\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x940:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa4\x95"
      u8"\xe0\xa4\xbc\xe0\xa4\x96\xe0\xa4\xbc\xe0\xa4\x97\xe0\xa4\xbc\xe0\xa4\x9c\xe0\xa4\xbc\xe0\xa4\xa1\xe0"
      u8"\xa4\xbc\xe0\xa4\xa2\xe0\xa4\xbc\xe0\xa4\xab\xe0\xa4\xbc\xe0\xa4\xaf\xe0\xa4\xbc\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x9c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa7\x87\xe0\xa6\xbe\xe0\xa7\x87\xe0\xa7\x97\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa6\xa1\xe0\xa6\xbc\xe0\xa6\xa2\xe0"
      u8"\xa6\xbc\0\0\0\0\0\0\xe0\xa6\xaf\xe0\xa6\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xa00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe0\xa8\xb2\xe0\xa8\xbc\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xa8\xb8\xe0\xa8\xbc\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xa40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe0\xa8\x96\xe0\xa8\xbc\xe0\xa8\x97\xe0\xa8\xbc\xe0\xa8\x9c\xe0\xa8\xbc\0\0\0\0\0\0\0\0\0\0\0\0\xe0"
      u8"\xa8\xab\xe0\xa8\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xb40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0"
      u8"\xad\x87\xe0\xad\x96\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xad\x87\xe0\xac\xbe\xe0\xad\x87\xe0\xad\x97\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xac\xa1\xe0\xac\xbc\xe0"
      u8"\xac\xa2\xe0\xac\xbc\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xb80:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xae\x92\xe0\xaf\x97\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
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
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

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
      u8"\xe0\xb2\xbf\xe0\xb3\x95\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xb3\x86\xe0\xb3\x95\0\0\0\xe0\xb3\x86\xe0\xb3\x96\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\xe0\xb3\x86\xe0\xb3\x82\0\0\0\xe0\xb3\x86\xe0\xb3\x82\xe0\xb3\x95\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xd40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xe0\xb5\x86\xe0\xb4\xbe\xe0\xb5\x87\xe0\xb4\xbe\xe0\xb5\x86\xe0\xb5\x97\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xdc0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xb7\x99\xe0\xb7\x8a\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\xe0\xb7\x99\xe0\xb7\x8f\0\0\0\xe0\xb7\x99\xe0\xb7\x8f\xe0\xb7\x8a\xe0\xb7\x99\xe0"
      u8"\xb7\x9f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0"

      // Start of 0xf40:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x82\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x8c\xe0\xbe\xb7\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x91\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\xe0\xbd\x96\xe0\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x9b\xe0"
      u8"\xbe\xb7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\x80\xe0\xbe\xb5\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\xbd\xb1\xe0"
      u8"\xbd\xb2\0\0\0\0\0\0\xe0\xbd\xb1\xe0\xbd\xb4\xe0\xbe\xb2\xe0\xbe\x80\0\0\0\0\0\0\xe0\xbe\xb3\xe0\xbe"
      u8"\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

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
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe1\x80\xa5\xe1\x80\xae\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

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
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0"

      // Start of 0x1e00:
      u8"\x41\xcc\xa5\0\0\x61\xcc\xa5\0\0\x42\xcc\x87\0\0\x62\xcc\x87\0\0\x42\xcc\xa3\0\0\x62\xcc\xa3\0\0\x42"
      u8"\xcc\xb1\0\0\x62\xcc\xb1\0\0\x43\xcc\xa7\xcc\x81\x63\xcc\xa7\xcc\x81\x44\xcc\x87\0\0\x64\xcc\x87\0\0"
      u8"\x44\xcc\xa3\0\0\x64\xcc\xa3\0\0\x44\xcc\xb1\0\0\x64\xcc\xb1\0\0\x44\xcc\xa7\0\0\x64\xcc\xa7\0\0\x44"
      u8"\xcc\xad\0\0\x64\xcc\xad\0\0\x45\xcc\x84\xcc\x80\x65\xcc\x84\xcc\x80\x45\xcc\x84\xcc\x81\x65\xcc\x84"
      u8"\xcc\x81\x45\xcc\xad\0\0\x65\xcc\xad\0\0\x45\xcc\xb0\0\0\x65\xcc\xb0\0\0\x45\xcc\xa7\xcc\x86\x65\xcc"
      u8"\xa7\xcc\x86\x46\xcc\x87\0\0\x66\xcc\x87\0\0\x47\xcc\x84\0\0\x67\xcc\x84\0\0\x48\xcc\x87\0\0\x68\xcc"
      u8"\x87\0\0\x48\xcc\xa3\0\0\x68\xcc\xa3\0\0\x48\xcc\x88\0\0\x68\xcc\x88\0\0\x48\xcc\xa7\0\0\x68\xcc\xa7"
      u8"\0\0\x48\xcc\xae\0\0\x68\xcc\xae\0\0\x49\xcc\xb0\0\0\x69\xcc\xb0\0\0\x49\xcc\x88\xcc\x81\x69\xcc\x88"
      u8"\xcc\x81\x4b\xcc\x81\0\0\x6b\xcc\x81\0\0\x4b\xcc\xa3\0\0\x6b\xcc\xa3\0\0\x4b\xcc\xb1\0\0\x6b\xcc\xb1"
      u8"\0\0\x4c\xcc\xa3\0\0\x6c\xcc\xa3\0\0\x4c\xcc\xa3\xcc\x84\x6c\xcc\xa3\xcc\x84\x4c\xcc\xb1\0\0\x6c\xcc"
      u8"\xb1\0\0\x4c\xcc\xad\0\0\x6c\xcc\xad\0\0\x4d\xcc\x81\0\0\x6d\xcc\x81\0\0"

      // Start of 0x1e40:
      u8"\x4d\xcc\x87\0\0\x6d\xcc\x87\0\0\x4d\xcc\xa3\0\0\x6d\xcc\xa3\0\0\x4e\xcc\x87\0\0\x6e\xcc\x87\0\0\x4e"
      u8"\xcc\xa3\0\0\x6e\xcc\xa3\0\0\x4e\xcc\xb1\0\0\x6e\xcc\xb1\0\0\x4e\xcc\xad\0\0\x6e\xcc\xad\0\0\x4f\xcc"
      u8"\x83\xcc\x81\x6f\xcc\x83\xcc\x81\x4f\xcc\x83\xcc\x88\x6f\xcc\x83\xcc\x88\x4f\xcc\x84\xcc\x80\x6f\xcc"
      u8"\x84\xcc\x80\x4f\xcc\x84\xcc\x81\x6f\xcc\x84\xcc\x81\x50\xcc\x81\0\0\x70\xcc\x81\0\0\x50\xcc\x87\0\0"
      u8"\x70\xcc\x87\0\0\x52\xcc\x87\0\0\x72\xcc\x87\0\0\x52\xcc\xa3\0\0\x72\xcc\xa3\0\0\x52\xcc\xa3\xcc\x84"
      u8"\x72\xcc\xa3\xcc\x84\x52\xcc\xb1\0\0\x72\xcc\xb1\0\0\x53\xcc\x87\0\0\x73\xcc\x87\0\0\x53\xcc\xa3\0\0"
      u8"\x73\xcc\xa3\0\0\x53\xcc\x81\xcc\x87\x73\xcc\x81\xcc\x87\x53\xcc\x8c\xcc\x87\x73\xcc\x8c\xcc\x87\x53"
      u8"\xcc\xa3\xcc\x87\x73\xcc\xa3\xcc\x87\x54\xcc\x87\0\0\x74\xcc\x87\0\0\x54\xcc\xa3\0\0\x74\xcc\xa3\0\0"
      u8"\x54\xcc\xb1\0\0\x74\xcc\xb1\0\0\x54\xcc\xad\0\0\x74\xcc\xad\0\0\x55\xcc\xa4\0\0\x75\xcc\xa4\0\0\x55"
      u8"\xcc\xb0\0\0\x75\xcc\xb0\0\0\x55\xcc\xad\0\0\x75\xcc\xad\0\0\x55\xcc\x83\xcc\x81\x75\xcc\x83\xcc\x81"
      u8"\x55\xcc\x84\xcc\x88\x75\xcc\x84\xcc\x88\x56\xcc\x83\0\0\x76\xcc\x83\0\0\x56\xcc\xa3\0\0\x76\xcc\xa3"
      u8"\0\0"

      // Start of 0x1e80:
      u8"\x57\xcc\x80\0\0\x77\xcc\x80\0\0\x57\xcc\x81\0\0\x77\xcc\x81\0\0\x57\xcc\x88\0\0\x77\xcc\x88\0\0\x57"
      u8"\xcc\x87\0\0\x77\xcc\x87\0\0\x57\xcc\xa3\0\0\x77\xcc\xa3\0\0\x58\xcc\x87\0\0\x78\xcc\x87\0\0\x58\xcc"
      u8"\x88\0\0\x78\xcc\x88\0\0\x59\xcc\x87\0\0\x79\xcc\x87\0\0\x5a\xcc\x82\0\0\x7a\xcc\x82\0\0\x5a\xcc\xa3"
      u8"\0\0\x7a\xcc\xa3\0\0\x5a\xcc\xb1\0\0\x7a\xcc\xb1\0\0\x68\xcc\xb1\0\0\x74\xcc\x88\0\0\x77\xcc\x8a\0\0"
      u8"\x79\xcc\x8a\0\0\0\0\0\0\0\xc5\xbf\xcc\x87\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x41\xcc\xa3\0\0"
      u8"\x61\xcc\xa3\0\0\x41\xcc\x89\0\0\x61\xcc\x89\0\0\x41\xcc\x82\xcc\x81\x61\xcc\x82\xcc\x81\x41\xcc\x82"
      u8"\xcc\x80\x61\xcc\x82\xcc\x80\x41\xcc\x82\xcc\x89\x61\xcc\x82\xcc\x89\x41\xcc\x82\xcc\x83\x61\xcc\x82"
      u8"\xcc\x83\x41\xcc\xa3\xcc\x82\x61\xcc\xa3\xcc\x82\x41\xcc\x86\xcc\x81\x61\xcc\x86\xcc\x81\x41\xcc\x86"
      u8"\xcc\x80\x61\xcc\x86\xcc\x80\x41\xcc\x86\xcc\x89\x61\xcc\x86\xcc\x89\x41\xcc\x86\xcc\x83\x61\xcc\x86"
      u8"\xcc\x83\x41\xcc\xa3\xcc\x86\x61\xcc\xa3\xcc\x86\x45\xcc\xa3\0\0\x65\xcc\xa3\0\0\x45\xcc\x89\0\0\x65"
      u8"\xcc\x89\0\0\x45\xcc\x83\0\0\x65\xcc\x83\0\0\x45\xcc\x82\xcc\x81\x65\xcc\x82\xcc\x81"

      // Start of 0x1ec0:
      u8"\x45\xcc\x82\xcc\x80\x65\xcc\x82\xcc\x80\x45\xcc\x82\xcc\x89\x65\xcc\x82\xcc\x89\x45\xcc\x82\xcc\x83"
      u8"\x65\xcc\x82\xcc\x83\x45\xcc\xa3\xcc\x82\x65\xcc\xa3\xcc\x82\x49\xcc\x89\0\0\x69\xcc\x89\0\0\x49\xcc"
      u8"\xa3\0\0\x69\xcc\xa3\0\0\x4f\xcc\xa3\0\0\x6f\xcc\xa3\0\0\x4f\xcc\x89\0\0\x6f\xcc\x89\0\0\x4f\xcc\x82"
      u8"\xcc\x81\x6f\xcc\x82\xcc\x81\x4f\xcc\x82\xcc\x80\x6f\xcc\x82\xcc\x80\x4f\xcc\x82\xcc\x89\x6f\xcc\x82"
      u8"\xcc\x89\x4f\xcc\x82\xcc\x83\x6f\xcc\x82\xcc\x83\x4f\xcc\xa3\xcc\x82\x6f\xcc\xa3\xcc\x82\x4f\xcc\x9b"
      u8"\xcc\x81\x6f\xcc\x9b\xcc\x81\x4f\xcc\x9b\xcc\x80\x6f\xcc\x9b\xcc\x80\x4f\xcc\x9b\xcc\x89\x6f\xcc\x9b"
      u8"\xcc\x89\x4f\xcc\x9b\xcc\x83\x6f\xcc\x9b\xcc\x83\x4f\xcc\x9b\xcc\xa3\x6f\xcc\x9b\xcc\xa3\x55\xcc\xa3"
      u8"\0\0\x75\xcc\xa3\0\0\x55\xcc\x89\0\0\x75\xcc\x89\0\0\x55\xcc\x9b\xcc\x81\x75\xcc\x9b\xcc\x81\x55\xcc"
      u8"\x9b\xcc\x80\x75\xcc\x9b\xcc\x80\x55\xcc\x9b\xcc\x89\x75\xcc\x9b\xcc\x89\x55\xcc\x9b\xcc\x83\x75\xcc"
      u8"\x9b\xcc\x83\x55\xcc\x9b\xcc\xa3\x75\xcc\x9b\xcc\xa3\x59\xcc\x80\0\0\x79\xcc\x80\0\0\x59\xcc\xa3\0\0"
      u8"\x79\xcc\xa3\0\0\x59\xcc\x89\0\0\x79\xcc\x89\0\0\x59\xcc\x83\0\0\x79\xcc\x83\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1f00:
      u8"\xce\xb1\xcc\x93\0\0\xce\xb1\xcc\x94\0\0\xce\xb1\xcc\x93\xcc\x80\xce\xb1\xcc\x94\xcc\x80\xce\xb1\xcc"
      u8"\x93\xcc\x81\xce\xb1\xcc\x94\xcc\x81\xce\xb1\xcc\x93\xcd\x82\xce\xb1\xcc\x94\xcd\x82\xce\x91\xcc\x93"
      u8"\0\0\xce\x91\xcc\x94\0\0\xce\x91\xcc\x93\xcc\x80\xce\x91\xcc\x94\xcc\x80\xce\x91\xcc\x93\xcc\x81\xce"
      u8"\x91\xcc\x94\xcc\x81\xce\x91\xcc\x93\xcd\x82\xce\x91\xcc\x94\xcd\x82\xce\xb5\xcc\x93\0\0\xce\xb5\xcc"
      u8"\x94\0\0\xce\xb5\xcc\x93\xcc\x80\xce\xb5\xcc\x94\xcc\x80\xce\xb5\xcc\x93\xcc\x81\xce\xb5\xcc\x94\xcc"
      u8"\x81\0\0\0\0\0\0\0\0\0\0\0\0\xce\x95\xcc\x93\0\0\xce\x95\xcc\x94\0\0\xce\x95\xcc\x93\xcc\x80\xce\x95"
      u8"\xcc\x94\xcc\x80\xce\x95\xcc\x93\xcc\x81\xce\x95\xcc\x94\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\xce\xb7\xcc"
      u8"\x93\0\0\xce\xb7\xcc\x94\0\0\xce\xb7\xcc\x93\xcc\x80\xce\xb7\xcc\x94\xcc\x80\xce\xb7\xcc\x93\xcc\x81"
      u8"\xce\xb7\xcc\x94\xcc\x81\xce\xb7\xcc\x93\xcd\x82\xce\xb7\xcc\x94\xcd\x82\xce\x97\xcc\x93\0\0\xce\x97"
      u8"\xcc\x94\0\0\xce\x97\xcc\x93\xcc\x80\xce\x97\xcc\x94\xcc\x80\xce\x97\xcc\x93\xcc\x81\xce\x97\xcc\x94"
      u8"\xcc\x81\xce\x97\xcc\x93\xcd\x82\xce\x97\xcc\x94\xcd\x82\xce\xb9\xcc\x93\0\0\xce\xb9\xcc\x94\0\0\xce"
      u8"\xb9\xcc\x93\xcc\x80\xce\xb9\xcc\x94\xcc\x80\xce\xb9\xcc\x93\xcc\x81\xce\xb9\xcc\x94\xcc\x81\xce\xb9"
      u8"\xcc\x93\xcd\x82\xce\xb9\xcc\x94\xcd\x82\xce\x99\xcc\x93\0\0\xce\x99\xcc\x94\0\0\xce\x99\xcc\x93\xcc"
      u8"\x80\xce\x99\xcc\x94\xcc\x80\xce\x99\xcc\x93\xcc\x81\xce\x99\xcc\x94\xcc\x81\xce\x99\xcc\x93\xcd\x82"
      u8"\xce\x99\xcc\x94\xcd\x82"

      // Start of 0x1f40:
      u8"\xce\xbf\xcc\x93\0\0\xce\xbf\xcc\x94\0\0\xce\xbf\xcc\x93\xcc\x80\xce\xbf\xcc\x94\xcc\x80\xce\xbf\xcc"
      u8"\x93\xcc\x81\xce\xbf\xcc\x94\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\xce\x9f\xcc\x93\0\0\xce\x9f\xcc\x94\0\0"
      u8"\xce\x9f\xcc\x93\xcc\x80\xce\x9f\xcc\x94\xcc\x80\xce\x9f\xcc\x93\xcc\x81\xce\x9f\xcc\x94\xcc\x81\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xcf\x85\xcc\x93\0\0\xcf\x85\xcc\x94\0\0\xcf\x85\xcc\x93\xcc\x80\xcf\x85\xcc\x94"
      u8"\xcc\x80\xcf\x85\xcc\x93\xcc\x81\xcf\x85\xcc\x94\xcc\x81\xcf\x85\xcc\x93\xcd\x82\xcf\x85\xcc\x94\xcd"
      u8"\x82\0\0\0\0\0\0\xce\xa5\xcc\x94\0\0\0\0\0\0\0\0\xce\xa5\xcc\x94\xcc\x80\0\0\0\0\0\0\xce\xa5\xcc\x94"
      u8"\xcc\x81\0\0\0\0\0\0\xce\xa5\xcc\x94\xcd\x82\xcf\x89\xcc\x93\0\0\xcf\x89\xcc\x94\0\0\xcf\x89\xcc\x93"
      u8"\xcc\x80\xcf\x89\xcc\x94\xcc\x80\xcf\x89\xcc\x93\xcc\x81\xcf\x89\xcc\x94\xcc\x81\xcf\x89\xcc\x93\xcd"
      u8"\x82\xcf\x89\xcc\x94\xcd\x82\xce\xa9\xcc\x93\0\0\xce\xa9\xcc\x94\0\0\xce\xa9\xcc\x93\xcc\x80\xce\xa9"
      u8"\xcc\x94\xcc\x80\xce\xa9\xcc\x93\xcc\x81\xce\xa9\xcc\x94\xcc\x81\xce\xa9\xcc\x93\xcd\x82\xce\xa9\xcc"
      u8"\x94\xcd\x82\xce\xb1\xcc\x80\0\0\xce\xb1\xcc\x81\0\0\xce\xb5\xcc\x80\0\0\xce\xb5\xcc\x81\0\0\xce\xb7"
      u8"\xcc\x80\0\0\xce\xb7\xcc\x81\0\0\xce\xb9\xcc\x80\0\0\xce\xb9\xcc\x81\0\0\xce\xbf\xcc\x80\0\0\xce\xbf"
      u8"\xcc\x81\0\0\xcf\x85\xcc\x80\0\0\xcf\x85\xcc\x81\0\0\xcf\x89\xcc\x80\0\0\xcf\x89\xcc\x81\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0"

      // Start of 0x1f80:
      u8"\xce\xb1\xcc\x93\xcd\x85\0\0\xce\xb1\xcc\x94\xcd\x85\0\0\xce\xb1\xcc\x93\xcc\x80\xcd\x85\xce\xb1\xcc"
      u8"\x94\xcc\x80\xcd\x85\xce\xb1\xcc\x93\xcc\x81\xcd\x85\xce\xb1\xcc\x94\xcc\x81\xcd\x85\xce\xb1\xcc\x93"
      u8"\xcd\x82\xcd\x85\xce\xb1\xcc\x94\xcd\x82\xcd\x85\xce\x91\xcc\x93\xcd\x85\0\0\xce\x91\xcc\x94\xcd\x85"
      u8"\0\0\xce\x91\xcc\x93\xcc\x80\xcd\x85\xce\x91\xcc\x94\xcc\x80\xcd\x85\xce\x91\xcc\x93\xcc\x81\xcd\x85"
      u8"\xce\x91\xcc\x94\xcc\x81\xcd\x85\xce\x91\xcc\x93\xcd\x82\xcd\x85\xce\x91\xcc\x94\xcd\x82\xcd\x85\xce"
      u8"\xb7\xcc\x93\xcd\x85\0\0\xce\xb7\xcc\x94\xcd\x85\0\0\xce\xb7\xcc\x93\xcc\x80\xcd\x85\xce\xb7\xcc\x94"
      u8"\xcc\x80\xcd\x85\xce\xb7\xcc\x93\xcc\x81\xcd\x85\xce\xb7\xcc\x94\xcc\x81\xcd\x85\xce\xb7\xcc\x93\xcd"
      u8"\x82\xcd\x85\xce\xb7\xcc\x94\xcd\x82\xcd\x85\xce\x97\xcc\x93\xcd\x85\0\0\xce\x97\xcc\x94\xcd\x85\0\0"
      u8"\xce\x97\xcc\x93\xcc\x80\xcd\x85\xce\x97\xcc\x94\xcc\x80\xcd\x85\xce\x97\xcc\x93\xcc\x81\xcd\x85\xce"
      u8"\x97\xcc\x94\xcc\x81\xcd\x85\xce\x97\xcc\x93\xcd\x82\xcd\x85\xce\x97\xcc\x94\xcd\x82\xcd\x85\xcf\x89"
      u8"\xcc\x93\xcd\x85\0\0\xcf\x89\xcc\x94\xcd\x85\0\0\xcf\x89\xcc\x93\xcc\x80\xcd\x85\xcf\x89\xcc\x94\xcc"
      u8"\x80\xcd\x85\xcf\x89\xcc\x93\xcc\x81\xcd\x85\xcf\x89\xcc\x94\xcc\x81\xcd\x85\xcf\x89\xcc\x93\xcd\x82"
      u8"\xcd\x85\xcf\x89\xcc\x94\xcd\x82\xcd\x85\xce\xa9\xcc\x93\xcd\x85\0\0\xce\xa9\xcc\x94\xcd\x85\0\0\xce"
      u8"\xa9\xcc\x93\xcc\x80\xcd\x85\xce\xa9\xcc\x94\xcc\x80\xcd\x85\xce\xa9\xcc\x93\xcc\x81\xcd\x85\xce\xa9"
      u8"\xcc\x94\xcc\x81\xcd\x85\xce\xa9\xcc\x93\xcd\x82\xcd\x85\xce\xa9\xcc\x94\xcd\x82\xcd\x85\xce\xb1\xcc"
      u8"\x86\0\0\0\0\xce\xb1\xcc\x84\0\0\0\0\xce\xb1\xcc\x80\xcd\x85\0\0\xce\xb1\xcd\x85\0\0\0\0\xce\xb1\xcc"
      u8"\x81\xcd\x85\0\0\0\0\0\0\0\0\0\0\xce\xb1\xcd\x82\0\0\0\0\xce\xb1\xcd\x82\xcd\x85\0\0\xce\x91\xcc\x86"
      u8"\0\0\0\0\xce\x91\xcc\x84\0\0\0\0\xce\x91\xcc\x80\0\0\0\0\xce\x91\xcc\x81\0\0\0\0\xce\x91\xcd\x85\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xce\xb9\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1fc0:
      u8"\0\0\0\0\0\0\xc2\xa8\xcd\x82\0\0\xce\xb7\xcc\x80\xcd\x85\xce\xb7\xcd\x85\0\0\xce\xb7\xcc\x81\xcd\x85"
      u8"\0\0\0\0\0\0\xce\xb7\xcd\x82\0\0\xce\xb7\xcd\x82\xcd\x85\xce\x95\xcc\x80\0\0\xce\x95\xcc\x81\0\0\xce"
      u8"\x97\xcc\x80\0\0\xce\x97\xcc\x81\0\0\xce\x97\xcd\x85\0\0\xe1\xbe\xbf\xcc\x80\0\xe1\xbe\xbf\xcc\x81\0"
      u8"\xe1\xbe\xbf\xcd\x82\0\xce\xb9\xcc\x86\0\0\xce\xb9\xcc\x84\0\0\xce\xb9\xcc\x88\xcc\x80\xce\xb9\xcc"
      u8"\x88\xcc\x81\0\0\0\0\0\0\0\0\0\0\0\0\xce\xb9\xcd\x82\0\0\xce\xb9\xcc\x88\xcd\x82\xce\x99\xcc\x86\0\0"
      u8"\xce\x99\xcc\x84\0\0\xce\x99\xcc\x80\0\0\xce\x99\xcc\x81\0\0\0\0\0\0\0\0\xe1\xbf\xbe\xcc\x80\0\xe1"
      u8"\xbf\xbe\xcc\x81\0\xe1\xbf\xbe\xcd\x82\0\xcf\x85\xcc\x86\0\0\xcf\x85\xcc\x84\0\0\xcf\x85\xcc\x88\xcc"
      u8"\x80\xcf\x85\xcc\x88\xcc\x81\xcf\x81\xcc\x93\0\0\xcf\x81\xcc\x94\0\0\xcf\x85\xcd\x82\0\0\xcf\x85\xcc"
      u8"\x88\xcd\x82\xce\xa5\xcc\x86\0\0\xce\xa5\xcc\x84\0\0\xce\xa5\xcc\x80\0\0\xce\xa5\xcc\x81\0\0\xce\xa1"
      u8"\xcc\x94\0\0\xc2\xa8\xcc\x80\0\0\xc2\xa8\xcc\x81\0\0\x60\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xcf\x89"
      u8"\xcc\x80\xcd\x85\xcf\x89\xcd\x85\0\0\xcf\x89\xcc\x81\xcd\x85\0\0\0\0\0\0\xcf\x89\xcd\x82\0\0\xcf\x89"
      u8"\xcd\x82\xcd\x85\xce\x9f\xcc\x80\0\0\xce\x9f\xcc\x81\0\0\xce\xa9\xcc\x80\0\0\xce\xa9\xcc\x81\0\0\xce"
      u8"\xa9\xcd\x85\0\0\xc2\xb4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2000:
      u8"\xe2\x80\x82\xe2\x80\x83\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2100:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xce\xa9\0\0\0\0\0\0\0\0\0\0\x4b\0\0\x41\xcc\x8a\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2180:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x86\x90\xcc\xb8\xe2\x86\x92\xcc\xb8"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x86\x94\xcc\xb8"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x21c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x87\x90\xcc\xb8\xe2\x87\x94\xcc\xb8\xe2\x87\x92\xcc\xb8\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2200:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88\x83\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\xe2\x88\x88\xcc\xb8\0\0\0\0\0\0\0\0\0\0\xe2\x88\x8b\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x88"
      u8"\xa3\xcc\xb8\0\0\0\0\0\xe2\x88\xa5\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

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
      u8"\xe2\x8a\xa2\xcc\xb8\xe2\x8a\xa8\xcc\xb8\xe2\x8a\xa9\xcc\xb8\xe2\x8a\xab\xcc\xb8\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x22c0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xe2\x89\xbc\xcc\xb8\xe2\x89\xbd\xcc\xb8\xe2\x8a\x91\xcc\xb8\xe2\x8a\x92\xcc\xb8"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\x8a\xb2\xcc\xb8\xe2\x8a\xb3\xcc\xb8"
      u8"\xe2\x8a\xb4\xcc\xb8\xe2\x8a\xb5\xcc\xb8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2300:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x80\x88\xe3\x80\x89\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x2ac0:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe2\xab\x9d\xcc\xb8"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0"

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
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x82\x9d\xe3\x82\x99\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe3\x82\xab\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xad"
      u8"\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xaf\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xb1\xe3\x82\x99\0\0\0\0\0\0\xe3"
      u8"\x82\xb3\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xb5\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xb7\xe3\x82\x99\0\0\0\0"
      u8"\0\0\xe3\x82\xb9\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xbb\xe3\x82\x99\0\0\0\0\0\0\xe3\x82\xbd\xe3\x82\x99"
      u8"\0\0\0\0\0\0"

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
      u8"\x82\x99\0\0\0\0\0\0"

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
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0xfb00:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd7\x99\xd6\xb4\0\0\0\0\0\0\0\0\xd7\xb2\xd6\xb7\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xd7\xa9\xd7\x81\0\0\xd7\xa9\xd7\x82\0\0\xd7\xa9\xd6\xbc\xd7\x81\xd7\xa9\xd6\xbc"
      u8"\xd7\x82\xd7\x90\xd6\xb7\0\0\xd7\x90\xd6\xb8\0\0\xd7\x90\xd6\xbc\0\0\xd7\x91\xd6\xbc\0\0\xd7\x92\xd6"
      u8"\xbc\0\0\xd7\x93\xd6\xbc\0\0\xd7\x94\xd6\xbc\0\0\xd7\x95\xd6\xbc\0\0\xd7\x96\xd6\xbc\0\0\0\0\0\0\0\0"
      u8"\xd7\x98\xd6\xbc\0\0\xd7\x99\xd6\xbc\0\0\xd7\x9a\xd6\xbc\0\0\xd7\x9b\xd6\xbc\0\0\xd7\x9c\xd6\xbc\0\0"
      u8"\0\0\0\0\0\0\xd7\x9e\xd6\xbc\0\0\0\0\0\0\0\0"

      // Start of 0xfb40:
      u8"\xd7\xa0\xd6\xbc\xd7\xa1\xd6\xbc\0\0\0\0\xd7\xa3\xd6\xbc\xd7\xa4\xd6\xbc\0\0\0\0\xd7\xa6\xd6\xbc\xd7"
      u8"\xa7\xd6\xbc\xd7\xa8\xd6\xbc\xd7\xa9\xd6\xbc\xd7\xaa\xd6\xbc\xd7\x95\xd6\xb9\xd7\x91\xd6\xbf\xd7\x9b"
      u8"\xd6\xbf\xd7\xa4\xd6\xbf\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x11080:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\xf0\x91\x82\x99\xf0\x91\x82\xba\0\0\0\0\0\0\0\0\xf0\x91\x82\x9b\xf0\x91\x82\xba\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xf0\x91\x82\xa5\xf0\x91\x82\xba\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x11100:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0\x91\x84\xb1\xf0\x91\x84\xa7\xf0\x91\x84\xb2\xf0\x91\x84\xa7"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x11340:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xf0\x91\x8d\x87\xf0\x91"
      u8"\x8c\xbe\xf0\x91\x8d\x87\xf0\x91\x8d\x97\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

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
      u8"\x91\xa4\xb5\xf0\x91\xa4\xb0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

      // Start of 0x1d140:
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\xf0\x9d\x85\x97\xf0\x9d\x85\xa5\0\0\0\0\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\0\0\0\0"
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\xf0"
      u8"\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb0\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb1\xf0\x9d"
      u8"\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

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
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
      u8"\0\0\0\0\0\0\0\0\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\0\0\0\0\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\0\0\0\0\xf0"
      u8"\x9d\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\xf0\x9d"
      u8"\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf"

      // Start of 0x1d1c0:
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
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
      u8"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

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
      u8"\x96\0\xe9\xbc\xbb\0\xf0\xaa\x98\x80",
      // done.
      26393UL // String Length
    };

    template <typename CharT = char8_t, typename CPType>
        requires(sizeof(CharT) == sizeof(char8_t))
    [[nodiscard]] static constexpr CharT const* decomp_ptr(
      decomp_index const code, CPType const code_point) noexcept {
        if constexpr (std::same_as<CharT, char8_t>) {
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
