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

    template <StringLike StrT, typename InputIt>
    constexpr void assign(StrT& str, InputIt beg, InputIt end) noexcept(StringView<StrT>) {
        if constexpr (StringView<StrT>) {
            str = StrT{beg, end};
        } else {
            str.assign(beg, end);
        }
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

    template <StringLike StrT>
    constexpr void clear(StrT& str) noexcept {
        if constexpr (StringView<StrT>) {
            str = StrT{};
        } else {
            str.clear(); // doesn't deallocate actually, so it's nothrow
        }
    }

} // namespace webpp::istl

#endif // WEBPP_STRING_LIKE_HPP
