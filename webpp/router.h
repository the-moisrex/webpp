#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "std/string_view.h"
#include <algorithm>
#include <functional>
#include <map>
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

    using matcher_t = std::function<bool(std::string_view)>;
    using method_t = std::string;

    /**
     * Places that the migrations will be run.
     */
    enum class migration_place {
        BEFORE_EVERYTHING, // before sending headers and running routes
        NORMAL,            // A normal route
        END_OF_HEADER,     // After the header is sent out
        BEFORE_BODY,       // Before the first character of body is sent
        AFTER_EVERYTHING,  // After everything
    };

    /**
     * This class stands for any migration type. I made this a class and not
     * just a std::function type so that the developers can use it's
     * constructors to use other type of functions.
     * @tparam Interface
     */
    template <typename Interface>
    class migration_t {
      public:
        using func_t = std::function<response(request const&)>;

        // func: response(request)
        explicit migration_t(func_t f) noexcept : func(std::move(f)) {}

        // func: void(request)
        explicit migration_t(
            std::function<void(request const&)> const& f) noexcept
            : func([&](auto const& req) {
                  f(req);
                  return ::webpp::request();
              }) {}

        // func: void(void)
        explicit migration_t(std::function<void(void)> const& f) noexcept
            : func([&](auto const& /* req */) {
                  f();
                  return ::webpp::request();
              }) {}

        // func: void(request, response)
        explicit migration_t(
            std::function<void(request const&, request&)> const& f) noexcept
            : func([&](auto const& req) {
                  auto res = ::webpp::request();
                  f(req, res);
                  return res;
              }) {}

        // func: response(request, response)
        // TODO: it's redundant, think about it's usage more
        explicit migration_t(
            std::function<request(request const&, request&)> const& f) noexcept
            : func([&](auto const& req) {
                  auto res = ::webpp::request();
                  return f(req, res);
              }) {}

        // TODO: add more constructors here; and try to use std::bind

        request operator()(request const& req) const noexcept {
            return func(req);
        }

      private:
        func_t func;
    };

    namespace matchers {

        struct path {
            std::string slug;

            bool operator()(std::string_view _slug) const noexcept {
                return _slug == path::slug;
            }

            bool operator()(path const& _path) const noexcept {
                return _path == *this;
            }

            bool operator==(std::string_view _slug) const noexcept {
                return _slug == path::slug;
            }

            bool operator==(path const& _path) const noexcept {
                return _path.slug == path::slug;
            }

            bool operator!=(path const& _path) const noexcept {
                return _path.slug != path::slug;
            }

            bool operator!=(std::string_view _slug) const noexcept {
                return _slug != path::slug;
            }
        };

        // TODO: overload operators here

    } // namespace matchers

    /**
     * @brief This route class contains one single root route and it's children
     */
    class route {

      private:
        matcher_t _matcher;
        std::multimap<migration_place, migration_t<Interface>> _migrations;
        method_t _method;
        bool active = true;

      public:
        route(method_t __method, matcher_t __matcher,
              migration_t<Interface> _what_to_do)
            : _method(std::move(__method)), _matcher(std::move(__matcher)) {
            _migrations.emplace(migration_place::NORMAL,
                                std::move(_what_to_do));
        }

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
        inline const auto& migrations() const noexcept { return _migrations; }
        inline route&
        migrations(const decltype(_migrations)& __migrations) noexcept {
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

    /**
     * This is the router; the developers need this class to inject their routes
     * and also add more migrations.
     *
     * @param Interface
     */
    template <typename Interface>
    class router {
        std::vector<route<Interface>> routes;

      public:
        request run(request& req) {
            // auto the_route = find_route(req);
            // request res;
            // return res;
        }

        template <typename... Args>
        router& use(Args&&... args) noexcept {
            routes.emplace_back(
                std::forward<Args>(args)...); // use route's constructors
            return *this;
        }

        template <typename... Args>
        auto& get(Args&&... args) noexcept {
            return use("GET", std::forward<Args>(args)...);
        }

        template <typename... Args>
        auto& post(Args&&... args) noexcept {
            return use("POST", std::forward<Args>(args)...);
        }

        template <typename... Args>
        auto& put(Args&&... args) noexcept {
            return use("PUT", std::forward<Args>(args)...);
        }

        template <typename... Args>
        auto& patch(Args&&... args) noexcept {
            return use("PATCH", std::forward<Args>(args)...);
        }

        template <typename... Args>
        auto&
        deletes(Args&&... args) noexcept { // There's a miss spell, I know.
            return use("DELETE", std::forward<Args>(args)...);
        }

        template <typename... Args>
        auto& head(Args&&... args) noexcept {
            // TODO: somehow prevent the body from printing (it should be
            // implemented in the interfaces and not here; because of the
            // multi-threading stuff and generally it's low level stuff that it
            // has to do)
            return use("HEAD", std::forward<Args>(args)...);
        }

        /**
         * Add a migration to all routes
         */
        void add_migration(
            migration_t<Interface> const&,
            migration_place place = migration_place::AFTER_EVERYTHING) noexcept;
    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
