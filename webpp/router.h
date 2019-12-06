#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "valves/valve.h"
#include <type_traits>
#include <vector>

namespace webpp {

    template <typename Signature>
    class function_ref;

    template <typename Return, typename... Args>
    class function_ref<Return(Args...)> final {
      private:
        using signature_type = Return(void*, Args...);

        void* _ptr;
        Return (*_erased_fn)(void*, Args...);

      public:
        template <typename T,
                  typename = std::enable_if_t<
                      std::is_invocable<T&, Args...>{} &&
                      !std::is_same<std::decay_t<T>, function_ref>{}>>
        constexpr function_ref(T&& x) noexcept
            : _ptr{(void*)std::addressof(x)} {
            _erased_fn = [](void* ptr, Args... xs) -> Return {
                return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
                    std::forward<Args>(xs)...);
            };
        }

        constexpr decltype(auto) operator()(Args... xs) const
            noexcept(noexcept(_erased_fn(_ptr, std::forward<Args>(xs)...))) {
            return _erased_fn(_ptr, std::forward<Args>(xs)...);
        }
    };

    /**
     * @brief This route class contains one single root route and it's children
     */
    template <typename Interface, typename Valve>
    class route {
        using signature = void(request_t<Interface> const&, response&);
        using migrator_t = function_ref<signature>;
        using condition_t = Valve;
        using req_t = request_t<Interface>;

        migrator_t migrator;
        condition_t condition = valves::empty;
        bool active = true;

      public:
        constexpr route(migrator_t m) : migrator(std::move(m)) {}

        /**
         * This can be used to just have a side-effect. This cannot throw an
         * exception.
         * @tparam Callable void(request)
         * @param c
         */
        constexpr route(
            function_ref<void(request_t<Interface> const&)> c) noexcept
            : migrator([=](auto& req, auto& res) { c(req); }) {}

        /**
         * This will have a response. This cannot throw an
         * exception
         * @tparam Callable void(response)
         * @param c
         */
        constexpr route(function_ref<void(response)> const& c) noexcept
            : migrator([=](auto& req, auto& res) { c(res); }) {}

        constexpr route(condition_t con, migrator_t m)
            : condition(std::move(con)), migrator(std::move(m)) {}

        constexpr route(route const&) noexcept = default;
        constexpr route(route&&) noexcept = default;

        /**
         * Check if the route is active
         */
        inline bool is_active() const noexcept { return active; }

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
        [[nodiscard]] inline response
        operator()(request_t<Interface>&) noexcept {
            return migrator();
        }

        /**
         * Check if the specified request matches the valve condition
         * @param req
         * @return bool
         */
        [[nodiscard]] inline bool
        is_match(request_t<Interface> const& req) const noexcept {
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
        template <typename RequestType>
        response run(RequestType& req) noexcept {
            // FIXME: make sure it's as performant as possible.
            response res;
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
