
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                generate_composition_tables.mjs
 *   Unicode UCD Database Creation Date: 2023-08-28
 *   This file's generation date:        Tue, 27 Aug 2024 21:47:04 GMT
 *   Unicode Version:                    15.1.0
 *   Total Table sizes in this file:
 *       - in bits:       0
 *       - in bytes:      0 B
 *       - in KibiBytes:  0 KiB
 *   Some other implementations' total table size was 45.2 KiB;
 *   So I saved 46 KiB and a better a locality.
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

#ifndef WEBPP_UNICODE_COMPOSITION_TABLES_HPP
#define WEBPP_UNICODE_COMPOSITION_TABLES_HPP

#include "../../utils/bits.hpp"

#include <array>
#include <cstdint>
#include <string_view>

namespace webpp::unicode::details {



    struct composition {
        static constexpr std::uint16_t chunk_mask  = 0x3FU;
        static constexpr std::size_t   chunk_size  = 64U;
        static constexpr std::uint8_t  chunk_shift = 6U;


        static constexpr std::size_t cp1_mask                = 0x0U;
        static constexpr std::size_t cp2_mask                = 0x0U;
        static constexpr std::size_t interleaved_magic_mask  = 0x0U;
        static constexpr std::size_t interleaved_magic_compl = 0x0U;
        static constexpr std::size_t last_mapped_bucket      = 0x2710U;

        /// This is a magical formula that absolutely does not make sense, but it works because math is
        /// magical. This will merge the 2 code points into one single value that then can be used to get the
        /// position of the values in the values table.
        template <typename CharT = char32_t>
        [[nodiscard]] static constexpr std::size_t magic_merge(CharT cp1, CharT cp2) noexcept {
            cp1 &= cp1_mask;
            cp2 &= cp2_mask;
            auto const intlvd =
              webpp::interleave_bits(static_cast<std::uint16_t>(cp1), static_cast<std::uint16_t>(cp2));
            auto const merged = (intlvd & interleaved_magic_mask) - interleaved_magic_compl;
            return (((merged >> chunk_shift) % last_mapped_bucket) << chunk_shift) | (merged & chunk_mask);
        }
    };


} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_COMPOSITION_TABLES_HPP
