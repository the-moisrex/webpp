#ifndef WEBPP_HTTP_STATIC_ROUTER_HPP
#define WEBPP_HTTP_STATIC_ROUTER_HPP

#include "../../extensions/extension.hpp"
#include "../../std/tuple.hpp"
#include "../bodies/string.hpp"
#include "../http_concepts.hpp"
#include "context.hpp"
#include "router_concepts.hpp"
#include "valves.hpp"

namespace webpp::http {


    /**
     * Const router is a router that satisfies that "Router" concept.
     */
    template <typename... RouteType>
    struct static_router {

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        forward_valve<RouteType...> routes;
        // NOLINTEND(misc-non-private-member-variables-in-classes)

        constexpr static_router(RouteType&&... _route) : routes(stl::forward<RouteType>(_route)...) {}

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


} // namespace webpp::http

#endif // WEBPP_HTTP_STATIC_ROUTER_HPP
