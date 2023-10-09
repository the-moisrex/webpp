// Created by moisrex on 10/9/23.

#ifndef WEBPP_STRING_LIKE_HPP
#define WEBPP_STRING_LIKE_HPP

#include "./string.hpp"
#include "./string_view.hpp"

namespace webpp::istl {

    template <typename T>
    concept StringLike = istl::String<T> || istl::StringView<T>;
}

#endif // WEBPP_STRING_LIKE_HPP
