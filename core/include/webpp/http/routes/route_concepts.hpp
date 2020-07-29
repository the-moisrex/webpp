// Created by moisrex on 5/9/20.

#ifndef WEBPP_ROUTE_CONCEPTS_H
#define WEBPP_ROUTE_CONCEPTS_H

#include "../response_concepts.hpp"
#include "./context_concepts.hpp"

namespace webpp {

    template <typename T>
    concept RouteResponse = Response<T> || ConvertibleToResponse<T> || Context<T> || stl::same_as<T, bool>;

    template <typename T, typename C = fake_context_type>
    concept CallableWithContext = requires (T c) {
                                    c.template operator()<C>;
                                  } /*|| (stl::is_class_v<stl::remove_cvref_t<T>> &&
                                   stl::is_member_function_pointer_v<&T::template operator()<C>>)*/ ||
                                  stl::is_invocable_v<T, C&>;

    template <typename T, typename C = fake_context_type>
    concept PotentialRoute = stl::is_void_v<T> || CallableWithContext<T, C>;

    template <typename T, typename C = fake_context_type>
    concept Route = requires(T obj) {
        typename T::template switched_context_type<C>;
    }
    &&CallableWithContext<T, typename T::template switched_context_type<C>>;

} // namespace webpp

#endif // WEBPP_ROUTE_CONCEPTS_H
