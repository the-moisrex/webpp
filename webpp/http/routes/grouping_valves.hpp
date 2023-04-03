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




    template <typename... Pres, typename... Posts, typename... Manglers, typename... Routes>
    struct valves_group<prerouting_valve<Pres...>,
                        postrouting_valve<Posts...>,
                        mangler_valve<Manglers...>,
                        forward_valve<Routes...>> : valve<valves_group<prerouting_valve<Pres...>,
                                                                       postrouting_valve<Posts...>,
                                                                       mangler_valve<Manglers...>,
                                                                       forward_valve<Routes...>>> {

        using valve_type   = valve<valves_group>;
        using pre_type     = prerouting_valve<Pres...>;
        using post_type    = postrouting_valve<Posts...>;
        using mangler_type = mangler_valve<Manglers...>;
        using route_type   = forward_valve<Routes...>;

      private:
        [[no_unique_address]] pre_type     pres;
        [[no_unique_address]] post_type    posts;
        [[no_unique_address]] mangler_type manglers;
        [[no_unique_address]] route_type   routes;

      public:
        template <istl::cvref_as<pre_type>     PreT,
                  istl::cvref_as<post_type>    PostT,
                  istl::cvref_as<mangler_type> ManglerT,
                  istl::cvref_as<route_type>   RouteT>
        constexpr valves_group(PreT&& inp_pre, PostT&& inp_post, ManglerT&& inp_mangler, RouteT&& inp_route)
          : pres{stl::forward<PreT>(inp_pre)},
            posts{stl::forward<PostT>(inp_post)},
            manglers{stl::forward<ManglerT>(inp_mangler)},
            routes{stl::forward<RouteT>(inp_route)} {}

        constexpr valves_group(valves_group const&)                = default;
        constexpr valves_group(valves_group&&) noexcept            = default;
        constexpr valves_group& operator=(valves_group const&)     = default;
        constexpr valves_group& operator=(valves_group&&) noexcept = default;
        constexpr ~valves_group()                                  = default;


        using valve_type::operator();

        template <Traits TraitsType>
        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type   = basic_context<TraitsType>;
            using pre_traits     = valve_traits<pre_type, context_type>;
            using post_traits    = valve_traits<post_type, context_type>;
            using mangler_traits = valve_traits<mangler_type, context_type>;
            using route_traits   = valve_traits<route_type, context_type>;
            pre_traits::call_set(pres, ctx);
            route_traits::call_set(routes, ctx);
            post_traits::call_set(posts, ctx);
        }


        template <typename RouterT>
        constexpr void setup(RouterT& router) {
            if constexpr (ValveRequiresSetup<RouterT, pre_type>) {
                pres.setup(router);
            }
            if constexpr (ValveRequiresSetup<RouterT, mangler_type>) {
                manglers.setup(router);
            }
            if constexpr (ValveRequiresSetup<RouterT, route_type>) {
                routes.setup(router);
            }
            if constexpr (ValveRequiresSetup<RouterT, post_type>) {
                posts.setup(router);
            }
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto append_preroute(Callable&& callable) {
            using callable_type = stl::remove_cvref_t<Callable>;
            using new_pre_type  = prerouting_valve<Pres..., Callable>;
            return valves_group<new_pre_type, post_type, mangler_type, route_type>{
              new_pre_type{stl::tuple_cat(pres.as_tuple(), stl::forward_as_tuple<Callable>(callable))},
              posts,
              manglers,
              routes};
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto append_postroute(Callable&& callable) {
            using callable_type = stl::remove_cvref_t<Callable>;
            using new_post_type = postrouting_valve<Posts..., Callable>;
            return valves_group<pre_type, new_post_type, mangler_type, route_type>{
              pres,
              new_post_type{stl::tuple_cat(posts.as_tuple(), stl::forward_as_tuple<Callable>(callable))},
              manglers,
              routes};
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto append_route(Callable&& callable) {
            using callable_type  = stl::remove_cvref_t<Callable>;
            using new_route_type = forward_valve<Routes..., Callable>;
            return valves_group<pre_type, post_type, mangler_type, new_route_type>{
              pres,
              posts,
              manglers,
              new_post_type{stl::tuple_cat(routes.as_tuple(), stl::forward_as_tuple<Callable>(callable))}};
        }


        constexpr void to_string(istl::String auto& out) const {
            out.append("- (");
            valve_to_string(out, pres);
            out.append(")");
            valve_to_string(out, routes);
            out.append("+ (");
            valve_to_string(out, posts);
            out.append(")");
        }
    };

} // namespace webpp::http

#endif // WEBPP_GROUPING_VALVES_HPP
