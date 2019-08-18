#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

/**
 *
 * Features of router:
 * - [ ]
 *
 */

namespace webpp {

    using matcher_t = std::function<bool(std::string const&)>;
    using migration_t = std::function<void(std::ostream&)>;

    namespace matchers {
        class path {
          private:
            std::string slug;

          public:
            path(std::string slug) : slug(std::move(slug)) {}
            bool operator()(std::string const& slug) {
                return slug == this->slug;
            }
        };
    } // namespace matchers

    /**
     * @brief This route class contains one single root route and it's children
     */
    class route {
      private:
        bool active = false;
        std::string method = "GET";
        std::vector<std::shared_ptr<route>> subroutes;
        matcher_t matcher;
        std::vector<migration_t> migrations;
        std::shared_ptr<route> parent;

      public:
        route(std::string method,
              std::shared_ptr<route> const& parent = nullptr,
              bool active = false)
            : method(std::move(method)), parent(parent), active(active) {}

        inline bool is_active() const noexcept {
            return active && !migrations.empty();
        }

        inline void activate() noexcept { active = true; }
        inline void diactivate() noexcept { active = false; }

        bool match(std::string const& path) const noexcept {}

        route* find(std::string const& path) noexcept {}
    };

    class router {
        std::vector<std::shared_ptr<route>> root_routes;

      public:
        template <class Interface>
        void run(::webpp::request<Interface>& req);
    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
