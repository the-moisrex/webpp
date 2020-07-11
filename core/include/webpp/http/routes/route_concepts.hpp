// Created by moisrex on 5/9/20.

#ifndef WEBPP_ROUTE_CONCEPTS_H
#define WEBPP_ROUTE_CONCEPTS_H

#include "../response_concepts.h"
#include "./context_concepts.h"

namespace webpp {

    template <typename T>
    concept RouteResponse = Response<T> || ConvertibleToResponse<T> ||
                            Context<T> || stl::same_as<T, bool>;

    // todo: complete this concept
    template <typename T>
    concept Route = requires {
        typename T::template route_type<fake_traits_type>;
        requires requires(typename T::template route_type<fake_traits_type> r) {
            { r() }
            ->RouteResponse;
        };
    };

    template <typename T>
    concept NextRoute = Route<T> || stl::same_as<T, void>;

} // namespace webpp

#endif // WEBPP_ROUTE_CONCEPTS_H
