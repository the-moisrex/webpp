#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "std/string_view.h"
#include <algorithm>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>
#include <map>

/**
 *
 * Features of router:
 * - [ ]
 *
 */

namespace webpp {

    using matcher_t = std::function<bool(std::string_view)>;
    using migration_t = std::function<void(std::ostream&)>;
    using method_t = std::string;

    enum class migration_place {
        BEFORE_EVERYTHING, AFTER_EVERYTHING,
        END_OF_HEADER, BEFORE_BODY
    };

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
    public:

      private:
        matcher_t _matcher;
        std::multimap<migration_place, migration_t> _migrations;
        method_t _method;
        bool active = true;

      public:

        route(method_t __method, matcher_t __matcher)
            : _method(std::move(__method)), _matcher(std::move(__matcher))
        {}

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

        inline const auto& matcher() const noexcept { return _matcher; }
        inline route& matcher(const matcher_t& __matcher) noexcept {
            route::_matcher = __matcher;
            return *this;
        }
        inline const auto &migrations() const noexcept {
            return _migrations;
        }
        inline route& migrations(const decltype(_migrations) &__migrations) noexcept {
            route::_migrations = __migrations;
            return *this;
        }
        inline const auto& method() const noexcept { return _method; }
        route& method(const std::string& __method) noexcept {
            route::_method = __method;
            return *this;
        }

        bool is_match(std::string_view path) const noexcept {
            return _matcher(path);
        }

    };

    class router {
        std::vector<route> routes;

      public:
        template <typename Interface>
        response<Interface> run(request<Interface>& req) {
            auto the_route = find_route(req);
            response<Interface> res;
            return res;
        }


        template <typename ...Args>
        router& use(Args && ...args) noexcept {
            routes.emplace_back(std::forward<Args>(args)...); // use route's constructors
            return *this;
        }

        template <typename ...Args>
        auto& get(Args && ...args) noexcept {
            return use("GET", std::forward<Args>(args)...);
        }

        template <typename ...Args>
        auto& post(Args && ...args) noexcept {
            return use("POST", std::forward<Args>(args)...);
        }

        template <typename ...Args>
        auto& put(Args && ...args) noexcept {
            return use("PUT", std::forward<Args>(args)...);
        }

        template <typename ...Args>
        auto& patch(Args && ...args) noexcept {
            return use("PATCH", std::forward<Args>(args)...);
        }

        template <typename ...Args>
        auto& deletes(Args && ...args) noexcept { // There's a miss spell, I know.
            return use("DELETE", std::forward<Args>(args)...);
        }

        template <typename ...Args>
        auto& head(Args && ...args) noexcept {
            return use("HEAD", std::forward<Args>(args)...); // TODO: somehow prevent the body from printing
        }

    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
