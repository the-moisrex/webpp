#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "valves/valve.h"
#include <algorithm>
#include <map>
#include <string>
#include <string_view>

namespace webpp {

    /**
     * @brief This route class contains one single root route and it's children
     */
    template <typename Interface>
    class route {
        response(migrator)(request_t<Interface>&);
        bool active = true;

      public:
        inline bool is_active() const noexcept {
            return active && !_migrations.empty();
        }

        inline route& activate() noexcept {
            active = true;
            return *this;
        }
        inline route& deactivate() noexcept {
            active = false;
            return *this;
        }
    };

    /**
     * This is the router; the developers need this class to inject their routes
     * and also add more migrations.
     *
     * @param Interface
     */
    template <typename Interface>
    class router {
        std::multimap<valves::valve<Interface>, route> routes;

      public:
        response run(request_t<Interface>&& req) {}

        router& on(route const& _route) noexcept {
            routes.emplace(valves::empty, _route);
            return *this;
        }

        router& on(route&& _route) noexcept {
            routes.emplace(valves::empty, std::move(_route));
            return *this;
        }

        router& on(valves::valve<Interface> const& v, route const& r) noexcept {
            routes.emplace(v, r);
            return *this;
        }

        router& on(valves::valve<Interface>&& v, route const& r) noexcept {
            routes.emplace(std::move(v), r);
            return *this;
        }

        router& on(valves::valve<Interface> const& v, route&& r) noexcept {
            routes.emplace(v, std::move(r));
            return *this;
        }

        router& on(valves::valve<Interface>&& v, route&& r) noexcept {
            routes.emplace(std::move(v), std::move(r));
            return *this;
        }
    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
