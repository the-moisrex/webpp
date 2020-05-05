#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "routes/route.h"
#include "std/vector.h"
#include "utils/const_list.h"
#include "utils/containers.h"
#include "utils/functional.h"

#include <functional>
#include <map>
#include <string_view>
#include <tuple>
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
     *       proioritization technique.
     *   - Priorities of the entry route list:
     *       This table will be used to change the priority of the entry route
     *       list. This priority change is done through the context extensions.
     *       So the context extensions need a way to access the router. The best
     *       way to do so is to add this route itself to the base_context. This
     *       is possible because the constext is created initially by the
     *       router itself and the changes in its type should be done inside the
     *       router too.
     *
     */



    //
    // template <typename Interface>
    // using route_sigs = overloaded<
    // std::function<void()>, std::function<void(request_t<Interface> const&)>,
    // std::function<void(response&)>,
    // std::function<void(request_t<Interface> const&, response&)>,
    // std::function<void(response&, request_t<Interface> const&)>,
    // std::function<response()>,
    // std::function<response(request_t<Interface> const&)>,
    // std::function<response(response&)>,
    // std::function<response(request_t<Interface> const&, response&)>,
    // std::function<response(response&, request_t<Interface> const&)>>;
    //
    template <typename Traits, typename Interface>
    struct dynamic_route {
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits.");
        using traits    = Traits;
        using interface = Interface;

      protected:
        using req_t = request_t<Traits, Interface> const&;
        using res_t = response_t<Traits>&;
        // todo: maybe don't use std::function? it's slow a bit (but not that much)
        using callback_t  = std::function<void(req_t, res_t)>;

        callback_t  callback  = nullptr;

      public:
        // fixme: it gives me error when I put "noexcept" here:
        dynamic_route() = default;
        dynamic_route(callback_t callback) noexcept : callback(callback) {
        }

        template <typename C>
        dynamic_route& operator=(C&& callback) noexcept {
            this->callback = [=](req_t req, res_t res) noexcept {
                return call_it(callback, req, res);
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

    /**
     * This is the router; the developers need this class to inject their routes
     * and also add more migrations.
     *
     * @param Interface
     */
    template <typename Traits, typename Interface,
              typename RouteList =
                stl::vector<Traits, dynamic_route<Traits, Interface>>>
    class router_t {
      public:
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits type.");

        using traits    = Traits;
        using interface = Interface;

      private:
        using req_t_raw = request_t<traits, interface>;
        using res_t_raw = response_t<traits>;
        using req_t     = req_t_raw const&;
        using res_t     = res_t_raw&;

        // this is the main route which includes other routes:
        // This is a "const_list":
        RouteList routes;

      public:
        template <typename... Args>
        constexpr router_t(Args&&... args) noexcept
          : routes(std::forward<Args>(args)...) {
        }

        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <typename RequestType  = req_t_raw,
                  typename ResponseType = res_t_raw>
        ResponseType operator()(RequestType& req) noexcept {
            // FIXME: make sure it's as performant as possible.
            ResponseType res;
            auto         for_each_route_do = [&](auto& _route) noexcept {
                if (_route.is_match(req))
                    _route(req, res);
            };
            if constexpr (is_specialization_of<RouteList, const_list>::value) {
                // for webpp::const_list
                routes.for_each(for_each_route_do);
            } else if constexpr (is_container_v<RouteList>) {
                // for containers
                std::for_each(
                  std::begin(routes), std::end(routes), [&](auto& _route) {
                      static_assert(
                        std::is_same_v<std::decay_t<decltype(_route)>,
                                       dynamic_route<traits, interface>>,
                        "The specified type is not a dynamic_route.");
                      for_each_route_do(_route);
                  });
            } else if constexpr (is_specialization_of<RouteList,
                                                      std::tuple>::value) {
                std::apply(for_each_route_do, routes);
            } else {
                // todo: what should I do here?
            }
            return res;
        }

        template <typename Route>
        constexpr auto on(Route&& _route) noexcept {
            static_assert(is_route<Route>::value,
                          "The specified route is not valid.");


            if constexpr (is_specialization_of<RouteList, std::tuple>::value) {
                // when it's a tuple
                auto _tup =
                  std::tuple_cat(routes, std::make_tuple(std::move(_route)));
                return router_t<Interface, decltype(_tup)>{_tup};

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
                routes.emplace_back(std::forward<Valve>(v), std::move(_route));

            } else {
                throw std::invalid_argument(
                  "The container for routes is unknown.");
            }
        }
    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
