// Created by moisrex on 8/2/24.

#ifndef WEBPP_UTILS_BITS_HPP
#define WEBPP_UTILS_BITS_HPP

#include <array>
#include <cstdint>

namespace webpp {

    namespace details {
        /// From https://graphics.stanford.edu/~seander/bithacks.html#InterleaveTableLookup
        static constexpr std::array<std::uint16_t, 256U> morton_table_256{
          0x0000U, 0x0001U, 0x0004U, 0x0005U, 0x0010U, 0x0011U, 0x0014U, 0x0015U, 0x0040U, 0x0041U, 0x0044U,
          0x0045U, 0x0050U, 0x0051U, 0x0054U, 0x0055U, 0x0100U, 0x0101U, 0x0104U, 0x0105U, 0x0110U, 0x0111U,
          0x0114U, 0x0115U, 0x0140U, 0x0141U, 0x0144U, 0x0145U, 0x0150U, 0x0151U, 0x0154U, 0x0155U, 0x0400U,
          0x0401U, 0x0404U, 0x0405U, 0x0410U, 0x0411U, 0x0414U, 0x0415U, 0x0440U, 0x0441U, 0x0444U, 0x0445U,
          0x0450U, 0x0451U, 0x0454U, 0x0455U, 0x0500U, 0x0501U, 0x0504U, 0x0505U, 0x0510U, 0x0511U, 0x0514U,
          0x0515U, 0x0540U, 0x0541U, 0x0544U, 0x0545U, 0x0550U, 0x0551U, 0x0554U, 0x0555U, 0x1000U, 0x1001U,
          0x1004U, 0x1005U, 0x1010U, 0x1011U, 0x1014U, 0x1015U, 0x1040U, 0x1041U, 0x1044U, 0x1045U, 0x1050U,
          0x1051U, 0x1054U, 0x1055U, 0x1100U, 0x1101U, 0x1104U, 0x1105U, 0x1110U, 0x1111U, 0x1114U, 0x1115U,
          0x1140U, 0x1141U, 0x1144U, 0x1145U, 0x1150U, 0x1151U, 0x1154U, 0x1155U, 0x1400U, 0x1401U, 0x1404U,
          0x1405U, 0x1410U, 0x1411U, 0x1414U, 0x1415U, 0x1440U, 0x1441U, 0x1444U, 0x1445U, 0x1450U, 0x1451U,
          0x1454U, 0x1455U, 0x1500U, 0x1501U, 0x1504U, 0x1505U, 0x1510U, 0x1511U, 0x1514U, 0x1515U, 0x1540U,
          0x1541U, 0x1544U, 0x1545U, 0x1550U, 0x1551U, 0x1554U, 0x1555U, 0x4000U, 0x4001U, 0x4004U, 0x4005U,
          0x4010U, 0x4011U, 0x4014U, 0x4015U, 0x4040U, 0x4041U, 0x4044U, 0x4045U, 0x4050U, 0x4051U, 0x4054U,
          0x4055U, 0x4100U, 0x4101U, 0x4104U, 0x4105U, 0x4110U, 0x4111U, 0x4114U, 0x4115U, 0x4140U, 0x4141U,
          0x4144U, 0x4145U, 0x4150U, 0x4151U, 0x4154U, 0x4155U, 0x4400U, 0x4401U, 0x4404U, 0x4405U, 0x4410U,
          0x4411U, 0x4414U, 0x4415U, 0x4440U, 0x4441U, 0x4444U, 0x4445U, 0x4450U, 0x4451U, 0x4454U, 0x4455U,
          0x4500U, 0x4501U, 0x4504U, 0x4505U, 0x4510U, 0x4511U, 0x4514U, 0x4515U, 0x4540U, 0x4541U, 0x4544U,
          0x4545U, 0x4550U, 0x4551U, 0x4554U, 0x4555U, 0x5000U, 0x5001U, 0x5004U, 0x5005U, 0x5010U, 0x5011U,
          0x5014U, 0x5015U, 0x5040U, 0x5041U, 0x5044U, 0x5045U, 0x5050U, 0x5051U, 0x5054U, 0x5055U, 0x5100U,
          0x5101U, 0x5104U, 0x5105U, 0x5110U, 0x5111U, 0x5114U, 0x5115U, 0x5140U, 0x5141U, 0x5144U, 0x5145U,
          0x5150U, 0x5151U, 0x5154U, 0x5155U, 0x5400U, 0x5401U, 0x5404U, 0x5405U, 0x5410U, 0x5411U, 0x5414U,
          0x5415U, 0x5440U, 0x5441U, 0x5444U, 0x5445U, 0x5450U, 0x5451U, 0x5454U, 0x5455U, 0x5500U, 0x5501U,
          0x5504U, 0x5505U, 0x5510U, 0x5511U, 0x5514U, 0x5515U, 0x5540U, 0x5541U, 0x5544U, 0x5545U, 0x5550U,
          0x5551U, 0x5554U, 0x5555U};
    } // namespace details

    /// Interleave bits
    /// From https://graphics.stanford.edu/~seander/bithacks.html#InterleaveTableLookup
    [[nodiscard]] static constexpr std::uint32_t interleave_bits(
      std::uint16_t const lhs,
      std::uint16_t const rhs) noexcept {
        using details::morton_table_256;
        // NOLINTBEGIN(*-magic-numbers, *-pro-bounds-constant-array-index)
        return static_cast<std::uint32_t>(morton_table_256[rhs >> 8U]) << 17U |
               static_cast<std::uint32_t>(morton_table_256[lhs >> 8U]) << 16U |
               static_cast<std::uint32_t>(morton_table_256[rhs & 0xFFU]) << 1U |
               static_cast<std::uint32_t>(morton_table_256[lhs & 0xFFU]);
        // NOLINTEND(*-magic-numbers, *-pro-bounds-constant-array-index)
    }

} // namespace webpp

#endif // WEBPP_UTILS_BITS_HPP
