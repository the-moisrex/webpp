
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_idna_mapping_tables3.mjs
 *   Unicode UCD Database Creation Date: 2024-08-25
 *   This file's generation date:        Tue, 22 Apr 2025 20:37:42 GMT
 *   Unicode Version:                    16.0.0
 *   Total Table sizes in this file:
 *       - in bits:       348016
 *       - in bytes:      43502 B
 *       - in KibiBytes:  42.48 KiB
 *   Some other implementations' total table size was 84.47 KiB;
 *   So I have saved 41.99 KiB.
 *
 * Details about the contents of this file can be found here:
 *   UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 * IDNA Mapping Tables are derived from here:
 *   https://www.unicode.org/Public/idna/latest/IdnaMappingTable.txt
 * UCD README file (used to check the version and creation date):
 *   https://www.unicode.org/Public/UCD/latest/ucd/ReadMe.txt
 */

#ifndef WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP
#define WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP

#include <array>
#include <cstdint>
#include <string_view>

namespace webpp::unicode::details {

    static constexpr auto last_disallowed = static_cast<char32_t>(0x110020);

    [[maybe_unused]] static constexpr std::uint16_t not_mapped = 0b1000'0000'0000'0000U;
    static constexpr std::uint16_t                  valid      = 0b1000'0000'0000'0011U;
    static constexpr std::uint16_t                  disallowed = 0b1000'0000'0000'0010U;



    /**
     * The last code point that has a mapping status:
     */
    static constexpr auto idna_mapping_trailing_zero = 0x110020UL;

    // Pick the table with this mask (between bools table and the block table)
    static constexpr std::uint16_t table_pick_mask = 0b1000'0000'0000'0000U;
    static constexpr auto          iblt            = table_pick_mask; // (IDNA Boolean Table) shortcut

    /**
     * Idna (Index Table)
     * IDNA Mapping Index table
     */
    struct alignas(std::uint16_t) idna_index {
        /// The shifts required to extract the values out of a std::uint16_t; you can use masks as well:
        static constexpr std::uint8_t use_second_table_shift = 15U;
        static constexpr std::uint8_t pos_shift              = 0U;

        /// The masks required to extracting the values out of a std::uint16_t; you can use shifts as well:
        static constexpr std::uint16_t use_second_table_mask = 0x8000U;
        static constexpr std::uint16_t pos_mask              = 0x7FFFU;

        // NOLINTBEGIN(*-non-private-member-variables-in-classes)

        /// Which values table should we choose will be decided by this field.
        bool use_second_table : 1 = false;

        /// This is the position that should be looked for in the values table.
        std::uint16_t pos     : 15 = 0;

        // NOLINTEND(*-non-private-member-variables-in-classes)

        /**
         * [1bits = use_second_table] + [15bits = pos]
         */
        explicit(false) consteval idna_index(std::uint16_t const value) noexcept :
          use_second_table{static_cast<bool>((value & use_second_table_mask) >> use_second_table_shift)},
          pos{static_cast<std::uint16_t>(value)} {}

        // NOLINTNEXTLINE(*-easily-swappable-parameters)
        explicit consteval idna_index(bool const inp_use_second_table, std::uint16_t const inp_pos) noexcept :
          use_second_table{inp_use_second_table}, pos{inp_pos} {}

        [[nodiscard]] constexpr std::uint16_t value() const noexcept {
            return static_cast<std::uint16_t>(pos) |
                   (static_cast<std::uint16_t>(use_second_table) << use_second_table_shift);
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

    struct idna_breakpoint_type {
        std::uint16_t starting;
        std::uint16_t ending;
        std::uint16_t offset;
        idna_index    common_value; // if it's not in the specified range, this value should be used.
    };

    /**
     * You can choose between the indices' table using these breakpoints:
     *
     * Table size in KibiBytes:  4.25 KiB
     */
    static constexpr std::array<idna_breakpoint_type, 68U> idna_breakpoints{
      idna_breakpoint_type{    .starting = 0,   .ending = 416,     .offset = 0, .common_value = iblt | 0x0U}, // Section
  // 1
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      { .starting = 1329,  .ending = 1374,   .offset = 913, .common_value = iblt | 0xAU}, // Section 2
      { .starting = 1991,  .ending = 2246,  .offset = 1530, .common_value = iblt | 0xAU}, // Section 3
      { .starting = 1991,  .ending = 2246,  .offset = 1530, .common_value = iblt | 0xAU}, // Section 3
      { .starting = 2929,  .ending = 2931,  .offset = 2213, .common_value = iblt | 0xAU}, // Section 4
      { .starting = 3556,  .ending = 4096,  .offset = 2838, .common_value = iblt | 0xAU}, // Section 5
      { .starting = 3556,  .ending = 4096,  .offset = 2838, .common_value = iblt | 0xAU}, // Section 5
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      { .starting = 5430,  .ending = 6430,  .offset = 4172, .common_value = iblt | 0x0U}, // Section 6
      { .starting = 5430,  .ending = 6430,  .offset = 4172, .common_value = iblt | 0x0U}, // Section 6
      { .starting = 5430,  .ending = 6430,  .offset = 4172, .common_value = iblt | 0x0U}, // Section 6
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {.starting = 28679, .ending = 28688, .offset = 26421, .common_value = iblt | 0x0U}, // Section 7
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {    .starting = 0,     .ending = 0,     .offset = 0, .common_value = iblt | 0x0U}, // Section Optimized Away
      {.starting = 34815, .ending = 34816, .offset = 32548, .common_value = iblt | 0x0U}, // Section 8
    };

    static constexpr std::uint16_t idna_last_breakpoint{0x8800U};
    static constexpr std::uint16_t idna_breakpoint_shift{9U};



    /**
     * idna Index Table (combined 8 sections)
     *
     * IDNA Mappings
     *
     * Each value contains 2 numbers hidden inside:
     *     [1bits = use_second_table] + [15bits = pos]
     *
     * Table size:
     *   - in bits:       36288
     *   - in bytes:      4536 B
     *   - in KibiBytes:  4.43 KiB
     */
    static constexpr std::array<idna_index, 2268ULL> idna_mapping_ref{
      iblt | 0x0U, iblt | 0x0U, 0x0U, iblt | 0x1U, iblt | 0x0U, 0x20U, 0x40U, iblt | 0x2U, 0x60U, 0x80U,
      0x9FU, 0xBFU, 0xDFU, 0xFFU, 0x11CU, 0x13CU, 0x15CU, 0x17CU, 0x19BU, iblt | 0x3U, iblt | 0x3U, 0x1ABU,
      0x1C4U, 0x1E4U, iblt | 0x4U, iblt | 0x4U, 0x204U, 0x214U, 0x234U, 0x254U, 0x265U, 0x285U, 0x2A5U,
      0x2C5U, iblt | 0x5U, 0x2E5U, 0x305U, 0x325U, 0x345U, 0x365U, 0x385U, 0x3A5U, 0x3C5U, iblt | 0x6U,
      0x3DEU, iblt | 0x7U, iblt | 0x8U, iblt | 0x9U, iblt | 0x0U, iblt | 0x7U, iblt | 0x7U, 0x3EFU,
      iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U,
      iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0x6U,
      iblt | 0x6U, iblt | 0x6U, iblt | 0x6U, iblt | 0xAU, iblt | 0x6U, iblt | 0x6U, 0x408U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, 0x428U, iblt | 0x0U, iblt | 0x0U, 0x448U, 0x466U, iblt | 0x2U, iblt | 0x2U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x2U, iblt | 0x0U, 0x486U, iblt | 0x2U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x2U, iblt | 0x0U, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U,
      iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0xAU,
      iblt | 0x2U, iblt | 0x2U, iblt | 0x0U, iblt | 0xAU, 0x4A5U, iblt | 0x2U, iblt | 0x0U, iblt | 0xAU,
      0x4C4U, 0x4E4U, iblt | 0x0U, 0x502U, iblt | 0xBU, 0x51FU, 0x53CU, 0x55BU, 0x579U, iblt | 0xAU,
      iblt | 0x0U, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, 0x599U, 0x5B9U, 0x5C9U,
      iblt | 0xCU, iblt | 0xCU, 0x5E6U, iblt | 0x2U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x606U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x2U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x626U, iblt | 0xDU, iblt | 0xDU,
      0x63CU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x5U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x5U, iblt | 0x0U, iblt | 0x0U, iblt | 0xEU,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x65CU, 0x67CU, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, 0x69CU, 0x6BCU, 0x6DCU, 0x6F5U, 0x715U, iblt | 0x0U, iblt | 0x0U, 0x735U, 0x755U, 0x775U,
      0x795U, 0x7B5U, 0x7D5U, 0x7F5U, 0x815U, 0x834U, 0x854U, 0x874U, 0x894U, 0x8B4U, 0x8D4U, 0x8F4U, 0x914U,
      0x934U, 0x950U, 0x96FU, 0x98FU, 0x9AFU, 0x9CFU, iblt | 0x2U, iblt | 0x3U, 0x9EFU, 0xA0FU, 0xA2FU,
      0xA4FU, 0xA6FU, iblt | 0x5U, iblt | 0x5U, iblt | 0x5U, iblt | 0x5U, 0xA83U, iblt | 0x7U, iblt | 0x7U,
      iblt | 0x7U, iblt | 0x7U, iblt | 0x7U, iblt | 0x7U, iblt | 0x7U, 0xA9AU, iblt | 0xFU, iblt | 0xFU,
      iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xDU, iblt | 0x9U, 0xABAU,
      0xADAU, 0xAFAU, 0xB1AU, 0xB3AU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU,
      iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU,
      iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU,
      iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU,
      iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, iblt | 0xFU,
      iblt | 0xFU, iblt | 0xFU, iblt | 0xFU, 0xB4EU, iblt | 0xBU, iblt | 0xBU, 0xB5BU, iblt | 0x10U,
      iblt | 0x10U, 0xB72U, iblt | 0xCU, iblt | 0xCU, iblt | 0xCU, iblt | 0xCU, iblt | 0xCU, iblt | 0xCU,
      iblt | 0xCU, iblt | 0xCU, iblt | 0xCU, 0xB92U, 0xBB2U, iblt | 0x5U, 0xBD2U, 0xBF2U, 0xC12U, 0xC32U,
      0xC52U, iblt | 0x6U, iblt | 0xEU, iblt | 0x6U, 0xC6BU, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U,
      iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0x2U, iblt | 0x0U, 0xC8AU, iblt | 0x0U, iblt | 0x0U,
      0xCAAU, 0xCCAU, 0xCEAU, 0xD0AU, 0xD2AU, 0xD4AU, 0xD6AU, 0xD8AU, iblt | 0xDU, 0xDAAU, 0xDB4U,
      iblt | 0x0U, iblt | 0x1U, 0xDCFU, iblt | 0x0U, iblt | 0x0U, 0xDEFU, iblt | 0x0U, 0xE0FU, 0xE2FU, 0xE4FU,
      0xE6FU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0xE8FU, 0xEAFU, 0xECFU, 0xEEFU, 0xF0FU, 0xF2FU, 0xF4FU,
      0xF6FU, 0xF8FU, 0xFAFU, 0xFCFU, 0xFEFU, 0x100FU, 0x102FU, 0x104FU, 0x106FU,
      // End of Section #1 [0, 416) containing 416 values (0.81 KiB). Skipping 913 values.

      iblt | 0x0U, 0x108FU, 0x10AFU, 0x10CFU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, 0x10EDU,
      0x110DU, 0x112DU, 0x114DU, 0x116DU, 0x118DU, 0x11AAU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xCU, iblt | 0xEU, iblt | 0x0U, iblt | 0xEU, 0x11C4U,
      0x11E4U, 0x1204U, 0x1224U,
      // End of Section #2 [1329, 1374) containing 45 values (0.09 KiB). Skipping 617 values.

      iblt | 0x0U, 0x1244U, 0x1264U, 0x1284U, 0x12A4U, 0x12C4U, 0x12E4U, 0x1304U, 0x1324U, 0x1344U, 0x1364U,
      0x1384U, 0x13A4U, 0x13C4U, 0x13E4U, 0x1404U, iblt | 0xEU, 0x1424U, 0x1444U, 0x1464U, 0x1482U, 0x14A0U,
      0x14C0U, 0x14DDU, 0x14FDU, 0x151DU, 0x153DU, 0x155DU, 0x157DU, 0x159DU, 0x15BDU, 0x15DDU, 0x15FDU,
      0x161DU, 0x163DU, 0x165BU, 0x167AU, 0x169AU, 0x16BAU, 0x16DAU, 0x16EAU, 0x170AU, 0x172AU, 0x174AU,
      0x176AU, 0x178AU, 0x17A9U, 0x17C8U, 0x17E7U, 0x1807U, 0x1827U, 0x1847U, 0x1867U, 0x1887U, 0x18A7U,
      0x18C6U, 0x18E6U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x3U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x5U, iblt | 0x0U, iblt | 0x3U, iblt | 0x3U, iblt | 0x3U, iblt | 0x3U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x3U, 0x1906U, 0x1926U, iblt | 0x11U, iblt | 0x11U, iblt | 0x11U, 0x193CU, 0x195CU,
      iblt | 0x8U, iblt | 0x8U, iblt | 0x8U, iblt | 0x8U, 0x1978U, 0x1998U, iblt | 0xAU, iblt | 0x10U,
      iblt | 0x10U, iblt | 0x10U, iblt | 0x10U, iblt | 0x10U, iblt | 0x10U, iblt | 0x10U, iblt | 0x10U,
      iblt | 0x10U, iblt | 0x10U, iblt | 0x10U, iblt | 0x10U, iblt | 0x10U, iblt | 0x8U, 0x19B7U, 0x19D7U,
      iblt | 0x1U, iblt | 0x1U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1U,
      iblt | 0x1U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1U, iblt | 0x1U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1U,
      iblt | 0x1U, iblt | 0x1U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1U, 0x19F7U, 0x1A17U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x1A37U, 0x1A57U, iblt | 0xEU, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x12U, iblt | 0x12U,
      iblt | 0xDU, iblt | 0x0U, iblt | 0x0U, iblt | 0x12U, iblt | 0x8U, iblt | 0x12U, iblt | 0x0U,
      iblt | 0xDU, iblt | 0x0U, iblt | 0x13U, iblt | 0x12U, iblt | 0x12U, iblt | 0x12U, iblt | 0x14U,
      iblt | 0x15U, iblt | 0x12U, iblt | 0x12U, iblt | 0xCU, iblt | 0x10U, iblt | 0x12U, iblt | 0xFU,
      iblt | 0x8U, iblt | 0x12U, iblt | 0x12U, iblt | 0x12U, iblt | 0x12U, iblt | 0x0U, iblt | 0x16U,
      iblt | 0x12U, iblt | 0xAU, iblt | 0x0U, iblt | 0x6U, iblt | 0xDU, iblt | 0x12U, iblt | 0x9U,
      iblt | 0x17U, iblt | 0x10U, iblt | 0x1U, iblt | 0x17U, iblt | 0xDU, iblt | 0x15U, iblt | 0x2U,
      iblt | 0x0U, iblt | 0x12U, iblt | 0x12U, iblt | 0x12U, iblt | 0xAU, iblt | 0x12U, iblt | 0x12U,
      iblt | 0x8U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x12U,
      iblt | 0x15U, iblt | 0x12U, iblt | 0x0U, iblt | 0x12U, iblt | 0x12U, iblt | 0x1U, iblt | 0x18U,
      iblt | 0x12U, iblt | 0x12U, iblt | 0xDU, iblt | 0x17U, iblt | 0x12U, iblt | 0x13U, iblt | 0x6U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x12U, iblt | 0x12U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x1A77U,
      // End of Section #3 [1991, 2246) containing 255 values (0.50 KiB). Skipping 683 values.

      iblt | 0x0U, 0x1A97U,
      // End of Section #4 [2929, 2931) containing 2 values (0.00 KiB). Skipping 625 values.

      iblt | 0x0U, 0x1AB7U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U,
      iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x19U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x1AD7U, 0x1AF7U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xEU, iblt | 0xEU, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xEU, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      0x1B17U, 0x1B37U, iblt | 0x1U, 0x1B52U, 0x1B72U, iblt | 0x9U, iblt | 0x1AU, iblt | 0x1AU, iblt | 0xEU,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1BU, iblt | 0x1BU, iblt | 0x1AU, iblt | 0x1AU,
      iblt | 0x12U, iblt | 0x1CU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1DU,
      iblt | 0x1DU, 0x1B92U, iblt | 0x1DU, 0x1BB2U, 0x1BD2U, 0x1BF2U, iblt | 0x1DU, 0x1C12U, 0x1C32U, 0x1C52U,
      iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, 0x1C72U,
      iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, 0x1C92U, 0x1CB2U, 0x1CD2U, iblt | 0x1DU, iblt | 0x1DU,
      iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, 0x1CF2U, 0x1D08U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x1D28U,
      0x1D48U, 0x1D68U, iblt | 0x7U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U,
      iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x1DU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U,
      iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U,
      iblt | 0x1DU, iblt | 0x0U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U,
      iblt | 0x16U, iblt | 0x1DU, iblt | 0x0U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U,
      iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U,
      iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x16U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x16U, 0x1D88U, 0x1DA8U,
      iblt | 0x13U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x1DU, iblt | 0x1EU, iblt | 0xFU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x1EU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x1DC8U,
      0x1DE8U, 0x1E06U, 0x1E26U, 0x1E46U, 0x1E65U, iblt | 0x17U, iblt | 0x17U, iblt | 0x17U, iblt | 0x17U,
      iblt | 0x17U, iblt | 0x17U, iblt | 0x17U, iblt | 0x17U, iblt | 0x17U, iblt | 0x17U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x2U, iblt | 0x0U, 0x1E84U,
      0x1EA4U, 0x1EC4U, 0x1EDAU, 0x1EEAU, iblt | 0x14U, iblt | 0x0U, iblt | 0x1DU, 0x1F0AU, 0x1F2AU, 0x1F4AU,
      iblt | 0x0U, iblt | 0x14U, iblt | 0x14U, iblt | 0x14U, iblt | 0x14U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x14U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x1F6AU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      // End of Section #5 [3556, 4096) containing 540 values (1.05 KiB). Skipping 1334 values.

      iblt | 0x0U, iblt | 0xEU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU, iblt | 0xEU,
      iblt | 0xEU, iblt | 0xEU, 0x1F8AU, 0x1FAAU, 0x1FCAU, 0x1FEAU, 0x200AU, 0x202AU, 0x204AU, 0x206AU,
      0x208AU, 0x20AAU, 0x20CAU, 0x20EAU, 0x210AU, 0x212AU, 0x214AU, 0x216AU, 0x218AU, iblt | 0xAU,
      iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU,
      iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU,
      iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU,
      iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU,
      iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU,
      iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0xAU, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      // End of Section #6 [5430, 6430) containing 1000 values (1.95 KiB). Skipping 22249 values.

      iblt | 0xAU, 0x21AAU, iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU, iblt | 0x1DU,
      iblt | 0x1DU, iblt | 0x5U,
      // End of Section #7 [28679, 28688) containing 9 values (0.02 KiB). Skipping 6127 values.

      iblt | 0x0U,
      // End of Section #8 [34815, 34816) containing 1 values (0.00 KiB).

      // done
    };




    /**
     * IDNA Values Table
     *
     * Block values of the IDNA Mappings; the values of this table points to the idna_mappings table if it's
     * not VALID or DISALLOWED specifically specified.
     *
     * Table size:
     *   - in bits:       138400
     *   - in bytes:      17300 B
     *   - in KibiBytes:  16.89 KiB
     */
    static constexpr std::array<std::uint16_t, 8650ULL> idna_mapping_blocks{

      // Start of 0x0-0x40, 0x80, 0x600, 0x960-0x9a0, 0x9e0-0xa00, 0xaa0-0xae0, 0xb20, 0xb80-0xba0, 0xbe0,
      // 0xde0, 0xe60, 0xee0, 0xfe0, 0x1180-0x13c0, 0x1400-0x1660, 0x16a0-0x1780, 0x1820-0x19a0,
      // 0x19e0-0x1a60, 0x1aa0-0x1ac0, 0x1b00-0x1c60, 0x1cc0-0x1d00, 0x1dc0-0x1de0, 0x2e60, 0x2ea0-0x2ec0,
      // 0x3040, 0x30a0-0x30c0, 0x3100, 0x31a0-0x31e0, 0x3400-0xa620, 0xab00, 0xabc0-0xf8e0, 0x10000-0x10040,
      // 0x10080-0x101a0, 0x101e0, 0x10280-0x103c0, 0x10800-0x10880, 0x108e0-0x10920, 0x10980-0x10a80,
      // 0x10ac0-0x10b80, 0x10c00-0x10c40, 0x10cc0-0x10d20, 0x10da0-0x10e40, 0x10ec0-0x10ee0, 0x10f60,
      // 0x10fa0, 0x111e0, 0x11260, 0x113e0, 0x114e0-0x11560, 0x115e0, 0x11760-0x117e0, 0x11840-0x11880,
      // 0x118c0-0x16e20, 0x16e60-0x1bc80, 0x1cc00-0x1cca0, 0x1cd00-0x1cea0, 0x1cf00-0x1cfc0, 0x1d000-0x1d120,
      // 0x1d260-0x1d2a0, 0x1d380-0x1d3e0, 0x1d800-0x1e000, 0x1e100-0x1e140, 0x1e2a0-0x1e2e0, 0x1e4e0,
      // 0x1e5e0, 0x1e7e0-0x1e8c0, 0x1e960-0x1ec40, 0x1ecc0-0x1ed00, 0x1ed40-0x1ede0, 0x1f000-0x1f0a0,
      // 0x1f0e0, 0x1f1c0, 0x1f260, 0x1f300-0x1f8c0, 0x1f900-0x1fbc0, 0x20000-0x2a6c0, 0x2a700-0x2ee40,
      // 0x30000-0x323a0, 0xe0200-0x110000:
      valid,

      // Start of 0x60, 0x3060, 0x107c0-0x107e0, 0x108a0-0x108c0, 0x10940-0x10960, 0x10aa0, 0x10ba0-0x10be0,
      // 0x10c60, 0x11340, 0x11640, 0x1d180:
      0,

      // Start of 0xe0, 0xa60-0xa80, 0xb00, 0xb60, 0xbc0, 0xc00-0xd60, 0xda0-0xdc0, 0xe40, 0x1160, 0x1680,
      // 0x20c0, 0x2d80-0x2e40, 0x113c0, 0x1f0c0:
      1,

      // Start of 0x260-0x280, 0x20e0, 0x10060, 0x10200-0x10260, 0x103e0:
      2,

      // Start of 0x300-0x320:
      3,

      // Start of 0x440, 0x19c0, 0x1a80, 0x21a0-0x2200, 0x2c40, 0x101c0, 0xe01e0:
      4,

      // Start of 0x560, 0x680-0x8c0, 0x900-0x920, 0x2d00, 0x2d40, 0x11280, 0x11740:
      5,

      // Start of 0x5a0, 0x620-0x640, 0x2240-0x2300, 0x1e080:
      6,

      // Start of 0x5c0, 0x104e0-0x10540, 0x10760, 0x10f20, 0x11140, 0x114c0:
      7,

      // Start of 0x5e0, 0x2440, 0x112e0, 0x1d1e0:
      8,

      // Start of 0x8e0, 0xd80, 0xe00, 0xe80, 0xfc0, 0x1000-0x1080, 0xa6a0-0xa700, 0x105a0, 0x11240, 0x11460,
      // 0x2fa20-0x2ffe0, 0x323c0-0xe00e0:
      9,

      // Start of 0xf20, 0x2a20-0x2a40:
      10,

      // Start of 0x1100-0x1120, 0x2ae0-0x2be0, 0xaac0, 0x110c0:
      11,

      // Start of 0x17c0-0x17e0, 0x2420, 0x2fe0, 0x10ea0, 0x10f80, 0x112a0, 0x11380, 0x116c0:
      12,

      // Start of 0x1ae0, 0x2d20, 0xa800-0xaaa0, 0xaae0, 0xab20, 0xfae0, 0x10d80, 0x1cec0-0x1cee0, 0x1cfe0,
      // 0x1d240, 0x1fc00-0x1ffe0, 0x2a6e0, 0x2ee60-0x2f7e0:
      13,

      // Start of 0x2340-0x2400, 0x2500-0x29e0, 0x11120, 0x1eca0:
      14,

      // Start of 0x2a80-0x2aa0, 0x105c0-0x10740, 0x110e0, 0x11320:
      15,

      // Start of 0x10440-0x10480:
      16,

      // Start of 0x10e60-0x10e80, 0x10f00, 0x10f40, 0x10fe0-0x11020, 0x11080-0x110a0, 0x11100,
      // 0x11160-0x111c0, 0x11220, 0x112c0, 0x11400-0x11440, 0x11480-0x114a0, 0x11580, 0x115c0,
      // 0x11600-0x11620, 0x11680-0x116a0, 0x11700, 0x11800-0x11820, 0x1d340:
      17,

      // Start of 0x10fc0, 0x11720, 0x1e940:
      18,

      // Start of 0x11040, 0x1f1a0, 0x1f280-0x1f2e0, 0x1f8e0:
      19,

      // Start of 0x11060, 0x113a0, 0x115a0:
      20,

      // Start of 0x11200, 0x1e0a0-0x1e0e0, 0x1e160-0x1e260, 0x1e300-0x1e4a0, 0x1e500-0x1e5a0,
      // 0x1e600-0x1e7c0, 0x1e8e0:
      21,

      // Start of 0x11300, 0x11360, 0x116e0, 0x1eec0-0x1efe0:
      22,

      // Start of 0x11660:
      23,

      // Start of 0x1bcc0-0x1cbe0:
      24,

      // Start of 0x1d200-0x1d220, 0x1d300-0x1d320:
      25,

      // Start of 0x1d2c0-0x1d2e0:
      valid,

      // Start of 0x1d360:
      valid,

      // Start of 0x1d400-0x1d420, 0x1d460, 0x1d4e0, 0x1d560-0x1d600, 0x1d640-0x1d680, 0x1d700-0x1d7a0,
      // 0x1e280, 0x1e4c0, 0x1e5c0, 0x1ec60, 0x1f1e0, 0xe0120-0xe01c0:
      valid,

      // Start of 0x1ec80, 0x1ed20:
      valid, valid,

      // Start of 0xa0:
      26, valid, valid, valid, valid, valid, valid, valid, 27, valid, 0, valid, valid, 28, valid, 29, valid,
      valid, 30, 31, 32, 33, valid, valid, 34, 35, 14, valid, 36, 37, 38, valid,

      // Start of 0xc0:
      39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, valid, 62,
      63, 64, 65, 66, 67, 68, valid,

      // Start of 0x100:
      69, valid, 70, valid, 71, valid, 72, valid, 73, valid, 74, valid, 75, valid, 76, valid, 77, valid, 78,
      valid, 79, valid, 80, valid, 81, valid, 82, valid, 83, valid, 84, valid,

      // Start of 0x120:
      85, valid, 86, valid, 87, valid, 88, valid, 89, valid, 90, valid, 91, valid, 92, valid, 93, valid, 94,
      94, 95, valid, 96, valid, valid, 97, valid, 98, valid, 99, valid,

      // Start of 0x140:
      100, 101, valid, 102, valid, 103, valid, 104, valid, 105, 106, valid, 107, valid, 108, valid, 109,
      valid, 110, valid, 111, valid, 112, valid, 113, valid, 114, valid, 115, valid, 116, valid,

      // Start of 0x160:
      117, valid, 118, valid, 119, valid, 120, valid, 121, valid, 122, valid, 123, valid, 124, valid, 125,
      valid, 126, valid, 127, valid, 128, valid, 129, 130, valid, 131, valid, 132, valid, 18,

      // Start of 0x180:
      valid, 133, 134, valid, 135, valid, 136, 137, valid, 138, 139, 140, valid, valid, 141, 142, 143, 144,
      valid, 145, 146, valid, 147, 148, 149, valid, valid, valid, 150, 151, valid, 152,

      // Start of 0x1a0:
      153, valid, 154, valid, 155, valid, 156, 157, valid, 158, valid, valid, 159, valid, 160, 161, valid,
      162, 163, 164, valid, 165, valid, 166, 167, valid, valid, valid, 168,

      // Start of 0x1c0:
      valid, valid, valid, valid, 169, 169, 169, 170, 170, 170, 171, 171, 171, 172, valid, 173, valid, 174,
      valid, 175, valid, 176, valid, 177, valid, 178, valid, 179, valid, valid, 180, valid,

      // Start of 0x1e0:
      181, valid, 182, valid, 183, valid, 184, valid, 185, valid, 186, valid, 187, valid, 188, valid, valid,
      189, 189, 189, 190, valid, 191, 192, 193, valid, 194, valid, 195, valid, 196, valid,

      // Start of 0x200:
      197, valid, 198, valid, 199, valid, 200, valid, 201, valid, 202, valid, 203, valid, 204, valid, 205,
      valid, 206, valid, 207, valid, 208, valid, 209, valid, 210, valid, 211, valid, 212, valid,

      // Start of 0x220:
      213, valid, 214, valid, 215, valid, 216, valid, 217, valid, 218, valid, 219, valid, 220, valid, 221,
      valid, 222, valid, valid, valid, valid, valid, valid, valid, 223, 224, valid, 225, 226,

      // Start of 0x240:
      valid, 227, valid, 228, 229, 230, 231, valid, 232, valid, 233, valid, 234, valid, 235, valid,

      // Start of 0x2a0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 7, 236, 9, 17, 237, 238, 239, 22, 24,

      // Start of 0x2c0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 240, 241, 242, 243, 244, 245, valid,
      valid,

      // Start of 0x2e0:
      146, 11, 18, 23, 246, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid,

      // Start of 0x340:
      247, 248, valid, 249, 250, 251, valid, valid, valid, valid, valid, valid, valid, valid, valid, 28,

      // Start of 0x360:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 252, valid, 253, valid, 254, valid, 255, valid, disallowed, disallowed, 256, valid, valid, valid,
      257, 258,

      // Start of 0x380:
      disallowed, disallowed, disallowed, disallowed, 32, 259, 260, 261, 262, 263, 264, disallowed, 265,
      disallowed, 266, 267, valid, 268, 269, 270, 271, 272, 273, 274, 275, 251, 276, 277, 33, 278, 279, 280,

      // Start of 0x3a0:
      281, 282, disallowed, 283, 284, 285, 286, 287, 288, 289, 290, 291, valid, valid, valid, valid, valid,

      // Start of 0x3c0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      292, 269, 275, 285, 266, 291, 286, 281, valid, 293, valid, 294, valid, 295, valid, 296, valid,

      // Start of 0x3e0:
      297, valid, 298, valid, 299, valid, 300, valid, 301, valid, 302, valid, 303, valid, 304, valid, 276,
      282, 283, valid, 275, 272, valid, 305, valid, 283, 306, valid, valid, 307, 308, 309,

      // Start of 0x400:
      310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330,
      331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341,

      // Start of 0x420:
      342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x460:
      358, valid, 359, valid, 360, valid, 361, valid, 362, valid, 363, valid, 364, valid, 365, valid, 366,
      valid, 367, valid, 368, valid, 369, valid, 370, valid, 371, valid, 372, valid, 373, valid,

      // Start of 0x480:
      374, valid, valid, valid, valid, valid, valid, valid, valid, valid, 375, valid, 376, valid, 377, valid,
      378, valid, 379, valid, 380, valid, 381, valid, 382, valid, 383, valid, 384, valid, 385, valid,

      // Start of 0x4a0:
      386, valid, 387, valid, 388, valid, 389, valid, 390, valid, 391, valid, 392, valid, 393, valid, 394,
      valid, 395, valid, 396, valid, 397, valid, 398, valid, 399, valid, 400, valid, 401, valid,

      // Start of 0x4c0:
      402, 403, valid, 404, valid, 405, valid, 406, valid, 407, valid, 408, valid, 409, valid, valid, 410,
      valid, 411, valid, 412, valid, 413, valid, 414, valid, 415, valid, 416, valid, 417, valid,

      // Start of 0x4e0:
      418, valid, 419, valid, 420, valid, 421, valid, 422, valid, 423, valid, 424, valid, 425, valid, 426,
      valid, 427, valid, 428, valid, 429, valid, 430, valid, 431, valid, 432, valid, 433, valid,

      // Start of 0x500:
      434, valid, 435, valid, 436, valid, 437, valid, 438, valid, 439, valid, 440, valid, 441, valid, 442,
      valid, 443, valid, 444, valid, 445, valid, 446, valid, 447, valid, 448, valid, 449, valid,

      // Start of 0x520:
      450, valid, 451, valid, 452, valid, 453, valid, 454, valid, 455, valid, 456, valid, 457, valid,
      disallowed, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472,

      // Start of 0x540:
      473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493,
      494, 495, disallowed, disallowed,

      // Start of 0x580:
      valid, valid, valid, valid, valid, valid, valid, 496, valid, valid, valid, disallowed, disallowed,
      valid, valid, valid, disallowed,

      // Start of 0x660:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, 497, 498, 499, 500,

      // Start of 0x940:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 501, 502, 503, 504, 505, 506, 507, 508,

      // Start of 0x9c0:
      valid, valid, valid, valid, valid, disallowed, disallowed, valid, valid, disallowed, disallowed, valid,
      valid, valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, valid, disallowed, disallowed, disallowed, disallowed, 509, 510, disallowed, 511,

      // Start of 0xa20:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, disallowed, valid, valid, valid, valid,
      valid, valid, valid, disallowed, valid, 512, disallowed, valid, 513, disallowed, valid, valid,
      disallowed, disallowed, valid, disallowed,

      // Start of 0xa40:
      valid, valid, valid, disallowed, disallowed, disallowed, disallowed, valid, valid, disallowed,
      disallowed, valid, valid, valid, disallowed, disallowed, disallowed, valid, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, 514, 515, 516, valid, disallowed, 517,
      disallowed,

      // Start of 0xb40:
      valid, valid, valid, valid, valid, disallowed, disallowed, valid, valid, disallowed, disallowed, valid,
      valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
      valid, valid, disallowed, disallowed, disallowed, disallowed, 518, 519, disallowed,

      // Start of 0xe20:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, 520, valid, valid, valid, valid, valid, valid, valid, disallowed,
      disallowed, disallowed, disallowed,

      // Start of 0xea0:
      valid, valid, valid, valid, disallowed, valid, disallowed, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, 521, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, disallowed, disallowed,

      // Start of 0xec0:
      valid, valid, valid, valid, valid, disallowed, valid, disallowed, valid, valid, valid, valid, valid,
      valid, valid, disallowed, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      disallowed, disallowed, 522, 523,

      // Start of 0xf00:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 524, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0xf40:
      valid, valid, valid, 525, valid, valid, valid, valid, disallowed, valid, valid, valid, valid, 526,
      valid, valid, valid, valid, 527, valid, valid, valid, valid, 528, valid, valid, valid, valid, 529,

      // Start of 0xf60:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 530, valid, valid, valid, disallowed,
      disallowed, disallowed, disallowed, valid, valid, 531, valid, 532, 533, 534, 535, 536, valid, valid,
      valid, valid, valid,

      // Start of 0xf80:
      valid, 537, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, 538, valid, valid, valid, valid, disallowed, valid, valid, valid, valid,
      539,

      // Start of 0xfa0:
      valid, valid, 540, valid, valid, valid, valid, 541, valid, valid, valid, valid, 542, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 543, valid, valid, valid,
      disallowed, valid, valid,

      // Start of 0x10a0:
      544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, 561, 562, 563, 564,
      565, 566, 567, 568, 569, 570, 571, 572, 573, 574, 575,

      // Start of 0x10c0:
      576, 577, 578, 579, 580, 581, disallowed, 582, disallowed, disallowed, disallowed, disallowed,
      disallowed, 583, disallowed, disallowed,

      // Start of 0x10e0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 584,

      // Start of 0x1140:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 28,

      // Start of 0x13e0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed, 585, 586, 587, 588, 589, 590,
      disallowed, disallowed,

      // Start of 0x17a0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, 28, 28,

      // Start of 0x1800:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 28, 28, 28, 28, 28, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed,

      // Start of 0x1c80:
      328, 330, 340, 343, 344, 344, 352, 359, 591, 592, valid, disallowed, disallowed, disallowed, disallowed,
      disallowed, 593, 594, 595, 596, 597, 598, 599, 600, 601, 602, 603, 604, 584, 605, 606, 607,

      // Start of 0x1ca0:
      608, 609, 610, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620, 621, 622, 623, 624, 625, 626, 627, 628,
      629, 630, 631, 632, 633, 634, disallowed, disallowed, 635, 636, 637,

      // Start of 0x1d20:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 0, 45, 1, valid, 3,
      4, 141, 6, 7, 8, 9, 10, 11, 12, 13, valid, 14, 214, 15, 17,

      // Start of 0x1d40:
      19, 20, 22, 0, 638, 639, 640, 1, 3, 4, 142, 143, 641, 6, valid, 10, 12, 106, 14, 136, 642, 643, 15, 19,
      20, 644, 150, 21, 645, 269, 270, 271,

      // Start of 0x1d60:
      286, 287, 8, 17, 20, 21, 269, 270, 282, 286, 287, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, 339,

      // Start of 0x1d80:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 646, 2, 647, 55,
      641,

      // Start of 0x1da0:
      5, 648, 649, 650, 148, 147, 651, 652, 653, 654, 655, 656, 657, 658, 151, 659, 660, 152, 661, 662, 158,
      663, 229, 162, 664, 163, 230, 25, 665, 666, 166, 275,

      // Start of 0x1e00:
      667, valid, 668, valid, 669, valid, 670, valid, 671, valid, 672, valid, 673, valid, 674, valid, 675,
      valid, 676, valid, 677, valid, 678, valid, 679, valid, 680, valid, 681, valid, 682, valid,

      // Start of 0x1e20:
      683, valid, 684, valid, 685, valid, 686, valid, 687, valid, 688, valid, 689, valid, 690, valid, 691,
      valid, 692, valid, 693, valid, 694, valid, 695, valid, 696, valid, 697, valid, 698, valid,

      // Start of 0x1e40:
      699, valid, 700, valid, 701, valid, 702, valid, 703, valid, 704, valid, 705, valid, 706, valid, 707,
      valid, 708, valid, 709, valid, 710, valid, 711, valid, 712, valid, 713, valid, 714, valid,

      // Start of 0x1e60:
      715, valid, 716, valid, 717, valid, 718, valid, 719, valid, 720, valid, 721, valid, 722, valid, 723,
      valid, 724, valid, 725, valid, 726, valid, 727, valid, 728, valid, 729, valid, 730, valid,

      // Start of 0x1e80:
      731, valid, 732, valid, 733, valid, 734, valid, 735, valid, 736, valid, 737, valid, 738, valid, 739,
      valid, 740, valid, 741, valid, valid, valid, valid, valid, 742, 715, valid, valid, 743, valid,

      // Start of 0x1ea0:
      744, valid, 745, valid, 746, valid, 747, valid, 748, valid, 749, valid, 750, valid, 751, valid, 752,
      valid, 753, valid, 754, valid, 755, valid, 756, valid, 757, valid, 758, valid, 759, valid,

      // Start of 0x1ec0:
      760, valid, 761, valid, 762, valid, 763, valid, 764, valid, 765, valid, 766, valid, 767, valid, 768,
      valid, 769, valid, 770, valid, 771, valid, 772, valid, 773, valid, 774, valid, 775, valid,

      // Start of 0x1ee0:
      776, valid, 777, valid, 778, valid, 779, valid, 780, valid, 781, valid, 782, valid, 783, valid, 784,
      valid, 785, valid, 786, valid, 787, valid, 788, valid, 789, valid, 790, valid, 791,

      // Start of 0x1f00:
      valid, valid, valid, valid, valid, valid, valid, valid, 792, 793, 794, 795, 796, 797, 798, 799, valid,
      valid, valid, valid, valid, valid, disallowed, disallowed, 800, 801, 802, 803, 804, 805, disallowed,
      disallowed,

      // Start of 0x1f20:
      valid, valid, valid, valid, valid, valid, valid, valid, 806, 807, 808, 809, 810, 811, 812, 813, valid,
      valid, valid, valid, valid, valid, valid, valid, 814, 815, 816, 817, 818, 819, 820, 821,

      // Start of 0x1f40:
      valid, valid, valid, valid, valid, valid, disallowed, disallowed, 822, 823, 824, 825, 826, 827,
      disallowed, disallowed, valid, valid, valid, valid, valid, valid, valid, valid, disallowed, 828,
      disallowed, 829, disallowed, 830, disallowed, 831,

      // Start of 0x1f60:
      valid, valid, valid, valid, valid, valid, valid, valid, 832, 833, 834, 835, 836, 837, 838, 839, valid,
      260, valid, 262, valid, 263, valid, 264, valid, 265, valid, 266, valid, 267, disallowed, disallowed,

      // Start of 0x1f80:
      840, 841, 842, 843, 844, 845, 846, 847, 840, 841, 842, 843, 844, 845, 846, 847, 848, 849, 850, 851, 852,
      853, 854, 855, 848, 849, 850, 851, 852, 853, 854, 855,

      // Start of 0x1fa0:
      856, 857, 858, 859, 860, 861, 862, 863, 856, 857, 858, 859, 860, 861, 862, 863, valid, valid, 864, 865,
      866, disallowed, valid, 867, 868, 869, 870, 260, 865, 871, 251, 871,

      // Start of 0x1fc0:
      872, 873, 874, 875, 876, disallowed, valid, 877, 878, 262, 879, 263, 875, 880, 881, 882, valid, valid,
      valid, 883, disallowed, disallowed, valid, valid, 884, 885, 886, 264, disallowed, 887, 888, 889,

      // Start of 0x1fe0:
      valid, valid, valid, 890, valid, valid, valid, valid, 891, 892, 893, 266, 894, 895, 259, 896,
      disallowed, disallowed, 897, 898, 899, disallowed, valid, 900, 901, 265, 902, 267, 898, 32, 903,
      disallowed,

      // Start of 0x2000:
      26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 28, valid, valid, disallowed, disallowed, valid, 904, valid,
      valid, valid, valid, valid, 905, valid, valid, valid, valid,

      // Start of 0x2020:
      valid, valid, valid, valid, disallowed, disallowed, disallowed, valid, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, 26, valid, valid, valid, 906, 907, valid,
      908, 909, valid, valid, valid, valid, 910, valid, 911,

      // Start of 0x2040:
      valid, valid, valid, valid, valid, valid, valid, 912, 913, 914, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, 915, valid, valid, valid, valid, valid, valid,
      valid, 26,

      // Start of 0x2060:
      28, 28, 28, 28, 28, disallowed, disallowed, disallowed, disallowed, disallowed, 28, 28, 28, 28, 28, 28,
      916, 8, disallowed, disallowed, 917, 918, 919, 920, 921, 922, 923, 924, 925, 926, 927, 13,

      // Start of 0x2080:
      916, 35, 30, 31, 917, 918, 919, 920, 921, 922, 923, 924, 925, 926, 927, disallowed, 0, 4, 14, 23, 142,
      7, 10, 11, 12, 13, 15, 18, 19, disallowed, disallowed, disallowed,

      // Start of 0x20a0:
      valid, valid, valid, valid, valid, valid, valid, valid, 928, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x2100:
      929, 930, 2, 931, valid, 932, 933, 143, valid, 934, 6, 7, 7, 7, 7, 88, 8, 8, 11, 11, valid, 13, 935,
      valid, valid, 15, 16, 17, 17, 17, valid, valid,

      // Start of 0x2120:
      936, 937, 938, valid, 25, valid, 289, valid, 25, valid, 10, 44, 1, 2, valid, 4, 4, 5, 939, 12, 14, 940,
      941, 942, 943, 8, valid, 944, 281, 270, 270, 281,

      // Start of 0x2140:
      945, valid, valid, valid, valid, 3, 3, 4, 8, 9, valid, valid, valid, valid, valid, valid, 946, 947, 948,
      949, 950, 951, 952, 953, 954, 955, 956, 957, 958, 959, 960, 961,

      // Start of 0x2160:
      8, 962, 963, 964, 21, 965, 966, 967, 968, 23, 969, 970, 11, 2, 3, 12, 8, 962, 963, 964, 21, 965, 966,
      967, 968, 23, 969, 970, 11, 2, 3, 12,

      // Start of 0x2180:
      valid, valid, valid, 971, valid, valid, valid, valid, valid, 972, valid, valid, disallowed, disallowed,
      disallowed, disallowed, valid, valid, valid, valid,

      // Start of 0x2220:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 973, 974, valid,
      975, 976, valid, valid, valid, valid, valid, valid,

      // Start of 0x2320:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 977, 978, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x2460:
      35, 30, 31, 917, 918, 919, 920, 921, 922, 979, 980, 981, 982, 983, 984, 985, 986, 987, 988, 989, 990,
      991, 992, 993, 994, 995, 996, 997, 998, 999, 1000, 1001,

      // Start of 0x2480:
      1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 1010,
      1011, 1012, 1013,

      // Start of 0x24a0:
      1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023, 1024, 1025, 1026, 1027, 1028, 1029, 1030,
      1031, 1032, 1033, 1034, 1035, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,

      // Start of 0x24c0:
      10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
      12, 13, 14, 15,

      // Start of 0x24e0:
      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 916, valid, valid, valid, valid, valid, valid, valid, valid,
      valid,

      // Start of 0x2a00:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 1036,

      // Start of 0x2a60:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, 1037, 1038, 1039,

      // Start of 0x2ac0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 1040, valid,
      valid, valid,

      // Start of 0x2c00:
      1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057,
      1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072,

      // Start of 0x2c20:
      1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x2c60:
      1089, valid, 1090, 1091, 1092, valid, valid, 1093, valid, 1094, valid, 1095, valid, 639, 657, 638, 646,
      valid, 1096, valid, valid, 1097, valid, valid, valid, valid, valid, valid, 9, 21, 1098, 1099,

      // Start of 0x2c80:
      1100, valid, 1101, valid, 1102, valid, 1103, valid, 1104, valid, 1105, valid, 1106, valid, 1107, valid,
      1108, valid, 1109, valid, 1110, valid, 1111, valid, 1112, valid, 1113, valid, 1114, valid, 1115, valid,

      // Start of 0x2ca0:
      1116, valid, 1117, valid, 1118, valid, 1119, valid, 1120, valid, 1121, valid, 1122, valid, 1123, valid,
      1124, valid, 1125, valid, 1126, valid, 1127, valid, 1128, valid, 1129, valid, 1130, valid, 1131, valid,

      // Start of 0x2cc0:
      1132, valid, 1133, valid, 1134, valid, 1135, valid, 1136, valid, 1137, valid, 1138, valid, 1139, valid,
      1140, valid, 1141, valid, 1142, valid, 1143, valid, 1144, valid, 1145, valid, 1146, valid, 1147, valid,

      // Start of 0x2ce0:
      1148, valid, 1149, valid, valid, valid, valid, valid, valid, valid, valid, 1150, valid, 1151, valid,
      valid, valid, valid, 1152, valid, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x2d60:
      valid, valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, 1153, valid, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed,

      // Start of 0x2e80:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, disallowed, valid, valid,
      valid, valid, 1154,

      // Start of 0x2ee0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, 1155, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x2f00:
      1156, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1170, 1171, 1172,
      1173, 1174, 1175, 1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183, 1184, 1185, 1186, 1187,

      // Start of 0x2f20:
      1188, 1189, 1190, 1191, 1192, 1193, 1194, 1195, 1196, 1197, 1198, 1199, 1200, 1201, 1202, 1203, 1204,
      1205, 1206, 1207, 1208, 1209, 1210, 1211, 1212, 1213, 1214, 1215, 1216, 1217, 1218, 1219,

      // Start of 0x2f40:
      1220, 1221, 1222, 1223, 1224, 1225, 1226, 1227, 1228, 1229, 1230, 1231, 1232, 1233, 1234, 1235, 1236,
      1237, 1238, 1239, 1240, 1241, 1242, 1243, 1244, 1245, 1246, 1247, 1248, 1249, 1250, 1251,

      // Start of 0x2f60:
      1252, 1253, 1254, 1255, 1256, 1257, 1258, 1259, 1260, 1261, 1262, 1263, 1264, 1265, 1266, 1267, 1268,
      1269, 1270, 1271, 1272, 1273, 1274, 1275, 1276, 1277, 1278, 1279, 1280, 1281, 1282, 1283,

      // Start of 0x2f80:
      1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1298, 1299, 1300,
      1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310, 1311, 1312, 1313, 1314, 1315,

      // Start of 0x2fa0:
      1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327, 1328, 1329, 1330, 1331, 1332,
      1333, 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344, 1345, 1346, 1347,

      // Start of 0x2fc0:
      1348, 1349, 1350, 1351, 1352, 1353, 1354, 1355, 1356, 1357, 1358, 1359, 1360, 1361, 1362, 1363, 1364,
      1365, 1366, 1367, 1368, 1369, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed,

      // Start of 0x3000:
      26, valid, 1370, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x3020:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, 1371, valid, 1179, 1372, 1373,

      // Start of 0x3080:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed, valid, valid, 1374,
      1375, valid, valid, 1376,

      // Start of 0x30e0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 1377,

      // Start of 0x3120:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, disallowed, 1378, 1379, 1380, 1381, 1382, 1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391,
      1392,

      // Start of 0x3140:
      1393, 1394, 1395, 1396, 1397, 1398, 1399, 1400, 1401, 1402, 1403, 1404, 1405, 1406, 1407, 1408, 1409,
      1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422, 1423, 1424,

      // Start of 0x3160:
      1425, 1426, 1427, 1428, 28, 1429, 1430, 1431, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440,
      1441, 1442, 1443, 1444, 1445, 1446, 1447, 1448, 1449, 1450, 1451, 1452, 1453, 1454, 1455,

      // Start of 0x3180:
      1456, 1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464, 1465, 1466, 1467, 1468, 1469, 1470, disallowed,
      valid, valid, 1156, 1162, 1471, 1472, 1473, 1474, 1475, 1476, 1160, 1477, 1478, 1479, 1480, 1164,

      // Start of 0x3200:
      1481, 1482, 1483, 1484, 1485, 1486, 1487, 1488, 1489, 1490, 1491, 1492, 1493, 1494, 1495, 1496, 1497,
      1498, 1499, 1500, 1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510, 1511, disallowed,

      // Start of 0x3220:
      1512, 1513, 1514, 1515, 1516, 1517, 1518, 1519, 1520, 1521, 1522, 1523, 1524, 1525, 1526, 1527, 1528,
      1529, 1530, 1531, 1532, 1533, 1534, 1535, 1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543,

      // Start of 0x3240:
      1544, 1545, 1546, 1547, 1548, 1549, 1222, 1550, valid, valid, valid, valid, valid, valid, valid, valid,
      1551, 1552, 1553, 1554, 1555, 1556, 1557, 1558, 1559, 1560, 1561, 1562, 1563, 1564, 1565, 1566,

      // Start of 0x3260:
      1378, 1381, 1384, 1386, 1394, 1395, 1398, 1400, 1401, 1403, 1404, 1405, 1406, 1407, 1567, 1568, 1569,
      1570, 1571, 1572, 1573, 1574, 1575, 1576, 1577, 1578, 1579, 1580, 1581, 1582, 1583, valid,

      // Start of 0x3280:
      1156, 1162, 1471, 1472, 1584, 1585, 1586, 1167, 1587, 1179, 1229, 1241, 1240, 1230, 1322, 1187, 1227,
      1588, 1589, 1590, 1591, 1592, 1593, 1594, 1595, 1596, 1597, 1193, 1598, 1599, 1600, 1601,

      // Start of 0x32a0:
      1602, 1603, 1604, 1605, 1473, 1474, 1475, 1606, 1607, 1608, 1609, 1610, 1611, 1612, 1613, 1614, 1615,
      1616, 1617, 1618, 1619, 1620, 1621, 1622, 1623, 1624, 1625, 1626, 1627, 1628, 1629, 1630,

      // Start of 0x32c0:
      1631, 1632, 1633, 1634, 1635, 1636, 1637, 1638, 1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647,
      1648, 1649, 1650, 1651, 1652, 1653, 1654, 1655, 1656, 1657, 1658, 1659, 1660, 1661, 1662,

      // Start of 0x32e0:
      1663, 1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677, 1678, 1679,
      1680, 1681, 1682, 1683, 1684, 1685, 1686, 1687, 1688, 1689, 1690, 1691, 1692, 1693, 1694,

      // Start of 0x3300:
      1695, 1696, 1697, 1698, 1699, 1700, 1701, 1702, 1703, 1704, 1705, 1706, 1707, 1708, 1709, 1710, 1711,
      1712, 1713, 1714, 1715, 1716, 1717, 1718, 1719, 1720, 1721, 1722, 1723, 1724, 1725, 1726,

      // Start of 0x3320:
      1727, 1728, 1729, 1730, 1731, 1732, 1733, 1734, 1735, 1736, 1737, 1738, 1739, 1740, 1741, 1742, 1743,
      1744, 1745, 1746, 1747, 1748, 1749, 1750, 1751, 1752, 1753, 1754, 1755, 1756, 1757, 1758,

      // Start of 0x3340:
      1759, 1760, 1761, 1762, 1763, 1764, 1765, 1766, 1767, 1768, 1769, 1770, 1771, 1772, 1773, 1774, 1775,
      1776, 1777, 1778, 1779, 1780, 1781, 1782, 1783, 1784, 1785, 1786, 1787, 1788, 1789, 1790,

      // Start of 0x3360:
      1791, 1792, 1793, 1794, 1795, 1796, 1797, 1798, 1799, 1800, 1801, 1802, 1803, 1804, 1805, 1806, 1807,
      1808, 1809, 1810, 1811, 1812, 1813, 1814, 1815, 1816, 1817, 1818, 1819, 1820, 1821, 1822,

      // Start of 0x3380:
      1823, 1824, 1825, 1826, 1827, 1828, 1829, 1830, 1831, 1832, 1833, 1834, 1835, 1836, 1837, 1838, 1839,
      1840, 1841, 1842, 1843, 1844, 1845, 1846, 1847, 1848, 1849, 1850, 1851, 1852, 1853, 1854,

      // Start of 0x33a0:
      1855, 1856, 1857, 1858, 1859, 1860, 1861, 1862, 1863, 1823, 1864, 1865, 1866, 1867, 1868, 1869, 1870,
      1871, 1872, 1873, 1874, 1875, 1876, 1877, 1878, 1877, 1879, 1880, 1881, 1882, 1883, 1882,

      // Start of 0x33c0:
      1884, 1885, disallowed, 1886, 1887, 1888, 1889, disallowed, 1890, 1891, 1892, 1893, 1894, 1895, 1853,
      1896, 1897, 1898, 1899, 1900, 1829, 1901, 1902, 1903, disallowed, 1904, 1905, 1906, 1907, 1908, 1909,
      1910,

      // Start of 0x33e0:
      1911, 1912, 1913, 1914, 1915, 1916, 1917, 1918, 1919, 1920, 1921, 1922, 1923, 1924, 1925, 1926, 1927,
      1928, 1929, 1930, 1931, 1932, 1933, 1934, 1935, 1936, 1937, 1938, 1939, 1940, 1941, 1942,

      // Start of 0xa640:
      1943, valid, 1944, valid, 1945, valid, 1946, valid, 1947, valid, 591, valid, 1948, valid, 1949, valid,
      1950, valid, 1951, valid, 1952, valid, 1953, valid, 1954, valid, 1955, valid, 1956, valid, 1957, valid,

      // Start of 0xa660:
      1958, valid, 1959, valid, 1960, valid, 1961, valid, 1962, valid, 1963, valid, 1964, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid,

      // Start of 0xa680:
      1965, valid, 1966, valid, 1967, valid, 1968, valid, 1969, valid, 1970, valid, 1971, valid, 1972, valid,
      1973, valid, 1974, valid, 1975, valid, 1976, valid, 1977, valid, 1978, valid, 352, 354,

      // Start of 0xa720:
      valid, valid, 1979, valid, 1980, valid, 1981, valid, 1982, valid, 1983, valid, 1984, valid, 1985, valid,
      valid, valid, 1986, valid, 1987, valid, 1988, valid, 1989, valid, 1990, valid, 1991, valid, 1992, valid,

      // Start of 0xa740:
      1993, valid, 1994, valid, 1995, valid, 1996, valid, 1997, valid, 1998, valid, 1999, valid, 2000, valid,
      2001, valid, 2002, valid, 2003, valid, 2004, valid, 2005, valid, 2006, valid, 2007, valid, 2008, valid,

      // Start of 0xa760:
      2009, valid, 2010, valid, 2011, valid, 2012, valid, 2013, valid, 2014, valid, 2015, valid, 2016, valid,
      2016, valid, valid, valid, valid, valid, valid, valid, valid, 2017, valid, 2018, valid, 2019, 2020,
      valid,

      // Start of 0xa780:
      2021, valid, 2022, valid, 2023, valid, 2024, valid, valid, valid, valid, 2025, valid, 650, valid, valid,
      2026, valid, 2027, valid, valid, valid, 2028, valid, 2029, valid, 2030, valid, 2031, valid, 2032, valid,

      // Start of 0xa7a0:
      2033, valid, 2034, valid, 2035, valid, 2036, valid, 2037, valid, 236, 641, 649, 2038, 651, valid, 2039,
      2040, 653, 2041, 2042, valid, 2043, valid, 2044, valid, 2045, valid, 2046, valid, 2047, valid,

      // Start of 0xa7c0:
      2048, valid, 2049, valid, 2050, 662, 2051, 2052, valid, 2053, valid, 2054, 2055, valid, disallowed,
      disallowed, 2056, valid, disallowed, valid, disallowed, valid, 2057, valid, 2058, valid, 2059, valid,
      2060,

      // Start of 0xa7e0:
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, 2, 5, 16, 2061, valid, valid, 88, 110,

      // Start of 0xab40:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 1981, 2062,
      1090, 2063,

      // Start of 0xab60:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 2064, valid, valid, disallowed,
      disallowed, disallowed, disallowed, 2065, 2066, 2067, 2068, 2069, 2070, 2071, 2072, 2073, 2074, 2075,
      2076, 2077, 2078, 2079, 2080,

      // Start of 0xab80:
      2081, 2082, 2083, 2084, 2085, 2086, 2087, 2088, 2089, 2090, 2091, 2092, 2093, 2094, 2095, 2096, 2097,
      2098, 2099, 2100, 2101, 2102, 2103, 2104, 2105, 2106, 2107, 2108, 2109, 2110, 2111, 2112,

      // Start of 0xaba0:
      2113, 2114, 2115, 2116, 2117, 2118, 2119, 2120, 2121, 2122, 2123, 2124, 2125, 2126, 2127, 2128, 2129,
      2130, 2131, 2132, 2133, 2134, 2135, 2136, 2137, 2138, 2139, 2140, 2141, 2142, 2143, 2144,

      // Start of 0xf900:
      2145, 2146, 1314, 2147, 2148, 2149, 2150, 1368, 1368, 2151, 1322, 2152, 2153, 2154, 2155, 2156, 2157,
      2158, 2159, 2160, 2161, 2162, 2163, 2164, 2165, 2166, 2167, 2168, 2169, 2170, 2171, 2172,

      // Start of 0xf920:
      2173, 2174, 2175, 2176, 2177, 2178, 2179, 2180, 2181, 2182, 2183, 2184, 2185, 2186, 2187, 2188, 2189,
      2190, 2191, 2192, 1280, 2193, 2194, 2195, 2196, 2197, 2198, 2199, 2200, 2201, 2202, 2203,

      // Start of 0xf940:
      1353, 2204, 2205, 2206, 2207, 2208, 2209, 2210, 2211, 2212, 2213, 2214, 2215, 2216, 2217, 2218, 2219,
      2220, 2221, 2222, 2223, 2224, 2225, 2226, 2227, 2228, 2229, 2230, 2161, 2231, 2232, 2233,

      // Start of 0xf960:
      2234, 2235, 2236, 2237, 2238, 2239, 2240, 2241, 2242, 2243, 2244, 2245, 2246, 2247, 2248, 2249, 2250,
      1316, 2251, 2252, 2253, 2254, 2255, 2256, 2257, 2258, 2259, 2260, 2261, 2262, 2263, 2264,

      // Start of 0xf980:
      2265, 1193, 2266, 2267, 2268, 2269, 2270, 2271, 2272, 2273, 1174, 2274, 2275, 2276, 2277, 2278, 2279,
      2280, 2281, 2282, 2283, 2284, 2285, 2286, 2287, 2288, 2289, 2290, 2291, 2292, 2293, 2294,

      // Start of 0xf9a0:
      2295, 2249, 2296, 2297, 2298, 2299, 2300, 2301, 2302, 2303, 2233, 2304, 2305, 2306, 2307, 2308, 2309,
      2310, 2311, 2312, 2313, 2314, 2315, 2316, 2317, 2318, 2319, 2320, 2321, 2322, 2323, 2161,

      // Start of 0xf9c0:
      2324, 2325, 2326, 2327, 1367, 2328, 2329, 2330, 2331, 2332, 2333, 2334, 2335, 2336, 2337, 2338, 2339,
      1585, 2340, 2341, 2342, 2343, 2344, 2345, 2346, 2347, 2348, 2235, 2349, 2350, 2351, 2352,

      // Start of 0xf9e0:
      2353, 2354, 2355, 2356, 2357, 2358, 2359, 2360, 2361, 1321, 2362, 2363, 2364, 2365, 2366, 2367, 2368,
      2369, 2370, 2371, 2372, 2373, 2374, 1272, 2375, 2376, 2377, 2378, 2379, 2380, 2381, 2382,

      // Start of 0xfa00:
      2383, 2384, 2385, 2386, 2387, 2388, 2389, 2390, 1299, 2391, 1302, 2392, 2393, 2394, valid, valid, 2395,
      valid, 2396, valid, valid, 2397, 2398, 2399, 2400, 2401, 2402, 2403, 2404, 2405, 1279, valid,

      // Start of 0xfa20:
      2406, valid, 2407, valid, valid, 2408, 2409, valid, valid, valid, 2410, 2411, 2412, 2413, 2414, 2415,
      2416, 2417, 2418, 2419, 2420, 2421, 2422, 2423, 2424, 2425, 2426, 2427, 1200, 2428, 2429, 2430,

      // Start of 0xfa40:
      2431, 2432, 2433, 2434, 2435, 2436, 2437, 2438, 2439, 2440, 2441, 2442, 1590, 2443, 2444, 2445, 2446,
      1594, 2447, 2448, 2449, 2450, 2451, 2285, 2452, 2453, 2454, 2455, 2456, 2457, 2457, 2458,

      // Start of 0xfa60:
      2459, 2460, 2461, 2462, 2463, 2464, 2465, 2408, 2466, 2467, 2468, 2469, 2470, 2471, disallowed,
      disallowed, 2472, 2473, 2474, 2475, 2476, 2477, 2478, 2479, 2422, 2480, 2481, 2482, 2395, 2483, 2484,
      2485,

      // Start of 0xfa80:
      2486, 2487, 2488, 2489, 2490, 2491, 2492, 2493, 2494, 2430, 2495, 2431, 2496, 2497, 2498, 2499, 2500,
      2396, 2182, 2501, 2502, 1233, 2250, 2333, 2503, 2504, 2438, 2505, 2439, 2506, 2507, 2508,

      // Start of 0xfaa0:
      2398, 2509, 2510, 2511, 2512, 2513, 2399, 2514, 2515, 2516, 2517, 2518, 2519, 2451, 2520, 2521, 2285,
      2522, 2455, 2523, 2524, 2525, 2526, 2527, 2460, 2528, 2407, 2529, 2461, 2231, 2530, 2462,

      // Start of 0xfac0:
      2531, 2464, 2532, 2533, 2534, 2535, 2536, 2466, 2404, 2537, 2467, 2538, 2468, 2539, 1368, 2540, 2541,
      2542, 2543, 2544, 2545, 2546, 2547, 2548, 2549, 2550, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed,

      // Start of 0xfb00:
      2551, 2552, 2553, 2554, 2555, 2556, 2556, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 2557, 2558, 2559,
      2560, 2561, disallowed, disallowed, disallowed, disallowed, disallowed, 2562, valid, 2563,

      // Start of 0xfb20:
      2564, 940, 943, 2565, 2566, 2567, 2568, 2569, 2570, 923, 2571, 2572, 2573, 2574, 2575, 2576, 2577, 2578,
      2579, 2580, 2581, 2582, 2583, disallowed, 2584, 2585, 2586, 2587, 2588, disallowed, 2589, disallowed,

      // Start of 0xfb40:
      2590, 2591, disallowed, 2592, 2593, disallowed, 2594, 2595, 2596, 2597, 2598, 2599, 2600, 2601, 2602,
      2603, 2604, 2604, 2605, 2605, 2605, 2605, 2606, 2606, 2606, 2606, 2607, 2607, 2607, 2607,

      // Start of 0xfb60:
      2608, 2608, 2609, 2609, 2609, 2609, 2610, 2610, 2610, 2610, 2611, 2611, 2611, 2611, 2612, 2612, 2612,
      2612, 2613, 2613, 2613, 2613, 2614, 2614, 2614, 2614, 2615, 2615, 2615, 2615,

      // Start of 0xfb80:
      2616, 2616, 2617, 2617, 2618, 2618, 2619, 2619, 2620, 2620, 2621, 2621, 2622, 2622, 2623, 2623, 2623,
      2623, 2624, 2624, 2624, 2624, 2625, 2625, 2625, 2625, 2626, 2626, 2626, 2626, 2627, 2627,

      // Start of 0xfba0:
      2628, 2628, 2628, 2628, 2629, 2629, 2630, 2630, 2630, 2630, 2631, 2631, 2631, 2631, 2632, 2632, 2633,
      2633, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0xfbc0:
      valid, valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, 2634, 2634, 2634, 2634, 2635, 2635, 2636, 2636, 2637, 2637, 499, 2638, 2638,

      // Start of 0xfbe0:
      2639, 2639, 2640, 2640, 2641, 2641, 2641, 2641, 2642, 2642, 2643, 2643, 2644, 2644, 2645, 2645, 2646,
      2646, 2647, 2647, 2648, 2648, 2649, 2649, 2649, 2650, 2650, 2650, 2651, 2651, 2651, 2651,

      // Start of 0xfc00:
      2652, 2653, 2654, 2650, 2655, 2656, 2657, 2658, 2659, 2660, 2661, 2662, 2663, 2664, 2665, 2666, 2667,
      2668, 2669, 2670, 2671, 2672, 2673, 2674, 2675, 2676, 2677, 2678, 2679, 2680, 2681, 2682,

      // Start of 0xfc20:
      2683, 2684, 2685, 2686, 2687, 2688, 2689, 2690, 2691, 2692, 2693, 2694, 2695, 2696, 2697, 2698, 2699,
      2700, 2701, 2702, 2703, 2704, 2705, 2706, 2707, 2708, 2709, 2710, 2711, 2712, 2713, 2714,

      // Start of 0xfc40:
      2715, 2716, 2717, 2718, 2719, 2720, 2721, 2722, 2723, 2724, 2725, 2726, 2727, 2728, 2729, 2730, 2731,
      2732, 2733, 2734, 2735, 2736, 2737, 2738, 2739, 2740, 2741, 2742, 2743, 2744, 2745, 2746,

      // Start of 0xfc60:
      2747, 2748, 2749, 2750, 2751, 2752, 2654, 2753, 2650, 2655, 2754, 2755, 2659, 2756, 2660, 2661, 2757,
      2758, 2665, 2759, 2666, 2667, 2760, 2761, 2669, 2762, 2670, 2671, 2700, 2701, 2704, 2705,

      // Start of 0xfc80:
      2706, 2710, 2711, 2712, 2713, 2717, 2718, 2719, 2763, 2723, 2764, 2765, 2729, 2766, 2730, 2731, 2744,
      2767, 2768, 2739, 2769, 2740, 2741, 2652, 2653, 2770, 2654, 2771, 2656, 2657, 2658, 2659,

      // Start of 0xfca0:
      2772, 2662, 2663, 2664, 2665, 2773, 2669, 2672, 2673, 2674, 2675, 2676, 2678, 2679, 2680, 2681, 2682,
      2683, 2774, 2684, 2685, 2686, 2687, 2688, 2689, 2691, 2692, 2693, 2694, 2695, 2696, 2697,

      // Start of 0xfcc0:
      2698, 2699, 2702, 2703, 2707, 2708, 2709, 2710, 2711, 2714, 2715, 2716, 2717, 2775, 2720, 2721, 2722,
      2723, 2726, 2727, 2728, 2729, 2776, 2732, 2733, 2777, 2736, 2737, 2738, 2739, 2778, 2654,

      // Start of 0xfce0:
      2771, 2659, 2772, 2665, 2773, 2669, 2779, 2682, 2780, 2781, 2782, 2710, 2711, 2717, 2729, 2776, 2739,
      2778, 2783, 2784, 2785, 2786, 2787, 2788, 2789, 2790, 2791, 2792, 2793, 2794, 2795, 2796,

      // Start of 0xfd00:
      2797, 2798, 2799, 2800, 2801, 2802, 2803, 2804, 2805, 2806, 2807, 2808, 2781, 2809, 2810, 2811, 2812,
      2786, 2787, 2788, 2789, 2790, 2791, 2792, 2793, 2794, 2795, 2796, 2797, 2798, 2799, 2800,

      // Start of 0xfd20:
      2801, 2802, 2803, 2804, 2805, 2806, 2807, 2808, 2781, 2809, 2810, 2811, 2812, 2806, 2807, 2808, 2781,
      2780, 2782, 2690, 2679, 2680, 2681, 2806, 2807, 2808, 2690, 2691, 2813, 2813,

      // Start of 0xfd40:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 2814, 2815, 2815, 2816, 2817, 2818, 2819, 2820, 2821, 2821, 2822, 2823, 2824, 2825, 2826,

      // Start of 0xfd60:
      2827, 2828, 2829, 2829, 2830, 2830, 2831, 2832, 2832, 2833, 2834, 2834, 2835, 2835, 2836, 2837, 2837,
      2838, 2838, 2839, 2840, 2841, 2842, 2842, 2843, 2844, 2845, 2846, 2847, 2847, 2848, 2849,

      // Start of 0xfd80:
      2850, 2851, 2852, 2853, 2853, 2854, 2854, 2855, 2855, 2856, 2857, 2858, 2859, 2860, 2861, 2862,
      disallowed, disallowed, 2863, 2864, 2865, 2866, 2867, 2868, 2868, 2869, 2870, 2871, 2872, 2872, 2873,
      2874,

      // Start of 0xfda0:
      2875, 2876, 2877, 2878, 2879, 2880, 2881, 2882, 2883, 2884, 2885, 2886, 2887, 2888, 2889, 2890, 2891,
      2892, 2893, 2894, 2848, 2850, 2895, 2896, 2897, 2898, 2899, 2900, 2899, 2897, 2901, 2902,

      // Start of 0xfdc0:
      2903, 2904, 2905, 2900, 2841, 2831, 2906, 2907, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, valid,

      // Start of 0xfde0:
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 2908,
      2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2918, 2919, 2920, valid, valid, valid,

      // Start of 0xfe00:
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 2921, 2922, disallowed, 2923, 257, 2924,
      2925, 2926, 2927, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0xfe20:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, disallowed, 2928, 2929, 2930, 2930, 926, 927, 2931, 2932, 2933, 2934, 2935, 2936, 2937, 2938,
      977,

      // Start of 0xfe40:
      978, 2939, 2940, 2941, 2942, valid, valid, 2943, 2944, 911, 911, 911, 911, 2930, 2930, 2930, 2921, 2922,
      disallowed, disallowed, 257, 2923, 2925, 2924, 2928, 926, 927, 2931, 2932, 2933, 2934, 2945,

      // Start of 0xfe60:
      2946, 2947, 923, 2948, 2949, 2950, 925, disallowed, 2951, 2952, 2953, 2954, disallowed, disallowed,
      disallowed, disallowed, 2955, 2956, 2957, valid, 2958, disallowed, 2959, 2960, 2961, 2962, 2963, 2964,
      2965, 2966, 2967, 2968,

      // Start of 0xfe80:
      2969, 2970, 2970, 2971, 2971, 2972, 2972, 2973, 2973, 2974, 2974, 2974, 2974, 2975, 2975, 2976, 2976,
      2976, 2976, 2977, 2977, 2978, 2978, 2978, 2978, 2979, 2979, 2979, 2979, 2980, 2980,

      // Start of 0xfea0:
      2980, 2981, 2981, 2981, 2981, 2982, 2982, 2982, 2982, 2983, 2983, 2984, 2984, 2985, 2985, 2986, 2986,
      2987, 2987, 2987, 2987, 2988, 2988, 2988, 2988, 2989, 2989, 2989, 2989, 2990, 2990,

      // Start of 0xfec0:
      2990, 2991, 2991, 2991, 2991, 2992, 2992, 2992, 2992, 2993, 2993, 2993, 2993, 2994, 2994, 2994, 2994,
      2995, 2995, 2995, 2995, 2996, 2996, 2996, 2996, 2997, 2997, 2997, 2997, 2998, 2998,

      // Start of 0xfee0:
      2998, 2999, 2999, 2999, 2999, 3000, 3000, 3000, 3000, 3001, 3001, 3001, 3001, 3002, 3002, 2642, 2642,
      3003, 3003, 3003, 3003, 3004, 3004, 3005, 3005, 3006, 3006, 3007, 3007, disallowed, disallowed, 28,

      // Start of 0xff00:
      disallowed, 2924, 3008, 2945, 2952, 2953, 2946, 3009, 926, 927, 2947, 923, 2921, 2948, 1370, 3010, 916,
      35, 30, 31, 917, 918, 919, 920, 921, 922, 2923, 257, 2949, 925, 2950, 2925,

      // Start of 0xff20:
      2954, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
      2943, 2951, 2944, 3011, 2930,

      // Start of 0xff40:
      896, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 2931,
      3012, 2932, 3013, 3014,

      // Start of 0xff60:
      3015, 1370, 2939, 2940, 2922, 3016, 1693, 3017, 3018, 3019, 3020, 3021, 3022, 3023, 3024, 3025, 3026,
      1647, 1648, 1649, 1650, 1651, 1652, 1653, 1654, 1655, 1656, 1657, 1658, 1659, 1660, 1661,

      // Start of 0xff80:
      1662, 1663, 1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677, 1678,
      1679, 1680, 1681, 1682, 1683, 1684, 1685, 1686, 1687, 1688, 1689, 1690, 3027, 3028, 3029,

      // Start of 0xffa0:
      28, 1378, 1379, 1380, 1381, 1382, 1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391, 1392, 1393,
      1394, 1395, 1396, 1397, 1398, 1399, 1400, 1401, 1402, 1403, 1404, 1405, 1406, 1407,

      // Start of 0xffc0:
      disallowed, disallowed, 1408, 1409, 1410, 1411, 1412, 1413, disallowed, disallowed, 1414, 1415, 1416,
      1417, 1418, 1419, disallowed, disallowed, 1420, 1421, 1422, 1423, 1424, 1425, disallowed, disallowed,
      1426, 1427, 1428, disallowed, disallowed, disallowed,

      // Start of 0xffe0:
      3030, 3031, 3032, 29, 3033, 3034, 3035, disallowed, 3036, 3037, 3038, 3039, 3040, 3041, 3042,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed,

      // Start of 0x10400:
      3043, 3044, 3045, 3046, 3047, 3048, 3049, 3050, 3051, 3052, 3053, 3054, 3055, 3056, 3057, 3058, 3059,
      3060, 3061, 3062, 3063, 3064, 3065, 3066, 3067, 3068, 3069, 3070, 3071, 3072, 3073, 3074,

      // Start of 0x10420:
      3075, 3076, 3077, 3078, 3079, 3080, 3081, 3082, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid,

      // Start of 0x104a0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, 3083, 3084, 3085, 3086, 3087, 3088, 3089, 3090, 3091,
      3092, 3093, 3094, 3095, 3096, 3097, 3098,

      // Start of 0x104c0:
      3099, 3100, 3101, 3102, 3103, 3104, 3105, 3106, 3107, 3108, 3109, 3110, 3111, 3112, 3113, 3114, 3115,
      3116, 3117, 3118, disallowed, disallowed, disallowed, disallowed, valid, valid, valid, valid,

      // Start of 0x10560:
      valid, valid, valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, valid, 3119, 3120, 3121, 3122, 3123, 3124,
      3125, 3126, 3127, 3128, 3129, disallowed, 3130, 3131, 3132, 3133,

      // Start of 0x10580:
      3134, 3135, 3136, 3137, 3138, 3139, 3140, 3141, 3142, 3143, 3144, disallowed, 3145, 3146, 3147, 3148,
      3149, 3150, 3151, disallowed, 3152, 3153, disallowed, valid, valid, valid, valid, valid, valid, valid,
      valid,

      // Start of 0x10780:
      valid, 3154, 3155, 45, 3156, 133, disallowed, 3157, 3158, 3159, 3160, 138, 139, 3161, 3162, 3163, 3164,
      2054, 3165, 145, 3166, 88, 3167, 3168, 3169, 3170, 3171, 2038, 3172, 3173, 3174, 3175,

      // Start of 0x107a0:
      3176, 3177, 62, 3178, 3179, 16, 3180, 3181, 1092, 3182, 156, 3183, 3184, 3185, 3186, 160, 3187,
      disallowed, 3188, 3189, 3190, 3191, 3192, 3193, 3194, 3195, 3196, disallowed, disallowed, disallowed,
      disallowed, disallowed,

      // Start of 0x10c80:
      3197, 3198, 3199, 3200, 3201, 3202, 3203, 3204, 3205, 3206, 3207, 3208, 3209, 3210, 3211, 3212, 3213,
      3214, 3215, 3216, 3217, 3218, 3219, 3220, 3221, 3222, 3223, 3224, 3225, 3226, 3227, 3228,

      // Start of 0x10ca0:
      3229, 3230, 3231, 3232, 3233, 3234, 3235, 3236, 3237, 3238, 3239, 3240, 3241, 3242, 3243, 3244, 3245,
      3246, 3247, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x10d40:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 3248, 3249, 3250, 3251, 3252, 3253, 3254, 3255, 3256, 3257, 3258, 3259, 3260, 3261, 3262, 3263,

      // Start of 0x10d60:
      3264, 3265, 3266, 3267, 3268, 3269, disallowed, disallowed, disallowed, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid,

      // Start of 0x118a0:
      3270, 3271, 3272, 3273, 3274, 3275, 3276, 3277, 3278, 3279, 3280, 3281, 3282, 3283, 3284, 3285, 3286,
      3287, 3288, 3289, 3290, 3291, 3292, 3293, 3294, 3295, 3296, 3297, 3298, 3299, 3300, 3301,

      // Start of 0x16e40:
      3302, 3303, 3304, 3305, 3306, 3307, 3308, 3309, 3310, 3311, 3312, 3313, 3314, 3315, 3316, 3317, 3318,
      3319, 3320, 3321, 3322, 3323, 3324, 3325, 3326, 3327, 3328, 3329, 3330, 3331, 3332, 3333,

      // Start of 0x1bca0:
      28, 28, 28, 28, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x1ccc0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,

      // Start of 0x1cce0:
      10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 916, 35, 30, 31, 917, 918, 919, 920,
      921, 922, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x1d140:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      3334, 3335,

      // Start of 0x1d160:
      3336, 3337, 3338, 3339, 3340, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, 28, 28, 28, 28, 28, 28, 28, 28,

      // Start of 0x1d1a0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 3341, 3342, 3343,
      3344, 3345,

      // Start of 0x1d1c0:
      3346, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x1d440:
      12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 1, 2, 3, 4, 5, 6, disallowed, 8, 9, 10, 11,
      12, 13, 14, 15, 16, 17,

      // Start of 0x1d480:
      24, 25, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,
      disallowed, 2, 3,

      // Start of 0x1d4a0:
      disallowed, disallowed, 6, disallowed, disallowed, 9, 10, disallowed, disallowed, 13, 14, 15, 16,
      disallowed, 18, 19, 20, 21, 22, 23, 24, 25, 0, 1, 2, 3, disallowed, 5, disallowed, 7, 8, 9,

      // Start of 0x1d4c0:
      10, 11, 12, 13, disallowed, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15,

      // Start of 0x1d500:
      22, 23, 24, 25, 0, 1, disallowed, 3, 4, 5, 6, disallowed, disallowed, 9, 10, 11, 12, 13, 14, 15, 16,
      disallowed, 18, 19, 20, 21, 22, 23, 24, disallowed, 0, 1,

      // Start of 0x1d520:
      2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 1,
      disallowed, 3, 4, 5, 6, disallowed,

      // Start of 0x1d540:
      8, 9, 10, 11, 12, disallowed, 14, disallowed, disallowed, disallowed, 18, 19, 20, 21, 22, 23, 24,
      disallowed, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,

      // Start of 0x1d620:
      24, 25, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,
      1, 2, 3,

      // Start of 0x1d6a0:
      22, 23, 24, 25, 3347, 3348, disallowed, disallowed, 268, 269, 270, 271, 272, 273, 274, 275, 251, 276,
      277, 33, 278, 279, 280, 281, 282, 275, 283, 284, 285, 286, 287, 288,

      // Start of 0x1d6c0:
      289, 3349, 268, 269, 270, 271, 272, 273, 274, 275, 251, 276, 277, 33, 278, 279, 280, 281, 282, 283, 283,
      284, 285, 286, 287, 288, 289, 3350, 272, 275, 276, 286,

      // Start of 0x1d6e0:
      282, 281, 268, 269, 270, 271, 272, 273, 274, 275, 251, 276, 277, 33, 278, 279, 280, 281, 282, 275, 283,
      284, 285, 286, 287, 288, 289, 3349, 268, 269, 270, 271,

      // Start of 0x1d7c0:
      287, 288, 289, 3350, 272, 275, 276, 286, 282, 281, 295, 295, disallowed, disallowed, 916, 35, 30, 31,
      917, 918, 919, 920,

      // Start of 0x1d7e0:
      921, 922, 916, 35, 30, 31, 917, 918, 919, 920, 921, 922, 916, 35, 30, 31, 917, 918, 919, 920, 921, 922,
      916, 35, 30, 31, 917, 918, 919, 920, 921, 922,

      // Start of 0x1e020:
      valid, valid, disallowed, valid, valid, disallowed, valid, valid, valid, valid, valid, disallowed,
      disallowed, disallowed, disallowed, disallowed, 326, 327, 328, 329, 330, 331, 332, 333, 334, 336, 337,
      338, 340, 341, 342, 343,

      // Start of 0x1e040:
      344, 345, 346, 347, 348, 349, 350, 353, 355, 356, 1969, 414, 316, 318, 422, 393, 402, 326, 327, 328,
      329, 330, 331, 332, 333, 334, 336, 337, 340, 341, 343, 345,

      // Start of 0x1e060:
      346, 347, 348, 349, 350, 352, 353, 378, 316, 315, 325, 391, 1950, 394, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x1e900:
      3351, 3352, 3353, 3354, 3355, 3356, 3357, 3358, 3359, 3360, 3361, 3362, 3363, 3364, 3365, 3366, 3367,
      3368, 3369, 3370, 3371, 3372, 3373, 3374, 3375, 3376, 3377, 3378, 3379, 3380, 3381, 3382,

      // Start of 0x1e920:
      3383, 3384, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x1ee00:
      2975, 2976, 2980, 2983, disallowed, 3002, 2986, 2981, 2991, 3003, 2997, 2998, 2999, 3000, 2987, 2993,
      2995, 2989, 2996, 2985, 2988, 2978, 2979, 2982, 2984, 2990, 2992, 2994, 3385, 2627, 3386, 3387,

      // Start of 0x1ee20:
      disallowed, 2976, 2980, disallowed, 3001, disallowed, disallowed, 2981, disallowed, 3003, 2997, 2998,
      2999, 3000, 2987, 2993, 2995, 2989, 2996, disallowed, 2988, 2978, 2979, 2982, disallowed, 2990,
      disallowed, 2994, disallowed, disallowed,

      // Start of 0x1ee40:
      disallowed, disallowed, 2980, disallowed, disallowed, disallowed, disallowed, 2981, disallowed, 3003,
      disallowed, 2998, disallowed, 3000, 2987, 2993, disallowed, 2989, 2996, disallowed, 2988, disallowed,
      disallowed, 2982, disallowed, 2990, disallowed, 2994, disallowed, 2627, disallowed, 3387,

      // Start of 0x1ee60:
      disallowed, 2976, 2980, disallowed, 3001, disallowed, disallowed, 2981, 2991, 3003, 2997, disallowed,
      2999, 3000, 2987, 2993, 2995, 2989, 2996, disallowed, 2988, 2978, 2979, 2982, disallowed, 2990, 2992,
      2994, 3385, disallowed, 3386, disallowed,

      // Start of 0x1ee80:
      2975, 2976, 2980, 2983, 3001, 3002, 2986, 2981, 2991, 3003, disallowed, 2998, 2999, 3000, 2987, 2993,
      2995, 2989, 2996, 2985, 2988, 2978, 2979, 2982, 2984, 2990, 2992, 2994, disallowed, disallowed,
      disallowed,

      // Start of 0x1eea0:
      disallowed, 2976, 2980, 2983, disallowed, 3002, 2986, 2981, 2991, 3003, disallowed, 2998, 2999, 3000,
      2987, 2993, 2995, 2989, 2996, 2985, 2988, 2978, 2979, 2982, 2984, 2990, 2992, 2994, disallowed,
      disallowed, disallowed,

      // Start of 0x1f100:
      disallowed, 3388, 3389, 3390, 3391, 3392, 3393, 3394, 3395, 3396, 3397, valid, valid, valid, valid,
      valid, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023, 1024, 1025,

      // Start of 0x1f120:
      1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 3398, 2, 17, 1888, 3399, valid, 0, 1, 2, 3,
      4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,

      // Start of 0x1f140:
      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 3400, 1877, 3401, 3402, 3403, 3404, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x1f160:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 3405, 3406, 3407, valid, valid,
      valid,

      // Start of 0x1f180:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 3408, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x1f200:
      3409, 3410, 1657, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 1219, 3411, 3412, 3413, 1162,
      3414, 3415, 1479, 3416, 3417, 3418, 2323, 3419, 3420, 3421, 3422,

      // Start of 0x1f220:
      3423, 3424, 1255, 3425, 3426, 3427, 3428, 3429, 3430, 1156, 1471, 3431, 1606, 1474, 1607, 3432, 1311,
      3433, 3434, 3435, 3436, 3437, 1589, 1229, 3438, 3439, 3440, 3441, disallowed, disallowed, disallowed,
      disallowed,

      // Start of 0x1f240:
      3442, 3443, 3444, 3445, 3446, 3447, 3448, 3449, 3450, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, 3451, 3452, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed,

      // Start of 0x1fbe0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 916, 35, 30, 31, 917, 918, 919, 920, 921, 922, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed,

      // Start of 0x2f800:
      3453, 3454, 3455, 3456, 3457, 2416, 3458, 3459, 3460, 3461, 2417, 3462, 3463, 3464, 2418, 3465, 3466,
      3467, 3468, 3469, 3470, 3421, 3471, 3472, 3473, 3474, 3475, 2473, 3476, 1172, 3477, 3478,

      // Start of 0x2f820:
      3479, 3480, 3439, 3481, 3482, 2478, 2419, 2420, 2479, 3483, 3484, 2237, 3485, 2421, 3486, 3487, 3488,
      3489, 3489, 3489, 3490, 3491, 3492, 3493, 3494, 3495, 3496, 3497, 3498, 3499, 3500, 3501,

      // Start of 0x2f840:
      3502, 3503, 3504, 3505, 3506, 3507, 3507, 2481, 3508, 3509, 3510, 3511, 2423, 3512, 3513, 3514, 2383,
      3515, 3516, 3517, 3518, 3519, 3520, 3521, 3522, 3523, 3524, 3525, 3526, 3414, 3527, 3528,

      // Start of 0x2f860:
      3529, 3530, 3531, 3532, 3533, 3534, 3535, 3536, 3537, 3538, 3539, 3539, 3540, 3541, 3542, 2233, 3543,
      3544, 3545, 3546, 3547, 1198, 3548, 3549, 1200, 3550, 3551, 3552, 3553, 3554, 3555, 3556,

      // Start of 0x2f880:
      3557, 3558, 3559, 3560, 3561, 3562, 3563, 3564, 3565, 3566, 3567, 3568, 3569, 3570, 2181, 3571, 1210,
      3572, 3572, 3573, 3574, 3574, 3575, 3576, 3577, 3578, 3579, 3580, 3581, 3582, 3583, 3584,

      // Start of 0x2f8a0:
      3585, 3586, 3587, 2428, 3588, 3589, 3590, 3591, 2493, 3591, 3592, 2430, 3593, 3594, 3595, 3596, 2431,
      2154, 3597, 3598, 3599, 3600, 3601, 3602, 3603, 3604, 3605, 3606, 3607, 3608, 3609, 3610,

      // Start of 0x2f8c0:
      3611, 3612, 3613, 3614, 3615, 3616, 3617, 3618, 2432, 3619, 3620, 3621, 3622, 3623, 3624, 2434, 3625,
      3626, 3627, 3628, 3629, 3630, 3631, 3632, 2182, 2501, 3633, 3634, 3635, 3636, 3637, 3638,

      // Start of 0x2f8e0:
      3639, 3640, 2435, 3641, 3642, 3643, 3644, 2543, 3645, 3646, 3647, 3648, 3649, 3650, 3651, 3652, 3653,
      3654, 3655, 3656, 3657, 2250, 3658, 3659, 3660, 3661, 3662, 3663, 3664, 3665, 3666, 3667,

      // Start of 0x2f900:
      3668, 2436, 2333, 3669, 3670, 3671, 3672, 3673, 3674, 3675, 3676, 2504, 3677, 3678, 3679, 3680, 3681,
      3682, 3683, 3684, 2505, 3685, 3686, 3687, 3688, 3689, 3690, 3691, 3692, 3693, 3694, 3695,

      // Start of 0x2f920:
      3696, 2507, 3697, 3698, 3699, 3700, 3701, 3702, 3703, 3704, 3705, 3706, 3707, 3707, 3708, 3709, 2509,
      3710, 3711, 3712, 3713, 3714, 3715, 3716, 2236, 3717, 3718, 3719, 3720, 3721, 3722, 3723,

      // Start of 0x2f940:
      2515, 3724, 3725, 3726, 3727, 3728, 3729, 3729, 2516, 2545, 3730, 3731, 3732, 3733, 3734, 2199, 2518,
      3735, 3736, 2446, 3737, 3738, 2403, 3739, 3740, 2449, 3741, 3742, 3743, 3744, 3744, 3745,

      // Start of 0x2f960:
      3746, 3747, 3748, 3749, 3750, 3751, 3752, 3753, 3754, 3755, 3756, 3757, 3758, 3759, 3760, 3761, 3762,
      3763, 3764, 3765, 3766, 3767, 3768, 3769, 3770, 3771, 2455, 3772, 3773, 3774, 3775, 3776,

      // Start of 0x2f980:
      3777, 3778, 3779, 3780, 3781, 3782, 3783, 3784, 3785, 3786, 3787, 3573, 3788, 3789, 3790, 3791, 3792,
      3793, 3794, 3795, 3796, 3797, 3798, 3799, 2253, 3800, 3801, 3802, 3803, 3804, 3805, 2458,

      // Start of 0x2f9a0:
      3806, 3807, 3808, 3809, 3810, 3811, 3812, 3813, 3814, 3815, 3816, 3817, 3818, 3819, 3820, 3821, 3822,
      3823, 3824, 3825, 2194, 3826, 3827, 3828, 3829, 3830, 3831, 2525, 3832, 3833, 3834, 3835,

      // Start of 0x2f9c0:
      3836, 3837, 3838, 3839, 1300, 3840, 3841, 3842, 3843, 3844, 3845, 3846, 3847, 3848, 3849, 3850, 2530,
      2531, 1307, 3851, 3852, 3853, 3854, 3855, 3856, 3857, 3858, 3859, 3860, 3861, 3862, 2532,

      // Start of 0x2f9e0:
      3863, 3864, 3865, 3866, 3867, 3868, 3869, 3870, 3871, 3872, 3873, 3874, 3875, 3876, 3877, 3878, 3879,
      3880, 3881, 3882, 3883, 3884, 3885, 3886, 3887, 3888, 3889, 3890, 3891, 3892, 2538, 2538,

      // Start of 0x2fa00:
      3893, 3894, 3895, 3896, 3897, 3898, 3899, 3900, 3901, 3902, 2539, 3903, 3904, 3905, 3906, 3907, 3908,
      3909, 3910, 3911, 3912, 1355, 3913, 1359, 3914, 3915, 3916, 3917, 1364, 3918, disallowed, disallowed,

      // Start of 0xe0100:
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28,
      // done
    };





    /**
     * IDNA Mapping Status Table
     *
     * Table size:
     *   - in bits:       544
     *   - in bytes:      68 B
     *   - in KibiBytes:  0.07 KiB
     */
    static constexpr std::array<std::uint32_t, 17ULL> idna_mappings_bools{
      0b1111'1111'1111'1111'1111'1111'1111'1111, 0b1111'1111'1111'1111'1111'1111'1111'1111,
      0b1111'1111'1111'1111'1111'1111'1111'1111, 0b1111'1111'1111'1111'1111'1111'1111'1111,
      0b1111'1111'1111'1111'1111'1111'1111'1111, 0b1111'1111'1111'1111'0000'0000'1111'1111,
      0b1111'1111'1100'0000'0000'0011'1111'0000, 0b1111'1111'1111'1111'1111'1111'1111'1111,
      0b1111'1111'1111'1111'1111'1111'1111'1111, 0b0000'0000'0000'0001'1111'1111'1111'1111,
      0b1111'1111'1111'1111'1110'0000'0000'0000, 0b0111'1111'1111'1111'1111'1111'1111'1111,
      0b1111'1111'0000'0000'0000'0000'0000'0000, 0b1011'1111'1111'1110'1000'0000'0011'1111,
      0b0000'0011'1111'1111'1110'0110'0111'1111, 0b0000'0000'0000'0010'0000'0000'0000'0000,
      0b0000'0000'0000'0000'0000'0000'0110'0000,
      // done
    };


    /**
     * IDNA Mappings
     *
     * Table size:
     *   - in bits:       137968
     *   - in bytes:      17246 B
     *   - in KibiBytes:  16.84 KiB
     */
    static constexpr std::basic_string_view<char8_t> idna_mappings{
      u8"\x61\0" // 41, AA, 1D2C, 1D43, 2090, 24B6, 24D0, FF21, FF41, 1CCD6, 1D400, 1D41A, 1D434, 1D44E,
                 // 1D468, 1D482, 1D49C, 1D4B6, 1D4D0, 1D4EA, 1D504, 1D51E, 1D538, 1D552, 1D56C, 1D586, 1D5A0,
                 // 1D5BA, 1D5D4, 1D5EE, 1D608, 1D622, 1D63C, 1D656, 1D670, 1D68A, 1F130
      u8"\x62\0" // 42, 1D2E, 1D47, 212C, 24B7, 24D1, FF22, FF42, 1CCD7, 1D401, 1D41B, 1D435, 1D44F, 1D469,
                 // 1D483, 1D4B7, 1D4D1, 1D4EB, 1D505, 1D51F, 1D539, 1D553, 1D56D, 1D587, 1D5A1, 1D5BB, 1D5D5,
                 // 1D5EF, 1D609, 1D623, 1D63D, 1D657, 1D671, 1D68B, 1F131
      u8"\x63\0" // 43, 1D9C, 2102, 212D, 216D, 217D, 24B8, 24D2, A7F2, FF23, FF43, 1CCD8, 1D402, 1D41C,
                 // 1D436, 1D450, 1D46A, 1D484, 1D49E, 1D4B8, 1D4D2, 1D4EC, 1D520, 1D554, 1D56E, 1D588, 1D5A2,
                 // 1D5BC, 1D5D6, 1D5F0, 1D60A, 1D624, 1D63E, 1D658, 1D672, 1D68C, 1F12B, 1F132
      u8"\x64\0" // 44, 1D30, 1D48, 2145, 2146, 216E, 217E, 24B9, 24D3, FF24, FF44, 1CCD9, 1D403, 1D41D,
                 // 1D437, 1D451, 1D46B, 1D485, 1D49F, 1D4B9, 1D4D3, 1D4ED, 1D507, 1D521, 1D53B, 1D555, 1D56F,
                 // 1D589, 1D5A3, 1D5BD, 1D5D7, 1D5F1, 1D60B, 1D625, 1D63F, 1D659, 1D673, 1D68D, 1F133
      u8"\x65\0" // 45, 1D31, 1D49, 2091, 212F, 2130, 2147, 24BA, 24D4, FF25, FF45, 1CCDA, 1D404, 1D41E,
                 // 1D438, 1D452, 1D46C, 1D486, 1D4D4, 1D4EE, 1D508, 1D522, 1D53C, 1D556, 1D570, 1D58A, 1D5A4,
                 // 1D5BE, 1D5D8, 1D5F2, 1D60C, 1D626, 1D640, 1D65A, 1D674, 1D68E, 1F134
      u8"\x66\0" // 46, 1DA0, 2131, 24BB, 24D5, A7F3, FF26, FF46, 1CCDB, 1D405, 1D41F, 1D439, 1D453, 1D46D,
                 // 1D487, 1D4BB, 1D4D5, 1D4EF, 1D509, 1D523, 1D53D, 1D557, 1D571, 1D58B, 1D5A5, 1D5BF, 1D5D9,
                 // 1D5F3, 1D60D, 1D627, 1D641, 1D65B, 1D675, 1D68F, 1F135
      u8"\x67\0" // 47, 1D33, 1D4D, 210A, 24BC, 24D6, FF27, FF47, 1CCDC, 1D406, 1D420, 1D43A, 1D454, 1D46E,
                 // 1D488, 1D4A2, 1D4D6, 1D4F0, 1D50A, 1D524, 1D53E, 1D558, 1D572, 1D58C, 1D5A6, 1D5C0, 1D5DA,
                 // 1D5F4, 1D60E, 1D628, 1D642, 1D65C, 1D676, 1D690, 1F136
      u8"\x68\0" // 48, 2B0, 1D34, 2095, 210B, 210C, 210D, 210E, 24BD, 24D7, FF28, FF48, 1CCDD, 1D407, 1D421,
                 // 1D43B, 1D46F, 1D489, 1D4BD, 1D4D7, 1D4F1, 1D525, 1D559, 1D573, 1D58D, 1D5A7, 1D5C1, 1D5DB,
                 // 1D5F5, 1D60F, 1D629, 1D643, 1D65D, 1D677, 1D691, 1F137
      u8"\x69\0" // 49, 1D35, 1D62, 2071, 2110, 2111, 2139, 2148, 2160, 2170, 24BE, 24D8, FF29, FF49, 1CCDE,
                 // 1D408, 1D422, 1D43C, 1D456, 1D470, 1D48A, 1D4BE, 1D4D8, 1D4F2, 1D526, 1D540, 1D55A, 1D574,
                 // 1D58E, 1D5A8, 1D5C2, 1D5DC, 1D5F6, 1D610, 1D62A, 1D644, 1D65E, 1D678, 1D692, 1F138
      u8"\x6a\0" // 4A, 2B2, 1D36, 2149, 24BF, 24D9, 2C7C, FF2A, FF4A, 1CCDF, 1D409, 1D423, 1D43D, 1D457,
                 // 1D471, 1D48B, 1D4A5, 1D4BF, 1D4D9, 1D4F3, 1D50D, 1D527, 1D541, 1D55B, 1D575, 1D58F, 1D5A9,
                 // 1D5C3, 1D5DD, 1D5F7, 1D611, 1D62B, 1D645, 1D65F, 1D679, 1D693, 1F139
      u8"\x6b\0" // 4B, 1D37, 1D4F, 2096, 212A, 24C0, 24DA, FF2B, FF4B, 1CCE0, 1D40A, 1D424, 1D43E, 1D458,
                 // 1D472, 1D48C, 1D4A6, 1D4C0, 1D4DA, 1D4F4, 1D50E, 1D528, 1D542, 1D55C, 1D576, 1D590, 1D5AA,
                 // 1D5C4, 1D5DE, 1D5F8, 1D612, 1D62C, 1D646, 1D660, 1D67A, 1D694, 1F13A
      u8"\x6c\0" // 4C, 2E1, 1D38, 2097, 2112, 2113, 216C, 217C, 24C1, 24DB, FF2C, FF4C, 1CCE1, 1D40B, 1D425,
                 // 1D43F, 1D459, 1D473, 1D48D, 1D4C1, 1D4DB, 1D4F5, 1D50F, 1D529, 1D543, 1D55D, 1D577, 1D591,
                 // 1D5AB, 1D5C5, 1D5DF, 1D5F9, 1D613, 1D62D, 1D647, 1D661, 1D67B, 1D695, 1F13B
      u8"\x6d\0" // 4D, 1D39, 1D50, 2098, 2133, 216F, 217F, 24C2, 24DC, FF2D, FF4D, 1CCE2, 1D40C, 1D426,
                 // 1D440, 1D45A, 1D474, 1D48E, 1D4C2, 1D4DC, 1D4F6, 1D510, 1D52A, 1D544, 1D55E, 1D578, 1D592,
                 // 1D5AC, 1D5C6, 1D5E0, 1D5FA, 1D614, 1D62E, 1D648, 1D662, 1D67C, 1D696, 1F13C
      u8"\x6e\0" // 4E, 1D3A, 207F, 2099, 2115, 24C3, 24DD, FF2E, FF4E, 1CCE3, 1D40D, 1D427, 1D441, 1D45B,
                 // 1D475, 1D48F, 1D4A9, 1D4C3, 1D4DD, 1D4F7, 1D511, 1D52B, 1D55F, 1D579, 1D593, 1D5AD, 1D5C7,
                 // 1D5E1, 1D5FB, 1D615, 1D62F, 1D649, 1D663, 1D67D, 1D697, 1F13D
      u8"\x6f\0" // 4F, BA, 1D3C, 1D52, 2092, 2134, 24C4, 24DE, FF2F, FF4F, 1CCE4, 1D40E, 1D428, 1D442, 1D45C,
                 // 1D476, 1D490, 1D4AA, 1D4DE, 1D4F8, 1D512, 1D52C, 1D546, 1D560, 1D57A, 1D594, 1D5AE, 1D5C8,
                 // 1D5E2, 1D5FC, 1D616, 1D630, 1D64A, 1D664, 1D67E, 1D698, 1F13E
      u8"\x70\0" // 50, 1D3E, 1D56, 209A, 2119, 24C5, 24DF, FF30, FF50, 1CCE5, 1D40F, 1D429, 1D443, 1D45D,
                 // 1D477, 1D491, 1D4AB, 1D4C5, 1D4DF, 1D4F9, 1D513, 1D52D, 1D561, 1D57B, 1D595, 1D5AF, 1D5C9,
                 // 1D5E3, 1D5FD, 1D617, 1D631, 1D64B, 1D665, 1D67F, 1D699, 1F13F
      u8"\x71\0" // 51, 211A, 24C6, 24E0, A7F4, FF31, FF51, 107A5, 1CCE6, 1D410, 1D42A, 1D444, 1D45E, 1D478,
                 // 1D492, 1D4AC, 1D4C6, 1D4E0, 1D4FA, 1D514, 1D52E, 1D562, 1D57C, 1D596, 1D5B0, 1D5CA, 1D5E4,
                 // 1D5FE, 1D618, 1D632, 1D64C, 1D666, 1D680, 1D69A, 1F140
      u8"\x72\0" // 52, 2B3, 1D3F, 1D63, 211B, 211C, 211D, 24C7, 24E1, FF32, FF52, 1CCE7, 1D411, 1D42B, 1D445,
                 // 1D45F, 1D479, 1D493, 1D4C7, 1D4E1, 1D4FB, 1D52F, 1D563, 1D57D, 1D597, 1D5B1, 1D5CB, 1D5E5,
                 // 1D5FF, 1D619, 1D633, 1D64D, 1D667, 1D681, 1D69B, 1F12C, 1F141
      u8"\x73\0" // 53, 17F, 2E2, 209B, 24C8, 24E2, FF33, FF53, 1CCE8, 1D412, 1D42C, 1D446, 1D460, 1D47A,
                 // 1D494, 1D4AE, 1D4C8, 1D4E2, 1D4FC, 1D516, 1D530, 1D54A, 1D564, 1D57E, 1D598, 1D5B2, 1D5CC,
                 // 1D5E6, 1D600, 1D61A, 1D634, 1D64E, 1D668, 1D682, 1D69C, 1F142
      u8"\x74\0" // 54, 1D40, 1D57, 209C, 24C9, 24E3, FF34, FF54, 1CCE9, 1D413, 1D42D, 1D447, 1D461, 1D47B,
                 // 1D495, 1D4AF, 1D4C9, 1D4E3, 1D4FD, 1D517, 1D531, 1D54B, 1D565, 1D57F, 1D599, 1D5B3, 1D5CD,
                 // 1D5E7, 1D601, 1D61B, 1D635, 1D64F, 1D669, 1D683, 1D69D, 1F143
      u8"\x75\0" // 55, 1D41, 1D58, 1D64, 24CA, 24E4, FF35, FF55, 1CCEA, 1D414, 1D42E, 1D448, 1D462, 1D47C,
                 // 1D496, 1D4B0, 1D4CA, 1D4E4, 1D4FE, 1D518, 1D532, 1D54C, 1D566, 1D580, 1D59A, 1D5B4, 1D5CE,
                 // 1D5E8, 1D602, 1D61C, 1D636, 1D650, 1D66A, 1D684, 1D69E, 1F144
      u8"\x76\0" // 56, 1D5B, 1D65, 2164, 2174, 24CB, 24E5, 2C7D, FF36, FF56, 1CCEB, 1D415, 1D42F, 1D449,
                 // 1D463, 1D47D, 1D497, 1D4B1, 1D4CB, 1D4E5, 1D4FF, 1D519, 1D533, 1D54D, 1D567, 1D581, 1D59B,
                 // 1D5B5, 1D5CF, 1D5E9, 1D603, 1D61D, 1D637, 1D651, 1D66B, 1D685, 1D69F, 1F145
      u8"\x77\0" // 57, 2B7, 1D42, 24CC, 24E6, FF37, FF57, 1CCEC, 1D416, 1D430, 1D44A, 1D464, 1D47E, 1D498,
                 // 1D4B2, 1D4CC, 1D4E6, 1D500, 1D51A, 1D534, 1D54E, 1D568, 1D582, 1D59C, 1D5B6, 1D5D0, 1D5EA,
                 // 1D604, 1D61E, 1D638, 1D652, 1D66C, 1D686, 1D6A0, 1F146
      u8"\x78\0" // 58, 2E3, 2093, 2169, 2179, 24CD, 24E7, FF38, FF58, 1CCED, 1D417, 1D431, 1D44B, 1D465,
                 // 1D47F, 1D499, 1D4B3, 1D4CD, 1D4E7, 1D501, 1D51B, 1D535, 1D54F, 1D569, 1D583, 1D59D, 1D5B7,
                 // 1D5D1, 1D5EB, 1D605, 1D61F, 1D639, 1D653, 1D66D, 1D687, 1D6A1, 1F147
      u8"\x79\0" // 59, 2B8, 24CE, 24E8, FF39, FF59, 1CCEE, 1D418, 1D432, 1D44C, 1D466, 1D480, 1D49A, 1D4B4,
                 // 1D4CE, 1D4E8, 1D502, 1D51C, 1D536, 1D550, 1D56A, 1D584, 1D59E, 1D5B8, 1D5D2, 1D5EC, 1D606,
                 // 1D620, 1D63A, 1D654, 1D66E, 1D688, 1D6A2, 1F148
      u8"\x7a\0" // 5A, 1DBB, 2124, 2128, 24CF, 24E9, FF3A, FF5A, 1CCEF, 1D419, 1D433, 1D44D, 1D467, 1D481,
                 // 1D49B, 1D4B5, 1D4CF, 1D4E9, 1D503, 1D537, 1D56B, 1D585, 1D59F, 1D5B9, 1D5D3, 1D5ED, 1D607,
                 // 1D621, 1D63B, 1D655, 1D66F, 1D689, 1D6A3, 1F149
      u8"\40\0"  // A0, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 200A, 202F, 205F, 3000
      u8"\40\xcc\x88\0" // A8
      u8"\0" // AD, 34F, 115F, 1160, 17B4, 17B5, 180B, 180C, 180D, 180E, 180F, 200B, 2060, 2061, 2062, 2063,
             // 2064, 206A, 206B, 206C, 206D, 206E, 206F, 3164, FE00, FE01, FE02, FE03, FE04, FE05, FE06,
             // FE07, FE08, FE09, FE0A, FE0B, FE0C, FE0D, FE0E, FE0F, FEFF, FFA0, 1BCA0, 1BCA1, 1BCA2, 1BCA3,
             // 1D173, 1D174, 1D175, 1D176, 1D177, 1D178, 1D179, 1D17A, E0100, E0101, E0102, E0103, E0104,
             // E0105, E0106, E0107, E0108, E0109, E010A, E010B, E010C, E010D, E010E, E010F, E0110, E0111,
             // E0112, E0113, E0114, E0115, E0116, E0117, E0118, E0119, E011A, E011B, E011C, E011D, E011E,
             // E011F, E0120, E0121, E0122, E0123, E0124, E0125, E0126, E0127, E0128, E0129, E012A, E012B,
             // E012C, E012D, E012E, E012F, E0130, E0131, E0132, E0133, E0134, E0135, E0136, E0137, E0138,
             // E0139, E013A, E013B, E013C, E013D, E013E, E013F, E0140, E0141, E0142, E0143, E0144, E0145,
             // E0146, E0147, E0148, E0149, E014A, E014B, E014C, E014D, E014E, E014F, E0150, E0151, E0152,
             // E0153, E0154, E0155, E0156, E0157, E0158, E0159, E015A, E015B, E015C, E015D, E015E, E015F,
             // E0160, E0161, E0162, E0163, E0164, E0165, E0166, E0167, E0168, E0169, E016A, E016B, E016C,
             // E016D, E016E, E016F, E0170, E0171, E0172, E0173, E0174, E0175, E0176, E0177, E0178, E0179,
             // E017A, E017B, E017C, E017D, E017E, E017F, E0180, E0181, E0182, E0183, E0184, E0185, E0186,
             // E0187, E0188, E0189, E018A, E018B, E018C, E018D, E018E, E018F, E0190, E0191, E0192, E0193,
             // E0194, E0195, E0196, E0197, E0198, E0199, E019A, E019B, E019C, E019D, E019E, E019F, E01A0,
             // E01A1, E01A2, E01A3, E01A4, E01A5, E01A6, E01A7, E01A8, E01A9, E01AA, E01AB, E01AC, E01AD,
             // E01AE, E01AF, E01B0, E01B1, E01B2, E01B3, E01B4, E01B5, E01B6, E01B7, E01B8, E01B9, E01BA,
             // E01BB, E01BC, E01BD, E01BE, E01BF, E01C0, E01C1, E01C2, E01C3, E01C4, E01C5, E01C6, E01C7,
             // E01C8, E01C9, E01CA, E01CB, E01CC, E01CD, E01CE, E01CF, E01D0, E01D1, E01D2, E01D3, E01D4,
             // E01D5, E01D6, E01D7, E01D8, E01D9, E01DA, E01DB, E01DC, E01DD, E01DE, E01DF, E01E0, E01E1,
             // E01E2, E01E3, E01E4, E01E5, E01E6, E01E7, E01E8, E01E9, E01EA, E01EB, E01EC, E01ED, E01EE,
             // E01EF
      u8"\40\xcc\x84\0" // AF, FFE3
      u8"\62\0"         // B2, 2082, 2461, FF12, 1CCF2, 1D7D0, 1D7DA, 1D7E4, 1D7EE, 1D7F8, 1FBF2
      u8"\63\0"         // B3, 2083, 2462, FF13, 1CCF3, 1D7D1, 1D7DB, 1D7E5, 1D7EF, 1D7F9, 1FBF3
      u8"\40\xcc\x81\0" // B4, 384, 1FFD
      u8"\xce\xbc\0"    // B5, 39C, 1D6B3, 1D6CD, 1D6ED, 1D707, 1D727, 1D741, 1D761, 1D77B, 1D79B, 1D7B5
      u8"\40\xcc\xa7\0" // B8
      u8"\61\0"         // B9, 2081, 2460, FF11, 1CCF1, 1D7CF, 1D7D9, 1D7E3, 1D7ED, 1D7F7, 1FBF1
      u8"\61\xe2\x81\x84\64\0" // BC
      u8"\61\xe2\x81\x84\62\0" // BD
      u8"\63\xe2\x81\x84\64\0" // BE
      u8"\xc3\xa0\0"           // C0
      u8"\xc3\xa1\0"           // C1
      u8"\xc3\xa2\0"           // C2
      u8"\xc3\xa3\0"           // C3
      u8"\xc3\xa4\0"           // C4
      u8"\xc3\xa5\0"           // C5, 212B
      u8"\xc3\xa6\0"           // C6, 1D2D, 10783
      u8"\xc3\xa7\0"           // C7
      u8"\xc3\xa8\0"           // C8
      u8"\xc3\xa9\0"           // C9
      u8"\xc3\xaa\0"           // CA
      u8"\xc3\xab\0"           // CB
      u8"\xc3\xac\0"           // CC
      u8"\xc3\xad\0"           // CD
      u8"\xc3\xae\0"           // CE
      u8"\xc3\xaf\0"           // CF
      u8"\xc3\xb0\0"           // D0, 1D9E
      u8"\xc3\xb1\0"           // D1
      u8"\xc3\xb2\0"           // D2
      u8"\xc3\xb3\0"           // D3
      u8"\xc3\xb4\0"           // D4
      u8"\xc3\xb5\0"           // D5
      u8"\xc3\xb6\0"           // D6
      u8"\xc3\xb8\0"           // D8, 107A2
      u8"\xc3\xb9\0"           // D9
      u8"\xc3\xba\0"           // DA
      u8"\xc3\xbb\0"           // DB
      u8"\xc3\xbc\0"           // DC
      u8"\xc3\xbd\0"           // DD
      u8"\xc3\xbe\0"           // DE
      u8"\xc4\x81\0"           // 100
      u8"\xc4\x83\0"           // 102
      u8"\xc4\x85\0"           // 104
      u8"\xc4\x87\0"           // 106
      u8"\xc4\x89\0"           // 108
      u8"\xc4\x8b\0"           // 10A
      u8"\xc4\x8d\0"           // 10C
      u8"\xc4\x8f\0"           // 10E
      u8"\xc4\x91\0"           // 110
      u8"\xc4\x93\0"           // 112
      u8"\xc4\x95\0"           // 114
      u8"\xc4\x97\0"           // 116
      u8"\xc4\x99\0"           // 118
      u8"\xc4\x9b\0"           // 11A
      u8"\xc4\x9d\0"           // 11C
      u8"\xc4\x9f\0"           // 11E
      u8"\xc4\xa1\0"           // 120
      u8"\xc4\xa3\0"           // 122
      u8"\xc4\xa5\0"           // 124
      u8"\xc4\xa7\0"           // 126, 210F, A7F8, 10795
      u8"\xc4\xa9\0"           // 128
      u8"\xc4\xab\0"           // 12A
      u8"\xc4\xad\0"           // 12C
      u8"\xc4\xaf\0"           // 12E
      u8"\x69\xcc\x87\0"       // 130
      u8"\x69\x6a\0"           // 132, 133
      u8"\xc4\xb5\0"           // 134
      u8"\xc4\xb7\0"           // 136
      u8"\xc4\xba\0"           // 139
      u8"\xc4\xbc\0"           // 13B
      u8"\xc4\xbe\0"           // 13D
      u8"\x6c\xc2\xb7\0"       // 13F, 140
      u8"\xc5\x82\0"           // 141
      u8"\xc5\x84\0"           // 143
      u8"\xc5\x86\0"           // 145
      u8"\xc5\x88\0"           // 147
      u8"\xca\xbc\x6e\0"       // 149
      u8"\xc5\x8b\0"           // 14A, 1D51
      u8"\xc5\x8d\0"           // 14C
      u8"\xc5\x8f\0"           // 14E
      u8"\xc5\x91\0"           // 150
      u8"\xc5\x93\0"           // 152, A7F9
      u8"\xc5\x95\0"           // 154
      u8"\xc5\x97\0"           // 156
      u8"\xc5\x99\0"           // 158
      u8"\xc5\x9b\0"           // 15A
      u8"\xc5\x9d\0"           // 15C
      u8"\xc5\x9f\0"           // 15E
      u8"\xc5\xa1\0"           // 160
      u8"\xc5\xa3\0"           // 162
      u8"\xc5\xa5\0"           // 164
      u8"\xc5\xa7\0"           // 166
      u8"\xc5\xa9\0"           // 168
      u8"\xc5\xab\0"           // 16A
      u8"\xc5\xad\0"           // 16C
      u8"\xc5\xaf\0"           // 16E
      u8"\xc5\xb1\0"           // 170
      u8"\xc5\xb3\0"           // 172
      u8"\xc5\xb5\0"           // 174
      u8"\xc5\xb7\0"           // 176
      u8"\xc3\xbf\0"           // 178
      u8"\xc5\xba\0"           // 179
      u8"\xc5\xbc\0"           // 17B
      u8"\xc5\xbe\0"           // 17D
      u8"\xc9\x93\0"           // 181, 10785
      u8"\xc6\x83\0"           // 182
      u8"\xc6\x85\0"           // 184
      u8"\xc9\x94\0"           // 186, 1D53
      u8"\xc6\x88\0"           // 187
      u8"\xc9\x96\0"           // 189, 1078B
      u8"\xc9\x97\0"           // 18A, 1078C
      u8"\xc6\x8c\0"           // 18B
      u8"\xc7\x9d\0"           // 18E, 1D32
      u8"\xc9\x99\0"           // 18F, 1D4A, 2094
      u8"\xc9\x9b\0"           // 190, 1D4B, 2107
      u8"\xc6\x92\0"           // 191
      u8"\xc9\xa0\0"           // 193, 10793
      u8"\xc9\xa3\0"           // 194, 2E0
      u8"\xc9\xa9\0"           // 196, 1DA5
      u8"\xc9\xa8\0"           // 197, 1DA4
      u8"\xc6\x99\0"           // 198
      u8"\xc9\xaf\0"           // 19C, 1D5A
      u8"\xc9\xb2\0"           // 19D, 1DAE
      u8"\xc9\xb5\0"           // 19F, 1DB1
      u8"\xc6\xa1\0"           // 1A0
      u8"\xc6\xa3\0"           // 1A2
      u8"\xc6\xa5\0"           // 1A4
      u8"\xca\x80\0"           // 1A6, 107AA
      u8"\xc6\xa8\0"           // 1A7
      u8"\xca\x83\0"           // 1A9, 1DB4
      u8"\xc6\xad\0"           // 1AC
      u8"\xca\x88\0"           // 1AE, 107AF
      u8"\xc6\xb0\0"           // 1AF
      u8"\xca\x8a\0"           // 1B1, 1DB7
      u8"\xca\x8b\0"           // 1B2, 1DB9
      u8"\xc6\xb4\0"           // 1B3
      u8"\xc6\xb6\0"           // 1B5
      u8"\xca\x92\0"           // 1B7, 1DBE
      u8"\xc6\xb9\0"           // 1B8
      u8"\xc6\xbd\0"           // 1BC
      u8"\x64\xc5\xbe\0"       // 1C4, 1C5, 1C6
      u8"\x6c\x6a\0"           // 1C7, 1C8, 1C9
      u8"\x6e\x6a\0"           // 1CA, 1CB, 1CC
      u8"\xc7\x8e\0"           // 1CD
      u8"\xc7\x90\0"           // 1CF
      u8"\xc7\x92\0"           // 1D1
      u8"\xc7\x94\0"           // 1D3
      u8"\xc7\x96\0"           // 1D5
      u8"\xc7\x98\0"           // 1D7
      u8"\xc7\x9a\0"           // 1D9
      u8"\xc7\x9c\0"           // 1DB
      u8"\xc7\x9f\0"           // 1DE
      u8"\xc7\xa1\0"           // 1E0
      u8"\xc7\xa3\0"           // 1E2
      u8"\xc7\xa5\0"           // 1E4
      u8"\xc7\xa7\0"           // 1E6
      u8"\xc7\xa9\0"           // 1E8
      u8"\xc7\xab\0"           // 1EA
      u8"\xc7\xad\0"           // 1EC
      u8"\xc7\xaf\0"           // 1EE
      u8"\x64\x7a\0"           // 1F1, 1F2, 1F3
      u8"\xc7\xb5\0"           // 1F4
      u8"\xc6\x95\0"           // 1F6
      u8"\xc6\xbf\0"           // 1F7
      u8"\xc7\xb9\0"           // 1F8
      u8"\xc7\xbb\0"           // 1FA
      u8"\xc7\xbd\0"           // 1FC
      u8"\xc7\xbf\0"           // 1FE
      u8"\xc8\x81\0"           // 200
      u8"\xc8\x83\0"           // 202
      u8"\xc8\x85\0"           // 204
      u8"\xc8\x87\0"           // 206
      u8"\xc8\x89\0"           // 208
      u8"\xc8\x8b\0"           // 20A
      u8"\xc8\x8d\0"           // 20C
      u8"\xc8\x8f\0"           // 20E
      u8"\xc8\x91\0"           // 210
      u8"\xc8\x93\0"           // 212
      u8"\xc8\x95\0"           // 214
      u8"\xc8\x97\0"           // 216
      u8"\xc8\x99\0"           // 218
      u8"\xc8\x9b\0"           // 21A
      u8"\xc8\x9d\0"           // 21C
      u8"\xc8\x9f\0"           // 21E
      u8"\xc6\x9e\0"           // 220
      u8"\xc8\xa3\0"           // 222, 1D3D
      u8"\xc8\xa5\0"           // 224
      u8"\xc8\xa7\0"           // 226
      u8"\xc8\xa9\0"           // 228
      u8"\xc8\xab\0"           // 22A
      u8"\xc8\xad\0"           // 22C
      u8"\xc8\xaf\0"           // 22E
      u8"\xc8\xb1\0"           // 230
      u8"\xc8\xb3\0"           // 232
      u8"\xe2\xb1\xa5\0"       // 23A
      u8"\xc8\xbc\0"           // 23B
      u8"\xc6\x9a\0"           // 23D
      u8"\xe2\xb1\xa6\0"       // 23E
      u8"\xc9\x82\0"           // 241
      u8"\xc6\x80\0"           // 243
      u8"\xca\x89\0"           // 244, 1DB6
      u8"\xca\x8c\0"           // 245, 1DBA
      u8"\xc9\x87\0"           // 246
      u8"\xc9\x89\0"           // 248
      u8"\xc9\x8b\0"           // 24A
      u8"\xc9\x8d\0"           // 24C
      u8"\xc9\x8f\0"           // 24E
      u8"\xc9\xa6\0"           // 2B1, A7AA
      u8"\xc9\xb9\0"           // 2B4
      u8"\xc9\xbb\0"           // 2B5
      u8"\xca\x81\0"           // 2B6
      u8"\40\xcc\x86\0"        // 2D8
      u8"\40\xcc\x87\0"        // 2D9
      u8"\40\xcc\x8a\0"        // 2DA
      u8"\40\xcc\xa8\0"        // 2DB
      u8"\40\xcc\x83\0"        // 2DC
      u8"\40\xcc\x8b\0"        // 2DD
      u8"\xca\x95\0"           // 2E4
      u8"\xcc\x80\0"           // 340
      u8"\xcc\x81\0"           // 341
      u8"\xcc\x93\0"           // 343
      u8"\xcc\x88\xcc\x81\0"   // 344
      u8"\xce\xb9\0" // 345, 399, 1FBE, 1D6B0, 1D6CA, 1D6EA, 1D704, 1D724, 1D73E, 1D75E, 1D778, 1D798, 1D7B2
      u8"\xcd\xb1\0" // 370
      u8"\xcd\xb3\0" // 372
      u8"\xca\xb9\0" // 374
      u8"\xcd\xb7\0" // 376
      u8"\40\xce\xb9\0"         // 37A
      u8"\73\0"                 // 37E, FE14, FE54, FF1B
      u8"\xcf\xb3\0"            // 37F
      u8"\40\xcc\x88\xcc\x81\0" // 385, 1FEE
      u8"\xce\xac\0"            // 386, 1F71, 1FBB
      u8"\xc2\xb7\0"            // 387
      u8"\xce\xad\0"            // 388, 1F73, 1FC9
      u8"\xce\xae\0"            // 389, 1F75, 1FCB
      u8"\xce\xaf\0"            // 38A, 1F77, 1FDB
      u8"\xcf\x8c\0"            // 38C, 1F79, 1FF9
      u8"\xcf\x8d\0"            // 38E, 3D3, 1F7B, 1FEB
      u8"\xcf\x8e\0"            // 38F, 1F7D, 1FFB
      u8"\xce\xb1\0"            // 391, 1D6A8, 1D6C2, 1D6E2, 1D6FC, 1D71C, 1D736, 1D756, 1D770, 1D790, 1D7AA
      u8"\xce\xb2\0" // 392, 3D0, 1D5D, 1D66, 1D6A9, 1D6C3, 1D6E3, 1D6FD, 1D71D, 1D737, 1D757, 1D771, 1D791,
                     // 1D7AB
      u8"\xce\xb3\0" // 393, 1D5E, 1D67, 213D, 213E, 1D6AA, 1D6C4, 1D6E4, 1D6FE, 1D71E, 1D738, 1D758, 1D772,
                     // 1D792, 1D7AC
      u8"\xce\xb4\0" // 394, 1D5F, 1D6AB, 1D6C5, 1D6E5, 1D6FF, 1D71F, 1D739, 1D759, 1D773, 1D793, 1D7AD
      u8"\xce\xb5\0" // 395, 3F5, 1D6AC, 1D6C6, 1D6DC, 1D6E6, 1D700, 1D716, 1D720, 1D73A, 1D750, 1D75A, 1D774,
                     // 1D78A, 1D794, 1D7AE, 1D7C4
      u8"\xce\xb6\0" // 396, 1D6AD, 1D6C7, 1D6E7, 1D701, 1D721, 1D73B, 1D75B, 1D775, 1D795, 1D7AF
      u8"\xce\xb7\0" // 397, 1D6AE, 1D6C8, 1D6E8, 1D702, 1D722, 1D73C, 1D75C, 1D776, 1D796, 1D7B0
      u8"\xce\xb8\0" // 398, 3D1, 3F4, 1DBF, 1D6AF, 1D6B9, 1D6C9, 1D6DD, 1D6E9, 1D6F3, 1D703, 1D717, 1D723,
                     // 1D72D, 1D73D, 1D751, 1D75D, 1D767, 1D777, 1D78B, 1D797, 1D7A1, 1D7B1, 1D7C5
      u8"\xce\xba\0" // 39A, 3F0, 1D6B1, 1D6CB, 1D6DE, 1D6EB, 1D705, 1D718, 1D725, 1D73F, 1D752, 1D75F, 1D779,
                     // 1D78C, 1D799, 1D7B3, 1D7C6
      u8"\xce\xbb\0" // 39B, 1D6B2, 1D6CC, 1D6EC, 1D706, 1D726, 1D740, 1D760, 1D77A, 1D79A, 1D7B4
      u8"\xce\xbd\0" // 39D, 1D6B4, 1D6CE, 1D6EE, 1D708, 1D728, 1D742, 1D762, 1D77C, 1D79C, 1D7B6
      u8"\xce\xbe\0" // 39E, 1D6B5, 1D6CF, 1D6EF, 1D709, 1D729, 1D743, 1D763, 1D77D, 1D79D, 1D7B7
      u8"\xce\xbf\0" // 39F, 1D6B6, 1D6D0, 1D6F0, 1D70A, 1D72A, 1D744, 1D764, 1D77E, 1D79E, 1D7B8
      u8"\xcf\x80\0" // 3A0, 3D6, 213C, 213F, 1D6B7, 1D6D1, 1D6E1, 1D6F1, 1D70B, 1D71B, 1D72B, 1D745, 1D755,
                     // 1D765, 1D77F, 1D78F, 1D79F, 1D7B9, 1D7C9
      u8"\xcf\x81\0" // 3A1, 3F1, 1D68, 1D6B8, 1D6D2, 1D6E0, 1D6F2, 1D70C, 1D71A, 1D72C, 1D746, 1D754, 1D766,
                     // 1D780, 1D78E, 1D7A0, 1D7BA, 1D7C8
      u8"\xcf\x83\0" // 3A3, 3F2, 3F9, 1D6BA, 1D6D3, 1D6D4, 1D6F4, 1D70D, 1D70E, 1D72E, 1D747, 1D748, 1D768,
                     // 1D781, 1D782, 1D7A2, 1D7BB, 1D7BC
      u8"\xcf\x84\0" // 3A4, 1D6BB, 1D6D5, 1D6F5, 1D70F, 1D72F, 1D749, 1D769, 1D783, 1D7A3, 1D7BD
      u8"\xcf\x85\0" // 3A5, 3D2, 1D6BC, 1D6D6, 1D6F6, 1D710, 1D730, 1D74A, 1D76A, 1D784, 1D7A4, 1D7BE
      u8"\xcf\x86\0" // 3A6, 3D5, 1D60, 1D69, 1D6BD, 1D6D7, 1D6DF, 1D6F7, 1D711, 1D719, 1D731, 1D74B, 1D753,
                     // 1D76B, 1D785, 1D78D, 1D7A5, 1D7BF, 1D7C7
      u8"\xcf\x87\0" // 3A7, 1D61, 1D6A, 1D6BE, 1D6D8, 1D6F8, 1D712, 1D732, 1D74C, 1D76C, 1D786, 1D7A6, 1D7C0
      u8"\xcf\x88\0" // 3A8, 1D6BF, 1D6D9, 1D6F9, 1D713, 1D733, 1D74D, 1D76D, 1D787, 1D7A7, 1D7C1
      u8"\xcf\x89\0" // 3A9, 2126, 1D6C0, 1D6DA, 1D6FA, 1D714, 1D734, 1D74E, 1D76E, 1D788, 1D7A8, 1D7C2
      u8"\xcf\x8a\0" // 3AA
      u8"\xcf\x8b\0" // 3AB, 3D4
      u8"\xcf\x97\0" // 3CF
      u8"\xcf\x99\0" // 3D8
      u8"\xcf\x9b\0" // 3DA
      u8"\xcf\x9d\0" // 3DC, 1D7CA, 1D7CB
      u8"\xcf\x9f\0" // 3DE
      u8"\xcf\xa1\0" // 3E0
      u8"\xcf\xa3\0" // 3E2
      u8"\xcf\xa5\0" // 3E4
      u8"\xcf\xa7\0" // 3E6
      u8"\xcf\xa9\0" // 3E8
      u8"\xcf\xab\0" // 3EA
      u8"\xcf\xad\0" // 3EC
      u8"\xcf\xaf\0" // 3EE
      u8"\xcf\xb8\0" // 3F7
      u8"\xcf\xbb\0" // 3FA
      u8"\xcd\xbb\0" // 3FD
      u8"\xcd\xbc\0" // 3FE
      u8"\xcd\xbd\0" // 3FF
      u8"\xd1\x90\0" // 400
      u8"\xd1\x91\0" // 401
      u8"\xd1\x92\0" // 402
      u8"\xd1\x93\0" // 403
      u8"\xd1\x94\0" // 404
      u8"\xd1\x95\0" // 405, 1E069
      u8"\xd1\x96\0" // 406, 1E04C, 1E068
      u8"\xd1\x97\0" // 407
      u8"\xd1\x98\0" // 408, 1E04D
      u8"\xd1\x99\0" // 409
      u8"\xd1\x9a\0" // 40A
      u8"\xd1\x9b\0" // 40B
      u8"\xd1\x9c\0" // 40C
      u8"\xd1\x9d\0" // 40D
      u8"\xd1\x9e\0" // 40E
      u8"\xd1\x9f\0" // 40F, 1E06A
      u8"\xd0\xb0\0" // 410, 1E030, 1E051
      u8"\xd0\xb1\0" // 411, 1E031, 1E052
      u8"\xd0\xb2\0" // 412, 1C80, 1E032, 1E053
      u8"\xd0\xb3\0" // 413, 1E033, 1E054
      u8"\xd0\xb4\0" // 414, 1C81, 1E034, 1E055
      u8"\xd0\xb5\0" // 415, 1E035, 1E056
      u8"\xd0\xb6\0" // 416, 1E036, 1E057
      u8"\xd0\xb7\0" // 417, 1E037, 1E058
      u8"\xd0\xb8\0" // 418, 1E038, 1E059
      u8"\xd0\xb9\0" // 419
      u8"\xd0\xba\0" // 41A, 1E039, 1E05A
      u8"\xd0\xbb\0" // 41B, 1E03A, 1E05B
      u8"\xd0\xbc\0" // 41C, 1E03B
      u8"\xd0\xbd\0" // 41D, 1D78
      u8"\xd0\xbe\0" // 41E, 1C82, 1E03C, 1E05C
      u8"\xd0\xbf\0" // 41F, 1E03D, 1E05D
      u8"\xd1\x80\0" // 420, 1E03E
      u8"\xd1\x81\0" // 421, 1C83, 1E03F, 1E05E
      u8"\xd1\x82\0" // 422, 1C84, 1C85, 1E040
      u8"\xd1\x83\0" // 423, 1E041, 1E05F
      u8"\xd1\x84\0" // 424, 1E042, 1E060
      u8"\xd1\x85\0" // 425, 1E043, 1E061
      u8"\xd1\x86\0" // 426, 1E044, 1E062
      u8"\xd1\x87\0" // 427, 1E045, 1E063
      u8"\xd1\x88\0" // 428, 1E046, 1E064
      u8"\xd1\x89\0" // 429
      u8"\xd1\x8a\0" // 42A, 1C86, A69C, 1E065
      u8"\xd1\x8b\0" // 42B, 1E047, 1E066
      u8"\xd1\x8c\0" // 42C, A69D
      u8"\xd1\x8d\0" // 42D, 1E048
      u8"\xd1\x8e\0" // 42E, 1E049
      u8"\xd1\x8f\0" // 42F
      u8"\xd1\xa1\0" // 460
      u8"\xd1\xa3\0" // 462, 1C87
      u8"\xd1\xa5\0" // 464
      u8"\xd1\xa7\0" // 466
      u8"\xd1\xa9\0" // 468
      u8"\xd1\xab\0" // 46A
      u8"\xd1\xad\0" // 46C
      u8"\xd1\xaf\0" // 46E
      u8"\xd1\xb1\0" // 470
      u8"\xd1\xb3\0" // 472
      u8"\xd1\xb5\0" // 474
      u8"\xd1\xb7\0" // 476
      u8"\xd1\xb9\0" // 478
      u8"\xd1\xbb\0" // 47A
      u8"\xd1\xbd\0" // 47C
      u8"\xd1\xbf\0" // 47E
      u8"\xd2\x81\0" // 480
      u8"\xd2\x8b\0" // 48A
      u8"\xd2\x8d\0" // 48C
      u8"\xd2\x8f\0" // 48E
      u8"\xd2\x91\0" // 490, 1E067
      u8"\xd2\x93\0" // 492
      u8"\xd2\x95\0" // 494
      u8"\xd2\x97\0" // 496
      u8"\xd2\x99\0" // 498
      u8"\xd2\x9b\0" // 49A
      u8"\xd2\x9d\0" // 49C
      u8"\xd2\x9f\0" // 49E
      u8"\xd2\xa1\0" // 4A0
      u8"\xd2\xa3\0" // 4A2
      u8"\xd2\xa5\0" // 4A4
      u8"\xd2\xa7\0" // 4A6
      u8"\xd2\xa9\0" // 4A8
      u8"\xd2\xab\0" // 4AA, 1E06B
      u8"\xd2\xad\0" // 4AC
      u8"\xd2\xaf\0" // 4AE, 1E04F
      u8"\xd2\xb1\0" // 4B0, 1E06D
      u8"\xd2\xb3\0" // 4B2
      u8"\xd2\xb5\0" // 4B4
      u8"\xd2\xb7\0" // 4B6
      u8"\xd2\xb9\0" // 4B8
      u8"\xd2\xbb\0" // 4BA
      u8"\xd2\xbd\0" // 4BC
      u8"\xd2\xbf\0" // 4BE
      u8"\xd3\x8f\0" // 4C0, 1E050
      u8"\xd3\x82\0" // 4C1
      u8"\xd3\x84\0" // 4C3
      u8"\xd3\x86\0" // 4C5
      u8"\xd3\x88\0" // 4C7
      u8"\xd3\x8a\0" // 4C9
      u8"\xd3\x8c\0" // 4CB
      u8"\xd3\x8e\0" // 4CD
      u8"\xd3\x91\0" // 4D0
      u8"\xd3\x93\0" // 4D2
      u8"\xd3\x95\0" // 4D4
      u8"\xd3\x97\0" // 4D6
      u8"\xd3\x99\0" // 4D8, 1E04B
      u8"\xd3\x9b\0" // 4DA
      u8"\xd3\x9d\0" // 4DC
      u8"\xd3\x9f\0" // 4DE
      u8"\xd3\xa1\0" // 4E0
      u8"\xd3\xa3\0" // 4E2
      u8"\xd3\xa5\0" // 4E4
      u8"\xd3\xa7\0" // 4E6
      u8"\xd3\xa9\0" // 4E8, 1E04E
      u8"\xd3\xab\0" // 4EA
      u8"\xd3\xad\0" // 4EC
      u8"\xd3\xaf\0" // 4EE
      u8"\xd3\xb1\0" // 4F0
      u8"\xd3\xb3\0" // 4F2
      u8"\xd3\xb5\0" // 4F4
      u8"\xd3\xb7\0" // 4F6
      u8"\xd3\xb9\0" // 4F8
      u8"\xd3\xbb\0" // 4FA
      u8"\xd3\xbd\0" // 4FC
      u8"\xd3\xbf\0" // 4FE
      u8"\xd4\x81\0" // 500
      u8"\xd4\x83\0" // 502
      u8"\xd4\x85\0" // 504
      u8"\xd4\x87\0" // 506
      u8"\xd4\x89\0" // 508
      u8"\xd4\x8b\0" // 50A
      u8"\xd4\x8d\0" // 50C
      u8"\xd4\x8f\0" // 50E
      u8"\xd4\x91\0" // 510
      u8"\xd4\x93\0" // 512
      u8"\xd4\x95\0" // 514
      u8"\xd4\x97\0" // 516
      u8"\xd4\x99\0" // 518
      u8"\xd4\x9b\0" // 51A
      u8"\xd4\x9d\0" // 51C
      u8"\xd4\x9f\0" // 51E
      u8"\xd4\xa1\0" // 520
      u8"\xd4\xa3\0" // 522
      u8"\xd4\xa5\0" // 524
      u8"\xd4\xa7\0" // 526
      u8"\xd4\xa9\0" // 528
      u8"\xd4\xab\0" // 52A
      u8"\xd4\xad\0" // 52C
      u8"\xd4\xaf\0" // 52E
      u8"\xd5\xa1\0" // 531
      u8"\xd5\xa2\0" // 532
      u8"\xd5\xa3\0" // 533
      u8"\xd5\xa4\0" // 534
      u8"\xd5\xa5\0" // 535
      u8"\xd5\xa6\0" // 536
      u8"\xd5\xa7\0" // 537
      u8"\xd5\xa8\0" // 538
      u8"\xd5\xa9\0" // 539
      u8"\xd5\xaa\0" // 53A
      u8"\xd5\xab\0" // 53B
      u8"\xd5\xac\0" // 53C
      u8"\xd5\xad\0" // 53D
      u8"\xd5\xae\0" // 53E
      u8"\xd5\xaf\0" // 53F
      u8"\xd5\xb0\0" // 540
      u8"\xd5\xb1\0" // 541
      u8"\xd5\xb2\0" // 542
      u8"\xd5\xb3\0" // 543
      u8"\xd5\xb4\0" // 544
      u8"\xd5\xb5\0" // 545
      u8"\xd5\xb6\0" // 546
      u8"\xd5\xb7\0" // 547
      u8"\xd5\xb8\0" // 548
      u8"\xd5\xb9\0" // 549
      u8"\xd5\xba\0" // 54A
      u8"\xd5\xbb\0" // 54B
      u8"\xd5\xbc\0" // 54C
      u8"\xd5\xbd\0" // 54D
      u8"\xd5\xbe\0" // 54E
      u8"\xd5\xbf\0" // 54F
      u8"\xd6\x80\0" // 550
      u8"\xd6\x81\0" // 551
      u8"\xd6\x82\0" // 552
      u8"\xd6\x83\0" // 553
      u8"\xd6\x84\0" // 554
      u8"\xd6\x85\0" // 555
      u8"\xd6\x86\0" // 556
      u8"\xd5\xa5\xd6\x82\0"                                 // 587
      u8"\xd8\xa7\xd9\xb4\0"                                 // 675
      u8"\xd9\x88\xd9\xb4\0"                                 // 676
      u8"\xdb\x87\xd9\xb4\0"                                 // 677, FBDD
      u8"\xd9\x8a\xd9\xb4\0"                                 // 678
      u8"\xe0\xa4\x95\xe0\xa4\xbc\0"                         // 958
      u8"\xe0\xa4\x96\xe0\xa4\xbc\0"                         // 959
      u8"\xe0\xa4\x97\xe0\xa4\xbc\0"                         // 95A
      u8"\xe0\xa4\x9c\xe0\xa4\xbc\0"                         // 95B
      u8"\xe0\xa4\xa1\xe0\xa4\xbc\0"                         // 95C
      u8"\xe0\xa4\xa2\xe0\xa4\xbc\0"                         // 95D
      u8"\xe0\xa4\xab\xe0\xa4\xbc\0"                         // 95E
      u8"\xe0\xa4\xaf\xe0\xa4\xbc\0"                         // 95F
      u8"\xe0\xa6\xa1\xe0\xa6\xbc\0"                         // 9DC
      u8"\xe0\xa6\xa2\xe0\xa6\xbc\0"                         // 9DD
      u8"\xe0\xa6\xaf\xe0\xa6\xbc\0"                         // 9DF
      u8"\xe0\xa8\xb2\xe0\xa8\xbc\0"                         // A33
      u8"\xe0\xa8\xb8\xe0\xa8\xbc\0"                         // A36
      u8"\xe0\xa8\x96\xe0\xa8\xbc\0"                         // A59
      u8"\xe0\xa8\x97\xe0\xa8\xbc\0"                         // A5A
      u8"\xe0\xa8\x9c\xe0\xa8\xbc\0"                         // A5B
      u8"\xe0\xa8\xab\xe0\xa8\xbc\0"                         // A5E
      u8"\xe0\xac\xa1\xe0\xac\xbc\0"                         // B5C
      u8"\xe0\xac\xa2\xe0\xac\xbc\0"                         // B5D
      u8"\xe0\xb9\x8d\xe0\xb8\xb2\0"                         // E33
      u8"\xe0\xbb\x8d\xe0\xba\xb2\0"                         // EB3
      u8"\xe0\xba\xab\xe0\xba\x99\0"                         // EDC
      u8"\xe0\xba\xab\xe0\xba\xa1\0"                         // EDD
      u8"\xe0\xbc\x8b\0"                                     // F0C
      u8"\xe0\xbd\x82\xe0\xbe\xb7\0"                         // F43
      u8"\xe0\xbd\x8c\xe0\xbe\xb7\0"                         // F4D
      u8"\xe0\xbd\x91\xe0\xbe\xb7\0"                         // F52
      u8"\xe0\xbd\x96\xe0\xbe\xb7\0"                         // F57
      u8"\xe0\xbd\x9b\xe0\xbe\xb7\0"                         // F5C
      u8"\xe0\xbd\x80\xe0\xbe\xb5\0"                         // F69
      u8"\xe0\xbd\xb1\xe0\xbd\xb2\0"                         // F73
      u8"\xe0\xbd\xb1\xe0\xbd\xb4\0"                         // F75
      u8"\xe0\xbe\xb2\xe0\xbe\x80\0"                         // F76
      u8"\xe0\xbe\xb2\xe0\xbd\xb1\xe0\xbe\x80\0"             // F77
      u8"\xe0\xbe\xb3\xe0\xbe\x80\0"                         // F78
      u8"\xe0\xbe\xb3\xe0\xbd\xb1\xe0\xbe\x80\0"             // F79
      u8"\xe0\xbd\xb1\xe0\xbe\x80\0"                         // F81
      u8"\xe0\xbe\x92\xe0\xbe\xb7\0"                         // F93
      u8"\xe0\xbe\x9c\xe0\xbe\xb7\0"                         // F9D
      u8"\xe0\xbe\xa1\xe0\xbe\xb7\0"                         // FA2
      u8"\xe0\xbe\xa6\xe0\xbe\xb7\0"                         // FA7
      u8"\xe0\xbe\xab\xe0\xbe\xb7\0"                         // FAC
      u8"\xe0\xbe\x90\xe0\xbe\xb5\0"                         // FB9
      u8"\xe2\xb4\x80\0"                                     // 10A0
      u8"\xe2\xb4\x81\0"                                     // 10A1
      u8"\xe2\xb4\x82\0"                                     // 10A2
      u8"\xe2\xb4\x83\0"                                     // 10A3
      u8"\xe2\xb4\x84\0"                                     // 10A4
      u8"\xe2\xb4\x85\0"                                     // 10A5
      u8"\xe2\xb4\x86\0"                                     // 10A6
      u8"\xe2\xb4\x87\0"                                     // 10A7
      u8"\xe2\xb4\x88\0"                                     // 10A8
      u8"\xe2\xb4\x89\0"                                     // 10A9
      u8"\xe2\xb4\x8a\0"                                     // 10AA
      u8"\xe2\xb4\x8b\0"                                     // 10AB
      u8"\xe2\xb4\x8c\0"                                     // 10AC
      u8"\xe2\xb4\x8d\0"                                     // 10AD
      u8"\xe2\xb4\x8e\0"                                     // 10AE
      u8"\xe2\xb4\x8f\0"                                     // 10AF
      u8"\xe2\xb4\x90\0"                                     // 10B0
      u8"\xe2\xb4\x91\0"                                     // 10B1
      u8"\xe2\xb4\x92\0"                                     // 10B2
      u8"\xe2\xb4\x93\0"                                     // 10B3
      u8"\xe2\xb4\x94\0"                                     // 10B4
      u8"\xe2\xb4\x95\0"                                     // 10B5
      u8"\xe2\xb4\x96\0"                                     // 10B6
      u8"\xe2\xb4\x97\0"                                     // 10B7
      u8"\xe2\xb4\x98\0"                                     // 10B8
      u8"\xe2\xb4\x99\0"                                     // 10B9
      u8"\xe2\xb4\x9a\0"                                     // 10BA
      u8"\xe2\xb4\x9b\0"                                     // 10BB
      u8"\xe2\xb4\x9c\0"                                     // 10BC
      u8"\xe2\xb4\x9d\0"                                     // 10BD
      u8"\xe2\xb4\x9e\0"                                     // 10BE
      u8"\xe2\xb4\x9f\0"                                     // 10BF
      u8"\xe2\xb4\xa0\0"                                     // 10C0
      u8"\xe2\xb4\xa1\0"                                     // 10C1
      u8"\xe2\xb4\xa2\0"                                     // 10C2
      u8"\xe2\xb4\xa3\0"                                     // 10C3
      u8"\xe2\xb4\xa4\0"                                     // 10C4
      u8"\xe2\xb4\xa5\0"                                     // 10C5
      u8"\xe2\xb4\xa7\0"                                     // 10C7
      u8"\xe2\xb4\xad\0"                                     // 10CD
      u8"\xe1\x83\x9c\0"                                     // 10FC, 1C9C
      u8"\xe1\x8f\xb0\0"                                     // 13F8
      u8"\xe1\x8f\xb1\0"                                     // 13F9
      u8"\xe1\x8f\xb2\0"                                     // 13FA
      u8"\xe1\x8f\xb3\0"                                     // 13FB
      u8"\xe1\x8f\xb4\0"                                     // 13FC
      u8"\xe1\x8f\xb5\0"                                     // 13FD
      u8"\xea\x99\x8b\0"                                     // 1C88, A64A
      u8"\xe1\xb2\x8a\0"                                     // 1C89
      u8"\xe1\x83\x90\0"                                     // 1C90
      u8"\xe1\x83\x91\0"                                     // 1C91
      u8"\xe1\x83\x92\0"                                     // 1C92
      u8"\xe1\x83\x93\0"                                     // 1C93
      u8"\xe1\x83\x94\0"                                     // 1C94
      u8"\xe1\x83\x95\0"                                     // 1C95
      u8"\xe1\x83\x96\0"                                     // 1C96
      u8"\xe1\x83\x97\0"                                     // 1C97
      u8"\xe1\x83\x98\0"                                     // 1C98
      u8"\xe1\x83\x99\0"                                     // 1C99
      u8"\xe1\x83\x9a\0"                                     // 1C9A
      u8"\xe1\x83\x9b\0"                                     // 1C9B
      u8"\xe1\x83\x9d\0"                                     // 1C9D
      u8"\xe1\x83\x9e\0"                                     // 1C9E
      u8"\xe1\x83\x9f\0"                                     // 1C9F
      u8"\xe1\x83\xa0\0"                                     // 1CA0
      u8"\xe1\x83\xa1\0"                                     // 1CA1
      u8"\xe1\x83\xa2\0"                                     // 1CA2
      u8"\xe1\x83\xa3\0"                                     // 1CA3
      u8"\xe1\x83\xa4\0"                                     // 1CA4
      u8"\xe1\x83\xa5\0"                                     // 1CA5
      u8"\xe1\x83\xa6\0"                                     // 1CA6
      u8"\xe1\x83\xa7\0"                                     // 1CA7
      u8"\xe1\x83\xa8\0"                                     // 1CA8
      u8"\xe1\x83\xa9\0"                                     // 1CA9
      u8"\xe1\x83\xaa\0"                                     // 1CAA
      u8"\xe1\x83\xab\0"                                     // 1CAB
      u8"\xe1\x83\xac\0"                                     // 1CAC
      u8"\xe1\x83\xad\0"                                     // 1CAD
      u8"\xe1\x83\xae\0"                                     // 1CAE
      u8"\xe1\x83\xaf\0"                                     // 1CAF
      u8"\xe1\x83\xb0\0"                                     // 1CB0
      u8"\xe1\x83\xb1\0"                                     // 1CB1
      u8"\xe1\x83\xb2\0"                                     // 1CB2
      u8"\xe1\x83\xb3\0"                                     // 1CB3
      u8"\xe1\x83\xb4\0"                                     // 1CB4
      u8"\xe1\x83\xb5\0"                                     // 1CB5
      u8"\xe1\x83\xb6\0"                                     // 1CB6
      u8"\xe1\x83\xb7\0"                                     // 1CB7
      u8"\xe1\x83\xb8\0"                                     // 1CB8
      u8"\xe1\x83\xb9\0"                                     // 1CB9
      u8"\xe1\x83\xba\0"                                     // 1CBA
      u8"\xe1\x83\xbd\0"                                     // 1CBD
      u8"\xe1\x83\xbe\0"                                     // 1CBE
      u8"\xe1\x83\xbf\0"                                     // 1CBF
      u8"\xc9\x90\0"                                         // 1D44, 2C6F
      u8"\xc9\x91\0"                                         // 1D45, 2C6D
      u8"\xe1\xb4\x82\0"                                     // 1D46
      u8"\xc9\x9c\0"                                         // 1D4C, 1D9F, A7AB
      u8"\xe1\xb4\x96\0"                                     // 1D54
      u8"\xe1\xb4\x97\0"                                     // 1D55
      u8"\xe1\xb4\x9d\0"                                     // 1D59
      u8"\xe1\xb4\xa5\0"                                     // 1D5C
      u8"\xc9\x92\0"                                         // 1D9B, 2C70
      u8"\xc9\x95\0"                                         // 1D9D
      u8"\xc9\x9f\0"                                         // 1DA1
      u8"\xc9\xa1\0"                                         // 1DA2, A7AC
      u8"\xc9\xa5\0"                                         // 1DA3, A78D
      u8"\xc9\xaa\0"                                         // 1DA6, A7AE
      u8"\xe1\xb5\xbb\0"                                     // 1DA7
      u8"\xca\x9d\0"                                         // 1DA8, A7B2
      u8"\xc9\xad\0"                                         // 1DA9
      u8"\xe1\xb6\x85\0"                                     // 1DAA
      u8"\xca\x9f\0"                                         // 1DAB
      u8"\xc9\xb1\0"                                         // 1DAC, 2C6E
      u8"\xc9\xb0\0"                                         // 1DAD
      u8"\xc9\xb3\0"                                         // 1DAF
      u8"\xc9\xb4\0"                                         // 1DB0
      u8"\xc9\xb8\0"                                         // 1DB2
      u8"\xca\x82\0"                                         // 1DB3, A7C5
      u8"\xc6\xab\0"                                         // 1DB5
      u8"\xe1\xb4\x9c\0"                                     // 1DB8
      u8"\xca\x90\0"                                         // 1DBC
      u8"\xca\x91\0"                                         // 1DBD
      u8"\xe1\xb8\x81\0"                                     // 1E00
      u8"\xe1\xb8\x83\0"                                     // 1E02
      u8"\xe1\xb8\x85\0"                                     // 1E04
      u8"\xe1\xb8\x87\0"                                     // 1E06
      u8"\xe1\xb8\x89\0"                                     // 1E08
      u8"\xe1\xb8\x8b\0"                                     // 1E0A
      u8"\xe1\xb8\x8d\0"                                     // 1E0C
      u8"\xe1\xb8\x8f\0"                                     // 1E0E
      u8"\xe1\xb8\x91\0"                                     // 1E10
      u8"\xe1\xb8\x93\0"                                     // 1E12
      u8"\xe1\xb8\x95\0"                                     // 1E14
      u8"\xe1\xb8\x97\0"                                     // 1E16
      u8"\xe1\xb8\x99\0"                                     // 1E18
      u8"\xe1\xb8\x9b\0"                                     // 1E1A
      u8"\xe1\xb8\x9d\0"                                     // 1E1C
      u8"\xe1\xb8\x9f\0"                                     // 1E1E
      u8"\xe1\xb8\xa1\0"                                     // 1E20
      u8"\xe1\xb8\xa3\0"                                     // 1E22
      u8"\xe1\xb8\xa5\0"                                     // 1E24
      u8"\xe1\xb8\xa7\0"                                     // 1E26
      u8"\xe1\xb8\xa9\0"                                     // 1E28
      u8"\xe1\xb8\xab\0"                                     // 1E2A
      u8"\xe1\xb8\xad\0"                                     // 1E2C
      u8"\xe1\xb8\xaf\0"                                     // 1E2E
      u8"\xe1\xb8\xb1\0"                                     // 1E30
      u8"\xe1\xb8\xb3\0"                                     // 1E32
      u8"\xe1\xb8\xb5\0"                                     // 1E34
      u8"\xe1\xb8\xb7\0"                                     // 1E36
      u8"\xe1\xb8\xb9\0"                                     // 1E38
      u8"\xe1\xb8\xbb\0"                                     // 1E3A
      u8"\xe1\xb8\xbd\0"                                     // 1E3C
      u8"\xe1\xb8\xbf\0"                                     // 1E3E
      u8"\xe1\xb9\x81\0"                                     // 1E40
      u8"\xe1\xb9\x83\0"                                     // 1E42
      u8"\xe1\xb9\x85\0"                                     // 1E44
      u8"\xe1\xb9\x87\0"                                     // 1E46
      u8"\xe1\xb9\x89\0"                                     // 1E48
      u8"\xe1\xb9\x8b\0"                                     // 1E4A
      u8"\xe1\xb9\x8d\0"                                     // 1E4C
      u8"\xe1\xb9\x8f\0"                                     // 1E4E
      u8"\xe1\xb9\x91\0"                                     // 1E50
      u8"\xe1\xb9\x93\0"                                     // 1E52
      u8"\xe1\xb9\x95\0"                                     // 1E54
      u8"\xe1\xb9\x97\0"                                     // 1E56
      u8"\xe1\xb9\x99\0"                                     // 1E58
      u8"\xe1\xb9\x9b\0"                                     // 1E5A
      u8"\xe1\xb9\x9d\0"                                     // 1E5C
      u8"\xe1\xb9\x9f\0"                                     // 1E5E
      u8"\xe1\xb9\xa1\0"                                     // 1E60, 1E9B
      u8"\xe1\xb9\xa3\0"                                     // 1E62
      u8"\xe1\xb9\xa5\0"                                     // 1E64
      u8"\xe1\xb9\xa7\0"                                     // 1E66
      u8"\xe1\xb9\xa9\0"                                     // 1E68
      u8"\xe1\xb9\xab\0"                                     // 1E6A
      u8"\xe1\xb9\xad\0"                                     // 1E6C
      u8"\xe1\xb9\xaf\0"                                     // 1E6E
      u8"\xe1\xb9\xb1\0"                                     // 1E70
      u8"\xe1\xb9\xb3\0"                                     // 1E72
      u8"\xe1\xb9\xb5\0"                                     // 1E74
      u8"\xe1\xb9\xb7\0"                                     // 1E76
      u8"\xe1\xb9\xb9\0"                                     // 1E78
      u8"\xe1\xb9\xbb\0"                                     // 1E7A
      u8"\xe1\xb9\xbd\0"                                     // 1E7C
      u8"\xe1\xb9\xbf\0"                                     // 1E7E
      u8"\xe1\xba\x81\0"                                     // 1E80
      u8"\xe1\xba\x83\0"                                     // 1E82
      u8"\xe1\xba\x85\0"                                     // 1E84
      u8"\xe1\xba\x87\0"                                     // 1E86
      u8"\xe1\xba\x89\0"                                     // 1E88
      u8"\xe1\xba\x8b\0"                                     // 1E8A
      u8"\xe1\xba\x8d\0"                                     // 1E8C
      u8"\xe1\xba\x8f\0"                                     // 1E8E
      u8"\xe1\xba\x91\0"                                     // 1E90
      u8"\xe1\xba\x93\0"                                     // 1E92
      u8"\xe1\xba\x95\0"                                     // 1E94
      u8"\x61\xca\xbe\0"                                     // 1E9A
      u8"\xc3\x9f\0"                                         // 1E9E
      u8"\xe1\xba\xa1\0"                                     // 1EA0
      u8"\xe1\xba\xa3\0"                                     // 1EA2
      u8"\xe1\xba\xa5\0"                                     // 1EA4
      u8"\xe1\xba\xa7\0"                                     // 1EA6
      u8"\xe1\xba\xa9\0"                                     // 1EA8
      u8"\xe1\xba\xab\0"                                     // 1EAA
      u8"\xe1\xba\xad\0"                                     // 1EAC
      u8"\xe1\xba\xaf\0"                                     // 1EAE
      u8"\xe1\xba\xb1\0"                                     // 1EB0
      u8"\xe1\xba\xb3\0"                                     // 1EB2
      u8"\xe1\xba\xb5\0"                                     // 1EB4
      u8"\xe1\xba\xb7\0"                                     // 1EB6
      u8"\xe1\xba\xb9\0"                                     // 1EB8
      u8"\xe1\xba\xbb\0"                                     // 1EBA
      u8"\xe1\xba\xbd\0"                                     // 1EBC
      u8"\xe1\xba\xbf\0"                                     // 1EBE
      u8"\xe1\xbb\x81\0"                                     // 1EC0
      u8"\xe1\xbb\x83\0"                                     // 1EC2
      u8"\xe1\xbb\x85\0"                                     // 1EC4
      u8"\xe1\xbb\x87\0"                                     // 1EC6
      u8"\xe1\xbb\x89\0"                                     // 1EC8
      u8"\xe1\xbb\x8b\0"                                     // 1ECA
      u8"\xe1\xbb\x8d\0"                                     // 1ECC
      u8"\xe1\xbb\x8f\0"                                     // 1ECE
      u8"\xe1\xbb\x91\0"                                     // 1ED0
      u8"\xe1\xbb\x93\0"                                     // 1ED2
      u8"\xe1\xbb\x95\0"                                     // 1ED4
      u8"\xe1\xbb\x97\0"                                     // 1ED6
      u8"\xe1\xbb\x99\0"                                     // 1ED8
      u8"\xe1\xbb\x9b\0"                                     // 1EDA
      u8"\xe1\xbb\x9d\0"                                     // 1EDC
      u8"\xe1\xbb\x9f\0"                                     // 1EDE
      u8"\xe1\xbb\xa1\0"                                     // 1EE0
      u8"\xe1\xbb\xa3\0"                                     // 1EE2
      u8"\xe1\xbb\xa5\0"                                     // 1EE4
      u8"\xe1\xbb\xa7\0"                                     // 1EE6
      u8"\xe1\xbb\xa9\0"                                     // 1EE8
      u8"\xe1\xbb\xab\0"                                     // 1EEA
      u8"\xe1\xbb\xad\0"                                     // 1EEC
      u8"\xe1\xbb\xaf\0"                                     // 1EEE
      u8"\xe1\xbb\xb1\0"                                     // 1EF0
      u8"\xe1\xbb\xb3\0"                                     // 1EF2
      u8"\xe1\xbb\xb5\0"                                     // 1EF4
      u8"\xe1\xbb\xb7\0"                                     // 1EF6
      u8"\xe1\xbb\xb9\0"                                     // 1EF8
      u8"\xe1\xbb\xbb\0"                                     // 1EFA
      u8"\xe1\xbb\xbd\0"                                     // 1EFC
      u8"\xe1\xbb\xbf\0"                                     // 1EFE
      u8"\xe1\xbc\x80\0"                                     // 1F08
      u8"\xe1\xbc\x81\0"                                     // 1F09
      u8"\xe1\xbc\x82\0"                                     // 1F0A
      u8"\xe1\xbc\x83\0"                                     // 1F0B
      u8"\xe1\xbc\x84\0"                                     // 1F0C
      u8"\xe1\xbc\x85\0"                                     // 1F0D
      u8"\xe1\xbc\x86\0"                                     // 1F0E
      u8"\xe1\xbc\x87\0"                                     // 1F0F
      u8"\xe1\xbc\x90\0"                                     // 1F18
      u8"\xe1\xbc\x91\0"                                     // 1F19
      u8"\xe1\xbc\x92\0"                                     // 1F1A
      u8"\xe1\xbc\x93\0"                                     // 1F1B
      u8"\xe1\xbc\x94\0"                                     // 1F1C
      u8"\xe1\xbc\x95\0"                                     // 1F1D
      u8"\xe1\xbc\xa0\0"                                     // 1F28
      u8"\xe1\xbc\xa1\0"                                     // 1F29
      u8"\xe1\xbc\xa2\0"                                     // 1F2A
      u8"\xe1\xbc\xa3\0"                                     // 1F2B
      u8"\xe1\xbc\xa4\0"                                     // 1F2C
      u8"\xe1\xbc\xa5\0"                                     // 1F2D
      u8"\xe1\xbc\xa6\0"                                     // 1F2E
      u8"\xe1\xbc\xa7\0"                                     // 1F2F
      u8"\xe1\xbc\xb0\0"                                     // 1F38
      u8"\xe1\xbc\xb1\0"                                     // 1F39
      u8"\xe1\xbc\xb2\0"                                     // 1F3A
      u8"\xe1\xbc\xb3\0"                                     // 1F3B
      u8"\xe1\xbc\xb4\0"                                     // 1F3C
      u8"\xe1\xbc\xb5\0"                                     // 1F3D
      u8"\xe1\xbc\xb6\0"                                     // 1F3E
      u8"\xe1\xbc\xb7\0"                                     // 1F3F
      u8"\xe1\xbd\x80\0"                                     // 1F48
      u8"\xe1\xbd\x81\0"                                     // 1F49
      u8"\xe1\xbd\x82\0"                                     // 1F4A
      u8"\xe1\xbd\x83\0"                                     // 1F4B
      u8"\xe1\xbd\x84\0"                                     // 1F4C
      u8"\xe1\xbd\x85\0"                                     // 1F4D
      u8"\xe1\xbd\x91\0"                                     // 1F59
      u8"\xe1\xbd\x93\0"                                     // 1F5B
      u8"\xe1\xbd\x95\0"                                     // 1F5D
      u8"\xe1\xbd\x97\0"                                     // 1F5F
      u8"\xe1\xbd\xa0\0"                                     // 1F68
      u8"\xe1\xbd\xa1\0"                                     // 1F69
      u8"\xe1\xbd\xa2\0"                                     // 1F6A
      u8"\xe1\xbd\xa3\0"                                     // 1F6B
      u8"\xe1\xbd\xa4\0"                                     // 1F6C
      u8"\xe1\xbd\xa5\0"                                     // 1F6D
      u8"\xe1\xbd\xa6\0"                                     // 1F6E
      u8"\xe1\xbd\xa7\0"                                     // 1F6F
      u8"\xe1\xbc\x80\xce\xb9\0"                             // 1F80, 1F88
      u8"\xe1\xbc\x81\xce\xb9\0"                             // 1F81, 1F89
      u8"\xe1\xbc\x82\xce\xb9\0"                             // 1F82, 1F8A
      u8"\xe1\xbc\x83\xce\xb9\0"                             // 1F83, 1F8B
      u8"\xe1\xbc\x84\xce\xb9\0"                             // 1F84, 1F8C
      u8"\xe1\xbc\x85\xce\xb9\0"                             // 1F85, 1F8D
      u8"\xe1\xbc\x86\xce\xb9\0"                             // 1F86, 1F8E
      u8"\xe1\xbc\x87\xce\xb9\0"                             // 1F87, 1F8F
      u8"\xe1\xbc\xa0\xce\xb9\0"                             // 1F90, 1F98
      u8"\xe1\xbc\xa1\xce\xb9\0"                             // 1F91, 1F99
      u8"\xe1\xbc\xa2\xce\xb9\0"                             // 1F92, 1F9A
      u8"\xe1\xbc\xa3\xce\xb9\0"                             // 1F93, 1F9B
      u8"\xe1\xbc\xa4\xce\xb9\0"                             // 1F94, 1F9C
      u8"\xe1\xbc\xa5\xce\xb9\0"                             // 1F95, 1F9D
      u8"\xe1\xbc\xa6\xce\xb9\0"                             // 1F96, 1F9E
      u8"\xe1\xbc\xa7\xce\xb9\0"                             // 1F97, 1F9F
      u8"\xe1\xbd\xa0\xce\xb9\0"                             // 1FA0, 1FA8
      u8"\xe1\xbd\xa1\xce\xb9\0"                             // 1FA1, 1FA9
      u8"\xe1\xbd\xa2\xce\xb9\0"                             // 1FA2, 1FAA
      u8"\xe1\xbd\xa3\xce\xb9\0"                             // 1FA3, 1FAB
      u8"\xe1\xbd\xa4\xce\xb9\0"                             // 1FA4, 1FAC
      u8"\xe1\xbd\xa5\xce\xb9\0"                             // 1FA5, 1FAD
      u8"\xe1\xbd\xa6\xce\xb9\0"                             // 1FA6, 1FAE
      u8"\xe1\xbd\xa7\xce\xb9\0"                             // 1FA7, 1FAF
      u8"\xe1\xbd\xb0\xce\xb9\0"                             // 1FB2
      u8"\xce\xb1\xce\xb9\0"                                 // 1FB3, 1FBC
      u8"\xce\xac\xce\xb9\0"                                 // 1FB4
      u8"\xe1\xbe\xb6\xce\xb9\0"                             // 1FB7
      u8"\xe1\xbe\xb0\0"                                     // 1FB8
      u8"\xe1\xbe\xb1\0"                                     // 1FB9
      u8"\xe1\xbd\xb0\0"                                     // 1FBA
      u8"\40\xcc\x93\0"                                      // 1FBD, 1FBF
      u8"\40\xcd\x82\0"                                      // 1FC0
      u8"\40\xcc\x88\xcd\x82\0"                              // 1FC1
      u8"\xe1\xbd\xb4\xce\xb9\0"                             // 1FC2
      u8"\xce\xb7\xce\xb9\0"                                 // 1FC3, 1FCC
      u8"\xce\xae\xce\xb9\0"                                 // 1FC4
      u8"\xe1\xbf\x86\xce\xb9\0"                             // 1FC7
      u8"\xe1\xbd\xb2\0"                                     // 1FC8
      u8"\xe1\xbd\xb4\0"                                     // 1FCA
      u8"\40\xcc\x93\xcc\x80\0"                              // 1FCD
      u8"\40\xcc\x93\xcc\x81\0"                              // 1FCE
      u8"\40\xcc\x93\xcd\x82\0"                              // 1FCF
      u8"\xce\x90\0"                                         // 1FD3
      u8"\xe1\xbf\x90\0"                                     // 1FD8
      u8"\xe1\xbf\x91\0"                                     // 1FD9
      u8"\xe1\xbd\xb6\0"                                     // 1FDA
      u8"\40\xcc\x94\xcc\x80\0"                              // 1FDD
      u8"\40\xcc\x94\xcc\x81\0"                              // 1FDE
      u8"\40\xcc\x94\xcd\x82\0"                              // 1FDF
      u8"\xce\xb0\0"                                         // 1FE3
      u8"\xe1\xbf\xa0\0"                                     // 1FE8
      u8"\xe1\xbf\xa1\0"                                     // 1FE9
      u8"\xe1\xbd\xba\0"                                     // 1FEA
      u8"\xe1\xbf\xa5\0"                                     // 1FEC
      u8"\40\xcc\x88\xcc\x80\0"                              // 1FED
      u8"\x60\0"                                             // 1FEF, FF40
      u8"\xe1\xbd\xbc\xce\xb9\0"                             // 1FF2
      u8"\xcf\x89\xce\xb9\0"                                 // 1FF3, 1FFC
      u8"\xcf\x8e\xce\xb9\0"                                 // 1FF4
      u8"\xe1\xbf\xb6\xce\xb9\0"                             // 1FF7
      u8"\xe1\xbd\xb8\0"                                     // 1FF8
      u8"\xe1\xbd\xbc\0"                                     // 1FFA
      u8"\40\xcc\x94\0"                                      // 1FFE
      u8"\xe2\x80\x90\0"                                     // 2011
      u8"\40\xcc\xb3\0"                                      // 2017
      u8"\xe2\x80\xb2\xe2\x80\xb2\0"                         // 2033
      u8"\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\0"             // 2034
      u8"\xe2\x80\xb5\xe2\x80\xb5\0"                         // 2036
      u8"\xe2\x80\xb5\xe2\x80\xb5\xe2\x80\xb5\0"             // 2037
      u8"\41\41\0"                                           // 203C
      u8"\40\xcc\x85\0"                                      // 203E, FE49, FE4A, FE4B, FE4C
      u8"\77\77\0"                                           // 2047
      u8"\77\41\0"                                           // 2048
      u8"\41\77\0"                                           // 2049
      u8"\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\0" // 2057
      u8"\60\0"                   // 2070, 2080, 24EA, FF10, 1CCF0, 1D7CE, 1D7D8, 1D7E2, 1D7EC, 1D7F6, 1FBF0
      u8"\64\0"                   // 2074, 2084, 2463, FF14, 1CCF4, 1D7D2, 1D7DC, 1D7E6, 1D7F0, 1D7FA, 1FBF4
      u8"\65\0"                   // 2075, 2085, 2464, FF15, 1CCF5, 1D7D3, 1D7DD, 1D7E7, 1D7F1, 1D7FB, 1FBF5
      u8"\66\0"                   // 2076, 2086, 2465, FF16, 1CCF6, 1D7D4, 1D7DE, 1D7E8, 1D7F2, 1D7FC, 1FBF6
      u8"\67\0"                   // 2077, 2087, 2466, FF17, 1CCF7, 1D7D5, 1D7DF, 1D7E9, 1D7F3, 1D7FD, 1FBF7
      u8"\70\0"                   // 2078, 2088, 2467, FF18, 1CCF8, 1D7D6, 1D7E0, 1D7EA, 1D7F4, 1D7FE, 1FBF8
      u8"\71\0"                   // 2079, 2089, 2468, FF19, 1CCF9, 1D7D7, 1D7E1, 1D7EB, 1D7F5, 1D7FF, 1FBF9
      u8"\53\0"                   // 207A, 208A, FB29, FE62, FF0B
      u8"\xe2\x88\x92\0"          // 207B, 208B
      u8"\75\0"                   // 207C, 208C, FE66, FF1D
      u8"\50\0"                   // 207D, 208D, FE35, FE59, FF08
      u8"\51\0"                   // 207E, 208E, FE36, FE5A, FF09
      u8"\x72\x73\0"              // 20A8
      u8"\x61\57\x63\0"           // 2100
      u8"\x61\57\x73\0"           // 2101
      u8"\xc2\xb0\x63\0"          // 2103
      u8"\x63\57\x6f\0"           // 2105
      u8"\x63\57\x75\0"           // 2106
      u8"\xc2\xb0\x66\0"          // 2109
      u8"\x6e\x6f\0"              // 2116
      u8"\x73\x6d\0"              // 2120
      u8"\x74\x65\x6c\0"          // 2121
      u8"\x74\x6d\0"              // 2122
      u8"\xe2\x85\x8e\0"          // 2132
      u8"\xd7\x90\0"              // 2135, FB21
      u8"\xd7\x91\0"              // 2136
      u8"\xd7\x92\0"              // 2137
      u8"\xd7\x93\0"              // 2138, FB22
      u8"\x66\x61\x78\0"          // 213B
      u8"\xe2\x88\x91\0"          // 2140
      u8"\61\xe2\x81\x84\67\0"    // 2150
      u8"\61\xe2\x81\x84\71\0"    // 2151
      u8"\61\xe2\x81\x84\61\60\0" // 2152
      u8"\61\xe2\x81\x84\63\0"    // 2153
      u8"\62\xe2\x81\x84\63\0"    // 2154
      u8"\61\xe2\x81\x84\65\0"    // 2155
      u8"\62\xe2\x81\x84\65\0"    // 2156
      u8"\63\xe2\x81\x84\65\0"    // 2157
      u8"\64\xe2\x81\x84\65\0"    // 2158
      u8"\61\xe2\x81\x84\66\0"    // 2159
      u8"\65\xe2\x81\x84\66\0"    // 215A
      u8"\61\xe2\x81\x84\70\0"    // 215B
      u8"\63\xe2\x81\x84\70\0"    // 215C
      u8"\65\xe2\x81\x84\70\0"    // 215D
      u8"\67\xe2\x81\x84\70\0"    // 215E
      u8"\61\xe2\x81\x84\0"       // 215F
      u8"\x69\x69\0"              // 2161, 2171
      u8"\x69\x69\x69\0"          // 2162, 2172
      u8"\x69\x76\0"              // 2163, 2173
      u8"\x76\x69\0"              // 2165, 2175
      u8"\x76\x69\x69\0"          // 2166, 2176
      u8"\x76\x69\x69\x69\0"      // 2167, 2177
      u8"\x69\x78\0"              // 2168, 2178
      u8"\x78\x69\0"              // 216A, 217A
      u8"\x78\x69\x69\0"          // 216B, 217B
      u8"\xe2\x86\x84\0"          // 2183
      u8"\60\xe2\x81\x84\63\0"    // 2189
      u8"\xe2\x88\xab\xe2\x88\xab\0"                                     // 222C
      u8"\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\0"                         // 222D
      u8"\xe2\x88\xae\xe2\x88\xae\0"                                     // 222F
      u8"\xe2\x88\xae\xe2\x88\xae\xe2\x88\xae\0"                         // 2230
      u8"\xe3\x80\x88\0"                                                 // 2329, FE3F
      u8"\xe3\x80\x89\0"                                                 // 232A, FE40
      u8"\61\60\0"                                                       // 2469
      u8"\61\61\0"                                                       // 246A
      u8"\61\62\0"                                                       // 246B
      u8"\61\63\0"                                                       // 246C
      u8"\61\64\0"                                                       // 246D
      u8"\61\65\0"                                                       // 246E
      u8"\61\66\0"                                                       // 246F
      u8"\61\67\0"                                                       // 2470
      u8"\61\70\0"                                                       // 2471
      u8"\61\71\0"                                                       // 2472
      u8"\62\60\0"                                                       // 2473
      u8"\50\61\51\0"                                                    // 2474
      u8"\50\62\51\0"                                                    // 2475
      u8"\50\63\51\0"                                                    // 2476
      u8"\50\64\51\0"                                                    // 2477
      u8"\50\65\51\0"                                                    // 2478
      u8"\50\66\51\0"                                                    // 2479
      u8"\50\67\51\0"                                                    // 247A
      u8"\50\70\51\0"                                                    // 247B
      u8"\50\71\51\0"                                                    // 247C
      u8"\50\61\60\51\0"                                                 // 247D
      u8"\50\61\61\51\0"                                                 // 247E
      u8"\50\61\62\51\0"                                                 // 247F
      u8"\50\61\63\51\0"                                                 // 2480
      u8"\50\61\64\51\0"                                                 // 2481
      u8"\50\61\65\51\0"                                                 // 2482
      u8"\50\61\66\51\0"                                                 // 2483
      u8"\50\61\67\51\0"                                                 // 2484
      u8"\50\61\70\51\0"                                                 // 2485
      u8"\50\61\71\51\0"                                                 // 2486
      u8"\50\62\60\51\0"                                                 // 2487
      u8"\50\x61\51\0"                                                   // 249C, 1F110
      u8"\50\x62\51\0"                                                   // 249D, 1F111
      u8"\50\x63\51\0"                                                   // 249E, 1F112
      u8"\50\x64\51\0"                                                   // 249F, 1F113
      u8"\50\x65\51\0"                                                   // 24A0, 1F114
      u8"\50\x66\51\0"                                                   // 24A1, 1F115
      u8"\50\x67\51\0"                                                   // 24A2, 1F116
      u8"\50\x68\51\0"                                                   // 24A3, 1F117
      u8"\50\x69\51\0"                                                   // 24A4, 1F118
      u8"\50\x6a\51\0"                                                   // 24A5, 1F119
      u8"\50\x6b\51\0"                                                   // 24A6, 1F11A
      u8"\50\x6c\51\0"                                                   // 24A7, 1F11B
      u8"\50\x6d\51\0"                                                   // 24A8, 1F11C
      u8"\50\x6e\51\0"                                                   // 24A9, 1F11D
      u8"\50\x6f\51\0"                                                   // 24AA, 1F11E
      u8"\50\x70\51\0"                                                   // 24AB, 1F11F
      u8"\50\x71\51\0"                                                   // 24AC, 1F120
      u8"\50\x72\51\0"                                                   // 24AD, 1F121
      u8"\50\x73\51\0"                                                   // 24AE, 1F122
      u8"\50\x74\51\0"                                                   // 24AF, 1F123
      u8"\50\x75\51\0"                                                   // 24B0, 1F124
      u8"\50\x76\51\0"                                                   // 24B1, 1F125
      u8"\50\x77\51\0"                                                   // 24B2, 1F126
      u8"\50\x78\51\0"                                                   // 24B3, 1F127
      u8"\50\x79\51\0"                                                   // 24B4, 1F128
      u8"\50\x7a\51\0"                                                   // 24B5, 1F129
      u8"\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\0"             // 2A0C
      u8"\72\72\75\0"                                                    // 2A74
      u8"\75\75\0"                                                       // 2A75
      u8"\75\75\75\0"                                                    // 2A76
      u8"\xe2\xab\x9d\xcc\xb8\0"                                         // 2ADC
      u8"\xe2\xb0\xb0\0"                                                 // 2C00
      u8"\xe2\xb0\xb1\0"                                                 // 2C01
      u8"\xe2\xb0\xb2\0"                                                 // 2C02
      u8"\xe2\xb0\xb3\0"                                                 // 2C03
      u8"\xe2\xb0\xb4\0"                                                 // 2C04
      u8"\xe2\xb0\xb5\0"                                                 // 2C05
      u8"\xe2\xb0\xb6\0"                                                 // 2C06
      u8"\xe2\xb0\xb7\0"                                                 // 2C07
      u8"\xe2\xb0\xb8\0"                                                 // 2C08
      u8"\xe2\xb0\xb9\0"                                                 // 2C09
      u8"\xe2\xb0\xba\0"                                                 // 2C0A
      u8"\xe2\xb0\xbb\0"                                                 // 2C0B
      u8"\xe2\xb0\xbc\0"                                                 // 2C0C
      u8"\xe2\xb0\xbd\0"                                                 // 2C0D
      u8"\xe2\xb0\xbe\0"                                                 // 2C0E
      u8"\xe2\xb0\xbf\0"                                                 // 2C0F
      u8"\xe2\xb1\x80\0"                                                 // 2C10
      u8"\xe2\xb1\x81\0"                                                 // 2C11
      u8"\xe2\xb1\x82\0"                                                 // 2C12
      u8"\xe2\xb1\x83\0"                                                 // 2C13
      u8"\xe2\xb1\x84\0"                                                 // 2C14
      u8"\xe2\xb1\x85\0"                                                 // 2C15
      u8"\xe2\xb1\x86\0"                                                 // 2C16
      u8"\xe2\xb1\x87\0"                                                 // 2C17
      u8"\xe2\xb1\x88\0"                                                 // 2C18
      u8"\xe2\xb1\x89\0"                                                 // 2C19
      u8"\xe2\xb1\x8a\0"                                                 // 2C1A
      u8"\xe2\xb1\x8b\0"                                                 // 2C1B
      u8"\xe2\xb1\x8c\0"                                                 // 2C1C
      u8"\xe2\xb1\x8d\0"                                                 // 2C1D
      u8"\xe2\xb1\x8e\0"                                                 // 2C1E
      u8"\xe2\xb1\x8f\0"                                                 // 2C1F
      u8"\xe2\xb1\x90\0"                                                 // 2C20
      u8"\xe2\xb1\x91\0"                                                 // 2C21
      u8"\xe2\xb1\x92\0"                                                 // 2C22
      u8"\xe2\xb1\x93\0"                                                 // 2C23
      u8"\xe2\xb1\x94\0"                                                 // 2C24
      u8"\xe2\xb1\x95\0"                                                 // 2C25
      u8"\xe2\xb1\x96\0"                                                 // 2C26
      u8"\xe2\xb1\x97\0"                                                 // 2C27
      u8"\xe2\xb1\x98\0"                                                 // 2C28
      u8"\xe2\xb1\x99\0"                                                 // 2C29
      u8"\xe2\xb1\x9a\0"                                                 // 2C2A
      u8"\xe2\xb1\x9b\0"                                                 // 2C2B
      u8"\xe2\xb1\x9c\0"                                                 // 2C2C
      u8"\xe2\xb1\x9d\0"                                                 // 2C2D
      u8"\xe2\xb1\x9e\0"                                                 // 2C2E
      u8"\xe2\xb1\x9f\0"                                                 // 2C2F
      u8"\xe2\xb1\xa1\0"                                                 // 2C60
      u8"\xc9\xab\0"                                                     // 2C62, AB5E
      u8"\xe1\xb5\xbd\0"                                                 // 2C63
      u8"\xc9\xbd\0"                                                     // 2C64, 107A8
      u8"\xe2\xb1\xa8\0"                                                 // 2C67
      u8"\xe2\xb1\xaa\0"                                                 // 2C69
      u8"\xe2\xb1\xac\0"                                                 // 2C6B
      u8"\xe2\xb1\xb3\0"                                                 // 2C72
      u8"\xe2\xb1\xb6\0"                                                 // 2C75
      u8"\xc8\xbf\0"                                                     // 2C7E
      u8"\xc9\x80\0"                                                     // 2C7F
      u8"\xe2\xb2\x81\0"                                                 // 2C80
      u8"\xe2\xb2\x83\0"                                                 // 2C82
      u8"\xe2\xb2\x85\0"                                                 // 2C84
      u8"\xe2\xb2\x87\0"                                                 // 2C86
      u8"\xe2\xb2\x89\0"                                                 // 2C88
      u8"\xe2\xb2\x8b\0"                                                 // 2C8A
      u8"\xe2\xb2\x8d\0"                                                 // 2C8C
      u8"\xe2\xb2\x8f\0"                                                 // 2C8E
      u8"\xe2\xb2\x91\0"                                                 // 2C90
      u8"\xe2\xb2\x93\0"                                                 // 2C92
      u8"\xe2\xb2\x95\0"                                                 // 2C94
      u8"\xe2\xb2\x97\0"                                                 // 2C96
      u8"\xe2\xb2\x99\0"                                                 // 2C98
      u8"\xe2\xb2\x9b\0"                                                 // 2C9A
      u8"\xe2\xb2\x9d\0"                                                 // 2C9C
      u8"\xe2\xb2\x9f\0"                                                 // 2C9E
      u8"\xe2\xb2\xa1\0"                                                 // 2CA0
      u8"\xe2\xb2\xa3\0"                                                 // 2CA2
      u8"\xe2\xb2\xa5\0"                                                 // 2CA4
      u8"\xe2\xb2\xa7\0"                                                 // 2CA6
      u8"\xe2\xb2\xa9\0"                                                 // 2CA8
      u8"\xe2\xb2\xab\0"                                                 // 2CAA
      u8"\xe2\xb2\xad\0"                                                 // 2CAC
      u8"\xe2\xb2\xaf\0"                                                 // 2CAE
      u8"\xe2\xb2\xb1\0"                                                 // 2CB0
      u8"\xe2\xb2\xb3\0"                                                 // 2CB2
      u8"\xe2\xb2\xb5\0"                                                 // 2CB4
      u8"\xe2\xb2\xb7\0"                                                 // 2CB6
      u8"\xe2\xb2\xb9\0"                                                 // 2CB8
      u8"\xe2\xb2\xbb\0"                                                 // 2CBA
      u8"\xe2\xb2\xbd\0"                                                 // 2CBC
      u8"\xe2\xb2\xbf\0"                                                 // 2CBE
      u8"\xe2\xb3\x81\0"                                                 // 2CC0
      u8"\xe2\xb3\x83\0"                                                 // 2CC2
      u8"\xe2\xb3\x85\0"                                                 // 2CC4
      u8"\xe2\xb3\x87\0"                                                 // 2CC6
      u8"\xe2\xb3\x89\0"                                                 // 2CC8
      u8"\xe2\xb3\x8b\0"                                                 // 2CCA
      u8"\xe2\xb3\x8d\0"                                                 // 2CCC
      u8"\xe2\xb3\x8f\0"                                                 // 2CCE
      u8"\xe2\xb3\x91\0"                                                 // 2CD0
      u8"\xe2\xb3\x93\0"                                                 // 2CD2
      u8"\xe2\xb3\x95\0"                                                 // 2CD4
      u8"\xe2\xb3\x97\0"                                                 // 2CD6
      u8"\xe2\xb3\x99\0"                                                 // 2CD8
      u8"\xe2\xb3\x9b\0"                                                 // 2CDA
      u8"\xe2\xb3\x9d\0"                                                 // 2CDC
      u8"\xe2\xb3\x9f\0"                                                 // 2CDE
      u8"\xe2\xb3\xa1\0"                                                 // 2CE0
      u8"\xe2\xb3\xa3\0"                                                 // 2CE2
      u8"\xe2\xb3\xac\0"                                                 // 2CEB
      u8"\xe2\xb3\xae\0"                                                 // 2CED
      u8"\xe2\xb3\xb3\0"                                                 // 2CF2
      u8"\xe2\xb5\xa1\0"                                                 // 2D6F
      u8"\xe6\xaf\x8d\0"                                                 // 2E9F
      u8"\xe9\xbe\x9f\0"                                                 // 2EF3
      u8"\xe4\xb8\x80\0"                                                 // 2F00, 3192, 3280, 1F229
      u8"\xe4\xb8\xa8\0"                                                 // 2F01
      u8"\xe4\xb8\xb6\0"                                                 // 2F02
      u8"\xe4\xb8\xbf\0"                                                 // 2F03
      u8"\xe4\xb9\x99\0"                                                 // 2F04, 319A
      u8"\xe4\xba\x85\0"                                                 // 2F05
      u8"\xe4\xba\x8c\0"                                                 // 2F06, 3193, 3281, 1F214
      u8"\xe4\xba\xa0\0"                                                 // 2F07
      u8"\xe4\xba\xba\0"                                                 // 2F08, 319F
      u8"\xe5\x84\xbf\0"                                                 // 2F09
      u8"\xe5\x85\xa5\0"                                                 // 2F0A
      u8"\xe5\x85\xab\0"                                                 // 2F0B, 3287
      u8"\xe5\x86\x82\0"                                                 // 2F0C
      u8"\xe5\x86\x96\0"                                                 // 2F0D
      u8"\xe5\x86\xab\0"                                                 // 2F0E
      u8"\xe5\x87\xa0\0"                                                 // 2F0F
      u8"\xe5\x87\xb5\0"                                                 // 2F10, 2F81D
      u8"\xe5\x88\x80\0"                                                 // 2F11
      u8"\xe5\x8a\x9b\0"                                                 // 2F12, F98A
      u8"\xe5\x8b\xb9\0"                                                 // 2F13
      u8"\xe5\x8c\x95\0"                                                 // 2F14
      u8"\xe5\x8c\x9a\0"                                                 // 2F15
      u8"\xe5\x8c\xb8\0"                                                 // 2F16
      u8"\xe5\x8d\x81\0"                                                 // 2F17, 3038, 3289
      u8"\xe5\x8d\x9c\0"                                                 // 2F18
      u8"\xe5\x8d\xa9\0"                                                 // 2F19
      u8"\xe5\x8e\x82\0"                                                 // 2F1A
      u8"\xe5\x8e\xb6\0"                                                 // 2F1B
      u8"\xe5\x8f\x88\0"                                                 // 2F1C
      u8"\xe5\x8f\xa3\0"                                                 // 2F1D
      u8"\xe5\x9b\x97\0"                                                 // 2F1E
      u8"\xe5\x9c\x9f\0"                                                 // 2F1F, 328F
      u8"\xe5\xa3\xab\0"                                                 // 2F20
      u8"\xe5\xa4\x82\0"                                                 // 2F21
      u8"\xe5\xa4\x8a\0"                                                 // 2F22
      u8"\xe5\xa4\x95\0"                                                 // 2F23
      u8"\xe5\xa4\xa7\0"                                                 // 2F24
      u8"\xe5\xa5\xb3\0"                                                 // 2F25, 329B, F981
      u8"\xe5\xad\x90\0"                                                 // 2F26
      u8"\xe5\xae\x80\0"                                                 // 2F27
      u8"\xe5\xaf\xb8\0"                                                 // 2F28
      u8"\xe5\xb0\x8f\0"                                                 // 2F29
      u8"\xe5\xb0\xa2\0"                                                 // 2F2A, 2F875
      u8"\xe5\xb0\xb8\0"                                                 // 2F2B
      u8"\xe5\xb1\xae\0"                                                 // 2F2C, FA3C, 2F878
      u8"\xe5\xb1\xb1\0"                                                 // 2F2D
      u8"\xe5\xb7\x9b\0"                                                 // 2F2E
      u8"\xe5\xb7\xa5\0"                                                 // 2F2F
      u8"\xe5\xb7\xb1\0"                                                 // 2F30
      u8"\xe5\xb7\xbe\0"                                                 // 2F31
      u8"\xe5\xb9\xb2\0"                                                 // 2F32
      u8"\xe5\xb9\xba\0"                                                 // 2F33
      u8"\xe5\xb9\xbf\0"                                                 // 2F34
      u8"\xe5\xbb\xb4\0"                                                 // 2F35
      u8"\xe5\xbb\xbe\0"                                                 // 2F36, 2F890
      u8"\xe5\xbc\x8b\0"                                                 // 2F37
      u8"\xe5\xbc\x93\0"                                                 // 2F38
      u8"\xe5\xbd\x90\0"                                                 // 2F39
      u8"\xe5\xbd\xa1\0"                                                 // 2F3A
      u8"\xe5\xbd\xb3\0"                                                 // 2F3B
      u8"\xe5\xbf\x83\0"                                                 // 2F3C
      u8"\xe6\x88\x88\0"                                                 // 2F3D
      u8"\xe6\x88\xb6\0"                                                 // 2F3E
      u8"\xe6\x89\x8b\0"                                                 // 2F3F, 1F210
      u8"\xe6\x94\xaf\0"                                                 // 2F40
      u8"\xe6\x94\xb4\0"                                                 // 2F41
      u8"\xe6\x96\x87\0"                                                 // 2F42, 3246
      u8"\xe6\x96\x97\0"                                                 // 2F43
      u8"\xe6\x96\xa4\0"                                                 // 2F44
      u8"\xe6\x96\xb9\0"                                                 // 2F45
      u8"\xe6\x97\xa0\0"                                                 // 2F46
      u8"\xe6\x97\xa5\0"                                                 // 2F47, 3290
      u8"\xe6\x9b\xb0\0"                                                 // 2F48
      u8"\xe6\x9c\x88\0"                                                 // 2F49, 328A, 1F237
      u8"\xe6\x9c\xa8\0"                                                 // 2F4A, 328D
      u8"\xe6\xac\xa0\0"                                                 // 2F4B
      u8"\xe6\xad\xa2\0"                                                 // 2F4C
      u8"\xe6\xad\xb9\0"                                                 // 2F4D, FA95
      u8"\xe6\xae\xb3\0"                                                 // 2F4E
      u8"\xe6\xaf\x8b\0"                                                 // 2F4F
      u8"\xe6\xaf\x94\0"                                                 // 2F50
      u8"\xe6\xaf\x9b\0"                                                 // 2F51
      u8"\xe6\xb0\x8f\0"                                                 // 2F52
      u8"\xe6\xb0\x94\0"                                                 // 2F53
      u8"\xe6\xb0\xb4\0"                                                 // 2F54, 328C
      u8"\xe7\x81\xab\0"                                                 // 2F55, 328B
      u8"\xe7\x88\xaa\0"                                                 // 2F56
      u8"\xe7\x88\xb6\0"                                                 // 2F57
      u8"\xe7\x88\xbb\0"                                                 // 2F58
      u8"\xe7\x88\xbf\0"                                                 // 2F59
      u8"\xe7\x89\x87\0"                                                 // 2F5A
      u8"\xe7\x89\x99\0"                                                 // 2F5B
      u8"\xe7\x89\x9b\0"                                                 // 2F5C
      u8"\xe7\x8a\xac\0"                                                 // 2F5D
      u8"\xe7\x8e\x84\0"                                                 // 2F5E
      u8"\xe7\x8e\x89\0"                                                 // 2F5F
      u8"\xe7\x93\x9c\0"                                                 // 2F60
      u8"\xe7\x93\xa6\0"                                                 // 2F61
      u8"\xe7\x94\x98\0"                                                 // 2F62
      u8"\xe7\x94\x9f\0"                                                 // 2F63, 1F222
      u8"\xe7\x94\xa8\0"                                                 // 2F64
      u8"\xe7\x94\xb0\0"                                                 // 2F65
      u8"\xe7\x96\x8b\0"                                                 // 2F66
      u8"\xe7\x96\x92\0"                                                 // 2F67
      u8"\xe7\x99\xb6\0"                                                 // 2F68
      u8"\xe7\x99\xbd\0"                                                 // 2F69
      u8"\xe7\x9a\xae\0"                                                 // 2F6A
      u8"\xe7\x9a\xbf\0"                                                 // 2F6B
      u8"\xe7\x9b\xae\0"                                                 // 2F6C
      u8"\xe7\x9f\x9b\0"                                                 // 2F6D
      u8"\xe7\x9f\xa2\0"                                                 // 2F6E
      u8"\xe7\x9f\xb3\0"                                                 // 2F6F
      u8"\xe7\xa4\xba\0"                                                 // 2F70
      u8"\xe7\xa6\xb8\0"                                                 // 2F71
      u8"\xe7\xa6\xbe\0"                                                 // 2F72
      u8"\xe7\xa9\xb4\0"                                                 // 2F73
      u8"\xe7\xab\x8b\0"                                                 // 2F74, F9F7
      u8"\xe7\xab\xb9\0"                                                 // 2F75
      u8"\xe7\xb1\xb3\0"                                                 // 2F76
      u8"\xe7\xb3\xb8\0"                                                 // 2F77
      u8"\xe7\xbc\xb6\0"                                                 // 2F78
      u8"\xe7\xbd\x91\0"                                                 // 2F79
      u8"\xe7\xbe\x8a\0"                                                 // 2F7A
      u8"\xe7\xbe\xbd\0"                                                 // 2F7B, FA1E
      u8"\xe8\x80\x81\0"                                                 // 2F7C, F934
      u8"\xe8\x80\x8c\0"                                                 // 2F7D
      u8"\xe8\x80\x92\0"                                                 // 2F7E
      u8"\xe8\x80\xb3\0"                                                 // 2F7F
      u8"\xe8\x81\xbf\0"                                                 // 2F80
      u8"\xe8\x82\x89\0"                                                 // 2F81
      u8"\xe8\x87\xa3\0"                                                 // 2F82
      u8"\xe8\x87\xaa\0"                                                 // 2F83
      u8"\xe8\x87\xb3\0"                                                 // 2F84
      u8"\xe8\x87\xbc\0"                                                 // 2F85
      u8"\xe8\x88\x8c\0"                                                 // 2F86
      u8"\xe8\x88\x9b\0"                                                 // 2F87
      u8"\xe8\x88\x9f\0"                                                 // 2F88
      u8"\xe8\x89\xae\0"                                                 // 2F89
      u8"\xe8\x89\xb2\0"                                                 // 2F8A
      u8"\xe8\x89\xb8\0"                                                 // 2F8B
      u8"\xe8\x99\x8d\0"                                                 // 2F8C
      u8"\xe8\x99\xab\0"                                                 // 2F8D
      u8"\xe8\xa1\x80\0"                                                 // 2F8E
      u8"\xe8\xa1\x8c\0"                                                 // 2F8F, FA08
      u8"\xe8\xa1\xa3\0"                                                 // 2F90, 2F9C4
      u8"\xe8\xa5\xbe\0"                                                 // 2F91
      u8"\xe8\xa6\x8b\0"                                                 // 2F92, FA0A
      u8"\xe8\xa7\x92\0"                                                 // 2F93
      u8"\xe8\xa8\x80\0"                                                 // 2F94
      u8"\xe8\xb0\xb7\0"                                                 // 2F95
      u8"\xe8\xb1\x86\0"                                                 // 2F96
      u8"\xe8\xb1\x95\0"                                                 // 2F97, 2F9D2
      u8"\xe8\xb1\xb8\0"                                                 // 2F98
      u8"\xe8\xb2\x9d\0"                                                 // 2F99
      u8"\xe8\xb5\xa4\0"                                                 // 2F9A
      u8"\xe8\xb5\xb0\0"                                                 // 2F9B, 1F230
      u8"\xe8\xb6\xb3\0"                                                 // 2F9C
      u8"\xe8\xba\xab\0"                                                 // 2F9D
      u8"\xe8\xbb\x8a\0"                                                 // 2F9E, F902
      u8"\xe8\xbe\x9b\0"                                                 // 2F9F
      u8"\xe8\xbe\xb0\0"                                                 // 2FA0, F971
      u8"\xe8\xbe\xb5\0"                                                 // 2FA1
      u8"\xe9\x82\x91\0"                                                 // 2FA2
      u8"\xe9\x85\x89\0"                                                 // 2FA3
      u8"\xe9\x87\x86\0"                                                 // 2FA4
      u8"\xe9\x87\x8c\0"                                                 // 2FA5, F9E9
      u8"\xe9\x87\x91\0"                                                 // 2FA6, 328E, F90A
      u8"\xe9\x95\xb7\0"                                                 // 2FA7
      u8"\xe9\x96\x80\0"                                                 // 2FA8
      u8"\xe9\x98\x9c\0"                                                 // 2FA9
      u8"\xe9\x9a\xb6\0"                                                 // 2FAA
      u8"\xe9\x9a\xb9\0"                                                 // 2FAB
      u8"\xe9\x9b\xa8\0"                                                 // 2FAC
      u8"\xe9\x9d\x91\0"                                                 // 2FAD
      u8"\xe9\x9d\x9e\0"                                                 // 2FAE
      u8"\xe9\x9d\xa2\0"                                                 // 2FAF
      u8"\xe9\x9d\xa9\0"                                                 // 2FB0
      u8"\xe9\x9f\x8b\0"                                                 // 2FB1
      u8"\xe9\x9f\xad\0"                                                 // 2FB2
      u8"\xe9\x9f\xb3\0"                                                 // 2FB3
      u8"\xe9\xa0\x81\0"                                                 // 2FB4
      u8"\xe9\xa2\xa8\0"                                                 // 2FB5
      u8"\xe9\xa3\x9b\0"                                                 // 2FB6
      u8"\xe9\xa3\x9f\0"                                                 // 2FB7
      u8"\xe9\xa6\x96\0"                                                 // 2FB8
      u8"\xe9\xa6\x99\0"                                                 // 2FB9
      u8"\xe9\xa6\xac\0"                                                 // 2FBA
      u8"\xe9\xaa\xa8\0"                                                 // 2FBB
      u8"\xe9\xab\x98\0"                                                 // 2FBC
      u8"\xe9\xab\x9f\0"                                                 // 2FBD
      u8"\xe9\xac\xa5\0"                                                 // 2FBE
      u8"\xe9\xac\xaf\0"                                                 // 2FBF
      u8"\xe9\xac\xb2\0"                                                 // 2FC0
      u8"\xe9\xac\xbc\0"                                                 // 2FC1
      u8"\xe9\xad\x9a\0"                                                 // 2FC2
      u8"\xe9\xb3\xa5\0"                                                 // 2FC3
      u8"\xe9\xb9\xb5\0"                                                 // 2FC4
      u8"\xe9\xb9\xbf\0"                                                 // 2FC5, F940
      u8"\xe9\xba\xa5\0"                                                 // 2FC6
      u8"\xe9\xba\xbb\0"                                                 // 2FC7, 2FA15
      u8"\xe9\xbb\x83\0"                                                 // 2FC8
      u8"\xe9\xbb\x8d\0"                                                 // 2FC9
      u8"\xe9\xbb\x91\0"                                                 // 2FCA
      u8"\xe9\xbb\xb9\0"                                                 // 2FCB, 2FA17
      u8"\xe9\xbb\xbd\0"                                                 // 2FCC
      u8"\xe9\xbc\x8e\0"                                                 // 2FCD
      u8"\xe9\xbc\x93\0"                                                 // 2FCE
      u8"\xe9\xbc\xa0\0"                                                 // 2FCF
      u8"\xe9\xbc\xbb\0"                                                 // 2FD0, 2FA1C
      u8"\xe9\xbd\x8a\0"                                                 // 2FD1
      u8"\xe9\xbd\x92\0"                                                 // 2FD2
      u8"\xe9\xbe\x8d\0"                                                 // 2FD3, F9C4
      u8"\xe9\xbe\x9c\0"                                                 // 2FD4, F907, F908, FACE
      u8"\xe9\xbe\xa0\0"                                                 // 2FD5
      u8"\56\0"                                                          // 3002, FF0E, FF61
      u8"\xe3\x80\x92\0"                                                 // 3036
      u8"\xe5\x8d\x84\0"                                                 // 3039
      u8"\xe5\x8d\x85\0"                                                 // 303A
      u8"\40\xe3\x82\x99\0"                                              // 309B
      u8"\40\xe3\x82\x9a\0"                                              // 309C
      u8"\xe3\x82\x88\xe3\x82\x8a\0"                                     // 309F
      u8"\xe3\x82\xb3\xe3\x83\x88\0"                                     // 30FF
      u8"\xe1\x84\x80\0"                                                 // 3131, 3260, FFA1
      u8"\xe1\x84\x81\0"                                                 // 3132, FFA2
      u8"\xe1\x86\xaa\0"                                                 // 3133, FFA3
      u8"\xe1\x84\x82\0"                                                 // 3134, 3261, FFA4
      u8"\xe1\x86\xac\0"                                                 // 3135, FFA5
      u8"\xe1\x86\xad\0"                                                 // 3136, FFA6
      u8"\xe1\x84\x83\0"                                                 // 3137, 3262, FFA7
      u8"\xe1\x84\x84\0"                                                 // 3138, FFA8
      u8"\xe1\x84\x85\0"                                                 // 3139, 3263, FFA9
      u8"\xe1\x86\xb0\0"                                                 // 313A, FFAA
      u8"\xe1\x86\xb1\0"                                                 // 313B, FFAB
      u8"\xe1\x86\xb2\0"                                                 // 313C, FFAC
      u8"\xe1\x86\xb3\0"                                                 // 313D, FFAD
      u8"\xe1\x86\xb4\0"                                                 // 313E, FFAE
      u8"\xe1\x86\xb5\0"                                                 // 313F, FFAF
      u8"\xe1\x84\x9a\0"                                                 // 3140, FFB0
      u8"\xe1\x84\x86\0"                                                 // 3141, 3264, FFB1
      u8"\xe1\x84\x87\0"                                                 // 3142, 3265, FFB2
      u8"\xe1\x84\x88\0"                                                 // 3143, FFB3
      u8"\xe1\x84\xa1\0"                                                 // 3144, FFB4
      u8"\xe1\x84\x89\0"                                                 // 3145, 3266, FFB5
      u8"\xe1\x84\x8a\0"                                                 // 3146, FFB6
      u8"\xe1\x84\x8b\0"                                                 // 3147, 3267, FFB7
      u8"\xe1\x84\x8c\0"                                                 // 3148, 3268, FFB8
      u8"\xe1\x84\x8d\0"                                                 // 3149, FFB9
      u8"\xe1\x84\x8e\0"                                                 // 314A, 3269, FFBA
      u8"\xe1\x84\x8f\0"                                                 // 314B, 326A, FFBB
      u8"\xe1\x84\x90\0"                                                 // 314C, 326B, FFBC
      u8"\xe1\x84\x91\0"                                                 // 314D, 326C, FFBD
      u8"\xe1\x84\x92\0"                                                 // 314E, 326D, FFBE
      u8"\xe1\x85\xa1\0"                                                 // 314F, FFC2
      u8"\xe1\x85\xa2\0"                                                 // 3150, FFC3
      u8"\xe1\x85\xa3\0"                                                 // 3151, FFC4
      u8"\xe1\x85\xa4\0"                                                 // 3152, FFC5
      u8"\xe1\x85\xa5\0"                                                 // 3153, FFC6
      u8"\xe1\x85\xa6\0"                                                 // 3154, FFC7
      u8"\xe1\x85\xa7\0"                                                 // 3155, FFCA
      u8"\xe1\x85\xa8\0"                                                 // 3156, FFCB
      u8"\xe1\x85\xa9\0"                                                 // 3157, FFCC
      u8"\xe1\x85\xaa\0"                                                 // 3158, FFCD
      u8"\xe1\x85\xab\0"                                                 // 3159, FFCE
      u8"\xe1\x85\xac\0"                                                 // 315A, FFCF
      u8"\xe1\x85\xad\0"                                                 // 315B, FFD2
      u8"\xe1\x85\xae\0"                                                 // 315C, FFD3
      u8"\xe1\x85\xaf\0"                                                 // 315D, FFD4
      u8"\xe1\x85\xb0\0"                                                 // 315E, FFD5
      u8"\xe1\x85\xb1\0"                                                 // 315F, FFD6
      u8"\xe1\x85\xb2\0"                                                 // 3160, FFD7
      u8"\xe1\x85\xb3\0"                                                 // 3161, FFDA
      u8"\xe1\x85\xb4\0"                                                 // 3162, FFDB
      u8"\xe1\x85\xb5\0"                                                 // 3163, FFDC
      u8"\xe1\x84\x94\0"                                                 // 3165
      u8"\xe1\x84\x95\0"                                                 // 3166
      u8"\xe1\x87\x87\0"                                                 // 3167
      u8"\xe1\x87\x88\0"                                                 // 3168
      u8"\xe1\x87\x8c\0"                                                 // 3169
      u8"\xe1\x87\x8e\0"                                                 // 316A
      u8"\xe1\x87\x93\0"                                                 // 316B
      u8"\xe1\x87\x97\0"                                                 // 316C
      u8"\xe1\x87\x99\0"                                                 // 316D
      u8"\xe1\x84\x9c\0"                                                 // 316E
      u8"\xe1\x87\x9d\0"                                                 // 316F
      u8"\xe1\x87\x9f\0"                                                 // 3170
      u8"\xe1\x84\x9d\0"                                                 // 3171
      u8"\xe1\x84\x9e\0"                                                 // 3172
      u8"\xe1\x84\xa0\0"                                                 // 3173
      u8"\xe1\x84\xa2\0"                                                 // 3174
      u8"\xe1\x84\xa3\0"                                                 // 3175
      u8"\xe1\x84\xa7\0"                                                 // 3176
      u8"\xe1\x84\xa9\0"                                                 // 3177
      u8"\xe1\x84\xab\0"                                                 // 3178
      u8"\xe1\x84\xac\0"                                                 // 3179
      u8"\xe1\x84\xad\0"                                                 // 317A
      u8"\xe1\x84\xae\0"                                                 // 317B
      u8"\xe1\x84\xaf\0"                                                 // 317C
      u8"\xe1\x84\xb2\0"                                                 // 317D
      u8"\xe1\x84\xb6\0"                                                 // 317E
      u8"\xe1\x85\x80\0"                                                 // 317F
      u8"\xe1\x85\x87\0"                                                 // 3180
      u8"\xe1\x85\x8c\0"                                                 // 3181
      u8"\xe1\x87\xb1\0"                                                 // 3182
      u8"\xe1\x87\xb2\0"                                                 // 3183
      u8"\xe1\x85\x97\0"                                                 // 3184
      u8"\xe1\x85\x98\0"                                                 // 3185
      u8"\xe1\x85\x99\0"                                                 // 3186
      u8"\xe1\x86\x84\0"                                                 // 3187
      u8"\xe1\x86\x85\0"                                                 // 3188
      u8"\xe1\x86\x88\0"                                                 // 3189
      u8"\xe1\x86\x91\0"                                                 // 318A
      u8"\xe1\x86\x92\0"                                                 // 318B
      u8"\xe1\x86\x94\0"                                                 // 318C
      u8"\xe1\x86\x9e\0"                                                 // 318D
      u8"\xe1\x86\xa1\0"                                                 // 318E
      u8"\xe4\xb8\x89\0"                                                 // 3194, 3282, 1F22A
      u8"\xe5\x9b\x9b\0"                                                 // 3195, 3283
      u8"\xe4\xb8\x8a\0"                                                 // 3196, 32A4
      u8"\xe4\xb8\xad\0"                                                 // 3197, 32A5, 1F22D
      u8"\xe4\xb8\x8b\0"                                                 // 3198, 32A6
      u8"\xe7\x94\xb2\0"                                                 // 3199
      u8"\xe4\xb8\x99\0"                                                 // 319B
      u8"\xe4\xb8\x81\0"                                                 // 319C
      u8"\xe5\xa4\xa9\0"                                                 // 319D, 1F217
      u8"\xe5\x9c\xb0\0"                                                 // 319E
      u8"\50\xe1\x84\x80\51\0"                                           // 3200
      u8"\50\xe1\x84\x82\51\0"                                           // 3201
      u8"\50\xe1\x84\x83\51\0"                                           // 3202
      u8"\50\xe1\x84\x85\51\0"                                           // 3203
      u8"\50\xe1\x84\x86\51\0"                                           // 3204
      u8"\50\xe1\x84\x87\51\0"                                           // 3205
      u8"\50\xe1\x84\x89\51\0"                                           // 3206
      u8"\50\xe1\x84\x8b\51\0"                                           // 3207
      u8"\50\xe1\x84\x8c\51\0"                                           // 3208
      u8"\50\xe1\x84\x8e\51\0"                                           // 3209
      u8"\50\xe1\x84\x8f\51\0"                                           // 320A
      u8"\50\xe1\x84\x90\51\0"                                           // 320B
      u8"\50\xe1\x84\x91\51\0"                                           // 320C
      u8"\50\xe1\x84\x92\51\0"                                           // 320D
      u8"\50\xea\xb0\x80\51\0"                                           // 320E
      u8"\50\xeb\x82\x98\51\0"                                           // 320F
      u8"\50\xeb\x8b\xa4\51\0"                                           // 3210
      u8"\50\xeb\x9d\xbc\51\0"                                           // 3211
      u8"\50\xeb\xa7\x88\51\0"                                           // 3212
      u8"\50\xeb\xb0\x94\51\0"                                           // 3213
      u8"\50\xec\x82\xac\51\0"                                           // 3214
      u8"\50\xec\x95\x84\51\0"                                           // 3215
      u8"\50\xec\x9e\x90\51\0"                                           // 3216
      u8"\50\xec\xb0\xa8\51\0"                                           // 3217
      u8"\50\xec\xb9\xb4\51\0"                                           // 3218
      u8"\50\xed\x83\x80\51\0"                                           // 3219
      u8"\50\xed\x8c\x8c\51\0"                                           // 321A
      u8"\50\xed\x95\x98\51\0"                                           // 321B
      u8"\50\xec\xa3\xbc\51\0"                                           // 321C
      u8"\50\xec\x98\xa4\xec\xa0\x84\51\0"                               // 321D
      u8"\50\xec\x98\xa4\xed\x9b\x84\51\0"                               // 321E
      u8"\50\xe4\xb8\x80\51\0"                                           // 3220
      u8"\50\xe4\xba\x8c\51\0"                                           // 3221
      u8"\50\xe4\xb8\x89\51\0"                                           // 3222
      u8"\50\xe5\x9b\x9b\51\0"                                           // 3223
      u8"\50\xe4\xba\x94\51\0"                                           // 3224
      u8"\50\xe5\x85\xad\51\0"                                           // 3225
      u8"\50\xe4\xb8\x83\51\0"                                           // 3226
      u8"\50\xe5\x85\xab\51\0"                                           // 3227
      u8"\50\xe4\xb9\x9d\51\0"                                           // 3228
      u8"\50\xe5\x8d\x81\51\0"                                           // 3229
      u8"\50\xe6\x9c\x88\51\0"                                           // 322A
      u8"\50\xe7\x81\xab\51\0"                                           // 322B
      u8"\50\xe6\xb0\xb4\51\0"                                           // 322C
      u8"\50\xe6\x9c\xa8\51\0"                                           // 322D
      u8"\50\xe9\x87\x91\51\0"                                           // 322E
      u8"\50\xe5\x9c\x9f\51\0"                                           // 322F
      u8"\50\xe6\x97\xa5\51\0"                                           // 3230
      u8"\50\xe6\xa0\xaa\51\0"                                           // 3231
      u8"\50\xe6\x9c\x89\51\0"                                           // 3232
      u8"\50\xe7\xa4\xbe\51\0"                                           // 3233
      u8"\50\xe5\x90\x8d\51\0"                                           // 3234
      u8"\50\xe7\x89\xb9\51\0"                                           // 3235
      u8"\50\xe8\xb2\xa1\51\0"                                           // 3236
      u8"\50\xe7\xa5\x9d\51\0"                                           // 3237
      u8"\50\xe5\x8a\xb4\51\0"                                           // 3238
      u8"\50\xe4\xbb\xa3\51\0"                                           // 3239
      u8"\50\xe5\x91\xbc\51\0"                                           // 323A
      u8"\50\xe5\xad\xa6\51\0"                                           // 323B
      u8"\50\xe7\x9b\xa3\51\0"                                           // 323C
      u8"\50\xe4\xbc\x81\51\0"                                           // 323D
      u8"\50\xe8\xb3\x87\51\0"                                           // 323E
      u8"\50\xe5\x8d\x94\51\0"                                           // 323F
      u8"\50\xe7\xa5\xad\51\0"                                           // 3240
      u8"\50\xe4\xbc\x91\51\0"                                           // 3241
      u8"\50\xe8\x87\xaa\51\0"                                           // 3242
      u8"\50\xe8\x87\xb3\51\0"                                           // 3243
      u8"\xe5\x95\x8f\0"                                                 // 3244
      u8"\xe5\xb9\xbc\0"                                                 // 3245
      u8"\xe7\xae\x8f\0"                                                 // 3247
      u8"\x70\x74\x65\0"                                                 // 3250
      u8"\62\61\0"                                                       // 3251
      u8"\62\62\0"                                                       // 3252
      u8"\62\63\0"                                                       // 3253
      u8"\62\64\0"                                                       // 3254
      u8"\62\65\0"                                                       // 3255
      u8"\62\66\0"                                                       // 3256
      u8"\62\67\0"                                                       // 3257
      u8"\62\70\0"                                                       // 3258
      u8"\62\71\0"                                                       // 3259
      u8"\63\60\0"                                                       // 325A
      u8"\63\61\0"                                                       // 325B
      u8"\63\62\0"                                                       // 325C
      u8"\63\63\0"                                                       // 325D
      u8"\63\64\0"                                                       // 325E
      u8"\63\65\0"                                                       // 325F
      u8"\xea\xb0\x80\0"                                                 // 326E
      u8"\xeb\x82\x98\0"                                                 // 326F
      u8"\xeb\x8b\xa4\0"                                                 // 3270
      u8"\xeb\x9d\xbc\0"                                                 // 3271
      u8"\xeb\xa7\x88\0"                                                 // 3272
      u8"\xeb\xb0\x94\0"                                                 // 3273
      u8"\xec\x82\xac\0"                                                 // 3274
      u8"\xec\x95\x84\0"                                                 // 3275
      u8"\xec\x9e\x90\0"                                                 // 3276
      u8"\xec\xb0\xa8\0"                                                 // 3277
      u8"\xec\xb9\xb4\0"                                                 // 3278
      u8"\xed\x83\x80\0"                                                 // 3279
      u8"\xed\x8c\x8c\0"                                                 // 327A
      u8"\xed\x95\x98\0"                                                 // 327B
      u8"\xec\xb0\xb8\xea\xb3\xa0\0"                                     // 327C
      u8"\xec\xa3\xbc\xec\x9d\x98\0"                                     // 327D
      u8"\xec\x9a\xb0\0"                                                 // 327E
      u8"\xe4\xba\x94\0"                                                 // 3284
      u8"\xe5\x85\xad\0"                                                 // 3285, F9D1
      u8"\xe4\xb8\x83\0"                                                 // 3286
      u8"\xe4\xb9\x9d\0"                                                 // 3288
      u8"\xe6\xa0\xaa\0"                                                 // 3291
      u8"\xe6\x9c\x89\0"                                                 // 3292, 1F236
      u8"\xe7\xa4\xbe\0"                                                 // 3293, FA4C
      u8"\xe5\x90\x8d\0"                                                 // 3294
      u8"\xe7\x89\xb9\0"                                                 // 3295
      u8"\xe8\xb2\xa1\0"                                                 // 3296
      u8"\xe7\xa5\x9d\0"                                                 // 3297, FA51
      u8"\xe5\x8a\xb4\0"                                                 // 3298
      u8"\xe7\xa7\x98\0"                                                 // 3299
      u8"\xe7\x94\xb7\0"                                                 // 329A
      u8"\xe9\x81\xa9\0"                                                 // 329C
      u8"\xe5\x84\xaa\0"                                                 // 329D
      u8"\xe5\x8d\xb0\0"                                                 // 329E
      u8"\xe6\xb3\xa8\0"                                                 // 329F
      u8"\xe9\xa0\x85\0"                                                 // 32A0
      u8"\xe4\xbc\x91\0"                                                 // 32A1
      u8"\xe5\x86\x99\0"                                                 // 32A2
      u8"\xe6\xad\xa3\0"                                                 // 32A3
      u8"\xe5\xb7\xa6\0"                                                 // 32A7, 1F22C
      u8"\xe5\x8f\xb3\0"                                                 // 32A8, 1F22E
      u8"\xe5\x8c\xbb\0"                                                 // 32A9
      u8"\xe5\xae\x97\0"                                                 // 32AA
      u8"\xe5\xad\xa6\0"                                                 // 32AB
      u8"\xe7\x9b\xa3\0"                                                 // 32AC
      u8"\xe4\xbc\x81\0"                                                 // 32AD
      u8"\xe8\xb3\x87\0"                                                 // 32AE
      u8"\xe5\x8d\x94\0"                                                 // 32AF
      u8"\xe5\xa4\x9c\0"                                                 // 32B0
      u8"\63\66\0"                                                       // 32B1
      u8"\63\67\0"                                                       // 32B2
      u8"\63\70\0"                                                       // 32B3
      u8"\63\71\0"                                                       // 32B4
      u8"\64\60\0"                                                       // 32B5
      u8"\64\61\0"                                                       // 32B6
      u8"\64\62\0"                                                       // 32B7
      u8"\64\63\0"                                                       // 32B8
      u8"\64\64\0"                                                       // 32B9
      u8"\64\65\0"                                                       // 32BA
      u8"\64\66\0"                                                       // 32BB
      u8"\64\67\0"                                                       // 32BC
      u8"\64\70\0"                                                       // 32BD
      u8"\64\71\0"                                                       // 32BE
      u8"\65\60\0"                                                       // 32BF
      u8"\61\xe6\x9c\x88\0"                                              // 32C0
      u8"\62\xe6\x9c\x88\0"                                              // 32C1
      u8"\63\xe6\x9c\x88\0"                                              // 32C2
      u8"\64\xe6\x9c\x88\0"                                              // 32C3
      u8"\65\xe6\x9c\x88\0"                                              // 32C4
      u8"\66\xe6\x9c\x88\0"                                              // 32C5
      u8"\67\xe6\x9c\x88\0"                                              // 32C6
      u8"\70\xe6\x9c\x88\0"                                              // 32C7
      u8"\71\xe6\x9c\x88\0"                                              // 32C8
      u8"\61\60\xe6\x9c\x88\0"                                           // 32C9
      u8"\61\61\xe6\x9c\x88\0"                                           // 32CA
      u8"\61\62\xe6\x9c\x88\0"                                           // 32CB
      u8"\x68\x67\0"                                                     // 32CC
      u8"\x65\x72\x67\0"                                                 // 32CD
      u8"\x65\x76\0"                                                     // 32CE
      u8"\x6c\x74\x64\0"                                                 // 32CF
      u8"\xe3\x82\xa2\0"                                                 // 32D0, FF71
      u8"\xe3\x82\xa4\0"                                                 // 32D1, FF72
      u8"\xe3\x82\xa6\0"                                                 // 32D2, FF73
      u8"\xe3\x82\xa8\0"                                                 // 32D3, FF74
      u8"\xe3\x82\xaa\0"                                                 // 32D4, FF75
      u8"\xe3\x82\xab\0"                                                 // 32D5, FF76
      u8"\xe3\x82\xad\0"                                                 // 32D6, FF77
      u8"\xe3\x82\xaf\0"                                                 // 32D7, FF78
      u8"\xe3\x82\xb1\0"                                                 // 32D8, FF79
      u8"\xe3\x82\xb3\0"                                                 // 32D9, FF7A
      u8"\xe3\x82\xb5\0"                                                 // 32DA, FF7B, 1F202
      u8"\xe3\x82\xb7\0"                                                 // 32DB, FF7C
      u8"\xe3\x82\xb9\0"                                                 // 32DC, FF7D
      u8"\xe3\x82\xbb\0"                                                 // 32DD, FF7E
      u8"\xe3\x82\xbd\0"                                                 // 32DE, FF7F
      u8"\xe3\x82\xbf\0"                                                 // 32DF, FF80
      u8"\xe3\x83\x81\0"                                                 // 32E0, FF81
      u8"\xe3\x83\x84\0"                                                 // 32E1, FF82
      u8"\xe3\x83\x86\0"                                                 // 32E2, FF83
      u8"\xe3\x83\x88\0"                                                 // 32E3, FF84
      u8"\xe3\x83\x8a\0"                                                 // 32E4, FF85
      u8"\xe3\x83\x8b\0"                                                 // 32E5, FF86
      u8"\xe3\x83\x8c\0"                                                 // 32E6, FF87
      u8"\xe3\x83\x8d\0"                                                 // 32E7, FF88
      u8"\xe3\x83\x8e\0"                                                 // 32E8, FF89
      u8"\xe3\x83\x8f\0"                                                 // 32E9, FF8A
      u8"\xe3\x83\x92\0"                                                 // 32EA, FF8B
      u8"\xe3\x83\x95\0"                                                 // 32EB, FF8C
      u8"\xe3\x83\x98\0"                                                 // 32EC, FF8D
      u8"\xe3\x83\x9b\0"                                                 // 32ED, FF8E
      u8"\xe3\x83\x9e\0"                                                 // 32EE, FF8F
      u8"\xe3\x83\x9f\0"                                                 // 32EF, FF90
      u8"\xe3\x83\xa0\0"                                                 // 32F0, FF91
      u8"\xe3\x83\xa1\0"                                                 // 32F1, FF92
      u8"\xe3\x83\xa2\0"                                                 // 32F2, FF93
      u8"\xe3\x83\xa4\0"                                                 // 32F3, FF94
      u8"\xe3\x83\xa6\0"                                                 // 32F4, FF95
      u8"\xe3\x83\xa8\0"                                                 // 32F5, FF96
      u8"\xe3\x83\xa9\0"                                                 // 32F6, FF97
      u8"\xe3\x83\xaa\0"                                                 // 32F7, FF98
      u8"\xe3\x83\xab\0"                                                 // 32F8, FF99
      u8"\xe3\x83\xac\0"                                                 // 32F9, FF9A
      u8"\xe3\x83\xad\0"                                                 // 32FA, FF9B
      u8"\xe3\x83\xaf\0"                                                 // 32FB, FF9C
      u8"\xe3\x83\xb0\0"                                                 // 32FC
      u8"\xe3\x83\xb1\0"                                                 // 32FD
      u8"\xe3\x83\xb2\0"                                                 // 32FE, FF66
      u8"\xe4\xbb\xa4\xe5\x92\x8c\0"                                     // 32FF
      u8"\xe3\x82\xa2\xe3\x83\x91\xe3\x83\xbc\xe3\x83\x88\0"             // 3300
      u8"\xe3\x82\xa2\xe3\x83\xab\xe3\x83\x95\xe3\x82\xa1\0"             // 3301
      u8"\xe3\x82\xa2\xe3\x83\xb3\xe3\x83\x9a\xe3\x82\xa2\0"             // 3302
      u8"\xe3\x82\xa2\xe3\x83\xbc\xe3\x83\xab\0"                         // 3303
      u8"\xe3\x82\xa4\xe3\x83\x8b\xe3\x83\xb3\xe3\x82\xb0\0"             // 3304
      u8"\xe3\x82\xa4\xe3\x83\xb3\xe3\x83\x81\0"                         // 3305
      u8"\xe3\x82\xa6\xe3\x82\xa9\xe3\x83\xb3\0"                         // 3306
      u8"\xe3\x82\xa8\xe3\x82\xb9\xe3\x82\xaf\xe3\x83\xbc\xe3\x83\x89\0" // 3307
      u8"\xe3\x82\xa8\xe3\x83\xbc\xe3\x82\xab\xe3\x83\xbc\0"             // 3308
      u8"\xe3\x82\xaa\xe3\x83\xb3\xe3\x82\xb9\0"                         // 3309
      u8"\xe3\x82\xaa\xe3\x83\xbc\xe3\x83\xa0\0"                         // 330A
      u8"\xe3\x82\xab\xe3\x82\xa4\xe3\x83\xaa\0"                         // 330B
      u8"\xe3\x82\xab\xe3\x83\xa9\xe3\x83\x83\xe3\x83\x88\0"             // 330C
      u8"\xe3\x82\xab\xe3\x83\xad\xe3\x83\xaa\xe3\x83\xbc\0"             // 330D
      u8"\xe3\x82\xac\xe3\x83\xad\xe3\x83\xb3\0"                         // 330E
      u8"\xe3\x82\xac\xe3\x83\xb3\xe3\x83\x9e\0"                         // 330F
      u8"\xe3\x82\xae\xe3\x82\xac\0"                                     // 3310
      u8"\xe3\x82\xae\xe3\x83\x8b\xe3\x83\xbc\0"                         // 3311
      u8"\xe3\x82\xad\xe3\x83\xa5\xe3\x83\xaa\xe3\x83\xbc\0"             // 3312
      u8"\xe3\x82\xae\xe3\x83\xab\xe3\x83\x80\xe3\x83\xbc\0"             // 3313
      u8"\xe3\x82\xad\xe3\x83\xad\0"                                     // 3314
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\0" // 3315
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x83\xa1\xe3\x83\xbc\xe3\x83\x88\xe3\x83\xab\0" // 3316
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x83\xaf\xe3\x83\x83\xe3\x83\x88\0"             // 3317
      u8"\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\0"                                     // 3318
      u8"\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\xe3\x83\x88\xe3\x83\xb3\0"             // 3319
      u8"\xe3\x82\xaf\xe3\x83\xab\xe3\x82\xbc\xe3\x82\xa4\xe3\x83\xad\0"             // 331A
      u8"\xe3\x82\xaf\xe3\x83\xad\xe3\x83\xbc\xe3\x83\x8d\0"                         // 331B
      u8"\xe3\x82\xb1\xe3\x83\xbc\xe3\x82\xb9\0"                                     // 331C
      u8"\xe3\x82\xb3\xe3\x83\xab\xe3\x83\x8a\0"                                     // 331D
      u8"\xe3\x82\xb3\xe3\x83\xbc\xe3\x83\x9d\0"                                     // 331E
      u8"\xe3\x82\xb5\xe3\x82\xa4\xe3\x82\xaf\xe3\x83\xab\0"                         // 331F
      u8"\xe3\x82\xb5\xe3\x83\xb3\xe3\x83\x81\xe3\x83\xbc\xe3\x83\xa0\0"             // 3320
      u8"\xe3\x82\xb7\xe3\x83\xaa\xe3\x83\xb3\xe3\x82\xb0\0"                         // 3321
      u8"\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x81\0"                                     // 3322
      u8"\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x88\0"                                     // 3323
      u8"\xe3\x83\x80\xe3\x83\xbc\xe3\x82\xb9\0"                                     // 3324
      u8"\xe3\x83\x87\xe3\x82\xb7\0"                                                 // 3325
      u8"\xe3\x83\x89\xe3\x83\xab\0"                                                 // 3326
      u8"\xe3\x83\x88\xe3\x83\xb3\0"                                                 // 3327
      u8"\xe3\x83\x8a\xe3\x83\x8e\0"                                                 // 3328
      u8"\xe3\x83\x8e\xe3\x83\x83\xe3\x83\x88\0"                                     // 3329
      u8"\xe3\x83\x8f\xe3\x82\xa4\xe3\x83\x84\0"                                     // 332A
      u8"\xe3\x83\x91\xe3\x83\xbc\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x88\0"             // 332B
      u8"\xe3\x83\x91\xe3\x83\xbc\xe3\x83\x84\0"                                     // 332C
      u8"\xe3\x83\x90\xe3\x83\xbc\xe3\x83\xac\xe3\x83\xab\0"                         // 332D
      u8"\xe3\x83\x94\xe3\x82\xa2\xe3\x82\xb9\xe3\x83\x88\xe3\x83\xab\0"             // 332E
      u8"\xe3\x83\x94\xe3\x82\xaf\xe3\x83\xab\0"                                     // 332F
      u8"\xe3\x83\x94\xe3\x82\xb3\0"                                                 // 3330
      u8"\xe3\x83\x93\xe3\x83\xab\0"                                                 // 3331
      u8"\xe3\x83\x95\xe3\x82\xa1\xe3\x83\xa9\xe3\x83\x83\xe3\x83\x89\0"             // 3332
      u8"\xe3\x83\x95\xe3\x82\xa3\xe3\x83\xbc\xe3\x83\x88\0"                         // 3333
      u8"\xe3\x83\x96\xe3\x83\x83\xe3\x82\xb7\xe3\x82\xa7\xe3\x83\xab\0"             // 3334
      u8"\xe3\x83\x95\xe3\x83\xa9\xe3\x83\xb3\0"                                     // 3335
      u8"\xe3\x83\x98\xe3\x82\xaf\xe3\x82\xbf\xe3\x83\xbc\xe3\x83\xab\0"             // 3336
      u8"\xe3\x83\x9a\xe3\x82\xbd\0"                                                 // 3337
      u8"\xe3\x83\x9a\xe3\x83\x8b\xe3\x83\x92\0"                                     // 3338
      u8"\xe3\x83\x98\xe3\x83\xab\xe3\x83\x84\0"                                     // 3339
      u8"\xe3\x83\x9a\xe3\x83\xb3\xe3\x82\xb9\0"                                     // 333A
      u8"\xe3\x83\x9a\xe3\x83\xbc\xe3\x82\xb8\0"                                     // 333B
      u8"\xe3\x83\x99\xe3\x83\xbc\xe3\x82\xbf\0"                                     // 333C
      u8"\xe3\x83\x9d\xe3\x82\xa4\xe3\x83\xb3\xe3\x83\x88\0"                         // 333D
      u8"\xe3\x83\x9c\xe3\x83\xab\xe3\x83\x88\0"                                     // 333E
      u8"\xe3\x83\x9b\xe3\x83\xb3\0"                                                 // 333F
      u8"\xe3\x83\x9d\xe3\x83\xb3\xe3\x83\x89\0"                                     // 3340
      u8"\xe3\x83\x9b\xe3\x83\xbc\xe3\x83\xab\0"                                     // 3341
      u8"\xe3\x83\x9b\xe3\x83\xbc\xe3\x83\xb3\0"                                     // 3342
      u8"\xe3\x83\x9e\xe3\x82\xa4\xe3\x82\xaf\xe3\x83\xad\0"                         // 3343
      u8"\xe3\x83\x9e\xe3\x82\xa4\xe3\x83\xab\0"                                     // 3344
      u8"\xe3\x83\x9e\xe3\x83\x83\xe3\x83\x8f\0"                                     // 3345
      u8"\xe3\x83\x9e\xe3\x83\xab\xe3\x82\xaf\0"                                     // 3346
      u8"\xe3\x83\x9e\xe3\x83\xb3\xe3\x82\xb7\xe3\x83\xa7\xe3\x83\xb3\0"             // 3347
      u8"\xe3\x83\x9f\xe3\x82\xaf\xe3\x83\xad\xe3\x83\xb3\0"                         // 3348
      u8"\xe3\x83\x9f\xe3\x83\xaa\0"                                                 // 3349
      u8"\xe3\x83\x9f\xe3\x83\xaa\xe3\x83\x90\xe3\x83\xbc\xe3\x83\xab\0"             // 334A
      u8"\xe3\x83\xa1\xe3\x82\xac\0"                                                 // 334B
      u8"\xe3\x83\xa1\xe3\x82\xac\xe3\x83\x88\xe3\x83\xb3\0"                         // 334C
      u8"\xe3\x83\xa1\xe3\x83\xbc\xe3\x83\x88\xe3\x83\xab\0"                         // 334D
      u8"\xe3\x83\xa4\xe3\x83\xbc\xe3\x83\x89\0"                                     // 334E
      u8"\xe3\x83\xa4\xe3\x83\xbc\xe3\x83\xab\0"                                     // 334F
      u8"\xe3\x83\xa6\xe3\x82\xa2\xe3\x83\xb3\0"                                     // 3350
      u8"\xe3\x83\xaa\xe3\x83\x83\xe3\x83\x88\xe3\x83\xab\0"                         // 3351
      u8"\xe3\x83\xaa\xe3\x83\xa9\0"                                                 // 3352
      u8"\xe3\x83\xab\xe3\x83\x94\xe3\x83\xbc\0"                                     // 3353
      u8"\xe3\x83\xab\xe3\x83\xbc\xe3\x83\x96\xe3\x83\xab\0"                         // 3354
      u8"\xe3\x83\xac\xe3\x83\xa0\0"                                                 // 3355
      u8"\xe3\x83\xac\xe3\x83\xb3\xe3\x83\x88\xe3\x82\xb2\xe3\x83\xb3\0"             // 3356
      u8"\xe3\x83\xaf\xe3\x83\x83\xe3\x83\x88\0"                                     // 3357
      u8"\60\xe7\x82\xb9\0"                                                          // 3358
      u8"\61\xe7\x82\xb9\0"                                                          // 3359
      u8"\62\xe7\x82\xb9\0"                                                          // 335A
      u8"\63\xe7\x82\xb9\0"                                                          // 335B
      u8"\64\xe7\x82\xb9\0"                                                          // 335C
      u8"\65\xe7\x82\xb9\0"                                                          // 335D
      u8"\66\xe7\x82\xb9\0"                                                          // 335E
      u8"\67\xe7\x82\xb9\0"                                                          // 335F
      u8"\70\xe7\x82\xb9\0"                                                          // 3360
      u8"\71\xe7\x82\xb9\0"                                                          // 3361
      u8"\61\60\xe7\x82\xb9\0"                                                       // 3362
      u8"\61\61\xe7\x82\xb9\0"                                                       // 3363
      u8"\61\62\xe7\x82\xb9\0"                                                       // 3364
      u8"\61\63\xe7\x82\xb9\0"                                                       // 3365
      u8"\61\64\xe7\x82\xb9\0"                                                       // 3366
      u8"\61\65\xe7\x82\xb9\0"                                                       // 3367
      u8"\61\66\xe7\x82\xb9\0"                                                       // 3368
      u8"\61\67\xe7\x82\xb9\0"                                                       // 3369
      u8"\61\70\xe7\x82\xb9\0"                                                       // 336A
      u8"\61\71\xe7\x82\xb9\0"                                                       // 336B
      u8"\62\60\xe7\x82\xb9\0"                                                       // 336C
      u8"\62\61\xe7\x82\xb9\0"                                                       // 336D
      u8"\62\62\xe7\x82\xb9\0"                                                       // 336E
      u8"\62\63\xe7\x82\xb9\0"                                                       // 336F
      u8"\62\64\xe7\x82\xb9\0"                                                       // 3370
      u8"\x68\x70\x61\0"                                                             // 3371
      u8"\x64\x61\0"                                                                 // 3372
      u8"\x61\x75\0"                                                                 // 3373
      u8"\x62\x61\x72\0"                                                             // 3374
      u8"\x6f\x76\0"                                                                 // 3375
      u8"\x70\x63\0"                                                                 // 3376
      u8"\x64\x6d\0"                                                                 // 3377
      u8"\x64\x6d\62\0"                                                              // 3378
      u8"\x64\x6d\63\0"                                                              // 3379
      u8"\x69\x75\0"                                                                 // 337A
      u8"\xe5\xb9\xb3\xe6\x88\x90\0"                                                 // 337B
      u8"\xe6\x98\xad\xe5\x92\x8c\0"                                                 // 337C
      u8"\xe5\xa4\xa7\xe6\xad\xa3\0"                                                 // 337D
      u8"\xe6\x98\x8e\xe6\xb2\xbb\0"                                                 // 337E
      u8"\xe6\xa0\xaa\xe5\xbc\x8f\xe4\xbc\x9a\xe7\xa4\xbe\0"                         // 337F
      u8"\x70\x61\0"                                                                 // 3380, 33A9
      u8"\x6e\x61\0"                                                                 // 3381
      u8"\xce\xbc\x61\0"                                                             // 3382
      u8"\x6d\x61\0"                                                                 // 3383
      u8"\x6b\x61\0"                                                                 // 3384
      u8"\x6b\x62\0"                                                                 // 3385
      u8"\x6d\x62\0"                                                                 // 3386, 33D4
      u8"\x67\x62\0"                                                                 // 3387
      u8"\x63\x61\x6c\0"                                                             // 3388
      u8"\x6b\x63\x61\x6c\0"                                                         // 3389
      u8"\x70\x66\0"                                                                 // 338A
      u8"\x6e\x66\0"                                                                 // 338B
      u8"\xce\xbc\x66\0"                                                             // 338C
      u8"\xce\xbc\x67\0"                                                             // 338D
      u8"\x6d\x67\0"                                                                 // 338E
      u8"\x6b\x67\0"                                                                 // 338F
      u8"\x68\x7a\0"                                                                 // 3390
      u8"\x6b\x68\x7a\0"                                                             // 3391
      u8"\x6d\x68\x7a\0"                                                             // 3392
      u8"\x67\x68\x7a\0"                                                             // 3393
      u8"\x74\x68\x7a\0"                                                             // 3394
      u8"\xce\xbc\x6c\0"                                                             // 3395
      u8"\x6d\x6c\0"                                                                 // 3396
      u8"\x64\x6c\0"                                                                 // 3397
      u8"\x6b\x6c\0"                                                                 // 3398
      u8"\x66\x6d\0"                                                                 // 3399
      u8"\x6e\x6d\0"                                                                 // 339A
      u8"\xce\xbc\x6d\0"                                                             // 339B
      u8"\x6d\x6d\0"                                                                 // 339C
      u8"\x63\x6d\0"                                                                 // 339D
      u8"\x6b\x6d\0"                                                                 // 339E, 33CE
      u8"\x6d\x6d\62\0"                                                              // 339F
      u8"\x63\x6d\62\0"                                                              // 33A0
      u8"\x6d\62\0"                                                                  // 33A1
      u8"\x6b\x6d\62\0"                                                              // 33A2
      u8"\x6d\x6d\63\0"                                                              // 33A3
      u8"\x63\x6d\63\0"                                                              // 33A4
      u8"\x6d\63\0"                                                                  // 33A5
      u8"\x6b\x6d\63\0"                                                              // 33A6
      u8"\x6d\xe2\x88\x95\x73\0"                                                     // 33A7
      u8"\x6d\xe2\x88\x95\x73\62\0"                                                  // 33A8
      u8"\x6b\x70\x61\0"                                                             // 33AA
      u8"\x6d\x70\x61\0"                                                             // 33AB
      u8"\x67\x70\x61\0"                                                             // 33AC
      u8"\x72\x61\x64\0"                                                             // 33AD
      u8"\x72\x61\x64\xe2\x88\x95\x73\0"                                             // 33AE
      u8"\x72\x61\x64\xe2\x88\x95\x73\62\0"                                          // 33AF
      u8"\x70\x73\0"                                                                 // 33B0
      u8"\x6e\x73\0"                                                                 // 33B1
      u8"\xce\xbc\x73\0"                                                             // 33B2
      u8"\x6d\x73\0"                                                                 // 33B3
      u8"\x70\x76\0"                                                                 // 33B4
      u8"\x6e\x76\0"                                                                 // 33B5
      u8"\xce\xbc\x76\0"                                                             // 33B6
      u8"\x6d\x76\0"                                                                 // 33B7, 33B9, 1F14B
      u8"\x6b\x76\0"                                                                 // 33B8
      u8"\x70\x77\0"                                                                 // 33BA
      u8"\x6e\x77\0"                                                                 // 33BB
      u8"\xce\xbc\x77\0"                                                             // 33BC
      u8"\x6d\x77\0"                                                                 // 33BD, 33BF
      u8"\x6b\x77\0"                                                                 // 33BE
      u8"\x6b\xcf\x89\0"                                                             // 33C0
      u8"\x6d\xcf\x89\0"                                                             // 33C1
      u8"\x62\x71\0"                                                                 // 33C3
      u8"\x63\x63\0"                                                                 // 33C4
      u8"\x63\x64\0"                                                                 // 33C5, 1F12D
      u8"\x63\xe2\x88\x95\x6b\x67\0"                                                 // 33C6
      u8"\x64\x62\0"                                                                 // 33C8
      u8"\x67\x79\0"                                                                 // 33C9
      u8"\x68\x61\0"                                                                 // 33CA
      u8"\x68\x70\0"                                                                 // 33CB
      u8"\x69\x6e\0"                                                                 // 33CC
      u8"\x6b\x6b\0"                                                                 // 33CD
      u8"\x6b\x74\0"                                                                 // 33CF
      u8"\x6c\x6d\0"                                                                 // 33D0
      u8"\x6c\x6e\0"                                                                 // 33D1
      u8"\x6c\x6f\x67\0"                                                             // 33D2
      u8"\x6c\x78\0"                                                                 // 33D3
      u8"\x6d\x69\x6c\0"                                                             // 33D5
      u8"\x6d\x6f\x6c\0"                                                             // 33D6
      u8"\x70\x68\0"                                                                 // 33D7
      u8"\x70\x70\x6d\0"                                                             // 33D9
      u8"\x70\x72\0"                                                                 // 33DA
      u8"\x73\x72\0"                                                                 // 33DB
      u8"\x73\x76\0"                                                                 // 33DC
      u8"\x77\x62\0"                                                                 // 33DD
      u8"\x76\xe2\x88\x95\x6d\0"                                                     // 33DE
      u8"\x61\xe2\x88\x95\x6d\0"                                                     // 33DF
      u8"\61\xe6\x97\xa5\0"                                                          // 33E0
      u8"\62\xe6\x97\xa5\0"                                                          // 33E1
      u8"\63\xe6\x97\xa5\0"                                                          // 33E2
      u8"\64\xe6\x97\xa5\0"                                                          // 33E3
      u8"\65\xe6\x97\xa5\0"                                                          // 33E4
      u8"\66\xe6\x97\xa5\0"                                                          // 33E5
      u8"\67\xe6\x97\xa5\0"                                                          // 33E6
      u8"\70\xe6\x97\xa5\0"                                                          // 33E7
      u8"\71\xe6\x97\xa5\0"                                                          // 33E8
      u8"\61\60\xe6\x97\xa5\0"                                                       // 33E9
      u8"\61\61\xe6\x97\xa5\0"                                                       // 33EA
      u8"\61\62\xe6\x97\xa5\0"                                                       // 33EB
      u8"\61\63\xe6\x97\xa5\0"                                                       // 33EC
      u8"\61\64\xe6\x97\xa5\0"                                                       // 33ED
      u8"\61\65\xe6\x97\xa5\0"                                                       // 33EE
      u8"\61\66\xe6\x97\xa5\0"                                                       // 33EF
      u8"\61\67\xe6\x97\xa5\0"                                                       // 33F0
      u8"\61\70\xe6\x97\xa5\0"                                                       // 33F1
      u8"\61\71\xe6\x97\xa5\0"                                                       // 33F2
      u8"\62\60\xe6\x97\xa5\0"                                                       // 33F3
      u8"\62\61\xe6\x97\xa5\0"                                                       // 33F4
      u8"\62\62\xe6\x97\xa5\0"                                                       // 33F5
      u8"\62\63\xe6\x97\xa5\0"                                                       // 33F6
      u8"\62\64\xe6\x97\xa5\0"                                                       // 33F7
      u8"\62\65\xe6\x97\xa5\0"                                                       // 33F8
      u8"\62\66\xe6\x97\xa5\0"                                                       // 33F9
      u8"\62\67\xe6\x97\xa5\0"                                                       // 33FA
      u8"\62\70\xe6\x97\xa5\0"                                                       // 33FB
      u8"\62\71\xe6\x97\xa5\0"                                                       // 33FC
      u8"\63\60\xe6\x97\xa5\0"                                                       // 33FD
      u8"\63\61\xe6\x97\xa5\0"                                                       // 33FE
      u8"\x67\x61\x6c\0"                                                             // 33FF
      u8"\xea\x99\x81\0"                                                             // A640
      u8"\xea\x99\x83\0"                                                             // A642
      u8"\xea\x99\x85\0"                                                             // A644
      u8"\xea\x99\x87\0"                                                             // A646
      u8"\xea\x99\x89\0"                                                             // A648
      u8"\xea\x99\x8d\0"                                                             // A64C
      u8"\xea\x99\x8f\0"                                                             // A64E
      u8"\xea\x99\x91\0"                                                             // A650, 1E06C
      u8"\xea\x99\x93\0"                                                             // A652
      u8"\xea\x99\x95\0"                                                             // A654
      u8"\xea\x99\x97\0"                                                             // A656
      u8"\xea\x99\x99\0"                                                             // A658
      u8"\xea\x99\x9b\0"                                                             // A65A
      u8"\xea\x99\x9d\0"                                                             // A65C
      u8"\xea\x99\x9f\0"                                                             // A65E
      u8"\xea\x99\xa1\0"                                                             // A660
      u8"\xea\x99\xa3\0"                                                             // A662
      u8"\xea\x99\xa5\0"                                                             // A664
      u8"\xea\x99\xa7\0"                                                             // A666
      u8"\xea\x99\xa9\0"                                                             // A668
      u8"\xea\x99\xab\0"                                                             // A66A
      u8"\xea\x99\xad\0"                                                             // A66C
      u8"\xea\x9a\x81\0"                                                             // A680
      u8"\xea\x9a\x83\0"                                                             // A682
      u8"\xea\x9a\x85\0"                                                             // A684
      u8"\xea\x9a\x87\0"                                                             // A686
      u8"\xea\x9a\x89\0"                                                             // A688, 1E04A
      u8"\xea\x9a\x8b\0"                                                             // A68A
      u8"\xea\x9a\x8d\0"                                                             // A68C
      u8"\xea\x9a\x8f\0"                                                             // A68E
      u8"\xea\x9a\x91\0"                                                             // A690
      u8"\xea\x9a\x93\0"                                                             // A692
      u8"\xea\x9a\x95\0"                                                             // A694
      u8"\xea\x9a\x97\0"                                                             // A696
      u8"\xea\x9a\x99\0"                                                             // A698
      u8"\xea\x9a\x9b\0"                                                             // A69A
      u8"\xea\x9c\xa3\0"                                                             // A722
      u8"\xea\x9c\xa5\0"                                                             // A724
      u8"\xea\x9c\xa7\0"                                                             // A726, AB5C
      u8"\xea\x9c\xa9\0"                                                             // A728
      u8"\xea\x9c\xab\0"                                                             // A72A
      u8"\xea\x9c\xad\0"                                                             // A72C
      u8"\xea\x9c\xaf\0"                                                             // A72E
      u8"\xea\x9c\xb3\0"                                                             // A732
      u8"\xea\x9c\xb5\0"                                                             // A734
      u8"\xea\x9c\xb7\0"                                                             // A736
      u8"\xea\x9c\xb9\0"                                                             // A738
      u8"\xea\x9c\xbb\0"                                                             // A73A
      u8"\xea\x9c\xbd\0"                                                             // A73C
      u8"\xea\x9c\xbf\0"                                                             // A73E
      u8"\xea\x9d\x81\0"                                                             // A740
      u8"\xea\x9d\x83\0"                                                             // A742
      u8"\xea\x9d\x85\0"                                                             // A744
      u8"\xea\x9d\x87\0"                                                             // A746
      u8"\xea\x9d\x89\0"                                                             // A748
      u8"\xea\x9d\x8b\0"                                                             // A74A
      u8"\xea\x9d\x8d\0"                                                             // A74C
      u8"\xea\x9d\x8f\0"                                                             // A74E
      u8"\xea\x9d\x91\0"                                                             // A750
      u8"\xea\x9d\x93\0"                                                             // A752
      u8"\xea\x9d\x95\0"                                                             // A754
      u8"\xea\x9d\x97\0"                                                             // A756
      u8"\xea\x9d\x99\0"                                                             // A758
      u8"\xea\x9d\x9b\0"                                                             // A75A
      u8"\xea\x9d\x9d\0"                                                             // A75C
      u8"\xea\x9d\x9f\0"                                                             // A75E
      u8"\xea\x9d\xa1\0"                                                             // A760
      u8"\xea\x9d\xa3\0"                                                             // A762
      u8"\xea\x9d\xa5\0"                                                             // A764
      u8"\xea\x9d\xa7\0"                                                             // A766
      u8"\xea\x9d\xa9\0"                                                             // A768
      u8"\xea\x9d\xab\0"                                                             // A76A
      u8"\xea\x9d\xad\0"                                                             // A76C
      u8"\xea\x9d\xaf\0"                                                             // A76E, A770
      u8"\xea\x9d\xba\0"                                                             // A779
      u8"\xea\x9d\xbc\0"                                                             // A77B
      u8"\xe1\xb5\xb9\0"                                                             // A77D
      u8"\xea\x9d\xbf\0"                                                             // A77E
      u8"\xea\x9e\x81\0"                                                             // A780
      u8"\xea\x9e\x83\0"                                                             // A782
      u8"\xea\x9e\x85\0"                                                             // A784
      u8"\xea\x9e\x87\0"                                                             // A786
      u8"\xea\x9e\x8c\0"                                                             // A78B
      u8"\xea\x9e\x91\0"                                                             // A790
      u8"\xea\x9e\x93\0"                                                             // A792
      u8"\xea\x9e\x97\0"                                                             // A796
      u8"\xea\x9e\x99\0"                                                             // A798
      u8"\xea\x9e\x9b\0"                                                             // A79A
      u8"\xea\x9e\x9d\0"                                                             // A79C
      u8"\xea\x9e\x9f\0"                                                             // A79E
      u8"\xea\x9e\xa1\0"                                                             // A7A0
      u8"\xea\x9e\xa3\0"                                                             // A7A2
      u8"\xea\x9e\xa5\0"                                                             // A7A4
      u8"\xea\x9e\xa7\0"                                                             // A7A6
      u8"\xea\x9e\xa9\0"                                                             // A7A8
      u8"\xc9\xac\0"                                                                 // A7AD, 1079B
      u8"\xca\x9e\0"                                                                 // A7B0
      u8"\xca\x87\0"                                                                 // A7B1
      u8"\xea\xad\x93\0"                                                             // A7B3
      u8"\xea\x9e\xb5\0"                                                             // A7B4
      u8"\xea\x9e\xb7\0"                                                             // A7B6
      u8"\xea\x9e\xb9\0"                                                             // A7B8
      u8"\xea\x9e\xbb\0"                                                             // A7BA
      u8"\xea\x9e\xbd\0"                                                             // A7BC
      u8"\xea\x9e\xbf\0"                                                             // A7BE
      u8"\xea\x9f\x81\0"                                                             // A7C0
      u8"\xea\x9f\x83\0"                                                             // A7C2
      u8"\xea\x9e\x94\0"                                                             // A7C4
      u8"\xe1\xb6\x8e\0"                                                             // A7C6
      u8"\xea\x9f\x88\0"                                                             // A7C7
      u8"\xea\x9f\x8a\0"                                                             // A7C9
      u8"\xc9\xa4\0"                                                                 // A7CB, 10791
      u8"\xea\x9f\x8d\0"                                                             // A7CC
      u8"\xea\x9f\x91\0"                                                             // A7D0
      u8"\xea\x9f\x97\0"                                                             // A7D6
      u8"\xea\x9f\x99\0"                                                             // A7D8
      u8"\xea\x9f\x9b\0"                                                             // A7DA
      u8"\xc6\x9b\0"                                                                 // A7DC
      u8"\xea\x9f\xb6\0"                                                             // A7F5
      u8"\xea\xac\xb7\0"                                                             // AB5D
      u8"\xea\xad\x92\0"                                                             // AB5F
      u8"\xca\x8d\0"                                                                 // AB69
      u8"\xe1\x8e\xa0\0"                                                             // AB70
      u8"\xe1\x8e\xa1\0"                                                             // AB71
      u8"\xe1\x8e\xa2\0"                                                             // AB72
      u8"\xe1\x8e\xa3\0"                                                             // AB73
      u8"\xe1\x8e\xa4\0"                                                             // AB74
      u8"\xe1\x8e\xa5\0"                                                             // AB75
      u8"\xe1\x8e\xa6\0"                                                             // AB76
      u8"\xe1\x8e\xa7\0"                                                             // AB77
      u8"\xe1\x8e\xa8\0"                                                             // AB78
      u8"\xe1\x8e\xa9\0"                                                             // AB79
      u8"\xe1\x8e\xaa\0"                                                             // AB7A
      u8"\xe1\x8e\xab\0"                                                             // AB7B
      u8"\xe1\x8e\xac\0"                                                             // AB7C
      u8"\xe1\x8e\xad\0"                                                             // AB7D
      u8"\xe1\x8e\xae\0"                                                             // AB7E
      u8"\xe1\x8e\xaf\0"                                                             // AB7F
      u8"\xe1\x8e\xb0\0"                                                             // AB80
      u8"\xe1\x8e\xb1\0"                                                             // AB81
      u8"\xe1\x8e\xb2\0"                                                             // AB82
      u8"\xe1\x8e\xb3\0"                                                             // AB83
      u8"\xe1\x8e\xb4\0"                                                             // AB84
      u8"\xe1\x8e\xb5\0"                                                             // AB85
      u8"\xe1\x8e\xb6\0"                                                             // AB86
      u8"\xe1\x8e\xb7\0"                                                             // AB87
      u8"\xe1\x8e\xb8\0"                                                             // AB88
      u8"\xe1\x8e\xb9\0"                                                             // AB89
      u8"\xe1\x8e\xba\0"                                                             // AB8A
      u8"\xe1\x8e\xbb\0"                                                             // AB8B
      u8"\xe1\x8e\xbc\0"                                                             // AB8C
      u8"\xe1\x8e\xbd\0"                                                             // AB8D
      u8"\xe1\x8e\xbe\0"                                                             // AB8E
      u8"\xe1\x8e\xbf\0"                                                             // AB8F
      u8"\xe1\x8f\x80\0"                                                             // AB90
      u8"\xe1\x8f\x81\0"                                                             // AB91
      u8"\xe1\x8f\x82\0"                                                             // AB92
      u8"\xe1\x8f\x83\0"                                                             // AB93
      u8"\xe1\x8f\x84\0"                                                             // AB94
      u8"\xe1\x8f\x85\0"                                                             // AB95
      u8"\xe1\x8f\x86\0"                                                             // AB96
      u8"\xe1\x8f\x87\0"                                                             // AB97
      u8"\xe1\x8f\x88\0"                                                             // AB98
      u8"\xe1\x8f\x89\0"                                                             // AB99
      u8"\xe1\x8f\x8a\0"                                                             // AB9A
      u8"\xe1\x8f\x8b\0"                                                             // AB9B
      u8"\xe1\x8f\x8c\0"                                                             // AB9C
      u8"\xe1\x8f\x8d\0"                                                             // AB9D
      u8"\xe1\x8f\x8e\0"                                                             // AB9E
      u8"\xe1\x8f\x8f\0"                                                             // AB9F
      u8"\xe1\x8f\x90\0"                                                             // ABA0
      u8"\xe1\x8f\x91\0"                                                             // ABA1
      u8"\xe1\x8f\x92\0"                                                             // ABA2
      u8"\xe1\x8f\x93\0"                                                             // ABA3
      u8"\xe1\x8f\x94\0"                                                             // ABA4
      u8"\xe1\x8f\x95\0"                                                             // ABA5
      u8"\xe1\x8f\x96\0"                                                             // ABA6
      u8"\xe1\x8f\x97\0"                                                             // ABA7
      u8"\xe1\x8f\x98\0"                                                             // ABA8
      u8"\xe1\x8f\x99\0"                                                             // ABA9
      u8"\xe1\x8f\x9a\0"                                                             // ABAA
      u8"\xe1\x8f\x9b\0"                                                             // ABAB
      u8"\xe1\x8f\x9c\0"                                                             // ABAC
      u8"\xe1\x8f\x9d\0"                                                             // ABAD
      u8"\xe1\x8f\x9e\0"                                                             // ABAE
      u8"\xe1\x8f\x9f\0"                                                             // ABAF
      u8"\xe1\x8f\xa0\0"                                                             // ABB0
      u8"\xe1\x8f\xa1\0"                                                             // ABB1
      u8"\xe1\x8f\xa2\0"                                                             // ABB2
      u8"\xe1\x8f\xa3\0"                                                             // ABB3
      u8"\xe1\x8f\xa4\0"                                                             // ABB4
      u8"\xe1\x8f\xa5\0"                                                             // ABB5
      u8"\xe1\x8f\xa6\0"                                                             // ABB6
      u8"\xe1\x8f\xa7\0"                                                             // ABB7
      u8"\xe1\x8f\xa8\0"                                                             // ABB8
      u8"\xe1\x8f\xa9\0"                                                             // ABB9
      u8"\xe1\x8f\xaa\0"                                                             // ABBA
      u8"\xe1\x8f\xab\0"                                                             // ABBB
      u8"\xe1\x8f\xac\0"                                                             // ABBC
      u8"\xe1\x8f\xad\0"                                                             // ABBD
      u8"\xe1\x8f\xae\0"                                                             // ABBE
      u8"\xe1\x8f\xaf\0"                                                             // ABBF
      u8"\xe8\xb1\x88\0"                                                             // F900
      u8"\xe6\x9b\xb4\0"                                                             // F901
      u8"\xe8\xb3\x88\0"                                                             // F903
      u8"\xe6\xbb\x91\0"                                                             // F904
      u8"\xe4\xb8\xb2\0"                                                             // F905
      u8"\xe5\x8f\xa5\0"                                                             // F906
      u8"\xe5\xa5\x91\0"                                                             // F909
      u8"\xe5\x96\x87\0"                                                             // F90B
      u8"\xe5\xa5\x88\0"                                                             // F90C
      u8"\xe6\x87\xb6\0"                                                             // F90D, 2F8B1
      u8"\xe7\x99\xa9\0"                                                             // F90E
      u8"\xe7\xbe\x85\0"                                                             // F90F
      u8"\xe8\x98\xbf\0"                                                             // F910
      u8"\xe8\x9e\xba\0"                                                             // F911
      u8"\xe8\xa3\xb8\0"                                                             // F912
      u8"\xe9\x82\x8f\0"                                                             // F913
      u8"\xe6\xa8\x82\0"                                                             // F914, F95C, F9BF
      u8"\xe6\xb4\x9b\0"                                                             // F915
      u8"\xe7\x83\x99\0"                                                             // F916
      u8"\xe7\x8f\x9e\0"                                                             // F917
      u8"\xe8\x90\xbd\0"                                                             // F918
      u8"\xe9\x85\xaa\0"                                                             // F919
      u8"\xe9\xa7\xb1\0"                                                             // F91A
      u8"\xe4\xba\x82\0"                                                             // F91B
      u8"\xe5\x8d\xb5\0"                                                             // F91C
      u8"\xe6\xac\x84\0"                                                             // F91D
      u8"\xe7\x88\x9b\0"                                                             // F91E
      u8"\xe8\x98\xad\0"                                                             // F91F
      u8"\xe9\xb8\x9e\0"                                                             // F920
      u8"\xe5\xb5\x90\0"                                                             // F921
      u8"\xe6\xbf\xab\0"                                                             // F922
      u8"\xe8\x97\x8d\0"                                                             // F923
      u8"\xe8\xa5\xa4\0"                                                             // F924
      u8"\xe6\x8b\x89\0"                                                             // F925
      u8"\xe8\x87\x98\0"                                                             // F926
      u8"\xe8\xa0\x9f\0"                                                             // F927
      u8"\xe5\xbb\x8a\0"                                                             // F928, 2F88E
      u8"\xe6\x9c\x97\0"                                                             // F929, FA92, 2F8D8
      u8"\xe6\xb5\xaa\0"                                                             // F92A
      u8"\xe7\x8b\xbc\0"                                                             // F92B
      u8"\xe9\x83\x8e\0"                                                             // F92C
      u8"\xe4\xbe\x86\0"                                                             // F92D
      u8"\xe5\x86\xb7\0"                                                             // F92E
      u8"\xe5\x8b\x9e\0"                                                             // F92F
      u8"\xe6\x93\x84\0"                                                             // F930
      u8"\xe6\xab\x93\0"                                                             // F931
      u8"\xe7\x88\x90\0"                                                             // F932
      u8"\xe7\x9b\xa7\0"                                                             // F933
      u8"\xe8\x98\x86\0"                                                             // F935
      u8"\xe8\x99\x9c\0"                                                             // F936, 2F9B4
      u8"\xe8\xb7\xaf\0"                                                             // F937
      u8"\xe9\x9c\xb2\0"                                                             // F938
      u8"\xe9\xad\xaf\0"                                                             // F939
      u8"\xe9\xb7\xba\0"                                                             // F93A
      u8"\xe7\xa2\x8c\0"                                                             // F93B, 2F94F
      u8"\xe7\xa5\xbf\0"                                                             // F93C
      u8"\xe7\xb6\xa0\0"                                                             // F93D
      u8"\xe8\x8f\x89\0"                                                             // F93E
      u8"\xe9\x8c\x84\0"                                                             // F93F
      u8"\xe8\xab\x96\0"                                                             // F941
      u8"\xe5\xa3\x9f\0"                                                             // F942
      u8"\xe5\xbc\x84\0"                                                             // F943
      u8"\xe7\xb1\xa0\0"                                                             // F944
      u8"\xe8\x81\xbe\0"                                                             // F945
      u8"\xe7\x89\xa2\0"                                                             // F946
      u8"\xe7\xa3\x8a\0"                                                             // F947
      u8"\xe8\xb3\x82\0"                                                             // F948
      u8"\xe9\x9b\xb7\0"                                                             // F949
      u8"\xe5\xa3\x98\0"                                                             // F94A
      u8"\xe5\xb1\xa2\0"                                                             // F94B
      u8"\xe6\xa8\x93\0"                                                             // F94C
      u8"\xe6\xb7\x9a\0"                                                             // F94D
      u8"\xe6\xbc\x8f\0"                                                             // F94E
      u8"\xe7\xb4\xaf\0"                                                             // F94F
      u8"\xe7\xb8\xb7\0"                                                             // F950
      u8"\xe9\x99\x8b\0"                                                             // F951
      u8"\xe5\x8b\x92\0"                                                             // F952
      u8"\xe8\x82\x8b\0"                                                             // F953
      u8"\xe5\x87\x9c\0"                                                             // F954
      u8"\xe5\x87\x8c\0"                                                             // F955
      u8"\xe7\xa8\x9c\0"                                                             // F956
      u8"\xe7\xb6\xbe\0"                                                             // F957
      u8"\xe8\x8f\xb1\0"                                                             // F958
      u8"\xe9\x99\xb5\0"                                                             // F959
      u8"\xe8\xae\x80\0"                                                             // F95A
      u8"\xe6\x8b\x8f\0"                                                             // F95B
      u8"\xe8\xab\xbe\0"                                                             // F95D, FABD
      u8"\xe4\xb8\xb9\0"                                                             // F95E
      u8"\xe5\xaf\xa7\0"                                                             // F95F, F9AA, 2F86F
      u8"\xe6\x80\x92\0"                                                             // F960
      u8"\xe7\x8e\x87\0"                                                             // F961, F9DB
      u8"\xe7\x95\xb0\0"                                                             // F962, 2F938
      u8"\xe5\x8c\x97\0"                                                             // F963, 2F82B
      u8"\xe7\xa3\xbb\0"                                                             // F964
      u8"\xe4\xbe\xbf\0"                                                             // F965
      u8"\xe5\xbe\xa9\0"                                                             // F966
      u8"\xe4\xb8\x8d\0"                                                             // F967
      u8"\xe6\xb3\x8c\0"                                                             // F968
      u8"\xe6\x95\xb8\0"                                                             // F969
      u8"\xe7\xb4\xa2\0"                                                             // F96A
      u8"\xe5\x8f\x83\0"                                                             // F96B
      u8"\xe5\xa1\x9e\0"                                                             // F96C
      u8"\xe7\x9c\x81\0"                                                             // F96D
      u8"\xe8\x91\x89\0"                                                             // F96E
      u8"\xe8\xaa\xaa\0"                                                             // F96F, F9A1
      u8"\xe6\xae\xba\0"                                                             // F970, FA96, 2F8F5
      u8"\xe6\xb2\x88\0"                                                             // F972
      u8"\xe6\x8b\xbe\0"                                                             // F973
      u8"\xe8\x8b\xa5\0"                                                             // F974, 2F998
      u8"\xe6\x8e\xa0\0"                                                             // F975
      u8"\xe7\x95\xa5\0"                                                             // F976
      u8"\xe4\xba\xae\0"                                                             // F977
      u8"\xe5\x85\xa9\0"                                                             // F978
      u8"\xe5\x87\x89\0"                                                             // F979
      u8"\xe6\xa2\x81\0"                                                             // F97A
      u8"\xe7\xb3\xa7\0"                                                             // F97B
      u8"\xe8\x89\xaf\0"                                                             // F97C
      u8"\xe8\xab\x92\0"                                                             // F97D
      u8"\xe9\x87\x8f\0"                                                             // F97E
      u8"\xe5\x8b\xb5\0"                                                             // F97F
      u8"\xe5\x91\x82\0"                                                             // F980
      u8"\xe5\xbb\xac\0"                                                             // F982
      u8"\xe6\x97\x85\0"                                                             // F983
      u8"\xe6\xbf\xbe\0"                                                             // F984
      u8"\xe7\xa4\xaa\0"                                                             // F985
      u8"\xe9\x96\xad\0"                                                             // F986
      u8"\xe9\xa9\xaa\0"                                                             // F987
      u8"\xe9\xba\x97\0"                                                             // F988
      u8"\xe9\xbb\x8e\0"                                                             // F989
      u8"\xe6\x9b\x86\0"                                                             // F98B
      u8"\xe6\xad\xb7\0"                                                             // F98C
      u8"\xe8\xbd\xa2\0"                                                             // F98D
      u8"\xe5\xb9\xb4\0"                                                             // F98E
      u8"\xe6\x86\x90\0"                                                             // F98F
      u8"\xe6\x88\x80\0"                                                             // F990
      u8"\xe6\x92\x9a\0"                                                             // F991
      u8"\xe6\xbc\xa3\0"                                                             // F992
      u8"\xe7\x85\x89\0"                                                             // F993
      u8"\xe7\x92\x89\0"                                                             // F994
      u8"\xe7\xa7\x8a\0"                                                             // F995
      u8"\xe7\xb7\xb4\0"                                                             // F996, FA57, FAB0
      u8"\xe8\x81\xaf\0"                                                             // F997
      u8"\xe8\xbc\xa6\0"                                                             // F998
      u8"\xe8\x93\xae\0"                                                             // F999
      u8"\xe9\x80\xa3\0"                                                             // F99A
      u8"\xe9\x8d\x8a\0"                                                             // F99B
      u8"\xe5\x88\x97\0"                                                             // F99C
      u8"\xe5\x8a\xa3\0"                                                             // F99D
      u8"\xe5\x92\xbd\0"                                                             // F99E
      u8"\xe7\x83\x88\0"                                                             // F99F
      u8"\xe8\xa3\x82\0"                                                             // F9A0
      u8"\xe5\xbb\x89\0"                                                             // F9A2
      u8"\xe5\xbf\xb5\0"                                                             // F9A3
      u8"\xe6\x8d\xbb\0"                                                             // F9A4
      u8"\xe6\xae\xae\0"                                                             // F9A5
      u8"\xe7\xb0\xbe\0"                                                             // F9A6
      u8"\xe7\x8d\xb5\0"                                                             // F9A7
      u8"\xe4\xbb\xa4\0"                                                             // F9A8
      u8"\xe5\x9b\xb9\0"                                                             // F9A9
      u8"\xe5\xb6\xba\0"                                                             // F9AB
      u8"\xe6\x80\x9c\0"                                                             // F9AC
      u8"\xe7\x8e\xb2\0"                                                             // F9AD
      u8"\xe7\x91\xa9\0"                                                             // F9AE
      u8"\xe7\xbe\x9a\0"                                                             // F9AF
      u8"\xe8\x81\x86\0"                                                             // F9B0
      u8"\xe9\x88\xb4\0"                                                             // F9B1
      u8"\xe9\x9b\xb6\0"                                                             // F9B2
      u8"\xe9\x9d\x88\0"                                                             // F9B3
      u8"\xe9\xa0\x98\0"                                                             // F9B4
      u8"\xe4\xbe\x8b\0"                                                             // F9B5
      u8"\xe7\xa6\xae\0"                                                             // F9B6
      u8"\xe9\x86\xb4\0"                                                             // F9B7
      u8"\xe9\x9a\xb8\0"                                                             // F9B8
      u8"\xe6\x83\xa1\0"                                                             // F9B9
      u8"\xe4\xba\x86\0"                                                             // F9BA
      u8"\xe5\x83\x9a\0"                                                             // F9BB
      u8"\xe5\xaf\xae\0"                                                             // F9BC
      u8"\xe5\xb0\xbf\0"                                                             // F9BD
      u8"\xe6\x96\x99\0"                                                             // F9BE, 1F21B
      u8"\xe7\x87\x8e\0"                                                             // F9C0
      u8"\xe7\x99\x82\0"                                                             // F9C1
      u8"\xe8\x93\xbc\0"                                                             // F9C2
      u8"\xe9\x81\xbc\0"                                                             // F9C3
      u8"\xe6\x9a\x88\0"                                                             // F9C5
      u8"\xe9\x98\xae\0"                                                             // F9C6
      u8"\xe5\x8a\x89\0"                                                             // F9C7
      u8"\xe6\x9d\xbb\0"                                                             // F9C8
      u8"\xe6\x9f\xb3\0"                                                             // F9C9
      u8"\xe6\xb5\x81\0"                                                             // F9CA, FA97, 2F902
      u8"\xe6\xba\x9c\0"                                                             // F9CB
      u8"\xe7\x90\x89\0"                                                             // F9CC
      u8"\xe7\x95\x99\0"                                                             // F9CD
      u8"\xe7\xa1\xab\0"                                                             // F9CE
      u8"\xe7\xb4\x90\0"                                                             // F9CF
      u8"\xe9\xa1\x9e\0"                                                             // F9D0
      u8"\xe6\x88\xae\0"                                                             // F9D2
      u8"\xe9\x99\xb8\0"                                                             // F9D3
      u8"\xe5\x80\xab\0"                                                             // F9D4
      u8"\xe5\xb4\x99\0"                                                             // F9D5
      u8"\xe6\xb7\xaa\0"                                                             // F9D6
      u8"\xe8\xbc\xaa\0"                                                             // F9D7
      u8"\xe5\xbe\x8b\0"                                                             // F9D8
      u8"\xe6\x85\x84\0"                                                             // F9D9
      u8"\xe6\xa0\x97\0"                                                             // F9DA
      u8"\xe9\x9a\x86\0"                                                             // F9DC
      u8"\xe5\x88\xa9\0"                                                             // F9DD
      u8"\xe5\x90\x8f\0"                                                             // F9DE
      u8"\xe5\xb1\xa5\0"                                                             // F9DF
      u8"\xe6\x98\x93\0"                                                             // F9E0
      u8"\xe6\x9d\x8e\0"                                                             // F9E1
      u8"\xe6\xa2\xa8\0"                                                             // F9E2
      u8"\xe6\xb3\xa5\0"                                                             // F9E3
      u8"\xe7\x90\x86\0"                                                             // F9E4
      u8"\xe7\x97\xa2\0"                                                             // F9E5
      u8"\xe7\xbd\xb9\0"                                                             // F9E6
      u8"\xe8\xa3\x8f\0"                                                             // F9E7
      u8"\xe8\xa3\xa1\0"                                                             // F9E8
      u8"\xe9\x9b\xa2\0"                                                             // F9EA
      u8"\xe5\x8c\xbf\0"                                                             // F9EB
      u8"\xe6\xba\xba\0"                                                             // F9EC
      u8"\xe5\x90\x9d\0"                                                             // F9ED
      u8"\xe7\x87\x90\0"                                                             // F9EE
      u8"\xe7\x92\x98\0"                                                             // F9EF
      u8"\xe8\x97\xba\0"                                                             // F9F0
      u8"\xe9\x9a\xa3\0"                                                             // F9F1
      u8"\xe9\xb1\x97\0"                                                             // F9F2
      u8"\xe9\xba\x9f\0"                                                             // F9F3
      u8"\xe6\x9e\x97\0"                                                             // F9F4
      u8"\xe6\xb7\x8b\0"                                                             // F9F5
      u8"\xe8\x87\xa8\0"                                                             // F9F6
      u8"\xe7\xac\xa0\0"                                                             // F9F8
      u8"\xe7\xb2\x92\0"                                                             // F9F9
      u8"\xe7\x8b\x80\0"                                                             // F9FA
      u8"\xe7\x82\x99\0"                                                             // F9FB
      u8"\xe8\xad\x98\0"                                                             // F9FC
      u8"\xe4\xbb\x80\0"                                                             // F9FD
      u8"\xe8\x8c\xb6\0"                                                             // F9FE
      u8"\xe5\x88\xba\0"                                                             // F9FF
      u8"\xe5\x88\x87\0"                                                             // FA00, 2F850
      u8"\xe5\xba\xa6\0"                                                             // FA01
      u8"\xe6\x8b\x93\0"                                                             // FA02
      u8"\xe7\xb3\x96\0"                                                             // FA03
      u8"\xe5\xae\x85\0"                                                             // FA04
      u8"\xe6\xb4\x9e\0"                                                             // FA05
      u8"\xe6\x9a\xb4\0"                                                             // FA06
      u8"\xe8\xbc\xbb\0"                                                             // FA07
      u8"\xe9\x99\x8d\0"                                                             // FA09
      u8"\xe5\xbb\x93\0"                                                             // FA0B
      u8"\xe5\x85\x80\0"                                                             // FA0C
      u8"\xe5\x97\x80\0"                                                             // FA0D
      u8"\xe5\xa1\x9a\0"                                                             // FA10, FA7C
      u8"\xe6\x99\xb4\0"                                                             // FA12, FA91
      u8"\xe5\x87\x9e\0"                                                             // FA15
      u8"\xe7\x8c\xaa\0"                                                             // FA16, FAA0
      u8"\xe7\x9b\x8a\0"                                                             // FA17, FAA6
      u8"\xe7\xa4\xbc\0"                                                             // FA18
      u8"\xe7\xa5\x9e\0"                                                             // FA19
      u8"\xe7\xa5\xa5\0"                                                             // FA1A
      u8"\xe7\xa6\x8f\0"                                                             // FA1B, 2F956
      u8"\xe9\x9d\x96\0"                                                             // FA1C, FAC8
      u8"\xe7\xb2\xbe\0"                                                             // FA1D
      u8"\xe8\x98\x92\0"                                                             // FA20
      u8"\xe8\xab\xb8\0"                                                             // FA22, FABA
      u8"\xe9\x80\xb8\0"                                                             // FA25, FA67
      u8"\xe9\x83\xbd\0"                                                             // FA26
      u8"\xe9\xa3\xaf\0"                                                             // FA2A
      u8"\xe9\xa3\xbc\0"                                                             // FA2B
      u8"\xe9\xa4\xa8\0"                                                             // FA2C
      u8"\xe9\xb6\xb4\0"                                                             // FA2D
      u8"\xe9\x83\x9e\0"                                                             // FA2E
      u8"\xe9\x9a\xb7\0"                                                             // FA2F
      u8"\xe4\xbe\xae\0"                                                             // FA30, 2F805
      u8"\xe5\x83\xa7\0"                                                             // FA31, 2F80A
      u8"\xe5\x85\x8d\0"                                                             // FA32, 2F80E
      u8"\xe5\x8b\x89\0"                                                             // FA33, 2F826
      u8"\xe5\x8b\xa4\0"                                                             // FA34, 2F827
      u8"\xe5\x8d\x91\0"                                                             // FA35, 2F82D
      u8"\xe5\x96\x9d\0"                                                             // FA36, FA78
      u8"\xe5\x98\x86\0"                                                             // FA37, 2F84C
      u8"\xe5\x99\xa8\0"                                                             // FA38
      u8"\xe5\xa1\x80\0"                                                             // FA39
      u8"\xe5\xa2\xa8\0"                                                             // FA3A
      u8"\xe5\xb1\xa4\0"                                                             // FA3B
      u8"\xe6\x82\x94\0"                                                             // FA3D, 2F8A3
      u8"\xe6\x85\xa8\0"                                                             // FA3E
      u8"\xe6\x86\x8e\0"                                                             // FA3F, FA89, 2F8AB
      u8"\xe6\x87\xb2\0"                                                             // FA40, FA8B, 2F8B0
      u8"\xe6\x95\x8f\0"                                                             // FA41, 2F8C8
      u8"\xe6\x97\xa2\0"                                                             // FA42
      u8"\xe6\x9a\x91\0"                                                             // FA43, 2F8CF
      u8"\xe6\xa2\x85\0"                                                             // FA44, 2F8E2
      u8"\xe6\xb5\xb7\0"                                                             // FA45, 2F901
      u8"\xe6\xb8\x9a\0"                                                             // FA46
      u8"\xe6\xbc\xa2\0"                                                             // FA47, FA9A
      u8"\xe7\x85\xae\0"                                                             // FA48, FA9C
      u8"\xe7\x88\xab\0"                                                             // FA49
      u8"\xe7\x90\xa2\0"                                                             // FA4A
      u8"\xe7\xa2\x91\0"                                                             // FA4B
      u8"\xe7\xa5\x89\0"                                                             // FA4D
      u8"\xe7\xa5\x88\0"                                                             // FA4E
      u8"\xe7\xa5\x90\0"                                                             // FA4F
      u8"\xe7\xa5\x96\0"                                                             // FA50, 2F953
      u8"\xe7\xa6\x8d\0"                                                             // FA52
      u8"\xe7\xa6\x8e\0"                                                             // FA53
      u8"\xe7\xa9\x80\0"                                                             // FA54, 2F959
      u8"\xe7\xaa\x81\0"                                                             // FA55
      u8"\xe7\xaf\x80\0"                                                             // FA56, FAAD
      u8"\xe7\xb8\x89\0"                                                             // FA58
      u8"\xe7\xb9\x81\0"                                                             // FA59
      u8"\xe7\xbd\xb2\0"                                                             // FA5A
      u8"\xe8\x80\x85\0"                                                             // FA5B, FAB2, 2F97A
      u8"\xe8\x87\xad\0"                                                             // FA5C
      u8"\xe8\x89\xb9\0"                                                             // FA5D, FA5E
      u8"\xe8\x91\x97\0"                                                             // FA5F, 2F99F
      u8"\xe8\xa4\x90\0"                                                             // FA60
      u8"\xe8\xa6\x96\0"                                                             // FA61, FAB8
      u8"\xe8\xac\x81\0"                                                             // FA62, FABC
      u8"\xe8\xac\xb9\0"                                                             // FA63, FABF
      u8"\xe8\xb3\x93\0"                                                             // FA64
      u8"\xe8\xb4\x88\0"                                                             // FA65, FAC1
      u8"\xe8\xbe\xb6\0"                                                             // FA66
      u8"\xe9\x9b\xa3\0"                                                             // FA68, FAC7
      u8"\xe9\x9f\xbf\0"                                                             // FA69, FACA
      u8"\xe9\xa0\xbb\0"                                                             // FA6A, FACC
      u8"\xe6\x81\xb5\0"                                                             // FA6B
      u8"\xf0\xa4\x8b\xae\0"                                                         // FA6C
      u8"\xe8\x88\x98\0"                                                             // FA6D
      u8"\xe4\xb8\xa6\0"                                                             // FA70
      u8"\xe5\x86\xb5\0"                                                             // FA71, 2F81B
      u8"\xe5\x85\xa8\0"                                                             // FA72
      u8"\xe4\xbe\x80\0"                                                             // FA73
      u8"\xe5\x85\x85\0"                                                             // FA74
      u8"\xe5\x86\x80\0"                                                             // FA75
      u8"\xe5\x8b\x87\0"                                                             // FA76, 2F825
      u8"\xe5\x8b\xba\0"                                                             // FA77, 2F828
      u8"\xe5\x95\x95\0"                                                             // FA79
      u8"\xe5\x96\x99\0"                                                             // FA7A, 2F847
      u8"\xe5\x97\xa2\0"                                                             // FA7B
      u8"\xe5\xa2\xb3\0"                                                             // FA7D
      u8"\xe5\xa5\x84\0"                                                             // FA7E
      u8"\xe5\xa5\x94\0"                                                             // FA7F
      u8"\xe5\xa9\xa2\0"                                                             // FA80
      u8"\xe5\xac\xa8\0"                                                             // FA81
      u8"\xe5\xbb\x92\0"                                                             // FA82
      u8"\xe5\xbb\x99\0"                                                             // FA83
      u8"\xe5\xbd\xa9\0"                                                             // FA84
      u8"\xe5\xbe\xad\0"                                                             // FA85
      u8"\xe6\x83\x98\0"                                                             // FA86
      u8"\xe6\x85\x8e\0"                                                             // FA87, 2F8A8
      u8"\xe6\x84\x88\0"                                                             // FA88
      u8"\xe6\x85\xa0\0"                                                             // FA8A
      u8"\xe6\x88\xb4\0"                                                             // FA8C
      u8"\xe6\x8f\x84\0"                                                             // FA8D
      u8"\xe6\x90\x9c\0"                                                             // FA8E
      u8"\xe6\x91\x92\0"                                                             // FA8F
      u8"\xe6\x95\x96\0"                                                             // FA90
      u8"\xe6\x9c\x9b\0"                                                             // FA93, 2F8D9
      u8"\xe6\x9d\x96\0"                                                             // FA94
      u8"\xe6\xbb\x9b\0"                                                             // FA98
      u8"\xe6\xbb\x8b\0"                                                             // FA99, 2F90B
      u8"\xe7\x80\x9e\0"                                                             // FA9B, 2F914
      u8"\xe7\x9e\xa7\0"                                                             // FA9D
      u8"\xe7\x88\xb5\0"                                                             // FA9E, 2F921
      u8"\xe7\x8a\xaf\0"                                                             // FA9F
      u8"\xe7\x91\xb1\0"                                                             // FAA1, 2F930
      u8"\xe7\x94\x86\0"                                                             // FAA2
      u8"\xe7\x94\xbb\0"                                                             // FAA3
      u8"\xe7\x98\x9d\0"                                                             // FAA4
      u8"\xe7\x98\x9f\0"                                                             // FAA5
      u8"\xe7\x9b\x9b\0"                                                             // FAA7
      u8"\xe7\x9b\xb4\0"                                                             // FAA8, 2F940
      u8"\xe7\x9d\x8a\0"                                                             // FAA9, 2F948
      u8"\xe7\x9d\x80\0"                                                             // FAAA
      u8"\xe7\xa3\x8c\0"                                                             // FAAB, 2F950
      u8"\xe7\xaa\xb1\0"                                                             // FAAC
      u8"\xe7\xb1\xbb\0"                                                             // FAAE
      u8"\xe7\xb5\x9b\0"                                                             // FAAF
      u8"\xe7\xbc\xbe\0"                                                             // FAB1
      u8"\xe8\x8d\x92\0"                                                             // FAB3
      u8"\xe8\x8f\xaf\0"                                                             // FAB4
      u8"\xe8\x9d\xb9\0"                                                             // FAB5, 2F9BB
      u8"\xe8\xa5\x81\0"                                                             // FAB6
      u8"\xe8\xa6\x86\0"                                                             // FAB7
      u8"\xe8\xaa\xbf\0"                                                             // FAB9
      u8"\xe8\xab\x8b\0"                                                             // FABB
      u8"\xe8\xab\xad\0"                                                             // FABE, 2F9D0
      u8"\xe8\xae\x8a\0"                                                             // FAC0, 2F9D1
      u8"\xe8\xbc\xb8\0"                                                             // FAC2, 2F9DF
      u8"\xe9\x81\xb2\0"                                                             // FAC3
      u8"\xe9\x86\x99\0"                                                             // FAC4
      u8"\xe9\x89\xb6\0"                                                             // FAC5
      u8"\xe9\x99\xbc\0"                                                             // FAC6
      u8"\xe9\x9f\x9b\0"                                                             // FAC9
      u8"\xe9\xa0\x8b\0"                                                             // FACB, 2F9FE, 2F9FF
      u8"\xe9\xac\x92\0"                                                             // FACD, 2FA0A
      u8"\xf0\xa2\xa1\x8a\0"                                                         // FACF
      u8"\xf0\xa2\xa1\x84\0"                                                         // FAD0
      u8"\xf0\xa3\x8f\x95\0"                                                         // FAD1
      u8"\xe3\xae\x9d\0"                                                             // FAD2, 2F8E7
      u8"\xe4\x80\x98\0"                                                             // FAD3
      u8"\xe4\x80\xb9\0"                                                             // FAD4, 2F949
      u8"\xf0\xa5\x89\x89\0"                                                         // FAD5
      u8"\xf0\xa5\xb3\x90\0"                                                         // FAD6
      u8"\xf0\xa7\xbb\x93\0"                                                         // FAD7
      u8"\xe9\xbd\x83\0"                                                             // FAD8
      u8"\xe9\xbe\x8e\0"                                                             // FAD9
      u8"\x66\x66\0"                                                                 // FB00
      u8"\x66\x69\0"                                                                 // FB01
      u8"\x66\x6c\0"                                                                 // FB02
      u8"\x66\x66\x69\0"                                                             // FB03
      u8"\x66\x66\x6c\0"                                                             // FB04
      u8"\x73\x74\0"                                                                 // FB05, FB06
      u8"\xd5\xb4\xd5\xb6\0"                                                         // FB13
      u8"\xd5\xb4\xd5\xa5\0"                                                         // FB14
      u8"\xd5\xb4\xd5\xab\0"                                                         // FB15
      u8"\xd5\xbe\xd5\xb6\0"                                                         // FB16
      u8"\xd5\xb4\xd5\xad\0"                                                         // FB17
      u8"\xd7\x99\xd6\xb4\0"                                                         // FB1D
      u8"\xd7\xb2\xd6\xb7\0"                                                         // FB1F
      u8"\xd7\xa2\0"                                                                 // FB20
      u8"\xd7\x94\0"                                                                 // FB23
      u8"\xd7\x9b\0"                                                                 // FB24
      u8"\xd7\x9c\0"                                                                 // FB25
      u8"\xd7\x9d\0"                                                                 // FB26
      u8"\xd7\xa8\0"                                                                 // FB27
      u8"\xd7\xaa\0"                                                                 // FB28
      u8"\xd7\xa9\xd7\x81\0"                                                         // FB2A
      u8"\xd7\xa9\xd7\x82\0"                                                         // FB2B
      u8"\xd7\xa9\xd6\xbc\xd7\x81\0"                                                 // FB2C
      u8"\xd7\xa9\xd6\xbc\xd7\x82\0"                                                 // FB2D
      u8"\xd7\x90\xd6\xb7\0"                                                         // FB2E
      u8"\xd7\x90\xd6\xb8\0"                                                         // FB2F
      u8"\xd7\x90\xd6\xbc\0"                                                         // FB30
      u8"\xd7\x91\xd6\xbc\0"                                                         // FB31
      u8"\xd7\x92\xd6\xbc\0"                                                         // FB32
      u8"\xd7\x93\xd6\xbc\0"                                                         // FB33
      u8"\xd7\x94\xd6\xbc\0"                                                         // FB34
      u8"\xd7\x95\xd6\xbc\0"                                                         // FB35
      u8"\xd7\x96\xd6\xbc\0"                                                         // FB36
      u8"\xd7\x98\xd6\xbc\0"                                                         // FB38
      u8"\xd7\x99\xd6\xbc\0"                                                         // FB39
      u8"\xd7\x9a\xd6\xbc\0"                                                         // FB3A
      u8"\xd7\x9b\xd6\xbc\0"                                                         // FB3B
      u8"\xd7\x9c\xd6\xbc\0"                                                         // FB3C
      u8"\xd7\x9e\xd6\xbc\0"                                                         // FB3E
      u8"\xd7\xa0\xd6\xbc\0"                                                         // FB40
      u8"\xd7\xa1\xd6\xbc\0"                                                         // FB41
      u8"\xd7\xa3\xd6\xbc\0"                                                         // FB43
      u8"\xd7\xa4\xd6\xbc\0"                                                         // FB44
      u8"\xd7\xa6\xd6\xbc\0"                                                         // FB46
      u8"\xd7\xa7\xd6\xbc\0"                                                         // FB47
      u8"\xd7\xa8\xd6\xbc\0"                                                         // FB48
      u8"\xd7\xa9\xd6\xbc\0"                                                         // FB49
      u8"\xd7\xaa\xd6\xbc\0"                                                         // FB4A
      u8"\xd7\x95\xd6\xb9\0"                                                         // FB4B
      u8"\xd7\x91\xd6\xbf\0"                                                         // FB4C
      u8"\xd7\x9b\xd6\xbf\0"                                                         // FB4D
      u8"\xd7\xa4\xd6\xbf\0"                                                         // FB4E
      u8"\xd7\x90\xd7\x9c\0"                                                         // FB4F
      u8"\xd9\xb1\0"                                                                 // FB50, FB51
      u8"\xd9\xbb\0"                                                                 // FB52, FB53, FB54, FB55
      u8"\xd9\xbe\0"                                                                 // FB56, FB57, FB58, FB59
      u8"\xda\x80\0"                                                                 // FB5A, FB5B, FB5C, FB5D
      u8"\xd9\xba\0"                                                                 // FB5E, FB5F, FB60, FB61
      u8"\xd9\xbf\0"                                                                 // FB62, FB63, FB64, FB65
      u8"\xd9\xb9\0"                                                                 // FB66, FB67, FB68, FB69
      u8"\xda\xa4\0"                                                                 // FB6A, FB6B, FB6C, FB6D
      u8"\xda\xa6\0"                                                                 // FB6E, FB6F, FB70, FB71
      u8"\xda\x84\0"                                                                 // FB72, FB73, FB74, FB75
      u8"\xda\x83\0"                                                                 // FB76, FB77, FB78, FB79
      u8"\xda\x86\0"                                                                 // FB7A, FB7B, FB7C, FB7D
      u8"\xda\x87\0"                                                                 // FB7E, FB7F, FB80, FB81
      u8"\xda\x8d\0"                                                                 // FB82, FB83
      u8"\xda\x8c\0"                                                                 // FB84, FB85
      u8"\xda\x8e\0"                                                                 // FB86, FB87
      u8"\xda\x88\0"                                                                 // FB88, FB89
      u8"\xda\x98\0"                                                                 // FB8A, FB8B
      u8"\xda\x91\0"                                                                 // FB8C, FB8D
      u8"\xda\xa9\0"                                                                 // FB8E, FB8F, FB90, FB91
      u8"\xda\xaf\0"                                                                 // FB92, FB93, FB94, FB95
      u8"\xda\xb3\0"                                                                 // FB96, FB97, FB98, FB99
      u8"\xda\xb1\0"                                                                 // FB9A, FB9B, FB9C, FB9D
      u8"\xda\xba\0"                                                    // FB9E, FB9F, 1EE1D, 1EE5D
      u8"\xda\xbb\0"                                                    // FBA0, FBA1, FBA2, FBA3
      u8"\xdb\x80\0"                                                    // FBA4, FBA5
      u8"\xdb\x81\0"                                                    // FBA6, FBA7, FBA8, FBA9
      u8"\xda\xbe\0"                                                    // FBAA, FBAB, FBAC, FBAD
      u8"\xdb\x92\0"                                                    // FBAE, FBAF
      u8"\xdb\x93\0"                                                    // FBB0, FBB1
      u8"\xda\xad\0"                                                    // FBD3, FBD4, FBD5, FBD6
      u8"\xdb\x87\0"                                                    // FBD7, FBD8
      u8"\xdb\x86\0"                                                    // FBD9, FBDA
      u8"\xdb\x88\0"                                                    // FBDB, FBDC
      u8"\xdb\x8b\0"                                                    // FBDE, FBDF
      u8"\xdb\x85\0"                                                    // FBE0, FBE1
      u8"\xdb\x89\0"                                                    // FBE2, FBE3
      u8"\xdb\x90\0"                                                    // FBE4, FBE5, FBE6, FBE7
      u8"\xd9\x89\0"                                                    // FBE8, FBE9, FEEF, FEF0
      u8"\xd8\xa6\xd8\xa7\0"                                            // FBEA, FBEB
      u8"\xd8\xa6\xdb\x95\0"                                            // FBEC, FBED
      u8"\xd8\xa6\xd9\x88\0"                                            // FBEE, FBEF
      u8"\xd8\xa6\xdb\x87\0"                                            // FBF0, FBF1
      u8"\xd8\xa6\xdb\x86\0"                                            // FBF2, FBF3
      u8"\xd8\xa6\xdb\x88\0"                                            // FBF4, FBF5
      u8"\xd8\xa6\xdb\x90\0"                                            // FBF6, FBF7, FBF8
      u8"\xd8\xa6\xd9\x89\0"                                            // FBF9, FBFA, FBFB, FC03, FC68
      u8"\xdb\x8c\0"                                                    // FBFC, FBFD, FBFE, FBFF
      u8"\xd8\xa6\xd8\xac\0"                                            // FC00, FC97
      u8"\xd8\xa6\xd8\xad\0"                                            // FC01, FC98
      u8"\xd8\xa6\xd9\x85\0"                                            // FC02, FC66, FC9A, FCDF
      u8"\xd8\xa6\xd9\x8a\0"                                            // FC04, FC69
      u8"\xd8\xa8\xd8\xac\0"                                            // FC05, FC9C
      u8"\xd8\xa8\xd8\xad\0"                                            // FC06, FC9D
      u8"\xd8\xa8\xd8\xae\0"                                            // FC07, FC9E
      u8"\xd8\xa8\xd9\x85\0"                                            // FC08, FC6C, FC9F, FCE1
      u8"\xd8\xa8\xd9\x89\0"                                            // FC09, FC6E
      u8"\xd8\xa8\xd9\x8a\0"                                            // FC0A, FC6F
      u8"\xd8\xaa\xd8\xac\0"                                            // FC0B, FCA1
      u8"\xd8\xaa\xd8\xad\0"                                            // FC0C, FCA2
      u8"\xd8\xaa\xd8\xae\0"                                            // FC0D, FCA3
      u8"\xd8\xaa\xd9\x85\0"                                            // FC0E, FC72, FCA4, FCE3
      u8"\xd8\xaa\xd9\x89\0"                                            // FC0F, FC74
      u8"\xd8\xaa\xd9\x8a\0"                                            // FC10, FC75
      u8"\xd8\xab\xd8\xac\0"                                            // FC11
      u8"\xd8\xab\xd9\x85\0"                                            // FC12, FC78, FCA6, FCE5
      u8"\xd8\xab\xd9\x89\0"                                            // FC13, FC7A
      u8"\xd8\xab\xd9\x8a\0"                                            // FC14, FC7B
      u8"\xd8\xac\xd8\xad\0"                                            // FC15, FCA7
      u8"\xd8\xac\xd9\x85\0"                                            // FC16, FCA8
      u8"\xd8\xad\xd8\xac\0"                                            // FC17, FCA9
      u8"\xd8\xad\xd9\x85\0"                                            // FC18, FCAA
      u8"\xd8\xae\xd8\xac\0"                                            // FC19, FCAB
      u8"\xd8\xae\xd8\xad\0"                                            // FC1A
      u8"\xd8\xae\xd9\x85\0"                                            // FC1B, FCAC
      u8"\xd8\xb3\xd8\xac\0"                                            // FC1C, FCAD, FD34
      u8"\xd8\xb3\xd8\xad\0"                                            // FC1D, FCAE, FD35
      u8"\xd8\xb3\xd8\xae\0"                                            // FC1E, FCAF, FD36
      u8"\xd8\xb3\xd9\x85\0"                                            // FC1F, FCB0, FCE7
      u8"\xd8\xb5\xd8\xad\0"                                            // FC20, FCB1
      u8"\xd8\xb5\xd9\x85\0"                                            // FC21, FCB3
      u8"\xd8\xb6\xd8\xac\0"                                            // FC22, FCB4
      u8"\xd8\xb6\xd8\xad\0"                                            // FC23, FCB5
      u8"\xd8\xb6\xd8\xae\0"                                            // FC24, FCB6
      u8"\xd8\xb6\xd9\x85\0"                                            // FC25, FCB7
      u8"\xd8\xb7\xd8\xad\0"                                            // FC26, FCB8
      u8"\xd8\xb7\xd9\x85\0"                                            // FC27, FD33, FD3A
      u8"\xd8\xb8\xd9\x85\0"                                            // FC28, FCB9, FD3B
      u8"\xd8\xb9\xd8\xac\0"                                            // FC29, FCBA
      u8"\xd8\xb9\xd9\x85\0"                                            // FC2A, FCBB
      u8"\xd8\xba\xd8\xac\0"                                            // FC2B, FCBC
      u8"\xd8\xba\xd9\x85\0"                                            // FC2C, FCBD
      u8"\xd9\x81\xd8\xac\0"                                            // FC2D, FCBE
      u8"\xd9\x81\xd8\xad\0"                                            // FC2E, FCBF
      u8"\xd9\x81\xd8\xae\0"                                            // FC2F, FCC0
      u8"\xd9\x81\xd9\x85\0"                                            // FC30, FCC1
      u8"\xd9\x81\xd9\x89\0"                                            // FC31, FC7C
      u8"\xd9\x81\xd9\x8a\0"                                            // FC32, FC7D
      u8"\xd9\x82\xd8\xad\0"                                            // FC33, FCC2
      u8"\xd9\x82\xd9\x85\0"                                            // FC34, FCC3
      u8"\xd9\x82\xd9\x89\0"                                            // FC35, FC7E
      u8"\xd9\x82\xd9\x8a\0"                                            // FC36, FC7F
      u8"\xd9\x83\xd8\xa7\0"                                            // FC37, FC80
      u8"\xd9\x83\xd8\xac\0"                                            // FC38, FCC4
      u8"\xd9\x83\xd8\xad\0"                                            // FC39, FCC5
      u8"\xd9\x83\xd8\xae\0"                                            // FC3A, FCC6
      u8"\xd9\x83\xd9\x84\0"                                            // FC3B, FC81, FCC7, FCEB
      u8"\xd9\x83\xd9\x85\0"                                            // FC3C, FC82, FCC8, FCEC
      u8"\xd9\x83\xd9\x89\0"                                            // FC3D, FC83
      u8"\xd9\x83\xd9\x8a\0"                                            // FC3E, FC84
      u8"\xd9\x84\xd8\xac\0"                                            // FC3F, FCC9
      u8"\xd9\x84\xd8\xad\0"                                            // FC40, FCCA
      u8"\xd9\x84\xd8\xae\0"                                            // FC41, FCCB
      u8"\xd9\x84\xd9\x85\0"                                            // FC42, FC85, FCCC, FCED
      u8"\xd9\x84\xd9\x89\0"                                            // FC43, FC86
      u8"\xd9\x84\xd9\x8a\0"                                            // FC44, FC87
      u8"\xd9\x85\xd8\xac\0"                                            // FC45, FCCE
      u8"\xd9\x85\xd8\xad\0"                                            // FC46, FCCF
      u8"\xd9\x85\xd8\xae\0"                                            // FC47, FCD0
      u8"\xd9\x85\xd9\x85\0"                                            // FC48, FC89, FCD1
      u8"\xd9\x85\xd9\x89\0"                                            // FC49
      u8"\xd9\x85\xd9\x8a\0"                                            // FC4A
      u8"\xd9\x86\xd8\xac\0"                                            // FC4B, FCD2
      u8"\xd9\x86\xd8\xad\0"                                            // FC4C, FCD3
      u8"\xd9\x86\xd8\xae\0"                                            // FC4D, FCD4
      u8"\xd9\x86\xd9\x85\0"                                            // FC4E, FC8C, FCD5, FCEE
      u8"\xd9\x86\xd9\x89\0"                                            // FC4F, FC8E
      u8"\xd9\x86\xd9\x8a\0"                                            // FC50, FC8F
      u8"\xd9\x87\xd8\xac\0"                                            // FC51, FCD7
      u8"\xd9\x87\xd9\x85\0"                                            // FC52, FCD8
      u8"\xd9\x87\xd9\x89\0"                                            // FC53
      u8"\xd9\x87\xd9\x8a\0"                                            // FC54
      u8"\xd9\x8a\xd8\xac\0"                                            // FC55, FCDA
      u8"\xd9\x8a\xd8\xad\0"                                            // FC56, FCDB
      u8"\xd9\x8a\xd8\xae\0"                                            // FC57, FCDC
      u8"\xd9\x8a\xd9\x85\0"                                            // FC58, FC93, FCDD, FCF0
      u8"\xd9\x8a\xd9\x89\0"                                            // FC59, FC95
      u8"\xd9\x8a\xd9\x8a\0"                                            // FC5A, FC96
      u8"\xd8\xb0\xd9\xb0\0"                                            // FC5B
      u8"\xd8\xb1\xd9\xb0\0"                                            // FC5C
      u8"\xd9\x89\xd9\xb0\0"                                            // FC5D, FC90
      u8"\40\xd9\x8c\xd9\x91\0"                                         // FC5E
      u8"\40\xd9\x8d\xd9\x91\0"                                         // FC5F
      u8"\40\xd9\x8e\xd9\x91\0"                                         // FC60
      u8"\40\xd9\x8f\xd9\x91\0"                                         // FC61
      u8"\40\xd9\x90\xd9\x91\0"                                         // FC62
      u8"\40\xd9\x91\xd9\xb0\0"                                         // FC63
      u8"\xd8\xa6\xd8\xb1\0"                                            // FC64
      u8"\xd8\xa6\xd8\xb2\0"                                            // FC65
      u8"\xd8\xa6\xd9\x86\0"                                            // FC67
      u8"\xd8\xa8\xd8\xb1\0"                                            // FC6A
      u8"\xd8\xa8\xd8\xb2\0"                                            // FC6B
      u8"\xd8\xa8\xd9\x86\0"                                            // FC6D
      u8"\xd8\xaa\xd8\xb1\0"                                            // FC70
      u8"\xd8\xaa\xd8\xb2\0"                                            // FC71
      u8"\xd8\xaa\xd9\x86\0"                                            // FC73
      u8"\xd8\xab\xd8\xb1\0"                                            // FC76
      u8"\xd8\xab\xd8\xb2\0"                                            // FC77
      u8"\xd8\xab\xd9\x86\0"                                            // FC79
      u8"\xd9\x85\xd8\xa7\0"                                            // FC88
      u8"\xd9\x86\xd8\xb1\0"                                            // FC8A
      u8"\xd9\x86\xd8\xb2\0"                                            // FC8B
      u8"\xd9\x86\xd9\x86\0"                                            // FC8D
      u8"\xd9\x8a\xd8\xb1\0"                                            // FC91
      u8"\xd9\x8a\xd8\xb2\0"                                            // FC92
      u8"\xd9\x8a\xd9\x86\0"                                            // FC94
      u8"\xd8\xa6\xd8\xae\0"                                            // FC99
      u8"\xd8\xa6\xd9\x87\0"                                            // FC9B, FCE0
      u8"\xd8\xa8\xd9\x87\0"                                            // FCA0, FCE2
      u8"\xd8\xaa\xd9\x87\0"                                            // FCA5, FCE4
      u8"\xd8\xb5\xd8\xae\0"                                            // FCB2
      u8"\xd9\x84\xd9\x87\0"                                            // FCCD
      u8"\xd9\x86\xd9\x87\0"                                            // FCD6, FCEF
      u8"\xd9\x87\xd9\xb0\0"                                            // FCD9
      u8"\xd9\x8a\xd9\x87\0"                                            // FCDE, FCF1
      u8"\xd8\xab\xd9\x87\0"                                            // FCE6
      u8"\xd8\xb3\xd9\x87\0"                                            // FCE8, FD31
      u8"\xd8\xb4\xd9\x85\0"                                            // FCE9, FD0C, FD28, FD30
      u8"\xd8\xb4\xd9\x87\0"                                            // FCEA, FD32
      u8"\xd9\x80\xd9\x8e\xd9\x91\0"                                    // FCF2
      u8"\xd9\x80\xd9\x8f\xd9\x91\0"                                    // FCF3
      u8"\xd9\x80\xd9\x90\xd9\x91\0"                                    // FCF4
      u8"\xd8\xb7\xd9\x89\0"                                            // FCF5, FD11
      u8"\xd8\xb7\xd9\x8a\0"                                            // FCF6, FD12
      u8"\xd8\xb9\xd9\x89\0"                                            // FCF7, FD13
      u8"\xd8\xb9\xd9\x8a\0"                                            // FCF8, FD14
      u8"\xd8\xba\xd9\x89\0"                                            // FCF9, FD15
      u8"\xd8\xba\xd9\x8a\0"                                            // FCFA, FD16
      u8"\xd8\xb3\xd9\x89\0"                                            // FCFB, FD17
      u8"\xd8\xb3\xd9\x8a\0"                                            // FCFC, FD18
      u8"\xd8\xb4\xd9\x89\0"                                            // FCFD, FD19
      u8"\xd8\xb4\xd9\x8a\0"                                            // FCFE, FD1A
      u8"\xd8\xad\xd9\x89\0"                                            // FCFF, FD1B
      u8"\xd8\xad\xd9\x8a\0"                                            // FD00, FD1C
      u8"\xd8\xac\xd9\x89\0"                                            // FD01, FD1D
      u8"\xd8\xac\xd9\x8a\0"                                            // FD02, FD1E
      u8"\xd8\xae\xd9\x89\0"                                            // FD03, FD1F
      u8"\xd8\xae\xd9\x8a\0"                                            // FD04, FD20
      u8"\xd8\xb5\xd9\x89\0"                                            // FD05, FD21
      u8"\xd8\xb5\xd9\x8a\0"                                            // FD06, FD22
      u8"\xd8\xb6\xd9\x89\0"                                            // FD07, FD23
      u8"\xd8\xb6\xd9\x8a\0"                                            // FD08, FD24
      u8"\xd8\xb4\xd8\xac\0"                                            // FD09, FD25, FD2D, FD37
      u8"\xd8\xb4\xd8\xad\0"                                            // FD0A, FD26, FD2E, FD38
      u8"\xd8\xb4\xd8\xae\0"                                            // FD0B, FD27, FD2F, FD39
      u8"\xd8\xb4\xd8\xb1\0"                                            // FD0D, FD29
      u8"\xd8\xb3\xd8\xb1\0"                                            // FD0E, FD2A
      u8"\xd8\xb5\xd8\xb1\0"                                            // FD0F, FD2B
      u8"\xd8\xb6\xd8\xb1\0"                                            // FD10, FD2C
      u8"\xd8\xa7\xd9\x8b\0"                                            // FD3C, FD3D
      u8"\xd8\xaa\xd8\xac\xd9\x85\0"                                    // FD50
      u8"\xd8\xaa\xd8\xad\xd8\xac\0"                                    // FD51, FD52
      u8"\xd8\xaa\xd8\xad\xd9\x85\0"                                    // FD53
      u8"\xd8\xaa\xd8\xae\xd9\x85\0"                                    // FD54
      u8"\xd8\xaa\xd9\x85\xd8\xac\0"                                    // FD55
      u8"\xd8\xaa\xd9\x85\xd8\xad\0"                                    // FD56
      u8"\xd8\xaa\xd9\x85\xd8\xae\0"                                    // FD57
      u8"\xd8\xac\xd9\x85\xd8\xad\0"                                    // FD58, FD59
      u8"\xd8\xad\xd9\x85\xd9\x8a\0"                                    // FD5A
      u8"\xd8\xad\xd9\x85\xd9\x89\0"                                    // FD5B
      u8"\xd8\xb3\xd8\xad\xd8\xac\0"                                    // FD5C
      u8"\xd8\xb3\xd8\xac\xd8\xad\0"                                    // FD5D
      u8"\xd8\xb3\xd8\xac\xd9\x89\0"                                    // FD5E
      u8"\xd8\xb3\xd9\x85\xd8\xad\0"                                    // FD5F, FD60
      u8"\xd8\xb3\xd9\x85\xd8\xac\0"                                    // FD61
      u8"\xd8\xb3\xd9\x85\xd9\x85\0"                                    // FD62, FD63
      u8"\xd8\xb5\xd8\xad\xd8\xad\0"                                    // FD64, FD65
      u8"\xd8\xb5\xd9\x85\xd9\x85\0"                                    // FD66, FDC5
      u8"\xd8\xb4\xd8\xad\xd9\x85\0"                                    // FD67, FD68
      u8"\xd8\xb4\xd8\xac\xd9\x8a\0"                                    // FD69
      u8"\xd8\xb4\xd9\x85\xd8\xae\0"                                    // FD6A, FD6B
      u8"\xd8\xb4\xd9\x85\xd9\x85\0"                                    // FD6C, FD6D
      u8"\xd8\xb6\xd8\xad\xd9\x89\0"                                    // FD6E
      u8"\xd8\xb6\xd8\xae\xd9\x85\0"                                    // FD6F, FD70
      u8"\xd8\xb7\xd9\x85\xd8\xad\0"                                    // FD71, FD72
      u8"\xd8\xb7\xd9\x85\xd9\x85\0"                                    // FD73
      u8"\xd8\xb7\xd9\x85\xd9\x8a\0"                                    // FD74
      u8"\xd8\xb9\xd8\xac\xd9\x85\0"                                    // FD75, FDC4
      u8"\xd8\xb9\xd9\x85\xd9\x85\0"                                    // FD76, FD77
      u8"\xd8\xb9\xd9\x85\xd9\x89\0"                                    // FD78
      u8"\xd8\xba\xd9\x85\xd9\x85\0"                                    // FD79
      u8"\xd8\xba\xd9\x85\xd9\x8a\0"                                    // FD7A
      u8"\xd8\xba\xd9\x85\xd9\x89\0"                                    // FD7B
      u8"\xd9\x81\xd8\xae\xd9\x85\0"                                    // FD7C, FD7D
      u8"\xd9\x82\xd9\x85\xd8\xad\0"                                    // FD7E, FDB4
      u8"\xd9\x82\xd9\x85\xd9\x85\0"                                    // FD7F
      u8"\xd9\x84\xd8\xad\xd9\x85\0"                                    // FD80, FDB5
      u8"\xd9\x84\xd8\xad\xd9\x8a\0"                                    // FD81
      u8"\xd9\x84\xd8\xad\xd9\x89\0"                                    // FD82
      u8"\xd9\x84\xd8\xac\xd8\xac\0"                                    // FD83, FD84
      u8"\xd9\x84\xd8\xae\xd9\x85\0"                                    // FD85, FD86
      u8"\xd9\x84\xd9\x85\xd8\xad\0"                                    // FD87, FD88
      u8"\xd9\x85\xd8\xad\xd8\xac\0"                                    // FD89
      u8"\xd9\x85\xd8\xad\xd9\x85\0"                                    // FD8A
      u8"\xd9\x85\xd8\xad\xd9\x8a\0"                                    // FD8B
      u8"\xd9\x85\xd8\xac\xd8\xad\0"                                    // FD8C
      u8"\xd9\x85\xd8\xac\xd9\x85\0"                                    // FD8D
      u8"\xd9\x85\xd8\xae\xd8\xac\0"                                    // FD8E
      u8"\xd9\x85\xd8\xae\xd9\x85\0"                                    // FD8F
      u8"\xd9\x85\xd8\xac\xd8\xae\0"                                    // FD92
      u8"\xd9\x87\xd9\x85\xd8\xac\0"                                    // FD93
      u8"\xd9\x87\xd9\x85\xd9\x85\0"                                    // FD94
      u8"\xd9\x86\xd8\xad\xd9\x85\0"                                    // FD95
      u8"\xd9\x86\xd8\xad\xd9\x89\0"                                    // FD96
      u8"\xd9\x86\xd8\xac\xd9\x85\0"                                    // FD97, FD98
      u8"\xd9\x86\xd8\xac\xd9\x89\0"                                    // FD99
      u8"\xd9\x86\xd9\x85\xd9\x8a\0"                                    // FD9A
      u8"\xd9\x86\xd9\x85\xd9\x89\0"                                    // FD9B
      u8"\xd9\x8a\xd9\x85\xd9\x85\0"                                    // FD9C, FD9D
      u8"\xd8\xa8\xd8\xae\xd9\x8a\0"                                    // FD9E
      u8"\xd8\xaa\xd8\xac\xd9\x8a\0"                                    // FD9F
      u8"\xd8\xaa\xd8\xac\xd9\x89\0"                                    // FDA0
      u8"\xd8\xaa\xd8\xae\xd9\x8a\0"                                    // FDA1
      u8"\xd8\xaa\xd8\xae\xd9\x89\0"                                    // FDA2
      u8"\xd8\xaa\xd9\x85\xd9\x8a\0"                                    // FDA3
      u8"\xd8\xaa\xd9\x85\xd9\x89\0"                                    // FDA4
      u8"\xd8\xac\xd9\x85\xd9\x8a\0"                                    // FDA5
      u8"\xd8\xac\xd8\xad\xd9\x89\0"                                    // FDA6
      u8"\xd8\xac\xd9\x85\xd9\x89\0"                                    // FDA7
      u8"\xd8\xb3\xd8\xae\xd9\x89\0"                                    // FDA8
      u8"\xd8\xb5\xd8\xad\xd9\x8a\0"                                    // FDA9
      u8"\xd8\xb4\xd8\xad\xd9\x8a\0"                                    // FDAA
      u8"\xd8\xb6\xd8\xad\xd9\x8a\0"                                    // FDAB
      u8"\xd9\x84\xd8\xac\xd9\x8a\0"                                    // FDAC
      u8"\xd9\x84\xd9\x85\xd9\x8a\0"                                    // FDAD
      u8"\xd9\x8a\xd8\xad\xd9\x8a\0"                                    // FDAE
      u8"\xd9\x8a\xd8\xac\xd9\x8a\0"                                    // FDAF
      u8"\xd9\x8a\xd9\x85\xd9\x8a\0"                                    // FDB0
      u8"\xd9\x85\xd9\x85\xd9\x8a\0"                                    // FDB1
      u8"\xd9\x82\xd9\x85\xd9\x8a\0"                                    // FDB2
      u8"\xd9\x86\xd8\xad\xd9\x8a\0"                                    // FDB3
      u8"\xd8\xb9\xd9\x85\xd9\x8a\0"                                    // FDB6
      u8"\xd9\x83\xd9\x85\xd9\x8a\0"                                    // FDB7
      u8"\xd9\x86\xd8\xac\xd8\xad\0"                                    // FDB8, FDBD
      u8"\xd9\x85\xd8\xae\xd9\x8a\0"                                    // FDB9
      u8"\xd9\x84\xd8\xac\xd9\x85\0"                                    // FDBA, FDBC
      u8"\xd9\x83\xd9\x85\xd9\x85\0"                                    // FDBB, FDC3
      u8"\xd8\xac\xd8\xad\xd9\x8a\0"                                    // FDBE
      u8"\xd8\xad\xd8\xac\xd9\x8a\0"                                    // FDBF
      u8"\xd9\x85\xd8\xac\xd9\x8a\0"                                    // FDC0
      u8"\xd9\x81\xd9\x85\xd9\x8a\0"                                    // FDC1
      u8"\xd8\xa8\xd8\xad\xd9\x8a\0"                                    // FDC2
      u8"\xd8\xb3\xd8\xae\xd9\x8a\0"                                    // FDC6
      u8"\xd9\x86\xd8\xac\xd9\x8a\0"                                    // FDC7
      u8"\xd8\xb5\xd9\x84\xdb\x92\0"                                    // FDF0
      u8"\xd9\x82\xd9\x84\xdb\x92\0"                                    // FDF1
      u8"\xd8\xa7\xd9\x84\xd9\x84\xd9\x87\0"                            // FDF2
      u8"\xd8\xa7\xd9\x83\xd8\xa8\xd8\xb1\0"                            // FDF3
      u8"\xd9\x85\xd8\xad\xd9\x85\xd8\xaf\0"                            // FDF4
      u8"\xd8\xb5\xd9\x84\xd8\xb9\xd9\x85\0"                            // FDF5
      u8"\xd8\xb1\xd8\xb3\xd9\x88\xd9\x84\0"                            // FDF6
      u8"\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\0"                            // FDF7
      u8"\xd9\x88\xd8\xb3\xd9\x84\xd9\x85\0"                            // FDF8
      u8"\xd8\xb5\xd9\x84\xd9\x89\0"                                    // FDF9
      u8"\xd8\xb5\xd9\x84\xd9\x89\40\xd8\xa7\xd9\x84\xd9\x84\xd9\x87\40\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\40"
      u8"\xd9\x88\xd8\xb3\xd9\x84\xd9\x85\0"                            // FDFA
      u8"\xd8\xac\xd9\x84\40\xd8\xac\xd9\x84\xd8\xa7\xd9\x84\xd9\x87\0" // FDFB
      u8"\xd8\xb1\xdb\x8c\xd8\xa7\xd9\x84\0"                            // FDFC
      u8"\54\0"                                                         // FE10, FE50, FF0C
      u8"\xe3\x80\x81\0"                                                // FE11, FE51, FF64
      u8"\72\0"                                                         // FE13, FE55, FF1A
      u8"\41\0"                                                         // FE15, FE57, FF01
      u8"\77\0"                                                         // FE16, FE56, FF1F
      u8"\xe3\x80\x96\0"                                                // FE17
      u8"\xe3\x80\x97\0"                                                // FE18
      u8"\xe2\x80\x94\0"                                                // FE31, FE58
      u8"\xe2\x80\x93\0"                                                // FE32
      u8"\x5f\0"                                                        // FE33, FE34, FE4D, FE4E, FE4F, FF3F
      u8"\x7b\0"                                                        // FE37, FE5B, FF5B
      u8"\x7d\0"                                                        // FE38, FE5C, FF5D
      u8"\xe3\x80\x94\0"                                                // FE39, FE5D
      u8"\xe3\x80\x95\0"                                                // FE3A, FE5E
      u8"\xe3\x80\x90\0"                                                // FE3B
      u8"\xe3\x80\x91\0"                                                // FE3C
      u8"\xe3\x80\x8a\0"                                                // FE3D
      u8"\xe3\x80\x8b\0"                                                // FE3E
      u8"\xe3\x80\x8c\0"                                                // FE41, FF62
      u8"\xe3\x80\x8d\0"                                                // FE42, FF63
      u8"\xe3\x80\x8e\0"                                                // FE43
      u8"\xe3\x80\x8f\0"                                                // FE44
      u8"\x5b\0"                                                        // FE47, FF3B
      u8"\x5d\0"                                                        // FE48, FF3D
      u8"\43\0"                                                         // FE5F, FF03
      u8"\46\0"                                                         // FE60, FF06
      u8"\52\0"                                                         // FE61, FF0A
      u8"\55\0"                                                         // FE63, FF0D
      u8"\74\0"                                                         // FE64, FF1C
      u8"\76\0"                                                         // FE65, FF1E
      u8"\x5c\0"                                                        // FE68, FF3C
      u8"\44\0"                                                         // FE69, FF04
      u8"\45\0"                                                         // FE6A, FF05
      u8"\x40\0"                                                        // FE6B, FF20
      u8"\40\xd9\x8b\0"                                                 // FE70
      u8"\xd9\x80\xd9\x8b\0"                                            // FE71
      u8"\40\xd9\x8c\0"                                                 // FE72
      u8"\40\xd9\x8d\0"                                                 // FE74
      u8"\40\xd9\x8e\0"                                                 // FE76
      u8"\xd9\x80\xd9\x8e\0"                                            // FE77
      u8"\40\xd9\x8f\0"                                                 // FE78
      u8"\xd9\x80\xd9\x8f\0"                                            // FE79
      u8"\40\xd9\x90\0"                                                 // FE7A
      u8"\xd9\x80\xd9\x90\0"                                            // FE7B
      u8"\40\xd9\x91\0"                                                 // FE7C
      u8"\xd9\x80\xd9\x91\0"                                            // FE7D
      u8"\40\xd9\x92\0"                                                 // FE7E
      u8"\xd9\x80\xd9\x92\0"                                            // FE7F
      u8"\xd8\xa1\0"                                                    // FE80
      u8"\xd8\xa2\0"                                                    // FE81, FE82
      u8"\xd8\xa3\0"                                                    // FE83, FE84
      u8"\xd8\xa4\0"                                                    // FE85, FE86
      u8"\xd8\xa5\0"                                                    // FE87, FE88
      u8"\xd8\xa6\0"                                                    // FE89, FE8A, FE8B, FE8C
      u8"\xd8\xa7\0"                                                    // FE8D, FE8E, 1EE00, 1EE80
      u8"\xd8\xa8\0"         // FE8F, FE90, FE91, FE92, 1EE01, 1EE21, 1EE61, 1EE81, 1EEA1
      u8"\xd8\xa9\0"         // FE93, FE94
      u8"\xd8\xaa\0"         // FE95, FE96, FE97, FE98, 1EE15, 1EE35, 1EE75, 1EE95, 1EEB5
      u8"\xd8\xab\0"         // FE99, FE9A, FE9B, FE9C, 1EE16, 1EE36, 1EE76, 1EE96, 1EEB6
      u8"\xd8\xac\0"         // FE9D, FE9E, FE9F, FEA0, 1EE02, 1EE22, 1EE42, 1EE62, 1EE82, 1EEA2
      u8"\xd8\xad\0"         // FEA1, FEA2, FEA3, FEA4, 1EE07, 1EE27, 1EE47, 1EE67, 1EE87, 1EEA7
      u8"\xd8\xae\0"         // FEA5, FEA6, FEA7, FEA8, 1EE17, 1EE37, 1EE57, 1EE77, 1EE97, 1EEB7
      u8"\xd8\xaf\0"         // FEA9, FEAA, 1EE03, 1EE83, 1EEA3
      u8"\xd8\xb0\0"         // FEAB, FEAC, 1EE18, 1EE98, 1EEB8
      u8"\xd8\xb1\0"         // FEAD, FEAE, 1EE13, 1EE93, 1EEB3
      u8"\xd8\xb2\0"         // FEAF, FEB0, 1EE06, 1EE86, 1EEA6
      u8"\xd8\xb3\0"         // FEB1, FEB2, FEB3, FEB4, 1EE0E, 1EE2E, 1EE4E, 1EE6E, 1EE8E, 1EEAE
      u8"\xd8\xb4\0"         // FEB5, FEB6, FEB7, FEB8, 1EE14, 1EE34, 1EE54, 1EE74, 1EE94, 1EEB4
      u8"\xd8\xb5\0"         // FEB9, FEBA, FEBB, FEBC, 1EE11, 1EE31, 1EE51, 1EE71, 1EE91, 1EEB1
      u8"\xd8\xb6\0"         // FEBD, FEBE, FEBF, FEC0, 1EE19, 1EE39, 1EE59, 1EE79, 1EE99, 1EEB9
      u8"\xd8\xb7\0"         // FEC1, FEC2, FEC3, FEC4, 1EE08, 1EE68, 1EE88, 1EEA8
      u8"\xd8\xb8\0"         // FEC5, FEC6, FEC7, FEC8, 1EE1A, 1EE7A, 1EE9A, 1EEBA
      u8"\xd8\xb9\0"         // FEC9, FECA, FECB, FECC, 1EE0F, 1EE2F, 1EE4F, 1EE6F, 1EE8F, 1EEAF
      u8"\xd8\xba\0"         // FECD, FECE, FECF, FED0, 1EE1B, 1EE3B, 1EE5B, 1EE7B, 1EE9B, 1EEBB
      u8"\xd9\x81\0"         // FED1, FED2, FED3, FED4, 1EE10, 1EE30, 1EE70, 1EE90, 1EEB0
      u8"\xd9\x82\0"         // FED5, FED6, FED7, FED8, 1EE12, 1EE32, 1EE52, 1EE72, 1EE92, 1EEB2
      u8"\xd9\x83\0"         // FED9, FEDA, FEDB, FEDC, 1EE0A, 1EE2A, 1EE6A
      u8"\xd9\x84\0"         // FEDD, FEDE, FEDF, FEE0, 1EE0B, 1EE2B, 1EE4B, 1EE8B, 1EEAB
      u8"\xd9\x85\0"         // FEE1, FEE2, FEE3, FEE4, 1EE0C, 1EE2C, 1EE6C, 1EE8C, 1EEAC
      u8"\xd9\x86\0"         // FEE5, FEE6, FEE7, FEE8, 1EE0D, 1EE2D, 1EE4D, 1EE6D, 1EE8D, 1EEAD
      u8"\xd9\x87\0"         // FEE9, FEEA, FEEB, FEEC, 1EE24, 1EE64, 1EE84
      u8"\xd9\x88\0"         // FEED, FEEE, 1EE05, 1EE85, 1EEA5
      u8"\xd9\x8a\0"         // FEF1, FEF2, FEF3, FEF4, 1EE09, 1EE29, 1EE49, 1EE69, 1EE89, 1EEA9
      u8"\xd9\x84\xd8\xa2\0" // FEF5, FEF6
      u8"\xd9\x84\xd8\xa3\0" // FEF7, FEF8
      u8"\xd9\x84\xd8\xa5\0" // FEF9, FEFA
      u8"\xd9\x84\xd8\xa7\0" // FEFB, FEFC
      u8"\42\0"              // FF02
      u8"\47\0"              // FF07
      u8"\57\0"              // FF0F
      u8"\x5e\0"             // FF3E
      u8"\x7c\0"             // FF5C
      u8"\x7e\0"             // FF5E
      u8"\xe2\xa6\x85\0"     // FF5F
      u8"\xe2\xa6\x86\0"     // FF60
      u8"\xe3\x83\xbb\0"     // FF65
      u8"\xe3\x82\xa1\0"     // FF67
      u8"\xe3\x82\xa3\0"     // FF68
      u8"\xe3\x82\xa5\0"     // FF69
      u8"\xe3\x82\xa7\0"     // FF6A
      u8"\xe3\x82\xa9\0"     // FF6B
      u8"\xe3\x83\xa3\0"     // FF6C
      u8"\xe3\x83\xa5\0"     // FF6D
      u8"\xe3\x83\xa7\0"     // FF6E
      u8"\xe3\x83\x83\0"     // FF6F
      u8"\xe3\x83\xbc\0"     // FF70
      u8"\xe3\x83\xb3\0"     // FF9D
      u8"\xe3\x82\x99\0"     // FF9E
      u8"\xe3\x82\x9a\0"     // FF9F
      u8"\xc2\xa2\0"         // FFE0
      u8"\xc2\xa3\0"         // FFE1
      u8"\xc2\xac\0"         // FFE2
      u8"\xc2\xa6\0"         // FFE4
      u8"\xc2\xa5\0"         // FFE5
      u8"\xe2\x82\xa9\0"     // FFE6
      u8"\xe2\x94\x82\0"     // FFE8
      u8"\xe2\x86\x90\0"     // FFE9
      u8"\xe2\x86\x91\0"     // FFEA
      u8"\xe2\x86\x92\0"     // FFEB
      u8"\xe2\x86\x93\0"     // FFEC
      u8"\xe2\x96\xa0\0"     // FFED
      u8"\xe2\x97\x8b\0"     // FFEE
      u8"\xf0\x90\x90\xa8\0" // 10400
      u8"\xf0\x90\x90\xa9\0" // 10401
      u8"\xf0\x90\x90\xaa\0" // 10402
      u8"\xf0\x90\x90\xab\0" // 10403
      u8"\xf0\x90\x90\xac\0" // 10404
      u8"\xf0\x90\x90\xad\0" // 10405
      u8"\xf0\x90\x90\xae\0" // 10406
      u8"\xf0\x90\x90\xaf\0" // 10407
      u8"\xf0\x90\x90\xb0\0" // 10408
      u8"\xf0\x90\x90\xb1\0" // 10409
      u8"\xf0\x90\x90\xb2\0" // 1040A
      u8"\xf0\x90\x90\xb3\0" // 1040B
      u8"\xf0\x90\x90\xb4\0" // 1040C
      u8"\xf0\x90\x90\xb5\0" // 1040D
      u8"\xf0\x90\x90\xb6\0" // 1040E
      u8"\xf0\x90\x90\xb7\0" // 1040F
      u8"\xf0\x90\x90\xb8\0" // 10410
      u8"\xf0\x90\x90\xb9\0" // 10411
      u8"\xf0\x90\x90\xba\0" // 10412
      u8"\xf0\x90\x90\xbb\0" // 10413
      u8"\xf0\x90\x90\xbc\0" // 10414
      u8"\xf0\x90\x90\xbd\0" // 10415
      u8"\xf0\x90\x90\xbe\0" // 10416
      u8"\xf0\x90\x90\xbf\0" // 10417
      u8"\xf0\x90\x91\x80\0" // 10418
      u8"\xf0\x90\x91\x81\0" // 10419
      u8"\xf0\x90\x91\x82\0" // 1041A
      u8"\xf0\x90\x91\x83\0" // 1041B
      u8"\xf0\x90\x91\x84\0" // 1041C
      u8"\xf0\x90\x91\x85\0" // 1041D
      u8"\xf0\x90\x91\x86\0" // 1041E
      u8"\xf0\x90\x91\x87\0" // 1041F
      u8"\xf0\x90\x91\x88\0" // 10420
      u8"\xf0\x90\x91\x89\0" // 10421
      u8"\xf0\x90\x91\x8a\0" // 10422
      u8"\xf0\x90\x91\x8b\0" // 10423
      u8"\xf0\x90\x91\x8c\0" // 10424
      u8"\xf0\x90\x91\x8d\0" // 10425
      u8"\xf0\x90\x91\x8e\0" // 10426
      u8"\xf0\x90\x91\x8f\0" // 10427
      u8"\xf0\x90\x93\x98\0" // 104B0
      u8"\xf0\x90\x93\x99\0" // 104B1
      u8"\xf0\x90\x93\x9a\0" // 104B2
      u8"\xf0\x90\x93\x9b\0" // 104B3
      u8"\xf0\x90\x93\x9c\0" // 104B4
      u8"\xf0\x90\x93\x9d\0" // 104B5
      u8"\xf0\x90\x93\x9e\0" // 104B6
      u8"\xf0\x90\x93\x9f\0" // 104B7
      u8"\xf0\x90\x93\xa0\0" // 104B8
      u8"\xf0\x90\x93\xa1\0" // 104B9
      u8"\xf0\x90\x93\xa2\0" // 104BA
      u8"\xf0\x90\x93\xa3\0" // 104BB
      u8"\xf0\x90\x93\xa4\0" // 104BC
      u8"\xf0\x90\x93\xa5\0" // 104BD
      u8"\xf0\x90\x93\xa6\0" // 104BE
      u8"\xf0\x90\x93\xa7\0" // 104BF
      u8"\xf0\x90\x93\xa8\0" // 104C0
      u8"\xf0\x90\x93\xa9\0" // 104C1
      u8"\xf0\x90\x93\xaa\0" // 104C2
      u8"\xf0\x90\x93\xab\0" // 104C3
      u8"\xf0\x90\x93\xac\0" // 104C4
      u8"\xf0\x90\x93\xad\0" // 104C5
      u8"\xf0\x90\x93\xae\0" // 104C6
      u8"\xf0\x90\x93\xaf\0" // 104C7
      u8"\xf0\x90\x93\xb0\0" // 104C8
      u8"\xf0\x90\x93\xb1\0" // 104C9
      u8"\xf0\x90\x93\xb2\0" // 104CA
      u8"\xf0\x90\x93\xb3\0" // 104CB
      u8"\xf0\x90\x93\xb4\0" // 104CC
      u8"\xf0\x90\x93\xb5\0" // 104CD
      u8"\xf0\x90\x93\xb6\0" // 104CE
      u8"\xf0\x90\x93\xb7\0" // 104CF
      u8"\xf0\x90\x93\xb8\0" // 104D0
      u8"\xf0\x90\x93\xb9\0" // 104D1
      u8"\xf0\x90\x93\xba\0" // 104D2
      u8"\xf0\x90\x93\xbb\0" // 104D3
      u8"\xf0\x90\x96\x97\0" // 10570
      u8"\xf0\x90\x96\x98\0" // 10571
      u8"\xf0\x90\x96\x99\0" // 10572
      u8"\xf0\x90\x96\x9a\0" // 10573
      u8"\xf0\x90\x96\x9b\0" // 10574
      u8"\xf0\x90\x96\x9c\0" // 10575
      u8"\xf0\x90\x96\x9d\0" // 10576
      u8"\xf0\x90\x96\x9e\0" // 10577
      u8"\xf0\x90\x96\x9f\0" // 10578
      u8"\xf0\x90\x96\xa0\0" // 10579
      u8"\xf0\x90\x96\xa1\0" // 1057A
      u8"\xf0\x90\x96\xa3\0" // 1057C
      u8"\xf0\x90\x96\xa4\0" // 1057D
      u8"\xf0\x90\x96\xa5\0" // 1057E
      u8"\xf0\x90\x96\xa6\0" // 1057F
      u8"\xf0\x90\x96\xa7\0" // 10580
      u8"\xf0\x90\x96\xa8\0" // 10581
      u8"\xf0\x90\x96\xa9\0" // 10582
      u8"\xf0\x90\x96\xaa\0" // 10583
      u8"\xf0\x90\x96\xab\0" // 10584
      u8"\xf0\x90\x96\xac\0" // 10585
      u8"\xf0\x90\x96\xad\0" // 10586
      u8"\xf0\x90\x96\xae\0" // 10587
      u8"\xf0\x90\x96\xaf\0" // 10588
      u8"\xf0\x90\x96\xb0\0" // 10589
      u8"\xf0\x90\x96\xb1\0" // 1058A
      u8"\xf0\x90\x96\xb3\0" // 1058C
      u8"\xf0\x90\x96\xb4\0" // 1058D
      u8"\xf0\x90\x96\xb5\0" // 1058E
      u8"\xf0\x90\x96\xb6\0" // 1058F
      u8"\xf0\x90\x96\xb7\0" // 10590
      u8"\xf0\x90\x96\xb8\0" // 10591
      u8"\xf0\x90\x96\xb9\0" // 10592
      u8"\xf0\x90\x96\xbb\0" // 10594
      u8"\xf0\x90\x96\xbc\0" // 10595
      u8"\xcb\x90\0"         // 10781
      u8"\xcb\x91\0"         // 10782
      u8"\xca\x99\0"         // 10784
      u8"\xca\xa3\0"         // 10787
      u8"\xea\xad\xa6\0"     // 10788
      u8"\xca\xa5\0"         // 10789
      u8"\xca\xa4\0"         // 1078A
      u8"\xe1\xb6\x91\0"     // 1078D
      u8"\xc9\x98\0"         // 1078E
      u8"\xc9\x9e\0"         // 1078F
      u8"\xca\xa9\0"         // 10790
      u8"\xc9\xa2\0"         // 10792
      u8"\xca\x9b\0"         // 10794
      u8"\xca\x9c\0"         // 10796
      u8"\xc9\xa7\0"         // 10797
      u8"\xca\x84\0"         // 10798
      u8"\xca\xaa\0"         // 10799
      u8"\xca\xab\0"         // 1079A
      u8"\xf0\x9d\xbc\x84\0" // 1079C
      u8"\xea\x9e\x8e\0"     // 1079D
      u8"\xc9\xae\0"         // 1079E
      u8"\xf0\x9d\xbc\x85\0" // 1079F
      u8"\xca\x8e\0"         // 107A0
      u8"\xf0\x9d\xbc\x86\0" // 107A1
      u8"\xc9\xb6\0"         // 107A3
      u8"\xc9\xb7\0"         // 107A4
      u8"\xc9\xba\0"         // 107A6
      u8"\xf0\x9d\xbc\x88\0" // 107A7
      u8"\xc9\xbe\0"         // 107A9
      u8"\xca\xa8\0"         // 107AB
      u8"\xca\xa6\0"         // 107AC
      u8"\xea\xad\xa7\0"     // 107AD
      u8"\xca\xa7\0"         // 107AE
      u8"\xe2\xb1\xb1\0"     // 107B0
      u8"\xca\x8f\0"         // 107B2
      u8"\xca\xa1\0"         // 107B3
      u8"\xca\xa2\0"         // 107B4
      u8"\xca\x98\0"         // 107B5
      u8"\xc7\x80\0"         // 107B6
      u8"\xc7\x81\0"         // 107B7
      u8"\xc7\x82\0"         // 107B8
      u8"\xf0\x9d\xbc\x8a\0" // 107B9
      u8"\xf0\x9d\xbc\x9e\0" // 107BA
      u8"\xf0\x90\xb3\x80\0" // 10C80
      u8"\xf0\x90\xb3\x81\0" // 10C81
      u8"\xf0\x90\xb3\x82\0" // 10C82
      u8"\xf0\x90\xb3\x83\0" // 10C83
      u8"\xf0\x90\xb3\x84\0" // 10C84
      u8"\xf0\x90\xb3\x85\0" // 10C85
      u8"\xf0\x90\xb3\x86\0" // 10C86
      u8"\xf0\x90\xb3\x87\0" // 10C87
      u8"\xf0\x90\xb3\x88\0" // 10C88
      u8"\xf0\x90\xb3\x89\0" // 10C89
      u8"\xf0\x90\xb3\x8a\0" // 10C8A
      u8"\xf0\x90\xb3\x8b\0" // 10C8B
      u8"\xf0\x90\xb3\x8c\0" // 10C8C
      u8"\xf0\x90\xb3\x8d\0" // 10C8D
      u8"\xf0\x90\xb3\x8e\0" // 10C8E
      u8"\xf0\x90\xb3\x8f\0" // 10C8F
      u8"\xf0\x90\xb3\x90\0" // 10C90
      u8"\xf0\x90\xb3\x91\0" // 10C91
      u8"\xf0\x90\xb3\x92\0" // 10C92
      u8"\xf0\x90\xb3\x93\0" // 10C93
      u8"\xf0\x90\xb3\x94\0" // 10C94
      u8"\xf0\x90\xb3\x95\0" // 10C95
      u8"\xf0\x90\xb3\x96\0" // 10C96
      u8"\xf0\x90\xb3\x97\0" // 10C97
      u8"\xf0\x90\xb3\x98\0" // 10C98
      u8"\xf0\x90\xb3\x99\0" // 10C99
      u8"\xf0\x90\xb3\x9a\0" // 10C9A
      u8"\xf0\x90\xb3\x9b\0" // 10C9B
      u8"\xf0\x90\xb3\x9c\0" // 10C9C
      u8"\xf0\x90\xb3\x9d\0" // 10C9D
      u8"\xf0\x90\xb3\x9e\0" // 10C9E
      u8"\xf0\x90\xb3\x9f\0" // 10C9F
      u8"\xf0\x90\xb3\xa0\0" // 10CA0
      u8"\xf0\x90\xb3\xa1\0" // 10CA1
      u8"\xf0\x90\xb3\xa2\0" // 10CA2
      u8"\xf0\x90\xb3\xa3\0" // 10CA3
      u8"\xf0\x90\xb3\xa4\0" // 10CA4
      u8"\xf0\x90\xb3\xa5\0" // 10CA5
      u8"\xf0\x90\xb3\xa6\0" // 10CA6
      u8"\xf0\x90\xb3\xa7\0" // 10CA7
      u8"\xf0\x90\xb3\xa8\0" // 10CA8
      u8"\xf0\x90\xb3\xa9\0" // 10CA9
      u8"\xf0\x90\xb3\xaa\0" // 10CAA
      u8"\xf0\x90\xb3\xab\0" // 10CAB
      u8"\xf0\x90\xb3\xac\0" // 10CAC
      u8"\xf0\x90\xb3\xad\0" // 10CAD
      u8"\xf0\x90\xb3\xae\0" // 10CAE
      u8"\xf0\x90\xb3\xaf\0" // 10CAF
      u8"\xf0\x90\xb3\xb0\0" // 10CB0
      u8"\xf0\x90\xb3\xb1\0" // 10CB1
      u8"\xf0\x90\xb3\xb2\0" // 10CB2
      u8"\xf0\x90\xb5\xb0\0" // 10D50
      u8"\xf0\x90\xb5\xb1\0" // 10D51
      u8"\xf0\x90\xb5\xb2\0" // 10D52
      u8"\xf0\x90\xb5\xb3\0" // 10D53
      u8"\xf0\x90\xb5\xb4\0" // 10D54
      u8"\xf0\x90\xb5\xb5\0" // 10D55
      u8"\xf0\x90\xb5\xb6\0" // 10D56
      u8"\xf0\x90\xb5\xb7\0" // 10D57
      u8"\xf0\x90\xb5\xb8\0" // 10D58
      u8"\xf0\x90\xb5\xb9\0" // 10D59
      u8"\xf0\x90\xb5\xba\0" // 10D5A
      u8"\xf0\x90\xb5\xbb\0" // 10D5B
      u8"\xf0\x90\xb5\xbc\0" // 10D5C
      u8"\xf0\x90\xb5\xbd\0" // 10D5D
      u8"\xf0\x90\xb5\xbe\0" // 10D5E
      u8"\xf0\x90\xb5\xbf\0" // 10D5F
      u8"\xf0\x90\xb6\x80\0" // 10D60
      u8"\xf0\x90\xb6\x81\0" // 10D61
      u8"\xf0\x90\xb6\x82\0" // 10D62
      u8"\xf0\x90\xb6\x83\0" // 10D63
      u8"\xf0\x90\xb6\x84\0" // 10D64
      u8"\xf0\x90\xb6\x85\0" // 10D65
      u8"\xf0\x91\xa3\x80\0" // 118A0
      u8"\xf0\x91\xa3\x81\0" // 118A1
      u8"\xf0\x91\xa3\x82\0" // 118A2
      u8"\xf0\x91\xa3\x83\0" // 118A3
      u8"\xf0\x91\xa3\x84\0" // 118A4
      u8"\xf0\x91\xa3\x85\0" // 118A5
      u8"\xf0\x91\xa3\x86\0" // 118A6
      u8"\xf0\x91\xa3\x87\0" // 118A7
      u8"\xf0\x91\xa3\x88\0" // 118A8
      u8"\xf0\x91\xa3\x89\0" // 118A9
      u8"\xf0\x91\xa3\x8a\0" // 118AA
      u8"\xf0\x91\xa3\x8b\0" // 118AB
      u8"\xf0\x91\xa3\x8c\0" // 118AC
      u8"\xf0\x91\xa3\x8d\0" // 118AD
      u8"\xf0\x91\xa3\x8e\0" // 118AE
      u8"\xf0\x91\xa3\x8f\0" // 118AF
      u8"\xf0\x91\xa3\x90\0" // 118B0
      u8"\xf0\x91\xa3\x91\0" // 118B1
      u8"\xf0\x91\xa3\x92\0" // 118B2
      u8"\xf0\x91\xa3\x93\0" // 118B3
      u8"\xf0\x91\xa3\x94\0" // 118B4
      u8"\xf0\x91\xa3\x95\0" // 118B5
      u8"\xf0\x91\xa3\x96\0" // 118B6
      u8"\xf0\x91\xa3\x97\0" // 118B7
      u8"\xf0\x91\xa3\x98\0" // 118B8
      u8"\xf0\x91\xa3\x99\0" // 118B9
      u8"\xf0\x91\xa3\x9a\0" // 118BA
      u8"\xf0\x91\xa3\x9b\0" // 118BB
      u8"\xf0\x91\xa3\x9c\0" // 118BC
      u8"\xf0\x91\xa3\x9d\0" // 118BD
      u8"\xf0\x91\xa3\x9e\0" // 118BE
      u8"\xf0\x91\xa3\x9f\0" // 118BF
      u8"\xf0\x96\xb9\xa0\0" // 16E40
      u8"\xf0\x96\xb9\xa1\0" // 16E41
      u8"\xf0\x96\xb9\xa2\0" // 16E42
      u8"\xf0\x96\xb9\xa3\0" // 16E43
      u8"\xf0\x96\xb9\xa4\0" // 16E44
      u8"\xf0\x96\xb9\xa5\0" // 16E45
      u8"\xf0\x96\xb9\xa6\0" // 16E46
      u8"\xf0\x96\xb9\xa7\0" // 16E47
      u8"\xf0\x96\xb9\xa8\0" // 16E48
      u8"\xf0\x96\xb9\xa9\0" // 16E49
      u8"\xf0\x96\xb9\xaa\0" // 16E4A
      u8"\xf0\x96\xb9\xab\0" // 16E4B
      u8"\xf0\x96\xb9\xac\0" // 16E4C
      u8"\xf0\x96\xb9\xad\0" // 16E4D
      u8"\xf0\x96\xb9\xae\0" // 16E4E
      u8"\xf0\x96\xb9\xaf\0" // 16E4F
      u8"\xf0\x96\xb9\xb0\0" // 16E50
      u8"\xf0\x96\xb9\xb1\0" // 16E51
      u8"\xf0\x96\xb9\xb2\0" // 16E52
      u8"\xf0\x96\xb9\xb3\0" // 16E53
      u8"\xf0\x96\xb9\xb4\0" // 16E54
      u8"\xf0\x96\xb9\xb5\0" // 16E55
      u8"\xf0\x96\xb9\xb6\0" // 16E56
      u8"\xf0\x96\xb9\xb7\0" // 16E57
      u8"\xf0\x96\xb9\xb8\0" // 16E58
      u8"\xf0\x96\xb9\xb9\0" // 16E59
      u8"\xf0\x96\xb9\xba\0" // 16E5A
      u8"\xf0\x96\xb9\xbb\0" // 16E5B
      u8"\xf0\x96\xb9\xbc\0" // 16E5C
      u8"\xf0\x96\xb9\xbd\0" // 16E5D
      u8"\xf0\x96\xb9\xbe\0" // 16E5E
      u8"\xf0\x96\xb9\xbf\0" // 16E5F
      u8"\xf0\x9d\x85\x97\xf0\x9d\x85\xa5\0"                 // 1D15E
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\0"                 // 1D15F
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0" // 1D160
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0" // 1D161
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb0\0" // 1D162
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb1\0" // 1D163
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb2\0" // 1D164
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\0"                 // 1D1BB
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\0"                 // 1D1BC
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0" // 1D1BD
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0" // 1D1BE
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0" // 1D1BF
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0" // 1D1C0
      u8"\xc4\xb1\0"                                         // 1D6A4
      u8"\xc8\xb7\0"                                         // 1D6A5
      u8"\xe2\x88\x87\0"                                     // 1D6C1, 1D6FB, 1D735, 1D76F, 1D7A9
      u8"\xe2\x88\x82\0"                                     // 1D6DB, 1D715, 1D74F, 1D789, 1D7C3
      u8"\xf0\x9e\xa4\xa2\0"                                 // 1E900
      u8"\xf0\x9e\xa4\xa3\0"                                 // 1E901
      u8"\xf0\x9e\xa4\xa4\0"                                 // 1E902
      u8"\xf0\x9e\xa4\xa5\0"                                 // 1E903
      u8"\xf0\x9e\xa4\xa6\0"                                 // 1E904
      u8"\xf0\x9e\xa4\xa7\0"                                 // 1E905
      u8"\xf0\x9e\xa4\xa8\0"                                 // 1E906
      u8"\xf0\x9e\xa4\xa9\0"                                 // 1E907
      u8"\xf0\x9e\xa4\xaa\0"                                 // 1E908
      u8"\xf0\x9e\xa4\xab\0"                                 // 1E909
      u8"\xf0\x9e\xa4\xac\0"                                 // 1E90A
      u8"\xf0\x9e\xa4\xad\0"                                 // 1E90B
      u8"\xf0\x9e\xa4\xae\0"                                 // 1E90C
      u8"\xf0\x9e\xa4\xaf\0"                                 // 1E90D
      u8"\xf0\x9e\xa4\xb0\0"                                 // 1E90E
      u8"\xf0\x9e\xa4\xb1\0"                                 // 1E90F
      u8"\xf0\x9e\xa4\xb2\0"                                 // 1E910
      u8"\xf0\x9e\xa4\xb3\0"                                 // 1E911
      u8"\xf0\x9e\xa4\xb4\0"                                 // 1E912
      u8"\xf0\x9e\xa4\xb5\0"                                 // 1E913
      u8"\xf0\x9e\xa4\xb6\0"                                 // 1E914
      u8"\xf0\x9e\xa4\xb7\0"                                 // 1E915
      u8"\xf0\x9e\xa4\xb8\0"                                 // 1E916
      u8"\xf0\x9e\xa4\xb9\0"                                 // 1E917
      u8"\xf0\x9e\xa4\xba\0"                                 // 1E918
      u8"\xf0\x9e\xa4\xbb\0"                                 // 1E919
      u8"\xf0\x9e\xa4\xbc\0"                                 // 1E91A
      u8"\xf0\x9e\xa4\xbd\0"                                 // 1E91B
      u8"\xf0\x9e\xa4\xbe\0"                                 // 1E91C
      u8"\xf0\x9e\xa4\xbf\0"                                 // 1E91D
      u8"\xf0\x9e\xa5\x80\0"                                 // 1E91E
      u8"\xf0\x9e\xa5\x81\0"                                 // 1E91F
      u8"\xf0\x9e\xa5\x82\0"                                 // 1E920
      u8"\xf0\x9e\xa5\x83\0"                                 // 1E921
      u8"\xd9\xae\0"                                         // 1EE1C, 1EE7C
      u8"\xda\xa1\0"                                         // 1EE1E, 1EE7E
      u8"\xd9\xaf\0"                                         // 1EE1F, 1EE5F
      u8"\60\54\0"                                           // 1F101
      u8"\61\54\0"                                           // 1F102
      u8"\62\54\0"                                           // 1F103
      u8"\63\54\0"                                           // 1F104
      u8"\64\54\0"                                           // 1F105
      u8"\65\54\0"                                           // 1F106
      u8"\66\54\0"                                           // 1F107
      u8"\67\54\0"                                           // 1F108
      u8"\70\54\0"                                           // 1F109
      u8"\71\54\0"                                           // 1F10A
      u8"\xe3\x80\x94\x73\xe3\x80\x95\0"                     // 1F12A
      u8"\x77\x7a\0"                                         // 1F12E
      u8"\x68\x76\0"                                         // 1F14A
      u8"\x73\x64\0"                                         // 1F14C
      u8"\x73\x73\0"                                         // 1F14D
      u8"\x70\x70\x76\0"                                     // 1F14E
      u8"\x77\x63\0"                                         // 1F14F
      u8"\x6d\x63\0"                                         // 1F16A
      u8"\x6d\x64\0"                                         // 1F16B
      u8"\x6d\x72\0"                                         // 1F16C
      u8"\x64\x6a\0"                                         // 1F190
      u8"\xe3\x81\xbb\xe3\x81\x8b\0"                         // 1F200
      u8"\xe3\x82\xb3\xe3\x82\xb3\0"                         // 1F201
      u8"\xe5\xad\x97\0"                                     // 1F211
      u8"\xe5\x8f\x8c\0"                                     // 1F212
      u8"\xe3\x83\x87\0"                                     // 1F213
      u8"\xe5\xa4\x9a\0"                                     // 1F215, 2F85D
      u8"\xe8\xa7\xa3\0"                                     // 1F216
      u8"\xe4\xba\xa4\0"                                     // 1F218
      u8"\xe6\x98\xa0\0"                                     // 1F219
      u8"\xe7\x84\xa1\0"                                     // 1F21A
      u8"\xe5\x89\x8d\0"                                     // 1F21C
      u8"\xe5\xbe\x8c\0"                                     // 1F21D
      u8"\xe5\x86\x8d\0"                                     // 1F21E, 2F815
      u8"\xe6\x96\xb0\0"                                     // 1F21F
      u8"\xe5\x88\x9d\0"                                     // 1F220
      u8"\xe7\xb5\x82\0"                                     // 1F221
      u8"\xe8\xb2\xa9\0"                                     // 1F223
      u8"\xe5\xa3\xb0\0"                                     // 1F224
      u8"\xe5\x90\xb9\0"                                     // 1F225
      u8"\xe6\xbc\x94\0"                                     // 1F226
      u8"\xe6\x8a\x95\0"                                     // 1F227
      u8"\xe6\x8d\x95\0"                                     // 1F228
      u8"\xe9\x81\x8a\0"                                     // 1F22B
      u8"\xe6\x8c\x87\0"                                     // 1F22F
      u8"\xe6\x89\x93\0"                                     // 1F231
      u8"\xe7\xa6\x81\0"                                     // 1F232
      u8"\xe7\xa9\xba\0"                                     // 1F233
      u8"\xe5\x90\x88\0"                                     // 1F234
      u8"\xe6\xba\x80\0"                                     // 1F235
      u8"\xe7\x94\xb3\0"                                     // 1F238
      u8"\xe5\x89\xb2\0"                                     // 1F239, 2F822
      u8"\xe5\x96\xb6\0"                                     // 1F23A
      u8"\xe9\x85\x8d\0"                                     // 1F23B
      u8"\xe3\x80\x94\xe6\x9c\xac\xe3\x80\x95\0"             // 1F240
      u8"\xe3\x80\x94\xe4\xb8\x89\xe3\x80\x95\0"             // 1F241
      u8"\xe3\x80\x94\xe4\xba\x8c\xe3\x80\x95\0"             // 1F242
      u8"\xe3\x80\x94\xe5\xae\x89\xe3\x80\x95\0"             // 1F243
      u8"\xe3\x80\x94\xe7\x82\xb9\xe3\x80\x95\0"             // 1F244
      u8"\xe3\x80\x94\xe6\x89\x93\xe3\x80\x95\0"             // 1F245
      u8"\xe3\x80\x94\xe7\x9b\x97\xe3\x80\x95\0"             // 1F246
      u8"\xe3\x80\x94\xe5\x8b\x9d\xe3\x80\x95\0"             // 1F247
      u8"\xe3\x80\x94\xe6\x95\x97\xe3\x80\x95\0"             // 1F248
      u8"\xe5\xbe\x97\0"                                     // 1F250
      u8"\xe5\x8f\xaf\0"                                     // 1F251
      u8"\xe4\xb8\xbd\0"                                     // 2F800
      u8"\xe4\xb8\xb8\0"                                     // 2F801
      u8"\xe4\xb9\x81\0"                                     // 2F802
      u8"\xf0\xa0\x84\xa2\0"                                 // 2F803
      u8"\xe4\xbd\xa0\0"                                     // 2F804
      u8"\xe4\xbe\xbb\0"                                     // 2F806
      u8"\xe5\x80\x82\0"                                     // 2F807
      u8"\xe5\x81\xba\0"                                     // 2F808
      u8"\xe5\x82\x99\0"                                     // 2F809
      u8"\xe5\x83\x8f\0"                                     // 2F80B
      u8"\xe3\x92\x9e\0"                                     // 2F80C
      u8"\xf0\xa0\x98\xba\0"                                 // 2F80D
      u8"\xe5\x85\x94\0"                                     // 2F80F
      u8"\xe5\x85\xa4\0"                                     // 2F810
      u8"\xe5\x85\xb7\0"                                     // 2F811
      u8"\xf0\xa0\x94\x9c\0"                                 // 2F812
      u8"\xe3\x92\xb9\0"                                     // 2F813
      u8"\xe5\x85\xa7\0"                                     // 2F814
      u8"\xf0\xa0\x95\x8b\0"                                 // 2F816
      u8"\xe5\x86\x97\0"                                     // 2F817
      u8"\xe5\x86\xa4\0"                                     // 2F818
      u8"\xe4\xbb\x8c\0"                                     // 2F819
      u8"\xe5\x86\xac\0"                                     // 2F81A
      u8"\xf0\xa9\x87\x9f\0"                                 // 2F81C
      u8"\xe5\x88\x83\0"                                     // 2F81E
      u8"\xe3\x93\x9f\0"                                     // 2F81F
      u8"\xe5\x88\xbb\0"                                     // 2F820
      u8"\xe5\x89\x86\0"                                     // 2F821
      u8"\xe5\x89\xb7\0"                                     // 2F823
      u8"\xe3\x94\x95\0"                                     // 2F824
      u8"\xe5\x8c\x85\0"                                     // 2F829
      u8"\xe5\x8c\x86\0"                                     // 2F82A
      u8"\xe5\x8d\x89\0"                                     // 2F82C
      u8"\xe5\x8d\x9a\0"                                     // 2F82E
      u8"\xe5\x8d\xb3\0"                                     // 2F82F
      u8"\xe5\x8d\xbd\0"                                     // 2F830
      u8"\xe5\x8d\xbf\0"                                     // 2F831, 2F832, 2F833
      u8"\xf0\xa0\xa8\xac\0"                                 // 2F834
      u8"\xe7\x81\xb0\0"                                     // 2F835
      u8"\xe5\x8f\x8a\0"                                     // 2F836
      u8"\xe5\x8f\x9f\0"                                     // 2F837
      u8"\xf0\xa0\xad\xa3\0"                                 // 2F838
      u8"\xe5\x8f\xab\0"                                     // 2F839
      u8"\xe5\x8f\xb1\0"                                     // 2F83A
      u8"\xe5\x90\x86\0"                                     // 2F83B
      u8"\xe5\x92\x9e\0"                                     // 2F83C
      u8"\xe5\x90\xb8\0"                                     // 2F83D
      u8"\xe5\x91\x88\0"                                     // 2F83E
      u8"\xe5\x91\xa8\0"                                     // 2F83F
      u8"\xe5\x92\xa2\0"                                     // 2F840
      u8"\xe5\x93\xb6\0"                                     // 2F841
      u8"\xe5\x94\x90\0"                                     // 2F842
      u8"\xe5\x95\x93\0"                                     // 2F843
      u8"\xe5\x95\xa3\0"                                     // 2F844
      u8"\xe5\x96\x84\0"                                     // 2F845, 2F846
      u8"\xe5\x96\xab\0"                                     // 2F848
      u8"\xe5\x96\xb3\0"                                     // 2F849
      u8"\xe5\x97\x82\0"                                     // 2F84A
      u8"\xe5\x9c\x96\0"                                     // 2F84B
      u8"\xe5\x9c\x97\0"                                     // 2F84D
      u8"\xe5\x99\x91\0"                                     // 2F84E
      u8"\xe5\x99\xb4\0"                                     // 2F84F
      u8"\xe5\xa3\xae\0"                                     // 2F851
      u8"\xe5\x9f\x8e\0"                                     // 2F852
      u8"\xe5\x9f\xb4\0"                                     // 2F853
      u8"\xe5\xa0\x8d\0"                                     // 2F854
      u8"\xe5\x9e\x8b\0"                                     // 2F855
      u8"\xe5\xa0\xb2\0"                                     // 2F856
      u8"\xe5\xa0\xb1\0"                                     // 2F857
      u8"\xe5\xa2\xac\0"                                     // 2F858
      u8"\xf0\xa1\x93\xa4\0"                                 // 2F859
      u8"\xe5\xa3\xb2\0"                                     // 2F85A
      u8"\xe5\xa3\xb7\0"                                     // 2F85B
      u8"\xe5\xa4\x86\0"                                     // 2F85C
      u8"\xe5\xa4\xa2\0"                                     // 2F85E
      u8"\xe5\xa5\xa2\0"                                     // 2F85F
      u8"\xf0\xa1\x9a\xa8\0"                                 // 2F860
      u8"\xf0\xa1\x9b\xaa\0"                                 // 2F861
      u8"\xe5\xa7\xac\0"                                     // 2F862
      u8"\xe5\xa8\x9b\0"                                     // 2F863
      u8"\xe5\xa8\xa7\0"                                     // 2F864
      u8"\xe5\xa7\x98\0"                                     // 2F865
      u8"\xe5\xa9\xa6\0"                                     // 2F866
      u8"\xe3\x9b\xae\0"                                     // 2F867
      u8"\xe3\x9b\xbc\0"                                     // 2F868
      u8"\xe5\xac\x88\0"                                     // 2F869
      u8"\xe5\xac\xbe\0"                                     // 2F86A, 2F86B
      u8"\xf0\xa1\xa7\x88\0"                                 // 2F86C
      u8"\xe5\xaf\x83\0"                                     // 2F86D
      u8"\xe5\xaf\x98\0"                                     // 2F86E
      u8"\xe5\xaf\xb3\0"                                     // 2F870
      u8"\xf0\xa1\xac\x98\0"                                 // 2F871
      u8"\xe5\xaf\xbf\0"                                     // 2F872
      u8"\xe5\xb0\x86\0"                                     // 2F873
      u8"\xe5\xbd\x93\0"                                     // 2F874
      u8"\xe3\x9e\x81\0"                                     // 2F876
      u8"\xe5\xb1\xa0\0"                                     // 2F877
      u8"\xe5\xb3\x80\0"                                     // 2F879
      u8"\xe5\xb2\x8d\0"                                     // 2F87A
      u8"\xf0\xa1\xb7\xa4\0"                                 // 2F87B
      u8"\xe5\xb5\x83\0"                                     // 2F87C
      u8"\xf0\xa1\xb7\xa6\0"                                 // 2F87D
      u8"\xe5\xb5\xae\0"                                     // 2F87E
      u8"\xe5\xb5\xab\0"                                     // 2F87F
      u8"\xe5\xb5\xbc\0"                                     // 2F880
      u8"\xe5\xb7\xa1\0"                                     // 2F881
      u8"\xe5\xb7\xa2\0"                                     // 2F882
      u8"\xe3\xa0\xaf\0"                                     // 2F883
      u8"\xe5\xb7\xbd\0"                                     // 2F884
      u8"\xe5\xb8\xa8\0"                                     // 2F885
      u8"\xe5\xb8\xbd\0"                                     // 2F886
      u8"\xe5\xb9\xa9\0"                                     // 2F887
      u8"\xe3\xa1\xa2\0"                                     // 2F888
      u8"\xf0\xa2\x86\x83\0"                                 // 2F889
      u8"\xe3\xa1\xbc\0"                                     // 2F88A
      u8"\xe5\xba\xb0\0"                                     // 2F88B
      u8"\xe5\xba\xb3\0"                                     // 2F88C
      u8"\xe5\xba\xb6\0"                                     // 2F88D
      u8"\xf0\xaa\x8e\x92\0"                                 // 2F88F
      u8"\xf0\xa2\x8c\xb1\0"                                 // 2F891, 2F892
      u8"\xe8\x88\x81\0"                                     // 2F893, 2F98B
      u8"\xe5\xbc\xa2\0"                                     // 2F894, 2F895
      u8"\xe3\xa3\x87\0"                                     // 2F896
      u8"\xf0\xa3\x8a\xb8\0"                                 // 2F897
      u8"\xf0\xa6\x87\x9a\0"                                 // 2F898
      u8"\xe5\xbd\xa2\0"                                     // 2F899
      u8"\xe5\xbd\xab\0"                                     // 2F89A
      u8"\xe3\xa3\xa3\0"                                     // 2F89B
      u8"\xe5\xbe\x9a\0"                                     // 2F89C
      u8"\xe5\xbf\x8d\0"                                     // 2F89D
      u8"\xe5\xbf\x97\0"                                     // 2F89E
      u8"\xe5\xbf\xb9\0"                                     // 2F89F
      u8"\xe6\x82\x81\0"                                     // 2F8A0
      u8"\xe3\xa4\xba\0"                                     // 2F8A1
      u8"\xe3\xa4\x9c\0"                                     // 2F8A2
      u8"\xf0\xa2\x9b\x94\0"                                 // 2F8A4
      u8"\xe6\x83\x87\0"                                     // 2F8A5
      u8"\xe6\x85\x88\0"                                     // 2F8A6
      u8"\xe6\x85\x8c\0"                                     // 2F8A7, 2F8A9
      u8"\xe6\x85\xba\0"                                     // 2F8AA
      u8"\xe6\x86\xb2\0"                                     // 2F8AC
      u8"\xe6\x86\xa4\0"                                     // 2F8AD
      u8"\xe6\x86\xaf\0"                                     // 2F8AE
      u8"\xe6\x87\x9e\0"                                     // 2F8AF
      u8"\xe6\x88\x90\0"                                     // 2F8B2
      u8"\xe6\x88\x9b\0"                                     // 2F8B3
      u8"\xe6\x89\x9d\0"                                     // 2F8B4
      u8"\xe6\x8a\xb1\0"                                     // 2F8B5
      u8"\xe6\x8b\x94\0"                                     // 2F8B6
      u8"\xe6\x8d\x90\0"                                     // 2F8B7
      u8"\xf0\xa2\xac\x8c\0"                                 // 2F8B8
      u8"\xe6\x8c\xbd\0"                                     // 2F8B9
      u8"\xe6\x8b\xbc\0"                                     // 2F8BA
      u8"\xe6\x8d\xa8\0"                                     // 2F8BB
      u8"\xe6\x8e\x83\0"                                     // 2F8BC
      u8"\xe6\x8f\xa4\0"                                     // 2F8BD
      u8"\xf0\xa2\xaf\xb1\0"                                 // 2F8BE
      u8"\xe6\x90\xa2\0"                                     // 2F8BF
      u8"\xe6\x8f\x85\0"                                     // 2F8C0
      u8"\xe6\x8e\xa9\0"                                     // 2F8C1
      u8"\xe3\xa8\xae\0"                                     // 2F8C2
      u8"\xe6\x91\xa9\0"                                     // 2F8C3
      u8"\xe6\x91\xbe\0"                                     // 2F8C4
      u8"\xe6\x92\x9d\0"                                     // 2F8C5
      u8"\xe6\x91\xb7\0"                                     // 2F8C6
      u8"\xe3\xa9\xac\0"                                     // 2F8C7
      u8"\xe6\x95\xac\0"                                     // 2F8C9
      u8"\xf0\xa3\x80\x8a\0"                                 // 2F8CA
      u8"\xe6\x97\xa3\0"                                     // 2F8CB
      u8"\xe6\x9b\xb8\0"                                     // 2F8CC
      u8"\xe6\x99\x89\0"                                     // 2F8CD
      u8"\xe3\xac\x99\0"                                     // 2F8CE
      u8"\xe3\xac\x88\0"                                     // 2F8D0
      u8"\xe3\xab\xa4\0"                                     // 2F8D1
      u8"\xe5\x86\x92\0"                                     // 2F8D2
      u8"\xe5\x86\x95\0"                                     // 2F8D3
      u8"\xe6\x9c\x80\0"                                     // 2F8D4
      u8"\xe6\x9a\x9c\0"                                     // 2F8D5
      u8"\xe8\x82\xad\0"                                     // 2F8D6
      u8"\xe4\x8f\x99\0"                                     // 2F8D7
      u8"\xe6\x9c\xa1\0"                                     // 2F8DA
      u8"\xe6\x9d\x9e\0"                                     // 2F8DB
      u8"\xe6\x9d\x93\0"                                     // 2F8DC
      u8"\xf0\xa3\x8f\x83\0"                                 // 2F8DD
      u8"\xe3\xad\x89\0"                                     // 2F8DE
      u8"\xe6\x9f\xba\0"                                     // 2F8DF
      u8"\xe6\x9e\x85\0"                                     // 2F8E0
      u8"\xe6\xa1\x92\0"                                     // 2F8E1
      u8"\xf0\xa3\x91\xad\0"                                 // 2F8E3
      u8"\xe6\xa2\x8e\0"                                     // 2F8E4
      u8"\xe6\xa0\x9f\0"                                     // 2F8E5
      u8"\xe6\xa4\x94\0"                                     // 2F8E6
      u8"\xe6\xa5\x82\0"                                     // 2F8E8
      u8"\xe6\xa6\xa3\0"                                     // 2F8E9
      u8"\xe6\xa7\xaa\0"                                     // 2F8EA
      u8"\xe6\xaa\xa8\0"                                     // 2F8EB
      u8"\xf0\xa3\x9a\xa3\0"                                 // 2F8EC
      u8"\xe6\xab\x9b\0"                                     // 2F8ED
      u8"\xe3\xb0\x98\0"                                     // 2F8EE
      u8"\xe6\xac\xa1\0"                                     // 2F8EF
      u8"\xf0\xa3\xa2\xa7\0"                                 // 2F8F0
      u8"\xe6\xad\x94\0"                                     // 2F8F1
      u8"\xe3\xb1\x8e\0"                                     // 2F8F2
      u8"\xe6\xad\xb2\0"                                     // 2F8F3
      u8"\xe6\xae\x9f\0"                                     // 2F8F4
      u8"\xe6\xae\xbb\0"                                     // 2F8F6
      u8"\xf0\xa3\xaa\x8d\0"                                 // 2F8F7
      u8"\xf0\xa1\xb4\x8b\0"                                 // 2F8F8
      u8"\xf0\xa3\xab\xba\0"                                 // 2F8F9
      u8"\xe6\xb1\x8e\0"                                     // 2F8FA
      u8"\xf0\xa3\xb2\xbc\0"                                 // 2F8FB
      u8"\xe6\xb2\xbf\0"                                     // 2F8FC
      u8"\xe6\xb3\x8d\0"                                     // 2F8FD
      u8"\xe6\xb1\xa7\0"                                     // 2F8FE
      u8"\xe6\xb4\x96\0"                                     // 2F8FF
      u8"\xe6\xb4\xbe\0"                                     // 2F900
      u8"\xe6\xb5\xa9\0"                                     // 2F903
      u8"\xe6\xb5\xb8\0"                                     // 2F904
      u8"\xe6\xb6\x85\0"                                     // 2F905
      u8"\xf0\xa3\xb4\x9e\0"                                 // 2F906
      u8"\xe6\xb4\xb4\0"                                     // 2F907
      u8"\xe6\xb8\xaf\0"                                     // 2F908
      u8"\xe6\xb9\xae\0"                                     // 2F909
      u8"\xe3\xb4\xb3\0"                                     // 2F90A
      u8"\xe6\xbb\x87\0"                                     // 2F90C
      u8"\xf0\xa3\xbb\x91\0"                                 // 2F90D
      u8"\xe6\xb7\xb9\0"                                     // 2F90E
      u8"\xe6\xbd\xae\0"                                     // 2F90F
      u8"\xf0\xa3\xbd\x9e\0"                                 // 2F910
      u8"\xf0\xa3\xbe\x8e\0"                                 // 2F911
      u8"\xe6\xbf\x86\0"                                     // 2F912
      u8"\xe7\x80\xb9\0"                                     // 2F913
      u8"\xe7\x80\x9b\0"                                     // 2F915
      u8"\xe3\xb6\x96\0"                                     // 2F916
      u8"\xe7\x81\x8a\0"                                     // 2F917
      u8"\xe7\x81\xbd\0"                                     // 2F918
      u8"\xe7\x81\xb7\0"                                     // 2F919
      u8"\xe7\x82\xad\0"                                     // 2F91A
      u8"\xf0\xa0\x94\xa5\0"                                 // 2F91B
      u8"\xe7\x85\x85\0"                                     // 2F91C
      u8"\xf0\xa4\x89\xa3\0"                                 // 2F91D
      u8"\xe7\x86\x9c\0"                                     // 2F91E
      u8"\xf0\xa4\x8e\xab\0"                                 // 2F91F
      u8"\xe7\x88\xa8\0"                                     // 2F920
      u8"\xe7\x89\x90\0"                                     // 2F922
      u8"\xf0\xa4\x98\x88\0"                                 // 2F923
      u8"\xe7\x8a\x80\0"                                     // 2F924
      u8"\xe7\x8a\x95\0"                                     // 2F925
      u8"\xf0\xa4\x9c\xb5\0"                                 // 2F926
      u8"\xf0\xa4\xa0\x94\0"                                 // 2F927
      u8"\xe7\x8d\xba\0"                                     // 2F928
      u8"\xe7\x8e\x8b\0"                                     // 2F929
      u8"\xe3\xba\xac\0"                                     // 2F92A
      u8"\xe7\x8e\xa5\0"                                     // 2F92B
      u8"\xe3\xba\xb8\0"                                     // 2F92C, 2F92D
      u8"\xe7\x91\x87\0"                                     // 2F92E
      u8"\xe7\x91\x9c\0"                                     // 2F92F
      u8"\xe7\x92\x85\0"                                     // 2F931
      u8"\xe7\x93\x8a\0"                                     // 2F932
      u8"\xe3\xbc\x9b\0"                                     // 2F933
      u8"\xe7\x94\xa4\0"                                     // 2F934
      u8"\xf0\xa4\xb0\xb6\0"                                 // 2F935
      u8"\xe7\x94\xbe\0"                                     // 2F936
      u8"\xf0\xa4\xb2\x92\0"                                 // 2F937
      u8"\xf0\xa2\x86\x9f\0"                                 // 2F939
      u8"\xe7\x98\x90\0"                                     // 2F93A
      u8"\xf0\xa4\xbe\xa1\0"                                 // 2F93B
      u8"\xf0\xa4\xbe\xb8\0"                                 // 2F93C
      u8"\xf0\xa5\x81\x84\0"                                 // 2F93D
      u8"\xe3\xbf\xbc\0"                                     // 2F93E
      u8"\xe4\x80\x88\0"                                     // 2F93F
      u8"\xf0\xa5\x83\xb3\0"                                 // 2F941
      u8"\xf0\xa5\x83\xb2\0"                                 // 2F942
      u8"\xf0\xa5\x84\x99\0"                                 // 2F943
      u8"\xf0\xa5\x84\xb3\0"                                 // 2F944
      u8"\xe7\x9c\x9e\0"                                     // 2F945
      u8"\xe7\x9c\x9f\0"                                     // 2F946, 2F947
      u8"\xe7\x9e\x8b\0"                                     // 2F94A
      u8"\xe4\x81\x86\0"                                     // 2F94B
      u8"\xe4\x82\x96\0"                                     // 2F94C
      u8"\xf0\xa5\x90\x9d\0"                                 // 2F94D
      u8"\xe7\xa1\x8e\0"                                     // 2F94E
      u8"\xe4\x83\xa3\0"                                     // 2F951
      u8"\xf0\xa5\x98\xa6\0"                                 // 2F952
      u8"\xf0\xa5\x9a\x9a\0"                                 // 2F954
      u8"\xf0\xa5\x9b\x85\0"                                 // 2F955
      u8"\xe7\xa7\xab\0"                                     // 2F957
      u8"\xe4\x84\xaf\0"                                     // 2F958
      u8"\xe7\xa9\x8a\0"                                     // 2F95A
      u8"\xe7\xa9\x8f\0"                                     // 2F95B
      u8"\xf0\xa5\xa5\xbc\0"                                 // 2F95C
      u8"\xf0\xa5\xaa\xa7\0"                                 // 2F95D, 2F95E
      u8"\xe7\xab\xae\0"                                     // 2F95F
      u8"\xe4\x88\x82\0"                                     // 2F960
      u8"\xf0\xa5\xae\xab\0"                                 // 2F961
      u8"\xe7\xaf\x86\0"                                     // 2F962
      u8"\xe7\xaf\x89\0"                                     // 2F963
      u8"\xe4\x88\xa7\0"                                     // 2F964
      u8"\xf0\xa5\xb2\x80\0"                                 // 2F965
      u8"\xe7\xb3\x92\0"                                     // 2F966
      u8"\xe4\x8a\xa0\0"                                     // 2F967
      u8"\xe7\xb3\xa8\0"                                     // 2F968
      u8"\xe7\xb3\xa3\0"                                     // 2F969
      u8"\xe7\xb4\x80\0"                                     // 2F96A
      u8"\xf0\xa5\xbe\x86\0"                                 // 2F96B
      u8"\xe7\xb5\xa3\0"                                     // 2F96C
      u8"\xe4\x8c\x81\0"                                     // 2F96D
      u8"\xe7\xb7\x87\0"                                     // 2F96E
      u8"\xe7\xb8\x82\0"                                     // 2F96F
      u8"\xe7\xb9\x85\0"                                     // 2F970
      u8"\xe4\x8c\xb4\0"                                     // 2F971
      u8"\xf0\xa6\x88\xa8\0"                                 // 2F972
      u8"\xf0\xa6\x89\x87\0"                                 // 2F973
      u8"\xe4\x8d\x99\0"                                     // 2F974
      u8"\xf0\xa6\x8b\x99\0"                                 // 2F975
      u8"\xe7\xbd\xba\0"                                     // 2F976
      u8"\xf0\xa6\x8c\xbe\0"                                 // 2F977
      u8"\xe7\xbe\x95\0"                                     // 2F978
      u8"\xe7\xbf\xba\0"                                     // 2F979
      u8"\xf0\xa6\x93\x9a\0"                                 // 2F97B
      u8"\xf0\xa6\x94\xa3\0"                                 // 2F97C
      u8"\xe8\x81\xa0\0"                                     // 2F97D
      u8"\xf0\xa6\x96\xa8\0"                                 // 2F97E
      u8"\xe8\x81\xb0\0"                                     // 2F97F
      u8"\xf0\xa3\x8d\x9f\0"                                 // 2F980
      u8"\xe4\x8f\x95\0"                                     // 2F981
      u8"\xe8\x82\xb2\0"                                     // 2F982
      u8"\xe8\x84\x83\0"                                     // 2F983
      u8"\xe4\x90\x8b\0"                                     // 2F984
      u8"\xe8\x84\xbe\0"                                     // 2F985
      u8"\xe5\xaa\xb5\0"                                     // 2F986
      u8"\xf0\xa6\x9e\xa7\0"                                 // 2F987
      u8"\xf0\xa6\x9e\xb5\0"                                 // 2F988
      u8"\xf0\xa3\x8e\x93\0"                                 // 2F989
      u8"\xf0\xa3\x8e\x9c\0"                                 // 2F98A
      u8"\xe8\x88\x84\0"                                     // 2F98C
      u8"\xe8\xbe\x9e\0"                                     // 2F98D
      u8"\xe4\x91\xab\0"                                     // 2F98E
      u8"\xe8\x8a\x91\0"                                     // 2F98F
      u8"\xe8\x8a\x8b\0"                                     // 2F990
      u8"\xe8\x8a\x9d\0"                                     // 2F991
      u8"\xe5\x8a\xb3\0"                                     // 2F992
      u8"\xe8\x8a\xb1\0"                                     // 2F993
      u8"\xe8\x8a\xb3\0"                                     // 2F994
      u8"\xe8\x8a\xbd\0"                                     // 2F995
      u8"\xe8\x8b\xa6\0"                                     // 2F996
      u8"\xf0\xa6\xac\xbc\0"                                 // 2F997
      u8"\xe8\x8c\x9d\0"                                     // 2F999
      u8"\xe8\x8d\xa3\0"                                     // 2F99A
      u8"\xe8\x8e\xad\0"                                     // 2F99B
      u8"\xe8\x8c\xa3\0"                                     // 2F99C
      u8"\xe8\x8e\xbd\0"                                     // 2F99D
      u8"\xe8\x8f\xa7\0"                                     // 2F99E
      u8"\xe8\x8d\x93\0"                                     // 2F9A0
      u8"\xe8\x8f\x8a\0"                                     // 2F9A1
      u8"\xe8\x8f\x8c\0"                                     // 2F9A2
      u8"\xe8\x8f\x9c\0"                                     // 2F9A3
      u8"\xf0\xa6\xb0\xb6\0"                                 // 2F9A4
      u8"\xf0\xa6\xb5\xab\0"                                 // 2F9A5
      u8"\xf0\xa6\xb3\x95\0"                                 // 2F9A6
      u8"\xe4\x94\xab\0"                                     // 2F9A7
      u8"\xe8\x93\xb1\0"                                     // 2F9A8
      u8"\xe8\x93\xb3\0"                                     // 2F9A9
      u8"\xe8\x94\x96\0"                                     // 2F9AA
      u8"\xf0\xa7\x8f\x8a\0"                                 // 2F9AB
      u8"\xe8\x95\xa4\0"                                     // 2F9AC
      u8"\xf0\xa6\xbc\xac\0"                                 // 2F9AD
      u8"\xe4\x95\x9d\0"                                     // 2F9AE
      u8"\xe4\x95\xa1\0"                                     // 2F9AF
      u8"\xf0\xa6\xbe\xb1\0"                                 // 2F9B0
      u8"\xf0\xa7\x83\x92\0"                                 // 2F9B1
      u8"\xe4\x95\xab\0"                                     // 2F9B2
      u8"\xe8\x99\x90\0"                                     // 2F9B3
      u8"\xe8\x99\xa7\0"                                     // 2F9B5
      u8"\xe8\x99\xa9\0"                                     // 2F9B6
      u8"\xe8\x9a\xa9\0"                                     // 2F9B7
      u8"\xe8\x9a\x88\0"                                     // 2F9B8
      u8"\xe8\x9c\x8e\0"                                     // 2F9B9
      u8"\xe8\x9b\xa2\0"                                     // 2F9BA
      u8"\xe8\x9c\xa8\0"                                     // 2F9BC
      u8"\xe8\x9d\xab\0"                                     // 2F9BD
      u8"\xe8\x9e\x86\0"                                     // 2F9BE
      u8"\xe4\x97\x97\0"                                     // 2F9BF
      u8"\xe8\x9f\xa1\0"                                     // 2F9C0
      u8"\xe8\xa0\x81\0"                                     // 2F9C1
      u8"\xe4\x97\xb9\0"                                     // 2F9C2
      u8"\xe8\xa1\xa0\0"                                     // 2F9C3
      u8"\xf0\xa7\x99\xa7\0"                                 // 2F9C5
      u8"\xe8\xa3\x97\0"                                     // 2F9C6
      u8"\xe8\xa3\x9e\0"                                     // 2F9C7
      u8"\xe4\x98\xb5\0"                                     // 2F9C8
      u8"\xe8\xa3\xba\0"                                     // 2F9C9
      u8"\xe3\x92\xbb\0"                                     // 2F9CA
      u8"\xf0\xa7\xa2\xae\0"                                 // 2F9CB
      u8"\xf0\xa7\xa5\xa6\0"                                 // 2F9CC
      u8"\xe4\x9a\xbe\0"                                     // 2F9CD
      u8"\xe4\x9b\x87\0"                                     // 2F9CE
      u8"\xe8\xaa\xa0\0"                                     // 2F9CF
      u8"\xf0\xa7\xb2\xa8\0"                                 // 2F9D3
      u8"\xe8\xb2\xab\0"                                     // 2F9D4
      u8"\xe8\xb3\x81\0"                                     // 2F9D5
      u8"\xe8\xb4\x9b\0"                                     // 2F9D6
      u8"\xe8\xb5\xb7\0"                                     // 2F9D7
      u8"\xf0\xa7\xbc\xaf\0"                                 // 2F9D8
      u8"\xf0\xa0\xa0\x84\0"                                 // 2F9D9
      u8"\xe8\xb7\x8b\0"                                     // 2F9DA
      u8"\xe8\xb6\xbc\0"                                     // 2F9DB
      u8"\xe8\xb7\xb0\0"                                     // 2F9DC
      u8"\xf0\xa0\xa3\x9e\0"                                 // 2F9DD
      u8"\xe8\xbb\x94\0"                                     // 2F9DE
      u8"\xf0\xa8\x97\x92\0"                                 // 2F9E0
      u8"\xf0\xa8\x97\xad\0"                                 // 2F9E1
      u8"\xe9\x82\x94\0"                                     // 2F9E2
      u8"\xe9\x83\xb1\0"                                     // 2F9E3
      u8"\xe9\x84\x91\0"                                     // 2F9E4
      u8"\xf0\xa8\x9c\xae\0"                                 // 2F9E5
      u8"\xe9\x84\x9b\0"                                     // 2F9E6
      u8"\xe9\x88\xb8\0"                                     // 2F9E7
      u8"\xe9\x8b\x97\0"                                     // 2F9E8
      u8"\xe9\x8b\x98\0"                                     // 2F9E9
      u8"\xe9\x89\xbc\0"                                     // 2F9EA
      u8"\xe9\x8f\xb9\0"                                     // 2F9EB
      u8"\xe9\x90\x95\0"                                     // 2F9EC
      u8"\xf0\xa8\xaf\xba\0"                                 // 2F9ED
      u8"\xe9\x96\x8b\0"                                     // 2F9EE
      u8"\xe4\xa6\x95\0"                                     // 2F9EF
      u8"\xe9\x96\xb7\0"                                     // 2F9F0
      u8"\xf0\xa8\xb5\xb7\0"                                 // 2F9F1
      u8"\xe4\xa7\xa6\0"                                     // 2F9F2
      u8"\xe9\x9b\x83\0"                                     // 2F9F3
      u8"\xe5\xb6\xb2\0"                                     // 2F9F4
      u8"\xe9\x9c\xa3\0"                                     // 2F9F5
      u8"\xf0\xa9\x85\x85\0"                                 // 2F9F6
      u8"\xf0\xa9\x88\x9a\0"                                 // 2F9F7
      u8"\xe4\xa9\xae\0"                                     // 2F9F8
      u8"\xe4\xa9\xb6\0"                                     // 2F9F9
      u8"\xe9\x9f\xa0\0"                                     // 2F9FA
      u8"\xf0\xa9\x90\x8a\0"                                 // 2F9FB
      u8"\xe4\xaa\xb2\0"                                     // 2F9FC
      u8"\xf0\xa9\x92\x96\0"                                 // 2F9FD
      u8"\xe9\xa0\xa9\0"                                     // 2FA00
      u8"\xf0\xa9\x96\xb6\0"                                 // 2FA01
      u8"\xe9\xa3\xa2\0"                                     // 2FA02
      u8"\xe4\xac\xb3\0"                                     // 2FA03
      u8"\xe9\xa4\xa9\0"                                     // 2FA04
      u8"\xe9\xa6\xa7\0"                                     // 2FA05
      u8"\xe9\xa7\x82\0"                                     // 2FA06
      u8"\xe9\xa7\xbe\0"                                     // 2FA07
      u8"\xe4\xaf\x8e\0"                                     // 2FA08
      u8"\xf0\xa9\xac\xb0\0"                                 // 2FA09
      u8"\xe9\xb1\x80\0"                                     // 2FA0B
      u8"\xe9\xb3\xbd\0"                                     // 2FA0C
      u8"\xe4\xb3\x8e\0"                                     // 2FA0D
      u8"\xe4\xb3\xad\0"                                     // 2FA0E
      u8"\xe9\xb5\xa7\0"                                     // 2FA0F
      u8"\xf0\xaa\x83\x8e\0"                                 // 2FA10
      u8"\xe4\xb3\xb8\0"                                     // 2FA11
      u8"\xf0\xaa\x84\x85\0"                                 // 2FA12
      u8"\xf0\xaa\x88\x8e\0"                                 // 2FA13
      u8"\xf0\xaa\x8a\x91\0"                                 // 2FA14
      u8"\xe4\xb5\x96\0"                                     // 2FA16
      u8"\xe9\xbb\xbe\0"                                     // 2FA18
      u8"\xe9\xbc\x85\0"                                     // 2FA19
      u8"\xe9\xbc\x8f\0"                                     // 2FA1A
      u8"\xe9\xbc\x96\0"                                     // 2FA1B
      u8"\xf0\xaa\x98\x80\0"                                 // 2FA1D
      ,
      17246UL                                                // String Length
    };

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP
