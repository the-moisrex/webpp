// Created by moisrex on 4/3/23.

#ifndef WEBPP_GROUPING_VALVES_HPP
#define WEBPP_GROUPING_VALVES_HPP

#include "valve_traits.hpp"

namespace webpp::http {



    template <typename... PreRoutes>
    struct prerouting_valve : forward_valve<PreRoutes...> {
        using forward_valve<PreRoutes...>::forward_valve;
        constexpr prerouting_valve(prerouting_valve const&)                = default;
        constexpr prerouting_valve(prerouting_valve&&) noexcept            = default;
        constexpr prerouting_valve& operator=(prerouting_valve const&)     = default;
        constexpr prerouting_valve& operator=(prerouting_valve&&) noexcept = default;
        constexpr ~prerouting_valve()                                      = default;
    };

    template <typename... PostRoutes>
    struct postrouting_valve : forward_valve<PostRoutes...> {
        using forward_valve<PostRoutes...>::forward_valve;
        constexpr postrouting_valve(postrouting_valve const&)                = default;
        constexpr postrouting_valve(postrouting_valve&&) noexcept            = default;
        constexpr postrouting_valve& operator=(postrouting_valve const&)     = default;
        constexpr postrouting_valve& operator=(postrouting_valve&&) noexcept = default;
        constexpr ~postrouting_valve()                                       = default;
    };

    template <typename... ManglerType>
    struct mangler_valve : valve<mangler_valve<ManglerType...>> {
        using valve_type = valve<mangler_valve<ManglerType...>>;
        using tuple_type = stl::tuple<ManglerType...>;

      private:
        tuple_type manglers;

        template <typename NextCallable>
        struct next_callable {
          private:
            tuple_type*   manglers_ptr;
            NextCallable* next;

          public:
            constexpr next_callable(tuple_type& inp_manglers, NextCallable& inp_next) noexcept
              : manglers_ptr{&inp_manglers},
                next{&inp_next} {}

            // to make sure it matches the "basic_next_route"'s return type
            template <Traits TraitsType>
            constexpr void call_next(basic_context<TraitsType>& ctx) const {
                valve_traits<NextCallable, basic_context<TraitsType>>::call_set(*next, ctx);
            }

            template <Traits TraitsType, stl::size_t Index = 0>
            constexpr void operator()(basic_context<TraitsType>& ctx) const {
                if constexpr (Index == stl::tuple_size_v<tuple_type> - 1) {
                    (stl::get<stl::tuple_size_v<tuple_type> - 1>(*manglers_ptr))(
                      ctx,
                      basic_next_route<TraitsType>{*this,
                                                   &next_callable<NextCallable>::call_next<TraitsType>});
                } else if constexpr (stl::tuple_size_v<tuple_type> != 0) {
                    (stl::get<Index>(*manglers_ptr))(
                      ctx,
                      basic_next_route<TraitsType>{
                        *this,
                        &next_callable<NextCallable>::operator()<TraitsType, Index + 1>});
                }
            }
        };


      public:
        template <typename... Args>
            requires stl::constructible_from<tuple_type, Args...>
        explicit constexpr mangler_valve(Args&&... args) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, Args...>)
          : manglers{stl::forward<Args>(args)...} {}

        constexpr mangler_valve(mangler_valve const&)                     = default;
        constexpr mangler_valve(mangler_valve&&) noexcept                 = default;
        constexpr mangler_valve& operator=(mangler_valve&&) noexcept      = default;
        constexpr mangler_valve& operator=(mangler_valve const&) noexcept = default;
        constexpr ~mangler_valve()                                        = default;

        using valve_type::operator();

        template <Traits TraitsType, typename NextCallable>
        constexpr bool operator()(basic_context<TraitsType>& ctx, NextCallable&& next) {
            if constexpr (sizeof...(ManglerType) == 0) {
                return valve_traits<NextCallable, basic_context<TraitsType>>::call_set_get(next, ctx);
            } else {
                next_callable<stl::remove_cvref_t<NextCallable>> const callers{manglers, next};
                callers(ctx);
                return true;
            }
        }

        [[nodiscard]] constexpr tuple_type& as_tuple() noexcept {
            return manglers;
        }

        [[nodiscard]] constexpr tuple_type const& as_tuple() const noexcept {
            return manglers;
        }

        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... funcs) constexpr {
                  ((out.append(" >> ("), valve_to_string(out, funcs), out.append(" )")), ...);
              },
              as_tuple());
        }

        template <typename RouterT>
            requires((ValveRequiresSetup<RouterT, ManglerType> || ...))
        constexpr void setup(RouterT& inp_router) {
            istl::for_each_element(
              [&inp_router]<typename T>([[maybe_unused]] T&& inp_callable) constexpr {
                  if constexpr (ValveRequiresSetup<RouterT, T>) {
                      inp_callable.setup(inp_router);
                  }
              },
              as_tuple());
        }
    };

    /**
     * Forward Valve can contain multiple callables and calls them in-order
     */
    template <typename... Callables>
    struct forward_valve : valve<forward_valve<Callables...>> {
        using valve_type = valve<forward_valve<Callables...>>;
        using tuple_type = stl::tuple<Callables...>;

      private:
        tuple_type callables;

      public:
        template <typename... Args>
            requires stl::constructible_from<tuple_type, Args...>
        explicit constexpr forward_valve(Args&&... args) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, Args...>)
          : callables{stl::forward<Args>(args)...} {}

        constexpr forward_valve(forward_valve const&)                     = default;
        constexpr forward_valve(forward_valve&&) noexcept                 = default;
        constexpr forward_valve& operator=(forward_valve&&) noexcept      = default;
        constexpr forward_valve& operator=(forward_valve const&) noexcept = default;
        constexpr ~forward_valve()                                        = default;

        using valve_type::operator();

        template <Traits TraitsType>
        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            return stl::apply(
              [&ctx]<typename... T>(T&&... funcs) constexpr {
                  return (valve_traits<T, context_type>::call_set_get(stl::forward<T>(funcs), ctx) && ...);
              },
              as_tuple());
        }

        [[nodiscard]] constexpr tuple_type& as_tuple() noexcept {
            return callables;
        }

        [[nodiscard]] constexpr tuple_type const& as_tuple() const noexcept {
            return callables;
        }

        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... funcs) constexpr {
                  ((out.append(" >> ("), valve_to_string(out, stl::forward<T>(funcs)), out.append(" )")),
                   ...);
              },
              as_tuple());
        }

        template <typename RouterT>
            requires((ValveRequiresSetup<RouterT, Callables> || ...))
        constexpr void setup(RouterT& inp_router) {
            istl::for_each_element(
              [&inp_router]<typename T>([[maybe_unused]] T&& inp_callable) constexpr {
                  if constexpr (ValveRequiresSetup<RouterT, T>) {
                      stl::forward<T>(inp_callable).setup(inp_router);
                  }
              },
              as_tuple());
        }
    };

    template <typename... Pres, typename... Posts, typename... Manglers, typename Routes>
    struct valves_group<prerouting_valve<Pres...>,
                        postrouting_valve<Posts...>,
                        mangler_valve<Manglers...>,
                        Routes>
      : valve<valves_group<prerouting_valve<Pres...>,
                           postrouting_valve<Posts...>,
                           mangler_valve<Manglers...>,
                           Routes>> {
        using valve_type   = valve<valves_group>;
        using pre_type     = prerouting_valve<Pres...>;
        using post_type    = postrouting_valve<Posts...>;
        using mangler_type = mangler_valve<Manglers...>;
        using route_type   = Routes;

      private:
        [[no_unique_address]] pre_type     pres{};
        [[no_unique_address]] post_type    posts{};
        [[no_unique_address]] mangler_type manglers{};
        [[no_unique_address]] route_type   routes{};

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

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <istl::cvref_as<pre_type> T, istl::cvref_as<route_type> R>
        explicit constexpr valves_group(T&& inp_pre, R&& inp_routes = route_type{})
          : pres{stl::forward<T>(inp_pre)},
            routes{stl::forward<R>(inp_routes)} {}

        template <istl::cvref_as<post_type> T, istl::cvref_as<route_type> R>
        explicit constexpr valves_group(T&& inp_post, R&& inp_routes = route_type{})
          : posts{stl::forward<T>(inp_post)},
            routes{stl::forward<R>(inp_routes)} {}

        template <istl::cvref_as<mangler_type> T, istl::cvref_as<route_type> R>
        explicit constexpr valves_group(T&& inp_mangler, R&& inp_routes = route_type{})
          : manglers{stl::forward<T>(inp_mangler)},
            routes{stl::forward<R>(inp_routes)} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr valves_group(valves_group const&)                = default;
        constexpr valves_group(valves_group&&) noexcept            = default;
        constexpr valves_group& operator=(valves_group const&)     = default;
        constexpr valves_group& operator=(valves_group&&) noexcept = default;
        constexpr ~valves_group()                                  = default;


        using valve_type::operator();

        template <Traits TraitsType>
        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using pre_traits   = valve_traits<pre_type, context_type>;
            using post_traits  = valve_traits<post_type, context_type>;
            if constexpr (sizeof...(Pres) > 0) {
                if (!pre_traits::call_set_get(pres, ctx)) {
                    return;
                }
            }
            if (!manglers(ctx, routes)) {
                return;
            }
            if constexpr (sizeof...(Posts) > 0) {
                post_traits::call_set(posts, ctx);
            }
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

        [[nodiscard]] constexpr route_type const& get_routes() const noexcept {
            return routes;
        }

        [[nodiscard]] constexpr route_type& get_routes() noexcept {
            return routes;
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto append_preroute(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            using new_pre_type  = prerouting_valve<Pres..., callable_type>;
            return valves_group<new_pre_type, post_type, mangler_type, route_type>{
              new_pre_type{
                stl::tuple_cat(pres.as_tuple(), stl::make_tuple(stl::forward<Callable>(callable)))},
              posts,
              manglers,
              routes};
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto append_postroute(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            using new_post_type = postrouting_valve<Posts..., callable_type>;
            return valves_group<pre_type, new_post_type, mangler_type, route_type>{
              pres,
              new_post_type{
                stl::tuple_cat(posts.as_tuple(), stl::make_tuple(stl::forward<Callable>(callable)))},
              manglers,
              routes};
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto append_mangler(Callable&& callable) const {
            using callable_type    = stl::remove_cvref_t<Callable>;
            using new_mangler_type = mangler_valve<Manglers..., callable_type>;
            return valves_group<pre_type, post_type, new_mangler_type, route_type>{
              pres,
              posts,
              new_mangler_type{
                stl::tuple_cat(manglers.as_tuple(), stl::make_tuple(stl::forward<Callable>(callable)))},
              routes};
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto replace_route(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            return valves_group<pre_type, post_type, mangler_type, callable_type>{
              pres,
              posts,
              manglers,
              stl::forward<Callable>(callable)};
        }

        constexpr void to_string(istl::String auto& out) const {
            if constexpr (sizeof...(Pres) > 0) {
                out.append("- (");
                valve_to_string(out, pres);
                out.append(")");
            }
            if constexpr (sizeof...(Manglers) > 0) {
                out.append("* (");
                valve_to_string(out, manglers);
                out.append(")");
            }
            valve_to_string(out, routes);
            if constexpr (sizeof...(Posts) > 0) {
                out.append(" + (");
                valve_to_string(out, posts);
                out.append(")");
            }
        }
    };

    template <typename... T>
    valves_group(prerouting_valve<T...>)
      -> valves_group<prerouting_valve<T...>, postrouting_valve<>, mangler_valve<>, forward_valve<>>;

    template <typename... T, typename R>
    valves_group(prerouting_valve<T...>,
                 R&&) -> valves_group<prerouting_valve<T...>, postrouting_valve<>, mangler_valve<>, R>;

    template <typename... T>
    valves_group(postrouting_valve<T...>)
      -> valves_group<prerouting_valve<>, postrouting_valve<T...>, mangler_valve<>, forward_valve<>>;

    template <typename... T, typename R>
    valves_group(postrouting_valve<T...>,
                 R&&) -> valves_group<prerouting_valve<>, postrouting_valve<T...>, mangler_valve<>, R>;

    template <typename... T>
    valves_group(mangler_valve<T...>)
      -> valves_group<prerouting_valve<>, postrouting_valve<>, mangler_valve<T...>, forward_valve<>>;

    template <typename... T, typename R>
    valves_group(mangler_valve<T...>,
                 R&&) -> valves_group<prerouting_valve<>, postrouting_valve<>, mangler_valve<T...>, R>;

    template <typename T>
    valves_group(T&&) -> valves_group<prerouting_valve<>, postrouting_valve<>, mangler_valve<>, T>;

    template <typename... T>
    postrouting_valve(T&&...) -> postrouting_valve<T...>;

    template <typename... T>
    prerouting_valve(T&&...) -> prerouting_valve<T...>;

    template <typename... T>
    mangler_valve(T&&...) -> mangler_valve<T...>;

} // namespace webpp::http

#endif // WEBPP_GROUPING_VALVES_HPP
