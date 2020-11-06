// Created by moisrex on 11/4/20.

#ifndef WEBPP_FRAGMENT_HPP
#define WEBPP_FRAGMENT_HPP

#include "../std/string.hpp"

namespace webpp::uri {

    template <istl::String StringType = stl::string>
    struct basic_fragment : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;

        template <typename ...T>
        constexpr basic_fragment(T&&...args) : string_type{stl::forward<T>(args)...} {}
    };


}

#endif // WEBPP_FRAGMENT_HPP
