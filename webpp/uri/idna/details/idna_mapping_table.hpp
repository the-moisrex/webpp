
/**
 * Attention: Auto-generated file, don't modify.
 *
 *   Auto generated from:          generate_idna_mapping_table2.mjs
 *   IDNA Creation Date:           2024-07-03, 21:52:28 GMT
 *   This file's generation date:  Thu, 16 Jan 2025 21:06:32 GMT
 *   IDNA Mapping Table Version:   16.0.0
 *   Size:                         59746 B
 *                                 58.35 KiB
 *
 * Details about the contents of this file can be found here:
 *   UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 */

#ifndef WEBPP_URI_IDNA_MAPPING_TABLE_HPP
#define WEBPP_URI_IDNA_MAPPING_TABLE_HPP

#include <array>
#include <cstdint>
#include <string_view>

namespace webpp::uri::idna::details {

    static constexpr std::uint16_t magic_rem       = 1U;
    static constexpr auto          last_disallowed = static_cast<char32_t>(0xe01f0);
    static constexpr std::uint8_t  batch_bit_count = 8U;
    static constexpr std::uint8_t  batch_mask      = 0xFFU;

    [[maybe_unused]] static constexpr std::uint16_t not_mapped = 0b1000'0000'0000'0000U;
    static constexpr std::uint16_t                  valid      = 0b1000'0000'0000'0001U;
    static constexpr std::uint16_t                  disallowed = 0b1000'0000'0000'0010U;

    // Pick the table with this mask (between bools table and the block table)
    static constexpr std::uint16_t table_pick_mask = 0b1000'0000'0000'0000U;
    static constexpr auto          blt             = table_pick_mask; // shortcut

    /**
     * IDNA Reference Table.
     *
     * Table size: 7172 B or 7.00 KiB
     */
    static constexpr std::array<std::uint16_t, 3586ULL> idna_refs{
      0x0U,          0x1U,          0x2U,          0x3U,          0x4U,          0x5U,          0x6U,
      blt | 0x0U,    blt | 0x100U,  0x7U,          0x8U,          0x9U,          blt | 0x200U,  blt | 0x300U,
      0xAU,          0xBU,          0xCU,          0xDU,          blt | 0x400U,  0xEU,          blt | 0x500U,
      blt | 0x4D8U,  blt | 0x600U,  0xFU,          0x10U,         blt | 0x700U,  blt | 0x800U,  blt | 0x900U,
      0x11U,         0x12U,         0x13U,         0x14U,         0x15U,         0x16U,         0x17U,
      0x18U,         0x19U,         blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      0x1AU,         blt | 0xA00U,  0x1BU,         0x1CU,         0x1DU,         0x1EU,         0x1FU,
      0x20U,         0x21U,         0x22U,         blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0xB00U,  blt | 0x4D8U,  0x23U,         0x24U,
      blt | 0xC00U,  blt | 0xD00U,  blt | 0xE00U,  0x25U,         blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0xF00U,  blt | 0x1000U,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  0x26U,         0x27U,         0x28U,
      0x29U,         0x2AU,         0x2BU,         0x2CU,         blt | 0x1100U, blt | 0x1200U, blt | 0x1300U,
      blt | 0x1400U, 0x2DU,         0x2EU,         blt | 0x4D8U,  0x2FU,         blt | 0x1500U, blt | 0x1600U,
      blt | 0x1700U, blt | 0x1800U, 0x30U,         0x31U,         blt | 0x1900U, blt | 0x1A00U, blt | 0x1B00U,
      blt | 0x1C00U, blt | 0x1D00U, blt | 0x1E00U, blt | 0x1F00U, blt | 0x2000U, blt | 0x2100U, blt | 0x2200U,
      0x32U,         blt | 0x2300U, blt | 0x2400U, blt | 0x2500U, blt | 0x2600U, blt | 0x2700U, blt | 0x2800U,
      blt | 0x2900U, blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x2A00U, blt | 0x2B00U, blt | 0x2C00U,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0x2D00U, blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x2E00U, blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x2F00U, blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x2BFDU, blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0x2C0AU, blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x3000U, blt | 0x3100U,
      blt | 0xFFCU,  blt | 0x3200U, 0x33U,         blt | 0x3300U, blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x3400U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x3500U, blt | 0xFF3U,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0x3600U, blt | 0x4D8U,  blt | 0x3700U,
      blt | 0x2EFFU, blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  0x34U,         blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  0x35U,         blt | 0x4D8U,
      blt | 0x2B90U, blt | 0x3800U, blt | 0x3900U, 0x36U,         blt | 0x3A00U, blt | 0x3B00U, 0x37U,
      0x38U,         0x39U,         0x3AU,         blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x3C00U, blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0x3D00U, 0x3BU,         blt | 0x3E00U, blt | 0x3F00U,
      blt | 0xFFCU,  blt | 0x4000U, blt | 0x4100U, blt | 0xFFCU,  blt | 0x4200U, blt | 0x4300U, 0x3CU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0x4400U, blt | 0x4500U, 0x3DU,         blt | 0xFFCU,  blt | 0x4600U,
      0x3EU,         0x3FU,         blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4700U, blt | 0x4800U,
      blt | 0x4900U, blt | 0x4D8U,  blt | 0x4A00U, 0x40U,         blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4B00U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4C00U, blt | 0x4D00U, blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4E00U, blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4F00U, blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x2BE6U, blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  0x41U,         0x42U,         0x43U,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x5000U, blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,
      blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x4D8U,  blt | 0x2B94U, blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,  blt | 0xFFCU,
      blt | 0xFFCU,  0x44U};


    /**
     * IDNA Reference Blocks Table (for valid or disallowed values only).
     *
     *  - true:  valid
     *  - false: disallowed
     *
     * Table size: 2592 B or 2.53 KiB
     */
    static constexpr std::array<std::uint32_t, 648ULL> idna_ref_bools{
      0b1111'1111'1111'1111'0011'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1110'0111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0011'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1110'0111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0111'1111'1111'1111'0011'1111'1111'1111U,
      0b0100'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'0111'1111'1111U,
      0b1111'1111'1000'0000'0111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1011U,
      0b1111'1111'1111'1101'1101'1111'1111'1111U, 0b1111'0011'1111'1111'1111'1101'1111'1111U,
      0b0010'0111'0110'0000'0011'1101'1101'1111U, 0b1111'1111'1000'0000'1111'1111'1100'1111U,
      0b1111'1111'1111'1101'1101'1111'1111'1111U, 0b1111'0011'1110'1111'1111'1101'1111'1111U,
      0b0110'0000'0110'0000'0011'1101'1101'1111U, 0b0000'0000'0000'1110'1111'1111'1100'1111U,
      0b1111'1111'1111'1101'1101'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'0000'1111'1101'1101'1111U, 0b1111'1111'1111'1111'1111'1111'1100'1111U,
      0b1111'1100'0111'1111'1111'1111'1110'1110U, 0b0010'1111'1111'1011'1111'1111'1111'1111U,
      0b1111'1111'0101'1111'1000'0100'0111'1111U, 0b0000'0000'0001'1100'1111'1111'1100'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0011'1101'0111'1111'0011'1101'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0011'1101'1111'1111U, 0b0111'1111'0011'1101'1111'1111'1111'1111U,
      0b1111'1111'0111'1111'1111'1111'0011'1101U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0001'1111'1111'1111'1111'1111'1111'1110U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0001'1111'1111'1111'1111'1111'1111U,
      0b0111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'1111'1111'1111'0000'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'0001U, 0b0000'0000'0001'1111'0011'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'1111'1111'1111U,
      0b1100'0111'1111'1111'0000'0011'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1100'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0111'1111'1111'1111'1111'1111'1111'1111U, 0b1001'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'0000'0011'1111'1111U, 0b1111'1111'1111'1111'0011'1111'1111'1111U,
      0b0000'0000'0000'0000'0111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1101'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'0000'0000'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1100'1111'1111'1111'1111'1111U,
      0b1111'1111'1011'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0001'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0000'0111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0011'1111'1111'0001'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'1100'0000'0011'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1000'0000'0000'1111'1111'1111'1111'1111U, 0b0001'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1100'0011'1111'1111'1011'1111'1111'1111U, 0b0111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0111'1111'1111'1111'1111'1111U,
      0b1111'0011'1111'1111'0011'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1000'0000'0000'0000'0000'0000'0111U, 0b0000'0000'0111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'0000'0000'1111U,
      0b1111'1111'1111'1111'1111'1000'0111'1111U, 0b0000'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1110'1111'1111'1111U, 0b1011'0111'1111'1111'1111'1111'0111'1111U,
      0b0011'1111'1111'1111'0011'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1000'0111U, 0b1111'1111'1000'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0001'1111'1111'1111'0111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0001U,
      0b1111'1111'1111'1111'0000'0000'0000'0000U, 0b0011'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0001'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0001'1111'1111'1111'1111U, 0b0000'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1110'0000'0000'1111U,
      0b1111'1111'1111'1111'0000'0111'1111'1111U, 0b0000'0111'1111'1111'1111'1111'1111'1111U,
      0b1011'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0011'1111'1111'1111'0000'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1101'0011'1111U, 0b1001'0001'1011'1111'1111'1111'1111'1111U,
      0b1111'1111'1011'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'1111'1111'1000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b1111'1000'0011'0111'1111'1111'1111'1111U,
      0b1000'1111'1111'1111'1111'1111'1111'1111U, 0b1000'0011'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'0000'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1100'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1110'1110'1111'1111'0000'0110'1111U, 0b1000'0111'0011'1111'1111'1111'1111'1111U,
      0b0000'0001'1111'1111'0000'0001'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0111'1111'1111'1000'0111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1110'0011'1111'1111'1111'1111'1111U,
      0b1111'1111'0011'1111'1111'1111'1111'1111U, 0b1111'1111'0000'0111'1111'1111'1111'1111U,
      0b0001'1110'0000'0011'1111'1111'1111'1111U, 0b0000'0000'0000'0000'1111'1110'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0011'0011'1011'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0001'1100U, 0b1111'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'0000'1111'1111U,
      0b0000'0011'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0011'1111'1111U, 0b1111'1111'1111'1111'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'1111'1111'1111U, 0b0000'0000'0111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1100'0011'1111'1111'1111U, 0b1000'0000'0011'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1101'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0000'0000'0111U, 0b0000'0011'1111'1111'0000'0001'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1101'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0000'1111'1111U, 0b0000'0000'0111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0001'1111'1111'1111'1111'1110U,
      0b1111'1111'1111'1011'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0011U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1011'1111'1111'1111'1011'1101'0111'1111U, 0b1111'1111'1111'1111'0000'0011'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0011'1111'1111'0000'0111'1111'1111U,
      0b1111'1111'1111'1001'1001'1111'1110'1111U, 0b1111'1011'1110'1101'1111'1101'1111'1111U,
      0b1110'0000'1000'0001'0011'1001'1001'1111U, 0b0000'0000'0001'1111'0001'1111'1100'1111U,
      0b1111'1111'1111'1111'0100'1011'1111'1111U, 0b1111'1111'1011'1111'1111'1111'1111'1111U,
      0b0000'0001'1011'1111'1111'0111'1010'0101U, 0b0000'0000'0000'0000'0000'0000'0000'0110U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1110'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0011U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'0000'0000'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'0011'1111'1111'1111'1111'1111U,
      0b0011'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'0000'0000'0001'1111U, 0b0000'0000'0000'0000'0001'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0011'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0011'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'1111U,
      0b1110'0111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'0110'1111'1111'0010'0111'1111U, 0b1111'1001'1011'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'0000'0000'0111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b1111'1111'1111'1111'1111'1100'1111'1111U,
      0b1111'1100'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0001'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0000'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'0000'0000'0111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0001'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0011'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0011'1111'1111'0000'0000'0000'0011U,
      0b1111'1111'1111'1111'1111'1101'1111'1111U, 0b1111'1111'0111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0000'0011'1111U, 0b1111'1111'1111'1111'0001'1111'1111'1111U,
      0b1111'1111'1111'1100'1111'1111'1111'1111U, 0b0000'0000'0111'1111'1111'1110'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1011'0111'1111U, 0b1011'0100'0111'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'0000'0000'1111'1111U, 0b1111'1111'1111'1111'1111'1101'1011'1111U,
      0b0000'0001'1111'1011'0111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0011'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0001'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1101'1111'1111'1111'1111U, 0b1100'0111'1111'1111'1111'1111'1111'1111U,
      0b0000'0111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0001'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1000'0000'0000'0011'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0001'1111'0111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'0000'0000'0000'0000U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'1111'1111'1111'1111U,
      0b0000'0000'0011'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0001'1111'1111'1111'1111'1111'1111U,
      0b0111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1100'0011'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0011'1111'1111U, 0b0000'0000'0011'1111'0011'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1011'1111'1111'0000'0000'0011'1111U, 0b1110'0000'1111'1111'1111'1111'1111'1011U,
      0b0000'0000'0000'0000'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0011'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1000'0111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1000'0000'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0011'0000'0000'0001'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0011'1111'1111'1111'1111'1111U, 0b1000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0110'1111'1110'1111'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0100'0000'0000'0000'0111U,
      0b0000'0000'0010'0111'0000'0000'0000'0000U, 0b1111'1111'1111'1111'0000'0000'1111'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0011'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0000'0111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0011'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0011'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'1111'1111'1111'1111'1111U, 0b0000'0000'0000'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0111'1111'1111'1111'1111'1111U, 0b0000'0001'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1000'0000'0000'0000'1111'1111'1111U, 0b0000'0000'0000'0000'1111'1111'1111'1110U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0111'1110'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0011'1111'1111'1111'0001'1111'1111'1111U,
      0b0000'0000'0000'0000'1100'0011'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1000'0011'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'0000'0000'0000'0000U, 0b0000'0011'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'0000'0000'0000'0000U, 0b1000'0111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0111'1111'1111'1111'0110'1111'0111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0111'1111'1111'1111'1001'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b1111'1111'1111'1110'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0001'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1110U, 0b0011'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b0000'0000'0000'0000'0000'0000'0000'0000U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1110'0111'1111'1111'1111U,
      0b1111'1111'1111'1110'1111'1111'1111'1110U, 0b0000'0000'0011'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'0000'1111'1111'1111'1111'1111'1111U, 0b0001'1111'1111'1111'0001'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1000'0111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0001'0000'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0011'1111'1111'0000'0000'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0000'1111'1111U, 0b0000'1111'1111'1111'0011'1111'1111'1111U,
      0b0000'0000'0000'0000'0000'0000'0000'0011U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0000'0000'0000'1111'1111'1111'1111'1111U, 0b0001'1111'1111'1111'0011'1111'1111'1111U,
      0b1111'1111'1111'1111'1000'0011'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1001'1111'1111'1111'1100'0000'0111'1111U, 0b0000'0001'1111'1111'0000'0011'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0000'0000'0000'0000'0000'0000'0000U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b0000'0011'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b0011'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'0000'0000'0011U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'0000'0000'0000'0001U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'0000'0111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U,
      0b1111'1111'1111'1111'1111'1111'1111'1111U, 0b1111'1111'1111'1111'1111'1111'1111'1111U};

    /**
     * IDNA Reference Blocks Table.
     *
     * Table size: 35328 B or 34.50 KiB
     */
    static constexpr std::array<std::array<std::uint16_t, 256ULL>, 69ULL> idna_ref_blocks{
      {


       // Block #0
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      0U,         2U,         4U,         6U,         8U,         10U,        12U,
         14U,        16U,        18U,        20U,        22U,        24U,        26U,        28U,
         30U,        32U,        34U,        36U,        38U,        40U,        42U,        44U,
         46U,        48U,        50U,        valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         52U,        valid,      valid,      valid,      valid,      valid,      valid,      valid,
         54U,        valid,      0U,         valid,      valid,      58U,        valid,      59U,
         valid,      valid,      63U,        65U,        67U,        71U,        valid,      valid,
         74U,        78U,        28U,        valid,      80U,        86U,        92U,        valid,
         98U,        101U,       104U,       107U,       110U,       113U,       116U,       119U,
         122U,       125U,       128U,       131U,       134U,       137U,       140U,       143U,
         146U,       149U,       152U,       155U,       158U,       161U,       164U,       valid,
         167U,       170U,       173U,       176U,       179U,       182U,       185U,       valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid},

       // Block #1
        {188U,  valid, 191U,  valid, 194U,  valid, 197U,  valid, 200U,  valid, 203U,  valid, 206U,  valid,
         209U,  valid, 212U,  valid, 215U,  valid, 218U,  valid, 221U,  valid, 224U,  valid, 227U,  valid,
         230U,  valid, 233U,  valid, 236U,  valid, 239U,  valid, 242U,  valid, 245U,  valid, 248U,  valid,
         251U,  valid, 254U,  valid, 257U,  valid, 260U,  valid, 264U,  264U,  267U,  valid, 270U,  valid,
         valid, 273U,  valid, 276U,  valid, 279U,  valid, 282U,  282U,  286U,  valid, 289U,  valid, 292U,
         valid, 295U,  valid, 298U,  302U,  valid, 305U,  valid, 308U,  valid, 311U,  valid, 314U,  valid,
         317U,  valid, 320U,  valid, 323U,  valid, 326U,  valid, 329U,  valid, 332U,  valid, 335U,  valid,
         338U,  valid, 341U,  valid, 344U,  valid, 347U,  valid, 350U,  valid, 353U,  valid, 356U,  valid,
         359U,  valid, 362U,  valid, 365U,  valid, 368U,  valid, 371U,  374U,  valid, 377U,  valid, 380U,
         valid, 36U,   valid, 383U,  386U,  valid, 389U,  valid, 392U,  395U,  valid, 398U,  401U,  404U,
         valid, valid, 407U,  410U,  413U,  416U,  valid, 419U,  422U,  valid, 425U,  428U,  431U,  valid,
         valid, valid, 434U,  437U,  valid, 440U,  443U,  valid, 446U,  valid, 449U,  valid, 452U,  455U,
         valid, 458U,  valid, valid, 461U,  valid, 464U,  467U,  valid, 470U,  473U,  476U,  valid, 479U,
         valid, 482U,  485U,  valid, valid, valid, 488U,  valid, valid, valid, valid, valid, valid, valid,
         491U,  491U,  491U,  495U,  495U,  495U,  498U,  498U,  498U,  501U,  valid, 504U,  valid, 507U,
         valid, 510U,  valid, 513U,  valid, 516U,  valid, 519U,  valid, 522U,  valid, valid, 525U,  valid,
         528U,  valid, 531U,  valid, 534U,  valid, 537U,  valid, 540U,  valid, 543U,  valid, 546U,  valid,
         549U,  valid, valid, 552U,  552U,  552U,  555U,  valid, 558U,  561U,  564U,  valid, 567U,  valid,
         570U,  valid, 573U,  valid},

       // Block #2
        {576U,  valid, 579U,  valid, 582U,  valid, 585U,  valid, 588U,  valid, 591U,  valid, 594U,  valid,
         597U,  valid, 600U,  valid, 603U,  valid, 606U,  valid, 609U,  valid, 612U,  valid, 615U,  valid,
         618U,  valid, 621U,  valid, 624U,  valid, 627U,  valid, 630U,  valid, 633U,  valid, 636U,  valid,
         639U,  valid, 642U,  valid, 645U,  valid, 648U,  valid, 651U,  valid, valid, valid, valid, valid,
         valid, valid, 654U,  658U,  valid, 661U,  664U,  valid, valid, 668U,  valid, 671U,  674U,  677U,
         680U,  valid, 683U,  valid, 686U,  valid, 689U,  valid, 692U,  valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, 14U,   695U,  18U,   34U,   698U,  701U,
         704U,  44U,   48U,   valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, 707U,  711U,  715U,  719U,  723U,  727U,  valid, valid,
         422U,  22U,   36U,   46U,   731U,  valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid},

       // Block #3
        {valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      734U,       737U,
         valid,      740U,  743U,  748U,  valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, 58U,   valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, 751U,  valid, 754U,  valid, 757U,  valid,      760U,       valid,      disallowed,
         disallowed, 763U,  valid, valid, valid, 767U,  769U,  disallowed, disallowed, disallowed, disallowed,
         67U,        772U,  778U,  781U,  784U,  787U,  790U,  disallowed, 793U,       disallowed, 796U,
         799U,       valid, 802U,  805U,  808U,  811U,  814U,  817U,       820U,       823U,       748U,
         826U,       829U,  71U,   832U,  835U,  838U,  841U,  844U,       disallowed, 847U,       850U,
         853U,       856U,  859U,  862U,  865U,  868U,  871U,  valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      874U,       805U,
         823U,       853U,  796U,  871U,  856U,  841U,  valid, 877U,       valid,      880U,       valid,
         883U,       valid, 886U,  valid, 889U,  valid, 892U,  valid,      895U,       valid,      898U,
         valid,      901U,  valid, 904U,  valid, 907U,  valid, 910U,       valid,      826U,       844U,
         847U,       valid, 823U,  814U,  valid, 913U,  valid, 847U,       916U,       valid,      valid,
         919U,       922U,  925U},

       // Block #4
        {928U,  931U,  934U,  937U,  940U,  943U,  946U,  949U,  952U,  955U,  958U,  961U,  964U,  967U,
         970U,  973U,  976U,  979U,  982U,  985U,  988U,  991U,  994U,  997U,  1000U, 1003U, 1006U, 1009U,
         1012U, 1015U, 1018U, 1021U, 1024U, 1027U, 1030U, 1033U, 1036U, 1039U, 1042U, 1045U, 1048U, 1051U,
         1054U, 1057U, 1060U, 1063U, 1066U, 1069U, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 1072U, valid,
         1075U, valid, 1078U, valid, 1081U, valid, 1084U, valid, 1087U, valid, 1090U, valid, 1093U, valid,
         1096U, valid, 1099U, valid, 1102U, valid, 1105U, valid, 1108U, valid, 1111U, valid, 1114U, valid,
         1117U, valid, 1120U, valid, valid, valid, valid, valid, valid, valid, valid, valid, 1123U, valid,
         1126U, valid, 1129U, valid, 1132U, valid, 1135U, valid, 1138U, valid, 1141U, valid, 1144U, valid,
         1147U, valid, 1150U, valid, 1153U, valid, 1156U, valid, 1159U, valid, 1162U, valid, 1165U, valid,
         1168U, valid, 1171U, valid, 1174U, valid, 1177U, valid, 1180U, valid, 1183U, valid, 1186U, valid,
         1189U, valid, 1192U, valid, 1195U, valid, 1198U, valid, 1201U, valid, 1204U, 1207U, valid, 1210U,
         valid, 1213U, valid, 1216U, valid, 1219U, valid, 1222U, valid, 1225U, valid, valid, 1228U, valid,
         1231U, valid, 1234U, valid, 1237U, valid, 1240U, valid, 1243U, valid, 1246U, valid, 1249U, valid,
         1252U, valid, 1255U, valid, 1258U, valid, 1261U, valid, 1264U, valid, 1267U, valid, 1270U, valid,
         1273U, valid, 1276U, valid, 1279U, valid, 1282U, valid, 1285U, valid, 1288U, valid, 1291U, valid,
         1294U, valid, 1297U, valid},

       // Block #5
        {1300U,      valid,      1303U,      valid,      1306U,      valid,      1309U,      valid,
         1312U,      valid,      1315U,      valid,      1318U,      valid,      1321U,      valid,
         1324U,      valid,      1327U,      valid,      1330U,      valid,      1333U,      valid,
         1336U,      valid,      1339U,      valid,      1342U,      valid,      1345U,      valid,
         1348U,      valid,      1351U,      valid,      1354U,      valid,      1357U,      valid,
         1360U,      valid,      1363U,      valid,      1366U,      valid,      1369U,      valid,
         disallowed, 1372U,      1375U,      1378U,      1381U,      1384U,      1387U,      1390U,
         1393U,      1396U,      1399U,      1402U,      1405U,      1408U,      1411U,      1414U,
         1417U,      1420U,      1423U,      1426U,      1429U,      1432U,      1435U,      1438U,
         1441U,      1444U,      1447U,      1450U,      1453U,      1456U,      1459U,      1462U,
         1465U,      1468U,      1471U,      1474U,      1477U,      1480U,      1483U,      disallowed,
         disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      1486U,
         valid,      valid,      valid,      disallowed, disallowed, valid,      valid,      valid,
         disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #6
        {disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      disallowed, valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         1491U,      1496U,      1501U,      1506U,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid, valid, valid,
         valid,      valid,      valid,      valid},

       // Block #7
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         1511U,      1518U,      1525U,      1532U,      1539U,      1546U,      1553U,      1560U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, disallowed, valid,
         valid,      disallowed, disallowed, valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, valid,      disallowed, disallowed, disallowed, valid,      valid,
         valid,      valid,      disallowed, disallowed, valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, disallowed, valid,
         valid,      disallowed, disallowed, valid,      valid,      valid,      valid,      disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
         disallowed, disallowed, disallowed, disallowed, 1567U,      1574U,      disallowed, 1581U,
         valid,      valid,      valid,      valid,      disallowed, disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed},

       // Block #8
        {disallowed, valid,      valid,      valid,      disallowed, valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, valid,
         valid,      disallowed, disallowed, valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, valid,      1588U,      disallowed, valid,      1595U,      disallowed,
         valid,      valid,      disallowed, disallowed, valid,      disallowed, valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, valid,
         valid,      disallowed, disallowed, valid,      valid,      valid,      disallowed, disallowed,
         disallowed, valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, 1602U,      1609U,      1616U,      valid,      disallowed, 1623U,      disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, valid,      valid,      valid,      disallowed, valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, valid,
         valid,      valid,      disallowed, valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, valid,      valid,      disallowed, valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, valid,
         valid,      valid,      disallowed, valid,      valid,      valid,      disallowed, disallowed,
         valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      disallowed, disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid},

       // Block #9
        {disallowed, valid,      valid,      valid,      disallowed, valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, disallowed, valid,
         valid,      disallowed, disallowed, valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, valid,      valid,      disallowed, valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, disallowed, valid,
         valid,      disallowed, disallowed, valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, 1630U,      1637U,      disallowed, valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, valid,      valid,      disallowed, valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, valid,      valid,
         valid,      disallowed, valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, valid,      valid,      disallowed, valid,      disallowed, valid,      valid,
         disallowed, disallowed, disallowed, valid,      valid,      disallowed, disallowed, disallowed,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, valid,      valid,
         valid,      disallowed, valid,      valid,      valid,      valid,      disallowed, disallowed,
         valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #10
        {disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      1644U,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, valid,      valid,      disallowed, valid,      disallowed, valid,      valid,
         valid,      valid,      valid,      disallowed, valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, valid,      disallowed, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      1651U,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      disallowed, valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, 1658U,      1665U,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #11
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      1672U,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      1676U,      valid,      valid,      valid,      valid,
         disallowed, valid,      valid,      valid,      valid,      1683U,      valid,      valid,
         valid,      valid,      1690U,      valid,      valid,      valid,      valid,      1697U,
         valid,      valid,      valid,      valid,      1704U,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      1711U,      valid,      valid,      valid,      disallowed, disallowed, disallowed,
         disallowed, valid,      valid,      1718U,      valid,      1725U,      1732U,      1739U,
         1749U,      1756U,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      1766U,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      1773U,      valid,      valid,      valid,      valid,
         disallowed, valid,      valid,      valid,      valid,      1780U,      valid,      valid,
         valid,      valid,      1787U,      valid,      valid,      valid,      valid,      1794U,
         valid,      valid,      valid,      valid,      1801U,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      1808U,      valid,      valid,      valid,      disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #12
        {valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      1815U, 1819U,
         1823U,      1827U, 1831U,      1835U,      1839U,      1843U,      1847U,      1851U, 1855U,
         1859U,      1863U, 1867U,      1871U,      1875U,      1879U,      1883U,      1887U, 1891U,
         1895U,      1899U, 1903U,      1907U,      1911U,      1915U,      1919U,      1923U, 1927U,
         1931U,      1935U, 1939U,      1943U,      1947U,      1951U,      1955U,      1959U, 1963U,
         disallowed, 1967U, disallowed, disallowed, disallowed, disallowed, disallowed, 1971U, disallowed,
         disallowed, valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
         1975U,      valid, valid,      valid},

       // Block #13
        {valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 58U,   58U,   valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid},

       // Block #14
        {valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, disallowed, valid,      valid,      valid,      valid,      disallowed, disallowed,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      disallowed, disallowed, valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid, valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         1979U, 1983U,      1987U,      1991U,      1995U,      1999U,      disallowed, disallowed},

       // Block #15
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, valid,      valid,
         valid,      disallowed, valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      58U,        58U,        valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #16
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      58U,        58U,        58U,        58U,        58U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #17
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         982U,       988U,       1018U,      1027U,      1030U,      1030U,      1054U,      1075U,
         2003U,      2007U,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         2011U,      2015U,      2019U,      2023U,      2027U,      2031U,      2035U,      2039U,
         2043U,      2047U,      2051U,      2055U,      1975U,      2059U,      2063U,      2067U,
         2071U,      2075U,      2079U,      2083U,      2087U,      2091U,      2095U,      2099U,
         2103U,      2107U,      2111U,      2115U,      2119U,      2123U,      2127U,      2131U,
         2135U,      2139U,      2143U,      2147U,      2151U,      2155U,      2159U,      2163U,
         2167U,      2171U,      2175U,      disallowed, disallowed, 2179U,      2183U,      2187U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #18
        {valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, 0U,    116U,  2U,    valid, 6U,    8U,    407U,  12U,   14U,   16U,   18U,   20U,
         22U,   24U,   26U,   valid, 28U,   627U,  30U,   34U,   38U,   40U,   44U,   0U,    2191U, 2194U,
         2197U, 2U,    6U,    8U,    410U,  413U,  2201U, 12U,   valid, 20U,   24U,   302U,  28U,   392U,
         2204U, 2208U, 30U,   38U,   40U,   2212U, 434U,  42U,   2216U, 805U,  808U,  811U,  856U,  859U,
         16U,   34U,   40U,   42U,   805U,  808U,  844U,  856U,  859U,  valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, 1015U, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, 2220U, 4U,    2223U, 146U,  2201U, 10U,   2226U, 2229U, 2232U, 428U,  425U,  2235U, 2238U,
         2242U, 2245U, 2248U, 2252U, 2255U, 2258U, 437U,  2261U, 2264U, 440U,  2267U, 2270U, 458U,  2273U,
         674U,  470U,  2276U, 473U,  677U,  50U,   2280U, 2283U, 482U,  823U,  valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid},

       // Block #19
        {2286U, valid, 2290U, valid, 2294U, valid, 2298U, valid, 2302U, valid, 2306U, valid, 2310U, valid,
         2314U, valid, 2318U, valid, 2322U, valid, 2326U, valid, 2330U, valid, 2334U, valid, 2338U, valid,
         2342U, valid, 2346U, valid, 2350U, valid, 2354U, valid, 2358U, valid, 2362U, valid, 2366U, valid,
         2370U, valid, 2374U, valid, 2378U, valid, 2382U, valid, 2386U, valid, 2390U, valid, 2394U, valid,
         2398U, valid, 2402U, valid, 2406U, valid, 2410U, valid, 2414U, valid, 2418U, valid, 2422U, valid,
         2426U, valid, 2430U, valid, 2434U, valid, 2438U, valid, 2442U, valid, 2446U, valid, 2450U, valid,
         2454U, valid, 2458U, valid, 2462U, valid, 2466U, valid, 2470U, valid, 2474U, valid, 2478U, valid,
         2482U, valid, 2486U, valid, 2490U, valid, 2494U, valid, 2498U, valid, 2502U, valid, 2506U, valid,
         2510U, valid, 2514U, valid, 2518U, valid, 2522U, valid, 2526U, valid, 2530U, valid, 2534U, valid,
         2538U, valid, 2542U, valid, 2546U, valid, 2550U, valid, 2554U, valid, 2558U, valid, 2562U, valid,
         2566U, valid, 2570U, valid, 2574U, valid, 2578U, valid, 2582U, valid, valid, valid, valid, valid,
         2586U, 2478U, valid, valid, 2590U, valid, 2593U, valid, 2597U, valid, 2601U, valid, 2605U, valid,
         2609U, valid, 2613U, valid, 2617U, valid, 2621U, valid, 2625U, valid, 2629U, valid, 2633U, valid,
         2637U, valid, 2641U, valid, 2645U, valid, 2649U, valid, 2653U, valid, 2657U, valid, 2661U, valid,
         2665U, valid, 2669U, valid, 2673U, valid, 2677U, valid, 2681U, valid, 2685U, valid, 2689U, valid,
         2693U, valid, 2697U, valid, 2701U, valid, 2705U, valid, 2709U, valid, 2713U, valid, 2717U, valid,
         2721U, valid, 2725U, valid, 2729U, valid, 2733U, valid, 2737U, valid, 2741U, valid, 2745U, valid,
         2749U, valid, 2753U, valid, 2757U, valid, 2761U, valid, 2765U, valid, 2769U, valid, 2773U, valid,
         2777U, valid, 2781U, valid},

       // Block #20
        {valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
         2785U,      2789U,      2793U,      2797U, 2801U,      2805U,      2809U,      2813U,
         valid,      valid,      valid,      valid, valid,      valid,      disallowed, disallowed,
         2817U,      2821U,      2825U,      2829U, 2833U,      2837U,      disallowed, disallowed,
         valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
         2841U,      2845U,      2849U,      2853U, 2857U,      2861U,      2865U,      2869U,
         valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
         2873U,      2877U,      2881U,      2885U, 2889U,      2893U,      2897U,      2901U,
         valid,      valid,      valid,      valid, valid,      valid,      disallowed, disallowed,
         2905U,      2909U,      2913U,      2917U, 2921U,      2925U,      disallowed, disallowed,
         valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
         disallowed, 2929U,      disallowed, 2933U, disallowed, 2937U,      disallowed, 2941U,
         valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
         2945U,      2949U,      2953U,      2957U, 2961U,      2965U,      2969U,      2973U,
         valid,      778U,       valid,      784U,  valid,      787U,       valid,      790U,
         valid,      793U,       valid,      796U,  valid,      799U,       disallowed, disallowed,
         2977U,      2983U,      2989U,      2995U, 3001U,      3007U,      3013U,      3019U,
         2977U,      2983U,      2989U,      2995U, 3001U,      3007U,      3013U,      3019U,
         3025U,      3031U,      3037U,      3043U, 3049U,      3055U,      3061U,      3067U,
         3025U,      3031U,      3037U,      3043U, 3049U,      3055U,      3061U,      3067U,
         3073U,      3079U,      3085U,      3091U, 3097U,      3103U,      3109U,      3115U,
         3073U,      3079U,      3085U,      3091U, 3097U,      3103U,      3109U,      3115U,
         valid,      valid,      3121U,      3127U, 3132U,      disallowed, valid,      3137U,
         3143U,      3147U,      3151U,      778U,  3127U,      3155U,      748U,       3155U,
         3159U,      3163U,      3169U,      3175U, 3180U,      disallowed, valid,      3185U,
         3191U,      784U,       3195U,      787U,  3175U,      3199U,      3205U,      3211U,
         valid,      valid,      valid,      3217U, disallowed, disallowed, valid,      valid,
         3220U,      3224U,      3228U,      790U,  disallowed, 3232U,      3238U,      3244U,
         valid,      valid,      valid,      3250U, valid,      valid,      valid,      valid,
         3253U,      3257U,      3261U,      796U,  3265U,      3269U,      772U,       3275U,
         disallowed, disallowed, 3277U,      3283U, 3288U,      disallowed, valid,      3293U,
         3299U,      793U,       3303U,      799U,  3283U,      67U,        3307U,      disallowed},

       // Block #21
        {52U,        52U,        52U,        52U,        52U,        52U,        52U,        52U,
         52U,        52U,        52U,        58U,        valid,      valid,      disallowed, disallowed,
         valid,      3311U,      valid,      valid,      valid,      valid,      valid,      3315U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 52U,
         valid,      valid,      valid,      3319U,      3326U,      valid,      3336U,      3343U,
         valid,      valid,      valid,      valid,      3353U,      valid,      3356U,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      3360U,
         3363U,      3366U,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      3369U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      52U,
         58U,        58U,        58U,        58U,        58U,        disallowed, disallowed, disallowed,
         disallowed, disallowed, 58U,        58U,        58U,        58U,        58U,        58U,
         3382U,      16U,        disallowed, disallowed, 3384U,      3386U,      3388U,      3390U,
         3392U,      3394U,      3396U,      3398U,      3402U,      3404U,      3406U,      26U,
         3382U,      78U,        63U,        65U,        3384U,      3386U,      3388U,      3390U,
         3392U,      3394U,      3396U,      3398U,      3402U,      3404U,      3406U,      disallowed,
         0U,         8U,         28U,        46U,        410U,       14U,        20U,        22U,
         24U,        26U,        30U,        36U,        38U,        disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         3408U,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #22
        {3411U,      3415U, 4U,    3419U, valid, 3423U, 3427U, 413U,  valid,      3431U,      12U,
         14U,        14U,   14U,   14U,   245U,  16U,   16U,   22U,   22U,        valid,      26U,
         3435U,      valid, valid, 30U,   32U,   34U,   34U,   34U,   valid,      valid,      3438U,
         3441U,      3445U, valid, 50U,   valid, 865U,  valid, 50U,   valid,      20U,        113U,
         2U,         4U,    valid, 8U,    8U,    10U,   3448U, 24U,   28U,        3452U,      3455U,
         3458U,      3461U, 16U,   valid, 3464U, 841U,  808U,  808U,  841U,       3468U,      valid,
         valid,      valid, valid, 6U,    6U,    8U,    16U,   18U,   valid,      valid,      valid,
         valid,      valid, valid, 3472U, 3478U, 3484U, 3491U, 3497U, 3503U,      3509U,      3515U,
         3521U,      3527U, 3533U, 3539U, 3545U, 3551U, 3557U, 3563U, 16U,        3568U,      3571U,
         3575U,      42U,   3578U, 3581U, 3585U, 3590U, 46U,   3593U, 3596U,      22U,        4U,
         6U,         24U,   16U,   3568U, 3571U, 3575U, 42U,   3578U, 3581U,      3585U,      3590U,
         46U,        3593U, 3596U, 22U,   4U,    6U,    24U,   valid, valid,      valid,      3600U,
         valid,      valid, valid, valid, valid, 3604U, valid, valid, disallowed, disallowed, disallowed,
         disallowed, valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid, valid, valid, valid, valid, valid, valid,      valid,      valid,
         valid,      valid, valid},

       // Block #23
        {valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, 3610U, 3617U, valid, 3627U, 3634U, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid},

       // Block #24
        {valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 3644U,
         3648U, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid},

       // Block #25
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         78U,        63U,        65U,        3384U,      3386U,      3388U,      3390U,      3392U,
         3394U,      3652U,      3655U,      3658U,      3661U,      3664U,      3667U,      3670U,
         3673U,      3676U,      3679U,      3682U,      3685U,      3689U,      3693U,      3697U,
         3701U,      3705U,      3709U,      3713U,      3717U,      3721U,      3726U,      3731U,
         3736U,      3741U,      3746U,      3751U,      3756U,      3761U,      3766U,      3771U,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, 3776U,      3780U,      3784U,      3788U,
         3792U,      3796U,      3800U,      3804U,      3808U,      3812U,      3816U,      3820U,
         3824U,      3828U,      3832U,      3836U,      3840U,      3844U,      3848U,      3852U,
         3856U,      3860U,      3864U,      3868U,      3872U,      3876U,      0U,         2U,
         4U,         6U,         8U,         10U,        12U,        14U,        16U,        18U,
         20U,        22U,        24U,        26U,        28U,        30U,        32U,        34U,
         36U,        38U,        40U,        42U,        44U,        46U,        48U,        50U,
         0U,         2U,         4U,         6U,         8U,         10U,        12U,        14U,
         16U,        18U,        20U,        22U,        24U,        26U,        28U,        30U,
         32U,        34U,        36U,        38U,        40U,        42U,        44U,        46U,
         48U,        50U,        3382U,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid},

       // Block #26
        {valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 3880U, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, 3893U, 3897U, 3900U, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 3904U, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
         valid, valid, valid, valid},

       // Block #27
        {3910U, 3914U, 3918U,      3922U,      3926U,      3930U,      3934U,      3938U, 3942U, 3946U, 3950U,
         3954U, 3958U, 3962U,      3966U,      3970U,      3974U,      3978U,      3982U, 3986U, 3990U, 3994U,
         3998U, 4002U, 4006U,      4010U,      4014U,      4018U,      4022U,      4026U, 4030U, 4034U, 4038U,
         4042U, 4046U, 4050U,      4054U,      4058U,      4062U,      4066U,      4070U, 4074U, 4078U, 4082U,
         4086U, 4090U, 4094U,      4098U,      valid,      valid,      valid,      valid, valid, valid, valid,
         valid, valid, valid,      valid,      valid,      valid,      valid,      valid, valid, valid, valid,
         valid, valid, valid,      valid,      valid,      valid,      valid,      valid, valid, valid, valid,
         valid, valid, valid,      valid,      valid,      valid,      valid,      valid, valid, valid, valid,
         valid, valid, valid,      valid,      valid,      valid,      valid,      valid, 4102U, valid, 4106U,
         4109U, 4113U, valid,      valid,      4116U,      valid,      4120U,      valid, 4124U, valid, 2194U,
         2255U, 2191U, 2220U,      valid,      4128U,      valid,      valid,      4132U, valid, valid, valid,
         valid, valid, valid,      18U,        42U,        4136U,      4139U,      4142U, valid, 4146U, valid,
         4150U, valid, 4154U,      valid,      4158U,      valid,      4162U,      valid, 4166U, valid, 4170U,
         valid, 4174U, valid,      4178U,      valid,      4182U,      valid,      4186U, valid, 4190U, valid,
         4194U, valid, 4198U,      valid,      4202U,      valid,      4206U,      valid, 4210U, valid, 4214U,
         valid, 4218U, valid,      4222U,      valid,      4226U,      valid,      4230U, valid, 4234U, valid,
         4238U, valid, 4242U,      valid,      4246U,      valid,      4250U,      valid, 4254U, valid, 4258U,
         valid, 4262U, valid,      4266U,      valid,      4270U,      valid,      4274U, valid, 4278U, valid,
         4282U, valid, 4286U,      valid,      4290U,      valid,      4294U,      valid, 4298U, valid, 4302U,
         valid, 4306U, valid,      4310U,      valid,      4314U,      valid,      4318U, valid, 4322U, valid,
         4326U, valid, 4330U,      valid,      4334U,      valid,      4338U,      valid, valid, valid, valid,
         valid, valid, valid,      valid,      4342U,      valid,      4346U,      valid, valid, valid, valid,
         4350U, valid, disallowed, disallowed, disallowed, disallowed, disallowed, valid, valid, valid, valid,
         valid, valid, valid},

       // Block #28
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, valid,      disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 4354U,
         valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid},

       // Block #29
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, valid,      valid,      valid,      valid,      4358U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      4362U,      disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #30
        {4366U,      4370U,      4374U,      4378U,      4382U,      4386U,      4390U,      4394U,
         4398U,      4402U,      4406U,      4410U,      4414U,      4418U,      4422U,      4426U,
         4430U,      4434U,      4438U,      4442U,      4446U,      4450U,      4454U,      4458U,
         4462U,      4466U,      4470U,      4474U,      4478U,      4482U,      4486U,      4490U,
         4494U,      4498U,      4502U,      4506U,      4510U,      4514U,      4518U,      4522U,
         4526U,      4530U,      4534U,      4538U,      4542U,      4546U,      4550U,      4554U,
         4558U,      4562U,      4566U,      4570U,      4574U,      4578U,      4582U,      4586U,
         4590U,      4594U,      4598U,      4602U,      4606U,      4610U,      4614U,      4618U,
         4622U,      4626U,      4630U,      4634U,      4638U,      4642U,      4646U,      4650U,
         4654U,      4658U,      4662U,      4666U,      4670U,      4674U,      4678U,      4682U,
         4686U,      4690U,      4694U,      4698U,      4702U,      4706U,      4710U,      4714U,
         4718U,      4722U,      4726U,      4730U,      4734U,      4738U,      4742U,      4746U,
         4750U,      4754U,      4758U,      4762U,      4766U,      4770U,      4774U,      4778U,
         4782U,      4786U,      4790U,      4794U,      4798U,      4802U,      4806U,      4810U,
         4814U,      4818U,      4822U,      4826U,      4830U,      4834U,      4838U,      4842U,
         4846U,      4850U,      4854U,      4858U,      4862U,      4866U,      4870U,      4874U,
         4878U,      4882U,      4886U,      4890U,      4894U,      4898U,      4902U,      4906U,
         4910U,      4914U,      4918U,      4922U,      4926U,      4930U,      4934U,      4938U,
         4942U,      4946U,      4950U,      4954U,      4958U,      4962U,      4966U,      4970U,
         4974U,      4978U,      4982U,      4986U,      4990U,      4994U,      4998U,      5002U,
         5006U,      5010U,      5014U,      5018U,      5022U,      5026U,      5030U,      5034U,
         5038U,      5042U,      5046U,      5050U,      5054U,      5058U,      5062U,      5066U,
         5070U,      5074U,      5078U,      5082U,      5086U,      5090U,      5094U,      5098U,
         5102U,      5106U,      5110U,      5114U,      5118U,      5122U,      5126U,      5130U,
         5134U,      5138U,      5142U,      5146U,      5150U,      5154U,      5158U,      5162U,
         5166U,      5170U,      5174U,      5178U,      5182U,      5186U,      5190U,      5194U,
         5198U,      5202U,      5206U,      5210U,      5214U,      5218U,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #31
        {52U,   valid, 5222U, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, 5224U, valid,      4458U,      5228U, 5232U, valid,
         valid, valid, valid, valid, disallowed, valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, disallowed, disallowed, valid, valid, 5236U,
         5241U, valid, valid, 5246U, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
         valid, valid, valid, 5253U},

       // Block #32
        {disallowed, disallowed, disallowed, disallowed, disallowed, valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, 5260U,      5264U,      5268U,      5272U,      5276U,      5280U,      5284U,
         5288U,      5292U,      5296U,      5300U,      5304U,      5308U,      5312U,      5316U,
         5320U,      5324U,      5328U,      5332U,      5336U,      5340U,      5344U,      5348U,
         5352U,      5356U,      5360U,      5364U,      5368U,      5372U,      5376U,      5380U,
         5384U,      5388U,      5392U,      5396U,      5400U,      5404U,      5408U,      5412U,
         5416U,      5420U,      5424U,      5428U,      5432U,      5436U,      5440U,      5444U,
         5448U,      5452U,      5456U,      5460U,      58U,        5464U,      5468U,      5472U,
         5476U,      5480U,      5484U,      5488U,      5492U,      5496U,      5500U,      5504U,
         5508U,      5512U,      5516U,      5520U,      5524U,      5528U,      5532U,      5536U,
         5540U,      5544U,      5548U,      5552U,      5556U,      5560U,      5564U,      5568U,
         5572U,      5576U,      5580U,      5584U,      5588U,      5592U,      5596U,      5600U,
         5604U,      5608U,      5612U,      5616U,      5620U,      5624U,      5628U,      disallowed,
         valid,      valid,      4366U,      4390U,      5632U,      5636U,      5640U,      5644U,
         5648U,      5652U,      4382U,      5656U,      5660U,      5664U,      5668U,      4398U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid},

       // Block #33
        {5672U, 5678U, 5684U, 5690U, 5696U, 5702U,      5708U, 5714U, 5720U, 5726U, 5732U, 5738U, 5744U,
         5750U, 5756U, 5762U, 5768U, 5774U, 5780U,      5786U, 5792U, 5798U, 5804U, 5810U, 5816U, 5822U,
         5828U, 5834U, 5840U, 5846U, 5855U, disallowed, 5864U, 5870U, 5876U, 5882U, 5888U, 5894U, 5900U,
         5906U, 5912U, 5918U, 5924U, 5930U, 5936U,      5942U, 5948U, 5954U, 5960U, 5966U, 5972U, 5978U,
         5984U, 5990U, 5996U, 6002U, 6008U, 6014U,      6020U, 6026U, 6032U, 6038U, 6044U, 6050U, 6056U,
         6062U, 6068U, 6074U, 6080U, 6084U, 4630U,      6088U, valid, valid, valid, valid, valid, valid,
         valid, valid, 6092U, 6096U, 6099U, 6102U,      6105U, 6108U, 6111U, 6114U, 6117U, 6120U, 6123U,
         6126U, 6129U, 6132U, 6135U, 6138U, 5260U,      5272U, 5284U, 5292U, 5324U, 5328U, 5340U, 5348U,
         5352U, 5360U, 5364U, 5368U, 5372U, 5376U,      6141U, 6145U, 6149U, 6153U, 6157U, 6161U, 6165U,
         6169U, 6173U, 6177U, 6181U, 6185U, 6189U,      6193U, 6197U, 6204U, 6211U, valid, 4366U, 4390U,
         5632U, 5636U, 6215U, 6219U, 6223U, 4410U,      6227U, 4458U, 4658U, 4706U, 4702U, 4662U, 5030U,
         4490U, 4650U, 6231U, 6235U, 6239U, 6243U,      6247U, 6251U, 6255U, 6259U, 6263U, 6267U, 4514U,
         6271U, 6275U, 6279U, 6283U, 6287U, 6291U,      6295U, 6299U, 5640U, 5644U, 5648U, 6303U, 6307U,
         6311U, 6315U, 6319U, 6323U, 6327U, 6331U,      6335U, 6339U, 6343U, 6346U, 6349U, 6352U, 6355U,
         6358U, 6361U, 6364U, 6367U, 6370U, 6373U,      6376U, 6379U, 6382U, 6385U, 6388U, 6393U, 6398U,
         6403U, 6408U, 6413U, 6418U, 6423U, 6428U,      6433U, 6439U, 6445U, 6451U, 6454U, 6458U, 6461U,
         6465U, 6469U, 6473U, 6477U, 6481U, 6485U,      6489U, 6493U, 6497U, 6501U, 6505U, 6509U, 6513U,
         6517U, 6521U, 6525U, 6529U, 6533U, 6537U,      6541U, 6545U, 6549U, 6553U, 6557U, 6561U, 6565U,
         6569U, 6573U, 6577U, 6581U, 6585U, 6589U,      6593U, 6597U, 6601U, 6605U, 6609U, 6613U, 6617U,
         6621U, 6625U, 6629U, 6633U, 6637U, 6641U,      6645U, 6649U, 6653U},

       // Block #34
        {6660U,      6673U, 6686U,      6699U, 6709U, 6722U, 6732U, 6742U,      6758U, 6771U, 6781U, 6791U,
         6801U,      6814U, 6827U,      6837U, 6847U, 6854U, 6864U, 6877U,      6890U, 6897U, 6913U, 6932U,
         6948U,      6958U, 6974U,      6990U, 7003U, 7013U, 7023U, 7033U,      7046U, 7062U, 7075U, 7085U,
         7095U,      7105U, 7112U,      7119U, 7126U, 7133U, 7143U, 7153U,      7169U, 7179U, 7192U, 7208U,
         7218U,      7225U, 7232U,      7248U, 7261U, 7277U, 7287U, 7303U,      7310U, 7320U, 7330U, 7340U,
         7350U,      7360U, 7373U,      7383U, 7390U, 7400U, 7410U, 7420U,      7433U, 7443U, 7453U, 7463U,
         7479U,      7492U, 7499U,      7515U, 7522U, 7535U, 7548U, 7558U,      7568U, 7578U, 7591U, 7598U,
         7608U,      7621U, 7628U,      7644U, 7654U, 7659U, 7664U, 7669U,      7674U, 7679U, 7684U, 7689U,
         7694U,      7699U, 7704U,      7710U, 7716U, 7722U, 7728U, 7734U,      7740U, 7746U, 7752U, 7758U,
         7764U,      7770U, 7776U,      7782U, 7788U, 7794U, 7798U, 7801U,      7804U, 7808U, 7811U, 7814U,
         7817U,      7821U, 7825U,      7828U, 7835U, 7842U, 7849U, 7856U,      7869U, 7872U, 7875U, 7879U,
         7882U,      7885U, 7888U,      7891U, 7894U, 7898U, 7903U, 7906U,      7909U, 7913U, 7917U, 7920U,
         7923U,      7926U, 7930U,      7934U, 7938U, 7942U, 7946U, 7949U,      7952U, 7955U, 7958U, 7961U,
         7965U,      7968U, 7971U,      7974U, 7978U, 7982U, 7985U, 7989U,      7993U, 7997U, 8000U, 8004U,
         8010U,      7869U, 8017U,      8021U, 8025U, 8029U, 8033U, 8041U,      8050U, 8053U, 8056U, 8060U,
         8063U,      8066U, 8069U,      8073U, 8076U, 8073U, 8079U, 8082U,      8085U, 8089U, 8092U, 8089U,
         8095U,      8099U, disallowed, 8103U, 8106U, 8109U, 8112U, disallowed, 8119U, 8122U, 8125U, 8128U,
         8131U,      8134U, 7971U,      8137U, 8140U, 8143U, 8146U, 8150U,      7888U, 8153U, 8157U, 8161U,
         disallowed, 8164U, 8168U,      8171U, 8174U, 8177U, 8180U, 8186U,      8192U, 8197U, 8202U, 8207U,
         8212U,      8217U, 8222U,      8227U, 8232U, 8237U, 8243U, 8249U,      8255U, 8261U, 8267U, 8273U,
         8279U,      8285U, 8291U,      8297U, 8303U, 8309U, 8315U, 8321U,      8327U, 8333U, 8339U, 8345U,
         8351U,      8357U, 8363U,      8369U},

       // Block #35
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         8373U,      valid,      8377U,      valid,      8381U,      valid,      8385U,      valid,
         8389U,      valid,      2003U,      valid,      8393U,      valid,      8397U,      valid,
         8401U,      valid,      8405U,      valid,      8409U,      valid,      8413U,      valid,
         8417U,      valid,      8421U,      valid,      8425U,      valid,      8429U,      valid,
         8433U,      valid,      8437U,      valid,      8441U,      valid,      8445U,      valid,
         8449U,      valid,      8453U,      valid,      8457U,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         8461U,      valid,      8465U,      valid,      8469U,      valid,      8473U,      valid,
         8477U,      valid,      8481U,      valid,      8485U,      valid,      8489U,      valid,
         8493U,      valid,      8497U,      valid,      8501U,      valid,      8505U,      valid,
         8509U,      valid,      8513U,      valid,      1054U,      1060U,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #36
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      8517U,      valid,      8521U,      valid,      8525U,      valid,
         8529U,      valid,      8533U,      valid,      8537U,      valid,      8541U,      valid,
         valid,      valid,      8545U,      valid,      8549U,      valid,      8553U,      valid,
         8557U,      valid,      8561U,      valid,      8565U,      valid,      8569U,      valid,
         8573U,      valid,      8577U,      valid,      8581U,      valid,      8585U,      valid,
         8589U,      valid,      8593U,      valid,      8597U,      valid,      8601U,      valid,
         8605U,      valid,      8609U,      valid,      8613U,      valid,      8617U,      valid,
         8621U,      valid,      8625U,      valid,      8629U,      valid,      8633U,      valid,
         8637U,      valid,      8641U,      valid,      8645U,      valid,      8649U,      valid,
         8653U,      valid,      8657U,      valid,      8661U,      valid,      8665U,      valid,
         8665U,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      8669U,      valid,      8673U,      valid,      8677U,      8681U,      valid,
         8685U,      valid,      8689U,      valid,      8693U,      valid,      8697U,      valid,
         valid,      valid,      valid,      8701U,      valid,      2232U,      valid,      valid,
         8705U,      valid,      8709U,      valid,      valid,      valid,      8713U,      valid,
         8717U,      valid,      8721U,      valid,      8725U,      valid,      8729U,      valid,
         8733U,      valid,      8737U,      valid,      8741U,      valid,      8745U,      valid,
         8749U,      valid,      695U,       2201U,      2229U,      8753U,      2235U,      valid,
         8756U,      8759U,      2242U,      8762U,      8766U,      valid,      8770U,      valid,
         8774U,      valid,      8778U,      valid,      8782U,      valid,      8786U,      valid,
         8790U,      valid,      8794U,      valid,      8798U,      2270U,      8802U,      8806U,
         valid,      8810U,      valid,      8814U,      8817U,      valid,      disallowed, disallowed,
         8821U,      valid,      disallowed, valid,      disallowed, valid,      8825U,      valid,
         8829U,      valid,      8833U,      valid,      8837U,      disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, 4U,         10U,        32U,        8840U,      valid,      valid,
         245U,       314U,       valid,      valid,      valid,      valid,      valid,      valid},

       // Block #37
        {disallowed, valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         disallowed, valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         disallowed, valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      8525U,      8844U,      4106U,      8848U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      8852U,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         8855U,      8859U,      8863U,      8867U,      8871U,      8875U,      8879U,      8883U,
         8887U,      8891U,      8895U,      8899U,      8903U,      8907U,      8911U,      8915U,
         8919U,      8923U,      8927U,      8931U,      8935U,      8939U,      8943U,      8947U,
         8951U,      8955U,      8959U,      8963U,      8967U,      8971U,      8975U,      8979U,
         8983U,      8987U,      8991U,      8995U,      8999U,      9003U,      9007U,      9011U,
         9015U,      9019U,      9023U,      9027U,      9031U,      9035U,      9039U,      9043U,
         9047U,      9051U,      9055U,      9059U,      9063U,      9067U,      9071U,      9075U,
         9079U,      9083U,      9087U,      9091U,      9095U,      9099U,      9103U,      9107U,
         9111U,      9115U,      9119U,      9123U,      9127U,      9131U,      9135U,      9139U,
         9143U,      9147U,      9151U,      9155U,      9159U,      9163U,      9167U,      9171U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #38
        {9175U,  9179U,  4998U,  9183U,  9187U,  9191U,  9195U,  5214U,  5214U,  9199U,  5030U,  9203U,
         9207U,  9211U,  9215U,  9219U,  9223U,  9227U,  9231U,  9235U,  9239U,  9243U,  9247U,  9251U,
         9255U,  9259U,  9263U,  9267U,  9271U,  9275U,  9279U,  9283U,  9287U,  9291U,  9295U,  9299U,
         9303U,  9307U,  9311U,  9315U,  9319U,  9323U,  9327U,  9331U,  9335U,  9339U,  9343U,  9347U,
         9351U,  9355U,  9359U,  9363U,  4862U,  9367U,  9371U,  9375U,  9379U,  9383U,  9387U,  9391U,
         9395U,  9399U,  9403U,  9407U,  5154U,  9411U,  9415U,  9419U,  9423U,  9427U,  9431U,  9435U,
         9439U,  9443U,  9447U,  9451U,  9455U,  9459U,  9463U,  9467U,  9471U,  9475U,  9479U,  9483U,
         9487U,  9491U,  9495U,  9499U,  9503U,  9507U,  9511U,  9515U,  9239U,  9519U,  9523U,  9527U,
         9531U,  9535U,  9539U,  9543U,  9547U,  9551U,  9555U,  9559U,  9563U,  9567U,  9571U,  9575U,
         9579U,  9583U,  9587U,  9591U,  9595U,  5006U,  9599U,  9603U,  9607U,  9611U,  9615U,  9619U,
         9623U,  9627U,  9631U,  9635U,  9639U,  9643U,  9647U,  9651U,  9655U,  4514U,  9659U,  9663U,
         9667U,  9671U,  9675U,  9679U,  9683U,  9687U,  4438U,  9691U,  9695U,  9699U,  9703U,  9707U,
         9711U,  9715U,  9719U,  9723U,  9727U,  9731U,  9735U,  9739U,  9743U,  9747U,  9751U,  9755U,
         9759U,  9763U,  9767U,  9771U,  9775U,  9591U,  9779U,  9783U,  9787U,  9791U,  9795U,  9799U,
         9803U,  9807U,  9527U,  9811U,  9815U,  9819U,  9823U,  9827U,  9831U,  9835U,  9839U,  9843U,
         9847U,  9851U,  9855U,  9859U,  9863U,  9867U,  9871U,  9875U,  9879U,  9883U,  9887U,  9239U,
         9891U,  9895U,  9899U,  9903U,  5210U,  9907U,  9911U,  9915U,  9919U,  9923U,  9927U,  9931U,
         9935U,  9939U,  9943U,  9947U,  9951U,  6219U,  9955U,  9959U,  9963U,  9967U,  9971U,  9975U,
         9979U,  9983U,  9987U,  9535U,  9991U,  9995U,  9999U,  10003U, 10007U, 10011U, 10015U, 10019U,
         10023U, 10027U, 10031U, 10035U, 10039U, 5026U,  10043U, 10047U, 10051U, 10055U, 10059U, 10063U,
         10067U, 10071U, 10075U, 10079U, 10083U, 10087U, 10091U, 4830U,  10095U, 10099U, 10103U, 10107U,
         10111U, 10115U, 10119U, 10123U},

       // Block #39
        {10127U,     10131U,     10135U,     10139U,     10143U,     10147U,     10151U,     10155U,
         4938U,      10159U,     4950U,      10163U,     10167U,     10171U,     valid,      valid,
         10175U,     valid,      10179U,     valid,      valid,      10183U,     10187U,     10191U,
         10195U,     10199U,     10203U,     10207U,     10211U,     10215U,     4858U,      valid,
         10219U,     valid,      10223U,     valid,      valid,      10227U,     10231U,     valid,
         valid,      valid,      10235U,     10239U,     10243U,     10247U,     10251U,     10255U,
         10259U,     10263U,     10267U,     10271U,     10275U,     10279U,     10283U,     10287U,
         10291U,     10295U,     10299U,     10303U,     4542U,      10307U,     10311U,     10315U,
         10319U,     10323U,     10327U,     10331U,     10335U,     10339U,     10343U,     10347U,
         10351U,     10355U,     10359U,     10363U,     6239U,      10367U,     10371U,     10375U,
         10379U,     6255U,      10383U,     10387U,     10391U,     10395U,     10399U,     9735U,
         10403U,     10407U,     10411U,     10415U,     10419U,     10423U,     10423U,     10427U,
         10431U,     10435U,     10439U,     10443U,     10447U,     10451U,     10455U,     10227U,
         10459U,     10463U,     10467U,     10471U,     10475U,     10480U,     disallowed, disallowed,
         10484U,     10488U,     10492U,     10496U,     10500U,     10504U,     10508U,     10512U,
         10283U,     10516U,     10520U,     10524U,     10175U,     10528U,     10532U,     10536U,
         10540U,     10544U,     10548U,     10552U,     10556U,     10560U,     10564U,     10568U,
         10572U,     10315U,     10576U,     10319U,     10580U,     10584U,     10588U,     10592U,
         10596U,     10179U,     9323U,      10600U,     10604U,     4674U,      9595U,      9927U,
         10608U,     10612U,     10347U,     10616U,     10351U,     10620U,     10624U,     10628U,
         10187U,     10632U,     10636U,     10640U,     10644U,     10648U,     10191U,     10652U,
         10656U,     10660U,     10664U,     10668U,     10672U,     10399U,     10676U,     10680U,
         9735U,      10684U,     10415U,     10688U,     10692U,     10696U,     10700U,     10704U,
         10435U,     10708U,     10223U,     10712U,     10439U,     9519U,      10716U,     10443U,
         10720U,     10451U,     10724U,     10728U,     10732U,     10736U,     10740U,     10459U,
         10211U,     10744U,     10463U,     10748U,     10467U,     10752U,     5214U,      10756U,
         10761U,     10766U,     10771U,     10775U,     10779U,     10783U,     10788U,     10793U,
         10798U,     10802U,     disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #40
        {10806U,     10809U,     10812U,     10815U,     10819U,     10823U,     10823U,     disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, 10826U,     10831U,     10836U,     10841U,     10846U,
         disallowed, disallowed, disallowed, disallowed, disallowed, 10851U,     valid,      10856U,
         10861U,     3452U,      3461U,      10864U,     10867U,     10870U,     10873U,     10876U,
         10879U,     3396U,      10882U,     10887U,     10892U,     10899U,     10906U,     10911U,
         10916U,     10921U,     10926U,     10931U,     10936U,     10941U,     10946U,     disallowed,
         10951U,     10956U,     10961U,     10966U,     10971U,     disallowed, 10976U,     disallowed,
         10981U,     10986U,     disallowed, 10991U,     10996U,     disallowed, 11001U,     11006U,
         11011U,     11016U,     11021U,     11026U,     11031U,     11036U,     11041U,     11046U,
         11051U,     11051U,     11054U,     11054U,     11054U,     11054U,     11057U,     11057U,
         11057U,     11057U,     11060U,     11060U,     11060U,     11060U,     11063U,     11063U,
         11063U,     11063U,     11066U,     11066U,     11066U,     11066U,     11069U,     11069U,
         11069U,     11069U,     11072U,     11072U,     11072U,     11072U,     11075U,     11075U,
         11075U,     11075U,     11078U,     11078U,     11078U,     11078U,     11081U,     11081U,
         11081U,     11081U,     11084U,     11084U,     11084U,     11084U,     11087U,     11087U,
         11087U,     11087U,     11090U,     11090U,     11093U,     11093U,     11096U,     11096U,
         11099U,     11099U,     11102U,     11102U,     11105U,     11105U,     11108U,     11108U,
         11108U,     11108U,     11111U,     11111U,     11111U,     11111U,     11114U,     11114U,
         11114U,     11114U,     11117U,     11117U,     11117U,     11117U,     11120U,     11120U,
         11123U,     11123U,     11123U,     11123U,     11126U,     11126U,     11129U,     11129U,
         11129U,     11129U,     11132U,     11132U,     11132U,     11132U,     11135U,     11135U,
         11138U,     11138U,     valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, 11141U,     11141U,     11141U,     11141U,     11144U,
         11144U,     11147U,     11147U,     11150U,     11150U,     1501U,      11153U,     11153U,
         11156U,     11156U,     11159U,     11159U,     11162U,     11162U,     11162U,     11162U,
         11165U,     11165U,     11168U,     11168U,     11173U,     11173U,     11178U,     11178U,
         11183U,     11183U,     11188U,     11188U,     11193U,     11193U,     11198U,     11198U,
         11198U,     11203U,     11203U,     11203U,     11208U,     11208U,     11208U,     11208U},

       // Block #41
        {11211U, 11216U, 11221U, 11203U, 11226U, 11231U, 11236U, 11241U, 11246U, 11251U, 11256U, 11261U,
         11266U, 11271U, 11276U, 11281U, 11286U, 11291U, 11296U, 11301U, 11306U, 11311U, 11316U, 11321U,
         11326U, 11331U, 11336U, 11341U, 11346U, 11351U, 11356U, 11361U, 11366U, 11371U, 11376U, 11381U,
         11386U, 11391U, 11396U, 11401U, 11406U, 11411U, 11416U, 11421U, 11426U, 11431U, 11436U, 11441U,
         11446U, 11451U, 11456U, 11461U, 11466U, 11471U, 11476U, 11481U, 11486U, 11491U, 11496U, 11501U,
         11506U, 11511U, 11516U, 11521U, 11526U, 11531U, 11536U, 11541U, 11546U, 11551U, 11556U, 11561U,
         11566U, 11571U, 11576U, 11581U, 11586U, 11591U, 11596U, 11601U, 11606U, 11611U, 11616U, 11621U,
         11626U, 11631U, 11636U, 11641U, 11646U, 11651U, 11656U, 11661U, 11666U, 11671U, 11676U, 11682U,
         11688U, 11694U, 11700U, 11706U, 11712U, 11717U, 11221U, 11722U, 11203U, 11226U, 11727U, 11732U,
         11246U, 11737U, 11251U, 11256U, 11742U, 11747U, 11276U, 11752U, 11281U, 11286U, 11757U, 11762U,
         11296U, 11767U, 11301U, 11306U, 11451U, 11456U, 11471U, 11476U, 11481U, 11501U, 11506U, 11511U,
         11516U, 11536U, 11541U, 11546U, 11772U, 11566U, 11777U, 11782U, 11596U, 11787U, 11601U, 11606U,
         11671U, 11792U, 11797U, 11646U, 11802U, 11651U, 11656U, 11211U, 11216U, 11807U, 11221U, 11812U,
         11231U, 11236U, 11241U, 11246U, 11817U, 11261U, 11266U, 11271U, 11276U, 11822U, 11296U, 11311U,
         11316U, 11321U, 11326U, 11331U, 11341U, 11346U, 11351U, 11356U, 11361U, 11366U, 11827U, 11371U,
         11376U, 11381U, 11386U, 11391U, 11396U, 11406U, 11411U, 11416U, 11421U, 11426U, 11431U, 11436U,
         11441U, 11446U, 11461U, 11466U, 11486U, 11491U, 11496U, 11501U, 11506U, 11521U, 11526U, 11531U,
         11536U, 11832U, 11551U, 11556U, 11561U, 11566U, 11581U, 11586U, 11591U, 11596U, 11837U, 11611U,
         11616U, 11842U, 11631U, 11636U, 11641U, 11646U, 11847U, 11221U, 11812U, 11246U, 11817U, 11276U,
         11822U, 11296U, 11852U, 11361U, 11857U, 11862U, 11867U, 11501U, 11506U, 11536U, 11596U, 11837U,
         11646U, 11847U, 11872U, 11879U, 11886U, 11893U, 11898U, 11903U, 11908U, 11913U, 11918U, 11923U,
         11928U, 11933U, 11938U, 11943U},

       // Block #42
        {11948U,     11953U,     11958U,     11963U,     11968U,     11973U,     11978U,     11983U,
         11988U,     11993U,     11998U,     12003U,     11862U,     12008U,     12013U,     12018U,
         12023U,     11893U,     11898U,     11903U,     11908U,     11913U,     11918U,     11923U,
         11928U,     11933U,     11938U,     11943U,     11948U,     11953U,     11958U,     11963U,
         11968U,     11973U,     11978U,     11983U,     11988U,     11993U,     11998U,     12003U,
         11862U,     12008U,     12013U,     12018U,     12023U,     11993U,     11998U,     12003U,
         11862U,     11857U,     11867U,     11401U,     11346U,     11351U,     11356U,     11993U,
         11998U,     12003U,     11401U,     11406U,     12028U,     12028U,     valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         12033U,     12040U,     12040U,     12047U,     12054U,     12061U,     12068U,     12075U,
         12082U,     12082U,     12089U,     12096U,     12103U,     12110U,     12117U,     12124U,
         12124U,     12131U,     12138U,     12138U,     12145U,     12145U,     12152U,     12159U,
         12159U,     12166U,     12173U,     12173U,     12180U,     12180U,     12187U,     12194U,
         12194U,     12201U,     12201U,     12208U,     12215U,     12222U,     12229U,     12229U,
         12236U,     12243U,     12250U,     12257U,     12264U,     12264U,     12271U,     12278U,
         12285U,     12292U,     12299U,     12306U,     12306U,     12313U,     12313U,     12320U,
         12320U,     12327U,     12334U,     12341U,     12348U,     12355U,     12362U,     12369U,
         disallowed, disallowed, 12376U,     12383U,     12390U,     12397U,     12404U,     12411U,
         12411U,     12418U,     12425U,     12432U,     12439U,     12439U,     12446U,     12453U,
         12460U,     12467U,     12474U,     12481U,     12488U,     12495U,     12502U,     12509U,
         12516U,     12523U,     12530U,     12537U,     12544U,     12551U,     12558U,     12565U,
         12572U,     12579U,     12586U,     12593U,     12271U,     12285U,     12600U,     12607U,
         12614U,     12621U,     12628U,     12635U,     12628U,     12614U,     12642U,     12649U,
         12656U,     12663U,     12670U,     12635U,     12222U,     12152U,     12677U,     12684U,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         12691U,     12698U,     12705U,     12714U,     12723U,     12732U,     12741U,     12750U,
         12759U,     12768U,     12775U,     12809U,     12825U,     valid,      valid,      valid},

       // Block #43
        {58U,        58U,        58U,        58U,        58U,        58U,    58U,    58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,    58U,    12834U,     12836U,
         disallowed, 12840U,     767U,       12842U,     12844U,     12846U, 12850U, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, valid,  valid,  valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,  valid,  valid,      valid,
         valid,      valid,      valid,      disallowed, 12854U,     12858U, 12862U, 12862U,     3404U,
         3406U,      12864U,     12866U,     12868U,     12872U,     12876U, 12880U, 12884U,     12888U,
         3644U,      3648U,      12892U,     12896U,     12900U,     12904U, valid,  valid,      12908U,
         12910U,     3356U,      3356U,      3356U,      3356U,      12862U, 12862U, 12862U,     12834U,
         12836U,     disallowed, disallowed, 767U,       12840U,     12844U, 12842U, 12854U,     3404U,
         3406U,      12864U,     12866U,     12868U,     12872U,     12912U, 12914U, 12916U,     3396U,
         12918U,     12920U,     12922U,     3402U,      disallowed, 12924U, 12926U, 12928U,     12930U,
         disallowed, disallowed, disallowed, disallowed, 12932U,     12936U, 12941U, valid,      12945U,
         disallowed, 12949U,     12953U,     12958U,     12962U,     12967U, 12971U, 12976U,     12980U,
         12985U,     12989U,     12994U,     12997U,     12997U,     13000U, 13000U, 13003U,     13003U,
         13006U,     13006U,     13009U,     13009U,     13009U,     13009U, 13012U, 13012U,     13015U,
         13015U,     13015U,     13015U,     13018U,     13018U,     13021U, 13021U, 13021U,     13021U,
         13024U,     13024U,     13024U,     13024U,     13027U,     13027U, 13027U, 13027U,     13030U,
         13030U,     13030U,     13030U,     13033U,     13033U,     13033U, 13033U, 13036U,     13036U,
         13039U,     13039U,     13042U,     13042U,     13045U,     13045U, 13048U, 13048U,     13048U,
         13048U,     13051U,     13051U,     13051U,     13051U,     13054U, 13054U, 13054U,     13054U,
         13057U,     13057U,     13057U,     13057U,     13060U,     13060U, 13060U, 13060U,     13063U,
         13063U,     13063U,     13063U,     13066U,     13066U,     13066U, 13066U, 13069U,     13069U,
         13069U,     13069U,     13072U,     13072U,     13072U,     13072U, 13075U, 13075U,     13075U,
         13075U,     13078U,     13078U,     13078U,     13078U,     13081U, 13081U, 13081U,     13081U,
         13084U,     13084U,     13084U,     13084U,     13087U,     13087U, 13087U, 13087U,     13090U,
         13090U,     13090U,     13090U,     13093U,     13093U,     11165U, 11165U, 13096U,     13096U,
         13096U,     13096U,     13099U,     13099U,     13104U,     13104U, 13109U, 13109U,     13114U,
         13114U,     disallowed, disallowed, 58U},

       // Block #44
        {disallowed, 12842U,     13119U,     12912U,     12926U,     12928U,     12914U,     13121U,
         3404U,      3406U,      12916U,     3396U,      12834U,     12918U,     5222U,      13123U,
         3382U,      78U,        63U,        65U,        3384U,      3386U,      3388U,      3390U,
         3392U,      3394U,      12840U,     767U,       12920U,     3402U,      12922U,     12844U,
         12930U,     0U,         2U,         4U,         6U,         8U,         10U,        12U,
         14U,        16U,        18U,        20U,        22U,        24U,        26U,        28U,
         30U,        32U,        34U,        36U,        38U,        40U,        42U,        44U,
         46U,        48U,        50U,        12908U,     12924U,     12910U,     13125U,     12862U,
         3275U,      0U,         2U,         4U,         6U,         8U,         10U,        12U,
         14U,        16U,        18U,        20U,        22U,        24U,        26U,        28U,
         30U,        32U,        34U,        36U,        38U,        40U,        42U,        44U,
         46U,        48U,        50U,        12864U,     13127U,     12866U,     13129U,     13131U,
         13135U,     5222U,      12892U,     12896U,     12836U,     13139U,     6649U,      13143U,
         13147U,     13151U,     13155U,     13159U,     13163U,     13167U,     13171U,     13175U,
         13179U,     6465U,      6469U,      6473U,      6477U,      6481U,      6485U,      6489U,
         6493U,      6497U,      6501U,      6505U,      6509U,      6513U,      6517U,      6521U,
         6525U,      6529U,      6533U,      6537U,      6541U,      6545U,      6549U,      6553U,
         6557U,      6561U,      6565U,      6569U,      6573U,      6577U,      6581U,      6585U,
         6589U,      6593U,      6597U,      6601U,      6605U,      6609U,      6613U,      6617U,
         6621U,      6625U,      6629U,      6633U,      6637U,      13183U,     13187U,     13191U,
         58U,        5260U,      5264U,      5268U,      5272U,      5276U,      5280U,      5284U,
         5288U,      5292U,      5296U,      5300U,      5304U,      5308U,      5312U,      5316U,
         5320U,      5324U,      5328U,      5332U,      5336U,      5340U,      5344U,      5348U,
         5352U,      5356U,      5360U,      5364U,      5368U,      5372U,      5376U,      disallowed,
         disallowed, disallowed, 5380U,      5384U,      5388U,      5392U,      5396U,      5400U,
         disallowed, disallowed, 5404U,      5408U,      5412U,      5416U,      5420U,      5424U,
         disallowed, disallowed, 5428U,      5432U,      5436U,      5440U,      5444U,      5448U,
         disallowed, disallowed, 5452U,      5456U,      5460U,      disallowed, disallowed, disallowed,
         13195U,     13198U,     13201U,     59U,        13204U,     13207U,     13210U,     disallowed,
         13214U,     13218U,     13222U,     13226U,     13230U,     13234U,     13238U,     disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #45
        {13242U, 13247U, 13252U,     13257U,     13262U,     13267U,     13272U,     13277U,
         13282U, 13287U, 13292U,     13297U,     13302U,     13307U,     13312U,     13317U,
         13322U, 13327U, 13332U,     13337U,     13342U,     13347U,     13352U,     13357U,
         13362U, 13367U, 13372U,     13377U,     13382U,     13387U,     13392U,     13397U,
         13402U, 13407U, 13412U,     13417U,     13422U,     13427U,     13432U,     13437U,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      disallowed, disallowed,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         13442U, 13447U, 13452U,     13457U,     13462U,     13467U,     13472U,     13477U,
         13482U, 13487U, 13492U,     13497U,     13502U,     13507U,     13512U,     13517U,
         13522U, 13527U, 13532U,     13537U,     13542U,     13547U,     13552U,     13557U,
         13562U, 13567U, 13572U,     13577U,     13582U,     13587U,     13592U,     13597U,
         13602U, 13607U, 13612U,     13617U,     disallowed, disallowed, disallowed, disallowed,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      valid,      valid,      valid,      valid,
         valid,  valid,  valid,      valid,      disallowed, disallowed, disallowed, disallowed},

       // Block #46
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
         13622U,     13627U,     13632U,     13637U,     13642U,     13647U,     13652U,     13657U,
         13662U,     13667U,     13672U,     disallowed, 13677U,     13682U,     13687U,     13692U,
         13697U,     13702U,     13707U,     13712U,     13717U,     13722U,     13727U,     13732U,
         13737U,     13742U,     13747U,     disallowed, 13752U,     13757U,     13762U,     13767U,
         13772U,     13777U,     13782U,     disallowed, 13787U,     13792U,     disallowed, valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, valid,      valid,      disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #47
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      13797U,     13800U,     116U,       13803U,     383U,       disallowed, 13806U,
         13809U,     13813U,     13816U,     398U,       401U,       13819U,     13823U,     13826U,
         13829U,     8814U,      13832U,     419U,       13835U,     245U,       13838U,     13841U,
         13844U,     13847U,     13850U,     8753U,      13853U,     13858U,     13862U,     13865U,
         13870U,     13873U,     167U,       13878U,     13881U,     32U,        13884U,     13887U,
         4113U,      13892U,     452U,       13895U,     13898U,     13901U,     13905U,     464U,
         13908U,     disallowed, 13912U,     13915U,     13918U,     13921U,     13924U,     13927U,
         13930U,     13933U,     13938U,     disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #48
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         13943U,     13948U,     13953U,     13958U,     13963U,     13968U,     13973U,     13978U,
         13983U,     13988U,     13993U,     13998U,     14003U,     14008U,     14013U,     14018U,
         14023U,     14028U,     14033U,     14038U,     14043U,     14048U,     14053U,     14058U,
         14063U,     14068U,     14073U,     14078U,     14083U,     14088U,     14093U,     14098U,
         14103U,     14108U,     14113U,     14118U,     14123U,     14128U,     14133U,     14138U,
         14143U,     14148U,     14153U,     14158U,     14163U,     14168U,     14173U,     14178U,
         14183U,     14188U,     14193U,     disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, valid,      valid,      valid,      valid,      valid,      valid},

       // Block #49
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         14198U,     14203U,     14208U,     14213U,     14218U,     14223U,     14228U,     14233U,
         14238U,     14243U,     14248U,     14253U,     14258U,     14263U,     14268U,     14273U,
         14278U,     14283U,     14288U,     14293U,     14298U,     14303U,     disallowed, disallowed,
         disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #50
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         14308U,     14313U,     14318U,     14323U,     14328U,     14333U,     14338U,     14343U,
         14348U,     14353U,     14358U,     14363U,     14368U,     14373U,     14378U,     14383U,
         14388U,     14393U,     14398U,     14403U,     14408U,     14413U,     14418U,     14423U,
         14428U,     14433U,     14438U,     14443U,     14448U,     14453U,     14458U,     14463U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid},

       // Block #51
        {disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         14468U,     14473U,     14478U,     14483U,     14488U,     14493U,     14498U,     14503U,
         14508U,     14513U,     14518U,     14523U,     14528U,     14533U,     14538U,     14543U,
         14548U,     14553U,     14558U,     14563U,     14568U,     14573U,     14578U,     14583U,
         14588U,     14593U,     14598U,     14603U,     14608U,     14613U,     14618U,     14623U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #52
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, valid,      valid,      valid,      valid,
         58U,        58U,        58U,        58U,        disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #53
        {valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
         valid,      valid,      valid,      valid,      0U,         2U,        4U,    6U,    8U,    10U,
         12U,        14U,        16U,        18U,        20U,        22U,       24U,   26U,   28U,   30U,
         32U,        34U,        36U,        38U,        40U,        42U,       44U,   46U,   48U,   50U,
         3382U,      78U,        63U,        65U,        3384U,      3386U,     3388U, 3390U, 3392U, 3394U,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #54
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      14628U,     14637U,
         14646U,     14659U,     14672U,     14685U,     14698U,     valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      14711U,     14720U,     14729U,     14742U,     14755U,
         14768U,     valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #55
        {0U,  2U,         4U,         6U,  8U,         10U,        12U,        14U,        16U,
         18U, 20U,        22U,        24U, 26U,        28U,        30U,        32U,        34U,
         36U, 38U,        40U,        42U, 44U,        46U,        48U,        50U,        0U,
         2U,  4U,         6U,         8U,  10U,        12U,        14U,        16U,        18U,
         20U, 22U,        24U,        26U, 28U,        30U,        32U,        34U,        36U,
         38U, 40U,        42U,        44U, 46U,        48U,        50U,        0U,         2U,
         4U,  6U,         8U,         10U, 12U,        14U,        16U,        18U,        20U,
         22U, 24U,        26U,        28U, 30U,        32U,        34U,        36U,        38U,
         40U, 42U,        44U,        46U, 48U,        50U,        0U,         2U,         4U,
         6U,  8U,         10U,        12U, disallowed, 16U,        18U,        20U,        22U,
         24U, 26U,        28U,        30U, 32U,        34U,        36U,        38U,        40U,
         42U, 44U,        46U,        48U, 50U,        0U,         2U,         4U,         6U,
         8U,  10U,        12U,        14U, 16U,        18U,        20U,        22U,        24U,
         26U, 28U,        30U,        32U, 34U,        36U,        38U,        40U,        42U,
         44U, 46U,        48U,        50U, 0U,         2U,         4U,         6U,         8U,
         10U, 12U,        14U,        16U, 18U,        20U,        22U,        24U,        26U,
         28U, 30U,        32U,        34U, 36U,        38U,        40U,        42U,        44U,
         46U, 48U,        50U,        0U,  disallowed, 4U,         6U,         disallowed, disallowed,
         12U, disallowed, disallowed, 18U, 20U,        disallowed, disallowed, 26U,        28U,
         30U, 32U,        disallowed, 36U, 38U,        40U,        42U,        44U,        46U,
         48U, 50U,        0U,         2U,  4U,         6U,         disallowed, 10U,        disallowed,
         14U, 16U,        18U,        20U, 22U,        24U,        26U,        disallowed, 30U,
         32U, 34U,        36U,        38U, 40U,        42U,        44U,        46U,        48U,
         50U, 0U,         2U,         4U,  6U,         8U,         10U,        12U,        14U,
         16U, 18U,        20U,        22U, 24U,        26U,        28U,        30U,        32U,
         34U, 36U,        38U,        40U, 42U,        44U,        46U,        48U,        50U,
         0U,  2U,         4U,         6U,  8U,         10U,        12U,        14U,        16U,
         18U, 20U,        22U,        24U, 26U,        28U,        30U,        32U,        34U,
         36U, 38U,        40U,        42U},

       // Block #56
        {44U, 46U,        48U,        50U,        0U,  2U,  disallowed, 6U,  8U,         10U,
         12U, disallowed, disallowed, 18U,        20U, 22U, 24U,        26U, 28U,        30U,
         32U, disallowed, 36U,        38U,        40U, 42U, 44U,        46U, 48U,        disallowed,
         0U,  2U,         4U,         6U,         8U,  10U, 12U,        14U, 16U,        18U,
         20U, 22U,        24U,        26U,        28U, 30U, 32U,        34U, 36U,        38U,
         40U, 42U,        44U,        46U,        48U, 50U, 0U,         2U,  disallowed, 6U,
         8U,  10U,        12U,        disallowed, 16U, 18U, 20U,        22U, 24U,        disallowed,
         28U, disallowed, disallowed, disallowed, 36U, 38U, 40U,        42U, 44U,        46U,
         48U, disallowed, 0U,         2U,         4U,  6U,  8U,         10U, 12U,        14U,
         16U, 18U,        20U,        22U,        24U, 26U, 28U,        30U, 32U,        34U,
         36U, 38U,        40U,        42U,        44U, 46U, 48U,        50U, 0U,         2U,
         4U,  6U,         8U,         10U,        12U, 14U, 16U,        18U, 20U,        22U,
         24U, 26U,        28U,        30U,        32U, 34U, 36U,        38U, 40U,        42U,
         44U, 46U,        48U,        50U,        0U,  2U,  4U,         6U,  8U,         10U,
         12U, 14U,        16U,        18U,        20U, 22U, 24U,        26U, 28U,        30U,
         32U, 34U,        36U,        38U,        40U, 42U, 44U,        46U, 48U,        50U,
         0U,  2U,         4U,         6U,         8U,  10U, 12U,        14U, 16U,        18U,
         20U, 22U,        24U,        26U,        28U, 30U, 32U,        34U, 36U,        38U,
         40U, 42U,        44U,        46U,        48U, 50U, 0U,         2U,  4U,         6U,
         8U,  10U,        12U,        14U,        16U, 18U, 20U,        22U, 24U,        26U,
         28U, 30U,        32U,        34U,        36U, 38U, 40U,        42U, 44U,        46U,
         48U, 50U,        0U,         2U,         4U,  6U,  8U,         10U, 12U,        14U,
         16U, 18U,        20U,        22U,        24U, 26U, 28U,        30U, 32U,        34U,
         36U, 38U,        40U,        42U,        44U, 46U, 48U,        50U, 0U,         2U,
         4U,  6U,         8U,         10U,        12U, 14U, 16U,        18U, 20U,        22U,
         24U, 26U,        28U,        30U,        32U, 34U},

       // Block #57
        {36U,  38U,  40U,  42U,  44U,  46U,  48U,  50U,  0U,   2U,     4U,     6U,     8U,         10U,
         12U,  14U,  16U,  18U,  20U,  22U,  24U,  26U,  28U,  30U,    32U,    34U,    36U,        38U,
         40U,  42U,  44U,  46U,  48U,  50U,  0U,   2U,   4U,   6U,     8U,     10U,    12U,        14U,
         16U,  18U,  20U,  22U,  24U,  26U,  28U,  30U,  32U,  34U,    36U,    38U,    40U,        42U,
         44U,  46U,  48U,  50U,  0U,   2U,   4U,   6U,   8U,   10U,    12U,    14U,    16U,        18U,
         20U,  22U,  24U,  26U,  28U,  30U,  32U,  34U,  36U,  38U,    40U,    42U,    44U,        46U,
         48U,  50U,  0U,   2U,   4U,   6U,   8U,   10U,  12U,  14U,    16U,    18U,    20U,        22U,
         24U,  26U,  28U,  30U,  32U,  34U,  36U,  38U,  40U,  42U,    44U,    46U,    48U,        50U,
         0U,   2U,   4U,   6U,   8U,   10U,  12U,  14U,  16U,  18U,    20U,    22U,    24U,        26U,
         28U,  30U,  32U,  34U,  36U,  38U,  40U,  42U,  44U,  46U,    48U,    50U,    0U,         2U,
         4U,   6U,   8U,   10U,  12U,  14U,  16U,  18U,  20U,  22U,    24U,    26U,    28U,        30U,
         32U,  34U,  36U,  38U,  40U,  42U,  44U,  46U,  48U,  50U,    14781U, 14784U, disallowed, disallowed,
         802U, 805U, 808U, 811U, 814U, 817U, 820U, 823U, 748U, 826U,   829U,   71U,    832U,       835U,
         838U, 841U, 844U, 823U, 847U, 850U, 853U, 856U, 859U, 862U,   865U,   14787U, 802U,       805U,
         808U, 811U, 814U, 817U, 820U, 823U, 748U, 826U, 829U, 71U,    832U,   835U,   838U,       841U,
         844U, 847U, 847U, 850U, 853U, 856U, 859U, 862U, 865U, 14791U, 814U,   823U,   826U,       856U,
         844U, 841U, 802U, 805U, 808U, 811U, 814U, 817U, 820U, 823U,   748U,   826U,   829U,       71U,
         832U, 835U, 838U, 841U, 844U, 823U, 847U, 850U, 853U, 856U,   859U,   862U,   865U,       14787U,
         802U, 805U, 808U, 811U},

       // Block #58
        {814U,       817U,       820U,  823U,   748U,  826U,   829U,  71U,    832U,  835U,   838U,  841U,
         844U,       847U,       847U,  850U,   853U,  856U,   859U,  862U,   865U,  14791U, 814U,  823U,
         826U,       856U,       844U,  841U,   802U,  805U,   808U,  811U,   814U,  817U,   820U,  823U,
         748U,       826U,       829U,  71U,    832U,  835U,   838U,  841U,   844U,  823U,   847U,  850U,
         853U,       856U,       859U,  862U,   865U,  14787U, 802U,  805U,   808U,  811U,   814U,  817U,
         820U,       823U,       748U,  826U,   829U,  71U,    832U,  835U,   838U,  841U,   844U,  847U,
         847U,       850U,       853U,  856U,   859U,  862U,   865U,  14791U, 814U,  823U,   826U,  856U,
         844U,       841U,       802U,  805U,   808U,  811U,   814U,  817U,   820U,  823U,   748U,  826U,
         829U,       71U,        832U,  835U,   838U,  841U,   844U,  823U,   847U,  850U,   853U,  856U,
         859U,       862U,       865U,  14787U, 802U,  805U,   808U,  811U,   814U,  817U,   820U,  823U,
         748U,       826U,       829U,  71U,    832U,  835U,   838U,  841U,   844U,  847U,   847U,  850U,
         853U,       856U,       859U,  862U,   865U,  14791U, 814U,  823U,   826U,  856U,   844U,  841U,
         802U,       805U,       808U,  811U,   814U,  817U,   820U,  823U,   748U,  826U,   829U,  71U,
         832U,       835U,       838U,  841U,   844U,  823U,   847U,  850U,   853U,  856U,   859U,  862U,
         865U,       14787U,     802U,  805U,   808U,  811U,   814U,  817U,   820U,  823U,   748U,  826U,
         829U,       71U,        832U,  835U,   838U,  841U,   844U,  847U,   847U,  850U,   853U,  856U,
         859U,       862U,       865U,  14791U, 814U,  823U,   826U,  856U,   844U,  841U,   883U,  883U,
         disallowed, disallowed, 3382U, 78U,    63U,   65U,    3384U, 3386U,  3388U, 3390U,  3392U, 3394U,
         3382U,      78U,        63U,   65U,    3384U, 3386U,  3388U, 3390U,  3392U, 3394U,  3382U, 78U,
         63U,        65U,        3384U, 3386U,  3388U, 3390U,  3392U, 3394U,  3382U, 78U,    63U,   65U,
         3384U,      3386U,      3388U, 3390U,  3392U, 3394U,  3382U, 78U,    63U,   65U,    3384U, 3386U,
         3388U,      3390U,      3392U, 3394U},

       // Block #59
        {valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      disallowed, disallowed, valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, valid,      valid,      disallowed, valid,      valid,
         valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
         976U,       979U,       982U,       985U,       988U,       991U,       994U,       997U,
         1000U,      1006U,      1009U,      1012U,      1018U,      1021U,      1024U,      1027U,
         1030U,      1033U,      1036U,      1039U,      1042U,      1045U,      1048U,      1057U,
         1063U,      1066U,      8477U,      1240U,      946U,       952U,       1264U,      1177U,
         1204U,      976U,       979U,       982U,       985U,       988U,       991U,       994U,
         997U,       1000U,      1006U,      1009U,      1018U,      1021U,      1027U,      1033U,
         1036U,      1039U,      1042U,      1045U,      1048U,      1054U,      1057U,      1132U,
         946U,       943U,       973U,       1171U,      8401U,      1180U,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #60
        {14795U,     14800U,     14805U,     14810U,     14815U,     14820U,     14825U,     14830U,
         14835U,     14840U,     14845U,     14850U,     14855U,     14860U,     14865U,     14870U,
         14875U,     14880U,     14885U,     14890U,     14895U,     14900U,     14905U,     14910U,
         14915U,     14920U,     14925U,     14930U,     14935U,     14940U,     14945U,     14950U,
         14955U,     14960U,     valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, valid,      valid,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #61
        {13012U,     13015U,     13027U,     13036U,     disallowed, 13093U,     13045U,     13030U,
         13060U,     13096U,     13078U,     13081U,     13084U,     13087U,     13048U,     13066U,
         13072U,     13054U,     13075U,     13042U,     13051U,     13021U,     13024U,     13033U,
         13039U,     13057U,     13063U,     13069U,     14965U,     11120U,     14968U,     14971U,
         disallowed, 13015U,     13027U,     disallowed, 13090U,     disallowed, disallowed, 13030U,
         disallowed, 13096U,     13078U,     13081U,     13084U,     13087U,     13048U,     13066U,
         13072U,     13054U,     13075U,     disallowed, 13051U,     13021U,     13024U,     13033U,
         disallowed, 13057U,     disallowed, 13069U,     disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, 13027U,     disallowed, disallowed, disallowed, disallowed, 13030U,
         disallowed, 13096U,     disallowed, 13081U,     disallowed, 13087U,     13048U,     13066U,
         disallowed, 13054U,     13075U,     disallowed, 13051U,     disallowed, disallowed, 13033U,
         disallowed, 13057U,     disallowed, 13069U,     disallowed, 11120U,     disallowed, 14971U,
         disallowed, 13015U,     13027U,     disallowed, 13090U,     disallowed, disallowed, 13030U,
         13060U,     13096U,     13078U,     disallowed, 13084U,     13087U,     13048U,     13066U,
         13072U,     13054U,     13075U,     disallowed, 13051U,     13021U,     13024U,     13033U,
         disallowed, 13057U,     13063U,     13069U,     14965U,     disallowed, 14968U,     disallowed,
         13012U,     13015U,     13027U,     13036U,     13090U,     13093U,     13045U,     13030U,
         13060U,     13096U,     disallowed, 13081U,     13084U,     13087U,     13048U,     13066U,
         13072U,     13054U,     13075U,     13042U,     13051U,     13021U,     13024U,     13033U,
         13039U,     13057U,     13063U,     13069U,     disallowed, disallowed, disallowed, disallowed,
         disallowed, 13015U,     13027U,     13036U,     disallowed, 13093U,     13045U,     13030U,
         13060U,     13096U,     disallowed, 13081U,     13084U,     13087U,     13048U,     13066U,
         13072U,     13054U,     13075U,     13042U,     13051U,     13021U,     13024U,     13033U,
         13039U,     13057U,     13063U,     13069U,     disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #62
        {disallowed, 14974U,     14977U,     14980U,     14983U,     14986U,     14989U,     14992U,
         14995U,     14998U,     15001U,     valid,      valid,      valid,      valid,      valid,
         3776U,      3780U,      3784U,      3788U,      3792U,      3796U,      3800U,      3804U,
         3808U,      3812U,      3816U,      3820U,      3824U,      3828U,      3832U,      3836U,
         3840U,      3844U,      3848U,      3852U,      3856U,      3860U,      3864U,      3868U,
         3872U,      3876U,      15004U,     4U,         34U,        8109U,      15012U,     valid,
         0U,         2U,         4U,         6U,         8U,         10U,        12U,        14U,
         16U,        18U,        20U,        22U,        24U,        26U,        28U,        30U,
         32U,        34U,        36U,        38U,        40U,        42U,        44U,        46U,
         48U,        50U,        15015U,     8073U,      15018U,     15021U,     15024U,     15028U,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      15031U,     15034U,     15037U,     valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         15040U,     valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
         valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid},

       // Block #63
        {15043U,     15050U,     6505U,      disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         4618U,      15057U,     15061U,     15065U,     4390U,      15069U,     15073U,     5664U,
         15077U,     15081U,     15085U,     9887U,      15089U,     15093U,     15097U,     15101U,
         15105U,     15109U,     4762U,      15113U,     15117U,     15121U,     15125U,     15129U,
         15133U,     4366U,      5632U,      15137U,     6303U,      5644U,      6307U,      15141U,
         4986U,      15145U,     15149U,     15153U,     15157U,     15161U,     6235U,      4658U,
         15165U,     15169U,     15173U,     15177U,     disallowed, disallowed, disallowed, disallowed,
         15181U,     15191U,     15201U,     15211U,     15221U,     15231U,     15241U,     15251U,
         15261U,     disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         15271U,     15275U,     disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #64
        {valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      disallowed, valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
         valid,      valid,      valid,      valid,      valid, valid, 3382U, 78U,        63U,
         65U,        3384U,      3386U,      3388U,      3390U, 3392U, 3394U, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed},

       // Block #65
        {15279U, 15283U, 15287U, 15291U, 15296U, 10259U, 15300U, 15304U, 15308U, 15312U, 10263U, 15316U,
         15320U, 15324U, 10267U, 15329U, 15333U, 15337U, 15341U, 15346U, 15350U, 15097U, 15354U, 15359U,
         15363U, 15367U, 15371U, 10488U, 15375U, 4430U,  15380U, 15384U, 15388U, 15392U, 15169U, 15396U,
         15400U, 10508U, 10271U, 10275U, 10512U, 15404U, 15408U, 9543U,  15412U, 10279U, 15416U, 15420U,
         15424U, 15428U, 15428U, 15428U, 15432U, 15437U, 15441U, 15445U, 15449U, 15454U, 15458U, 15462U,
         15466U, 15470U, 15474U, 15478U, 15482U, 15486U, 15490U, 15494U, 15498U, 15502U, 15502U, 10520U,
         15506U, 15510U, 15514U, 15518U, 10287U, 15522U, 15526U, 15530U, 10127U, 15534U, 15538U, 15542U,
         15546U, 15550U, 15554U, 15558U, 15562U, 15566U, 15571U, 15575U, 15579U, 15069U, 15583U, 15587U,
         15591U, 15596U, 15601U, 15605U, 15609U, 15613U, 15617U, 15621U, 15625U, 15629U, 15633U, 15633U,
         15637U, 15642U, 15646U, 9527U,  15650U, 15654U, 15659U, 15663U, 15667U, 4534U,  15671U, 15675U,
         4542U,  15679U, 15683U, 15687U, 15692U, 15696U, 15701U, 15705U, 15709U, 15713U, 15717U, 15721U,
         15725U, 15729U, 15733U, 15737U, 15741U, 15745U, 15750U, 15754U, 15758U, 15762U, 9319U,  15766U,
         4582U,  15771U, 15771U, 15776U, 15780U, 15780U, 15784U, 15788U, 15793U, 15798U, 15802U, 15806U,
         15810U, 15814U, 15818U, 15822U, 15826U, 15830U, 15834U, 10307U, 15838U, 15843U, 15847U, 15851U,
         10568U, 15851U, 15855U, 10315U, 15859U, 15863U, 15867U, 15871U, 10319U, 9211U,  15875U, 15879U,
         15883U, 15887U, 15891U, 15895U, 15899U, 15904U, 15908U, 15912U, 15916U, 15920U, 15924U, 15929U,
         15933U, 15937U, 15941U, 15945U, 15949U, 15953U, 15957U, 15961U, 10323U, 15965U, 15969U, 15974U,
         15978U, 15982U, 15986U, 10331U, 15990U, 15994U, 15998U, 16002U, 16006U, 16010U, 16014U, 16018U,
         9323U,  10600U, 16022U, 16026U, 16030U, 16034U, 16039U, 16043U, 16047U, 16051U, 10335U, 16055U,
         16060U, 16064U, 16068U, 10771U, 16072U, 16076U, 16080U, 16084U, 16088U, 16093U, 16097U, 16101U,
         16105U, 16110U, 16114U, 16118U, 16122U, 9595U,  16126U, 16130U, 16135U, 16140U, 16145U, 16149U,
         16154U, 16158U, 16162U, 16166U},

       // Block #66
        {16170U, 10339U, 9927U,  16174U, 16178U, 16182U, 16186U, 16191U, 16195U, 16199U, 16203U, 10612U,
         16207U, 16211U, 16216U, 16220U, 16224U, 16229U, 16234U, 16238U, 10616U, 16242U, 16246U, 16250U,
         16254U, 16258U, 16262U, 16266U, 16271U, 16275U, 16280U, 16284U, 16289U, 10624U, 16293U, 16297U,
         16302U, 16306U, 16310U, 16315U, 16320U, 16324U, 16328U, 16332U, 16336U, 16336U, 16340U, 16344U,
         10632U, 16348U, 16352U, 16356U, 16360U, 16364U, 16369U, 16373U, 9539U,  16378U, 16383U, 16387U,
         16392U, 16397U, 16402U, 16406U, 10656U, 16410U, 16415U, 16420U, 16425U, 16430U, 16434U, 16434U,
         10660U, 10779U, 16438U, 16442U, 16446U, 16450U, 16455U, 9391U,  10668U, 16459U, 16463U, 10379U,
         16468U, 16473U, 10207U, 16478U, 16482U, 10391U, 16486U, 16490U, 16494U, 16499U, 16499U, 16504U,
         16508U, 16512U, 16517U, 16521U, 16525U, 16529U, 16534U, 16538U, 16542U, 16546U, 16550U, 16554U,
         16559U, 16563U, 16567U, 16571U, 16575U, 16579U, 16583U, 16588U, 16593U, 16597U, 16602U, 16606U,
         16611U, 16615U, 10415U, 16619U, 16624U, 16629U, 16633U, 16638U, 16642U, 16647U, 16651U, 16655U,
         16659U, 16663U, 16667U, 16671U, 16676U, 16681U, 16686U, 15776U, 16691U, 16695U, 16699U, 16703U,
         16707U, 16711U, 16715U, 16719U, 16723U, 16727U, 16731U, 16735U, 9607U,  16740U, 16744U, 16748U,
         16752U, 16756U, 16760U, 10427U, 16764U, 16768U, 16772U, 16776U, 16780U, 16785U, 16790U, 16795U,
         16799U, 16803U, 16807U, 16811U, 16816U, 16820U, 16825U, 16829U, 16833U, 16838U, 16843U, 16847U,
         9371U,  16851U, 16855U, 16859U, 16863U, 16867U, 16871U, 10696U, 16875U, 16879U, 16883U, 16887U,
         16891U, 16895U, 16899U, 16903U, 4942U,  16907U, 16912U, 16916U, 16920U, 16924U, 16928U, 16932U,
         16937U, 16942U, 16946U, 16950U, 10716U, 10720U, 4970U,  16954U, 16959U, 16963U, 16967U, 16971U,
         16975U, 16980U, 16985U, 16989U, 16993U, 16997U, 17002U, 10724U, 17006U, 17011U, 17016U, 17020U,
         17024U, 17028U, 17033U, 17037U, 17041U, 17045U, 17049U, 17053U, 17057U, 17061U, 17066U, 17070U,
         17074U, 17078U, 17083U, 17087U, 17091U, 17095U, 17099U, 17104U, 17109U, 17113U, 17117U, 17121U,
         17126U, 17130U, 10748U, 10748U},

       // Block #67
        {17135U,     17139U,     17144U,     17148U,     17152U,     17156U,     17160U,     17164U,
         17168U,     17172U,     10752U,     17177U,     17181U,     17185U,     17189U,     17193U,
         17197U,     17202U,     17206U,     17211U,     17216U,     5162U,      17221U,     5178U,
         17225U,     17229U,     17233U,     17237U,     5198U,      17241U,     disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed},

       // Block #68
        {58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         58U,        58U,        58U,        58U,        58U,        58U,        58U,        58U,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
         disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}

      }
    };

    /**
     * IDNA Mapped Code Points Table.
     * Each mapping ends with EOF '\0'.
     *
     * Table size: 17246 B or 16.84 KiB
     */
    static constexpr std::basic_string_view<char8_t> idna_mappings{
      u8"\x61\0" // #0: [41, AA, 1D2C, 1D43, 2090, 24B6, 24D0, FF21, FF41, 1CCD6, 1D400, 1D41A, 1D434, 1D44E,
                 // 1D468, 1D482, 1D49C, 1D4B6, 1D4D0, 1D4EA, 1D504, 1D51E, 1D538, 1D552, 1D56C, 1D586, 1D5A0,
                 // 1D5BA, 1D5D4, 1D5EE, 1D608, 1D622, 1D63C, 1D656, 1D670, 1D68A, 1F130] ==> [61]
      u8"\x62\0" // #1: [42, 1D2E, 1D47, 212C, 24B7, 24D1, FF22, FF42, 1CCD7, 1D401, 1D41B, 1D435, 1D44F,
                 // 1D469, 1D483, 1D4B7, 1D4D1, 1D4EB, 1D505, 1D51F, 1D539, 1D553, 1D56D, 1D587, 1D5A1, 1D5BB,
                 // 1D5D5, 1D5EF, 1D609, 1D623, 1D63D, 1D657, 1D671, 1D68B, 1F131] ==> [62]
      u8"\x63\0" // #2: [43, 1D9C, 2102, 212D, 216D, 217D, 24B8, 24D2, A7F2, FF23, FF43, 1CCD8, 1D402, 1D41C,
                 // 1D436, 1D450, 1D46A, 1D484, 1D49E, 1D4B8, 1D4D2, 1D4EC, 1D520, 1D554, 1D56E, 1D588, 1D5A2,
                 // 1D5BC, 1D5D6, 1D5F0, 1D60A, 1D624, 1D63E, 1D658, 1D672, 1D68C, 1F12B, 1F132] ==> [63]
      u8"\x64\0" // #3: [44, 1D30, 1D48, 2145, 2146, 216E, 217E, 24B9, 24D3, FF24, FF44, 1CCD9, 1D403, 1D41D,
                 // 1D437, 1D451, 1D46B, 1D485, 1D49F, 1D4B9, 1D4D3, 1D4ED, 1D507, 1D521, 1D53B, 1D555, 1D56F,
                 // 1D589, 1D5A3, 1D5BD, 1D5D7, 1D5F1, 1D60B, 1D625, 1D63F, 1D659, 1D673, 1D68D, 1F133] ==>
                 // [64]
      u8"\x65\0" // #4: [45, 1D31, 1D49, 2091, 212F, 2130, 2147, 24BA, 24D4, FF25, FF45, 1CCDA, 1D404, 1D41E,
                 // 1D438, 1D452, 1D46C, 1D486, 1D4D4, 1D4EE, 1D508, 1D522, 1D53C, 1D556, 1D570, 1D58A, 1D5A4,
                 // 1D5BE, 1D5D8, 1D5F2, 1D60C, 1D626, 1D640, 1D65A, 1D674, 1D68E, 1F134] ==> [65]
      u8"\x66\0" // #5: [46, 1DA0, 2131, 24BB, 24D5, A7F3, FF26, FF46, 1CCDB, 1D405, 1D41F, 1D439, 1D453,
                 // 1D46D, 1D487, 1D4BB, 1D4D5, 1D4EF, 1D509, 1D523, 1D53D, 1D557, 1D571, 1D58B, 1D5A5, 1D5BF,
                 // 1D5D9, 1D5F3, 1D60D, 1D627, 1D641, 1D65B, 1D675, 1D68F, 1F135] ==> [66]
      u8"\x67\0" // #6: [47, 1D33, 1D4D, 210A, 24BC, 24D6, FF27, FF47, 1CCDC, 1D406, 1D420, 1D43A, 1D454,
                 // 1D46E, 1D488, 1D4A2, 1D4D6, 1D4F0, 1D50A, 1D524, 1D53E, 1D558, 1D572, 1D58C, 1D5A6, 1D5C0,
                 // 1D5DA, 1D5F4, 1D60E, 1D628, 1D642, 1D65C, 1D676, 1D690, 1F136] ==> [67]
      u8"\x68\0" // #7: [48, 2B0, 1D34, 2095, 210B, 210C, 210D, 210E, 24BD, 24D7, FF28, FF48, 1CCDD, 1D407,
                 // 1D421, 1D43B, 1D46F, 1D489, 1D4BD, 1D4D7, 1D4F1, 1D525, 1D559, 1D573, 1D58D, 1D5A7, 1D5C1,
                 // 1D5DB, 1D5F5, 1D60F, 1D629, 1D643, 1D65D, 1D677, 1D691, 1F137] ==> [68]
      u8"\x69\0" // #8: [49, 1D35, 1D62, 2071, 2110, 2111, 2139, 2148, 2160, 2170, 24BE, 24D8, FF29, FF49,
                 // 1CCDE, 1D408, 1D422, 1D43C, 1D456, 1D470, 1D48A, 1D4BE, 1D4D8, 1D4F2, 1D526, 1D540, 1D55A,
                 // 1D574, 1D58E, 1D5A8, 1D5C2, 1D5DC, 1D5F6, 1D610, 1D62A, 1D644, 1D65E, 1D678, 1D692, 1F138]
                 // ==> [69]
      u8"\x6a\0" // #9: [4A, 2B2, 1D36, 2149, 24BF, 24D9, 2C7C, FF2A, FF4A, 1CCDF, 1D409, 1D423, 1D43D, 1D457,
                 // 1D471, 1D48B, 1D4A5, 1D4BF, 1D4D9, 1D4F3, 1D50D, 1D527, 1D541, 1D55B, 1D575, 1D58F, 1D5A9,
                 // 1D5C3, 1D5DD, 1D5F7, 1D611, 1D62B, 1D645, 1D65F, 1D679, 1D693, 1F139] ==> [6A]
      u8"\x6b\0" // #10: [4B, 1D37, 1D4F, 2096, 212A, 24C0, 24DA, FF2B, FF4B, 1CCE0, 1D40A, 1D424, 1D43E,
                 // 1D458, 1D472, 1D48C, 1D4A6, 1D4C0, 1D4DA, 1D4F4, 1D50E, 1D528, 1D542, 1D55C, 1D576, 1D590,
                 // 1D5AA, 1D5C4, 1D5DE, 1D5F8, 1D612, 1D62C, 1D646, 1D660, 1D67A, 1D694, 1F13A] ==> [6B]
      u8"\x6c\0" // #11: [4C, 2E1, 1D38, 2097, 2112, 2113, 216C, 217C, 24C1, 24DB, FF2C, FF4C, 1CCE1, 1D40B,
                 // 1D425, 1D43F, 1D459, 1D473, 1D48D, 1D4C1, 1D4DB, 1D4F5, 1D50F, 1D529, 1D543, 1D55D, 1D577,
                 // 1D591, 1D5AB, 1D5C5, 1D5DF, 1D5F9, 1D613, 1D62D, 1D647, 1D661, 1D67B, 1D695, 1F13B] ==>
                 // [6C]
      u8"\x6d\0" // #12: [4D, 1D39, 1D50, 2098, 2133, 216F, 217F, 24C2, 24DC, FF2D, FF4D, 1CCE2, 1D40C, 1D426,
                 // 1D440, 1D45A, 1D474, 1D48E, 1D4C2, 1D4DC, 1D4F6, 1D510, 1D52A, 1D544, 1D55E, 1D578, 1D592,
                 // 1D5AC, 1D5C6, 1D5E0, 1D5FA, 1D614, 1D62E, 1D648, 1D662, 1D67C, 1D696, 1F13C] ==> [6D]
      u8"\x6e\0" // #13: [4E, 1D3A, 207F, 2099, 2115, 24C3, 24DD, FF2E, FF4E, 1CCE3, 1D40D, 1D427, 1D441,
                 // 1D45B, 1D475, 1D48F, 1D4A9, 1D4C3, 1D4DD, 1D4F7, 1D511, 1D52B, 1D55F, 1D579, 1D593, 1D5AD,
                 // 1D5C7, 1D5E1, 1D5FB, 1D615, 1D62F, 1D649, 1D663, 1D67D, 1D697, 1F13D] ==> [6E]
      u8"\x6f\0" // #14: [4F, BA, 1D3C, 1D52, 2092, 2134, 24C4, 24DE, FF2F, FF4F, 1CCE4, 1D40E, 1D428, 1D442,
                 // 1D45C, 1D476, 1D490, 1D4AA, 1D4DE, 1D4F8, 1D512, 1D52C, 1D546, 1D560, 1D57A, 1D594, 1D5AE,
                 // 1D5C8, 1D5E2, 1D5FC, 1D616, 1D630, 1D64A, 1D664, 1D67E, 1D698, 1F13E] ==> [6F]
      u8"\x70\0" // #15: [50, 1D3E, 1D56, 209A, 2119, 24C5, 24DF, FF30, FF50, 1CCE5, 1D40F, 1D429, 1D443,
                 // 1D45D, 1D477, 1D491, 1D4AB, 1D4C5, 1D4DF, 1D4F9, 1D513, 1D52D, 1D561, 1D57B, 1D595, 1D5AF,
                 // 1D5C9, 1D5E3, 1D5FD, 1D617, 1D631, 1D64B, 1D665, 1D67F, 1D699, 1F13F] ==> [70]
      u8"\x71\0" // #16: [51, 211A, 24C6, 24E0, A7F4, FF31, FF51, 107A5, 1CCE6, 1D410, 1D42A, 1D444, 1D45E,
                 // 1D478, 1D492, 1D4AC, 1D4C6, 1D4E0, 1D4FA, 1D514, 1D52E, 1D562, 1D57C, 1D596, 1D5B0, 1D5CA,
                 // 1D5E4, 1D5FE, 1D618, 1D632, 1D64C, 1D666, 1D680, 1D69A, 1F140] ==> [71]
      u8"\x72\0" // #17: [52, 2B3, 1D3F, 1D63, 211B, 211C, 211D, 24C7, 24E1, FF32, FF52, 1CCE7, 1D411, 1D42B,
                 // 1D445, 1D45F, 1D479, 1D493, 1D4C7, 1D4E1, 1D4FB, 1D52F, 1D563, 1D57D, 1D597, 1D5B1, 1D5CB,
                 // 1D5E5, 1D5FF, 1D619, 1D633, 1D64D, 1D667, 1D681, 1D69B, 1F12C, 1F141] ==> [72]
      u8"\x73\0" // #18: [53, 17F, 2E2, 209B, 24C8, 24E2, FF33, FF53, 1CCE8, 1D412, 1D42C, 1D446, 1D460,
                 // 1D47A, 1D494, 1D4AE, 1D4C8, 1D4E2, 1D4FC, 1D516, 1D530, 1D54A, 1D564, 1D57E, 1D598, 1D5B2,
                 // 1D5CC, 1D5E6, 1D600, 1D61A, 1D634, 1D64E, 1D668, 1D682, 1D69C, 1F142] ==> [73]
      u8"\x74\0" // #19: [54, 1D40, 1D57, 209C, 24C9, 24E3, FF34, FF54, 1CCE9, 1D413, 1D42D, 1D447, 1D461,
                 // 1D47B, 1D495, 1D4AF, 1D4C9, 1D4E3, 1D4FD, 1D517, 1D531, 1D54B, 1D565, 1D57F, 1D599, 1D5B3,
                 // 1D5CD, 1D5E7, 1D601, 1D61B, 1D635, 1D64F, 1D669, 1D683, 1D69D, 1F143] ==> [74]
      u8"\x75\0" // #20: [55, 1D41, 1D58, 1D64, 24CA, 24E4, FF35, FF55, 1CCEA, 1D414, 1D42E, 1D448, 1D462,
                 // 1D47C, 1D496, 1D4B0, 1D4CA, 1D4E4, 1D4FE, 1D518, 1D532, 1D54C, 1D566, 1D580, 1D59A, 1D5B4,
                 // 1D5CE, 1D5E8, 1D602, 1D61C, 1D636, 1D650, 1D66A, 1D684, 1D69E, 1F144] ==> [75]
      u8"\x76\0" // #21: [56, 1D5B, 1D65, 2164, 2174, 24CB, 24E5, 2C7D, FF36, FF56, 1CCEB, 1D415, 1D42F,
                 // 1D449, 1D463, 1D47D, 1D497, 1D4B1, 1D4CB, 1D4E5, 1D4FF, 1D519, 1D533, 1D54D, 1D567, 1D581,
                 // 1D59B, 1D5B5, 1D5CF, 1D5E9, 1D603, 1D61D, 1D637, 1D651, 1D66B, 1D685, 1D69F, 1F145] ==>
                 // [76]
      u8"\x77\0" // #22: [57, 2B7, 1D42, 24CC, 24E6, FF37, FF57, 1CCEC, 1D416, 1D430, 1D44A, 1D464, 1D47E,
                 // 1D498, 1D4B2, 1D4CC, 1D4E6, 1D500, 1D51A, 1D534, 1D54E, 1D568, 1D582, 1D59C, 1D5B6, 1D5D0,
                 // 1D5EA, 1D604, 1D61E, 1D638, 1D652, 1D66C, 1D686, 1D6A0, 1F146] ==> [77]
      u8"\x78\0" // #23: [58, 2E3, 2093, 2169, 2179, 24CD, 24E7, FF38, FF58, 1CCED, 1D417, 1D431, 1D44B,
                 // 1D465, 1D47F, 1D499, 1D4B3, 1D4CD, 1D4E7, 1D501, 1D51B, 1D535, 1D54F, 1D569, 1D583, 1D59D,
                 // 1D5B7, 1D5D1, 1D5EB, 1D605, 1D61F, 1D639, 1D653, 1D66D, 1D687, 1D6A1, 1F147] ==> [78]
      u8"\x79\0" // #24: [59, 2B8, 24CE, 24E8, FF39, FF59, 1CCEE, 1D418, 1D432, 1D44C, 1D466, 1D480, 1D49A,
                 // 1D4B4, 1D4CE, 1D4E8, 1D502, 1D51C, 1D536, 1D550, 1D56A, 1D584, 1D59E, 1D5B8, 1D5D2, 1D5EC,
                 // 1D606, 1D620, 1D63A, 1D654, 1D66E, 1D688, 1D6A2, 1F148] ==> [79]
      u8"\x7a\0" // #25: [5A, 1DBB, 2124, 2128, 24CF, 24E9, FF3A, FF5A, 1CCEF, 1D419, 1D433, 1D44D, 1D467,
                 // 1D481, 1D49B, 1D4B5, 1D4CF, 1D4E9, 1D503, 1D537, 1D56B, 1D585, 1D59F, 1D5B9, 1D5D3, 1D5ED,
                 // 1D607, 1D621, 1D63B, 1D655, 1D66F, 1D689, 1D6A3, 1F149] ==> [7A]
      u8"\40\0"  // #26: [A0, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 200A, 202F, 205F,
                 // 3000] ==> [20]
      u8"\40\xcc\x88\0" // #27: [A8] ==> [20, 308]
      u8"\0" // #28: [AD, 34F, 115F, 1160, 17B4, 17B5, 180B, 180C, 180D, 180E, 180F, 200B, 2060, 2061, 2062,
             // 2063, 2064, 206A, 206B, 206C, 206D, 206E, 206F, 3164, FE00, FE01, FE02, FE03, FE04, FE05,
             // FE06, FE07, FE08, FE09, FE0A, FE0B, FE0C, FE0D, FE0E, FE0F, FEFF, FFA0, 1BCA0, 1BCA1, 1BCA2,
             // 1BCA3, 1D173, 1D174, 1D175, 1D176, 1D177, 1D178, 1D179, 1D17A, E0100, E0101, E0102, E0103,
             // E0104, E0105, E0106, E0107, E0108, E0109, E010A, E010B, E010C, E010D, E010E, E010F, E0110,
             // E0111, E0112, E0113, E0114, E0115, E0116, E0117, E0118, E0119, E011A, E011B, E011C, E011D,
             // E011E, E011F, E0120, E0121, E0122, E0123, E0124, E0125, E0126, E0127, E0128, E0129, E012A,
             // E012B, E012C, E012D, E012E, E012F, E0130, E0131, E0132, E0133, E0134, E0135, E0136, E0137,
             // E0138, E0139, E013A, E013B, E013C, E013D, E013E, E013F, E0140, E0141, E0142, E0143, E0144,
             // E0145, E0146, E0147, E0148, E0149, E014A, E014B, E014C, E014D, E014E, E014F, E0150, E0151,
             // E0152, E0153, E0154, E0155, E0156, E0157, E0158, E0159, E015A, E015B, E015C, E015D, E015E,
             // E015F, E0160, E0161, E0162, E0163, E0164, E0165, E0166, E0167, E0168, E0169, E016A, E016B,
             // E016C, E016D, E016E, E016F, E0170, E0171, E0172, E0173, E0174, E0175, E0176, E0177, E0178,
             // E0179, E017A, E017B, E017C, E017D, E017E, E017F, E0180, E0181, E0182, E0183, E0184, E0185,
             // E0186, E0187, E0188, E0189, E018A, E018B, E018C, E018D, E018E, E018F, E0190, E0191, E0192,
             // E0193, E0194, E0195, E0196, E0197, E0198, E0199, E019A, E019B, E019C, E019D, E019E, E019F,
             // E01A0, E01A1, E01A2, E01A3, E01A4, E01A5, E01A6, E01A7, E01A8, E01A9, E01AA, E01AB, E01AC,
             // E01AD, E01AE, E01AF, E01B0, E01B1, E01B2, E01B3, E01B4, E01B5, E01B6, E01B7, E01B8, E01B9,
             // E01BA, E01BB, E01BC, E01BD, E01BE, E01BF, E01C0, E01C1, E01C2, E01C3, E01C4, E01C5, E01C6,
             // E01C7, E01C8, E01C9, E01CA, E01CB, E01CC, E01CD, E01CE, E01CF, E01D0, E01D1, E01D2, E01D3,
             // E01D4, E01D5, E01D6, E01D7, E01D8, E01D9, E01DA, E01DB, E01DC, E01DD, E01DE, E01DF, E01E0,
             // E01E1, E01E2, E01E3, E01E4, E01E5, E01E6, E01E7, E01E8, E01E9, E01EA, E01EB, E01EC, E01ED,
             // E01EE, E01EF] ==> []
      u8"\40\xcc\x84\0" // #29: [AF, FFE3] ==> [20, 304]
      u8"\62\0" // #30: [B2, 2082, 2461, FF12, 1CCF2, 1D7D0, 1D7DA, 1D7E4, 1D7EE, 1D7F8, 1FBF2] ==> [32]
      u8"\63\0" // #31: [B3, 2083, 2462, FF13, 1CCF3, 1D7D1, 1D7DB, 1D7E5, 1D7EF, 1D7F9, 1FBF3] ==> [33]
      u8"\40\xcc\x81\0" // #32: [B4, 384, 1FFD] ==> [20, 301]
      u8"\xce\xbc\0" // #33: [B5, 39C, 1D6B3, 1D6CD, 1D6ED, 1D707, 1D727, 1D741, 1D761, 1D77B, 1D79B, 1D7B5]
                     // ==> [3BC]
      u8"\40\xcc\xa7\0" // #34: [B8] ==> [20, 327]
      u8"\61\0" // #35: [B9, 2081, 2460, FF11, 1CCF1, 1D7CF, 1D7D9, 1D7E3, 1D7ED, 1D7F7, 1FBF1] ==> [31]
      u8"\61\xe2\x81\x84\64\0" // #36: [BC] ==> [31, 2044, 34]
      u8"\61\xe2\x81\x84\62\0" // #37: [BD] ==> [31, 2044, 32]
      u8"\63\xe2\x81\x84\64\0" // #38: [BE] ==> [33, 2044, 34]
      u8"\xc3\xa0\0"           // #39: [C0] ==> [E0]
      u8"\xc3\xa1\0"           // #40: [C1] ==> [E1]
      u8"\xc3\xa2\0"           // #41: [C2] ==> [E2]
      u8"\xc3\xa3\0"           // #42: [C3] ==> [E3]
      u8"\xc3\xa4\0"           // #43: [C4] ==> [E4]
      u8"\xc3\xa5\0"           // #44: [C5, 212B] ==> [E5]
      u8"\xc3\xa6\0"           // #45: [C6, 1D2D, 10783] ==> [E6]
      u8"\xc3\xa7\0"           // #46: [C7] ==> [E7]
      u8"\xc3\xa8\0"           // #47: [C8] ==> [E8]
      u8"\xc3\xa9\0"           // #48: [C9] ==> [E9]
      u8"\xc3\xaa\0"           // #49: [CA] ==> [EA]
      u8"\xc3\xab\0"           // #50: [CB] ==> [EB]
      u8"\xc3\xac\0"           // #51: [CC] ==> [EC]
      u8"\xc3\xad\0"           // #52: [CD] ==> [ED]
      u8"\xc3\xae\0"           // #53: [CE] ==> [EE]
      u8"\xc3\xaf\0"           // #54: [CF] ==> [EF]
      u8"\xc3\xb0\0"           // #55: [D0, 1D9E] ==> [F0]
      u8"\xc3\xb1\0"           // #56: [D1] ==> [F1]
      u8"\xc3\xb2\0"           // #57: [D2] ==> [F2]
      u8"\xc3\xb3\0"           // #58: [D3] ==> [F3]
      u8"\xc3\xb4\0"           // #59: [D4] ==> [F4]
      u8"\xc3\xb5\0"           // #60: [D5] ==> [F5]
      u8"\xc3\xb6\0"           // #61: [D6] ==> [F6]
      u8"\xc3\xb8\0"           // #62: [D8, 107A2] ==> [F8]
      u8"\xc3\xb9\0"           // #63: [D9] ==> [F9]
      u8"\xc3\xba\0"           // #64: [DA] ==> [FA]
      u8"\xc3\xbb\0"           // #65: [DB] ==> [FB]
      u8"\xc3\xbc\0"           // #66: [DC] ==> [FC]
      u8"\xc3\xbd\0"           // #67: [DD] ==> [FD]
      u8"\xc3\xbe\0"           // #68: [DE] ==> [FE]
      u8"\xc4\x81\0"           // #69: [100] ==> [101]
      u8"\xc4\x83\0"           // #70: [102] ==> [103]
      u8"\xc4\x85\0"           // #71: [104] ==> [105]
      u8"\xc4\x87\0"           // #72: [106] ==> [107]
      u8"\xc4\x89\0"           // #73: [108] ==> [109]
      u8"\xc4\x8b\0"           // #74: [10A] ==> [10B]
      u8"\xc4\x8d\0"           // #75: [10C] ==> [10D]
      u8"\xc4\x8f\0"           // #76: [10E] ==> [10F]
      u8"\xc4\x91\0"           // #77: [110] ==> [111]
      u8"\xc4\x93\0"           // #78: [112] ==> [113]
      u8"\xc4\x95\0"           // #79: [114] ==> [115]
      u8"\xc4\x97\0"           // #80: [116] ==> [117]
      u8"\xc4\x99\0"           // #81: [118] ==> [119]
      u8"\xc4\x9b\0"           // #82: [11A] ==> [11B]
      u8"\xc4\x9d\0"           // #83: [11C] ==> [11D]
      u8"\xc4\x9f\0"           // #84: [11E] ==> [11F]
      u8"\xc4\xa1\0"           // #85: [120] ==> [121]
      u8"\xc4\xa3\0"           // #86: [122] ==> [123]
      u8"\xc4\xa5\0"           // #87: [124] ==> [125]
      u8"\xc4\xa7\0"           // #88: [126, 210F, A7F8, 10795] ==> [127]
      u8"\xc4\xa9\0"           // #89: [128] ==> [129]
      u8"\xc4\xab\0"           // #90: [12A] ==> [12B]
      u8"\xc4\xad\0"           // #91: [12C] ==> [12D]
      u8"\xc4\xaf\0"           // #92: [12E] ==> [12F]
      u8"\x69\xcc\x87\0"       // #93: [130] ==> [69, 307]
      u8"\x69\x6a\0"           // #94: [132, 133] ==> [69, 6A]
      u8"\xc4\xb5\0"           // #95: [134] ==> [135]
      u8"\xc4\xb7\0"           // #96: [136] ==> [137]
      u8"\xc4\xba\0"           // #97: [139] ==> [13A]
      u8"\xc4\xbc\0"           // #98: [13B] ==> [13C]
      u8"\xc4\xbe\0"           // #99: [13D] ==> [13E]
      u8"\x6c\xc2\xb7\0"       // #100: [13F, 140] ==> [6C, B7]
      u8"\xc5\x82\0"           // #101: [141] ==> [142]
      u8"\xc5\x84\0"           // #102: [143] ==> [144]
      u8"\xc5\x86\0"           // #103: [145] ==> [146]
      u8"\xc5\x88\0"           // #104: [147] ==> [148]
      u8"\xca\xbc\x6e\0"       // #105: [149] ==> [2BC, 6E]
      u8"\xc5\x8b\0"           // #106: [14A, 1D51] ==> [14B]
      u8"\xc5\x8d\0"           // #107: [14C] ==> [14D]
      u8"\xc5\x8f\0"           // #108: [14E] ==> [14F]
      u8"\xc5\x91\0"           // #109: [150] ==> [151]
      u8"\xc5\x93\0"           // #110: [152, A7F9] ==> [153]
      u8"\xc5\x95\0"           // #111: [154] ==> [155]
      u8"\xc5\x97\0"           // #112: [156] ==> [157]
      u8"\xc5\x99\0"           // #113: [158] ==> [159]
      u8"\xc5\x9b\0"           // #114: [15A] ==> [15B]
      u8"\xc5\x9d\0"           // #115: [15C] ==> [15D]
      u8"\xc5\x9f\0"           // #116: [15E] ==> [15F]
      u8"\xc5\xa1\0"           // #117: [160] ==> [161]
      u8"\xc5\xa3\0"           // #118: [162] ==> [163]
      u8"\xc5\xa5\0"           // #119: [164] ==> [165]
      u8"\xc5\xa7\0"           // #120: [166] ==> [167]
      u8"\xc5\xa9\0"           // #121: [168] ==> [169]
      u8"\xc5\xab\0"           // #122: [16A] ==> [16B]
      u8"\xc5\xad\0"           // #123: [16C] ==> [16D]
      u8"\xc5\xaf\0"           // #124: [16E] ==> [16F]
      u8"\xc5\xb1\0"           // #125: [170] ==> [171]
      u8"\xc5\xb3\0"           // #126: [172] ==> [173]
      u8"\xc5\xb5\0"           // #127: [174] ==> [175]
      u8"\xc5\xb7\0"           // #128: [176] ==> [177]
      u8"\xc3\xbf\0"           // #129: [178] ==> [FF]
      u8"\xc5\xba\0"           // #130: [179] ==> [17A]
      u8"\xc5\xbc\0"           // #131: [17B] ==> [17C]
      u8"\xc5\xbe\0"           // #132: [17D] ==> [17E]
      u8"\xc9\x93\0"           // #133: [181, 10785] ==> [253]
      u8"\xc6\x83\0"           // #134: [182] ==> [183]
      u8"\xc6\x85\0"           // #135: [184] ==> [185]
      u8"\xc9\x94\0"           // #136: [186, 1D53] ==> [254]
      u8"\xc6\x88\0"           // #137: [187] ==> [188]
      u8"\xc9\x96\0"           // #138: [189, 1078B] ==> [256]
      u8"\xc9\x97\0"           // #139: [18A, 1078C] ==> [257]
      u8"\xc6\x8c\0"           // #140: [18B] ==> [18C]
      u8"\xc7\x9d\0"           // #141: [18E, 1D32] ==> [1DD]
      u8"\xc9\x99\0"           // #142: [18F, 1D4A, 2094] ==> [259]
      u8"\xc9\x9b\0"           // #143: [190, 1D4B, 2107] ==> [25B]
      u8"\xc6\x92\0"           // #144: [191] ==> [192]
      u8"\xc9\xa0\0"           // #145: [193, 10793] ==> [260]
      u8"\xc9\xa3\0"           // #146: [194, 2E0] ==> [263]
      u8"\xc9\xa9\0"           // #147: [196, 1DA5] ==> [269]
      u8"\xc9\xa8\0"           // #148: [197, 1DA4] ==> [268]
      u8"\xc6\x99\0"           // #149: [198] ==> [199]
      u8"\xc9\xaf\0"           // #150: [19C, 1D5A] ==> [26F]
      u8"\xc9\xb2\0"           // #151: [19D, 1DAE] ==> [272]
      u8"\xc9\xb5\0"           // #152: [19F, 1DB1] ==> [275]
      u8"\xc6\xa1\0"           // #153: [1A0] ==> [1A1]
      u8"\xc6\xa3\0"           // #154: [1A2] ==> [1A3]
      u8"\xc6\xa5\0"           // #155: [1A4] ==> [1A5]
      u8"\xca\x80\0"           // #156: [1A6, 107AA] ==> [280]
      u8"\xc6\xa8\0"           // #157: [1A7] ==> [1A8]
      u8"\xca\x83\0"           // #158: [1A9, 1DB4] ==> [283]
      u8"\xc6\xad\0"           // #159: [1AC] ==> [1AD]
      u8"\xca\x88\0"           // #160: [1AE, 107AF] ==> [288]
      u8"\xc6\xb0\0"           // #161: [1AF] ==> [1B0]
      u8"\xca\x8a\0"           // #162: [1B1, 1DB7] ==> [28A]
      u8"\xca\x8b\0"           // #163: [1B2, 1DB9] ==> [28B]
      u8"\xc6\xb4\0"           // #164: [1B3] ==> [1B4]
      u8"\xc6\xb6\0"           // #165: [1B5] ==> [1B6]
      u8"\xca\x92\0"           // #166: [1B7, 1DBE] ==> [292]
      u8"\xc6\xb9\0"           // #167: [1B8] ==> [1B9]
      u8"\xc6\xbd\0"           // #168: [1BC] ==> [1BD]
      u8"\x64\xc5\xbe\0"       // #169: [1C4, 1C5, 1C6] ==> [64, 17E]
      u8"\x6c\x6a\0"           // #170: [1C7, 1C8, 1C9] ==> [6C, 6A]
      u8"\x6e\x6a\0"           // #171: [1CA, 1CB, 1CC] ==> [6E, 6A]
      u8"\xc7\x8e\0"           // #172: [1CD] ==> [1CE]
      u8"\xc7\x90\0"           // #173: [1CF] ==> [1D0]
      u8"\xc7\x92\0"           // #174: [1D1] ==> [1D2]
      u8"\xc7\x94\0"           // #175: [1D3] ==> [1D4]
      u8"\xc7\x96\0"           // #176: [1D5] ==> [1D6]
      u8"\xc7\x98\0"           // #177: [1D7] ==> [1D8]
      u8"\xc7\x9a\0"           // #178: [1D9] ==> [1DA]
      u8"\xc7\x9c\0"           // #179: [1DB] ==> [1DC]
      u8"\xc7\x9f\0"           // #180: [1DE] ==> [1DF]
      u8"\xc7\xa1\0"           // #181: [1E0] ==> [1E1]
      u8"\xc7\xa3\0"           // #182: [1E2] ==> [1E3]
      u8"\xc7\xa5\0"           // #183: [1E4] ==> [1E5]
      u8"\xc7\xa7\0"           // #184: [1E6] ==> [1E7]
      u8"\xc7\xa9\0"           // #185: [1E8] ==> [1E9]
      u8"\xc7\xab\0"           // #186: [1EA] ==> [1EB]
      u8"\xc7\xad\0"           // #187: [1EC] ==> [1ED]
      u8"\xc7\xaf\0"           // #188: [1EE] ==> [1EF]
      u8"\x64\x7a\0"           // #189: [1F1, 1F2, 1F3] ==> [64, 7A]
      u8"\xc7\xb5\0"           // #190: [1F4] ==> [1F5]
      u8"\xc6\x95\0"           // #191: [1F6] ==> [195]
      u8"\xc6\xbf\0"           // #192: [1F7] ==> [1BF]
      u8"\xc7\xb9\0"           // #193: [1F8] ==> [1F9]
      u8"\xc7\xbb\0"           // #194: [1FA] ==> [1FB]
      u8"\xc7\xbd\0"           // #195: [1FC] ==> [1FD]
      u8"\xc7\xbf\0"           // #196: [1FE] ==> [1FF]
      u8"\xc8\x81\0"           // #197: [200] ==> [201]
      u8"\xc8\x83\0"           // #198: [202] ==> [203]
      u8"\xc8\x85\0"           // #199: [204] ==> [205]
      u8"\xc8\x87\0"           // #200: [206] ==> [207]
      u8"\xc8\x89\0"           // #201: [208] ==> [209]
      u8"\xc8\x8b\0"           // #202: [20A] ==> [20B]
      u8"\xc8\x8d\0"           // #203: [20C] ==> [20D]
      u8"\xc8\x8f\0"           // #204: [20E] ==> [20F]
      u8"\xc8\x91\0"           // #205: [210] ==> [211]
      u8"\xc8\x93\0"           // #206: [212] ==> [213]
      u8"\xc8\x95\0"           // #207: [214] ==> [215]
      u8"\xc8\x97\0"           // #208: [216] ==> [217]
      u8"\xc8\x99\0"           // #209: [218] ==> [219]
      u8"\xc8\x9b\0"           // #210: [21A] ==> [21B]
      u8"\xc8\x9d\0"           // #211: [21C] ==> [21D]
      u8"\xc8\x9f\0"           // #212: [21E] ==> [21F]
      u8"\xc6\x9e\0"           // #213: [220] ==> [19E]
      u8"\xc8\xa3\0"           // #214: [222, 1D3D] ==> [223]
      u8"\xc8\xa5\0"           // #215: [224] ==> [225]
      u8"\xc8\xa7\0"           // #216: [226] ==> [227]
      u8"\xc8\xa9\0"           // #217: [228] ==> [229]
      u8"\xc8\xab\0"           // #218: [22A] ==> [22B]
      u8"\xc8\xad\0"           // #219: [22C] ==> [22D]
      u8"\xc8\xaf\0"           // #220: [22E] ==> [22F]
      u8"\xc8\xb1\0"           // #221: [230] ==> [231]
      u8"\xc8\xb3\0"           // #222: [232] ==> [233]
      u8"\xe2\xb1\xa5\0"       // #223: [23A] ==> [2C65]
      u8"\xc8\xbc\0"           // #224: [23B] ==> [23C]
      u8"\xc6\x9a\0"           // #225: [23D] ==> [19A]
      u8"\xe2\xb1\xa6\0"       // #226: [23E] ==> [2C66]
      u8"\xc9\x82\0"           // #227: [241] ==> [242]
      u8"\xc6\x80\0"           // #228: [243] ==> [180]
      u8"\xca\x89\0"           // #229: [244, 1DB6] ==> [289]
      u8"\xca\x8c\0"           // #230: [245, 1DBA] ==> [28C]
      u8"\xc9\x87\0"           // #231: [246] ==> [247]
      u8"\xc9\x89\0"           // #232: [248] ==> [249]
      u8"\xc9\x8b\0"           // #233: [24A] ==> [24B]
      u8"\xc9\x8d\0"           // #234: [24C] ==> [24D]
      u8"\xc9\x8f\0"           // #235: [24E] ==> [24F]
      u8"\xc9\xa6\0"           // #236: [2B1, A7AA] ==> [266]
      u8"\xc9\xb9\0"           // #237: [2B4] ==> [279]
      u8"\xc9\xbb\0"           // #238: [2B5] ==> [27B]
      u8"\xca\x81\0"           // #239: [2B6] ==> [281]
      u8"\40\xcc\x86\0"        // #240: [2D8] ==> [20, 306]
      u8"\40\xcc\x87\0"        // #241: [2D9] ==> [20, 307]
      u8"\40\xcc\x8a\0"        // #242: [2DA] ==> [20, 30A]
      u8"\40\xcc\xa8\0"        // #243: [2DB] ==> [20, 328]
      u8"\40\xcc\x83\0"        // #244: [2DC] ==> [20, 303]
      u8"\40\xcc\x8b\0"        // #245: [2DD] ==> [20, 30B]
      u8"\xca\x95\0"           // #246: [2E4] ==> [295]
      u8"\xcc\x80\0"           // #247: [340] ==> [300]
      u8"\xcc\x81\0"           // #248: [341] ==> [301]
      u8"\xcc\x93\0"           // #249: [343] ==> [313]
      u8"\xcc\x88\xcc\x81\0"   // #250: [344] ==> [308, 301]
      u8"\xce\xb9\0" // #251: [345, 399, 1FBE, 1D6B0, 1D6CA, 1D6EA, 1D704, 1D724, 1D73E, 1D75E, 1D778, 1D798,
                     // 1D7B2] ==> [3B9]
      u8"\xcd\xb1\0" // #252: [370] ==> [371]
      u8"\xcd\xb3\0" // #253: [372] ==> [373]
      u8"\xca\xb9\0" // #254: [374] ==> [2B9]
      u8"\xcd\xb7\0" // #255: [376] ==> [377]
      u8"\40\xce\xb9\0"         // #256: [37A] ==> [20, 3B9]
      u8"\73\0"                 // #257: [37E, FE14, FE54, FF1B] ==> [3B]
      u8"\xcf\xb3\0"            // #258: [37F] ==> [3F3]
      u8"\40\xcc\x88\xcc\x81\0" // #259: [385, 1FEE] ==> [20, 308, 301]
      u8"\xce\xac\0"            // #260: [386, 1F71, 1FBB] ==> [3AC]
      u8"\xc2\xb7\0"            // #261: [387] ==> [B7]
      u8"\xce\xad\0"            // #262: [388, 1F73, 1FC9] ==> [3AD]
      u8"\xce\xae\0"            // #263: [389, 1F75, 1FCB] ==> [3AE]
      u8"\xce\xaf\0"            // #264: [38A, 1F77, 1FDB] ==> [3AF]
      u8"\xcf\x8c\0"            // #265: [38C, 1F79, 1FF9] ==> [3CC]
      u8"\xcf\x8d\0"            // #266: [38E, 3D3, 1F7B, 1FEB] ==> [3CD]
      u8"\xcf\x8e\0"            // #267: [38F, 1F7D, 1FFB] ==> [3CE]
      u8"\xce\xb1\0" // #268: [391, 1D6A8, 1D6C2, 1D6E2, 1D6FC, 1D71C, 1D736, 1D756, 1D770, 1D790, 1D7AA] ==>
                     // [3B1]
      u8"\xce\xb2\0" // #269: [392, 3D0, 1D5D, 1D66, 1D6A9, 1D6C3, 1D6E3, 1D6FD, 1D71D, 1D737, 1D757, 1D771,
                     // 1D791, 1D7AB] ==> [3B2]
      u8"\xce\xb3\0" // #270: [393, 1D5E, 1D67, 213D, 213E, 1D6AA, 1D6C4, 1D6E4, 1D6FE, 1D71E, 1D738, 1D758,
                     // 1D772, 1D792, 1D7AC] ==> [3B3]
      u8"\xce\xb4\0" // #271: [394, 1D5F, 1D6AB, 1D6C5, 1D6E5, 1D6FF, 1D71F, 1D739, 1D759, 1D773, 1D793,
                     // 1D7AD] ==> [3B4]
      u8"\xce\xb5\0" // #272: [395, 3F5, 1D6AC, 1D6C6, 1D6DC, 1D6E6, 1D700, 1D716, 1D720, 1D73A, 1D750, 1D75A,
                     // 1D774, 1D78A, 1D794, 1D7AE, 1D7C4] ==> [3B5]
      u8"\xce\xb6\0" // #273: [396, 1D6AD, 1D6C7, 1D6E7, 1D701, 1D721, 1D73B, 1D75B, 1D775, 1D795, 1D7AF] ==>
                     // [3B6]
      u8"\xce\xb7\0" // #274: [397, 1D6AE, 1D6C8, 1D6E8, 1D702, 1D722, 1D73C, 1D75C, 1D776, 1D796, 1D7B0] ==>
                     // [3B7]
      u8"\xce\xb8\0" // #275: [398, 3D1, 3F4, 1DBF, 1D6AF, 1D6B9, 1D6C9, 1D6DD, 1D6E9, 1D6F3, 1D703, 1D717,
                     // 1D723, 1D72D, 1D73D, 1D751, 1D75D, 1D767, 1D777, 1D78B, 1D797, 1D7A1, 1D7B1, 1D7C5]
                     // ==> [3B8]
      u8"\xce\xba\0" // #276: [39A, 3F0, 1D6B1, 1D6CB, 1D6DE, 1D6EB, 1D705, 1D718, 1D725, 1D73F, 1D752, 1D75F,
                     // 1D779, 1D78C, 1D799, 1D7B3, 1D7C6] ==> [3BA]
      u8"\xce\xbb\0" // #277: [39B, 1D6B2, 1D6CC, 1D6EC, 1D706, 1D726, 1D740, 1D760, 1D77A, 1D79A, 1D7B4] ==>
                     // [3BB]
      u8"\xce\xbd\0" // #278: [39D, 1D6B4, 1D6CE, 1D6EE, 1D708, 1D728, 1D742, 1D762, 1D77C, 1D79C, 1D7B6] ==>
                     // [3BD]
      u8"\xce\xbe\0" // #279: [39E, 1D6B5, 1D6CF, 1D6EF, 1D709, 1D729, 1D743, 1D763, 1D77D, 1D79D, 1D7B7] ==>
                     // [3BE]
      u8"\xce\xbf\0" // #280: [39F, 1D6B6, 1D6D0, 1D6F0, 1D70A, 1D72A, 1D744, 1D764, 1D77E, 1D79E, 1D7B8] ==>
                     // [3BF]
      u8"\xcf\x80\0" // #281: [3A0, 3D6, 213C, 213F, 1D6B7, 1D6D1, 1D6E1, 1D6F1, 1D70B, 1D71B, 1D72B, 1D745,
                     // 1D755, 1D765, 1D77F, 1D78F, 1D79F, 1D7B9, 1D7C9] ==> [3C0]
      u8"\xcf\x81\0" // #282: [3A1, 3F1, 1D68, 1D6B8, 1D6D2, 1D6E0, 1D6F2, 1D70C, 1D71A, 1D72C, 1D746, 1D754,
                     // 1D766, 1D780, 1D78E, 1D7A0, 1D7BA, 1D7C8] ==> [3C1]
      u8"\xcf\x83\0" // #283: [3A3, 3F2, 3F9, 1D6BA, 1D6D3, 1D6D4, 1D6F4, 1D70D, 1D70E, 1D72E, 1D747, 1D748,
                     // 1D768, 1D781, 1D782, 1D7A2, 1D7BB, 1D7BC] ==> [3C3]
      u8"\xcf\x84\0" // #284: [3A4, 1D6BB, 1D6D5, 1D6F5, 1D70F, 1D72F, 1D749, 1D769, 1D783, 1D7A3, 1D7BD] ==>
                     // [3C4]
      u8"\xcf\x85\0" // #285: [3A5, 3D2, 1D6BC, 1D6D6, 1D6F6, 1D710, 1D730, 1D74A, 1D76A, 1D784, 1D7A4, 1D7BE]
                     // ==> [3C5]
      u8"\xcf\x86\0" // #286: [3A6, 3D5, 1D60, 1D69, 1D6BD, 1D6D7, 1D6DF, 1D6F7, 1D711, 1D719, 1D731, 1D74B,
                     // 1D753, 1D76B, 1D785, 1D78D, 1D7A5, 1D7BF, 1D7C7] ==> [3C6]
      u8"\xcf\x87\0" // #287: [3A7, 1D61, 1D6A, 1D6BE, 1D6D8, 1D6F8, 1D712, 1D732, 1D74C, 1D76C, 1D786, 1D7A6,
                     // 1D7C0] ==> [3C7]
      u8"\xcf\x88\0" // #288: [3A8, 1D6BF, 1D6D9, 1D6F9, 1D713, 1D733, 1D74D, 1D76D, 1D787, 1D7A7, 1D7C1] ==>
                     // [3C8]
      u8"\xcf\x89\0" // #289: [3A9, 2126, 1D6C0, 1D6DA, 1D6FA, 1D714, 1D734, 1D74E, 1D76E, 1D788, 1D7A8,
                     // 1D7C2] ==> [3C9]
      u8"\xcf\x8a\0" // #290: [3AA] ==> [3CA]
      u8"\xcf\x8b\0" // #291: [3AB, 3D4] ==> [3CB]
      u8"\xcf\x97\0" // #292: [3CF] ==> [3D7]
      u8"\xcf\x99\0" // #293: [3D8] ==> [3D9]
      u8"\xcf\x9b\0" // #294: [3DA] ==> [3DB]
      u8"\xcf\x9d\0" // #295: [3DC, 1D7CA, 1D7CB] ==> [3DD]
      u8"\xcf\x9f\0" // #296: [3DE] ==> [3DF]
      u8"\xcf\xa1\0" // #297: [3E0] ==> [3E1]
      u8"\xcf\xa3\0" // #298: [3E2] ==> [3E3]
      u8"\xcf\xa5\0" // #299: [3E4] ==> [3E5]
      u8"\xcf\xa7\0" // #300: [3E6] ==> [3E7]
      u8"\xcf\xa9\0" // #301: [3E8] ==> [3E9]
      u8"\xcf\xab\0" // #302: [3EA] ==> [3EB]
      u8"\xcf\xad\0" // #303: [3EC] ==> [3ED]
      u8"\xcf\xaf\0" // #304: [3EE] ==> [3EF]
      u8"\xcf\xb8\0" // #305: [3F7] ==> [3F8]
      u8"\xcf\xbb\0" // #306: [3FA] ==> [3FB]
      u8"\xcd\xbb\0" // #307: [3FD] ==> [37B]
      u8"\xcd\xbc\0" // #308: [3FE] ==> [37C]
      u8"\xcd\xbd\0" // #309: [3FF] ==> [37D]
      u8"\xd1\x90\0" // #310: [400] ==> [450]
      u8"\xd1\x91\0" // #311: [401] ==> [451]
      u8"\xd1\x92\0" // #312: [402] ==> [452]
      u8"\xd1\x93\0" // #313: [403] ==> [453]
      u8"\xd1\x94\0" // #314: [404] ==> [454]
      u8"\xd1\x95\0" // #315: [405, 1E069] ==> [455]
      u8"\xd1\x96\0" // #316: [406, 1E04C, 1E068] ==> [456]
      u8"\xd1\x97\0" // #317: [407] ==> [457]
      u8"\xd1\x98\0" // #318: [408, 1E04D] ==> [458]
      u8"\xd1\x99\0" // #319: [409] ==> [459]
      u8"\xd1\x9a\0" // #320: [40A] ==> [45A]
      u8"\xd1\x9b\0" // #321: [40B] ==> [45B]
      u8"\xd1\x9c\0" // #322: [40C] ==> [45C]
      u8"\xd1\x9d\0" // #323: [40D] ==> [45D]
      u8"\xd1\x9e\0" // #324: [40E] ==> [45E]
      u8"\xd1\x9f\0" // #325: [40F, 1E06A] ==> [45F]
      u8"\xd0\xb0\0" // #326: [410, 1E030, 1E051] ==> [430]
      u8"\xd0\xb1\0" // #327: [411, 1E031, 1E052] ==> [431]
      u8"\xd0\xb2\0" // #328: [412, 1C80, 1E032, 1E053] ==> [432]
      u8"\xd0\xb3\0" // #329: [413, 1E033, 1E054] ==> [433]
      u8"\xd0\xb4\0" // #330: [414, 1C81, 1E034, 1E055] ==> [434]
      u8"\xd0\xb5\0" // #331: [415, 1E035, 1E056] ==> [435]
      u8"\xd0\xb6\0" // #332: [416, 1E036, 1E057] ==> [436]
      u8"\xd0\xb7\0" // #333: [417, 1E037, 1E058] ==> [437]
      u8"\xd0\xb8\0" // #334: [418, 1E038, 1E059] ==> [438]
      u8"\xd0\xb9\0" // #335: [419] ==> [439]
      u8"\xd0\xba\0" // #336: [41A, 1E039, 1E05A] ==> [43A]
      u8"\xd0\xbb\0" // #337: [41B, 1E03A, 1E05B] ==> [43B]
      u8"\xd0\xbc\0" // #338: [41C, 1E03B] ==> [43C]
      u8"\xd0\xbd\0" // #339: [41D, 1D78] ==> [43D]
      u8"\xd0\xbe\0" // #340: [41E, 1C82, 1E03C, 1E05C] ==> [43E]
      u8"\xd0\xbf\0" // #341: [41F, 1E03D, 1E05D] ==> [43F]
      u8"\xd1\x80\0" // #342: [420, 1E03E] ==> [440]
      u8"\xd1\x81\0" // #343: [421, 1C83, 1E03F, 1E05E] ==> [441]
      u8"\xd1\x82\0" // #344: [422, 1C84, 1C85, 1E040] ==> [442]
      u8"\xd1\x83\0" // #345: [423, 1E041, 1E05F] ==> [443]
      u8"\xd1\x84\0" // #346: [424, 1E042, 1E060] ==> [444]
      u8"\xd1\x85\0" // #347: [425, 1E043, 1E061] ==> [445]
      u8"\xd1\x86\0" // #348: [426, 1E044, 1E062] ==> [446]
      u8"\xd1\x87\0" // #349: [427, 1E045, 1E063] ==> [447]
      u8"\xd1\x88\0" // #350: [428, 1E046, 1E064] ==> [448]
      u8"\xd1\x89\0" // #351: [429] ==> [449]
      u8"\xd1\x8a\0" // #352: [42A, 1C86, A69C, 1E065] ==> [44A]
      u8"\xd1\x8b\0" // #353: [42B, 1E047, 1E066] ==> [44B]
      u8"\xd1\x8c\0" // #354: [42C, A69D] ==> [44C]
      u8"\xd1\x8d\0" // #355: [42D, 1E048] ==> [44D]
      u8"\xd1\x8e\0" // #356: [42E, 1E049] ==> [44E]
      u8"\xd1\x8f\0" // #357: [42F] ==> [44F]
      u8"\xd1\xa1\0" // #358: [460] ==> [461]
      u8"\xd1\xa3\0" // #359: [462, 1C87] ==> [463]
      u8"\xd1\xa5\0" // #360: [464] ==> [465]
      u8"\xd1\xa7\0" // #361: [466] ==> [467]
      u8"\xd1\xa9\0" // #362: [468] ==> [469]
      u8"\xd1\xab\0" // #363: [46A] ==> [46B]
      u8"\xd1\xad\0" // #364: [46C] ==> [46D]
      u8"\xd1\xaf\0" // #365: [46E] ==> [46F]
      u8"\xd1\xb1\0" // #366: [470] ==> [471]
      u8"\xd1\xb3\0" // #367: [472] ==> [473]
      u8"\xd1\xb5\0" // #368: [474] ==> [475]
      u8"\xd1\xb7\0" // #369: [476] ==> [477]
      u8"\xd1\xb9\0" // #370: [478] ==> [479]
      u8"\xd1\xbb\0" // #371: [47A] ==> [47B]
      u8"\xd1\xbd\0" // #372: [47C] ==> [47D]
      u8"\xd1\xbf\0" // #373: [47E] ==> [47F]
      u8"\xd2\x81\0" // #374: [480] ==> [481]
      u8"\xd2\x8b\0" // #375: [48A] ==> [48B]
      u8"\xd2\x8d\0" // #376: [48C] ==> [48D]
      u8"\xd2\x8f\0" // #377: [48E] ==> [48F]
      u8"\xd2\x91\0" // #378: [490, 1E067] ==> [491]
      u8"\xd2\x93\0" // #379: [492] ==> [493]
      u8"\xd2\x95\0" // #380: [494] ==> [495]
      u8"\xd2\x97\0" // #381: [496] ==> [497]
      u8"\xd2\x99\0" // #382: [498] ==> [499]
      u8"\xd2\x9b\0" // #383: [49A] ==> [49B]
      u8"\xd2\x9d\0" // #384: [49C] ==> [49D]
      u8"\xd2\x9f\0" // #385: [49E] ==> [49F]
      u8"\xd2\xa1\0" // #386: [4A0] ==> [4A1]
      u8"\xd2\xa3\0" // #387: [4A2] ==> [4A3]
      u8"\xd2\xa5\0" // #388: [4A4] ==> [4A5]
      u8"\xd2\xa7\0" // #389: [4A6] ==> [4A7]
      u8"\xd2\xa9\0" // #390: [4A8] ==> [4A9]
      u8"\xd2\xab\0" // #391: [4AA, 1E06B] ==> [4AB]
      u8"\xd2\xad\0" // #392: [4AC] ==> [4AD]
      u8"\xd2\xaf\0" // #393: [4AE, 1E04F] ==> [4AF]
      u8"\xd2\xb1\0" // #394: [4B0, 1E06D] ==> [4B1]
      u8"\xd2\xb3\0" // #395: [4B2] ==> [4B3]
      u8"\xd2\xb5\0" // #396: [4B4] ==> [4B5]
      u8"\xd2\xb7\0" // #397: [4B6] ==> [4B7]
      u8"\xd2\xb9\0" // #398: [4B8] ==> [4B9]
      u8"\xd2\xbb\0" // #399: [4BA] ==> [4BB]
      u8"\xd2\xbd\0" // #400: [4BC] ==> [4BD]
      u8"\xd2\xbf\0" // #401: [4BE] ==> [4BF]
      u8"\xd3\x8f\0" // #402: [4C0, 1E050] ==> [4CF]
      u8"\xd3\x82\0" // #403: [4C1] ==> [4C2]
      u8"\xd3\x84\0" // #404: [4C3] ==> [4C4]
      u8"\xd3\x86\0" // #405: [4C5] ==> [4C6]
      u8"\xd3\x88\0" // #406: [4C7] ==> [4C8]
      u8"\xd3\x8a\0" // #407: [4C9] ==> [4CA]
      u8"\xd3\x8c\0" // #408: [4CB] ==> [4CC]
      u8"\xd3\x8e\0" // #409: [4CD] ==> [4CE]
      u8"\xd3\x91\0" // #410: [4D0] ==> [4D1]
      u8"\xd3\x93\0" // #411: [4D2] ==> [4D3]
      u8"\xd3\x95\0" // #412: [4D4] ==> [4D5]
      u8"\xd3\x97\0" // #413: [4D6] ==> [4D7]
      u8"\xd3\x99\0" // #414: [4D8, 1E04B] ==> [4D9]
      u8"\xd3\x9b\0" // #415: [4DA] ==> [4DB]
      u8"\xd3\x9d\0" // #416: [4DC] ==> [4DD]
      u8"\xd3\x9f\0" // #417: [4DE] ==> [4DF]
      u8"\xd3\xa1\0" // #418: [4E0] ==> [4E1]
      u8"\xd3\xa3\0" // #419: [4E2] ==> [4E3]
      u8"\xd3\xa5\0" // #420: [4E4] ==> [4E5]
      u8"\xd3\xa7\0" // #421: [4E6] ==> [4E7]
      u8"\xd3\xa9\0" // #422: [4E8, 1E04E] ==> [4E9]
      u8"\xd3\xab\0" // #423: [4EA] ==> [4EB]
      u8"\xd3\xad\0" // #424: [4EC] ==> [4ED]
      u8"\xd3\xaf\0" // #425: [4EE] ==> [4EF]
      u8"\xd3\xb1\0" // #426: [4F0] ==> [4F1]
      u8"\xd3\xb3\0" // #427: [4F2] ==> [4F3]
      u8"\xd3\xb5\0" // #428: [4F4] ==> [4F5]
      u8"\xd3\xb7\0" // #429: [4F6] ==> [4F7]
      u8"\xd3\xb9\0" // #430: [4F8] ==> [4F9]
      u8"\xd3\xbb\0" // #431: [4FA] ==> [4FB]
      u8"\xd3\xbd\0" // #432: [4FC] ==> [4FD]
      u8"\xd3\xbf\0" // #433: [4FE] ==> [4FF]
      u8"\xd4\x81\0" // #434: [500] ==> [501]
      u8"\xd4\x83\0" // #435: [502] ==> [503]
      u8"\xd4\x85\0" // #436: [504] ==> [505]
      u8"\xd4\x87\0" // #437: [506] ==> [507]
      u8"\xd4\x89\0" // #438: [508] ==> [509]
      u8"\xd4\x8b\0" // #439: [50A] ==> [50B]
      u8"\xd4\x8d\0" // #440: [50C] ==> [50D]
      u8"\xd4\x8f\0" // #441: [50E] ==> [50F]
      u8"\xd4\x91\0" // #442: [510] ==> [511]
      u8"\xd4\x93\0" // #443: [512] ==> [513]
      u8"\xd4\x95\0" // #444: [514] ==> [515]
      u8"\xd4\x97\0" // #445: [516] ==> [517]
      u8"\xd4\x99\0" // #446: [518] ==> [519]
      u8"\xd4\x9b\0" // #447: [51A] ==> [51B]
      u8"\xd4\x9d\0" // #448: [51C] ==> [51D]
      u8"\xd4\x9f\0" // #449: [51E] ==> [51F]
      u8"\xd4\xa1\0" // #450: [520] ==> [521]
      u8"\xd4\xa3\0" // #451: [522] ==> [523]
      u8"\xd4\xa5\0" // #452: [524] ==> [525]
      u8"\xd4\xa7\0" // #453: [526] ==> [527]
      u8"\xd4\xa9\0" // #454: [528] ==> [529]
      u8"\xd4\xab\0" // #455: [52A] ==> [52B]
      u8"\xd4\xad\0" // #456: [52C] ==> [52D]
      u8"\xd4\xaf\0" // #457: [52E] ==> [52F]
      u8"\xd5\xa1\0" // #458: [531] ==> [561]
      u8"\xd5\xa2\0" // #459: [532] ==> [562]
      u8"\xd5\xa3\0" // #460: [533] ==> [563]
      u8"\xd5\xa4\0" // #461: [534] ==> [564]
      u8"\xd5\xa5\0" // #462: [535] ==> [565]
      u8"\xd5\xa6\0" // #463: [536] ==> [566]
      u8"\xd5\xa7\0" // #464: [537] ==> [567]
      u8"\xd5\xa8\0" // #465: [538] ==> [568]
      u8"\xd5\xa9\0" // #466: [539] ==> [569]
      u8"\xd5\xaa\0" // #467: [53A] ==> [56A]
      u8"\xd5\xab\0" // #468: [53B] ==> [56B]
      u8"\xd5\xac\0" // #469: [53C] ==> [56C]
      u8"\xd5\xad\0" // #470: [53D] ==> [56D]
      u8"\xd5\xae\0" // #471: [53E] ==> [56E]
      u8"\xd5\xaf\0" // #472: [53F] ==> [56F]
      u8"\xd5\xb0\0" // #473: [540] ==> [570]
      u8"\xd5\xb1\0" // #474: [541] ==> [571]
      u8"\xd5\xb2\0" // #475: [542] ==> [572]
      u8"\xd5\xb3\0" // #476: [543] ==> [573]
      u8"\xd5\xb4\0" // #477: [544] ==> [574]
      u8"\xd5\xb5\0" // #478: [545] ==> [575]
      u8"\xd5\xb6\0" // #479: [546] ==> [576]
      u8"\xd5\xb7\0" // #480: [547] ==> [577]
      u8"\xd5\xb8\0" // #481: [548] ==> [578]
      u8"\xd5\xb9\0" // #482: [549] ==> [579]
      u8"\xd5\xba\0" // #483: [54A] ==> [57A]
      u8"\xd5\xbb\0" // #484: [54B] ==> [57B]
      u8"\xd5\xbc\0" // #485: [54C] ==> [57C]
      u8"\xd5\xbd\0" // #486: [54D] ==> [57D]
      u8"\xd5\xbe\0" // #487: [54E] ==> [57E]
      u8"\xd5\xbf\0" // #488: [54F] ==> [57F]
      u8"\xd6\x80\0" // #489: [550] ==> [580]
      u8"\xd6\x81\0" // #490: [551] ==> [581]
      u8"\xd6\x82\0" // #491: [552] ==> [582]
      u8"\xd6\x83\0" // #492: [553] ==> [583]
      u8"\xd6\x84\0" // #493: [554] ==> [584]
      u8"\xd6\x85\0" // #494: [555] ==> [585]
      u8"\xd6\x86\0" // #495: [556] ==> [586]
      u8"\xd5\xa5\xd6\x82\0"                     // #496: [587] ==> [565, 582]
      u8"\xd8\xa7\xd9\xb4\0"                     // #497: [675] ==> [627, 674]
      u8"\xd9\x88\xd9\xb4\0"                     // #498: [676] ==> [648, 674]
      u8"\xdb\x87\xd9\xb4\0"                     // #499: [677, FBDD] ==> [6C7, 674]
      u8"\xd9\x8a\xd9\xb4\0"                     // #500: [678] ==> [64A, 674]
      u8"\xe0\xa4\x95\xe0\xa4\xbc\0"             // #501: [958] ==> [915, 93C]
      u8"\xe0\xa4\x96\xe0\xa4\xbc\0"             // #502: [959] ==> [916, 93C]
      u8"\xe0\xa4\x97\xe0\xa4\xbc\0"             // #503: [95A] ==> [917, 93C]
      u8"\xe0\xa4\x9c\xe0\xa4\xbc\0"             // #504: [95B] ==> [91C, 93C]
      u8"\xe0\xa4\xa1\xe0\xa4\xbc\0"             // #505: [95C] ==> [921, 93C]
      u8"\xe0\xa4\xa2\xe0\xa4\xbc\0"             // #506: [95D] ==> [922, 93C]
      u8"\xe0\xa4\xab\xe0\xa4\xbc\0"             // #507: [95E] ==> [92B, 93C]
      u8"\xe0\xa4\xaf\xe0\xa4\xbc\0"             // #508: [95F] ==> [92F, 93C]
      u8"\xe0\xa6\xa1\xe0\xa6\xbc\0"             // #509: [9DC] ==> [9A1, 9BC]
      u8"\xe0\xa6\xa2\xe0\xa6\xbc\0"             // #510: [9DD] ==> [9A2, 9BC]
      u8"\xe0\xa6\xaf\xe0\xa6\xbc\0"             // #511: [9DF] ==> [9AF, 9BC]
      u8"\xe0\xa8\xb2\xe0\xa8\xbc\0"             // #512: [A33] ==> [A32, A3C]
      u8"\xe0\xa8\xb8\xe0\xa8\xbc\0"             // #513: [A36] ==> [A38, A3C]
      u8"\xe0\xa8\x96\xe0\xa8\xbc\0"             // #514: [A59] ==> [A16, A3C]
      u8"\xe0\xa8\x97\xe0\xa8\xbc\0"             // #515: [A5A] ==> [A17, A3C]
      u8"\xe0\xa8\x9c\xe0\xa8\xbc\0"             // #516: [A5B] ==> [A1C, A3C]
      u8"\xe0\xa8\xab\xe0\xa8\xbc\0"             // #517: [A5E] ==> [A2B, A3C]
      u8"\xe0\xac\xa1\xe0\xac\xbc\0"             // #518: [B5C] ==> [B21, B3C]
      u8"\xe0\xac\xa2\xe0\xac\xbc\0"             // #519: [B5D] ==> [B22, B3C]
      u8"\xe0\xb9\x8d\xe0\xb8\xb2\0"             // #520: [E33] ==> [E4D, E32]
      u8"\xe0\xbb\x8d\xe0\xba\xb2\0"             // #521: [EB3] ==> [ECD, EB2]
      u8"\xe0\xba\xab\xe0\xba\x99\0"             // #522: [EDC] ==> [EAB, E99]
      u8"\xe0\xba\xab\xe0\xba\xa1\0"             // #523: [EDD] ==> [EAB, EA1]
      u8"\xe0\xbc\x8b\0"                         // #524: [F0C] ==> [F0B]
      u8"\xe0\xbd\x82\xe0\xbe\xb7\0"             // #525: [F43] ==> [F42, FB7]
      u8"\xe0\xbd\x8c\xe0\xbe\xb7\0"             // #526: [F4D] ==> [F4C, FB7]
      u8"\xe0\xbd\x91\xe0\xbe\xb7\0"             // #527: [F52] ==> [F51, FB7]
      u8"\xe0\xbd\x96\xe0\xbe\xb7\0"             // #528: [F57] ==> [F56, FB7]
      u8"\xe0\xbd\x9b\xe0\xbe\xb7\0"             // #529: [F5C] ==> [F5B, FB7]
      u8"\xe0\xbd\x80\xe0\xbe\xb5\0"             // #530: [F69] ==> [F40, FB5]
      u8"\xe0\xbd\xb1\xe0\xbd\xb2\0"             // #531: [F73] ==> [F71, F72]
      u8"\xe0\xbd\xb1\xe0\xbd\xb4\0"             // #532: [F75] ==> [F71, F74]
      u8"\xe0\xbe\xb2\xe0\xbe\x80\0"             // #533: [F76] ==> [FB2, F80]
      u8"\xe0\xbe\xb2\xe0\xbd\xb1\xe0\xbe\x80\0" // #534: [F77] ==> [FB2, F71, F80]
      u8"\xe0\xbe\xb3\xe0\xbe\x80\0"             // #535: [F78] ==> [FB3, F80]
      u8"\xe0\xbe\xb3\xe0\xbd\xb1\xe0\xbe\x80\0" // #536: [F79] ==> [FB3, F71, F80]
      u8"\xe0\xbd\xb1\xe0\xbe\x80\0"             // #537: [F81] ==> [F71, F80]
      u8"\xe0\xbe\x92\xe0\xbe\xb7\0"             // #538: [F93] ==> [F92, FB7]
      u8"\xe0\xbe\x9c\xe0\xbe\xb7\0"             // #539: [F9D] ==> [F9C, FB7]
      u8"\xe0\xbe\xa1\xe0\xbe\xb7\0"             // #540: [FA2] ==> [FA1, FB7]
      u8"\xe0\xbe\xa6\xe0\xbe\xb7\0"             // #541: [FA7] ==> [FA6, FB7]
      u8"\xe0\xbe\xab\xe0\xbe\xb7\0"             // #542: [FAC] ==> [FAB, FB7]
      u8"\xe0\xbe\x90\xe0\xbe\xb5\0"             // #543: [FB9] ==> [F90, FB5]
      u8"\xe2\xb4\x80\0"                         // #544: [10A0] ==> [2D00]
      u8"\xe2\xb4\x81\0"                         // #545: [10A1] ==> [2D01]
      u8"\xe2\xb4\x82\0"                         // #546: [10A2] ==> [2D02]
      u8"\xe2\xb4\x83\0"                         // #547: [10A3] ==> [2D03]
      u8"\xe2\xb4\x84\0"                         // #548: [10A4] ==> [2D04]
      u8"\xe2\xb4\x85\0"                         // #549: [10A5] ==> [2D05]
      u8"\xe2\xb4\x86\0"                         // #550: [10A6] ==> [2D06]
      u8"\xe2\xb4\x87\0"                         // #551: [10A7] ==> [2D07]
      u8"\xe2\xb4\x88\0"                         // #552: [10A8] ==> [2D08]
      u8"\xe2\xb4\x89\0"                         // #553: [10A9] ==> [2D09]
      u8"\xe2\xb4\x8a\0"                         // #554: [10AA] ==> [2D0A]
      u8"\xe2\xb4\x8b\0"                         // #555: [10AB] ==> [2D0B]
      u8"\xe2\xb4\x8c\0"                         // #556: [10AC] ==> [2D0C]
      u8"\xe2\xb4\x8d\0"                         // #557: [10AD] ==> [2D0D]
      u8"\xe2\xb4\x8e\0"                         // #558: [10AE] ==> [2D0E]
      u8"\xe2\xb4\x8f\0"                         // #559: [10AF] ==> [2D0F]
      u8"\xe2\xb4\x90\0"                         // #560: [10B0] ==> [2D10]
      u8"\xe2\xb4\x91\0"                         // #561: [10B1] ==> [2D11]
      u8"\xe2\xb4\x92\0"                         // #562: [10B2] ==> [2D12]
      u8"\xe2\xb4\x93\0"                         // #563: [10B3] ==> [2D13]
      u8"\xe2\xb4\x94\0"                         // #564: [10B4] ==> [2D14]
      u8"\xe2\xb4\x95\0"                         // #565: [10B5] ==> [2D15]
      u8"\xe2\xb4\x96\0"                         // #566: [10B6] ==> [2D16]
      u8"\xe2\xb4\x97\0"                         // #567: [10B7] ==> [2D17]
      u8"\xe2\xb4\x98\0"                         // #568: [10B8] ==> [2D18]
      u8"\xe2\xb4\x99\0"                         // #569: [10B9] ==> [2D19]
      u8"\xe2\xb4\x9a\0"                         // #570: [10BA] ==> [2D1A]
      u8"\xe2\xb4\x9b\0"                         // #571: [10BB] ==> [2D1B]
      u8"\xe2\xb4\x9c\0"                         // #572: [10BC] ==> [2D1C]
      u8"\xe2\xb4\x9d\0"                         // #573: [10BD] ==> [2D1D]
      u8"\xe2\xb4\x9e\0"                         // #574: [10BE] ==> [2D1E]
      u8"\xe2\xb4\x9f\0"                         // #575: [10BF] ==> [2D1F]
      u8"\xe2\xb4\xa0\0"                         // #576: [10C0] ==> [2D20]
      u8"\xe2\xb4\xa1\0"                         // #577: [10C1] ==> [2D21]
      u8"\xe2\xb4\xa2\0"                         // #578: [10C2] ==> [2D22]
      u8"\xe2\xb4\xa3\0"                         // #579: [10C3] ==> [2D23]
      u8"\xe2\xb4\xa4\0"                         // #580: [10C4] ==> [2D24]
      u8"\xe2\xb4\xa5\0"                         // #581: [10C5] ==> [2D25]
      u8"\xe2\xb4\xa7\0"                         // #582: [10C7] ==> [2D27]
      u8"\xe2\xb4\xad\0"                         // #583: [10CD] ==> [2D2D]
      u8"\xe1\x83\x9c\0"                         // #584: [10FC, 1C9C] ==> [10DC]
      u8"\xe1\x8f\xb0\0"                         // #585: [13F8] ==> [13F0]
      u8"\xe1\x8f\xb1\0"                         // #586: [13F9] ==> [13F1]
      u8"\xe1\x8f\xb2\0"                         // #587: [13FA] ==> [13F2]
      u8"\xe1\x8f\xb3\0"                         // #588: [13FB] ==> [13F3]
      u8"\xe1\x8f\xb4\0"                         // #589: [13FC] ==> [13F4]
      u8"\xe1\x8f\xb5\0"                         // #590: [13FD] ==> [13F5]
      u8"\xea\x99\x8b\0"                         // #591: [1C88, A64A] ==> [A64B]
      u8"\xe1\xb2\x8a\0"                         // #592: [1C89] ==> [1C8A]
      u8"\xe1\x83\x90\0"                         // #593: [1C90] ==> [10D0]
      u8"\xe1\x83\x91\0"                         // #594: [1C91] ==> [10D1]
      u8"\xe1\x83\x92\0"                         // #595: [1C92] ==> [10D2]
      u8"\xe1\x83\x93\0"                         // #596: [1C93] ==> [10D3]
      u8"\xe1\x83\x94\0"                         // #597: [1C94] ==> [10D4]
      u8"\xe1\x83\x95\0"                         // #598: [1C95] ==> [10D5]
      u8"\xe1\x83\x96\0"                         // #599: [1C96] ==> [10D6]
      u8"\xe1\x83\x97\0"                         // #600: [1C97] ==> [10D7]
      u8"\xe1\x83\x98\0"                         // #601: [1C98] ==> [10D8]
      u8"\xe1\x83\x99\0"                         // #602: [1C99] ==> [10D9]
      u8"\xe1\x83\x9a\0"                         // #603: [1C9A] ==> [10DA]
      u8"\xe1\x83\x9b\0"                         // #604: [1C9B] ==> [10DB]
      u8"\xe1\x83\x9d\0"                         // #605: [1C9D] ==> [10DD]
      u8"\xe1\x83\x9e\0"                         // #606: [1C9E] ==> [10DE]
      u8"\xe1\x83\x9f\0"                         // #607: [1C9F] ==> [10DF]
      u8"\xe1\x83\xa0\0"                         // #608: [1CA0] ==> [10E0]
      u8"\xe1\x83\xa1\0"                         // #609: [1CA1] ==> [10E1]
      u8"\xe1\x83\xa2\0"                         // #610: [1CA2] ==> [10E2]
      u8"\xe1\x83\xa3\0"                         // #611: [1CA3] ==> [10E3]
      u8"\xe1\x83\xa4\0"                         // #612: [1CA4] ==> [10E4]
      u8"\xe1\x83\xa5\0"                         // #613: [1CA5] ==> [10E5]
      u8"\xe1\x83\xa6\0"                         // #614: [1CA6] ==> [10E6]
      u8"\xe1\x83\xa7\0"                         // #615: [1CA7] ==> [10E7]
      u8"\xe1\x83\xa8\0"                         // #616: [1CA8] ==> [10E8]
      u8"\xe1\x83\xa9\0"                         // #617: [1CA9] ==> [10E9]
      u8"\xe1\x83\xaa\0"                         // #618: [1CAA] ==> [10EA]
      u8"\xe1\x83\xab\0"                         // #619: [1CAB] ==> [10EB]
      u8"\xe1\x83\xac\0"                         // #620: [1CAC] ==> [10EC]
      u8"\xe1\x83\xad\0"                         // #621: [1CAD] ==> [10ED]
      u8"\xe1\x83\xae\0"                         // #622: [1CAE] ==> [10EE]
      u8"\xe1\x83\xaf\0"                         // #623: [1CAF] ==> [10EF]
      u8"\xe1\x83\xb0\0"                         // #624: [1CB0] ==> [10F0]
      u8"\xe1\x83\xb1\0"                         // #625: [1CB1] ==> [10F1]
      u8"\xe1\x83\xb2\0"                         // #626: [1CB2] ==> [10F2]
      u8"\xe1\x83\xb3\0"                         // #627: [1CB3] ==> [10F3]
      u8"\xe1\x83\xb4\0"                         // #628: [1CB4] ==> [10F4]
      u8"\xe1\x83\xb5\0"                         // #629: [1CB5] ==> [10F5]
      u8"\xe1\x83\xb6\0"                         // #630: [1CB6] ==> [10F6]
      u8"\xe1\x83\xb7\0"                         // #631: [1CB7] ==> [10F7]
      u8"\xe1\x83\xb8\0"                         // #632: [1CB8] ==> [10F8]
      u8"\xe1\x83\xb9\0"                         // #633: [1CB9] ==> [10F9]
      u8"\xe1\x83\xba\0"                         // #634: [1CBA] ==> [10FA]
      u8"\xe1\x83\xbd\0"                         // #635: [1CBD] ==> [10FD]
      u8"\xe1\x83\xbe\0"                         // #636: [1CBE] ==> [10FE]
      u8"\xe1\x83\xbf\0"                         // #637: [1CBF] ==> [10FF]
      u8"\xc9\x90\0"                             // #638: [1D44, 2C6F] ==> [250]
      u8"\xc9\x91\0"                             // #639: [1D45, 2C6D] ==> [251]
      u8"\xe1\xb4\x82\0"                         // #640: [1D46] ==> [1D02]
      u8"\xc9\x9c\0"                             // #641: [1D4C, 1D9F, A7AB] ==> [25C]
      u8"\xe1\xb4\x96\0"                         // #642: [1D54] ==> [1D16]
      u8"\xe1\xb4\x97\0"                         // #643: [1D55] ==> [1D17]
      u8"\xe1\xb4\x9d\0"                         // #644: [1D59] ==> [1D1D]
      u8"\xe1\xb4\xa5\0"                         // #645: [1D5C] ==> [1D25]
      u8"\xc9\x92\0"                             // #646: [1D9B, 2C70] ==> [252]
      u8"\xc9\x95\0"                             // #647: [1D9D] ==> [255]
      u8"\xc9\x9f\0"                             // #648: [1DA1] ==> [25F]
      u8"\xc9\xa1\0"                             // #649: [1DA2, A7AC] ==> [261]
      u8"\xc9\xa5\0"                             // #650: [1DA3, A78D] ==> [265]
      u8"\xc9\xaa\0"                             // #651: [1DA6, A7AE] ==> [26A]
      u8"\xe1\xb5\xbb\0"                         // #652: [1DA7] ==> [1D7B]
      u8"\xca\x9d\0"                             // #653: [1DA8, A7B2] ==> [29D]
      u8"\xc9\xad\0"                             // #654: [1DA9] ==> [26D]
      u8"\xe1\xb6\x85\0"                         // #655: [1DAA] ==> [1D85]
      u8"\xca\x9f\0"                             // #656: [1DAB] ==> [29F]
      u8"\xc9\xb1\0"                             // #657: [1DAC, 2C6E] ==> [271]
      u8"\xc9\xb0\0"                             // #658: [1DAD] ==> [270]
      u8"\xc9\xb3\0"                             // #659: [1DAF] ==> [273]
      u8"\xc9\xb4\0"                             // #660: [1DB0] ==> [274]
      u8"\xc9\xb8\0"                             // #661: [1DB2] ==> [278]
      u8"\xca\x82\0"                             // #662: [1DB3, A7C5] ==> [282]
      u8"\xc6\xab\0"                             // #663: [1DB5] ==> [1AB]
      u8"\xe1\xb4\x9c\0"                         // #664: [1DB8] ==> [1D1C]
      u8"\xca\x90\0"                             // #665: [1DBC] ==> [290]
      u8"\xca\x91\0"                             // #666: [1DBD] ==> [291]
      u8"\xe1\xb8\x81\0"                         // #667: [1E00] ==> [1E01]
      u8"\xe1\xb8\x83\0"                         // #668: [1E02] ==> [1E03]
      u8"\xe1\xb8\x85\0"                         // #669: [1E04] ==> [1E05]
      u8"\xe1\xb8\x87\0"                         // #670: [1E06] ==> [1E07]
      u8"\xe1\xb8\x89\0"                         // #671: [1E08] ==> [1E09]
      u8"\xe1\xb8\x8b\0"                         // #672: [1E0A] ==> [1E0B]
      u8"\xe1\xb8\x8d\0"                         // #673: [1E0C] ==> [1E0D]
      u8"\xe1\xb8\x8f\0"                         // #674: [1E0E] ==> [1E0F]
      u8"\xe1\xb8\x91\0"                         // #675: [1E10] ==> [1E11]
      u8"\xe1\xb8\x93\0"                         // #676: [1E12] ==> [1E13]
      u8"\xe1\xb8\x95\0"                         // #677: [1E14] ==> [1E15]
      u8"\xe1\xb8\x97\0"                         // #678: [1E16] ==> [1E17]
      u8"\xe1\xb8\x99\0"                         // #679: [1E18] ==> [1E19]
      u8"\xe1\xb8\x9b\0"                         // #680: [1E1A] ==> [1E1B]
      u8"\xe1\xb8\x9d\0"                         // #681: [1E1C] ==> [1E1D]
      u8"\xe1\xb8\x9f\0"                         // #682: [1E1E] ==> [1E1F]
      u8"\xe1\xb8\xa1\0"                         // #683: [1E20] ==> [1E21]
      u8"\xe1\xb8\xa3\0"                         // #684: [1E22] ==> [1E23]
      u8"\xe1\xb8\xa5\0"                         // #685: [1E24] ==> [1E25]
      u8"\xe1\xb8\xa7\0"                         // #686: [1E26] ==> [1E27]
      u8"\xe1\xb8\xa9\0"                         // #687: [1E28] ==> [1E29]
      u8"\xe1\xb8\xab\0"                         // #688: [1E2A] ==> [1E2B]
      u8"\xe1\xb8\xad\0"                         // #689: [1E2C] ==> [1E2D]
      u8"\xe1\xb8\xaf\0"                         // #690: [1E2E] ==> [1E2F]
      u8"\xe1\xb8\xb1\0"                         // #691: [1E30] ==> [1E31]
      u8"\xe1\xb8\xb3\0"                         // #692: [1E32] ==> [1E33]
      u8"\xe1\xb8\xb5\0"                         // #693: [1E34] ==> [1E35]
      u8"\xe1\xb8\xb7\0"                         // #694: [1E36] ==> [1E37]
      u8"\xe1\xb8\xb9\0"                         // #695: [1E38] ==> [1E39]
      u8"\xe1\xb8\xbb\0"                         // #696: [1E3A] ==> [1E3B]
      u8"\xe1\xb8\xbd\0"                         // #697: [1E3C] ==> [1E3D]
      u8"\xe1\xb8\xbf\0"                         // #698: [1E3E] ==> [1E3F]
      u8"\xe1\xb9\x81\0"                         // #699: [1E40] ==> [1E41]
      u8"\xe1\xb9\x83\0"                         // #700: [1E42] ==> [1E43]
      u8"\xe1\xb9\x85\0"                         // #701: [1E44] ==> [1E45]
      u8"\xe1\xb9\x87\0"                         // #702: [1E46] ==> [1E47]
      u8"\xe1\xb9\x89\0"                         // #703: [1E48] ==> [1E49]
      u8"\xe1\xb9\x8b\0"                         // #704: [1E4A] ==> [1E4B]
      u8"\xe1\xb9\x8d\0"                         // #705: [1E4C] ==> [1E4D]
      u8"\xe1\xb9\x8f\0"                         // #706: [1E4E] ==> [1E4F]
      u8"\xe1\xb9\x91\0"                         // #707: [1E50] ==> [1E51]
      u8"\xe1\xb9\x93\0"                         // #708: [1E52] ==> [1E53]
      u8"\xe1\xb9\x95\0"                         // #709: [1E54] ==> [1E55]
      u8"\xe1\xb9\x97\0"                         // #710: [1E56] ==> [1E57]
      u8"\xe1\xb9\x99\0"                         // #711: [1E58] ==> [1E59]
      u8"\xe1\xb9\x9b\0"                         // #712: [1E5A] ==> [1E5B]
      u8"\xe1\xb9\x9d\0"                         // #713: [1E5C] ==> [1E5D]
      u8"\xe1\xb9\x9f\0"                         // #714: [1E5E] ==> [1E5F]
      u8"\xe1\xb9\xa1\0"                         // #715: [1E60, 1E9B] ==> [1E61]
      u8"\xe1\xb9\xa3\0"                         // #716: [1E62] ==> [1E63]
      u8"\xe1\xb9\xa5\0"                         // #717: [1E64] ==> [1E65]
      u8"\xe1\xb9\xa7\0"                         // #718: [1E66] ==> [1E67]
      u8"\xe1\xb9\xa9\0"                         // #719: [1E68] ==> [1E69]
      u8"\xe1\xb9\xab\0"                         // #720: [1E6A] ==> [1E6B]
      u8"\xe1\xb9\xad\0"                         // #721: [1E6C] ==> [1E6D]
      u8"\xe1\xb9\xaf\0"                         // #722: [1E6E] ==> [1E6F]
      u8"\xe1\xb9\xb1\0"                         // #723: [1E70] ==> [1E71]
      u8"\xe1\xb9\xb3\0"                         // #724: [1E72] ==> [1E73]
      u8"\xe1\xb9\xb5\0"                         // #725: [1E74] ==> [1E75]
      u8"\xe1\xb9\xb7\0"                         // #726: [1E76] ==> [1E77]
      u8"\xe1\xb9\xb9\0"                         // #727: [1E78] ==> [1E79]
      u8"\xe1\xb9\xbb\0"                         // #728: [1E7A] ==> [1E7B]
      u8"\xe1\xb9\xbd\0"                         // #729: [1E7C] ==> [1E7D]
      u8"\xe1\xb9\xbf\0"                         // #730: [1E7E] ==> [1E7F]
      u8"\xe1\xba\x81\0"                         // #731: [1E80] ==> [1E81]
      u8"\xe1\xba\x83\0"                         // #732: [1E82] ==> [1E83]
      u8"\xe1\xba\x85\0"                         // #733: [1E84] ==> [1E85]
      u8"\xe1\xba\x87\0"                         // #734: [1E86] ==> [1E87]
      u8"\xe1\xba\x89\0"                         // #735: [1E88] ==> [1E89]
      u8"\xe1\xba\x8b\0"                         // #736: [1E8A] ==> [1E8B]
      u8"\xe1\xba\x8d\0"                         // #737: [1E8C] ==> [1E8D]
      u8"\xe1\xba\x8f\0"                         // #738: [1E8E] ==> [1E8F]
      u8"\xe1\xba\x91\0"                         // #739: [1E90] ==> [1E91]
      u8"\xe1\xba\x93\0"                         // #740: [1E92] ==> [1E93]
      u8"\xe1\xba\x95\0"                         // #741: [1E94] ==> [1E95]
      u8"\x61\xca\xbe\0"                         // #742: [1E9A] ==> [61, 2BE]
      u8"\xc3\x9f\0"                             // #743: [1E9E] ==> [DF]
      u8"\xe1\xba\xa1\0"                         // #744: [1EA0] ==> [1EA1]
      u8"\xe1\xba\xa3\0"                         // #745: [1EA2] ==> [1EA3]
      u8"\xe1\xba\xa5\0"                         // #746: [1EA4] ==> [1EA5]
      u8"\xe1\xba\xa7\0"                         // #747: [1EA6] ==> [1EA7]
      u8"\xe1\xba\xa9\0"                         // #748: [1EA8] ==> [1EA9]
      u8"\xe1\xba\xab\0"                         // #749: [1EAA] ==> [1EAB]
      u8"\xe1\xba\xad\0"                         // #750: [1EAC] ==> [1EAD]
      u8"\xe1\xba\xaf\0"                         // #751: [1EAE] ==> [1EAF]
      u8"\xe1\xba\xb1\0"                         // #752: [1EB0] ==> [1EB1]
      u8"\xe1\xba\xb3\0"                         // #753: [1EB2] ==> [1EB3]
      u8"\xe1\xba\xb5\0"                         // #754: [1EB4] ==> [1EB5]
      u8"\xe1\xba\xb7\0"                         // #755: [1EB6] ==> [1EB7]
      u8"\xe1\xba\xb9\0"                         // #756: [1EB8] ==> [1EB9]
      u8"\xe1\xba\xbb\0"                         // #757: [1EBA] ==> [1EBB]
      u8"\xe1\xba\xbd\0"                         // #758: [1EBC] ==> [1EBD]
      u8"\xe1\xba\xbf\0"                         // #759: [1EBE] ==> [1EBF]
      u8"\xe1\xbb\x81\0"                         // #760: [1EC0] ==> [1EC1]
      u8"\xe1\xbb\x83\0"                         // #761: [1EC2] ==> [1EC3]
      u8"\xe1\xbb\x85\0"                         // #762: [1EC4] ==> [1EC5]
      u8"\xe1\xbb\x87\0"                         // #763: [1EC6] ==> [1EC7]
      u8"\xe1\xbb\x89\0"                         // #764: [1EC8] ==> [1EC9]
      u8"\xe1\xbb\x8b\0"                         // #765: [1ECA] ==> [1ECB]
      u8"\xe1\xbb\x8d\0"                         // #766: [1ECC] ==> [1ECD]
      u8"\xe1\xbb\x8f\0"                         // #767: [1ECE] ==> [1ECF]
      u8"\xe1\xbb\x91\0"                         // #768: [1ED0] ==> [1ED1]
      u8"\xe1\xbb\x93\0"                         // #769: [1ED2] ==> [1ED3]
      u8"\xe1\xbb\x95\0"                         // #770: [1ED4] ==> [1ED5]
      u8"\xe1\xbb\x97\0"                         // #771: [1ED6] ==> [1ED7]
      u8"\xe1\xbb\x99\0"                         // #772: [1ED8] ==> [1ED9]
      u8"\xe1\xbb\x9b\0"                         // #773: [1EDA] ==> [1EDB]
      u8"\xe1\xbb\x9d\0"                         // #774: [1EDC] ==> [1EDD]
      u8"\xe1\xbb\x9f\0"                         // #775: [1EDE] ==> [1EDF]
      u8"\xe1\xbb\xa1\0"                         // #776: [1EE0] ==> [1EE1]
      u8"\xe1\xbb\xa3\0"                         // #777: [1EE2] ==> [1EE3]
      u8"\xe1\xbb\xa5\0"                         // #778: [1EE4] ==> [1EE5]
      u8"\xe1\xbb\xa7\0"                         // #779: [1EE6] ==> [1EE7]
      u8"\xe1\xbb\xa9\0"                         // #780: [1EE8] ==> [1EE9]
      u8"\xe1\xbb\xab\0"                         // #781: [1EEA] ==> [1EEB]
      u8"\xe1\xbb\xad\0"                         // #782: [1EEC] ==> [1EED]
      u8"\xe1\xbb\xaf\0"                         // #783: [1EEE] ==> [1EEF]
      u8"\xe1\xbb\xb1\0"                         // #784: [1EF0] ==> [1EF1]
      u8"\xe1\xbb\xb3\0"                         // #785: [1EF2] ==> [1EF3]
      u8"\xe1\xbb\xb5\0"                         // #786: [1EF4] ==> [1EF5]
      u8"\xe1\xbb\xb7\0"                         // #787: [1EF6] ==> [1EF7]
      u8"\xe1\xbb\xb9\0"                         // #788: [1EF8] ==> [1EF9]
      u8"\xe1\xbb\xbb\0"                         // #789: [1EFA] ==> [1EFB]
      u8"\xe1\xbb\xbd\0"                         // #790: [1EFC] ==> [1EFD]
      u8"\xe1\xbb\xbf\0"                         // #791: [1EFE] ==> [1EFF]
      u8"\xe1\xbc\x80\0"                         // #792: [1F08] ==> [1F00]
      u8"\xe1\xbc\x81\0"                         // #793: [1F09] ==> [1F01]
      u8"\xe1\xbc\x82\0"                         // #794: [1F0A] ==> [1F02]
      u8"\xe1\xbc\x83\0"                         // #795: [1F0B] ==> [1F03]
      u8"\xe1\xbc\x84\0"                         // #796: [1F0C] ==> [1F04]
      u8"\xe1\xbc\x85\0"                         // #797: [1F0D] ==> [1F05]
      u8"\xe1\xbc\x86\0"                         // #798: [1F0E] ==> [1F06]
      u8"\xe1\xbc\x87\0"                         // #799: [1F0F] ==> [1F07]
      u8"\xe1\xbc\x90\0"                         // #800: [1F18] ==> [1F10]
      u8"\xe1\xbc\x91\0"                         // #801: [1F19] ==> [1F11]
      u8"\xe1\xbc\x92\0"                         // #802: [1F1A] ==> [1F12]
      u8"\xe1\xbc\x93\0"                         // #803: [1F1B] ==> [1F13]
      u8"\xe1\xbc\x94\0"                         // #804: [1F1C] ==> [1F14]
      u8"\xe1\xbc\x95\0"                         // #805: [1F1D] ==> [1F15]
      u8"\xe1\xbc\xa0\0"                         // #806: [1F28] ==> [1F20]
      u8"\xe1\xbc\xa1\0"                         // #807: [1F29] ==> [1F21]
      u8"\xe1\xbc\xa2\0"                         // #808: [1F2A] ==> [1F22]
      u8"\xe1\xbc\xa3\0"                         // #809: [1F2B] ==> [1F23]
      u8"\xe1\xbc\xa4\0"                         // #810: [1F2C] ==> [1F24]
      u8"\xe1\xbc\xa5\0"                         // #811: [1F2D] ==> [1F25]
      u8"\xe1\xbc\xa6\0"                         // #812: [1F2E] ==> [1F26]
      u8"\xe1\xbc\xa7\0"                         // #813: [1F2F] ==> [1F27]
      u8"\xe1\xbc\xb0\0"                         // #814: [1F38] ==> [1F30]
      u8"\xe1\xbc\xb1\0"                         // #815: [1F39] ==> [1F31]
      u8"\xe1\xbc\xb2\0"                         // #816: [1F3A] ==> [1F32]
      u8"\xe1\xbc\xb3\0"                         // #817: [1F3B] ==> [1F33]
      u8"\xe1\xbc\xb4\0"                         // #818: [1F3C] ==> [1F34]
      u8"\xe1\xbc\xb5\0"                         // #819: [1F3D] ==> [1F35]
      u8"\xe1\xbc\xb6\0"                         // #820: [1F3E] ==> [1F36]
      u8"\xe1\xbc\xb7\0"                         // #821: [1F3F] ==> [1F37]
      u8"\xe1\xbd\x80\0"                         // #822: [1F48] ==> [1F40]
      u8"\xe1\xbd\x81\0"                         // #823: [1F49] ==> [1F41]
      u8"\xe1\xbd\x82\0"                         // #824: [1F4A] ==> [1F42]
      u8"\xe1\xbd\x83\0"                         // #825: [1F4B] ==> [1F43]
      u8"\xe1\xbd\x84\0"                         // #826: [1F4C] ==> [1F44]
      u8"\xe1\xbd\x85\0"                         // #827: [1F4D] ==> [1F45]
      u8"\xe1\xbd\x91\0"                         // #828: [1F59] ==> [1F51]
      u8"\xe1\xbd\x93\0"                         // #829: [1F5B] ==> [1F53]
      u8"\xe1\xbd\x95\0"                         // #830: [1F5D] ==> [1F55]
      u8"\xe1\xbd\x97\0"                         // #831: [1F5F] ==> [1F57]
      u8"\xe1\xbd\xa0\0"                         // #832: [1F68] ==> [1F60]
      u8"\xe1\xbd\xa1\0"                         // #833: [1F69] ==> [1F61]
      u8"\xe1\xbd\xa2\0"                         // #834: [1F6A] ==> [1F62]
      u8"\xe1\xbd\xa3\0"                         // #835: [1F6B] ==> [1F63]
      u8"\xe1\xbd\xa4\0"                         // #836: [1F6C] ==> [1F64]
      u8"\xe1\xbd\xa5\0"                         // #837: [1F6D] ==> [1F65]
      u8"\xe1\xbd\xa6\0"                         // #838: [1F6E] ==> [1F66]
      u8"\xe1\xbd\xa7\0"                         // #839: [1F6F] ==> [1F67]
      u8"\xe1\xbc\x80\xce\xb9\0"                 // #840: [1F80, 1F88] ==> [1F00, 3B9]
      u8"\xe1\xbc\x81\xce\xb9\0"                 // #841: [1F81, 1F89] ==> [1F01, 3B9]
      u8"\xe1\xbc\x82\xce\xb9\0"                 // #842: [1F82, 1F8A] ==> [1F02, 3B9]
      u8"\xe1\xbc\x83\xce\xb9\0"                 // #843: [1F83, 1F8B] ==> [1F03, 3B9]
      u8"\xe1\xbc\x84\xce\xb9\0"                 // #844: [1F84, 1F8C] ==> [1F04, 3B9]
      u8"\xe1\xbc\x85\xce\xb9\0"                 // #845: [1F85, 1F8D] ==> [1F05, 3B9]
      u8"\xe1\xbc\x86\xce\xb9\0"                 // #846: [1F86, 1F8E] ==> [1F06, 3B9]
      u8"\xe1\xbc\x87\xce\xb9\0"                 // #847: [1F87, 1F8F] ==> [1F07, 3B9]
      u8"\xe1\xbc\xa0\xce\xb9\0"                 // #848: [1F90, 1F98] ==> [1F20, 3B9]
      u8"\xe1\xbc\xa1\xce\xb9\0"                 // #849: [1F91, 1F99] ==> [1F21, 3B9]
      u8"\xe1\xbc\xa2\xce\xb9\0"                 // #850: [1F92, 1F9A] ==> [1F22, 3B9]
      u8"\xe1\xbc\xa3\xce\xb9\0"                 // #851: [1F93, 1F9B] ==> [1F23, 3B9]
      u8"\xe1\xbc\xa4\xce\xb9\0"                 // #852: [1F94, 1F9C] ==> [1F24, 3B9]
      u8"\xe1\xbc\xa5\xce\xb9\0"                 // #853: [1F95, 1F9D] ==> [1F25, 3B9]
      u8"\xe1\xbc\xa6\xce\xb9\0"                 // #854: [1F96, 1F9E] ==> [1F26, 3B9]
      u8"\xe1\xbc\xa7\xce\xb9\0"                 // #855: [1F97, 1F9F] ==> [1F27, 3B9]
      u8"\xe1\xbd\xa0\xce\xb9\0"                 // #856: [1FA0, 1FA8] ==> [1F60, 3B9]
      u8"\xe1\xbd\xa1\xce\xb9\0"                 // #857: [1FA1, 1FA9] ==> [1F61, 3B9]
      u8"\xe1\xbd\xa2\xce\xb9\0"                 // #858: [1FA2, 1FAA] ==> [1F62, 3B9]
      u8"\xe1\xbd\xa3\xce\xb9\0"                 // #859: [1FA3, 1FAB] ==> [1F63, 3B9]
      u8"\xe1\xbd\xa4\xce\xb9\0"                 // #860: [1FA4, 1FAC] ==> [1F64, 3B9]
      u8"\xe1\xbd\xa5\xce\xb9\0"                 // #861: [1FA5, 1FAD] ==> [1F65, 3B9]
      u8"\xe1\xbd\xa6\xce\xb9\0"                 // #862: [1FA6, 1FAE] ==> [1F66, 3B9]
      u8"\xe1\xbd\xa7\xce\xb9\0"                 // #863: [1FA7, 1FAF] ==> [1F67, 3B9]
      u8"\xe1\xbd\xb0\xce\xb9\0"                 // #864: [1FB2] ==> [1F70, 3B9]
      u8"\xce\xb1\xce\xb9\0"                     // #865: [1FB3, 1FBC] ==> [3B1, 3B9]
      u8"\xce\xac\xce\xb9\0"                     // #866: [1FB4] ==> [3AC, 3B9]
      u8"\xe1\xbe\xb6\xce\xb9\0"                 // #867: [1FB7] ==> [1FB6, 3B9]
      u8"\xe1\xbe\xb0\0"                         // #868: [1FB8] ==> [1FB0]
      u8"\xe1\xbe\xb1\0"                         // #869: [1FB9] ==> [1FB1]
      u8"\xe1\xbd\xb0\0"                         // #870: [1FBA] ==> [1F70]
      u8"\40\xcc\x93\0"                          // #871: [1FBD, 1FBF] ==> [20, 313]
      u8"\40\xcd\x82\0"                          // #872: [1FC0] ==> [20, 342]
      u8"\40\xcc\x88\xcd\x82\0"                  // #873: [1FC1] ==> [20, 308, 342]
      u8"\xe1\xbd\xb4\xce\xb9\0"                 // #874: [1FC2] ==> [1F74, 3B9]
      u8"\xce\xb7\xce\xb9\0"                     // #875: [1FC3, 1FCC] ==> [3B7, 3B9]
      u8"\xce\xae\xce\xb9\0"                     // #876: [1FC4] ==> [3AE, 3B9]
      u8"\xe1\xbf\x86\xce\xb9\0"                 // #877: [1FC7] ==> [1FC6, 3B9]
      u8"\xe1\xbd\xb2\0"                         // #878: [1FC8] ==> [1F72]
      u8"\xe1\xbd\xb4\0"                         // #879: [1FCA] ==> [1F74]
      u8"\40\xcc\x93\xcc\x80\0"                  // #880: [1FCD] ==> [20, 313, 300]
      u8"\40\xcc\x93\xcc\x81\0"                  // #881: [1FCE] ==> [20, 313, 301]
      u8"\40\xcc\x93\xcd\x82\0"                  // #882: [1FCF] ==> [20, 313, 342]
      u8"\xce\x90\0"                             // #883: [1FD3] ==> [390]
      u8"\xe1\xbf\x90\0"                         // #884: [1FD8] ==> [1FD0]
      u8"\xe1\xbf\x91\0"                         // #885: [1FD9] ==> [1FD1]
      u8"\xe1\xbd\xb6\0"                         // #886: [1FDA] ==> [1F76]
      u8"\40\xcc\x94\xcc\x80\0"                  // #887: [1FDD] ==> [20, 314, 300]
      u8"\40\xcc\x94\xcc\x81\0"                  // #888: [1FDE] ==> [20, 314, 301]
      u8"\40\xcc\x94\xcd\x82\0"                  // #889: [1FDF] ==> [20, 314, 342]
      u8"\xce\xb0\0"                             // #890: [1FE3] ==> [3B0]
      u8"\xe1\xbf\xa0\0"                         // #891: [1FE8] ==> [1FE0]
      u8"\xe1\xbf\xa1\0"                         // #892: [1FE9] ==> [1FE1]
      u8"\xe1\xbd\xba\0"                         // #893: [1FEA] ==> [1F7A]
      u8"\xe1\xbf\xa5\0"                         // #894: [1FEC] ==> [1FE5]
      u8"\40\xcc\x88\xcc\x80\0"                  // #895: [1FED] ==> [20, 308, 300]
      u8"\x60\0"                                 // #896: [1FEF, FF40] ==> [60]
      u8"\xe1\xbd\xbc\xce\xb9\0"                 // #897: [1FF2] ==> [1F7C, 3B9]
      u8"\xcf\x89\xce\xb9\0"                     // #898: [1FF3, 1FFC] ==> [3C9, 3B9]
      u8"\xcf\x8e\xce\xb9\0"                     // #899: [1FF4] ==> [3CE, 3B9]
      u8"\xe1\xbf\xb6\xce\xb9\0"                 // #900: [1FF7] ==> [1FF6, 3B9]
      u8"\xe1\xbd\xb8\0"                         // #901: [1FF8] ==> [1F78]
      u8"\xe1\xbd\xbc\0"                         // #902: [1FFA] ==> [1F7C]
      u8"\40\xcc\x94\0"                          // #903: [1FFE] ==> [20, 314]
      u8"\xe2\x80\x90\0"                         // #904: [2011] ==> [2010]
      u8"\40\xcc\xb3\0"                          // #905: [2017] ==> [20, 333]
      u8"\xe2\x80\xb2\xe2\x80\xb2\0"             // #906: [2033] ==> [2032, 2032]
      u8"\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\0" // #907: [2034] ==> [2032, 2032, 2032]
      u8"\xe2\x80\xb5\xe2\x80\xb5\0"             // #908: [2036] ==> [2035, 2035]
      u8"\xe2\x80\xb5\xe2\x80\xb5\xe2\x80\xb5\0" // #909: [2037] ==> [2035, 2035, 2035]
      u8"\41\41\0"                               // #910: [203C] ==> [21, 21]
      u8"\40\xcc\x85\0"                          // #911: [203E, FE49, FE4A, FE4B, FE4C] ==> [20, 305]
      u8"\77\77\0"                               // #912: [2047] ==> [3F, 3F]
      u8"\77\41\0"                               // #913: [2048] ==> [3F, 21]
      u8"\41\77\0"                               // #914: [2049] ==> [21, 3F]
      u8"\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\0" // #915: [2057] ==> [2032, 2032, 2032, 2032]
      u8"\60\0" // #916: [2070, 2080, 24EA, FF10, 1CCF0, 1D7CE, 1D7D8, 1D7E2, 1D7EC, 1D7F6, 1FBF0] ==> [30]
      u8"\64\0" // #917: [2074, 2084, 2463, FF14, 1CCF4, 1D7D2, 1D7DC, 1D7E6, 1D7F0, 1D7FA, 1FBF4] ==> [34]
      u8"\65\0" // #918: [2075, 2085, 2464, FF15, 1CCF5, 1D7D3, 1D7DD, 1D7E7, 1D7F1, 1D7FB, 1FBF5] ==> [35]
      u8"\66\0" // #919: [2076, 2086, 2465, FF16, 1CCF6, 1D7D4, 1D7DE, 1D7E8, 1D7F2, 1D7FC, 1FBF6] ==> [36]
      u8"\67\0" // #920: [2077, 2087, 2466, FF17, 1CCF7, 1D7D5, 1D7DF, 1D7E9, 1D7F3, 1D7FD, 1FBF7] ==> [37]
      u8"\70\0" // #921: [2078, 2088, 2467, FF18, 1CCF8, 1D7D6, 1D7E0, 1D7EA, 1D7F4, 1D7FE, 1FBF8] ==> [38]
      u8"\71\0" // #922: [2079, 2089, 2468, FF19, 1CCF9, 1D7D7, 1D7E1, 1D7EB, 1D7F5, 1D7FF, 1FBF9] ==> [39]
      u8"\53\0" // #923: [207A, 208A, FB29, FE62, FF0B] ==> [2B]
      u8"\xe2\x88\x92\0"                                     // #924: [207B, 208B] ==> [2212]
      u8"\75\0"                                              // #925: [207C, 208C, FE66, FF1D] ==> [3D]
      u8"\50\0"                                              // #926: [207D, 208D, FE35, FE59, FF08] ==> [28]
      u8"\51\0"                                              // #927: [207E, 208E, FE36, FE5A, FF09] ==> [29]
      u8"\x72\x73\0"                                         // #928: [20A8] ==> [72, 73]
      u8"\x61\57\x63\0"                                      // #929: [2100] ==> [61, 2F, 63]
      u8"\x61\57\x73\0"                                      // #930: [2101] ==> [61, 2F, 73]
      u8"\xc2\xb0\x63\0"                                     // #931: [2103] ==> [B0, 63]
      u8"\x63\57\x6f\0"                                      // #932: [2105] ==> [63, 2F, 6F]
      u8"\x63\57\x75\0"                                      // #933: [2106] ==> [63, 2F, 75]
      u8"\xc2\xb0\x66\0"                                     // #934: [2109] ==> [B0, 66]
      u8"\x6e\x6f\0"                                         // #935: [2116] ==> [6E, 6F]
      u8"\x73\x6d\0"                                         // #936: [2120] ==> [73, 6D]
      u8"\x74\x65\x6c\0"                                     // #937: [2121] ==> [74, 65, 6C]
      u8"\x74\x6d\0"                                         // #938: [2122] ==> [74, 6D]
      u8"\xe2\x85\x8e\0"                                     // #939: [2132] ==> [214E]
      u8"\xd7\x90\0"                                         // #940: [2135, FB21] ==> [5D0]
      u8"\xd7\x91\0"                                         // #941: [2136] ==> [5D1]
      u8"\xd7\x92\0"                                         // #942: [2137] ==> [5D2]
      u8"\xd7\x93\0"                                         // #943: [2138, FB22] ==> [5D3]
      u8"\x66\x61\x78\0"                                     // #944: [213B] ==> [66, 61, 78]
      u8"\xe2\x88\x91\0"                                     // #945: [2140] ==> [2211]
      u8"\61\xe2\x81\x84\67\0"                               // #946: [2150] ==> [31, 2044, 37]
      u8"\61\xe2\x81\x84\71\0"                               // #947: [2151] ==> [31, 2044, 39]
      u8"\61\xe2\x81\x84\61\60\0"                            // #948: [2152] ==> [31, 2044, 31, 30]
      u8"\61\xe2\x81\x84\63\0"                               // #949: [2153] ==> [31, 2044, 33]
      u8"\62\xe2\x81\x84\63\0"                               // #950: [2154] ==> [32, 2044, 33]
      u8"\61\xe2\x81\x84\65\0"                               // #951: [2155] ==> [31, 2044, 35]
      u8"\62\xe2\x81\x84\65\0"                               // #952: [2156] ==> [32, 2044, 35]
      u8"\63\xe2\x81\x84\65\0"                               // #953: [2157] ==> [33, 2044, 35]
      u8"\64\xe2\x81\x84\65\0"                               // #954: [2158] ==> [34, 2044, 35]
      u8"\61\xe2\x81\x84\66\0"                               // #955: [2159] ==> [31, 2044, 36]
      u8"\65\xe2\x81\x84\66\0"                               // #956: [215A] ==> [35, 2044, 36]
      u8"\61\xe2\x81\x84\70\0"                               // #957: [215B] ==> [31, 2044, 38]
      u8"\63\xe2\x81\x84\70\0"                               // #958: [215C] ==> [33, 2044, 38]
      u8"\65\xe2\x81\x84\70\0"                               // #959: [215D] ==> [35, 2044, 38]
      u8"\67\xe2\x81\x84\70\0"                               // #960: [215E] ==> [37, 2044, 38]
      u8"\61\xe2\x81\x84\0"                                  // #961: [215F] ==> [31, 2044]
      u8"\x69\x69\0"                                         // #962: [2161, 2171] ==> [69, 69]
      u8"\x69\x69\x69\0"                                     // #963: [2162, 2172] ==> [69, 69, 69]
      u8"\x69\x76\0"                                         // #964: [2163, 2173] ==> [69, 76]
      u8"\x76\x69\0"                                         // #965: [2165, 2175] ==> [76, 69]
      u8"\x76\x69\x69\0"                                     // #966: [2166, 2176] ==> [76, 69, 69]
      u8"\x76\x69\x69\x69\0"                                 // #967: [2167, 2177] ==> [76, 69, 69, 69]
      u8"\x69\x78\0"                                         // #968: [2168, 2178] ==> [69, 78]
      u8"\x78\x69\0"                                         // #969: [216A, 217A] ==> [78, 69]
      u8"\x78\x69\x69\0"                                     // #970: [216B, 217B] ==> [78, 69, 69]
      u8"\xe2\x86\x84\0"                                     // #971: [2183] ==> [2184]
      u8"\60\xe2\x81\x84\63\0"                               // #972: [2189] ==> [30, 2044, 33]
      u8"\xe2\x88\xab\xe2\x88\xab\0"                         // #973: [222C] ==> [222B, 222B]
      u8"\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\0"             // #974: [222D] ==> [222B, 222B, 222B]
      u8"\xe2\x88\xae\xe2\x88\xae\0"                         // #975: [222F] ==> [222E, 222E]
      u8"\xe2\x88\xae\xe2\x88\xae\xe2\x88\xae\0"             // #976: [2230] ==> [222E, 222E, 222E]
      u8"\xe3\x80\x88\0"                                     // #977: [2329, FE3F] ==> [3008]
      u8"\xe3\x80\x89\0"                                     // #978: [232A, FE40] ==> [3009]
      u8"\61\60\0"                                           // #979: [2469] ==> [31, 30]
      u8"\61\61\0"                                           // #980: [246A] ==> [31, 31]
      u8"\61\62\0"                                           // #981: [246B] ==> [31, 32]
      u8"\61\63\0"                                           // #982: [246C] ==> [31, 33]
      u8"\61\64\0"                                           // #983: [246D] ==> [31, 34]
      u8"\61\65\0"                                           // #984: [246E] ==> [31, 35]
      u8"\61\66\0"                                           // #985: [246F] ==> [31, 36]
      u8"\61\67\0"                                           // #986: [2470] ==> [31, 37]
      u8"\61\70\0"                                           // #987: [2471] ==> [31, 38]
      u8"\61\71\0"                                           // #988: [2472] ==> [31, 39]
      u8"\62\60\0"                                           // #989: [2473] ==> [32, 30]
      u8"\50\61\51\0"                                        // #990: [2474] ==> [28, 31, 29]
      u8"\50\62\51\0"                                        // #991: [2475] ==> [28, 32, 29]
      u8"\50\63\51\0"                                        // #992: [2476] ==> [28, 33, 29]
      u8"\50\64\51\0"                                        // #993: [2477] ==> [28, 34, 29]
      u8"\50\65\51\0"                                        // #994: [2478] ==> [28, 35, 29]
      u8"\50\66\51\0"                                        // #995: [2479] ==> [28, 36, 29]
      u8"\50\67\51\0"                                        // #996: [247A] ==> [28, 37, 29]
      u8"\50\70\51\0"                                        // #997: [247B] ==> [28, 38, 29]
      u8"\50\71\51\0"                                        // #998: [247C] ==> [28, 39, 29]
      u8"\50\61\60\51\0"                                     // #999: [247D] ==> [28, 31, 30, 29]
      u8"\50\61\61\51\0"                                     // #1000: [247E] ==> [28, 31, 31, 29]
      u8"\50\61\62\51\0"                                     // #1001: [247F] ==> [28, 31, 32, 29]
      u8"\50\61\63\51\0"                                     // #1002: [2480] ==> [28, 31, 33, 29]
      u8"\50\61\64\51\0"                                     // #1003: [2481] ==> [28, 31, 34, 29]
      u8"\50\61\65\51\0"                                     // #1004: [2482] ==> [28, 31, 35, 29]
      u8"\50\61\66\51\0"                                     // #1005: [2483] ==> [28, 31, 36, 29]
      u8"\50\61\67\51\0"                                     // #1006: [2484] ==> [28, 31, 37, 29]
      u8"\50\61\70\51\0"                                     // #1007: [2485] ==> [28, 31, 38, 29]
      u8"\50\61\71\51\0"                                     // #1008: [2486] ==> [28, 31, 39, 29]
      u8"\50\62\60\51\0"                                     // #1009: [2487] ==> [28, 32, 30, 29]
      u8"\50\x61\51\0"                                       // #1010: [249C, 1F110] ==> [28, 61, 29]
      u8"\50\x62\51\0"                                       // #1011: [249D, 1F111] ==> [28, 62, 29]
      u8"\50\x63\51\0"                                       // #1012: [249E, 1F112] ==> [28, 63, 29]
      u8"\50\x64\51\0"                                       // #1013: [249F, 1F113] ==> [28, 64, 29]
      u8"\50\x65\51\0"                                       // #1014: [24A0, 1F114] ==> [28, 65, 29]
      u8"\50\x66\51\0"                                       // #1015: [24A1, 1F115] ==> [28, 66, 29]
      u8"\50\x67\51\0"                                       // #1016: [24A2, 1F116] ==> [28, 67, 29]
      u8"\50\x68\51\0"                                       // #1017: [24A3, 1F117] ==> [28, 68, 29]
      u8"\50\x69\51\0"                                       // #1018: [24A4, 1F118] ==> [28, 69, 29]
      u8"\50\x6a\51\0"                                       // #1019: [24A5, 1F119] ==> [28, 6A, 29]
      u8"\50\x6b\51\0"                                       // #1020: [24A6, 1F11A] ==> [28, 6B, 29]
      u8"\50\x6c\51\0"                                       // #1021: [24A7, 1F11B] ==> [28, 6C, 29]
      u8"\50\x6d\51\0"                                       // #1022: [24A8, 1F11C] ==> [28, 6D, 29]
      u8"\50\x6e\51\0"                                       // #1023: [24A9, 1F11D] ==> [28, 6E, 29]
      u8"\50\x6f\51\0"                                       // #1024: [24AA, 1F11E] ==> [28, 6F, 29]
      u8"\50\x70\51\0"                                       // #1025: [24AB, 1F11F] ==> [28, 70, 29]
      u8"\50\x71\51\0"                                       // #1026: [24AC, 1F120] ==> [28, 71, 29]
      u8"\50\x72\51\0"                                       // #1027: [24AD, 1F121] ==> [28, 72, 29]
      u8"\50\x73\51\0"                                       // #1028: [24AE, 1F122] ==> [28, 73, 29]
      u8"\50\x74\51\0"                                       // #1029: [24AF, 1F123] ==> [28, 74, 29]
      u8"\50\x75\51\0"                                       // #1030: [24B0, 1F124] ==> [28, 75, 29]
      u8"\50\x76\51\0"                                       // #1031: [24B1, 1F125] ==> [28, 76, 29]
      u8"\50\x77\51\0"                                       // #1032: [24B2, 1F126] ==> [28, 77, 29]
      u8"\50\x78\51\0"                                       // #1033: [24B3, 1F127] ==> [28, 78, 29]
      u8"\50\x79\51\0"                                       // #1034: [24B4, 1F128] ==> [28, 79, 29]
      u8"\50\x7a\51\0"                                       // #1035: [24B5, 1F129] ==> [28, 7A, 29]
      u8"\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\0" // #1036: [2A0C] ==> [222B, 222B, 222B, 222B]
      u8"\72\72\75\0"                                        // #1037: [2A74] ==> [3A, 3A, 3D]
      u8"\75\75\0"                                           // #1038: [2A75] ==> [3D, 3D]
      u8"\75\75\75\0"                                        // #1039: [2A76] ==> [3D, 3D, 3D]
      u8"\xe2\xab\x9d\xcc\xb8\0"                             // #1040: [2ADC] ==> [2ADD, 338]
      u8"\xe2\xb0\xb0\0"                                     // #1041: [2C00] ==> [2C30]
      u8"\xe2\xb0\xb1\0"                                     // #1042: [2C01] ==> [2C31]
      u8"\xe2\xb0\xb2\0"                                     // #1043: [2C02] ==> [2C32]
      u8"\xe2\xb0\xb3\0"                                     // #1044: [2C03] ==> [2C33]
      u8"\xe2\xb0\xb4\0"                                     // #1045: [2C04] ==> [2C34]
      u8"\xe2\xb0\xb5\0"                                     // #1046: [2C05] ==> [2C35]
      u8"\xe2\xb0\xb6\0"                                     // #1047: [2C06] ==> [2C36]
      u8"\xe2\xb0\xb7\0"                                     // #1048: [2C07] ==> [2C37]
      u8"\xe2\xb0\xb8\0"                                     // #1049: [2C08] ==> [2C38]
      u8"\xe2\xb0\xb9\0"                                     // #1050: [2C09] ==> [2C39]
      u8"\xe2\xb0\xba\0"                                     // #1051: [2C0A] ==> [2C3A]
      u8"\xe2\xb0\xbb\0"                                     // #1052: [2C0B] ==> [2C3B]
      u8"\xe2\xb0\xbc\0"                                     // #1053: [2C0C] ==> [2C3C]
      u8"\xe2\xb0\xbd\0"                                     // #1054: [2C0D] ==> [2C3D]
      u8"\xe2\xb0\xbe\0"                                     // #1055: [2C0E] ==> [2C3E]
      u8"\xe2\xb0\xbf\0"                                     // #1056: [2C0F] ==> [2C3F]
      u8"\xe2\xb1\x80\0"                                     // #1057: [2C10] ==> [2C40]
      u8"\xe2\xb1\x81\0"                                     // #1058: [2C11] ==> [2C41]
      u8"\xe2\xb1\x82\0"                                     // #1059: [2C12] ==> [2C42]
      u8"\xe2\xb1\x83\0"                                     // #1060: [2C13] ==> [2C43]
      u8"\xe2\xb1\x84\0"                                     // #1061: [2C14] ==> [2C44]
      u8"\xe2\xb1\x85\0"                                     // #1062: [2C15] ==> [2C45]
      u8"\xe2\xb1\x86\0"                                     // #1063: [2C16] ==> [2C46]
      u8"\xe2\xb1\x87\0"                                     // #1064: [2C17] ==> [2C47]
      u8"\xe2\xb1\x88\0"                                     // #1065: [2C18] ==> [2C48]
      u8"\xe2\xb1\x89\0"                                     // #1066: [2C19] ==> [2C49]
      u8"\xe2\xb1\x8a\0"                                     // #1067: [2C1A] ==> [2C4A]
      u8"\xe2\xb1\x8b\0"                                     // #1068: [2C1B] ==> [2C4B]
      u8"\xe2\xb1\x8c\0"                                     // #1069: [2C1C] ==> [2C4C]
      u8"\xe2\xb1\x8d\0"                                     // #1070: [2C1D] ==> [2C4D]
      u8"\xe2\xb1\x8e\0"                                     // #1071: [2C1E] ==> [2C4E]
      u8"\xe2\xb1\x8f\0"                                     // #1072: [2C1F] ==> [2C4F]
      u8"\xe2\xb1\x90\0"                                     // #1073: [2C20] ==> [2C50]
      u8"\xe2\xb1\x91\0"                                     // #1074: [2C21] ==> [2C51]
      u8"\xe2\xb1\x92\0"                                     // #1075: [2C22] ==> [2C52]
      u8"\xe2\xb1\x93\0"                                     // #1076: [2C23] ==> [2C53]
      u8"\xe2\xb1\x94\0"                                     // #1077: [2C24] ==> [2C54]
      u8"\xe2\xb1\x95\0"                                     // #1078: [2C25] ==> [2C55]
      u8"\xe2\xb1\x96\0"                                     // #1079: [2C26] ==> [2C56]
      u8"\xe2\xb1\x97\0"                                     // #1080: [2C27] ==> [2C57]
      u8"\xe2\xb1\x98\0"                                     // #1081: [2C28] ==> [2C58]
      u8"\xe2\xb1\x99\0"                                     // #1082: [2C29] ==> [2C59]
      u8"\xe2\xb1\x9a\0"                                     // #1083: [2C2A] ==> [2C5A]
      u8"\xe2\xb1\x9b\0"                                     // #1084: [2C2B] ==> [2C5B]
      u8"\xe2\xb1\x9c\0"                                     // #1085: [2C2C] ==> [2C5C]
      u8"\xe2\xb1\x9d\0"                                     // #1086: [2C2D] ==> [2C5D]
      u8"\xe2\xb1\x9e\0"                                     // #1087: [2C2E] ==> [2C5E]
      u8"\xe2\xb1\x9f\0"                                     // #1088: [2C2F] ==> [2C5F]
      u8"\xe2\xb1\xa1\0"                                     // #1089: [2C60] ==> [2C61]
      u8"\xc9\xab\0"                                         // #1090: [2C62, AB5E] ==> [26B]
      u8"\xe1\xb5\xbd\0"                                     // #1091: [2C63] ==> [1D7D]
      u8"\xc9\xbd\0"                                         // #1092: [2C64, 107A8] ==> [27D]
      u8"\xe2\xb1\xa8\0"                                     // #1093: [2C67] ==> [2C68]
      u8"\xe2\xb1\xaa\0"                                     // #1094: [2C69] ==> [2C6A]
      u8"\xe2\xb1\xac\0"                                     // #1095: [2C6B] ==> [2C6C]
      u8"\xe2\xb1\xb3\0"                                     // #1096: [2C72] ==> [2C73]
      u8"\xe2\xb1\xb6\0"                                     // #1097: [2C75] ==> [2C76]
      u8"\xc8\xbf\0"                                         // #1098: [2C7E] ==> [23F]
      u8"\xc9\x80\0"                                         // #1099: [2C7F] ==> [240]
      u8"\xe2\xb2\x81\0"                                     // #1100: [2C80] ==> [2C81]
      u8"\xe2\xb2\x83\0"                                     // #1101: [2C82] ==> [2C83]
      u8"\xe2\xb2\x85\0"                                     // #1102: [2C84] ==> [2C85]
      u8"\xe2\xb2\x87\0"                                     // #1103: [2C86] ==> [2C87]
      u8"\xe2\xb2\x89\0"                                     // #1104: [2C88] ==> [2C89]
      u8"\xe2\xb2\x8b\0"                                     // #1105: [2C8A] ==> [2C8B]
      u8"\xe2\xb2\x8d\0"                                     // #1106: [2C8C] ==> [2C8D]
      u8"\xe2\xb2\x8f\0"                                     // #1107: [2C8E] ==> [2C8F]
      u8"\xe2\xb2\x91\0"                                     // #1108: [2C90] ==> [2C91]
      u8"\xe2\xb2\x93\0"                                     // #1109: [2C92] ==> [2C93]
      u8"\xe2\xb2\x95\0"                                     // #1110: [2C94] ==> [2C95]
      u8"\xe2\xb2\x97\0"                                     // #1111: [2C96] ==> [2C97]
      u8"\xe2\xb2\x99\0"                                     // #1112: [2C98] ==> [2C99]
      u8"\xe2\xb2\x9b\0"                                     // #1113: [2C9A] ==> [2C9B]
      u8"\xe2\xb2\x9d\0"                                     // #1114: [2C9C] ==> [2C9D]
      u8"\xe2\xb2\x9f\0"                                     // #1115: [2C9E] ==> [2C9F]
      u8"\xe2\xb2\xa1\0"                                     // #1116: [2CA0] ==> [2CA1]
      u8"\xe2\xb2\xa3\0"                                     // #1117: [2CA2] ==> [2CA3]
      u8"\xe2\xb2\xa5\0"                                     // #1118: [2CA4] ==> [2CA5]
      u8"\xe2\xb2\xa7\0"                                     // #1119: [2CA6] ==> [2CA7]
      u8"\xe2\xb2\xa9\0"                                     // #1120: [2CA8] ==> [2CA9]
      u8"\xe2\xb2\xab\0"                                     // #1121: [2CAA] ==> [2CAB]
      u8"\xe2\xb2\xad\0"                                     // #1122: [2CAC] ==> [2CAD]
      u8"\xe2\xb2\xaf\0"                                     // #1123: [2CAE] ==> [2CAF]
      u8"\xe2\xb2\xb1\0"                                     // #1124: [2CB0] ==> [2CB1]
      u8"\xe2\xb2\xb3\0"                                     // #1125: [2CB2] ==> [2CB3]
      u8"\xe2\xb2\xb5\0"                                     // #1126: [2CB4] ==> [2CB5]
      u8"\xe2\xb2\xb7\0"                                     // #1127: [2CB6] ==> [2CB7]
      u8"\xe2\xb2\xb9\0"                                     // #1128: [2CB8] ==> [2CB9]
      u8"\xe2\xb2\xbb\0"                                     // #1129: [2CBA] ==> [2CBB]
      u8"\xe2\xb2\xbd\0"                                     // #1130: [2CBC] ==> [2CBD]
      u8"\xe2\xb2\xbf\0"                                     // #1131: [2CBE] ==> [2CBF]
      u8"\xe2\xb3\x81\0"                                     // #1132: [2CC0] ==> [2CC1]
      u8"\xe2\xb3\x83\0"                                     // #1133: [2CC2] ==> [2CC3]
      u8"\xe2\xb3\x85\0"                                     // #1134: [2CC4] ==> [2CC5]
      u8"\xe2\xb3\x87\0"                                     // #1135: [2CC6] ==> [2CC7]
      u8"\xe2\xb3\x89\0"                                     // #1136: [2CC8] ==> [2CC9]
      u8"\xe2\xb3\x8b\0"                                     // #1137: [2CCA] ==> [2CCB]
      u8"\xe2\xb3\x8d\0"                                     // #1138: [2CCC] ==> [2CCD]
      u8"\xe2\xb3\x8f\0"                                     // #1139: [2CCE] ==> [2CCF]
      u8"\xe2\xb3\x91\0"                                     // #1140: [2CD0] ==> [2CD1]
      u8"\xe2\xb3\x93\0"                                     // #1141: [2CD2] ==> [2CD3]
      u8"\xe2\xb3\x95\0"                                     // #1142: [2CD4] ==> [2CD5]
      u8"\xe2\xb3\x97\0"                                     // #1143: [2CD6] ==> [2CD7]
      u8"\xe2\xb3\x99\0"                                     // #1144: [2CD8] ==> [2CD9]
      u8"\xe2\xb3\x9b\0"                                     // #1145: [2CDA] ==> [2CDB]
      u8"\xe2\xb3\x9d\0"                                     // #1146: [2CDC] ==> [2CDD]
      u8"\xe2\xb3\x9f\0"                                     // #1147: [2CDE] ==> [2CDF]
      u8"\xe2\xb3\xa1\0"                                     // #1148: [2CE0] ==> [2CE1]
      u8"\xe2\xb3\xa3\0"                                     // #1149: [2CE2] ==> [2CE3]
      u8"\xe2\xb3\xac\0"                                     // #1150: [2CEB] ==> [2CEC]
      u8"\xe2\xb3\xae\0"                                     // #1151: [2CED] ==> [2CEE]
      u8"\xe2\xb3\xb3\0"                                     // #1152: [2CF2] ==> [2CF3]
      u8"\xe2\xb5\xa1\0"                                     // #1153: [2D6F] ==> [2D61]
      u8"\xe6\xaf\x8d\0"                                     // #1154: [2E9F] ==> [6BCD]
      u8"\xe9\xbe\x9f\0"                                     // #1155: [2EF3] ==> [9F9F]
      u8"\xe4\xb8\x80\0"                                     // #1156: [2F00, 3192, 3280, 1F229] ==> [4E00]
      u8"\xe4\xb8\xa8\0"                                     // #1157: [2F01] ==> [4E28]
      u8"\xe4\xb8\xb6\0"                                     // #1158: [2F02] ==> [4E36]
      u8"\xe4\xb8\xbf\0"                                     // #1159: [2F03] ==> [4E3F]
      u8"\xe4\xb9\x99\0"                                     // #1160: [2F04, 319A] ==> [4E59]
      u8"\xe4\xba\x85\0"                                     // #1161: [2F05] ==> [4E85]
      u8"\xe4\xba\x8c\0"                                     // #1162: [2F06, 3193, 3281, 1F214] ==> [4E8C]
      u8"\xe4\xba\xa0\0"                                     // #1163: [2F07] ==> [4EA0]
      u8"\xe4\xba\xba\0"                                     // #1164: [2F08, 319F] ==> [4EBA]
      u8"\xe5\x84\xbf\0"                                     // #1165: [2F09] ==> [513F]
      u8"\xe5\x85\xa5\0"                                     // #1166: [2F0A] ==> [5165]
      u8"\xe5\x85\xab\0"                                     // #1167: [2F0B, 3287] ==> [516B]
      u8"\xe5\x86\x82\0"                                     // #1168: [2F0C] ==> [5182]
      u8"\xe5\x86\x96\0"                                     // #1169: [2F0D] ==> [5196]
      u8"\xe5\x86\xab\0"                                     // #1170: [2F0E] ==> [51AB]
      u8"\xe5\x87\xa0\0"                                     // #1171: [2F0F] ==> [51E0]
      u8"\xe5\x87\xb5\0"                                     // #1172: [2F10, 2F81D] ==> [51F5]
      u8"\xe5\x88\x80\0"                                     // #1173: [2F11] ==> [5200]
      u8"\xe5\x8a\x9b\0"                                     // #1174: [2F12, F98A] ==> [529B]
      u8"\xe5\x8b\xb9\0"                                     // #1175: [2F13] ==> [52F9]
      u8"\xe5\x8c\x95\0"                                     // #1176: [2F14] ==> [5315]
      u8"\xe5\x8c\x9a\0"                                     // #1177: [2F15] ==> [531A]
      u8"\xe5\x8c\xb8\0"                                     // #1178: [2F16] ==> [5338]
      u8"\xe5\x8d\x81\0"                                     // #1179: [2F17, 3038, 3289] ==> [5341]
      u8"\xe5\x8d\x9c\0"                                     // #1180: [2F18] ==> [535C]
      u8"\xe5\x8d\xa9\0"                                     // #1181: [2F19] ==> [5369]
      u8"\xe5\x8e\x82\0"                                     // #1182: [2F1A] ==> [5382]
      u8"\xe5\x8e\xb6\0"                                     // #1183: [2F1B] ==> [53B6]
      u8"\xe5\x8f\x88\0"                                     // #1184: [2F1C] ==> [53C8]
      u8"\xe5\x8f\xa3\0"                                     // #1185: [2F1D] ==> [53E3]
      u8"\xe5\x9b\x97\0"                                     // #1186: [2F1E] ==> [56D7]
      u8"\xe5\x9c\x9f\0"                                     // #1187: [2F1F, 328F] ==> [571F]
      u8"\xe5\xa3\xab\0"                                     // #1188: [2F20] ==> [58EB]
      u8"\xe5\xa4\x82\0"                                     // #1189: [2F21] ==> [5902]
      u8"\xe5\xa4\x8a\0"                                     // #1190: [2F22] ==> [590A]
      u8"\xe5\xa4\x95\0"                                     // #1191: [2F23] ==> [5915]
      u8"\xe5\xa4\xa7\0"                                     // #1192: [2F24] ==> [5927]
      u8"\xe5\xa5\xb3\0"                                     // #1193: [2F25, 329B, F981] ==> [5973]
      u8"\xe5\xad\x90\0"                                     // #1194: [2F26] ==> [5B50]
      u8"\xe5\xae\x80\0"                                     // #1195: [2F27] ==> [5B80]
      u8"\xe5\xaf\xb8\0"                                     // #1196: [2F28] ==> [5BF8]
      u8"\xe5\xb0\x8f\0"                                     // #1197: [2F29] ==> [5C0F]
      u8"\xe5\xb0\xa2\0"                                     // #1198: [2F2A, 2F875] ==> [5C22]
      u8"\xe5\xb0\xb8\0"                                     // #1199: [2F2B] ==> [5C38]
      u8"\xe5\xb1\xae\0"                                     // #1200: [2F2C, FA3C, 2F878] ==> [5C6E]
      u8"\xe5\xb1\xb1\0"                                     // #1201: [2F2D] ==> [5C71]
      u8"\xe5\xb7\x9b\0"                                     // #1202: [2F2E] ==> [5DDB]
      u8"\xe5\xb7\xa5\0"                                     // #1203: [2F2F] ==> [5DE5]
      u8"\xe5\xb7\xb1\0"                                     // #1204: [2F30] ==> [5DF1]
      u8"\xe5\xb7\xbe\0"                                     // #1205: [2F31] ==> [5DFE]
      u8"\xe5\xb9\xb2\0"                                     // #1206: [2F32] ==> [5E72]
      u8"\xe5\xb9\xba\0"                                     // #1207: [2F33] ==> [5E7A]
      u8"\xe5\xb9\xbf\0"                                     // #1208: [2F34] ==> [5E7F]
      u8"\xe5\xbb\xb4\0"                                     // #1209: [2F35] ==> [5EF4]
      u8"\xe5\xbb\xbe\0"                                     // #1210: [2F36, 2F890] ==> [5EFE]
      u8"\xe5\xbc\x8b\0"                                     // #1211: [2F37] ==> [5F0B]
      u8"\xe5\xbc\x93\0"                                     // #1212: [2F38] ==> [5F13]
      u8"\xe5\xbd\x90\0"                                     // #1213: [2F39] ==> [5F50]
      u8"\xe5\xbd\xa1\0"                                     // #1214: [2F3A] ==> [5F61]
      u8"\xe5\xbd\xb3\0"                                     // #1215: [2F3B] ==> [5F73]
      u8"\xe5\xbf\x83\0"                                     // #1216: [2F3C] ==> [5FC3]
      u8"\xe6\x88\x88\0"                                     // #1217: [2F3D] ==> [6208]
      u8"\xe6\x88\xb6\0"                                     // #1218: [2F3E] ==> [6236]
      u8"\xe6\x89\x8b\0"                                     // #1219: [2F3F, 1F210] ==> [624B]
      u8"\xe6\x94\xaf\0"                                     // #1220: [2F40] ==> [652F]
      u8"\xe6\x94\xb4\0"                                     // #1221: [2F41] ==> [6534]
      u8"\xe6\x96\x87\0"                                     // #1222: [2F42, 3246] ==> [6587]
      u8"\xe6\x96\x97\0"                                     // #1223: [2F43] ==> [6597]
      u8"\xe6\x96\xa4\0"                                     // #1224: [2F44] ==> [65A4]
      u8"\xe6\x96\xb9\0"                                     // #1225: [2F45] ==> [65B9]
      u8"\xe6\x97\xa0\0"                                     // #1226: [2F46] ==> [65E0]
      u8"\xe6\x97\xa5\0"                                     // #1227: [2F47, 3290] ==> [65E5]
      u8"\xe6\x9b\xb0\0"                                     // #1228: [2F48] ==> [66F0]
      u8"\xe6\x9c\x88\0"                                     // #1229: [2F49, 328A, 1F237] ==> [6708]
      u8"\xe6\x9c\xa8\0"                                     // #1230: [2F4A, 328D] ==> [6728]
      u8"\xe6\xac\xa0\0"                                     // #1231: [2F4B] ==> [6B20]
      u8"\xe6\xad\xa2\0"                                     // #1232: [2F4C] ==> [6B62]
      u8"\xe6\xad\xb9\0"                                     // #1233: [2F4D, FA95] ==> [6B79]
      u8"\xe6\xae\xb3\0"                                     // #1234: [2F4E] ==> [6BB3]
      u8"\xe6\xaf\x8b\0"                                     // #1235: [2F4F] ==> [6BCB]
      u8"\xe6\xaf\x94\0"                                     // #1236: [2F50] ==> [6BD4]
      u8"\xe6\xaf\x9b\0"                                     // #1237: [2F51] ==> [6BDB]
      u8"\xe6\xb0\x8f\0"                                     // #1238: [2F52] ==> [6C0F]
      u8"\xe6\xb0\x94\0"                                     // #1239: [2F53] ==> [6C14]
      u8"\xe6\xb0\xb4\0"                                     // #1240: [2F54, 328C] ==> [6C34]
      u8"\xe7\x81\xab\0"                                     // #1241: [2F55, 328B] ==> [706B]
      u8"\xe7\x88\xaa\0"                                     // #1242: [2F56] ==> [722A]
      u8"\xe7\x88\xb6\0"                                     // #1243: [2F57] ==> [7236]
      u8"\xe7\x88\xbb\0"                                     // #1244: [2F58] ==> [723B]
      u8"\xe7\x88\xbf\0"                                     // #1245: [2F59] ==> [723F]
      u8"\xe7\x89\x87\0"                                     // #1246: [2F5A] ==> [7247]
      u8"\xe7\x89\x99\0"                                     // #1247: [2F5B] ==> [7259]
      u8"\xe7\x89\x9b\0"                                     // #1248: [2F5C] ==> [725B]
      u8"\xe7\x8a\xac\0"                                     // #1249: [2F5D] ==> [72AC]
      u8"\xe7\x8e\x84\0"                                     // #1250: [2F5E] ==> [7384]
      u8"\xe7\x8e\x89\0"                                     // #1251: [2F5F] ==> [7389]
      u8"\xe7\x93\x9c\0"                                     // #1252: [2F60] ==> [74DC]
      u8"\xe7\x93\xa6\0"                                     // #1253: [2F61] ==> [74E6]
      u8"\xe7\x94\x98\0"                                     // #1254: [2F62] ==> [7518]
      u8"\xe7\x94\x9f\0"                                     // #1255: [2F63, 1F222] ==> [751F]
      u8"\xe7\x94\xa8\0"                                     // #1256: [2F64] ==> [7528]
      u8"\xe7\x94\xb0\0"                                     // #1257: [2F65] ==> [7530]
      u8"\xe7\x96\x8b\0"                                     // #1258: [2F66] ==> [758B]
      u8"\xe7\x96\x92\0"                                     // #1259: [2F67] ==> [7592]
      u8"\xe7\x99\xb6\0"                                     // #1260: [2F68] ==> [7676]
      u8"\xe7\x99\xbd\0"                                     // #1261: [2F69] ==> [767D]
      u8"\xe7\x9a\xae\0"                                     // #1262: [2F6A] ==> [76AE]
      u8"\xe7\x9a\xbf\0"                                     // #1263: [2F6B] ==> [76BF]
      u8"\xe7\x9b\xae\0"                                     // #1264: [2F6C] ==> [76EE]
      u8"\xe7\x9f\x9b\0"                                     // #1265: [2F6D] ==> [77DB]
      u8"\xe7\x9f\xa2\0"                                     // #1266: [2F6E] ==> [77E2]
      u8"\xe7\x9f\xb3\0"                                     // #1267: [2F6F] ==> [77F3]
      u8"\xe7\xa4\xba\0"                                     // #1268: [2F70] ==> [793A]
      u8"\xe7\xa6\xb8\0"                                     // #1269: [2F71] ==> [79B8]
      u8"\xe7\xa6\xbe\0"                                     // #1270: [2F72] ==> [79BE]
      u8"\xe7\xa9\xb4\0"                                     // #1271: [2F73] ==> [7A74]
      u8"\xe7\xab\x8b\0"                                     // #1272: [2F74, F9F7] ==> [7ACB]
      u8"\xe7\xab\xb9\0"                                     // #1273: [2F75] ==> [7AF9]
      u8"\xe7\xb1\xb3\0"                                     // #1274: [2F76] ==> [7C73]
      u8"\xe7\xb3\xb8\0"                                     // #1275: [2F77] ==> [7CF8]
      u8"\xe7\xbc\xb6\0"                                     // #1276: [2F78] ==> [7F36]
      u8"\xe7\xbd\x91\0"                                     // #1277: [2F79] ==> [7F51]
      u8"\xe7\xbe\x8a\0"                                     // #1278: [2F7A] ==> [7F8A]
      u8"\xe7\xbe\xbd\0"                                     // #1279: [2F7B, FA1E] ==> [7FBD]
      u8"\xe8\x80\x81\0"                                     // #1280: [2F7C, F934] ==> [8001]
      u8"\xe8\x80\x8c\0"                                     // #1281: [2F7D] ==> [800C]
      u8"\xe8\x80\x92\0"                                     // #1282: [2F7E] ==> [8012]
      u8"\xe8\x80\xb3\0"                                     // #1283: [2F7F] ==> [8033]
      u8"\xe8\x81\xbf\0"                                     // #1284: [2F80] ==> [807F]
      u8"\xe8\x82\x89\0"                                     // #1285: [2F81] ==> [8089]
      u8"\xe8\x87\xa3\0"                                     // #1286: [2F82] ==> [81E3]
      u8"\xe8\x87\xaa\0"                                     // #1287: [2F83] ==> [81EA]
      u8"\xe8\x87\xb3\0"                                     // #1288: [2F84] ==> [81F3]
      u8"\xe8\x87\xbc\0"                                     // #1289: [2F85] ==> [81FC]
      u8"\xe8\x88\x8c\0"                                     // #1290: [2F86] ==> [820C]
      u8"\xe8\x88\x9b\0"                                     // #1291: [2F87] ==> [821B]
      u8"\xe8\x88\x9f\0"                                     // #1292: [2F88] ==> [821F]
      u8"\xe8\x89\xae\0"                                     // #1293: [2F89] ==> [826E]
      u8"\xe8\x89\xb2\0"                                     // #1294: [2F8A] ==> [8272]
      u8"\xe8\x89\xb8\0"                                     // #1295: [2F8B] ==> [8278]
      u8"\xe8\x99\x8d\0"                                     // #1296: [2F8C] ==> [864D]
      u8"\xe8\x99\xab\0"                                     // #1297: [2F8D] ==> [866B]
      u8"\xe8\xa1\x80\0"                                     // #1298: [2F8E] ==> [8840]
      u8"\xe8\xa1\x8c\0"                                     // #1299: [2F8F, FA08] ==> [884C]
      u8"\xe8\xa1\xa3\0"                                     // #1300: [2F90, 2F9C4] ==> [8863]
      u8"\xe8\xa5\xbe\0"                                     // #1301: [2F91] ==> [897E]
      u8"\xe8\xa6\x8b\0"                                     // #1302: [2F92, FA0A] ==> [898B]
      u8"\xe8\xa7\x92\0"                                     // #1303: [2F93] ==> [89D2]
      u8"\xe8\xa8\x80\0"                                     // #1304: [2F94] ==> [8A00]
      u8"\xe8\xb0\xb7\0"                                     // #1305: [2F95] ==> [8C37]
      u8"\xe8\xb1\x86\0"                                     // #1306: [2F96] ==> [8C46]
      u8"\xe8\xb1\x95\0"                                     // #1307: [2F97, 2F9D2] ==> [8C55]
      u8"\xe8\xb1\xb8\0"                                     // #1308: [2F98] ==> [8C78]
      u8"\xe8\xb2\x9d\0"                                     // #1309: [2F99] ==> [8C9D]
      u8"\xe8\xb5\xa4\0"                                     // #1310: [2F9A] ==> [8D64]
      u8"\xe8\xb5\xb0\0"                                     // #1311: [2F9B, 1F230] ==> [8D70]
      u8"\xe8\xb6\xb3\0"                                     // #1312: [2F9C] ==> [8DB3]
      u8"\xe8\xba\xab\0"                                     // #1313: [2F9D] ==> [8EAB]
      u8"\xe8\xbb\x8a\0"                                     // #1314: [2F9E, F902] ==> [8ECA]
      u8"\xe8\xbe\x9b\0"                                     // #1315: [2F9F] ==> [8F9B]
      u8"\xe8\xbe\xb0\0"                                     // #1316: [2FA0, F971] ==> [8FB0]
      u8"\xe8\xbe\xb5\0"                                     // #1317: [2FA1] ==> [8FB5]
      u8"\xe9\x82\x91\0"                                     // #1318: [2FA2] ==> [9091]
      u8"\xe9\x85\x89\0"                                     // #1319: [2FA3] ==> [9149]
      u8"\xe9\x87\x86\0"                                     // #1320: [2FA4] ==> [91C6]
      u8"\xe9\x87\x8c\0"                                     // #1321: [2FA5, F9E9] ==> [91CC]
      u8"\xe9\x87\x91\0"                                     // #1322: [2FA6, 328E, F90A] ==> [91D1]
      u8"\xe9\x95\xb7\0"                                     // #1323: [2FA7] ==> [9577]
      u8"\xe9\x96\x80\0"                                     // #1324: [2FA8] ==> [9580]
      u8"\xe9\x98\x9c\0"                                     // #1325: [2FA9] ==> [961C]
      u8"\xe9\x9a\xb6\0"                                     // #1326: [2FAA] ==> [96B6]
      u8"\xe9\x9a\xb9\0"                                     // #1327: [2FAB] ==> [96B9]
      u8"\xe9\x9b\xa8\0"                                     // #1328: [2FAC] ==> [96E8]
      u8"\xe9\x9d\x91\0"                                     // #1329: [2FAD] ==> [9751]
      u8"\xe9\x9d\x9e\0"                                     // #1330: [2FAE] ==> [975E]
      u8"\xe9\x9d\xa2\0"                                     // #1331: [2FAF] ==> [9762]
      u8"\xe9\x9d\xa9\0"                                     // #1332: [2FB0] ==> [9769]
      u8"\xe9\x9f\x8b\0"                                     // #1333: [2FB1] ==> [97CB]
      u8"\xe9\x9f\xad\0"                                     // #1334: [2FB2] ==> [97ED]
      u8"\xe9\x9f\xb3\0"                                     // #1335: [2FB3] ==> [97F3]
      u8"\xe9\xa0\x81\0"                                     // #1336: [2FB4] ==> [9801]
      u8"\xe9\xa2\xa8\0"                                     // #1337: [2FB5] ==> [98A8]
      u8"\xe9\xa3\x9b\0"                                     // #1338: [2FB6] ==> [98DB]
      u8"\xe9\xa3\x9f\0"                                     // #1339: [2FB7] ==> [98DF]
      u8"\xe9\xa6\x96\0"                                     // #1340: [2FB8] ==> [9996]
      u8"\xe9\xa6\x99\0"                                     // #1341: [2FB9] ==> [9999]
      u8"\xe9\xa6\xac\0"                                     // #1342: [2FBA] ==> [99AC]
      u8"\xe9\xaa\xa8\0"                                     // #1343: [2FBB] ==> [9AA8]
      u8"\xe9\xab\x98\0"                                     // #1344: [2FBC] ==> [9AD8]
      u8"\xe9\xab\x9f\0"                                     // #1345: [2FBD] ==> [9ADF]
      u8"\xe9\xac\xa5\0"                                     // #1346: [2FBE] ==> [9B25]
      u8"\xe9\xac\xaf\0"                                     // #1347: [2FBF] ==> [9B2F]
      u8"\xe9\xac\xb2\0"                                     // #1348: [2FC0] ==> [9B32]
      u8"\xe9\xac\xbc\0"                                     // #1349: [2FC1] ==> [9B3C]
      u8"\xe9\xad\x9a\0"                                     // #1350: [2FC2] ==> [9B5A]
      u8"\xe9\xb3\xa5\0"                                     // #1351: [2FC3] ==> [9CE5]
      u8"\xe9\xb9\xb5\0"                                     // #1352: [2FC4] ==> [9E75]
      u8"\xe9\xb9\xbf\0"                                     // #1353: [2FC5, F940] ==> [9E7F]
      u8"\xe9\xba\xa5\0"                                     // #1354: [2FC6] ==> [9EA5]
      u8"\xe9\xba\xbb\0"                                     // #1355: [2FC7, 2FA15] ==> [9EBB]
      u8"\xe9\xbb\x83\0"                                     // #1356: [2FC8] ==> [9EC3]
      u8"\xe9\xbb\x8d\0"                                     // #1357: [2FC9] ==> [9ECD]
      u8"\xe9\xbb\x91\0"                                     // #1358: [2FCA] ==> [9ED1]
      u8"\xe9\xbb\xb9\0"                                     // #1359: [2FCB, 2FA17] ==> [9EF9]
      u8"\xe9\xbb\xbd\0"                                     // #1360: [2FCC] ==> [9EFD]
      u8"\xe9\xbc\x8e\0"                                     // #1361: [2FCD] ==> [9F0E]
      u8"\xe9\xbc\x93\0"                                     // #1362: [2FCE] ==> [9F13]
      u8"\xe9\xbc\xa0\0"                                     // #1363: [2FCF] ==> [9F20]
      u8"\xe9\xbc\xbb\0"                                     // #1364: [2FD0, 2FA1C] ==> [9F3B]
      u8"\xe9\xbd\x8a\0"                                     // #1365: [2FD1] ==> [9F4A]
      u8"\xe9\xbd\x92\0"                                     // #1366: [2FD2] ==> [9F52]
      u8"\xe9\xbe\x8d\0"                                     // #1367: [2FD3, F9C4] ==> [9F8D]
      u8"\xe9\xbe\x9c\0"                                     // #1368: [2FD4, F907, F908, FACE] ==> [9F9C]
      u8"\xe9\xbe\xa0\0"                                     // #1369: [2FD5] ==> [9FA0]
      u8"\56\0"                                              // #1370: [3002, FF0E, FF61] ==> [2E]
      u8"\xe3\x80\x92\0"                                     // #1371: [3036] ==> [3012]
      u8"\xe5\x8d\x84\0"                                     // #1372: [3039] ==> [5344]
      u8"\xe5\x8d\x85\0"                                     // #1373: [303A] ==> [5345]
      u8"\40\xe3\x82\x99\0"                                  // #1374: [309B] ==> [20, 3099]
      u8"\40\xe3\x82\x9a\0"                                  // #1375: [309C] ==> [20, 309A]
      u8"\xe3\x82\x88\xe3\x82\x8a\0"                         // #1376: [309F] ==> [3088, 308A]
      u8"\xe3\x82\xb3\xe3\x83\x88\0"                         // #1377: [30FF] ==> [30B3, 30C8]
      u8"\xe1\x84\x80\0"                                     // #1378: [3131, 3260, FFA1] ==> [1100]
      u8"\xe1\x84\x81\0"                                     // #1379: [3132, FFA2] ==> [1101]
      u8"\xe1\x86\xaa\0"                                     // #1380: [3133, FFA3] ==> [11AA]
      u8"\xe1\x84\x82\0"                                     // #1381: [3134, 3261, FFA4] ==> [1102]
      u8"\xe1\x86\xac\0"                                     // #1382: [3135, FFA5] ==> [11AC]
      u8"\xe1\x86\xad\0"                                     // #1383: [3136, FFA6] ==> [11AD]
      u8"\xe1\x84\x83\0"                                     // #1384: [3137, 3262, FFA7] ==> [1103]
      u8"\xe1\x84\x84\0"                                     // #1385: [3138, FFA8] ==> [1104]
      u8"\xe1\x84\x85\0"                                     // #1386: [3139, 3263, FFA9] ==> [1105]
      u8"\xe1\x86\xb0\0"                                     // #1387: [313A, FFAA] ==> [11B0]
      u8"\xe1\x86\xb1\0"                                     // #1388: [313B, FFAB] ==> [11B1]
      u8"\xe1\x86\xb2\0"                                     // #1389: [313C, FFAC] ==> [11B2]
      u8"\xe1\x86\xb3\0"                                     // #1390: [313D, FFAD] ==> [11B3]
      u8"\xe1\x86\xb4\0"                                     // #1391: [313E, FFAE] ==> [11B4]
      u8"\xe1\x86\xb5\0"                                     // #1392: [313F, FFAF] ==> [11B5]
      u8"\xe1\x84\x9a\0"                                     // #1393: [3140, FFB0] ==> [111A]
      u8"\xe1\x84\x86\0"                                     // #1394: [3141, 3264, FFB1] ==> [1106]
      u8"\xe1\x84\x87\0"                                     // #1395: [3142, 3265, FFB2] ==> [1107]
      u8"\xe1\x84\x88\0"                                     // #1396: [3143, FFB3] ==> [1108]
      u8"\xe1\x84\xa1\0"                                     // #1397: [3144, FFB4] ==> [1121]
      u8"\xe1\x84\x89\0"                                     // #1398: [3145, 3266, FFB5] ==> [1109]
      u8"\xe1\x84\x8a\0"                                     // #1399: [3146, FFB6] ==> [110A]
      u8"\xe1\x84\x8b\0"                                     // #1400: [3147, 3267, FFB7] ==> [110B]
      u8"\xe1\x84\x8c\0"                                     // #1401: [3148, 3268, FFB8] ==> [110C]
      u8"\xe1\x84\x8d\0"                                     // #1402: [3149, FFB9] ==> [110D]
      u8"\xe1\x84\x8e\0"                                     // #1403: [314A, 3269, FFBA] ==> [110E]
      u8"\xe1\x84\x8f\0"                                     // #1404: [314B, 326A, FFBB] ==> [110F]
      u8"\xe1\x84\x90\0"                                     // #1405: [314C, 326B, FFBC] ==> [1110]
      u8"\xe1\x84\x91\0"                                     // #1406: [314D, 326C, FFBD] ==> [1111]
      u8"\xe1\x84\x92\0"                                     // #1407: [314E, 326D, FFBE] ==> [1112]
      u8"\xe1\x85\xa1\0"                                     // #1408: [314F, FFC2] ==> [1161]
      u8"\xe1\x85\xa2\0"                                     // #1409: [3150, FFC3] ==> [1162]
      u8"\xe1\x85\xa3\0"                                     // #1410: [3151, FFC4] ==> [1163]
      u8"\xe1\x85\xa4\0"                                     // #1411: [3152, FFC5] ==> [1164]
      u8"\xe1\x85\xa5\0"                                     // #1412: [3153, FFC6] ==> [1165]
      u8"\xe1\x85\xa6\0"                                     // #1413: [3154, FFC7] ==> [1166]
      u8"\xe1\x85\xa7\0"                                     // #1414: [3155, FFCA] ==> [1167]
      u8"\xe1\x85\xa8\0"                                     // #1415: [3156, FFCB] ==> [1168]
      u8"\xe1\x85\xa9\0"                                     // #1416: [3157, FFCC] ==> [1169]
      u8"\xe1\x85\xaa\0"                                     // #1417: [3158, FFCD] ==> [116A]
      u8"\xe1\x85\xab\0"                                     // #1418: [3159, FFCE] ==> [116B]
      u8"\xe1\x85\xac\0"                                     // #1419: [315A, FFCF] ==> [116C]
      u8"\xe1\x85\xad\0"                                     // #1420: [315B, FFD2] ==> [116D]
      u8"\xe1\x85\xae\0"                                     // #1421: [315C, FFD3] ==> [116E]
      u8"\xe1\x85\xaf\0"                                     // #1422: [315D, FFD4] ==> [116F]
      u8"\xe1\x85\xb0\0"                                     // #1423: [315E, FFD5] ==> [1170]
      u8"\xe1\x85\xb1\0"                                     // #1424: [315F, FFD6] ==> [1171]
      u8"\xe1\x85\xb2\0"                                     // #1425: [3160, FFD7] ==> [1172]
      u8"\xe1\x85\xb3\0"                                     // #1426: [3161, FFDA] ==> [1173]
      u8"\xe1\x85\xb4\0"                                     // #1427: [3162, FFDB] ==> [1174]
      u8"\xe1\x85\xb5\0"                                     // #1428: [3163, FFDC] ==> [1175]
      u8"\xe1\x84\x94\0"                                     // #1429: [3165] ==> [1114]
      u8"\xe1\x84\x95\0"                                     // #1430: [3166] ==> [1115]
      u8"\xe1\x87\x87\0"                                     // #1431: [3167] ==> [11C7]
      u8"\xe1\x87\x88\0"                                     // #1432: [3168] ==> [11C8]
      u8"\xe1\x87\x8c\0"                                     // #1433: [3169] ==> [11CC]
      u8"\xe1\x87\x8e\0"                                     // #1434: [316A] ==> [11CE]
      u8"\xe1\x87\x93\0"                                     // #1435: [316B] ==> [11D3]
      u8"\xe1\x87\x97\0"                                     // #1436: [316C] ==> [11D7]
      u8"\xe1\x87\x99\0"                                     // #1437: [316D] ==> [11D9]
      u8"\xe1\x84\x9c\0"                                     // #1438: [316E] ==> [111C]
      u8"\xe1\x87\x9d\0"                                     // #1439: [316F] ==> [11DD]
      u8"\xe1\x87\x9f\0"                                     // #1440: [3170] ==> [11DF]
      u8"\xe1\x84\x9d\0"                                     // #1441: [3171] ==> [111D]
      u8"\xe1\x84\x9e\0"                                     // #1442: [3172] ==> [111E]
      u8"\xe1\x84\xa0\0"                                     // #1443: [3173] ==> [1120]
      u8"\xe1\x84\xa2\0"                                     // #1444: [3174] ==> [1122]
      u8"\xe1\x84\xa3\0"                                     // #1445: [3175] ==> [1123]
      u8"\xe1\x84\xa7\0"                                     // #1446: [3176] ==> [1127]
      u8"\xe1\x84\xa9\0"                                     // #1447: [3177] ==> [1129]
      u8"\xe1\x84\xab\0"                                     // #1448: [3178] ==> [112B]
      u8"\xe1\x84\xac\0"                                     // #1449: [3179] ==> [112C]
      u8"\xe1\x84\xad\0"                                     // #1450: [317A] ==> [112D]
      u8"\xe1\x84\xae\0"                                     // #1451: [317B] ==> [112E]
      u8"\xe1\x84\xaf\0"                                     // #1452: [317C] ==> [112F]
      u8"\xe1\x84\xb2\0"                                     // #1453: [317D] ==> [1132]
      u8"\xe1\x84\xb6\0"                                     // #1454: [317E] ==> [1136]
      u8"\xe1\x85\x80\0"                                     // #1455: [317F] ==> [1140]
      u8"\xe1\x85\x87\0"                                     // #1456: [3180] ==> [1147]
      u8"\xe1\x85\x8c\0"                                     // #1457: [3181] ==> [114C]
      u8"\xe1\x87\xb1\0"                                     // #1458: [3182] ==> [11F1]
      u8"\xe1\x87\xb2\0"                                     // #1459: [3183] ==> [11F2]
      u8"\xe1\x85\x97\0"                                     // #1460: [3184] ==> [1157]
      u8"\xe1\x85\x98\0"                                     // #1461: [3185] ==> [1158]
      u8"\xe1\x85\x99\0"                                     // #1462: [3186] ==> [1159]
      u8"\xe1\x86\x84\0"                                     // #1463: [3187] ==> [1184]
      u8"\xe1\x86\x85\0"                                     // #1464: [3188] ==> [1185]
      u8"\xe1\x86\x88\0"                                     // #1465: [3189] ==> [1188]
      u8"\xe1\x86\x91\0"                                     // #1466: [318A] ==> [1191]
      u8"\xe1\x86\x92\0"                                     // #1467: [318B] ==> [1192]
      u8"\xe1\x86\x94\0"                                     // #1468: [318C] ==> [1194]
      u8"\xe1\x86\x9e\0"                                     // #1469: [318D] ==> [119E]
      u8"\xe1\x86\xa1\0"                                     // #1470: [318E] ==> [11A1]
      u8"\xe4\xb8\x89\0"                                     // #1471: [3194, 3282, 1F22A] ==> [4E09]
      u8"\xe5\x9b\x9b\0"                                     // #1472: [3195, 3283] ==> [56DB]
      u8"\xe4\xb8\x8a\0"                                     // #1473: [3196, 32A4] ==> [4E0A]
      u8"\xe4\xb8\xad\0"                                     // #1474: [3197, 32A5, 1F22D] ==> [4E2D]
      u8"\xe4\xb8\x8b\0"                                     // #1475: [3198, 32A6] ==> [4E0B]
      u8"\xe7\x94\xb2\0"                                     // #1476: [3199] ==> [7532]
      u8"\xe4\xb8\x99\0"                                     // #1477: [319B] ==> [4E19]
      u8"\xe4\xb8\x81\0"                                     // #1478: [319C] ==> [4E01]
      u8"\xe5\xa4\xa9\0"                                     // #1479: [319D, 1F217] ==> [5929]
      u8"\xe5\x9c\xb0\0"                                     // #1480: [319E] ==> [5730]
      u8"\50\xe1\x84\x80\51\0"                               // #1481: [3200] ==> [28, 1100, 29]
      u8"\50\xe1\x84\x82\51\0"                               // #1482: [3201] ==> [28, 1102, 29]
      u8"\50\xe1\x84\x83\51\0"                               // #1483: [3202] ==> [28, 1103, 29]
      u8"\50\xe1\x84\x85\51\0"                               // #1484: [3203] ==> [28, 1105, 29]
      u8"\50\xe1\x84\x86\51\0"                               // #1485: [3204] ==> [28, 1106, 29]
      u8"\50\xe1\x84\x87\51\0"                               // #1486: [3205] ==> [28, 1107, 29]
      u8"\50\xe1\x84\x89\51\0"                               // #1487: [3206] ==> [28, 1109, 29]
      u8"\50\xe1\x84\x8b\51\0"                               // #1488: [3207] ==> [28, 110B, 29]
      u8"\50\xe1\x84\x8c\51\0"                               // #1489: [3208] ==> [28, 110C, 29]
      u8"\50\xe1\x84\x8e\51\0"                               // #1490: [3209] ==> [28, 110E, 29]
      u8"\50\xe1\x84\x8f\51\0"                               // #1491: [320A] ==> [28, 110F, 29]
      u8"\50\xe1\x84\x90\51\0"                               // #1492: [320B] ==> [28, 1110, 29]
      u8"\50\xe1\x84\x91\51\0"                               // #1493: [320C] ==> [28, 1111, 29]
      u8"\50\xe1\x84\x92\51\0"                               // #1494: [320D] ==> [28, 1112, 29]
      u8"\50\xea\xb0\x80\51\0"                               // #1495: [320E] ==> [28, AC00, 29]
      u8"\50\xeb\x82\x98\51\0"                               // #1496: [320F] ==> [28, B098, 29]
      u8"\50\xeb\x8b\xa4\51\0"                               // #1497: [3210] ==> [28, B2E4, 29]
      u8"\50\xeb\x9d\xbc\51\0"                               // #1498: [3211] ==> [28, B77C, 29]
      u8"\50\xeb\xa7\x88\51\0"                               // #1499: [3212] ==> [28, B9C8, 29]
      u8"\50\xeb\xb0\x94\51\0"                               // #1500: [3213] ==> [28, BC14, 29]
      u8"\50\xec\x82\xac\51\0"                               // #1501: [3214] ==> [28, C0AC, 29]
      u8"\50\xec\x95\x84\51\0"                               // #1502: [3215] ==> [28, C544, 29]
      u8"\50\xec\x9e\x90\51\0"                               // #1503: [3216] ==> [28, C790, 29]
      u8"\50\xec\xb0\xa8\51\0"                               // #1504: [3217] ==> [28, CC28, 29]
      u8"\50\xec\xb9\xb4\51\0"                               // #1505: [3218] ==> [28, CE74, 29]
      u8"\50\xed\x83\x80\51\0"                               // #1506: [3219] ==> [28, D0C0, 29]
      u8"\50\xed\x8c\x8c\51\0"                               // #1507: [321A] ==> [28, D30C, 29]
      u8"\50\xed\x95\x98\51\0"                               // #1508: [321B] ==> [28, D558, 29]
      u8"\50\xec\xa3\xbc\51\0"                               // #1509: [321C] ==> [28, C8FC, 29]
      u8"\50\xec\x98\xa4\xec\xa0\x84\51\0"                   // #1510: [321D] ==> [28, C624, C804, 29]
      u8"\50\xec\x98\xa4\xed\x9b\x84\51\0"                   // #1511: [321E] ==> [28, C624, D6C4, 29]
      u8"\50\xe4\xb8\x80\51\0"                               // #1512: [3220] ==> [28, 4E00, 29]
      u8"\50\xe4\xba\x8c\51\0"                               // #1513: [3221] ==> [28, 4E8C, 29]
      u8"\50\xe4\xb8\x89\51\0"                               // #1514: [3222] ==> [28, 4E09, 29]
      u8"\50\xe5\x9b\x9b\51\0"                               // #1515: [3223] ==> [28, 56DB, 29]
      u8"\50\xe4\xba\x94\51\0"                               // #1516: [3224] ==> [28, 4E94, 29]
      u8"\50\xe5\x85\xad\51\0"                               // #1517: [3225] ==> [28, 516D, 29]
      u8"\50\xe4\xb8\x83\51\0"                               // #1518: [3226] ==> [28, 4E03, 29]
      u8"\50\xe5\x85\xab\51\0"                               // #1519: [3227] ==> [28, 516B, 29]
      u8"\50\xe4\xb9\x9d\51\0"                               // #1520: [3228] ==> [28, 4E5D, 29]
      u8"\50\xe5\x8d\x81\51\0"                               // #1521: [3229] ==> [28, 5341, 29]
      u8"\50\xe6\x9c\x88\51\0"                               // #1522: [322A] ==> [28, 6708, 29]
      u8"\50\xe7\x81\xab\51\0"                               // #1523: [322B] ==> [28, 706B, 29]
      u8"\50\xe6\xb0\xb4\51\0"                               // #1524: [322C] ==> [28, 6C34, 29]
      u8"\50\xe6\x9c\xa8\51\0"                               // #1525: [322D] ==> [28, 6728, 29]
      u8"\50\xe9\x87\x91\51\0"                               // #1526: [322E] ==> [28, 91D1, 29]
      u8"\50\xe5\x9c\x9f\51\0"                               // #1527: [322F] ==> [28, 571F, 29]
      u8"\50\xe6\x97\xa5\51\0"                               // #1528: [3230] ==> [28, 65E5, 29]
      u8"\50\xe6\xa0\xaa\51\0"                               // #1529: [3231] ==> [28, 682A, 29]
      u8"\50\xe6\x9c\x89\51\0"                               // #1530: [3232] ==> [28, 6709, 29]
      u8"\50\xe7\xa4\xbe\51\0"                               // #1531: [3233] ==> [28, 793E, 29]
      u8"\50\xe5\x90\x8d\51\0"                               // #1532: [3234] ==> [28, 540D, 29]
      u8"\50\xe7\x89\xb9\51\0"                               // #1533: [3235] ==> [28, 7279, 29]
      u8"\50\xe8\xb2\xa1\51\0"                               // #1534: [3236] ==> [28, 8CA1, 29]
      u8"\50\xe7\xa5\x9d\51\0"                               // #1535: [3237] ==> [28, 795D, 29]
      u8"\50\xe5\x8a\xb4\51\0"                               // #1536: [3238] ==> [28, 52B4, 29]
      u8"\50\xe4\xbb\xa3\51\0"                               // #1537: [3239] ==> [28, 4EE3, 29]
      u8"\50\xe5\x91\xbc\51\0"                               // #1538: [323A] ==> [28, 547C, 29]
      u8"\50\xe5\xad\xa6\51\0"                               // #1539: [323B] ==> [28, 5B66, 29]
      u8"\50\xe7\x9b\xa3\51\0"                               // #1540: [323C] ==> [28, 76E3, 29]
      u8"\50\xe4\xbc\x81\51\0"                               // #1541: [323D] ==> [28, 4F01, 29]
      u8"\50\xe8\xb3\x87\51\0"                               // #1542: [323E] ==> [28, 8CC7, 29]
      u8"\50\xe5\x8d\x94\51\0"                               // #1543: [323F] ==> [28, 5354, 29]
      u8"\50\xe7\xa5\xad\51\0"                               // #1544: [3240] ==> [28, 796D, 29]
      u8"\50\xe4\xbc\x91\51\0"                               // #1545: [3241] ==> [28, 4F11, 29]
      u8"\50\xe8\x87\xaa\51\0"                               // #1546: [3242] ==> [28, 81EA, 29]
      u8"\50\xe8\x87\xb3\51\0"                               // #1547: [3243] ==> [28, 81F3, 29]
      u8"\xe5\x95\x8f\0"                                     // #1548: [3244] ==> [554F]
      u8"\xe5\xb9\xbc\0"                                     // #1549: [3245] ==> [5E7C]
      u8"\xe7\xae\x8f\0"                                     // #1550: [3247] ==> [7B8F]
      u8"\x70\x74\x65\0"                                     // #1551: [3250] ==> [70, 74, 65]
      u8"\62\61\0"                                           // #1552: [3251] ==> [32, 31]
      u8"\62\62\0"                                           // #1553: [3252] ==> [32, 32]
      u8"\62\63\0"                                           // #1554: [3253] ==> [32, 33]
      u8"\62\64\0"                                           // #1555: [3254] ==> [32, 34]
      u8"\62\65\0"                                           // #1556: [3255] ==> [32, 35]
      u8"\62\66\0"                                           // #1557: [3256] ==> [32, 36]
      u8"\62\67\0"                                           // #1558: [3257] ==> [32, 37]
      u8"\62\70\0"                                           // #1559: [3258] ==> [32, 38]
      u8"\62\71\0"                                           // #1560: [3259] ==> [32, 39]
      u8"\63\60\0"                                           // #1561: [325A] ==> [33, 30]
      u8"\63\61\0"                                           // #1562: [325B] ==> [33, 31]
      u8"\63\62\0"                                           // #1563: [325C] ==> [33, 32]
      u8"\63\63\0"                                           // #1564: [325D] ==> [33, 33]
      u8"\63\64\0"                                           // #1565: [325E] ==> [33, 34]
      u8"\63\65\0"                                           // #1566: [325F] ==> [33, 35]
      u8"\xea\xb0\x80\0"                                     // #1567: [326E] ==> [AC00]
      u8"\xeb\x82\x98\0"                                     // #1568: [326F] ==> [B098]
      u8"\xeb\x8b\xa4\0"                                     // #1569: [3270] ==> [B2E4]
      u8"\xeb\x9d\xbc\0"                                     // #1570: [3271] ==> [B77C]
      u8"\xeb\xa7\x88\0"                                     // #1571: [3272] ==> [B9C8]
      u8"\xeb\xb0\x94\0"                                     // #1572: [3273] ==> [BC14]
      u8"\xec\x82\xac\0"                                     // #1573: [3274] ==> [C0AC]
      u8"\xec\x95\x84\0"                                     // #1574: [3275] ==> [C544]
      u8"\xec\x9e\x90\0"                                     // #1575: [3276] ==> [C790]
      u8"\xec\xb0\xa8\0"                                     // #1576: [3277] ==> [CC28]
      u8"\xec\xb9\xb4\0"                                     // #1577: [3278] ==> [CE74]
      u8"\xed\x83\x80\0"                                     // #1578: [3279] ==> [D0C0]
      u8"\xed\x8c\x8c\0"                                     // #1579: [327A] ==> [D30C]
      u8"\xed\x95\x98\0"                                     // #1580: [327B] ==> [D558]
      u8"\xec\xb0\xb8\xea\xb3\xa0\0"                         // #1581: [327C] ==> [CC38, ACE0]
      u8"\xec\xa3\xbc\xec\x9d\x98\0"                         // #1582: [327D] ==> [C8FC, C758]
      u8"\xec\x9a\xb0\0"                                     // #1583: [327E] ==> [C6B0]
      u8"\xe4\xba\x94\0"                                     // #1584: [3284] ==> [4E94]
      u8"\xe5\x85\xad\0"                                     // #1585: [3285, F9D1] ==> [516D]
      u8"\xe4\xb8\x83\0"                                     // #1586: [3286] ==> [4E03]
      u8"\xe4\xb9\x9d\0"                                     // #1587: [3288] ==> [4E5D]
      u8"\xe6\xa0\xaa\0"                                     // #1588: [3291] ==> [682A]
      u8"\xe6\x9c\x89\0"                                     // #1589: [3292, 1F236] ==> [6709]
      u8"\xe7\xa4\xbe\0"                                     // #1590: [3293, FA4C] ==> [793E]
      u8"\xe5\x90\x8d\0"                                     // #1591: [3294] ==> [540D]
      u8"\xe7\x89\xb9\0"                                     // #1592: [3295] ==> [7279]
      u8"\xe8\xb2\xa1\0"                                     // #1593: [3296] ==> [8CA1]
      u8"\xe7\xa5\x9d\0"                                     // #1594: [3297, FA51] ==> [795D]
      u8"\xe5\x8a\xb4\0"                                     // #1595: [3298] ==> [52B4]
      u8"\xe7\xa7\x98\0"                                     // #1596: [3299] ==> [79D8]
      u8"\xe7\x94\xb7\0"                                     // #1597: [329A] ==> [7537]
      u8"\xe9\x81\xa9\0"                                     // #1598: [329C] ==> [9069]
      u8"\xe5\x84\xaa\0"                                     // #1599: [329D] ==> [512A]
      u8"\xe5\x8d\xb0\0"                                     // #1600: [329E] ==> [5370]
      u8"\xe6\xb3\xa8\0"                                     // #1601: [329F] ==> [6CE8]
      u8"\xe9\xa0\x85\0"                                     // #1602: [32A0] ==> [9805]
      u8"\xe4\xbc\x91\0"                                     // #1603: [32A1] ==> [4F11]
      u8"\xe5\x86\x99\0"                                     // #1604: [32A2] ==> [5199]
      u8"\xe6\xad\xa3\0"                                     // #1605: [32A3] ==> [6B63]
      u8"\xe5\xb7\xa6\0"                                     // #1606: [32A7, 1F22C] ==> [5DE6]
      u8"\xe5\x8f\xb3\0"                                     // #1607: [32A8, 1F22E] ==> [53F3]
      u8"\xe5\x8c\xbb\0"                                     // #1608: [32A9] ==> [533B]
      u8"\xe5\xae\x97\0"                                     // #1609: [32AA] ==> [5B97]
      u8"\xe5\xad\xa6\0"                                     // #1610: [32AB] ==> [5B66]
      u8"\xe7\x9b\xa3\0"                                     // #1611: [32AC] ==> [76E3]
      u8"\xe4\xbc\x81\0"                                     // #1612: [32AD] ==> [4F01]
      u8"\xe8\xb3\x87\0"                                     // #1613: [32AE] ==> [8CC7]
      u8"\xe5\x8d\x94\0"                                     // #1614: [32AF] ==> [5354]
      u8"\xe5\xa4\x9c\0"                                     // #1615: [32B0] ==> [591C]
      u8"\63\66\0"                                           // #1616: [32B1] ==> [33, 36]
      u8"\63\67\0"                                           // #1617: [32B2] ==> [33, 37]
      u8"\63\70\0"                                           // #1618: [32B3] ==> [33, 38]
      u8"\63\71\0"                                           // #1619: [32B4] ==> [33, 39]
      u8"\64\60\0"                                           // #1620: [32B5] ==> [34, 30]
      u8"\64\61\0"                                           // #1621: [32B6] ==> [34, 31]
      u8"\64\62\0"                                           // #1622: [32B7] ==> [34, 32]
      u8"\64\63\0"                                           // #1623: [32B8] ==> [34, 33]
      u8"\64\64\0"                                           // #1624: [32B9] ==> [34, 34]
      u8"\64\65\0"                                           // #1625: [32BA] ==> [34, 35]
      u8"\64\66\0"                                           // #1626: [32BB] ==> [34, 36]
      u8"\64\67\0"                                           // #1627: [32BC] ==> [34, 37]
      u8"\64\70\0"                                           // #1628: [32BD] ==> [34, 38]
      u8"\64\71\0"                                           // #1629: [32BE] ==> [34, 39]
      u8"\65\60\0"                                           // #1630: [32BF] ==> [35, 30]
      u8"\61\xe6\x9c\x88\0"                                  // #1631: [32C0] ==> [31, 6708]
      u8"\62\xe6\x9c\x88\0"                                  // #1632: [32C1] ==> [32, 6708]
      u8"\63\xe6\x9c\x88\0"                                  // #1633: [32C2] ==> [33, 6708]
      u8"\64\xe6\x9c\x88\0"                                  // #1634: [32C3] ==> [34, 6708]
      u8"\65\xe6\x9c\x88\0"                                  // #1635: [32C4] ==> [35, 6708]
      u8"\66\xe6\x9c\x88\0"                                  // #1636: [32C5] ==> [36, 6708]
      u8"\67\xe6\x9c\x88\0"                                  // #1637: [32C6] ==> [37, 6708]
      u8"\70\xe6\x9c\x88\0"                                  // #1638: [32C7] ==> [38, 6708]
      u8"\71\xe6\x9c\x88\0"                                  // #1639: [32C8] ==> [39, 6708]
      u8"\61\60\xe6\x9c\x88\0"                               // #1640: [32C9] ==> [31, 30, 6708]
      u8"\61\61\xe6\x9c\x88\0"                               // #1641: [32CA] ==> [31, 31, 6708]
      u8"\61\62\xe6\x9c\x88\0"                               // #1642: [32CB] ==> [31, 32, 6708]
      u8"\x68\x67\0"                                         // #1643: [32CC] ==> [68, 67]
      u8"\x65\x72\x67\0"                                     // #1644: [32CD] ==> [65, 72, 67]
      u8"\x65\x76\0"                                         // #1645: [32CE] ==> [65, 76]
      u8"\x6c\x74\x64\0"                                     // #1646: [32CF] ==> [6C, 74, 64]
      u8"\xe3\x82\xa2\0"                                     // #1647: [32D0, FF71] ==> [30A2]
      u8"\xe3\x82\xa4\0"                                     // #1648: [32D1, FF72] ==> [30A4]
      u8"\xe3\x82\xa6\0"                                     // #1649: [32D2, FF73] ==> [30A6]
      u8"\xe3\x82\xa8\0"                                     // #1650: [32D3, FF74] ==> [30A8]
      u8"\xe3\x82\xaa\0"                                     // #1651: [32D4, FF75] ==> [30AA]
      u8"\xe3\x82\xab\0"                                     // #1652: [32D5, FF76] ==> [30AB]
      u8"\xe3\x82\xad\0"                                     // #1653: [32D6, FF77] ==> [30AD]
      u8"\xe3\x82\xaf\0"                                     // #1654: [32D7, FF78] ==> [30AF]
      u8"\xe3\x82\xb1\0"                                     // #1655: [32D8, FF79] ==> [30B1]
      u8"\xe3\x82\xb3\0"                                     // #1656: [32D9, FF7A] ==> [30B3]
      u8"\xe3\x82\xb5\0"                                     // #1657: [32DA, FF7B, 1F202] ==> [30B5]
      u8"\xe3\x82\xb7\0"                                     // #1658: [32DB, FF7C] ==> [30B7]
      u8"\xe3\x82\xb9\0"                                     // #1659: [32DC, FF7D] ==> [30B9]
      u8"\xe3\x82\xbb\0"                                     // #1660: [32DD, FF7E] ==> [30BB]
      u8"\xe3\x82\xbd\0"                                     // #1661: [32DE, FF7F] ==> [30BD]
      u8"\xe3\x82\xbf\0"                                     // #1662: [32DF, FF80] ==> [30BF]
      u8"\xe3\x83\x81\0"                                     // #1663: [32E0, FF81] ==> [30C1]
      u8"\xe3\x83\x84\0"                                     // #1664: [32E1, FF82] ==> [30C4]
      u8"\xe3\x83\x86\0"                                     // #1665: [32E2, FF83] ==> [30C6]
      u8"\xe3\x83\x88\0"                                     // #1666: [32E3, FF84] ==> [30C8]
      u8"\xe3\x83\x8a\0"                                     // #1667: [32E4, FF85] ==> [30CA]
      u8"\xe3\x83\x8b\0"                                     // #1668: [32E5, FF86] ==> [30CB]
      u8"\xe3\x83\x8c\0"                                     // #1669: [32E6, FF87] ==> [30CC]
      u8"\xe3\x83\x8d\0"                                     // #1670: [32E7, FF88] ==> [30CD]
      u8"\xe3\x83\x8e\0"                                     // #1671: [32E8, FF89] ==> [30CE]
      u8"\xe3\x83\x8f\0"                                     // #1672: [32E9, FF8A] ==> [30CF]
      u8"\xe3\x83\x92\0"                                     // #1673: [32EA, FF8B] ==> [30D2]
      u8"\xe3\x83\x95\0"                                     // #1674: [32EB, FF8C] ==> [30D5]
      u8"\xe3\x83\x98\0"                                     // #1675: [32EC, FF8D] ==> [30D8]
      u8"\xe3\x83\x9b\0"                                     // #1676: [32ED, FF8E] ==> [30DB]
      u8"\xe3\x83\x9e\0"                                     // #1677: [32EE, FF8F] ==> [30DE]
      u8"\xe3\x83\x9f\0"                                     // #1678: [32EF, FF90] ==> [30DF]
      u8"\xe3\x83\xa0\0"                                     // #1679: [32F0, FF91] ==> [30E0]
      u8"\xe3\x83\xa1\0"                                     // #1680: [32F1, FF92] ==> [30E1]
      u8"\xe3\x83\xa2\0"                                     // #1681: [32F2, FF93] ==> [30E2]
      u8"\xe3\x83\xa4\0"                                     // #1682: [32F3, FF94] ==> [30E4]
      u8"\xe3\x83\xa6\0"                                     // #1683: [32F4, FF95] ==> [30E6]
      u8"\xe3\x83\xa8\0"                                     // #1684: [32F5, FF96] ==> [30E8]
      u8"\xe3\x83\xa9\0"                                     // #1685: [32F6, FF97] ==> [30E9]
      u8"\xe3\x83\xaa\0"                                     // #1686: [32F7, FF98] ==> [30EA]
      u8"\xe3\x83\xab\0"                                     // #1687: [32F8, FF99] ==> [30EB]
      u8"\xe3\x83\xac\0"                                     // #1688: [32F9, FF9A] ==> [30EC]
      u8"\xe3\x83\xad\0"                                     // #1689: [32FA, FF9B] ==> [30ED]
      u8"\xe3\x83\xaf\0"                                     // #1690: [32FB, FF9C] ==> [30EF]
      u8"\xe3\x83\xb0\0"                                     // #1691: [32FC] ==> [30F0]
      u8"\xe3\x83\xb1\0"                                     // #1692: [32FD] ==> [30F1]
      u8"\xe3\x83\xb2\0"                                     // #1693: [32FE, FF66] ==> [30F2]
      u8"\xe4\xbb\xa4\xe5\x92\x8c\0"                         // #1694: [32FF] ==> [4EE4, 548C]
      u8"\xe3\x82\xa2\xe3\x83\x91\xe3\x83\xbc\xe3\x83\x88\0" // #1695: [3300] ==> [30A2, 30D1, 30FC, 30C8]
      u8"\xe3\x82\xa2\xe3\x83\xab\xe3\x83\x95\xe3\x82\xa1\0" // #1696: [3301] ==> [30A2, 30EB, 30D5, 30A1]
      u8"\xe3\x82\xa2\xe3\x83\xb3\xe3\x83\x9a\xe3\x82\xa2\0" // #1697: [3302] ==> [30A2, 30F3, 30DA, 30A2]
      u8"\xe3\x82\xa2\xe3\x83\xbc\xe3\x83\xab\0"             // #1698: [3303] ==> [30A2, 30FC, 30EB]
      u8"\xe3\x82\xa4\xe3\x83\x8b\xe3\x83\xb3\xe3\x82\xb0\0" // #1699: [3304] ==> [30A4, 30CB, 30F3, 30B0]
      u8"\xe3\x82\xa4\xe3\x83\xb3\xe3\x83\x81\0"             // #1700: [3305] ==> [30A4, 30F3, 30C1]
      u8"\xe3\x82\xa6\xe3\x82\xa9\xe3\x83\xb3\0"             // #1701: [3306] ==> [30A6, 30A9, 30F3]
      u8"\xe3\x82\xa8\xe3\x82\xb9\xe3\x82\xaf\xe3\x83\xbc\xe3\x83\x89\0" // #1702: [3307] ==> [30A8, 30B9,
                                                                         // 30AF, 30FC, 30C9]
      u8"\xe3\x82\xa8\xe3\x83\xbc\xe3\x82\xab\xe3\x83\xbc\0" // #1703: [3308] ==> [30A8, 30FC, 30AB, 30FC]
      u8"\xe3\x82\xaa\xe3\x83\xb3\xe3\x82\xb9\0"             // #1704: [3309] ==> [30AA, 30F3, 30B9]
      u8"\xe3\x82\xaa\xe3\x83\xbc\xe3\x83\xa0\0"             // #1705: [330A] ==> [30AA, 30FC, 30E0]
      u8"\xe3\x82\xab\xe3\x82\xa4\xe3\x83\xaa\0"             // #1706: [330B] ==> [30AB, 30A4, 30EA]
      u8"\xe3\x82\xab\xe3\x83\xa9\xe3\x83\x83\xe3\x83\x88\0" // #1707: [330C] ==> [30AB, 30E9, 30C3, 30C8]
      u8"\xe3\x82\xab\xe3\x83\xad\xe3\x83\xaa\xe3\x83\xbc\0" // #1708: [330D] ==> [30AB, 30ED, 30EA, 30FC]
      u8"\xe3\x82\xac\xe3\x83\xad\xe3\x83\xb3\0"             // #1709: [330E] ==> [30AC, 30ED, 30F3]
      u8"\xe3\x82\xac\xe3\x83\xb3\xe3\x83\x9e\0"             // #1710: [330F] ==> [30AC, 30F3, 30DE]
      u8"\xe3\x82\xae\xe3\x82\xac\0"                         // #1711: [3310] ==> [30AE, 30AC]
      u8"\xe3\x82\xae\xe3\x83\x8b\xe3\x83\xbc\0"             // #1712: [3311] ==> [30AE, 30CB, 30FC]
      u8"\xe3\x82\xad\xe3\x83\xa5\xe3\x83\xaa\xe3\x83\xbc\0" // #1713: [3312] ==> [30AD, 30E5, 30EA, 30FC]
      u8"\xe3\x82\xae\xe3\x83\xab\xe3\x83\x80\xe3\x83\xbc\0" // #1714: [3313] ==> [30AE, 30EB, 30C0, 30FC]
      u8"\xe3\x82\xad\xe3\x83\xad\0"                         // #1715: [3314] ==> [30AD, 30ED]
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\0" // #1716: [3315] ==> [30AD, 30ED,
                                                                         // 30B0, 30E9, 30E0]
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x83\xa1\xe3\x83\xbc\xe3\x83\x88\xe3\x83\xab\0" // #1717: [3316] ==>
                                                                                     // [30AD, 30ED, 30E1,
                                                                                     // 30FC, 30C8, 30EB]
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x83\xaf\xe3\x83\x83\xe3\x83\x88\0" // #1718: [3317] ==> [30AD, 30ED,
                                                                         // 30EF, 30C3, 30C8]
      u8"\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\0" // #1719: [3318] ==> [30B0, 30E9, 30E0]
      u8"\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\xe3\x83\x88\xe3\x83\xb3\0" // #1720: [3319] ==> [30B0, 30E9,
                                                                         // 30E0, 30C8, 30F3]
      u8"\xe3\x82\xaf\xe3\x83\xab\xe3\x82\xbc\xe3\x82\xa4\xe3\x83\xad\0" // #1721: [331A] ==> [30AF, 30EB,
                                                                         // 30BC, 30A4, 30ED]
      u8"\xe3\x82\xaf\xe3\x83\xad\xe3\x83\xbc\xe3\x83\x8d\0" // #1722: [331B] ==> [30AF, 30ED, 30FC, 30CD]
      u8"\xe3\x82\xb1\xe3\x83\xbc\xe3\x82\xb9\0"             // #1723: [331C] ==> [30B1, 30FC, 30B9]
      u8"\xe3\x82\xb3\xe3\x83\xab\xe3\x83\x8a\0"             // #1724: [331D] ==> [30B3, 30EB, 30CA]
      u8"\xe3\x82\xb3\xe3\x83\xbc\xe3\x83\x9d\0"             // #1725: [331E] ==> [30B3, 30FC, 30DD]
      u8"\xe3\x82\xb5\xe3\x82\xa4\xe3\x82\xaf\xe3\x83\xab\0" // #1726: [331F] ==> [30B5, 30A4, 30AF, 30EB]
      u8"\xe3\x82\xb5\xe3\x83\xb3\xe3\x83\x81\xe3\x83\xbc\xe3\x83\xa0\0" // #1727: [3320] ==> [30B5, 30F3,
                                                                         // 30C1, 30FC, 30E0]
      u8"\xe3\x82\xb7\xe3\x83\xaa\xe3\x83\xb3\xe3\x82\xb0\0" // #1728: [3321] ==> [30B7, 30EA, 30F3, 30B0]
      u8"\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x81\0"             // #1729: [3322] ==> [30BB, 30F3, 30C1]
      u8"\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x88\0"             // #1730: [3323] ==> [30BB, 30F3, 30C8]
      u8"\xe3\x83\x80\xe3\x83\xbc\xe3\x82\xb9\0"             // #1731: [3324] ==> [30C0, 30FC, 30B9]
      u8"\xe3\x83\x87\xe3\x82\xb7\0"                         // #1732: [3325] ==> [30C7, 30B7]
      u8"\xe3\x83\x89\xe3\x83\xab\0"                         // #1733: [3326] ==> [30C9, 30EB]
      u8"\xe3\x83\x88\xe3\x83\xb3\0"                         // #1734: [3327] ==> [30C8, 30F3]
      u8"\xe3\x83\x8a\xe3\x83\x8e\0"                         // #1735: [3328] ==> [30CA, 30CE]
      u8"\xe3\x83\x8e\xe3\x83\x83\xe3\x83\x88\0"             // #1736: [3329] ==> [30CE, 30C3, 30C8]
      u8"\xe3\x83\x8f\xe3\x82\xa4\xe3\x83\x84\0"             // #1737: [332A] ==> [30CF, 30A4, 30C4]
      u8"\xe3\x83\x91\xe3\x83\xbc\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x88\0" // #1738: [332B] ==> [30D1, 30FC,
                                                                         // 30BB, 30F3, 30C8]
      u8"\xe3\x83\x91\xe3\x83\xbc\xe3\x83\x84\0"             // #1739: [332C] ==> [30D1, 30FC, 30C4]
      u8"\xe3\x83\x90\xe3\x83\xbc\xe3\x83\xac\xe3\x83\xab\0" // #1740: [332D] ==> [30D0, 30FC, 30EC, 30EB]
      u8"\xe3\x83\x94\xe3\x82\xa2\xe3\x82\xb9\xe3\x83\x88\xe3\x83\xab\0" // #1741: [332E] ==> [30D4, 30A2,
                                                                         // 30B9, 30C8, 30EB]
      u8"\xe3\x83\x94\xe3\x82\xaf\xe3\x83\xab\0" // #1742: [332F] ==> [30D4, 30AF, 30EB]
      u8"\xe3\x83\x94\xe3\x82\xb3\0"             // #1743: [3330] ==> [30D4, 30B3]
      u8"\xe3\x83\x93\xe3\x83\xab\0"             // #1744: [3331] ==> [30D3, 30EB]
      u8"\xe3\x83\x95\xe3\x82\xa1\xe3\x83\xa9\xe3\x83\x83\xe3\x83\x89\0" // #1745: [3332] ==> [30D5, 30A1,
                                                                         // 30E9, 30C3, 30C9]
      u8"\xe3\x83\x95\xe3\x82\xa3\xe3\x83\xbc\xe3\x83\x88\0" // #1746: [3333] ==> [30D5, 30A3, 30FC, 30C8]
      u8"\xe3\x83\x96\xe3\x83\x83\xe3\x82\xb7\xe3\x82\xa7\xe3\x83\xab\0" // #1747: [3334] ==> [30D6, 30C3,
                                                                         // 30B7, 30A7, 30EB]
      u8"\xe3\x83\x95\xe3\x83\xa9\xe3\x83\xb3\0" // #1748: [3335] ==> [30D5, 30E9, 30F3]
      u8"\xe3\x83\x98\xe3\x82\xaf\xe3\x82\xbf\xe3\x83\xbc\xe3\x83\xab\0" // #1749: [3336] ==> [30D8, 30AF,
                                                                         // 30BF, 30FC, 30EB]
      u8"\xe3\x83\x9a\xe3\x82\xbd\0"                                     // #1750: [3337] ==> [30DA, 30BD]
      u8"\xe3\x83\x9a\xe3\x83\x8b\xe3\x83\x92\0"             // #1751: [3338] ==> [30DA, 30CB, 30D2]
      u8"\xe3\x83\x98\xe3\x83\xab\xe3\x83\x84\0"             // #1752: [3339] ==> [30D8, 30EB, 30C4]
      u8"\xe3\x83\x9a\xe3\x83\xb3\xe3\x82\xb9\0"             // #1753: [333A] ==> [30DA, 30F3, 30B9]
      u8"\xe3\x83\x9a\xe3\x83\xbc\xe3\x82\xb8\0"             // #1754: [333B] ==> [30DA, 30FC, 30B8]
      u8"\xe3\x83\x99\xe3\x83\xbc\xe3\x82\xbf\0"             // #1755: [333C] ==> [30D9, 30FC, 30BF]
      u8"\xe3\x83\x9d\xe3\x82\xa4\xe3\x83\xb3\xe3\x83\x88\0" // #1756: [333D] ==> [30DD, 30A4, 30F3, 30C8]
      u8"\xe3\x83\x9c\xe3\x83\xab\xe3\x83\x88\0"             // #1757: [333E] ==> [30DC, 30EB, 30C8]
      u8"\xe3\x83\x9b\xe3\x83\xb3\0"                         // #1758: [333F] ==> [30DB, 30F3]
      u8"\xe3\x83\x9d\xe3\x83\xb3\xe3\x83\x89\0"             // #1759: [3340] ==> [30DD, 30F3, 30C9]
      u8"\xe3\x83\x9b\xe3\x83\xbc\xe3\x83\xab\0"             // #1760: [3341] ==> [30DB, 30FC, 30EB]
      u8"\xe3\x83\x9b\xe3\x83\xbc\xe3\x83\xb3\0"             // #1761: [3342] ==> [30DB, 30FC, 30F3]
      u8"\xe3\x83\x9e\xe3\x82\xa4\xe3\x82\xaf\xe3\x83\xad\0" // #1762: [3343] ==> [30DE, 30A4, 30AF, 30ED]
      u8"\xe3\x83\x9e\xe3\x82\xa4\xe3\x83\xab\0"             // #1763: [3344] ==> [30DE, 30A4, 30EB]
      u8"\xe3\x83\x9e\xe3\x83\x83\xe3\x83\x8f\0"             // #1764: [3345] ==> [30DE, 30C3, 30CF]
      u8"\xe3\x83\x9e\xe3\x83\xab\xe3\x82\xaf\0"             // #1765: [3346] ==> [30DE, 30EB, 30AF]
      u8"\xe3\x83\x9e\xe3\x83\xb3\xe3\x82\xb7\xe3\x83\xa7\xe3\x83\xb3\0" // #1766: [3347] ==> [30DE, 30F3,
                                                                         // 30B7, 30E7, 30F3]
      u8"\xe3\x83\x9f\xe3\x82\xaf\xe3\x83\xad\xe3\x83\xb3\0" // #1767: [3348] ==> [30DF, 30AF, 30ED, 30F3]
      u8"\xe3\x83\x9f\xe3\x83\xaa\0"                         // #1768: [3349] ==> [30DF, 30EA]
      u8"\xe3\x83\x9f\xe3\x83\xaa\xe3\x83\x90\xe3\x83\xbc\xe3\x83\xab\0" // #1769: [334A] ==> [30DF, 30EA,
                                                                         // 30D0, 30FC, 30EB]
      u8"\xe3\x83\xa1\xe3\x82\xac\0"                                     // #1770: [334B] ==> [30E1, 30AC]
      u8"\xe3\x83\xa1\xe3\x82\xac\xe3\x83\x88\xe3\x83\xb3\0" // #1771: [334C] ==> [30E1, 30AC, 30C8, 30F3]
      u8"\xe3\x83\xa1\xe3\x83\xbc\xe3\x83\x88\xe3\x83\xab\0" // #1772: [334D] ==> [30E1, 30FC, 30C8, 30EB]
      u8"\xe3\x83\xa4\xe3\x83\xbc\xe3\x83\x89\0"             // #1773: [334E] ==> [30E4, 30FC, 30C9]
      u8"\xe3\x83\xa4\xe3\x83\xbc\xe3\x83\xab\0"             // #1774: [334F] ==> [30E4, 30FC, 30EB]
      u8"\xe3\x83\xa6\xe3\x82\xa2\xe3\x83\xb3\0"             // #1775: [3350] ==> [30E6, 30A2, 30F3]
      u8"\xe3\x83\xaa\xe3\x83\x83\xe3\x83\x88\xe3\x83\xab\0" // #1776: [3351] ==> [30EA, 30C3, 30C8, 30EB]
      u8"\xe3\x83\xaa\xe3\x83\xa9\0"                         // #1777: [3352] ==> [30EA, 30E9]
      u8"\xe3\x83\xab\xe3\x83\x94\xe3\x83\xbc\0"             // #1778: [3353] ==> [30EB, 30D4, 30FC]
      u8"\xe3\x83\xab\xe3\x83\xbc\xe3\x83\x96\xe3\x83\xab\0" // #1779: [3354] ==> [30EB, 30FC, 30D6, 30EB]
      u8"\xe3\x83\xac\xe3\x83\xa0\0"                         // #1780: [3355] ==> [30EC, 30E0]
      u8"\xe3\x83\xac\xe3\x83\xb3\xe3\x83\x88\xe3\x82\xb2\xe3\x83\xb3\0" // #1781: [3356] ==> [30EC, 30F3,
                                                                         // 30C8, 30B2, 30F3]
      u8"\xe3\x83\xaf\xe3\x83\x83\xe3\x83\x88\0"             // #1782: [3357] ==> [30EF, 30C3, 30C8]
      u8"\60\xe7\x82\xb9\0"                                  // #1783: [3358] ==> [30, 70B9]
      u8"\61\xe7\x82\xb9\0"                                  // #1784: [3359] ==> [31, 70B9]
      u8"\62\xe7\x82\xb9\0"                                  // #1785: [335A] ==> [32, 70B9]
      u8"\63\xe7\x82\xb9\0"                                  // #1786: [335B] ==> [33, 70B9]
      u8"\64\xe7\x82\xb9\0"                                  // #1787: [335C] ==> [34, 70B9]
      u8"\65\xe7\x82\xb9\0"                                  // #1788: [335D] ==> [35, 70B9]
      u8"\66\xe7\x82\xb9\0"                                  // #1789: [335E] ==> [36, 70B9]
      u8"\67\xe7\x82\xb9\0"                                  // #1790: [335F] ==> [37, 70B9]
      u8"\70\xe7\x82\xb9\0"                                  // #1791: [3360] ==> [38, 70B9]
      u8"\71\xe7\x82\xb9\0"                                  // #1792: [3361] ==> [39, 70B9]
      u8"\61\60\xe7\x82\xb9\0"                               // #1793: [3362] ==> [31, 30, 70B9]
      u8"\61\61\xe7\x82\xb9\0"                               // #1794: [3363] ==> [31, 31, 70B9]
      u8"\61\62\xe7\x82\xb9\0"                               // #1795: [3364] ==> [31, 32, 70B9]
      u8"\61\63\xe7\x82\xb9\0"                               // #1796: [3365] ==> [31, 33, 70B9]
      u8"\61\64\xe7\x82\xb9\0"                               // #1797: [3366] ==> [31, 34, 70B9]
      u8"\61\65\xe7\x82\xb9\0"                               // #1798: [3367] ==> [31, 35, 70B9]
      u8"\61\66\xe7\x82\xb9\0"                               // #1799: [3368] ==> [31, 36, 70B9]
      u8"\61\67\xe7\x82\xb9\0"                               // #1800: [3369] ==> [31, 37, 70B9]
      u8"\61\70\xe7\x82\xb9\0"                               // #1801: [336A] ==> [31, 38, 70B9]
      u8"\61\71\xe7\x82\xb9\0"                               // #1802: [336B] ==> [31, 39, 70B9]
      u8"\62\60\xe7\x82\xb9\0"                               // #1803: [336C] ==> [32, 30, 70B9]
      u8"\62\61\xe7\x82\xb9\0"                               // #1804: [336D] ==> [32, 31, 70B9]
      u8"\62\62\xe7\x82\xb9\0"                               // #1805: [336E] ==> [32, 32, 70B9]
      u8"\62\63\xe7\x82\xb9\0"                               // #1806: [336F] ==> [32, 33, 70B9]
      u8"\62\64\xe7\x82\xb9\0"                               // #1807: [3370] ==> [32, 34, 70B9]
      u8"\x68\x70\x61\0"                                     // #1808: [3371] ==> [68, 70, 61]
      u8"\x64\x61\0"                                         // #1809: [3372] ==> [64, 61]
      u8"\x61\x75\0"                                         // #1810: [3373] ==> [61, 75]
      u8"\x62\x61\x72\0"                                     // #1811: [3374] ==> [62, 61, 72]
      u8"\x6f\x76\0"                                         // #1812: [3375] ==> [6F, 76]
      u8"\x70\x63\0"                                         // #1813: [3376] ==> [70, 63]
      u8"\x64\x6d\0"                                         // #1814: [3377] ==> [64, 6D]
      u8"\x64\x6d\62\0"                                      // #1815: [3378] ==> [64, 6D, 32]
      u8"\x64\x6d\63\0"                                      // #1816: [3379] ==> [64, 6D, 33]
      u8"\x69\x75\0"                                         // #1817: [337A] ==> [69, 75]
      u8"\xe5\xb9\xb3\xe6\x88\x90\0"                         // #1818: [337B] ==> [5E73, 6210]
      u8"\xe6\x98\xad\xe5\x92\x8c\0"                         // #1819: [337C] ==> [662D, 548C]
      u8"\xe5\xa4\xa7\xe6\xad\xa3\0"                         // #1820: [337D] ==> [5927, 6B63]
      u8"\xe6\x98\x8e\xe6\xb2\xbb\0"                         // #1821: [337E] ==> [660E, 6CBB]
      u8"\xe6\xa0\xaa\xe5\xbc\x8f\xe4\xbc\x9a\xe7\xa4\xbe\0" // #1822: [337F] ==> [682A, 5F0F, 4F1A, 793E]
      u8"\x70\x61\0"                                         // #1823: [3380, 33A9] ==> [70, 61]
      u8"\x6e\x61\0"                                         // #1824: [3381] ==> [6E, 61]
      u8"\xce\xbc\x61\0"                                     // #1825: [3382] ==> [3BC, 61]
      u8"\x6d\x61\0"                                         // #1826: [3383] ==> [6D, 61]
      u8"\x6b\x61\0"                                         // #1827: [3384] ==> [6B, 61]
      u8"\x6b\x62\0"                                         // #1828: [3385] ==> [6B, 62]
      u8"\x6d\x62\0"                                         // #1829: [3386, 33D4] ==> [6D, 62]
      u8"\x67\x62\0"                                         // #1830: [3387] ==> [67, 62]
      u8"\x63\x61\x6c\0"                                     // #1831: [3388] ==> [63, 61, 6C]
      u8"\x6b\x63\x61\x6c\0"                                 // #1832: [3389] ==> [6B, 63, 61, 6C]
      u8"\x70\x66\0"                                         // #1833: [338A] ==> [70, 66]
      u8"\x6e\x66\0"                                         // #1834: [338B] ==> [6E, 66]
      u8"\xce\xbc\x66\0"                                     // #1835: [338C] ==> [3BC, 66]
      u8"\xce\xbc\x67\0"                                     // #1836: [338D] ==> [3BC, 67]
      u8"\x6d\x67\0"                                         // #1837: [338E] ==> [6D, 67]
      u8"\x6b\x67\0"                                         // #1838: [338F] ==> [6B, 67]
      u8"\x68\x7a\0"                                         // #1839: [3390] ==> [68, 7A]
      u8"\x6b\x68\x7a\0"                                     // #1840: [3391] ==> [6B, 68, 7A]
      u8"\x6d\x68\x7a\0"                                     // #1841: [3392] ==> [6D, 68, 7A]
      u8"\x67\x68\x7a\0"                                     // #1842: [3393] ==> [67, 68, 7A]
      u8"\x74\x68\x7a\0"                                     // #1843: [3394] ==> [74, 68, 7A]
      u8"\xce\xbc\x6c\0"                                     // #1844: [3395] ==> [3BC, 6C]
      u8"\x6d\x6c\0"                                         // #1845: [3396] ==> [6D, 6C]
      u8"\x64\x6c\0"                                         // #1846: [3397] ==> [64, 6C]
      u8"\x6b\x6c\0"                                         // #1847: [3398] ==> [6B, 6C]
      u8"\x66\x6d\0"                                         // #1848: [3399] ==> [66, 6D]
      u8"\x6e\x6d\0"                                         // #1849: [339A] ==> [6E, 6D]
      u8"\xce\xbc\x6d\0"                                     // #1850: [339B] ==> [3BC, 6D]
      u8"\x6d\x6d\0"                                         // #1851: [339C] ==> [6D, 6D]
      u8"\x63\x6d\0"                                         // #1852: [339D] ==> [63, 6D]
      u8"\x6b\x6d\0"                                         // #1853: [339E, 33CE] ==> [6B, 6D]
      u8"\x6d\x6d\62\0"                                      // #1854: [339F] ==> [6D, 6D, 32]
      u8"\x63\x6d\62\0"                                      // #1855: [33A0] ==> [63, 6D, 32]
      u8"\x6d\62\0"                                          // #1856: [33A1] ==> [6D, 32]
      u8"\x6b\x6d\62\0"                                      // #1857: [33A2] ==> [6B, 6D, 32]
      u8"\x6d\x6d\63\0"                                      // #1858: [33A3] ==> [6D, 6D, 33]
      u8"\x63\x6d\63\0"                                      // #1859: [33A4] ==> [63, 6D, 33]
      u8"\x6d\63\0"                                          // #1860: [33A5] ==> [6D, 33]
      u8"\x6b\x6d\63\0"                                      // #1861: [33A6] ==> [6B, 6D, 33]
      u8"\x6d\xe2\x88\x95\x73\0"                             // #1862: [33A7] ==> [6D, 2215, 73]
      u8"\x6d\xe2\x88\x95\x73\62\0"                          // #1863: [33A8] ==> [6D, 2215, 73, 32]
      u8"\x6b\x70\x61\0"                                     // #1864: [33AA] ==> [6B, 70, 61]
      u8"\x6d\x70\x61\0"                                     // #1865: [33AB] ==> [6D, 70, 61]
      u8"\x67\x70\x61\0"                                     // #1866: [33AC] ==> [67, 70, 61]
      u8"\x72\x61\x64\0"                                     // #1867: [33AD] ==> [72, 61, 64]
      u8"\x72\x61\x64\xe2\x88\x95\x73\0"                     // #1868: [33AE] ==> [72, 61, 64, 2215, 73]
      u8"\x72\x61\x64\xe2\x88\x95\x73\62\0"                  // #1869: [33AF] ==> [72, 61, 64, 2215, 73, 32]
      u8"\x70\x73\0"                                         // #1870: [33B0] ==> [70, 73]
      u8"\x6e\x73\0"                                         // #1871: [33B1] ==> [6E, 73]
      u8"\xce\xbc\x73\0"                                     // #1872: [33B2] ==> [3BC, 73]
      u8"\x6d\x73\0"                                         // #1873: [33B3] ==> [6D, 73]
      u8"\x70\x76\0"                                         // #1874: [33B4] ==> [70, 76]
      u8"\x6e\x76\0"                                         // #1875: [33B5] ==> [6E, 76]
      u8"\xce\xbc\x76\0"                                     // #1876: [33B6] ==> [3BC, 76]
      u8"\x6d\x76\0"                                         // #1877: [33B7, 33B9, 1F14B] ==> [6D, 76]
      u8"\x6b\x76\0"                                         // #1878: [33B8] ==> [6B, 76]
      u8"\x70\x77\0"                                         // #1879: [33BA] ==> [70, 77]
      u8"\x6e\x77\0"                                         // #1880: [33BB] ==> [6E, 77]
      u8"\xce\xbc\x77\0"                                     // #1881: [33BC] ==> [3BC, 77]
      u8"\x6d\x77\0"                                         // #1882: [33BD, 33BF] ==> [6D, 77]
      u8"\x6b\x77\0"                                         // #1883: [33BE] ==> [6B, 77]
      u8"\x6b\xcf\x89\0"                                     // #1884: [33C0] ==> [6B, 3C9]
      u8"\x6d\xcf\x89\0"                                     // #1885: [33C1] ==> [6D, 3C9]
      u8"\x62\x71\0"                                         // #1886: [33C3] ==> [62, 71]
      u8"\x63\x63\0"                                         // #1887: [33C4] ==> [63, 63]
      u8"\x63\x64\0"                                         // #1888: [33C5, 1F12D] ==> [63, 64]
      u8"\x63\xe2\x88\x95\x6b\x67\0"                         // #1889: [33C6] ==> [63, 2215, 6B, 67]
      u8"\x64\x62\0"                                         // #1890: [33C8] ==> [64, 62]
      u8"\x67\x79\0"                                         // #1891: [33C9] ==> [67, 79]
      u8"\x68\x61\0"                                         // #1892: [33CA] ==> [68, 61]
      u8"\x68\x70\0"                                         // #1893: [33CB] ==> [68, 70]
      u8"\x69\x6e\0"                                         // #1894: [33CC] ==> [69, 6E]
      u8"\x6b\x6b\0"                                         // #1895: [33CD] ==> [6B, 6B]
      u8"\x6b\x74\0"                                         // #1896: [33CF] ==> [6B, 74]
      u8"\x6c\x6d\0"                                         // #1897: [33D0] ==> [6C, 6D]
      u8"\x6c\x6e\0"                                         // #1898: [33D1] ==> [6C, 6E]
      u8"\x6c\x6f\x67\0"                                     // #1899: [33D2] ==> [6C, 6F, 67]
      u8"\x6c\x78\0"                                         // #1900: [33D3] ==> [6C, 78]
      u8"\x6d\x69\x6c\0"                                     // #1901: [33D5] ==> [6D, 69, 6C]
      u8"\x6d\x6f\x6c\0"                                     // #1902: [33D6] ==> [6D, 6F, 6C]
      u8"\x70\x68\0"                                         // #1903: [33D7] ==> [70, 68]
      u8"\x70\x70\x6d\0"                                     // #1904: [33D9] ==> [70, 70, 6D]
      u8"\x70\x72\0"                                         // #1905: [33DA] ==> [70, 72]
      u8"\x73\x72\0"                                         // #1906: [33DB] ==> [73, 72]
      u8"\x73\x76\0"                                         // #1907: [33DC] ==> [73, 76]
      u8"\x77\x62\0"                                         // #1908: [33DD] ==> [77, 62]
      u8"\x76\xe2\x88\x95\x6d\0"                             // #1909: [33DE] ==> [76, 2215, 6D]
      u8"\x61\xe2\x88\x95\x6d\0"                             // #1910: [33DF] ==> [61, 2215, 6D]
      u8"\61\xe6\x97\xa5\0"                                  // #1911: [33E0] ==> [31, 65E5]
      u8"\62\xe6\x97\xa5\0"                                  // #1912: [33E1] ==> [32, 65E5]
      u8"\63\xe6\x97\xa5\0"                                  // #1913: [33E2] ==> [33, 65E5]
      u8"\64\xe6\x97\xa5\0"                                  // #1914: [33E3] ==> [34, 65E5]
      u8"\65\xe6\x97\xa5\0"                                  // #1915: [33E4] ==> [35, 65E5]
      u8"\66\xe6\x97\xa5\0"                                  // #1916: [33E5] ==> [36, 65E5]
      u8"\67\xe6\x97\xa5\0"                                  // #1917: [33E6] ==> [37, 65E5]
      u8"\70\xe6\x97\xa5\0"                                  // #1918: [33E7] ==> [38, 65E5]
      u8"\71\xe6\x97\xa5\0"                                  // #1919: [33E8] ==> [39, 65E5]
      u8"\61\60\xe6\x97\xa5\0"                               // #1920: [33E9] ==> [31, 30, 65E5]
      u8"\61\61\xe6\x97\xa5\0"                               // #1921: [33EA] ==> [31, 31, 65E5]
      u8"\61\62\xe6\x97\xa5\0"                               // #1922: [33EB] ==> [31, 32, 65E5]
      u8"\61\63\xe6\x97\xa5\0"                               // #1923: [33EC] ==> [31, 33, 65E5]
      u8"\61\64\xe6\x97\xa5\0"                               // #1924: [33ED] ==> [31, 34, 65E5]
      u8"\61\65\xe6\x97\xa5\0"                               // #1925: [33EE] ==> [31, 35, 65E5]
      u8"\61\66\xe6\x97\xa5\0"                               // #1926: [33EF] ==> [31, 36, 65E5]
      u8"\61\67\xe6\x97\xa5\0"                               // #1927: [33F0] ==> [31, 37, 65E5]
      u8"\61\70\xe6\x97\xa5\0"                               // #1928: [33F1] ==> [31, 38, 65E5]
      u8"\61\71\xe6\x97\xa5\0"                               // #1929: [33F2] ==> [31, 39, 65E5]
      u8"\62\60\xe6\x97\xa5\0"                               // #1930: [33F3] ==> [32, 30, 65E5]
      u8"\62\61\xe6\x97\xa5\0"                               // #1931: [33F4] ==> [32, 31, 65E5]
      u8"\62\62\xe6\x97\xa5\0"                               // #1932: [33F5] ==> [32, 32, 65E5]
      u8"\62\63\xe6\x97\xa5\0"                               // #1933: [33F6] ==> [32, 33, 65E5]
      u8"\62\64\xe6\x97\xa5\0"                               // #1934: [33F7] ==> [32, 34, 65E5]
      u8"\62\65\xe6\x97\xa5\0"                               // #1935: [33F8] ==> [32, 35, 65E5]
      u8"\62\66\xe6\x97\xa5\0"                               // #1936: [33F9] ==> [32, 36, 65E5]
      u8"\62\67\xe6\x97\xa5\0"                               // #1937: [33FA] ==> [32, 37, 65E5]
      u8"\62\70\xe6\x97\xa5\0"                               // #1938: [33FB] ==> [32, 38, 65E5]
      u8"\62\71\xe6\x97\xa5\0"                               // #1939: [33FC] ==> [32, 39, 65E5]
      u8"\63\60\xe6\x97\xa5\0"                               // #1940: [33FD] ==> [33, 30, 65E5]
      u8"\63\61\xe6\x97\xa5\0"                               // #1941: [33FE] ==> [33, 31, 65E5]
      u8"\x67\x61\x6c\0"                                     // #1942: [33FF] ==> [67, 61, 6C]
      u8"\xea\x99\x81\0"                                     // #1943: [A640] ==> [A641]
      u8"\xea\x99\x83\0"                                     // #1944: [A642] ==> [A643]
      u8"\xea\x99\x85\0"                                     // #1945: [A644] ==> [A645]
      u8"\xea\x99\x87\0"                                     // #1946: [A646] ==> [A647]
      u8"\xea\x99\x89\0"                                     // #1947: [A648] ==> [A649]
      u8"\xea\x99\x8d\0"                                     // #1948: [A64C] ==> [A64D]
      u8"\xea\x99\x8f\0"                                     // #1949: [A64E] ==> [A64F]
      u8"\xea\x99\x91\0"                                     // #1950: [A650, 1E06C] ==> [A651]
      u8"\xea\x99\x93\0"                                     // #1951: [A652] ==> [A653]
      u8"\xea\x99\x95\0"                                     // #1952: [A654] ==> [A655]
      u8"\xea\x99\x97\0"                                     // #1953: [A656] ==> [A657]
      u8"\xea\x99\x99\0"                                     // #1954: [A658] ==> [A659]
      u8"\xea\x99\x9b\0"                                     // #1955: [A65A] ==> [A65B]
      u8"\xea\x99\x9d\0"                                     // #1956: [A65C] ==> [A65D]
      u8"\xea\x99\x9f\0"                                     // #1957: [A65E] ==> [A65F]
      u8"\xea\x99\xa1\0"                                     // #1958: [A660] ==> [A661]
      u8"\xea\x99\xa3\0"                                     // #1959: [A662] ==> [A663]
      u8"\xea\x99\xa5\0"                                     // #1960: [A664] ==> [A665]
      u8"\xea\x99\xa7\0"                                     // #1961: [A666] ==> [A667]
      u8"\xea\x99\xa9\0"                                     // #1962: [A668] ==> [A669]
      u8"\xea\x99\xab\0"                                     // #1963: [A66A] ==> [A66B]
      u8"\xea\x99\xad\0"                                     // #1964: [A66C] ==> [A66D]
      u8"\xea\x9a\x81\0"                                     // #1965: [A680] ==> [A681]
      u8"\xea\x9a\x83\0"                                     // #1966: [A682] ==> [A683]
      u8"\xea\x9a\x85\0"                                     // #1967: [A684] ==> [A685]
      u8"\xea\x9a\x87\0"                                     // #1968: [A686] ==> [A687]
      u8"\xea\x9a\x89\0"                                     // #1969: [A688, 1E04A] ==> [A689]
      u8"\xea\x9a\x8b\0"                                     // #1970: [A68A] ==> [A68B]
      u8"\xea\x9a\x8d\0"                                     // #1971: [A68C] ==> [A68D]
      u8"\xea\x9a\x8f\0"                                     // #1972: [A68E] ==> [A68F]
      u8"\xea\x9a\x91\0"                                     // #1973: [A690] ==> [A691]
      u8"\xea\x9a\x93\0"                                     // #1974: [A692] ==> [A693]
      u8"\xea\x9a\x95\0"                                     // #1975: [A694] ==> [A695]
      u8"\xea\x9a\x97\0"                                     // #1976: [A696] ==> [A697]
      u8"\xea\x9a\x99\0"                                     // #1977: [A698] ==> [A699]
      u8"\xea\x9a\x9b\0"                                     // #1978: [A69A] ==> [A69B]
      u8"\xea\x9c\xa3\0"                                     // #1979: [A722] ==> [A723]
      u8"\xea\x9c\xa5\0"                                     // #1980: [A724] ==> [A725]
      u8"\xea\x9c\xa7\0"                                     // #1981: [A726, AB5C] ==> [A727]
      u8"\xea\x9c\xa9\0"                                     // #1982: [A728] ==> [A729]
      u8"\xea\x9c\xab\0"                                     // #1983: [A72A] ==> [A72B]
      u8"\xea\x9c\xad\0"                                     // #1984: [A72C] ==> [A72D]
      u8"\xea\x9c\xaf\0"                                     // #1985: [A72E] ==> [A72F]
      u8"\xea\x9c\xb3\0"                                     // #1986: [A732] ==> [A733]
      u8"\xea\x9c\xb5\0"                                     // #1987: [A734] ==> [A735]
      u8"\xea\x9c\xb7\0"                                     // #1988: [A736] ==> [A737]
      u8"\xea\x9c\xb9\0"                                     // #1989: [A738] ==> [A739]
      u8"\xea\x9c\xbb\0"                                     // #1990: [A73A] ==> [A73B]
      u8"\xea\x9c\xbd\0"                                     // #1991: [A73C] ==> [A73D]
      u8"\xea\x9c\xbf\0"                                     // #1992: [A73E] ==> [A73F]
      u8"\xea\x9d\x81\0"                                     // #1993: [A740] ==> [A741]
      u8"\xea\x9d\x83\0"                                     // #1994: [A742] ==> [A743]
      u8"\xea\x9d\x85\0"                                     // #1995: [A744] ==> [A745]
      u8"\xea\x9d\x87\0"                                     // #1996: [A746] ==> [A747]
      u8"\xea\x9d\x89\0"                                     // #1997: [A748] ==> [A749]
      u8"\xea\x9d\x8b\0"                                     // #1998: [A74A] ==> [A74B]
      u8"\xea\x9d\x8d\0"                                     // #1999: [A74C] ==> [A74D]
      u8"\xea\x9d\x8f\0"                                     // #2000: [A74E] ==> [A74F]
      u8"\xea\x9d\x91\0"                                     // #2001: [A750] ==> [A751]
      u8"\xea\x9d\x93\0"                                     // #2002: [A752] ==> [A753]
      u8"\xea\x9d\x95\0"                                     // #2003: [A754] ==> [A755]
      u8"\xea\x9d\x97\0"                                     // #2004: [A756] ==> [A757]
      u8"\xea\x9d\x99\0"                                     // #2005: [A758] ==> [A759]
      u8"\xea\x9d\x9b\0"                                     // #2006: [A75A] ==> [A75B]
      u8"\xea\x9d\x9d\0"                                     // #2007: [A75C] ==> [A75D]
      u8"\xea\x9d\x9f\0"                                     // #2008: [A75E] ==> [A75F]
      u8"\xea\x9d\xa1\0"                                     // #2009: [A760] ==> [A761]
      u8"\xea\x9d\xa3\0"                                     // #2010: [A762] ==> [A763]
      u8"\xea\x9d\xa5\0"                                     // #2011: [A764] ==> [A765]
      u8"\xea\x9d\xa7\0"                                     // #2012: [A766] ==> [A767]
      u8"\xea\x9d\xa9\0"                                     // #2013: [A768] ==> [A769]
      u8"\xea\x9d\xab\0"                                     // #2014: [A76A] ==> [A76B]
      u8"\xea\x9d\xad\0"                                     // #2015: [A76C] ==> [A76D]
      u8"\xea\x9d\xaf\0"                                     // #2016: [A76E, A770] ==> [A76F]
      u8"\xea\x9d\xba\0"                                     // #2017: [A779] ==> [A77A]
      u8"\xea\x9d\xbc\0"                                     // #2018: [A77B] ==> [A77C]
      u8"\xe1\xb5\xb9\0"                                     // #2019: [A77D] ==> [1D79]
      u8"\xea\x9d\xbf\0"                                     // #2020: [A77E] ==> [A77F]
      u8"\xea\x9e\x81\0"                                     // #2021: [A780] ==> [A781]
      u8"\xea\x9e\x83\0"                                     // #2022: [A782] ==> [A783]
      u8"\xea\x9e\x85\0"                                     // #2023: [A784] ==> [A785]
      u8"\xea\x9e\x87\0"                                     // #2024: [A786] ==> [A787]
      u8"\xea\x9e\x8c\0"                                     // #2025: [A78B] ==> [A78C]
      u8"\xea\x9e\x91\0"                                     // #2026: [A790] ==> [A791]
      u8"\xea\x9e\x93\0"                                     // #2027: [A792] ==> [A793]
      u8"\xea\x9e\x97\0"                                     // #2028: [A796] ==> [A797]
      u8"\xea\x9e\x99\0"                                     // #2029: [A798] ==> [A799]
      u8"\xea\x9e\x9b\0"                                     // #2030: [A79A] ==> [A79B]
      u8"\xea\x9e\x9d\0"                                     // #2031: [A79C] ==> [A79D]
      u8"\xea\x9e\x9f\0"                                     // #2032: [A79E] ==> [A79F]
      u8"\xea\x9e\xa1\0"                                     // #2033: [A7A0] ==> [A7A1]
      u8"\xea\x9e\xa3\0"                                     // #2034: [A7A2] ==> [A7A3]
      u8"\xea\x9e\xa5\0"                                     // #2035: [A7A4] ==> [A7A5]
      u8"\xea\x9e\xa7\0"                                     // #2036: [A7A6] ==> [A7A7]
      u8"\xea\x9e\xa9\0"                                     // #2037: [A7A8] ==> [A7A9]
      u8"\xc9\xac\0"                                         // #2038: [A7AD, 1079B] ==> [26C]
      u8"\xca\x9e\0"                                         // #2039: [A7B0] ==> [29E]
      u8"\xca\x87\0"                                         // #2040: [A7B1] ==> [287]
      u8"\xea\xad\x93\0"                                     // #2041: [A7B3] ==> [AB53]
      u8"\xea\x9e\xb5\0"                                     // #2042: [A7B4] ==> [A7B5]
      u8"\xea\x9e\xb7\0"                                     // #2043: [A7B6] ==> [A7B7]
      u8"\xea\x9e\xb9\0"                                     // #2044: [A7B8] ==> [A7B9]
      u8"\xea\x9e\xbb\0"                                     // #2045: [A7BA] ==> [A7BB]
      u8"\xea\x9e\xbd\0"                                     // #2046: [A7BC] ==> [A7BD]
      u8"\xea\x9e\xbf\0"                                     // #2047: [A7BE] ==> [A7BF]
      u8"\xea\x9f\x81\0"                                     // #2048: [A7C0] ==> [A7C1]
      u8"\xea\x9f\x83\0"                                     // #2049: [A7C2] ==> [A7C3]
      u8"\xea\x9e\x94\0"                                     // #2050: [A7C4] ==> [A794]
      u8"\xe1\xb6\x8e\0"                                     // #2051: [A7C6] ==> [1D8E]
      u8"\xea\x9f\x88\0"                                     // #2052: [A7C7] ==> [A7C8]
      u8"\xea\x9f\x8a\0"                                     // #2053: [A7C9] ==> [A7CA]
      u8"\xc9\xa4\0"                                         // #2054: [A7CB, 10791] ==> [264]
      u8"\xea\x9f\x8d\0"                                     // #2055: [A7CC] ==> [A7CD]
      u8"\xea\x9f\x91\0"                                     // #2056: [A7D0] ==> [A7D1]
      u8"\xea\x9f\x97\0"                                     // #2057: [A7D6] ==> [A7D7]
      u8"\xea\x9f\x99\0"                                     // #2058: [A7D8] ==> [A7D9]
      u8"\xea\x9f\x9b\0"                                     // #2059: [A7DA] ==> [A7DB]
      u8"\xc6\x9b\0"                                         // #2060: [A7DC] ==> [19B]
      u8"\xea\x9f\xb6\0"                                     // #2061: [A7F5] ==> [A7F6]
      u8"\xea\xac\xb7\0"                                     // #2062: [AB5D] ==> [AB37]
      u8"\xea\xad\x92\0"                                     // #2063: [AB5F] ==> [AB52]
      u8"\xca\x8d\0"                                         // #2064: [AB69] ==> [28D]
      u8"\xe1\x8e\xa0\0"                                     // #2065: [AB70] ==> [13A0]
      u8"\xe1\x8e\xa1\0"                                     // #2066: [AB71] ==> [13A1]
      u8"\xe1\x8e\xa2\0"                                     // #2067: [AB72] ==> [13A2]
      u8"\xe1\x8e\xa3\0"                                     // #2068: [AB73] ==> [13A3]
      u8"\xe1\x8e\xa4\0"                                     // #2069: [AB74] ==> [13A4]
      u8"\xe1\x8e\xa5\0"                                     // #2070: [AB75] ==> [13A5]
      u8"\xe1\x8e\xa6\0"                                     // #2071: [AB76] ==> [13A6]
      u8"\xe1\x8e\xa7\0"                                     // #2072: [AB77] ==> [13A7]
      u8"\xe1\x8e\xa8\0"                                     // #2073: [AB78] ==> [13A8]
      u8"\xe1\x8e\xa9\0"                                     // #2074: [AB79] ==> [13A9]
      u8"\xe1\x8e\xaa\0"                                     // #2075: [AB7A] ==> [13AA]
      u8"\xe1\x8e\xab\0"                                     // #2076: [AB7B] ==> [13AB]
      u8"\xe1\x8e\xac\0"                                     // #2077: [AB7C] ==> [13AC]
      u8"\xe1\x8e\xad\0"                                     // #2078: [AB7D] ==> [13AD]
      u8"\xe1\x8e\xae\0"                                     // #2079: [AB7E] ==> [13AE]
      u8"\xe1\x8e\xaf\0"                                     // #2080: [AB7F] ==> [13AF]
      u8"\xe1\x8e\xb0\0"                                     // #2081: [AB80] ==> [13B0]
      u8"\xe1\x8e\xb1\0"                                     // #2082: [AB81] ==> [13B1]
      u8"\xe1\x8e\xb2\0"                                     // #2083: [AB82] ==> [13B2]
      u8"\xe1\x8e\xb3\0"                                     // #2084: [AB83] ==> [13B3]
      u8"\xe1\x8e\xb4\0"                                     // #2085: [AB84] ==> [13B4]
      u8"\xe1\x8e\xb5\0"                                     // #2086: [AB85] ==> [13B5]
      u8"\xe1\x8e\xb6\0"                                     // #2087: [AB86] ==> [13B6]
      u8"\xe1\x8e\xb7\0"                                     // #2088: [AB87] ==> [13B7]
      u8"\xe1\x8e\xb8\0"                                     // #2089: [AB88] ==> [13B8]
      u8"\xe1\x8e\xb9\0"                                     // #2090: [AB89] ==> [13B9]
      u8"\xe1\x8e\xba\0"                                     // #2091: [AB8A] ==> [13BA]
      u8"\xe1\x8e\xbb\0"                                     // #2092: [AB8B] ==> [13BB]
      u8"\xe1\x8e\xbc\0"                                     // #2093: [AB8C] ==> [13BC]
      u8"\xe1\x8e\xbd\0"                                     // #2094: [AB8D] ==> [13BD]
      u8"\xe1\x8e\xbe\0"                                     // #2095: [AB8E] ==> [13BE]
      u8"\xe1\x8e\xbf\0"                                     // #2096: [AB8F] ==> [13BF]
      u8"\xe1\x8f\x80\0"                                     // #2097: [AB90] ==> [13C0]
      u8"\xe1\x8f\x81\0"                                     // #2098: [AB91] ==> [13C1]
      u8"\xe1\x8f\x82\0"                                     // #2099: [AB92] ==> [13C2]
      u8"\xe1\x8f\x83\0"                                     // #2100: [AB93] ==> [13C3]
      u8"\xe1\x8f\x84\0"                                     // #2101: [AB94] ==> [13C4]
      u8"\xe1\x8f\x85\0"                                     // #2102: [AB95] ==> [13C5]
      u8"\xe1\x8f\x86\0"                                     // #2103: [AB96] ==> [13C6]
      u8"\xe1\x8f\x87\0"                                     // #2104: [AB97] ==> [13C7]
      u8"\xe1\x8f\x88\0"                                     // #2105: [AB98] ==> [13C8]
      u8"\xe1\x8f\x89\0"                                     // #2106: [AB99] ==> [13C9]
      u8"\xe1\x8f\x8a\0"                                     // #2107: [AB9A] ==> [13CA]
      u8"\xe1\x8f\x8b\0"                                     // #2108: [AB9B] ==> [13CB]
      u8"\xe1\x8f\x8c\0"                                     // #2109: [AB9C] ==> [13CC]
      u8"\xe1\x8f\x8d\0"                                     // #2110: [AB9D] ==> [13CD]
      u8"\xe1\x8f\x8e\0"                                     // #2111: [AB9E] ==> [13CE]
      u8"\xe1\x8f\x8f\0"                                     // #2112: [AB9F] ==> [13CF]
      u8"\xe1\x8f\x90\0"                                     // #2113: [ABA0] ==> [13D0]
      u8"\xe1\x8f\x91\0"                                     // #2114: [ABA1] ==> [13D1]
      u8"\xe1\x8f\x92\0"                                     // #2115: [ABA2] ==> [13D2]
      u8"\xe1\x8f\x93\0"                                     // #2116: [ABA3] ==> [13D3]
      u8"\xe1\x8f\x94\0"                                     // #2117: [ABA4] ==> [13D4]
      u8"\xe1\x8f\x95\0"                                     // #2118: [ABA5] ==> [13D5]
      u8"\xe1\x8f\x96\0"                                     // #2119: [ABA6] ==> [13D6]
      u8"\xe1\x8f\x97\0"                                     // #2120: [ABA7] ==> [13D7]
      u8"\xe1\x8f\x98\0"                                     // #2121: [ABA8] ==> [13D8]
      u8"\xe1\x8f\x99\0"                                     // #2122: [ABA9] ==> [13D9]
      u8"\xe1\x8f\x9a\0"                                     // #2123: [ABAA] ==> [13DA]
      u8"\xe1\x8f\x9b\0"                                     // #2124: [ABAB] ==> [13DB]
      u8"\xe1\x8f\x9c\0"                                     // #2125: [ABAC] ==> [13DC]
      u8"\xe1\x8f\x9d\0"                                     // #2126: [ABAD] ==> [13DD]
      u8"\xe1\x8f\x9e\0"                                     // #2127: [ABAE] ==> [13DE]
      u8"\xe1\x8f\x9f\0"                                     // #2128: [ABAF] ==> [13DF]
      u8"\xe1\x8f\xa0\0"                                     // #2129: [ABB0] ==> [13E0]
      u8"\xe1\x8f\xa1\0"                                     // #2130: [ABB1] ==> [13E1]
      u8"\xe1\x8f\xa2\0"                                     // #2131: [ABB2] ==> [13E2]
      u8"\xe1\x8f\xa3\0"                                     // #2132: [ABB3] ==> [13E3]
      u8"\xe1\x8f\xa4\0"                                     // #2133: [ABB4] ==> [13E4]
      u8"\xe1\x8f\xa5\0"                                     // #2134: [ABB5] ==> [13E5]
      u8"\xe1\x8f\xa6\0"                                     // #2135: [ABB6] ==> [13E6]
      u8"\xe1\x8f\xa7\0"                                     // #2136: [ABB7] ==> [13E7]
      u8"\xe1\x8f\xa8\0"                                     // #2137: [ABB8] ==> [13E8]
      u8"\xe1\x8f\xa9\0"                                     // #2138: [ABB9] ==> [13E9]
      u8"\xe1\x8f\xaa\0"                                     // #2139: [ABBA] ==> [13EA]
      u8"\xe1\x8f\xab\0"                                     // #2140: [ABBB] ==> [13EB]
      u8"\xe1\x8f\xac\0"                                     // #2141: [ABBC] ==> [13EC]
      u8"\xe1\x8f\xad\0"                                     // #2142: [ABBD] ==> [13ED]
      u8"\xe1\x8f\xae\0"                                     // #2143: [ABBE] ==> [13EE]
      u8"\xe1\x8f\xaf\0"                                     // #2144: [ABBF] ==> [13EF]
      u8"\xe8\xb1\x88\0"                                     // #2145: [F900] ==> [8C48]
      u8"\xe6\x9b\xb4\0"                                     // #2146: [F901] ==> [66F4]
      u8"\xe8\xb3\x88\0"                                     // #2147: [F903] ==> [8CC8]
      u8"\xe6\xbb\x91\0"                                     // #2148: [F904] ==> [6ED1]
      u8"\xe4\xb8\xb2\0"                                     // #2149: [F905] ==> [4E32]
      u8"\xe5\x8f\xa5\0"                                     // #2150: [F906] ==> [53E5]
      u8"\xe5\xa5\x91\0"                                     // #2151: [F909] ==> [5951]
      u8"\xe5\x96\x87\0"                                     // #2152: [F90B] ==> [5587]
      u8"\xe5\xa5\x88\0"                                     // #2153: [F90C] ==> [5948]
      u8"\xe6\x87\xb6\0"                                     // #2154: [F90D, 2F8B1] ==> [61F6]
      u8"\xe7\x99\xa9\0"                                     // #2155: [F90E] ==> [7669]
      u8"\xe7\xbe\x85\0"                                     // #2156: [F90F] ==> [7F85]
      u8"\xe8\x98\xbf\0"                                     // #2157: [F910] ==> [863F]
      u8"\xe8\x9e\xba\0"                                     // #2158: [F911] ==> [87BA]
      u8"\xe8\xa3\xb8\0"                                     // #2159: [F912] ==> [88F8]
      u8"\xe9\x82\x8f\0"                                     // #2160: [F913] ==> [908F]
      u8"\xe6\xa8\x82\0"                                     // #2161: [F914, F95C, F9BF] ==> [6A02]
      u8"\xe6\xb4\x9b\0"                                     // #2162: [F915] ==> [6D1B]
      u8"\xe7\x83\x99\0"                                     // #2163: [F916] ==> [70D9]
      u8"\xe7\x8f\x9e\0"                                     // #2164: [F917] ==> [73DE]
      u8"\xe8\x90\xbd\0"                                     // #2165: [F918] ==> [843D]
      u8"\xe9\x85\xaa\0"                                     // #2166: [F919] ==> [916A]
      u8"\xe9\xa7\xb1\0"                                     // #2167: [F91A] ==> [99F1]
      u8"\xe4\xba\x82\0"                                     // #2168: [F91B] ==> [4E82]
      u8"\xe5\x8d\xb5\0"                                     // #2169: [F91C] ==> [5375]
      u8"\xe6\xac\x84\0"                                     // #2170: [F91D] ==> [6B04]
      u8"\xe7\x88\x9b\0"                                     // #2171: [F91E] ==> [721B]
      u8"\xe8\x98\xad\0"                                     // #2172: [F91F] ==> [862D]
      u8"\xe9\xb8\x9e\0"                                     // #2173: [F920] ==> [9E1E]
      u8"\xe5\xb5\x90\0"                                     // #2174: [F921] ==> [5D50]
      u8"\xe6\xbf\xab\0"                                     // #2175: [F922] ==> [6FEB]
      u8"\xe8\x97\x8d\0"                                     // #2176: [F923] ==> [85CD]
      u8"\xe8\xa5\xa4\0"                                     // #2177: [F924] ==> [8964]
      u8"\xe6\x8b\x89\0"                                     // #2178: [F925] ==> [62C9]
      u8"\xe8\x87\x98\0"                                     // #2179: [F926] ==> [81D8]
      u8"\xe8\xa0\x9f\0"                                     // #2180: [F927] ==> [881F]
      u8"\xe5\xbb\x8a\0"                                     // #2181: [F928, 2F88E] ==> [5ECA]
      u8"\xe6\x9c\x97\0"                                     // #2182: [F929, FA92, 2F8D8] ==> [6717]
      u8"\xe6\xb5\xaa\0"                                     // #2183: [F92A] ==> [6D6A]
      u8"\xe7\x8b\xbc\0"                                     // #2184: [F92B] ==> [72FC]
      u8"\xe9\x83\x8e\0"                                     // #2185: [F92C] ==> [90CE]
      u8"\xe4\xbe\x86\0"                                     // #2186: [F92D] ==> [4F86]
      u8"\xe5\x86\xb7\0"                                     // #2187: [F92E] ==> [51B7]
      u8"\xe5\x8b\x9e\0"                                     // #2188: [F92F] ==> [52DE]
      u8"\xe6\x93\x84\0"                                     // #2189: [F930] ==> [64C4]
      u8"\xe6\xab\x93\0"                                     // #2190: [F931] ==> [6AD3]
      u8"\xe7\x88\x90\0"                                     // #2191: [F932] ==> [7210]
      u8"\xe7\x9b\xa7\0"                                     // #2192: [F933] ==> [76E7]
      u8"\xe8\x98\x86\0"                                     // #2193: [F935] ==> [8606]
      u8"\xe8\x99\x9c\0"                                     // #2194: [F936, 2F9B4] ==> [865C]
      u8"\xe8\xb7\xaf\0"                                     // #2195: [F937] ==> [8DEF]
      u8"\xe9\x9c\xb2\0"                                     // #2196: [F938] ==> [9732]
      u8"\xe9\xad\xaf\0"                                     // #2197: [F939] ==> [9B6F]
      u8"\xe9\xb7\xba\0"                                     // #2198: [F93A] ==> [9DFA]
      u8"\xe7\xa2\x8c\0"                                     // #2199: [F93B, 2F94F] ==> [788C]
      u8"\xe7\xa5\xbf\0"                                     // #2200: [F93C] ==> [797F]
      u8"\xe7\xb6\xa0\0"                                     // #2201: [F93D] ==> [7DA0]
      u8"\xe8\x8f\x89\0"                                     // #2202: [F93E] ==> [83C9]
      u8"\xe9\x8c\x84\0"                                     // #2203: [F93F] ==> [9304]
      u8"\xe8\xab\x96\0"                                     // #2204: [F941] ==> [8AD6]
      u8"\xe5\xa3\x9f\0"                                     // #2205: [F942] ==> [58DF]
      u8"\xe5\xbc\x84\0"                                     // #2206: [F943] ==> [5F04]
      u8"\xe7\xb1\xa0\0"                                     // #2207: [F944] ==> [7C60]
      u8"\xe8\x81\xbe\0"                                     // #2208: [F945] ==> [807E]
      u8"\xe7\x89\xa2\0"                                     // #2209: [F946] ==> [7262]
      u8"\xe7\xa3\x8a\0"                                     // #2210: [F947] ==> [78CA]
      u8"\xe8\xb3\x82\0"                                     // #2211: [F948] ==> [8CC2]
      u8"\xe9\x9b\xb7\0"                                     // #2212: [F949] ==> [96F7]
      u8"\xe5\xa3\x98\0"                                     // #2213: [F94A] ==> [58D8]
      u8"\xe5\xb1\xa2\0"                                     // #2214: [F94B] ==> [5C62]
      u8"\xe6\xa8\x93\0"                                     // #2215: [F94C] ==> [6A13]
      u8"\xe6\xb7\x9a\0"                                     // #2216: [F94D] ==> [6DDA]
      u8"\xe6\xbc\x8f\0"                                     // #2217: [F94E] ==> [6F0F]
      u8"\xe7\xb4\xaf\0"                                     // #2218: [F94F] ==> [7D2F]
      u8"\xe7\xb8\xb7\0"                                     // #2219: [F950] ==> [7E37]
      u8"\xe9\x99\x8b\0"                                     // #2220: [F951] ==> [964B]
      u8"\xe5\x8b\x92\0"                                     // #2221: [F952] ==> [52D2]
      u8"\xe8\x82\x8b\0"                                     // #2222: [F953] ==> [808B]
      u8"\xe5\x87\x9c\0"                                     // #2223: [F954] ==> [51DC]
      u8"\xe5\x87\x8c\0"                                     // #2224: [F955] ==> [51CC]
      u8"\xe7\xa8\x9c\0"                                     // #2225: [F956] ==> [7A1C]
      u8"\xe7\xb6\xbe\0"                                     // #2226: [F957] ==> [7DBE]
      u8"\xe8\x8f\xb1\0"                                     // #2227: [F958] ==> [83F1]
      u8"\xe9\x99\xb5\0"                                     // #2228: [F959] ==> [9675]
      u8"\xe8\xae\x80\0"                                     // #2229: [F95A] ==> [8B80]
      u8"\xe6\x8b\x8f\0"                                     // #2230: [F95B] ==> [62CF]
      u8"\xe8\xab\xbe\0"                                     // #2231: [F95D, FABD] ==> [8AFE]
      u8"\xe4\xb8\xb9\0"                                     // #2232: [F95E] ==> [4E39]
      u8"\xe5\xaf\xa7\0"                                     // #2233: [F95F, F9AA, 2F86F] ==> [5BE7]
      u8"\xe6\x80\x92\0"                                     // #2234: [F960] ==> [6012]
      u8"\xe7\x8e\x87\0"                                     // #2235: [F961, F9DB] ==> [7387]
      u8"\xe7\x95\xb0\0"                                     // #2236: [F962, 2F938] ==> [7570]
      u8"\xe5\x8c\x97\0"                                     // #2237: [F963, 2F82B] ==> [5317]
      u8"\xe7\xa3\xbb\0"                                     // #2238: [F964] ==> [78FB]
      u8"\xe4\xbe\xbf\0"                                     // #2239: [F965] ==> [4FBF]
      u8"\xe5\xbe\xa9\0"                                     // #2240: [F966] ==> [5FA9]
      u8"\xe4\xb8\x8d\0"                                     // #2241: [F967] ==> [4E0D]
      u8"\xe6\xb3\x8c\0"                                     // #2242: [F968] ==> [6CCC]
      u8"\xe6\x95\xb8\0"                                     // #2243: [F969] ==> [6578]
      u8"\xe7\xb4\xa2\0"                                     // #2244: [F96A] ==> [7D22]
      u8"\xe5\x8f\x83\0"                                     // #2245: [F96B] ==> [53C3]
      u8"\xe5\xa1\x9e\0"                                     // #2246: [F96C] ==> [585E]
      u8"\xe7\x9c\x81\0"                                     // #2247: [F96D] ==> [7701]
      u8"\xe8\x91\x89\0"                                     // #2248: [F96E] ==> [8449]
      u8"\xe8\xaa\xaa\0"                                     // #2249: [F96F, F9A1] ==> [8AAA]
      u8"\xe6\xae\xba\0"                                     // #2250: [F970, FA96, 2F8F5] ==> [6BBA]
      u8"\xe6\xb2\x88\0"                                     // #2251: [F972] ==> [6C88]
      u8"\xe6\x8b\xbe\0"                                     // #2252: [F973] ==> [62FE]
      u8"\xe8\x8b\xa5\0"                                     // #2253: [F974, 2F998] ==> [82E5]
      u8"\xe6\x8e\xa0\0"                                     // #2254: [F975] ==> [63A0]
      u8"\xe7\x95\xa5\0"                                     // #2255: [F976] ==> [7565]
      u8"\xe4\xba\xae\0"                                     // #2256: [F977] ==> [4EAE]
      u8"\xe5\x85\xa9\0"                                     // #2257: [F978] ==> [5169]
      u8"\xe5\x87\x89\0"                                     // #2258: [F979] ==> [51C9]
      u8"\xe6\xa2\x81\0"                                     // #2259: [F97A] ==> [6881]
      u8"\xe7\xb3\xa7\0"                                     // #2260: [F97B] ==> [7CE7]
      u8"\xe8\x89\xaf\0"                                     // #2261: [F97C] ==> [826F]
      u8"\xe8\xab\x92\0"                                     // #2262: [F97D] ==> [8AD2]
      u8"\xe9\x87\x8f\0"                                     // #2263: [F97E] ==> [91CF]
      u8"\xe5\x8b\xb5\0"                                     // #2264: [F97F] ==> [52F5]
      u8"\xe5\x91\x82\0"                                     // #2265: [F980] ==> [5442]
      u8"\xe5\xbb\xac\0"                                     // #2266: [F982] ==> [5EEC]
      u8"\xe6\x97\x85\0"                                     // #2267: [F983] ==> [65C5]
      u8"\xe6\xbf\xbe\0"                                     // #2268: [F984] ==> [6FFE]
      u8"\xe7\xa4\xaa\0"                                     // #2269: [F985] ==> [792A]
      u8"\xe9\x96\xad\0"                                     // #2270: [F986] ==> [95AD]
      u8"\xe9\xa9\xaa\0"                                     // #2271: [F987] ==> [9A6A]
      u8"\xe9\xba\x97\0"                                     // #2272: [F988] ==> [9E97]
      u8"\xe9\xbb\x8e\0"                                     // #2273: [F989] ==> [9ECE]
      u8"\xe6\x9b\x86\0"                                     // #2274: [F98B] ==> [66C6]
      u8"\xe6\xad\xb7\0"                                     // #2275: [F98C] ==> [6B77]
      u8"\xe8\xbd\xa2\0"                                     // #2276: [F98D] ==> [8F62]
      u8"\xe5\xb9\xb4\0"                                     // #2277: [F98E] ==> [5E74]
      u8"\xe6\x86\x90\0"                                     // #2278: [F98F] ==> [6190]
      u8"\xe6\x88\x80\0"                                     // #2279: [F990] ==> [6200]
      u8"\xe6\x92\x9a\0"                                     // #2280: [F991] ==> [649A]
      u8"\xe6\xbc\xa3\0"                                     // #2281: [F992] ==> [6F23]
      u8"\xe7\x85\x89\0"                                     // #2282: [F993] ==> [7149]
      u8"\xe7\x92\x89\0"                                     // #2283: [F994] ==> [7489]
      u8"\xe7\xa7\x8a\0"                                     // #2284: [F995] ==> [79CA]
      u8"\xe7\xb7\xb4\0"                                     // #2285: [F996, FA57, FAB0] ==> [7DF4]
      u8"\xe8\x81\xaf\0"                                     // #2286: [F997] ==> [806F]
      u8"\xe8\xbc\xa6\0"                                     // #2287: [F998] ==> [8F26]
      u8"\xe8\x93\xae\0"                                     // #2288: [F999] ==> [84EE]
      u8"\xe9\x80\xa3\0"                                     // #2289: [F99A] ==> [9023]
      u8"\xe9\x8d\x8a\0"                                     // #2290: [F99B] ==> [934A]
      u8"\xe5\x88\x97\0"                                     // #2291: [F99C] ==> [5217]
      u8"\xe5\x8a\xa3\0"                                     // #2292: [F99D] ==> [52A3]
      u8"\xe5\x92\xbd\0"                                     // #2293: [F99E] ==> [54BD]
      u8"\xe7\x83\x88\0"                                     // #2294: [F99F] ==> [70C8]
      u8"\xe8\xa3\x82\0"                                     // #2295: [F9A0] ==> [88C2]
      u8"\xe5\xbb\x89\0"                                     // #2296: [F9A2] ==> [5EC9]
      u8"\xe5\xbf\xb5\0"                                     // #2297: [F9A3] ==> [5FF5]
      u8"\xe6\x8d\xbb\0"                                     // #2298: [F9A4] ==> [637B]
      u8"\xe6\xae\xae\0"                                     // #2299: [F9A5] ==> [6BAE]
      u8"\xe7\xb0\xbe\0"                                     // #2300: [F9A6] ==> [7C3E]
      u8"\xe7\x8d\xb5\0"                                     // #2301: [F9A7] ==> [7375]
      u8"\xe4\xbb\xa4\0"                                     // #2302: [F9A8] ==> [4EE4]
      u8"\xe5\x9b\xb9\0"                                     // #2303: [F9A9] ==> [56F9]
      u8"\xe5\xb6\xba\0"                                     // #2304: [F9AB] ==> [5DBA]
      u8"\xe6\x80\x9c\0"                                     // #2305: [F9AC] ==> [601C]
      u8"\xe7\x8e\xb2\0"                                     // #2306: [F9AD] ==> [73B2]
      u8"\xe7\x91\xa9\0"                                     // #2307: [F9AE] ==> [7469]
      u8"\xe7\xbe\x9a\0"                                     // #2308: [F9AF] ==> [7F9A]
      u8"\xe8\x81\x86\0"                                     // #2309: [F9B0] ==> [8046]
      u8"\xe9\x88\xb4\0"                                     // #2310: [F9B1] ==> [9234]
      u8"\xe9\x9b\xb6\0"                                     // #2311: [F9B2] ==> [96F6]
      u8"\xe9\x9d\x88\0"                                     // #2312: [F9B3] ==> [9748]
      u8"\xe9\xa0\x98\0"                                     // #2313: [F9B4] ==> [9818]
      u8"\xe4\xbe\x8b\0"                                     // #2314: [F9B5] ==> [4F8B]
      u8"\xe7\xa6\xae\0"                                     // #2315: [F9B6] ==> [79AE]
      u8"\xe9\x86\xb4\0"                                     // #2316: [F9B7] ==> [91B4]
      u8"\xe9\x9a\xb8\0"                                     // #2317: [F9B8] ==> [96B8]
      u8"\xe6\x83\xa1\0"                                     // #2318: [F9B9] ==> [60E1]
      u8"\xe4\xba\x86\0"                                     // #2319: [F9BA] ==> [4E86]
      u8"\xe5\x83\x9a\0"                                     // #2320: [F9BB] ==> [50DA]
      u8"\xe5\xaf\xae\0"                                     // #2321: [F9BC] ==> [5BEE]
      u8"\xe5\xb0\xbf\0"                                     // #2322: [F9BD] ==> [5C3F]
      u8"\xe6\x96\x99\0"                                     // #2323: [F9BE, 1F21B] ==> [6599]
      u8"\xe7\x87\x8e\0"                                     // #2324: [F9C0] ==> [71CE]
      u8"\xe7\x99\x82\0"                                     // #2325: [F9C1] ==> [7642]
      u8"\xe8\x93\xbc\0"                                     // #2326: [F9C2] ==> [84FC]
      u8"\xe9\x81\xbc\0"                                     // #2327: [F9C3] ==> [907C]
      u8"\xe6\x9a\x88\0"                                     // #2328: [F9C5] ==> [6688]
      u8"\xe9\x98\xae\0"                                     // #2329: [F9C6] ==> [962E]
      u8"\xe5\x8a\x89\0"                                     // #2330: [F9C7] ==> [5289]
      u8"\xe6\x9d\xbb\0"                                     // #2331: [F9C8] ==> [677B]
      u8"\xe6\x9f\xb3\0"                                     // #2332: [F9C9] ==> [67F3]
      u8"\xe6\xb5\x81\0"                                     // #2333: [F9CA, FA97, 2F902] ==> [6D41]
      u8"\xe6\xba\x9c\0"                                     // #2334: [F9CB] ==> [6E9C]
      u8"\xe7\x90\x89\0"                                     // #2335: [F9CC] ==> [7409]
      u8"\xe7\x95\x99\0"                                     // #2336: [F9CD] ==> [7559]
      u8"\xe7\xa1\xab\0"                                     // #2337: [F9CE] ==> [786B]
      u8"\xe7\xb4\x90\0"                                     // #2338: [F9CF] ==> [7D10]
      u8"\xe9\xa1\x9e\0"                                     // #2339: [F9D0] ==> [985E]
      u8"\xe6\x88\xae\0"                                     // #2340: [F9D2] ==> [622E]
      u8"\xe9\x99\xb8\0"                                     // #2341: [F9D3] ==> [9678]
      u8"\xe5\x80\xab\0"                                     // #2342: [F9D4] ==> [502B]
      u8"\xe5\xb4\x99\0"                                     // #2343: [F9D5] ==> [5D19]
      u8"\xe6\xb7\xaa\0"                                     // #2344: [F9D6] ==> [6DEA]
      u8"\xe8\xbc\xaa\0"                                     // #2345: [F9D7] ==> [8F2A]
      u8"\xe5\xbe\x8b\0"                                     // #2346: [F9D8] ==> [5F8B]
      u8"\xe6\x85\x84\0"                                     // #2347: [F9D9] ==> [6144]
      u8"\xe6\xa0\x97\0"                                     // #2348: [F9DA] ==> [6817]
      u8"\xe9\x9a\x86\0"                                     // #2349: [F9DC] ==> [9686]
      u8"\xe5\x88\xa9\0"                                     // #2350: [F9DD] ==> [5229]
      u8"\xe5\x90\x8f\0"                                     // #2351: [F9DE] ==> [540F]
      u8"\xe5\xb1\xa5\0"                                     // #2352: [F9DF] ==> [5C65]
      u8"\xe6\x98\x93\0"                                     // #2353: [F9E0] ==> [6613]
      u8"\xe6\x9d\x8e\0"                                     // #2354: [F9E1] ==> [674E]
      u8"\xe6\xa2\xa8\0"                                     // #2355: [F9E2] ==> [68A8]
      u8"\xe6\xb3\xa5\0"                                     // #2356: [F9E3] ==> [6CE5]
      u8"\xe7\x90\x86\0"                                     // #2357: [F9E4] ==> [7406]
      u8"\xe7\x97\xa2\0"                                     // #2358: [F9E5] ==> [75E2]
      u8"\xe7\xbd\xb9\0"                                     // #2359: [F9E6] ==> [7F79]
      u8"\xe8\xa3\x8f\0"                                     // #2360: [F9E7] ==> [88CF]
      u8"\xe8\xa3\xa1\0"                                     // #2361: [F9E8] ==> [88E1]
      u8"\xe9\x9b\xa2\0"                                     // #2362: [F9EA] ==> [96E2]
      u8"\xe5\x8c\xbf\0"                                     // #2363: [F9EB] ==> [533F]
      u8"\xe6\xba\xba\0"                                     // #2364: [F9EC] ==> [6EBA]
      u8"\xe5\x90\x9d\0"                                     // #2365: [F9ED] ==> [541D]
      u8"\xe7\x87\x90\0"                                     // #2366: [F9EE] ==> [71D0]
      u8"\xe7\x92\x98\0"                                     // #2367: [F9EF] ==> [7498]
      u8"\xe8\x97\xba\0"                                     // #2368: [F9F0] ==> [85FA]
      u8"\xe9\x9a\xa3\0"                                     // #2369: [F9F1] ==> [96A3]
      u8"\xe9\xb1\x97\0"                                     // #2370: [F9F2] ==> [9C57]
      u8"\xe9\xba\x9f\0"                                     // #2371: [F9F3] ==> [9E9F]
      u8"\xe6\x9e\x97\0"                                     // #2372: [F9F4] ==> [6797]
      u8"\xe6\xb7\x8b\0"                                     // #2373: [F9F5] ==> [6DCB]
      u8"\xe8\x87\xa8\0"                                     // #2374: [F9F6] ==> [81E8]
      u8"\xe7\xac\xa0\0"                                     // #2375: [F9F8] ==> [7B20]
      u8"\xe7\xb2\x92\0"                                     // #2376: [F9F9] ==> [7C92]
      u8"\xe7\x8b\x80\0"                                     // #2377: [F9FA] ==> [72C0]
      u8"\xe7\x82\x99\0"                                     // #2378: [F9FB] ==> [7099]
      u8"\xe8\xad\x98\0"                                     // #2379: [F9FC] ==> [8B58]
      u8"\xe4\xbb\x80\0"                                     // #2380: [F9FD] ==> [4EC0]
      u8"\xe8\x8c\xb6\0"                                     // #2381: [F9FE] ==> [8336]
      u8"\xe5\x88\xba\0"                                     // #2382: [F9FF] ==> [523A]
      u8"\xe5\x88\x87\0"                                     // #2383: [FA00, 2F850] ==> [5207]
      u8"\xe5\xba\xa6\0"                                     // #2384: [FA01] ==> [5EA6]
      u8"\xe6\x8b\x93\0"                                     // #2385: [FA02] ==> [62D3]
      u8"\xe7\xb3\x96\0"                                     // #2386: [FA03] ==> [7CD6]
      u8"\xe5\xae\x85\0"                                     // #2387: [FA04] ==> [5B85]
      u8"\xe6\xb4\x9e\0"                                     // #2388: [FA05] ==> [6D1E]
      u8"\xe6\x9a\xb4\0"                                     // #2389: [FA06] ==> [66B4]
      u8"\xe8\xbc\xbb\0"                                     // #2390: [FA07] ==> [8F3B]
      u8"\xe9\x99\x8d\0"                                     // #2391: [FA09] ==> [964D]
      u8"\xe5\xbb\x93\0"                                     // #2392: [FA0B] ==> [5ED3]
      u8"\xe5\x85\x80\0"                                     // #2393: [FA0C] ==> [5140]
      u8"\xe5\x97\x80\0"                                     // #2394: [FA0D] ==> [55C0]
      u8"\xe5\xa1\x9a\0"                                     // #2395: [FA10, FA7C] ==> [585A]
      u8"\xe6\x99\xb4\0"                                     // #2396: [FA12, FA91] ==> [6674]
      u8"\xe5\x87\x9e\0"                                     // #2397: [FA15] ==> [51DE]
      u8"\xe7\x8c\xaa\0"                                     // #2398: [FA16, FAA0] ==> [732A]
      u8"\xe7\x9b\x8a\0"                                     // #2399: [FA17, FAA6] ==> [76CA]
      u8"\xe7\xa4\xbc\0"                                     // #2400: [FA18] ==> [793C]
      u8"\xe7\xa5\x9e\0"                                     // #2401: [FA19] ==> [795E]
      u8"\xe7\xa5\xa5\0"                                     // #2402: [FA1A] ==> [7965]
      u8"\xe7\xa6\x8f\0"                                     // #2403: [FA1B, 2F956] ==> [798F]
      u8"\xe9\x9d\x96\0"                                     // #2404: [FA1C, FAC8] ==> [9756]
      u8"\xe7\xb2\xbe\0"                                     // #2405: [FA1D] ==> [7CBE]
      u8"\xe8\x98\x92\0"                                     // #2406: [FA20] ==> [8612]
      u8"\xe8\xab\xb8\0"                                     // #2407: [FA22, FABA] ==> [8AF8]
      u8"\xe9\x80\xb8\0"                                     // #2408: [FA25, FA67] ==> [9038]
      u8"\xe9\x83\xbd\0"                                     // #2409: [FA26] ==> [90FD]
      u8"\xe9\xa3\xaf\0"                                     // #2410: [FA2A] ==> [98EF]
      u8"\xe9\xa3\xbc\0"                                     // #2411: [FA2B] ==> [98FC]
      u8"\xe9\xa4\xa8\0"                                     // #2412: [FA2C] ==> [9928]
      u8"\xe9\xb6\xb4\0"                                     // #2413: [FA2D] ==> [9DB4]
      u8"\xe9\x83\x9e\0"                                     // #2414: [FA2E] ==> [90DE]
      u8"\xe9\x9a\xb7\0"                                     // #2415: [FA2F] ==> [96B7]
      u8"\xe4\xbe\xae\0"                                     // #2416: [FA30, 2F805] ==> [4FAE]
      u8"\xe5\x83\xa7\0"                                     // #2417: [FA31, 2F80A] ==> [50E7]
      u8"\xe5\x85\x8d\0"                                     // #2418: [FA32, 2F80E] ==> [514D]
      u8"\xe5\x8b\x89\0"                                     // #2419: [FA33, 2F826] ==> [52C9]
      u8"\xe5\x8b\xa4\0"                                     // #2420: [FA34, 2F827] ==> [52E4]
      u8"\xe5\x8d\x91\0"                                     // #2421: [FA35, 2F82D] ==> [5351]
      u8"\xe5\x96\x9d\0"                                     // #2422: [FA36, FA78] ==> [559D]
      u8"\xe5\x98\x86\0"                                     // #2423: [FA37, 2F84C] ==> [5606]
      u8"\xe5\x99\xa8\0"                                     // #2424: [FA38] ==> [5668]
      u8"\xe5\xa1\x80\0"                                     // #2425: [FA39] ==> [5840]
      u8"\xe5\xa2\xa8\0"                                     // #2426: [FA3A] ==> [58A8]
      u8"\xe5\xb1\xa4\0"                                     // #2427: [FA3B] ==> [5C64]
      u8"\xe6\x82\x94\0"                                     // #2428: [FA3D, 2F8A3] ==> [6094]
      u8"\xe6\x85\xa8\0"                                     // #2429: [FA3E] ==> [6168]
      u8"\xe6\x86\x8e\0"                                     // #2430: [FA3F, FA89, 2F8AB] ==> [618E]
      u8"\xe6\x87\xb2\0"                                     // #2431: [FA40, FA8B, 2F8B0] ==> [61F2]
      u8"\xe6\x95\x8f\0"                                     // #2432: [FA41, 2F8C8] ==> [654F]
      u8"\xe6\x97\xa2\0"                                     // #2433: [FA42] ==> [65E2]
      u8"\xe6\x9a\x91\0"                                     // #2434: [FA43, 2F8CF] ==> [6691]
      u8"\xe6\xa2\x85\0"                                     // #2435: [FA44, 2F8E2] ==> [6885]
      u8"\xe6\xb5\xb7\0"                                     // #2436: [FA45, 2F901] ==> [6D77]
      u8"\xe6\xb8\x9a\0"                                     // #2437: [FA46] ==> [6E1A]
      u8"\xe6\xbc\xa2\0"                                     // #2438: [FA47, FA9A] ==> [6F22]
      u8"\xe7\x85\xae\0"                                     // #2439: [FA48, FA9C] ==> [716E]
      u8"\xe7\x88\xab\0"                                     // #2440: [FA49] ==> [722B]
      u8"\xe7\x90\xa2\0"                                     // #2441: [FA4A] ==> [7422]
      u8"\xe7\xa2\x91\0"                                     // #2442: [FA4B] ==> [7891]
      u8"\xe7\xa5\x89\0"                                     // #2443: [FA4D] ==> [7949]
      u8"\xe7\xa5\x88\0"                                     // #2444: [FA4E] ==> [7948]
      u8"\xe7\xa5\x90\0"                                     // #2445: [FA4F] ==> [7950]
      u8"\xe7\xa5\x96\0"                                     // #2446: [FA50, 2F953] ==> [7956]
      u8"\xe7\xa6\x8d\0"                                     // #2447: [FA52] ==> [798D]
      u8"\xe7\xa6\x8e\0"                                     // #2448: [FA53] ==> [798E]
      u8"\xe7\xa9\x80\0"                                     // #2449: [FA54, 2F959] ==> [7A40]
      u8"\xe7\xaa\x81\0"                                     // #2450: [FA55] ==> [7A81]
      u8"\xe7\xaf\x80\0"                                     // #2451: [FA56, FAAD] ==> [7BC0]
      u8"\xe7\xb8\x89\0"                                     // #2452: [FA58] ==> [7E09]
      u8"\xe7\xb9\x81\0"                                     // #2453: [FA59] ==> [7E41]
      u8"\xe7\xbd\xb2\0"                                     // #2454: [FA5A] ==> [7F72]
      u8"\xe8\x80\x85\0"                                     // #2455: [FA5B, FAB2, 2F97A] ==> [8005]
      u8"\xe8\x87\xad\0"                                     // #2456: [FA5C] ==> [81ED]
      u8"\xe8\x89\xb9\0"                                     // #2457: [FA5D, FA5E] ==> [8279]
      u8"\xe8\x91\x97\0"                                     // #2458: [FA5F, 2F99F] ==> [8457]
      u8"\xe8\xa4\x90\0"                                     // #2459: [FA60] ==> [8910]
      u8"\xe8\xa6\x96\0"                                     // #2460: [FA61, FAB8] ==> [8996]
      u8"\xe8\xac\x81\0"                                     // #2461: [FA62, FABC] ==> [8B01]
      u8"\xe8\xac\xb9\0"                                     // #2462: [FA63, FABF] ==> [8B39]
      u8"\xe8\xb3\x93\0"                                     // #2463: [FA64] ==> [8CD3]
      u8"\xe8\xb4\x88\0"                                     // #2464: [FA65, FAC1] ==> [8D08]
      u8"\xe8\xbe\xb6\0"                                     // #2465: [FA66] ==> [8FB6]
      u8"\xe9\x9b\xa3\0"                                     // #2466: [FA68, FAC7] ==> [96E3]
      u8"\xe9\x9f\xbf\0"                                     // #2467: [FA69, FACA] ==> [97FF]
      u8"\xe9\xa0\xbb\0"                                     // #2468: [FA6A, FACC] ==> [983B]
      u8"\xe6\x81\xb5\0"                                     // #2469: [FA6B] ==> [6075]
      u8"\xf0\xa4\x8b\xae\0"                                 // #2470: [FA6C] ==> [242EE]
      u8"\xe8\x88\x98\0"                                     // #2471: [FA6D] ==> [8218]
      u8"\xe4\xb8\xa6\0"                                     // #2472: [FA70] ==> [4E26]
      u8"\xe5\x86\xb5\0"                                     // #2473: [FA71, 2F81B] ==> [51B5]
      u8"\xe5\x85\xa8\0"                                     // #2474: [FA72] ==> [5168]
      u8"\xe4\xbe\x80\0"                                     // #2475: [FA73] ==> [4F80]
      u8"\xe5\x85\x85\0"                                     // #2476: [FA74] ==> [5145]
      u8"\xe5\x86\x80\0"                                     // #2477: [FA75] ==> [5180]
      u8"\xe5\x8b\x87\0"                                     // #2478: [FA76, 2F825] ==> [52C7]
      u8"\xe5\x8b\xba\0"                                     // #2479: [FA77, 2F828] ==> [52FA]
      u8"\xe5\x95\x95\0"                                     // #2480: [FA79] ==> [5555]
      u8"\xe5\x96\x99\0"                                     // #2481: [FA7A, 2F847] ==> [5599]
      u8"\xe5\x97\xa2\0"                                     // #2482: [FA7B] ==> [55E2]
      u8"\xe5\xa2\xb3\0"                                     // #2483: [FA7D] ==> [58B3]
      u8"\xe5\xa5\x84\0"                                     // #2484: [FA7E] ==> [5944]
      u8"\xe5\xa5\x94\0"                                     // #2485: [FA7F] ==> [5954]
      u8"\xe5\xa9\xa2\0"                                     // #2486: [FA80] ==> [5A62]
      u8"\xe5\xac\xa8\0"                                     // #2487: [FA81] ==> [5B28]
      u8"\xe5\xbb\x92\0"                                     // #2488: [FA82] ==> [5ED2]
      u8"\xe5\xbb\x99\0"                                     // #2489: [FA83] ==> [5ED9]
      u8"\xe5\xbd\xa9\0"                                     // #2490: [FA84] ==> [5F69]
      u8"\xe5\xbe\xad\0"                                     // #2491: [FA85] ==> [5FAD]
      u8"\xe6\x83\x98\0"                                     // #2492: [FA86] ==> [60D8]
      u8"\xe6\x85\x8e\0"                                     // #2493: [FA87, 2F8A8] ==> [614E]
      u8"\xe6\x84\x88\0"                                     // #2494: [FA88] ==> [6108]
      u8"\xe6\x85\xa0\0"                                     // #2495: [FA8A] ==> [6160]
      u8"\xe6\x88\xb4\0"                                     // #2496: [FA8C] ==> [6234]
      u8"\xe6\x8f\x84\0"                                     // #2497: [FA8D] ==> [63C4]
      u8"\xe6\x90\x9c\0"                                     // #2498: [FA8E] ==> [641C]
      u8"\xe6\x91\x92\0"                                     // #2499: [FA8F] ==> [6452]
      u8"\xe6\x95\x96\0"                                     // #2500: [FA90] ==> [6556]
      u8"\xe6\x9c\x9b\0"                                     // #2501: [FA93, 2F8D9] ==> [671B]
      u8"\xe6\x9d\x96\0"                                     // #2502: [FA94] ==> [6756]
      u8"\xe6\xbb\x9b\0"                                     // #2503: [FA98] ==> [6EDB]
      u8"\xe6\xbb\x8b\0"                                     // #2504: [FA99, 2F90B] ==> [6ECB]
      u8"\xe7\x80\x9e\0"                                     // #2505: [FA9B, 2F914] ==> [701E]
      u8"\xe7\x9e\xa7\0"                                     // #2506: [FA9D] ==> [77A7]
      u8"\xe7\x88\xb5\0"                                     // #2507: [FA9E, 2F921] ==> [7235]
      u8"\xe7\x8a\xaf\0"                                     // #2508: [FA9F] ==> [72AF]
      u8"\xe7\x91\xb1\0"                                     // #2509: [FAA1, 2F930] ==> [7471]
      u8"\xe7\x94\x86\0"                                     // #2510: [FAA2] ==> [7506]
      u8"\xe7\x94\xbb\0"                                     // #2511: [FAA3] ==> [753B]
      u8"\xe7\x98\x9d\0"                                     // #2512: [FAA4] ==> [761D]
      u8"\xe7\x98\x9f\0"                                     // #2513: [FAA5] ==> [761F]
      u8"\xe7\x9b\x9b\0"                                     // #2514: [FAA7] ==> [76DB]
      u8"\xe7\x9b\xb4\0"                                     // #2515: [FAA8, 2F940] ==> [76F4]
      u8"\xe7\x9d\x8a\0"                                     // #2516: [FAA9, 2F948] ==> [774A]
      u8"\xe7\x9d\x80\0"                                     // #2517: [FAAA] ==> [7740]
      u8"\xe7\xa3\x8c\0"                                     // #2518: [FAAB, 2F950] ==> [78CC]
      u8"\xe7\xaa\xb1\0"                                     // #2519: [FAAC] ==> [7AB1]
      u8"\xe7\xb1\xbb\0"                                     // #2520: [FAAE] ==> [7C7B]
      u8"\xe7\xb5\x9b\0"                                     // #2521: [FAAF] ==> [7D5B]
      u8"\xe7\xbc\xbe\0"                                     // #2522: [FAB1] ==> [7F3E]
      u8"\xe8\x8d\x92\0"                                     // #2523: [FAB3] ==> [8352]
      u8"\xe8\x8f\xaf\0"                                     // #2524: [FAB4] ==> [83EF]
      u8"\xe8\x9d\xb9\0"                                     // #2525: [FAB5, 2F9BB] ==> [8779]
      u8"\xe8\xa5\x81\0"                                     // #2526: [FAB6] ==> [8941]
      u8"\xe8\xa6\x86\0"                                     // #2527: [FAB7] ==> [8986]
      u8"\xe8\xaa\xbf\0"                                     // #2528: [FAB9] ==> [8ABF]
      u8"\xe8\xab\x8b\0"                                     // #2529: [FABB] ==> [8ACB]
      u8"\xe8\xab\xad\0"                                     // #2530: [FABE, 2F9D0] ==> [8AED]
      u8"\xe8\xae\x8a\0"                                     // #2531: [FAC0, 2F9D1] ==> [8B8A]
      u8"\xe8\xbc\xb8\0"                                     // #2532: [FAC2, 2F9DF] ==> [8F38]
      u8"\xe9\x81\xb2\0"                                     // #2533: [FAC3] ==> [9072]
      u8"\xe9\x86\x99\0"                                     // #2534: [FAC4] ==> [9199]
      u8"\xe9\x89\xb6\0"                                     // #2535: [FAC5] ==> [9276]
      u8"\xe9\x99\xbc\0"                                     // #2536: [FAC6] ==> [967C]
      u8"\xe9\x9f\x9b\0"                                     // #2537: [FAC9] ==> [97DB]
      u8"\xe9\xa0\x8b\0"                                     // #2538: [FACB, 2F9FE, 2F9FF] ==> [980B]
      u8"\xe9\xac\x92\0"                                     // #2539: [FACD, 2FA0A] ==> [9B12]
      u8"\xf0\xa2\xa1\x8a\0"                                 // #2540: [FACF] ==> [2284A]
      u8"\xf0\xa2\xa1\x84\0"                                 // #2541: [FAD0] ==> [22844]
      u8"\xf0\xa3\x8f\x95\0"                                 // #2542: [FAD1] ==> [233D5]
      u8"\xe3\xae\x9d\0"                                     // #2543: [FAD2, 2F8E7] ==> [3B9D]
      u8"\xe4\x80\x98\0"                                     // #2544: [FAD3] ==> [4018]
      u8"\xe4\x80\xb9\0"                                     // #2545: [FAD4, 2F949] ==> [4039]
      u8"\xf0\xa5\x89\x89\0"                                 // #2546: [FAD5] ==> [25249]
      u8"\xf0\xa5\xb3\x90\0"                                 // #2547: [FAD6] ==> [25CD0]
      u8"\xf0\xa7\xbb\x93\0"                                 // #2548: [FAD7] ==> [27ED3]
      u8"\xe9\xbd\x83\0"                                     // #2549: [FAD8] ==> [9F43]
      u8"\xe9\xbe\x8e\0"                                     // #2550: [FAD9] ==> [9F8E]
      u8"\x66\x66\0"                                         // #2551: [FB00] ==> [66, 66]
      u8"\x66\x69\0"                                         // #2552: [FB01] ==> [66, 69]
      u8"\x66\x6c\0"                                         // #2553: [FB02] ==> [66, 6C]
      u8"\x66\x66\x69\0"                                     // #2554: [FB03] ==> [66, 66, 69]
      u8"\x66\x66\x6c\0"                                     // #2555: [FB04] ==> [66, 66, 6C]
      u8"\x73\x74\0"                                         // #2556: [FB05, FB06] ==> [73, 74]
      u8"\xd5\xb4\xd5\xb6\0"                                 // #2557: [FB13] ==> [574, 576]
      u8"\xd5\xb4\xd5\xa5\0"                                 // #2558: [FB14] ==> [574, 565]
      u8"\xd5\xb4\xd5\xab\0"                                 // #2559: [FB15] ==> [574, 56B]
      u8"\xd5\xbe\xd5\xb6\0"                                 // #2560: [FB16] ==> [57E, 576]
      u8"\xd5\xb4\xd5\xad\0"                                 // #2561: [FB17] ==> [574, 56D]
      u8"\xd7\x99\xd6\xb4\0"                                 // #2562: [FB1D] ==> [5D9, 5B4]
      u8"\xd7\xb2\xd6\xb7\0"                                 // #2563: [FB1F] ==> [5F2, 5B7]
      u8"\xd7\xa2\0"                                         // #2564: [FB20] ==> [5E2]
      u8"\xd7\x94\0"                                         // #2565: [FB23] ==> [5D4]
      u8"\xd7\x9b\0"                                         // #2566: [FB24] ==> [5DB]
      u8"\xd7\x9c\0"                                         // #2567: [FB25] ==> [5DC]
      u8"\xd7\x9d\0"                                         // #2568: [FB26] ==> [5DD]
      u8"\xd7\xa8\0"                                         // #2569: [FB27] ==> [5E8]
      u8"\xd7\xaa\0"                                         // #2570: [FB28] ==> [5EA]
      u8"\xd7\xa9\xd7\x81\0"                                 // #2571: [FB2A] ==> [5E9, 5C1]
      u8"\xd7\xa9\xd7\x82\0"                                 // #2572: [FB2B] ==> [5E9, 5C2]
      u8"\xd7\xa9\xd6\xbc\xd7\x81\0"                         // #2573: [FB2C] ==> [5E9, 5BC, 5C1]
      u8"\xd7\xa9\xd6\xbc\xd7\x82\0"                         // #2574: [FB2D] ==> [5E9, 5BC, 5C2]
      u8"\xd7\x90\xd6\xb7\0"                                 // #2575: [FB2E] ==> [5D0, 5B7]
      u8"\xd7\x90\xd6\xb8\0"                                 // #2576: [FB2F] ==> [5D0, 5B8]
      u8"\xd7\x90\xd6\xbc\0"                                 // #2577: [FB30] ==> [5D0, 5BC]
      u8"\xd7\x91\xd6\xbc\0"                                 // #2578: [FB31] ==> [5D1, 5BC]
      u8"\xd7\x92\xd6\xbc\0"                                 // #2579: [FB32] ==> [5D2, 5BC]
      u8"\xd7\x93\xd6\xbc\0"                                 // #2580: [FB33] ==> [5D3, 5BC]
      u8"\xd7\x94\xd6\xbc\0"                                 // #2581: [FB34] ==> [5D4, 5BC]
      u8"\xd7\x95\xd6\xbc\0"                                 // #2582: [FB35] ==> [5D5, 5BC]
      u8"\xd7\x96\xd6\xbc\0"                                 // #2583: [FB36] ==> [5D6, 5BC]
      u8"\xd7\x98\xd6\xbc\0"                                 // #2584: [FB38] ==> [5D8, 5BC]
      u8"\xd7\x99\xd6\xbc\0"                                 // #2585: [FB39] ==> [5D9, 5BC]
      u8"\xd7\x9a\xd6\xbc\0"                                 // #2586: [FB3A] ==> [5DA, 5BC]
      u8"\xd7\x9b\xd6\xbc\0"                                 // #2587: [FB3B] ==> [5DB, 5BC]
      u8"\xd7\x9c\xd6\xbc\0"                                 // #2588: [FB3C] ==> [5DC, 5BC]
      u8"\xd7\x9e\xd6\xbc\0"                                 // #2589: [FB3E] ==> [5DE, 5BC]
      u8"\xd7\xa0\xd6\xbc\0"                                 // #2590: [FB40] ==> [5E0, 5BC]
      u8"\xd7\xa1\xd6\xbc\0"                                 // #2591: [FB41] ==> [5E1, 5BC]
      u8"\xd7\xa3\xd6\xbc\0"                                 // #2592: [FB43] ==> [5E3, 5BC]
      u8"\xd7\xa4\xd6\xbc\0"                                 // #2593: [FB44] ==> [5E4, 5BC]
      u8"\xd7\xa6\xd6\xbc\0"                                 // #2594: [FB46] ==> [5E6, 5BC]
      u8"\xd7\xa7\xd6\xbc\0"                                 // #2595: [FB47] ==> [5E7, 5BC]
      u8"\xd7\xa8\xd6\xbc\0"                                 // #2596: [FB48] ==> [5E8, 5BC]
      u8"\xd7\xa9\xd6\xbc\0"                                 // #2597: [FB49] ==> [5E9, 5BC]
      u8"\xd7\xaa\xd6\xbc\0"                                 // #2598: [FB4A] ==> [5EA, 5BC]
      u8"\xd7\x95\xd6\xb9\0"                                 // #2599: [FB4B] ==> [5D5, 5B9]
      u8"\xd7\x91\xd6\xbf\0"                                 // #2600: [FB4C] ==> [5D1, 5BF]
      u8"\xd7\x9b\xd6\xbf\0"                                 // #2601: [FB4D] ==> [5DB, 5BF]
      u8"\xd7\xa4\xd6\xbf\0"                                 // #2602: [FB4E] ==> [5E4, 5BF]
      u8"\xd7\x90\xd7\x9c\0"                                 // #2603: [FB4F] ==> [5D0, 5DC]
      u8"\xd9\xb1\0"                                         // #2604: [FB50, FB51] ==> [671]
      u8"\xd9\xbb\0"                                         // #2605: [FB52, FB53, FB54, FB55] ==> [67B]
      u8"\xd9\xbe\0"                                         // #2606: [FB56, FB57, FB58, FB59] ==> [67E]
      u8"\xda\x80\0"                                         // #2607: [FB5A, FB5B, FB5C, FB5D] ==> [680]
      u8"\xd9\xba\0"                                         // #2608: [FB5E, FB5F, FB60, FB61] ==> [67A]
      u8"\xd9\xbf\0"                                         // #2609: [FB62, FB63, FB64, FB65] ==> [67F]
      u8"\xd9\xb9\0"                                         // #2610: [FB66, FB67, FB68, FB69] ==> [679]
      u8"\xda\xa4\0"                                         // #2611: [FB6A, FB6B, FB6C, FB6D] ==> [6A4]
      u8"\xda\xa6\0"                                         // #2612: [FB6E, FB6F, FB70, FB71] ==> [6A6]
      u8"\xda\x84\0"                                         // #2613: [FB72, FB73, FB74, FB75] ==> [684]
      u8"\xda\x83\0"                                         // #2614: [FB76, FB77, FB78, FB79] ==> [683]
      u8"\xda\x86\0"                                         // #2615: [FB7A, FB7B, FB7C, FB7D] ==> [686]
      u8"\xda\x87\0"                                         // #2616: [FB7E, FB7F, FB80, FB81] ==> [687]
      u8"\xda\x8d\0"                                         // #2617: [FB82, FB83] ==> [68D]
      u8"\xda\x8c\0"                                         // #2618: [FB84, FB85] ==> [68C]
      u8"\xda\x8e\0"                                         // #2619: [FB86, FB87] ==> [68E]
      u8"\xda\x88\0"                                         // #2620: [FB88, FB89] ==> [688]
      u8"\xda\x98\0"                                         // #2621: [FB8A, FB8B] ==> [698]
      u8"\xda\x91\0"                                         // #2622: [FB8C, FB8D] ==> [691]
      u8"\xda\xa9\0"                                         // #2623: [FB8E, FB8F, FB90, FB91] ==> [6A9]
      u8"\xda\xaf\0"                                         // #2624: [FB92, FB93, FB94, FB95] ==> [6AF]
      u8"\xda\xb3\0"                                         // #2625: [FB96, FB97, FB98, FB99] ==> [6B3]
      u8"\xda\xb1\0"                                         // #2626: [FB9A, FB9B, FB9C, FB9D] ==> [6B1]
      u8"\xda\xba\0"                                         // #2627: [FB9E, FB9F, 1EE1D, 1EE5D] ==> [6BA]
      u8"\xda\xbb\0"                                         // #2628: [FBA0, FBA1, FBA2, FBA3] ==> [6BB]
      u8"\xdb\x80\0"                                         // #2629: [FBA4, FBA5] ==> [6C0]
      u8"\xdb\x81\0"                                         // #2630: [FBA6, FBA7, FBA8, FBA9] ==> [6C1]
      u8"\xda\xbe\0"                                         // #2631: [FBAA, FBAB, FBAC, FBAD] ==> [6BE]
      u8"\xdb\x92\0"                                         // #2632: [FBAE, FBAF] ==> [6D2]
      u8"\xdb\x93\0"                                         // #2633: [FBB0, FBB1] ==> [6D3]
      u8"\xda\xad\0"                                         // #2634: [FBD3, FBD4, FBD5, FBD6] ==> [6AD]
      u8"\xdb\x87\0"                                         // #2635: [FBD7, FBD8] ==> [6C7]
      u8"\xdb\x86\0"                                         // #2636: [FBD9, FBDA] ==> [6C6]
      u8"\xdb\x88\0"                                         // #2637: [FBDB, FBDC] ==> [6C8]
      u8"\xdb\x8b\0"                                         // #2638: [FBDE, FBDF] ==> [6CB]
      u8"\xdb\x85\0"                                         // #2639: [FBE0, FBE1] ==> [6C5]
      u8"\xdb\x89\0"                                         // #2640: [FBE2, FBE3] ==> [6C9]
      u8"\xdb\x90\0"                                         // #2641: [FBE4, FBE5, FBE6, FBE7] ==> [6D0]
      u8"\xd9\x89\0"                                         // #2642: [FBE8, FBE9, FEEF, FEF0] ==> [649]
      u8"\xd8\xa6\xd8\xa7\0"                                 // #2643: [FBEA, FBEB] ==> [626, 627]
      u8"\xd8\xa6\xdb\x95\0"                                 // #2644: [FBEC, FBED] ==> [626, 6D5]
      u8"\xd8\xa6\xd9\x88\0"                                 // #2645: [FBEE, FBEF] ==> [626, 648]
      u8"\xd8\xa6\xdb\x87\0"                                 // #2646: [FBF0, FBF1] ==> [626, 6C7]
      u8"\xd8\xa6\xdb\x86\0"                                 // #2647: [FBF2, FBF3] ==> [626, 6C6]
      u8"\xd8\xa6\xdb\x88\0"                                 // #2648: [FBF4, FBF5] ==> [626, 6C8]
      u8"\xd8\xa6\xdb\x90\0"                                 // #2649: [FBF6, FBF7, FBF8] ==> [626, 6D0]
      u8"\xd8\xa6\xd9\x89\0"                 // #2650: [FBF9, FBFA, FBFB, FC03, FC68] ==> [626, 649]
      u8"\xdb\x8c\0"                         // #2651: [FBFC, FBFD, FBFE, FBFF] ==> [6CC]
      u8"\xd8\xa6\xd8\xac\0"                 // #2652: [FC00, FC97] ==> [626, 62C]
      u8"\xd8\xa6\xd8\xad\0"                 // #2653: [FC01, FC98] ==> [626, 62D]
      u8"\xd8\xa6\xd9\x85\0"                 // #2654: [FC02, FC66, FC9A, FCDF] ==> [626, 645]
      u8"\xd8\xa6\xd9\x8a\0"                 // #2655: [FC04, FC69] ==> [626, 64A]
      u8"\xd8\xa8\xd8\xac\0"                 // #2656: [FC05, FC9C] ==> [628, 62C]
      u8"\xd8\xa8\xd8\xad\0"                 // #2657: [FC06, FC9D] ==> [628, 62D]
      u8"\xd8\xa8\xd8\xae\0"                 // #2658: [FC07, FC9E] ==> [628, 62E]
      u8"\xd8\xa8\xd9\x85\0"                 // #2659: [FC08, FC6C, FC9F, FCE1] ==> [628, 645]
      u8"\xd8\xa8\xd9\x89\0"                 // #2660: [FC09, FC6E] ==> [628, 649]
      u8"\xd8\xa8\xd9\x8a\0"                 // #2661: [FC0A, FC6F] ==> [628, 64A]
      u8"\xd8\xaa\xd8\xac\0"                 // #2662: [FC0B, FCA1] ==> [62A, 62C]
      u8"\xd8\xaa\xd8\xad\0"                 // #2663: [FC0C, FCA2] ==> [62A, 62D]
      u8"\xd8\xaa\xd8\xae\0"                 // #2664: [FC0D, FCA3] ==> [62A, 62E]
      u8"\xd8\xaa\xd9\x85\0"                 // #2665: [FC0E, FC72, FCA4, FCE3] ==> [62A, 645]
      u8"\xd8\xaa\xd9\x89\0"                 // #2666: [FC0F, FC74] ==> [62A, 649]
      u8"\xd8\xaa\xd9\x8a\0"                 // #2667: [FC10, FC75] ==> [62A, 64A]
      u8"\xd8\xab\xd8\xac\0"                 // #2668: [FC11] ==> [62B, 62C]
      u8"\xd8\xab\xd9\x85\0"                 // #2669: [FC12, FC78, FCA6, FCE5] ==> [62B, 645]
      u8"\xd8\xab\xd9\x89\0"                 // #2670: [FC13, FC7A] ==> [62B, 649]
      u8"\xd8\xab\xd9\x8a\0"                 // #2671: [FC14, FC7B] ==> [62B, 64A]
      u8"\xd8\xac\xd8\xad\0"                 // #2672: [FC15, FCA7] ==> [62C, 62D]
      u8"\xd8\xac\xd9\x85\0"                 // #2673: [FC16, FCA8] ==> [62C, 645]
      u8"\xd8\xad\xd8\xac\0"                 // #2674: [FC17, FCA9] ==> [62D, 62C]
      u8"\xd8\xad\xd9\x85\0"                 // #2675: [FC18, FCAA] ==> [62D, 645]
      u8"\xd8\xae\xd8\xac\0"                 // #2676: [FC19, FCAB] ==> [62E, 62C]
      u8"\xd8\xae\xd8\xad\0"                 // #2677: [FC1A] ==> [62E, 62D]
      u8"\xd8\xae\xd9\x85\0"                 // #2678: [FC1B, FCAC] ==> [62E, 645]
      u8"\xd8\xb3\xd8\xac\0"                 // #2679: [FC1C, FCAD, FD34] ==> [633, 62C]
      u8"\xd8\xb3\xd8\xad\0"                 // #2680: [FC1D, FCAE, FD35] ==> [633, 62D]
      u8"\xd8\xb3\xd8\xae\0"                 // #2681: [FC1E, FCAF, FD36] ==> [633, 62E]
      u8"\xd8\xb3\xd9\x85\0"                 // #2682: [FC1F, FCB0, FCE7] ==> [633, 645]
      u8"\xd8\xb5\xd8\xad\0"                 // #2683: [FC20, FCB1] ==> [635, 62D]
      u8"\xd8\xb5\xd9\x85\0"                 // #2684: [FC21, FCB3] ==> [635, 645]
      u8"\xd8\xb6\xd8\xac\0"                 // #2685: [FC22, FCB4] ==> [636, 62C]
      u8"\xd8\xb6\xd8\xad\0"                 // #2686: [FC23, FCB5] ==> [636, 62D]
      u8"\xd8\xb6\xd8\xae\0"                 // #2687: [FC24, FCB6] ==> [636, 62E]
      u8"\xd8\xb6\xd9\x85\0"                 // #2688: [FC25, FCB7] ==> [636, 645]
      u8"\xd8\xb7\xd8\xad\0"                 // #2689: [FC26, FCB8] ==> [637, 62D]
      u8"\xd8\xb7\xd9\x85\0"                 // #2690: [FC27, FD33, FD3A] ==> [637, 645]
      u8"\xd8\xb8\xd9\x85\0"                 // #2691: [FC28, FCB9, FD3B] ==> [638, 645]
      u8"\xd8\xb9\xd8\xac\0"                 // #2692: [FC29, FCBA] ==> [639, 62C]
      u8"\xd8\xb9\xd9\x85\0"                 // #2693: [FC2A, FCBB] ==> [639, 645]
      u8"\xd8\xba\xd8\xac\0"                 // #2694: [FC2B, FCBC] ==> [63A, 62C]
      u8"\xd8\xba\xd9\x85\0"                 // #2695: [FC2C, FCBD] ==> [63A, 645]
      u8"\xd9\x81\xd8\xac\0"                 // #2696: [FC2D, FCBE] ==> [641, 62C]
      u8"\xd9\x81\xd8\xad\0"                 // #2697: [FC2E, FCBF] ==> [641, 62D]
      u8"\xd9\x81\xd8\xae\0"                 // #2698: [FC2F, FCC0] ==> [641, 62E]
      u8"\xd9\x81\xd9\x85\0"                 // #2699: [FC30, FCC1] ==> [641, 645]
      u8"\xd9\x81\xd9\x89\0"                 // #2700: [FC31, FC7C] ==> [641, 649]
      u8"\xd9\x81\xd9\x8a\0"                 // #2701: [FC32, FC7D] ==> [641, 64A]
      u8"\xd9\x82\xd8\xad\0"                 // #2702: [FC33, FCC2] ==> [642, 62D]
      u8"\xd9\x82\xd9\x85\0"                 // #2703: [FC34, FCC3] ==> [642, 645]
      u8"\xd9\x82\xd9\x89\0"                 // #2704: [FC35, FC7E] ==> [642, 649]
      u8"\xd9\x82\xd9\x8a\0"                 // #2705: [FC36, FC7F] ==> [642, 64A]
      u8"\xd9\x83\xd8\xa7\0"                 // #2706: [FC37, FC80] ==> [643, 627]
      u8"\xd9\x83\xd8\xac\0"                 // #2707: [FC38, FCC4] ==> [643, 62C]
      u8"\xd9\x83\xd8\xad\0"                 // #2708: [FC39, FCC5] ==> [643, 62D]
      u8"\xd9\x83\xd8\xae\0"                 // #2709: [FC3A, FCC6] ==> [643, 62E]
      u8"\xd9\x83\xd9\x84\0"                 // #2710: [FC3B, FC81, FCC7, FCEB] ==> [643, 644]
      u8"\xd9\x83\xd9\x85\0"                 // #2711: [FC3C, FC82, FCC8, FCEC] ==> [643, 645]
      u8"\xd9\x83\xd9\x89\0"                 // #2712: [FC3D, FC83] ==> [643, 649]
      u8"\xd9\x83\xd9\x8a\0"                 // #2713: [FC3E, FC84] ==> [643, 64A]
      u8"\xd9\x84\xd8\xac\0"                 // #2714: [FC3F, FCC9] ==> [644, 62C]
      u8"\xd9\x84\xd8\xad\0"                 // #2715: [FC40, FCCA] ==> [644, 62D]
      u8"\xd9\x84\xd8\xae\0"                 // #2716: [FC41, FCCB] ==> [644, 62E]
      u8"\xd9\x84\xd9\x85\0"                 // #2717: [FC42, FC85, FCCC, FCED] ==> [644, 645]
      u8"\xd9\x84\xd9\x89\0"                 // #2718: [FC43, FC86] ==> [644, 649]
      u8"\xd9\x84\xd9\x8a\0"                 // #2719: [FC44, FC87] ==> [644, 64A]
      u8"\xd9\x85\xd8\xac\0"                 // #2720: [FC45, FCCE] ==> [645, 62C]
      u8"\xd9\x85\xd8\xad\0"                 // #2721: [FC46, FCCF] ==> [645, 62D]
      u8"\xd9\x85\xd8\xae\0"                 // #2722: [FC47, FCD0] ==> [645, 62E]
      u8"\xd9\x85\xd9\x85\0"                 // #2723: [FC48, FC89, FCD1] ==> [645, 645]
      u8"\xd9\x85\xd9\x89\0"                 // #2724: [FC49] ==> [645, 649]
      u8"\xd9\x85\xd9\x8a\0"                 // #2725: [FC4A] ==> [645, 64A]
      u8"\xd9\x86\xd8\xac\0"                 // #2726: [FC4B, FCD2] ==> [646, 62C]
      u8"\xd9\x86\xd8\xad\0"                 // #2727: [FC4C, FCD3] ==> [646, 62D]
      u8"\xd9\x86\xd8\xae\0"                 // #2728: [FC4D, FCD4] ==> [646, 62E]
      u8"\xd9\x86\xd9\x85\0"                 // #2729: [FC4E, FC8C, FCD5, FCEE] ==> [646, 645]
      u8"\xd9\x86\xd9\x89\0"                 // #2730: [FC4F, FC8E] ==> [646, 649]
      u8"\xd9\x86\xd9\x8a\0"                 // #2731: [FC50, FC8F] ==> [646, 64A]
      u8"\xd9\x87\xd8\xac\0"                 // #2732: [FC51, FCD7] ==> [647, 62C]
      u8"\xd9\x87\xd9\x85\0"                 // #2733: [FC52, FCD8] ==> [647, 645]
      u8"\xd9\x87\xd9\x89\0"                 // #2734: [FC53] ==> [647, 649]
      u8"\xd9\x87\xd9\x8a\0"                 // #2735: [FC54] ==> [647, 64A]
      u8"\xd9\x8a\xd8\xac\0"                 // #2736: [FC55, FCDA] ==> [64A, 62C]
      u8"\xd9\x8a\xd8\xad\0"                 // #2737: [FC56, FCDB] ==> [64A, 62D]
      u8"\xd9\x8a\xd8\xae\0"                 // #2738: [FC57, FCDC] ==> [64A, 62E]
      u8"\xd9\x8a\xd9\x85\0"                 // #2739: [FC58, FC93, FCDD, FCF0] ==> [64A, 645]
      u8"\xd9\x8a\xd9\x89\0"                 // #2740: [FC59, FC95] ==> [64A, 649]
      u8"\xd9\x8a\xd9\x8a\0"                 // #2741: [FC5A, FC96] ==> [64A, 64A]
      u8"\xd8\xb0\xd9\xb0\0"                 // #2742: [FC5B] ==> [630, 670]
      u8"\xd8\xb1\xd9\xb0\0"                 // #2743: [FC5C] ==> [631, 670]
      u8"\xd9\x89\xd9\xb0\0"                 // #2744: [FC5D, FC90] ==> [649, 670]
      u8"\40\xd9\x8c\xd9\x91\0"              // #2745: [FC5E] ==> [20, 64C, 651]
      u8"\40\xd9\x8d\xd9\x91\0"              // #2746: [FC5F] ==> [20, 64D, 651]
      u8"\40\xd9\x8e\xd9\x91\0"              // #2747: [FC60] ==> [20, 64E, 651]
      u8"\40\xd9\x8f\xd9\x91\0"              // #2748: [FC61] ==> [20, 64F, 651]
      u8"\40\xd9\x90\xd9\x91\0"              // #2749: [FC62] ==> [20, 650, 651]
      u8"\40\xd9\x91\xd9\xb0\0"              // #2750: [FC63] ==> [20, 651, 670]
      u8"\xd8\xa6\xd8\xb1\0"                 // #2751: [FC64] ==> [626, 631]
      u8"\xd8\xa6\xd8\xb2\0"                 // #2752: [FC65] ==> [626, 632]
      u8"\xd8\xa6\xd9\x86\0"                 // #2753: [FC67] ==> [626, 646]
      u8"\xd8\xa8\xd8\xb1\0"                 // #2754: [FC6A] ==> [628, 631]
      u8"\xd8\xa8\xd8\xb2\0"                 // #2755: [FC6B] ==> [628, 632]
      u8"\xd8\xa8\xd9\x86\0"                 // #2756: [FC6D] ==> [628, 646]
      u8"\xd8\xaa\xd8\xb1\0"                 // #2757: [FC70] ==> [62A, 631]
      u8"\xd8\xaa\xd8\xb2\0"                 // #2758: [FC71] ==> [62A, 632]
      u8"\xd8\xaa\xd9\x86\0"                 // #2759: [FC73] ==> [62A, 646]
      u8"\xd8\xab\xd8\xb1\0"                 // #2760: [FC76] ==> [62B, 631]
      u8"\xd8\xab\xd8\xb2\0"                 // #2761: [FC77] ==> [62B, 632]
      u8"\xd8\xab\xd9\x86\0"                 // #2762: [FC79] ==> [62B, 646]
      u8"\xd9\x85\xd8\xa7\0"                 // #2763: [FC88] ==> [645, 627]
      u8"\xd9\x86\xd8\xb1\0"                 // #2764: [FC8A] ==> [646, 631]
      u8"\xd9\x86\xd8\xb2\0"                 // #2765: [FC8B] ==> [646, 632]
      u8"\xd9\x86\xd9\x86\0"                 // #2766: [FC8D] ==> [646, 646]
      u8"\xd9\x8a\xd8\xb1\0"                 // #2767: [FC91] ==> [64A, 631]
      u8"\xd9\x8a\xd8\xb2\0"                 // #2768: [FC92] ==> [64A, 632]
      u8"\xd9\x8a\xd9\x86\0"                 // #2769: [FC94] ==> [64A, 646]
      u8"\xd8\xa6\xd8\xae\0"                 // #2770: [FC99] ==> [626, 62E]
      u8"\xd8\xa6\xd9\x87\0"                 // #2771: [FC9B, FCE0] ==> [626, 647]
      u8"\xd8\xa8\xd9\x87\0"                 // #2772: [FCA0, FCE2] ==> [628, 647]
      u8"\xd8\xaa\xd9\x87\0"                 // #2773: [FCA5, FCE4] ==> [62A, 647]
      u8"\xd8\xb5\xd8\xae\0"                 // #2774: [FCB2] ==> [635, 62E]
      u8"\xd9\x84\xd9\x87\0"                 // #2775: [FCCD] ==> [644, 647]
      u8"\xd9\x86\xd9\x87\0"                 // #2776: [FCD6, FCEF] ==> [646, 647]
      u8"\xd9\x87\xd9\xb0\0"                 // #2777: [FCD9] ==> [647, 670]
      u8"\xd9\x8a\xd9\x87\0"                 // #2778: [FCDE, FCF1] ==> [64A, 647]
      u8"\xd8\xab\xd9\x87\0"                 // #2779: [FCE6] ==> [62B, 647]
      u8"\xd8\xb3\xd9\x87\0"                 // #2780: [FCE8, FD31] ==> [633, 647]
      u8"\xd8\xb4\xd9\x85\0"                 // #2781: [FCE9, FD0C, FD28, FD30] ==> [634, 645]
      u8"\xd8\xb4\xd9\x87\0"                 // #2782: [FCEA, FD32] ==> [634, 647]
      u8"\xd9\x80\xd9\x8e\xd9\x91\0"         // #2783: [FCF2] ==> [640, 64E, 651]
      u8"\xd9\x80\xd9\x8f\xd9\x91\0"         // #2784: [FCF3] ==> [640, 64F, 651]
      u8"\xd9\x80\xd9\x90\xd9\x91\0"         // #2785: [FCF4] ==> [640, 650, 651]
      u8"\xd8\xb7\xd9\x89\0"                 // #2786: [FCF5, FD11] ==> [637, 649]
      u8"\xd8\xb7\xd9\x8a\0"                 // #2787: [FCF6, FD12] ==> [637, 64A]
      u8"\xd8\xb9\xd9\x89\0"                 // #2788: [FCF7, FD13] ==> [639, 649]
      u8"\xd8\xb9\xd9\x8a\0"                 // #2789: [FCF8, FD14] ==> [639, 64A]
      u8"\xd8\xba\xd9\x89\0"                 // #2790: [FCF9, FD15] ==> [63A, 649]
      u8"\xd8\xba\xd9\x8a\0"                 // #2791: [FCFA, FD16] ==> [63A, 64A]
      u8"\xd8\xb3\xd9\x89\0"                 // #2792: [FCFB, FD17] ==> [633, 649]
      u8"\xd8\xb3\xd9\x8a\0"                 // #2793: [FCFC, FD18] ==> [633, 64A]
      u8"\xd8\xb4\xd9\x89\0"                 // #2794: [FCFD, FD19] ==> [634, 649]
      u8"\xd8\xb4\xd9\x8a\0"                 // #2795: [FCFE, FD1A] ==> [634, 64A]
      u8"\xd8\xad\xd9\x89\0"                 // #2796: [FCFF, FD1B] ==> [62D, 649]
      u8"\xd8\xad\xd9\x8a\0"                 // #2797: [FD00, FD1C] ==> [62D, 64A]
      u8"\xd8\xac\xd9\x89\0"                 // #2798: [FD01, FD1D] ==> [62C, 649]
      u8"\xd8\xac\xd9\x8a\0"                 // #2799: [FD02, FD1E] ==> [62C, 64A]
      u8"\xd8\xae\xd9\x89\0"                 // #2800: [FD03, FD1F] ==> [62E, 649]
      u8"\xd8\xae\xd9\x8a\0"                 // #2801: [FD04, FD20] ==> [62E, 64A]
      u8"\xd8\xb5\xd9\x89\0"                 // #2802: [FD05, FD21] ==> [635, 649]
      u8"\xd8\xb5\xd9\x8a\0"                 // #2803: [FD06, FD22] ==> [635, 64A]
      u8"\xd8\xb6\xd9\x89\0"                 // #2804: [FD07, FD23] ==> [636, 649]
      u8"\xd8\xb6\xd9\x8a\0"                 // #2805: [FD08, FD24] ==> [636, 64A]
      u8"\xd8\xb4\xd8\xac\0"                 // #2806: [FD09, FD25, FD2D, FD37] ==> [634, 62C]
      u8"\xd8\xb4\xd8\xad\0"                 // #2807: [FD0A, FD26, FD2E, FD38] ==> [634, 62D]
      u8"\xd8\xb4\xd8\xae\0"                 // #2808: [FD0B, FD27, FD2F, FD39] ==> [634, 62E]
      u8"\xd8\xb4\xd8\xb1\0"                 // #2809: [FD0D, FD29] ==> [634, 631]
      u8"\xd8\xb3\xd8\xb1\0"                 // #2810: [FD0E, FD2A] ==> [633, 631]
      u8"\xd8\xb5\xd8\xb1\0"                 // #2811: [FD0F, FD2B] ==> [635, 631]
      u8"\xd8\xb6\xd8\xb1\0"                 // #2812: [FD10, FD2C] ==> [636, 631]
      u8"\xd8\xa7\xd9\x8b\0"                 // #2813: [FD3C, FD3D] ==> [627, 64B]
      u8"\xd8\xaa\xd8\xac\xd9\x85\0"         // #2814: [FD50] ==> [62A, 62C, 645]
      u8"\xd8\xaa\xd8\xad\xd8\xac\0"         // #2815: [FD51, FD52] ==> [62A, 62D, 62C]
      u8"\xd8\xaa\xd8\xad\xd9\x85\0"         // #2816: [FD53] ==> [62A, 62D, 645]
      u8"\xd8\xaa\xd8\xae\xd9\x85\0"         // #2817: [FD54] ==> [62A, 62E, 645]
      u8"\xd8\xaa\xd9\x85\xd8\xac\0"         // #2818: [FD55] ==> [62A, 645, 62C]
      u8"\xd8\xaa\xd9\x85\xd8\xad\0"         // #2819: [FD56] ==> [62A, 645, 62D]
      u8"\xd8\xaa\xd9\x85\xd8\xae\0"         // #2820: [FD57] ==> [62A, 645, 62E]
      u8"\xd8\xac\xd9\x85\xd8\xad\0"         // #2821: [FD58, FD59] ==> [62C, 645, 62D]
      u8"\xd8\xad\xd9\x85\xd9\x8a\0"         // #2822: [FD5A] ==> [62D, 645, 64A]
      u8"\xd8\xad\xd9\x85\xd9\x89\0"         // #2823: [FD5B] ==> [62D, 645, 649]
      u8"\xd8\xb3\xd8\xad\xd8\xac\0"         // #2824: [FD5C] ==> [633, 62D, 62C]
      u8"\xd8\xb3\xd8\xac\xd8\xad\0"         // #2825: [FD5D] ==> [633, 62C, 62D]
      u8"\xd8\xb3\xd8\xac\xd9\x89\0"         // #2826: [FD5E] ==> [633, 62C, 649]
      u8"\xd8\xb3\xd9\x85\xd8\xad\0"         // #2827: [FD5F, FD60] ==> [633, 645, 62D]
      u8"\xd8\xb3\xd9\x85\xd8\xac\0"         // #2828: [FD61] ==> [633, 645, 62C]
      u8"\xd8\xb3\xd9\x85\xd9\x85\0"         // #2829: [FD62, FD63] ==> [633, 645, 645]
      u8"\xd8\xb5\xd8\xad\xd8\xad\0"         // #2830: [FD64, FD65] ==> [635, 62D, 62D]
      u8"\xd8\xb5\xd9\x85\xd9\x85\0"         // #2831: [FD66, FDC5] ==> [635, 645, 645]
      u8"\xd8\xb4\xd8\xad\xd9\x85\0"         // #2832: [FD67, FD68] ==> [634, 62D, 645]
      u8"\xd8\xb4\xd8\xac\xd9\x8a\0"         // #2833: [FD69] ==> [634, 62C, 64A]
      u8"\xd8\xb4\xd9\x85\xd8\xae\0"         // #2834: [FD6A, FD6B] ==> [634, 645, 62E]
      u8"\xd8\xb4\xd9\x85\xd9\x85\0"         // #2835: [FD6C, FD6D] ==> [634, 645, 645]
      u8"\xd8\xb6\xd8\xad\xd9\x89\0"         // #2836: [FD6E] ==> [636, 62D, 649]
      u8"\xd8\xb6\xd8\xae\xd9\x85\0"         // #2837: [FD6F, FD70] ==> [636, 62E, 645]
      u8"\xd8\xb7\xd9\x85\xd8\xad\0"         // #2838: [FD71, FD72] ==> [637, 645, 62D]
      u8"\xd8\xb7\xd9\x85\xd9\x85\0"         // #2839: [FD73] ==> [637, 645, 645]
      u8"\xd8\xb7\xd9\x85\xd9\x8a\0"         // #2840: [FD74] ==> [637, 645, 64A]
      u8"\xd8\xb9\xd8\xac\xd9\x85\0"         // #2841: [FD75, FDC4] ==> [639, 62C, 645]
      u8"\xd8\xb9\xd9\x85\xd9\x85\0"         // #2842: [FD76, FD77] ==> [639, 645, 645]
      u8"\xd8\xb9\xd9\x85\xd9\x89\0"         // #2843: [FD78] ==> [639, 645, 649]
      u8"\xd8\xba\xd9\x85\xd9\x85\0"         // #2844: [FD79] ==> [63A, 645, 645]
      u8"\xd8\xba\xd9\x85\xd9\x8a\0"         // #2845: [FD7A] ==> [63A, 645, 64A]
      u8"\xd8\xba\xd9\x85\xd9\x89\0"         // #2846: [FD7B] ==> [63A, 645, 649]
      u8"\xd9\x81\xd8\xae\xd9\x85\0"         // #2847: [FD7C, FD7D] ==> [641, 62E, 645]
      u8"\xd9\x82\xd9\x85\xd8\xad\0"         // #2848: [FD7E, FDB4] ==> [642, 645, 62D]
      u8"\xd9\x82\xd9\x85\xd9\x85\0"         // #2849: [FD7F] ==> [642, 645, 645]
      u8"\xd9\x84\xd8\xad\xd9\x85\0"         // #2850: [FD80, FDB5] ==> [644, 62D, 645]
      u8"\xd9\x84\xd8\xad\xd9\x8a\0"         // #2851: [FD81] ==> [644, 62D, 64A]
      u8"\xd9\x84\xd8\xad\xd9\x89\0"         // #2852: [FD82] ==> [644, 62D, 649]
      u8"\xd9\x84\xd8\xac\xd8\xac\0"         // #2853: [FD83, FD84] ==> [644, 62C, 62C]
      u8"\xd9\x84\xd8\xae\xd9\x85\0"         // #2854: [FD85, FD86] ==> [644, 62E, 645]
      u8"\xd9\x84\xd9\x85\xd8\xad\0"         // #2855: [FD87, FD88] ==> [644, 645, 62D]
      u8"\xd9\x85\xd8\xad\xd8\xac\0"         // #2856: [FD89] ==> [645, 62D, 62C]
      u8"\xd9\x85\xd8\xad\xd9\x85\0"         // #2857: [FD8A] ==> [645, 62D, 645]
      u8"\xd9\x85\xd8\xad\xd9\x8a\0"         // #2858: [FD8B] ==> [645, 62D, 64A]
      u8"\xd9\x85\xd8\xac\xd8\xad\0"         // #2859: [FD8C] ==> [645, 62C, 62D]
      u8"\xd9\x85\xd8\xac\xd9\x85\0"         // #2860: [FD8D] ==> [645, 62C, 645]
      u8"\xd9\x85\xd8\xae\xd8\xac\0"         // #2861: [FD8E] ==> [645, 62E, 62C]
      u8"\xd9\x85\xd8\xae\xd9\x85\0"         // #2862: [FD8F] ==> [645, 62E, 645]
      u8"\xd9\x85\xd8\xac\xd8\xae\0"         // #2863: [FD92] ==> [645, 62C, 62E]
      u8"\xd9\x87\xd9\x85\xd8\xac\0"         // #2864: [FD93] ==> [647, 645, 62C]
      u8"\xd9\x87\xd9\x85\xd9\x85\0"         // #2865: [FD94] ==> [647, 645, 645]
      u8"\xd9\x86\xd8\xad\xd9\x85\0"         // #2866: [FD95] ==> [646, 62D, 645]
      u8"\xd9\x86\xd8\xad\xd9\x89\0"         // #2867: [FD96] ==> [646, 62D, 649]
      u8"\xd9\x86\xd8\xac\xd9\x85\0"         // #2868: [FD97, FD98] ==> [646, 62C, 645]
      u8"\xd9\x86\xd8\xac\xd9\x89\0"         // #2869: [FD99] ==> [646, 62C, 649]
      u8"\xd9\x86\xd9\x85\xd9\x8a\0"         // #2870: [FD9A] ==> [646, 645, 64A]
      u8"\xd9\x86\xd9\x85\xd9\x89\0"         // #2871: [FD9B] ==> [646, 645, 649]
      u8"\xd9\x8a\xd9\x85\xd9\x85\0"         // #2872: [FD9C, FD9D] ==> [64A, 645, 645]
      u8"\xd8\xa8\xd8\xae\xd9\x8a\0"         // #2873: [FD9E] ==> [628, 62E, 64A]
      u8"\xd8\xaa\xd8\xac\xd9\x8a\0"         // #2874: [FD9F] ==> [62A, 62C, 64A]
      u8"\xd8\xaa\xd8\xac\xd9\x89\0"         // #2875: [FDA0] ==> [62A, 62C, 649]
      u8"\xd8\xaa\xd8\xae\xd9\x8a\0"         // #2876: [FDA1] ==> [62A, 62E, 64A]
      u8"\xd8\xaa\xd8\xae\xd9\x89\0"         // #2877: [FDA2] ==> [62A, 62E, 649]
      u8"\xd8\xaa\xd9\x85\xd9\x8a\0"         // #2878: [FDA3] ==> [62A, 645, 64A]
      u8"\xd8\xaa\xd9\x85\xd9\x89\0"         // #2879: [FDA4] ==> [62A, 645, 649]
      u8"\xd8\xac\xd9\x85\xd9\x8a\0"         // #2880: [FDA5] ==> [62C, 645, 64A]
      u8"\xd8\xac\xd8\xad\xd9\x89\0"         // #2881: [FDA6] ==> [62C, 62D, 649]
      u8"\xd8\xac\xd9\x85\xd9\x89\0"         // #2882: [FDA7] ==> [62C, 645, 649]
      u8"\xd8\xb3\xd8\xae\xd9\x89\0"         // #2883: [FDA8] ==> [633, 62E, 649]
      u8"\xd8\xb5\xd8\xad\xd9\x8a\0"         // #2884: [FDA9] ==> [635, 62D, 64A]
      u8"\xd8\xb4\xd8\xad\xd9\x8a\0"         // #2885: [FDAA] ==> [634, 62D, 64A]
      u8"\xd8\xb6\xd8\xad\xd9\x8a\0"         // #2886: [FDAB] ==> [636, 62D, 64A]
      u8"\xd9\x84\xd8\xac\xd9\x8a\0"         // #2887: [FDAC] ==> [644, 62C, 64A]
      u8"\xd9\x84\xd9\x85\xd9\x8a\0"         // #2888: [FDAD] ==> [644, 645, 64A]
      u8"\xd9\x8a\xd8\xad\xd9\x8a\0"         // #2889: [FDAE] ==> [64A, 62D, 64A]
      u8"\xd9\x8a\xd8\xac\xd9\x8a\0"         // #2890: [FDAF] ==> [64A, 62C, 64A]
      u8"\xd9\x8a\xd9\x85\xd9\x8a\0"         // #2891: [FDB0] ==> [64A, 645, 64A]
      u8"\xd9\x85\xd9\x85\xd9\x8a\0"         // #2892: [FDB1] ==> [645, 645, 64A]
      u8"\xd9\x82\xd9\x85\xd9\x8a\0"         // #2893: [FDB2] ==> [642, 645, 64A]
      u8"\xd9\x86\xd8\xad\xd9\x8a\0"         // #2894: [FDB3] ==> [646, 62D, 64A]
      u8"\xd8\xb9\xd9\x85\xd9\x8a\0"         // #2895: [FDB6] ==> [639, 645, 64A]
      u8"\xd9\x83\xd9\x85\xd9\x8a\0"         // #2896: [FDB7] ==> [643, 645, 64A]
      u8"\xd9\x86\xd8\xac\xd8\xad\0"         // #2897: [FDB8, FDBD] ==> [646, 62C, 62D]
      u8"\xd9\x85\xd8\xae\xd9\x8a\0"         // #2898: [FDB9] ==> [645, 62E, 64A]
      u8"\xd9\x84\xd8\xac\xd9\x85\0"         // #2899: [FDBA, FDBC] ==> [644, 62C, 645]
      u8"\xd9\x83\xd9\x85\xd9\x85\0"         // #2900: [FDBB, FDC3] ==> [643, 645, 645]
      u8"\xd8\xac\xd8\xad\xd9\x8a\0"         // #2901: [FDBE] ==> [62C, 62D, 64A]
      u8"\xd8\xad\xd8\xac\xd9\x8a\0"         // #2902: [FDBF] ==> [62D, 62C, 64A]
      u8"\xd9\x85\xd8\xac\xd9\x8a\0"         // #2903: [FDC0] ==> [645, 62C, 64A]
      u8"\xd9\x81\xd9\x85\xd9\x8a\0"         // #2904: [FDC1] ==> [641, 645, 64A]
      u8"\xd8\xa8\xd8\xad\xd9\x8a\0"         // #2905: [FDC2] ==> [628, 62D, 64A]
      u8"\xd8\xb3\xd8\xae\xd9\x8a\0"         // #2906: [FDC6] ==> [633, 62E, 64A]
      u8"\xd9\x86\xd8\xac\xd9\x8a\0"         // #2907: [FDC7] ==> [646, 62C, 64A]
      u8"\xd8\xb5\xd9\x84\xdb\x92\0"         // #2908: [FDF0] ==> [635, 644, 6D2]
      u8"\xd9\x82\xd9\x84\xdb\x92\0"         // #2909: [FDF1] ==> [642, 644, 6D2]
      u8"\xd8\xa7\xd9\x84\xd9\x84\xd9\x87\0" // #2910: [FDF2] ==> [627, 644, 644, 647]
      u8"\xd8\xa7\xd9\x83\xd8\xa8\xd8\xb1\0" // #2911: [FDF3] ==> [627, 643, 628, 631]
      u8"\xd9\x85\xd8\xad\xd9\x85\xd8\xaf\0" // #2912: [FDF4] ==> [645, 62D, 645, 62F]
      u8"\xd8\xb5\xd9\x84\xd8\xb9\xd9\x85\0" // #2913: [FDF5] ==> [635, 644, 639, 645]
      u8"\xd8\xb1\xd8\xb3\xd9\x88\xd9\x84\0" // #2914: [FDF6] ==> [631, 633, 648, 644]
      u8"\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\0" // #2915: [FDF7] ==> [639, 644, 64A, 647]
      u8"\xd9\x88\xd8\xb3\xd9\x84\xd9\x85\0" // #2916: [FDF8] ==> [648, 633, 644, 645]
      u8"\xd8\xb5\xd9\x84\xd9\x89\0"         // #2917: [FDF9] ==> [635, 644, 649]
      u8"\xd8\xb5\xd9\x84\xd9\x89\40\xd8\xa7\xd9\x84\xd9\x84\xd9\x87\40\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\40"
      u8"\xd9\x88\xd8\xb3\xd9\x84\xd9\x85\0" // #2918: [FDFA] ==> [635, 644, 649, 20, 627, 644, 644, 647, 20,
                                             // 639, 644, 64A, 647, 20, 648, 633, 644, 645]
      u8"\xd8\xac\xd9\x84\40\xd8\xac\xd9\x84\xd8\xa7\xd9\x84\xd9\x87\0" // #2919: [FDFB] ==> [62C, 644, 20,
                                                                        // 62C, 644, 627, 644, 647]
      u8"\xd8\xb1\xdb\x8c\xd8\xa7\xd9\x84\0" // #2920: [FDFC] ==> [631, 6CC, 627, 644]
      u8"\54\0"                              // #2921: [FE10, FE50, FF0C] ==> [2C]
      u8"\xe3\x80\x81\0"                     // #2922: [FE11, FE51, FF64] ==> [3001]
      u8"\72\0"                              // #2923: [FE13, FE55, FF1A] ==> [3A]
      u8"\41\0"                              // #2924: [FE15, FE57, FF01] ==> [21]
      u8"\77\0"                              // #2925: [FE16, FE56, FF1F] ==> [3F]
      u8"\xe3\x80\x96\0"                     // #2926: [FE17] ==> [3016]
      u8"\xe3\x80\x97\0"                     // #2927: [FE18] ==> [3017]
      u8"\xe2\x80\x94\0"                     // #2928: [FE31, FE58] ==> [2014]
      u8"\xe2\x80\x93\0"                     // #2929: [FE32] ==> [2013]
      u8"\x5f\0"                             // #2930: [FE33, FE34, FE4D, FE4E, FE4F, FF3F] ==> [5F]
      u8"\x7b\0"                             // #2931: [FE37, FE5B, FF5B] ==> [7B]
      u8"\x7d\0"                             // #2932: [FE38, FE5C, FF5D] ==> [7D]
      u8"\xe3\x80\x94\0"                     // #2933: [FE39, FE5D] ==> [3014]
      u8"\xe3\x80\x95\0"                     // #2934: [FE3A, FE5E] ==> [3015]
      u8"\xe3\x80\x90\0"                     // #2935: [FE3B] ==> [3010]
      u8"\xe3\x80\x91\0"                     // #2936: [FE3C] ==> [3011]
      u8"\xe3\x80\x8a\0"                     // #2937: [FE3D] ==> [300A]
      u8"\xe3\x80\x8b\0"                     // #2938: [FE3E] ==> [300B]
      u8"\xe3\x80\x8c\0"                     // #2939: [FE41, FF62] ==> [300C]
      u8"\xe3\x80\x8d\0"                     // #2940: [FE42, FF63] ==> [300D]
      u8"\xe3\x80\x8e\0"                     // #2941: [FE43] ==> [300E]
      u8"\xe3\x80\x8f\0"                     // #2942: [FE44] ==> [300F]
      u8"\x5b\0"                             // #2943: [FE47, FF3B] ==> [5B]
      u8"\x5d\0"                             // #2944: [FE48, FF3D] ==> [5D]
      u8"\43\0"                              // #2945: [FE5F, FF03] ==> [23]
      u8"\46\0"                              // #2946: [FE60, FF06] ==> [26]
      u8"\52\0"                              // #2947: [FE61, FF0A] ==> [2A]
      u8"\55\0"                              // #2948: [FE63, FF0D] ==> [2D]
      u8"\74\0"                              // #2949: [FE64, FF1C] ==> [3C]
      u8"\76\0"                              // #2950: [FE65, FF1E] ==> [3E]
      u8"\x5c\0"                             // #2951: [FE68, FF3C] ==> [5C]
      u8"\44\0"                              // #2952: [FE69, FF04] ==> [24]
      u8"\45\0"                              // #2953: [FE6A, FF05] ==> [25]
      u8"\x40\0"                             // #2954: [FE6B, FF20] ==> [40]
      u8"\40\xd9\x8b\0"                      // #2955: [FE70] ==> [20, 64B]
      u8"\xd9\x80\xd9\x8b\0"                 // #2956: [FE71] ==> [640, 64B]
      u8"\40\xd9\x8c\0"                      // #2957: [FE72] ==> [20, 64C]
      u8"\40\xd9\x8d\0"                      // #2958: [FE74] ==> [20, 64D]
      u8"\40\xd9\x8e\0"                      // #2959: [FE76] ==> [20, 64E]
      u8"\xd9\x80\xd9\x8e\0"                 // #2960: [FE77] ==> [640, 64E]
      u8"\40\xd9\x8f\0"                      // #2961: [FE78] ==> [20, 64F]
      u8"\xd9\x80\xd9\x8f\0"                 // #2962: [FE79] ==> [640, 64F]
      u8"\40\xd9\x90\0"                      // #2963: [FE7A] ==> [20, 650]
      u8"\xd9\x80\xd9\x90\0"                 // #2964: [FE7B] ==> [640, 650]
      u8"\40\xd9\x91\0"                      // #2965: [FE7C] ==> [20, 651]
      u8"\xd9\x80\xd9\x91\0"                 // #2966: [FE7D] ==> [640, 651]
      u8"\40\xd9\x92\0"                      // #2967: [FE7E] ==> [20, 652]
      u8"\xd9\x80\xd9\x92\0"                 // #2968: [FE7F] ==> [640, 652]
      u8"\xd8\xa1\0"                         // #2969: [FE80] ==> [621]
      u8"\xd8\xa2\0"                         // #2970: [FE81, FE82] ==> [622]
      u8"\xd8\xa3\0"                         // #2971: [FE83, FE84] ==> [623]
      u8"\xd8\xa4\0"                         // #2972: [FE85, FE86] ==> [624]
      u8"\xd8\xa5\0"                         // #2973: [FE87, FE88] ==> [625]
      u8"\xd8\xa6\0"                         // #2974: [FE89, FE8A, FE8B, FE8C] ==> [626]
      u8"\xd8\xa7\0"                         // #2975: [FE8D, FE8E, 1EE00, 1EE80] ==> [627]
      u8"\xd8\xa8\0" // #2976: [FE8F, FE90, FE91, FE92, 1EE01, 1EE21, 1EE61, 1EE81, 1EEA1] ==> [628]
      u8"\xd8\xa9\0" // #2977: [FE93, FE94] ==> [629]
      u8"\xd8\xaa\0" // #2978: [FE95, FE96, FE97, FE98, 1EE15, 1EE35, 1EE75, 1EE95, 1EEB5] ==> [62A]
      u8"\xd8\xab\0" // #2979: [FE99, FE9A, FE9B, FE9C, 1EE16, 1EE36, 1EE76, 1EE96, 1EEB6] ==> [62B]
      u8"\xd8\xac\0" // #2980: [FE9D, FE9E, FE9F, FEA0, 1EE02, 1EE22, 1EE42, 1EE62, 1EE82, 1EEA2] ==> [62C]
      u8"\xd8\xad\0" // #2981: [FEA1, FEA2, FEA3, FEA4, 1EE07, 1EE27, 1EE47, 1EE67, 1EE87, 1EEA7] ==> [62D]
      u8"\xd8\xae\0" // #2982: [FEA5, FEA6, FEA7, FEA8, 1EE17, 1EE37, 1EE57, 1EE77, 1EE97, 1EEB7] ==> [62E]
      u8"\xd8\xaf\0" // #2983: [FEA9, FEAA, 1EE03, 1EE83, 1EEA3] ==> [62F]
      u8"\xd8\xb0\0" // #2984: [FEAB, FEAC, 1EE18, 1EE98, 1EEB8] ==> [630]
      u8"\xd8\xb1\0" // #2985: [FEAD, FEAE, 1EE13, 1EE93, 1EEB3] ==> [631]
      u8"\xd8\xb2\0" // #2986: [FEAF, FEB0, 1EE06, 1EE86, 1EEA6] ==> [632]
      u8"\xd8\xb3\0" // #2987: [FEB1, FEB2, FEB3, FEB4, 1EE0E, 1EE2E, 1EE4E, 1EE6E, 1EE8E, 1EEAE] ==> [633]
      u8"\xd8\xb4\0" // #2988: [FEB5, FEB6, FEB7, FEB8, 1EE14, 1EE34, 1EE54, 1EE74, 1EE94, 1EEB4] ==> [634]
      u8"\xd8\xb5\0" // #2989: [FEB9, FEBA, FEBB, FEBC, 1EE11, 1EE31, 1EE51, 1EE71, 1EE91, 1EEB1] ==> [635]
      u8"\xd8\xb6\0" // #2990: [FEBD, FEBE, FEBF, FEC0, 1EE19, 1EE39, 1EE59, 1EE79, 1EE99, 1EEB9] ==> [636]
      u8"\xd8\xb7\0" // #2991: [FEC1, FEC2, FEC3, FEC4, 1EE08, 1EE68, 1EE88, 1EEA8] ==> [637]
      u8"\xd8\xb8\0" // #2992: [FEC5, FEC6, FEC7, FEC8, 1EE1A, 1EE7A, 1EE9A, 1EEBA] ==> [638]
      u8"\xd8\xb9\0" // #2993: [FEC9, FECA, FECB, FECC, 1EE0F, 1EE2F, 1EE4F, 1EE6F, 1EE8F, 1EEAF] ==> [639]
      u8"\xd8\xba\0" // #2994: [FECD, FECE, FECF, FED0, 1EE1B, 1EE3B, 1EE5B, 1EE7B, 1EE9B, 1EEBB] ==> [63A]
      u8"\xd9\x81\0" // #2995: [FED1, FED2, FED3, FED4, 1EE10, 1EE30, 1EE70, 1EE90, 1EEB0] ==> [641]
      u8"\xd9\x82\0" // #2996: [FED5, FED6, FED7, FED8, 1EE12, 1EE32, 1EE52, 1EE72, 1EE92, 1EEB2] ==> [642]
      u8"\xd9\x83\0" // #2997: [FED9, FEDA, FEDB, FEDC, 1EE0A, 1EE2A, 1EE6A] ==> [643]
      u8"\xd9\x84\0" // #2998: [FEDD, FEDE, FEDF, FEE0, 1EE0B, 1EE2B, 1EE4B, 1EE8B, 1EEAB] ==> [644]
      u8"\xd9\x85\0" // #2999: [FEE1, FEE2, FEE3, FEE4, 1EE0C, 1EE2C, 1EE6C, 1EE8C, 1EEAC] ==> [645]
      u8"\xd9\x86\0" // #3000: [FEE5, FEE6, FEE7, FEE8, 1EE0D, 1EE2D, 1EE4D, 1EE6D, 1EE8D, 1EEAD] ==> [646]
      u8"\xd9\x87\0" // #3001: [FEE9, FEEA, FEEB, FEEC, 1EE24, 1EE64, 1EE84] ==> [647]
      u8"\xd9\x88\0" // #3002: [FEED, FEEE, 1EE05, 1EE85, 1EEA5] ==> [648]
      u8"\xd9\x8a\0" // #3003: [FEF1, FEF2, FEF3, FEF4, 1EE09, 1EE29, 1EE49, 1EE69, 1EE89, 1EEA9] ==> [64A]
      u8"\xd9\x84\xd8\xa2\0"                                 // #3004: [FEF5, FEF6] ==> [644, 622]
      u8"\xd9\x84\xd8\xa3\0"                                 // #3005: [FEF7, FEF8] ==> [644, 623]
      u8"\xd9\x84\xd8\xa5\0"                                 // #3006: [FEF9, FEFA] ==> [644, 625]
      u8"\xd9\x84\xd8\xa7\0"                                 // #3007: [FEFB, FEFC] ==> [644, 627]
      u8"\42\0"                                              // #3008: [FF02] ==> [22]
      u8"\47\0"                                              // #3009: [FF07] ==> [27]
      u8"\57\0"                                              // #3010: [FF0F] ==> [2F]
      u8"\x5e\0"                                             // #3011: [FF3E] ==> [5E]
      u8"\x7c\0"                                             // #3012: [FF5C] ==> [7C]
      u8"\x7e\0"                                             // #3013: [FF5E] ==> [7E]
      u8"\xe2\xa6\x85\0"                                     // #3014: [FF5F] ==> [2985]
      u8"\xe2\xa6\x86\0"                                     // #3015: [FF60] ==> [2986]
      u8"\xe3\x83\xbb\0"                                     // #3016: [FF65] ==> [30FB]
      u8"\xe3\x82\xa1\0"                                     // #3017: [FF67] ==> [30A1]
      u8"\xe3\x82\xa3\0"                                     // #3018: [FF68] ==> [30A3]
      u8"\xe3\x82\xa5\0"                                     // #3019: [FF69] ==> [30A5]
      u8"\xe3\x82\xa7\0"                                     // #3020: [FF6A] ==> [30A7]
      u8"\xe3\x82\xa9\0"                                     // #3021: [FF6B] ==> [30A9]
      u8"\xe3\x83\xa3\0"                                     // #3022: [FF6C] ==> [30E3]
      u8"\xe3\x83\xa5\0"                                     // #3023: [FF6D] ==> [30E5]
      u8"\xe3\x83\xa7\0"                                     // #3024: [FF6E] ==> [30E7]
      u8"\xe3\x83\x83\0"                                     // #3025: [FF6F] ==> [30C3]
      u8"\xe3\x83\xbc\0"                                     // #3026: [FF70] ==> [30FC]
      u8"\xe3\x83\xb3\0"                                     // #3027: [FF9D] ==> [30F3]
      u8"\xe3\x82\x99\0"                                     // #3028: [FF9E] ==> [3099]
      u8"\xe3\x82\x9a\0"                                     // #3029: [FF9F] ==> [309A]
      u8"\xc2\xa2\0"                                         // #3030: [FFE0] ==> [A2]
      u8"\xc2\xa3\0"                                         // #3031: [FFE1] ==> [A3]
      u8"\xc2\xac\0"                                         // #3032: [FFE2] ==> [AC]
      u8"\xc2\xa6\0"                                         // #3033: [FFE4] ==> [A6]
      u8"\xc2\xa5\0"                                         // #3034: [FFE5] ==> [A5]
      u8"\xe2\x82\xa9\0"                                     // #3035: [FFE6] ==> [20A9]
      u8"\xe2\x94\x82\0"                                     // #3036: [FFE8] ==> [2502]
      u8"\xe2\x86\x90\0"                                     // #3037: [FFE9] ==> [2190]
      u8"\xe2\x86\x91\0"                                     // #3038: [FFEA] ==> [2191]
      u8"\xe2\x86\x92\0"                                     // #3039: [FFEB] ==> [2192]
      u8"\xe2\x86\x93\0"                                     // #3040: [FFEC] ==> [2193]
      u8"\xe2\x96\xa0\0"                                     // #3041: [FFED] ==> [25A0]
      u8"\xe2\x97\x8b\0"                                     // #3042: [FFEE] ==> [25CB]
      u8"\xf0\x90\x90\xa8\0"                                 // #3043: [10400] ==> [10428]
      u8"\xf0\x90\x90\xa9\0"                                 // #3044: [10401] ==> [10429]
      u8"\xf0\x90\x90\xaa\0"                                 // #3045: [10402] ==> [1042A]
      u8"\xf0\x90\x90\xab\0"                                 // #3046: [10403] ==> [1042B]
      u8"\xf0\x90\x90\xac\0"                                 // #3047: [10404] ==> [1042C]
      u8"\xf0\x90\x90\xad\0"                                 // #3048: [10405] ==> [1042D]
      u8"\xf0\x90\x90\xae\0"                                 // #3049: [10406] ==> [1042E]
      u8"\xf0\x90\x90\xaf\0"                                 // #3050: [10407] ==> [1042F]
      u8"\xf0\x90\x90\xb0\0"                                 // #3051: [10408] ==> [10430]
      u8"\xf0\x90\x90\xb1\0"                                 // #3052: [10409] ==> [10431]
      u8"\xf0\x90\x90\xb2\0"                                 // #3053: [1040A] ==> [10432]
      u8"\xf0\x90\x90\xb3\0"                                 // #3054: [1040B] ==> [10433]
      u8"\xf0\x90\x90\xb4\0"                                 // #3055: [1040C] ==> [10434]
      u8"\xf0\x90\x90\xb5\0"                                 // #3056: [1040D] ==> [10435]
      u8"\xf0\x90\x90\xb6\0"                                 // #3057: [1040E] ==> [10436]
      u8"\xf0\x90\x90\xb7\0"                                 // #3058: [1040F] ==> [10437]
      u8"\xf0\x90\x90\xb8\0"                                 // #3059: [10410] ==> [10438]
      u8"\xf0\x90\x90\xb9\0"                                 // #3060: [10411] ==> [10439]
      u8"\xf0\x90\x90\xba\0"                                 // #3061: [10412] ==> [1043A]
      u8"\xf0\x90\x90\xbb\0"                                 // #3062: [10413] ==> [1043B]
      u8"\xf0\x90\x90\xbc\0"                                 // #3063: [10414] ==> [1043C]
      u8"\xf0\x90\x90\xbd\0"                                 // #3064: [10415] ==> [1043D]
      u8"\xf0\x90\x90\xbe\0"                                 // #3065: [10416] ==> [1043E]
      u8"\xf0\x90\x90\xbf\0"                                 // #3066: [10417] ==> [1043F]
      u8"\xf0\x90\x91\x80\0"                                 // #3067: [10418] ==> [10440]
      u8"\xf0\x90\x91\x81\0"                                 // #3068: [10419] ==> [10441]
      u8"\xf0\x90\x91\x82\0"                                 // #3069: [1041A] ==> [10442]
      u8"\xf0\x90\x91\x83\0"                                 // #3070: [1041B] ==> [10443]
      u8"\xf0\x90\x91\x84\0"                                 // #3071: [1041C] ==> [10444]
      u8"\xf0\x90\x91\x85\0"                                 // #3072: [1041D] ==> [10445]
      u8"\xf0\x90\x91\x86\0"                                 // #3073: [1041E] ==> [10446]
      u8"\xf0\x90\x91\x87\0"                                 // #3074: [1041F] ==> [10447]
      u8"\xf0\x90\x91\x88\0"                                 // #3075: [10420] ==> [10448]
      u8"\xf0\x90\x91\x89\0"                                 // #3076: [10421] ==> [10449]
      u8"\xf0\x90\x91\x8a\0"                                 // #3077: [10422] ==> [1044A]
      u8"\xf0\x90\x91\x8b\0"                                 // #3078: [10423] ==> [1044B]
      u8"\xf0\x90\x91\x8c\0"                                 // #3079: [10424] ==> [1044C]
      u8"\xf0\x90\x91\x8d\0"                                 // #3080: [10425] ==> [1044D]
      u8"\xf0\x90\x91\x8e\0"                                 // #3081: [10426] ==> [1044E]
      u8"\xf0\x90\x91\x8f\0"                                 // #3082: [10427] ==> [1044F]
      u8"\xf0\x90\x93\x98\0"                                 // #3083: [104B0] ==> [104D8]
      u8"\xf0\x90\x93\x99\0"                                 // #3084: [104B1] ==> [104D9]
      u8"\xf0\x90\x93\x9a\0"                                 // #3085: [104B2] ==> [104DA]
      u8"\xf0\x90\x93\x9b\0"                                 // #3086: [104B3] ==> [104DB]
      u8"\xf0\x90\x93\x9c\0"                                 // #3087: [104B4] ==> [104DC]
      u8"\xf0\x90\x93\x9d\0"                                 // #3088: [104B5] ==> [104DD]
      u8"\xf0\x90\x93\x9e\0"                                 // #3089: [104B6] ==> [104DE]
      u8"\xf0\x90\x93\x9f\0"                                 // #3090: [104B7] ==> [104DF]
      u8"\xf0\x90\x93\xa0\0"                                 // #3091: [104B8] ==> [104E0]
      u8"\xf0\x90\x93\xa1\0"                                 // #3092: [104B9] ==> [104E1]
      u8"\xf0\x90\x93\xa2\0"                                 // #3093: [104BA] ==> [104E2]
      u8"\xf0\x90\x93\xa3\0"                                 // #3094: [104BB] ==> [104E3]
      u8"\xf0\x90\x93\xa4\0"                                 // #3095: [104BC] ==> [104E4]
      u8"\xf0\x90\x93\xa5\0"                                 // #3096: [104BD] ==> [104E5]
      u8"\xf0\x90\x93\xa6\0"                                 // #3097: [104BE] ==> [104E6]
      u8"\xf0\x90\x93\xa7\0"                                 // #3098: [104BF] ==> [104E7]
      u8"\xf0\x90\x93\xa8\0"                                 // #3099: [104C0] ==> [104E8]
      u8"\xf0\x90\x93\xa9\0"                                 // #3100: [104C1] ==> [104E9]
      u8"\xf0\x90\x93\xaa\0"                                 // #3101: [104C2] ==> [104EA]
      u8"\xf0\x90\x93\xab\0"                                 // #3102: [104C3] ==> [104EB]
      u8"\xf0\x90\x93\xac\0"                                 // #3103: [104C4] ==> [104EC]
      u8"\xf0\x90\x93\xad\0"                                 // #3104: [104C5] ==> [104ED]
      u8"\xf0\x90\x93\xae\0"                                 // #3105: [104C6] ==> [104EE]
      u8"\xf0\x90\x93\xaf\0"                                 // #3106: [104C7] ==> [104EF]
      u8"\xf0\x90\x93\xb0\0"                                 // #3107: [104C8] ==> [104F0]
      u8"\xf0\x90\x93\xb1\0"                                 // #3108: [104C9] ==> [104F1]
      u8"\xf0\x90\x93\xb2\0"                                 // #3109: [104CA] ==> [104F2]
      u8"\xf0\x90\x93\xb3\0"                                 // #3110: [104CB] ==> [104F3]
      u8"\xf0\x90\x93\xb4\0"                                 // #3111: [104CC] ==> [104F4]
      u8"\xf0\x90\x93\xb5\0"                                 // #3112: [104CD] ==> [104F5]
      u8"\xf0\x90\x93\xb6\0"                                 // #3113: [104CE] ==> [104F6]
      u8"\xf0\x90\x93\xb7\0"                                 // #3114: [104CF] ==> [104F7]
      u8"\xf0\x90\x93\xb8\0"                                 // #3115: [104D0] ==> [104F8]
      u8"\xf0\x90\x93\xb9\0"                                 // #3116: [104D1] ==> [104F9]
      u8"\xf0\x90\x93\xba\0"                                 // #3117: [104D2] ==> [104FA]
      u8"\xf0\x90\x93\xbb\0"                                 // #3118: [104D3] ==> [104FB]
      u8"\xf0\x90\x96\x97\0"                                 // #3119: [10570] ==> [10597]
      u8"\xf0\x90\x96\x98\0"                                 // #3120: [10571] ==> [10598]
      u8"\xf0\x90\x96\x99\0"                                 // #3121: [10572] ==> [10599]
      u8"\xf0\x90\x96\x9a\0"                                 // #3122: [10573] ==> [1059A]
      u8"\xf0\x90\x96\x9b\0"                                 // #3123: [10574] ==> [1059B]
      u8"\xf0\x90\x96\x9c\0"                                 // #3124: [10575] ==> [1059C]
      u8"\xf0\x90\x96\x9d\0"                                 // #3125: [10576] ==> [1059D]
      u8"\xf0\x90\x96\x9e\0"                                 // #3126: [10577] ==> [1059E]
      u8"\xf0\x90\x96\x9f\0"                                 // #3127: [10578] ==> [1059F]
      u8"\xf0\x90\x96\xa0\0"                                 // #3128: [10579] ==> [105A0]
      u8"\xf0\x90\x96\xa1\0"                                 // #3129: [1057A] ==> [105A1]
      u8"\xf0\x90\x96\xa3\0"                                 // #3130: [1057C] ==> [105A3]
      u8"\xf0\x90\x96\xa4\0"                                 // #3131: [1057D] ==> [105A4]
      u8"\xf0\x90\x96\xa5\0"                                 // #3132: [1057E] ==> [105A5]
      u8"\xf0\x90\x96\xa6\0"                                 // #3133: [1057F] ==> [105A6]
      u8"\xf0\x90\x96\xa7\0"                                 // #3134: [10580] ==> [105A7]
      u8"\xf0\x90\x96\xa8\0"                                 // #3135: [10581] ==> [105A8]
      u8"\xf0\x90\x96\xa9\0"                                 // #3136: [10582] ==> [105A9]
      u8"\xf0\x90\x96\xaa\0"                                 // #3137: [10583] ==> [105AA]
      u8"\xf0\x90\x96\xab\0"                                 // #3138: [10584] ==> [105AB]
      u8"\xf0\x90\x96\xac\0"                                 // #3139: [10585] ==> [105AC]
      u8"\xf0\x90\x96\xad\0"                                 // #3140: [10586] ==> [105AD]
      u8"\xf0\x90\x96\xae\0"                                 // #3141: [10587] ==> [105AE]
      u8"\xf0\x90\x96\xaf\0"                                 // #3142: [10588] ==> [105AF]
      u8"\xf0\x90\x96\xb0\0"                                 // #3143: [10589] ==> [105B0]
      u8"\xf0\x90\x96\xb1\0"                                 // #3144: [1058A] ==> [105B1]
      u8"\xf0\x90\x96\xb3\0"                                 // #3145: [1058C] ==> [105B3]
      u8"\xf0\x90\x96\xb4\0"                                 // #3146: [1058D] ==> [105B4]
      u8"\xf0\x90\x96\xb5\0"                                 // #3147: [1058E] ==> [105B5]
      u8"\xf0\x90\x96\xb6\0"                                 // #3148: [1058F] ==> [105B6]
      u8"\xf0\x90\x96\xb7\0"                                 // #3149: [10590] ==> [105B7]
      u8"\xf0\x90\x96\xb8\0"                                 // #3150: [10591] ==> [105B8]
      u8"\xf0\x90\x96\xb9\0"                                 // #3151: [10592] ==> [105B9]
      u8"\xf0\x90\x96\xbb\0"                                 // #3152: [10594] ==> [105BB]
      u8"\xf0\x90\x96\xbc\0"                                 // #3153: [10595] ==> [105BC]
      u8"\xcb\x90\0"                                         // #3154: [10781] ==> [2D0]
      u8"\xcb\x91\0"                                         // #3155: [10782] ==> [2D1]
      u8"\xca\x99\0"                                         // #3156: [10784] ==> [299]
      u8"\xca\xa3\0"                                         // #3157: [10787] ==> [2A3]
      u8"\xea\xad\xa6\0"                                     // #3158: [10788] ==> [AB66]
      u8"\xca\xa5\0"                                         // #3159: [10789] ==> [2A5]
      u8"\xca\xa4\0"                                         // #3160: [1078A] ==> [2A4]
      u8"\xe1\xb6\x91\0"                                     // #3161: [1078D] ==> [1D91]
      u8"\xc9\x98\0"                                         // #3162: [1078E] ==> [258]
      u8"\xc9\x9e\0"                                         // #3163: [1078F] ==> [25E]
      u8"\xca\xa9\0"                                         // #3164: [10790] ==> [2A9]
      u8"\xc9\xa2\0"                                         // #3165: [10792] ==> [262]
      u8"\xca\x9b\0"                                         // #3166: [10794] ==> [29B]
      u8"\xca\x9c\0"                                         // #3167: [10796] ==> [29C]
      u8"\xc9\xa7\0"                                         // #3168: [10797] ==> [267]
      u8"\xca\x84\0"                                         // #3169: [10798] ==> [284]
      u8"\xca\xaa\0"                                         // #3170: [10799] ==> [2AA]
      u8"\xca\xab\0"                                         // #3171: [1079A] ==> [2AB]
      u8"\xf0\x9d\xbc\x84\0"                                 // #3172: [1079C] ==> [1DF04]
      u8"\xea\x9e\x8e\0"                                     // #3173: [1079D] ==> [A78E]
      u8"\xc9\xae\0"                                         // #3174: [1079E] ==> [26E]
      u8"\xf0\x9d\xbc\x85\0"                                 // #3175: [1079F] ==> [1DF05]
      u8"\xca\x8e\0"                                         // #3176: [107A0] ==> [28E]
      u8"\xf0\x9d\xbc\x86\0"                                 // #3177: [107A1] ==> [1DF06]
      u8"\xc9\xb6\0"                                         // #3178: [107A3] ==> [276]
      u8"\xc9\xb7\0"                                         // #3179: [107A4] ==> [277]
      u8"\xc9\xba\0"                                         // #3180: [107A6] ==> [27A]
      u8"\xf0\x9d\xbc\x88\0"                                 // #3181: [107A7] ==> [1DF08]
      u8"\xc9\xbe\0"                                         // #3182: [107A9] ==> [27E]
      u8"\xca\xa8\0"                                         // #3183: [107AB] ==> [2A8]
      u8"\xca\xa6\0"                                         // #3184: [107AC] ==> [2A6]
      u8"\xea\xad\xa7\0"                                     // #3185: [107AD] ==> [AB67]
      u8"\xca\xa7\0"                                         // #3186: [107AE] ==> [2A7]
      u8"\xe2\xb1\xb1\0"                                     // #3187: [107B0] ==> [2C71]
      u8"\xca\x8f\0"                                         // #3188: [107B2] ==> [28F]
      u8"\xca\xa1\0"                                         // #3189: [107B3] ==> [2A1]
      u8"\xca\xa2\0"                                         // #3190: [107B4] ==> [2A2]
      u8"\xca\x98\0"                                         // #3191: [107B5] ==> [298]
      u8"\xc7\x80\0"                                         // #3192: [107B6] ==> [1C0]
      u8"\xc7\x81\0"                                         // #3193: [107B7] ==> [1C1]
      u8"\xc7\x82\0"                                         // #3194: [107B8] ==> [1C2]
      u8"\xf0\x9d\xbc\x8a\0"                                 // #3195: [107B9] ==> [1DF0A]
      u8"\xf0\x9d\xbc\x9e\0"                                 // #3196: [107BA] ==> [1DF1E]
      u8"\xf0\x90\xb3\x80\0"                                 // #3197: [10C80] ==> [10CC0]
      u8"\xf0\x90\xb3\x81\0"                                 // #3198: [10C81] ==> [10CC1]
      u8"\xf0\x90\xb3\x82\0"                                 // #3199: [10C82] ==> [10CC2]
      u8"\xf0\x90\xb3\x83\0"                                 // #3200: [10C83] ==> [10CC3]
      u8"\xf0\x90\xb3\x84\0"                                 // #3201: [10C84] ==> [10CC4]
      u8"\xf0\x90\xb3\x85\0"                                 // #3202: [10C85] ==> [10CC5]
      u8"\xf0\x90\xb3\x86\0"                                 // #3203: [10C86] ==> [10CC6]
      u8"\xf0\x90\xb3\x87\0"                                 // #3204: [10C87] ==> [10CC7]
      u8"\xf0\x90\xb3\x88\0"                                 // #3205: [10C88] ==> [10CC8]
      u8"\xf0\x90\xb3\x89\0"                                 // #3206: [10C89] ==> [10CC9]
      u8"\xf0\x90\xb3\x8a\0"                                 // #3207: [10C8A] ==> [10CCA]
      u8"\xf0\x90\xb3\x8b\0"                                 // #3208: [10C8B] ==> [10CCB]
      u8"\xf0\x90\xb3\x8c\0"                                 // #3209: [10C8C] ==> [10CCC]
      u8"\xf0\x90\xb3\x8d\0"                                 // #3210: [10C8D] ==> [10CCD]
      u8"\xf0\x90\xb3\x8e\0"                                 // #3211: [10C8E] ==> [10CCE]
      u8"\xf0\x90\xb3\x8f\0"                                 // #3212: [10C8F] ==> [10CCF]
      u8"\xf0\x90\xb3\x90\0"                                 // #3213: [10C90] ==> [10CD0]
      u8"\xf0\x90\xb3\x91\0"                                 // #3214: [10C91] ==> [10CD1]
      u8"\xf0\x90\xb3\x92\0"                                 // #3215: [10C92] ==> [10CD2]
      u8"\xf0\x90\xb3\x93\0"                                 // #3216: [10C93] ==> [10CD3]
      u8"\xf0\x90\xb3\x94\0"                                 // #3217: [10C94] ==> [10CD4]
      u8"\xf0\x90\xb3\x95\0"                                 // #3218: [10C95] ==> [10CD5]
      u8"\xf0\x90\xb3\x96\0"                                 // #3219: [10C96] ==> [10CD6]
      u8"\xf0\x90\xb3\x97\0"                                 // #3220: [10C97] ==> [10CD7]
      u8"\xf0\x90\xb3\x98\0"                                 // #3221: [10C98] ==> [10CD8]
      u8"\xf0\x90\xb3\x99\0"                                 // #3222: [10C99] ==> [10CD9]
      u8"\xf0\x90\xb3\x9a\0"                                 // #3223: [10C9A] ==> [10CDA]
      u8"\xf0\x90\xb3\x9b\0"                                 // #3224: [10C9B] ==> [10CDB]
      u8"\xf0\x90\xb3\x9c\0"                                 // #3225: [10C9C] ==> [10CDC]
      u8"\xf0\x90\xb3\x9d\0"                                 // #3226: [10C9D] ==> [10CDD]
      u8"\xf0\x90\xb3\x9e\0"                                 // #3227: [10C9E] ==> [10CDE]
      u8"\xf0\x90\xb3\x9f\0"                                 // #3228: [10C9F] ==> [10CDF]
      u8"\xf0\x90\xb3\xa0\0"                                 // #3229: [10CA0] ==> [10CE0]
      u8"\xf0\x90\xb3\xa1\0"                                 // #3230: [10CA1] ==> [10CE1]
      u8"\xf0\x90\xb3\xa2\0"                                 // #3231: [10CA2] ==> [10CE2]
      u8"\xf0\x90\xb3\xa3\0"                                 // #3232: [10CA3] ==> [10CE3]
      u8"\xf0\x90\xb3\xa4\0"                                 // #3233: [10CA4] ==> [10CE4]
      u8"\xf0\x90\xb3\xa5\0"                                 // #3234: [10CA5] ==> [10CE5]
      u8"\xf0\x90\xb3\xa6\0"                                 // #3235: [10CA6] ==> [10CE6]
      u8"\xf0\x90\xb3\xa7\0"                                 // #3236: [10CA7] ==> [10CE7]
      u8"\xf0\x90\xb3\xa8\0"                                 // #3237: [10CA8] ==> [10CE8]
      u8"\xf0\x90\xb3\xa9\0"                                 // #3238: [10CA9] ==> [10CE9]
      u8"\xf0\x90\xb3\xaa\0"                                 // #3239: [10CAA] ==> [10CEA]
      u8"\xf0\x90\xb3\xab\0"                                 // #3240: [10CAB] ==> [10CEB]
      u8"\xf0\x90\xb3\xac\0"                                 // #3241: [10CAC] ==> [10CEC]
      u8"\xf0\x90\xb3\xad\0"                                 // #3242: [10CAD] ==> [10CED]
      u8"\xf0\x90\xb3\xae\0"                                 // #3243: [10CAE] ==> [10CEE]
      u8"\xf0\x90\xb3\xaf\0"                                 // #3244: [10CAF] ==> [10CEF]
      u8"\xf0\x90\xb3\xb0\0"                                 // #3245: [10CB0] ==> [10CF0]
      u8"\xf0\x90\xb3\xb1\0"                                 // #3246: [10CB1] ==> [10CF1]
      u8"\xf0\x90\xb3\xb2\0"                                 // #3247: [10CB2] ==> [10CF2]
      u8"\xf0\x90\xb5\xb0\0"                                 // #3248: [10D50] ==> [10D70]
      u8"\xf0\x90\xb5\xb1\0"                                 // #3249: [10D51] ==> [10D71]
      u8"\xf0\x90\xb5\xb2\0"                                 // #3250: [10D52] ==> [10D72]
      u8"\xf0\x90\xb5\xb3\0"                                 // #3251: [10D53] ==> [10D73]
      u8"\xf0\x90\xb5\xb4\0"                                 // #3252: [10D54] ==> [10D74]
      u8"\xf0\x90\xb5\xb5\0"                                 // #3253: [10D55] ==> [10D75]
      u8"\xf0\x90\xb5\xb6\0"                                 // #3254: [10D56] ==> [10D76]
      u8"\xf0\x90\xb5\xb7\0"                                 // #3255: [10D57] ==> [10D77]
      u8"\xf0\x90\xb5\xb8\0"                                 // #3256: [10D58] ==> [10D78]
      u8"\xf0\x90\xb5\xb9\0"                                 // #3257: [10D59] ==> [10D79]
      u8"\xf0\x90\xb5\xba\0"                                 // #3258: [10D5A] ==> [10D7A]
      u8"\xf0\x90\xb5\xbb\0"                                 // #3259: [10D5B] ==> [10D7B]
      u8"\xf0\x90\xb5\xbc\0"                                 // #3260: [10D5C] ==> [10D7C]
      u8"\xf0\x90\xb5\xbd\0"                                 // #3261: [10D5D] ==> [10D7D]
      u8"\xf0\x90\xb5\xbe\0"                                 // #3262: [10D5E] ==> [10D7E]
      u8"\xf0\x90\xb5\xbf\0"                                 // #3263: [10D5F] ==> [10D7F]
      u8"\xf0\x90\xb6\x80\0"                                 // #3264: [10D60] ==> [10D80]
      u8"\xf0\x90\xb6\x81\0"                                 // #3265: [10D61] ==> [10D81]
      u8"\xf0\x90\xb6\x82\0"                                 // #3266: [10D62] ==> [10D82]
      u8"\xf0\x90\xb6\x83\0"                                 // #3267: [10D63] ==> [10D83]
      u8"\xf0\x90\xb6\x84\0"                                 // #3268: [10D64] ==> [10D84]
      u8"\xf0\x90\xb6\x85\0"                                 // #3269: [10D65] ==> [10D85]
      u8"\xf0\x91\xa3\x80\0"                                 // #3270: [118A0] ==> [118C0]
      u8"\xf0\x91\xa3\x81\0"                                 // #3271: [118A1] ==> [118C1]
      u8"\xf0\x91\xa3\x82\0"                                 // #3272: [118A2] ==> [118C2]
      u8"\xf0\x91\xa3\x83\0"                                 // #3273: [118A3] ==> [118C3]
      u8"\xf0\x91\xa3\x84\0"                                 // #3274: [118A4] ==> [118C4]
      u8"\xf0\x91\xa3\x85\0"                                 // #3275: [118A5] ==> [118C5]
      u8"\xf0\x91\xa3\x86\0"                                 // #3276: [118A6] ==> [118C6]
      u8"\xf0\x91\xa3\x87\0"                                 // #3277: [118A7] ==> [118C7]
      u8"\xf0\x91\xa3\x88\0"                                 // #3278: [118A8] ==> [118C8]
      u8"\xf0\x91\xa3\x89\0"                                 // #3279: [118A9] ==> [118C9]
      u8"\xf0\x91\xa3\x8a\0"                                 // #3280: [118AA] ==> [118CA]
      u8"\xf0\x91\xa3\x8b\0"                                 // #3281: [118AB] ==> [118CB]
      u8"\xf0\x91\xa3\x8c\0"                                 // #3282: [118AC] ==> [118CC]
      u8"\xf0\x91\xa3\x8d\0"                                 // #3283: [118AD] ==> [118CD]
      u8"\xf0\x91\xa3\x8e\0"                                 // #3284: [118AE] ==> [118CE]
      u8"\xf0\x91\xa3\x8f\0"                                 // #3285: [118AF] ==> [118CF]
      u8"\xf0\x91\xa3\x90\0"                                 // #3286: [118B0] ==> [118D0]
      u8"\xf0\x91\xa3\x91\0"                                 // #3287: [118B1] ==> [118D1]
      u8"\xf0\x91\xa3\x92\0"                                 // #3288: [118B2] ==> [118D2]
      u8"\xf0\x91\xa3\x93\0"                                 // #3289: [118B3] ==> [118D3]
      u8"\xf0\x91\xa3\x94\0"                                 // #3290: [118B4] ==> [118D4]
      u8"\xf0\x91\xa3\x95\0"                                 // #3291: [118B5] ==> [118D5]
      u8"\xf0\x91\xa3\x96\0"                                 // #3292: [118B6] ==> [118D6]
      u8"\xf0\x91\xa3\x97\0"                                 // #3293: [118B7] ==> [118D7]
      u8"\xf0\x91\xa3\x98\0"                                 // #3294: [118B8] ==> [118D8]
      u8"\xf0\x91\xa3\x99\0"                                 // #3295: [118B9] ==> [118D9]
      u8"\xf0\x91\xa3\x9a\0"                                 // #3296: [118BA] ==> [118DA]
      u8"\xf0\x91\xa3\x9b\0"                                 // #3297: [118BB] ==> [118DB]
      u8"\xf0\x91\xa3\x9c\0"                                 // #3298: [118BC] ==> [118DC]
      u8"\xf0\x91\xa3\x9d\0"                                 // #3299: [118BD] ==> [118DD]
      u8"\xf0\x91\xa3\x9e\0"                                 // #3300: [118BE] ==> [118DE]
      u8"\xf0\x91\xa3\x9f\0"                                 // #3301: [118BF] ==> [118DF]
      u8"\xf0\x96\xb9\xa0\0"                                 // #3302: [16E40] ==> [16E60]
      u8"\xf0\x96\xb9\xa1\0"                                 // #3303: [16E41] ==> [16E61]
      u8"\xf0\x96\xb9\xa2\0"                                 // #3304: [16E42] ==> [16E62]
      u8"\xf0\x96\xb9\xa3\0"                                 // #3305: [16E43] ==> [16E63]
      u8"\xf0\x96\xb9\xa4\0"                                 // #3306: [16E44] ==> [16E64]
      u8"\xf0\x96\xb9\xa5\0"                                 // #3307: [16E45] ==> [16E65]
      u8"\xf0\x96\xb9\xa6\0"                                 // #3308: [16E46] ==> [16E66]
      u8"\xf0\x96\xb9\xa7\0"                                 // #3309: [16E47] ==> [16E67]
      u8"\xf0\x96\xb9\xa8\0"                                 // #3310: [16E48] ==> [16E68]
      u8"\xf0\x96\xb9\xa9\0"                                 // #3311: [16E49] ==> [16E69]
      u8"\xf0\x96\xb9\xaa\0"                                 // #3312: [16E4A] ==> [16E6A]
      u8"\xf0\x96\xb9\xab\0"                                 // #3313: [16E4B] ==> [16E6B]
      u8"\xf0\x96\xb9\xac\0"                                 // #3314: [16E4C] ==> [16E6C]
      u8"\xf0\x96\xb9\xad\0"                                 // #3315: [16E4D] ==> [16E6D]
      u8"\xf0\x96\xb9\xae\0"                                 // #3316: [16E4E] ==> [16E6E]
      u8"\xf0\x96\xb9\xaf\0"                                 // #3317: [16E4F] ==> [16E6F]
      u8"\xf0\x96\xb9\xb0\0"                                 // #3318: [16E50] ==> [16E70]
      u8"\xf0\x96\xb9\xb1\0"                                 // #3319: [16E51] ==> [16E71]
      u8"\xf0\x96\xb9\xb2\0"                                 // #3320: [16E52] ==> [16E72]
      u8"\xf0\x96\xb9\xb3\0"                                 // #3321: [16E53] ==> [16E73]
      u8"\xf0\x96\xb9\xb4\0"                                 // #3322: [16E54] ==> [16E74]
      u8"\xf0\x96\xb9\xb5\0"                                 // #3323: [16E55] ==> [16E75]
      u8"\xf0\x96\xb9\xb6\0"                                 // #3324: [16E56] ==> [16E76]
      u8"\xf0\x96\xb9\xb7\0"                                 // #3325: [16E57] ==> [16E77]
      u8"\xf0\x96\xb9\xb8\0"                                 // #3326: [16E58] ==> [16E78]
      u8"\xf0\x96\xb9\xb9\0"                                 // #3327: [16E59] ==> [16E79]
      u8"\xf0\x96\xb9\xba\0"                                 // #3328: [16E5A] ==> [16E7A]
      u8"\xf0\x96\xb9\xbb\0"                                 // #3329: [16E5B] ==> [16E7B]
      u8"\xf0\x96\xb9\xbc\0"                                 // #3330: [16E5C] ==> [16E7C]
      u8"\xf0\x96\xb9\xbd\0"                                 // #3331: [16E5D] ==> [16E7D]
      u8"\xf0\x96\xb9\xbe\0"                                 // #3332: [16E5E] ==> [16E7E]
      u8"\xf0\x96\xb9\xbf\0"                                 // #3333: [16E5F] ==> [16E7F]
      u8"\xf0\x9d\x85\x97\xf0\x9d\x85\xa5\0"                 // #3334: [1D15E] ==> [1D157, 1D165]
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\0"                 // #3335: [1D15F] ==> [1D158, 1D165]
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0" // #3336: [1D160] ==> [1D158, 1D165, 1D16E]
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0" // #3337: [1D161] ==> [1D158, 1D165, 1D16F]
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb0\0" // #3338: [1D162] ==> [1D158, 1D165, 1D170]
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb1\0" // #3339: [1D163] ==> [1D158, 1D165, 1D171]
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb2\0" // #3340: [1D164] ==> [1D158, 1D165, 1D172]
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\0"                 // #3341: [1D1BB] ==> [1D1B9, 1D165]
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\0"                 // #3342: [1D1BC] ==> [1D1BA, 1D165]
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0" // #3343: [1D1BD] ==> [1D1B9, 1D165, 1D16E]
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0" // #3344: [1D1BE] ==> [1D1BA, 1D165, 1D16E]
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0" // #3345: [1D1BF] ==> [1D1B9, 1D165, 1D16F]
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0" // #3346: [1D1C0] ==> [1D1BA, 1D165, 1D16F]
      u8"\xc4\xb1\0"                                         // #3347: [1D6A4] ==> [131]
      u8"\xc8\xb7\0"                                         // #3348: [1D6A5] ==> [237]
      u8"\xe2\x88\x87\0"                         // #3349: [1D6C1, 1D6FB, 1D735, 1D76F, 1D7A9] ==> [2207]
      u8"\xe2\x88\x82\0"                         // #3350: [1D6DB, 1D715, 1D74F, 1D789, 1D7C3] ==> [2202]
      u8"\xf0\x9e\xa4\xa2\0"                     // #3351: [1E900] ==> [1E922]
      u8"\xf0\x9e\xa4\xa3\0"                     // #3352: [1E901] ==> [1E923]
      u8"\xf0\x9e\xa4\xa4\0"                     // #3353: [1E902] ==> [1E924]
      u8"\xf0\x9e\xa4\xa5\0"                     // #3354: [1E903] ==> [1E925]
      u8"\xf0\x9e\xa4\xa6\0"                     // #3355: [1E904] ==> [1E926]
      u8"\xf0\x9e\xa4\xa7\0"                     // #3356: [1E905] ==> [1E927]
      u8"\xf0\x9e\xa4\xa8\0"                     // #3357: [1E906] ==> [1E928]
      u8"\xf0\x9e\xa4\xa9\0"                     // #3358: [1E907] ==> [1E929]
      u8"\xf0\x9e\xa4\xaa\0"                     // #3359: [1E908] ==> [1E92A]
      u8"\xf0\x9e\xa4\xab\0"                     // #3360: [1E909] ==> [1E92B]
      u8"\xf0\x9e\xa4\xac\0"                     // #3361: [1E90A] ==> [1E92C]
      u8"\xf0\x9e\xa4\xad\0"                     // #3362: [1E90B] ==> [1E92D]
      u8"\xf0\x9e\xa4\xae\0"                     // #3363: [1E90C] ==> [1E92E]
      u8"\xf0\x9e\xa4\xaf\0"                     // #3364: [1E90D] ==> [1E92F]
      u8"\xf0\x9e\xa4\xb0\0"                     // #3365: [1E90E] ==> [1E930]
      u8"\xf0\x9e\xa4\xb1\0"                     // #3366: [1E90F] ==> [1E931]
      u8"\xf0\x9e\xa4\xb2\0"                     // #3367: [1E910] ==> [1E932]
      u8"\xf0\x9e\xa4\xb3\0"                     // #3368: [1E911] ==> [1E933]
      u8"\xf0\x9e\xa4\xb4\0"                     // #3369: [1E912] ==> [1E934]
      u8"\xf0\x9e\xa4\xb5\0"                     // #3370: [1E913] ==> [1E935]
      u8"\xf0\x9e\xa4\xb6\0"                     // #3371: [1E914] ==> [1E936]
      u8"\xf0\x9e\xa4\xb7\0"                     // #3372: [1E915] ==> [1E937]
      u8"\xf0\x9e\xa4\xb8\0"                     // #3373: [1E916] ==> [1E938]
      u8"\xf0\x9e\xa4\xb9\0"                     // #3374: [1E917] ==> [1E939]
      u8"\xf0\x9e\xa4\xba\0"                     // #3375: [1E918] ==> [1E93A]
      u8"\xf0\x9e\xa4\xbb\0"                     // #3376: [1E919] ==> [1E93B]
      u8"\xf0\x9e\xa4\xbc\0"                     // #3377: [1E91A] ==> [1E93C]
      u8"\xf0\x9e\xa4\xbd\0"                     // #3378: [1E91B] ==> [1E93D]
      u8"\xf0\x9e\xa4\xbe\0"                     // #3379: [1E91C] ==> [1E93E]
      u8"\xf0\x9e\xa4\xbf\0"                     // #3380: [1E91D] ==> [1E93F]
      u8"\xf0\x9e\xa5\x80\0"                     // #3381: [1E91E] ==> [1E940]
      u8"\xf0\x9e\xa5\x81\0"                     // #3382: [1E91F] ==> [1E941]
      u8"\xf0\x9e\xa5\x82\0"                     // #3383: [1E920] ==> [1E942]
      u8"\xf0\x9e\xa5\x83\0"                     // #3384: [1E921] ==> [1E943]
      u8"\xd9\xae\0"                             // #3385: [1EE1C, 1EE7C] ==> [66E]
      u8"\xda\xa1\0"                             // #3386: [1EE1E, 1EE7E] ==> [6A1]
      u8"\xd9\xaf\0"                             // #3387: [1EE1F, 1EE5F] ==> [66F]
      u8"\60\54\0"                               // #3388: [1F101] ==> [30, 2C]
      u8"\61\54\0"                               // #3389: [1F102] ==> [31, 2C]
      u8"\62\54\0"                               // #3390: [1F103] ==> [32, 2C]
      u8"\63\54\0"                               // #3391: [1F104] ==> [33, 2C]
      u8"\64\54\0"                               // #3392: [1F105] ==> [34, 2C]
      u8"\65\54\0"                               // #3393: [1F106] ==> [35, 2C]
      u8"\66\54\0"                               // #3394: [1F107] ==> [36, 2C]
      u8"\67\54\0"                               // #3395: [1F108] ==> [37, 2C]
      u8"\70\54\0"                               // #3396: [1F109] ==> [38, 2C]
      u8"\71\54\0"                               // #3397: [1F10A] ==> [39, 2C]
      u8"\xe3\x80\x94\x73\xe3\x80\x95\0"         // #3398: [1F12A] ==> [3014, 73, 3015]
      u8"\x77\x7a\0"                             // #3399: [1F12E] ==> [77, 7A]
      u8"\x68\x76\0"                             // #3400: [1F14A] ==> [68, 76]
      u8"\x73\x64\0"                             // #3401: [1F14C] ==> [73, 64]
      u8"\x73\x73\0"                             // #3402: [1F14D] ==> [73, 73]
      u8"\x70\x70\x76\0"                         // #3403: [1F14E] ==> [70, 70, 76]
      u8"\x77\x63\0"                             // #3404: [1F14F] ==> [77, 63]
      u8"\x6d\x63\0"                             // #3405: [1F16A] ==> [6D, 63]
      u8"\x6d\x64\0"                             // #3406: [1F16B] ==> [6D, 64]
      u8"\x6d\x72\0"                             // #3407: [1F16C] ==> [6D, 72]
      u8"\x64\x6a\0"                             // #3408: [1F190] ==> [64, 6A]
      u8"\xe3\x81\xbb\xe3\x81\x8b\0"             // #3409: [1F200] ==> [307B, 304B]
      u8"\xe3\x82\xb3\xe3\x82\xb3\0"             // #3410: [1F201] ==> [30B3, 30B3]
      u8"\xe5\xad\x97\0"                         // #3411: [1F211] ==> [5B57]
      u8"\xe5\x8f\x8c\0"                         // #3412: [1F212] ==> [53CC]
      u8"\xe3\x83\x87\0"                         // #3413: [1F213] ==> [30C7]
      u8"\xe5\xa4\x9a\0"                         // #3414: [1F215, 2F85D] ==> [591A]
      u8"\xe8\xa7\xa3\0"                         // #3415: [1F216] ==> [89E3]
      u8"\xe4\xba\xa4\0"                         // #3416: [1F218] ==> [4EA4]
      u8"\xe6\x98\xa0\0"                         // #3417: [1F219] ==> [6620]
      u8"\xe7\x84\xa1\0"                         // #3418: [1F21A] ==> [7121]
      u8"\xe5\x89\x8d\0"                         // #3419: [1F21C] ==> [524D]
      u8"\xe5\xbe\x8c\0"                         // #3420: [1F21D] ==> [5F8C]
      u8"\xe5\x86\x8d\0"                         // #3421: [1F21E, 2F815] ==> [518D]
      u8"\xe6\x96\xb0\0"                         // #3422: [1F21F] ==> [65B0]
      u8"\xe5\x88\x9d\0"                         // #3423: [1F220] ==> [521D]
      u8"\xe7\xb5\x82\0"                         // #3424: [1F221] ==> [7D42]
      u8"\xe8\xb2\xa9\0"                         // #3425: [1F223] ==> [8CA9]
      u8"\xe5\xa3\xb0\0"                         // #3426: [1F224] ==> [58F0]
      u8"\xe5\x90\xb9\0"                         // #3427: [1F225] ==> [5439]
      u8"\xe6\xbc\x94\0"                         // #3428: [1F226] ==> [6F14]
      u8"\xe6\x8a\x95\0"                         // #3429: [1F227] ==> [6295]
      u8"\xe6\x8d\x95\0"                         // #3430: [1F228] ==> [6355]
      u8"\xe9\x81\x8a\0"                         // #3431: [1F22B] ==> [904A]
      u8"\xe6\x8c\x87\0"                         // #3432: [1F22F] ==> [6307]
      u8"\xe6\x89\x93\0"                         // #3433: [1F231] ==> [6253]
      u8"\xe7\xa6\x81\0"                         // #3434: [1F232] ==> [7981]
      u8"\xe7\xa9\xba\0"                         // #3435: [1F233] ==> [7A7A]
      u8"\xe5\x90\x88\0"                         // #3436: [1F234] ==> [5408]
      u8"\xe6\xba\x80\0"                         // #3437: [1F235] ==> [6E80]
      u8"\xe7\x94\xb3\0"                         // #3438: [1F238] ==> [7533]
      u8"\xe5\x89\xb2\0"                         // #3439: [1F239, 2F822] ==> [5272]
      u8"\xe5\x96\xb6\0"                         // #3440: [1F23A] ==> [55B6]
      u8"\xe9\x85\x8d\0"                         // #3441: [1F23B] ==> [914D]
      u8"\xe3\x80\x94\xe6\x9c\xac\xe3\x80\x95\0" // #3442: [1F240] ==> [3014, 672C, 3015]
      u8"\xe3\x80\x94\xe4\xb8\x89\xe3\x80\x95\0" // #3443: [1F241] ==> [3014, 4E09, 3015]
      u8"\xe3\x80\x94\xe4\xba\x8c\xe3\x80\x95\0" // #3444: [1F242] ==> [3014, 4E8C, 3015]
      u8"\xe3\x80\x94\xe5\xae\x89\xe3\x80\x95\0" // #3445: [1F243] ==> [3014, 5B89, 3015]
      u8"\xe3\x80\x94\xe7\x82\xb9\xe3\x80\x95\0" // #3446: [1F244] ==> [3014, 70B9, 3015]
      u8"\xe3\x80\x94\xe6\x89\x93\xe3\x80\x95\0" // #3447: [1F245] ==> [3014, 6253, 3015]
      u8"\xe3\x80\x94\xe7\x9b\x97\xe3\x80\x95\0" // #3448: [1F246] ==> [3014, 76D7, 3015]
      u8"\xe3\x80\x94\xe5\x8b\x9d\xe3\x80\x95\0" // #3449: [1F247] ==> [3014, 52DD, 3015]
      u8"\xe3\x80\x94\xe6\x95\x97\xe3\x80\x95\0" // #3450: [1F248] ==> [3014, 6557, 3015]
      u8"\xe5\xbe\x97\0"                         // #3451: [1F250] ==> [5F97]
      u8"\xe5\x8f\xaf\0"                         // #3452: [1F251] ==> [53EF]
      u8"\xe4\xb8\xbd\0"                         // #3453: [2F800] ==> [4E3D]
      u8"\xe4\xb8\xb8\0"                         // #3454: [2F801] ==> [4E38]
      u8"\xe4\xb9\x81\0"                         // #3455: [2F802] ==> [4E41]
      u8"\xf0\xa0\x84\xa2\0"                     // #3456: [2F803] ==> [20122]
      u8"\xe4\xbd\xa0\0"                         // #3457: [2F804] ==> [4F60]
      u8"\xe4\xbe\xbb\0"                         // #3458: [2F806] ==> [4FBB]
      u8"\xe5\x80\x82\0"                         // #3459: [2F807] ==> [5002]
      u8"\xe5\x81\xba\0"                         // #3460: [2F808] ==> [507A]
      u8"\xe5\x82\x99\0"                         // #3461: [2F809] ==> [5099]
      u8"\xe5\x83\x8f\0"                         // #3462: [2F80B] ==> [50CF]
      u8"\xe3\x92\x9e\0"                         // #3463: [2F80C] ==> [349E]
      u8"\xf0\xa0\x98\xba\0"                     // #3464: [2F80D] ==> [2063A]
      u8"\xe5\x85\x94\0"                         // #3465: [2F80F] ==> [5154]
      u8"\xe5\x85\xa4\0"                         // #3466: [2F810] ==> [5164]
      u8"\xe5\x85\xb7\0"                         // #3467: [2F811] ==> [5177]
      u8"\xf0\xa0\x94\x9c\0"                     // #3468: [2F812] ==> [2051C]
      u8"\xe3\x92\xb9\0"                         // #3469: [2F813] ==> [34B9]
      u8"\xe5\x85\xa7\0"                         // #3470: [2F814] ==> [5167]
      u8"\xf0\xa0\x95\x8b\0"                     // #3471: [2F816] ==> [2054B]
      u8"\xe5\x86\x97\0"                         // #3472: [2F817] ==> [5197]
      u8"\xe5\x86\xa4\0"                         // #3473: [2F818] ==> [51A4]
      u8"\xe4\xbb\x8c\0"                         // #3474: [2F819] ==> [4ECC]
      u8"\xe5\x86\xac\0"                         // #3475: [2F81A] ==> [51AC]
      u8"\xf0\xa9\x87\x9f\0"                     // #3476: [2F81C] ==> [291DF]
      u8"\xe5\x88\x83\0"                         // #3477: [2F81E] ==> [5203]
      u8"\xe3\x93\x9f\0"                         // #3478: [2F81F] ==> [34DF]
      u8"\xe5\x88\xbb\0"                         // #3479: [2F820] ==> [523B]
      u8"\xe5\x89\x86\0"                         // #3480: [2F821] ==> [5246]
      u8"\xe5\x89\xb7\0"                         // #3481: [2F823] ==> [5277]
      u8"\xe3\x94\x95\0"                         // #3482: [2F824] ==> [3515]
      u8"\xe5\x8c\x85\0"                         // #3483: [2F829] ==> [5305]
      u8"\xe5\x8c\x86\0"                         // #3484: [2F82A] ==> [5306]
      u8"\xe5\x8d\x89\0"                         // #3485: [2F82C] ==> [5349]
      u8"\xe5\x8d\x9a\0"                         // #3486: [2F82E] ==> [535A]
      u8"\xe5\x8d\xb3\0"                         // #3487: [2F82F] ==> [5373]
      u8"\xe5\x8d\xbd\0"                         // #3488: [2F830] ==> [537D]
      u8"\xe5\x8d\xbf\0"                         // #3489: [2F831, 2F832, 2F833] ==> [537F]
      u8"\xf0\xa0\xa8\xac\0"                     // #3490: [2F834] ==> [20A2C]
      u8"\xe7\x81\xb0\0"                         // #3491: [2F835] ==> [7070]
      u8"\xe5\x8f\x8a\0"                         // #3492: [2F836] ==> [53CA]
      u8"\xe5\x8f\x9f\0"                         // #3493: [2F837] ==> [53DF]
      u8"\xf0\xa0\xad\xa3\0"                     // #3494: [2F838] ==> [20B63]
      u8"\xe5\x8f\xab\0"                         // #3495: [2F839] ==> [53EB]
      u8"\xe5\x8f\xb1\0"                         // #3496: [2F83A] ==> [53F1]
      u8"\xe5\x90\x86\0"                         // #3497: [2F83B] ==> [5406]
      u8"\xe5\x92\x9e\0"                         // #3498: [2F83C] ==> [549E]
      u8"\xe5\x90\xb8\0"                         // #3499: [2F83D] ==> [5438]
      u8"\xe5\x91\x88\0"                         // #3500: [2F83E] ==> [5448]
      u8"\xe5\x91\xa8\0"                         // #3501: [2F83F] ==> [5468]
      u8"\xe5\x92\xa2\0"                         // #3502: [2F840] ==> [54A2]
      u8"\xe5\x93\xb6\0"                         // #3503: [2F841] ==> [54F6]
      u8"\xe5\x94\x90\0"                         // #3504: [2F842] ==> [5510]
      u8"\xe5\x95\x93\0"                         // #3505: [2F843] ==> [5553]
      u8"\xe5\x95\xa3\0"                         // #3506: [2F844] ==> [5563]
      u8"\xe5\x96\x84\0"                         // #3507: [2F845, 2F846] ==> [5584]
      u8"\xe5\x96\xab\0"                         // #3508: [2F848] ==> [55AB]
      u8"\xe5\x96\xb3\0"                         // #3509: [2F849] ==> [55B3]
      u8"\xe5\x97\x82\0"                         // #3510: [2F84A] ==> [55C2]
      u8"\xe5\x9c\x96\0"                         // #3511: [2F84B] ==> [5716]
      u8"\xe5\x9c\x97\0"                         // #3512: [2F84D] ==> [5717]
      u8"\xe5\x99\x91\0"                         // #3513: [2F84E] ==> [5651]
      u8"\xe5\x99\xb4\0"                         // #3514: [2F84F] ==> [5674]
      u8"\xe5\xa3\xae\0"                         // #3515: [2F851] ==> [58EE]
      u8"\xe5\x9f\x8e\0"                         // #3516: [2F852] ==> [57CE]
      u8"\xe5\x9f\xb4\0"                         // #3517: [2F853] ==> [57F4]
      u8"\xe5\xa0\x8d\0"                         // #3518: [2F854] ==> [580D]
      u8"\xe5\x9e\x8b\0"                         // #3519: [2F855] ==> [578B]
      u8"\xe5\xa0\xb2\0"                         // #3520: [2F856] ==> [5832]
      u8"\xe5\xa0\xb1\0"                         // #3521: [2F857] ==> [5831]
      u8"\xe5\xa2\xac\0"                         // #3522: [2F858] ==> [58AC]
      u8"\xf0\xa1\x93\xa4\0"                     // #3523: [2F859] ==> [214E4]
      u8"\xe5\xa3\xb2\0"                         // #3524: [2F85A] ==> [58F2]
      u8"\xe5\xa3\xb7\0"                         // #3525: [2F85B] ==> [58F7]
      u8"\xe5\xa4\x86\0"                         // #3526: [2F85C] ==> [5906]
      u8"\xe5\xa4\xa2\0"                         // #3527: [2F85E] ==> [5922]
      u8"\xe5\xa5\xa2\0"                         // #3528: [2F85F] ==> [5962]
      u8"\xf0\xa1\x9a\xa8\0"                     // #3529: [2F860] ==> [216A8]
      u8"\xf0\xa1\x9b\xaa\0"                     // #3530: [2F861] ==> [216EA]
      u8"\xe5\xa7\xac\0"                         // #3531: [2F862] ==> [59EC]
      u8"\xe5\xa8\x9b\0"                         // #3532: [2F863] ==> [5A1B]
      u8"\xe5\xa8\xa7\0"                         // #3533: [2F864] ==> [5A27]
      u8"\xe5\xa7\x98\0"                         // #3534: [2F865] ==> [59D8]
      u8"\xe5\xa9\xa6\0"                         // #3535: [2F866] ==> [5A66]
      u8"\xe3\x9b\xae\0"                         // #3536: [2F867] ==> [36EE]
      u8"\xe3\x9b\xbc\0"                         // #3537: [2F868] ==> [36FC]
      u8"\xe5\xac\x88\0"                         // #3538: [2F869] ==> [5B08]
      u8"\xe5\xac\xbe\0"                         // #3539: [2F86A, 2F86B] ==> [5B3E]
      u8"\xf0\xa1\xa7\x88\0"                     // #3540: [2F86C] ==> [219C8]
      u8"\xe5\xaf\x83\0"                         // #3541: [2F86D] ==> [5BC3]
      u8"\xe5\xaf\x98\0"                         // #3542: [2F86E] ==> [5BD8]
      u8"\xe5\xaf\xb3\0"                         // #3543: [2F870] ==> [5BF3]
      u8"\xf0\xa1\xac\x98\0"                     // #3544: [2F871] ==> [21B18]
      u8"\xe5\xaf\xbf\0"                         // #3545: [2F872] ==> [5BFF]
      u8"\xe5\xb0\x86\0"                         // #3546: [2F873] ==> [5C06]
      u8"\xe5\xbd\x93\0"                         // #3547: [2F874] ==> [5F53]
      u8"\xe3\x9e\x81\0"                         // #3548: [2F876] ==> [3781]
      u8"\xe5\xb1\xa0\0"                         // #3549: [2F877] ==> [5C60]
      u8"\xe5\xb3\x80\0"                         // #3550: [2F879] ==> [5CC0]
      u8"\xe5\xb2\x8d\0"                         // #3551: [2F87A] ==> [5C8D]
      u8"\xf0\xa1\xb7\xa4\0"                     // #3552: [2F87B] ==> [21DE4]
      u8"\xe5\xb5\x83\0"                         // #3553: [2F87C] ==> [5D43]
      u8"\xf0\xa1\xb7\xa6\0"                     // #3554: [2F87D] ==> [21DE6]
      u8"\xe5\xb5\xae\0"                         // #3555: [2F87E] ==> [5D6E]
      u8"\xe5\xb5\xab\0"                         // #3556: [2F87F] ==> [5D6B]
      u8"\xe5\xb5\xbc\0"                         // #3557: [2F880] ==> [5D7C]
      u8"\xe5\xb7\xa1\0"                         // #3558: [2F881] ==> [5DE1]
      u8"\xe5\xb7\xa2\0"                         // #3559: [2F882] ==> [5DE2]
      u8"\xe3\xa0\xaf\0"                         // #3560: [2F883] ==> [382F]
      u8"\xe5\xb7\xbd\0"                         // #3561: [2F884] ==> [5DFD]
      u8"\xe5\xb8\xa8\0"                         // #3562: [2F885] ==> [5E28]
      u8"\xe5\xb8\xbd\0"                         // #3563: [2F886] ==> [5E3D]
      u8"\xe5\xb9\xa9\0"                         // #3564: [2F887] ==> [5E69]
      u8"\xe3\xa1\xa2\0"                         // #3565: [2F888] ==> [3862]
      u8"\xf0\xa2\x86\x83\0"                     // #3566: [2F889] ==> [22183]
      u8"\xe3\xa1\xbc\0"                         // #3567: [2F88A] ==> [387C]
      u8"\xe5\xba\xb0\0"                         // #3568: [2F88B] ==> [5EB0]
      u8"\xe5\xba\xb3\0"                         // #3569: [2F88C] ==> [5EB3]
      u8"\xe5\xba\xb6\0"                         // #3570: [2F88D] ==> [5EB6]
      u8"\xf0\xaa\x8e\x92\0"                     // #3571: [2F88F] ==> [2A392]
      u8"\xf0\xa2\x8c\xb1\0"                     // #3572: [2F891, 2F892] ==> [22331]
      u8"\xe8\x88\x81\0"                         // #3573: [2F893, 2F98B] ==> [8201]
      u8"\xe5\xbc\xa2\0"                         // #3574: [2F894, 2F895] ==> [5F22]
      u8"\xe3\xa3\x87\0"                         // #3575: [2F896] ==> [38C7]
      u8"\xf0\xa3\x8a\xb8\0"                     // #3576: [2F897] ==> [232B8]
      u8"\xf0\xa6\x87\x9a\0"                     // #3577: [2F898] ==> [261DA]
      u8"\xe5\xbd\xa2\0"                         // #3578: [2F899] ==> [5F62]
      u8"\xe5\xbd\xab\0"                         // #3579: [2F89A] ==> [5F6B]
      u8"\xe3\xa3\xa3\0"                         // #3580: [2F89B] ==> [38E3]
      u8"\xe5\xbe\x9a\0"                         // #3581: [2F89C] ==> [5F9A]
      u8"\xe5\xbf\x8d\0"                         // #3582: [2F89D] ==> [5FCD]
      u8"\xe5\xbf\x97\0"                         // #3583: [2F89E] ==> [5FD7]
      u8"\xe5\xbf\xb9\0"                         // #3584: [2F89F] ==> [5FF9]
      u8"\xe6\x82\x81\0"                         // #3585: [2F8A0] ==> [6081]
      u8"\xe3\xa4\xba\0"                         // #3586: [2F8A1] ==> [393A]
      u8"\xe3\xa4\x9c\0"                         // #3587: [2F8A2] ==> [391C]
      u8"\xf0\xa2\x9b\x94\0"                     // #3588: [2F8A4] ==> [226D4]
      u8"\xe6\x83\x87\0"                         // #3589: [2F8A5] ==> [60C7]
      u8"\xe6\x85\x88\0"                         // #3590: [2F8A6] ==> [6148]
      u8"\xe6\x85\x8c\0"                         // #3591: [2F8A7, 2F8A9] ==> [614C]
      u8"\xe6\x85\xba\0"                         // #3592: [2F8AA] ==> [617A]
      u8"\xe6\x86\xb2\0"                         // #3593: [2F8AC] ==> [61B2]
      u8"\xe6\x86\xa4\0"                         // #3594: [2F8AD] ==> [61A4]
      u8"\xe6\x86\xaf\0"                         // #3595: [2F8AE] ==> [61AF]
      u8"\xe6\x87\x9e\0"                         // #3596: [2F8AF] ==> [61DE]
      u8"\xe6\x88\x90\0"                         // #3597: [2F8B2] ==> [6210]
      u8"\xe6\x88\x9b\0"                         // #3598: [2F8B3] ==> [621B]
      u8"\xe6\x89\x9d\0"                         // #3599: [2F8B4] ==> [625D]
      u8"\xe6\x8a\xb1\0"                         // #3600: [2F8B5] ==> [62B1]
      u8"\xe6\x8b\x94\0"                         // #3601: [2F8B6] ==> [62D4]
      u8"\xe6\x8d\x90\0"                         // #3602: [2F8B7] ==> [6350]
      u8"\xf0\xa2\xac\x8c\0"                     // #3603: [2F8B8] ==> [22B0C]
      u8"\xe6\x8c\xbd\0"                         // #3604: [2F8B9] ==> [633D]
      u8"\xe6\x8b\xbc\0"                         // #3605: [2F8BA] ==> [62FC]
      u8"\xe6\x8d\xa8\0"                         // #3606: [2F8BB] ==> [6368]
      u8"\xe6\x8e\x83\0"                         // #3607: [2F8BC] ==> [6383]
      u8"\xe6\x8f\xa4\0"                         // #3608: [2F8BD] ==> [63E4]
      u8"\xf0\xa2\xaf\xb1\0"                     // #3609: [2F8BE] ==> [22BF1]
      u8"\xe6\x90\xa2\0"                         // #3610: [2F8BF] ==> [6422]
      u8"\xe6\x8f\x85\0"                         // #3611: [2F8C0] ==> [63C5]
      u8"\xe6\x8e\xa9\0"                         // #3612: [2F8C1] ==> [63A9]
      u8"\xe3\xa8\xae\0"                         // #3613: [2F8C2] ==> [3A2E]
      u8"\xe6\x91\xa9\0"                         // #3614: [2F8C3] ==> [6469]
      u8"\xe6\x91\xbe\0"                         // #3615: [2F8C4] ==> [647E]
      u8"\xe6\x92\x9d\0"                         // #3616: [2F8C5] ==> [649D]
      u8"\xe6\x91\xb7\0"                         // #3617: [2F8C6] ==> [6477]
      u8"\xe3\xa9\xac\0"                         // #3618: [2F8C7] ==> [3A6C]
      u8"\xe6\x95\xac\0"                         // #3619: [2F8C9] ==> [656C]
      u8"\xf0\xa3\x80\x8a\0"                     // #3620: [2F8CA] ==> [2300A]
      u8"\xe6\x97\xa3\0"                         // #3621: [2F8CB] ==> [65E3]
      u8"\xe6\x9b\xb8\0"                         // #3622: [2F8CC] ==> [66F8]
      u8"\xe6\x99\x89\0"                         // #3623: [2F8CD] ==> [6649]
      u8"\xe3\xac\x99\0"                         // #3624: [2F8CE] ==> [3B19]
      u8"\xe3\xac\x88\0"                         // #3625: [2F8D0] ==> [3B08]
      u8"\xe3\xab\xa4\0"                         // #3626: [2F8D1] ==> [3AE4]
      u8"\xe5\x86\x92\0"                         // #3627: [2F8D2] ==> [5192]
      u8"\xe5\x86\x95\0"                         // #3628: [2F8D3] ==> [5195]
      u8"\xe6\x9c\x80\0"                         // #3629: [2F8D4] ==> [6700]
      u8"\xe6\x9a\x9c\0"                         // #3630: [2F8D5] ==> [669C]
      u8"\xe8\x82\xad\0"                         // #3631: [2F8D6] ==> [80AD]
      u8"\xe4\x8f\x99\0"                         // #3632: [2F8D7] ==> [43D9]
      u8"\xe6\x9c\xa1\0"                         // #3633: [2F8DA] ==> [6721]
      u8"\xe6\x9d\x9e\0"                         // #3634: [2F8DB] ==> [675E]
      u8"\xe6\x9d\x93\0"                         // #3635: [2F8DC] ==> [6753]
      u8"\xf0\xa3\x8f\x83\0"                     // #3636: [2F8DD] ==> [233C3]
      u8"\xe3\xad\x89\0"                         // #3637: [2F8DE] ==> [3B49]
      u8"\xe6\x9f\xba\0"                         // #3638: [2F8DF] ==> [67FA]
      u8"\xe6\x9e\x85\0"                         // #3639: [2F8E0] ==> [6785]
      u8"\xe6\xa1\x92\0"                         // #3640: [2F8E1] ==> [6852]
      u8"\xf0\xa3\x91\xad\0"                     // #3641: [2F8E3] ==> [2346D]
      u8"\xe6\xa2\x8e\0"                         // #3642: [2F8E4] ==> [688E]
      u8"\xe6\xa0\x9f\0"                         // #3643: [2F8E5] ==> [681F]
      u8"\xe6\xa4\x94\0"                         // #3644: [2F8E6] ==> [6914]
      u8"\xe6\xa5\x82\0"                         // #3645: [2F8E8] ==> [6942]
      u8"\xe6\xa6\xa3\0"                         // #3646: [2F8E9] ==> [69A3]
      u8"\xe6\xa7\xaa\0"                         // #3647: [2F8EA] ==> [69EA]
      u8"\xe6\xaa\xa8\0"                         // #3648: [2F8EB] ==> [6AA8]
      u8"\xf0\xa3\x9a\xa3\0"                     // #3649: [2F8EC] ==> [236A3]
      u8"\xe6\xab\x9b\0"                         // #3650: [2F8ED] ==> [6ADB]
      u8"\xe3\xb0\x98\0"                         // #3651: [2F8EE] ==> [3C18]
      u8"\xe6\xac\xa1\0"                         // #3652: [2F8EF] ==> [6B21]
      u8"\xf0\xa3\xa2\xa7\0"                     // #3653: [2F8F0] ==> [238A7]
      u8"\xe6\xad\x94\0"                         // #3654: [2F8F1] ==> [6B54]
      u8"\xe3\xb1\x8e\0"                         // #3655: [2F8F2] ==> [3C4E]
      u8"\xe6\xad\xb2\0"                         // #3656: [2F8F3] ==> [6B72]
      u8"\xe6\xae\x9f\0"                         // #3657: [2F8F4] ==> [6B9F]
      u8"\xe6\xae\xbb\0"                         // #3658: [2F8F6] ==> [6BBB]
      u8"\xf0\xa3\xaa\x8d\0"                     // #3659: [2F8F7] ==> [23A8D]
      u8"\xf0\xa1\xb4\x8b\0"                     // #3660: [2F8F8] ==> [21D0B]
      u8"\xf0\xa3\xab\xba\0"                     // #3661: [2F8F9] ==> [23AFA]
      u8"\xe6\xb1\x8e\0"                         // #3662: [2F8FA] ==> [6C4E]
      u8"\xf0\xa3\xb2\xbc\0"                     // #3663: [2F8FB] ==> [23CBC]
      u8"\xe6\xb2\xbf\0"                         // #3664: [2F8FC] ==> [6CBF]
      u8"\xe6\xb3\x8d\0"                         // #3665: [2F8FD] ==> [6CCD]
      u8"\xe6\xb1\xa7\0"                         // #3666: [2F8FE] ==> [6C67]
      u8"\xe6\xb4\x96\0"                         // #3667: [2F8FF] ==> [6D16]
      u8"\xe6\xb4\xbe\0"                         // #3668: [2F900] ==> [6D3E]
      u8"\xe6\xb5\xa9\0"                         // #3669: [2F903] ==> [6D69]
      u8"\xe6\xb5\xb8\0"                         // #3670: [2F904] ==> [6D78]
      u8"\xe6\xb6\x85\0"                         // #3671: [2F905] ==> [6D85]
      u8"\xf0\xa3\xb4\x9e\0"                     // #3672: [2F906] ==> [23D1E]
      u8"\xe6\xb4\xb4\0"                         // #3673: [2F907] ==> [6D34]
      u8"\xe6\xb8\xaf\0"                         // #3674: [2F908] ==> [6E2F]
      u8"\xe6\xb9\xae\0"                         // #3675: [2F909] ==> [6E6E]
      u8"\xe3\xb4\xb3\0"                         // #3676: [2F90A] ==> [3D33]
      u8"\xe6\xbb\x87\0"                         // #3677: [2F90C] ==> [6EC7]
      u8"\xf0\xa3\xbb\x91\0"                     // #3678: [2F90D] ==> [23ED1]
      u8"\xe6\xb7\xb9\0"                         // #3679: [2F90E] ==> [6DF9]
      u8"\xe6\xbd\xae\0"                         // #3680: [2F90F] ==> [6F6E]
      u8"\xf0\xa3\xbd\x9e\0"                     // #3681: [2F910] ==> [23F5E]
      u8"\xf0\xa3\xbe\x8e\0"                     // #3682: [2F911] ==> [23F8E]
      u8"\xe6\xbf\x86\0"                         // #3683: [2F912] ==> [6FC6]
      u8"\xe7\x80\xb9\0"                         // #3684: [2F913] ==> [7039]
      u8"\xe7\x80\x9b\0"                         // #3685: [2F915] ==> [701B]
      u8"\xe3\xb6\x96\0"                         // #3686: [2F916] ==> [3D96]
      u8"\xe7\x81\x8a\0"                         // #3687: [2F917] ==> [704A]
      u8"\xe7\x81\xbd\0"                         // #3688: [2F918] ==> [707D]
      u8"\xe7\x81\xb7\0"                         // #3689: [2F919] ==> [7077]
      u8"\xe7\x82\xad\0"                         // #3690: [2F91A] ==> [70AD]
      u8"\xf0\xa0\x94\xa5\0"                     // #3691: [2F91B] ==> [20525]
      u8"\xe7\x85\x85\0"                         // #3692: [2F91C] ==> [7145]
      u8"\xf0\xa4\x89\xa3\0"                     // #3693: [2F91D] ==> [24263]
      u8"\xe7\x86\x9c\0"                         // #3694: [2F91E] ==> [719C]
      u8"\xf0\xa4\x8e\xab\0"                     // #3695: [2F91F] ==> [243AB]
      u8"\xe7\x88\xa8\0"                         // #3696: [2F920] ==> [7228]
      u8"\xe7\x89\x90\0"                         // #3697: [2F922] ==> [7250]
      u8"\xf0\xa4\x98\x88\0"                     // #3698: [2F923] ==> [24608]
      u8"\xe7\x8a\x80\0"                         // #3699: [2F924] ==> [7280]
      u8"\xe7\x8a\x95\0"                         // #3700: [2F925] ==> [7295]
      u8"\xf0\xa4\x9c\xb5\0"                     // #3701: [2F926] ==> [24735]
      u8"\xf0\xa4\xa0\x94\0"                     // #3702: [2F927] ==> [24814]
      u8"\xe7\x8d\xba\0"                         // #3703: [2F928] ==> [737A]
      u8"\xe7\x8e\x8b\0"                         // #3704: [2F929] ==> [738B]
      u8"\xe3\xba\xac\0"                         // #3705: [2F92A] ==> [3EAC]
      u8"\xe7\x8e\xa5\0"                         // #3706: [2F92B] ==> [73A5]
      u8"\xe3\xba\xb8\0"                         // #3707: [2F92C, 2F92D] ==> [3EB8]
      u8"\xe7\x91\x87\0"                         // #3708: [2F92E] ==> [7447]
      u8"\xe7\x91\x9c\0"                         // #3709: [2F92F] ==> [745C]
      u8"\xe7\x92\x85\0"                         // #3710: [2F931] ==> [7485]
      u8"\xe7\x93\x8a\0"                         // #3711: [2F932] ==> [74CA]
      u8"\xe3\xbc\x9b\0"                         // #3712: [2F933] ==> [3F1B]
      u8"\xe7\x94\xa4\0"                         // #3713: [2F934] ==> [7524]
      u8"\xf0\xa4\xb0\xb6\0"                     // #3714: [2F935] ==> [24C36]
      u8"\xe7\x94\xbe\0"                         // #3715: [2F936] ==> [753E]
      u8"\xf0\xa4\xb2\x92\0"                     // #3716: [2F937] ==> [24C92]
      u8"\xf0\xa2\x86\x9f\0"                     // #3717: [2F939] ==> [2219F]
      u8"\xe7\x98\x90\0"                         // #3718: [2F93A] ==> [7610]
      u8"\xf0\xa4\xbe\xa1\0"                     // #3719: [2F93B] ==> [24FA1]
      u8"\xf0\xa4\xbe\xb8\0"                     // #3720: [2F93C] ==> [24FB8]
      u8"\xf0\xa5\x81\x84\0"                     // #3721: [2F93D] ==> [25044]
      u8"\xe3\xbf\xbc\0"                         // #3722: [2F93E] ==> [3FFC]
      u8"\xe4\x80\x88\0"                         // #3723: [2F93F] ==> [4008]
      u8"\xf0\xa5\x83\xb3\0"                     // #3724: [2F941] ==> [250F3]
      u8"\xf0\xa5\x83\xb2\0"                     // #3725: [2F942] ==> [250F2]
      u8"\xf0\xa5\x84\x99\0"                     // #3726: [2F943] ==> [25119]
      u8"\xf0\xa5\x84\xb3\0"                     // #3727: [2F944] ==> [25133]
      u8"\xe7\x9c\x9e\0"                         // #3728: [2F945] ==> [771E]
      u8"\xe7\x9c\x9f\0"                         // #3729: [2F946, 2F947] ==> [771F]
      u8"\xe7\x9e\x8b\0"                         // #3730: [2F94A] ==> [778B]
      u8"\xe4\x81\x86\0"                         // #3731: [2F94B] ==> [4046]
      u8"\xe4\x82\x96\0"                         // #3732: [2F94C] ==> [4096]
      u8"\xf0\xa5\x90\x9d\0"                     // #3733: [2F94D] ==> [2541D]
      u8"\xe7\xa1\x8e\0"                         // #3734: [2F94E] ==> [784E]
      u8"\xe4\x83\xa3\0"                         // #3735: [2F951] ==> [40E3]
      u8"\xf0\xa5\x98\xa6\0"                     // #3736: [2F952] ==> [25626]
      u8"\xf0\xa5\x9a\x9a\0"                     // #3737: [2F954] ==> [2569A]
      u8"\xf0\xa5\x9b\x85\0"                     // #3738: [2F955] ==> [256C5]
      u8"\xe7\xa7\xab\0"                         // #3739: [2F957] ==> [79EB]
      u8"\xe4\x84\xaf\0"                         // #3740: [2F958] ==> [412F]
      u8"\xe7\xa9\x8a\0"                         // #3741: [2F95A] ==> [7A4A]
      u8"\xe7\xa9\x8f\0"                         // #3742: [2F95B] ==> [7A4F]
      u8"\xf0\xa5\xa5\xbc\0"                     // #3743: [2F95C] ==> [2597C]
      u8"\xf0\xa5\xaa\xa7\0"                     // #3744: [2F95D, 2F95E] ==> [25AA7]
      u8"\xe7\xab\xae\0"                         // #3745: [2F95F] ==> [7AEE]
      u8"\xe4\x88\x82\0"                         // #3746: [2F960] ==> [4202]
      u8"\xf0\xa5\xae\xab\0"                     // #3747: [2F961] ==> [25BAB]
      u8"\xe7\xaf\x86\0"                         // #3748: [2F962] ==> [7BC6]
      u8"\xe7\xaf\x89\0"                         // #3749: [2F963] ==> [7BC9]
      u8"\xe4\x88\xa7\0"                         // #3750: [2F964] ==> [4227]
      u8"\xf0\xa5\xb2\x80\0"                     // #3751: [2F965] ==> [25C80]
      u8"\xe7\xb3\x92\0"                         // #3752: [2F966] ==> [7CD2]
      u8"\xe4\x8a\xa0\0"                         // #3753: [2F967] ==> [42A0]
      u8"\xe7\xb3\xa8\0"                         // #3754: [2F968] ==> [7CE8]
      u8"\xe7\xb3\xa3\0"                         // #3755: [2F969] ==> [7CE3]
      u8"\xe7\xb4\x80\0"                         // #3756: [2F96A] ==> [7D00]
      u8"\xf0\xa5\xbe\x86\0"                     // #3757: [2F96B] ==> [25F86]
      u8"\xe7\xb5\xa3\0"                         // #3758: [2F96C] ==> [7D63]
      u8"\xe4\x8c\x81\0"                         // #3759: [2F96D] ==> [4301]
      u8"\xe7\xb7\x87\0"                         // #3760: [2F96E] ==> [7DC7]
      u8"\xe7\xb8\x82\0"                         // #3761: [2F96F] ==> [7E02]
      u8"\xe7\xb9\x85\0"                         // #3762: [2F970] ==> [7E45]
      u8"\xe4\x8c\xb4\0"                         // #3763: [2F971] ==> [4334]
      u8"\xf0\xa6\x88\xa8\0"                     // #3764: [2F972] ==> [26228]
      u8"\xf0\xa6\x89\x87\0"                     // #3765: [2F973] ==> [26247]
      u8"\xe4\x8d\x99\0"                         // #3766: [2F974] ==> [4359]
      u8"\xf0\xa6\x8b\x99\0"                     // #3767: [2F975] ==> [262D9]
      u8"\xe7\xbd\xba\0"                         // #3768: [2F976] ==> [7F7A]
      u8"\xf0\xa6\x8c\xbe\0"                     // #3769: [2F977] ==> [2633E]
      u8"\xe7\xbe\x95\0"                         // #3770: [2F978] ==> [7F95]
      u8"\xe7\xbf\xba\0"                         // #3771: [2F979] ==> [7FFA]
      u8"\xf0\xa6\x93\x9a\0"                     // #3772: [2F97B] ==> [264DA]
      u8"\xf0\xa6\x94\xa3\0"                     // #3773: [2F97C] ==> [26523]
      u8"\xe8\x81\xa0\0"                         // #3774: [2F97D] ==> [8060]
      u8"\xf0\xa6\x96\xa8\0"                     // #3775: [2F97E] ==> [265A8]
      u8"\xe8\x81\xb0\0"                         // #3776: [2F97F] ==> [8070]
      u8"\xf0\xa3\x8d\x9f\0"                     // #3777: [2F980] ==> [2335F]
      u8"\xe4\x8f\x95\0"                         // #3778: [2F981] ==> [43D5]
      u8"\xe8\x82\xb2\0"                         // #3779: [2F982] ==> [80B2]
      u8"\xe8\x84\x83\0"                         // #3780: [2F983] ==> [8103]
      u8"\xe4\x90\x8b\0"                         // #3781: [2F984] ==> [440B]
      u8"\xe8\x84\xbe\0"                         // #3782: [2F985] ==> [813E]
      u8"\xe5\xaa\xb5\0"                         // #3783: [2F986] ==> [5AB5]
      u8"\xf0\xa6\x9e\xa7\0"                     // #3784: [2F987] ==> [267A7]
      u8"\xf0\xa6\x9e\xb5\0"                     // #3785: [2F988] ==> [267B5]
      u8"\xf0\xa3\x8e\x93\0"                     // #3786: [2F989] ==> [23393]
      u8"\xf0\xa3\x8e\x9c\0"                     // #3787: [2F98A] ==> [2339C]
      u8"\xe8\x88\x84\0"                         // #3788: [2F98C] ==> [8204]
      u8"\xe8\xbe\x9e\0"                         // #3789: [2F98D] ==> [8F9E]
      u8"\xe4\x91\xab\0"                         // #3790: [2F98E] ==> [446B]
      u8"\xe8\x8a\x91\0"                         // #3791: [2F98F] ==> [8291]
      u8"\xe8\x8a\x8b\0"                         // #3792: [2F990] ==> [828B]
      u8"\xe8\x8a\x9d\0"                         // #3793: [2F991] ==> [829D]
      u8"\xe5\x8a\xb3\0"                         // #3794: [2F992] ==> [52B3]
      u8"\xe8\x8a\xb1\0"                         // #3795: [2F993] ==> [82B1]
      u8"\xe8\x8a\xb3\0"                         // #3796: [2F994] ==> [82B3]
      u8"\xe8\x8a\xbd\0"                         // #3797: [2F995] ==> [82BD]
      u8"\xe8\x8b\xa6\0"                         // #3798: [2F996] ==> [82E6]
      u8"\xf0\xa6\xac\xbc\0"                     // #3799: [2F997] ==> [26B3C]
      u8"\xe8\x8c\x9d\0"                         // #3800: [2F999] ==> [831D]
      u8"\xe8\x8d\xa3\0"                         // #3801: [2F99A] ==> [8363]
      u8"\xe8\x8e\xad\0"                         // #3802: [2F99B] ==> [83AD]
      u8"\xe8\x8c\xa3\0"                         // #3803: [2F99C] ==> [8323]
      u8"\xe8\x8e\xbd\0"                         // #3804: [2F99D] ==> [83BD]
      u8"\xe8\x8f\xa7\0"                         // #3805: [2F99E] ==> [83E7]
      u8"\xe8\x8d\x93\0"                         // #3806: [2F9A0] ==> [8353]
      u8"\xe8\x8f\x8a\0"                         // #3807: [2F9A1] ==> [83CA]
      u8"\xe8\x8f\x8c\0"                         // #3808: [2F9A2] ==> [83CC]
      u8"\xe8\x8f\x9c\0"                         // #3809: [2F9A3] ==> [83DC]
      u8"\xf0\xa6\xb0\xb6\0"                     // #3810: [2F9A4] ==> [26C36]
      u8"\xf0\xa6\xb5\xab\0"                     // #3811: [2F9A5] ==> [26D6B]
      u8"\xf0\xa6\xb3\x95\0"                     // #3812: [2F9A6] ==> [26CD5]
      u8"\xe4\x94\xab\0"                         // #3813: [2F9A7] ==> [452B]
      u8"\xe8\x93\xb1\0"                         // #3814: [2F9A8] ==> [84F1]
      u8"\xe8\x93\xb3\0"                         // #3815: [2F9A9] ==> [84F3]
      u8"\xe8\x94\x96\0"                         // #3816: [2F9AA] ==> [8516]
      u8"\xf0\xa7\x8f\x8a\0"                     // #3817: [2F9AB] ==> [273CA]
      u8"\xe8\x95\xa4\0"                         // #3818: [2F9AC] ==> [8564]
      u8"\xf0\xa6\xbc\xac\0"                     // #3819: [2F9AD] ==> [26F2C]
      u8"\xe4\x95\x9d\0"                         // #3820: [2F9AE] ==> [455D]
      u8"\xe4\x95\xa1\0"                         // #3821: [2F9AF] ==> [4561]
      u8"\xf0\xa6\xbe\xb1\0"                     // #3822: [2F9B0] ==> [26FB1]
      u8"\xf0\xa7\x83\x92\0"                     // #3823: [2F9B1] ==> [270D2]
      u8"\xe4\x95\xab\0"                         // #3824: [2F9B2] ==> [456B]
      u8"\xe8\x99\x90\0"                         // #3825: [2F9B3] ==> [8650]
      u8"\xe8\x99\xa7\0"                         // #3826: [2F9B5] ==> [8667]
      u8"\xe8\x99\xa9\0"                         // #3827: [2F9B6] ==> [8669]
      u8"\xe8\x9a\xa9\0"                         // #3828: [2F9B7] ==> [86A9]
      u8"\xe8\x9a\x88\0"                         // #3829: [2F9B8] ==> [8688]
      u8"\xe8\x9c\x8e\0"                         // #3830: [2F9B9] ==> [870E]
      u8"\xe8\x9b\xa2\0"                         // #3831: [2F9BA] ==> [86E2]
      u8"\xe8\x9c\xa8\0"                         // #3832: [2F9BC] ==> [8728]
      u8"\xe8\x9d\xab\0"                         // #3833: [2F9BD] ==> [876B]
      u8"\xe8\x9e\x86\0"                         // #3834: [2F9BE] ==> [8786]
      u8"\xe4\x97\x97\0"                         // #3835: [2F9BF] ==> [45D7]
      u8"\xe8\x9f\xa1\0"                         // #3836: [2F9C0] ==> [87E1]
      u8"\xe8\xa0\x81\0"                         // #3837: [2F9C1] ==> [8801]
      u8"\xe4\x97\xb9\0"                         // #3838: [2F9C2] ==> [45F9]
      u8"\xe8\xa1\xa0\0"                         // #3839: [2F9C3] ==> [8860]
      u8"\xf0\xa7\x99\xa7\0"                     // #3840: [2F9C5] ==> [27667]
      u8"\xe8\xa3\x97\0"                         // #3841: [2F9C6] ==> [88D7]
      u8"\xe8\xa3\x9e\0"                         // #3842: [2F9C7] ==> [88DE]
      u8"\xe4\x98\xb5\0"                         // #3843: [2F9C8] ==> [4635]
      u8"\xe8\xa3\xba\0"                         // #3844: [2F9C9] ==> [88FA]
      u8"\xe3\x92\xbb\0"                         // #3845: [2F9CA] ==> [34BB]
      u8"\xf0\xa7\xa2\xae\0"                     // #3846: [2F9CB] ==> [278AE]
      u8"\xf0\xa7\xa5\xa6\0"                     // #3847: [2F9CC] ==> [27966]
      u8"\xe4\x9a\xbe\0"                         // #3848: [2F9CD] ==> [46BE]
      u8"\xe4\x9b\x87\0"                         // #3849: [2F9CE] ==> [46C7]
      u8"\xe8\xaa\xa0\0"                         // #3850: [2F9CF] ==> [8AA0]
      u8"\xf0\xa7\xb2\xa8\0"                     // #3851: [2F9D3] ==> [27CA8]
      u8"\xe8\xb2\xab\0"                         // #3852: [2F9D4] ==> [8CAB]
      u8"\xe8\xb3\x81\0"                         // #3853: [2F9D5] ==> [8CC1]
      u8"\xe8\xb4\x9b\0"                         // #3854: [2F9D6] ==> [8D1B]
      u8"\xe8\xb5\xb7\0"                         // #3855: [2F9D7] ==> [8D77]
      u8"\xf0\xa7\xbc\xaf\0"                     // #3856: [2F9D8] ==> [27F2F]
      u8"\xf0\xa0\xa0\x84\0"                     // #3857: [2F9D9] ==> [20804]
      u8"\xe8\xb7\x8b\0"                         // #3858: [2F9DA] ==> [8DCB]
      u8"\xe8\xb6\xbc\0"                         // #3859: [2F9DB] ==> [8DBC]
      u8"\xe8\xb7\xb0\0"                         // #3860: [2F9DC] ==> [8DF0]
      u8"\xf0\xa0\xa3\x9e\0"                     // #3861: [2F9DD] ==> [208DE]
      u8"\xe8\xbb\x94\0"                         // #3862: [2F9DE] ==> [8ED4]
      u8"\xf0\xa8\x97\x92\0"                     // #3863: [2F9E0] ==> [285D2]
      u8"\xf0\xa8\x97\xad\0"                     // #3864: [2F9E1] ==> [285ED]
      u8"\xe9\x82\x94\0"                         // #3865: [2F9E2] ==> [9094]
      u8"\xe9\x83\xb1\0"                         // #3866: [2F9E3] ==> [90F1]
      u8"\xe9\x84\x91\0"                         // #3867: [2F9E4] ==> [9111]
      u8"\xf0\xa8\x9c\xae\0"                     // #3868: [2F9E5] ==> [2872E]
      u8"\xe9\x84\x9b\0"                         // #3869: [2F9E6] ==> [911B]
      u8"\xe9\x88\xb8\0"                         // #3870: [2F9E7] ==> [9238]
      u8"\xe9\x8b\x97\0"                         // #3871: [2F9E8] ==> [92D7]
      u8"\xe9\x8b\x98\0"                         // #3872: [2F9E9] ==> [92D8]
      u8"\xe9\x89\xbc\0"                         // #3873: [2F9EA] ==> [927C]
      u8"\xe9\x8f\xb9\0"                         // #3874: [2F9EB] ==> [93F9]
      u8"\xe9\x90\x95\0"                         // #3875: [2F9EC] ==> [9415]
      u8"\xf0\xa8\xaf\xba\0"                     // #3876: [2F9ED] ==> [28BFA]
      u8"\xe9\x96\x8b\0"                         // #3877: [2F9EE] ==> [958B]
      u8"\xe4\xa6\x95\0"                         // #3878: [2F9EF] ==> [4995]
      u8"\xe9\x96\xb7\0"                         // #3879: [2F9F0] ==> [95B7]
      u8"\xf0\xa8\xb5\xb7\0"                     // #3880: [2F9F1] ==> [28D77]
      u8"\xe4\xa7\xa6\0"                         // #3881: [2F9F2] ==> [49E6]
      u8"\xe9\x9b\x83\0"                         // #3882: [2F9F3] ==> [96C3]
      u8"\xe5\xb6\xb2\0"                         // #3883: [2F9F4] ==> [5DB2]
      u8"\xe9\x9c\xa3\0"                         // #3884: [2F9F5] ==> [9723]
      u8"\xf0\xa9\x85\x85\0"                     // #3885: [2F9F6] ==> [29145]
      u8"\xf0\xa9\x88\x9a\0"                     // #3886: [2F9F7] ==> [2921A]
      u8"\xe4\xa9\xae\0"                         // #3887: [2F9F8] ==> [4A6E]
      u8"\xe4\xa9\xb6\0"                         // #3888: [2F9F9] ==> [4A76]
      u8"\xe9\x9f\xa0\0"                         // #3889: [2F9FA] ==> [97E0]
      u8"\xf0\xa9\x90\x8a\0"                     // #3890: [2F9FB] ==> [2940A]
      u8"\xe4\xaa\xb2\0"                         // #3891: [2F9FC] ==> [4AB2]
      u8"\xf0\xa9\x92\x96\0"                     // #3892: [2F9FD] ==> [29496]
      u8"\xe9\xa0\xa9\0"                         // #3893: [2FA00] ==> [9829]
      u8"\xf0\xa9\x96\xb6\0"                     // #3894: [2FA01] ==> [295B6]
      u8"\xe9\xa3\xa2\0"                         // #3895: [2FA02] ==> [98E2]
      u8"\xe4\xac\xb3\0"                         // #3896: [2FA03] ==> [4B33]
      u8"\xe9\xa4\xa9\0"                         // #3897: [2FA04] ==> [9929]
      u8"\xe9\xa6\xa7\0"                         // #3898: [2FA05] ==> [99A7]
      u8"\xe9\xa7\x82\0"                         // #3899: [2FA06] ==> [99C2]
      u8"\xe9\xa7\xbe\0"                         // #3900: [2FA07] ==> [99FE]
      u8"\xe4\xaf\x8e\0"                         // #3901: [2FA08] ==> [4BCE]
      u8"\xf0\xa9\xac\xb0\0"                     // #3902: [2FA09] ==> [29B30]
      u8"\xe9\xb1\x80\0"                         // #3903: [2FA0B] ==> [9C40]
      u8"\xe9\xb3\xbd\0"                         // #3904: [2FA0C] ==> [9CFD]
      u8"\xe4\xb3\x8e\0"                         // #3905: [2FA0D] ==> [4CCE]
      u8"\xe4\xb3\xad\0"                         // #3906: [2FA0E] ==> [4CED]
      u8"\xe9\xb5\xa7\0"                         // #3907: [2FA0F] ==> [9D67]
      u8"\xf0\xaa\x83\x8e\0"                     // #3908: [2FA10] ==> [2A0CE]
      u8"\xe4\xb3\xb8\0"                         // #3909: [2FA11] ==> [4CF8]
      u8"\xf0\xaa\x84\x85\0"                     // #3910: [2FA12] ==> [2A105]
      u8"\xf0\xaa\x88\x8e\0"                     // #3911: [2FA13] ==> [2A20E]
      u8"\xf0\xaa\x8a\x91\0"                     // #3912: [2FA14] ==> [2A291]
      u8"\xe4\xb5\x96\0"                         // #3913: [2FA16] ==> [4D56]
      u8"\xe9\xbb\xbe\0"                         // #3914: [2FA18] ==> [9EFE]
      u8"\xe9\xbc\x85\0"                         // #3915: [2FA19] ==> [9F05]
      u8"\xe9\xbc\x8f\0"                         // #3916: [2FA1A] ==> [9F0F]
      u8"\xe9\xbc\x96\0"                         // #3917: [2FA1B] ==> [9F16]
      u8"\xf0\xaa\x98\x80\0"                     // #3918: [2FA1D] ==> [2A600]
      ,
      17246UL                                    // String Length
    };



} // namespace webpp::uri::idna::details

#endif // WEBPP_URI_IDNA_MAPPING_TABLE_HPP
