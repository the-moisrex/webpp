// Created by moisrex on 11/4/20.

#ifndef WEBPP_FRAGMENTS_HPP
#define WEBPP_FRAGMENTS_HPP

#include "../std/string.hpp"

namespace webpp::uri {

    template <istl::String StringType = stl::string>
    struct basic_fragments : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;

        template <typename ...T>
        constexpr basic_fragments(T&&...args) : string_type{stl::forward<T>(args)...} {}
    };


}

#endif // WEBPP_FRAGMENTS_HPP
