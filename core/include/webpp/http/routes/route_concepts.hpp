// Created by moisrex on 5/9/20.

#ifndef WEBPP_ROUTE_CONCEPTS_H
#define WEBPP_ROUTE_CONCEPTS_H

#include "../response_concepts.hpp"
#include "./context_concepts.hpp"

namespace webpp {

    template <typename T>
    concept RouteResponse = Response<T> || ConvertibleToResponse<T> || Context<T> || stl::same_as<T, bool>;

    template <typename T, typename C = fake_context_type>
    concept PotentialRoute = stl::is_void_v<T> ||
                             (stl::is_class_v<stl::remove_cvref_t<T>> &&
                              stl::is_default_constructible_v<stl::remove_cvref_t<T>>) ||
                             requires(T func, C& ctx) {
        {func(ctx)};
    };

    template <typename T, typename C = fake_context_type>
    concept Route = requires(T obj, C& ctx) {
        typename T::template switched_context_type<C>;
        {obj.template operator()<C>(ctx)};
    };

} // namespace webpp

#endif // WEBPP_ROUTE_CONCEPTS_H
