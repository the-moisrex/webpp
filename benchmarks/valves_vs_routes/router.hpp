#ifndef WEBPP_HTTP_STATIC_ROUTER_HPP
#define WEBPP_HTTP_STATIC_ROUTER_HPP

#include "../../webpp/extensions/extension.hpp"
#include "../../webpp/http/bodies/string.hpp"
#include "../../webpp/http/http_concepts.hpp"
#include "../../webpp/http/routes/context.hpp"
#include "../../webpp/http/routes/router_concepts.hpp"
#include "../../webpp/std/optional.hpp"
#include "../../webpp/std/tuple.hpp"
#include "../../webpp/std/utility.hpp"
#include "../../webpp/std/vector.hpp"

namespace webpp::http {


    /**
     * Const router is a router that satisfies that "Router" concept.
     */
    template <ExtensionList NewRootExtensions = empty_extension_pack, typename... RouteType>
    struct router {
        using extension_list_type = stl::remove_cvref_t<NewRootExtensions>;


        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        stl::tuple<RouteType...> routes;

        // NOLINTEND(misc-non-private-member-variables-in-classes)

        constexpr router(NewRootExtensions&&, RouteType&&... _route) noexcept
          : routes(stl::forward<RouteType>(_route)...) {}

        constexpr router(RouteType&&... _route) noexcept : routes(stl::forward<RouteType>(_route)...) {}

        constexpr router(router const&) noexcept = default;
        constexpr router(router&&) noexcept      = default;
        constexpr ~router()                      = default;

        constexpr router& operator=(router&&) noexcept = delete;
        constexpr router& operator=(router const&)     = delete;

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
            // The specified index is not valid
            stl::terminate();
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
        [[nodiscard]] constexpr decltype(auto)
        handle_primary_results(ResT&& res, CtxT&& ctx, ReqT&& req) const noexcept {
            using result_type  = stl::remove_cvref_t<ResT>;
            using context_type = stl::remove_cvref_t<CtxT>;

            if constexpr (HTTPResponse<result_type> || istl::Optional<result_type>) {
                return stl::forward<ResT>(res); // let the "next_route" function handle it
            } else if constexpr (stl::is_integral_v<result_type>) {
                return ctx.error(res);          // error code
            } else if constexpr (Route<result_type, context_type>) {
                auto res2       = call_route(res, ctx, req);
                using res2_type = stl::remove_cvref_t<decltype(res2)>;
                if constexpr (stl::is_void_v<res2_type>) {
                    return true; // run the next route
                } else {
                    return handle_primary_results(stl::move(res2), stl::forward<CtxT>(ctx), req);
                }
            } else if constexpr (requires { ctx.create_response(stl::forward<ResT>(res)); }) {
                return ctx.create_response(stl::forward<ResT>(res));
            } else {
                static_assert_false(result_type, "We don't know how to handle your output.");
            }
        }

        template <stl::size_t Index = 0, typename ResT, Context CtxT, HTTPRequest ReqT>
        constexpr HTTPResponse decltype(auto)
        basic_next_route(ResT&& res, CtxT&& ctx, ReqT&& req) const noexcept {
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
                    return istl::deref(basic_next_route<Index>(
                      handle_primary_results(res.value(), ctx, req),
                      stl::forward<CtxT>(ctx),
                      req));
                } else {
                    // We don't need to handle the result of this route, because there's none;
                    // So we just call the next route for the result.
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
                return stl::forward<ResT>(res);
            } else if constexpr (stl::same_as<result_type, bool>) {
                // if the user returns "true", then we'll check the next route, otherwise, it's a
                // "route handling termination signal" for us.
                if (res) {
                    return operator()<next_route_index>(ctx, req);
                } else {
                    return ctx.error(status_code::not_found);
                }
            } else {
                ctx.logger.error("Router", "unknown response type");
                return ctx.error(status_code::internal_server_error, "Unknown response type.");
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
            using req_type = stl::remove_cvref_t<RequestType>;
            using merged_extensions =
              typename merge_root_extensions<typename req_type::root_extensions, NewRootExtensions>::type;
            using context_type = simple_context<req_type, merged_extensions>;
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
            // constexpr auto next_route_index  = Index + 1;

            // todo: inject "path extension" here so you don't have to parse the URI for every route

            if constexpr (no_routes || passed_last_route) {
                // this is adds a 404 error response to the end of the routes essentially
                return ctx.error(status_code::not_found);
            } else {
                // handling root-level route calls:
                auto route = stl::get<Index>(routes);

                // todo
                // ctx.call_pre_entryroute_methods();
                // todo: we might have a context switching, what should we do?
                // ctx.call_post_entryroute_methods();

                using res_t = stl::remove_cvref_t<decltype(call_route(route, ctx, req))>;
                if constexpr (stl::is_void_v<res_t>) {
                    // because "handle_route_results" can't handle void inputs, here's how we deal with it
                    call_route(route, ctx, req);
                    return ctx.error(status_code::not_found);
                } else {
                    return basic_next_route<Index>(
                      handle_primary_results(call_route(route, ctx, req), ctx, req),
                      ctx,
                      req);
                }
            }
        }

        /**
         * Append a string representation of the routes
         */
        template <istl::String StrT, HTTPRequest ReqT, stl::size_t Index = 0>
        void to_string(StrT& out, ReqT&& req) const {
            using req_type = stl::remove_cvref_t<ReqT>;
            using merged_extensions =
              typename merge_root_extensions<typename req_type::root_extensions, NewRootExtensions>::type;
            using context_type = simple_context<req_type, merged_extensions>;

            auto const this_route = stl::get<Index>(routes);
            this_route.to_string(out, context_type{req}, req);

            // print the next route as well
            constexpr bool last_route = Index == (route_count() - 1);
            if constexpr (!last_route) {
                out.push_back('\n');
                to_string<StrT, ReqT, Index + 1>(out, stl::forward<ReqT>(req));
            }
        }

        /**
         * Get a string representation of the routes.
         */
        template <istl::String StrT = stl::string, HTTPRequest ReqT>
        StrT to_string(ReqT&& req) const {
            StrT out;
            to_string(out, stl::forward<ReqT>(req));
            return out;
        }
    };

    template <typename ExtensionListType, typename... RouteType>
    router(ExtensionListType&&, RouteType&&...) -> router<ExtensionListType, RouteType...>;

    template <typename... RouteType>
        requires(sizeof...(RouteType) > 0 &&
                 !istl::is_specialization_of_v<istl::first_type_t<RouteType...>, extension_pack>)
    router(RouteType&&...) -> router<empty_extension_pack, RouteType...>;


} // namespace webpp::http

#endif // WEBPP_HTTP_STATIC_ROUTER_HPP
