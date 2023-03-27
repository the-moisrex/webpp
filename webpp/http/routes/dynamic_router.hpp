#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../extensions/extension.hpp"
#include "../../std/functional.hpp"
#include "../../std/memory.hpp"
#include "../../std/string.hpp"
#include "../../std/vector.hpp"
#include "../../traits/enable_traits.hpp"
#include "../http_concepts.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "../status_code.hpp"
#include "./context.hpp"
#include "valves.hpp"

#include <any>

namespace webpp::http {


    template <Traits>
    struct basic_dynamic_router;


    template <Traits TraitsType, typename Callable = void>
    struct dynamic_route final : dynamic_route<TraitsType, void> {
        using traits_type   = TraitsType;
        using string_type   = traits::general_string<traits_type>;
        using context_type  = basic_context<traits_type>;
        using callable_type = stl::remove_cvref_t<Callable>;
        using router_type   = basic_dynamic_router<traits_type>;

      private:
        callable_type callable;

      public:
        dynamic_route(Callable&& new_callable) noexcept : callable(stl::move(new_callable)) {}
        dynamic_route(Callable const& new_callable) noexcept(
          stl::is_nothrow_copy_constructible_v<callable_type>)
          : callable(new_callable) {}
        dynamic_route(dynamic_route const&)                     = delete;
        dynamic_route(dynamic_route&&) noexcept                 = default;
        dynamic_route& operator=(dynamic_route const&) noexcept = delete;
        dynamic_route& operator=(dynamic_route&&) noexcept      = default;
        ~dynamic_route() final                                  = default;

        void operator()(context_type& ctx) final {
            callable(ctx);
        }

        void operator()(context_type& ctx, [[maybe_unused]] router_type& router) final {
            if constexpr (stl::is_invocable_v<callable_type, context_type&, router_type&>) {
                callable(ctx, router);
            } else {
                callable(ctx);
            }
        }

        void to_string(string_type& out) const final {
            valve_to_string(out, callable);
        }


        void setup([[maybe_unused]] router_type& router) final {
            if constexpr (ValveRequiresSetup<router_type, callable_type>) {
                callable.setup(router);
            }
        }
    };


    template <Traits TraitsType>
    struct dynamic_route<TraitsType, void> {
        using traits_type  = TraitsType;
        using string_type  = traits::general_string<traits_type>;
        using context_type = basic_context<traits_type>;
        using router_type  = basic_dynamic_router<traits_type>;

        dynamic_route()                                         = default;
        dynamic_route(dynamic_route const&)                     = default;
        dynamic_route(dynamic_route&&) noexcept                 = default;
        dynamic_route& operator=(dynamic_route const&) noexcept = default;
        dynamic_route& operator=(dynamic_route&&) noexcept      = default;

        virtual ~dynamic_route() = default;

        virtual void operator()(context_type& ctx, [[maybe_unused]] router_type& router) = 0;
        virtual void operator()(context_type& ctx)                                       = 0;
        virtual void to_string(string_type& out) const                                   = 0;
        virtual void setup(router_type& out)                                             = 0;
    };


    /**
     * @brief A Router that's is fully customizable at runtime
     *
     * This class will be used directly by the developers using this whole library. So be nice and careful
     * and user friendly.
     */
    template <Traits TraitsType>
    struct basic_dynamic_router : enable_traits<TraitsType>, valve<TraitsType, void> {
        using valve_type         = valve<TraitsType, void>;
        using root_extensions    = empty_extension_pack;
        using traits_type        = TraitsType;
        using etraits            = enable_traits<traits_type>;
        using non_owner_etraits  = typename etraits::non_owner_type;
        using route_type         = dynamic_route<traits_type>;
        using dynamic_route_type = istl::dynamic<route_type, traits::general_allocator<traits_type>>;
        using vector_allocator   = traits::general_allocator<traits_type, dynamic_route_type>;
        using string_type        = traits::general_string<traits_type>;
        using string_view_type   = traits::string_view<traits_type>;
        using objects_type       = stl::vector<stl::any, traits::general_allocator<traits_type, stl::any>>;
        using routes_type        = stl::vector<dynamic_route_type, vector_allocator>;
        using response_type      = basic_response<traits_type>;
        using context_type       = basic_context<traits_type>;
        using request_type       = basic_request<traits_type>;

        static constexpr auto log_cat = "DRouter";

        static_assert(HTTPResponse<response_type>,
                      "For some reason the response type is not a valid match for HTTPResponse concept.");

      private:
        // todo: implement a function_vector that'll require only one allocator not one for each
        routes_type routes;


        /**
         * This method checks the context and see if we have reached the end of the routing or not.
         */
        template <typename CtxT>
        [[nodiscard]] constexpr bool continue_routing(CtxT& ctx) const noexcept {
            return ctx.response.empty();
        }

      public:
        // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)

        // These are the callable types
        objects_type objects;

        // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)

        constexpr basic_dynamic_router() noexcept
            requires(etraits::is_resource_owner)
          : etraits{},
            objects{alloc::general_alloc_for<objects_type>(*this)} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <EnabledTraits ET>
            requires(!istl::cvref_as<ET, basic_dynamic_router>)
        constexpr basic_dynamic_router(ET&& et)
          : etraits{stl::forward<ET>(et)},
            objects{alloc::general_alloc_for<objects_type>(*this)} {}
        // NOLINTEND(bugprone-forwarding-reference-overload)


        /**
         * Response with the specified status code.
         */
        constexpr response_type error(status_code code) {
            return response(code);
        }

        constexpr response_type response(status_code code) {
            return {this->get_traits(), code};
        }


        // Append a migration
        template <typename C>
        constexpr basic_dynamic_router& operator+=(C&& callable) {
            using callable_type  = typename route_optimizer<C>::type;
            using new_route_type = dynamic_route<traits_type, callable_type>;

            new_route_type route{stl::forward<C>(callable)};
            route.setup(*this);
            routes.emplace_back(stl::move(route));
            return *this;
        }


        /**
         * Calling this will get you a response.
         * Don't call this operator for your sub-routers; this will guarantees a response and your
         * parent router will stop processing its next routes because this router returned a 404 response.
         */
        template <HTTPRequest ReqType>
            requires(!Context<ReqType>)
        [[nodiscard]] constexpr response_type operator()(ReqType&& in_req) {
            context_type ctx{stl::forward<ReqType>(in_req)};

            // call the router with the specified context, fill the response
            this->operator()(ctx);

            // if it didn't fill the response:
            if (ctx.response.empty()) {
                // fill the response with 404 error page
                ctx.response = error(http::status_code::not_found);
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
                route->operator()(ctx);
                if (!continue_routing(ctx)) {
                    return;
                }
            }
        }

        constexpr void to_string(istl::String auto& out) const {
            for (auto& route : routes) {
                route->to_string(out);
                out.append("\n");
            }
        }


        template <istl::String StrT = string_type>
        constexpr StrT to_string() {
            StrT out{alloc::general_alloc_for<StrT>(*this)};
            to_string(out);
            return out;
        }
    };


    using dynamic_router = basic_dynamic_router<default_dynamic_traits>;

} // namespace webpp::http

#endif // WEBPP_DYNAMIC_ROUTER_HPP
