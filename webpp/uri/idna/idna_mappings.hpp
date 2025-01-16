// Created by moisrex on 4/9/24.

#ifndef WEBPP_URI_IDNA_MAPPINGS_HPP
#define WEBPP_URI_IDNA_MAPPINGS_HPP

#include "../../std/string.hpp"
#include "../../std/string_view.hpp"
#include "../../unicode/unicode.hpp"
#include "./details/idna_mapping_table.hpp"

#include <cassert>
#include <climits>
#include <cstdint>

namespace webpp::uri::idna {


    struct mapping_status {
        using type = std::uint16_t;

        static constexpr std::uint8_t NOT_MAPPED = 0b1U << 7U;
        static constexpr std::uint8_t MAPPED     = 0b0U << 7U; // `ignored` is mapped too
        static constexpr std::uint8_t VALID      = 0b1U << 7U | 0b001U;
        static constexpr std::uint8_t DISALLOWED = 0b1U << 7U | 0b010U;
    };

    template <unicode::UTF32 CharT>
    [[nodiscard]] static constexpr mapping_status::type status_of(CharT const code_point) noexcept {
        using details::idna_ref_blocks;
        using details::idna_refs;

        auto const [block_pos, base_pos] = idna_refs[code_point >> 8U];
        auto const map_pos =
          static_cast<mapping_status::type>(idna_ref_blocks[block_pos][code_point & 0xFFU]);
        auto const is_mapped = map_pos < mapping_status::NOT_MAPPED;

        if (!is_mapped) {
            return map_pos; // either VALID or DISALLOWED
        }

        return (base_pos << 8U) | map_pos;
    }

    /**
     * Perform the mapping for a single character
     * @returns false if the code point is not allowed to be in a URL
     */
    template <unicode::UTF32 CharT, istl::String OutStrT>
    [[nodiscard]] static constexpr bool map(CharT const code_point, OutStrT& out) {
        using details::idna_mappings;
        auto const pos = status_of(code_point);

        // ignored code points are mapped to nothing, so no special code is needed
        switch (pos) {
            case mapping_status::DISALLOWED: return false; // it's not allowed
            case mapping_status::VALID:
                // todo: you can optimize this, we don't have to re-convert the code point
                unicode::unchecked::append(out, code_point);
                return true;

            default: { // mapped
                auto ptr = idna_mappings + pos;
                if constexpr (unicode::UTF8String<OutStrT>) {
                    for (; *ptr != u8'\0'; ++ptr) {
                        unicode::unchecked::append(out, *ptr);
                    }
                } else {
                    while (*ptr != u8'\0') {
                        auto const cur_cp = unicode::unchecked::next_code_point(ptr);
                        unicode::unchecked::append(out, cur_cp);
                    }
                }
                return true;
            }
        }
    }

    /**
     * Mapping Step of the IDNA Processing
     * UTS #46: https://www.unicode.org/reports/tr46/#ProcessingStepMap
     */
    template <bool UseSTD3ASCIIRules = false, istl::String OutStrT, typename Iter>
    [[nodiscard]] static constexpr bool map(Iter beg, Iter end, OutStrT& out) {
        using enum unicode::checked::error_handling;


        for (auto pos = beg; pos != end;) {
            auto const code_point = unicode::checked::next_code_point<return_negated_char>(pos, end);
            if (static_cast<stl::int32_t>(code_point) < 0) [[unlikely]] {
                unicode::unchecked::append(out, -code_point);
                continue;
            }

            if (!map(code_point, out)) {
                return false;
            }
        }
        return true;
    }

    template <bool UseSTD3ASCIIRules = false, istl::String OutStrT, istl::StringViewifiable InpStrT>
    [[nodiscard]] static constexpr bool map(InpStrT&& src, OutStrT& out) {
        auto const src_view = istl::string_viewify(stl::forward<InpStrT>(src));
        using iterator      = typename decltype(src_view)::iterator;
        return map<UseSTD3ASCIIRules, OutStrT, iterator>(stl::begin(src_view), stl::end(src_view), out);
    }

} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_MAPPINGS_HPP
