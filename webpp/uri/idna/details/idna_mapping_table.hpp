
/**
 * Attention: Auto-generated file, don't modify.
 *
 *   Auto generated from:          generate_idna_mapping_table2.mjs
 *   IDNA Creation Date:           2024-07-03, 21:52:28 GMT
 *   This file's generation date:  Thu, 16 Jan 2025 00:08:27 GMT
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
    static constexpr char32_t      last_diallowed  = U'\xe01f0';
    static constexpr std::uint8_t  batch_bit_count = 8U;

    static constexpr std::uint16_t not_mapped = 0b1000'0000'0000'0000U;
    static constexpr std::uint16_t valid      = 0b1000'0000'0000'0001U;
    static constexpr std::uint16_t disallowed = 0b1000'0000'0000'0010U;

    // Pick the table with this mask (between bools table and the block table)
    static constexpr std::uint16_t table_pick_mask = 0b1000'0000'0000'0000U;
    static constexpr auto          blt             = table_pick_mask; // shortcut

    /**
     * IDNA Reference Table
     *
     * Table size: 7172 B or 7.00 KiB
     */
    static constexpr std::array<std::uint16_t, 3586ULL> idna_refs{
      0x0U,          0x1U,          0x2U,          0x3U,          0x4U,          0x5U,          0x6U,
      blt | 0x0U,    blt | 0x100U,  0x7U,          0x8U,          0x9U,          blt | 0x200U,  blt | 0x300U,
      0xaU,          0xbU,          0xcU,          0xdU,          blt | 0x400U,  0xeU,          blt | 0x500U,
      blt | 0x4d8U,  blt | 0x600U,  0xfU,          0x10U,         blt | 0x700U,  blt | 0x800U,  blt | 0x900U,
      0x11U,         0x12U,         0x13U,         0x14U,         0x15U,         0x16U,         0x17U,
      0x18U,         0x19U,         blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      0x1aU,         blt | 0xa00U,  0x1bU,         0x1cU,         0x1dU,         0x1eU,         0x1fU,
      0x20U,         0x21U,         0x22U,         blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0xb00U,  blt | 0x4d8U,  0x23U,         0x24U,
      blt | 0xc00U,  blt | 0xd00U,  blt | 0xe00U,  0x25U,         blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0xf00U,  blt | 0x1000U,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  0x26U,         0x27U,         0x28U,
      0x29U,         0x2aU,         0x2bU,         0x2cU,         blt | 0x1100U, blt | 0x1200U, blt | 0x1300U,
      blt | 0x1400U, 0x2dU,         0x2eU,         blt | 0x4d8U,  0x2fU,         blt | 0x1500U, blt | 0x1600U,
      blt | 0x1700U, blt | 0x1800U, 0x30U,         0x31U,         blt | 0x1900U, blt | 0x1a00U, blt | 0x1b00U,
      blt | 0x1c00U, blt | 0x1d00U, blt | 0x1e00U, blt | 0x1f00U, blt | 0x2000U, blt | 0x2100U, blt | 0x2200U,
      0x32U,         blt | 0x2300U, blt | 0x2400U, blt | 0x2500U, blt | 0x2600U, blt | 0x2700U, blt | 0x2800U,
      blt | 0x2900U, blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x2a00U, blt | 0x2b00U, blt | 0x2c00U,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0x2d00U, blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x2e00U, blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x2f00U, blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x2bfdU, blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0x2c0aU, blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x3000U, blt | 0x3100U,
      blt | 0xffcU,  blt | 0x3200U, 0x33U,         blt | 0x3300U, blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x3400U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x3500U, blt | 0xff3U,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0x3600U, blt | 0x4d8U,  blt | 0x3700U,
      blt | 0x2effU, blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  0x34U,         blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  0x35U,         blt | 0x4d8U,
      blt | 0x2b90U, blt | 0x3800U, blt | 0x3900U, 0x36U,         blt | 0x3a00U, blt | 0x3b00U, 0x37U,
      0x38U,         0x39U,         0x3aU,         blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x3c00U, blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0x3d00U, 0x3bU,         blt | 0x3e00U, blt | 0x3f00U,
      blt | 0xffcU,  blt | 0x4000U, blt | 0x4100U, blt | 0xffcU,  blt | 0x4200U, blt | 0x4300U, 0x3cU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0x4400U, blt | 0x4500U, 0x3dU,         blt | 0xffcU,  blt | 0x4600U,
      0x3eU,         0x3fU,         blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4700U, blt | 0x4800U,
      blt | 0x4900U, blt | 0x4d8U,  blt | 0x4a00U, 0x40U,         blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4b00U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4c00U, blt | 0x4d00U, blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4e00U, blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4f00U, blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x2be6U, blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  0x41U,         0x42U,         0x43U,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x5000U, blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,
      blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x4d8U,  blt | 0x2b94U, blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,  blt | 0xffcU,
      blt | 0xffcU,  0x44U};


    /**
     * IDNA Reference Blocks Table (for valid or disallowed values only)
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
     * IDNA Reference Blocks Table
     *
     * Table size: 35328 B or 34.50 KiB
     */
    static constexpr std::array<std::array<std::uint16_t, 256ULL>, 69ULL> idna_ref_blocks{

      // Block #0
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,
        0x7,        0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,
        0xf,        0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,
        0x17,       0x18,       0x19,       valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        0x1a,       valid,      valid,      valid,      valid,      valid,      valid,      valid,
        0x1b,       valid,      0x0,        valid,      valid,      0x1c,       valid,      0x1d,
        valid,      valid,      0x1e,       0x1f,       0x20,       0x21,       valid,      valid,
        0x22,       0x23,       0xe,        valid,      0x24,       0x25,       0x26,       valid,
        0x27,       0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,
        0x2f,       0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,
        0x37,       0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       valid,
        0x3e,       0x3f,       0x40,       0x41,       0x42,       0x43,       0x44,       valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid}},
      // Block #1
      {{0x45,  valid, 0x46,  valid, 0x47,  valid, 0x48,  valid, 0x49,  valid, 0x4a,  valid, 0x4b,  valid,
        0x4c,  valid, 0x4d,  valid, 0x4e,  valid, 0x4f,  valid, 0x50,  valid, 0x51,  valid, 0x52,  valid,
        0x53,  valid, 0x54,  valid, 0x55,  valid, 0x56,  valid, 0x57,  valid, 0x58,  valid, 0x59,  valid,
        0x5a,  valid, 0x5b,  valid, 0x5c,  valid, 0x5d,  valid, 0x5e,  0x5e,  0x5f,  valid, 0x60,  valid,
        valid, 0x61,  valid, 0x62,  valid, 0x63,  valid, 0x64,  0x64,  0x65,  valid, 0x66,  valid, 0x67,
        valid, 0x68,  valid, 0x69,  0x6a,  valid, 0x6b,  valid, 0x6c,  valid, 0x6d,  valid, 0x6e,  valid,
        0x6f,  valid, 0x70,  valid, 0x71,  valid, 0x72,  valid, 0x73,  valid, 0x74,  valid, 0x75,  valid,
        0x76,  valid, 0x77,  valid, 0x78,  valid, 0x79,  valid, 0x7a,  valid, 0x7b,  valid, 0x7c,  valid,
        0x7d,  valid, 0x7e,  valid, 0x7f,  valid, 0x80,  valid, 0x81,  0x82,  valid, 0x83,  valid, 0x84,
        valid, 0x12,  valid, 0x85,  0x86,  valid, 0x87,  valid, 0x88,  0x89,  valid, 0x8a,  0x8b,  0x8c,
        valid, valid, 0x8d,  0x8e,  0x8f,  0x90,  valid, 0x91,  0x92,  valid, 0x93,  0x94,  0x95,  valid,
        valid, valid, 0x96,  0x97,  valid, 0x98,  0x99,  valid, 0x9a,  valid, 0x9b,  valid, 0x9c,  0x9d,
        valid, 0x9e,  valid, valid, 0x9f,  valid, 0xa0,  0xa1,  valid, 0xa2,  0xa3,  0xa4,  valid, 0xa5,
        valid, 0xa6,  0xa7,  valid, valid, valid, 0xa8,  valid, valid, valid, valid, valid, valid, valid,
        0xa9,  0xa9,  0xa9,  0xaa,  0xaa,  0xaa,  0xab,  0xab,  0xab,  0xac,  valid, 0xad,  valid, 0xae,
        valid, 0xaf,  valid, 0xb0,  valid, 0xb1,  valid, 0xb2,  valid, 0xb3,  valid, valid, 0xb4,  valid,
        0xb5,  valid, 0xb6,  valid, 0xb7,  valid, 0xb8,  valid, 0xb9,  valid, 0xba,  valid, 0xbb,  valid,
        0xbc,  valid, valid, 0xbd,  0xbd,  0xbd,  0xbe,  valid, 0xbf,  0xc0,  0xc1,  valid, 0xc2,  valid,
        0xc3,  valid, 0xc4,  valid}},
      // Block #2
      {{0xc5,  valid, 0xc6,  valid, 0xc7,  valid, 0xc8,  valid, 0xc9,  valid, 0xca,  valid, 0xcb,  valid,
        0xcc,  valid, 0xcd,  valid, 0xce,  valid, 0xcf,  valid, 0xd0,  valid, 0xd1,  valid, 0xd2,  valid,
        0xd3,  valid, 0xd4,  valid, 0xd5,  valid, 0xd6,  valid, 0xd7,  valid, 0xd8,  valid, 0xd9,  valid,
        0xda,  valid, 0xdb,  valid, 0xdc,  valid, 0xdd,  valid, 0xde,  valid, valid, valid, valid, valid,
        valid, valid, 0xdf,  0xe0,  valid, 0xe1,  0xe2,  valid, valid, 0xe3,  valid, 0xe4,  0xe5,  0xe6,
        0xe7,  valid, 0xe8,  valid, 0xe9,  valid, 0xea,  valid, 0xeb,  valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, 0x7,   0xec,  0x9,   0x11,  0xed,  0xee,
        0xef,  0x16,  0x18,  valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, 0xf0,  0xf1,  0xf2,  0xf3,  0xf4,  0xf5,  valid, valid,
        0x92,  0xb,   0x12,  0x17,  0xf6,  valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid}},
      // Block #3
      {{valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      0xf7,       0xf8,
        valid,      0xf9,  0xfa,  0xfb,  valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, 0x1c,  valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, 0xfc,  valid, 0xfd,  valid, 0xfe,  valid,      0xff,       valid,      disallowed,
        disallowed, 0x100, valid, valid, valid, 0x101, 0x102, disallowed, disallowed, disallowed, disallowed,
        0x20,       0x103, 0x104, 0x105, 0x106, 0x107, 0x108, disallowed, 0x109,      disallowed, 0x10a,
        0x10b,      valid, 0x10c, 0x10d, 0x10e, 0x10f, 0x110, 0x111,      0x112,      0x113,      0xfb,
        0x114,      0x115, 0x21,  0x116, 0x117, 0x118, 0x119, 0x11a,      disallowed, 0x11b,      0x11c,
        0x11d,      0x11e, 0x11f, 0x120, 0x121, 0x122, 0x123, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      valid,      valid,
        valid,      valid, valid, valid, valid, valid, valid, valid,      valid,      0x124,      0x10d,
        0x113,      0x11d, 0x10a, 0x123, 0x11e, 0x119, valid, 0x125,      valid,      0x126,      valid,
        0x127,      valid, 0x128, valid, 0x129, valid, 0x12a, valid,      0x12b,      valid,      0x12c,
        valid,      0x12d, valid, 0x12e, valid, 0x12f, valid, 0x130,      valid,      0x114,      0x11a,
        0x11b,      valid, 0x113, 0x110, valid, 0x131, valid, 0x11b,      0x132,      valid,      valid,
        0x133,      0x134, 0x135}},
      // Block #4
      {{0x136, 0x137, 0x138, 0x139, 0x13a, 0x13b, 0x13c, 0x13d, 0x13e, 0x13f, 0x140, 0x141, 0x142, 0x143,
        0x144, 0x145, 0x146, 0x147, 0x148, 0x149, 0x14a, 0x14b, 0x14c, 0x14d, 0x14e, 0x14f, 0x150, 0x151,
        0x152, 0x153, 0x154, 0x155, 0x156, 0x157, 0x158, 0x159, 0x15a, 0x15b, 0x15c, 0x15d, 0x15e, 0x15f,
        0x160, 0x161, 0x162, 0x163, 0x164, 0x165, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 0x166, valid,
        0x167, valid, 0x168, valid, 0x169, valid, 0x16a, valid, 0x16b, valid, 0x16c, valid, 0x16d, valid,
        0x16e, valid, 0x16f, valid, 0x170, valid, 0x171, valid, 0x172, valid, 0x173, valid, 0x174, valid,
        0x175, valid, 0x176, valid, valid, valid, valid, valid, valid, valid, valid, valid, 0x177, valid,
        0x178, valid, 0x179, valid, 0x17a, valid, 0x17b, valid, 0x17c, valid, 0x17d, valid, 0x17e, valid,
        0x17f, valid, 0x180, valid, 0x181, valid, 0x182, valid, 0x183, valid, 0x184, valid, 0x185, valid,
        0x186, valid, 0x187, valid, 0x188, valid, 0x189, valid, 0x18a, valid, 0x18b, valid, 0x18c, valid,
        0x18d, valid, 0x18e, valid, 0x18f, valid, 0x190, valid, 0x191, valid, 0x192, 0x193, valid, 0x194,
        valid, 0x195, valid, 0x196, valid, 0x197, valid, 0x198, valid, 0x199, valid, valid, 0x19a, valid,
        0x19b, valid, 0x19c, valid, 0x19d, valid, 0x19e, valid, 0x19f, valid, 0x1a0, valid, 0x1a1, valid,
        0x1a2, valid, 0x1a3, valid, 0x1a4, valid, 0x1a5, valid, 0x1a6, valid, 0x1a7, valid, 0x1a8, valid,
        0x1a9, valid, 0x1aa, valid, 0x1ab, valid, 0x1ac, valid, 0x1ad, valid, 0x1ae, valid, 0x1af, valid,
        0x1b0, valid, 0x1b1, valid}},
      // Block #5
      {{0x1b2,      valid,      0x1b3,      valid,      0x1b4,      valid,      0x1b5,      valid,
        0x1b6,      valid,      0x1b7,      valid,      0x1b8,      valid,      0x1b9,      valid,
        0x1ba,      valid,      0x1bb,      valid,      0x1bc,      valid,      0x1bd,      valid,
        0x1be,      valid,      0x1bf,      valid,      0x1c0,      valid,      0x1c1,      valid,
        0x1c2,      valid,      0x1c3,      valid,      0x1c4,      valid,      0x1c5,      valid,
        0x1c6,      valid,      0x1c7,      valid,      0x1c8,      valid,      0x1c9,      valid,
        disallowed, 0x1ca,      0x1cb,      0x1cc,      0x1cd,      0x1ce,      0x1cf,      0x1d0,
        0x1d1,      0x1d2,      0x1d3,      0x1d4,      0x1d5,      0x1d6,      0x1d7,      0x1d8,
        0x1d9,      0x1da,      0x1db,      0x1dc,      0x1dd,      0x1de,      0x1df,      0x1e0,
        0x1e1,      0x1e2,      0x1e3,      0x1e4,      0x1e5,      0x1e6,      0x1e7,      0x1e8,
        0x1e9,      0x1ea,      0x1eb,      0x1ec,      0x1ed,      0x1ee,      0x1ef,      disallowed,
        disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      0x1f0,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #6
      {{disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid, valid, valid,
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
        0x1f1,      0x1f2,      0x1f3,      0x1f4,      valid,      valid,      valid, valid, valid,
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
        valid,      valid,      valid,      valid}},
      // Block #7
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        0x1f5,      0x1f6,      0x1f7,      0x1f8,      0x1f9,      0x1fa,      0x1fb,      0x1fc,
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
        disallowed, disallowed, disallowed, disallowed, 0x1fd,      0x1fe,      disallowed, 0x1ff,
        valid,      valid,      valid,      valid,      disallowed, disallowed, valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed}},
      // Block #8
      {{disallowed, valid,      valid,      valid,      disallowed, valid,      valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, valid,
        valid,      disallowed, disallowed, valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      disallowed, valid,      valid,      valid,      valid,      valid,      valid,
        valid,      disallowed, valid,      0x200,      disallowed, valid,      0x201,      disallowed,
        valid,      valid,      disallowed, disallowed, valid,      disallowed, valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, valid,
        valid,      disallowed, disallowed, valid,      valid,      valid,      disallowed, disallowed,
        disallowed, valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, 0x202,      0x203,      0x204,      valid,      disallowed, 0x205,      disallowed,
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
        disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid}},
      // Block #9
      {{disallowed, valid,      valid,      valid,      disallowed, valid,      valid,      valid,
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
        disallowed, disallowed, disallowed, disallowed, 0x206,      0x207,      disallowed, valid,
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
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #10
      {{disallowed, valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      0x208,      valid,      valid,      valid,      valid,
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
        valid,      valid,      valid,      0x209,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      disallowed, valid,      disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      disallowed, disallowed, 0x20a,      0x20b,      valid,      valid,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #11
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      0x20c,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      0x20d,      valid,      valid,      valid,      valid,
        disallowed, valid,      valid,      valid,      valid,      0x20e,      valid,      valid,
        valid,      valid,      0x20f,      valid,      valid,      valid,      valid,      0x210,
        valid,      valid,      valid,      valid,      0x211,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      0x212,      valid,      valid,      valid,      disallowed, disallowed, disallowed,
        disallowed, valid,      valid,      0x213,      valid,      0x214,      0x215,      0x216,
        0x217,      0x218,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      0x219,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      0x21a,      valid,      valid,      valid,      valid,
        disallowed, valid,      valid,      valid,      valid,      0x21b,      valid,      valid,
        valid,      valid,      0x21c,      valid,      valid,      valid,      valid,      0x21d,
        valid,      valid,      valid,      valid,      0x21e,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      0x21f,      valid,      valid,      valid,      disallowed, valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      disallowed, valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #12
      {{valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
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
        valid,      valid, valid,      valid,      valid,      valid,      valid,      0x220, 0x221,
        0x222,      0x223, 0x224,      0x225,      0x226,      0x227,      0x228,      0x229, 0x22a,
        0x22b,      0x22c, 0x22d,      0x22e,      0x22f,      0x230,      0x231,      0x232, 0x233,
        0x234,      0x235, 0x236,      0x237,      0x238,      0x239,      0x23a,      0x23b, 0x23c,
        0x23d,      0x23e, 0x23f,      0x240,      0x241,      0x242,      0x243,      0x244, 0x245,
        disallowed, 0x246, disallowed, disallowed, disallowed, disallowed, disallowed, 0x247, disallowed,
        disallowed, valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid, valid,      valid,      valid,      valid,      valid,      valid, valid,
        0x248,      valid, valid,      valid}},
      // Block #13
      {{valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 0x1c,  0x1c,  valid,
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
        valid, valid, valid, valid}},
      // Block #14
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, disallowed,
        valid,      valid,      valid,      valid,      disallowed, disallowed, valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      disallowed, disallowed, valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, disallowed,
        disallowed, disallowed, valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid, valid,
        valid,      valid,      valid,      disallowed, disallowed, 0x249,      0x24a,      0x24b, 0x24c,
        0x24d,      0x24e,      disallowed, disallowed}},
      // Block #15
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        valid,      valid,      valid,      valid,      0x1c,       0x1c,       valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #16
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #17
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        0x148,      0x14a,      0x154,      0x157,      0x158,      0x158,      0x160,      0x167,
        0x24f,      0x250,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
        0x251,      0x252,      0x253,      0x254,      0x255,      0x256,      0x257,      0x258,
        0x259,      0x25a,      0x25b,      0x25c,      0x248,      0x25d,      0x25e,      0x25f,
        0x260,      0x261,      0x262,      0x263,      0x264,      0x265,      0x266,      0x267,
        0x268,      0x269,      0x26a,      0x26b,      0x26c,      0x26d,      0x26e,      0x26f,
        0x270,      0x271,      0x272,      0x273,      0x274,      0x275,      0x276,      0x277,
        0x278,      0x279,      0x27a,      disallowed, disallowed, 0x27b,      0x27c,      0x27d,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #18
      {{valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, 0x0,   0x2d,  0x1,   valid, 0x3,   0x4,   0x8d,  0x6,   0x7,   0x8,   0x9,   0xa,
        0xb,   0xc,   0xd,   valid, 0xe,   0xd6,  0xf,   0x11,  0x13,  0x14,  0x16,  0x0,   0x27e, 0x27f,
        0x280, 0x1,   0x3,   0x4,   0x8e,  0x8f,  0x281, 0x6,   valid, 0xa,   0xc,   0x6a,  0xe,   0x88,
        0x282, 0x283, 0xf,   0x13,  0x14,  0x284, 0x96,  0x15,  0x285, 0x10d, 0x10e, 0x10f, 0x11e, 0x11f,
        0x8,   0x11,  0x14,  0x15,  0x10d, 0x10e, 0x11a, 0x11e, 0x11f, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, 0x153, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, 0x286, 0x2,   0x287, 0x37,  0x281, 0x5,   0x288, 0x289, 0x28a, 0x94,  0x93,  0x28b, 0x28c,
        0x28d, 0x28e, 0x28f, 0x290, 0x291, 0x292, 0x97,  0x293, 0x294, 0x98,  0x295, 0x296, 0x9e,  0x297,
        0xe5,  0xa2,  0x298, 0xa3,  0xe6,  0x19,  0x299, 0x29a, 0xa6,  0x113, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid}},
      // Block #19
      {{0x29b, valid, 0x29c, valid, 0x29d, valid, 0x29e, valid, 0x29f, valid, 0x2a0, valid, 0x2a1, valid,
        0x2a2, valid, 0x2a3, valid, 0x2a4, valid, 0x2a5, valid, 0x2a6, valid, 0x2a7, valid, 0x2a8, valid,
        0x2a9, valid, 0x2aa, valid, 0x2ab, valid, 0x2ac, valid, 0x2ad, valid, 0x2ae, valid, 0x2af, valid,
        0x2b0, valid, 0x2b1, valid, 0x2b2, valid, 0x2b3, valid, 0x2b4, valid, 0x2b5, valid, 0x2b6, valid,
        0x2b7, valid, 0x2b8, valid, 0x2b9, valid, 0x2ba, valid, 0x2bb, valid, 0x2bc, valid, 0x2bd, valid,
        0x2be, valid, 0x2bf, valid, 0x2c0, valid, 0x2c1, valid, 0x2c2, valid, 0x2c3, valid, 0x2c4, valid,
        0x2c5, valid, 0x2c6, valid, 0x2c7, valid, 0x2c8, valid, 0x2c9, valid, 0x2ca, valid, 0x2cb, valid,
        0x2cc, valid, 0x2cd, valid, 0x2ce, valid, 0x2cf, valid, 0x2d0, valid, 0x2d1, valid, 0x2d2, valid,
        0x2d3, valid, 0x2d4, valid, 0x2d5, valid, 0x2d6, valid, 0x2d7, valid, 0x2d8, valid, 0x2d9, valid,
        0x2da, valid, 0x2db, valid, 0x2dc, valid, 0x2dd, valid, 0x2de, valid, 0x2df, valid, 0x2e0, valid,
        0x2e1, valid, 0x2e2, valid, 0x2e3, valid, 0x2e4, valid, 0x2e5, valid, valid, valid, valid, valid,
        0x2e6, 0x2cb, valid, valid, 0x2e7, valid, 0x2e8, valid, 0x2e9, valid, 0x2ea, valid, 0x2eb, valid,
        0x2ec, valid, 0x2ed, valid, 0x2ee, valid, 0x2ef, valid, 0x2f0, valid, 0x2f1, valid, 0x2f2, valid,
        0x2f3, valid, 0x2f4, valid, 0x2f5, valid, 0x2f6, valid, 0x2f7, valid, 0x2f8, valid, 0x2f9, valid,
        0x2fa, valid, 0x2fb, valid, 0x2fc, valid, 0x2fd, valid, 0x2fe, valid, 0x2ff, valid, 0x300, valid,
        0x301, valid, 0x302, valid, 0x303, valid, 0x304, valid, 0x305, valid, 0x306, valid, 0x307, valid,
        0x308, valid, 0x309, valid, 0x30a, valid, 0x30b, valid, 0x30c, valid, 0x30d, valid, 0x30e, valid,
        0x30f, valid, 0x310, valid, 0x311, valid, 0x312, valid, 0x313, valid, 0x314, valid, 0x315, valid,
        0x316, valid, 0x317, valid}},
      // Block #20
      {{valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
        0x318,      0x319,      0x31a,      0x31b, 0x31c,      0x31d,      0x31e,      0x31f,
        valid,      valid,      valid,      valid, valid,      valid,      disallowed, disallowed,
        0x320,      0x321,      0x322,      0x323, 0x324,      0x325,      disallowed, disallowed,
        valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
        0x326,      0x327,      0x328,      0x329, 0x32a,      0x32b,      0x32c,      0x32d,
        valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
        0x32e,      0x32f,      0x330,      0x331, 0x332,      0x333,      0x334,      0x335,
        valid,      valid,      valid,      valid, valid,      valid,      disallowed, disallowed,
        0x336,      0x337,      0x338,      0x339, 0x33a,      0x33b,      disallowed, disallowed,
        valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
        disallowed, 0x33c,      disallowed, 0x33d, disallowed, 0x33e,      disallowed, 0x33f,
        valid,      valid,      valid,      valid, valid,      valid,      valid,      valid,
        0x340,      0x341,      0x342,      0x343, 0x344,      0x345,      0x346,      0x347,
        valid,      0x104,      valid,      0x106, valid,      0x107,      valid,      0x108,
        valid,      0x109,      valid,      0x10a, valid,      0x10b,      disallowed, disallowed,
        0x348,      0x349,      0x34a,      0x34b, 0x34c,      0x34d,      0x34e,      0x34f,
        0x348,      0x349,      0x34a,      0x34b, 0x34c,      0x34d,      0x34e,      0x34f,
        0x350,      0x351,      0x352,      0x353, 0x354,      0x355,      0x356,      0x357,
        0x350,      0x351,      0x352,      0x353, 0x354,      0x355,      0x356,      0x357,
        0x358,      0x359,      0x35a,      0x35b, 0x35c,      0x35d,      0x35e,      0x35f,
        0x358,      0x359,      0x35a,      0x35b, 0x35c,      0x35d,      0x35e,      0x35f,
        valid,      valid,      0x360,      0x361, 0x362,      disallowed, valid,      0x363,
        0x364,      0x365,      0x366,      0x104, 0x361,      0x367,      0xfb,       0x367,
        0x368,      0x369,      0x36a,      0x36b, 0x36c,      disallowed, valid,      0x36d,
        0x36e,      0x106,      0x36f,      0x107, 0x36b,      0x370,      0x371,      0x372,
        valid,      valid,      valid,      0x373, disallowed, disallowed, valid,      valid,
        0x374,      0x375,      0x376,      0x108, disallowed, 0x377,      0x378,      0x379,
        valid,      valid,      valid,      0x37a, valid,      valid,      valid,      valid,
        0x37b,      0x37c,      0x37d,      0x10a, 0x37e,      0x37f,      0x103,      0x380,
        disallowed, disallowed, 0x381,      0x382, 0x383,      disallowed, valid,      0x384,
        0x385,      0x109,      0x386,      0x10b, 0x382,      0x20,       0x387,      disallowed}},
      // Block #21
      {{0x1a,       0x1a,       0x1a,       0x1a,       0x1a,       0x1a,       0x1a,       0x1a,
        0x1a,       0x1a,       0x1a,       0x1c,       valid,      valid,      disallowed, disallowed,
        valid,      0x388,      valid,      valid,      valid,      valid,      valid,      0x389,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, valid,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 0x1a,
        valid,      valid,      valid,      0x38a,      0x38b,      valid,      0x38c,      0x38d,
        valid,      valid,      valid,      valid,      0x38e,      valid,      0x38f,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      0x390,
        0x391,      0x392,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      0x393,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      0x1a,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       disallowed, disallowed, disallowed,
        disallowed, disallowed, 0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x394,      0x8,        disallowed, disallowed, 0x395,      0x396,      0x397,      0x398,
        0x399,      0x39a,      0x39b,      0x39c,      0x39d,      0x39e,      0x39f,      0xd,
        0x394,      0x23,       0x1e,       0x1f,       0x395,      0x396,      0x397,      0x398,
        0x399,      0x39a,      0x39b,      0x39c,      0x39d,      0x39e,      0x39f,      disallowed,
        0x0,        0x4,        0xe,        0x17,       0x8e,       0x7,        0xa,        0xb,
        0xc,        0xd,        0xf,        0x12,       0x13,       disallowed, disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        0x3a0,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #22
      {{0x3a1,      0x3a2, 0x2,   0x3a3, valid, 0x3a4, 0x3a5, 0x8f,  valid,      0x3a6,      0x6,
        0x7,        0x7,   0x7,   0x7,   0x58,  0x8,   0x8,   0xb,   0xb,        valid,      0xd,
        0x3a7,      valid, valid, 0xf,   0x10,  0x11,  0x11,  0x11,  valid,      valid,      0x3a8,
        0x3a9,      0x3aa, valid, 0x19,  valid, 0x121, valid, 0x19,  valid,      0xa,        0x2c,
        0x1,        0x2,   valid, 0x4,   0x4,   0x5,   0x3ab, 0xc,   0xe,        0x3ac,      0x3ad,
        0x3ae,      0x3af, 0x8,   valid, 0x3b0, 0x119, 0x10e, 0x10e, 0x119,      0x3b1,      valid,
        valid,      valid, valid, 0x3,   0x3,   0x4,   0x8,   0x9,   valid,      valid,      valid,
        valid,      valid, valid, 0x3b2, 0x3b3, 0x3b4, 0x3b5, 0x3b6, 0x3b7,      0x3b8,      0x3b9,
        0x3ba,      0x3bb, 0x3bc, 0x3bd, 0x3be, 0x3bf, 0x3c0, 0x3c1, 0x8,        0x3c2,      0x3c3,
        0x3c4,      0x15,  0x3c5, 0x3c6, 0x3c7, 0x3c8, 0x17,  0x3c9, 0x3ca,      0xb,        0x2,
        0x3,        0xc,   0x8,   0x3c2, 0x3c3, 0x3c4, 0x15,  0x3c5, 0x3c6,      0x3c7,      0x3c8,
        0x17,       0x3c9, 0x3ca, 0xb,   0x2,   0x3,   0xc,   valid, valid,      valid,      0x3cb,
        valid,      valid, valid, valid, valid, 0x3cc, valid, valid, disallowed, disallowed, disallowed,
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
        valid,      valid, valid}},
      // Block #23
      {{valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, 0x3cd, 0x3ce, valid, 0x3cf, 0x3d0, valid, valid, valid, valid, valid, valid, valid,
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
        valid, valid, valid, valid}},
      // Block #24
      {{valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 0x3d1,
        0x3d2, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
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
        valid, valid, valid, valid}},
      // Block #25
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        0x23,       0x1e,       0x1f,       0x395,      0x396,      0x397,      0x398,      0x399,
        0x39a,      0x3d3,      0x3d4,      0x3d5,      0x3d6,      0x3d7,      0x3d8,      0x3d9,
        0x3da,      0x3db,      0x3dc,      0x3dd,      0x3de,      0x3df,      0x3e0,      0x3e1,
        0x3e2,      0x3e3,      0x3e4,      0x3e5,      0x3e6,      0x3e7,      0x3e8,      0x3e9,
        0x3ea,      0x3eb,      0x3ec,      0x3ed,      0x3ee,      0x3ef,      0x3f0,      0x3f1,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, 0x3f2,      0x3f3,      0x3f4,      0x3f5,
        0x3f6,      0x3f7,      0x3f8,      0x3f9,      0x3fa,      0x3fb,      0x3fc,      0x3fd,
        0x3fe,      0x3ff,      0x400,      0x401,      0x402,      0x403,      0x404,      0x405,
        0x406,      0x407,      0x408,      0x409,      0x40a,      0x40b,      0x0,        0x1,
        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,        0x8,        0x9,
        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,        0x10,       0x11,
        0x12,       0x13,       0x14,       0x15,       0x16,       0x17,       0x18,       0x19,
        0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
        0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
        0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
        0x18,       0x19,       0x394,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid}},
      // Block #26
      {{valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 0x40c, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, 0x40d, 0x40e, 0x40f, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, 0x410, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid, valid,
        valid, valid, valid, valid}},
      // Block #27
      {{0x411, 0x412, 0x413,      0x414,      0x415,      0x416,      0x417,      0x418, 0x419, 0x41a, 0x41b,
        0x41c, 0x41d, 0x41e,      0x41f,      0x420,      0x421,      0x422,      0x423, 0x424, 0x425, 0x426,
        0x427, 0x428, 0x429,      0x42a,      0x42b,      0x42c,      0x42d,      0x42e, 0x42f, 0x430, 0x431,
        0x432, 0x433, 0x434,      0x435,      0x436,      0x437,      0x438,      0x439, 0x43a, 0x43b, 0x43c,
        0x43d, 0x43e, 0x43f,      0x440,      valid,      valid,      valid,      valid, valid, valid, valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid, valid, valid, valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid, valid, valid, valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid, valid, valid, valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid, 0x441, valid, 0x442,
        0x443, 0x444, valid,      valid,      0x445,      valid,      0x446,      valid, 0x447, valid, 0x27f,
        0x291, 0x27e, 0x286,      valid,      0x448,      valid,      valid,      0x449, valid, valid, valid,
        valid, valid, valid,      0x9,        0x15,       0x44a,      0x44b,      0x44c, valid, 0x44d, valid,
        0x44e, valid, 0x44f,      valid,      0x450,      valid,      0x451,      valid, 0x452, valid, 0x453,
        valid, 0x454, valid,      0x455,      valid,      0x456,      valid,      0x457, valid, 0x458, valid,
        0x459, valid, 0x45a,      valid,      0x45b,      valid,      0x45c,      valid, 0x45d, valid, 0x45e,
        valid, 0x45f, valid,      0x460,      valid,      0x461,      valid,      0x462, valid, 0x463, valid,
        0x464, valid, 0x465,      valid,      0x466,      valid,      0x467,      valid, 0x468, valid, 0x469,
        valid, 0x46a, valid,      0x46b,      valid,      0x46c,      valid,      0x46d, valid, 0x46e, valid,
        0x46f, valid, 0x470,      valid,      0x471,      valid,      0x472,      valid, 0x473, valid, 0x474,
        valid, 0x475, valid,      0x476,      valid,      0x477,      valid,      0x478, valid, 0x479, valid,
        0x47a, valid, 0x47b,      valid,      0x47c,      valid,      0x47d,      valid, valid, valid, valid,
        valid, valid, valid,      valid,      0x47e,      valid,      0x47f,      valid, valid, valid, valid,
        0x480, valid, disallowed, disallowed, disallowed, disallowed, disallowed, valid, valid, valid, valid,
        valid, valid, valid}},
      // Block #28
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, 0x481,
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
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid}},
      // Block #29
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        valid,      valid,      disallowed, valid,      valid,      valid,      valid,      0x482,
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
        valid,      valid,      valid,      0x483,      disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #30
      {{0x484,      0x485,      0x486,      0x487,      0x488,      0x489,      0x48a,      0x48b,
        0x48c,      0x48d,      0x48e,      0x48f,      0x490,      0x491,      0x492,      0x493,
        0x494,      0x495,      0x496,      0x497,      0x498,      0x499,      0x49a,      0x49b,
        0x49c,      0x49d,      0x49e,      0x49f,      0x4a0,      0x4a1,      0x4a2,      0x4a3,
        0x4a4,      0x4a5,      0x4a6,      0x4a7,      0x4a8,      0x4a9,      0x4aa,      0x4ab,
        0x4ac,      0x4ad,      0x4ae,      0x4af,      0x4b0,      0x4b1,      0x4b2,      0x4b3,
        0x4b4,      0x4b5,      0x4b6,      0x4b7,      0x4b8,      0x4b9,      0x4ba,      0x4bb,
        0x4bc,      0x4bd,      0x4be,      0x4bf,      0x4c0,      0x4c1,      0x4c2,      0x4c3,
        0x4c4,      0x4c5,      0x4c6,      0x4c7,      0x4c8,      0x4c9,      0x4ca,      0x4cb,
        0x4cc,      0x4cd,      0x4ce,      0x4cf,      0x4d0,      0x4d1,      0x4d2,      0x4d3,
        0x4d4,      0x4d5,      0x4d6,      0x4d7,      0x4d8,      0x4d9,      0x4da,      0x4db,
        0x4dc,      0x4dd,      0x4de,      0x4df,      0x4e0,      0x4e1,      0x4e2,      0x4e3,
        0x4e4,      0x4e5,      0x4e6,      0x4e7,      0x4e8,      0x4e9,      0x4ea,      0x4eb,
        0x4ec,      0x4ed,      0x4ee,      0x4ef,      0x4f0,      0x4f1,      0x4f2,      0x4f3,
        0x4f4,      0x4f5,      0x4f6,      0x4f7,      0x4f8,      0x4f9,      0x4fa,      0x4fb,
        0x4fc,      0x4fd,      0x4fe,      0x4ff,      0x500,      0x501,      0x502,      0x503,
        0x504,      0x505,      0x506,      0x507,      0x508,      0x509,      0x50a,      0x50b,
        0x50c,      0x50d,      0x50e,      0x50f,      0x510,      0x511,      0x512,      0x513,
        0x514,      0x515,      0x516,      0x517,      0x518,      0x519,      0x51a,      0x51b,
        0x51c,      0x51d,      0x51e,      0x51f,      0x520,      0x521,      0x522,      0x523,
        0x524,      0x525,      0x526,      0x527,      0x528,      0x529,      0x52a,      0x52b,
        0x52c,      0x52d,      0x52e,      0x52f,      0x530,      0x531,      0x532,      0x533,
        0x534,      0x535,      0x536,      0x537,      0x538,      0x539,      0x53a,      0x53b,
        0x53c,      0x53d,      0x53e,      0x53f,      0x540,      0x541,      0x542,      0x543,
        0x544,      0x545,      0x546,      0x547,      0x548,      0x549,      0x54a,      0x54b,
        0x54c,      0x54d,      0x54e,      0x54f,      0x550,      0x551,      0x552,      0x553,
        0x554,      0x555,      0x556,      0x557,      0x558,      0x559,      disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #31
      {{0x1a,  valid, 0x55a, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, 0x55b, valid,      0x49b,      0x55c, 0x55d, valid,
        valid, valid, valid, valid, disallowed, valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, disallowed, disallowed, valid, valid, 0x55e,
        0x55f, valid, valid, 0x560, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, valid, valid,      valid, valid, valid,      valid,      valid, valid, valid,
        valid, valid, valid, 0x561}},
      // Block #32
      {{disallowed, disallowed, disallowed, disallowed, disallowed, valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        disallowed, 0x562,      0x563,      0x564,      0x565,      0x566,      0x567,      0x568,
        0x569,      0x56a,      0x56b,      0x56c,      0x56d,      0x56e,      0x56f,      0x570,
        0x571,      0x572,      0x573,      0x574,      0x575,      0x576,      0x577,      0x578,
        0x579,      0x57a,      0x57b,      0x57c,      0x57d,      0x57e,      0x57f,      0x580,
        0x581,      0x582,      0x583,      0x584,      0x585,      0x586,      0x587,      0x588,
        0x589,      0x58a,      0x58b,      0x58c,      0x58d,      0x58e,      0x58f,      0x590,
        0x591,      0x592,      0x593,      0x594,      0x1c,       0x595,      0x596,      0x597,
        0x598,      0x599,      0x59a,      0x59b,      0x59c,      0x59d,      0x59e,      0x59f,
        0x5a0,      0x5a1,      0x5a2,      0x5a3,      0x5a4,      0x5a5,      0x5a6,      0x5a7,
        0x5a8,      0x5a9,      0x5aa,      0x5ab,      0x5ac,      0x5ad,      0x5ae,      0x5af,
        0x5b0,      0x5b1,      0x5b2,      0x5b3,      0x5b4,      0x5b5,      0x5b6,      0x5b7,
        0x5b8,      0x5b9,      0x5ba,      0x5bb,      0x5bc,      0x5bd,      0x5be,      disallowed,
        valid,      valid,      0x484,      0x48a,      0x5bf,      0x5c0,      0x5c1,      0x5c2,
        0x5c3,      0x5c4,      0x488,      0x5c5,      0x5c6,      0x5c7,      0x5c8,      0x48c,
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
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid}},
      // Block #33
      {{0x5c9, 0x5ca, 0x5cb, 0x5cc,      0x5cd, 0x5ce, 0x5cf, 0x5d0, 0x5d1, 0x5d2, 0x5d3, 0x5d4, 0x5d5, 0x5d6,
        0x5d7, 0x5d8, 0x5d9, 0x5da,      0x5db, 0x5dc, 0x5dd, 0x5de, 0x5df, 0x5e0, 0x5e1, 0x5e2, 0x5e3, 0x5e4,
        0x5e5, 0x5e6, 0x5e7, disallowed, 0x5e8, 0x5e9, 0x5ea, 0x5eb, 0x5ec, 0x5ed, 0x5ee, 0x5ef, 0x5f0, 0x5f1,
        0x5f2, 0x5f3, 0x5f4, 0x5f5,      0x5f6, 0x5f7, 0x5f8, 0x5f9, 0x5fa, 0x5fb, 0x5fc, 0x5fd, 0x5fe, 0x5ff,
        0x600, 0x601, 0x602, 0x603,      0x604, 0x605, 0x606, 0x607, 0x608, 0x609, 0x60a, 0x60b, 0x60c, 0x60d,
        0x4c6, 0x60e, valid, valid,      valid, valid, valid, valid, valid, valid, 0x60f, 0x610, 0x611, 0x612,
        0x613, 0x614, 0x615, 0x616,      0x617, 0x618, 0x619, 0x61a, 0x61b, 0x61c, 0x61d, 0x61e, 0x562, 0x565,
        0x568, 0x56a, 0x572, 0x573,      0x576, 0x578, 0x579, 0x57b, 0x57c, 0x57d, 0x57e, 0x57f, 0x61f, 0x620,
        0x621, 0x622, 0x623, 0x624,      0x625, 0x626, 0x627, 0x628, 0x629, 0x62a, 0x62b, 0x62c, 0x62d, 0x62e,
        0x62f, valid, 0x484, 0x48a,      0x5bf, 0x5c0, 0x630, 0x631, 0x632, 0x48f, 0x633, 0x49b, 0x4cd, 0x4d9,
        0x4d8, 0x4ce, 0x52a, 0x4a3,      0x4cb, 0x634, 0x635, 0x636, 0x637, 0x638, 0x639, 0x63a, 0x63b, 0x63c,
        0x63d, 0x4a9, 0x63e, 0x63f,      0x640, 0x641, 0x642, 0x643, 0x644, 0x645, 0x5c1, 0x5c2, 0x5c3, 0x646,
        0x647, 0x648, 0x649, 0x64a,      0x64b, 0x64c, 0x64d, 0x64e, 0x64f, 0x650, 0x651, 0x652, 0x653, 0x654,
        0x655, 0x656, 0x657, 0x658,      0x659, 0x65a, 0x65b, 0x65c, 0x65d, 0x65e, 0x65f, 0x660, 0x661, 0x662,
        0x663, 0x664, 0x665, 0x666,      0x667, 0x668, 0x669, 0x66a, 0x66b, 0x66c, 0x66d, 0x66e, 0x66f, 0x670,
        0x671, 0x672, 0x673, 0x674,      0x675, 0x676, 0x677, 0x678, 0x679, 0x67a, 0x67b, 0x67c, 0x67d, 0x67e,
        0x67f, 0x680, 0x681, 0x682,      0x683, 0x684, 0x685, 0x686, 0x687, 0x688, 0x689, 0x68a, 0x68b, 0x68c,
        0x68d, 0x68e, 0x68f, 0x690,      0x691, 0x692, 0x693, 0x694, 0x695, 0x696, 0x697, 0x698, 0x699, 0x69a,
        0x69b, 0x69c, 0x69d, 0x69e}},
      // Block #34
      {{0x69f,      0x6a0, 0x6a1,      0x6a2, 0x6a3, 0x6a4, 0x6a5, 0x6a6,      0x6a7, 0x6a8, 0x6a9, 0x6aa,
        0x6ab,      0x6ac, 0x6ad,      0x6ae, 0x6af, 0x6b0, 0x6b1, 0x6b2,      0x6b3, 0x6b4, 0x6b5, 0x6b6,
        0x6b7,      0x6b8, 0x6b9,      0x6ba, 0x6bb, 0x6bc, 0x6bd, 0x6be,      0x6bf, 0x6c0, 0x6c1, 0x6c2,
        0x6c3,      0x6c4, 0x6c5,      0x6c6, 0x6c7, 0x6c8, 0x6c9, 0x6ca,      0x6cb, 0x6cc, 0x6cd, 0x6ce,
        0x6cf,      0x6d0, 0x6d1,      0x6d2, 0x6d3, 0x6d4, 0x6d5, 0x6d6,      0x6d7, 0x6d8, 0x6d9, 0x6da,
        0x6db,      0x6dc, 0x6dd,      0x6de, 0x6df, 0x6e0, 0x6e1, 0x6e2,      0x6e3, 0x6e4, 0x6e5, 0x6e6,
        0x6e7,      0x6e8, 0x6e9,      0x6ea, 0x6eb, 0x6ec, 0x6ed, 0x6ee,      0x6ef, 0x6f0, 0x6f1, 0x6f2,
        0x6f3,      0x6f4, 0x6f5,      0x6f6, 0x6f7, 0x6f8, 0x6f9, 0x6fa,      0x6fb, 0x6fc, 0x6fd, 0x6fe,
        0x6ff,      0x700, 0x701,      0x702, 0x703, 0x704, 0x705, 0x706,      0x707, 0x708, 0x709, 0x70a,
        0x70b,      0x70c, 0x70d,      0x70e, 0x70f, 0x710, 0x711, 0x712,      0x713, 0x714, 0x715, 0x716,
        0x717,      0x718, 0x719,      0x71a, 0x71b, 0x71c, 0x71d, 0x71e,      0x71f, 0x720, 0x721, 0x722,
        0x723,      0x724, 0x725,      0x726, 0x727, 0x728, 0x729, 0x72a,      0x72b, 0x72c, 0x72d, 0x72e,
        0x72f,      0x730, 0x731,      0x732, 0x733, 0x734, 0x735, 0x736,      0x737, 0x738, 0x739, 0x73a,
        0x73b,      0x73c, 0x73d,      0x73e, 0x73f, 0x740, 0x741, 0x742,      0x743, 0x744, 0x745, 0x746,
        0x747,      0x71f, 0x748,      0x749, 0x74a, 0x74b, 0x74c, 0x74d,      0x74e, 0x74f, 0x750, 0x751,
        0x752,      0x753, 0x754,      0x755, 0x756, 0x755, 0x757, 0x758,      0x759, 0x75a, 0x75b, 0x75a,
        0x75c,      0x75d, disallowed, 0x75e, 0x75f, 0x760, 0x761, disallowed, 0x762, 0x763, 0x764, 0x765,
        0x766,      0x767, 0x73d,      0x768, 0x769, 0x76a, 0x76b, 0x76c,      0x725, 0x76d, 0x76e, 0x76f,
        disallowed, 0x770, 0x771,      0x772, 0x773, 0x774, 0x775, 0x776,      0x777, 0x778, 0x779, 0x77a,
        0x77b,      0x77c, 0x77d,      0x77e, 0x77f, 0x780, 0x781, 0x782,      0x783, 0x784, 0x785, 0x786,
        0x787,      0x788, 0x789,      0x78a, 0x78b, 0x78c, 0x78d, 0x78e,      0x78f, 0x790, 0x791, 0x792,
        0x793,      0x794, 0x795,      0x796}},
      // Block #35
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        0x797,      valid,      0x798,      valid,      0x799,      valid,      0x79a,      valid,
        0x79b,      valid,      0x24f,      valid,      0x79c,      valid,      0x79d,      valid,
        0x79e,      valid,      0x79f,      valid,      0x7a0,      valid,      0x7a1,      valid,
        0x7a2,      valid,      0x7a3,      valid,      0x7a4,      valid,      0x7a5,      valid,
        0x7a6,      valid,      0x7a7,      valid,      0x7a8,      valid,      0x7a9,      valid,
        0x7aa,      valid,      0x7ab,      valid,      0x7ac,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        0x7ad,      valid,      0x7ae,      valid,      0x7af,      valid,      0x7b0,      valid,
        0x7b1,      valid,      0x7b2,      valid,      0x7b3,      valid,      0x7b4,      valid,
        0x7b5,      valid,      0x7b6,      valid,      0x7b7,      valid,      0x7b8,      valid,
        0x7b9,      valid,      0x7ba,      valid,      0x160,      0x162,      valid,      valid,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #36
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      0x7bb,      valid,      0x7bc,      valid,      0x7bd,      valid,
        0x7be,      valid,      0x7bf,      valid,      0x7c0,      valid,      0x7c1,      valid,
        valid,      valid,      0x7c2,      valid,      0x7c3,      valid,      0x7c4,      valid,
        0x7c5,      valid,      0x7c6,      valid,      0x7c7,      valid,      0x7c8,      valid,
        0x7c9,      valid,      0x7ca,      valid,      0x7cb,      valid,      0x7cc,      valid,
        0x7cd,      valid,      0x7ce,      valid,      0x7cf,      valid,      0x7d0,      valid,
        0x7d1,      valid,      0x7d2,      valid,      0x7d3,      valid,      0x7d4,      valid,
        0x7d5,      valid,      0x7d6,      valid,      0x7d7,      valid,      0x7d8,      valid,
        0x7d9,      valid,      0x7da,      valid,      0x7db,      valid,      0x7dc,      valid,
        0x7dd,      valid,      0x7de,      valid,      0x7df,      valid,      0x7e0,      valid,
        0x7e0,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      0x7e1,      valid,      0x7e2,      valid,      0x7e3,      0x7e4,      valid,
        0x7e5,      valid,      0x7e6,      valid,      0x7e7,      valid,      0x7e8,      valid,
        valid,      valid,      valid,      0x7e9,      valid,      0x28a,      valid,      valid,
        0x7ea,      valid,      0x7eb,      valid,      valid,      valid,      0x7ec,      valid,
        0x7ed,      valid,      0x7ee,      valid,      0x7ef,      valid,      0x7f0,      valid,
        0x7f1,      valid,      0x7f2,      valid,      0x7f3,      valid,      0x7f4,      valid,
        0x7f5,      valid,      0xec,       0x281,      0x289,      0x7f6,      0x28b,      valid,
        0x7f7,      0x7f8,      0x28d,      0x7f9,      0x7fa,      valid,      0x7fb,      valid,
        0x7fc,      valid,      0x7fd,      valid,      0x7fe,      valid,      0x7ff,      valid,
        0x800,      valid,      0x801,      valid,      0x802,      0x296,      0x803,      0x804,
        valid,      0x805,      valid,      0x806,      0x807,      valid,      disallowed, disallowed,
        0x808,      valid,      disallowed, valid,      disallowed, valid,      0x809,      valid,
        0x80a,      valid,      0x80b,      valid,      0x80c,      disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, 0x2,        0x5,        0x10,       0x80d,      valid,      valid,
        0x58,       0x6e,       valid,      valid,      valid,      valid,      valid,      valid}},
      // Block #37
      {{disallowed, valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
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
        valid,      valid,      valid,      valid,      0x7bd,      0x80e,      0x442,      0x80f,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      0x810,      valid,      valid,      disallowed, disallowed, disallowed, disallowed,
        0x811,      0x812,      0x813,      0x814,      0x815,      0x816,      0x817,      0x818,
        0x819,      0x81a,      0x81b,      0x81c,      0x81d,      0x81e,      0x81f,      0x820,
        0x821,      0x822,      0x823,      0x824,      0x825,      0x826,      0x827,      0x828,
        0x829,      0x82a,      0x82b,      0x82c,      0x82d,      0x82e,      0x82f,      0x830,
        0x831,      0x832,      0x833,      0x834,      0x835,      0x836,      0x837,      0x838,
        0x839,      0x83a,      0x83b,      0x83c,      0x83d,      0x83e,      0x83f,      0x840,
        0x841,      0x842,      0x843,      0x844,      0x845,      0x846,      0x847,      0x848,
        0x849,      0x84a,      0x84b,      0x84c,      0x84d,      0x84e,      0x84f,      0x850,
        0x851,      0x852,      0x853,      0x854,      0x855,      0x856,      0x857,      0x858,
        0x859,      0x85a,      0x85b,      0x85c,      0x85d,      0x85e,      0x85f,      0x860,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #38
      {{0x861, 0x862, 0x522, 0x863, 0x864, 0x865, 0x866, 0x558, 0x558, 0x867, 0x52a, 0x868, 0x869, 0x86a,
        0x86b, 0x86c, 0x86d, 0x86e, 0x86f, 0x870, 0x871, 0x872, 0x873, 0x874, 0x875, 0x876, 0x877, 0x878,
        0x879, 0x87a, 0x87b, 0x87c, 0x87d, 0x87e, 0x87f, 0x880, 0x881, 0x882, 0x883, 0x884, 0x885, 0x886,
        0x887, 0x888, 0x889, 0x88a, 0x88b, 0x88c, 0x88d, 0x88e, 0x88f, 0x890, 0x500, 0x891, 0x892, 0x893,
        0x894, 0x895, 0x896, 0x897, 0x898, 0x899, 0x89a, 0x89b, 0x549, 0x89c, 0x89d, 0x89e, 0x89f, 0x8a0,
        0x8a1, 0x8a2, 0x8a3, 0x8a4, 0x8a5, 0x8a6, 0x8a7, 0x8a8, 0x8a9, 0x8aa, 0x8ab, 0x8ac, 0x8ad, 0x8ae,
        0x8af, 0x8b0, 0x8b1, 0x8b2, 0x8b3, 0x8b4, 0x8b5, 0x8b6, 0x871, 0x8b7, 0x8b8, 0x8b9, 0x8ba, 0x8bb,
        0x8bc, 0x8bd, 0x8be, 0x8bf, 0x8c0, 0x8c1, 0x8c2, 0x8c3, 0x8c4, 0x8c5, 0x8c6, 0x8c7, 0x8c8, 0x8c9,
        0x8ca, 0x524, 0x8cb, 0x8cc, 0x8cd, 0x8ce, 0x8cf, 0x8d0, 0x8d1, 0x8d2, 0x8d3, 0x8d4, 0x8d5, 0x8d6,
        0x8d7, 0x8d8, 0x8d9, 0x4a9, 0x8da, 0x8db, 0x8dc, 0x8dd, 0x8de, 0x8df, 0x8e0, 0x8e1, 0x496, 0x8e2,
        0x8e3, 0x8e4, 0x8e5, 0x8e6, 0x8e7, 0x8e8, 0x8e9, 0x8ea, 0x8eb, 0x8ec, 0x8ed, 0x8ee, 0x8ef, 0x8f0,
        0x8f1, 0x8f2, 0x8f3, 0x8f4, 0x8f5, 0x8f6, 0x8f7, 0x8c9, 0x8f8, 0x8f9, 0x8fa, 0x8fb, 0x8fc, 0x8fd,
        0x8fe, 0x8ff, 0x8b9, 0x900, 0x901, 0x902, 0x903, 0x904, 0x905, 0x906, 0x907, 0x908, 0x909, 0x90a,
        0x90b, 0x90c, 0x90d, 0x90e, 0x90f, 0x910, 0x911, 0x912, 0x913, 0x871, 0x914, 0x915, 0x916, 0x917,
        0x557, 0x918, 0x919, 0x91a, 0x91b, 0x91c, 0x91d, 0x91e, 0x91f, 0x920, 0x921, 0x922, 0x923, 0x631,
        0x924, 0x925, 0x926, 0x927, 0x928, 0x929, 0x92a, 0x92b, 0x92c, 0x8bb, 0x92d, 0x92e, 0x92f, 0x930,
        0x931, 0x932, 0x933, 0x934, 0x935, 0x936, 0x937, 0x938, 0x939, 0x529, 0x93a, 0x93b, 0x93c, 0x93d,
        0x93e, 0x93f, 0x940, 0x941, 0x942, 0x943, 0x944, 0x945, 0x946, 0x4f8, 0x947, 0x948, 0x949, 0x94a,
        0x94b, 0x94c, 0x94d, 0x94e}},
      // Block #39
      {{0x94f,      0x950,      0x951,      0x952,      0x953,      0x954,      0x955,      0x956,
        0x513,      0x957,      0x516,      0x958,      0x959,      0x95a,      valid,      valid,
        0x95b,      valid,      0x95c,      valid,      valid,      0x95d,      0x95e,      0x95f,
        0x960,      0x961,      0x962,      0x963,      0x964,      0x965,      0x4ff,      valid,
        0x966,      valid,      0x967,      valid,      valid,      0x968,      0x969,      valid,
        valid,      valid,      0x96a,      0x96b,      0x96c,      0x96d,      0x96e,      0x96f,
        0x970,      0x971,      0x972,      0x973,      0x974,      0x975,      0x976,      0x977,
        0x978,      0x979,      0x97a,      0x97b,      0x4b0,      0x97c,      0x97d,      0x97e,
        0x97f,      0x980,      0x981,      0x982,      0x983,      0x984,      0x985,      0x986,
        0x987,      0x988,      0x989,      0x98a,      0x636,      0x98b,      0x98c,      0x98d,
        0x98e,      0x63a,      0x98f,      0x990,      0x991,      0x992,      0x993,      0x8ed,
        0x994,      0x995,      0x996,      0x997,      0x998,      0x999,      0x999,      0x99a,
        0x99b,      0x99c,      0x99d,      0x99e,      0x99f,      0x9a0,      0x9a1,      0x968,
        0x9a2,      0x9a3,      0x9a4,      0x9a5,      0x9a6,      0x9a7,      disallowed, disallowed,
        0x9a8,      0x9a9,      0x9aa,      0x9ab,      0x9ac,      0x9ad,      0x9ae,      0x9af,
        0x976,      0x9b0,      0x9b1,      0x9b2,      0x95b,      0x9b3,      0x9b4,      0x9b5,
        0x9b6,      0x9b7,      0x9b8,      0x9b9,      0x9ba,      0x9bb,      0x9bc,      0x9bd,
        0x9be,      0x97e,      0x9bf,      0x97f,      0x9c0,      0x9c1,      0x9c2,      0x9c3,
        0x9c4,      0x95c,      0x886,      0x9c5,      0x9c6,      0x4d1,      0x8ca,      0x91d,
        0x9c7,      0x9c8,      0x986,      0x9c9,      0x987,      0x9ca,      0x9cb,      0x9cc,
        0x95e,      0x9cd,      0x9ce,      0x9cf,      0x9d0,      0x9d1,      0x95f,      0x9d2,
        0x9d3,      0x9d4,      0x9d5,      0x9d6,      0x9d7,      0x993,      0x9d8,      0x9d9,
        0x8ed,      0x9da,      0x997,      0x9db,      0x9dc,      0x9dd,      0x9de,      0x9df,
        0x99c,      0x9e0,      0x967,      0x9e1,      0x99d,      0x8b7,      0x9e2,      0x99e,
        0x9e3,      0x9a0,      0x9e4,      0x9e5,      0x9e6,      0x9e7,      0x9e8,      0x9a2,
        0x964,      0x9e9,      0x9a3,      0x9ea,      0x9a4,      0x9eb,      0x558,      0x9ec,
        0x9ed,      0x9ee,      0x9ef,      0x9f0,      0x9f1,      0x9f2,      0x9f3,      0x9f4,
        0x9f5,      0x9f6,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #40
      {{0x9f7,      0x9f8,      0x9f9,      0x9fa,      0x9fb,      0x9fc,      0x9fc,      disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, 0x9fd,      0x9fe,      0x9ff,      0xa00,      0xa01,
        disallowed, disallowed, disallowed, disallowed, disallowed, 0xa02,      valid,      0xa03,
        0xa04,      0x3ac,      0x3af,      0xa05,      0xa06,      0xa07,      0xa08,      0xa09,
        0xa0a,      0x39b,      0xa0b,      0xa0c,      0xa0d,      0xa0e,      0xa0f,      0xa10,
        0xa11,      0xa12,      0xa13,      0xa14,      0xa15,      0xa16,      0xa17,      disallowed,
        0xa18,      0xa19,      0xa1a,      0xa1b,      0xa1c,      disallowed, 0xa1d,      disallowed,
        0xa1e,      0xa1f,      disallowed, 0xa20,      0xa21,      disallowed, 0xa22,      0xa23,
        0xa24,      0xa25,      0xa26,      0xa27,      0xa28,      0xa29,      0xa2a,      0xa2b,
        0xa2c,      0xa2c,      0xa2d,      0xa2d,      0xa2d,      0xa2d,      0xa2e,      0xa2e,
        0xa2e,      0xa2e,      0xa2f,      0xa2f,      0xa2f,      0xa2f,      0xa30,      0xa30,
        0xa30,      0xa30,      0xa31,      0xa31,      0xa31,      0xa31,      0xa32,      0xa32,
        0xa32,      0xa32,      0xa33,      0xa33,      0xa33,      0xa33,      0xa34,      0xa34,
        0xa34,      0xa34,      0xa35,      0xa35,      0xa35,      0xa35,      0xa36,      0xa36,
        0xa36,      0xa36,      0xa37,      0xa37,      0xa37,      0xa37,      0xa38,      0xa38,
        0xa38,      0xa38,      0xa39,      0xa39,      0xa3a,      0xa3a,      0xa3b,      0xa3b,
        0xa3c,      0xa3c,      0xa3d,      0xa3d,      0xa3e,      0xa3e,      0xa3f,      0xa3f,
        0xa3f,      0xa3f,      0xa40,      0xa40,      0xa40,      0xa40,      0xa41,      0xa41,
        0xa41,      0xa41,      0xa42,      0xa42,      0xa42,      0xa42,      0xa43,      0xa43,
        0xa44,      0xa44,      0xa44,      0xa44,      0xa45,      0xa45,      0xa46,      0xa46,
        0xa46,      0xa46,      0xa47,      0xa47,      0xa47,      0xa47,      0xa48,      0xa48,
        0xa49,      0xa49,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, 0xa4a,      0xa4a,      0xa4a,      0xa4a,      0xa4b,
        0xa4b,      0xa4c,      0xa4c,      0xa4d,      0xa4d,      0x1f3,      0xa4e,      0xa4e,
        0xa4f,      0xa4f,      0xa50,      0xa50,      0xa51,      0xa51,      0xa51,      0xa51,
        0xa52,      0xa52,      0xa53,      0xa53,      0xa54,      0xa54,      0xa55,      0xa55,
        0xa56,      0xa56,      0xa57,      0xa57,      0xa58,      0xa58,      0xa59,      0xa59,
        0xa59,      0xa5a,      0xa5a,      0xa5a,      0xa5b,      0xa5b,      0xa5b,      0xa5b}},
      // Block #41
      {{0xa5c, 0xa5d, 0xa5e, 0xa5a, 0xa5f, 0xa60, 0xa61, 0xa62, 0xa63, 0xa64, 0xa65, 0xa66, 0xa67, 0xa68,
        0xa69, 0xa6a, 0xa6b, 0xa6c, 0xa6d, 0xa6e, 0xa6f, 0xa70, 0xa71, 0xa72, 0xa73, 0xa74, 0xa75, 0xa76,
        0xa77, 0xa78, 0xa79, 0xa7a, 0xa7b, 0xa7c, 0xa7d, 0xa7e, 0xa7f, 0xa80, 0xa81, 0xa82, 0xa83, 0xa84,
        0xa85, 0xa86, 0xa87, 0xa88, 0xa89, 0xa8a, 0xa8b, 0xa8c, 0xa8d, 0xa8e, 0xa8f, 0xa90, 0xa91, 0xa92,
        0xa93, 0xa94, 0xa95, 0xa96, 0xa97, 0xa98, 0xa99, 0xa9a, 0xa9b, 0xa9c, 0xa9d, 0xa9e, 0xa9f, 0xaa0,
        0xaa1, 0xaa2, 0xaa3, 0xaa4, 0xaa5, 0xaa6, 0xaa7, 0xaa8, 0xaa9, 0xaaa, 0xaab, 0xaac, 0xaad, 0xaae,
        0xaaf, 0xab0, 0xab1, 0xab2, 0xab3, 0xab4, 0xab5, 0xab6, 0xab7, 0xab8, 0xab9, 0xaba, 0xabb, 0xabc,
        0xabd, 0xabe, 0xabf, 0xac0, 0xa5e, 0xac1, 0xa5a, 0xa5f, 0xac2, 0xac3, 0xa63, 0xac4, 0xa64, 0xa65,
        0xac5, 0xac6, 0xa69, 0xac7, 0xa6a, 0xa6b, 0xac8, 0xac9, 0xa6d, 0xaca, 0xa6e, 0xa6f, 0xa8c, 0xa8d,
        0xa90, 0xa91, 0xa92, 0xa96, 0xa97, 0xa98, 0xa99, 0xa9d, 0xa9e, 0xa9f, 0xacb, 0xaa3, 0xacc, 0xacd,
        0xaa9, 0xace, 0xaaa, 0xaab, 0xab8, 0xacf, 0xad0, 0xab3, 0xad1, 0xab4, 0xab5, 0xa5c, 0xa5d, 0xad2,
        0xa5e, 0xad3, 0xa60, 0xa61, 0xa62, 0xa63, 0xad4, 0xa66, 0xa67, 0xa68, 0xa69, 0xad5, 0xa6d, 0xa70,
        0xa71, 0xa72, 0xa73, 0xa74, 0xa76, 0xa77, 0xa78, 0xa79, 0xa7a, 0xa7b, 0xad6, 0xa7c, 0xa7d, 0xa7e,
        0xa7f, 0xa80, 0xa81, 0xa83, 0xa84, 0xa85, 0xa86, 0xa87, 0xa88, 0xa89, 0xa8a, 0xa8b, 0xa8e, 0xa8f,
        0xa93, 0xa94, 0xa95, 0xa96, 0xa97, 0xa9a, 0xa9b, 0xa9c, 0xa9d, 0xad7, 0xaa0, 0xaa1, 0xaa2, 0xaa3,
        0xaa6, 0xaa7, 0xaa8, 0xaa9, 0xad8, 0xaac, 0xaad, 0xad9, 0xab0, 0xab1, 0xab2, 0xab3, 0xada, 0xa5e,
        0xad3, 0xa63, 0xad4, 0xa69, 0xad5, 0xa6d, 0xadb, 0xa7a, 0xadc, 0xadd, 0xade, 0xa96, 0xa97, 0xa9d,
        0xaa9, 0xad8, 0xab3, 0xada, 0xadf, 0xae0, 0xae1, 0xae2, 0xae3, 0xae4, 0xae5, 0xae6, 0xae7, 0xae8,
        0xae9, 0xaea, 0xaeb, 0xaec}},
      // Block #42
      {{0xaed,      0xaee,      0xaef,      0xaf0,      0xaf1,      0xaf2,      0xaf3,      0xaf4,
        0xaf5,      0xaf6,      0xaf7,      0xaf8,      0xadd,      0xaf9,      0xafa,      0xafb,
        0xafc,      0xae2,      0xae3,      0xae4,      0xae5,      0xae6,      0xae7,      0xae8,
        0xae9,      0xaea,      0xaeb,      0xaec,      0xaed,      0xaee,      0xaef,      0xaf0,
        0xaf1,      0xaf2,      0xaf3,      0xaf4,      0xaf5,      0xaf6,      0xaf7,      0xaf8,
        0xadd,      0xaf9,      0xafa,      0xafb,      0xafc,      0xaf6,      0xaf7,      0xaf8,
        0xadd,      0xadc,      0xade,      0xa82,      0xa77,      0xa78,      0xa79,      0xaf6,
        0xaf7,      0xaf8,      0xa82,      0xa83,      0xafd,      0xafd,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        0xafe,      0xaff,      0xaff,      0xb00,      0xb01,      0xb02,      0xb03,      0xb04,
        0xb05,      0xb05,      0xb06,      0xb07,      0xb08,      0xb09,      0xb0a,      0xb0b,
        0xb0b,      0xb0c,      0xb0d,      0xb0d,      0xb0e,      0xb0e,      0xb0f,      0xb10,
        0xb10,      0xb11,      0xb12,      0xb12,      0xb13,      0xb13,      0xb14,      0xb15,
        0xb15,      0xb16,      0xb16,      0xb17,      0xb18,      0xb19,      0xb1a,      0xb1a,
        0xb1b,      0xb1c,      0xb1d,      0xb1e,      0xb1f,      0xb1f,      0xb20,      0xb21,
        0xb22,      0xb23,      0xb24,      0xb25,      0xb25,      0xb26,      0xb26,      0xb27,
        0xb27,      0xb28,      0xb29,      0xb2a,      0xb2b,      0xb2c,      0xb2d,      0xb2e,
        disallowed, disallowed, 0xb2f,      0xb30,      0xb31,      0xb32,      0xb33,      0xb34,
        0xb34,      0xb35,      0xb36,      0xb37,      0xb38,      0xb38,      0xb39,      0xb3a,
        0xb3b,      0xb3c,      0xb3d,      0xb3e,      0xb3f,      0xb40,      0xb41,      0xb42,
        0xb43,      0xb44,      0xb45,      0xb46,      0xb47,      0xb48,      0xb49,      0xb4a,
        0xb4b,      0xb4c,      0xb4d,      0xb4e,      0xb20,      0xb22,      0xb4f,      0xb50,
        0xb51,      0xb52,      0xb53,      0xb54,      0xb53,      0xb51,      0xb55,      0xb56,
        0xb57,      0xb58,      0xb59,      0xb54,      0xb19,      0xb0f,      0xb5a,      0xb5b,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        0xb5c,      0xb5d,      0xb5e,      0xb5f,      0xb60,      0xb61,      0xb62,      0xb63,
        0xb64,      0xb65,      0xb66,      0xb67,      0xb68,      valid,      valid,      valid}},
      // Block #43
      {{0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,  0x1c,  0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,  0x1c,  0xb69,      0xb6a,
        disallowed, 0xb6b,      0x101,      0xb6c,      0xb6d,      0xb6e, 0xb6f, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, valid, valid, valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid, valid, valid,      valid,
        valid,      valid,      valid,      disallowed, 0xb70,      0xb71, 0xb72, 0xb72,      0x39e,
        0x39f,      0xb73,      0xb74,      0xb75,      0xb76,      0xb77, 0xb78, 0xb79,      0xb7a,
        0x3d1,      0x3d2,      0xb7b,      0xb7c,      0xb7d,      0xb7e, valid, valid,      0xb7f,
        0xb80,      0x38f,      0x38f,      0x38f,      0x38f,      0xb72, 0xb72, 0xb72,      0xb69,
        0xb6a,      disallowed, disallowed, 0x101,      0xb6b,      0xb6d, 0xb6c, 0xb70,      0x39e,
        0x39f,      0xb73,      0xb74,      0xb75,      0xb76,      0xb81, 0xb82, 0xb83,      0x39b,
        0xb84,      0xb85,      0xb86,      0x39d,      disallowed, 0xb87, 0xb88, 0xb89,      0xb8a,
        disallowed, disallowed, disallowed, disallowed, 0xb8b,      0xb8c, 0xb8d, valid,      0xb8e,
        disallowed, 0xb8f,      0xb90,      0xb91,      0xb92,      0xb93, 0xb94, 0xb95,      0xb96,
        0xb97,      0xb98,      0xb99,      0xb9a,      0xb9a,      0xb9b, 0xb9b, 0xb9c,      0xb9c,
        0xb9d,      0xb9d,      0xb9e,      0xb9e,      0xb9e,      0xb9e, 0xb9f, 0xb9f,      0xba0,
        0xba0,      0xba0,      0xba0,      0xba1,      0xba1,      0xba2, 0xba2, 0xba2,      0xba2,
        0xba3,      0xba3,      0xba3,      0xba3,      0xba4,      0xba4, 0xba4, 0xba4,      0xba5,
        0xba5,      0xba5,      0xba5,      0xba6,      0xba6,      0xba6, 0xba6, 0xba7,      0xba7,
        0xba8,      0xba8,      0xba9,      0xba9,      0xbaa,      0xbaa, 0xbab, 0xbab,      0xbab,
        0xbab,      0xbac,      0xbac,      0xbac,      0xbac,      0xbad, 0xbad, 0xbad,      0xbad,
        0xbae,      0xbae,      0xbae,      0xbae,      0xbaf,      0xbaf, 0xbaf, 0xbaf,      0xbb0,
        0xbb0,      0xbb0,      0xbb0,      0xbb1,      0xbb1,      0xbb1, 0xbb1, 0xbb2,      0xbb2,
        0xbb2,      0xbb2,      0xbb3,      0xbb3,      0xbb3,      0xbb3, 0xbb4, 0xbb4,      0xbb4,
        0xbb4,      0xbb5,      0xbb5,      0xbb5,      0xbb5,      0xbb6, 0xbb6, 0xbb6,      0xbb6,
        0xbb7,      0xbb7,      0xbb7,      0xbb7,      0xbb8,      0xbb8, 0xbb8, 0xbb8,      0xbb9,
        0xbb9,      0xbb9,      0xbb9,      0xbba,      0xbba,      0xa52, 0xa52, 0xbbb,      0xbbb,
        0xbbb,      0xbbb,      0xbbc,      0xbbc,      0xbbd,      0xbbd, 0xbbe, 0xbbe,      0xbbf,
        0xbbf,      disallowed, disallowed, 0x1c}},
      // Block #44
      {{disallowed, 0xb6c,      0xbc0,      0xb81,      0xb88,      0xb89,      0xb82,      0xbc1,
        0x39e,      0x39f,      0xb83,      0x39b,      0xb69,      0xb84,      0x55a,      0xbc2,
        0x394,      0x23,       0x1e,       0x1f,       0x395,      0x396,      0x397,      0x398,
        0x399,      0x39a,      0xb6b,      0x101,      0xb85,      0x39d,      0xb86,      0xb6d,
        0xb8a,      0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,
        0x7,        0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,
        0xf,        0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,
        0x17,       0x18,       0x19,       0xb7f,      0xb87,      0xb80,      0xbc3,      0xb72,
        0x380,      0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,
        0x7,        0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,
        0xf,        0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,
        0x17,       0x18,       0x19,       0xb73,      0xbc4,      0xb74,      0xbc5,      0xbc6,
        0xbc7,      0x55a,      0xb7b,      0xb7c,      0xb6a,      0xbc8,      0x69d,      0xbc9,
        0xbca,      0xbcb,      0xbcc,      0xbcd,      0xbce,      0xbcf,      0xbd0,      0xbd1,
        0xbd2,      0x66f,      0x670,      0x671,      0x672,      0x673,      0x674,      0x675,
        0x676,      0x677,      0x678,      0x679,      0x67a,      0x67b,      0x67c,      0x67d,
        0x67e,      0x67f,      0x680,      0x681,      0x682,      0x683,      0x684,      0x685,
        0x686,      0x687,      0x688,      0x689,      0x68a,      0x68b,      0x68c,      0x68d,
        0x68e,      0x68f,      0x690,      0x691,      0x692,      0x693,      0x694,      0x695,
        0x696,      0x697,      0x698,      0x699,      0x69a,      0xbd3,      0xbd4,      0xbd5,
        0x1c,       0x562,      0x563,      0x564,      0x565,      0x566,      0x567,      0x568,
        0x569,      0x56a,      0x56b,      0x56c,      0x56d,      0x56e,      0x56f,      0x570,
        0x571,      0x572,      0x573,      0x574,      0x575,      0x576,      0x577,      0x578,
        0x579,      0x57a,      0x57b,      0x57c,      0x57d,      0x57e,      0x57f,      disallowed,
        disallowed, disallowed, 0x580,      0x581,      0x582,      0x583,      0x584,      0x585,
        disallowed, disallowed, 0x586,      0x587,      0x588,      0x589,      0x58a,      0x58b,
        disallowed, disallowed, 0x58c,      0x58d,      0x58e,      0x58f,      0x590,      0x591,
        disallowed, disallowed, 0x592,      0x593,      0x594,      disallowed, disallowed, disallowed,
        0xbd6,      0xbd7,      0xbd8,      0x1d,       0xbd9,      0xbda,      0xbdb,      disallowed,
        0xbdc,      0xbdd,      0xbde,      0xbdf,      0xbe0,      0xbe1,      0xbe2,      disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #45
      {{0xbe3, 0xbe4, 0xbe5,      0xbe6,      0xbe7,      0xbe8,      0xbe9,      0xbea,
        0xbeb, 0xbec, 0xbed,      0xbee,      0xbef,      0xbf0,      0xbf1,      0xbf2,
        0xbf3, 0xbf4, 0xbf5,      0xbf6,      0xbf7,      0xbf8,      0xbf9,      0xbfa,
        0xbfb, 0xbfc, 0xbfd,      0xbfe,      0xbff,      0xc00,      0xc01,      0xc02,
        0xc03, 0xc04, 0xc05,      0xc06,      0xc07,      0xc08,      0xc09,      0xc0a,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      disallowed, disallowed,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        0xc0b, 0xc0c, 0xc0d,      0xc0e,      0xc0f,      0xc10,      0xc11,      0xc12,
        0xc13, 0xc14, 0xc15,      0xc16,      0xc17,      0xc18,      0xc19,      0xc1a,
        0xc1b, 0xc1c, 0xc1d,      0xc1e,      0xc1f,      0xc20,      0xc21,      0xc22,
        0xc23, 0xc24, 0xc25,      0xc26,      0xc27,      0xc28,      0xc29,      0xc2a,
        0xc2b, 0xc2c, 0xc2d,      0xc2e,      disallowed, disallowed, disallowed, disallowed,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      valid,      valid,      valid,      valid,
        valid, valid, valid,      valid,      disallowed, disallowed, disallowed, disallowed}},
      // Block #46
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        0xc2f,      0xc30,      0xc31,      0xc32,      0xc33,      0xc34,      0xc35,      0xc36,
        0xc37,      0xc38,      0xc39,      disallowed, 0xc3a,      0xc3b,      0xc3c,      0xc3d,
        0xc3e,      0xc3f,      0xc40,      0xc41,      0xc42,      0xc43,      0xc44,      0xc45,
        0xc46,      0xc47,      0xc48,      disallowed, 0xc49,      0xc4a,      0xc4b,      0xc4c,
        0xc4d,      0xc4e,      0xc4f,      disallowed, 0xc50,      0xc51,      disallowed, valid,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #47
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        valid,      0xc52,      0xc53,      0x2d,       0xc54,      0x85,       disallowed, 0xc55,
        0xc56,      0xc57,      0xc58,      0x8a,       0x8b,       0xc59,      0xc5a,      0xc5b,
        0xc5c,      0x806,      0xc5d,      0x91,       0xc5e,      0x58,       0xc5f,      0xc60,
        0xc61,      0xc62,      0xc63,      0x7f6,      0xc64,      0xc65,      0xc66,      0xc67,
        0xc68,      0xc69,      0x3e,       0xc6a,      0xc6b,      0x10,       0xc6c,      0xc6d,
        0x444,      0xc6e,      0x9c,       0xc6f,      0xc70,      0xc71,      0xc72,      0xa0,
        0xc73,      disallowed, 0xc74,      0xc75,      0xc76,      0xc77,      0xc78,      0xc79,
        0xc7a,      0xc7b,      0xc7c,      disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #48
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        0xc7d,      0xc7e,      0xc7f,      0xc80,      0xc81,      0xc82,      0xc83,      0xc84,
        0xc85,      0xc86,      0xc87,      0xc88,      0xc89,      0xc8a,      0xc8b,      0xc8c,
        0xc8d,      0xc8e,      0xc8f,      0xc90,      0xc91,      0xc92,      0xc93,      0xc94,
        0xc95,      0xc96,      0xc97,      0xc98,      0xc99,      0xc9a,      0xc9b,      0xc9c,
        0xc9d,      0xc9e,      0xc9f,      0xca0,      0xca1,      0xca2,      0xca3,      0xca4,
        0xca5,      0xca6,      0xca7,      0xca8,      0xca9,      0xcaa,      0xcab,      0xcac,
        0xcad,      0xcae,      0xcaf,      disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, valid,      valid,      valid,      valid,      valid,      valid}},
      // Block #49
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        0xcb0,      0xcb1,      0xcb2,      0xcb3,      0xcb4,      0xcb5,      0xcb6,      0xcb7,
        0xcb8,      0xcb9,      0xcba,      0xcbb,      0xcbc,      0xcbd,      0xcbe,      0xcbf,
        0xcc0,      0xcc1,      0xcc2,      0xcc3,      0xcc4,      0xcc5,      disallowed, disallowed,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #50
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        0xcc6,      0xcc7,      0xcc8,      0xcc9,      0xcca,      0xccb,      0xccc,      0xccd,
        0xcce,      0xccf,      0xcd0,      0xcd1,      0xcd2,      0xcd3,      0xcd4,      0xcd5,
        0xcd6,      0xcd7,      0xcd8,      0xcd9,      0xcda,      0xcdb,      0xcdc,      0xcdd,
        0xcde,      0xcdf,      0xce0,      0xce1,      0xce2,      0xce3,      0xce4,      0xce5,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, valid}},
      // Block #51
      {{disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        0xce6,      0xce7,      0xce8,      0xce9,      0xcea,      0xceb,      0xcec,      0xced,
        0xcee,      0xcef,      0xcf0,      0xcf1,      0xcf2,      0xcf3,      0xcf4,      0xcf5,
        0xcf6,      0xcf7,      0xcf8,      0xcf9,      0xcfa,      0xcfb,      0xcfc,      0xcfd,
        0xcfe,      0xcff,      0xd00,      0xd01,      0xd02,      0xd03,      0xd04,      0xd05,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #52
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        0x1c,       0x1c,       0x1c,       0x1c,       disallowed, disallowed, disallowed, disallowed,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #53
      {{valid,      valid,      valid,      valid,      valid,      valid,     valid, valid, valid, valid,
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
        valid,      valid,      valid,      valid,      0x0,        0x1,       0x2,   0x3,   0x4,   0x5,
        0x6,        0x7,        0x8,        0x9,        0xa,        0xb,       0xc,   0xd,   0xe,   0xf,
        0x10,       0x11,       0x12,       0x13,       0x14,       0x15,      0x16,  0x17,  0x18,  0x19,
        0x394,      0x23,       0x1e,       0x1f,       0x395,      0x396,     0x397, 0x398, 0x399, 0x39a,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #54
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        valid,      valid,      valid,      valid,      valid,      valid,      0xd06,      0xd07,
        0xd08,      0xd09,      0xd0a,      0xd0b,      0xd0c,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      0xd0d,      0xd0e,      0xd0f,      0xd10,      0xd11,
        0xd12,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #55
      {{0x0,  0x1,        0x2,        0x3,  0x4,        0x5,        0x6,        0x7,        0x8,
        0x9,  0xa,        0xb,        0xc,  0xd,        0xe,        0xf,        0x10,       0x11,
        0x12, 0x13,       0x14,       0x15, 0x16,       0x17,       0x18,       0x19,       0x0,
        0x1,  0x2,        0x3,        0x4,  0x5,        0x6,        0x7,        0x8,        0x9,
        0xa,  0xb,        0xc,        0xd,  0xe,        0xf,        0x10,       0x11,       0x12,
        0x13, 0x14,       0x15,       0x16, 0x17,       0x18,       0x19,       0x0,        0x1,
        0x2,  0x3,        0x4,        0x5,  0x6,        0x7,        0x8,        0x9,        0xa,
        0xb,  0xc,        0xd,        0xe,  0xf,        0x10,       0x11,       0x12,       0x13,
        0x14, 0x15,       0x16,       0x17, 0x18,       0x19,       0x0,        0x1,        0x2,
        0x3,  0x4,        0x5,        0x6,  disallowed, 0x8,        0x9,        0xa,        0xb,
        0xc,  0xd,        0xe,        0xf,  0x10,       0x11,       0x12,       0x13,       0x14,
        0x15, 0x16,       0x17,       0x18, 0x19,       0x0,        0x1,        0x2,        0x3,
        0x4,  0x5,        0x6,        0x7,  0x8,        0x9,        0xa,        0xb,        0xc,
        0xd,  0xe,        0xf,        0x10, 0x11,       0x12,       0x13,       0x14,       0x15,
        0x16, 0x17,       0x18,       0x19, 0x0,        0x1,        0x2,        0x3,        0x4,
        0x5,  0x6,        0x7,        0x8,  0x9,        0xa,        0xb,        0xc,        0xd,
        0xe,  0xf,        0x10,       0x11, 0x12,       0x13,       0x14,       0x15,       0x16,
        0x17, 0x18,       0x19,       0x0,  disallowed, 0x2,        0x3,        disallowed, disallowed,
        0x6,  disallowed, disallowed, 0x9,  0xa,        disallowed, disallowed, 0xd,        0xe,
        0xf,  0x10,       disallowed, 0x12, 0x13,       0x14,       0x15,       0x16,       0x17,
        0x18, 0x19,       0x0,        0x1,  0x2,        0x3,        disallowed, 0x5,        disallowed,
        0x7,  0x8,        0x9,        0xa,  0xb,        0xc,        0xd,        disallowed, 0xf,
        0x10, 0x11,       0x12,       0x13, 0x14,       0x15,       0x16,       0x17,       0x18,
        0x19, 0x0,        0x1,        0x2,  0x3,        0x4,        0x5,        0x6,        0x7,
        0x8,  0x9,        0xa,        0xb,  0xc,        0xd,        0xe,        0xf,        0x10,
        0x11, 0x12,       0x13,       0x14, 0x15,       0x16,       0x17,       0x18,       0x19,
        0x0,  0x1,        0x2,        0x3,  0x4,        0x5,        0x6,        0x7,        0x8,
        0x9,  0xa,        0xb,        0xc,  0xd,        0xe,        0xf,        0x10,       0x11,
        0x12, 0x13,       0x14,       0x15}},
      // Block #56
      {{0x16, 0x17,       0x18,       0x19,       0x0,  0x1,  disallowed, 0x3,  0x4,        0x5,
        0x6,  disallowed, disallowed, 0x9,        0xa,  0xb,  0xc,        0xd,  0xe,        0xf,
        0x10, disallowed, 0x12,       0x13,       0x14, 0x15, 0x16,       0x17, 0x18,       disallowed,
        0x0,  0x1,        0x2,        0x3,        0x4,  0x5,  0x6,        0x7,  0x8,        0x9,
        0xa,  0xb,        0xc,        0xd,        0xe,  0xf,  0x10,       0x11, 0x12,       0x13,
        0x14, 0x15,       0x16,       0x17,       0x18, 0x19, 0x0,        0x1,  disallowed, 0x3,
        0x4,  0x5,        0x6,        disallowed, 0x8,  0x9,  0xa,        0xb,  0xc,        disallowed,
        0xe,  disallowed, disallowed, disallowed, 0x12, 0x13, 0x14,       0x15, 0x16,       0x17,
        0x18, disallowed, 0x0,        0x1,        0x2,  0x3,  0x4,        0x5,  0x6,        0x7,
        0x8,  0x9,        0xa,        0xb,        0xc,  0xd,  0xe,        0xf,  0x10,       0x11,
        0x12, 0x13,       0x14,       0x15,       0x16, 0x17, 0x18,       0x19, 0x0,        0x1,
        0x2,  0x3,        0x4,        0x5,        0x6,  0x7,  0x8,        0x9,  0xa,        0xb,
        0xc,  0xd,        0xe,        0xf,        0x10, 0x11, 0x12,       0x13, 0x14,       0x15,
        0x16, 0x17,       0x18,       0x19,       0x0,  0x1,  0x2,        0x3,  0x4,        0x5,
        0x6,  0x7,        0x8,        0x9,        0xa,  0xb,  0xc,        0xd,  0xe,        0xf,
        0x10, 0x11,       0x12,       0x13,       0x14, 0x15, 0x16,       0x17, 0x18,       0x19,
        0x0,  0x1,        0x2,        0x3,        0x4,  0x5,  0x6,        0x7,  0x8,        0x9,
        0xa,  0xb,        0xc,        0xd,        0xe,  0xf,  0x10,       0x11, 0x12,       0x13,
        0x14, 0x15,       0x16,       0x17,       0x18, 0x19, 0x0,        0x1,  0x2,        0x3,
        0x4,  0x5,        0x6,        0x7,        0x8,  0x9,  0xa,        0xb,  0xc,        0xd,
        0xe,  0xf,        0x10,       0x11,       0x12, 0x13, 0x14,       0x15, 0x16,       0x17,
        0x18, 0x19,       0x0,        0x1,        0x2,  0x3,  0x4,        0x5,  0x6,        0x7,
        0x8,  0x9,        0xa,        0xb,        0xc,  0xd,  0xe,        0xf,  0x10,       0x11,
        0x12, 0x13,       0x14,       0x15,       0x16, 0x17, 0x18,       0x19, 0x0,        0x1,
        0x2,  0x3,        0x4,        0x5,        0x6,  0x7,  0x8,        0x9,  0xa,        0xb,
        0xc,  0xd,        0xe,        0xf,        0x10, 0x11}},
      // Block #57
      {{0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0x0,   0x1,   0x2,        0x3,        0x4,
        0x5,   0x6,   0x7,   0x8,   0x9,   0xa,   0xb,   0xc,   0xd,   0xe,   0xf,        0x10,       0x11,
        0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0x0,   0x1,   0x2,        0x3,        0x4,
        0x5,   0x6,   0x7,   0x8,   0x9,   0xa,   0xb,   0xc,   0xd,   0xe,   0xf,        0x10,       0x11,
        0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0x0,   0x1,   0x2,        0x3,        0x4,
        0x5,   0x6,   0x7,   0x8,   0x9,   0xa,   0xb,   0xc,   0xd,   0xe,   0xf,        0x10,       0x11,
        0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0x0,   0x1,   0x2,        0x3,        0x4,
        0x5,   0x6,   0x7,   0x8,   0x9,   0xa,   0xb,   0xc,   0xd,   0xe,   0xf,        0x10,       0x11,
        0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0x0,   0x1,   0x2,        0x3,        0x4,
        0x5,   0x6,   0x7,   0x8,   0x9,   0xa,   0xb,   0xc,   0xd,   0xe,   0xf,        0x10,       0x11,
        0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0x0,   0x1,   0x2,        0x3,        0x4,
        0x5,   0x6,   0x7,   0x8,   0x9,   0xa,   0xb,   0xc,   0xd,   0xe,   0xf,        0x10,       0x11,
        0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0xd13, 0xd14, disallowed, disallowed, 0x10c,
        0x10d, 0x10e, 0x10f, 0x110, 0x111, 0x112, 0x113, 0xfb,  0x114, 0x115, 0x21,       0x116,      0x117,
        0x118, 0x119, 0x11a, 0x113, 0x11b, 0x11c, 0x11d, 0x11e, 0x11f, 0x120, 0x121,      0xd15,      0x10c,
        0x10d, 0x10e, 0x10f, 0x110, 0x111, 0x112, 0x113, 0xfb,  0x114, 0x115, 0x21,       0x116,      0x117,
        0x118, 0x119, 0x11a, 0x11b, 0x11b, 0x11c, 0x11d, 0x11e, 0x11f, 0x120, 0x121,      0xd16,      0x110,
        0x113, 0x114, 0x11e, 0x11a, 0x119, 0x10c, 0x10d, 0x10e, 0x10f, 0x110, 0x111,      0x112,      0x113,
        0xfb,  0x114, 0x115, 0x21,  0x116, 0x117, 0x118, 0x119, 0x11a, 0x113, 0x11b,      0x11c,      0x11d,
        0x11e, 0x11f, 0x120, 0x121, 0xd15, 0x10c, 0x10d, 0x10e, 0x10f}},
      // Block #58
      {{0x110, 0x111, 0x112, 0x113, 0xfb,  0x114, 0x115, 0x21,  0x116, 0x117,      0x118,      0x119, 0x11a,
        0x11b, 0x11b, 0x11c, 0x11d, 0x11e, 0x11f, 0x120, 0x121, 0xd16, 0x110,      0x113,      0x114, 0x11e,
        0x11a, 0x119, 0x10c, 0x10d, 0x10e, 0x10f, 0x110, 0x111, 0x112, 0x113,      0xfb,       0x114, 0x115,
        0x21,  0x116, 0x117, 0x118, 0x119, 0x11a, 0x113, 0x11b, 0x11c, 0x11d,      0x11e,      0x11f, 0x120,
        0x121, 0xd15, 0x10c, 0x10d, 0x10e, 0x10f, 0x110, 0x111, 0x112, 0x113,      0xfb,       0x114, 0x115,
        0x21,  0x116, 0x117, 0x118, 0x119, 0x11a, 0x11b, 0x11b, 0x11c, 0x11d,      0x11e,      0x11f, 0x120,
        0x121, 0xd16, 0x110, 0x113, 0x114, 0x11e, 0x11a, 0x119, 0x10c, 0x10d,      0x10e,      0x10f, 0x110,
        0x111, 0x112, 0x113, 0xfb,  0x114, 0x115, 0x21,  0x116, 0x117, 0x118,      0x119,      0x11a, 0x113,
        0x11b, 0x11c, 0x11d, 0x11e, 0x11f, 0x120, 0x121, 0xd15, 0x10c, 0x10d,      0x10e,      0x10f, 0x110,
        0x111, 0x112, 0x113, 0xfb,  0x114, 0x115, 0x21,  0x116, 0x117, 0x118,      0x119,      0x11a, 0x11b,
        0x11b, 0x11c, 0x11d, 0x11e, 0x11f, 0x120, 0x121, 0xd16, 0x110, 0x113,      0x114,      0x11e, 0x11a,
        0x119, 0x10c, 0x10d, 0x10e, 0x10f, 0x110, 0x111, 0x112, 0x113, 0xfb,       0x114,      0x115, 0x21,
        0x116, 0x117, 0x118, 0x119, 0x11a, 0x113, 0x11b, 0x11c, 0x11d, 0x11e,      0x11f,      0x120, 0x121,
        0xd15, 0x10c, 0x10d, 0x10e, 0x10f, 0x110, 0x111, 0x112, 0x113, 0xfb,       0x114,      0x115, 0x21,
        0x116, 0x117, 0x118, 0x119, 0x11a, 0x11b, 0x11b, 0x11c, 0x11d, 0x11e,      0x11f,      0x120, 0x121,
        0xd16, 0x110, 0x113, 0x114, 0x11e, 0x11a, 0x119, 0x127, 0x127, disallowed, disallowed, 0x394, 0x23,
        0x1e,  0x1f,  0x395, 0x396, 0x397, 0x398, 0x399, 0x39a, 0x394, 0x23,       0x1e,       0x1f,  0x395,
        0x396, 0x397, 0x398, 0x399, 0x39a, 0x394, 0x23,  0x1e,  0x1f,  0x395,      0x396,      0x397, 0x398,
        0x399, 0x39a, 0x394, 0x23,  0x1e,  0x1f,  0x395, 0x396, 0x397, 0x398,      0x399,      0x39a, 0x394,
        0x23,  0x1e,  0x1f,  0x395, 0x396, 0x397, 0x398, 0x399, 0x39a}},
      // Block #59
      {{valid,      valid,      valid,      valid,      valid,      valid,      valid,      disallowed,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      disallowed, disallowed, valid,      valid,      valid,      valid,      valid,
        valid,      valid,      disallowed, valid,      valid,      disallowed, valid,      valid,
        valid,      valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed,
        0x146,      0x147,      0x148,      0x149,      0x14a,      0x14b,      0x14c,      0x14d,
        0x14e,      0x150,      0x151,      0x152,      0x154,      0x155,      0x156,      0x157,
        0x158,      0x159,      0x15a,      0x15b,      0x15c,      0x15d,      0x15e,      0x161,
        0x163,      0x164,      0x7b1,      0x19e,      0x13c,      0x13e,      0x1a6,      0x189,
        0x192,      0x146,      0x147,      0x148,      0x149,      0x14a,      0x14b,      0x14c,
        0x14d,      0x14e,      0x150,      0x151,      0x154,      0x155,      0x157,      0x159,
        0x15a,      0x15b,      0x15c,      0x15d,      0x15e,      0x160,      0x161,      0x17a,
        0x13c,      0x13b,      0x145,      0x187,      0x79e,      0x18a,      disallowed, disallowed,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #60
      {{0xd17,      0xd18,      0xd19,      0xd1a,      0xd1b,      0xd1c,      0xd1d,      0xd1e,
        0xd1f,      0xd20,      0xd21,      0xd22,      0xd23,      0xd24,      0xd25,      0xd26,
        0xd27,      0xd28,      0xd29,      0xd2a,      0xd2b,      0xd2c,      0xd2d,      0xd2e,
        0xd2f,      0xd30,      0xd31,      0xd32,      0xd33,      0xd34,      0xd35,      0xd36,
        0xd37,      0xd38,      valid,      valid,      valid,      valid,      valid,      valid,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #61
      {{0xb9f,      0xba0,      0xba4,      0xba7,      disallowed, 0xbba,      0xbaa,      0xba5,
        0xbaf,      0xbbb,      0xbb5,      0xbb6,      0xbb7,      0xbb8,      0xbab,      0xbb1,
        0xbb3,      0xbad,      0xbb4,      0xba9,      0xbac,      0xba2,      0xba3,      0xba6,
        0xba8,      0xbae,      0xbb0,      0xbb2,      0xd39,      0xa43,      0xd3a,      0xd3b,
        disallowed, 0xba0,      0xba4,      disallowed, 0xbb9,      disallowed, disallowed, 0xba5,
        disallowed, 0xbbb,      0xbb5,      0xbb6,      0xbb7,      0xbb8,      0xbab,      0xbb1,
        0xbb3,      0xbad,      0xbb4,      disallowed, 0xbac,      0xba2,      0xba3,      0xba6,
        disallowed, 0xbae,      disallowed, 0xbb2,      disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, 0xba4,      disallowed, disallowed, disallowed, disallowed, 0xba5,
        disallowed, 0xbbb,      disallowed, 0xbb6,      disallowed, 0xbb8,      0xbab,      0xbb1,
        disallowed, 0xbad,      0xbb4,      disallowed, 0xbac,      disallowed, disallowed, 0xba6,
        disallowed, 0xbae,      disallowed, 0xbb2,      disallowed, 0xa43,      disallowed, 0xd3b,
        disallowed, 0xba0,      0xba4,      disallowed, 0xbb9,      disallowed, disallowed, 0xba5,
        0xbaf,      0xbbb,      0xbb5,      disallowed, 0xbb7,      0xbb8,      0xbab,      0xbb1,
        0xbb3,      0xbad,      0xbb4,      disallowed, 0xbac,      0xba2,      0xba3,      0xba6,
        disallowed, 0xbae,      0xbb0,      0xbb2,      0xd39,      disallowed, 0xd3a,      disallowed,
        0xb9f,      0xba0,      0xba4,      0xba7,      0xbb9,      0xbba,      0xbaa,      0xba5,
        0xbaf,      0xbbb,      disallowed, 0xbb6,      0xbb7,      0xbb8,      0xbab,      0xbb1,
        0xbb3,      0xbad,      0xbb4,      0xba9,      0xbac,      0xba2,      0xba3,      0xba6,
        0xba8,      0xbae,      0xbb0,      0xbb2,      disallowed, disallowed, disallowed, disallowed,
        disallowed, 0xba0,      0xba4,      0xba7,      disallowed, 0xbba,      0xbaa,      0xba5,
        0xbaf,      0xbbb,      disallowed, 0xbb6,      0xbb7,      0xbb8,      0xbab,      0xbb1,
        0xbb3,      0xbad,      0xbb4,      0xba9,      0xbac,      0xba2,      0xba3,      0xba6,
        0xba8,      0xbae,      0xbb0,      0xbb2,      disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        valid,      valid,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #62
      {{disallowed, 0xd3c,      0xd3d,      0xd3e,      0xd3f,      0xd40,      0xd41,      0xd42,
        0xd43,      0xd44,      0xd45,      valid,      valid,      valid,      valid,      valid,
        0x3f2,      0x3f3,      0x3f4,      0x3f5,      0x3f6,      0x3f7,      0x3f8,      0x3f9,
        0x3fa,      0x3fb,      0x3fc,      0x3fd,      0x3fe,      0x3ff,      0x400,      0x401,
        0x402,      0x403,      0x404,      0x405,      0x406,      0x407,      0x408,      0x409,
        0x40a,      0x40b,      0xd46,      0x2,        0x11,       0x760,      0xd47,      valid,
        0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
        0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
        0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
        0x18,       0x19,       0xd48,      0x755,      0xd49,      0xd4a,      0xd4b,      0xd4c,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      0xd4d,      0xd4e,      0xd4f,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
        0xd50,      valid,      valid,      valid,      valid,      valid,      valid,      valid,
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
        valid,      valid,      valid,      valid,      valid,      valid,      valid,      valid}},
      // Block #63
      {{0xd51,      0xd52,      0x679,      disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        0x4c3,      0xd53,      0xd54,      0xd55,      0x48a,      0xd56,      0xd57,      0x5c7,
        0xd58,      0xd59,      0xd5a,      0x913,      0xd5b,      0xd5c,      0xd5d,      0xd5e,
        0xd5f,      0xd60,      0x4e7,      0xd61,      0xd62,      0xd63,      0xd64,      0xd65,
        0xd66,      0x484,      0x5bf,      0xd67,      0x646,      0x5c2,      0x647,      0xd68,
        0x51f,      0xd69,      0xd6a,      0xd6b,      0xd6c,      0xd6d,      0x635,      0x4cd,
        0xd6e,      0xd6f,      0xd70,      0xd71,      disallowed, disallowed, disallowed, disallowed,
        0xd72,      0xd73,      0xd74,      0xd75,      0xd76,      0xd77,      0xd78,      0xd79,
        0xd7a,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        0xd7b,      0xd7c,      disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #64
      {{valid,      valid,      valid,      valid,      valid, valid, valid, valid,      valid,
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
        valid,      valid,      valid,      valid,      valid, valid, 0x394, 0x23,       0x1e,
        0x1f,       0x395,      0x396,      0x397,      0x398, 0x399, 0x39a, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed}},
      // Block #65
      {{0xd7d, 0xd7e, 0xd7f, 0xd80, 0xd81, 0x970, 0xd82, 0xd83, 0xd84, 0xd85, 0x971, 0xd86, 0xd87, 0xd88,
        0x972, 0xd89, 0xd8a, 0xd8b, 0xd8c, 0xd8d, 0xd8e, 0xd5d, 0xd8f, 0xd90, 0xd91, 0xd92, 0xd93, 0x9a9,
        0xd94, 0x494, 0xd95, 0xd96, 0xd97, 0xd98, 0xd6f, 0xd99, 0xd9a, 0x9ae, 0x973, 0x974, 0x9af, 0xd9b,
        0xd9c, 0x8bd, 0xd9d, 0x975, 0xd9e, 0xd9f, 0xda0, 0xda1, 0xda1, 0xda1, 0xda2, 0xda3, 0xda4, 0xda5,
        0xda6, 0xda7, 0xda8, 0xda9, 0xdaa, 0xdab, 0xdac, 0xdad, 0xdae, 0xdaf, 0xdb0, 0xdb1, 0xdb2, 0xdb3,
        0xdb3, 0x9b1, 0xdb4, 0xdb5, 0xdb6, 0xdb7, 0x977, 0xdb8, 0xdb9, 0xdba, 0x94f, 0xdbb, 0xdbc, 0xdbd,
        0xdbe, 0xdbf, 0xdc0, 0xdc1, 0xdc2, 0xdc3, 0xdc4, 0xdc5, 0xdc6, 0xd56, 0xdc7, 0xdc8, 0xdc9, 0xdca,
        0xdcb, 0xdcc, 0xdcd, 0xdce, 0xdcf, 0xdd0, 0xdd1, 0xdd2, 0xdd3, 0xdd3, 0xdd4, 0xdd5, 0xdd6, 0x8b9,
        0xdd7, 0xdd8, 0xdd9, 0xdda, 0xddb, 0x4ae, 0xddc, 0xddd, 0x4b0, 0xdde, 0xddf, 0xde0, 0xde1, 0xde2,
        0xde3, 0xde4, 0xde5, 0xde6, 0xde7, 0xde8, 0xde9, 0xdea, 0xdeb, 0xdec, 0xded, 0xdee, 0xdef, 0xdf0,
        0xdf1, 0xdf2, 0x885, 0xdf3, 0x4ba, 0xdf4, 0xdf4, 0xdf5, 0xdf6, 0xdf6, 0xdf7, 0xdf8, 0xdf9, 0xdfa,
        0xdfb, 0xdfc, 0xdfd, 0xdfe, 0xdff, 0xe00, 0xe01, 0xe02, 0xe03, 0x97c, 0xe04, 0xe05, 0xe06, 0xe07,
        0x9bd, 0xe07, 0xe08, 0x97e, 0xe09, 0xe0a, 0xe0b, 0xe0c, 0x97f, 0x86a, 0xe0d, 0xe0e, 0xe0f, 0xe10,
        0xe11, 0xe12, 0xe13, 0xe14, 0xe15, 0xe16, 0xe17, 0xe18, 0xe19, 0xe1a, 0xe1b, 0xe1c, 0xe1d, 0xe1e,
        0xe1f, 0xe20, 0xe21, 0xe22, 0x980, 0xe23, 0xe24, 0xe25, 0xe26, 0xe27, 0xe28, 0x982, 0xe29, 0xe2a,
        0xe2b, 0xe2c, 0xe2d, 0xe2e, 0xe2f, 0xe30, 0x886, 0x9c5, 0xe31, 0xe32, 0xe33, 0xe34, 0xe35, 0xe36,
        0xe37, 0xe38, 0x983, 0xe39, 0xe3a, 0xe3b, 0xe3c, 0x9ef, 0xe3d, 0xe3e, 0xe3f, 0xe40, 0xe41, 0xe42,
        0xe43, 0xe44, 0xe45, 0xe46, 0xe47, 0xe48, 0xe49, 0x8ca, 0xe4a, 0xe4b, 0xe4c, 0xe4d, 0xe4e, 0xe4f,
        0xe50, 0xe51, 0xe52, 0xe53}},
      // Block #66
      {{0xe54, 0x984, 0x91d, 0xe55, 0xe56, 0xe57, 0xe58, 0xe59, 0xe5a, 0xe5b, 0xe5c, 0x9c8, 0xe5d, 0xe5e,
        0xe5f, 0xe60, 0xe61, 0xe62, 0xe63, 0xe64, 0x9c9, 0xe65, 0xe66, 0xe67, 0xe68, 0xe69, 0xe6a, 0xe6b,
        0xe6c, 0xe6d, 0xe6e, 0xe6f, 0xe70, 0x9cb, 0xe71, 0xe72, 0xe73, 0xe74, 0xe75, 0xe76, 0xe77, 0xe78,
        0xe79, 0xe7a, 0xe7b, 0xe7b, 0xe7c, 0xe7d, 0x9cd, 0xe7e, 0xe7f, 0xe80, 0xe81, 0xe82, 0xe83, 0xe84,
        0x8bc, 0xe85, 0xe86, 0xe87, 0xe88, 0xe89, 0xe8a, 0xe8b, 0x9d3, 0xe8c, 0xe8d, 0xe8e, 0xe8f, 0xe90,
        0xe91, 0xe91, 0x9d4, 0x9f1, 0xe92, 0xe93, 0xe94, 0xe95, 0xe96, 0x897, 0x9d6, 0xe97, 0xe98, 0x98e,
        0xe99, 0xe9a, 0x963, 0xe9b, 0xe9c, 0x991, 0xe9d, 0xe9e, 0xe9f, 0xea0, 0xea0, 0xea1, 0xea2, 0xea3,
        0xea4, 0xea5, 0xea6, 0xea7, 0xea8, 0xea9, 0xeaa, 0xeab, 0xeac, 0xead, 0xeae, 0xeaf, 0xeb0, 0xeb1,
        0xeb2, 0xeb3, 0xeb4, 0xeb5, 0xeb6, 0xeb7, 0xeb8, 0xeb9, 0xeba, 0xebb, 0x997, 0xebc, 0xebd, 0xebe,
        0xebf, 0xec0, 0xec1, 0xec2, 0xec3, 0xec4, 0xec5, 0xec6, 0xec7, 0xec8, 0xec9, 0xeca, 0xecb, 0xdf5,
        0xecc, 0xecd, 0xece, 0xecf, 0xed0, 0xed1, 0xed2, 0xed3, 0xed4, 0xed5, 0xed6, 0xed7, 0x8cd, 0xed8,
        0xed9, 0xeda, 0xedb, 0xedc, 0xedd, 0x99a, 0xede, 0xedf, 0xee0, 0xee1, 0xee2, 0xee3, 0xee4, 0xee5,
        0xee6, 0xee7, 0xee8, 0xee9, 0xeea, 0xeeb, 0xeec, 0xeed, 0xeee, 0xeef, 0xef0, 0xef1, 0x892, 0xef2,
        0xef3, 0xef4, 0xef5, 0xef6, 0xef7, 0x9dd, 0xef8, 0xef9, 0xefa, 0xefb, 0xefc, 0xefd, 0xefe, 0xeff,
        0x514, 0xf00, 0xf01, 0xf02, 0xf03, 0xf04, 0xf05, 0xf06, 0xf07, 0xf08, 0xf09, 0xf0a, 0x9e2, 0x9e3,
        0x51b, 0xf0b, 0xf0c, 0xf0d, 0xf0e, 0xf0f, 0xf10, 0xf11, 0xf12, 0xf13, 0xf14, 0xf15, 0xf16, 0x9e4,
        0xf17, 0xf18, 0xf19, 0xf1a, 0xf1b, 0xf1c, 0xf1d, 0xf1e, 0xf1f, 0xf20, 0xf21, 0xf22, 0xf23, 0xf24,
        0xf25, 0xf26, 0xf27, 0xf28, 0xf29, 0xf2a, 0xf2b, 0xf2c, 0xf2d, 0xf2e, 0xf2f, 0xf30, 0xf31, 0xf32,
        0xf33, 0xf34, 0x9ea, 0x9ea}},
      // Block #67
      {{0xf35,      0xf36,      0xf37,      0xf38,      0xf39,      0xf3a,      0xf3b,      0xf3c,
        0xf3d,      0xf3e,      0x9eb,      0xf3f,      0xf40,      0xf41,      0xf42,      0xf43,
        0xf44,      0xf45,      0xf46,      0xf47,      0xf48,      0x54b,      0xf49,      0x54f,
        0xf4a,      0xf4b,      0xf4c,      0xf4d,      0x554,      0xf4e,      disallowed, disallowed,
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
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}},
      // Block #68
      {{0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,       0x1c,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed,
        disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed, disallowed}}

    };

    /**
     * IDNA Mapped Code Points Table
     * Each mapping ends with EOF '\0'.
     *
     * Table size: 17246 B or 16.84 KiB
     */
    static constexpr std::basic_string_view<char8_t> idna_mappings{
      u8"\x61\0"
      u8"\x62\0"
      u8"\x63\0"
      u8"\x64\0"
      u8"\x65\0"
      u8"\x66\0"
      u8"\x67\0"
      u8"\x68\0"
      u8"\x69\0"
      u8"\x6a\0"
      u8"\x6b\0"
      u8"\x6c\0"
      u8"\x6d\0"
      u8"\x6e\0"
      u8"\x6f\0"
      u8"\x70\0"
      u8"\x71\0"
      u8"\x72\0"
      u8"\x73\0"
      u8"\x74\0"
      u8"\x75\0"
      u8"\x76\0"
      u8"\x77\0"
      u8"\x78\0"
      u8"\x79\0"
      u8"\x7a\0"
      u8"\40\0"
      u8"\40\xcc\x88\0"
      u8"\0"
      u8"\40\xcc\x84\0"
      u8"\62\0"
      u8"\63\0"
      u8"\40\xcc\x81\0"
      u8"\xce\xbc\0"
      u8"\40\xcc\xa7\0"
      u8"\61\0"
      u8"\61\xe2\x81\x84\64\0"
      u8"\61\xe2\x81\x84\62\0"
      u8"\63\xe2\x81\x84\64\0"
      u8"\xc3\xa0\0"
      u8"\xc3\xa1\0"
      u8"\xc3\xa2\0"
      u8"\xc3\xa3\0"
      u8"\xc3\xa4\0"
      u8"\xc3\xa5\0"
      u8"\xc3\xa6\0"
      u8"\xc3\xa7\0"
      u8"\xc3\xa8\0"
      u8"\xc3\xa9\0"
      u8"\xc3\xaa\0"
      u8"\xc3\xab\0"
      u8"\xc3\xac\0"
      u8"\xc3\xad\0"
      u8"\xc3\xae\0"
      u8"\xc3\xaf\0"
      u8"\xc3\xb0\0"
      u8"\xc3\xb1\0"
      u8"\xc3\xb2\0"
      u8"\xc3\xb3\0"
      u8"\xc3\xb4\0"
      u8"\xc3\xb5\0"
      u8"\xc3\xb6\0"
      u8"\xc3\xb8\0"
      u8"\xc3\xb9\0"
      u8"\xc3\xba\0"
      u8"\xc3\xbb\0"
      u8"\xc3\xbc\0"
      u8"\xc3\xbd\0"
      u8"\xc3\xbe\0"
      u8"\xc4\x81\0"
      u8"\xc4\x83\0"
      u8"\xc4\x85\0"
      u8"\xc4\x87\0"
      u8"\xc4\x89\0"
      u8"\xc4\x8b\0"
      u8"\xc4\x8d\0"
      u8"\xc4\x8f\0"
      u8"\xc4\x91\0"
      u8"\xc4\x93\0"
      u8"\xc4\x95\0"
      u8"\xc4\x97\0"
      u8"\xc4\x99\0"
      u8"\xc4\x9b\0"
      u8"\xc4\x9d\0"
      u8"\xc4\x9f\0"
      u8"\xc4\xa1\0"
      u8"\xc4\xa3\0"
      u8"\xc4\xa5\0"
      u8"\xc4\xa7\0"
      u8"\xc4\xa9\0"
      u8"\xc4\xab\0"
      u8"\xc4\xad\0"
      u8"\xc4\xaf\0"
      u8"\x69\xcc\x87\0"
      u8"\x69\x6a\0"
      u8"\xc4\xb5\0"
      u8"\xc4\xb7\0"
      u8"\xc4\xba\0"
      u8"\xc4\xbc\0"
      u8"\xc4\xbe\0"
      u8"\x6c\xc2\xb7\0"
      u8"\xc5\x82\0"
      u8"\xc5\x84\0"
      u8"\xc5\x86\0"
      u8"\xc5\x88\0"
      u8"\xca\xbc\x6e\0"
      u8"\xc5\x8b\0"
      u8"\xc5\x8d\0"
      u8"\xc5\x8f\0"
      u8"\xc5\x91\0"
      u8"\xc5\x93\0"
      u8"\xc5\x95\0"
      u8"\xc5\x97\0"
      u8"\xc5\x99\0"
      u8"\xc5\x9b\0"
      u8"\xc5\x9d\0"
      u8"\xc5\x9f\0"
      u8"\xc5\xa1\0"
      u8"\xc5\xa3\0"
      u8"\xc5\xa5\0"
      u8"\xc5\xa7\0"
      u8"\xc5\xa9\0"
      u8"\xc5\xab\0"
      u8"\xc5\xad\0"
      u8"\xc5\xaf\0"
      u8"\xc5\xb1\0"
      u8"\xc5\xb3\0"
      u8"\xc5\xb5\0"
      u8"\xc5\xb7\0"
      u8"\xc3\xbf\0"
      u8"\xc5\xba\0"
      u8"\xc5\xbc\0"
      u8"\xc5\xbe\0"
      u8"\xc9\x93\0"
      u8"\xc6\x83\0"
      u8"\xc6\x85\0"
      u8"\xc9\x94\0"
      u8"\xc6\x88\0"
      u8"\xc9\x96\0"
      u8"\xc9\x97\0"
      u8"\xc6\x8c\0"
      u8"\xc7\x9d\0"
      u8"\xc9\x99\0"
      u8"\xc9\x9b\0"
      u8"\xc6\x92\0"
      u8"\xc9\xa0\0"
      u8"\xc9\xa3\0"
      u8"\xc9\xa9\0"
      u8"\xc9\xa8\0"
      u8"\xc6\x99\0"
      u8"\xc9\xaf\0"
      u8"\xc9\xb2\0"
      u8"\xc9\xb5\0"
      u8"\xc6\xa1\0"
      u8"\xc6\xa3\0"
      u8"\xc6\xa5\0"
      u8"\xca\x80\0"
      u8"\xc6\xa8\0"
      u8"\xca\x83\0"
      u8"\xc6\xad\0"
      u8"\xca\x88\0"
      u8"\xc6\xb0\0"
      u8"\xca\x8a\0"
      u8"\xca\x8b\0"
      u8"\xc6\xb4\0"
      u8"\xc6\xb6\0"
      u8"\xca\x92\0"
      u8"\xc6\xb9\0"
      u8"\xc6\xbd\0"
      u8"\x64\xc5\xbe\0"
      u8"\x6c\x6a\0"
      u8"\x6e\x6a\0"
      u8"\xc7\x8e\0"
      u8"\xc7\x90\0"
      u8"\xc7\x92\0"
      u8"\xc7\x94\0"
      u8"\xc7\x96\0"
      u8"\xc7\x98\0"
      u8"\xc7\x9a\0"
      u8"\xc7\x9c\0"
      u8"\xc7\x9f\0"
      u8"\xc7\xa1\0"
      u8"\xc7\xa3\0"
      u8"\xc7\xa5\0"
      u8"\xc7\xa7\0"
      u8"\xc7\xa9\0"
      u8"\xc7\xab\0"
      u8"\xc7\xad\0"
      u8"\xc7\xaf\0"
      u8"\x64\x7a\0"
      u8"\xc7\xb5\0"
      u8"\xc6\x95\0"
      u8"\xc6\xbf\0"
      u8"\xc7\xb9\0"
      u8"\xc7\xbb\0"
      u8"\xc7\xbd\0"
      u8"\xc7\xbf\0"
      u8"\xc8\x81\0"
      u8"\xc8\x83\0"
      u8"\xc8\x85\0"
      u8"\xc8\x87\0"
      u8"\xc8\x89\0"
      u8"\xc8\x8b\0"
      u8"\xc8\x8d\0"
      u8"\xc8\x8f\0"
      u8"\xc8\x91\0"
      u8"\xc8\x93\0"
      u8"\xc8\x95\0"
      u8"\xc8\x97\0"
      u8"\xc8\x99\0"
      u8"\xc8\x9b\0"
      u8"\xc8\x9d\0"
      u8"\xc8\x9f\0"
      u8"\xc6\x9e\0"
      u8"\xc8\xa3\0"
      u8"\xc8\xa5\0"
      u8"\xc8\xa7\0"
      u8"\xc8\xa9\0"
      u8"\xc8\xab\0"
      u8"\xc8\xad\0"
      u8"\xc8\xaf\0"
      u8"\xc8\xb1\0"
      u8"\xc8\xb3\0"
      u8"\xe2\xb1\xa5\0"
      u8"\xc8\xbc\0"
      u8"\xc6\x9a\0"
      u8"\xe2\xb1\xa6\0"
      u8"\xc9\x82\0"
      u8"\xc6\x80\0"
      u8"\xca\x89\0"
      u8"\xca\x8c\0"
      u8"\xc9\x87\0"
      u8"\xc9\x89\0"
      u8"\xc9\x8b\0"
      u8"\xc9\x8d\0"
      u8"\xc9\x8f\0"
      u8"\xc9\xa6\0"
      u8"\xc9\xb9\0"
      u8"\xc9\xbb\0"
      u8"\xca\x81\0"
      u8"\40\xcc\x86\0"
      u8"\40\xcc\x87\0"
      u8"\40\xcc\x8a\0"
      u8"\40\xcc\xa8\0"
      u8"\40\xcc\x83\0"
      u8"\40\xcc\x8b\0"
      u8"\xca\x95\0"
      u8"\xcc\x80\0"
      u8"\xcc\x81\0"
      u8"\xcc\x93\0"
      u8"\xcc\x88\xcc\x81\0"
      u8"\xce\xb9\0"
      u8"\xcd\xb1\0"
      u8"\xcd\xb3\0"
      u8"\xca\xb9\0"
      u8"\xcd\xb7\0"
      u8"\40\xce\xb9\0"
      u8"\73\0"
      u8"\xcf\xb3\0"
      u8"\40\xcc\x88\xcc\x81\0"
      u8"\xce\xac\0"
      u8"\xc2\xb7\0"
      u8"\xce\xad\0"
      u8"\xce\xae\0"
      u8"\xce\xaf\0"
      u8"\xcf\x8c\0"
      u8"\xcf\x8d\0"
      u8"\xcf\x8e\0"
      u8"\xce\xb1\0"
      u8"\xce\xb2\0"
      u8"\xce\xb3\0"
      u8"\xce\xb4\0"
      u8"\xce\xb5\0"
      u8"\xce\xb6\0"
      u8"\xce\xb7\0"
      u8"\xce\xb8\0"
      u8"\xce\xba\0"
      u8"\xce\xbb\0"
      u8"\xce\xbd\0"
      u8"\xce\xbe\0"
      u8"\xce\xbf\0"
      u8"\xcf\x80\0"
      u8"\xcf\x81\0"
      u8"\xcf\x83\0"
      u8"\xcf\x84\0"
      u8"\xcf\x85\0"
      u8"\xcf\x86\0"
      u8"\xcf\x87\0"
      u8"\xcf\x88\0"
      u8"\xcf\x89\0"
      u8"\xcf\x8a\0"
      u8"\xcf\x8b\0"
      u8"\xcf\x97\0"
      u8"\xcf\x99\0"
      u8"\xcf\x9b\0"
      u8"\xcf\x9d\0"
      u8"\xcf\x9f\0"
      u8"\xcf\xa1\0"
      u8"\xcf\xa3\0"
      u8"\xcf\xa5\0"
      u8"\xcf\xa7\0"
      u8"\xcf\xa9\0"
      u8"\xcf\xab\0"
      u8"\xcf\xad\0"
      u8"\xcf\xaf\0"
      u8"\xcf\xb8\0"
      u8"\xcf\xbb\0"
      u8"\xcd\xbb\0"
      u8"\xcd\xbc\0"
      u8"\xcd\xbd\0"
      u8"\xd1\x90\0"
      u8"\xd1\x91\0"
      u8"\xd1\x92\0"
      u8"\xd1\x93\0"
      u8"\xd1\x94\0"
      u8"\xd1\x95\0"
      u8"\xd1\x96\0"
      u8"\xd1\x97\0"
      u8"\xd1\x98\0"
      u8"\xd1\x99\0"
      u8"\xd1\x9a\0"
      u8"\xd1\x9b\0"
      u8"\xd1\x9c\0"
      u8"\xd1\x9d\0"
      u8"\xd1\x9e\0"
      u8"\xd1\x9f\0"
      u8"\xd0\xb0\0"
      u8"\xd0\xb1\0"
      u8"\xd0\xb2\0"
      u8"\xd0\xb3\0"
      u8"\xd0\xb4\0"
      u8"\xd0\xb5\0"
      u8"\xd0\xb6\0"
      u8"\xd0\xb7\0"
      u8"\xd0\xb8\0"
      u8"\xd0\xb9\0"
      u8"\xd0\xba\0"
      u8"\xd0\xbb\0"
      u8"\xd0\xbc\0"
      u8"\xd0\xbd\0"
      u8"\xd0\xbe\0"
      u8"\xd0\xbf\0"
      u8"\xd1\x80\0"
      u8"\xd1\x81\0"
      u8"\xd1\x82\0"
      u8"\xd1\x83\0"
      u8"\xd1\x84\0"
      u8"\xd1\x85\0"
      u8"\xd1\x86\0"
      u8"\xd1\x87\0"
      u8"\xd1\x88\0"
      u8"\xd1\x89\0"
      u8"\xd1\x8a\0"
      u8"\xd1\x8b\0"
      u8"\xd1\x8c\0"
      u8"\xd1\x8d\0"
      u8"\xd1\x8e\0"
      u8"\xd1\x8f\0"
      u8"\xd1\xa1\0"
      u8"\xd1\xa3\0"
      u8"\xd1\xa5\0"
      u8"\xd1\xa7\0"
      u8"\xd1\xa9\0"
      u8"\xd1\xab\0"
      u8"\xd1\xad\0"
      u8"\xd1\xaf\0"
      u8"\xd1\xb1\0"
      u8"\xd1\xb3\0"
      u8"\xd1\xb5\0"
      u8"\xd1\xb7\0"
      u8"\xd1\xb9\0"
      u8"\xd1\xbb\0"
      u8"\xd1\xbd\0"
      u8"\xd1\xbf\0"
      u8"\xd2\x81\0"
      u8"\xd2\x8b\0"
      u8"\xd2\x8d\0"
      u8"\xd2\x8f\0"
      u8"\xd2\x91\0"
      u8"\xd2\x93\0"
      u8"\xd2\x95\0"
      u8"\xd2\x97\0"
      u8"\xd2\x99\0"
      u8"\xd2\x9b\0"
      u8"\xd2\x9d\0"
      u8"\xd2\x9f\0"
      u8"\xd2\xa1\0"
      u8"\xd2\xa3\0"
      u8"\xd2\xa5\0"
      u8"\xd2\xa7\0"
      u8"\xd2\xa9\0"
      u8"\xd2\xab\0"
      u8"\xd2\xad\0"
      u8"\xd2\xaf\0"
      u8"\xd2\xb1\0"
      u8"\xd2\xb3\0"
      u8"\xd2\xb5\0"
      u8"\xd2\xb7\0"
      u8"\xd2\xb9\0"
      u8"\xd2\xbb\0"
      u8"\xd2\xbd\0"
      u8"\xd2\xbf\0"
      u8"\xd3\x8f\0"
      u8"\xd3\x82\0"
      u8"\xd3\x84\0"
      u8"\xd3\x86\0"
      u8"\xd3\x88\0"
      u8"\xd3\x8a\0"
      u8"\xd3\x8c\0"
      u8"\xd3\x8e\0"
      u8"\xd3\x91\0"
      u8"\xd3\x93\0"
      u8"\xd3\x95\0"
      u8"\xd3\x97\0"
      u8"\xd3\x99\0"
      u8"\xd3\x9b\0"
      u8"\xd3\x9d\0"
      u8"\xd3\x9f\0"
      u8"\xd3\xa1\0"
      u8"\xd3\xa3\0"
      u8"\xd3\xa5\0"
      u8"\xd3\xa7\0"
      u8"\xd3\xa9\0"
      u8"\xd3\xab\0"
      u8"\xd3\xad\0"
      u8"\xd3\xaf\0"
      u8"\xd3\xb1\0"
      u8"\xd3\xb3\0"
      u8"\xd3\xb5\0"
      u8"\xd3\xb7\0"
      u8"\xd3\xb9\0"
      u8"\xd3\xbb\0"
      u8"\xd3\xbd\0"
      u8"\xd3\xbf\0"
      u8"\xd4\x81\0"
      u8"\xd4\x83\0"
      u8"\xd4\x85\0"
      u8"\xd4\x87\0"
      u8"\xd4\x89\0"
      u8"\xd4\x8b\0"
      u8"\xd4\x8d\0"
      u8"\xd4\x8f\0"
      u8"\xd4\x91\0"
      u8"\xd4\x93\0"
      u8"\xd4\x95\0"
      u8"\xd4\x97\0"
      u8"\xd4\x99\0"
      u8"\xd4\x9b\0"
      u8"\xd4\x9d\0"
      u8"\xd4\x9f\0"
      u8"\xd4\xa1\0"
      u8"\xd4\xa3\0"
      u8"\xd4\xa5\0"
      u8"\xd4\xa7\0"
      u8"\xd4\xa9\0"
      u8"\xd4\xab\0"
      u8"\xd4\xad\0"
      u8"\xd4\xaf\0"
      u8"\xd5\xa1\0"
      u8"\xd5\xa2\0"
      u8"\xd5\xa3\0"
      u8"\xd5\xa4\0"
      u8"\xd5\xa5\0"
      u8"\xd5\xa6\0"
      u8"\xd5\xa7\0"
      u8"\xd5\xa8\0"
      u8"\xd5\xa9\0"
      u8"\xd5\xaa\0"
      u8"\xd5\xab\0"
      u8"\xd5\xac\0"
      u8"\xd5\xad\0"
      u8"\xd5\xae\0"
      u8"\xd5\xaf\0"
      u8"\xd5\xb0\0"
      u8"\xd5\xb1\0"
      u8"\xd5\xb2\0"
      u8"\xd5\xb3\0"
      u8"\xd5\xb4\0"
      u8"\xd5\xb5\0"
      u8"\xd5\xb6\0"
      u8"\xd5\xb7\0"
      u8"\xd5\xb8\0"
      u8"\xd5\xb9\0"
      u8"\xd5\xba\0"
      u8"\xd5\xbb\0"
      u8"\xd5\xbc\0"
      u8"\xd5\xbd\0"
      u8"\xd5\xbe\0"
      u8"\xd5\xbf\0"
      u8"\xd6\x80\0"
      u8"\xd6\x81\0"
      u8"\xd6\x82\0"
      u8"\xd6\x83\0"
      u8"\xd6\x84\0"
      u8"\xd6\x85\0"
      u8"\xd6\x86\0"
      u8"\xd5\xa5\xd6\x82\0"
      u8"\xd8\xa7\xd9\xb4\0"
      u8"\xd9\x88\xd9\xb4\0"
      u8"\xdb\x87\xd9\xb4\0"
      u8"\xd9\x8a\xd9\xb4\0"
      u8"\xe0\xa4\x95\xe0\xa4\xbc\0"
      u8"\xe0\xa4\x96\xe0\xa4\xbc\0"
      u8"\xe0\xa4\x97\xe0\xa4\xbc\0"
      u8"\xe0\xa4\x9c\xe0\xa4\xbc\0"
      u8"\xe0\xa4\xa1\xe0\xa4\xbc\0"
      u8"\xe0\xa4\xa2\xe0\xa4\xbc\0"
      u8"\xe0\xa4\xab\xe0\xa4\xbc\0"
      u8"\xe0\xa4\xaf\xe0\xa4\xbc\0"
      u8"\xe0\xa6\xa1\xe0\xa6\xbc\0"
      u8"\xe0\xa6\xa2\xe0\xa6\xbc\0"
      u8"\xe0\xa6\xaf\xe0\xa6\xbc\0"
      u8"\xe0\xa8\xb2\xe0\xa8\xbc\0"
      u8"\xe0\xa8\xb8\xe0\xa8\xbc\0"
      u8"\xe0\xa8\x96\xe0\xa8\xbc\0"
      u8"\xe0\xa8\x97\xe0\xa8\xbc\0"
      u8"\xe0\xa8\x9c\xe0\xa8\xbc\0"
      u8"\xe0\xa8\xab\xe0\xa8\xbc\0"
      u8"\xe0\xac\xa1\xe0\xac\xbc\0"
      u8"\xe0\xac\xa2\xe0\xac\xbc\0"
      u8"\xe0\xb9\x8d\xe0\xb8\xb2\0"
      u8"\xe0\xbb\x8d\xe0\xba\xb2\0"
      u8"\xe0\xba\xab\xe0\xba\x99\0"
      u8"\xe0\xba\xab\xe0\xba\xa1\0"
      u8"\xe0\xbc\x8b\0"
      u8"\xe0\xbd\x82\xe0\xbe\xb7\0"
      u8"\xe0\xbd\x8c\xe0\xbe\xb7\0"
      u8"\xe0\xbd\x91\xe0\xbe\xb7\0"
      u8"\xe0\xbd\x96\xe0\xbe\xb7\0"
      u8"\xe0\xbd\x9b\xe0\xbe\xb7\0"
      u8"\xe0\xbd\x80\xe0\xbe\xb5\0"
      u8"\xe0\xbd\xb1\xe0\xbd\xb2\0"
      u8"\xe0\xbd\xb1\xe0\xbd\xb4\0"
      u8"\xe0\xbe\xb2\xe0\xbe\x80\0"
      u8"\xe0\xbe\xb2\xe0\xbd\xb1\xe0\xbe\x80\0"
      u8"\xe0\xbe\xb3\xe0\xbe\x80\0"
      u8"\xe0\xbe\xb3\xe0\xbd\xb1\xe0\xbe\x80\0"
      u8"\xe0\xbd\xb1\xe0\xbe\x80\0"
      u8"\xe0\xbe\x92\xe0\xbe\xb7\0"
      u8"\xe0\xbe\x9c\xe0\xbe\xb7\0"
      u8"\xe0\xbe\xa1\xe0\xbe\xb7\0"
      u8"\xe0\xbe\xa6\xe0\xbe\xb7\0"
      u8"\xe0\xbe\xab\xe0\xbe\xb7\0"
      u8"\xe0\xbe\x90\xe0\xbe\xb5\0"
      u8"\xe2\xb4\x80\0"
      u8"\xe2\xb4\x81\0"
      u8"\xe2\xb4\x82\0"
      u8"\xe2\xb4\x83\0"
      u8"\xe2\xb4\x84\0"
      u8"\xe2\xb4\x85\0"
      u8"\xe2\xb4\x86\0"
      u8"\xe2\xb4\x87\0"
      u8"\xe2\xb4\x88\0"
      u8"\xe2\xb4\x89\0"
      u8"\xe2\xb4\x8a\0"
      u8"\xe2\xb4\x8b\0"
      u8"\xe2\xb4\x8c\0"
      u8"\xe2\xb4\x8d\0"
      u8"\xe2\xb4\x8e\0"
      u8"\xe2\xb4\x8f\0"
      u8"\xe2\xb4\x90\0"
      u8"\xe2\xb4\x91\0"
      u8"\xe2\xb4\x92\0"
      u8"\xe2\xb4\x93\0"
      u8"\xe2\xb4\x94\0"
      u8"\xe2\xb4\x95\0"
      u8"\xe2\xb4\x96\0"
      u8"\xe2\xb4\x97\0"
      u8"\xe2\xb4\x98\0"
      u8"\xe2\xb4\x99\0"
      u8"\xe2\xb4\x9a\0"
      u8"\xe2\xb4\x9b\0"
      u8"\xe2\xb4\x9c\0"
      u8"\xe2\xb4\x9d\0"
      u8"\xe2\xb4\x9e\0"
      u8"\xe2\xb4\x9f\0"
      u8"\xe2\xb4\xa0\0"
      u8"\xe2\xb4\xa1\0"
      u8"\xe2\xb4\xa2\0"
      u8"\xe2\xb4\xa3\0"
      u8"\xe2\xb4\xa4\0"
      u8"\xe2\xb4\xa5\0"
      u8"\xe2\xb4\xa7\0"
      u8"\xe2\xb4\xad\0"
      u8"\xe1\x83\x9c\0"
      u8"\xe1\x8f\xb0\0"
      u8"\xe1\x8f\xb1\0"
      u8"\xe1\x8f\xb2\0"
      u8"\xe1\x8f\xb3\0"
      u8"\xe1\x8f\xb4\0"
      u8"\xe1\x8f\xb5\0"
      u8"\xea\x99\x8b\0"
      u8"\xe1\xb2\x8a\0"
      u8"\xe1\x83\x90\0"
      u8"\xe1\x83\x91\0"
      u8"\xe1\x83\x92\0"
      u8"\xe1\x83\x93\0"
      u8"\xe1\x83\x94\0"
      u8"\xe1\x83\x95\0"
      u8"\xe1\x83\x96\0"
      u8"\xe1\x83\x97\0"
      u8"\xe1\x83\x98\0"
      u8"\xe1\x83\x99\0"
      u8"\xe1\x83\x9a\0"
      u8"\xe1\x83\x9b\0"
      u8"\xe1\x83\x9d\0"
      u8"\xe1\x83\x9e\0"
      u8"\xe1\x83\x9f\0"
      u8"\xe1\x83\xa0\0"
      u8"\xe1\x83\xa1\0"
      u8"\xe1\x83\xa2\0"
      u8"\xe1\x83\xa3\0"
      u8"\xe1\x83\xa4\0"
      u8"\xe1\x83\xa5\0"
      u8"\xe1\x83\xa6\0"
      u8"\xe1\x83\xa7\0"
      u8"\xe1\x83\xa8\0"
      u8"\xe1\x83\xa9\0"
      u8"\xe1\x83\xaa\0"
      u8"\xe1\x83\xab\0"
      u8"\xe1\x83\xac\0"
      u8"\xe1\x83\xad\0"
      u8"\xe1\x83\xae\0"
      u8"\xe1\x83\xaf\0"
      u8"\xe1\x83\xb0\0"
      u8"\xe1\x83\xb1\0"
      u8"\xe1\x83\xb2\0"
      u8"\xe1\x83\xb3\0"
      u8"\xe1\x83\xb4\0"
      u8"\xe1\x83\xb5\0"
      u8"\xe1\x83\xb6\0"
      u8"\xe1\x83\xb7\0"
      u8"\xe1\x83\xb8\0"
      u8"\xe1\x83\xb9\0"
      u8"\xe1\x83\xba\0"
      u8"\xe1\x83\xbd\0"
      u8"\xe1\x83\xbe\0"
      u8"\xe1\x83\xbf\0"
      u8"\xc9\x90\0"
      u8"\xc9\x91\0"
      u8"\xe1\xb4\x82\0"
      u8"\xc9\x9c\0"
      u8"\xe1\xb4\x96\0"
      u8"\xe1\xb4\x97\0"
      u8"\xe1\xb4\x9d\0"
      u8"\xe1\xb4\xa5\0"
      u8"\xc9\x92\0"
      u8"\xc9\x95\0"
      u8"\xc9\x9f\0"
      u8"\xc9\xa1\0"
      u8"\xc9\xa5\0"
      u8"\xc9\xaa\0"
      u8"\xe1\xb5\xbb\0"
      u8"\xca\x9d\0"
      u8"\xc9\xad\0"
      u8"\xe1\xb6\x85\0"
      u8"\xca\x9f\0"
      u8"\xc9\xb1\0"
      u8"\xc9\xb0\0"
      u8"\xc9\xb3\0"
      u8"\xc9\xb4\0"
      u8"\xc9\xb8\0"
      u8"\xca\x82\0"
      u8"\xc6\xab\0"
      u8"\xe1\xb4\x9c\0"
      u8"\xca\x90\0"
      u8"\xca\x91\0"
      u8"\xe1\xb8\x81\0"
      u8"\xe1\xb8\x83\0"
      u8"\xe1\xb8\x85\0"
      u8"\xe1\xb8\x87\0"
      u8"\xe1\xb8\x89\0"
      u8"\xe1\xb8\x8b\0"
      u8"\xe1\xb8\x8d\0"
      u8"\xe1\xb8\x8f\0"
      u8"\xe1\xb8\x91\0"
      u8"\xe1\xb8\x93\0"
      u8"\xe1\xb8\x95\0"
      u8"\xe1\xb8\x97\0"
      u8"\xe1\xb8\x99\0"
      u8"\xe1\xb8\x9b\0"
      u8"\xe1\xb8\x9d\0"
      u8"\xe1\xb8\x9f\0"
      u8"\xe1\xb8\xa1\0"
      u8"\xe1\xb8\xa3\0"
      u8"\xe1\xb8\xa5\0"
      u8"\xe1\xb8\xa7\0"
      u8"\xe1\xb8\xa9\0"
      u8"\xe1\xb8\xab\0"
      u8"\xe1\xb8\xad\0"
      u8"\xe1\xb8\xaf\0"
      u8"\xe1\xb8\xb1\0"
      u8"\xe1\xb8\xb3\0"
      u8"\xe1\xb8\xb5\0"
      u8"\xe1\xb8\xb7\0"
      u8"\xe1\xb8\xb9\0"
      u8"\xe1\xb8\xbb\0"
      u8"\xe1\xb8\xbd\0"
      u8"\xe1\xb8\xbf\0"
      u8"\xe1\xb9\x81\0"
      u8"\xe1\xb9\x83\0"
      u8"\xe1\xb9\x85\0"
      u8"\xe1\xb9\x87\0"
      u8"\xe1\xb9\x89\0"
      u8"\xe1\xb9\x8b\0"
      u8"\xe1\xb9\x8d\0"
      u8"\xe1\xb9\x8f\0"
      u8"\xe1\xb9\x91\0"
      u8"\xe1\xb9\x93\0"
      u8"\xe1\xb9\x95\0"
      u8"\xe1\xb9\x97\0"
      u8"\xe1\xb9\x99\0"
      u8"\xe1\xb9\x9b\0"
      u8"\xe1\xb9\x9d\0"
      u8"\xe1\xb9\x9f\0"
      u8"\xe1\xb9\xa1\0"
      u8"\xe1\xb9\xa3\0"
      u8"\xe1\xb9\xa5\0"
      u8"\xe1\xb9\xa7\0"
      u8"\xe1\xb9\xa9\0"
      u8"\xe1\xb9\xab\0"
      u8"\xe1\xb9\xad\0"
      u8"\xe1\xb9\xaf\0"
      u8"\xe1\xb9\xb1\0"
      u8"\xe1\xb9\xb3\0"
      u8"\xe1\xb9\xb5\0"
      u8"\xe1\xb9\xb7\0"
      u8"\xe1\xb9\xb9\0"
      u8"\xe1\xb9\xbb\0"
      u8"\xe1\xb9\xbd\0"
      u8"\xe1\xb9\xbf\0"
      u8"\xe1\xba\x81\0"
      u8"\xe1\xba\x83\0"
      u8"\xe1\xba\x85\0"
      u8"\xe1\xba\x87\0"
      u8"\xe1\xba\x89\0"
      u8"\xe1\xba\x8b\0"
      u8"\xe1\xba\x8d\0"
      u8"\xe1\xba\x8f\0"
      u8"\xe1\xba\x91\0"
      u8"\xe1\xba\x93\0"
      u8"\xe1\xba\x95\0"
      u8"\x61\xca\xbe\0"
      u8"\xc3\x9f\0"
      u8"\xe1\xba\xa1\0"
      u8"\xe1\xba\xa3\0"
      u8"\xe1\xba\xa5\0"
      u8"\xe1\xba\xa7\0"
      u8"\xe1\xba\xa9\0"
      u8"\xe1\xba\xab\0"
      u8"\xe1\xba\xad\0"
      u8"\xe1\xba\xaf\0"
      u8"\xe1\xba\xb1\0"
      u8"\xe1\xba\xb3\0"
      u8"\xe1\xba\xb5\0"
      u8"\xe1\xba\xb7\0"
      u8"\xe1\xba\xb9\0"
      u8"\xe1\xba\xbb\0"
      u8"\xe1\xba\xbd\0"
      u8"\xe1\xba\xbf\0"
      u8"\xe1\xbb\x81\0"
      u8"\xe1\xbb\x83\0"
      u8"\xe1\xbb\x85\0"
      u8"\xe1\xbb\x87\0"
      u8"\xe1\xbb\x89\0"
      u8"\xe1\xbb\x8b\0"
      u8"\xe1\xbb\x8d\0"
      u8"\xe1\xbb\x8f\0"
      u8"\xe1\xbb\x91\0"
      u8"\xe1\xbb\x93\0"
      u8"\xe1\xbb\x95\0"
      u8"\xe1\xbb\x97\0"
      u8"\xe1\xbb\x99\0"
      u8"\xe1\xbb\x9b\0"
      u8"\xe1\xbb\x9d\0"
      u8"\xe1\xbb\x9f\0"
      u8"\xe1\xbb\xa1\0"
      u8"\xe1\xbb\xa3\0"
      u8"\xe1\xbb\xa5\0"
      u8"\xe1\xbb\xa7\0"
      u8"\xe1\xbb\xa9\0"
      u8"\xe1\xbb\xab\0"
      u8"\xe1\xbb\xad\0"
      u8"\xe1\xbb\xaf\0"
      u8"\xe1\xbb\xb1\0"
      u8"\xe1\xbb\xb3\0"
      u8"\xe1\xbb\xb5\0"
      u8"\xe1\xbb\xb7\0"
      u8"\xe1\xbb\xb9\0"
      u8"\xe1\xbb\xbb\0"
      u8"\xe1\xbb\xbd\0"
      u8"\xe1\xbb\xbf\0"
      u8"\xe1\xbc\x80\0"
      u8"\xe1\xbc\x81\0"
      u8"\xe1\xbc\x82\0"
      u8"\xe1\xbc\x83\0"
      u8"\xe1\xbc\x84\0"
      u8"\xe1\xbc\x85\0"
      u8"\xe1\xbc\x86\0"
      u8"\xe1\xbc\x87\0"
      u8"\xe1\xbc\x90\0"
      u8"\xe1\xbc\x91\0"
      u8"\xe1\xbc\x92\0"
      u8"\xe1\xbc\x93\0"
      u8"\xe1\xbc\x94\0"
      u8"\xe1\xbc\x95\0"
      u8"\xe1\xbc\xa0\0"
      u8"\xe1\xbc\xa1\0"
      u8"\xe1\xbc\xa2\0"
      u8"\xe1\xbc\xa3\0"
      u8"\xe1\xbc\xa4\0"
      u8"\xe1\xbc\xa5\0"
      u8"\xe1\xbc\xa6\0"
      u8"\xe1\xbc\xa7\0"
      u8"\xe1\xbc\xb0\0"
      u8"\xe1\xbc\xb1\0"
      u8"\xe1\xbc\xb2\0"
      u8"\xe1\xbc\xb3\0"
      u8"\xe1\xbc\xb4\0"
      u8"\xe1\xbc\xb5\0"
      u8"\xe1\xbc\xb6\0"
      u8"\xe1\xbc\xb7\0"
      u8"\xe1\xbd\x80\0"
      u8"\xe1\xbd\x81\0"
      u8"\xe1\xbd\x82\0"
      u8"\xe1\xbd\x83\0"
      u8"\xe1\xbd\x84\0"
      u8"\xe1\xbd\x85\0"
      u8"\xe1\xbd\x91\0"
      u8"\xe1\xbd\x93\0"
      u8"\xe1\xbd\x95\0"
      u8"\xe1\xbd\x97\0"
      u8"\xe1\xbd\xa0\0"
      u8"\xe1\xbd\xa1\0"
      u8"\xe1\xbd\xa2\0"
      u8"\xe1\xbd\xa3\0"
      u8"\xe1\xbd\xa4\0"
      u8"\xe1\xbd\xa5\0"
      u8"\xe1\xbd\xa6\0"
      u8"\xe1\xbd\xa7\0"
      u8"\xe1\xbc\x80\xce\xb9\0"
      u8"\xe1\xbc\x81\xce\xb9\0"
      u8"\xe1\xbc\x82\xce\xb9\0"
      u8"\xe1\xbc\x83\xce\xb9\0"
      u8"\xe1\xbc\x84\xce\xb9\0"
      u8"\xe1\xbc\x85\xce\xb9\0"
      u8"\xe1\xbc\x86\xce\xb9\0"
      u8"\xe1\xbc\x87\xce\xb9\0"
      u8"\xe1\xbc\xa0\xce\xb9\0"
      u8"\xe1\xbc\xa1\xce\xb9\0"
      u8"\xe1\xbc\xa2\xce\xb9\0"
      u8"\xe1\xbc\xa3\xce\xb9\0"
      u8"\xe1\xbc\xa4\xce\xb9\0"
      u8"\xe1\xbc\xa5\xce\xb9\0"
      u8"\xe1\xbc\xa6\xce\xb9\0"
      u8"\xe1\xbc\xa7\xce\xb9\0"
      u8"\xe1\xbd\xa0\xce\xb9\0"
      u8"\xe1\xbd\xa1\xce\xb9\0"
      u8"\xe1\xbd\xa2\xce\xb9\0"
      u8"\xe1\xbd\xa3\xce\xb9\0"
      u8"\xe1\xbd\xa4\xce\xb9\0"
      u8"\xe1\xbd\xa5\xce\xb9\0"
      u8"\xe1\xbd\xa6\xce\xb9\0"
      u8"\xe1\xbd\xa7\xce\xb9\0"
      u8"\xe1\xbd\xb0\xce\xb9\0"
      u8"\xce\xb1\xce\xb9\0"
      u8"\xce\xac\xce\xb9\0"
      u8"\xe1\xbe\xb6\xce\xb9\0"
      u8"\xe1\xbe\xb0\0"
      u8"\xe1\xbe\xb1\0"
      u8"\xe1\xbd\xb0\0"
      u8"\40\xcc\x93\0"
      u8"\40\xcd\x82\0"
      u8"\40\xcc\x88\xcd\x82\0"
      u8"\xe1\xbd\xb4\xce\xb9\0"
      u8"\xce\xb7\xce\xb9\0"
      u8"\xce\xae\xce\xb9\0"
      u8"\xe1\xbf\x86\xce\xb9\0"
      u8"\xe1\xbd\xb2\0"
      u8"\xe1\xbd\xb4\0"
      u8"\40\xcc\x93\xcc\x80\0"
      u8"\40\xcc\x93\xcc\x81\0"
      u8"\40\xcc\x93\xcd\x82\0"
      u8"\xce\x90\0"
      u8"\xe1\xbf\x90\0"
      u8"\xe1\xbf\x91\0"
      u8"\xe1\xbd\xb6\0"
      u8"\40\xcc\x94\xcc\x80\0"
      u8"\40\xcc\x94\xcc\x81\0"
      u8"\40\xcc\x94\xcd\x82\0"
      u8"\xce\xb0\0"
      u8"\xe1\xbf\xa0\0"
      u8"\xe1\xbf\xa1\0"
      u8"\xe1\xbd\xba\0"
      u8"\xe1\xbf\xa5\0"
      u8"\40\xcc\x88\xcc\x80\0"
      u8"\x60\0"
      u8"\xe1\xbd\xbc\xce\xb9\0"
      u8"\xcf\x89\xce\xb9\0"
      u8"\xcf\x8e\xce\xb9\0"
      u8"\xe1\xbf\xb6\xce\xb9\0"
      u8"\xe1\xbd\xb8\0"
      u8"\xe1\xbd\xbc\0"
      u8"\40\xcc\x94\0"
      u8"\xe2\x80\x90\0"
      u8"\40\xcc\xb3\0"
      u8"\xe2\x80\xb2\xe2\x80\xb2\0"
      u8"\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\0"
      u8"\xe2\x80\xb5\xe2\x80\xb5\0"
      u8"\xe2\x80\xb5\xe2\x80\xb5\xe2\x80\xb5\0"
      u8"\41\41\0"
      u8"\40\xcc\x85\0"
      u8"\77\77\0"
      u8"\77\41\0"
      u8"\41\77\0"
      u8"\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\xe2\x80\xb2\0"
      u8"\60\0"
      u8"\64\0"
      u8"\65\0"
      u8"\66\0"
      u8"\67\0"
      u8"\70\0"
      u8"\71\0"
      u8"\53\0"
      u8"\xe2\x88\x92\0"
      u8"\75\0"
      u8"\50\0"
      u8"\51\0"
      u8"\x72\x73\0"
      u8"\x61\57\x63\0"
      u8"\x61\57\x73\0"
      u8"\xc2\xb0\x63\0"
      u8"\x63\57\x6f\0"
      u8"\x63\57\x75\0"
      u8"\xc2\xb0\x66\0"
      u8"\x6e\x6f\0"
      u8"\x73\x6d\0"
      u8"\x74\x65\x6c\0"
      u8"\x74\x6d\0"
      u8"\xe2\x85\x8e\0"
      u8"\xd7\x90\0"
      u8"\xd7\x91\0"
      u8"\xd7\x92\0"
      u8"\xd7\x93\0"
      u8"\x66\x61\x78\0"
      u8"\xe2\x88\x91\0"
      u8"\61\xe2\x81\x84\67\0"
      u8"\61\xe2\x81\x84\71\0"
      u8"\61\xe2\x81\x84\61\60\0"
      u8"\61\xe2\x81\x84\63\0"
      u8"\62\xe2\x81\x84\63\0"
      u8"\61\xe2\x81\x84\65\0"
      u8"\62\xe2\x81\x84\65\0"
      u8"\63\xe2\x81\x84\65\0"
      u8"\64\xe2\x81\x84\65\0"
      u8"\61\xe2\x81\x84\66\0"
      u8"\65\xe2\x81\x84\66\0"
      u8"\61\xe2\x81\x84\70\0"
      u8"\63\xe2\x81\x84\70\0"
      u8"\65\xe2\x81\x84\70\0"
      u8"\67\xe2\x81\x84\70\0"
      u8"\61\xe2\x81\x84\0"
      u8"\x69\x69\0"
      u8"\x69\x69\x69\0"
      u8"\x69\x76\0"
      u8"\x76\x69\0"
      u8"\x76\x69\x69\0"
      u8"\x76\x69\x69\x69\0"
      u8"\x69\x78\0"
      u8"\x78\x69\0"
      u8"\x78\x69\x69\0"
      u8"\xe2\x86\x84\0"
      u8"\60\xe2\x81\x84\63\0"
      u8"\xe2\x88\xab\xe2\x88\xab\0"
      u8"\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\0"
      u8"\xe2\x88\xae\xe2\x88\xae\0"
      u8"\xe2\x88\xae\xe2\x88\xae\xe2\x88\xae\0"
      u8"\xe3\x80\x88\0"
      u8"\xe3\x80\x89\0"
      u8"\61\60\0"
      u8"\61\61\0"
      u8"\61\62\0"
      u8"\61\63\0"
      u8"\61\64\0"
      u8"\61\65\0"
      u8"\61\66\0"
      u8"\61\67\0"
      u8"\61\70\0"
      u8"\61\71\0"
      u8"\62\60\0"
      u8"\50\61\51\0"
      u8"\50\62\51\0"
      u8"\50\63\51\0"
      u8"\50\64\51\0"
      u8"\50\65\51\0"
      u8"\50\66\51\0"
      u8"\50\67\51\0"
      u8"\50\70\51\0"
      u8"\50\71\51\0"
      u8"\50\61\60\51\0"
      u8"\50\61\61\51\0"
      u8"\50\61\62\51\0"
      u8"\50\61\63\51\0"
      u8"\50\61\64\51\0"
      u8"\50\61\65\51\0"
      u8"\50\61\66\51\0"
      u8"\50\61\67\51\0"
      u8"\50\61\70\51\0"
      u8"\50\61\71\51\0"
      u8"\50\62\60\51\0"
      u8"\50\x61\51\0"
      u8"\50\x62\51\0"
      u8"\50\x63\51\0"
      u8"\50\x64\51\0"
      u8"\50\x65\51\0"
      u8"\50\x66\51\0"
      u8"\50\x67\51\0"
      u8"\50\x68\51\0"
      u8"\50\x69\51\0"
      u8"\50\x6a\51\0"
      u8"\50\x6b\51\0"
      u8"\50\x6c\51\0"
      u8"\50\x6d\51\0"
      u8"\50\x6e\51\0"
      u8"\50\x6f\51\0"
      u8"\50\x70\51\0"
      u8"\50\x71\51\0"
      u8"\50\x72\51\0"
      u8"\50\x73\51\0"
      u8"\50\x74\51\0"
      u8"\50\x75\51\0"
      u8"\50\x76\51\0"
      u8"\50\x77\51\0"
      u8"\50\x78\51\0"
      u8"\50\x79\51\0"
      u8"\50\x7a\51\0"
      u8"\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\xe2\x88\xab\0"
      u8"\72\72\75\0"
      u8"\75\75\0"
      u8"\75\75\75\0"
      u8"\xe2\xab\x9d\xcc\xb8\0"
      u8"\xe2\xb0\xb0\0"
      u8"\xe2\xb0\xb1\0"
      u8"\xe2\xb0\xb2\0"
      u8"\xe2\xb0\xb3\0"
      u8"\xe2\xb0\xb4\0"
      u8"\xe2\xb0\xb5\0"
      u8"\xe2\xb0\xb6\0"
      u8"\xe2\xb0\xb7\0"
      u8"\xe2\xb0\xb8\0"
      u8"\xe2\xb0\xb9\0"
      u8"\xe2\xb0\xba\0"
      u8"\xe2\xb0\xbb\0"
      u8"\xe2\xb0\xbc\0"
      u8"\xe2\xb0\xbd\0"
      u8"\xe2\xb0\xbe\0"
      u8"\xe2\xb0\xbf\0"
      u8"\xe2\xb1\x80\0"
      u8"\xe2\xb1\x81\0"
      u8"\xe2\xb1\x82\0"
      u8"\xe2\xb1\x83\0"
      u8"\xe2\xb1\x84\0"
      u8"\xe2\xb1\x85\0"
      u8"\xe2\xb1\x86\0"
      u8"\xe2\xb1\x87\0"
      u8"\xe2\xb1\x88\0"
      u8"\xe2\xb1\x89\0"
      u8"\xe2\xb1\x8a\0"
      u8"\xe2\xb1\x8b\0"
      u8"\xe2\xb1\x8c\0"
      u8"\xe2\xb1\x8d\0"
      u8"\xe2\xb1\x8e\0"
      u8"\xe2\xb1\x8f\0"
      u8"\xe2\xb1\x90\0"
      u8"\xe2\xb1\x91\0"
      u8"\xe2\xb1\x92\0"
      u8"\xe2\xb1\x93\0"
      u8"\xe2\xb1\x94\0"
      u8"\xe2\xb1\x95\0"
      u8"\xe2\xb1\x96\0"
      u8"\xe2\xb1\x97\0"
      u8"\xe2\xb1\x98\0"
      u8"\xe2\xb1\x99\0"
      u8"\xe2\xb1\x9a\0"
      u8"\xe2\xb1\x9b\0"
      u8"\xe2\xb1\x9c\0"
      u8"\xe2\xb1\x9d\0"
      u8"\xe2\xb1\x9e\0"
      u8"\xe2\xb1\x9f\0"
      u8"\xe2\xb1\xa1\0"
      u8"\xc9\xab\0"
      u8"\xe1\xb5\xbd\0"
      u8"\xc9\xbd\0"
      u8"\xe2\xb1\xa8\0"
      u8"\xe2\xb1\xaa\0"
      u8"\xe2\xb1\xac\0"
      u8"\xe2\xb1\xb3\0"
      u8"\xe2\xb1\xb6\0"
      u8"\xc8\xbf\0"
      u8"\xc9\x80\0"
      u8"\xe2\xb2\x81\0"
      u8"\xe2\xb2\x83\0"
      u8"\xe2\xb2\x85\0"
      u8"\xe2\xb2\x87\0"
      u8"\xe2\xb2\x89\0"
      u8"\xe2\xb2\x8b\0"
      u8"\xe2\xb2\x8d\0"
      u8"\xe2\xb2\x8f\0"
      u8"\xe2\xb2\x91\0"
      u8"\xe2\xb2\x93\0"
      u8"\xe2\xb2\x95\0"
      u8"\xe2\xb2\x97\0"
      u8"\xe2\xb2\x99\0"
      u8"\xe2\xb2\x9b\0"
      u8"\xe2\xb2\x9d\0"
      u8"\xe2\xb2\x9f\0"
      u8"\xe2\xb2\xa1\0"
      u8"\xe2\xb2\xa3\0"
      u8"\xe2\xb2\xa5\0"
      u8"\xe2\xb2\xa7\0"
      u8"\xe2\xb2\xa9\0"
      u8"\xe2\xb2\xab\0"
      u8"\xe2\xb2\xad\0"
      u8"\xe2\xb2\xaf\0"
      u8"\xe2\xb2\xb1\0"
      u8"\xe2\xb2\xb3\0"
      u8"\xe2\xb2\xb5\0"
      u8"\xe2\xb2\xb7\0"
      u8"\xe2\xb2\xb9\0"
      u8"\xe2\xb2\xbb\0"
      u8"\xe2\xb2\xbd\0"
      u8"\xe2\xb2\xbf\0"
      u8"\xe2\xb3\x81\0"
      u8"\xe2\xb3\x83\0"
      u8"\xe2\xb3\x85\0"
      u8"\xe2\xb3\x87\0"
      u8"\xe2\xb3\x89\0"
      u8"\xe2\xb3\x8b\0"
      u8"\xe2\xb3\x8d\0"
      u8"\xe2\xb3\x8f\0"
      u8"\xe2\xb3\x91\0"
      u8"\xe2\xb3\x93\0"
      u8"\xe2\xb3\x95\0"
      u8"\xe2\xb3\x97\0"
      u8"\xe2\xb3\x99\0"
      u8"\xe2\xb3\x9b\0"
      u8"\xe2\xb3\x9d\0"
      u8"\xe2\xb3\x9f\0"
      u8"\xe2\xb3\xa1\0"
      u8"\xe2\xb3\xa3\0"
      u8"\xe2\xb3\xac\0"
      u8"\xe2\xb3\xae\0"
      u8"\xe2\xb3\xb3\0"
      u8"\xe2\xb5\xa1\0"
      u8"\xe6\xaf\x8d\0"
      u8"\xe9\xbe\x9f\0"
      u8"\xe4\xb8\x80\0"
      u8"\xe4\xb8\xa8\0"
      u8"\xe4\xb8\xb6\0"
      u8"\xe4\xb8\xbf\0"
      u8"\xe4\xb9\x99\0"
      u8"\xe4\xba\x85\0"
      u8"\xe4\xba\x8c\0"
      u8"\xe4\xba\xa0\0"
      u8"\xe4\xba\xba\0"
      u8"\xe5\x84\xbf\0"
      u8"\xe5\x85\xa5\0"
      u8"\xe5\x85\xab\0"
      u8"\xe5\x86\x82\0"
      u8"\xe5\x86\x96\0"
      u8"\xe5\x86\xab\0"
      u8"\xe5\x87\xa0\0"
      u8"\xe5\x87\xb5\0"
      u8"\xe5\x88\x80\0"
      u8"\xe5\x8a\x9b\0"
      u8"\xe5\x8b\xb9\0"
      u8"\xe5\x8c\x95\0"
      u8"\xe5\x8c\x9a\0"
      u8"\xe5\x8c\xb8\0"
      u8"\xe5\x8d\x81\0"
      u8"\xe5\x8d\x9c\0"
      u8"\xe5\x8d\xa9\0"
      u8"\xe5\x8e\x82\0"
      u8"\xe5\x8e\xb6\0"
      u8"\xe5\x8f\x88\0"
      u8"\xe5\x8f\xa3\0"
      u8"\xe5\x9b\x97\0"
      u8"\xe5\x9c\x9f\0"
      u8"\xe5\xa3\xab\0"
      u8"\xe5\xa4\x82\0"
      u8"\xe5\xa4\x8a\0"
      u8"\xe5\xa4\x95\0"
      u8"\xe5\xa4\xa7\0"
      u8"\xe5\xa5\xb3\0"
      u8"\xe5\xad\x90\0"
      u8"\xe5\xae\x80\0"
      u8"\xe5\xaf\xb8\0"
      u8"\xe5\xb0\x8f\0"
      u8"\xe5\xb0\xa2\0"
      u8"\xe5\xb0\xb8\0"
      u8"\xe5\xb1\xae\0"
      u8"\xe5\xb1\xb1\0"
      u8"\xe5\xb7\x9b\0"
      u8"\xe5\xb7\xa5\0"
      u8"\xe5\xb7\xb1\0"
      u8"\xe5\xb7\xbe\0"
      u8"\xe5\xb9\xb2\0"
      u8"\xe5\xb9\xba\0"
      u8"\xe5\xb9\xbf\0"
      u8"\xe5\xbb\xb4\0"
      u8"\xe5\xbb\xbe\0"
      u8"\xe5\xbc\x8b\0"
      u8"\xe5\xbc\x93\0"
      u8"\xe5\xbd\x90\0"
      u8"\xe5\xbd\xa1\0"
      u8"\xe5\xbd\xb3\0"
      u8"\xe5\xbf\x83\0"
      u8"\xe6\x88\x88\0"
      u8"\xe6\x88\xb6\0"
      u8"\xe6\x89\x8b\0"
      u8"\xe6\x94\xaf\0"
      u8"\xe6\x94\xb4\0"
      u8"\xe6\x96\x87\0"
      u8"\xe6\x96\x97\0"
      u8"\xe6\x96\xa4\0"
      u8"\xe6\x96\xb9\0"
      u8"\xe6\x97\xa0\0"
      u8"\xe6\x97\xa5\0"
      u8"\xe6\x9b\xb0\0"
      u8"\xe6\x9c\x88\0"
      u8"\xe6\x9c\xa8\0"
      u8"\xe6\xac\xa0\0"
      u8"\xe6\xad\xa2\0"
      u8"\xe6\xad\xb9\0"
      u8"\xe6\xae\xb3\0"
      u8"\xe6\xaf\x8b\0"
      u8"\xe6\xaf\x94\0"
      u8"\xe6\xaf\x9b\0"
      u8"\xe6\xb0\x8f\0"
      u8"\xe6\xb0\x94\0"
      u8"\xe6\xb0\xb4\0"
      u8"\xe7\x81\xab\0"
      u8"\xe7\x88\xaa\0"
      u8"\xe7\x88\xb6\0"
      u8"\xe7\x88\xbb\0"
      u8"\xe7\x88\xbf\0"
      u8"\xe7\x89\x87\0"
      u8"\xe7\x89\x99\0"
      u8"\xe7\x89\x9b\0"
      u8"\xe7\x8a\xac\0"
      u8"\xe7\x8e\x84\0"
      u8"\xe7\x8e\x89\0"
      u8"\xe7\x93\x9c\0"
      u8"\xe7\x93\xa6\0"
      u8"\xe7\x94\x98\0"
      u8"\xe7\x94\x9f\0"
      u8"\xe7\x94\xa8\0"
      u8"\xe7\x94\xb0\0"
      u8"\xe7\x96\x8b\0"
      u8"\xe7\x96\x92\0"
      u8"\xe7\x99\xb6\0"
      u8"\xe7\x99\xbd\0"
      u8"\xe7\x9a\xae\0"
      u8"\xe7\x9a\xbf\0"
      u8"\xe7\x9b\xae\0"
      u8"\xe7\x9f\x9b\0"
      u8"\xe7\x9f\xa2\0"
      u8"\xe7\x9f\xb3\0"
      u8"\xe7\xa4\xba\0"
      u8"\xe7\xa6\xb8\0"
      u8"\xe7\xa6\xbe\0"
      u8"\xe7\xa9\xb4\0"
      u8"\xe7\xab\x8b\0"
      u8"\xe7\xab\xb9\0"
      u8"\xe7\xb1\xb3\0"
      u8"\xe7\xb3\xb8\0"
      u8"\xe7\xbc\xb6\0"
      u8"\xe7\xbd\x91\0"
      u8"\xe7\xbe\x8a\0"
      u8"\xe7\xbe\xbd\0"
      u8"\xe8\x80\x81\0"
      u8"\xe8\x80\x8c\0"
      u8"\xe8\x80\x92\0"
      u8"\xe8\x80\xb3\0"
      u8"\xe8\x81\xbf\0"
      u8"\xe8\x82\x89\0"
      u8"\xe8\x87\xa3\0"
      u8"\xe8\x87\xaa\0"
      u8"\xe8\x87\xb3\0"
      u8"\xe8\x87\xbc\0"
      u8"\xe8\x88\x8c\0"
      u8"\xe8\x88\x9b\0"
      u8"\xe8\x88\x9f\0"
      u8"\xe8\x89\xae\0"
      u8"\xe8\x89\xb2\0"
      u8"\xe8\x89\xb8\0"
      u8"\xe8\x99\x8d\0"
      u8"\xe8\x99\xab\0"
      u8"\xe8\xa1\x80\0"
      u8"\xe8\xa1\x8c\0"
      u8"\xe8\xa1\xa3\0"
      u8"\xe8\xa5\xbe\0"
      u8"\xe8\xa6\x8b\0"
      u8"\xe8\xa7\x92\0"
      u8"\xe8\xa8\x80\0"
      u8"\xe8\xb0\xb7\0"
      u8"\xe8\xb1\x86\0"
      u8"\xe8\xb1\x95\0"
      u8"\xe8\xb1\xb8\0"
      u8"\xe8\xb2\x9d\0"
      u8"\xe8\xb5\xa4\0"
      u8"\xe8\xb5\xb0\0"
      u8"\xe8\xb6\xb3\0"
      u8"\xe8\xba\xab\0"
      u8"\xe8\xbb\x8a\0"
      u8"\xe8\xbe\x9b\0"
      u8"\xe8\xbe\xb0\0"
      u8"\xe8\xbe\xb5\0"
      u8"\xe9\x82\x91\0"
      u8"\xe9\x85\x89\0"
      u8"\xe9\x87\x86\0"
      u8"\xe9\x87\x8c\0"
      u8"\xe9\x87\x91\0"
      u8"\xe9\x95\xb7\0"
      u8"\xe9\x96\x80\0"
      u8"\xe9\x98\x9c\0"
      u8"\xe9\x9a\xb6\0"
      u8"\xe9\x9a\xb9\0"
      u8"\xe9\x9b\xa8\0"
      u8"\xe9\x9d\x91\0"
      u8"\xe9\x9d\x9e\0"
      u8"\xe9\x9d\xa2\0"
      u8"\xe9\x9d\xa9\0"
      u8"\xe9\x9f\x8b\0"
      u8"\xe9\x9f\xad\0"
      u8"\xe9\x9f\xb3\0"
      u8"\xe9\xa0\x81\0"
      u8"\xe9\xa2\xa8\0"
      u8"\xe9\xa3\x9b\0"
      u8"\xe9\xa3\x9f\0"
      u8"\xe9\xa6\x96\0"
      u8"\xe9\xa6\x99\0"
      u8"\xe9\xa6\xac\0"
      u8"\xe9\xaa\xa8\0"
      u8"\xe9\xab\x98\0"
      u8"\xe9\xab\x9f\0"
      u8"\xe9\xac\xa5\0"
      u8"\xe9\xac\xaf\0"
      u8"\xe9\xac\xb2\0"
      u8"\xe9\xac\xbc\0"
      u8"\xe9\xad\x9a\0"
      u8"\xe9\xb3\xa5\0"
      u8"\xe9\xb9\xb5\0"
      u8"\xe9\xb9\xbf\0"
      u8"\xe9\xba\xa5\0"
      u8"\xe9\xba\xbb\0"
      u8"\xe9\xbb\x83\0"
      u8"\xe9\xbb\x8d\0"
      u8"\xe9\xbb\x91\0"
      u8"\xe9\xbb\xb9\0"
      u8"\xe9\xbb\xbd\0"
      u8"\xe9\xbc\x8e\0"
      u8"\xe9\xbc\x93\0"
      u8"\xe9\xbc\xa0\0"
      u8"\xe9\xbc\xbb\0"
      u8"\xe9\xbd\x8a\0"
      u8"\xe9\xbd\x92\0"
      u8"\xe9\xbe\x8d\0"
      u8"\xe9\xbe\x9c\0"
      u8"\xe9\xbe\xa0\0"
      u8"\56\0"
      u8"\xe3\x80\x92\0"
      u8"\xe5\x8d\x84\0"
      u8"\xe5\x8d\x85\0"
      u8"\40\xe3\x82\x99\0"
      u8"\40\xe3\x82\x9a\0"
      u8"\xe3\x82\x88\xe3\x82\x8a\0"
      u8"\xe3\x82\xb3\xe3\x83\x88\0"
      u8"\xe1\x84\x80\0"
      u8"\xe1\x84\x81\0"
      u8"\xe1\x86\xaa\0"
      u8"\xe1\x84\x82\0"
      u8"\xe1\x86\xac\0"
      u8"\xe1\x86\xad\0"
      u8"\xe1\x84\x83\0"
      u8"\xe1\x84\x84\0"
      u8"\xe1\x84\x85\0"
      u8"\xe1\x86\xb0\0"
      u8"\xe1\x86\xb1\0"
      u8"\xe1\x86\xb2\0"
      u8"\xe1\x86\xb3\0"
      u8"\xe1\x86\xb4\0"
      u8"\xe1\x86\xb5\0"
      u8"\xe1\x84\x9a\0"
      u8"\xe1\x84\x86\0"
      u8"\xe1\x84\x87\0"
      u8"\xe1\x84\x88\0"
      u8"\xe1\x84\xa1\0"
      u8"\xe1\x84\x89\0"
      u8"\xe1\x84\x8a\0"
      u8"\xe1\x84\x8b\0"
      u8"\xe1\x84\x8c\0"
      u8"\xe1\x84\x8d\0"
      u8"\xe1\x84\x8e\0"
      u8"\xe1\x84\x8f\0"
      u8"\xe1\x84\x90\0"
      u8"\xe1\x84\x91\0"
      u8"\xe1\x84\x92\0"
      u8"\xe1\x85\xa1\0"
      u8"\xe1\x85\xa2\0"
      u8"\xe1\x85\xa3\0"
      u8"\xe1\x85\xa4\0"
      u8"\xe1\x85\xa5\0"
      u8"\xe1\x85\xa6\0"
      u8"\xe1\x85\xa7\0"
      u8"\xe1\x85\xa8\0"
      u8"\xe1\x85\xa9\0"
      u8"\xe1\x85\xaa\0"
      u8"\xe1\x85\xab\0"
      u8"\xe1\x85\xac\0"
      u8"\xe1\x85\xad\0"
      u8"\xe1\x85\xae\0"
      u8"\xe1\x85\xaf\0"
      u8"\xe1\x85\xb0\0"
      u8"\xe1\x85\xb1\0"
      u8"\xe1\x85\xb2\0"
      u8"\xe1\x85\xb3\0"
      u8"\xe1\x85\xb4\0"
      u8"\xe1\x85\xb5\0"
      u8"\xe1\x84\x94\0"
      u8"\xe1\x84\x95\0"
      u8"\xe1\x87\x87\0"
      u8"\xe1\x87\x88\0"
      u8"\xe1\x87\x8c\0"
      u8"\xe1\x87\x8e\0"
      u8"\xe1\x87\x93\0"
      u8"\xe1\x87\x97\0"
      u8"\xe1\x87\x99\0"
      u8"\xe1\x84\x9c\0"
      u8"\xe1\x87\x9d\0"
      u8"\xe1\x87\x9f\0"
      u8"\xe1\x84\x9d\0"
      u8"\xe1\x84\x9e\0"
      u8"\xe1\x84\xa0\0"
      u8"\xe1\x84\xa2\0"
      u8"\xe1\x84\xa3\0"
      u8"\xe1\x84\xa7\0"
      u8"\xe1\x84\xa9\0"
      u8"\xe1\x84\xab\0"
      u8"\xe1\x84\xac\0"
      u8"\xe1\x84\xad\0"
      u8"\xe1\x84\xae\0"
      u8"\xe1\x84\xaf\0"
      u8"\xe1\x84\xb2\0"
      u8"\xe1\x84\xb6\0"
      u8"\xe1\x85\x80\0"
      u8"\xe1\x85\x87\0"
      u8"\xe1\x85\x8c\0"
      u8"\xe1\x87\xb1\0"
      u8"\xe1\x87\xb2\0"
      u8"\xe1\x85\x97\0"
      u8"\xe1\x85\x98\0"
      u8"\xe1\x85\x99\0"
      u8"\xe1\x86\x84\0"
      u8"\xe1\x86\x85\0"
      u8"\xe1\x86\x88\0"
      u8"\xe1\x86\x91\0"
      u8"\xe1\x86\x92\0"
      u8"\xe1\x86\x94\0"
      u8"\xe1\x86\x9e\0"
      u8"\xe1\x86\xa1\0"
      u8"\xe4\xb8\x89\0"
      u8"\xe5\x9b\x9b\0"
      u8"\xe4\xb8\x8a\0"
      u8"\xe4\xb8\xad\0"
      u8"\xe4\xb8\x8b\0"
      u8"\xe7\x94\xb2\0"
      u8"\xe4\xb8\x99\0"
      u8"\xe4\xb8\x81\0"
      u8"\xe5\xa4\xa9\0"
      u8"\xe5\x9c\xb0\0"
      u8"\50\xe1\x84\x80\51\0"
      u8"\50\xe1\x84\x82\51\0"
      u8"\50\xe1\x84\x83\51\0"
      u8"\50\xe1\x84\x85\51\0"
      u8"\50\xe1\x84\x86\51\0"
      u8"\50\xe1\x84\x87\51\0"
      u8"\50\xe1\x84\x89\51\0"
      u8"\50\xe1\x84\x8b\51\0"
      u8"\50\xe1\x84\x8c\51\0"
      u8"\50\xe1\x84\x8e\51\0"
      u8"\50\xe1\x84\x8f\51\0"
      u8"\50\xe1\x84\x90\51\0"
      u8"\50\xe1\x84\x91\51\0"
      u8"\50\xe1\x84\x92\51\0"
      u8"\50\xea\xb0\x80\51\0"
      u8"\50\xeb\x82\x98\51\0"
      u8"\50\xeb\x8b\xa4\51\0"
      u8"\50\xeb\x9d\xbc\51\0"
      u8"\50\xeb\xa7\x88\51\0"
      u8"\50\xeb\xb0\x94\51\0"
      u8"\50\xec\x82\xac\51\0"
      u8"\50\xec\x95\x84\51\0"
      u8"\50\xec\x9e\x90\51\0"
      u8"\50\xec\xb0\xa8\51\0"
      u8"\50\xec\xb9\xb4\51\0"
      u8"\50\xed\x83\x80\51\0"
      u8"\50\xed\x8c\x8c\51\0"
      u8"\50\xed\x95\x98\51\0"
      u8"\50\xec\xa3\xbc\51\0"
      u8"\50\xec\x98\xa4\xec\xa0\x84\51\0"
      u8"\50\xec\x98\xa4\xed\x9b\x84\51\0"
      u8"\50\xe4\xb8\x80\51\0"
      u8"\50\xe4\xba\x8c\51\0"
      u8"\50\xe4\xb8\x89\51\0"
      u8"\50\xe5\x9b\x9b\51\0"
      u8"\50\xe4\xba\x94\51\0"
      u8"\50\xe5\x85\xad\51\0"
      u8"\50\xe4\xb8\x83\51\0"
      u8"\50\xe5\x85\xab\51\0"
      u8"\50\xe4\xb9\x9d\51\0"
      u8"\50\xe5\x8d\x81\51\0"
      u8"\50\xe6\x9c\x88\51\0"
      u8"\50\xe7\x81\xab\51\0"
      u8"\50\xe6\xb0\xb4\51\0"
      u8"\50\xe6\x9c\xa8\51\0"
      u8"\50\xe9\x87\x91\51\0"
      u8"\50\xe5\x9c\x9f\51\0"
      u8"\50\xe6\x97\xa5\51\0"
      u8"\50\xe6\xa0\xaa\51\0"
      u8"\50\xe6\x9c\x89\51\0"
      u8"\50\xe7\xa4\xbe\51\0"
      u8"\50\xe5\x90\x8d\51\0"
      u8"\50\xe7\x89\xb9\51\0"
      u8"\50\xe8\xb2\xa1\51\0"
      u8"\50\xe7\xa5\x9d\51\0"
      u8"\50\xe5\x8a\xb4\51\0"
      u8"\50\xe4\xbb\xa3\51\0"
      u8"\50\xe5\x91\xbc\51\0"
      u8"\50\xe5\xad\xa6\51\0"
      u8"\50\xe7\x9b\xa3\51\0"
      u8"\50\xe4\xbc\x81\51\0"
      u8"\50\xe8\xb3\x87\51\0"
      u8"\50\xe5\x8d\x94\51\0"
      u8"\50\xe7\xa5\xad\51\0"
      u8"\50\xe4\xbc\x91\51\0"
      u8"\50\xe8\x87\xaa\51\0"
      u8"\50\xe8\x87\xb3\51\0"
      u8"\xe5\x95\x8f\0"
      u8"\xe5\xb9\xbc\0"
      u8"\xe7\xae\x8f\0"
      u8"\x70\x74\x65\0"
      u8"\62\61\0"
      u8"\62\62\0"
      u8"\62\63\0"
      u8"\62\64\0"
      u8"\62\65\0"
      u8"\62\66\0"
      u8"\62\67\0"
      u8"\62\70\0"
      u8"\62\71\0"
      u8"\63\60\0"
      u8"\63\61\0"
      u8"\63\62\0"
      u8"\63\63\0"
      u8"\63\64\0"
      u8"\63\65\0"
      u8"\xea\xb0\x80\0"
      u8"\xeb\x82\x98\0"
      u8"\xeb\x8b\xa4\0"
      u8"\xeb\x9d\xbc\0"
      u8"\xeb\xa7\x88\0"
      u8"\xeb\xb0\x94\0"
      u8"\xec\x82\xac\0"
      u8"\xec\x95\x84\0"
      u8"\xec\x9e\x90\0"
      u8"\xec\xb0\xa8\0"
      u8"\xec\xb9\xb4\0"
      u8"\xed\x83\x80\0"
      u8"\xed\x8c\x8c\0"
      u8"\xed\x95\x98\0"
      u8"\xec\xb0\xb8\xea\xb3\xa0\0"
      u8"\xec\xa3\xbc\xec\x9d\x98\0"
      u8"\xec\x9a\xb0\0"
      u8"\xe4\xba\x94\0"
      u8"\xe5\x85\xad\0"
      u8"\xe4\xb8\x83\0"
      u8"\xe4\xb9\x9d\0"
      u8"\xe6\xa0\xaa\0"
      u8"\xe6\x9c\x89\0"
      u8"\xe7\xa4\xbe\0"
      u8"\xe5\x90\x8d\0"
      u8"\xe7\x89\xb9\0"
      u8"\xe8\xb2\xa1\0"
      u8"\xe7\xa5\x9d\0"
      u8"\xe5\x8a\xb4\0"
      u8"\xe7\xa7\x98\0"
      u8"\xe7\x94\xb7\0"
      u8"\xe9\x81\xa9\0"
      u8"\xe5\x84\xaa\0"
      u8"\xe5\x8d\xb0\0"
      u8"\xe6\xb3\xa8\0"
      u8"\xe9\xa0\x85\0"
      u8"\xe4\xbc\x91\0"
      u8"\xe5\x86\x99\0"
      u8"\xe6\xad\xa3\0"
      u8"\xe5\xb7\xa6\0"
      u8"\xe5\x8f\xb3\0"
      u8"\xe5\x8c\xbb\0"
      u8"\xe5\xae\x97\0"
      u8"\xe5\xad\xa6\0"
      u8"\xe7\x9b\xa3\0"
      u8"\xe4\xbc\x81\0"
      u8"\xe8\xb3\x87\0"
      u8"\xe5\x8d\x94\0"
      u8"\xe5\xa4\x9c\0"
      u8"\63\66\0"
      u8"\63\67\0"
      u8"\63\70\0"
      u8"\63\71\0"
      u8"\64\60\0"
      u8"\64\61\0"
      u8"\64\62\0"
      u8"\64\63\0"
      u8"\64\64\0"
      u8"\64\65\0"
      u8"\64\66\0"
      u8"\64\67\0"
      u8"\64\70\0"
      u8"\64\71\0"
      u8"\65\60\0"
      u8"\61\xe6\x9c\x88\0"
      u8"\62\xe6\x9c\x88\0"
      u8"\63\xe6\x9c\x88\0"
      u8"\64\xe6\x9c\x88\0"
      u8"\65\xe6\x9c\x88\0"
      u8"\66\xe6\x9c\x88\0"
      u8"\67\xe6\x9c\x88\0"
      u8"\70\xe6\x9c\x88\0"
      u8"\71\xe6\x9c\x88\0"
      u8"\61\60\xe6\x9c\x88\0"
      u8"\61\61\xe6\x9c\x88\0"
      u8"\61\62\xe6\x9c\x88\0"
      u8"\x68\x67\0"
      u8"\x65\x72\x67\0"
      u8"\x65\x76\0"
      u8"\x6c\x74\x64\0"
      u8"\xe3\x82\xa2\0"
      u8"\xe3\x82\xa4\0"
      u8"\xe3\x82\xa6\0"
      u8"\xe3\x82\xa8\0"
      u8"\xe3\x82\xaa\0"
      u8"\xe3\x82\xab\0"
      u8"\xe3\x82\xad\0"
      u8"\xe3\x82\xaf\0"
      u8"\xe3\x82\xb1\0"
      u8"\xe3\x82\xb3\0"
      u8"\xe3\x82\xb5\0"
      u8"\xe3\x82\xb7\0"
      u8"\xe3\x82\xb9\0"
      u8"\xe3\x82\xbb\0"
      u8"\xe3\x82\xbd\0"
      u8"\xe3\x82\xbf\0"
      u8"\xe3\x83\x81\0"
      u8"\xe3\x83\x84\0"
      u8"\xe3\x83\x86\0"
      u8"\xe3\x83\x88\0"
      u8"\xe3\x83\x8a\0"
      u8"\xe3\x83\x8b\0"
      u8"\xe3\x83\x8c\0"
      u8"\xe3\x83\x8d\0"
      u8"\xe3\x83\x8e\0"
      u8"\xe3\x83\x8f\0"
      u8"\xe3\x83\x92\0"
      u8"\xe3\x83\x95\0"
      u8"\xe3\x83\x98\0"
      u8"\xe3\x83\x9b\0"
      u8"\xe3\x83\x9e\0"
      u8"\xe3\x83\x9f\0"
      u8"\xe3\x83\xa0\0"
      u8"\xe3\x83\xa1\0"
      u8"\xe3\x83\xa2\0"
      u8"\xe3\x83\xa4\0"
      u8"\xe3\x83\xa6\0"
      u8"\xe3\x83\xa8\0"
      u8"\xe3\x83\xa9\0"
      u8"\xe3\x83\xaa\0"
      u8"\xe3\x83\xab\0"
      u8"\xe3\x83\xac\0"
      u8"\xe3\x83\xad\0"
      u8"\xe3\x83\xaf\0"
      u8"\xe3\x83\xb0\0"
      u8"\xe3\x83\xb1\0"
      u8"\xe3\x83\xb2\0"
      u8"\xe4\xbb\xa4\xe5\x92\x8c\0"
      u8"\xe3\x82\xa2\xe3\x83\x91\xe3\x83\xbc\xe3\x83\x88\0"
      u8"\xe3\x82\xa2\xe3\x83\xab\xe3\x83\x95\xe3\x82\xa1\0"
      u8"\xe3\x82\xa2\xe3\x83\xb3\xe3\x83\x9a\xe3\x82\xa2\0"
      u8"\xe3\x82\xa2\xe3\x83\xbc\xe3\x83\xab\0"
      u8"\xe3\x82\xa4\xe3\x83\x8b\xe3\x83\xb3\xe3\x82\xb0\0"
      u8"\xe3\x82\xa4\xe3\x83\xb3\xe3\x83\x81\0"
      u8"\xe3\x82\xa6\xe3\x82\xa9\xe3\x83\xb3\0"
      u8"\xe3\x82\xa8\xe3\x82\xb9\xe3\x82\xaf\xe3\x83\xbc\xe3\x83\x89\0"
      u8"\xe3\x82\xa8\xe3\x83\xbc\xe3\x82\xab\xe3\x83\xbc\0"
      u8"\xe3\x82\xaa\xe3\x83\xb3\xe3\x82\xb9\0"
      u8"\xe3\x82\xaa\xe3\x83\xbc\xe3\x83\xa0\0"
      u8"\xe3\x82\xab\xe3\x82\xa4\xe3\x83\xaa\0"
      u8"\xe3\x82\xab\xe3\x83\xa9\xe3\x83\x83\xe3\x83\x88\0"
      u8"\xe3\x82\xab\xe3\x83\xad\xe3\x83\xaa\xe3\x83\xbc\0"
      u8"\xe3\x82\xac\xe3\x83\xad\xe3\x83\xb3\0"
      u8"\xe3\x82\xac\xe3\x83\xb3\xe3\x83\x9e\0"
      u8"\xe3\x82\xae\xe3\x82\xac\0"
      u8"\xe3\x82\xae\xe3\x83\x8b\xe3\x83\xbc\0"
      u8"\xe3\x82\xad\xe3\x83\xa5\xe3\x83\xaa\xe3\x83\xbc\0"
      u8"\xe3\x82\xae\xe3\x83\xab\xe3\x83\x80\xe3\x83\xbc\0"
      u8"\xe3\x82\xad\xe3\x83\xad\0"
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\0"
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x83\xa1\xe3\x83\xbc\xe3\x83\x88\xe3\x83\xab\0"
      u8"\xe3\x82\xad\xe3\x83\xad\xe3\x83\xaf\xe3\x83\x83\xe3\x83\x88\0"
      u8"\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\0"
      u8"\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\xe3\x83\x88\xe3\x83\xb3\0"
      u8"\xe3\x82\xaf\xe3\x83\xab\xe3\x82\xbc\xe3\x82\xa4\xe3\x83\xad\0"
      u8"\xe3\x82\xaf\xe3\x83\xad\xe3\x83\xbc\xe3\x83\x8d\0"
      u8"\xe3\x82\xb1\xe3\x83\xbc\xe3\x82\xb9\0"
      u8"\xe3\x82\xb3\xe3\x83\xab\xe3\x83\x8a\0"
      u8"\xe3\x82\xb3\xe3\x83\xbc\xe3\x83\x9d\0"
      u8"\xe3\x82\xb5\xe3\x82\xa4\xe3\x82\xaf\xe3\x83\xab\0"
      u8"\xe3\x82\xb5\xe3\x83\xb3\xe3\x83\x81\xe3\x83\xbc\xe3\x83\xa0\0"
      u8"\xe3\x82\xb7\xe3\x83\xaa\xe3\x83\xb3\xe3\x82\xb0\0"
      u8"\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x81\0"
      u8"\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x88\0"
      u8"\xe3\x83\x80\xe3\x83\xbc\xe3\x82\xb9\0"
      u8"\xe3\x83\x87\xe3\x82\xb7\0"
      u8"\xe3\x83\x89\xe3\x83\xab\0"
      u8"\xe3\x83\x88\xe3\x83\xb3\0"
      u8"\xe3\x83\x8a\xe3\x83\x8e\0"
      u8"\xe3\x83\x8e\xe3\x83\x83\xe3\x83\x88\0"
      u8"\xe3\x83\x8f\xe3\x82\xa4\xe3\x83\x84\0"
      u8"\xe3\x83\x91\xe3\x83\xbc\xe3\x82\xbb\xe3\x83\xb3\xe3\x83\x88\0"
      u8"\xe3\x83\x91\xe3\x83\xbc\xe3\x83\x84\0"
      u8"\xe3\x83\x90\xe3\x83\xbc\xe3\x83\xac\xe3\x83\xab\0"
      u8"\xe3\x83\x94\xe3\x82\xa2\xe3\x82\xb9\xe3\x83\x88\xe3\x83\xab\0"
      u8"\xe3\x83\x94\xe3\x82\xaf\xe3\x83\xab\0"
      u8"\xe3\x83\x94\xe3\x82\xb3\0"
      u8"\xe3\x83\x93\xe3\x83\xab\0"
      u8"\xe3\x83\x95\xe3\x82\xa1\xe3\x83\xa9\xe3\x83\x83\xe3\x83\x89\0"
      u8"\xe3\x83\x95\xe3\x82\xa3\xe3\x83\xbc\xe3\x83\x88\0"
      u8"\xe3\x83\x96\xe3\x83\x83\xe3\x82\xb7\xe3\x82\xa7\xe3\x83\xab\0"
      u8"\xe3\x83\x95\xe3\x83\xa9\xe3\x83\xb3\0"
      u8"\xe3\x83\x98\xe3\x82\xaf\xe3\x82\xbf\xe3\x83\xbc\xe3\x83\xab\0"
      u8"\xe3\x83\x9a\xe3\x82\xbd\0"
      u8"\xe3\x83\x9a\xe3\x83\x8b\xe3\x83\x92\0"
      u8"\xe3\x83\x98\xe3\x83\xab\xe3\x83\x84\0"
      u8"\xe3\x83\x9a\xe3\x83\xb3\xe3\x82\xb9\0"
      u8"\xe3\x83\x9a\xe3\x83\xbc\xe3\x82\xb8\0"
      u8"\xe3\x83\x99\xe3\x83\xbc\xe3\x82\xbf\0"
      u8"\xe3\x83\x9d\xe3\x82\xa4\xe3\x83\xb3\xe3\x83\x88\0"
      u8"\xe3\x83\x9c\xe3\x83\xab\xe3\x83\x88\0"
      u8"\xe3\x83\x9b\xe3\x83\xb3\0"
      u8"\xe3\x83\x9d\xe3\x83\xb3\xe3\x83\x89\0"
      u8"\xe3\x83\x9b\xe3\x83\xbc\xe3\x83\xab\0"
      u8"\xe3\x83\x9b\xe3\x83\xbc\xe3\x83\xb3\0"
      u8"\xe3\x83\x9e\xe3\x82\xa4\xe3\x82\xaf\xe3\x83\xad\0"
      u8"\xe3\x83\x9e\xe3\x82\xa4\xe3\x83\xab\0"
      u8"\xe3\x83\x9e\xe3\x83\x83\xe3\x83\x8f\0"
      u8"\xe3\x83\x9e\xe3\x83\xab\xe3\x82\xaf\0"
      u8"\xe3\x83\x9e\xe3\x83\xb3\xe3\x82\xb7\xe3\x83\xa7\xe3\x83\xb3\0"
      u8"\xe3\x83\x9f\xe3\x82\xaf\xe3\x83\xad\xe3\x83\xb3\0"
      u8"\xe3\x83\x9f\xe3\x83\xaa\0"
      u8"\xe3\x83\x9f\xe3\x83\xaa\xe3\x83\x90\xe3\x83\xbc\xe3\x83\xab\0"
      u8"\xe3\x83\xa1\xe3\x82\xac\0"
      u8"\xe3\x83\xa1\xe3\x82\xac\xe3\x83\x88\xe3\x83\xb3\0"
      u8"\xe3\x83\xa1\xe3\x83\xbc\xe3\x83\x88\xe3\x83\xab\0"
      u8"\xe3\x83\xa4\xe3\x83\xbc\xe3\x83\x89\0"
      u8"\xe3\x83\xa4\xe3\x83\xbc\xe3\x83\xab\0"
      u8"\xe3\x83\xa6\xe3\x82\xa2\xe3\x83\xb3\0"
      u8"\xe3\x83\xaa\xe3\x83\x83\xe3\x83\x88\xe3\x83\xab\0"
      u8"\xe3\x83\xaa\xe3\x83\xa9\0"
      u8"\xe3\x83\xab\xe3\x83\x94\xe3\x83\xbc\0"
      u8"\xe3\x83\xab\xe3\x83\xbc\xe3\x83\x96\xe3\x83\xab\0"
      u8"\xe3\x83\xac\xe3\x83\xa0\0"
      u8"\xe3\x83\xac\xe3\x83\xb3\xe3\x83\x88\xe3\x82\xb2\xe3\x83\xb3\0"
      u8"\xe3\x83\xaf\xe3\x83\x83\xe3\x83\x88\0"
      u8"\60\xe7\x82\xb9\0"
      u8"\61\xe7\x82\xb9\0"
      u8"\62\xe7\x82\xb9\0"
      u8"\63\xe7\x82\xb9\0"
      u8"\64\xe7\x82\xb9\0"
      u8"\65\xe7\x82\xb9\0"
      u8"\66\xe7\x82\xb9\0"
      u8"\67\xe7\x82\xb9\0"
      u8"\70\xe7\x82\xb9\0"
      u8"\71\xe7\x82\xb9\0"
      u8"\61\60\xe7\x82\xb9\0"
      u8"\61\61\xe7\x82\xb9\0"
      u8"\61\62\xe7\x82\xb9\0"
      u8"\61\63\xe7\x82\xb9\0"
      u8"\61\64\xe7\x82\xb9\0"
      u8"\61\65\xe7\x82\xb9\0"
      u8"\61\66\xe7\x82\xb9\0"
      u8"\61\67\xe7\x82\xb9\0"
      u8"\61\70\xe7\x82\xb9\0"
      u8"\61\71\xe7\x82\xb9\0"
      u8"\62\60\xe7\x82\xb9\0"
      u8"\62\61\xe7\x82\xb9\0"
      u8"\62\62\xe7\x82\xb9\0"
      u8"\62\63\xe7\x82\xb9\0"
      u8"\62\64\xe7\x82\xb9\0"
      u8"\x68\x70\x61\0"
      u8"\x64\x61\0"
      u8"\x61\x75\0"
      u8"\x62\x61\x72\0"
      u8"\x6f\x76\0"
      u8"\x70\x63\0"
      u8"\x64\x6d\0"
      u8"\x64\x6d\62\0"
      u8"\x64\x6d\63\0"
      u8"\x69\x75\0"
      u8"\xe5\xb9\xb3\xe6\x88\x90\0"
      u8"\xe6\x98\xad\xe5\x92\x8c\0"
      u8"\xe5\xa4\xa7\xe6\xad\xa3\0"
      u8"\xe6\x98\x8e\xe6\xb2\xbb\0"
      u8"\xe6\xa0\xaa\xe5\xbc\x8f\xe4\xbc\x9a\xe7\xa4\xbe\0"
      u8"\x70\x61\0"
      u8"\x6e\x61\0"
      u8"\xce\xbc\x61\0"
      u8"\x6d\x61\0"
      u8"\x6b\x61\0"
      u8"\x6b\x62\0"
      u8"\x6d\x62\0"
      u8"\x67\x62\0"
      u8"\x63\x61\x6c\0"
      u8"\x6b\x63\x61\x6c\0"
      u8"\x70\x66\0"
      u8"\x6e\x66\0"
      u8"\xce\xbc\x66\0"
      u8"\xce\xbc\x67\0"
      u8"\x6d\x67\0"
      u8"\x6b\x67\0"
      u8"\x68\x7a\0"
      u8"\x6b\x68\x7a\0"
      u8"\x6d\x68\x7a\0"
      u8"\x67\x68\x7a\0"
      u8"\x74\x68\x7a\0"
      u8"\xce\xbc\x6c\0"
      u8"\x6d\x6c\0"
      u8"\x64\x6c\0"
      u8"\x6b\x6c\0"
      u8"\x66\x6d\0"
      u8"\x6e\x6d\0"
      u8"\xce\xbc\x6d\0"
      u8"\x6d\x6d\0"
      u8"\x63\x6d\0"
      u8"\x6b\x6d\0"
      u8"\x6d\x6d\62\0"
      u8"\x63\x6d\62\0"
      u8"\x6d\62\0"
      u8"\x6b\x6d\62\0"
      u8"\x6d\x6d\63\0"
      u8"\x63\x6d\63\0"
      u8"\x6d\63\0"
      u8"\x6b\x6d\63\0"
      u8"\x6d\xe2\x88\x95\x73\0"
      u8"\x6d\xe2\x88\x95\x73\62\0"
      u8"\x6b\x70\x61\0"
      u8"\x6d\x70\x61\0"
      u8"\x67\x70\x61\0"
      u8"\x72\x61\x64\0"
      u8"\x72\x61\x64\xe2\x88\x95\x73\0"
      u8"\x72\x61\x64\xe2\x88\x95\x73\62\0"
      u8"\x70\x73\0"
      u8"\x6e\x73\0"
      u8"\xce\xbc\x73\0"
      u8"\x6d\x73\0"
      u8"\x70\x76\0"
      u8"\x6e\x76\0"
      u8"\xce\xbc\x76\0"
      u8"\x6d\x76\0"
      u8"\x6b\x76\0"
      u8"\x70\x77\0"
      u8"\x6e\x77\0"
      u8"\xce\xbc\x77\0"
      u8"\x6d\x77\0"
      u8"\x6b\x77\0"
      u8"\x6b\xcf\x89\0"
      u8"\x6d\xcf\x89\0"
      u8"\x62\x71\0"
      u8"\x63\x63\0"
      u8"\x63\x64\0"
      u8"\x63\xe2\x88\x95\x6b\x67\0"
      u8"\x64\x62\0"
      u8"\x67\x79\0"
      u8"\x68\x61\0"
      u8"\x68\x70\0"
      u8"\x69\x6e\0"
      u8"\x6b\x6b\0"
      u8"\x6b\x74\0"
      u8"\x6c\x6d\0"
      u8"\x6c\x6e\0"
      u8"\x6c\x6f\x67\0"
      u8"\x6c\x78\0"
      u8"\x6d\x69\x6c\0"
      u8"\x6d\x6f\x6c\0"
      u8"\x70\x68\0"
      u8"\x70\x70\x6d\0"
      u8"\x70\x72\0"
      u8"\x73\x72\0"
      u8"\x73\x76\0"
      u8"\x77\x62\0"
      u8"\x76\xe2\x88\x95\x6d\0"
      u8"\x61\xe2\x88\x95\x6d\0"
      u8"\61\xe6\x97\xa5\0"
      u8"\62\xe6\x97\xa5\0"
      u8"\63\xe6\x97\xa5\0"
      u8"\64\xe6\x97\xa5\0"
      u8"\65\xe6\x97\xa5\0"
      u8"\66\xe6\x97\xa5\0"
      u8"\67\xe6\x97\xa5\0"
      u8"\70\xe6\x97\xa5\0"
      u8"\71\xe6\x97\xa5\0"
      u8"\61\60\xe6\x97\xa5\0"
      u8"\61\61\xe6\x97\xa5\0"
      u8"\61\62\xe6\x97\xa5\0"
      u8"\61\63\xe6\x97\xa5\0"
      u8"\61\64\xe6\x97\xa5\0"
      u8"\61\65\xe6\x97\xa5\0"
      u8"\61\66\xe6\x97\xa5\0"
      u8"\61\67\xe6\x97\xa5\0"
      u8"\61\70\xe6\x97\xa5\0"
      u8"\61\71\xe6\x97\xa5\0"
      u8"\62\60\xe6\x97\xa5\0"
      u8"\62\61\xe6\x97\xa5\0"
      u8"\62\62\xe6\x97\xa5\0"
      u8"\62\63\xe6\x97\xa5\0"
      u8"\62\64\xe6\x97\xa5\0"
      u8"\62\65\xe6\x97\xa5\0"
      u8"\62\66\xe6\x97\xa5\0"
      u8"\62\67\xe6\x97\xa5\0"
      u8"\62\70\xe6\x97\xa5\0"
      u8"\62\71\xe6\x97\xa5\0"
      u8"\63\60\xe6\x97\xa5\0"
      u8"\63\61\xe6\x97\xa5\0"
      u8"\x67\x61\x6c\0"
      u8"\xea\x99\x81\0"
      u8"\xea\x99\x83\0"
      u8"\xea\x99\x85\0"
      u8"\xea\x99\x87\0"
      u8"\xea\x99\x89\0"
      u8"\xea\x99\x8d\0"
      u8"\xea\x99\x8f\0"
      u8"\xea\x99\x91\0"
      u8"\xea\x99\x93\0"
      u8"\xea\x99\x95\0"
      u8"\xea\x99\x97\0"
      u8"\xea\x99\x99\0"
      u8"\xea\x99\x9b\0"
      u8"\xea\x99\x9d\0"
      u8"\xea\x99\x9f\0"
      u8"\xea\x99\xa1\0"
      u8"\xea\x99\xa3\0"
      u8"\xea\x99\xa5\0"
      u8"\xea\x99\xa7\0"
      u8"\xea\x99\xa9\0"
      u8"\xea\x99\xab\0"
      u8"\xea\x99\xad\0"
      u8"\xea\x9a\x81\0"
      u8"\xea\x9a\x83\0"
      u8"\xea\x9a\x85\0"
      u8"\xea\x9a\x87\0"
      u8"\xea\x9a\x89\0"
      u8"\xea\x9a\x8b\0"
      u8"\xea\x9a\x8d\0"
      u8"\xea\x9a\x8f\0"
      u8"\xea\x9a\x91\0"
      u8"\xea\x9a\x93\0"
      u8"\xea\x9a\x95\0"
      u8"\xea\x9a\x97\0"
      u8"\xea\x9a\x99\0"
      u8"\xea\x9a\x9b\0"
      u8"\xea\x9c\xa3\0"
      u8"\xea\x9c\xa5\0"
      u8"\xea\x9c\xa7\0"
      u8"\xea\x9c\xa9\0"
      u8"\xea\x9c\xab\0"
      u8"\xea\x9c\xad\0"
      u8"\xea\x9c\xaf\0"
      u8"\xea\x9c\xb3\0"
      u8"\xea\x9c\xb5\0"
      u8"\xea\x9c\xb7\0"
      u8"\xea\x9c\xb9\0"
      u8"\xea\x9c\xbb\0"
      u8"\xea\x9c\xbd\0"
      u8"\xea\x9c\xbf\0"
      u8"\xea\x9d\x81\0"
      u8"\xea\x9d\x83\0"
      u8"\xea\x9d\x85\0"
      u8"\xea\x9d\x87\0"
      u8"\xea\x9d\x89\0"
      u8"\xea\x9d\x8b\0"
      u8"\xea\x9d\x8d\0"
      u8"\xea\x9d\x8f\0"
      u8"\xea\x9d\x91\0"
      u8"\xea\x9d\x93\0"
      u8"\xea\x9d\x95\0"
      u8"\xea\x9d\x97\0"
      u8"\xea\x9d\x99\0"
      u8"\xea\x9d\x9b\0"
      u8"\xea\x9d\x9d\0"
      u8"\xea\x9d\x9f\0"
      u8"\xea\x9d\xa1\0"
      u8"\xea\x9d\xa3\0"
      u8"\xea\x9d\xa5\0"
      u8"\xea\x9d\xa7\0"
      u8"\xea\x9d\xa9\0"
      u8"\xea\x9d\xab\0"
      u8"\xea\x9d\xad\0"
      u8"\xea\x9d\xaf\0"
      u8"\xea\x9d\xba\0"
      u8"\xea\x9d\xbc\0"
      u8"\xe1\xb5\xb9\0"
      u8"\xea\x9d\xbf\0"
      u8"\xea\x9e\x81\0"
      u8"\xea\x9e\x83\0"
      u8"\xea\x9e\x85\0"
      u8"\xea\x9e\x87\0"
      u8"\xea\x9e\x8c\0"
      u8"\xea\x9e\x91\0"
      u8"\xea\x9e\x93\0"
      u8"\xea\x9e\x97\0"
      u8"\xea\x9e\x99\0"
      u8"\xea\x9e\x9b\0"
      u8"\xea\x9e\x9d\0"
      u8"\xea\x9e\x9f\0"
      u8"\xea\x9e\xa1\0"
      u8"\xea\x9e\xa3\0"
      u8"\xea\x9e\xa5\0"
      u8"\xea\x9e\xa7\0"
      u8"\xea\x9e\xa9\0"
      u8"\xc9\xac\0"
      u8"\xca\x9e\0"
      u8"\xca\x87\0"
      u8"\xea\xad\x93\0"
      u8"\xea\x9e\xb5\0"
      u8"\xea\x9e\xb7\0"
      u8"\xea\x9e\xb9\0"
      u8"\xea\x9e\xbb\0"
      u8"\xea\x9e\xbd\0"
      u8"\xea\x9e\xbf\0"
      u8"\xea\x9f\x81\0"
      u8"\xea\x9f\x83\0"
      u8"\xea\x9e\x94\0"
      u8"\xe1\xb6\x8e\0"
      u8"\xea\x9f\x88\0"
      u8"\xea\x9f\x8a\0"
      u8"\xc9\xa4\0"
      u8"\xea\x9f\x8d\0"
      u8"\xea\x9f\x91\0"
      u8"\xea\x9f\x97\0"
      u8"\xea\x9f\x99\0"
      u8"\xea\x9f\x9b\0"
      u8"\xc6\x9b\0"
      u8"\xea\x9f\xb6\0"
      u8"\xea\xac\xb7\0"
      u8"\xea\xad\x92\0"
      u8"\xca\x8d\0"
      u8"\xe1\x8e\xa0\0"
      u8"\xe1\x8e\xa1\0"
      u8"\xe1\x8e\xa2\0"
      u8"\xe1\x8e\xa3\0"
      u8"\xe1\x8e\xa4\0"
      u8"\xe1\x8e\xa5\0"
      u8"\xe1\x8e\xa6\0"
      u8"\xe1\x8e\xa7\0"
      u8"\xe1\x8e\xa8\0"
      u8"\xe1\x8e\xa9\0"
      u8"\xe1\x8e\xaa\0"
      u8"\xe1\x8e\xab\0"
      u8"\xe1\x8e\xac\0"
      u8"\xe1\x8e\xad\0"
      u8"\xe1\x8e\xae\0"
      u8"\xe1\x8e\xaf\0"
      u8"\xe1\x8e\xb0\0"
      u8"\xe1\x8e\xb1\0"
      u8"\xe1\x8e\xb2\0"
      u8"\xe1\x8e\xb3\0"
      u8"\xe1\x8e\xb4\0"
      u8"\xe1\x8e\xb5\0"
      u8"\xe1\x8e\xb6\0"
      u8"\xe1\x8e\xb7\0"
      u8"\xe1\x8e\xb8\0"
      u8"\xe1\x8e\xb9\0"
      u8"\xe1\x8e\xba\0"
      u8"\xe1\x8e\xbb\0"
      u8"\xe1\x8e\xbc\0"
      u8"\xe1\x8e\xbd\0"
      u8"\xe1\x8e\xbe\0"
      u8"\xe1\x8e\xbf\0"
      u8"\xe1\x8f\x80\0"
      u8"\xe1\x8f\x81\0"
      u8"\xe1\x8f\x82\0"
      u8"\xe1\x8f\x83\0"
      u8"\xe1\x8f\x84\0"
      u8"\xe1\x8f\x85\0"
      u8"\xe1\x8f\x86\0"
      u8"\xe1\x8f\x87\0"
      u8"\xe1\x8f\x88\0"
      u8"\xe1\x8f\x89\0"
      u8"\xe1\x8f\x8a\0"
      u8"\xe1\x8f\x8b\0"
      u8"\xe1\x8f\x8c\0"
      u8"\xe1\x8f\x8d\0"
      u8"\xe1\x8f\x8e\0"
      u8"\xe1\x8f\x8f\0"
      u8"\xe1\x8f\x90\0"
      u8"\xe1\x8f\x91\0"
      u8"\xe1\x8f\x92\0"
      u8"\xe1\x8f\x93\0"
      u8"\xe1\x8f\x94\0"
      u8"\xe1\x8f\x95\0"
      u8"\xe1\x8f\x96\0"
      u8"\xe1\x8f\x97\0"
      u8"\xe1\x8f\x98\0"
      u8"\xe1\x8f\x99\0"
      u8"\xe1\x8f\x9a\0"
      u8"\xe1\x8f\x9b\0"
      u8"\xe1\x8f\x9c\0"
      u8"\xe1\x8f\x9d\0"
      u8"\xe1\x8f\x9e\0"
      u8"\xe1\x8f\x9f\0"
      u8"\xe1\x8f\xa0\0"
      u8"\xe1\x8f\xa1\0"
      u8"\xe1\x8f\xa2\0"
      u8"\xe1\x8f\xa3\0"
      u8"\xe1\x8f\xa4\0"
      u8"\xe1\x8f\xa5\0"
      u8"\xe1\x8f\xa6\0"
      u8"\xe1\x8f\xa7\0"
      u8"\xe1\x8f\xa8\0"
      u8"\xe1\x8f\xa9\0"
      u8"\xe1\x8f\xaa\0"
      u8"\xe1\x8f\xab\0"
      u8"\xe1\x8f\xac\0"
      u8"\xe1\x8f\xad\0"
      u8"\xe1\x8f\xae\0"
      u8"\xe1\x8f\xaf\0"
      u8"\xe8\xb1\x88\0"
      u8"\xe6\x9b\xb4\0"
      u8"\xe8\xb3\x88\0"
      u8"\xe6\xbb\x91\0"
      u8"\xe4\xb8\xb2\0"
      u8"\xe5\x8f\xa5\0"
      u8"\xe5\xa5\x91\0"
      u8"\xe5\x96\x87\0"
      u8"\xe5\xa5\x88\0"
      u8"\xe6\x87\xb6\0"
      u8"\xe7\x99\xa9\0"
      u8"\xe7\xbe\x85\0"
      u8"\xe8\x98\xbf\0"
      u8"\xe8\x9e\xba\0"
      u8"\xe8\xa3\xb8\0"
      u8"\xe9\x82\x8f\0"
      u8"\xe6\xa8\x82\0"
      u8"\xe6\xb4\x9b\0"
      u8"\xe7\x83\x99\0"
      u8"\xe7\x8f\x9e\0"
      u8"\xe8\x90\xbd\0"
      u8"\xe9\x85\xaa\0"
      u8"\xe9\xa7\xb1\0"
      u8"\xe4\xba\x82\0"
      u8"\xe5\x8d\xb5\0"
      u8"\xe6\xac\x84\0"
      u8"\xe7\x88\x9b\0"
      u8"\xe8\x98\xad\0"
      u8"\xe9\xb8\x9e\0"
      u8"\xe5\xb5\x90\0"
      u8"\xe6\xbf\xab\0"
      u8"\xe8\x97\x8d\0"
      u8"\xe8\xa5\xa4\0"
      u8"\xe6\x8b\x89\0"
      u8"\xe8\x87\x98\0"
      u8"\xe8\xa0\x9f\0"
      u8"\xe5\xbb\x8a\0"
      u8"\xe6\x9c\x97\0"
      u8"\xe6\xb5\xaa\0"
      u8"\xe7\x8b\xbc\0"
      u8"\xe9\x83\x8e\0"
      u8"\xe4\xbe\x86\0"
      u8"\xe5\x86\xb7\0"
      u8"\xe5\x8b\x9e\0"
      u8"\xe6\x93\x84\0"
      u8"\xe6\xab\x93\0"
      u8"\xe7\x88\x90\0"
      u8"\xe7\x9b\xa7\0"
      u8"\xe8\x98\x86\0"
      u8"\xe8\x99\x9c\0"
      u8"\xe8\xb7\xaf\0"
      u8"\xe9\x9c\xb2\0"
      u8"\xe9\xad\xaf\0"
      u8"\xe9\xb7\xba\0"
      u8"\xe7\xa2\x8c\0"
      u8"\xe7\xa5\xbf\0"
      u8"\xe7\xb6\xa0\0"
      u8"\xe8\x8f\x89\0"
      u8"\xe9\x8c\x84\0"
      u8"\xe8\xab\x96\0"
      u8"\xe5\xa3\x9f\0"
      u8"\xe5\xbc\x84\0"
      u8"\xe7\xb1\xa0\0"
      u8"\xe8\x81\xbe\0"
      u8"\xe7\x89\xa2\0"
      u8"\xe7\xa3\x8a\0"
      u8"\xe8\xb3\x82\0"
      u8"\xe9\x9b\xb7\0"
      u8"\xe5\xa3\x98\0"
      u8"\xe5\xb1\xa2\0"
      u8"\xe6\xa8\x93\0"
      u8"\xe6\xb7\x9a\0"
      u8"\xe6\xbc\x8f\0"
      u8"\xe7\xb4\xaf\0"
      u8"\xe7\xb8\xb7\0"
      u8"\xe9\x99\x8b\0"
      u8"\xe5\x8b\x92\0"
      u8"\xe8\x82\x8b\0"
      u8"\xe5\x87\x9c\0"
      u8"\xe5\x87\x8c\0"
      u8"\xe7\xa8\x9c\0"
      u8"\xe7\xb6\xbe\0"
      u8"\xe8\x8f\xb1\0"
      u8"\xe9\x99\xb5\0"
      u8"\xe8\xae\x80\0"
      u8"\xe6\x8b\x8f\0"
      u8"\xe8\xab\xbe\0"
      u8"\xe4\xb8\xb9\0"
      u8"\xe5\xaf\xa7\0"
      u8"\xe6\x80\x92\0"
      u8"\xe7\x8e\x87\0"
      u8"\xe7\x95\xb0\0"
      u8"\xe5\x8c\x97\0"
      u8"\xe7\xa3\xbb\0"
      u8"\xe4\xbe\xbf\0"
      u8"\xe5\xbe\xa9\0"
      u8"\xe4\xb8\x8d\0"
      u8"\xe6\xb3\x8c\0"
      u8"\xe6\x95\xb8\0"
      u8"\xe7\xb4\xa2\0"
      u8"\xe5\x8f\x83\0"
      u8"\xe5\xa1\x9e\0"
      u8"\xe7\x9c\x81\0"
      u8"\xe8\x91\x89\0"
      u8"\xe8\xaa\xaa\0"
      u8"\xe6\xae\xba\0"
      u8"\xe6\xb2\x88\0"
      u8"\xe6\x8b\xbe\0"
      u8"\xe8\x8b\xa5\0"
      u8"\xe6\x8e\xa0\0"
      u8"\xe7\x95\xa5\0"
      u8"\xe4\xba\xae\0"
      u8"\xe5\x85\xa9\0"
      u8"\xe5\x87\x89\0"
      u8"\xe6\xa2\x81\0"
      u8"\xe7\xb3\xa7\0"
      u8"\xe8\x89\xaf\0"
      u8"\xe8\xab\x92\0"
      u8"\xe9\x87\x8f\0"
      u8"\xe5\x8b\xb5\0"
      u8"\xe5\x91\x82\0"
      u8"\xe5\xbb\xac\0"
      u8"\xe6\x97\x85\0"
      u8"\xe6\xbf\xbe\0"
      u8"\xe7\xa4\xaa\0"
      u8"\xe9\x96\xad\0"
      u8"\xe9\xa9\xaa\0"
      u8"\xe9\xba\x97\0"
      u8"\xe9\xbb\x8e\0"
      u8"\xe6\x9b\x86\0"
      u8"\xe6\xad\xb7\0"
      u8"\xe8\xbd\xa2\0"
      u8"\xe5\xb9\xb4\0"
      u8"\xe6\x86\x90\0"
      u8"\xe6\x88\x80\0"
      u8"\xe6\x92\x9a\0"
      u8"\xe6\xbc\xa3\0"
      u8"\xe7\x85\x89\0"
      u8"\xe7\x92\x89\0"
      u8"\xe7\xa7\x8a\0"
      u8"\xe7\xb7\xb4\0"
      u8"\xe8\x81\xaf\0"
      u8"\xe8\xbc\xa6\0"
      u8"\xe8\x93\xae\0"
      u8"\xe9\x80\xa3\0"
      u8"\xe9\x8d\x8a\0"
      u8"\xe5\x88\x97\0"
      u8"\xe5\x8a\xa3\0"
      u8"\xe5\x92\xbd\0"
      u8"\xe7\x83\x88\0"
      u8"\xe8\xa3\x82\0"
      u8"\xe5\xbb\x89\0"
      u8"\xe5\xbf\xb5\0"
      u8"\xe6\x8d\xbb\0"
      u8"\xe6\xae\xae\0"
      u8"\xe7\xb0\xbe\0"
      u8"\xe7\x8d\xb5\0"
      u8"\xe4\xbb\xa4\0"
      u8"\xe5\x9b\xb9\0"
      u8"\xe5\xb6\xba\0"
      u8"\xe6\x80\x9c\0"
      u8"\xe7\x8e\xb2\0"
      u8"\xe7\x91\xa9\0"
      u8"\xe7\xbe\x9a\0"
      u8"\xe8\x81\x86\0"
      u8"\xe9\x88\xb4\0"
      u8"\xe9\x9b\xb6\0"
      u8"\xe9\x9d\x88\0"
      u8"\xe9\xa0\x98\0"
      u8"\xe4\xbe\x8b\0"
      u8"\xe7\xa6\xae\0"
      u8"\xe9\x86\xb4\0"
      u8"\xe9\x9a\xb8\0"
      u8"\xe6\x83\xa1\0"
      u8"\xe4\xba\x86\0"
      u8"\xe5\x83\x9a\0"
      u8"\xe5\xaf\xae\0"
      u8"\xe5\xb0\xbf\0"
      u8"\xe6\x96\x99\0"
      u8"\xe7\x87\x8e\0"
      u8"\xe7\x99\x82\0"
      u8"\xe8\x93\xbc\0"
      u8"\xe9\x81\xbc\0"
      u8"\xe6\x9a\x88\0"
      u8"\xe9\x98\xae\0"
      u8"\xe5\x8a\x89\0"
      u8"\xe6\x9d\xbb\0"
      u8"\xe6\x9f\xb3\0"
      u8"\xe6\xb5\x81\0"
      u8"\xe6\xba\x9c\0"
      u8"\xe7\x90\x89\0"
      u8"\xe7\x95\x99\0"
      u8"\xe7\xa1\xab\0"
      u8"\xe7\xb4\x90\0"
      u8"\xe9\xa1\x9e\0"
      u8"\xe6\x88\xae\0"
      u8"\xe9\x99\xb8\0"
      u8"\xe5\x80\xab\0"
      u8"\xe5\xb4\x99\0"
      u8"\xe6\xb7\xaa\0"
      u8"\xe8\xbc\xaa\0"
      u8"\xe5\xbe\x8b\0"
      u8"\xe6\x85\x84\0"
      u8"\xe6\xa0\x97\0"
      u8"\xe9\x9a\x86\0"
      u8"\xe5\x88\xa9\0"
      u8"\xe5\x90\x8f\0"
      u8"\xe5\xb1\xa5\0"
      u8"\xe6\x98\x93\0"
      u8"\xe6\x9d\x8e\0"
      u8"\xe6\xa2\xa8\0"
      u8"\xe6\xb3\xa5\0"
      u8"\xe7\x90\x86\0"
      u8"\xe7\x97\xa2\0"
      u8"\xe7\xbd\xb9\0"
      u8"\xe8\xa3\x8f\0"
      u8"\xe8\xa3\xa1\0"
      u8"\xe9\x9b\xa2\0"
      u8"\xe5\x8c\xbf\0"
      u8"\xe6\xba\xba\0"
      u8"\xe5\x90\x9d\0"
      u8"\xe7\x87\x90\0"
      u8"\xe7\x92\x98\0"
      u8"\xe8\x97\xba\0"
      u8"\xe9\x9a\xa3\0"
      u8"\xe9\xb1\x97\0"
      u8"\xe9\xba\x9f\0"
      u8"\xe6\x9e\x97\0"
      u8"\xe6\xb7\x8b\0"
      u8"\xe8\x87\xa8\0"
      u8"\xe7\xac\xa0\0"
      u8"\xe7\xb2\x92\0"
      u8"\xe7\x8b\x80\0"
      u8"\xe7\x82\x99\0"
      u8"\xe8\xad\x98\0"
      u8"\xe4\xbb\x80\0"
      u8"\xe8\x8c\xb6\0"
      u8"\xe5\x88\xba\0"
      u8"\xe5\x88\x87\0"
      u8"\xe5\xba\xa6\0"
      u8"\xe6\x8b\x93\0"
      u8"\xe7\xb3\x96\0"
      u8"\xe5\xae\x85\0"
      u8"\xe6\xb4\x9e\0"
      u8"\xe6\x9a\xb4\0"
      u8"\xe8\xbc\xbb\0"
      u8"\xe9\x99\x8d\0"
      u8"\xe5\xbb\x93\0"
      u8"\xe5\x85\x80\0"
      u8"\xe5\x97\x80\0"
      u8"\xe5\xa1\x9a\0"
      u8"\xe6\x99\xb4\0"
      u8"\xe5\x87\x9e\0"
      u8"\xe7\x8c\xaa\0"
      u8"\xe7\x9b\x8a\0"
      u8"\xe7\xa4\xbc\0"
      u8"\xe7\xa5\x9e\0"
      u8"\xe7\xa5\xa5\0"
      u8"\xe7\xa6\x8f\0"
      u8"\xe9\x9d\x96\0"
      u8"\xe7\xb2\xbe\0"
      u8"\xe8\x98\x92\0"
      u8"\xe8\xab\xb8\0"
      u8"\xe9\x80\xb8\0"
      u8"\xe9\x83\xbd\0"
      u8"\xe9\xa3\xaf\0"
      u8"\xe9\xa3\xbc\0"
      u8"\xe9\xa4\xa8\0"
      u8"\xe9\xb6\xb4\0"
      u8"\xe9\x83\x9e\0"
      u8"\xe9\x9a\xb7\0"
      u8"\xe4\xbe\xae\0"
      u8"\xe5\x83\xa7\0"
      u8"\xe5\x85\x8d\0"
      u8"\xe5\x8b\x89\0"
      u8"\xe5\x8b\xa4\0"
      u8"\xe5\x8d\x91\0"
      u8"\xe5\x96\x9d\0"
      u8"\xe5\x98\x86\0"
      u8"\xe5\x99\xa8\0"
      u8"\xe5\xa1\x80\0"
      u8"\xe5\xa2\xa8\0"
      u8"\xe5\xb1\xa4\0"
      u8"\xe6\x82\x94\0"
      u8"\xe6\x85\xa8\0"
      u8"\xe6\x86\x8e\0"
      u8"\xe6\x87\xb2\0"
      u8"\xe6\x95\x8f\0"
      u8"\xe6\x97\xa2\0"
      u8"\xe6\x9a\x91\0"
      u8"\xe6\xa2\x85\0"
      u8"\xe6\xb5\xb7\0"
      u8"\xe6\xb8\x9a\0"
      u8"\xe6\xbc\xa2\0"
      u8"\xe7\x85\xae\0"
      u8"\xe7\x88\xab\0"
      u8"\xe7\x90\xa2\0"
      u8"\xe7\xa2\x91\0"
      u8"\xe7\xa5\x89\0"
      u8"\xe7\xa5\x88\0"
      u8"\xe7\xa5\x90\0"
      u8"\xe7\xa5\x96\0"
      u8"\xe7\xa6\x8d\0"
      u8"\xe7\xa6\x8e\0"
      u8"\xe7\xa9\x80\0"
      u8"\xe7\xaa\x81\0"
      u8"\xe7\xaf\x80\0"
      u8"\xe7\xb8\x89\0"
      u8"\xe7\xb9\x81\0"
      u8"\xe7\xbd\xb2\0"
      u8"\xe8\x80\x85\0"
      u8"\xe8\x87\xad\0"
      u8"\xe8\x89\xb9\0"
      u8"\xe8\x91\x97\0"
      u8"\xe8\xa4\x90\0"
      u8"\xe8\xa6\x96\0"
      u8"\xe8\xac\x81\0"
      u8"\xe8\xac\xb9\0"
      u8"\xe8\xb3\x93\0"
      u8"\xe8\xb4\x88\0"
      u8"\xe8\xbe\xb6\0"
      u8"\xe9\x9b\xa3\0"
      u8"\xe9\x9f\xbf\0"
      u8"\xe9\xa0\xbb\0"
      u8"\xe6\x81\xb5\0"
      u8"\xf0\xa4\x8b\xae\0"
      u8"\xe8\x88\x98\0"
      u8"\xe4\xb8\xa6\0"
      u8"\xe5\x86\xb5\0"
      u8"\xe5\x85\xa8\0"
      u8"\xe4\xbe\x80\0"
      u8"\xe5\x85\x85\0"
      u8"\xe5\x86\x80\0"
      u8"\xe5\x8b\x87\0"
      u8"\xe5\x8b\xba\0"
      u8"\xe5\x95\x95\0"
      u8"\xe5\x96\x99\0"
      u8"\xe5\x97\xa2\0"
      u8"\xe5\xa2\xb3\0"
      u8"\xe5\xa5\x84\0"
      u8"\xe5\xa5\x94\0"
      u8"\xe5\xa9\xa2\0"
      u8"\xe5\xac\xa8\0"
      u8"\xe5\xbb\x92\0"
      u8"\xe5\xbb\x99\0"
      u8"\xe5\xbd\xa9\0"
      u8"\xe5\xbe\xad\0"
      u8"\xe6\x83\x98\0"
      u8"\xe6\x85\x8e\0"
      u8"\xe6\x84\x88\0"
      u8"\xe6\x85\xa0\0"
      u8"\xe6\x88\xb4\0"
      u8"\xe6\x8f\x84\0"
      u8"\xe6\x90\x9c\0"
      u8"\xe6\x91\x92\0"
      u8"\xe6\x95\x96\0"
      u8"\xe6\x9c\x9b\0"
      u8"\xe6\x9d\x96\0"
      u8"\xe6\xbb\x9b\0"
      u8"\xe6\xbb\x8b\0"
      u8"\xe7\x80\x9e\0"
      u8"\xe7\x9e\xa7\0"
      u8"\xe7\x88\xb5\0"
      u8"\xe7\x8a\xaf\0"
      u8"\xe7\x91\xb1\0"
      u8"\xe7\x94\x86\0"
      u8"\xe7\x94\xbb\0"
      u8"\xe7\x98\x9d\0"
      u8"\xe7\x98\x9f\0"
      u8"\xe7\x9b\x9b\0"
      u8"\xe7\x9b\xb4\0"
      u8"\xe7\x9d\x8a\0"
      u8"\xe7\x9d\x80\0"
      u8"\xe7\xa3\x8c\0"
      u8"\xe7\xaa\xb1\0"
      u8"\xe7\xb1\xbb\0"
      u8"\xe7\xb5\x9b\0"
      u8"\xe7\xbc\xbe\0"
      u8"\xe8\x8d\x92\0"
      u8"\xe8\x8f\xaf\0"
      u8"\xe8\x9d\xb9\0"
      u8"\xe8\xa5\x81\0"
      u8"\xe8\xa6\x86\0"
      u8"\xe8\xaa\xbf\0"
      u8"\xe8\xab\x8b\0"
      u8"\xe8\xab\xad\0"
      u8"\xe8\xae\x8a\0"
      u8"\xe8\xbc\xb8\0"
      u8"\xe9\x81\xb2\0"
      u8"\xe9\x86\x99\0"
      u8"\xe9\x89\xb6\0"
      u8"\xe9\x99\xbc\0"
      u8"\xe9\x9f\x9b\0"
      u8"\xe9\xa0\x8b\0"
      u8"\xe9\xac\x92\0"
      u8"\xf0\xa2\xa1\x8a\0"
      u8"\xf0\xa2\xa1\x84\0"
      u8"\xf0\xa3\x8f\x95\0"
      u8"\xe3\xae\x9d\0"
      u8"\xe4\x80\x98\0"
      u8"\xe4\x80\xb9\0"
      u8"\xf0\xa5\x89\x89\0"
      u8"\xf0\xa5\xb3\x90\0"
      u8"\xf0\xa7\xbb\x93\0"
      u8"\xe9\xbd\x83\0"
      u8"\xe9\xbe\x8e\0"
      u8"\x66\x66\0"
      u8"\x66\x69\0"
      u8"\x66\x6c\0"
      u8"\x66\x66\x69\0"
      u8"\x66\x66\x6c\0"
      u8"\x73\x74\0"
      u8"\xd5\xb4\xd5\xb6\0"
      u8"\xd5\xb4\xd5\xa5\0"
      u8"\xd5\xb4\xd5\xab\0"
      u8"\xd5\xbe\xd5\xb6\0"
      u8"\xd5\xb4\xd5\xad\0"
      u8"\xd7\x99\xd6\xb4\0"
      u8"\xd7\xb2\xd6\xb7\0"
      u8"\xd7\xa2\0"
      u8"\xd7\x94\0"
      u8"\xd7\x9b\0"
      u8"\xd7\x9c\0"
      u8"\xd7\x9d\0"
      u8"\xd7\xa8\0"
      u8"\xd7\xaa\0"
      u8"\xd7\xa9\xd7\x81\0"
      u8"\xd7\xa9\xd7\x82\0"
      u8"\xd7\xa9\xd6\xbc\xd7\x81\0"
      u8"\xd7\xa9\xd6\xbc\xd7\x82\0"
      u8"\xd7\x90\xd6\xb7\0"
      u8"\xd7\x90\xd6\xb8\0"
      u8"\xd7\x90\xd6\xbc\0"
      u8"\xd7\x91\xd6\xbc\0"
      u8"\xd7\x92\xd6\xbc\0"
      u8"\xd7\x93\xd6\xbc\0"
      u8"\xd7\x94\xd6\xbc\0"
      u8"\xd7\x95\xd6\xbc\0"
      u8"\xd7\x96\xd6\xbc\0"
      u8"\xd7\x98\xd6\xbc\0"
      u8"\xd7\x99\xd6\xbc\0"
      u8"\xd7\x9a\xd6\xbc\0"
      u8"\xd7\x9b\xd6\xbc\0"
      u8"\xd7\x9c\xd6\xbc\0"
      u8"\xd7\x9e\xd6\xbc\0"
      u8"\xd7\xa0\xd6\xbc\0"
      u8"\xd7\xa1\xd6\xbc\0"
      u8"\xd7\xa3\xd6\xbc\0"
      u8"\xd7\xa4\xd6\xbc\0"
      u8"\xd7\xa6\xd6\xbc\0"
      u8"\xd7\xa7\xd6\xbc\0"
      u8"\xd7\xa8\xd6\xbc\0"
      u8"\xd7\xa9\xd6\xbc\0"
      u8"\xd7\xaa\xd6\xbc\0"
      u8"\xd7\x95\xd6\xb9\0"
      u8"\xd7\x91\xd6\xbf\0"
      u8"\xd7\x9b\xd6\xbf\0"
      u8"\xd7\xa4\xd6\xbf\0"
      u8"\xd7\x90\xd7\x9c\0"
      u8"\xd9\xb1\0"
      u8"\xd9\xbb\0"
      u8"\xd9\xbe\0"
      u8"\xda\x80\0"
      u8"\xd9\xba\0"
      u8"\xd9\xbf\0"
      u8"\xd9\xb9\0"
      u8"\xda\xa4\0"
      u8"\xda\xa6\0"
      u8"\xda\x84\0"
      u8"\xda\x83\0"
      u8"\xda\x86\0"
      u8"\xda\x87\0"
      u8"\xda\x8d\0"
      u8"\xda\x8c\0"
      u8"\xda\x8e\0"
      u8"\xda\x88\0"
      u8"\xda\x98\0"
      u8"\xda\x91\0"
      u8"\xda\xa9\0"
      u8"\xda\xaf\0"
      u8"\xda\xb3\0"
      u8"\xda\xb1\0"
      u8"\xda\xba\0"
      u8"\xda\xbb\0"
      u8"\xdb\x80\0"
      u8"\xdb\x81\0"
      u8"\xda\xbe\0"
      u8"\xdb\x92\0"
      u8"\xdb\x93\0"
      u8"\xda\xad\0"
      u8"\xdb\x87\0"
      u8"\xdb\x86\0"
      u8"\xdb\x88\0"
      u8"\xdb\x8b\0"
      u8"\xdb\x85\0"
      u8"\xdb\x89\0"
      u8"\xdb\x90\0"
      u8"\xd9\x89\0"
      u8"\xd8\xa6\xd8\xa7\0"
      u8"\xd8\xa6\xdb\x95\0"
      u8"\xd8\xa6\xd9\x88\0"
      u8"\xd8\xa6\xdb\x87\0"
      u8"\xd8\xa6\xdb\x86\0"
      u8"\xd8\xa6\xdb\x88\0"
      u8"\xd8\xa6\xdb\x90\0"
      u8"\xd8\xa6\xd9\x89\0"
      u8"\xdb\x8c\0"
      u8"\xd8\xa6\xd8\xac\0"
      u8"\xd8\xa6\xd8\xad\0"
      u8"\xd8\xa6\xd9\x85\0"
      u8"\xd8\xa6\xd9\x8a\0"
      u8"\xd8\xa8\xd8\xac\0"
      u8"\xd8\xa8\xd8\xad\0"
      u8"\xd8\xa8\xd8\xae\0"
      u8"\xd8\xa8\xd9\x85\0"
      u8"\xd8\xa8\xd9\x89\0"
      u8"\xd8\xa8\xd9\x8a\0"
      u8"\xd8\xaa\xd8\xac\0"
      u8"\xd8\xaa\xd8\xad\0"
      u8"\xd8\xaa\xd8\xae\0"
      u8"\xd8\xaa\xd9\x85\0"
      u8"\xd8\xaa\xd9\x89\0"
      u8"\xd8\xaa\xd9\x8a\0"
      u8"\xd8\xab\xd8\xac\0"
      u8"\xd8\xab\xd9\x85\0"
      u8"\xd8\xab\xd9\x89\0"
      u8"\xd8\xab\xd9\x8a\0"
      u8"\xd8\xac\xd8\xad\0"
      u8"\xd8\xac\xd9\x85\0"
      u8"\xd8\xad\xd8\xac\0"
      u8"\xd8\xad\xd9\x85\0"
      u8"\xd8\xae\xd8\xac\0"
      u8"\xd8\xae\xd8\xad\0"
      u8"\xd8\xae\xd9\x85\0"
      u8"\xd8\xb3\xd8\xac\0"
      u8"\xd8\xb3\xd8\xad\0"
      u8"\xd8\xb3\xd8\xae\0"
      u8"\xd8\xb3\xd9\x85\0"
      u8"\xd8\xb5\xd8\xad\0"
      u8"\xd8\xb5\xd9\x85\0"
      u8"\xd8\xb6\xd8\xac\0"
      u8"\xd8\xb6\xd8\xad\0"
      u8"\xd8\xb6\xd8\xae\0"
      u8"\xd8\xb6\xd9\x85\0"
      u8"\xd8\xb7\xd8\xad\0"
      u8"\xd8\xb7\xd9\x85\0"
      u8"\xd8\xb8\xd9\x85\0"
      u8"\xd8\xb9\xd8\xac\0"
      u8"\xd8\xb9\xd9\x85\0"
      u8"\xd8\xba\xd8\xac\0"
      u8"\xd8\xba\xd9\x85\0"
      u8"\xd9\x81\xd8\xac\0"
      u8"\xd9\x81\xd8\xad\0"
      u8"\xd9\x81\xd8\xae\0"
      u8"\xd9\x81\xd9\x85\0"
      u8"\xd9\x81\xd9\x89\0"
      u8"\xd9\x81\xd9\x8a\0"
      u8"\xd9\x82\xd8\xad\0"
      u8"\xd9\x82\xd9\x85\0"
      u8"\xd9\x82\xd9\x89\0"
      u8"\xd9\x82\xd9\x8a\0"
      u8"\xd9\x83\xd8\xa7\0"
      u8"\xd9\x83\xd8\xac\0"
      u8"\xd9\x83\xd8\xad\0"
      u8"\xd9\x83\xd8\xae\0"
      u8"\xd9\x83\xd9\x84\0"
      u8"\xd9\x83\xd9\x85\0"
      u8"\xd9\x83\xd9\x89\0"
      u8"\xd9\x83\xd9\x8a\0"
      u8"\xd9\x84\xd8\xac\0"
      u8"\xd9\x84\xd8\xad\0"
      u8"\xd9\x84\xd8\xae\0"
      u8"\xd9\x84\xd9\x85\0"
      u8"\xd9\x84\xd9\x89\0"
      u8"\xd9\x84\xd9\x8a\0"
      u8"\xd9\x85\xd8\xac\0"
      u8"\xd9\x85\xd8\xad\0"
      u8"\xd9\x85\xd8\xae\0"
      u8"\xd9\x85\xd9\x85\0"
      u8"\xd9\x85\xd9\x89\0"
      u8"\xd9\x85\xd9\x8a\0"
      u8"\xd9\x86\xd8\xac\0"
      u8"\xd9\x86\xd8\xad\0"
      u8"\xd9\x86\xd8\xae\0"
      u8"\xd9\x86\xd9\x85\0"
      u8"\xd9\x86\xd9\x89\0"
      u8"\xd9\x86\xd9\x8a\0"
      u8"\xd9\x87\xd8\xac\0"
      u8"\xd9\x87\xd9\x85\0"
      u8"\xd9\x87\xd9\x89\0"
      u8"\xd9\x87\xd9\x8a\0"
      u8"\xd9\x8a\xd8\xac\0"
      u8"\xd9\x8a\xd8\xad\0"
      u8"\xd9\x8a\xd8\xae\0"
      u8"\xd9\x8a\xd9\x85\0"
      u8"\xd9\x8a\xd9\x89\0"
      u8"\xd9\x8a\xd9\x8a\0"
      u8"\xd8\xb0\xd9\xb0\0"
      u8"\xd8\xb1\xd9\xb0\0"
      u8"\xd9\x89\xd9\xb0\0"
      u8"\40\xd9\x8c\xd9\x91\0"
      u8"\40\xd9\x8d\xd9\x91\0"
      u8"\40\xd9\x8e\xd9\x91\0"
      u8"\40\xd9\x8f\xd9\x91\0"
      u8"\40\xd9\x90\xd9\x91\0"
      u8"\40\xd9\x91\xd9\xb0\0"
      u8"\xd8\xa6\xd8\xb1\0"
      u8"\xd8\xa6\xd8\xb2\0"
      u8"\xd8\xa6\xd9\x86\0"
      u8"\xd8\xa8\xd8\xb1\0"
      u8"\xd8\xa8\xd8\xb2\0"
      u8"\xd8\xa8\xd9\x86\0"
      u8"\xd8\xaa\xd8\xb1\0"
      u8"\xd8\xaa\xd8\xb2\0"
      u8"\xd8\xaa\xd9\x86\0"
      u8"\xd8\xab\xd8\xb1\0"
      u8"\xd8\xab\xd8\xb2\0"
      u8"\xd8\xab\xd9\x86\0"
      u8"\xd9\x85\xd8\xa7\0"
      u8"\xd9\x86\xd8\xb1\0"
      u8"\xd9\x86\xd8\xb2\0"
      u8"\xd9\x86\xd9\x86\0"
      u8"\xd9\x8a\xd8\xb1\0"
      u8"\xd9\x8a\xd8\xb2\0"
      u8"\xd9\x8a\xd9\x86\0"
      u8"\xd8\xa6\xd8\xae\0"
      u8"\xd8\xa6\xd9\x87\0"
      u8"\xd8\xa8\xd9\x87\0"
      u8"\xd8\xaa\xd9\x87\0"
      u8"\xd8\xb5\xd8\xae\0"
      u8"\xd9\x84\xd9\x87\0"
      u8"\xd9\x86\xd9\x87\0"
      u8"\xd9\x87\xd9\xb0\0"
      u8"\xd9\x8a\xd9\x87\0"
      u8"\xd8\xab\xd9\x87\0"
      u8"\xd8\xb3\xd9\x87\0"
      u8"\xd8\xb4\xd9\x85\0"
      u8"\xd8\xb4\xd9\x87\0"
      u8"\xd9\x80\xd9\x8e\xd9\x91\0"
      u8"\xd9\x80\xd9\x8f\xd9\x91\0"
      u8"\xd9\x80\xd9\x90\xd9\x91\0"
      u8"\xd8\xb7\xd9\x89\0"
      u8"\xd8\xb7\xd9\x8a\0"
      u8"\xd8\xb9\xd9\x89\0"
      u8"\xd8\xb9\xd9\x8a\0"
      u8"\xd8\xba\xd9\x89\0"
      u8"\xd8\xba\xd9\x8a\0"
      u8"\xd8\xb3\xd9\x89\0"
      u8"\xd8\xb3\xd9\x8a\0"
      u8"\xd8\xb4\xd9\x89\0"
      u8"\xd8\xb4\xd9\x8a\0"
      u8"\xd8\xad\xd9\x89\0"
      u8"\xd8\xad\xd9\x8a\0"
      u8"\xd8\xac\xd9\x89\0"
      u8"\xd8\xac\xd9\x8a\0"
      u8"\xd8\xae\xd9\x89\0"
      u8"\xd8\xae\xd9\x8a\0"
      u8"\xd8\xb5\xd9\x89\0"
      u8"\xd8\xb5\xd9\x8a\0"
      u8"\xd8\xb6\xd9\x89\0"
      u8"\xd8\xb6\xd9\x8a\0"
      u8"\xd8\xb4\xd8\xac\0"
      u8"\xd8\xb4\xd8\xad\0"
      u8"\xd8\xb4\xd8\xae\0"
      u8"\xd8\xb4\xd8\xb1\0"
      u8"\xd8\xb3\xd8\xb1\0"
      u8"\xd8\xb5\xd8\xb1\0"
      u8"\xd8\xb6\xd8\xb1\0"
      u8"\xd8\xa7\xd9\x8b\0"
      u8"\xd8\xaa\xd8\xac\xd9\x85\0"
      u8"\xd8\xaa\xd8\xad\xd8\xac\0"
      u8"\xd8\xaa\xd8\xad\xd9\x85\0"
      u8"\xd8\xaa\xd8\xae\xd9\x85\0"
      u8"\xd8\xaa\xd9\x85\xd8\xac\0"
      u8"\xd8\xaa\xd9\x85\xd8\xad\0"
      u8"\xd8\xaa\xd9\x85\xd8\xae\0"
      u8"\xd8\xac\xd9\x85\xd8\xad\0"
      u8"\xd8\xad\xd9\x85\xd9\x8a\0"
      u8"\xd8\xad\xd9\x85\xd9\x89\0"
      u8"\xd8\xb3\xd8\xad\xd8\xac\0"
      u8"\xd8\xb3\xd8\xac\xd8\xad\0"
      u8"\xd8\xb3\xd8\xac\xd9\x89\0"
      u8"\xd8\xb3\xd9\x85\xd8\xad\0"
      u8"\xd8\xb3\xd9\x85\xd8\xac\0"
      u8"\xd8\xb3\xd9\x85\xd9\x85\0"
      u8"\xd8\xb5\xd8\xad\xd8\xad\0"
      u8"\xd8\xb5\xd9\x85\xd9\x85\0"
      u8"\xd8\xb4\xd8\xad\xd9\x85\0"
      u8"\xd8\xb4\xd8\xac\xd9\x8a\0"
      u8"\xd8\xb4\xd9\x85\xd8\xae\0"
      u8"\xd8\xb4\xd9\x85\xd9\x85\0"
      u8"\xd8\xb6\xd8\xad\xd9\x89\0"
      u8"\xd8\xb6\xd8\xae\xd9\x85\0"
      u8"\xd8\xb7\xd9\x85\xd8\xad\0"
      u8"\xd8\xb7\xd9\x85\xd9\x85\0"
      u8"\xd8\xb7\xd9\x85\xd9\x8a\0"
      u8"\xd8\xb9\xd8\xac\xd9\x85\0"
      u8"\xd8\xb9\xd9\x85\xd9\x85\0"
      u8"\xd8\xb9\xd9\x85\xd9\x89\0"
      u8"\xd8\xba\xd9\x85\xd9\x85\0"
      u8"\xd8\xba\xd9\x85\xd9\x8a\0"
      u8"\xd8\xba\xd9\x85\xd9\x89\0"
      u8"\xd9\x81\xd8\xae\xd9\x85\0"
      u8"\xd9\x82\xd9\x85\xd8\xad\0"
      u8"\xd9\x82\xd9\x85\xd9\x85\0"
      u8"\xd9\x84\xd8\xad\xd9\x85\0"
      u8"\xd9\x84\xd8\xad\xd9\x8a\0"
      u8"\xd9\x84\xd8\xad\xd9\x89\0"
      u8"\xd9\x84\xd8\xac\xd8\xac\0"
      u8"\xd9\x84\xd8\xae\xd9\x85\0"
      u8"\xd9\x84\xd9\x85\xd8\xad\0"
      u8"\xd9\x85\xd8\xad\xd8\xac\0"
      u8"\xd9\x85\xd8\xad\xd9\x85\0"
      u8"\xd9\x85\xd8\xad\xd9\x8a\0"
      u8"\xd9\x85\xd8\xac\xd8\xad\0"
      u8"\xd9\x85\xd8\xac\xd9\x85\0"
      u8"\xd9\x85\xd8\xae\xd8\xac\0"
      u8"\xd9\x85\xd8\xae\xd9\x85\0"
      u8"\xd9\x85\xd8\xac\xd8\xae\0"
      u8"\xd9\x87\xd9\x85\xd8\xac\0"
      u8"\xd9\x87\xd9\x85\xd9\x85\0"
      u8"\xd9\x86\xd8\xad\xd9\x85\0"
      u8"\xd9\x86\xd8\xad\xd9\x89\0"
      u8"\xd9\x86\xd8\xac\xd9\x85\0"
      u8"\xd9\x86\xd8\xac\xd9\x89\0"
      u8"\xd9\x86\xd9\x85\xd9\x8a\0"
      u8"\xd9\x86\xd9\x85\xd9\x89\0"
      u8"\xd9\x8a\xd9\x85\xd9\x85\0"
      u8"\xd8\xa8\xd8\xae\xd9\x8a\0"
      u8"\xd8\xaa\xd8\xac\xd9\x8a\0"
      u8"\xd8\xaa\xd8\xac\xd9\x89\0"
      u8"\xd8\xaa\xd8\xae\xd9\x8a\0"
      u8"\xd8\xaa\xd8\xae\xd9\x89\0"
      u8"\xd8\xaa\xd9\x85\xd9\x8a\0"
      u8"\xd8\xaa\xd9\x85\xd9\x89\0"
      u8"\xd8\xac\xd9\x85\xd9\x8a\0"
      u8"\xd8\xac\xd8\xad\xd9\x89\0"
      u8"\xd8\xac\xd9\x85\xd9\x89\0"
      u8"\xd8\xb3\xd8\xae\xd9\x89\0"
      u8"\xd8\xb5\xd8\xad\xd9\x8a\0"
      u8"\xd8\xb4\xd8\xad\xd9\x8a\0"
      u8"\xd8\xb6\xd8\xad\xd9\x8a\0"
      u8"\xd9\x84\xd8\xac\xd9\x8a\0"
      u8"\xd9\x84\xd9\x85\xd9\x8a\0"
      u8"\xd9\x8a\xd8\xad\xd9\x8a\0"
      u8"\xd9\x8a\xd8\xac\xd9\x8a\0"
      u8"\xd9\x8a\xd9\x85\xd9\x8a\0"
      u8"\xd9\x85\xd9\x85\xd9\x8a\0"
      u8"\xd9\x82\xd9\x85\xd9\x8a\0"
      u8"\xd9\x86\xd8\xad\xd9\x8a\0"
      u8"\xd8\xb9\xd9\x85\xd9\x8a\0"
      u8"\xd9\x83\xd9\x85\xd9\x8a\0"
      u8"\xd9\x86\xd8\xac\xd8\xad\0"
      u8"\xd9\x85\xd8\xae\xd9\x8a\0"
      u8"\xd9\x84\xd8\xac\xd9\x85\0"
      u8"\xd9\x83\xd9\x85\xd9\x85\0"
      u8"\xd8\xac\xd8\xad\xd9\x8a\0"
      u8"\xd8\xad\xd8\xac\xd9\x8a\0"
      u8"\xd9\x85\xd8\xac\xd9\x8a\0"
      u8"\xd9\x81\xd9\x85\xd9\x8a\0"
      u8"\xd8\xa8\xd8\xad\xd9\x8a\0"
      u8"\xd8\xb3\xd8\xae\xd9\x8a\0"
      u8"\xd9\x86\xd8\xac\xd9\x8a\0"
      u8"\xd8\xb5\xd9\x84\xdb\x92\0"
      u8"\xd9\x82\xd9\x84\xdb\x92\0"
      u8"\xd8\xa7\xd9\x84\xd9\x84\xd9\x87\0"
      u8"\xd8\xa7\xd9\x83\xd8\xa8\xd8\xb1\0"
      u8"\xd9\x85\xd8\xad\xd9\x85\xd8\xaf\0"
      u8"\xd8\xb5\xd9\x84\xd8\xb9\xd9\x85\0"
      u8"\xd8\xb1\xd8\xb3\xd9\x88\xd9\x84\0"
      u8"\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\0"
      u8"\xd9\x88\xd8\xb3\xd9\x84\xd9\x85\0"
      u8"\xd8\xb5\xd9\x84\xd9\x89\0"
      u8"\xd8\xb5\xd9\x84\xd9\x89\40\xd8\xa7\xd9\x84\xd9\x84\xd9\x87\40\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\40"
      u8"\xd9\x88\xd8\xb3\xd9\x84\xd9\x85\0"
      u8"\xd8\xac\xd9\x84\40\xd8\xac\xd9\x84\xd8\xa7\xd9\x84\xd9\x87\0"
      u8"\xd8\xb1\xdb\x8c\xd8\xa7\xd9\x84\0"
      u8"\54\0"
      u8"\xe3\x80\x81\0"
      u8"\72\0"
      u8"\41\0"
      u8"\77\0"
      u8"\xe3\x80\x96\0"
      u8"\xe3\x80\x97\0"
      u8"\xe2\x80\x94\0"
      u8"\xe2\x80\x93\0"
      u8"\x5f\0"
      u8"\x7b\0"
      u8"\x7d\0"
      u8"\xe3\x80\x94\0"
      u8"\xe3\x80\x95\0"
      u8"\xe3\x80\x90\0"
      u8"\xe3\x80\x91\0"
      u8"\xe3\x80\x8a\0"
      u8"\xe3\x80\x8b\0"
      u8"\xe3\x80\x8c\0"
      u8"\xe3\x80\x8d\0"
      u8"\xe3\x80\x8e\0"
      u8"\xe3\x80\x8f\0"
      u8"\x5b\0"
      u8"\x5d\0"
      u8"\43\0"
      u8"\46\0"
      u8"\52\0"
      u8"\55\0"
      u8"\74\0"
      u8"\76\0"
      u8"\x5c\0"
      u8"\44\0"
      u8"\45\0"
      u8"\x40\0"
      u8"\40\xd9\x8b\0"
      u8"\xd9\x80\xd9\x8b\0"
      u8"\40\xd9\x8c\0"
      u8"\40\xd9\x8d\0"
      u8"\40\xd9\x8e\0"
      u8"\xd9\x80\xd9\x8e\0"
      u8"\40\xd9\x8f\0"
      u8"\xd9\x80\xd9\x8f\0"
      u8"\40\xd9\x90\0"
      u8"\xd9\x80\xd9\x90\0"
      u8"\40\xd9\x91\0"
      u8"\xd9\x80\xd9\x91\0"
      u8"\40\xd9\x92\0"
      u8"\xd9\x80\xd9\x92\0"
      u8"\xd8\xa1\0"
      u8"\xd8\xa2\0"
      u8"\xd8\xa3\0"
      u8"\xd8\xa4\0"
      u8"\xd8\xa5\0"
      u8"\xd8\xa6\0"
      u8"\xd8\xa7\0"
      u8"\xd8\xa8\0"
      u8"\xd8\xa9\0"
      u8"\xd8\xaa\0"
      u8"\xd8\xab\0"
      u8"\xd8\xac\0"
      u8"\xd8\xad\0"
      u8"\xd8\xae\0"
      u8"\xd8\xaf\0"
      u8"\xd8\xb0\0"
      u8"\xd8\xb1\0"
      u8"\xd8\xb2\0"
      u8"\xd8\xb3\0"
      u8"\xd8\xb4\0"
      u8"\xd8\xb5\0"
      u8"\xd8\xb6\0"
      u8"\xd8\xb7\0"
      u8"\xd8\xb8\0"
      u8"\xd8\xb9\0"
      u8"\xd8\xba\0"
      u8"\xd9\x81\0"
      u8"\xd9\x82\0"
      u8"\xd9\x83\0"
      u8"\xd9\x84\0"
      u8"\xd9\x85\0"
      u8"\xd9\x86\0"
      u8"\xd9\x87\0"
      u8"\xd9\x88\0"
      u8"\xd9\x8a\0"
      u8"\xd9\x84\xd8\xa2\0"
      u8"\xd9\x84\xd8\xa3\0"
      u8"\xd9\x84\xd8\xa5\0"
      u8"\xd9\x84\xd8\xa7\0"
      u8"\42\0"
      u8"\47\0"
      u8"\57\0"
      u8"\x5e\0"
      u8"\x7c\0"
      u8"\x7e\0"
      u8"\xe2\xa6\x85\0"
      u8"\xe2\xa6\x86\0"
      u8"\xe3\x83\xbb\0"
      u8"\xe3\x82\xa1\0"
      u8"\xe3\x82\xa3\0"
      u8"\xe3\x82\xa5\0"
      u8"\xe3\x82\xa7\0"
      u8"\xe3\x82\xa9\0"
      u8"\xe3\x83\xa3\0"
      u8"\xe3\x83\xa5\0"
      u8"\xe3\x83\xa7\0"
      u8"\xe3\x83\x83\0"
      u8"\xe3\x83\xbc\0"
      u8"\xe3\x83\xb3\0"
      u8"\xe3\x82\x99\0"
      u8"\xe3\x82\x9a\0"
      u8"\xc2\xa2\0"
      u8"\xc2\xa3\0"
      u8"\xc2\xac\0"
      u8"\xc2\xa6\0"
      u8"\xc2\xa5\0"
      u8"\xe2\x82\xa9\0"
      u8"\xe2\x94\x82\0"
      u8"\xe2\x86\x90\0"
      u8"\xe2\x86\x91\0"
      u8"\xe2\x86\x92\0"
      u8"\xe2\x86\x93\0"
      u8"\xe2\x96\xa0\0"
      u8"\xe2\x97\x8b\0"
      u8"\xf0\x90\x90\xa8\0"
      u8"\xf0\x90\x90\xa9\0"
      u8"\xf0\x90\x90\xaa\0"
      u8"\xf0\x90\x90\xab\0"
      u8"\xf0\x90\x90\xac\0"
      u8"\xf0\x90\x90\xad\0"
      u8"\xf0\x90\x90\xae\0"
      u8"\xf0\x90\x90\xaf\0"
      u8"\xf0\x90\x90\xb0\0"
      u8"\xf0\x90\x90\xb1\0"
      u8"\xf0\x90\x90\xb2\0"
      u8"\xf0\x90\x90\xb3\0"
      u8"\xf0\x90\x90\xb4\0"
      u8"\xf0\x90\x90\xb5\0"
      u8"\xf0\x90\x90\xb6\0"
      u8"\xf0\x90\x90\xb7\0"
      u8"\xf0\x90\x90\xb8\0"
      u8"\xf0\x90\x90\xb9\0"
      u8"\xf0\x90\x90\xba\0"
      u8"\xf0\x90\x90\xbb\0"
      u8"\xf0\x90\x90\xbc\0"
      u8"\xf0\x90\x90\xbd\0"
      u8"\xf0\x90\x90\xbe\0"
      u8"\xf0\x90\x90\xbf\0"
      u8"\xf0\x90\x91\x80\0"
      u8"\xf0\x90\x91\x81\0"
      u8"\xf0\x90\x91\x82\0"
      u8"\xf0\x90\x91\x83\0"
      u8"\xf0\x90\x91\x84\0"
      u8"\xf0\x90\x91\x85\0"
      u8"\xf0\x90\x91\x86\0"
      u8"\xf0\x90\x91\x87\0"
      u8"\xf0\x90\x91\x88\0"
      u8"\xf0\x90\x91\x89\0"
      u8"\xf0\x90\x91\x8a\0"
      u8"\xf0\x90\x91\x8b\0"
      u8"\xf0\x90\x91\x8c\0"
      u8"\xf0\x90\x91\x8d\0"
      u8"\xf0\x90\x91\x8e\0"
      u8"\xf0\x90\x91\x8f\0"
      u8"\xf0\x90\x93\x98\0"
      u8"\xf0\x90\x93\x99\0"
      u8"\xf0\x90\x93\x9a\0"
      u8"\xf0\x90\x93\x9b\0"
      u8"\xf0\x90\x93\x9c\0"
      u8"\xf0\x90\x93\x9d\0"
      u8"\xf0\x90\x93\x9e\0"
      u8"\xf0\x90\x93\x9f\0"
      u8"\xf0\x90\x93\xa0\0"
      u8"\xf0\x90\x93\xa1\0"
      u8"\xf0\x90\x93\xa2\0"
      u8"\xf0\x90\x93\xa3\0"
      u8"\xf0\x90\x93\xa4\0"
      u8"\xf0\x90\x93\xa5\0"
      u8"\xf0\x90\x93\xa6\0"
      u8"\xf0\x90\x93\xa7\0"
      u8"\xf0\x90\x93\xa8\0"
      u8"\xf0\x90\x93\xa9\0"
      u8"\xf0\x90\x93\xaa\0"
      u8"\xf0\x90\x93\xab\0"
      u8"\xf0\x90\x93\xac\0"
      u8"\xf0\x90\x93\xad\0"
      u8"\xf0\x90\x93\xae\0"
      u8"\xf0\x90\x93\xaf\0"
      u8"\xf0\x90\x93\xb0\0"
      u8"\xf0\x90\x93\xb1\0"
      u8"\xf0\x90\x93\xb2\0"
      u8"\xf0\x90\x93\xb3\0"
      u8"\xf0\x90\x93\xb4\0"
      u8"\xf0\x90\x93\xb5\0"
      u8"\xf0\x90\x93\xb6\0"
      u8"\xf0\x90\x93\xb7\0"
      u8"\xf0\x90\x93\xb8\0"
      u8"\xf0\x90\x93\xb9\0"
      u8"\xf0\x90\x93\xba\0"
      u8"\xf0\x90\x93\xbb\0"
      u8"\xf0\x90\x96\x97\0"
      u8"\xf0\x90\x96\x98\0"
      u8"\xf0\x90\x96\x99\0"
      u8"\xf0\x90\x96\x9a\0"
      u8"\xf0\x90\x96\x9b\0"
      u8"\xf0\x90\x96\x9c\0"
      u8"\xf0\x90\x96\x9d\0"
      u8"\xf0\x90\x96\x9e\0"
      u8"\xf0\x90\x96\x9f\0"
      u8"\xf0\x90\x96\xa0\0"
      u8"\xf0\x90\x96\xa1\0"
      u8"\xf0\x90\x96\xa3\0"
      u8"\xf0\x90\x96\xa4\0"
      u8"\xf0\x90\x96\xa5\0"
      u8"\xf0\x90\x96\xa6\0"
      u8"\xf0\x90\x96\xa7\0"
      u8"\xf0\x90\x96\xa8\0"
      u8"\xf0\x90\x96\xa9\0"
      u8"\xf0\x90\x96\xaa\0"
      u8"\xf0\x90\x96\xab\0"
      u8"\xf0\x90\x96\xac\0"
      u8"\xf0\x90\x96\xad\0"
      u8"\xf0\x90\x96\xae\0"
      u8"\xf0\x90\x96\xaf\0"
      u8"\xf0\x90\x96\xb0\0"
      u8"\xf0\x90\x96\xb1\0"
      u8"\xf0\x90\x96\xb3\0"
      u8"\xf0\x90\x96\xb4\0"
      u8"\xf0\x90\x96\xb5\0"
      u8"\xf0\x90\x96\xb6\0"
      u8"\xf0\x90\x96\xb7\0"
      u8"\xf0\x90\x96\xb8\0"
      u8"\xf0\x90\x96\xb9\0"
      u8"\xf0\x90\x96\xbb\0"
      u8"\xf0\x90\x96\xbc\0"
      u8"\xcb\x90\0"
      u8"\xcb\x91\0"
      u8"\xca\x99\0"
      u8"\xca\xa3\0"
      u8"\xea\xad\xa6\0"
      u8"\xca\xa5\0"
      u8"\xca\xa4\0"
      u8"\xe1\xb6\x91\0"
      u8"\xc9\x98\0"
      u8"\xc9\x9e\0"
      u8"\xca\xa9\0"
      u8"\xc9\xa2\0"
      u8"\xca\x9b\0"
      u8"\xca\x9c\0"
      u8"\xc9\xa7\0"
      u8"\xca\x84\0"
      u8"\xca\xaa\0"
      u8"\xca\xab\0"
      u8"\xf0\x9d\xbc\x84\0"
      u8"\xea\x9e\x8e\0"
      u8"\xc9\xae\0"
      u8"\xf0\x9d\xbc\x85\0"
      u8"\xca\x8e\0"
      u8"\xf0\x9d\xbc\x86\0"
      u8"\xc9\xb6\0"
      u8"\xc9\xb7\0"
      u8"\xc9\xba\0"
      u8"\xf0\x9d\xbc\x88\0"
      u8"\xc9\xbe\0"
      u8"\xca\xa8\0"
      u8"\xca\xa6\0"
      u8"\xea\xad\xa7\0"
      u8"\xca\xa7\0"
      u8"\xe2\xb1\xb1\0"
      u8"\xca\x8f\0"
      u8"\xca\xa1\0"
      u8"\xca\xa2\0"
      u8"\xca\x98\0"
      u8"\xc7\x80\0"
      u8"\xc7\x81\0"
      u8"\xc7\x82\0"
      u8"\xf0\x9d\xbc\x8a\0"
      u8"\xf0\x9d\xbc\x9e\0"
      u8"\xf0\x90\xb3\x80\0"
      u8"\xf0\x90\xb3\x81\0"
      u8"\xf0\x90\xb3\x82\0"
      u8"\xf0\x90\xb3\x83\0"
      u8"\xf0\x90\xb3\x84\0"
      u8"\xf0\x90\xb3\x85\0"
      u8"\xf0\x90\xb3\x86\0"
      u8"\xf0\x90\xb3\x87\0"
      u8"\xf0\x90\xb3\x88\0"
      u8"\xf0\x90\xb3\x89\0"
      u8"\xf0\x90\xb3\x8a\0"
      u8"\xf0\x90\xb3\x8b\0"
      u8"\xf0\x90\xb3\x8c\0"
      u8"\xf0\x90\xb3\x8d\0"
      u8"\xf0\x90\xb3\x8e\0"
      u8"\xf0\x90\xb3\x8f\0"
      u8"\xf0\x90\xb3\x90\0"
      u8"\xf0\x90\xb3\x91\0"
      u8"\xf0\x90\xb3\x92\0"
      u8"\xf0\x90\xb3\x93\0"
      u8"\xf0\x90\xb3\x94\0"
      u8"\xf0\x90\xb3\x95\0"
      u8"\xf0\x90\xb3\x96\0"
      u8"\xf0\x90\xb3\x97\0"
      u8"\xf0\x90\xb3\x98\0"
      u8"\xf0\x90\xb3\x99\0"
      u8"\xf0\x90\xb3\x9a\0"
      u8"\xf0\x90\xb3\x9b\0"
      u8"\xf0\x90\xb3\x9c\0"
      u8"\xf0\x90\xb3\x9d\0"
      u8"\xf0\x90\xb3\x9e\0"
      u8"\xf0\x90\xb3\x9f\0"
      u8"\xf0\x90\xb3\xa0\0"
      u8"\xf0\x90\xb3\xa1\0"
      u8"\xf0\x90\xb3\xa2\0"
      u8"\xf0\x90\xb3\xa3\0"
      u8"\xf0\x90\xb3\xa4\0"
      u8"\xf0\x90\xb3\xa5\0"
      u8"\xf0\x90\xb3\xa6\0"
      u8"\xf0\x90\xb3\xa7\0"
      u8"\xf0\x90\xb3\xa8\0"
      u8"\xf0\x90\xb3\xa9\0"
      u8"\xf0\x90\xb3\xaa\0"
      u8"\xf0\x90\xb3\xab\0"
      u8"\xf0\x90\xb3\xac\0"
      u8"\xf0\x90\xb3\xad\0"
      u8"\xf0\x90\xb3\xae\0"
      u8"\xf0\x90\xb3\xaf\0"
      u8"\xf0\x90\xb3\xb0\0"
      u8"\xf0\x90\xb3\xb1\0"
      u8"\xf0\x90\xb3\xb2\0"
      u8"\xf0\x90\xb5\xb0\0"
      u8"\xf0\x90\xb5\xb1\0"
      u8"\xf0\x90\xb5\xb2\0"
      u8"\xf0\x90\xb5\xb3\0"
      u8"\xf0\x90\xb5\xb4\0"
      u8"\xf0\x90\xb5\xb5\0"
      u8"\xf0\x90\xb5\xb6\0"
      u8"\xf0\x90\xb5\xb7\0"
      u8"\xf0\x90\xb5\xb8\0"
      u8"\xf0\x90\xb5\xb9\0"
      u8"\xf0\x90\xb5\xba\0"
      u8"\xf0\x90\xb5\xbb\0"
      u8"\xf0\x90\xb5\xbc\0"
      u8"\xf0\x90\xb5\xbd\0"
      u8"\xf0\x90\xb5\xbe\0"
      u8"\xf0\x90\xb5\xbf\0"
      u8"\xf0\x90\xb6\x80\0"
      u8"\xf0\x90\xb6\x81\0"
      u8"\xf0\x90\xb6\x82\0"
      u8"\xf0\x90\xb6\x83\0"
      u8"\xf0\x90\xb6\x84\0"
      u8"\xf0\x90\xb6\x85\0"
      u8"\xf0\x91\xa3\x80\0"
      u8"\xf0\x91\xa3\x81\0"
      u8"\xf0\x91\xa3\x82\0"
      u8"\xf0\x91\xa3\x83\0"
      u8"\xf0\x91\xa3\x84\0"
      u8"\xf0\x91\xa3\x85\0"
      u8"\xf0\x91\xa3\x86\0"
      u8"\xf0\x91\xa3\x87\0"
      u8"\xf0\x91\xa3\x88\0"
      u8"\xf0\x91\xa3\x89\0"
      u8"\xf0\x91\xa3\x8a\0"
      u8"\xf0\x91\xa3\x8b\0"
      u8"\xf0\x91\xa3\x8c\0"
      u8"\xf0\x91\xa3\x8d\0"
      u8"\xf0\x91\xa3\x8e\0"
      u8"\xf0\x91\xa3\x8f\0"
      u8"\xf0\x91\xa3\x90\0"
      u8"\xf0\x91\xa3\x91\0"
      u8"\xf0\x91\xa3\x92\0"
      u8"\xf0\x91\xa3\x93\0"
      u8"\xf0\x91\xa3\x94\0"
      u8"\xf0\x91\xa3\x95\0"
      u8"\xf0\x91\xa3\x96\0"
      u8"\xf0\x91\xa3\x97\0"
      u8"\xf0\x91\xa3\x98\0"
      u8"\xf0\x91\xa3\x99\0"
      u8"\xf0\x91\xa3\x9a\0"
      u8"\xf0\x91\xa3\x9b\0"
      u8"\xf0\x91\xa3\x9c\0"
      u8"\xf0\x91\xa3\x9d\0"
      u8"\xf0\x91\xa3\x9e\0"
      u8"\xf0\x91\xa3\x9f\0"
      u8"\xf0\x96\xb9\xa0\0"
      u8"\xf0\x96\xb9\xa1\0"
      u8"\xf0\x96\xb9\xa2\0"
      u8"\xf0\x96\xb9\xa3\0"
      u8"\xf0\x96\xb9\xa4\0"
      u8"\xf0\x96\xb9\xa5\0"
      u8"\xf0\x96\xb9\xa6\0"
      u8"\xf0\x96\xb9\xa7\0"
      u8"\xf0\x96\xb9\xa8\0"
      u8"\xf0\x96\xb9\xa9\0"
      u8"\xf0\x96\xb9\xaa\0"
      u8"\xf0\x96\xb9\xab\0"
      u8"\xf0\x96\xb9\xac\0"
      u8"\xf0\x96\xb9\xad\0"
      u8"\xf0\x96\xb9\xae\0"
      u8"\xf0\x96\xb9\xaf\0"
      u8"\xf0\x96\xb9\xb0\0"
      u8"\xf0\x96\xb9\xb1\0"
      u8"\xf0\x96\xb9\xb2\0"
      u8"\xf0\x96\xb9\xb3\0"
      u8"\xf0\x96\xb9\xb4\0"
      u8"\xf0\x96\xb9\xb5\0"
      u8"\xf0\x96\xb9\xb6\0"
      u8"\xf0\x96\xb9\xb7\0"
      u8"\xf0\x96\xb9\xb8\0"
      u8"\xf0\x96\xb9\xb9\0"
      u8"\xf0\x96\xb9\xba\0"
      u8"\xf0\x96\xb9\xbb\0"
      u8"\xf0\x96\xb9\xbc\0"
      u8"\xf0\x96\xb9\xbd\0"
      u8"\xf0\x96\xb9\xbe\0"
      u8"\xf0\x96\xb9\xbf\0"
      u8"\xf0\x9d\x85\x97\xf0\x9d\x85\xa5\0"
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\0"
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0"
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0"
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb0\0"
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb1\0"
      u8"\xf0\x9d\x85\x98\xf0\x9d\x85\xa5\xf0\x9d\x85\xb2\0"
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\0"
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\0"
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0"
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\xf0\x9d\x85\xae\0"
      u8"\xf0\x9d\x86\xb9\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0"
      u8"\xf0\x9d\x86\xba\xf0\x9d\x85\xa5\xf0\x9d\x85\xaf\0"
      u8"\xc4\xb1\0"
      u8"\xc8\xb7\0"
      u8"\xe2\x88\x87\0"
      u8"\xe2\x88\x82\0"
      u8"\xf0\x9e\xa4\xa2\0"
      u8"\xf0\x9e\xa4\xa3\0"
      u8"\xf0\x9e\xa4\xa4\0"
      u8"\xf0\x9e\xa4\xa5\0"
      u8"\xf0\x9e\xa4\xa6\0"
      u8"\xf0\x9e\xa4\xa7\0"
      u8"\xf0\x9e\xa4\xa8\0"
      u8"\xf0\x9e\xa4\xa9\0"
      u8"\xf0\x9e\xa4\xaa\0"
      u8"\xf0\x9e\xa4\xab\0"
      u8"\xf0\x9e\xa4\xac\0"
      u8"\xf0\x9e\xa4\xad\0"
      u8"\xf0\x9e\xa4\xae\0"
      u8"\xf0\x9e\xa4\xaf\0"
      u8"\xf0\x9e\xa4\xb0\0"
      u8"\xf0\x9e\xa4\xb1\0"
      u8"\xf0\x9e\xa4\xb2\0"
      u8"\xf0\x9e\xa4\xb3\0"
      u8"\xf0\x9e\xa4\xb4\0"
      u8"\xf0\x9e\xa4\xb5\0"
      u8"\xf0\x9e\xa4\xb6\0"
      u8"\xf0\x9e\xa4\xb7\0"
      u8"\xf0\x9e\xa4\xb8\0"
      u8"\xf0\x9e\xa4\xb9\0"
      u8"\xf0\x9e\xa4\xba\0"
      u8"\xf0\x9e\xa4\xbb\0"
      u8"\xf0\x9e\xa4\xbc\0"
      u8"\xf0\x9e\xa4\xbd\0"
      u8"\xf0\x9e\xa4\xbe\0"
      u8"\xf0\x9e\xa4\xbf\0"
      u8"\xf0\x9e\xa5\x80\0"
      u8"\xf0\x9e\xa5\x81\0"
      u8"\xf0\x9e\xa5\x82\0"
      u8"\xf0\x9e\xa5\x83\0"
      u8"\xd9\xae\0"
      u8"\xda\xa1\0"
      u8"\xd9\xaf\0"
      u8"\60\54\0"
      u8"\61\54\0"
      u8"\62\54\0"
      u8"\63\54\0"
      u8"\64\54\0"
      u8"\65\54\0"
      u8"\66\54\0"
      u8"\67\54\0"
      u8"\70\54\0"
      u8"\71\54\0"
      u8"\xe3\x80\x94\x73\xe3\x80\x95\0"
      u8"\x77\x7a\0"
      u8"\x68\x76\0"
      u8"\x73\x64\0"
      u8"\x73\x73\0"
      u8"\x70\x70\x76\0"
      u8"\x77\x63\0"
      u8"\x6d\x63\0"
      u8"\x6d\x64\0"
      u8"\x6d\x72\0"
      u8"\x64\x6a\0"
      u8"\xe3\x81\xbb\xe3\x81\x8b\0"
      u8"\xe3\x82\xb3\xe3\x82\xb3\0"
      u8"\xe5\xad\x97\0"
      u8"\xe5\x8f\x8c\0"
      u8"\xe3\x83\x87\0"
      u8"\xe5\xa4\x9a\0"
      u8"\xe8\xa7\xa3\0"
      u8"\xe4\xba\xa4\0"
      u8"\xe6\x98\xa0\0"
      u8"\xe7\x84\xa1\0"
      u8"\xe5\x89\x8d\0"
      u8"\xe5\xbe\x8c\0"
      u8"\xe5\x86\x8d\0"
      u8"\xe6\x96\xb0\0"
      u8"\xe5\x88\x9d\0"
      u8"\xe7\xb5\x82\0"
      u8"\xe8\xb2\xa9\0"
      u8"\xe5\xa3\xb0\0"
      u8"\xe5\x90\xb9\0"
      u8"\xe6\xbc\x94\0"
      u8"\xe6\x8a\x95\0"
      u8"\xe6\x8d\x95\0"
      u8"\xe9\x81\x8a\0"
      u8"\xe6\x8c\x87\0"
      u8"\xe6\x89\x93\0"
      u8"\xe7\xa6\x81\0"
      u8"\xe7\xa9\xba\0"
      u8"\xe5\x90\x88\0"
      u8"\xe6\xba\x80\0"
      u8"\xe7\x94\xb3\0"
      u8"\xe5\x89\xb2\0"
      u8"\xe5\x96\xb6\0"
      u8"\xe9\x85\x8d\0"
      u8"\xe3\x80\x94\xe6\x9c\xac\xe3\x80\x95\0"
      u8"\xe3\x80\x94\xe4\xb8\x89\xe3\x80\x95\0"
      u8"\xe3\x80\x94\xe4\xba\x8c\xe3\x80\x95\0"
      u8"\xe3\x80\x94\xe5\xae\x89\xe3\x80\x95\0"
      u8"\xe3\x80\x94\xe7\x82\xb9\xe3\x80\x95\0"
      u8"\xe3\x80\x94\xe6\x89\x93\xe3\x80\x95\0"
      u8"\xe3\x80\x94\xe7\x9b\x97\xe3\x80\x95\0"
      u8"\xe3\x80\x94\xe5\x8b\x9d\xe3\x80\x95\0"
      u8"\xe3\x80\x94\xe6\x95\x97\xe3\x80\x95\0"
      u8"\xe5\xbe\x97\0"
      u8"\xe5\x8f\xaf\0"
      u8"\xe4\xb8\xbd\0"
      u8"\xe4\xb8\xb8\0"
      u8"\xe4\xb9\x81\0"
      u8"\xf0\xa0\x84\xa2\0"
      u8"\xe4\xbd\xa0\0"
      u8"\xe4\xbe\xbb\0"
      u8"\xe5\x80\x82\0"
      u8"\xe5\x81\xba\0"
      u8"\xe5\x82\x99\0"
      u8"\xe5\x83\x8f\0"
      u8"\xe3\x92\x9e\0"
      u8"\xf0\xa0\x98\xba\0"
      u8"\xe5\x85\x94\0"
      u8"\xe5\x85\xa4\0"
      u8"\xe5\x85\xb7\0"
      u8"\xf0\xa0\x94\x9c\0"
      u8"\xe3\x92\xb9\0"
      u8"\xe5\x85\xa7\0"
      u8"\xf0\xa0\x95\x8b\0"
      u8"\xe5\x86\x97\0"
      u8"\xe5\x86\xa4\0"
      u8"\xe4\xbb\x8c\0"
      u8"\xe5\x86\xac\0"
      u8"\xf0\xa9\x87\x9f\0"
      u8"\xe5\x88\x83\0"
      u8"\xe3\x93\x9f\0"
      u8"\xe5\x88\xbb\0"
      u8"\xe5\x89\x86\0"
      u8"\xe5\x89\xb7\0"
      u8"\xe3\x94\x95\0"
      u8"\xe5\x8c\x85\0"
      u8"\xe5\x8c\x86\0"
      u8"\xe5\x8d\x89\0"
      u8"\xe5\x8d\x9a\0"
      u8"\xe5\x8d\xb3\0"
      u8"\xe5\x8d\xbd\0"
      u8"\xe5\x8d\xbf\0"
      u8"\xf0\xa0\xa8\xac\0"
      u8"\xe7\x81\xb0\0"
      u8"\xe5\x8f\x8a\0"
      u8"\xe5\x8f\x9f\0"
      u8"\xf0\xa0\xad\xa3\0"
      u8"\xe5\x8f\xab\0"
      u8"\xe5\x8f\xb1\0"
      u8"\xe5\x90\x86\0"
      u8"\xe5\x92\x9e\0"
      u8"\xe5\x90\xb8\0"
      u8"\xe5\x91\x88\0"
      u8"\xe5\x91\xa8\0"
      u8"\xe5\x92\xa2\0"
      u8"\xe5\x93\xb6\0"
      u8"\xe5\x94\x90\0"
      u8"\xe5\x95\x93\0"
      u8"\xe5\x95\xa3\0"
      u8"\xe5\x96\x84\0"
      u8"\xe5\x96\xab\0"
      u8"\xe5\x96\xb3\0"
      u8"\xe5\x97\x82\0"
      u8"\xe5\x9c\x96\0"
      u8"\xe5\x9c\x97\0"
      u8"\xe5\x99\x91\0"
      u8"\xe5\x99\xb4\0"
      u8"\xe5\xa3\xae\0"
      u8"\xe5\x9f\x8e\0"
      u8"\xe5\x9f\xb4\0"
      u8"\xe5\xa0\x8d\0"
      u8"\xe5\x9e\x8b\0"
      u8"\xe5\xa0\xb2\0"
      u8"\xe5\xa0\xb1\0"
      u8"\xe5\xa2\xac\0"
      u8"\xf0\xa1\x93\xa4\0"
      u8"\xe5\xa3\xb2\0"
      u8"\xe5\xa3\xb7\0"
      u8"\xe5\xa4\x86\0"
      u8"\xe5\xa4\xa2\0"
      u8"\xe5\xa5\xa2\0"
      u8"\xf0\xa1\x9a\xa8\0"
      u8"\xf0\xa1\x9b\xaa\0"
      u8"\xe5\xa7\xac\0"
      u8"\xe5\xa8\x9b\0"
      u8"\xe5\xa8\xa7\0"
      u8"\xe5\xa7\x98\0"
      u8"\xe5\xa9\xa6\0"
      u8"\xe3\x9b\xae\0"
      u8"\xe3\x9b\xbc\0"
      u8"\xe5\xac\x88\0"
      u8"\xe5\xac\xbe\0"
      u8"\xf0\xa1\xa7\x88\0"
      u8"\xe5\xaf\x83\0"
      u8"\xe5\xaf\x98\0"
      u8"\xe5\xaf\xb3\0"
      u8"\xf0\xa1\xac\x98\0"
      u8"\xe5\xaf\xbf\0"
      u8"\xe5\xb0\x86\0"
      u8"\xe5\xbd\x93\0"
      u8"\xe3\x9e\x81\0"
      u8"\xe5\xb1\xa0\0"
      u8"\xe5\xb3\x80\0"
      u8"\xe5\xb2\x8d\0"
      u8"\xf0\xa1\xb7\xa4\0"
      u8"\xe5\xb5\x83\0"
      u8"\xf0\xa1\xb7\xa6\0"
      u8"\xe5\xb5\xae\0"
      u8"\xe5\xb5\xab\0"
      u8"\xe5\xb5\xbc\0"
      u8"\xe5\xb7\xa1\0"
      u8"\xe5\xb7\xa2\0"
      u8"\xe3\xa0\xaf\0"
      u8"\xe5\xb7\xbd\0"
      u8"\xe5\xb8\xa8\0"
      u8"\xe5\xb8\xbd\0"
      u8"\xe5\xb9\xa9\0"
      u8"\xe3\xa1\xa2\0"
      u8"\xf0\xa2\x86\x83\0"
      u8"\xe3\xa1\xbc\0"
      u8"\xe5\xba\xb0\0"
      u8"\xe5\xba\xb3\0"
      u8"\xe5\xba\xb6\0"
      u8"\xf0\xaa\x8e\x92\0"
      u8"\xf0\xa2\x8c\xb1\0"
      u8"\xe8\x88\x81\0"
      u8"\xe5\xbc\xa2\0"
      u8"\xe3\xa3\x87\0"
      u8"\xf0\xa3\x8a\xb8\0"
      u8"\xf0\xa6\x87\x9a\0"
      u8"\xe5\xbd\xa2\0"
      u8"\xe5\xbd\xab\0"
      u8"\xe3\xa3\xa3\0"
      u8"\xe5\xbe\x9a\0"
      u8"\xe5\xbf\x8d\0"
      u8"\xe5\xbf\x97\0"
      u8"\xe5\xbf\xb9\0"
      u8"\xe6\x82\x81\0"
      u8"\xe3\xa4\xba\0"
      u8"\xe3\xa4\x9c\0"
      u8"\xf0\xa2\x9b\x94\0"
      u8"\xe6\x83\x87\0"
      u8"\xe6\x85\x88\0"
      u8"\xe6\x85\x8c\0"
      u8"\xe6\x85\xba\0"
      u8"\xe6\x86\xb2\0"
      u8"\xe6\x86\xa4\0"
      u8"\xe6\x86\xaf\0"
      u8"\xe6\x87\x9e\0"
      u8"\xe6\x88\x90\0"
      u8"\xe6\x88\x9b\0"
      u8"\xe6\x89\x9d\0"
      u8"\xe6\x8a\xb1\0"
      u8"\xe6\x8b\x94\0"
      u8"\xe6\x8d\x90\0"
      u8"\xf0\xa2\xac\x8c\0"
      u8"\xe6\x8c\xbd\0"
      u8"\xe6\x8b\xbc\0"
      u8"\xe6\x8d\xa8\0"
      u8"\xe6\x8e\x83\0"
      u8"\xe6\x8f\xa4\0"
      u8"\xf0\xa2\xaf\xb1\0"
      u8"\xe6\x90\xa2\0"
      u8"\xe6\x8f\x85\0"
      u8"\xe6\x8e\xa9\0"
      u8"\xe3\xa8\xae\0"
      u8"\xe6\x91\xa9\0"
      u8"\xe6\x91\xbe\0"
      u8"\xe6\x92\x9d\0"
      u8"\xe6\x91\xb7\0"
      u8"\xe3\xa9\xac\0"
      u8"\xe6\x95\xac\0"
      u8"\xf0\xa3\x80\x8a\0"
      u8"\xe6\x97\xa3\0"
      u8"\xe6\x9b\xb8\0"
      u8"\xe6\x99\x89\0"
      u8"\xe3\xac\x99\0"
      u8"\xe3\xac\x88\0"
      u8"\xe3\xab\xa4\0"
      u8"\xe5\x86\x92\0"
      u8"\xe5\x86\x95\0"
      u8"\xe6\x9c\x80\0"
      u8"\xe6\x9a\x9c\0"
      u8"\xe8\x82\xad\0"
      u8"\xe4\x8f\x99\0"
      u8"\xe6\x9c\xa1\0"
      u8"\xe6\x9d\x9e\0"
      u8"\xe6\x9d\x93\0"
      u8"\xf0\xa3\x8f\x83\0"
      u8"\xe3\xad\x89\0"
      u8"\xe6\x9f\xba\0"
      u8"\xe6\x9e\x85\0"
      u8"\xe6\xa1\x92\0"
      u8"\xf0\xa3\x91\xad\0"
      u8"\xe6\xa2\x8e\0"
      u8"\xe6\xa0\x9f\0"
      u8"\xe6\xa4\x94\0"
      u8"\xe6\xa5\x82\0"
      u8"\xe6\xa6\xa3\0"
      u8"\xe6\xa7\xaa\0"
      u8"\xe6\xaa\xa8\0"
      u8"\xf0\xa3\x9a\xa3\0"
      u8"\xe6\xab\x9b\0"
      u8"\xe3\xb0\x98\0"
      u8"\xe6\xac\xa1\0"
      u8"\xf0\xa3\xa2\xa7\0"
      u8"\xe6\xad\x94\0"
      u8"\xe3\xb1\x8e\0"
      u8"\xe6\xad\xb2\0"
      u8"\xe6\xae\x9f\0"
      u8"\xe6\xae\xbb\0"
      u8"\xf0\xa3\xaa\x8d\0"
      u8"\xf0\xa1\xb4\x8b\0"
      u8"\xf0\xa3\xab\xba\0"
      u8"\xe6\xb1\x8e\0"
      u8"\xf0\xa3\xb2\xbc\0"
      u8"\xe6\xb2\xbf\0"
      u8"\xe6\xb3\x8d\0"
      u8"\xe6\xb1\xa7\0"
      u8"\xe6\xb4\x96\0"
      u8"\xe6\xb4\xbe\0"
      u8"\xe6\xb5\xa9\0"
      u8"\xe6\xb5\xb8\0"
      u8"\xe6\xb6\x85\0"
      u8"\xf0\xa3\xb4\x9e\0"
      u8"\xe6\xb4\xb4\0"
      u8"\xe6\xb8\xaf\0"
      u8"\xe6\xb9\xae\0"
      u8"\xe3\xb4\xb3\0"
      u8"\xe6\xbb\x87\0"
      u8"\xf0\xa3\xbb\x91\0"
      u8"\xe6\xb7\xb9\0"
      u8"\xe6\xbd\xae\0"
      u8"\xf0\xa3\xbd\x9e\0"
      u8"\xf0\xa3\xbe\x8e\0"
      u8"\xe6\xbf\x86\0"
      u8"\xe7\x80\xb9\0"
      u8"\xe7\x80\x9b\0"
      u8"\xe3\xb6\x96\0"
      u8"\xe7\x81\x8a\0"
      u8"\xe7\x81\xbd\0"
      u8"\xe7\x81\xb7\0"
      u8"\xe7\x82\xad\0"
      u8"\xf0\xa0\x94\xa5\0"
      u8"\xe7\x85\x85\0"
      u8"\xf0\xa4\x89\xa3\0"
      u8"\xe7\x86\x9c\0"
      u8"\xf0\xa4\x8e\xab\0"
      u8"\xe7\x88\xa8\0"
      u8"\xe7\x89\x90\0"
      u8"\xf0\xa4\x98\x88\0"
      u8"\xe7\x8a\x80\0"
      u8"\xe7\x8a\x95\0"
      u8"\xf0\xa4\x9c\xb5\0"
      u8"\xf0\xa4\xa0\x94\0"
      u8"\xe7\x8d\xba\0"
      u8"\xe7\x8e\x8b\0"
      u8"\xe3\xba\xac\0"
      u8"\xe7\x8e\xa5\0"
      u8"\xe3\xba\xb8\0"
      u8"\xe7\x91\x87\0"
      u8"\xe7\x91\x9c\0"
      u8"\xe7\x92\x85\0"
      u8"\xe7\x93\x8a\0"
      u8"\xe3\xbc\x9b\0"
      u8"\xe7\x94\xa4\0"
      u8"\xf0\xa4\xb0\xb6\0"
      u8"\xe7\x94\xbe\0"
      u8"\xf0\xa4\xb2\x92\0"
      u8"\xf0\xa2\x86\x9f\0"
      u8"\xe7\x98\x90\0"
      u8"\xf0\xa4\xbe\xa1\0"
      u8"\xf0\xa4\xbe\xb8\0"
      u8"\xf0\xa5\x81\x84\0"
      u8"\xe3\xbf\xbc\0"
      u8"\xe4\x80\x88\0"
      u8"\xf0\xa5\x83\xb3\0"
      u8"\xf0\xa5\x83\xb2\0"
      u8"\xf0\xa5\x84\x99\0"
      u8"\xf0\xa5\x84\xb3\0"
      u8"\xe7\x9c\x9e\0"
      u8"\xe7\x9c\x9f\0"
      u8"\xe7\x9e\x8b\0"
      u8"\xe4\x81\x86\0"
      u8"\xe4\x82\x96\0"
      u8"\xf0\xa5\x90\x9d\0"
      u8"\xe7\xa1\x8e\0"
      u8"\xe4\x83\xa3\0"
      u8"\xf0\xa5\x98\xa6\0"
      u8"\xf0\xa5\x9a\x9a\0"
      u8"\xf0\xa5\x9b\x85\0"
      u8"\xe7\xa7\xab\0"
      u8"\xe4\x84\xaf\0"
      u8"\xe7\xa9\x8a\0"
      u8"\xe7\xa9\x8f\0"
      u8"\xf0\xa5\xa5\xbc\0"
      u8"\xf0\xa5\xaa\xa7\0"
      u8"\xe7\xab\xae\0"
      u8"\xe4\x88\x82\0"
      u8"\xf0\xa5\xae\xab\0"
      u8"\xe7\xaf\x86\0"
      u8"\xe7\xaf\x89\0"
      u8"\xe4\x88\xa7\0"
      u8"\xf0\xa5\xb2\x80\0"
      u8"\xe7\xb3\x92\0"
      u8"\xe4\x8a\xa0\0"
      u8"\xe7\xb3\xa8\0"
      u8"\xe7\xb3\xa3\0"
      u8"\xe7\xb4\x80\0"
      u8"\xf0\xa5\xbe\x86\0"
      u8"\xe7\xb5\xa3\0"
      u8"\xe4\x8c\x81\0"
      u8"\xe7\xb7\x87\0"
      u8"\xe7\xb8\x82\0"
      u8"\xe7\xb9\x85\0"
      u8"\xe4\x8c\xb4\0"
      u8"\xf0\xa6\x88\xa8\0"
      u8"\xf0\xa6\x89\x87\0"
      u8"\xe4\x8d\x99\0"
      u8"\xf0\xa6\x8b\x99\0"
      u8"\xe7\xbd\xba\0"
      u8"\xf0\xa6\x8c\xbe\0"
      u8"\xe7\xbe\x95\0"
      u8"\xe7\xbf\xba\0"
      u8"\xf0\xa6\x93\x9a\0"
      u8"\xf0\xa6\x94\xa3\0"
      u8"\xe8\x81\xa0\0"
      u8"\xf0\xa6\x96\xa8\0"
      u8"\xe8\x81\xb0\0"
      u8"\xf0\xa3\x8d\x9f\0"
      u8"\xe4\x8f\x95\0"
      u8"\xe8\x82\xb2\0"
      u8"\xe8\x84\x83\0"
      u8"\xe4\x90\x8b\0"
      u8"\xe8\x84\xbe\0"
      u8"\xe5\xaa\xb5\0"
      u8"\xf0\xa6\x9e\xa7\0"
      u8"\xf0\xa6\x9e\xb5\0"
      u8"\xf0\xa3\x8e\x93\0"
      u8"\xf0\xa3\x8e\x9c\0"
      u8"\xe8\x88\x84\0"
      u8"\xe8\xbe\x9e\0"
      u8"\xe4\x91\xab\0"
      u8"\xe8\x8a\x91\0"
      u8"\xe8\x8a\x8b\0"
      u8"\xe8\x8a\x9d\0"
      u8"\xe5\x8a\xb3\0"
      u8"\xe8\x8a\xb1\0"
      u8"\xe8\x8a\xb3\0"
      u8"\xe8\x8a\xbd\0"
      u8"\xe8\x8b\xa6\0"
      u8"\xf0\xa6\xac\xbc\0"
      u8"\xe8\x8c\x9d\0"
      u8"\xe8\x8d\xa3\0"
      u8"\xe8\x8e\xad\0"
      u8"\xe8\x8c\xa3\0"
      u8"\xe8\x8e\xbd\0"
      u8"\xe8\x8f\xa7\0"
      u8"\xe8\x8d\x93\0"
      u8"\xe8\x8f\x8a\0"
      u8"\xe8\x8f\x8c\0"
      u8"\xe8\x8f\x9c\0"
      u8"\xf0\xa6\xb0\xb6\0"
      u8"\xf0\xa6\xb5\xab\0"
      u8"\xf0\xa6\xb3\x95\0"
      u8"\xe4\x94\xab\0"
      u8"\xe8\x93\xb1\0"
      u8"\xe8\x93\xb3\0"
      u8"\xe8\x94\x96\0"
      u8"\xf0\xa7\x8f\x8a\0"
      u8"\xe8\x95\xa4\0"
      u8"\xf0\xa6\xbc\xac\0"
      u8"\xe4\x95\x9d\0"
      u8"\xe4\x95\xa1\0"
      u8"\xf0\xa6\xbe\xb1\0"
      u8"\xf0\xa7\x83\x92\0"
      u8"\xe4\x95\xab\0"
      u8"\xe8\x99\x90\0"
      u8"\xe8\x99\xa7\0"
      u8"\xe8\x99\xa9\0"
      u8"\xe8\x9a\xa9\0"
      u8"\xe8\x9a\x88\0"
      u8"\xe8\x9c\x8e\0"
      u8"\xe8\x9b\xa2\0"
      u8"\xe8\x9c\xa8\0"
      u8"\xe8\x9d\xab\0"
      u8"\xe8\x9e\x86\0"
      u8"\xe4\x97\x97\0"
      u8"\xe8\x9f\xa1\0"
      u8"\xe8\xa0\x81\0"
      u8"\xe4\x97\xb9\0"
      u8"\xe8\xa1\xa0\0"
      u8"\xf0\xa7\x99\xa7\0"
      u8"\xe8\xa3\x97\0"
      u8"\xe8\xa3\x9e\0"
      u8"\xe4\x98\xb5\0"
      u8"\xe8\xa3\xba\0"
      u8"\xe3\x92\xbb\0"
      u8"\xf0\xa7\xa2\xae\0"
      u8"\xf0\xa7\xa5\xa6\0"
      u8"\xe4\x9a\xbe\0"
      u8"\xe4\x9b\x87\0"
      u8"\xe8\xaa\xa0\0"
      u8"\xf0\xa7\xb2\xa8\0"
      u8"\xe8\xb2\xab\0"
      u8"\xe8\xb3\x81\0"
      u8"\xe8\xb4\x9b\0"
      u8"\xe8\xb5\xb7\0"
      u8"\xf0\xa7\xbc\xaf\0"
      u8"\xf0\xa0\xa0\x84\0"
      u8"\xe8\xb7\x8b\0"
      u8"\xe8\xb6\xbc\0"
      u8"\xe8\xb7\xb0\0"
      u8"\xf0\xa0\xa3\x9e\0"
      u8"\xe8\xbb\x94\0"
      u8"\xf0\xa8\x97\x92\0"
      u8"\xf0\xa8\x97\xad\0"
      u8"\xe9\x82\x94\0"
      u8"\xe9\x83\xb1\0"
      u8"\xe9\x84\x91\0"
      u8"\xf0\xa8\x9c\xae\0"
      u8"\xe9\x84\x9b\0"
      u8"\xe9\x88\xb8\0"
      u8"\xe9\x8b\x97\0"
      u8"\xe9\x8b\x98\0"
      u8"\xe9\x89\xbc\0"
      u8"\xe9\x8f\xb9\0"
      u8"\xe9\x90\x95\0"
      u8"\xf0\xa8\xaf\xba\0"
      u8"\xe9\x96\x8b\0"
      u8"\xe4\xa6\x95\0"
      u8"\xe9\x96\xb7\0"
      u8"\xf0\xa8\xb5\xb7\0"
      u8"\xe4\xa7\xa6\0"
      u8"\xe9\x9b\x83\0"
      u8"\xe5\xb6\xb2\0"
      u8"\xe9\x9c\xa3\0"
      u8"\xf0\xa9\x85\x85\0"
      u8"\xf0\xa9\x88\x9a\0"
      u8"\xe4\xa9\xae\0"
      u8"\xe4\xa9\xb6\0"
      u8"\xe9\x9f\xa0\0"
      u8"\xf0\xa9\x90\x8a\0"
      u8"\xe4\xaa\xb2\0"
      u8"\xf0\xa9\x92\x96\0"
      u8"\xe9\xa0\xa9\0"
      u8"\xf0\xa9\x96\xb6\0"
      u8"\xe9\xa3\xa2\0"
      u8"\xe4\xac\xb3\0"
      u8"\xe9\xa4\xa9\0"
      u8"\xe9\xa6\xa7\0"
      u8"\xe9\xa7\x82\0"
      u8"\xe9\xa7\xbe\0"
      u8"\xe4\xaf\x8e\0"
      u8"\xf0\xa9\xac\xb0\0"
      u8"\xe9\xb1\x80\0"
      u8"\xe9\xb3\xbd\0"
      u8"\xe4\xb3\x8e\0"
      u8"\xe4\xb3\xad\0"
      u8"\xe9\xb5\xa7\0"
      u8"\xf0\xaa\x83\x8e\0"
      u8"\xe4\xb3\xb8\0"
      u8"\xf0\xaa\x84\x85\0"
      u8"\xf0\xaa\x88\x8e\0"
      u8"\xf0\xaa\x8a\x91\0"
      u8"\xe4\xb5\x96\0"
      u8"\xe9\xbb\xbe\0"
      u8"\xe9\xbc\x85\0"
      u8"\xe9\xbc\x8f\0"
      u8"\xe9\xbc\x96\0"
      u8"\xf0\xaa\x98\x80\0",
      17246UL // String Length
    };



} // namespace webpp::uri::idna::details

#endif // WEBPP_URI_IDNA_MAPPING_TABLE_HPP
