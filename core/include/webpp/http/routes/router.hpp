#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "../../extensions/extension.hpp"
#include "../../std/tuple.hpp"
#include "../../std/vector.hpp"
#include "../bodies/string.hpp"
#include "../request_concepts.hpp"
#include "../response_concepts.hpp"
#include "./context.hpp"
#include "./route_concepts.hpp"
#include "./router_concepts.hpp"

#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

namespace webpp {

    template <typename T>
    struct application_pointers {
        static constexpr bool value = stl::is_pointer_v<T> && stl::is_class_v<stl::remove_pointer_t<T>>;
    };


    /**
     * Const router is a router that satisfies that "Router" concept.
     *
     * @tparam InitialContextType
     * @tparam ExtensionListType
     * @tparam RouteType
     */
    template <ExtensionList ExtensionListType = empty_extension_pack, Route... RouteType>
    struct router {


        // todo: Additional routes extracted from the extensions
        //        using additional_routes = ;
        const stl::tuple<RouteType...> routes;

        constexpr router(RouteType&&... _route) noexcept : routes(stl::forward<RouteType>(_route)...) {
        }

        template <typename... AppTypes>
        requires((application_pointers<AppTypes>::value && ...))
          constexpr router(stl::tuple<AppTypes...> const& _apps, RouteType&&... _route) noexcept
          : routes(stl::forward<RouteType>(_route)...) {
            stl::apply(
              [this](auto&... _route) {
                  (..., (requires { {_route.app}; } && (_route.app = this)));
              },
              routes);
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

        auto error(Context auto const& ctx, status_code_type error_code,
                   stl::string_view phrase = "") const noexcept {
            // todo: add methods to change the default error template and individual ones
            stl::string_view _phrase = phrase.empty() ? status_reason_phrase(error_code) : phrase;
            return ctx.template response<string_response>(
              error_code,
              stl::format(
                R"html(<!doctype html><html><head><meta charset="utf-8"><title>{1} {2}!</title></head><body><h1>{1} {2}</h1></body></html>)html",
                error_code, _phrase));
        }

        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <typename RequestType>
        requires(Request<stl::remove_cvref_t<RequestType>>) Response auto
        operator()(RequestType& req) const noexcept {
            using req_type     = stl::remove_cvref_t<RequestType>;
            using context_type = simple_context<req_type, ExtensionListType>;
            return this->operator()(context_type{req});
        }

        template <typename ContextType, stl::size_t Index = 0>
        requires(Context<stl::remove_cvref_t<ContextType>>) Response auto
        operator()(ContextType&& ctx) const noexcept {

            if constexpr (sizeof...(RouteType) == 0) {
                return error(ctx, 404u);
            } else {

                // handling root-level route calls:
                using context_type              = stl::remove_cvref_t<ContextType>;
                constexpr auto next_route_index = Index + 1;
                constexpr auto route            = stl::get<Index>(routes);
                constexpr bool is_last_route    = Index == route_count() - 1;

                // setting the context features
                ctx.router_features.level            = router_stats::route_level::entryroute;
                ctx.router_features.last_entryroute  = is_last_route;
                ctx.router_features.entryroute_index = Index;

                auto res          = route(ctx);
                using result_type = decltype(res);

                if constexpr (Response<result_type>) {

                    // just call the context handlers
                    ctx.call_post_entryroute_methods();

                    // we're done; don't call the next route
                    return res;
                } else if constexpr (Context<result_type>) {
                    // context switching is happening here
                    // just call the next route or finish it with calling the
                    // context handlers
                    if constexpr (!is_last_route) {
                        return operator()<next_route_index>(stl::move(res));
                    } else {
                        // call the context handlers
                        ctx.call_post_entryroute_methods();
                        return error(ctx, 404u);
                    }
                } else {
                    // we just ignore anything else the user returns;
                    // todo: add a warning or log here so the user can find the issue
                }

                // call the next route:
                if constexpr (!is_last_route) {
                    return operator()<next_route_index>(stl::forward<context_type>(ctx));
                } else {
                    // call the context
                    ctx.call_post_entryroute_methods();
                    return error(ctx, 404u);
                }
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
