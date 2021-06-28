#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "../../extensions/extension.hpp"
#include "../../std/optional.hpp"
#include "../../std/tuple.hpp"
#include "../../std/vector.hpp"
#include "../bodies/string.hpp"
#include "../request_concepts.hpp"
#include "../response_concepts.hpp"
#include "./context.hpp"
#include "./route_concepts.hpp"
#include "./router_concepts.hpp"

namespace webpp::http {


    /**
     * Const router is a router that satisfies that "Router" concept.
     */
    template <ExtensionList ExtensionListType = empty_extension_pack,
              typename AppType                = void,
              typename... RouteType>
    struct router {
        using extension_list_type         = stl::remove_cvref_t<ExtensionListType>;
        using app_type                    = stl::remove_cvref_t<AppType>;
        static constexpr bool has_app_ref = !stl::is_void_v<app_type>;
        using app_ref =
          stl::conditional_t<has_app_ref, stl::add_lvalue_reference_t<app_type>, istl::nothing_type>;

        [[no_unique_address]] const app_ref app{};


        // todo: extract additional routes from extensions
        // todo: add router_extensions as well
        const stl::tuple<RouteType...> routes;

        constexpr router(ExtensionListType&&, RouteType&&... _route) noexcept
          : routes(stl::forward<RouteType>(_route)...) {}

        constexpr router(ExtensionListType&&, app_ref app_ref_obj, RouteType&&... _route) noexcept
          : app{app_ref_obj},
            routes(stl::forward<RouteType>(_route)...) {}

        constexpr router(RouteType&&... _route) noexcept : routes(stl::forward<RouteType>(_route)...) {}

        constexpr router(router const&) noexcept = default;
        constexpr router(router&&) noexcept      = default;

        /**
         * @return how many routes are in this router
         */
        [[nodiscard]] static constexpr stl::size_t route_count() noexcept {
            return sizeof...(RouteType);
        }

        /**
         * Get the nth route
         * @tparam N
         * @param i
         * @return
         */
        template <stl::size_t N = 0>
        consteval auto& operator[](stl::size_t i) const noexcept {
            if (N == i) {
                return stl::get<N>(routes);
            }
            if constexpr (N + 1 < route_count()) {
                return operator[]<N + 1>(i);
            }
            throw stl::invalid_argument("The specified index is not valid");
        }

      private:
        /**
         * This function will handle the edge cases of the returned types of the routes.
         * @returns bool to terminate or continue checking the next route
         * @returns response
         * @returns Context so context switching can happen in the caller
         * @returns optional; an optional that doesn't contain value will cause the next route to be called
         */
        template <typename ResT, Context CtxT, HTTPRequest ReqT>
        [[nodiscard]] constexpr auto handle_route_results(ResT&& res, CtxT&& ctx, ReqT&& req) const noexcept {
            using namespace stl;

            using result_type  = remove_cvref_t<ResT>;
            using context_type = remove_cvref_t<CtxT>;

            if constexpr (HTTPResponse<result_type>) {
                // we're done; don't call the next route
                return res;
            } else if constexpr (is_integral_v<result_type>) {
                return ctx.error(res); // error code
            } else if constexpr (Route<result_type, context_type>) {
                auto res2       = call_route(res, ctx, req);
                using res2_type = remove_cvref_t<decltype(res2)>;
                if constexpr (is_void_v<res2_type>) {
                    return true; // run the next route
                } else {
                    return handle_route_results(move(res2), forward<CtxT>(ctx), req);
                }
            } else if constexpr (requires {
                                     requires ConstructibleWithResponse<typename context_type::response_type,
                                                                        result_type>;
                                     requires ResponseBody<
                                       typename context_type::response_type::
                                         body_type>; // check if the response body is going to be a valid
                                                     // response body (which requires .str from extensions
                                                     // because response_body itself doesn't have a default
                                                     // value generator)
                                 }) {
                return ctx.response(forward<ResT>(res));
            } else if constexpr (istl::StringViewifiable<result_type>) {
                if constexpr (context_type::template has_extension<string_response>()) {
                    // Use string_response, response type to handle strings
                    return ctx.template response<string_response>(istl::string_viewify(forward<ResT>(res)));
                } else {
                    static_assert_false(
                      result_type,
                      "You returned a string, but the router doesn't have access to string_response extension. Pass it as an extension to the router.");
                }
            } else {
                // todo: consider "response extension" injection in order to get the right response type

                // we just ignore anything else the user returns
                // this part never happens because the "call_route" converts this part to the above parts
                return ctx.error(404u);
            }
        }


        template <stl::size_t Index = 0, typename ResT, Context CtxT, HTTPRequest ReqT>
        constexpr HTTPResponse auto next_route(ResT&& res, CtxT&& ctx, ReqT&& req) const noexcept {
            using result_type = stl::remove_cvref_t<ResT>;

            constexpr auto next_route_index = Index + 1;
            constexpr bool is_last_route    = Index == (route_count() - 1);

            // there are 5 scenarios that can happen in the top level routers:
            //   1. Top level context switching and call the next route
            //   2. Route handling termination
            //   3. Response has found
            //   4. Checking the next route otherwise
            //   5. Optional response

            if constexpr (istl::Optional<result_type>) {
                if (res) {
                    // Call this function for the same route, but strip out the optional struct
                    return next_route<Index>(handle_route_results(res.value(), stl::forward<CtxT>(ctx), req));
                } else {
                    // Just call the next route
                    return operator()<next_route_index>(stl::forward<CtxT>(ctx), req);
                }
            } else if constexpr (Context<result_type>) {
                // context switching
                if constexpr (is_last_route) {
                    ctx.logger.warning(
                      "Router",
                      "Router got Context Switching action on the last route. 404 will be returned.");
                }

                // calling the next route will return 404 error
                return operator()<next_route_index>(stl::forward<ResT>(res), req);
            } else if constexpr (HTTPResponse<result_type>) {
                // we found our response
                return res;
            } else if constexpr (stl::same_as<result_type, bool>) {
                // if the user returns "true", then we'll check the next route, otherwise, it's a
                // "route handling termination signal" for us.
                if (res) {
                    return operator()<next_route_index>(ctx, req);
                } else {
                    return ctx.error(404u);
                }
            } else {
                ctx.logger.error("Router", "unknown response type");
                return ctx.error(500u, "Unknown response type.");
            }
        }

      public:
        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <HTTPRequest RequestType>
        constexpr HTTPResponse auto operator()(RequestType&& req) const noexcept {
            using context_type = simple_context<stl::remove_cvref_t<RequestType>, extension_list_type>;
            static_assert(Context<context_type>,
                          "Web++ Internal Bug: the context_type is not a match for Context concept");
            return this->template operator()<0>(context_type{req}, req);
        }


        /**
         * Call the routes with the specified request and context.
         */
        template <stl::size_t Index = 0, Context CtxT, HTTPRequest ReqT>
        constexpr HTTPResponse auto operator()(CtxT&& ctx, ReqT&& req) const noexcept {

            constexpr bool no_routes         = route_count() == 0u;
            constexpr bool passed_last_route = Index > (route_count() - 1);

            if constexpr (no_routes || passed_last_route) {
                // this is adds a 404 error response to the end of the routes essentially
                return ctx.error(404u);
            } else {

                // handling root-level route calls:
                auto route = stl::get<Index>(routes);

                // It's noexcept, we call it knowing that.
                // todo
                // ctx.call_pre_entryroute_methods();
                // todo: we might have a context switching, what should we do?
                // ctx.call_post_entryroute_methods();

                using res_t = stl::remove_cvref_t<decltype(call_route(route, ctx, req))>;
                if constexpr (stl::is_void_v<res_t>) {
                    // because "handle_route_results" can't handle void inputs, here's how we deal with it
                    call_route(route, ctx, req);
                    return ctx.error(404u);
                } else {
                    return next_route<Index + 1>(handle_route_results(call_route(route, ctx, req), ctx, req),
                                                 ctx,
                                                 req);
                }
            }
        }

        /**
         * Append a string representation of the routes
         */
        template <istl::String StrT, HTTPRequest ReqT, stl::size_t Index = 0>
        void append_as_string(StrT& out, ReqT&& req) const {
            auto const this_route = stl::get<Index>(routes);
            using context_type    = simple_context<stl::remove_cvref_t<ReqT>, extension_list_type>;
            this_route.append_as_string(out, context_type{req}, req);

            // print the next route as well
            constexpr bool last_route = Index == (route_count() - 1);
            if constexpr (!last_route) {
                out.append("\n");
                append_as_string<StrT, ReqT, Index + 1>(out, stl::forward<ReqT>(req));
            }
        }

        /**
         * Get a string representation of the routes.
         */
        template <istl::String StrT = stl::string, HTTPRequest ReqT>
        StrT to_string(ReqT&& req) const {
            StrT out;
            append_as_string(out, stl::forward<ReqT>(req));
            return out;
        }
    };

    template <typename ExtensionListType, typename... RouteType>
    router(ExtensionListType&&, RouteType&&...) -> router<ExtensionListType, void, RouteType...>;

    template <typename ExtensionListType, Application AppType, typename... RouteType>
    router(ExtensionListType&&, AppType&, RouteType&&...) -> router<ExtensionListType, AppType, RouteType...>;

    template <typename... RouteType>
    requires(sizeof...(RouteType) > 0 &&
             !istl::is_specialization_of_v<istl::first_type_t<RouteType...>, extension_pack>)
      router(RouteType&&...)
    ->router<empty_extension_pack, void, RouteType...>;


} // namespace webpp::http

#endif // WEBPP_ROUTER_H
