// Created by moisrex on 4/2/23.

#ifndef WEBPP_DYNAMIC_ROUTE_HPP
#define WEBPP_DYNAMIC_ROUTE_HPP

#include "../../traits/traits.hpp"
#include "valves.hpp"

namespace webpp::http {



    template <Traits TraitsType, typename Callable>
    struct dynamic_route final : dynamic_route<TraitsType, void> {
        using traits_type   = TraitsType;
        using string_type   = traits::string<traits_type>;
        using context_type  = basic_context<traits_type>;
        using callable_type = stl::remove_cvref_t<Callable>;
        using router_type   = basic_dynamic_router<traits_type>;

      private:
        callable_type callable;

      public:
        explicit(false) dynamic_route(Callable&& new_callable) noexcept : callable(stl::move(new_callable)) {}

        explicit(false) dynamic_route(Callable const& new_callable) noexcept(
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
        using string_type  = traits::string<traits_type>;
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

        /**
         * Utility to get a string more easily; this method should not be used in the library itself.
         */
        template <typename StrT = string_type, typename... Args>
            requires((!istl::String<Args> && ...))
        [[nodiscard]] StrT to_string(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }
    };



} // namespace webpp::http

#endif // WEBPP_DYNAMIC_ROUTE_HPP
