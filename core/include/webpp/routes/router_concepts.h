// Created by moisrex on 5/10/20.

#ifndef WEBPP_ROUTER_CONCEPTS_H
#define WEBPP_ROUTER_CONCEPTS_H

#include <type_traits>

namespace webpp {

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
     */
    template <typename E>
    concept RouteExtension = requires(E e) {
        ::std::is_default_constructible_v<E>;
    };

    template <typename E>
    concept RouteExtensions = requires(E e) {
        ::std::is_default_constructible_v<E>;
    };


    template <typename ICT, typename T>
    concept has_initial_context_type = requires {
        typename T::template initial_context_type<ICT>::type;
    };

} // namespace webpp

#endif // WEBPP_ROUTER_CONCEPTS_H
