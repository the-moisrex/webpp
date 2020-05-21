// Created by moisrex on 5/10/20.

#ifndef WEBPP_ROUTER_CONCEPTS_H
#define WEBPP_ROUTER_CONCEPTS_H

#include "../../extensions/extension.h"
#include "../../std/tuple.h"

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
    concept InitialContextType = requires {
        T::template initial_context_type;
    };

    template <typename E>
    concept RouterExtension = Extension<E>;

    /**
     * it can be one of these 3:
     *   - a tuple of extensions
     *   - a router_extension_pack of extensions (maybe not? todo)
     *   - a single extension
     */
    template <typename T>
    concept RouterExtensionDependency = RouterExtension<T> || ::stl::Tuple<T>;

    template <typename... T>
    struct is_router_extension_list {
        static constexpr bool value = false;
    };

    template <RouterExtension... T>
    struct is_router_extension_list<::std::tuple<T...>> {
        static constexpr bool value = true;
    };

    template <RouterExtension... T>
    struct is_router_extension_list<typelist<T...>> {
        static constexpr bool value = true;
    };

    template <RouterExtension... T>
    struct is_router_extension_list<extension_pack<T...>> {
        static constexpr bool value = true;
    };

    template <typename T>
    concept RouterExtensionList = ExtensionList<T, is_router_extension_list>;


} // namespace webpp

#endif // WEBPP_ROUTER_CONCEPTS_H
