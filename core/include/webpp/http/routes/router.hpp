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
    template <ExtensionList ExtensionListType = empty_extension_pack, typename... RouteType>
    struct router {
        using extension_list_type = stl::remove_cvref_t<ExtensionListType>;

        // todo: extract additional routes from extensions
        // todo: add router_extensions as well
        const stl::tuple<RouteType...> routes;

        constexpr router(ExtensionListType&&, RouteType&&... _route) noexcept
          : routes(stl::forward<RouteType>(_route)...) {}

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
        template <stl::size_t Index = 0>
        [[nodiscard]] constexpr auto handle_route_results(auto&& res, auto&& ctx, auto&& req) const noexcept {
            using namespace stl;

            using result_type                   = remove_cvref_t<decltype(res)>;
            using context_type                  = remove_cvref_t<decltype(ctx)>;
            constexpr auto next_route_index     = Index + 1;
            constexpr bool is_passed_last_route = Index > (route_count() - 1);

            // Don't handle the edge cases of return types here, do it in the "call_route" function
            // for example, a function that returns strings and can be converted into a response
            // is going to be converted into a response in the "call_route" function
            if constexpr (istl::Optional<result_type>) {
                // if res is a context, context switching is automatically happens here,
                // the same goes for any other valid type

                if (res) {
                    return handle_route_results<Index>(res.value(), forward<decltype(ctx)>(ctx), req);
                } else {
                    // return a 500 error
                    constexpr auto err_code = 500u;
                    using ret_type =
                      decltype(handle_route_results<Index>(res.value(), forward<decltype(ctx)>(ctx), req));
                    if constexpr (is_convertible_v<typename result_type::value_type, unsigned long>) {
                        return handle_route_results<Index>(typename result_type::value_type{err_code},
                                                           forward<decltype(ctx)>(ctx),
                                                           req);
                    } else if constexpr (is_constructible_v<ret_type, decltype(err_code)>) {
                        return ret_type{err_code};
                    } else {
                        return handle_route_results<Index>(err_code, forward<decltype(ctx)>(ctx), req);
                    }
                }
                // todo: check if you can run the next route as well
                //                if constexpr (is_passed_last_route) {
                //                    return ctx.error(404u);
                //                } else {
                //                    return operator()<next_route_index>(forward<decltype(ctx)>(ctx),
                //                    req);
                //                }
            } else if constexpr (Context<result_type>) {
                // context switching is happening here
                // just call the next route or finish it with calling the context handlers
                if constexpr (!is_passed_last_route) {
                    return operator()<next_route_index>(forward<decltype(res)>(res), req);
                } else {
                    return ctx.error(404u);
                }
            } else if constexpr (HTTPResponse<result_type>) {
                // we're done; don't call the next route
                return res;
            } else if constexpr (is_same_v<result_type, bool>) {
                if (res) {
                    return operator()<next_route_index>(forward<decltype(ctx)>(ctx), req);
                } else {
                    return ctx.error(404u); // doesn't matter if it's the last route or not,
                    // returning false just means finish it
                }
            } else if constexpr (is_integral_v<result_type>) {
                return ctx.error(res); // error code
            } else if constexpr (Route<result_type, context_type>) {
                auto res2       = call_route(res, ctx, req);
                using res2_type = remove_cvref_t<decltype(res2)>;
                if constexpr (is_void_v<res2_type>) {
                    return operator()<next_route_index>(forward<decltype(ctx)>(ctx), req);
                } else {
                    return handle_route_results<Index>(move(res2), forward<decltype(ctx)>(ctx), req);
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
                return ctx.response(forward<decltype(res)>(res));
                // todo: consider "response extension" injection in order to get the right response type
            } else if constexpr (istl::StringViewifiable<result_type>) {
                // Use string_response response type to handle strings
                return ctx.template response<string_response>(
                  istl::string_viewify(forward<decltype(res)>(res)));
            } else {
                // we just ignore anything else the user returns
                // this part never happens because the "call_route" converts this part to the above parts
                return ctx.error(404u);
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


        template <stl::size_t Index = 0>
        constexpr HTTPResponse auto operator()(Context auto&& ctx, HTTPRequest auto&& req) const noexcept {

            constexpr bool no_routes       = route_count() == 0u;
            constexpr bool past_last_route = Index > (route_count() - 1);

            if constexpr (no_routes || past_last_route) {
                return ctx.error(404u);
            } else {

                // handling root-level route calls:
                auto route = stl::get<Index>(routes);

                // It's noexcept, we call it knowing that.
                // todo
                // ctx.call_pre_entryroute_methods();
                // todo: we might have a context switching, what should we do?
                // ctx.call_post_entryroute_methods();
                if constexpr (requires {
                                  { call_route(route, ctx, req) } -> stl::same_as<void>;
                              }) {
                    // because "handle_route_results" can't handle void inputs, here's how we deal with it
                    call_route(route, ctx, req);
                    return ctx.error(404u);
                } else {
                    return handle_route_results<Index>(call_route(route, ctx, req), ctx, req);
                }
            }
        }
    };

    template <typename ExtensionListType, typename... RouteType>
    router(ExtensionListType&&, RouteType&&...) -> router<ExtensionListType, RouteType...>;


    /*
    struct dynamic_route {

      protected:
        // todo: maybe don't use std::function? it's slow a bit (but not that much)
        using callback_t = std::function<void()>;

        callback_t callback = nullptr;

      public:
        // fixme: it gives me error when I put "noexcept" here:
        dynamic_route() = default;
        dynamic_route(callback_t callback) noexcept : callback(callback) {
        }

        template <typename C>
        dynamic_route& operator=(C&& callback) noexcept {
            this->callback = [=](req_t req, res_t res) noexcept {
                return call_route(callback, req, res);
            };
            return *this;
        }

        auto operator()(req_t req, res_t res) noexcept {
            return callback(req, res);
        }

        inline bool is_match(req_t req) noexcept {
            return condition(req);
        }
    };
     */


} // namespace webpp

#endif // WEBPP_ROUTER_H
