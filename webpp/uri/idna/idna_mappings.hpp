// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

#include "../../std/string.hpp"
#include "../../std/type_traits.hpp"
#include "../../unicode/unicode.hpp"
#include "./details/idna_mapping_table.hpp"

namespace webpp::uri::idna {

    namespace details {

        using map_table_byte_type         = typename decltype(idna_mapping_table)::value_type;
        using idna_mapping_table_iterator = typename decltype(idna_mapping_table)::const_iterator;

        static constexpr map_table_byte_type disallowed_mask = 0xFF00'0000;
        static constexpr map_table_byte_type mapped_mask     = 0x8000'0000;
        static constexpr map_table_byte_type sequenced_mask  = 0x7F00'0000; // 0b0111'1111'0000'0...

        // this is in order to check if the code point is a "sequenced code point" or not
        static constexpr map_table_byte_type sequenced_check_mask = sequenced_mask | mapped_mask;

        // We have 7 bits, but 0xFF would equal to disallowed_mask
        static constexpr map_table_byte_type length_limit = 126;

    } // namespace details

      /**
       * A table for list of mapped characters
       *
       * First Code Point Rules:
       *   - [1bit = 1] + [7bit = length] + [24bit = start]
       *   - start  = is the start of the range
       *   - length = the length of the range
       *   - (byte & 0x80000000 == 0x80000000) meaning far left bit is 0b1
       *   - if it starts with 0xFF000000, then it's a disabled range
       *
       * Bytes after the First Code Point:
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
       *   - Sequenced Mapped:
       *             [[1bit = 1] [7bits = length] [24bits = range-start]]
       *           + [[1bit = 0] [7bits = 1]      [24bits = mapped-value]];
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

        auto const asked_char = static_cast<map_table_byte_type>(inp_ch);

        // anything bigger than the last element is disallowed
        if ((asked_char & disallowed_mask) != 0U) {
            // last element:
            return idna_mapping_table.begin() + (idna_mapping_table.size() - 1);
        }

        map_table_byte_type const element = asked_char | disallowed_mask;
        auto                      length  = idna_mapping_table.size();
        auto                      chosen  = idna_mapping_table.begin(); // NOLINT(*-qualified-auto)

        // This is almost the same thing as std::partition_point and std::lower_bound, but with modifications.
        // ([element] [mapped] [mapped] [mapped], ...)
        //     ^      --------------------------
        //     |                 ^
        //     |                 |
        //     |     Should be ignored during binary search
        //     |
        //   first-code-point: this is the element we should find and compare against
        // Binary Search:
        for (;;) {
            length      >>= 1U;     // devided by 2
            auto middle   = chosen; // NOLINT(*-qualified-auto)
            std::advance(middle, length);

            // non-first-characters are ignored here
            decltype(length) remaining = 0;
            while ((*middle & mapped_mask) == 0U) {
                --middle;
                ++remaining;
            }

            // Why we need linear search in the middle of a binary search:
            //    if this last one is the result,
            //    dots are ignored characters, and
            //    '1' is the code-point we have to check.
            //     /
            // 1..X
            //  ^ length >>= 1
            // ^ length += remaining
            //  ^ length >>= 1
            // ^ length += remaining
            //  ^ length >>= 1
            //  Then it's a loop.
            if (chosen == middle) {
                stl::advance(middle, remaining);
                ++middle;

                // Linear Search (we could use this little algorithm instead of the whole thing,
                //                but it wouldn't be performant):
                for (; middle != idna_mapping_table.end(); ++middle) {
                    if ((*middle & mapped_mask) == 0U) {
                        continue;
                    }
                    if ((*middle | disallowed_mask) > element) {
                        break;
                    }
                    chosen = middle;
                }
                break;
            }
            if (auto const cur_element = *middle | disallowed_mask; cur_element <= element) {
                // let's look into the upper half now
                chosen  = middle;
                length += remaining;
            }
        }

        return chosen;
    }

    /**
     * Perform the mapping for a single character
     * @returns false if the character is not allowed
     */
    template <bool UseSTD3ASCIIRules = false, typename CharT, istl::String OutStrT>
    [[nodiscard]] static constexpr bool perform_mapping(CharT const inp_ch, OutStrT& out) {
        using details::disallowed_mask;
        using details::map_table_byte_type;
        using details::mapped_mask;

        // todo: uncomment this
        // static_assert(sizeof(CharT) >= 16U, // NOLINT(*-magic-numbers)
        //               "UTF-8 characters should not be used here, "
        //               "should first be converted to a 32bit character.");

        auto const cur_char   = static_cast<map_table_byte_type>(inp_ch);
        auto const pos        = find_mapping_byte(inp_ch);
        auto const first_code_point = *pos;
        auto const length           = (first_code_point & ~mapped_mask) >> 24U;

        if ([[maybe_unused]] bool const is_disallowed = length == disallowed_mask) {
            auto const range_start = first_code_point & ~disallowed_mask;
            auto const range_end   = *stl::next(pos);

            // we don't realy need to check this because the all the senarios that this would not be true, the
            // user has passed an invalid character anyway.
            assert(cur_char >= range_start && cur_char <= range_end);
            return false;
        }

        // mapping, or ignoring a character:
        {
            auto const range_start          = first_code_point & ~disallowed_mask;
            auto const range_end            = range_start + length;
            auto const next_pos             = stl::next(pos);
            bool const is_sequenced_mapping = *next_pos & details::sequenced_check_mask;

            // we don't really need to check the range because all the senarios that this would either be
            // caught by the "reference table", or it's already detected that it's an invalid character.
            assert(cur_char >= range_start && cur_char <= range_end);

            if (is_sequenced_mapping) {
                // It's a sequenced mapping
                auto const diff       = cur_char - range_start;
                auto const code_point  = (*next_pos & ~disallowed_mask) + diff;
                // todo: conver to utf-8
                out                   += static_cast<CharT>(code_point);
            } else {
                // loop until we find the next "first-code-point"
                // we don't need to check the length of the array, there's a valid `first-code-point`
                // character at the end of the table intentionally inserted for this purpose.
                for (auto cur_pos = next_pos; (*cur_pos & mapped_mask) == 0; ++cur_pos) {
                    auto const code_point  = *cur_pos & ~disallowed_mask;
                    // todo: conver to utf-8
                    // out.append(*cur_pos);
                    out                   += static_cast<CharT>(code_point);
                }
            }
        }

        // todo: handle STD3

        return true;
    }

    /**
     * Mapping Step of the IDNA Proccessing
     * UTS #46: https://www.unicode.org/reports/tr46/#ProcessingStepMap
     */
    template <bool UseSTD3ASCIIRules = false, istl::String OutStrT, typename Iter>
    [[nodiscard]] static constexpr bool map(Iter beg, Iter end, OutStrT& out) {
        using details::idna_reference_table;
        using ref_table_byte_type = typename decltype(idna_reference_table)::value_type;

        auto pos = beg;
        for (; pos != end; ++pos) {
            // todo: handle utf-8 characters:
            stl::size_t const byte_index   = static_cast<stl::size_t>(*pos) / sizeof(ref_table_byte_type);
            unsigned const    rem_index    = static_cast<stl::size_t>(*pos) % sizeof(ref_table_byte_type);
            ref_table_byte_type const byte = idna_reference_table[byte_index];

            if ([[maybe_unused]] bool const should_map = (byte & rem_index) != 0) {
                // now we should look at the mapping table
                if (!perform_mapping<UseSTD3ASCIIRules>(*pos)) {
                    return false;
                }
                continue;
            }

            // it's a valid character
            out.append(*pos);
        }
        return true;
    }

} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_MAPPINGS_HPP
