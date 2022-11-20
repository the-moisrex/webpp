// Created by moisrex on 5/10/20.

#ifndef WEBPP_ROUTER_CONCEPTS_HPP
#define WEBPP_ROUTER_CONCEPTS_HPP

#include "../../extensions/extension.hpp"
#include "../../std/tuple.hpp"
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




    ////////////////////////////// Context Concepts //////////////////////////////



    enum struct extension_method {
        pre_subroute,
        post_subroute,
        pre_entryroute,
        post_entryroute,
        pre_firstroute,
        post_lastroute
    };

    namespace details {
        template <typename A, extension_method em, typename ContextArgType>
        static constexpr bool has_method() noexcept {
            switch (em) {
                case extension_method::pre_subroute:
                    return requires(A obj, ContextArgType & ctx) {
                        obj.pre_subroute(ctx);
                    };
                case extension_method::post_subroute:
                    return requires(A obj, ContextArgType & ctx) {
                        obj.post_subroute(ctx);
                    };
                case extension_method::pre_entryroute:
                    return requires(A obj, ContextArgType & ctx) {
                        obj.pre_entryroute(ctx);
                    };
                case extension_method::post_entryroute:
                    return requires(A obj, ContextArgType & ctx) {
                        obj.post_entryroute(ctx);
                    };
                case extension_method::pre_firstroute:
                    return requires(A obj, ContextArgType & ctx) {
                        obj.pre_firstroute(ctx);
                    };
                case extension_method::post_lastroute:
                    return requires(A obj, ContextArgType & ctx) {
                        obj.post_lastroute(ctx);
                    };
                default: return false;
            }
        }
    } // namespace details

    template <typename A, extension_method em, typename ContextArgType>
    concept has_context_extension_method = details::has_method<A, em, ContextArgType>();


    template <typename T>
    concept ContextExtension = Extension<T>;

    template <typename T>
    concept Context = requires(stl::remove_cvref_t<T> c) {
        requires EnabledTraits<typename stl::remove_cvref_t<T>>;
        requires HTTPRequest<typename stl::remove_cvref_t<T>::request_type>;
        requires HTTPResponse<typename stl::remove_cvref_t<T>::response_type>;
        // c.request;
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

#endif // WEBPP_ROUTER_CONCEPTS_HPP
