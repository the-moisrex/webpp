// Created by moisrex on 5/9/20.

#ifndef WEBPP_ROUTE_CONCEPTS_H
#define WEBPP_ROUTE_CONCEPTS_H

#include "../response_concepts.h"
#include "./context_concepts.h"

namespace webpp {

    template <typename T>
    concept RouteResponse = Response<T> || ConvertibleToResponse<T> ||
                            Context<T> || ::std::same_as<T, bool>;

    // todo: complete this concept
    template <typename T>
    concept Route = requires(T r) {
        typename T::next_route_type;
        typename T::route_type;
        { r() }
        ->RouteResponse;
    };

    template <typename T>
    concept NextRoute = Route<T> || ::std::same_as<T, void>;

} // namespace webpp

#endif // WEBPP_ROUTE_CONCEPTS_H
