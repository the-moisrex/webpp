// Created by moisrex on 5/28/20.

#ifndef WEBPP_HEADER_CONCEPTS_H
#define WEBPP_HEADER_CONCEPTS_H

#include "../extensions/extension.hpp"
#include "../std/concepts.hpp"
#include "../traits/traits.hpp"

namespace webpp::http {

    template <typename T>
    concept Header = requires(T h) {
        requires Traits<typename T::traits_type>;
        {T::header_direction};
        { T::is_mutable } -> stl::same_as<bool>;
    };

    template <typename T>
    concept HeaderField = requires(T f) {
        requires Traits<typename T::traits_type>;
        T::header_direction;
        { T::is_mutable } -> stl::same_as<bool>;
        requires requires(traits::string_view<typename T::traits_type> str_view) {
            { f.is_name(str_view) } -> stl::same_as<bool>;
        };
    };

} // namespace webpp::http

#endif // WEBPP_HEADER_CONCEPTS_H
