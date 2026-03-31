#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../std/memory.hpp"
#include "../../std/string.hpp"
#include "../http_concepts.hpp"
#include "../status_code.hpp"
#include "./dynamic_route.hpp"

#include <any>
#include <vector>

namespace webpp::http {

    template <istl::CharType CharT, Allocator AllocT>
    struct basic_response;

    /**
     * @brief A Router that is fully customizable at runtime
     *
     * This class will be used directly by the developers using this whole library. So be nice and careful
     * and user-friendly.
     */
    template <istl::CharType CharT, Allocator AllocT>
    struct basic_dynamic_router : valve<void> {
        using valve_type         = valve<void>;
        using route_type         = dynamic_route<CharT, AllocT>;
        using dynamic_route_type = istl::dynamic<route_type, AllocT>;
        using vector_allocator   = typename stl::allocator_traits<AllocT>::template rebind_alloc<dynamic_route_type>;
        using string_type        = stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>;
        using string_view_type   = stl::basic_string_view<CharT>;
        using objects_type =
          stl::vector<stl::any, typename stl::allocator_traits<AllocT>::template rebind_alloc<stl::any>>;
        using objects_allocator_type = typename objects_type::allocator_type;
        using routes_type            = stl::vector<dynamic_route_type, vector_allocator>;
        using response_type          = basic_response<CharT, AllocT>;
        using context_type           = basic_context<CharT, AllocT>;

        static constexpr auto log_cat = "DRouter";

        static_assert(HTTPResponse<response_type>,
                      "For some reason the response type is not a valid match for HTTPResponse concept.");

      private:
        routes_type routes;

        /**
         * This method checks the context and see if we have reached the end of the routing or not.
         */
        template <typename CtxT>
        [[nodiscard]] static constexpr bool continue_routing(CtxT& ctx) noexcept {
            return ctx.response.empty();
        }

      public:
        /// These are the objects that will be used by the valves to make it
        /// easier for the user to pass member functions as valves in the routes.
        objects_type objects; // NOLINT(*-non-private-member-variables-in-classes)

        constexpr basic_dynamic_router() noexcept
          // do not use {} instead of () here, it'll construct an "any" object instead.
          : objects(get_allocator<objects_allocator_type>(*this)) {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        explicit constexpr basic_dynamic_router(AllocT inp_alloc = alloc)
          // do not use {} instead of () here, it'll construct an "any" object instead.
          : objects(inp_alloc) {}

        // NOLINTEND(bugprone-forwarding-reference-overload)


        /**
         * Get a response with the specified error code
         */
        constexpr response_type response(status_code const code) {
            return response_type{code};
        }

        /**
         * Response with the specified status code.
         */
        constexpr response_type error(status_code const code) {
            return response(code);
        }

        /// Append a migration
        template <typename C>
        constexpr basic_dynamic_router& operator+=(C&& callable) {
            using callable_type  = stl::remove_cvref_t<C>;
            using new_route_type = dynamic_route<callable_type, CharT, AllocT>;

            auto& route = routes.emplace_back(stl::type_identity<new_route_type>{}, stl::forward<C>(callable));
            route->setup(*this);
            return *this;
        }

        /**
         * Calling this will get you a response.
         * Don't call this operator for your sub-routers; this will guarantees a response and your
         * parent router will stop processing its next routes because this router returned a 404 response.
         */
        template <HTTPRequest ReqType>
        [[nodiscard]] constexpr response_type operator()(ReqType&& in_req) {
            context_type ctx{stl::forward<ReqType>(in_req)};

            // call the router with the specified context, fill the response
            this->operator()(ctx);

            // if it didn't fill the response:
            if (ctx.response.empty()) {
                // fill the response with 404 error page
                ctx.response = error(status_code::not_found);
            }

            return ctx.response;
        }

        using valve_type::operator();

        /**
         * Run the router with the specified context;
         * Sets the context's response if necessary
         *
         * This method does not set 404 error message at all, give you a chance to use this router as
         * a sub-router of a parent router and let the parent router to deal with these things.
         */
        constexpr void operator()(context_type& ctx) {
            for (auto& route : routes) {
                ctx.current_route(*route); // set the current route on context
                route->operator()(ctx);
                if (!continue_routing(ctx)) {
                    return;
                }
            }
            ctx.response = this->error(status_code::not_found);
        }

        constexpr void to_string(istl::String auto& out) const {
            for (auto& route : routes) {
                if constexpr (requires { route->to_string(out); }) {
                    route->to_string(out);
                } else {
                    // I know, looks not great, but to_string is a virtual call
                    string_type inout{get_alloc_for<string_type>(*this)};
                    to_string(inout);
                    out.append(stl::move(inout));
                }
                out.append("\n");
            }
        }

        template <istl::String StrT = string_type>
        [[nodiscard]] constexpr StrT to_string() const {
            StrT out{get_alloc_for<StrT>(*this)};
            to_string(out);
            return out;
        }
    };

    using dynamic_router = basic_dynamic_router<char>;

} // namespace webpp::http

#endif // WEBPP_DYNAMIC_ROUTER_HPP
