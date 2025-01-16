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
#include <iterator>

namespace webpp::uri::idna {

    template <unicode::UTF32 CharT = char32_t>
    [[nodiscard]] static constexpr stl::uint16_t status_of(CharT const code_point) noexcept {
        using details::batch_bit_count;
        using details::batch_mask;
        using details::idna_ref_blocks;
        using details::idna_ref_bools;
        using details::idna_refs;
        using details::not_mapped;

        if (code_point >= static_cast<CharT>(details::last_disallowed)) {
            return details::disallowed;
        }
        // NOLINTBEGIN(*-pro-bounds-constant-array-index)
        auto const ref     = idna_refs[code_point >> batch_bit_count];
        auto const ref_ptr = ref & static_cast<stl::uint16_t>(~details::table_pick_mask);
        if (ref_ptr != ref) {
            // looking at the idna_ref_bools table
            constexpr auto      pack_size = sizeof(typename decltype(idna_ref_bools)::value_type) * CHAR_BIT;
            stl::uint16_t const status_bit =
              0b1U & (idna_ref_bools[ref_ptr / pack_size] >> (pack_size - (ref_ptr % pack_size)));
            return details::disallowed | status_bit; // if it's 1, it'll become valid, otherwise it stays
                                                     // disallowed
        }

        return idna_ref_blocks[ref][code_point & batch_mask];
        // NOLINTEND(*-pro-bounds-constant-array-index)
    }

    /**
     * Perform the mapping for a single character
     * @returns false if the code point is not allowed to be in a URL
     */
    template <unicode::UTF32 CharT = char32_t, istl::String OutStrT = stl::u8string>
    static constexpr bool map(CharT const code_point, OutStrT& out) {
        using details::disallowed;
        using details::idna_mappings;
        using details::valid;
        auto const pos = status_of(code_point);

        // ignored code points are mapped to nothing, so no special code is needed
        switch (pos) {
            case disallowed: return false; // it's not allowed
            case valid:
                // todo: you can optimize this, we don't have to re-convert the code point
                unicode::unchecked::append(out, code_point);
                return true;

            default: { // mapped
                auto ptr = idna_mappings.begin() + pos;
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

    template <istl::String OutStrT = stl::u8string, unicode::UTF32 CharT = char32_t, typename... Args>
    static constexpr OutStrT mapped(CharT const code_point, Args&&... args) {
        OutStrT out{stl::forward<Args>(args)...};
        map(code_point, out);
        return out;
    }

    using idna_mappings_string_type = decltype(details::idna_mappings);

    [[nodiscard]] static constexpr idna_mappings_string_type::iterator mapped_begin(
      idna_mappings_string_type const& src) noexcept {
        using details::disallowed;
        using details::valid;
        auto const* beg        = stl::begin(src);
        auto const* end        = stl::end(src);
        auto const  code_point = unicode::checked::next_code_point(beg, end);

        // ignored code points are mapped to nothing, so no special code is needed
        switch (auto const pos = status_of(code_point)) {
            case disallowed: return end;
            case valid: return beg;
            default: return details::idna_mappings.begin() + pos; // mapped
        }
    }

    /**
     * Mapping Step of the IDNA Processing
     * UTS #46: https://www.unicode.org/reports/tr46/#ProcessingStepMap
     */
    template <istl::String OutStrT, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr bool map(Iter beg, Iter end, OutStrT& out) {
        using enum unicode::checked::error_handling;
        using unicode::checked::next_code_point;


        for (auto pos = beg; pos != end;) {
            auto const code_point = next_code_point<return_negated_char, char32_t, Iter>(pos, end);
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

    template <istl::String OutStrT, istl::StringViewifiable InpStrT>
    [[nodiscard]] static constexpr bool map(InpStrT&& src, OutStrT& out) {
        auto const src_view = istl::string_viewify(stl::forward<InpStrT>(src));
        using iterator      = typename decltype(src_view)::iterator;
        return map<OutStrT, iterator>(stl::begin(src_view), stl::end(src_view), out);
    }

} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_MAPPINGS_HPP
