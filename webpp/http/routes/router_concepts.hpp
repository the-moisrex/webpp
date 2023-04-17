// Created by moisrex on 5/10/20.

#ifndef WEBPP_ROUTER_CONCEPTS_HPP
#define WEBPP_ROUTER_CONCEPTS_HPP

#include "../../std/type_traits.hpp"
#include "../http_concepts.hpp"


namespace webpp::http {



    ////////////////////////////// Context Concepts //////////////////////////////


    template <typename T>
    concept Context = requires(stl::remove_cvref_t<T> c) {
                          requires EnabledTraits<typename stl::remove_cvref_t<T>>;
                          requires HTTPRequest<typename stl::remove_cvref_t<T>::request_type>;
                          requires HTTPResponse<typename stl::remove_cvref_t<T>::response_type>;
                          { c.request } -> stl::same_as<typename stl::remove_cvref_t<T>::request_ref>;
                      };



    ////////////////////////////// Route Concepts //////////////////////////////



    template <typename T>
    concept RouteResponse =
      HTTPResponse<T> || ConvertibleToResponse<T> || Context<T> || stl::same_as<T, bool>;

    // C = Context
    template <typename T, typename C>
    concept CallableWithContext = /*requires (T c) {
                                    c.template operator()<C>;
                                  } || (stl::is_class_v<stl::remove_cvref_t<T>> &&
                                   stl::is_member_function_pointer_v<&T::template operator()<C>>) ||*/
      (stl::is_invocable_v<T, stl::add_lvalue_reference_t<C>> &&
       RouteResponse<stl::invoke_result_t<T, stl::add_lvalue_reference_t<C>>>);

    template <typename T, typename C>
    concept PotentialRoute = requires(T route, C& ctx, typename C::request_type const& req) {
                                 requires requires {
                                              { route() };
                                          } || requires {
                                                   { route(ctx) };
                                               } || requires {
                                                        { route(req) };
                                                    } || requires {
                                                             { route(ctx, req) };
                                                         } || requires {
                                                                  { route(req, ctx) };
                                                              };
                             };

    template <typename T, typename C>
    concept Route = requires(T obj) {
                        requires CallableWithContext<T, typename T::template switched_context_type<C>>;
                        typename T::template switched_context_type<C>;
                    };



    template <Traits>
    struct basic_dynamic_router;

    template <Traits>
    struct basic_context;

    template <Traits T, typename Callable = void>
    struct dynamic_route;

    template <typename Self = void>
    struct valve;
    template <typename Callable>
    struct not_valve;
    template <typename Callable>
    struct negative_valve;
    template <typename Callable>
    struct positive_valve;
    template <typename LeftCallable, typename RightCallable>
    struct and_valve;
    template <typename LeftCallable, typename RightCallable>
    struct or_valve;
    template <typename... Callables>
    struct forward_valve;
    template <typename... PreRoutes>
    struct prerouting_valve;
    template <typename... PostRoutes>
    struct postrouting_valve;
    template <typename... CallableSegments>
    struct segment_valve;
    template <typename MemPtr>
    struct member_function_valve;
    template <typename... ManglerTypes>
    struct mangler_valve;
    template <typename Pres, typename Posts, typename Manglers, typename Routes>
    struct valves_group;



    template <typename Router, typename RouteType>
    concept ValveRequiresSetup = requires(RouteType route, Router& router) { route.setup(router); };

    template <typename Callable, typename RouterType>
    static constexpr void setup_route([[maybe_unused]] Callable&&  route,
                                      [[maybe_unused]] RouterType& router) {
        if constexpr (ValveRequiresSetup<RouterType, Callable>) {
            route.setup(router);
        }
    }

} // namespace webpp::http

#endif // WEBPP_ROUTER_CONCEPTS_HPP
