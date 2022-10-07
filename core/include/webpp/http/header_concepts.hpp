// Created by moisrex on 5/28/20.

#ifndef WEBPP_HEADER_CONCEPTS_H
#define WEBPP_HEADER_CONCEPTS_H

#include "../extensions/extension.hpp"
#include "../std/concepts.hpp"
#include "../traits/traits.hpp"

namespace webpp::http {

    /**
     * This concept is what the underlying Protocols expect to see in a response's header from apps.
     */
    template <typename T>
    concept HTTPHeader = requires(T h) {
        { h.str() } -> istl::StringViewifiable;
    };

    template <typename T>
    concept HTTPHeaderField = requires(T f) {
        requires Traits<typename T::traits_type>;
        requires requires(traits::string_view<typename T::traits_type> str_view) {
            { f.is_name(str_view) } -> stl::same_as<bool>;
        };
        { f.name } -> istl::StringViewifiable;
        { f.value } -> istl::StringViewifiable;
    };

} // namespace webpp::http

#endif // WEBPP_HEADER_CONCEPTS_H
