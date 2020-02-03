#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "valves/valve.h"
#include "utils/functional.h"
#include <type_traits>
#include <vector>

namespace webpp {

    // TODO: what should I do here?
    template<typename RequestType>
    void handle_exception(RequestType const &req) noexcept;

    /**
     * @brief This route class contains one single root route and it's children
     */
    template<typename Interface, typename Callable, typename Valve = valves::empty_t>
    class route : make_inheritable<Callable> {
        using req_t = request_t<Interface> const &;
        using res_t = response &;
        using condition_t = Valve;
        using callable = make_inheritable<Callable>;

        condition_t condition = valves::empty;

        // TODO: check for padding
        bool active = true;

        static_assert(std::is_invocable_v<callable, req_t, res_t>
                      || std::is_invocable_v<callable, req_t>
                      || std::is_invocable_v<callable, res_t>
                      || std::is_invocable_v<callable>, "We don't know how to call this callable you passed.");

    public:
        using callable::callable;
        using callable::operator();

        constexpr route() noexcept = default;

        constexpr route(Callable c) noexcept : callable(c) {}

        constexpr route(condition_t con, Callable c) noexcept : condition(std::move(con)), callable(c) {}

        constexpr route(route const &) noexcept = default;

        constexpr route(route &&) noexcept = default;

        /**
         * Check if the route is active
         */
        [[nodiscard]] inline bool is_active() const noexcept { return active; }

        /**
         * Reactivate the route
         */
        inline route &activate() noexcept {
            active = true;
            return *this;
        }

        /**
         * Deactivate the route
         */
        inline route &deactivate() noexcept {
            active = false;
            return *this;
        }

        /**
         * Run the migration
         * @return the response
         */
        inline void operator()(req_t req, res_t res) noexcept {

            // TODO: add more overrides. You can simulate "dependency injection" here

            if constexpr (std::is_invocable_v<callable, req_t>) {
                using RetType = std::invoke_result_t<callable, req_t>;
                if constexpr (!std::is_convertible_v<RetType, response>) {
                    if constexpr (std::is_nothrow_invocable_v<callable, req_t>) {
                        (void) callable::operator()(req);
                    } else {
                        try {
                            (void) callable::operator()(req);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                } else {
                    if constexpr (std::is_nothrow_invocable_v<callable, req_t>) {
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
                if constexpr (!std::is_convertible_v<RetType, response>) {
                    if constexpr (std::is_nothrow_invocable_v<callable, res_t>) {
                        (void) callable::operator()(res);
                    } else {
                        try {
                            (void) callable::operator()(res);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                } else {
                    // Yeah I know what it looks like. But we've got some stupid programmers out there!
                    if constexpr (std::is_nothrow_invocable_v<callable, res_t>) {
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
                if constexpr (!std::is_convertible_v<RetType, response>) {
                    if constexpr (std::is_nothrow_invocable_v<callable, req_t, res_t>) {
                        (void) callable::operator()(req, res);
                    } else {
                        try {
                            (void) callable::operator()(req, res);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                } else {
                    if constexpr (std::is_nothrow_invocable_v<callable, req_t, res_t>) {
                        res = callable::operator()(req, res);
                    } else {
                        try {
                            res = callable::operator()(req, res);
                        } catch (...) {
                            handle_exception(req);
                        }
                    }
                }
            } else {
                // This can't happen. So, chill
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


    /**
 * This is the router; the developers need this class to inject their routes
 * and also add more migrations.
 *
 * @param Interface
 */
    template<typename Interface, typename Routes>
    class router {

        // this is the main route which includes other routes:
        // This is a "const_list":
        Routes routes;

    public:
        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template<typename RequestType = request_t<Interface>,
                typename ResponseType = response>
        ResponseType run(RequestType &req) noexcept {
            // FIXME: make sure it's as performant as possible.
            ResponseType res;
            routes.for_each(
                    [&](auto const &_route) noexcept { _route.call(req, res); });
            return res;
        }

        template<typename Route>
        constexpr auto on(Route &&_route) const noexcept {
            return routes + route(valves::empty, std::forward<Route>(_route));
        }

        template<typename Valve, typename Route>
        constexpr auto on(Valve &&v, Route &&r) const noexcept {
            return routes +
                   route(std::forward<Valve>(v), std::forward<Route>(r));
        }
    };


}; // namespace webpp

#endif // WEBPP_ROUTER_H
