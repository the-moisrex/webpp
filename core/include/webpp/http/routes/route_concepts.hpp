// Created by moisrex on 5/9/20.

#ifndef WEBPP_ROUTE_CONCEPTS_H
#define WEBPP_ROUTE_CONCEPTS_H

#include "../response_concepts.hpp"
#include "./context_concepts.hpp"

namespace webpp {

    template <typename T>
    concept RouteResponse = Response<T> || ConvertibleToResponse<T> || Context<T> || stl::same_as<T, bool>;

    template <typename T>
    concept Route = stl::is_void_v<T> || requires(T obj) {
        {obj.template operator()<fake_context_type>(fake_context_type{})};
    };

} // namespace webpp

#endif // WEBPP_ROUTE_CONCEPTS_H
