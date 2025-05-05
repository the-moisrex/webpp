// Created by moisrex on 10/9/23.

#ifndef WEBPP_STRING_LIKE_HPP
#define WEBPP_STRING_LIKE_HPP

#include "./iterator.hpp"
#include "./string.hpp"
#include "./string_view.hpp"

namespace webpp::istl {

    template <typename T>
    concept StringLike = String<T> || StringView<T>;

    template <typename T>
    concept ModifiableString = String<T>;

    /// No need to use StringLike; this can be used for other types as well.
    template <typename StrT, typename InputIt>
    constexpr void assign(StrT& str, InputIt beg, InputIt end) noexcept(requires {
        {
            str.assign(beg, end)
        } noexcept;
    }) {
        str.assign(beg, end);
    }

    template <StringView StrT, typename InputIt>
    constexpr void assign(StrT& str, InputIt beg, InputIt end) noexcept {
        str = StrT{beg, end};
    }

    /**
     * The out can be:
     *   - an iterator,
     *   - a  string,
     *   - a  string view
     *   - a  vector of compatible values
     *
     * @tparam StrOrIter Can be a string/string-view/iterator/vector<CharT>/...
     */
    template <typename StrOrIter, typename StrTInput>
    constexpr void append(StrOrIter& out, StrTInput&& inp) noexcept(StringView<StrOrIter>) {
        if constexpr (StringView<StrOrIter>) {
            out = stl::forward<StrTInput>(inp);
        } else if constexpr (stl::output_iterator<StrOrIter, StrTInput>) {
            *out++ = stl::forward<StrTInput>(inp);
        } else if constexpr (String<StrOrIter> && CharType<StrTInput>) {
            out += stl::forward<StrTInput>(inp);
        } else {
            out.append(stl::forward<StrTInput>(inp));
        }
    }

    /// No need to use StringLike constraint on the type; we want it to be used for other types as well.
    template <typename StrT>
    constexpr void clear(StrT& str) noexcept {
        if constexpr (StringView<StrT>) {
            str = StrT{};
        } else {
            str.clear(); // doesn't deallocate, so it's nothrow
        }
    }

} // namespace webpp::istl

#endif // WEBPP_STRING_LIKE_HPP
