#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "./http/request_concepts.h"
#include "./http/response_concepts.h"
#include "./routes/context_concepts.h"
#include "./routes/route_concepts.h"
#include "./routes/router_concepts.h"
#include "std/vector.h"

#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

namespace webpp {


    template <RouteExtension... ExtensionType>
    struct route_extension_pack : public ExtensionType... {

        template <typename ICT>
        struct initial_context_type {
            using type = typename ICT::template rebind<ExtensionType...>;
        };
    };


    template <Context InitialContextType, RouteExtensions ExtensionsType,
              Route... RouteType>
    struct const_router {

        // Add any additional "context extensions" that the "route extensions"
        // might want
        using initial_context_type_original =
          ::std::decay_t<InitialContextType>;
        using initial_context_type = ::std::conditional<
          has_initial_context_type<initial_context_type_original,
                                   ExtensionsType>,
          typename ExtensionsType::template initial_context_type<
            initial_context_type_original>,
          initial_context_type_original>;

        const ::std::tuple<RouteType...> routes;

        constexpr const_router(RouteType&&... _route) noexcept
          : routes(::std::forward<RouteType>(_route)...) {
        }


        /**
         * @return how many routes are in this router
         */
        constexpr auto size() const noexcept {
            return sizeof...(RouteType);
        }

        /**
         * Get the nth route
         * @tparam N
         * @param i
         * @return
         */
        template <::std::size_t N = 0>
        constexpr auto& operator[](::std::size_t i) const noexcept {
            if (N == i) {
                return ::std::get<N>(routes);
            }
            if constexpr (N + 1 < size()) {
                return operator[]<N + 1>(i);
            }
            throw ::std::invalid_argument("The specified index is not valid");
        }

        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <Response ResponseType>
        Response auto operator()(Request auto& req) noexcept {
            ResponseType res;
                         operator()(req, res);
            return res;
        }

        inline void operator()(Request auto& req, Response auto& res) noexcept {
            initial_context_type ctx{.request = req, .response = res};
                                 operator()(ctx);
        }

        inline void operator()(Context auto&& ctx) noexcept {
            ((call_route(
               ::std::get<RouteType>(std::forward<decltype(ctx)>(ctx)))) ||
             ...);
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
          : routes(::std::forward<Args>(args)...) {
        }


        template <typename Route>
        constexpr auto on(Route&& _route) noexcept {
            static_assert(is_route<Route>::value,
                          "The specified route is not valid.");


            if constexpr (is_specialization_of<RouteList, ::std::tuple>::value)
    {
                // when it's a tuple
                auto _tup =
                  ::std::tuple_cat(routes,
    ::std::make_tuple(::std::move(_route))); return router_t<Interface,
    decltype(_tup)>{_tup};

            } else if constexpr (is_specialization_of<RouteList,
                                                      const_list>::value) {
                // for const_list (constexpr version)
                auto _the_routes = routes + ::std::move(_route);
                return router_t<Interface, decltype(_the_routes)>{_the_routes};

            } else if constexpr (is_container_v<RouteList>) {
                // for containers (dynamic)
                static_assert(
                  can_cast<Route, typename RouteList::value_type>::value,
                  "The specified route does not match the router version of "
                  "route.");
                routes.emplace_back(::std::forward<Route>(_route));

            } else {
                throw ::std::invalid_argument(
                  "The container for routes is unknown.");
            }
        }
    };
     */




    /*
    struct dynamic_route {

      protected:
        // todo: maybe don't use ::std::function? it's slow a bit (but not that much)
        using callback_t = ::std::function<void()>;

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
