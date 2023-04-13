// Created by moisrex on 5/10/20.

#ifndef WEBPP_ROUTER_CONCEPTS_HPP
#define WEBPP_ROUTER_CONCEPTS_HPP

#include "../../std/type_traits.hpp"
#include "../http_concepts.hpp"


namespace webpp::http {

    /**
     * Router:
     *
     * Router types:
     *   - Dynamic route   : dynamic_route
     *       The routes and the context can be changed at runtime.
     *   - Constexpr route : const_route
     *       No route can be changed at runtime. The initial context cannot be
     *       changed either.
     *
     * Routing table implementation:
     *   - Entry route list:
     *       It's a list of entry routes (which they include some sub-routes of
     *       their own).
     *       We don't need  a Global Routing table, we just need a good
     *       prioritization technique.
     *   - Priorities of the entry route list:
     *       This table will be used to change the priority of the entry route
     *       list. This priority change is done through the context extensions.
     *       So the context extensions need a way to access the router. The best
     *       way to do so is to add this route itself to the base_context. This
     *       is possible because the context is created initially by the
     *       router itself and the changes in its type should be done inside the
     *       router too.
     *
     *
     * Router Extensions:
     *   Router extensions have these features:
     *     - Can specify other extensions as dependencies of this extension.
     *     - Can add new extensions for the "initial context type".
     *     - Cannot change the initial extension type to something not valid.
     *     - Can add routes:
     *         - Before all routes
     *         - After all routes
     *
     */



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
    class basic_dynamic_router;

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



} // namespace webpp::http

#endif // WEBPP_ROUTER_CONCEPTS_HPP
