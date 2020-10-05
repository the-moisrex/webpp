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

#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

namespace webpp {

    template <typename T>
    struct application_pointers {
        static constexpr bool value = stl::is_pointer_v<T> && stl::is_class_v<stl::remove_pointer_t<T>>;
    };


    constexpr auto run_and_catch(auto&& callable, Context auto const& ctx, auto&&... args) noexcept {};

    /**
     * Const router is a router that satisfies that "Router" concept.
     */
    template </*fixme: ExtensionList*/ typename ExtensionListType = empty_extension_pack,
              /*fixme: Route*/ typename... RouteType>
    struct router {
        using extension_list_type = stl::remove_cvref_t<ExtensionListType>;

        // todo: Additional routes extracted from the extensions
        //        using additional_routes = ;
        const stl::tuple<RouteType...> routes;

        constexpr router(ExtensionListType&&, RouteType&&... _route) noexcept
          : routes(stl::forward<RouteType>(_route)...) {}

        constexpr router(RouteType&&... _route) noexcept : routes(stl::forward<RouteType>(_route)...) {}

        //        consteval router() noexcept              = delete;
        //        consteval router(router const&) noexcept = delete;
        //        consteval router(router&&) noexcept      = delete;

        //        template <typename... AppTypes>
        //        requires((application_pointers<AppTypes>::value &&
        //                  ...)) consteval router(stl::tuple<AppTypes...> const& _apps, RouteType&&...
        //                  _route) noexcept
        //          : routes(stl::forward<RouteType>(_route)...) {
        //            stl::apply(
        //              [&](auto&... app_ptr) {
        //                  (..., (requires { {_route.app}; } && (_route.app = app_ptr)));
        //              },
        //              _apps);
        //        }


        /**
         * @return how many routes are in this router
         */
        [[nodiscard]] static consteval stl::size_t route_count() noexcept {
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

        auto error(Context auto const& ctx, status_code_type error_code,
                   stl::string_view phrase = "") const noexcept {
            // todo: add methods to change the default error template and individual ones
            stl::string_view _phrase = phrase.empty() ? status_reason_phrase(error_code) : phrase;
            return ctx.template response<string_response>(
              error_code,
              stl::format(
                R"html(<!doctype html><html><head><meta charset="utf-8"><title>{1} {2}!</title></head><body><h1>{1} {2}</h1></body></html>)html",
                error_code, _phrase));
        }

      private:
        template <stl::size_t Index = 0>
        [[nodiscard]] inline auto handle_route_results(auto&& res, auto&& ctx, auto&& req) const noexcept {
            using result_type                   = stl::remove_cvref_t<decltype(res)>;
            using context_type                  = stl::remove_cvref_t<decltype(ctx)>;
            constexpr auto next_route_index     = Index + 1;
            constexpr bool is_passed_last_route = Index > (route_count() - 1);

            // Don't handle the edge cases of return types here, do it in the "call_route" function
            // for example, a function that returns strings and can be converted into a response
            // is going to be converted into a response in the "call_route" function
            if constexpr (istl::Optional<result_type>) {
                // if res is a context, context switching is automatically happens here,
                // the same goes for any other valid type

                if (res) {
                    return handle_route_results<Index>(res.value(),
                                                       stl::forward<decltype(ctx)>(ctx), req);
                } else {
                    // return a 500 error
                    if constexpr (stl::is_convertible_v<typename result_type::value_type, unsigned long>) {
                        return handle_route_results<Index>(typename result_type::value_type{500u},
                                                           stl::forward<decltype(ctx)>(ctx), req);
                    } else {
                        return handle_route_results<Index>(500u,
                                                           stl::forward<decltype(ctx)>(ctx), req);
                    }
                }
                // todo: check if you can run the next route as well
                //                if constexpr (is_passed_last_route) {
                //                    return ctx.error(404u);
                //                } else {
                //                    return operator()<next_route_index>(stl::forward<decltype(ctx)>(ctx),
                //                    req);
                //                }
            } else if constexpr (Context<result_type>) {
                // context switching is happening here
                // just call the next route or finish it with calling the context handlers
                if constexpr (!is_passed_last_route) {
                    return operator()<next_route_index>(stl::forward<decltype(res)>(res), req);
                } else {
                    return ctx.error(404u);
                }
            } else if constexpr (Response<result_type>) {
                // we're done; don't call the next route
                return res;
            } else if constexpr (stl::is_same_v<result_type, bool>) {
                if (res) {
                    return operator()<next_route_index>(stl::forward<decltype(ctx)>(ctx), req);
                } else {
                    return ctx.error(404u); // doesn't matter if it's the last route or not,
                                            // returning false just means finish it
                }
            } else if constexpr (Route<result_type>) {
                auto res2       = call_route(res, ctx, req);
                using res2_type = stl::remove_cvref_t<decltype(res2)>;
                if constexpr (stl::is_void_v<res2_type>) {
                    return operator()<next_route_index>(stl::forward<decltype(ctx)>(ctx), req);
                } else {
                    return handle_route_results<Index>(stl::move(res2), stl::forward<decltype(ctx)>(ctx),
                                                       req);
                }
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
        template <Request RequestType>
        Response auto operator()(RequestType& req) const noexcept {
            using req_type     = stl::remove_cvref_t<RequestType>;
            using context_type = simple_context<req_type, extension_list_type>;
            return this->template operator()<0>(context_type{req.logger, req.get_allocator()}, req);
        }


        template <stl::size_t Index = 0>
        Response auto operator()(Context auto&& ctx, Request auto const& req) const noexcept {

            static constexpr bool no_routes       = route_count() == 0u;
            static constexpr bool past_last_route = Index > (route_count() - 1);

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
                                  { call_route(route, ctx, req) }
                                  ->stl::same_as<void>;
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


    /**
     * This is the router; the developers need this class to inject their routes
     * and also add more migrations.
     *
     * @param Interface
    template <typename... Route, typename RouteList = const_list<Route...>>
    struct router_t {
        template <typename... Args>
        constexpr router_t(Args&&... args) noexcept
          : routes(std::forward<Args>(args)...) {
        }


        template <typename Route>
        constexpr auto on(Route&& _route) noexcept {
            static_assert(is_route<Route>::value,
                          "The specified route is not valid.");


            if constexpr (is_specialization_of<RouteList, std::tuple>::value)
    {
                // when it's a tuple
                auto _tup =
                  std::tuple_cat(routes,
    std::make_tuple(std::move(_route))); return router_t<Interface,
    decltype(_tup)>{_tup};

            } else if constexpr (is_specialization_of<RouteList,
                                                      const_list>::value) {
                // for const_list (constexpr version)
                auto _the_routes = routes + std::move(_route);
                return router_t<Interface, decltype(_the_routes)>{_the_routes};

            } else if constexpr (is_container_v<RouteList>) {
                // for containers (dynamic)
                static_assert(
                  can_cast<Route, typename RouteList::value_type>::value,
                  "The specified route does not match the router version of "
                  "route.");
                routes.emplace_back(std::forward<Route>(_route));

            } else {
                throw std::invalid_argument(
                  "The container for routes is unknown.");
            }
        }
    };
     */




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


}; // namespace webpp

#endif // WEBPP_ROUTER_H
