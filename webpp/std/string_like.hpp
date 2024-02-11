// Created by moisrex on 10/9/23.

#ifndef WEBPP_STRING_LIKE_HPP
#define WEBPP_STRING_LIKE_HPP

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

    template <StringLike StrT, StringLike StrTInput>
    constexpr void append(StrT& out, StrTInput const& inp) noexcept(StringView<StrT>) {
        if constexpr (StringView<StrT>) {
            out = inp;
        } else {
            out.append(inp);
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
