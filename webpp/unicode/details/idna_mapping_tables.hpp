
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_idna_mapping_tables3.mjs
 *   Unicode UCD Database Creation Date: 2024-08-25
 *   This file's generation date:        Thu, 24 Apr 2025 15:44:18 GMT
 *   Unicode Version:                    16.0.0
 *   Total Table sizes in this file:
 *       - in bits:       365680
 *       - in bytes:      45710 B
 *       - in KibiBytes:  44.64 KiB
 *   Some other implementations' total table size was 84.47 KiB;
 *   So I have saved 39.83 KiB.
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

namespace webpp::unicode::idna::details {

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
            return static_cast<std::uint16_t>(
              static_cast<std::uint16_t>(pos) |
              (static_cast<std::uint16_t>(use_second_table) << use_second_table_shift));
        }

        static constexpr std::uint16_t chunk_mask  = 0x1FU;
        static constexpr std::size_t   chunk_size  = 32U;
        static constexpr std::uint8_t  chunk_shift = 5U;

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

    struct idna_breakpoint_type {
        std::uint16_t starting;
        std::uint16_t ending;
        std::uint16_t offset;
        idna_index    common_value; // if it's not in the specified range, this value should be used.
    };

    /**
     * You can choose between the indices' table using these breakpoints:
     *
     * Table size in KibiBytes:  2.13 KiB
     */
    static constexpr std::array<idna_breakpoint_type, 34U> idna_breakpoints{
      idna_breakpoint_type{    .starting = 0,   .ending = 416,     .offset = 0,  .common_value = iblt | 0x0U}, // Section
                                                                                                    // 1
      { .starting = 1315,  .ending = 4096,   .offset = 899,  .common_value = iblt | 0x0U}, // Section 2
      { .starting = 1315,  .ending = 4096,   .offset = 899,  .common_value = iblt | 0x0U}, // Section 2
      { .starting = 1315,  .ending = 4096,   .offset = 899,  .common_value = iblt | 0x0U}, // Section 2
      { .starting = 1315,  .ending = 4096,   .offset = 899,  .common_value = iblt | 0x0U}, // Section 2
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      { .starting = 5430,  .ending = 6430,  .offset = 2233, .common_value = iblt | 0x20U}, // Section 3
      {.starting = 28679, .ending = 28688, .offset = 24482, .common_value = iblt | 0x20U}, // Section 4
      {.starting = 28679, .ending = 28688, .offset = 24482, .common_value = iblt | 0x20U}, // Section 4
      {.starting = 28679, .ending = 28688, .offset = 24482, .common_value = iblt | 0x20U}, // Section 4
      {.starting = 28679, .ending = 28688, .offset = 24482, .common_value = iblt | 0x20U}, // Section 4
      {.starting = 28679, .ending = 28688, .offset = 24482, .common_value = iblt | 0x20U}, // Section 4
      {.starting = 34815, .ending = 34816, .offset = 30609,         .common_value = 0x0U}, // Section 5
    };

    static constexpr std::uint16_t idna_last_breakpoint{0x8800U};
    static constexpr std::uint16_t idna_breakpoint_shift{10U};
    static constexpr idna_index    idna_common_pos{32800U}; // this is the last common value position


    /**
     * idna Index Table (combined 5 sections)
     *
     * IDNA Mappings
     *
     * Each value contains 2 numbers hidden inside:
     *     [1bits = use_second_table] + [15bits = pos]
     *
     * Table size:
     *   - in bits:       67312
     *   - in bytes:      8414 B
     *   - in KibiBytes:  8.22 KiB
     */
    static constexpr std::array<idna_index, 4207ULL> idna_mapping_ref{
      iblt | 0x0U, iblt | 0x0U, 0x0U, iblt | 0x0U, iblt | 0x20U, 0x20U, 0x40U, iblt | 0x0U, 0x60U, 0x80U,
      0x9FU, 0xBFU, 0xDFU, 0xFFU, 0x11CU, 0x13CU, 0x15CU, 0x17CU, 0x19BU, iblt | 0x0U, iblt | 0x0U, 0x1ABU,
      0x1C4U, 0x1E4U, iblt | 0x0U, iblt | 0x0U, 0x204U, 0x214U, 0x234U, 0x254U, 0x265U, 0x285U, 0x2A5U,
      0x2C5U, iblt | 0x0U, 0x2E5U, 0x305U, 0x325U, 0x345U, 0x365U, 0x385U, 0x3A5U, 0x3C5U, iblt | 0x0U,
      0x3DEU, iblt | 0x0U, iblt | 0x40U, iblt | 0x55U, iblt | 0x6FU, iblt | 0x0U, iblt | 0x0U, 0x3EFU,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x8CU, iblt | 0x0U, iblt | 0xAAU, iblt | 0x0U, iblt | 0xADU,
      iblt | 0x0U, iblt | 0x0U, iblt | 0xEU, iblt | 0x0U, iblt | 0xBAU, iblt | 0x0U, iblt | 0xC7U,
      iblt | 0xE7U, iblt | 0x107U, iblt | 0x118U, iblt | 0x0U, iblt | 0x0U, iblt | 0x89U, iblt | 0x0U,
      iblt | 0x0U, 0x408U, iblt | 0x0U, iblt | 0x134U, iblt | 0x14BU, 0x428U, iblt | 0x161U, iblt | 0x180U,
      0x448U, 0x466U, iblt | 0x1A0U, iblt | 0x1BFU, iblt | 0x1D6U, iblt | 0x1F2U, iblt | 0x212U,
      iblt | 0x232U, iblt | 0x1D6U, 0x486U, iblt | 0x24EU, iblt | 0x26CU, iblt | 0x28CU, iblt | 0x2AAU,
      iblt | 0x2C4U, iblt | 0x2E4U, iblt | 0x2FBU, iblt | 0x317U, iblt | 0x337U, iblt | 0x2E4U, iblt | 0x34EU,
      iblt | 0x36AU, iblt | 0x38AU, iblt | 0x2E4U, iblt | 0x0U, iblt | 0x3AAU, iblt | 0xB4U, iblt | 0x3CAU,
      iblt | 0x3E4U, iblt | 0x404U, iblt | 0x424U, iblt | 0x443U, 0x4A5U, iblt | 0x4U, iblt | 0x20U,
      iblt | 0x463U, 0x4C4U, 0x4E4U, iblt | 0x20U, 0x502U, iblt | 0x0U, 0x51FU, 0x53CU, 0x55BU, 0x579U,
      iblt | 0x476U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x599U,
      0x5B9U, 0x5C9U, iblt | 0x0U, iblt | 0x0U, 0x5E6U, 0x605U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x496U, iblt | 0x0U, iblt | 0x4B6U, iblt | 0x4C6U,
      iblt | 0x4E4U, iblt | 0x0U, iblt | 0x4FCU, iblt | 0x0U, iblt | 0xBAU, iblt | 0x3U, iblt | 0x6U,
      iblt | 0x0U, iblt | 0x0U, 0x60FU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x513U, iblt | 0x0U, iblt | 0x0U, iblt | 0x7U, iblt | 0x51AU, iblt | 0x9U, iblt | 0xCU,
      iblt | 0x539U, iblt | 0x0U, 0x62FU, iblt | 0x2U, iblt | 0x559U, 0x645U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x7U, iblt | 0x0U, iblt | 0x107U, iblt | 0x0U, iblt | 0xAU, iblt | 0x1U, iblt | 0x579U,
      iblt | 0x599U, iblt | 0x5ABU, iblt | 0x0U, iblt | 0x5CBU, iblt | 0x5E1U, iblt | 0x0U, iblt | 0x59DU,
      iblt | 0x0U, iblt | 0x1U, iblt | 0x5FFU, iblt | 0x559U, iblt | 0xAAU, iblt | 0x11U, iblt | 0x20U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x7EU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x61EU, iblt | 0x0U, iblt | 0x63AU, iblt | 0x5F2U, iblt | 0x0U, 0x665U, 0x685U, iblt | 0x40U,
      iblt | 0x5U, iblt | 0x0U, 0x6A5U, 0x6C5U, 0x6E5U, 0x6FEU, 0x71EU, iblt | 0x0U, iblt | 0x0U, 0x73EU,
      0x75EU, 0x77EU, 0x79EU, 0x7BEU, 0x7DEU, 0x7FEU, 0x81EU, 0x83DU, 0x85DU, 0x87DU, 0x89DU, 0x8BDU, 0x8DDU,
      0x8FDU, 0x91DU, 0x93DU, 0x959U, 0x978U, 0x998U, 0x9B8U, 0x9D8U, iblt | 0x659U, iblt | 0xFU, 0x9F8U,
      0xA18U, 0xA38U, 0xA58U, 0xA78U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0xA8CU, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0xAA3U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x16U,
      iblt | 0x15U, 0xAC3U, 0xAE3U, 0xB03U, 0xB23U, 0xB43U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0xB57U, iblt | 0x0U, iblt | 0x0U,
      0xB64U, iblt | 0x0U, iblt | 0x0U, 0xB7BU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0xC1U, iblt | 0x75U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0xB9BU, 0xBBBU, iblt | 0x0U, 0xBDBU,
      0xBFBU, 0xC1BU, 0xC3BU, 0xC5BU, iblt | 0x0U, iblt | 0x673U, iblt | 0x0U, 0xC74U, iblt | 0x9U,
      iblt | 0x68CU, iblt | 0x68CU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x2U, iblt | 0x20U, 0xC93U,
      iblt | 0x0U, iblt | 0x0U, 0xCB3U, 0xCD3U, 0xCF3U, 0xD13U, 0xD33U, 0xD53U, 0xD73U, 0xD93U, iblt | 0x20U,
      0xDB3U, 0xDBDU, iblt | 0x443U, iblt | 0x0U, 0xDD8U, iblt | 0x0U, iblt | 0x0U, 0xDF8U, iblt | 0x43FU,
      0xE18U, 0xE38U, 0xE58U, 0xE78U, iblt | 0x0U, iblt | 0x0U, iblt | 0x6A5U, 0xE98U, 0xEB8U, 0xED8U, 0xEF8U,
      0xF18U, 0xF38U, 0xF58U, 0xF78U, 0xF98U, 0xFB8U, 0xFD8U, 0xFF8U, 0x1018U, 0x1038U, 0x1058U, 0x1078U,
      // End of Section #1 [0, 416) containing 416 values (0.81 KiB). Skipping 899 values.

      iblt | 0x0U, iblt | 0x3D4U, iblt | 0x0U, iblt | 0x6BEU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x14U,
      0x1098U, 0x10B8U, 0x10D8U, iblt | 0x0U, iblt | 0x0U, iblt | 0x8U, iblt | 0x0U, 0x10F6U, 0x1116U,
      0x1136U, 0x1156U, 0x1176U, 0x1196U, 0x11B3U, iblt | 0x0U, iblt | 0x6D1U, iblt | 0x0U, iblt | 0x8U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x6E5U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x705U,
      iblt | 0x3U, iblt | 0x0U, iblt | 0x0U, iblt | 0x724U, iblt | 0x1U, iblt | 0x0U, iblt | 0x9U,
      iblt | 0x742U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x75FU, iblt | 0x9U, iblt | 0x779U,
      iblt | 0x799U, 0x11CDU, 0x11EDU, 0x120DU, 0x122DU, iblt | 0x0U, iblt | 0x7ABU, iblt | 0x0U, iblt | 0x0U,
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
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x7C1U, iblt | 0x7DAU, iblt | 0x4U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, 0x124DU, 0x126DU, 0x128DU, 0x12ADU, 0x12CDU,
      0x12EDU, 0x130DU, 0x132DU, 0x134DU, 0x136DU, 0x138DU, 0x13ADU, 0x13CDU, 0x13EDU, 0x140DU, iblt | 0x20U,
      0x142DU, 0x144DU, 0x146DU, 0x148BU, 0x14A9U, 0x14C9U, 0x14E6U, 0x1506U, 0x1526U, 0x1546U, 0x1566U,
      0x1586U, 0x15A6U, 0x15C6U, 0x15E6U, 0x1606U, 0x1626U, 0x1646U, 0x1664U, 0x1683U, 0x16A3U, 0x16C3U,
      0x16E3U, 0x16F3U, 0x1713U, 0x1733U, 0x1753U, 0x1773U, 0x1793U, 0x17B2U, 0x17D1U, 0x17F0U, 0x1810U,
      0x1830U, 0x1850U, 0x1870U, 0x1890U, 0x18B0U, 0x18CFU, 0x18EFU, iblt | 0x7FU, iblt | 0x7F3U,
      iblt | 0x812U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x5U, iblt | 0x82DU,
      iblt | 0x839U, iblt | 0x0U, iblt | 0x0U, iblt | 0x474U, iblt | 0x1FU, iblt | 0x859U, iblt | 0x2U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x3U, iblt | 0x0U, iblt | 0xFU,
      iblt | 0x4U, iblt | 0x0U, iblt | 0x875U, iblt | 0x107U, iblt | 0x5U, iblt | 0x445U, iblt | 0x0U,
      iblt | 0x891U, iblt | 0x20U, 0x190FU, 0x192FU, iblt | 0x0U, iblt | 0x0U, iblt | 0x2U, 0x1945U, 0x1965U,
      iblt | 0x4U, iblt | 0x0U, iblt | 0x40U, iblt | 0x0U, 0x1981U, 0x19A1U, iblt | 0x8B1U, iblt | 0x0U,
      iblt | 0xCU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x9U, iblt | 0xAU, iblt | 0x18U, 0x19C0U, 0x19E0U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x8D1U, iblt | 0x8DBU, iblt | 0x75U, iblt | 0x0U, iblt | 0x1U, iblt | 0x8FBU,
      iblt | 0x20U, iblt | 0x91BU, iblt | 0x936U, iblt | 0x955U, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U,
      iblt | 0x974U, iblt | 0xC5U, iblt | 0x0U, iblt | 0x990U, iblt | 0x9A9U, iblt | 0x9C8U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x20U, iblt | 0x0U, iblt | 0x9E8U, iblt | 0x0U, iblt | 0x837U, iblt | 0xBFU,
      iblt | 0x2CCU, iblt | 0xA08U, iblt | 0xA25U, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x17U, iblt | 0x20U, 0x1A00U, 0x1A20U, iblt | 0x0U, iblt | 0xA45U, iblt | 0x0U, iblt | 0xA5FU,
      0x1A40U, 0x1A60U, iblt | 0xA73U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x1U, iblt | 0x0U, iblt | 0xA93U, iblt | 0xAB1U, iblt | 0x24U, iblt | 0x0U,
      iblt | 0x40U, iblt | 0x6U, iblt | 0x859U, iblt | 0x16U, iblt | 0x859U, iblt | 0x14U, iblt | 0x9U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x7D3U, iblt | 0x51AU, iblt | 0x0U, iblt | 0x8CU, iblt | 0xAD1U,
      iblt | 0xAE8U, iblt | 0x0U, iblt | 0x76U, iblt | 0x40U, iblt | 0x9U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x704U, iblt | 0x79U, iblt | 0x0U, iblt | 0x1EU, iblt | 0x20U, iblt | 0xB08U,
      iblt | 0x196U, iblt | 0x0U, iblt | 0xB27U, iblt | 0x134U, iblt | 0xB47U, iblt | 0xB62U, iblt | 0xB7FU,
      iblt | 0xB9FU, iblt | 0x75U, iblt | 0xBBEU, iblt | 0xBD4U, iblt | 0x0U, iblt | 0x0U, iblt | 0x8DU,
      iblt | 0x1EU, iblt | 0x0U, iblt | 0x0U, iblt | 0xA5FU, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0xBFU, iblt | 0x2U, iblt | 0x20U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0xBF4U, iblt | 0x13U, iblt | 0x0U, iblt | 0x6U, iblt | 0x196U, iblt | 0x1CU,
      iblt | 0xBAU, iblt | 0x5CBU, iblt | 0x19U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x0U, iblt | 0x4U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, 0x1A80U, iblt | 0x0U,
      iblt | 0xC14U, iblt | 0xC33U, iblt | 0xC4BU, iblt | 0xC66U, iblt | 0x20U, iblt | 0x20U, iblt | 0xB0U,
      iblt | 0xBDU, iblt | 0x1BU, iblt | 0x0U, iblt | 0x0U, iblt | 0x40U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0xAD1U, iblt | 0x0U, iblt | 0x7U, iblt | 0x16U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0xC7EU, iblt | 0x82U, iblt | 0x92U, iblt | 0x6A5U,
      iblt | 0x3D4U, iblt | 0xC5U, iblt | 0xC9EU, iblt | 0x20U, iblt | 0x20U, iblt | 0xC40U, iblt | 0xCBEU,
      iblt | 0xA5FU, iblt | 0xC41U, iblt | 0xCDDU, iblt | 0x16U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x7U,
      iblt | 0x7AU, iblt | 0x937U, iblt | 0x5U, iblt | 0x20U, iblt | 0x20U, iblt | 0xCF6U, iblt | 0x0U,
      iblt | 0xD16U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x6U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xD35U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1CU, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x859U, iblt | 0x0U, iblt | 0x0U, iblt | 0xDU,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x10U, iblt | 0xAU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
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
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x5U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x19U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U,
      iblt | 0x6U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x7U, iblt | 0x1U, iblt | 0x7D7U, iblt | 0x0U, iblt | 0x1U, iblt | 0x196U, iblt | 0xD55U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0xD65U, iblt | 0xD7DU, iblt | 0x10U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0x6U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, 0x1AA0U, iblt | 0x0U, iblt | 0x5U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x7D6U, iblt | 0x0U,
      iblt | 0xD9AU, iblt | 0x20U, iblt | 0x20U, iblt | 0xDB5U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
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
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x8U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xAU, iblt | 0x21U, iblt | 0x17U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0xDC7U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0xDE7U, iblt | 0xDFAU, iblt | 0xE16U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x4U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xE2BU, iblt | 0xE3FU,
      0x1AC0U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, 0x1AE0U, 0x1B00U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0xCU, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0xAAU,
      iblt | 0x6BEU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x1CU, iblt | 0x20U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xAU, iblt | 0x0U,
      iblt | 0xB1U, 0x1B20U, 0x1B40U, iblt | 0x0U, 0x1B5BU, 0x1B7BU, iblt | 0x15U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x1AU, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0xCU, iblt | 0xCU, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x9U, iblt | 0x7U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, 0xB19U,
      0xB1FU, 0x1B9BU, 0xB2BU, 0x1BBBU, 0x1BDBU, 0x1BFBU, 0xB29U, 0x1C1BU, 0x1C3BU, 0x1C5BU, 0xB27U, 0xB2DU,
      0xB19U, 0xB1FU, 0xB25U, 0xB2BU, 0x1C7BU, 0xB1DU, 0xB23U, 0xB29U, 0x1C9BU, 0x1CBBU, 0x1CDBU, 0x1CC1U,
      0x1CA7U, 0x1CC7U, 0x1CADU, 0x1CCDU, 0x1CB3U, 0x1CFBU, 0x1D11U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0xE5BU, iblt | 0xE75U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x1U, iblt | 0xE90U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0xEB0U, 0x1D31U, 0x1D51U, 0x1D71U, iblt | 0xDEAU,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0xECBU, iblt | 0xEDFU, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x859U, iblt | 0x11U, iblt | 0x0U, iblt | 0x955U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x859U, iblt | 0x6U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x859U,
      iblt | 0xEFFU, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0xF1EU, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0xF38U, iblt | 0x20U, 0x1D91U, 0x1DB1U, iblt | 0xF58U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0xF78U, iblt | 0x0U, iblt | 0xBU, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x443U, iblt | 0x2U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, 0x1DD1U, 0x1DF1U, 0x1E0FU, 0x1E2FU, 0x1E4FU, 0x1E6EU, iblt | 0x20U, iblt | 0xF98U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x0U, iblt | 0x5CBU, iblt | 0x0U, iblt | 0x0U, iblt | 0xCU, iblt | 0xC6U,
      iblt | 0xD6U, iblt | 0xAU, 0x1E8DU, 0x1EADU, 0x1ECDU, 0x1EE3U, 0x1EF3U, iblt | 0x12U, iblt | 0x20U,
      iblt | 0x43EU, 0x1F13U, 0x1F33U, 0x1F53U, iblt | 0x1AU, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x974U, iblt | 0xFB8U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xF03U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x6U, iblt | 0xFC9U, iblt | 0x5CBU, iblt | 0x0U, iblt | 0xA5FU, iblt | 0x0U,
      iblt | 0x40U, iblt | 0xFE9U, iblt | 0x1EU, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0xCU,
      iblt | 0xEBBU, iblt | 0x965U, iblt | 0x0U, iblt | 0xFFEU, iblt | 0x101DU, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x78U, iblt | 0x0U, iblt | 0x0U, 0x1F73U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U,
      // End of Section #2 [1315, 4096) containing 2781 values (5.43 KiB). Skipping 1334 values.

      iblt | 0x0U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
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
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x6U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x2U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
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
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x1034U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
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
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x659U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x2U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      0x1F93U, 0x1FB3U, 0x1FD3U, 0x1FF3U, 0x2013U, 0x2033U, 0x2053U, 0x2073U, 0x2093U, 0x20B3U, 0x20D3U,
      0x20F3U, 0x2113U, 0x2133U, 0x2153U, 0x2173U, 0x2193U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U, iblt | 0x20U,
      iblt | 0x20U, iblt | 0x20U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U,
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
      iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x0U, iblt | 0x107U, iblt | 0x0U,
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
      iblt | 0x0U, iblt | 0x10U,
      // End of Section #3 [5430, 6430) containing 1000 values (1.95 KiB). Skipping 22249 values.

      iblt | 0x20U, 0x21B3U, 0x21B3U, 0x21B3U, 0x21B3U, 0x21B3U, 0x21B3U, 0x21B3U, 0x21C3U,
      // End of Section #4 [28679, 28688) containing 9 values (0.02 KiB). Skipping 6127 values.

      iblt | 0x20U,
      // End of Section #5 [34815, 34816) containing 1 values (0.00 KiB).

      // done
    };




    /**
     * IDNA Values Table
     *
     * Block values of the IDNA Mappings; the values of this table points to the idna_mappings table if it's
     * not VALID or DISALLOWED specifically specified.
     *
     * Table size:
     *   - in bits:       138800
     *   - in bytes:      17350 B
     *   - in KibiBytes:  16.94 KiB
     */
    static constexpr std::array<std::uint16_t, 8675ULL> idna_mapping_blocks{

      // Start of 0x0-0x60, 0xe0, 0x260-0x280, 0x300-0x320, 0x440, 0x560, 0x5a0, 0x620-0x640, 0x680-0x6a0,
      // 0x6e0, 0x720, 0x760-0x780, 0x7c0, 0x800, 0x8a0-0x8c0, 0x900-0x920, 0x960, 0xd20, 0xf20,
      // 0x1000-0x1080, 0x1100-0x1120, 0x1180-0x1220, 0x1260, 0x12e0, 0x1320, 0x13a0-0x13c0, 0x1400-0x1660,
      // 0x16a0-0x16c0, 0x1780, 0x1820-0x1840, 0x1880, 0x18c0, 0x1980, 0x19e0, 0x1a20, 0x1b00-0x1b20,
      // 0x1b60-0x1bc0, 0x1c00, 0x1c60, 0x1d00, 0x1dc0-0x1de0, 0x21a0-0x2200, 0x2240-0x2300, 0x2340-0x2400,
      // 0x2500-0x29e0, 0x2a20-0x2a40, 0x2a80-0x2aa0, 0x2ae0-0x2b40, 0x2ba0-0x2be0, 0x2c40, 0x2d00, 0x2d40,
      // 0x2de0-0x2e20, 0x2ea0-0x2ec0, 0x3060, 0x30a0-0x30c0, 0x31a0-0x31c0, 0x3400-0xa460, 0xa4a0,
      // 0xa4e0-0xa600, 0xa6a0-0xa6c0, 0xa700, 0xa800, 0xa840, 0xa880-0xa8a0, 0xa8e0-0xa920, 0xa980-0xa9a0,
      // 0xaa00, 0xaa60-0xaaa0, 0xabc0, 0xac00-0xd780, 0x10080-0x100c0, 0x10140-0x10160, 0x102a0, 0x10300,
      // 0x103a0, 0x10440-0x10460, 0x10500, 0x10540, 0x105c0, 0x10600-0x10700, 0x10860, 0x10980, 0x109e0,
      // 0x10a60-0x10a80, 0x10ac0, 0x10b00, 0x10c00-0x10c20, 0x10cc0, 0x10d00, 0x10e80, 0x10f00,
      // 0x11000-0x11020, 0x11080, 0x11100, 0x11180-0x111c0, 0x11220, 0x112c0, 0x11400-0x11420,
      // 0x11480-0x114a0, 0x11580, 0x11600-0x11620, 0x11680, 0x11800, 0x118c0, 0x11a00-0x11a20,
      // 0x11a60-0x11a80, 0x11ac0, 0x11bc0, 0x11fc0, 0x12000-0x12360, 0x12400-0x12440, 0x12480-0x12520,
      // 0x12fa0-0x12fc0, 0x13000-0x13400, 0x13460-0x143c0, 0x14400-0x14620, 0x16100, 0x16800-0x16a00,
      // 0x16a80, 0x16b00-0x16b20, 0x16d40, 0x16e60, 0x16f00-0x16f20, 0x16f60, 0x17000-0x187c0,
      // 0x18800-0x18ca0, 0x1b000-0x1b100, 0x1b180-0x1b2c0, 0x1bc00-0x1bc40, 0x1cc00-0x1cca0, 0x1cd00-0x1ce80,
      // 0x1cf00, 0x1cf60-0x1cfa0, 0x1d000-0x1d0c0, 0x1d100, 0x1d180, 0x1d200-0x1d220, 0x1d300-0x1d320,
      // 0x1d800-0x1da60, 0x1e100, 0x1e2c0, 0x1e800-0x1e8a0, 0x1ec80, 0x1f000, 0x1f040-0x1f060,
      // 0x1f300-0x1f6a0, 0x1f700-0x1f740, 0x1f780-0x1f7a0, 0x1f820, 0x1f860, 0x1f900-0x1fa20, 0x1faa0,
      // 0x1fb00-0x1fb60, 0x1fba0-0x1fbc0, 0x20000-0x2a6c0, 0x2a700-0x2b700, 0x2b740-0x2b7e0, 0x2b820-0x2ce80,
      // 0x2cec0-0x2ebc0, 0x2ec00-0x2ee20, 0x30000-0x31320, 0x31360-0x32380, 0x110000:
      valid,

      // Start of 0x1900, 0x1a40, 0xa9e0, 0x10880, 0x10e60, 0x16a40, 0x16aa0, 0x1df00:
      0,

      // Start of 0x17c0, 0x2e40, 0x101e0, 0x10480, 0x115c0, 0x1ed20, 0x2b800, 0x2ee40:
      2,

      // Start of 0x1360, 0xa960, 0x10280:
      4,

      // Start of 0xe40, 0xd7e0, 0x102e0, 0x104e0, 0x11820, 0x1b2e0:
      6,

      // Start of 0x1ce0, 0x100e0, 0x10360, 0x11f40, 0x143e0, 0x16e80:
      8,

      // Start of 0x1380, 0x10f40, 0x116a0, 0x12380, 0x16120, 0x16d60, 0x1e4e0, 0x1f7c0, 0x2b720:
      10,

      // Start of 0x16e0, 0x1860, 0x11ae0, 0x11ee0, 0x16a20, 0x1d360:
      12,

      // Start of 0xa6e0, 0xa860, 0x187e0:
      14,

      // Start of 0x1720, 0x2d80, 0xaa20, 0xaae0, 0x10720, 0x10fe0, 0x11160, 0x1d340:
      16,

      // Start of 0x18e0, 0x10740, 0x13440, 0x18cc0, 0x1d0e0, 0x1f0e0:
      18,

      // Start of 0x1eca0:
      20,

      // Start of 0x1740, 0x105e0, 0x1cea0, 0x1d2c0-0x1d2e0, 0x1f080, 0x1fa40:
      22,

      // Start of 0x12fe0:
      24,

      // Start of 0x7a0:
      26,

      // Start of 0x20e0, 0x102c0:
      28,

      // Start of 0x13420, 0x16b80, 0x323a0:
      30,

      // Start of 0x1ac0, 0x1e2a0:
      32,

      // Start of 0x1f1a0:
      34,

      // Start of 0x11660:
      36,

      // Start of 0xa620, 0x10fc0:
      38,

      // Start of 0x2440, 0x1d1e0:
      40,

      // Start of 0x2420, 0x10f80, 0x11b00, 0x11da0:
      42,

      // Start of 0x10c40, 0x18d00:
      44,

      // Start of 0x10760:
      46,

      // Start of 0x11740, 0x14640:
      48,

      // Start of 0x1d240, 0x1f260:
      50,

      // Start of 0x119e0:
      valid,

      // Start of 0x116e0, 0x12540, 0x1cfc0:
      valid, valid,

      // Start of 0x11240, 0x11460, 0x1f8c0:
      valid,

      // Start of 0x101a0:
      valid,

      // Start of 0x80-0xa0, 0xe60, 0xee0, 0xfe0, 0x1ae0, 0x2e60, 0x2fe0, 0xd800-0xf8e0, 0xfae0, 0x10060,
      // 0x10200-0x10260, 0x103e0, 0x107c0-0x107e0, 0x108c0, 0x10940-0x10960, 0x10aa0, 0x10bc0-0x10be0,
      // 0x10c60, 0x10da0-0x10e40, 0x11260, 0x114e0-0x11560, 0x115e0, 0x11760-0x117e0, 0x11840-0x11880,
      // 0x11960-0x11980, 0x11b20-0x11ba0, 0x11cc0-0x11ce0, 0x11dc0-0x11ec0, 0x11f60-0x11f80, 0x123a0-0x123e0,
      // 0x12560-0x12f60, 0x14660-0x160e0, 0x16140-0x167e0, 0x16ba0-0x16d20, 0x16d80-0x16e20, 0x16ea0-0x16ee0,
      // 0x16fa0-0x16fc0, 0x18d20-0x1afc0, 0x1b300-0x1bbe0, 0x1bcc0-0x1cbe0, 0x1cec0-0x1cee0, 0x1cfe0,
      // 0x1d260-0x1d2a0, 0x1d380-0x1d3e0, 0x1dac0-0x1dee0, 0x1df40-0x1dfe0, 0x1e0a0-0x1e0e0, 0x1e160-0x1e260,
      // 0x1e300-0x1e4a0, 0x1e500-0x1e5a0, 0x1e600-0x1e7c0, 0x1e8e0, 0x1e960-0x1ec40, 0x1ecc0-0x1ece0,
      // 0x1ed40-0x1ede0, 0x1eec0, 0x1ef00-0x1efe0, 0x1f1c0, 0x1f280-0x1f2e0, 0x1f8e0, 0x1fc00-0x1ffe0,
      // 0x2a6e0, 0x2ee60-0x2f7e0, 0x2fa20-0x2ffe0, 0x323c0-0xe00e0, 0xe0200-0x10ffe0:
      52,

      // Start of 0x18ce0:
      valid, valid, valid,

      // Start of 0x10ee0:
      valid, valid, valid, valid, 54, valid, 0, valid, valid, 58, valid, 59, valid, valid, 63, 65, 67, 71,
      valid, valid, 74, 78, 28, valid, 80, 86, 92, valid,

      // Start of 0xc0, 0x5c0, 0x1cc0, 0x10520, 0x10f20, 0x11140, 0x11a40, 0x1f880:
      98, 101, 104, 107, 110, 113, 116, 119, 122, 125, 128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158,

      // Start of 0x5e0:
      161, 164, valid, 167, 170, 173, 176, 179, 182, 185, valid,

      // Start of 0x100:
      188, valid, 191, valid, 194, valid, 197, valid, 200, valid, 203, valid, 206, valid, 209,

      // Start of 0x600:
      valid, 212, valid, 215, valid, 218,

      // Start of 0x2b80, 0x10840, 0x113a0:
      valid,

      // Start of 0x11120:
      221, valid,

      // Start of 0x1fb80:
      224,

      // Start of 0x11200:
      valid,

      // Start of 0x11f00:
      227, valid, 230, valid,

      // Start of 0x1b40:
      233,

      // Start of 0x10000:
      valid,

      // Start of 0x120:
      236, valid,

      // Start of 0x11c00:
      239, valid, 242, valid, 245, valid, 248,

      // Start of 0x8e0:
      valid, 251, valid,

      // Start of 0x6c0, 0x110a0:
      254,

      // Start of 0x11440:
      valid, 257, valid, 260, valid,

      // Start of 0x11c20:
      264, 264, 267, valid, 270, valid, valid, 273, valid, 276, valid, 279, valid,

      // Start of 0x140:
      282, 286, valid, 289, valid, 292, valid, 295, valid, 298, 302,

      // Start of 0x700, 0x1aa0, 0x1cf20:
      valid, 305, valid,

      // Start of 0x740:
      308, valid, 311,

      // Start of 0x119a0:
      valid,

      // Start of 0x1d120:
      314, valid, 317,

      // Start of 0xd60:
      valid, 320, valid, 323, valid, 326,

      // Start of 0x7e0, 0x1340, 0x11700:
      valid, 329, valid,

      // Start of 0x119c0:
      332, valid,

      // Start of 0x160, 0x10b40, 0x115a0:
      335, valid,

      // Start of 0x2b60:
      338, valid, 341, valid,

      // Start of 0x109c0, 0x11c80:
      344,

      // Start of 0x1f0a0:
      valid,

      // Start of 0x820:
      347, valid, 350, valid, 353, valid, 356, valid, 359, valid, 362, valid, 365, valid, 368,

      // Start of 0x1f0c0:
      valid, 371, 374, valid, 377, valid, 380, valid, 36,

      // Start of 0x180:
      valid, 383, 386, valid, 389, valid, 392, 395,

      // Start of 0x840:
      valid, 398, 401, 404, valid, valid, 407, 410, 413, 416, valid, 419, 422, valid, 425, 428, 431, valid,
      valid, valid, 434, 437, valid, 440,

      // Start of 0x1a0:
      443, valid, 446, valid, 449, valid, 452, 455,

      // Start of 0x860, 0x18a0, 0x10340, 0x31340:
      valid, 458, valid, valid, 461, valid, 464, 467, valid, 470, 473, 476, valid, 479, valid, 482, 485,

      // Start of 0x880:
      valid, valid, valid, 488,

      // Start of 0x1c0:
      valid, valid, valid, valid, 491, 491, 491, 495, 495, 495, 498, 498, 498, 501, valid, 504, valid, 507,
      valid, 510, valid, 513, valid, 516,

      // Start of 0x980, 0x11300:
      valid, 519, valid, 522, valid, valid, 525, valid,

      // Start of 0x1e0:
      528, valid, 531, valid, 534, valid, 537, valid, 540, valid, 543, valid, 546, valid, 549,

      // Start of 0x9a0:
      valid, valid, 552, 552, 552, 555, valid, 558, 561, 564, valid, 567, valid, 570, valid, 573, valid,

      // Start of 0x200:
      576, valid, 579, valid, 582,

      // Start of 0x9e0:
      valid, 585, valid, 588, valid, 591, valid, 594, valid, 597, valid, 600, valid, 603, valid, 606, valid,
      609, valid, 612, valid, 615, valid, 618, valid, 621, valid,

      // Start of 0x220:
      624, valid, 627, valid,

      // Start of 0xa00:
      630, valid, 633, valid, 636, valid, 639, valid, 642, valid, 645, valid, 648, valid, 651, valid, valid,
      valid, valid, valid, valid, valid,

      // Start of 0x112a0, 0x116c0, 0x16ac0:
      654, 658, valid, 661, 664,

      // Start of 0x240:
      valid, 668, valid, 671, 674,

      // Start of 0xa60:
      677, 680, valid, 683, valid, 686, valid, 689, valid, 692, valid,

      // Start of 0x2a0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 14, 695, 18, 34,

      // Start of 0xa80:
      698, 701, 704, 44, 48,

      // Start of 0x2c0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid,

      // Start of 0xaa0, 0xb20:
      valid, valid, valid, valid, valid, valid, 707, 711, 715, 719, 723, 727, valid, valid,

      // Start of 0x2e0:
      422, 22, 36, 46, 731, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0xac0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid,

      // Start of 0x340:
      734, 737, valid, 740, 743, 748, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0xae0:
      valid, 58,

      // Start of 0x360:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 751, valid, 754, valid, 757, valid, 760, valid, disallowed, disallowed, 763, valid, valid, valid,

      // Start of 0xb00:
      767, 769,

      // Start of 0x380:
      disallowed, disallowed, disallowed, disallowed, 67, 772, 778, 781, 784, 787, 790, disallowed, 793,
      disallowed, 796, 799, valid, 802, 805, 808, 811, 814, 817, 820, 823, 748,

      // Start of 0xb60:
      826, 829, 71, 832, 835, 838,

      // Start of 0x3a0:
      841, 844, disallowed, 847, 850, 853, 856, 859, 862, 865, 868, 871, valid, valid, valid, valid, valid,

      // Start of 0x3c0:
      valid, valid, valid, valid, valid, valid, valid,

      // Start of 0xb80:
      valid, valid, valid, valid, valid, valid, valid, valid, 874, 805, 823, 853, 796, 871, 856, 841, valid,
      877, valid, 880, valid, 883, valid, 886, valid,

      // Start of 0x3e0:
      889, valid, 892, valid, 895, valid, 898,

      // Start of 0xba0:
      valid, 901, valid, 904, valid, 907, valid, 910, valid, 826, 844, 847, valid, 823, 814, valid, 913,
      valid, 847, 916, valid, valid, 919, 922, 925,

      // Start of 0x400:
      928, 931, 934, 937, 940,

      // Start of 0xbc0:
      943, 946, 949, 952, 955, 958, 961, 964, 967, 970, 973, 976, 979, 982, 985, 988, 991, 994, 997, 1000,
      1003, 1006, 1009, 1012, 1015, 1018,

      // Start of 0xbe0:
      1021,

      // Start of 0x420:
      1024, 1027, 1030, 1033, 1036, 1039, 1042,

      // Start of 0x10b60:
      1045, 1048, 1051, 1054, 1057, 1060, 1063, 1066, 1069, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0xc00, 0xc80, 0xd00:
      valid,

      // Start of 0x460:
      1072, valid, 1075, valid, 1078, valid, 1081, valid, 1084, valid, 1087, valid, 1090, valid, 1093, valid,
      1096, valid, 1099, valid, 1102, valid,

      // Start of 0xc20:
      1105, valid, 1108, valid, 1111, valid, 1114, valid, 1117, valid,

      // Start of 0x480:
      1120, valid, valid, valid, valid, valid, valid, valid, valid, valid, 1123, valid, 1126, valid, 1129,
      valid, 1132, valid,

      // Start of 0xc40:
      1135, valid, 1138, valid, 1141, valid, 1144, valid, 1147, valid, 1150, valid, 1153, valid,

      // Start of 0x4a0:
      1156, valid, 1159, valid, 1162, valid, 1165, valid, 1168, valid, 1171, valid, 1174, valid, 1177, valid,
      1180, valid,

      // Start of 0xc60:
      1183, valid, 1186, valid, 1189, valid, 1192, valid, 1195, valid, 1198, valid, 1201, valid,

      // Start of 0x4c0:
      1204, 1207, valid, 1210, valid, 1213, valid, 1216, valid,

      // Start of 0xca0:
      1219, valid, 1222, valid, 1225, valid, valid, 1228, valid, 1231, valid, 1234, valid, 1237, valid, 1240,
      valid, 1243, valid, 1246, valid, 1249, valid,

      // Start of 0x4e0:
      1252, valid, 1255, valid, 1258,

      // Start of 0xcc0:
      valid, 1261, valid, 1264, valid, 1267, valid, 1270, valid, 1273, valid, 1276, valid, 1279, valid, 1282,
      valid, 1285, valid, 1288, valid, 1291, valid, 1294, valid, 1297, valid,

      // Start of 0x500:
      1300, valid, 1303, valid, 1306,

      // Start of 0xce0:
      valid, 1309, valid, 1312, valid, 1315, valid, 1318, valid, 1321, valid, 1324, valid, 1327, valid, 1330,
      valid, 1333, valid, 1336, valid, 1339, valid, 1342, valid, 1345, valid,

      // Start of 0x520:
      1348, valid, 1351, valid, 1354,

      // Start of 0xd40:
      valid, 1357, valid, 1360, valid, 1363, valid, 1366, valid, 1369, valid, disallowed, 1372, 1375, 1378,
      1381, 1384, 1387, 1390, 1393, 1396, 1399, 1402, 1405, 1408, 1411, 1414,

      // Start of 0x540:
      1417, 1420, 1423, 1426, 1429,

      // Start of 0xd80:
      1432, 1435, 1438, 1441, 1444, 1447, 1450, 1453, 1456, 1459,

      // Start of 0xa480, 0x11c60:
      1462, 1465, 1468, 1471, 1474, 1477, 1480, 1483, disallowed, disallowed,

      // Start of 0x580:
      valid, valid, valid, valid, valid, valid,

      // Start of 0xda0:
      valid, 1486, valid, valid, valid, disallowed, disallowed, valid, valid, valid, disallowed,

      // Start of 0x660:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid,

      // Start of 0xdc0:
      1491, 1496, 1501, 1506,

      // Start of 0x940:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 1511, 1518, 1525, 1532,

      // Start of 0xde0:
      1539, 1546, 1553, 1560,

      // Start of 0x9c0:
      valid, valid, valid, valid, valid, disallowed, disallowed, valid, valid, disallowed, disallowed, valid,
      valid, valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x1f1e0:
      disallowed,

      // Start of 0x3100:
      valid, disallowed, disallowed, disallowed,

      // Start of 0xe00, 0x3040, 0x1ed00:
      disallowed, 1567,

      // Start of 0x10380:
      1574, disallowed, 1581,

      // Start of 0xa20:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, disallowed, valid, valid, valid, valid,
      valid, valid, valid, disallowed, valid, 1588, disallowed, valid, 1595, disallowed, valid, valid,
      disallowed,

      // Start of 0xe80:
      disallowed, valid, disallowed,

      // Start of 0xa40:
      valid, valid, valid, disallowed, disallowed, disallowed, disallowed, valid, valid, disallowed,
      disallowed, valid, valid, valid,

      // Start of 0x10180:
      disallowed, disallowed,

      // Start of 0xfc0:
      disallowed, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      1602, 1609, 1616, valid, disallowed, 1623, disallowed,

      // Start of 0xb40:
      valid, valid, valid, valid, valid, disallowed, disallowed, valid, valid, disallowed, disallowed, valid,
      valid, valid, disallowed, disallowed,

      // Start of 0x1240:
      disallowed, disallowed, disallowed, disallowed, disallowed, valid, valid, valid, disallowed, disallowed,
      disallowed, disallowed, 1630, 1637, disallowed,

      // Start of 0xe20:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x1280:
      valid, valid, 1644, valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed, disallowed,
      disallowed,

      // Start of 0xea0:
      valid, valid,

      // Start of 0x12a0:
      valid, valid, disallowed, valid, disallowed, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, 1651, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      disallowed, disallowed,

      // Start of 0xec0, 0x12c0:
      valid, valid, valid, valid, valid, disallowed, valid, disallowed, valid, valid, valid, valid, valid,
      valid, valid, disallowed, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1300:
      valid, valid, disallowed, disallowed, 1658, 1665,

      // Start of 0xf00:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 1672, valid, valid,
      valid, valid,

      // Start of 0x1680:
      valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1700, 0x11060:
      valid, valid, valid, valid, valid,

      // Start of 0xf40:
      valid, valid, valid, 1676, valid, valid, valid, valid, disallowed, valid, valid, valid, valid, 1683,
      valid, valid, valid, valid, 1690, valid, valid, valid, valid, 1697, valid, valid,

      // Start of 0x1760:
      valid, valid, 1704,

      // Start of 0xf60:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 1711, valid, valid, valid, disallowed,
      disallowed, disallowed, disallowed, valid, valid, 1718, valid, 1725, 1732, 1739, 1749, 1756, valid,
      valid, valid,

      // Start of 0x17e0, 0x1a80:
      valid, valid,

      // Start of 0xf80:
      valid, 1766, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, 1773, valid, valid, valid, valid, disallowed, valid, valid, valid, valid,
      1780,

      // Start of 0xfa0, 0x1920:
      valid, valid, 1787, valid, valid, valid, valid, 1794, valid, valid, valid, valid, 1801, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 1808, valid, valid, valid,
      disallowed, valid, valid,

      // Start of 0x10a0, 0x1940:
      1815, 1819, 1823, 1827,

      // Start of 0x1a00:
      1831, 1835, 1839, 1843, 1847, 1851, 1855, 1859, 1863, 1867, 1871, 1875, 1879, 1883,

      // Start of 0x1960:
      1887, 1891, 1895, 1899, 1903, 1907, 1911, 1915, 1919, 1923, 1927, 1931, 1935, 1939,

      // Start of 0x10c0:
      1943, 1947, 1951, 1955, 1959, 1963, disallowed, 1967, disallowed, disallowed, disallowed, disallowed,
      disallowed, 1971, disallowed, disallowed,

      // Start of 0x10e0:
      valid, valid,

      // Start of 0x19a0, 0x11720, 0x1f020, 0x1f800:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x19c0:
      valid, valid, valid, valid, 1975,

      // Start of 0x1140:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1c40:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1a60:
      valid, valid, valid, valid, valid, valid,

      // Start of 0x1160:
      58, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x13e0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1be0:
      valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed, 1979, 1983, 1987, 1991, 1995,
      1999, disallowed, disallowed,

      // Start of 0x17a0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1c20:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 58, 58,

      // Start of 0x1800:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 58, 58, 58, 58, 58, valid,
      valid, valid, valid,

      // Start of 0x20c0, 0x2ebe0:
      valid, valid, valid, valid, valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed,

      // Start of 0x1c80:
      982, 988, 1018, 1027, 1030, 1030, 1054, 1075, 2003, 2007, valid, disallowed, disallowed, disallowed,

      // Start of 0x2d20:
      disallowed, disallowed, 2011, 2015, 2019, 2023, 2027, 2031, 2035, 2039, 2043, 2047, 2051, 2055, 1975,
      2059, 2063, 2067,

      // Start of 0x1ca0:
      2071, 2075, 2079, 2083, 2087, 2091, 2095,

      // Start of 0x2da0-0x2dc0:
      2099, 2103, 2107, 2111, 2115, 2119, 2123, 2127, 2131, 2135, 2139, 2143, 2147, 2151, 2155, 2159, 2163,
      2167, 2171, 2175, disallowed, disallowed, 2179, 2183, 2187,

      // Start of 0x1d20, 0x31e0, 0x11c40:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 0, 116, 2, valid, 6,
      8, 407, 12, 14, 16, 18, 20, 22,

      // Start of 0xa4c0, 0x1cf40:
      24, 26, valid, 28, 627, 30, 34,

      // Start of 0x1d40:
      38, 40, 44, 0, 2191, 2194, 2197, 2, 6, 8, 410, 413,

      // Start of 0xa820:
      2201, 12, valid, 20, 24, 302, 28, 392, 2204, 2208, 30, 38, 40, 2212, 434, 42, 2216, 805, 808, 811,

      // Start of 0x1d60, 0xa8c0:
      856, 859, 16, 34, 40, 42, 805, 808, 844, 856, 859, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, 1015,

      // Start of 0x1d80:
      valid, valid, valid, valid, valid, valid,

      // Start of 0x111e0:
      valid,

      // Start of 0xa940:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, 2220, 4, 2223, 146, 2201,

      // Start of 0x1da0:
      10, 2226, 2229, 2232, 428, 425,

      // Start of 0xa9c0:
      2235, 2238, 2242, 2245, 2248, 2252, 2255, 2258, 437, 2261, 2264, 440, 2267, 2270, 458, 2273, 674, 470,
      2276, 473, 677, 50, 2280, 2283, 482, 823,

      // Start of 0x1e00:
      2286, valid, 2290, valid,

      // Start of 0xaa40:
      2294, valid, 2298, valid, 2302, valid, 2306, valid, 2310, valid, 2314, valid, 2318, valid, 2322, valid,
      2326, valid, 2330, valid, 2334, valid, 2338, valid, 2342, valid, 2346, valid,

      // Start of 0x1e20:
      2350,

      // Start of 0xaac0:
      valid, 2354, valid, 2358, valid, 2362, valid, 2366, valid, 2370, valid, 2374, valid, 2378, valid, 2382,
      valid, 2386, valid, 2390, valid, 2394, valid, 2398, valid, 2402,

      // Start of 0xab00:
      valid, 2406, valid, 2410, valid,

      // Start of 0x1e40:
      2414, valid, 2418, valid, 2422, valid, 2426, valid, 2430, valid, 2434, valid, 2438, valid, 2442, valid,
      2446, valid, 2450, valid, 2454, valid, 2458, valid, 2462, valid, 2466,

      // Start of 0xab20:
      valid, 2470, valid, 2474, valid,

      // Start of 0x1e60:
      2478, valid, 2482, valid, 2486, valid, 2490, valid, 2494, valid, 2498, valid, 2502,

      // Start of 0xabe0:
      valid, 2506, valid, 2510, valid, 2514, valid, 2518, valid, 2522, valid, 2526, valid, 2530, valid, 2534,
      valid, 2538, valid,

      // Start of 0x1e80:
      2542, valid, 2546,

      // Start of 0xd7a0:
      valid, 2550, valid, 2554, valid, 2558, valid, 2562, valid, 2566, valid, 2570, valid, 2574, valid, 2578,
      valid, 2582,

      // Start of 0x11040:
      valid, valid, valid,

      // Start of 0x16f40:
      valid,

      // Start of 0x16a60:
      valid, 2586, 2478,

      // Start of 0xd7c0:
      valid, valid, 2590, valid,

      // Start of 0x1ea0:
      2593, valid, 2597, valid, 2601, valid, 2605, valid, 2609, valid, 2613, valid, 2617, valid, 2621, valid,
      2625, valid, 2629, valid, 2633,

      // Start of 0x10020:
      valid, 2637, valid, 2641, valid, 2645, valid, 2649, valid, 2653, valid,

      // Start of 0x1ec0:
      2657, valid, 2661, valid, 2665, valid, 2669, valid, 2673, valid, 2677, valid, 2681, valid, 2685, valid,
      2689, valid, 2693, valid,

      // Start of 0x10040:
      2697, valid, 2701, valid, 2705, valid, 2709, valid, 2713, valid, 2717, valid,

      // Start of 0x1ee0:
      2721, valid, 2725, valid, 2729, valid, 2733, valid, 2737, valid, 2741, valid, 2745, valid, 2749,

      // Start of 0x10100:
      valid, 2753, valid, 2757, valid, 2761, valid, 2765, valid, 2769,

      // Start of 0x10b20:
      valid, 2773,

      // Start of 0x10120:
      valid, 2777, valid, 2781,

      // Start of 0x1f00:
      valid, valid, valid, valid, valid, valid, valid, valid, 2785, 2789, 2793, 2797, 2801, 2805, 2809, 2813,
      valid, valid, valid, valid, valid, valid, disallowed, disallowed, 2817, 2821, 2825, 2829,

      // Start of 0x101c0, 0x10f60, 0x10fa0, 0x12f80, 0x1e280, 0x1e4c0, 0x1e5c0:
      2833, 2837, disallowed, disallowed,

      // Start of 0x1f20:
      valid, valid, valid, valid, valid, valid, valid, valid, 2841, 2845, 2849, 2853, 2857, 2861, 2865, 2869,
      valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x10320:
      2873, 2877, 2881, 2885, 2889, 2893, 2897, 2901,

      // Start of 0x1f40:
      valid, valid, valid, valid, valid, valid, disallowed, disallowed, 2905, 2909, 2913, 2917, 2921, 2925,
      disallowed, disallowed, valid, valid, valid, valid,

      // Start of 0x103c0:
      valid, valid, valid, valid, disallowed, 2929, disallowed, 2933, disallowed, 2937, disallowed, 2941,

      // Start of 0x1f60:
      valid, valid, valid, valid, valid, valid, valid, valid, 2945, 2949, 2953, 2957, 2961, 2965, 2969, 2973,
      valid, 778, valid, 784,

      // Start of 0x105a0:
      valid, 787, valid, 790, valid, 793, valid, 796, valid, 799, disallowed, disallowed,

      // Start of 0x1f80:
      2977, 2983, 2989, 2995, 3001, 3007, 3013, 3019, 2977, 2983, 2989, 2995, 3001, 3007, 3013, 3019, 3025,
      3031, 3037, 3043,

      // Start of 0x10800:
      3049, 3055, 3061, 3067, 3025, 3031, 3037, 3043, 3049, 3055,

      // Start of 0x10820:
      3061, 3067,

      // Start of 0x1fa0:
      3073, 3079, 3085, 3091, 3097, 3103, 3109, 3115, 3073, 3079, 3085, 3091, 3097, 3103, 3109, 3115, valid,
      valid, 3121, 3127, 3132, disallowed, valid, 3137, 3143, 3147, 3151, 778, 3127, 3155,

      // Start of 0x108a0:
      748, 3155,

      // Start of 0x1fc0:
      3159, 3163, 3169, 3175, 3180, disallowed, valid, 3185, 3191, 784, 3195, 787, 3175, 3199, 3205, 3211,
      valid, valid, valid, 3217, disallowed, disallowed, valid, valid, 3220, 3224, 3228, 790, disallowed,
      3232,

      // Start of 0x108e0:
      3238, 3244,

      // Start of 0x1fe0:
      valid, valid, valid, 3250, valid, valid, valid, valid, 3253, 3257, 3261, 796, 3265, 3269, 772, 3275,
      disallowed, disallowed, 3277, 3283, 3288, disallowed, valid, 3293, 3299,

      // Start of 0x10900:
      793,

      // Start of 0x11f20:
      3303, 799, 3283, 67, 3307, disallowed,

      // Start of 0x2000:
      52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 58, valid, valid, disallowed, disallowed, valid, 3311,
      valid, valid, valid, valid, valid, 3315,

      // Start of 0x10920, 0x1e2e0:
      valid, valid, valid, valid,

      // Start of 0x2020:
      valid, valid, valid, valid, disallowed, disallowed, disallowed, valid, disallowed, disallowed,
      disallowed, disallowed,

      // Start of 0x1fa80:
      disallowed, disallowed, disallowed, 52, valid, valid, valid, 3319, 3326, valid, 3336, 3343, valid,
      valid, valid,

      // Start of 0x109a0, 0x1f6c0:
      valid, 3353, valid, 3356,

      // Start of 0x2040:
      valid, valid, valid, valid, valid, valid, valid, 3360, 3363, 3366, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, 3369,

      // Start of 0x10a00:
      valid, valid, valid, valid, valid, valid, valid, 52,

      // Start of 0x2060:
      58, 58, 58, 58, 58, disallowed, disallowed, disallowed, disallowed, disallowed, 58, 58, 58, 58, 58, 58,
      3382,

      // Start of 0x10a20:
      16, disallowed, disallowed, 3384, 3386, 3388, 3390, 3392, 3394, 3396, 3398, 3402, 3404, 3406, 26,

      // Start of 0x2080:
      3382, 78, 63, 65, 3384, 3386, 3388, 3390, 3392, 3394, 3396, 3398, 3402, 3404, 3406, disallowed,

      // Start of 0x10a40:
      0, 8, 28, 46, 410, 14, 20, 22, 24, 26, 30, 36, 38, disallowed, disallowed, disallowed,

      // Start of 0x20a0:
      valid, valid, valid, valid, valid, valid, valid, valid, 3408, valid, valid, valid, valid, valid, valid,
      valid,

      // Start of 0x10ae0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid,

      // Start of 0x2100:
      3411, 3415, 4, 3419, valid, 3423, 3427, 413, valid, 3431, 12, 14, 14, 14, 14, 245,

      // Start of 0x10b80:
      16, 16, 22, 22, valid, 26, 3435, valid, valid, 30, 32, 34, 34, 34, valid, valid,

      // Start of 0x2120:
      3438, 3441, 3445, valid, 50, valid, 865, valid, 50, valid, 20, 113, 2,

      // Start of 0x10ba0:
      4, valid, 8, 8, 10, 3448, 24, 28, 3452, 3455, 3458, 3461, 16, valid, 3464, 841, 808, 808, 841,

      // Start of 0x2140:
      3468, valid, valid, valid, valid, 6, 6, 8, 16, 18, valid, valid, valid,

      // Start of 0x10ce0:
      valid, valid, valid, 3472, 3478, 3484, 3491, 3497, 3503, 3509, 3515, 3521, 3527, 3533, 3539, 3545, 3551,
      3557, 3563,

      // Start of 0x2160:
      16, 3568, 3571, 3575, 42, 3578, 3581,

      // Start of 0x10d20, 0x114c0, 0x11d40, 0x1f840:
      3585, 3590, 46, 3593, 3596, 22, 4, 6, 24, 16, 3568, 3571, 3575, 42, 3578, 3581, 3585, 3590, 46, 3593,

      // Start of 0x10d80:
      3596, 22, 4, 6, 24,

      // Start of 0x2180:
      valid, valid, valid, 3600, valid, valid, valid, valid, valid, 3604, valid, valid, disallowed,
      disallowed, disallowed, disallowed, valid, valid, valid, valid,

      // Start of 0x2220:
      valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x10ea0:
      valid, valid, valid, valid, valid, 3610, 3617, valid, 3627, 3634, valid, valid, valid, valid, valid,
      valid,

      // Start of 0x2320:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 3644, 3648, valid, valid, valid,

      // Start of 0x10ec0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid,

      // Start of 0x2460:
      78, 63, 65, 3384, 3386, 3388, 3390, 3392, 3394, 3652, 3655, 3658, 3661, 3664,

      // Start of 0x110c0, 0x11aa0:
      3667, 3670, 3673, 3676, 3679, 3682, 3685, 3689, 3693, 3697, 3701, 3705, 3709, 3713, 3717, 3721, 3726,
      3731,

      // Start of 0x2480:
      3736, 3741, 3746, 3751, 3756,

      // Start of 0x110e0:
      3761, 3766, 3771, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, 3776, 3780, 3784, 3788,

      // Start of 0x24a0:
      3792, 3796, 3800, 3804, 3808,

      // Start of 0x11280:
      3812, 3816, 3820, 3824, 3828, 3832, 3836, 3840, 3844, 3848, 3852, 3856, 3860, 3864, 3868, 3872, 3876,

      // Start of 0x1d400, 0x1d5a0:
      0, 2, 4, 6,

      // Start of 0x1d640:
      8, 10,

      // Start of 0x1d420, 0x1d5c0:
      12, 14, 16, 18,

      // Start of 0x24c0, 0x1d660:
      20, 22,

      // Start of 0x1d5e0:
      24, 26,

      // Start of 0x112e0, 0x1d560:
      28, 30,

      // Start of 0x1d4e0, 0x1d680:
      32, 34,

      // Start of 0x1d460, 0x1d600:
      36, 38,

      // Start of 0x1d580:
      40, 42, 44, 46, 48, 50, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,

      // Start of 0x24e0:
      32, 34, 36, 38,

      // Start of 0x11320:
      40, 42, 44, 46, 48, 50, 3382, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x2a00:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x11340:
      valid, 3880,

      // Start of 0x2a60:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, 3893, 3897, 3900,

      // Start of 0x2ac0:
      valid, valid, valid, valid,

      // Start of 0x11360:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 3904, valid, valid, valid,

      // Start of 0x2c00:
      3910, 3914, 3918, 3922,

      // Start of 0x11380:
      3926, 3930, 3934, 3938, 3942, 3946, 3950, 3954, 3958, 3962, 3966, 3970, 3974, 3978, 3982, 3986, 3990,
      3994, 3998, 4002, 4006, 4010, 4014, 4018, 4022, 4026, 4030, 4034,

      // Start of 0x2c20:
      4038, 4042, 4046,

      // Start of 0x113c0:
      4050, 4054, 4058, 4062, 4066, 4070, 4074, 4078, 4082, 4086, 4090, 4094, 4098, valid, valid, valid,
      valid, valid, valid, valid, valid, valid,

      // Start of 0x113e0:
      valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x2c60:
      4102, valid, 4106, 4109, 4113, valid, valid, 4116, valid, 4120, valid, 4124, valid, 2194, 2255, 2191,
      2220, valid, 4128, valid, valid, 4132, valid, valid, valid,

      // Start of 0x11640:
      valid, valid, valid, 18, 42, 4136, 4139,

      // Start of 0x2c80:
      4142, valid, 4146, valid, 4150, valid, 4154, valid, 4158, valid, 4162, valid, 4166, valid, 4170, valid,
      4174, valid, 4178, valid, 4182, valid, 4186, valid, 4190,

      // Start of 0x118e0:
      valid, 4194, valid, 4198, valid, 4202, valid,

      // Start of 0x2ca0:
      4206, valid, 4210, valid, 4214, valid, 4218, valid, 4222, valid, 4226, valid, 4230, valid, 4234, valid,
      4238, valid, 4242, valid, 4246, valid, 4250, valid,

      // Start of 0x11900:
      4254, valid, 4258, valid, 4262, valid, 4266, valid,

      // Start of 0x2cc0:
      4270, valid, 4274, valid, 4278,

      // Start of 0x11d00:
      valid,

      // Start of 0x11d60:
      4282, valid, 4286, valid, 4290, valid, 4294, valid, 4298, valid,

      // Start of 0x11920:
      4302, valid, 4306, valid, 4310, valid, 4314, valid, 4318, valid, 4322, valid, 4326, valid, 4330, valid,

      // Start of 0x2ce0:
      4334, valid, 4338, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x11940:
      4342, valid, 4346, valid, valid, valid, valid, 4350, valid, disallowed, disallowed, disallowed,
      disallowed, disallowed,

      // Start of 0x2d60:
      valid, valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed,

      // Start of 0x11be0:
      disallowed, disallowed, disallowed, disallowed, disallowed, 4354, valid, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed,

      // Start of 0x2e80:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x11ca0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      disallowed, valid, valid, valid, valid, 4358,

      // Start of 0x2ee0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x11d20:
      valid, valid, valid, valid, valid, valid, valid, valid, 4362, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed,

      // Start of 0x2f00:
      4366, 4370, 4374, 4378, 4382, 4386, 4390, 4394, 4398, 4402,

      // Start of 0x11d80:
      4406, 4410, 4414, 4418, 4422, 4426, 4430, 4434, 4438, 4442, 4446, 4450, 4454, 4458, 4462, 4466, 4470,
      4474, 4478, 4482, 4486, 4490,

      // Start of 0x2f20:
      4494, 4498, 4502,

      // Start of 0x11fa0:
      4506, 4510, 4514, 4518, 4522, 4526, 4530, 4534, 4538, 4542, 4546, 4550, 4554, 4558, 4562, 4566, 4570,
      4574, 4578, 4582, 4586, 4590, 4594, 4598, 4602, 4606, 4610, 4614, 4618,

      // Start of 0x2f40:
      4622, 4626, 4630,

      // Start of 0x11fe0:
      4634, 4638, 4642, 4646, 4650, 4654, 4658, 4662, 4666, 4670, 4674, 4678, 4682, 4686, 4690, 4694, 4698,
      4702, 4706, 4710, 4714, 4718, 4722, 4726, 4730, 4734, 4738, 4742, 4746,

      // Start of 0x2f60:
      4750, 4754,

      // Start of 0x12460:
      4758, 4762, 4766, 4770, 4774, 4778, 4782, 4786, 4790, 4794, 4798, 4802, 4806, 4810, 4814, 4818, 4822,
      4826, 4830, 4834, 4838, 4842, 4846, 4850, 4854, 4858, 4862, 4866, 4870, 4874,

      // Start of 0x2f80:
      4878, 4882,

      // Start of 0x16ae0:
      4886, 4890, 4894, 4898, 4902, 4906, 4910, 4914, 4918, 4922, 4926, 4930, 4934, 4938, 4942, 4946,

      // Start of 0x16b40:
      4950, 4954, 4958, 4962, 4966, 4970, 4974, 4978, 4982, 4986, 4990, 4994, 4998, 5002,

      // Start of 0x2fa0:
      5006, 5010, 5014, 5018, 5022, 5026, 5030, 5034, 5038, 5042,

      // Start of 0x16b60:
      5046, 5050, 5054, 5058, 5062, 5066, 5070, 5074, 5078, 5082, 5086, 5090, 5094, 5098, 5102, 5106, 5110,
      5114, 5118, 5122, 5126, 5130,

      // Start of 0x2fc0:
      5134, 5138, 5142, 5146, 5150, 5154, 5158,

      // Start of 0x16f80:
      5162, 5166, 5170, 5174, 5178, 5182, 5186, 5190, 5194, 5198, 5202, 5206, 5210, 5214, 5218, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed,

      // Start of 0x3000:
      52, valid,

      // Start of 0x16fe0:
      5222, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x3020:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1afe0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 5224, valid, 4458,
      5228, 5232,

      // Start of 0x3080:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1b120:
      valid, valid, valid,

      // Start of 0x1e080:
      valid, valid, valid, valid, valid, disallowed, disallowed, valid, valid, 5236, 5241, valid, valid, 5246,

      // Start of 0x30e0:
      valid, valid,

      // Start of 0x1b140:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x1b160:
      valid, 5253,

      // Start of 0x3120:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, disallowed, 5260, 5264,

      // Start of 0x1bc60:
      5268, 5272, 5276, 5280, 5284, 5288, 5292, 5296, 5300, 5304, 5308, 5312, 5316,

      // Start of 0x3140:
      5320, 5324, 5328, 5332, 5336, 5340, 5344,

      // Start of 0x1bc80:
      5348, 5352, 5356, 5360, 5364, 5368, 5372, 5376, 5380, 5384, 5388, 5392, 5396, 5400, 5404, 5408, 5412,
      5416, 5420, 5424, 5428, 5432, 5436, 5440, 5444,

      // Start of 0x3160:
      5448, 5452, 5456,

      // Start of 0x1da80:
      5460, 58, 5464, 5468, 5472, 5476, 5480, 5484, 5488, 5492, 5496, 5500, 5504, 5508, 5512, 5516, 5520,
      5524, 5528, 5532, 5536, 5540, 5544, 5548, 5552, 5556,

      // Start of 0x1daa0:
      5560, 5564, 5568,

      // Start of 0x3180:
      5572, 5576, 5580, 5584, 5588, 5592, 5596, 5600, 5604, 5608, 5612, 5616, 5620, 5624, 5628, disallowed,
      valid, valid, 4366, 4390, 5632, 5636, 5640, 5644,

      // Start of 0x1df20:
      5648, 5652, 4382, 5656, 5660, 5664, 5668, 4398,

      // Start of 0x3200:
      5672, 5678, 5684, 5690, 5696, 5702, 5708, 5714, 5720, 5726, 5732, 5738, 5744, 5750, 5756, 5762, 5768,
      5774, 5780, 5786, 5792, 5798, 5804, 5810,

      // Start of 0x1e000:
      5816, 5822, 5828, 5834, 5840, 5846, 5855, disallowed,

      // Start of 0x3220:
      5864, 5870, 5876,

      // Start of 0x1fa60:
      5882, 5888, 5894, 5900, 5906, 5912, 5918, 5924, 5930, 5936, 5942, 5948, 5954, 5960, 5966, 5972,

      // Start of 0x1e120:
      5978, 5984, 5990, 5996, 6002, 6008, 6014, 6020, 6026, 6032, 6038, 6044, 6050,

      // Start of 0x3240:
      6056, 6062, 6068, 6074, 6080, 6084, 4630,

      // Start of 0x1e140:
      6088, valid, valid, valid, valid, valid, valid, valid, valid, 6092, 6096, 6099, 6102, 6105, 6108, 6111,
      6114, 6117, 6120, 6123, 6126, 6129, 6132, 6135, 6138,

      // Start of 0x3260:
      5260, 5272, 5284, 5292, 5324, 5328, 5340,

      // Start of 0x1e5e0:
      5348, 5352, 5360, 5364,

      // Start of 0x1f760:
      5368, 5372, 5376, 6141, 6145, 6149, 6153, 6157, 6161, 6165, 6169, 6173, 6177, 6181, 6185, 6189, 6193,
      6197, 6204, 6211, valid,

      // Start of 0x3280:
      4366, 4390, 5632, 5636, 6215, 6219,

      // Start of 0x1e7e0:
      6223, 4410, 6227, 4458, 4658, 4706, 4702, 4662, 5030, 4490, 4650, 6231, 6235, 6239, 6243, 6247, 6251,
      6255, 6259, 6263, 6267, 4514, 6271, 6275, 6279, 6283,

      // Start of 0x32a0, 0x1e8c0:
      6287, 6291, 6295, 6299, 5640, 5644, 5648, 6303, 6307, 6311, 6315, 6319, 6323, 6327, 6331, 6335, 6339,
      6343, 6346, 6349, 6352, 6355, 6358, 6361, 6364, 6367, 6370, 6373, 6376, 6379, 6382, 6385,

      // Start of 0x32c0, 0x1e940:
      6388, 6393, 6398, 6403, 6408, 6413, 6418, 6423, 6428, 6433, 6439, 6445, 6451, 6454, 6458, 6461, 6465,
      6469, 6473, 6477, 6481, 6485, 6489, 6493, 6497, 6501, 6505, 6509, 6513, 6517, 6521, 6525,

      // Start of 0x32e0, 0x1ec60:
      6529, 6533, 6537, 6541, 6545, 6549, 6553, 6557, 6561, 6565, 6569, 6573, 6577, 6581, 6585, 6589, 6593,
      6597, 6601, 6605, 6609, 6613, 6617, 6621, 6625, 6629, 6633, 6637, 6641, 6645, 6649, 6653,

      // Start of 0x3300, 0x1eee0:
      6660, 6673, 6686, 6699, 6709, 6722, 6732, 6742, 6758, 6771, 6781, 6791, 6801, 6814, 6827, 6837, 6847,
      6854, 6864, 6877, 6890, 6897, 6913, 6932, 6948, 6958, 6974, 6990, 7003, 7013, 7023, 7033,

      // Start of 0x3320, 0x1f6e0:
      7046, 7062, 7075, 7085, 7095, 7105, 7112, 7119, 7126, 7133, 7143, 7153, 7169, 7179, 7192, 7208, 7218,

      // Start of 0x1f7e0:
      7225, 7232, 7248, 7261, 7277, 7287, 7303, 7310, 7320, 7330, 7340, 7350, 7360, 7373, 7383,

      // Start of 0x3340:
      7390, 7400, 7410, 7420, 7433, 7443, 7453, 7463, 7479, 7492, 7499, 7515, 7522, 7535, 7548, 7558, 7568,

      // Start of 0x1f8a0:
      7578, 7591, 7598, 7608, 7621, 7628, 7644, 7654, 7659, 7664, 7669, 7674, 7679, 7684, 7689,

      // Start of 0x3360:
      7694, 7699, 7704, 7710, 7716, 7722,

      // Start of 0x1fac0:
      7728, 7734, 7740, 7746, 7752, 7758, 7764, 7770, 7776, 7782, 7788, 7794, 7798, 7801, 7804, 7808, 7811,
      7814, 7817, 7821, 7825, 7828, 7835, 7842, 7849, 7856,

      // Start of 0x3380:
      7869, 7872, 7875, 7879, 7882,

      // Start of 0x1fae0:
      7885, 7888, 7891, 7894, 7898, 7903, 7906, 7909, 7913, 7917, 7920, 7923, 7926, 7930, 7934, 7938, 7942,
      7946, 7949, 7952, 7955, 7958, 7961,

      // Start of 0x2cea0:
      7965, 7968, 7971, 7974,

      // Start of 0x33a0:
      7978, 7982, 7985, 7989, 7993, 7997, 8000, 8004, 8010, 7869, 8017, 8021, 8025, 8029, 8033, 8041, 8050,
      8053, 8056, 8060, 8063, 8066, 8069, 8073, 8076, 8073, 8079, 8082, 8085, 8089, 8092, 8089,

      // Start of 0x33c0:
      8095, 8099, disallowed, 8103, 8106, 8109, 8112, disallowed, 8119, 8122, 8125, 8128, 8131, 8134, 7971,
      8137, 8140, 8143, 8146, 8150, 7888, 8153, 8157, 8161, disallowed, 8164, 8168, 8171, 8174, 8177, 8180,
      8186,

      // Start of 0x33e0:
      8192, 8197, 8202, 8207, 8212, 8217, 8222, 8227, 8232, 8237, 8243, 8249, 8255, 8261, 8267, 8273, 8279,
      8285, 8291, 8297, 8303, 8309, 8315, 8321, 8327, 8333, 8339, 8345, 8351, 8357, 8363, 8369,

      // Start of 0xa640:
      8373, valid, 8377, valid, 8381, valid, 8385, valid, 8389, valid, 2003, valid, 8393, valid, 8397, valid,
      8401, valid, 8405, valid, 8409, valid, 8413, valid, 8417, valid, 8421, valid, 8425, valid, 8429, valid,

      // Start of 0xa660:
      8433, valid, 8437, valid, 8441, valid, 8445, valid, 8449, valid, 8453, valid, 8457, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid,

      // Start of 0xa680:
      8461, valid, 8465, valid, 8469, valid, 8473, valid, 8477, valid, 8481, valid, 8485, valid, 8489, valid,
      8493, valid, 8497, valid, 8501, valid, 8505, valid, 8509, valid, 8513, valid, 1054, 1060,

      // Start of 0xa720:
      valid, valid, 8517, valid, 8521, valid, 8525, valid, 8529, valid, 8533, valid, 8537, valid, 8541, valid,
      valid, valid, 8545, valid, 8549, valid, 8553, valid, 8557, valid, 8561, valid, 8565, valid, 8569, valid,

      // Start of 0xa740:
      8573, valid, 8577, valid, 8581, valid, 8585, valid, 8589, valid, 8593, valid, 8597, valid, 8601, valid,
      8605, valid, 8609, valid, 8613, valid, 8617, valid, 8621, valid, 8625, valid, 8629, valid, 8633, valid,

      // Start of 0xa760:
      8637, valid, 8641, valid, 8645, valid, 8649, valid, 8653, valid, 8657, valid, 8661, valid, 8665, valid,
      8665, valid, valid, valid, valid, valid, valid, valid, valid, 8669, valid, 8673, valid, 8677, 8681,
      valid,

      // Start of 0xa780:
      8685, valid, 8689, valid, 8693, valid, 8697, valid, valid, valid, valid, 8701, valid, 2232, valid,
      valid, 8705, valid, 8709, valid, valid, valid, 8713, valid, 8717, valid, 8721, valid, 8725, valid, 8729,
      valid,

      // Start of 0xa7a0:
      8733, valid, 8737, valid, 8741, valid, 8745, valid, 8749, valid, 695, 2201, 2229, 8753, 2235, valid,
      8756, 8759, 2242, 8762, 8766, valid, 8770, valid, 8774, valid, 8778, valid, 8782, valid, 8786, valid,

      // Start of 0xa7c0:
      8790, valid, 8794, valid, 8798, 2270, 8802, 8806, valid, 8810, valid, 8814, 8817, valid, disallowed,
      disallowed, 8821, valid, disallowed, valid, disallowed, valid, 8825, valid, 8829, valid, 8833, valid,
      8837,

      // Start of 0xa7e0:
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, 4, 10, 32, 8840, valid, valid, 245, 314,

      // Start of 0xab40:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 8525, 8844,
      4106, 8848,

      // Start of 0xab60:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, 8852, valid, valid, disallowed,
      disallowed, disallowed, disallowed, 8855, 8859, 8863, 8867, 8871, 8875, 8879, 8883, 8887, 8891, 8895,
      8899, 8903, 8907, 8911, 8915,

      // Start of 0xab80:
      8919, 8923, 8927, 8931, 8935, 8939, 8943, 8947, 8951, 8955, 8959, 8963, 8967, 8971, 8975, 8979, 8983,
      8987, 8991, 8995, 8999, 9003, 9007, 9011, 9015, 9019, 9023, 9027, 9031, 9035, 9039, 9043,

      // Start of 0xaba0:
      9047, 9051, 9055, 9059, 9063, 9067, 9071, 9075, 9079, 9083, 9087, 9091, 9095, 9099, 9103, 9107, 9111,
      9115, 9119, 9123, 9127, 9131, 9135, 9139, 9143, 9147, 9151, 9155, 9159, 9163, 9167, 9171,

      // Start of 0xf900:
      9175, 9179, 4998, 9183, 9187, 9191, 9195, 5214, 5214, 9199, 5030, 9203, 9207, 9211, 9215, 9219, 9223,
      9227, 9231, 9235, 9239, 9243, 9247, 9251, 9255, 9259, 9263, 9267, 9271, 9275, 9279, 9283,

      // Start of 0xf920:
      9287, 9291, 9295, 9299, 9303, 9307, 9311, 9315, 9319, 9323, 9327, 9331, 9335, 9339, 9343, 9347, 9351,
      9355, 9359, 9363, 4862, 9367, 9371, 9375, 9379, 9383, 9387, 9391, 9395, 9399, 9403, 9407,

      // Start of 0xf940:
      5154, 9411, 9415, 9419, 9423, 9427, 9431, 9435, 9439, 9443, 9447, 9451, 9455, 9459, 9463, 9467, 9471,
      9475, 9479, 9483, 9487, 9491, 9495, 9499, 9503, 9507, 9511, 9515, 9239, 9519, 9523, 9527,

      // Start of 0xf960:
      9531, 9535, 9539, 9543, 9547, 9551, 9555, 9559, 9563, 9567, 9571, 9575, 9579, 9583, 9587, 9591, 9595,
      5006, 9599, 9603, 9607, 9611, 9615, 9619, 9623, 9627, 9631, 9635, 9639, 9643, 9647, 9651,

      // Start of 0xf980:
      9655, 4514, 9659, 9663, 9667, 9671, 9675, 9679, 9683, 9687, 4438, 9691, 9695, 9699, 9703, 9707, 9711,
      9715, 9719, 9723, 9727, 9731, 9735, 9739, 9743, 9747, 9751, 9755, 9759, 9763, 9767, 9771,

      // Start of 0xf9a0:
      9775, 9591, 9779, 9783, 9787, 9791, 9795, 9799, 9803, 9807, 9527, 9811, 9815, 9819, 9823, 9827, 9831,
      9835, 9839, 9843, 9847, 9851, 9855, 9859, 9863, 9867, 9871, 9875, 9879, 9883, 9887, 9239,

      // Start of 0xf9c0:
      9891, 9895, 9899, 9903, 5210, 9907, 9911, 9915, 9919, 9923, 9927, 9931, 9935, 9939, 9943, 9947, 9951,
      6219, 9955, 9959, 9963, 9967, 9971, 9975, 9979, 9983, 9987, 9535, 9991, 9995, 9999, 10003,

      // Start of 0xf9e0:
      10007, 10011, 10015, 10019, 10023, 10027, 10031, 10035, 10039, 5026, 10043, 10047, 10051, 10055, 10059,
      10063, 10067, 10071, 10075, 10079, 10083, 10087, 10091, 4830, 10095, 10099, 10103, 10107, 10111, 10115,
      10119, 10123,

      // Start of 0xfa00:
      10127, 10131, 10135, 10139, 10143, 10147, 10151, 10155, 4938, 10159, 4950, 10163, 10167, 10171, valid,
      valid, 10175, valid, 10179, valid, valid, 10183, 10187, 10191, 10195, 10199, 10203, 10207, 10211, 10215,
      4858, valid,

      // Start of 0xfa20:
      10219, valid, 10223, valid, valid, 10227, 10231, valid, valid, valid, 10235, 10239, 10243, 10247, 10251,
      10255, 10259, 10263, 10267, 10271, 10275, 10279, 10283, 10287, 10291, 10295, 10299, 10303, 4542, 10307,
      10311, 10315,

      // Start of 0xfa40:
      10319, 10323, 10327, 10331, 10335, 10339, 10343, 10347, 10351, 10355, 10359, 10363, 6239, 10367, 10371,
      10375, 10379, 6255, 10383, 10387, 10391, 10395, 10399, 9735, 10403, 10407, 10411, 10415, 10419, 10423,
      10423, 10427,

      // Start of 0xfa60:
      10431, 10435, 10439, 10443, 10447, 10451, 10455, 10227, 10459, 10463, 10467, 10471, 10475, 10480,
      disallowed, disallowed, 10484, 10488, 10492, 10496, 10500, 10504, 10508, 10512, 10283, 10516, 10520,
      10524, 10175, 10528, 10532, 10536,

      // Start of 0xfa80:
      10540, 10544, 10548, 10552, 10556, 10560, 10564, 10568, 10572, 10315, 10576, 10319, 10580, 10584, 10588,
      10592, 10596, 10179, 9323, 10600, 10604, 4674, 9595, 9927, 10608, 10612, 10347, 10616, 10351, 10620,
      10624, 10628,

      // Start of 0xfaa0:
      10187, 10632, 10636, 10640, 10644, 10648, 10191, 10652, 10656, 10660, 10664, 10668, 10672, 10399, 10676,
      10680, 9735, 10684, 10415, 10688, 10692, 10696, 10700, 10704, 10435, 10708, 10223, 10712, 10439, 9519,
      10716, 10443,

      // Start of 0xfac0:
      10720, 10451, 10724, 10728, 10732, 10736, 10740, 10459, 10211, 10744, 10463, 10748, 10467, 10752, 5214,
      10756, 10761, 10766, 10771, 10775, 10779, 10783, 10788, 10793, 10798, 10802, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed,

      // Start of 0xfb00:
      10806, 10809, 10812, 10815, 10819, 10823, 10823, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 10826,
      10831, 10836, 10841, 10846, disallowed, disallowed, disallowed, disallowed, disallowed, 10851, valid,
      10856,

      // Start of 0xfb20:
      10861, 3452, 3461, 10864, 10867, 10870, 10873, 10876, 10879, 3396, 10882, 10887, 10892, 10899, 10906,
      10911, 10916, 10921, 10926, 10931, 10936, 10941, 10946, disallowed, 10951, 10956, 10961, 10966, 10971,
      disallowed, 10976, disallowed,

      // Start of 0xfb40:
      10981, 10986, disallowed, 10991, 10996, disallowed, 11001, 11006, 11011, 11016, 11021, 11026, 11031,
      11036, 11041, 11046, 11051, 11051, 11054, 11054, 11054, 11054, 11057, 11057, 11057, 11057, 11060, 11060,
      11060, 11060,

      // Start of 0xfb60:
      11063, 11063, 11066, 11066, 11066, 11066, 11069, 11069, 11069, 11069, 11072, 11072, 11072, 11072, 11075,
      11075, 11075, 11075, 11078, 11078, 11078, 11078, 11081, 11081, 11081, 11081, 11084, 11084, 11084, 11084,

      // Start of 0xfb80:
      11087, 11087, 11090, 11090, 11093, 11093, 11096, 11096, 11099, 11099, 11102, 11102, 11105, 11105, 11108,
      11108, 11108, 11108, 11111, 11111, 11111, 11111, 11114, 11114, 11114, 11114, 11117, 11117, 11117, 11117,
      11120, 11120,

      // Start of 0xfba0:
      11123, 11123, 11123, 11123, 11126, 11126, 11129, 11129, 11129, 11129, 11132, 11132, 11132, 11132, 11135,
      11135, 11138, 11138, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,

      // Start of 0xfbc0:
      valid, valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, 11141, 11141, 11141, 11141, 11144, 11144, 11147, 11147, 11150, 11150, 1501, 11153, 11153,

      // Start of 0xfbe0:
      11156, 11156, 11159, 11159, 11162, 11162, 11162, 11162, 11165, 11165, 11168, 11168, 11173, 11173, 11178,
      11178, 11183, 11183, 11188, 11188, 11193, 11193, 11198, 11198, 11198, 11203, 11203, 11203, 11208, 11208,
      11208, 11208,

      // Start of 0xfc00:
      11211, 11216, 11221, 11203, 11226, 11231, 11236, 11241, 11246, 11251, 11256, 11261, 11266, 11271, 11276,
      11281, 11286, 11291, 11296, 11301, 11306, 11311, 11316, 11321, 11326, 11331, 11336, 11341, 11346, 11351,
      11356, 11361,

      // Start of 0xfc20:
      11366, 11371, 11376, 11381, 11386, 11391, 11396, 11401, 11406, 11411, 11416, 11421, 11426, 11431, 11436,
      11441, 11446, 11451, 11456, 11461, 11466, 11471, 11476, 11481, 11486, 11491, 11496, 11501, 11506, 11511,
      11516, 11521,

      // Start of 0xfc40:
      11526, 11531, 11536, 11541, 11546, 11551, 11556, 11561, 11566, 11571, 11576, 11581, 11586, 11591, 11596,
      11601, 11606, 11611, 11616, 11621, 11626, 11631, 11636, 11641, 11646, 11651, 11656, 11661, 11666, 11671,
      11676, 11682,

      // Start of 0xfc60:
      11688, 11694, 11700, 11706, 11712, 11717, 11221, 11722, 11203, 11226, 11727, 11732, 11246, 11737, 11251,
      11256, 11742, 11747, 11276, 11752, 11281, 11286, 11757, 11762, 11296, 11767, 11301, 11306, 11451, 11456,
      11471, 11476,

      // Start of 0xfc80:
      11481, 11501, 11506, 11511, 11516, 11536, 11541, 11546, 11772, 11566, 11777, 11782, 11596, 11787, 11601,
      11606, 11671, 11792, 11797, 11646, 11802, 11651, 11656, 11211, 11216, 11807, 11221, 11812, 11231, 11236,
      11241, 11246,

      // Start of 0xfca0:
      11817, 11261, 11266, 11271, 11276, 11822, 11296, 11311, 11316, 11321, 11326, 11331, 11341, 11346, 11351,
      11356, 11361, 11366, 11827, 11371, 11376, 11381, 11386, 11391, 11396, 11406, 11411, 11416, 11421, 11426,
      11431, 11436,

      // Start of 0xfcc0:
      11441, 11446, 11461, 11466, 11486, 11491, 11496, 11501, 11506, 11521, 11526, 11531, 11536, 11832, 11551,
      11556, 11561, 11566, 11581, 11586, 11591, 11596, 11837, 11611, 11616, 11842, 11631, 11636, 11641, 11646,
      11847, 11221,

      // Start of 0xfce0:
      11812, 11246, 11817, 11276, 11822, 11296, 11852, 11361, 11857, 11862, 11867, 11501, 11506, 11536, 11596,
      11837, 11646, 11847, 11872, 11879, 11886, 11893, 11898, 11903, 11908, 11913, 11918, 11923, 11928, 11933,
      11938, 11943,

      // Start of 0xfd00:
      11948, 11953, 11958, 11963, 11968, 11973, 11978, 11983, 11988, 11993, 11998, 12003, 11862, 12008, 12013,
      12018, 12023, 11893, 11898, 11903, 11908, 11913, 11918, 11923, 11928, 11933, 11938, 11943, 11948, 11953,
      11958, 11963,

      // Start of 0xfd20:
      11968, 11973, 11978, 11983, 11988, 11993, 11998, 12003, 11862, 12008, 12013, 12018, 12023, 11993, 11998,
      12003, 11862, 11857, 11867, 11401, 11346, 11351, 11356, 11993, 11998, 12003, 11401, 11406, 12028, 12028,

      // Start of 0xfd40:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 12033, 12040, 12040, 12047, 12054, 12061, 12068, 12075, 12082, 12082, 12089, 12096, 12103, 12110,
      12117,

      // Start of 0xfd60:
      12124, 12131, 12138, 12138, 12145, 12145, 12152, 12159, 12159, 12166, 12173, 12173, 12180, 12180, 12187,
      12194, 12194, 12201, 12201, 12208, 12215, 12222, 12229, 12229, 12236, 12243, 12250, 12257, 12264, 12264,
      12271, 12278,

      // Start of 0xfd80:
      12285, 12292, 12299, 12306, 12306, 12313, 12313, 12320, 12320, 12327, 12334, 12341, 12348, 12355, 12362,
      12369, disallowed, disallowed, 12376, 12383, 12390, 12397, 12404, 12411, 12411, 12418, 12425, 12432,
      12439, 12439, 12446, 12453,

      // Start of 0xfda0:
      12460, 12467, 12474, 12481, 12488, 12495, 12502, 12509, 12516, 12523, 12530, 12537, 12544, 12551, 12558,
      12565, 12572, 12579, 12586, 12593, 12271, 12285, 12600, 12607, 12614, 12621, 12628, 12635, 12628, 12614,
      12642, 12649,

      // Start of 0xfdc0:
      12656, 12663, 12670, 12635, 12222, 12152, 12677, 12684, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, valid,

      // Start of 0xfde0:
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 12691,
      12698, 12705, 12714, 12723, 12732, 12741, 12750, 12759, 12768, 12775, 12809, 12825, valid, valid, valid,

      // Start of 0xfe00:
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 12834, 12836, disallowed, 12840, 767,
      12842, 12844, 12846, 12850, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed,

      // Start of 0xfe20:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, disallowed, 12854, 12858, 12862, 12862, 3404, 3406, 12864, 12866, 12868, 12872, 12876, 12880,
      12884, 12888, 3644,

      // Start of 0xfe40:
      3648, 12892, 12896, 12900, 12904, valid, valid, 12908, 12910, 3356, 3356, 3356, 3356, 12862, 12862,
      12862, 12834, 12836, disallowed, disallowed, 767, 12840, 12844, 12842, 12854, 3404, 3406, 12864, 12866,
      12868, 12872, 12912,

      // Start of 0xfe60:
      12914, 12916, 3396, 12918, 12920, 12922, 3402, disallowed, 12924, 12926, 12928, 12930, disallowed,
      disallowed, disallowed, disallowed, 12932, 12936, 12941, valid, 12945, disallowed, 12949, 12953, 12958,
      12962, 12967, 12971, 12976, 12980, 12985, 12989,

      // Start of 0xfe80:
      12994, 12997, 12997, 13000, 13000, 13003, 13003, 13006, 13006, 13009, 13009, 13009, 13009, 13012, 13012,
      13015, 13015, 13015, 13015, 13018, 13018, 13021, 13021, 13021, 13021, 13024, 13024, 13024, 13024, 13027,
      13027,

      // Start of 0xfea0:
      13027, 13030, 13030, 13030, 13030, 13033, 13033, 13033, 13033, 13036, 13036, 13039, 13039, 13042, 13042,
      13045, 13045, 13048, 13048, 13048, 13048, 13051, 13051, 13051, 13051, 13054, 13054, 13054, 13054, 13057,
      13057,

      // Start of 0xfec0:
      13057, 13060, 13060, 13060, 13060, 13063, 13063, 13063, 13063, 13066, 13066, 13066, 13066, 13069, 13069,
      13069, 13069, 13072, 13072, 13072, 13072, 13075, 13075, 13075, 13075, 13078, 13078, 13078, 13078, 13081,
      13081,

      // Start of 0xfee0:
      13081, 13084, 13084, 13084, 13084, 13087, 13087, 13087, 13087, 13090, 13090, 13090, 13090, 13093, 13093,
      11165, 11165, 13096, 13096, 13096, 13096, 13099, 13099, 13104, 13104, 13109, 13109, 13114, 13114,
      disallowed, disallowed, 58,

      // Start of 0xff00:
      disallowed, 12842, 13119, 12912, 12926, 12928, 12914, 13121, 3404, 3406, 12916, 3396, 12834, 12918,
      5222, 13123, 3382, 78, 63, 65, 3384, 3386, 3388, 3390, 3392, 3394, 12840, 767, 12920, 3402, 12922,
      12844,

      // Start of 0xff20:
      12930, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48,
      50, 12908, 12924, 12910, 13125, 12862,

      // Start of 0xff40:
      3275, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50,
      12864, 13127, 12866, 13129, 13131,

      // Start of 0xff60:
      13135, 5222, 12892, 12896, 12836, 13139, 6649, 13143, 13147, 13151, 13155, 13159, 13163, 13167, 13171,
      13175, 13179, 6465, 6469, 6473, 6477, 6481, 6485, 6489, 6493, 6497, 6501, 6505, 6509, 6513, 6517, 6521,

      // Start of 0xff80:
      6525, 6529, 6533, 6537, 6541, 6545, 6549, 6553, 6557, 6561, 6565, 6569, 6573, 6577, 6581, 6585, 6589,
      6593, 6597, 6601, 6605, 6609, 6613, 6617, 6621, 6625, 6629, 6633, 6637, 13183, 13187, 13191,

      // Start of 0xffa0:
      58, 5260, 5264, 5268, 5272, 5276, 5280, 5284, 5288, 5292, 5296, 5300, 5304, 5308, 5312, 5316, 5320,
      5324, 5328, 5332, 5336, 5340, 5344, 5348, 5352, 5356, 5360, 5364, 5368, 5372, 5376,

      // Start of 0xffc0:
      disallowed, disallowed, 5380, 5384, 5388, 5392, 5396, 5400, disallowed, disallowed, 5404, 5408, 5412,
      5416, 5420, 5424, disallowed, disallowed, 5428, 5432, 5436, 5440, 5444, 5448, disallowed, disallowed,
      5452, 5456, 5460, disallowed, disallowed, disallowed,

      // Start of 0xffe0:
      13195, 13198, 13201, 59, 13204, 13207, 13210, disallowed, 13214, 13218, 13222, 13226, 13230, 13234,
      13238, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed,

      // Start of 0x10400:
      13242, 13247, 13252, 13257, 13262, 13267, 13272, 13277, 13282, 13287, 13292, 13297, 13302, 13307, 13312,
      13317, 13322, 13327, 13332, 13337, 13342, 13347, 13352, 13357, 13362, 13367, 13372, 13377, 13382, 13387,
      13392, 13397,

      // Start of 0x10420:
      13402, 13407, 13412, 13417, 13422, 13427, 13432, 13437, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid,

      // Start of 0x104a0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, 13442, 13447, 13452, 13457, 13462, 13467, 13472, 13477,
      13482, 13487, 13492, 13497, 13502, 13507, 13512, 13517,

      // Start of 0x104c0:
      13522, 13527, 13532, 13537, 13542, 13547, 13552, 13557, 13562, 13567, 13572, 13577, 13582, 13587, 13592,
      13597, 13602, 13607, 13612, 13617, disallowed, disallowed, disallowed, disallowed, valid, valid, valid,
      valid,

      // Start of 0x10560:
      valid, valid, valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, valid, 13622, 13627, 13632, 13637, 13642,
      13647, 13652, 13657, 13662, 13667, 13672, disallowed, 13677, 13682, 13687, 13692,

      // Start of 0x10580:
      13697, 13702, 13707, 13712, 13717, 13722, 13727, 13732, 13737, 13742, 13747, disallowed, 13752, 13757,
      13762, 13767, 13772, 13777, 13782, disallowed, 13787, 13792, disallowed, valid, valid, valid, valid,
      valid, valid, valid, valid,

      // Start of 0x10780:
      valid, 13797, 13800, 116, 13803, 383, disallowed, 13806, 13809, 13813, 13816, 398, 401, 13819, 13823,
      13826, 13829, 8814, 13832, 419, 13835, 245, 13838, 13841, 13844, 13847, 13850, 8753, 13853, 13858,
      13862, 13865,

      // Start of 0x107a0:
      13870, 13873, 167, 13878, 13881, 32, 13884, 13887, 4113, 13892, 452, 13895, 13898, 13901, 13905, 464,
      13908, disallowed, 13912, 13915, 13918, 13921, 13924, 13927, 13930, 13933, 13938, disallowed,
      disallowed, disallowed, disallowed, disallowed,

      // Start of 0x10c80:
      13943, 13948, 13953, 13958, 13963, 13968, 13973, 13978, 13983, 13988, 13993, 13998, 14003, 14008, 14013,
      14018, 14023, 14028, 14033, 14038, 14043, 14048, 14053, 14058, 14063, 14068, 14073, 14078, 14083, 14088,
      14093, 14098,

      // Start of 0x10ca0:
      14103, 14108, 14113, 14118, 14123, 14128, 14133, 14138, 14143, 14148, 14153, 14158, 14163, 14168, 14173,
      14178, 14183, 14188, 14193, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x10d40:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 14198, 14203, 14208, 14213, 14218, 14223, 14228, 14233, 14238, 14243, 14248, 14253, 14258, 14263,
      14268, 14273,

      // Start of 0x10d60:
      14278, 14283, 14288, 14293, 14298, 14303, disallowed, disallowed, disallowed, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid,

      // Start of 0x118a0:
      14308, 14313, 14318, 14323, 14328, 14333, 14338, 14343, 14348, 14353, 14358, 14363, 14368, 14373, 14378,
      14383, 14388, 14393, 14398, 14403, 14408, 14413, 14418, 14423, 14428, 14433, 14438, 14443, 14448, 14453,
      14458, 14463,

      // Start of 0x16e40:
      14468, 14473, 14478, 14483, 14488, 14493, 14498, 14503, 14508, 14513, 14518, 14523, 14528, 14533, 14538,
      14543, 14548, 14553, 14558, 14563, 14568, 14573, 14578, 14583, 14588, 14593, 14598, 14603, 14608, 14613,
      14618, 14623,

      // Start of 0x1bca0:
      58, 58, 58, 58, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x1ccc0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18,

      // Start of 0x1cce0:
      20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 3382, 78, 63, 65, 3384, 3386, 3388,
      3390, 3392, 3394, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x1d140:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      14628, 14637,

      // Start of 0x1d160:
      14646, 14659, 14672, 14685, 14698, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, 58, 58, 58, 58, 58, 58, 58, 58,

      // Start of 0x1d1a0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 14711, 14720, 14729,
      14742, 14755,

      // Start of 0x1d1c0:
      14768, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x1d440:
      24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 0, 2, 4, 6, 8, 10, 12, disallowed, 16, 18, 20,
      22, 24, 26, 28, 30, 32, 34,

      // Start of 0x1d480:
      48, 50, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48,
      50, 0, disallowed, 4, 6,

      // Start of 0x1d4a0:
      disallowed, disallowed, 12, disallowed, disallowed, 18, 20, disallowed, disallowed, 26, 28, 30, 32,
      disallowed, 36, 38, 40, 42, 44, 46, 48, 50, 0, 2, 4, 6, disallowed, 10, disallowed, 14, 16, 18,

      // Start of 0x1d4c0:
      20, 22, 24, 26, disallowed, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 0, 2, 4, 6, 8, 10, 12, 14, 16,
      18, 20, 22, 24, 26, 28, 30,

      // Start of 0x1d500:
      44, 46, 48, 50, 0, 2, disallowed, 6, 8, 10, 12, disallowed, disallowed, 18, 20, 22, 24, 26, 28, 30, 32,
      disallowed, 36, 38, 40, 42, 44, 46, 48, disallowed, 0, 2,

      // Start of 0x1d520:
      4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 0, 2,
      disallowed, 6, 8, 10, 12, disallowed,

      // Start of 0x1d540:
      16, 18, 20, 22, 24, disallowed, 28, disallowed, disallowed, disallowed, 36, 38, 40, 42, 44, 46, 48,
      disallowed, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26,

      // Start of 0x1d620:
      48, 50, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48,
      50, 0, 2, 4, 6,

      // Start of 0x1d6a0:
      44, 46, 48, 50, 14781, 14784, disallowed, disallowed, 802, 805, 808, 811,

      // Start of 0x1d720:
      814, 817, 820, 823, 748, 826,

      // Start of 0x1d760:
      829, 71, 832, 835, 838, 841,

      // Start of 0x1d7a0:
      844, 823, 847, 850, 853, 856, 859, 862,

      // Start of 0x1d6c0:
      865, 14787, 802, 805, 808, 811,

      // Start of 0x1d700:
      814, 817, 820, 823, 748, 826,

      // Start of 0x1d740:
      829, 71, 832, 835, 838, 841,

      // Start of 0x1d780:
      844, 847, 847, 850, 853, 856, 859, 862, 865, 14791, 814, 823, 826, 856,

      // Start of 0x1d6e0:
      844, 841, 802, 805, 808, 811, 814, 817, 820, 823, 748, 826, 829, 71, 832, 835, 838, 841, 844, 823, 847,
      850, 853, 856, 859, 862, 865, 14787, 802, 805, 808, 811,

      // Start of 0x1d7c0:
      859, 862, 865, 14791, 814, 823, 826, 856, 844, 841, 883, 883, disallowed, disallowed, 3382, 78, 63, 65,
      3384, 3386, 3388, 3390,

      // Start of 0x1d7e0:
      3392, 3394, 3382, 78, 63, 65, 3384, 3386, 3388, 3390, 3392, 3394, 3382, 78, 63, 65, 3384, 3386, 3388,
      3390, 3392, 3394, 3382, 78, 63, 65, 3384, 3386, 3388, 3390, 3392, 3394,

      // Start of 0x1e020:
      valid, valid, disallowed, valid, valid, disallowed, valid, valid, valid, valid, valid, disallowed,
      disallowed, disallowed, disallowed, disallowed, 976, 979, 982, 985, 988, 991, 994, 997, 1000, 1006,
      1009, 1012, 1018, 1021, 1024, 1027,

      // Start of 0x1e040:
      1030, 1033, 1036, 1039, 1042, 1045, 1048, 1057, 1063, 1066, 8477, 1240, 946, 952, 1264, 1177, 1204, 976,
      979, 982, 985, 988, 991, 994, 997, 1000, 1006, 1009, 1018, 1021, 1027, 1033,

      // Start of 0x1e060:
      1036, 1039, 1042, 1045, 1048, 1054, 1057, 1132, 946, 943, 973, 1171, 8401, 1180, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,

      // Start of 0x1e900:
      14795, 14800, 14805, 14810, 14815, 14820, 14825, 14830, 14835, 14840, 14845, 14850, 14855, 14860, 14865,
      14870, 14875, 14880, 14885, 14890, 14895, 14900, 14905, 14910, 14915, 14920, 14925, 14930, 14935, 14940,
      14945, 14950,

      // Start of 0x1e920:
      14955, 14960, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x1ee00:
      13012, 13015, 13027, 13036, disallowed, 13093, 13045, 13030, 13060, 13096, 13078, 13081, 13084, 13087,
      13048, 13066, 13072, 13054, 13075, 13042, 13051, 13021, 13024, 13033, 13039, 13057, 13063, 13069, 14965,
      11120, 14968, 14971,

      // Start of 0x1ee20:
      disallowed, 13015, 13027, disallowed, 13090, disallowed, disallowed, 13030, disallowed, 13096, 13078,
      13081, 13084, 13087, 13048, 13066, 13072, 13054, 13075, disallowed, 13051, 13021, 13024, 13033,
      disallowed, 13057, disallowed, 13069, disallowed, disallowed,

      // Start of 0x1ee40:
      disallowed, disallowed, 13027, disallowed, disallowed, disallowed, disallowed, 13030, disallowed, 13096,
      disallowed, 13081, disallowed, 13087, 13048, 13066, disallowed, 13054, 13075, disallowed, 13051,
      disallowed, disallowed, 13033, disallowed, 13057, disallowed, 13069, disallowed, 11120, disallowed,
      14971,

      // Start of 0x1ee60:
      disallowed, 13015, 13027, disallowed, 13090, disallowed, disallowed, 13030, 13060, 13096, 13078,
      disallowed, 13084, 13087, 13048, 13066, 13072, 13054, 13075, disallowed, 13051, 13021, 13024, 13033,
      disallowed, 13057, 13063, 13069, 14965, disallowed, 14968, disallowed,

      // Start of 0x1ee80:
      13012, 13015, 13027, 13036, 13090, 13093, 13045, 13030, 13060, 13096, disallowed, 13081, 13084, 13087,
      13048, 13066, 13072, 13054, 13075, 13042, 13051, 13021, 13024, 13033, 13039, 13057, 13063, 13069,
      disallowed, disallowed, disallowed,

      // Start of 0x1eea0:
      disallowed, 13015, 13027, 13036, disallowed, 13093, 13045, 13030, 13060, 13096, disallowed, 13081,
      13084, 13087, 13048, 13066, 13072, 13054, 13075, 13042, 13051, 13021, 13024, 13033, 13039, 13057, 13063,
      13069, disallowed, disallowed, disallowed,

      // Start of 0x1f100:
      disallowed, 14974, 14977, 14980, 14983, 14986, 14989, 14992, 14995, 14998, 15001, valid, valid, valid,
      valid, valid, 3776, 3780, 3784, 3788, 3792, 3796, 3800, 3804, 3808, 3812, 3816, 3820, 3824, 3828, 3832,
      3836,

      // Start of 0x1f120:
      3840, 3844, 3848, 3852, 3856, 3860, 3864, 3868, 3872, 3876, 15004, 4, 34, 8109, 15012, valid, 0, 2, 4,
      6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,

      // Start of 0x1f140:
      32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 15015, 8073, 15018, 15021, 15024, 15028, valid, valid, valid,
      valid, valid, valid,

      // Start of 0x1f160:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 15031, 15034, 15037, valid, valid,
      valid,

      // Start of 0x1f180:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 15040, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, valid,

      // Start of 0x1f200:
      15043, 15050, 6505, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 4618, 15057, 15061, 15065, 4390,
      15069, 15073, 5664, 15077, 15081, 15085, 9887, 15089, 15093, 15097, 15101,

      // Start of 0x1f220:
      15105, 15109, 4762, 15113, 15117, 15121, 15125, 15129, 15133, 4366, 5632, 15137, 6303, 5644, 6307,
      15141, 4986, 15145, 15149, 15153, 15157, 15161, 6235, 4658, 15165, 15169, 15173, 15177, disallowed,
      disallowed, disallowed, disallowed,

      // Start of 0x1f240:
      15181, 15191, 15201, 15211, 15221, 15231, 15241, 15251, 15261, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, 15271, 15275, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed,

      // Start of 0x1fbe0:
      valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
      valid, 3382, 78, 63, 65, 3384, 3386, 3388, 3390, 3392, 3394, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed,

      // Start of 0x2f800:
      15279, 15283, 15287, 15291, 15296, 10259, 15300, 15304, 15308, 15312, 10263, 15316, 15320, 15324, 10267,
      15329, 15333, 15337, 15341, 15346, 15350, 15097, 15354, 15359, 15363, 15367, 15371, 10488, 15375, 4430,
      15380, 15384,

      // Start of 0x2f820:
      15388, 15392, 15169, 15396, 15400, 10508, 10271, 10275, 10512, 15404, 15408, 9543, 15412, 10279, 15416,
      15420, 15424, 15428, 15428, 15428, 15432, 15437, 15441, 15445, 15449, 15454, 15458, 15462, 15466, 15470,
      15474, 15478,

      // Start of 0x2f840:
      15482, 15486, 15490, 15494, 15498, 15502, 15502, 10520, 15506, 15510, 15514, 15518, 10287, 15522, 15526,
      15530, 10127, 15534, 15538, 15542, 15546, 15550, 15554, 15558, 15562, 15566, 15571, 15575, 15579, 15069,
      15583, 15587,

      // Start of 0x2f860:
      15591, 15596, 15601, 15605, 15609, 15613, 15617, 15621, 15625, 15629, 15633, 15633, 15637, 15642, 15646,
      9527, 15650, 15654, 15659, 15663, 15667, 4534, 15671, 15675, 4542, 15679, 15683, 15687, 15692, 15696,
      15701, 15705,

      // Start of 0x2f880:
      15709, 15713, 15717, 15721, 15725, 15729, 15733, 15737, 15741, 15745, 15750, 15754, 15758, 15762, 9319,
      15766, 4582, 15771, 15771, 15776, 15780, 15780, 15784, 15788, 15793, 15798, 15802, 15806, 15810, 15814,
      15818, 15822,

      // Start of 0x2f8a0:
      15826, 15830, 15834, 10307, 15838, 15843, 15847, 15851, 10568, 15851, 15855, 10315, 15859, 15863, 15867,
      15871, 10319, 9211, 15875, 15879, 15883, 15887, 15891, 15895, 15899, 15904, 15908, 15912, 15916, 15920,
      15924, 15929,

      // Start of 0x2f8c0:
      15933, 15937, 15941, 15945, 15949, 15953, 15957, 15961, 10323, 15965, 15969, 15974, 15978, 15982, 15986,
      10331, 15990, 15994, 15998, 16002, 16006, 16010, 16014, 16018, 9323, 10600, 16022, 16026, 16030, 16034,
      16039, 16043,

      // Start of 0x2f8e0:
      16047, 16051, 10335, 16055, 16060, 16064, 16068, 10771, 16072, 16076, 16080, 16084, 16088, 16093, 16097,
      16101, 16105, 16110, 16114, 16118, 16122, 9595, 16126, 16130, 16135, 16140, 16145, 16149, 16154, 16158,
      16162, 16166,

      // Start of 0x2f900:
      16170, 10339, 9927, 16174, 16178, 16182, 16186, 16191, 16195, 16199, 16203, 10612, 16207, 16211, 16216,
      16220, 16224, 16229, 16234, 16238, 10616, 16242, 16246, 16250, 16254, 16258, 16262, 16266, 16271, 16275,
      16280, 16284,

      // Start of 0x2f920:
      16289, 10624, 16293, 16297, 16302, 16306, 16310, 16315, 16320, 16324, 16328, 16332, 16336, 16336, 16340,
      16344, 10632, 16348, 16352, 16356, 16360, 16364, 16369, 16373, 9539, 16378, 16383, 16387, 16392, 16397,
      16402, 16406,

      // Start of 0x2f940:
      10656, 16410, 16415, 16420, 16425, 16430, 16434, 16434, 10660, 10779, 16438, 16442, 16446, 16450, 16455,
      9391, 10668, 16459, 16463, 10379, 16468, 16473, 10207, 16478, 16482, 10391, 16486, 16490, 16494, 16499,
      16499, 16504,

      // Start of 0x2f960:
      16508, 16512, 16517, 16521, 16525, 16529, 16534, 16538, 16542, 16546, 16550, 16554, 16559, 16563, 16567,
      16571, 16575, 16579, 16583, 16588, 16593, 16597, 16602, 16606, 16611, 16615, 10415, 16619, 16624, 16629,
      16633, 16638,

      // Start of 0x2f980:
      16642, 16647, 16651, 16655, 16659, 16663, 16667, 16671, 16676, 16681, 16686, 15776, 16691, 16695, 16699,
      16703, 16707, 16711, 16715, 16719, 16723, 16727, 16731, 16735, 9607, 16740, 16744, 16748, 16752, 16756,
      16760, 10427,

      // Start of 0x2f9a0:
      16764, 16768, 16772, 16776, 16780, 16785, 16790, 16795, 16799, 16803, 16807, 16811, 16816, 16820, 16825,
      16829, 16833, 16838, 16843, 16847, 9371, 16851, 16855, 16859, 16863, 16867, 16871, 10696, 16875, 16879,
      16883, 16887,

      // Start of 0x2f9c0:
      16891, 16895, 16899, 16903, 4942, 16907, 16912, 16916, 16920, 16924, 16928, 16932, 16937, 16942, 16946,
      16950, 10716, 10720, 4970, 16954, 16959, 16963, 16967, 16971, 16975, 16980, 16985, 16989, 16993, 16997,
      17002, 10724,

      // Start of 0x2f9e0:
      17006, 17011, 17016, 17020, 17024, 17028, 17033, 17037, 17041, 17045, 17049, 17053, 17057, 17061, 17066,
      17070, 17074, 17078, 17083, 17087, 17091, 17095, 17099, 17104, 17109, 17113, 17117, 17121, 17126, 17130,
      10748, 10748,

      // Start of 0x2fa00:
      17135, 17139, 17144, 17148, 17152, 17156, 17160, 17164, 17168, 17172, 10752, 17177, 17181, 17185, 17189,
      17193, 17197, 17202, 17206, 17211, 17216, 5162, 17221, 5178, 17225, 17229, 17233, 17237, 5198, 17241,
      disallowed, disallowed,

      // Start of 0xe0100-0xe01c0:
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,

      // Start of 0xe01e0:
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
      disallowed, disallowed, disallowed, disallowed, disallowed,
      // done
    };





    /**
     * IDNA Mapping Status Table
     *
     * Table size:
     *   - in bits:       4192
     *   - in bytes:      524 B
     *   - in KibiBytes:  0.51 KiB
     */
    static constexpr std::array<std::uint32_t, 131ULL> idna_mappings_bools{
      0b1111'1111'1111'1111'1111'1111'1111'1111, 0b0000'0000'0000'0000'0000'0000'0000'0000,
      0b1111'1111'1111'1111'0000'0000'1111'1111, 0b1111'1111'1110'0000'0000'0011'1111'0000,
      0b1111'1111'1111'1111'1111'0111'1111'1111, 0b1111'1100'1111'1111'1111'1101'1111'1111,
      0b1111'1111'1001'1111'1111'1111'1111'1111, 0b1111'1111'1111'1111'1111'1111'1011'1111,
      0b1111'1111'1000'0011'1111'1111'1010'0111, 0b1111'1110'1111'1111'1000'0000'0111'1111,
      0b0010'1111'1110'1111'1111'1111'1001'1001, 0b1111'1111'1111'1111'1111'1111'1001'1110,
      0b1111'1111'1111'1001'1000'0111'1110'1110, 0b0000'0000'0111'1111'1111'1111'1100'0000,
      0b0111'1111'1111'1101'1101'1111'1111'0111, 0b1110'1110'1111'1100'1111'1011'0111'1111,
      0b1111'1111'0011'1100'0000'0000'0000'0100, 0b0111'1111'1011'1011'1111'1000'0000'1111,
      0b1111'1111'1111'0011'1111'1111'1110'0110, 0b1101'1100'0111'1110'1100'0000'0011'1111,
      0b1111'1100'0111'0001'1000'1101'0110'0011, 0b0000'0100'1111'0111'0001'1100'0011'1111,
      0b0111'1111'1111'1111'1111'1100'0000'0010, 0b1111'1111'1101'1101'1111'1111'1111'0000,
      0b1110'1111'1001'1111'1111'1111'1110'1111, 0b1110'0111'1001'0011'1011'0000'0001'1110,
      0b1111'1111'0111'1111'1100'0000'0111'1111, 0b1000'0000'1111'0111'0111'1100'1111'1011,
      0b0011'1011'1111'1111'0011'1101'1000'0001, 0b1100'0011'1111'0111'0111'1100'0000'0000,
      0b1111'1111'1111'1111'1011'1011'1111'1111, 0b1111'1111'1011'1111'1111'1111'1111'0001,
      0b1111'0101'1111'1000'0100'0111'1111'0010, 0b0000'0001'1100'1111'1111'1100'0000'1111,
      0b1111'1111'1111'1111'1111'1111'1111'0000, 0b1111'1111'1111'1111'1011'1110'1011'0111,
      0b0111'1111'1100'0001'1111'1111'1111'0111, 0b0111'1111'1100'1111'0101'1111'1100'1111,
      0b1100'1111'0111'1111'1111'1111'1100'1111, 0b1111'0111'1111'1111'1111'0011'1101'1111,
      0b1111'1111'1111'0011'1101'1111'1111'1111, 0b1111'1110'0000'0000'1111'1111'1111'1111,
      0b1111'1110'0000'0000'0001'1011'1011'1111, 0b1111'1110'0000'0111'1111'1110'0000'0111,
      0b1110'0010'0001'1111'1111'1110'0001'1111, 0b1111'1001'1111'1111'1111'1111'1111'1111,
      0b1111'1000'0111'1111'1111'1000'0000'0000, 0b1000'1111'1111'1110'0000'0111'1111'1111,
      0b1100'1111'1111'1111'1111'1111'1111'1111, 0b1111'1100'0000'0011'1111'1111'1111'1111,
      0b0000'0011'1110'0011'1111'1111'1111'1111, 0b0000'0101'1111'1111'1111'1110'0000'0000,
      0b1111'0111'1111'0111'1111'1111'1111'1001, 0b1111'1111'1110'0000'0000'0111'1111'0111,
      0b0011'1111'1111'1111'1100'0000'0001'1111, 0b0111'1111'1111'1000'0000'0111'1111'1110,
      0b0000'0001'1111'1111'1111'1111'1110'0000, 0b0011'1111'1111'1011'1111'1111'1111'0000,
      0b1100'1111'1111'1100'1111'1111'1111'1100, 0b1111'1100'0000'0000'0000'0000'0000'0011,
      0b1111'1110'0000'0000'1111'1100'1111'1100, 0b1111'1001'1111'1111'1111'1110'1111'1110,
      0b1111'1111'1111'1110'0000'0000'0001'1111, 0b1111'1011'1111'1111'1111'1111'1110'0001,
      0b1111'1111'1111'1101'1011'1111'1111'1111, 0b1111'1111'1111'0000'1111'1111'1111'1100,
      0b0000'0001'1111'1111'0001'1111'1111'1111, 0b0000'0001'1111'1111'1111'1110'0000'0000,
      0b1111'1110'0001'1111'1111'1111'1111'1100, 0b1111'1111'1111'0110'0000'0000'0111'1111,
      0b1111'1010'0111'1110'0011'0111'1111'0111, 0b0000'0100'1000'1101'1111'1111'1111'1111,
      0b1111'1000'0000'0000'0000'0111'1111'1100, 0b1111'1111'1100'0001'1011'1111'1111'1111,
      0b1111'1111'1110'0011'1111'1111'1111'1111, 0b1111'1111'1111'0000'0111'1111'1111'1111,
      0b1111'0000'0110'1111'0000'1111'1111'1111, 0b0111'1111'1111'1111'1111'1110'1110'1111,
      0b1111'1111'0000'0001'1111'1111'0000'1110, 0b0111'1111'1111'1000'0111'1111'0000'0001,
      0b0000'0011'1111'1111'1111'1111'0000'0000, 0b0000'0000'0001'1111'1100'0000'0001'1110,
      0b1000'0000'1111'1111'1111'1111'1110'0000, 0b0000'0001'1111'1111'1000'0000'0111'1111,
      0b1101'1111'1111'1000'0000'0000'0000'0110, 0b0000'0000'0011'1000'0000'0000'0001'1001,
      0b0000'0000'0000'1110'0000'0000'0000'0000, 0b1111'1111'0000'0001'1111'1111'1111'1110,
      0b1111'1111'1011'1101'0111'1111'0000'0011, 0b1111'1111'1000'0011'1111'1111'1011'1111,
      0b1111'0110'1111'1110'1111'1111'1000'0001, 0b1000'0010'0000'0100'1110'0110'0111'1101,
      0b1000'0000'0000'1111'1000'1111'1110'0111, 0b0111'1111'1111'1111'1010'0101'1111'1111,
      0b0000'0000'0110'1111'1111'1101'1110'1001, 0b0000'0001'1111'0000'0000'0000'0000'0000,
      0b1111'1111'1111'0000'0011'1111'1111'0000, 0b1001'0011'1111'1000'0000'0000'0111'1111,
      0b1111'1111'1111'1111'1111'1011'0111'1111, 0b1111'1111'1100'0000'0001'1111'1100'1101,
      0b1100'0000'1111'1111'1100'0000'0000'0000, 0b1100'0000'0001'1111'1111'1111'1011'1111,
      0b1110'1101'0001'1111'1111'1111'1111'1111, 0b0000'0000'0011'1111'0110'1111'1111'1111,
      0b1111'1111'1100'0000'0000'0000'0100'0000, 0b1111'1111'1110'0000'0000'0000'1111'1111,
      0b1111'1111'1110'0000'0000'0011'1110'1111, 0b0111'1111'1110'0000'0000'0111'1110'0111,
      0b1111'1100'0001'1111'1111'1111'1111'1111, 0b0000'0011'1111'1111'1111'1110'0000'0011,
      0b1111'0111'1000'0000'0000'0000'0110'0000, 0b0000'0010'0000'0000'0000'0011'1011'0111,
      0b0011'1100'0000'0000'1001'1100'0000'0000, 0b1111'1000'0011'1111'1111'1111'1100'0000,
      0b1111'1001'1111'1111'1000'0000'1111'1111, 0b1111'1111'1100'0000'0000'0000'0111'1111,
      0b0000'0111'1110'0000'0000'0000'0001'1111, 0b1111'1111'0111'1111'0000'0000'0000'0000,
      0b1111'1000'1111'1111'1111'1001'1111'1111, 0b1000'0000'0000'0000'0110'0001'1111'1111,
      0b1100'0011'1111'1111'1111'1111'1111'1111, 0b1001'1111'1111'1111'1101'1011'1101'1111,
      0b1111'1111'0000'0000'0111'1111'1111'1111, 0b0000'0000'1100'0011'1111'1111'0000'1111,
      0b0000'0000'1111'1111'1111'1110'0000'0000, 0b1111'1111'0000'0000'0000'0011'0000'0000,
      0b0000'0010'0001'1111'1111'1111'0001'1111, 0b1111'1110'0111'1111'1111'1110'0000'0000,
      0b1110'0111'1111'1111'1111'0000'0001'1111, 0b0000'0000'0011'1111'1110'0000'0111'1111,
      0b0000'0000'0000'1111'1111'1111'1111'0000,
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

} // namespace webpp::unicode::idna::details

#endif // WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP
