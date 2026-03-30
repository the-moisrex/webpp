// Created by moisrex on 4/2/23.

#ifndef WEBPP_DYNAMIC_ROUTE_HPP
#define WEBPP_DYNAMIC_ROUTE_HPP

#include "./router_concepts.hpp"
#include "./valves.hpp"

namespace webpp::http {



    template <typename Callable, istl::CharType CharT, Allocator AllocT = default_allocator_t<CharT>>
    struct dynamic_route final : dynamic_route<void, CharT, AllocT> {
        using callable_type  = Callable;
        using allocator_type = AllocT;
        using char_type      = CharT;
        using string_type    = stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>;
        using context_type   = basic_context<CharT, AllocT>;
        using router_type    = basic_dynamic_router<CharT, AllocT>;

        static_assert(stl::is_reference_v<callable_type>, "Remove the references.");

      private:
        callable_type callable;

      public:
        explicit(false) dynamic_route(Callable&& new_callable) noexcept : callable(stl::move(new_callable)) {}

        explicit(false) dynamic_route(Callable const& new_callable)
          noexcept(stl::is_nothrow_copy_constructible_v<callable_type>)
          : callable(new_callable) {}

        dynamic_route(dynamic_route const&)                     = delete;
        dynamic_route(dynamic_route&&) noexcept                 = default;
        dynamic_route& operator=(dynamic_route const&) noexcept = delete;
        dynamic_route& operator=(dynamic_route&&) noexcept      = default;
        ~dynamic_route()                                        = default;

        void operator()(context_type& ctx) {
            callable(ctx);
        }

        void operator()(context_type& ctx, [[maybe_unused]] router_type& router) {
            if constexpr (stl::is_invocable_v<callable_type, context_type&, router_type&>) {
                callable(ctx, router);
            } else {
                callable(ctx);
            }
        }

        void to_string(string_type& out) const {
            valve_to_string(out, callable);
        }

        void setup([[maybe_unused]] router_type& router) {
            if constexpr (ValveRequiresSetup<router_type, callable_type>) {
                callable.setup(router);
            }
        }
    };

    template <istl::CharType CharT, Allocator AllocT>
    struct dynamic_route<void, CharT, AllocT> {
        using callable_type  = void;
        using allocator_type = AllocT;
        using char_type      = CharT;
        using string_type    = stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>;
        using context_type   = basic_context<CharT, AllocT>;
        using router_type    = basic_dynamic_router<CharT, AllocT>;

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
        template <typename StrT = stl::basic_string<char, stl::char_traits<char>, default_allocator_t<char>>,
                  typename... Args>
            requires((!istl::String<Args> && ...))
        [[nodiscard]] StrT to_string(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }
    };



} // namespace webpp::http

#endif // WEBPP_DYNAMIC_ROUTE_HPP
