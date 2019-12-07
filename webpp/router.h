#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "valves/valve.h"
#include <type_traits>
#include <vector>

namespace webpp {

    // TODO: what should I do here?
    template <typename RequestType>
    void handle_exception(RequestType const& req) noexcept;

    /**
     * @brief This route class contains one single root route and it's children
     */
    template <typename RequestType, typename ResponseType, typename Valve,
              typename Callable = void(RequestType const&, ResponseType&),
              typename = std::enable_if_t<
                  !std::is_convertible_v<RequestType, ResponseType>>>
    class route {
        using req_t = RequestType;
        using res_t = ResponseType;
        using signature = void(req_t const&, res_t&);
        using condition_t = Valve;

        Callable migrator;
        condition_t condition = valves::empty;
        bool active = true;

      public:
        /**
         * This overload is used for migrations that have only side-effects.
         * The callable can throw exception and they will be handled properly.
         * @tparam Callable
         * @param c
         */
        template <typename C,
                  std::enable_if_t<std::is_invocable_v<C, req_t> &&
                                       !std::is_nothrow_invocable_v<C, req_t>,
                                   int> = 0>
        constexpr explicit route(C const& c) noexcept
            : migrator([=](auto& req, auto&) noexcept {
                  try {
                      c(req);
                  } catch (...) {
                      handle_exception(req);
                  }
              }) {}

        /**
         * This overload is used for migrations that have only side-effects.
         * This overload will only be used if the callable is marked as noexcept
         * @tparam Callable
         * @param c
         */
        template <typename C,
                  std::enable_if_t<
                      std::is_nothrow_invocable_r_v<void, C, req_t>, int> = 0>
        constexpr explicit route(C const& c) noexcept
            : migrator([=](auto& req, auto&) noexcept { c(req); }) {}

        /**
         * This overload is used for migrations that don't need the request in
         * order to produce the response.
         * This overload will handle those callables that are not marked
         * noexcept too.
         * @tparam Callable
         * @param c
         */
        template <typename C,
                  std::enable_if_t<std::is_invocable_v<C, res_t> &&
                                       !std::is_nothrow_invocable_v<C, res_t>,
                                   int> = 0>
        constexpr explicit route(C const& c) noexcept
            : migrator([=](auto& req, auto& res) noexcept {
                  try {
                      c(res);
                  } catch (...) {
                      handle_exception(req);
                  }
              }) {}

        /**
         * This overload is used for when the callable accepts both request and
         * response and also might throw an exception.
         * @tparam C
         * @param c
         */
        template <
            typename C,
            std::enable_if_t<std::is_invocable_v<C, req_t, res_t> &&
                                 !std::is_nothrow_invocable_v<C, req_t, res_t>,
                             int> = 0>
        constexpr explicit route(C const& c) noexcept
            : migrator([=](auto& req, auto& res) noexcept {
                  try {
                      c(req, res);
                  } catch (...) {
                      handle_exception(req);
                  }
              }) {}

        /**
         * This overload is used for when the callable accepts both request and
         * response as the input and also the callable is marked as noexcept.
         * @tparam C
         * @param c
         */
        template <typename C,
                  std::enable_if_t<std::is_nothrow_invocable_v<C, req_t, res_t>,
                                   int> = 0>
        constexpr explicit route(C c) noexcept : migrator(std::move(c)) {}
        /**
         * This overload is used for migrations that have only side-effects.
         * The callable can throw exception and they will be handled properly.
         * @tparam Callable
         * @param c
         */
        template <typename C,
                  std::enable_if_t<std::is_invocable_v<C, req_t> &&
                                       !std::is_nothrow_invocable_v<C, req_t>,
                                   int> = 0>
        constexpr explicit route(condition_t con, C const& c) noexcept
            : condition(std::move(con)),
              migrator([=](auto& req, auto&) noexcept {
                  try {
                      c(req);
                  } catch (...) {
                      handle_exception(req);
                  }
              }) {}

        /**
         * This overload is used for migrations that have only side-effects.
         * This overload will only be used if the callable is marked as noexcept
         * @tparam Callable
         * @param c
         */
        template <typename C,
                  std::enable_if_t<
                      std::is_nothrow_invocable_r_v<void, C, req_t>, int> = 0>
        constexpr explicit route(condition_t con, C const& c) noexcept
            : condition(std::move(con)),
              migrator([=](auto& req, auto&) noexcept { c(req); }) {}

        /**
         * This overload is used for migrations that don't need the request in
         * order to produce the response.
         * This overload will handle those callables that are not marked
         * noexcept too.
         * @tparam Callable
         * @param c
         */
        template <typename C,
                  std::enable_if_t<std::is_invocable_v<C, res_t> &&
                                       !std::is_nothrow_invocable_v<C, res_t>,
                                   int> = 0>
        constexpr explicit route(condition_t con, C const& c) noexcept
            : condition(std::move(con)),
              migrator([=](auto& req, auto& res) noexcept {
                  try {
                      c(res);
                  } catch (...) {
                      handle_exception(req);
                  }
              }) {}

        /**
         * This overload is used for when the callable accepts both request and
         * response and also might throw an exception.
         * @tparam C
         * @param con
         * @param c
         */
        template <
            typename C,
            std::enable_if_t<std::is_invocable_v<C, req_t, res_t> &&
                                 !std::is_nothrow_invocable_v<C, req_t, res_t>,
                             int> = 0>
        constexpr explicit route(condition_t con, C const& c) noexcept
            : condition(std::move(con)),
              migrator([=](auto& req, auto& res) noexcept {
                  try {
                      c(req, res);
                  } catch (...) {
                      handle_exception(req);
                  }
              }) {}

        /**
         * This overload is used for when the callable accepts both request and
         * response as the input and also the callable is marked noexcept.
         * @tparam C
         * @param con
         * @param c
         */
        template <typename C,
                  std::enable_if_t<std::is_nothrow_invocable_v<C, req_t, res_t>,
                                   int> = 0>
        constexpr explicit route(condition_t con, C c) noexcept
            : condition(std::move(con)), migrator(std::move(c)) {}

        constexpr route(route const&) noexcept = default;
        constexpr route(route&&) noexcept = default;

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
        inline void operator()(req_t& req, res_t& res) noexcept {
            return migrator(req, res);
        }

        /**
         * Check if the specified request matches the valve condition
         * @param req
         * @return bool
         */
        [[nodiscard]] inline bool is_match(req_t const& req) const noexcept {
            return active && condition(req);
        }
    };

    /**
     * This is the router; the developers need this class to inject their routes
     * and also add more migrations.
     *
     * @param Interface
     */
    template <typename Interface, typename Routes>
    class router_t {

        // this is the main route which includes other routes:
        // This is a "const_list":
        Routes routes;

      public:
        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <typename RequestType, typename ResponseType = response>
        ResponseType run(RequestType& req) noexcept {
            // FIXME: make sure it's as performant as possible.
            ResponseType res;
            routes.for_each(
                [&](auto const& _route) noexcept { _route.call(req, res); });
            return res;
        }

        constexpr auto on(route const& _route) const noexcept {
            return routes + route(valves::empty, _route);
        }

        constexpr auto on(route&& _route) const noexcept {
            return routes + route(valves::empty, std::move(_route));
        }

        constexpr auto on(valves::valve<Interface> const& v,
                          route const& r) const noexcept {
            return routes + route(v, r);
        }

        constexpr auto on(valves::valve<Interface>&& v, route const& r) const
            noexcept {
            return routes + route(std::move(v), r);
        }

        constexpr auto on(valves::valve<Interface> const& v, route&& r) const
            noexcept {
            return routes + route(v, std::move(r));
        }

        constexpr auto on(valves::valve<Interface>&& v, route&& r) const
            noexcept {
            return routes + route(std::move(v), std::move(r));
        }
    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
