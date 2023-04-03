// Created by moisrex on 4/3/23.

#ifndef WEBPP_GROUPING_VALVES_HPP
#define WEBPP_GROUPING_VALVES_HPP

#include "valve_traits.hpp"

namespace webpp::http {



    template <typename... PreRoutes>
    struct prerouting_valve : forward_valve<PreRoutes...> {
        using forward_valve<PreRoutes...>::forward_valve;
    };

    template <typename... PostRoutes>
    struct postrouting_valve : forward_valve<PostRoutes...> {
        using forward_valve<PostRoutes...>::forward_valve;
    };



    template <typename NextCallable, typename ManglerType>
    struct mangler_valve : valve<mangler_valve<ManglerType, NextCallable>> {
        using valve_type = valve<mangler_valve<ManglerType, NextCallable>>;

      private:
        [[no_unique_address]] struct next_callable {
          private:
            [[no_unique_address]] NextCallable next;

          public:
            constexpr next_callable(NextCallable&& inp_next) noexcept(
              stl::is_nothrow_move_constructible_v<NextCallable>)
              : next{stl::move(inp_next)} {}
            constexpr next_callable(NextCallable const& inp_next) noexcept(
              stl::is_nothrow_copy_constructible_v<NextCallable>)
              : next{inp_next} {}


            [[nodiscard]] NextCallable const& get_next() const noexcept {
                return next;
            }

            [[nodiscard]] NextCallable& get_next() noexcept {
                return next;
            }

            template <Traits TraitsType>
            constexpr bool operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;
                return valve_traits<NextCallable, context_type>::call_set_get(next, ctx);
            }
        } next;

        [[no_unique_address]] ManglerType mangler;

      public:
        constexpr mangler_valve(NextCallable&& inp_next, ManglerType&& inp_mangler) noexcept(
          stl::is_nothrow_move_constructible_v<NextCallable>&&
            stl::is_nothrow_move_constructible_v<ManglerType>)
          : next{stl::move(inp_next)},
            mangler{stl::move(inp_mangler)} {}
        constexpr mangler_valve(NextCallable const& inp_next, ManglerType const& inp_mangler) noexcept(
          stl::is_nothrow_copy_constructible_v<NextCallable>&&
            stl::is_nothrow_copy_constructible_v<ManglerType>)
          : next{inp_next},
            mangler{inp_mangler} {}

        constexpr mangler_valve(mangler_valve const&)                     = default;
        constexpr mangler_valve(mangler_valve&&) noexcept                 = default;
        constexpr mangler_valve& operator=(mangler_valve&&) noexcept      = default;
        constexpr mangler_valve& operator=(mangler_valve const&) noexcept = default;
        constexpr ~mangler_valve()                                        = default;


        using valve_type::operator();

        template <Traits TraitsType>
        constexpr void operator()(basic_context<TraitsType>& ctx) {
            mangler(ctx, next_route<TraitsType>{next});
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" (");
            valve_to_string(out, next.get_next());
            out.append(" ) % ");
            valve_to_string(out, mangler);
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, NextCallable> || ValveRequiresSetup<RouterT, ManglerType>)
        constexpr void setup(RouterT& router) {
            if constexpr (ValveRequiresSetup<RouterT, NextCallable>) {
                next.get_next().setup(router);
            }
            if constexpr (ValveRequiresSetup<RouterT, ManglerType>) {
                mangler.setup(router);
            }
        }
    };


    template <typename... Callables>
    struct forward_valve : valve<forward_valve<Callables...>>, stl::tuple<Callables...> {
        using valve_type = valve<forward_valve<Callables...>>;
        using tuple_type = stl::tuple<Callables...>;

        template <typename... Args>
            requires stl::constructible_from<tuple_type, Args...>
        constexpr forward_valve(Args&&... args) noexcept(stl::is_nothrow_constructible_v<tuple_type, Args...>)
          : tuple_type{stl::forward<Args>(args)...} {}

        constexpr forward_valve(forward_valve const&)                     = default;
        constexpr forward_valve(forward_valve&&) noexcept                 = default;
        constexpr forward_valve& operator=(forward_valve&&) noexcept      = default;
        constexpr forward_valve& operator=(forward_valve const&) noexcept = default;
        constexpr ~forward_valve()                                        = default;

        using valve_type::operator();

        template <Traits TraitsType>
        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            stl::apply(
              [&ctx]<typename... T>(T&&... funcs) constexpr {
                  (valve_traits<T, context_type>::call_set(stl::forward<T>(funcs), ctx), ...);
              },
              as_tuple());
        }

        [[nodiscard]] constexpr tuple_type& as_tuple() noexcept {
            return *static_cast<tuple_type*>(this);
        }

        [[nodiscard]] constexpr tuple_type const& as_tuple() const noexcept {
            return *static_cast<tuple_type const*>(this);
        }

        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... funcs) constexpr {
                  ((out.append(" >> ("), valve_to_string(out, funcs), out.append(" )")), ...);
              },
              as_tuple());
        }

        template <typename RouterT>
            requires((ValveRequiresSetup<RouterT, Callables> || ...))
        constexpr void setup(RouterT& inp_router) {
            stl::apply(
              [&inp_router]<typename... T>(T&&... inp_callables) constexpr {
                  (([]([[maybe_unused]] auto&& callable, [[maybe_unused]] RouterT& router) constexpr {
                       if constexpr (ValveRequiresSetup<RouterT, T>) {
                           callable.setup(router);
                       }
                   })(inp_callables, inp_router),
                   ...);
              },
              as_tuple());
        }
    };

} // namespace webpp::http

#endif // WEBPP_GROUPING_VALVES_HPP
