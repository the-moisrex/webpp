// Created by moisrex on 5/10/20.

#ifndef WEBPP_ROUTER_CONCEPTS_H
#define WEBPP_ROUTER_CONCEPTS_H

#include "../../extensions/extension.hpp"
#include "../../std/tuple.hpp"
#include "../../std/type_traits.hpp"


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
    template <typename T>
    concept Router = requires(T r) {
        // todo: this is wrong
        typename T::initial_context_type;
        // {r()} -> Response;
        // todo: add support for operator()
    };

    /**
     * Additional stuff that a router extension may have:
     *   - type     initial_context_type
     *   - type     additional_routes    : additional routes / router
     *   - static   todo: stuff we need in router extensions
     */
    template <typename E>
    concept RouterExtension = Extension<E>;

    template <typename T>
    concept RouterExtensionWithContextExtensions = RouterExtension<T> && requires {
        T::template context_extensions;
    };

    template <typename T>
    concept RouterExtensionWithAdditionalRoutes = requires {
        typename T::additional_routes;
    };


    template <typename T>
    struct is_router_extension {
        static constexpr bool value = RouterExtension<T>;
    };

    template <typename T>
    concept RouterExtensionList = ExtensionListOf<T, is_router_extension>;


} // namespace webpp::http

#endif // WEBPP_ROUTER_CONCEPTS_H
