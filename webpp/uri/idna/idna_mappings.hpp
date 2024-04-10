// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

#include "../../std/algorithm.hpp"
#include "../../std/string.hpp"
#include "../../std/type_traits.hpp"
#include "./details/idna_mapping_table.hpp"

namespace webpp::uri::idna {

    namespace details {

        using map_table_byte_type         = typename decltype(idna_mapping_table)::value_type;
        using idna_mapping_table_iterator = typename decltype(idna_mapping_table)::const_iterator;

        static constexpr map_table_byte_type disallowed_mask = 0xFF00'0000;
        static constexpr map_table_byte_type mapped_mask     = 0x8000'0000;

        // We have 7 bits, but 0xFF would equal to disallowedMask
        static constexpr map_table_byte_type length_limit = 126;

    } // namespace details

      /**
       * A table for list of mapped characters
       *
       * First Byte Rules:
       *   - [1bit = 1] + [7bit = length] + [24bit = start]
       *   - start  = is the start of the range
       *   - length = the length of the range
       *   - (byte & 0x80000000 == 0x80000000) meaning far left bit is 0b1
       *   - if it starts with 0xFF000000, then it's a disabled range
       *
       * Bytes after the First Byte:
       *   - Their far left bit will never be 0b1,
       *     that means (byte & 0x80000000 == 0b0)
       *   - You have to continue reading everything after each byte, until
       *     you reach an element that it's far left bit is one.
       *
       * Actions:
       *   - Mapped: [[1bit = 1] [7bits = length] [24bits = range-start]]
       *             + ... N number of characters you should map to ...;
       *
       *             it's first element starts with a 0x80000000, and
       *             anything after that is considered as what you need to
       *             map the range to.
       *
       *   - Ignored: It's equivalent of mapping to empty string
       *
       *   - Disallowed: [[8bits = 1] [24bits = range-start]]
       *                +[ [32bits = range-end] ]
       *             Disallowed is really contains range start and range end.
       */
    template <typename CharT>
    [[nodiscard]] static constexpr details::idna_mapping_table_iterator find_mapping_byte(
      CharT const inp_ch) {
        using details::disallowed_mask;
        using details::idna_mapping_table;
        using details::map_table_byte_type;
        using details::mapped_mask;

        map_table_byte_type const byte = inp_ch | mapped_mask;
        auto const                pos  = stl::lower_bound(
          idna_mapping_table.begin(),
          idna_mapping_table.end(),
          byte,
          [](map_table_byte_type const lhs, map_table_byte_type const rhs) constexpr noexcept {
              return (lhs | disallowed_mask) < (rhs | disallowed_mask);
          });

        return pos;
    }

    template <istl::String OutStrT, typename Iter>
    [[nodiscard]] static constexpr bool map(Iter beg, Iter end, OutStrT &out) {
        using details::idna_reference_table;
        using ref_table_byte_type = typename decltype(idna_reference_table)::value_type;

        auto pos = beg;
        for (; pos != end; ++pos) {
            // todo: handle utf-8 characters:
            stl::size_t const byte_index   = static_cast<stl::size_t>(*pos) / sizeof(ref_table_byte_type);
            unsigned const    rem_index    = static_cast<stl::size_t>(*pos) % sizeof(ref_table_byte_type);
            ref_table_byte_type const byte = idna_reference_table[byte_index];
            bool const                is_active = (byte & rem_index) != 0;

            if (is_active) {
                // now we should look at the mapping table

                continue;
            }

            // it's a valid character
            out += *pos;
        }
        return true;
    }

} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_MAPPINGS_HPP
