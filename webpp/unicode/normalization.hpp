// Created by moisrex on 4/15/24.

#ifndef WEBPP_UNICODE_NORMALIZATION_HPP
#define WEBPP_UNICODE_NORMALIZATION_HPP

#include "../std/iterator.hpp"
#include "../std/type_traits.hpp"

namespace webpp::unicode {


    /**
     * Is a normalized unicode string
     * UTX #15: https://www.unicode.org/reports/tr15/tr15-54.html
     *
     * 	When implementations keep strings in a normalized form, they can be assured that equivalent strings
     * have a unique binary representation
     *
     * @tparam Iter iterator
     * @tparam EIter end iterator
     * @param start start position
     * @param end if you don't pass it, it'll look for one character
     * @return true if it's normalized unicode
     */
    template <typename Iter, typename EIter = istl::nothing_type>
    [[nodiscard]] static constexpr bool is_normalized(Iter start, EIter end = {}) noexcept {
        using char_type = typename stl::iterator_traits<Iter>::value_type;
        return false;
    }

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_NORMALIZATION_HPP
