#ifndef WEBPP_HTTP_STATIC_ROUTER_HPP
#define WEBPP_HTTP_STATIC_ROUTER_HPP

#include "valves.hpp"

namespace webpp::http {


    /**
     * Const router is a router that satisfies that "Router" concept.
     */
    template <typename... RouteType>
    struct static_router {
        using routes_type = forward_valve<RouteType...>;

      private:
        routes_type routes;

        constexpr void setup_routes() {
            istl::for_each_element(
              [router = this]<typename R>([[maybe_unused]] R& route) {
                  if constexpr (ValveRequiresSetup<static_router, R>) {
                      route.setup(*router);
                  }
              },
              routes.as_tuple());
        }

      public:
        constexpr static_router(RouteType&&... _route) : routes{stl::forward<RouteType>(_route)...} {
            setup_routes();
        }

        constexpr static_router(stl::tuple<RouteType...>&& inp_routes) : routes{stl::move(inp_routes)} {
            setup_routes();
        }

        constexpr static_router(stl::tuple<RouteType...> const& inp_routes) : routes{inp_routes} {
            setup_routes();
        }

        constexpr static_router(static_router const&) noexcept = default;
        constexpr static_router(static_router&&) noexcept      = default;
        constexpr ~static_router()                             = default;

        constexpr static_router& operator=(static_router&&) noexcept = default;
        constexpr static_router& operator=(static_router const&)     = default;

        /**
         * @return how many routes are in this router
         */
        [[nodiscard]] static constexpr stl::size_t route_count() noexcept {
            return sizeof...(RouteType);
        }

        /**
         * Get the nth route
         * @tparam N
         * @param i
         * @return
         */
        template <stl::size_t N = 0>
        consteval auto& operator[](stl::size_t i) const noexcept {
            if (N == i) {
                return stl::get<N>(routes);
            }
            if constexpr (N + 1 < route_count()) {
                return operator[]<N + 1>(i);
            }
            // The specified index is not valid
            stl::terminate();
        }

      public:
        constexpr routes_type const& get_routes() const noexcept {
            return routes;
        }

        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <HTTPRequest RequestType>
        constexpr HTTPResponse auto operator()(RequestType&& req) {
            using req_type     = stl::remove_cvref_t<RequestType>;
            using traits_type  = typename req_type::traits_type;
            using context_type = basic_context<traits_type>;
            context_type ctx{req};
            // ctx.current_route(routes); // todo: is there a more accurate way to set individual sub-routes?
            routes(ctx);
            // if it didn't fill the response:
            if (ctx.response.empty()) {
                // fill the response with 404 error page
                ctx.response = http::status_code::not_found;
            }
            return stl::move(ctx.response);
        }


        /**
         * Call the routes with the specified request and context.
         */
        template <Traits TraitsType>
        constexpr void operator()(basic_context<TraitsType>& ctx) {
            routes(ctx);
        }

        /**
         * Append a string representation of the routes
         */
        template <istl::String StrT>
        constexpr void to_string(StrT& out) const {
            routes.to_string(out);
        }

        /**
         * Get a string representation of the routes.
         */
        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT to_string(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }
    };

    template <typename... RouteType>
    static_router(RouteType&&...) -> static_router<RouteType...>;

    template <typename... RouteType>
    static_router(stl::tuple<RouteType...>&&) -> static_router<RouteType...>;

    template <typename... RouteType>
    static_router(stl::tuple<RouteType...> const&) -> static_router<RouteType...>;

    template <typename... RouteType>
    static_router(stl::tuple<RouteType...>&) -> static_router<RouteType...>;

} // namespace webpp::http

#endif // WEBPP_HTTP_STATIC_ROUTER_HPP
