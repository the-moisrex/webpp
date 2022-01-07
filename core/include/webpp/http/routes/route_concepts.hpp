// Created by moisrex on 5/9/20.

#ifndef WEBPP_ROUTE_CONCEPTS_H
#define WEBPP_ROUTE_CONCEPTS_H

#include "../response_concepts.hpp"
#include "context_concepts.hpp"

namespace webpp::http {

    template <typename T>
    concept RouteResponse =
      HTTPResponse<T> || ConvertibleToResponse<T> || Context<T> || stl::same_as<T, bool>;

    // C = Context
    template <typename T, typename C>
    concept CallableWithContext = /*requires (T c) {
                                    c.template operator()<C>;
                                  } || (stl::is_class_v<stl::remove_cvref_t<T>> &&
                                   stl::is_member_function_pointer_v<&T::template operator()<C>>) ||*/
      (stl::is_invocable_v<T, stl::add_lvalue_reference_t<C>>&&
         RouteResponse<stl::invoke_result_t<T, stl::add_lvalue_reference_t<C>>>);

    template <typename T, typename C>
    concept PotentialRoute = requires(T route, C& ctx, typename C::request_type const& req) {
        requires requires {
            {route()};
        } || requires {
            {route(ctx)};
        } || requires {
            {route(req)};
        } || requires {
            {route(ctx, req)};
        } || requires {
            {route(req, ctx)};
        };
    };

    template <typename T, typename C>
    concept Route = requires(T obj) {
        requires CallableWithContext<T, typename T::template switched_context_type<C>>;
        typename T::template switched_context_type<C>;
    };

} // namespace webpp::http

#endif // WEBPP_ROUTE_CONCEPTS_H
