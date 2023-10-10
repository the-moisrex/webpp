// Created by moisrex on 10/9/23.

#ifndef WEBPP_STRING_LIKE_HPP
#define WEBPP_STRING_LIKE_HPP

#include "./string.hpp"
#include "./string_view.hpp"

namespace webpp::istl {

    template <typename T>
    concept StringLike = istl::String<T> || istl::StringView<T>;

    template <typename T>
    concept ModifiableString = istl::String<T>;


    template <istl::StringLike StrT>
    constexpr void assign(StrT&                        str,
                          typename StrT::const_pointer beg,
                          typename StrT::const_pointer end) noexcept(istl::StringView<StrT>) {
        if constexpr (istl::StringView<StrT>) {
            str = StrT{beg, end - beg};
        } else {
            str.assign(beg, end);
        }
    }


} // namespace webpp::istl

#endif // WEBPP_STRING_LIKE_HPP
