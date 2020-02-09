#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "utils/const_list.h"
#include "utils/containers.h"
#include "utils/functional.h"
#include "valves/valve.h"
#include <functional>
#include <tuple>
#include <type_traits>

namespace webpp {

    // TODO: what should I do here?
    template <typename RequestType>
    void handle_exception(RequestType const& req) noexcept {}

    template <typename T, typename U>
    struct can_cast
        : std::integral_constant<bool,
                                 (!std::is_void_v<T> && !std::is_void_v<U>)&&(
                                     std::is_convertible_v<T, U> ||
                                     std::is_constructible_v<T, U> ||
                                     std::is_assignable_v<T, U> ||
                                     std::is_convertible_v<U, std::string> ||
                                     std::is_constructible_v<U, std::string> ||
                                     std::is_assignable_v<U, std::string>)> {};

    template <typename T, typename U>
    inline constexpr bool can_convert_v = can_cast<T, U>::value;

    /**
     * @brief This route class contains one single root route and it's children
     */
    template <typename Interface, typename Callable,
              typename Valve = valves::empty_t>
    class route : make_inheritable<Callable> {
        using req_t = request_t<Interface> const&;
        using res_t = response&;
        using condition_t = Valve;
        using callable = make_inheritable<Callable>;

        condition_t condition = condition_t{};

        // TODO: check for padding
        bool active = true;

        //        static_assert(std::is_invocable_v<callable, req_t, res_t> ||
        //                          std::is_invocable_v<callable, req_t> ||
        //                          std::is_invocable_v<callable, res_t> ||
        //                          std::is_invocable_v<callable>,
        //                      "We don't know how to call this callable you
        //                      passed.");

      public:
        using callable::operator();

        template <typename... Args,
                  std::enable_if_t<std::is_constructible_v<callable, Args...>,
                                   int> = 0>
        constexpr route(Args&&... args) noexcept
            : callable{std::forward<Args>(args)...} {}

        template <typename... Args>
        constexpr route(condition_t con, Args&&... args) noexcept
            : condition(std::move(con)), callable{std::forward<Args>(args)...} {
        }

        constexpr route(Callable c) noexcept : callable(c) {}

        constexpr route(condition_t con) noexcept : condition(std::move(con)) {}

        //        constexpr route(condition_t con, Callable c) noexcept
        //            : condition(std::move(con)), callable(c) {}

        constexpr route(route const&) noexcept = default;

        constexpr route(route&&) noexcept = default;

        using callable::operator=;

        /**
         * Check if the route is active
         */
        [[nodiscard]] inline bool is_active() const noexcept { return active; }

        /**
         * Reactivate the route
         */
        inline route& activate() noexcept {
            active = true;
            return *this;
        }

        /**
         * Deactivate the route
         */
        inline route& deactivate() noexcept {
            active = false;
            return *this;
        }

        /**
         * Run the migration
         * @return the response
         */
        inline void operator()(req_t req, res_t res) noexcept {

            // TODO: add more overrides. You can simulate "dependency injection"
            // here

            if constexpr (std::is_invocable_v<callable, req_t>) {
                using RetType = std::invoke_result_t<callable, req_t>;
                if constexpr (!can_convert_v<RetType, response>) {
                    if constexpr (std::is_nothrow_invocable_v<callable,
                                                              req_t>) {
                        (void)callable::operator()(req);
                    } else {
                        try {
                            (void)callable::operator()(req);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                } else {
                    if constexpr (std::is_nothrow_invocable_v<callable,
                                                              req_t>) {
                        res = callable::operator()(req);
                    } else {
                        try {
                            res = callable::operator()(req);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                }
            } else if constexpr (std::is_invocable_v<callable, res_t>) {
                using RetType = std::invoke_result_t<callable, res_t>;
                if constexpr (!can_convert_v<RetType, response>) {
                    if constexpr (std::is_nothrow_invocable_v<callable,
                                                              res_t>) {
                        (void)callable::operator()(res);
                    } else {
                        try {
                            (void)callable::operator()(res);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                } else {
                    // Yeah, I know what it looks like, but we've got some
                    // stupid programmers out there!
                    if constexpr (std::is_nothrow_invocable_v<callable,
                                                              res_t>) {
                        res = callable::operator()(res);
                    } else {
                        try {
                            res = callable::operator()(res);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                }
            } else if constexpr (std::is_invocable_v<callable, req_t, res_t>) {
                using RetType = std::invoke_result_t<callable, req_t, res_t>;
                if constexpr (!can_convert_v<RetType, response>) {
                    if constexpr (std::is_nothrow_invocable_v<callable, req_t,
                                                              res_t>) {
                        (void)callable::operator()(req, res);
                    } else {
                        try {
                            (void)callable::operator()(req, res);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                } else {
                    if constexpr (std::is_nothrow_invocable_v<callable, req_t,
                                                              res_t>) {
                        res = callable::operator()(req, res);
                    } else {
                        try {
                            res = callable::operator()(req, res);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                }
            } else if constexpr (std::is_invocable_v<callable, res_t, req_t>) {
                using RetType = std::invoke_result_t<callable, res_t, req_t>;
                if constexpr (!can_convert_v<RetType, response>) {
                    if constexpr (std::is_nothrow_invocable_v<callable, res_t,
                                                              req_t>) {
                        (void)callable::operator()(res, req);
                    } else {
                        try {
                            (void)callable::operator()(res, req);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                } else {
                    if constexpr (std::is_nothrow_invocable_v<callable, res_t,
                                                              req_t>) {
                        res = callable::operator()(res, req);
                    } else {
                        try {
                            res = callable::operator()(res, req);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                }
            } else if (std::is_invocable_v<callable>) {
                using RetType = std::invoke_result_t<callable>;
                if constexpr (!can_convert_v<RetType, response>) {
                    if constexpr (std::is_nothrow_invocable_v<callable>) {
                        (void)callable::operator()();
                    } else {
                        try {
                            (void)callable::operator()();
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                } else {
                    if constexpr (std::is_nothrow_invocable_v<callable>) {
                        res = callable::operator()();
                    } else {
                        try {
                            res = callable::operator()();
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                }
            }
        }

        /**
         * Check if the specified request matches the valve condition
         * @param req
         * @return bool
         */
        [[nodiscard]] inline bool is_match(req_t req) const noexcept {
            return active && condition(req);
        }
    };

    template <typename Interface>
    using route_sigs = overloaded<
        std::function<void()>, std::function<void(request_t<Interface> const&)>,
        std::function<void(response&)>,
        std::function<void(request_t<Interface> const&, response&)>,
        std::function<void(response&, request_t<Interface> const&)>,
        std::function<response()>,
        std::function<response(request_t<Interface> const&)>,
        std::function<response(response&)>,
        std::function<response(request_t<Interface> const&, response&)>,
        std::function<response(response&, request_t<Interface> const&)>>;

    template <typename Interface>
    struct dynamic_route : public route<Interface, route_sigs<Interface>,
                                        valves::dynamic_valve<Interface>> {};

    /**
     * This is the router; the developers need this class to inject their routes
     * and also add more migrations.
     *
     * @param Interface
     */
    template <typename Interface, typename RouteList = const_list<>>
    class router {

        using req_t = request_t<Interface> const&;
        using res_t = response&;

        // this is the main route which includes other routes:
        // This is a "const_list":
        RouteList routes;

      public:
        template <typename... Args>
        constexpr router(Args&&... args) noexcept
            : routes(std::forward<Args>(args)...) {}

        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <typename RequestType = request_t<Interface>,
                  typename ResponseType = response>
        ResponseType operator()(RequestType& req) noexcept {
            // FIXME: make sure it's as performant as possible.
            ResponseType res;
            auto for_each_route_do = [&](auto& _route) noexcept {
                if (_route.is_match(req))
                    _route(req, res);
            };
            if constexpr (is_specialization_of<RouteList, const_list>::value) {
                // for webpp::const_list
                routes.for_each(for_each_route_do);
            } else if constexpr (is_container_v<RouteList>) {
                // for containers
                std::for_each(std::begin(routes), std::end(routes),
                              [&](auto& _route) {
                                  if constexpr (std::is_same_v<
                                                    decltype(_route),
                                                    dynamic_route<Interface>>) {
                                      for_each_route_do(_route);
                                  } else {
                                      // todo: what should I do here?
                                  }
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
        constexpr auto on(Route&& _route) const noexcept {
            return on(valves::empty, std::forward<Route>(_route));
        }

        template <typename Valve, typename Route>
        constexpr auto on(Valve&& v, Route&& r) noexcept {

            static_assert(
                std::is_invocable_v<Route, req_t, res_t> ||
                    std::is_invocable_v<route<Interface, Route>, req_t, res_t>,
                "The specified route is not valid.");

            auto _route = route<Interface, Route, Valve>{
                std::forward<Valve>(v), std::forward<Route>(r)};

            if constexpr (is_specialization_of<RouteList, std::tuple>::value) {

                // when it's a tuple
                auto _tup =
                    std::tuple_cat(routes, std::make_tuple(std::move(_route)));
                return router<Interface, decltype(_tup)>{_tup};

            } else if constexpr (is_specialization_of<RouteList,
                                                      const_list>::value) {
                // for const_list (constexpr version)
                auto _the_routes = routes + std::move(_route);
                return router<Interface, decltype(_the_routes)>{_the_routes};

            } else if constexpr (is_container_v<RouteList>) {

                // for containers (dynamic)
                if constexpr (can_cast<Route,
                                       typename RouteList::value_type>::value) {
                    routes.emplace_back(std::move(_route));
                } else {
                    // todo: error maybe?
                }

            } else {
                // todo: what should I do here?
            }
        }
    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
