// Created by moisrex on 5/9/20.

#ifndef WEBPP_ROUTE_CONCEPTS_H
#define WEBPP_ROUTE_CONCEPTS_H

#include "../response_concepts.hpp"
#include "./context_concepts.hpp"

namespace webpp {

    template <typename T>
    concept RouteResponse = Response<T> || ConvertibleToResponse<T> || Context<T> || stl::same_as<T, bool>;

    template <typename T>
    concept Route = stl::is_void_v<T> || requires (T obj) {
        obj.template operator()<fake_context_type>;
    };

    template <typename T>
    concept RealRoute = requires(T obj, fake_context_type ctx) {
        {obj.template operator()<fake_context_type>(ctx)};
    };

} // namespace webpp

#endif // WEBPP_ROUTE_CONCEPTS_H
