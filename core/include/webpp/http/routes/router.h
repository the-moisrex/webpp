#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "../../extensions/extension.h"
#include "../../std/vector.h"
#include "../request_concepts.h"
#include "../response_concepts.h"
#include "./context.h"
#include "./route_concepts.h"
#include "./router_concepts.h"

#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

namespace webpp {


    /**
     * Const router is a router that satisfies that "Router" concept.
     *
     * @tparam InitialContextType
     * @tparam ExtensionListType
     * @tparam RouteType
     */
    template <ExtensionList ExtensionListType = empty_extension_pack,
              Route... RouteType>
    struct const_router {


        // todo: Additional routes extracted from the extensions
        //        using additional_routes = ;

        const stl::tuple<RouteType...> routes;

        constexpr const_router(RouteType&&... _route) noexcept
          : routes(stl::forward<RouteType>(_route)...) {
        }


        /**
         * @return how many routes are in this router
         */
        constexpr auto route_count() const noexcept {
            return sizeof...(RouteType);
        }

        /**
         * Get the nth route
         * @tparam N
         * @param i
         * @return
         */
        template <stl::size_t N = 0>
        constexpr auto& operator[](stl::size_t i) const noexcept {
            if (N == i) {
                return stl::get<N>(routes);
            }
            if constexpr (N + 1 < route_count()) {
                return operator[]<N + 1>(i);
            }
            throw stl::invalid_argument("The specified index is not valid");
        }

        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <Response ResponseType>
        Response auto operator()(Request auto& req) noexcept {
            using req_type = decltype(req);
            // we can pass req_type to the extensie_type as an extra argument
            using context_type =
              typename ExtensionListType::template extensie_type<
                req_type::traits_type, context_descriptor, req_type>;
            return this->operator()(context_type{req});
        }

        template <stl::size_t Index = 0>
        Response auto operator()(Context auto&& ctx) noexcept {
            // handling root-level route calls:
            using context_type              = decltype(ctx);
            constexpr auto next_route_index = Index + 1;
            constexpr auto route            = stl::get<Index>(routes);
            constexpr bool is_last_route    = Index != route_count() - 1;
            ctx.call_pre_subroute_methods();
            auto res = route(ctx);
            ctx.call_post_subroute_methods();
            using result_type = decltype(res);
            if constexpr (stl::is_void_v<result_type>) {
                // nothing to do!
            } else if constexpr (Response<result_type>) {
                ctx.response = res;

                // just call the context handlers
                ctx.call_post_entryroute_methods();

                // we're done; don't call the next route
                return;
            } else if constexpr (Context<result_type>) {
                // context switching is happening here
                // just call the next route or finish it with calling the
                // context handlers
                if constexpr (is_last_route) {
                    operator()<next_route_index>(stl::move(res));
                } else {
                    // call the context handlers
                    ctx.call_post_entryroute_methods();
                }
                return; // done; no more routing
            } else {
                throw stl::invalid_argument(
                  "Your route is not returning something that we understand.");
            }

            // call the next route:
            if constexpr (is_last_route) {
                operator()<next_route_index>(stl::forward<context_type>(ctx));
            } else {
                // call the context
                ctx.call_post_entryroute_methods();
            }
        }
    };

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
