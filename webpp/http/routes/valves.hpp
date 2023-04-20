// Created by moisrex on 3/20/23.

#ifndef WEBPP_HTTP_ROUTER_VALVE_HPP
#define WEBPP_HTTP_ROUTER_VALVE_HPP

#include "../../std/tuple.hpp"
#include "grouping_valves.hpp"
#include "logical_valves.hpp"
#include "member_function_valves.hpp"
#include "path_valves.hpp"
#include "router_concepts.hpp"
#include "valve_traits.hpp"


namespace webpp::http {

    template <typename Self>
    struct valve {


        template <template <typename...> typename T>
        static constexpr bool is_self_of = istl::template_of_v<T, Self>;

        using routes_type = typename routes_type_of_valve<Self>::type;

        template <template <typename...> typename T>
        static constexpr bool is_routes_of = istl::template_of_v<T, routes_type>;


        template <typename Callable>
        [[nodiscard]] constexpr auto operator>>(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            if constexpr (is_routes_of<forward_valve>) {
                if constexpr (istl::template_of_v<forward_valve, callable_type>) {
                    // Prevent double forwarding (flatten the type):
                    //   forward_valve<C2..., forward_valve<C...>>
                    return rebind_self<forward_valve>(
                      stl::tuple_cat(routes().as_tuple(), callable.as_tuple()));
                } else { // append the segment
                    return rebind_self<forward_valve>(
                      stl::tuple_cat(routes().as_tuple(),
                                     stl::make_tuple(valvify_or(stl::forward<Callable>(callable)))));
                }
                // } else if constexpr (is_routes_of<segment_valve>) {
                //     // forward<segment, ...> = segment<forward, ...>
                //     // Operator precedence of '>>' is lower than the precedence of '/' and '%', so we're
                //     // changing that here:
                //     return rebind_self<segment_valve>(
                //       stl::tuple_cat(routes().as_tuple(),
                //                      stl::make_tuple(valvify_or(stl::forward<Callable>(callable)))));
            } else {
                return rebind_next<forward_valve>(stl::forward<Callable>(callable));
            }
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto operator&&([[maybe_unused]] Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            // todo: C && Negative == ?
            if constexpr (stl::same_as<routes_type, callable_type>) { // C && C = C
                return *this;
            } else if constexpr (stl::same_as<not_valve<routes_type>, callable_type>) { // C && !C == -C
                return rebind_self<negative_valve>(routes());
            } else if constexpr (stl::same_as<routes_type, not_valve<callable_type>>) { // !C && C == -C
                return rebind_self<negative_valve>(stl::forward<Callable>(callable));
            } else {
                return rebind_next<and_valve>(stl::forward<Callable>(callable));
            }
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator||(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            // todo: C || Positive == ?
            if constexpr (stl::same_as<routes_type, callable_type>) { // C || C = C
                return *this;
            } else if constexpr (stl::same_as<not_valve<routes_type>, callable_type>) { // C || !C == +C
                return rebind_self<positive_valve>(routes());
            } else if constexpr (stl::same_as<routes_type, not_valve<callable_type>>) { // !C || C == -C
                return rebind_self<positive_valve>(stl::forward<Callable>(callable));
            } else {
                return rebind_next<or_valve>(stl::forward<Callable>(callable));
            }
        }


        [[nodiscard]] constexpr auto operator!() const {
            if constexpr (istl::remove_template_of_v<not_valve, routes_type>) {
                // This only checks against the default traits'-type's return type
                if constexpr (stl::same_as<bool,
                                           typename valve_traits<routes_type>::return_type>) { // !!C == C
                    return routes().unwrap();
                } else { // !!C = +C
                    return rebind_self<positive_valve>(routes().unwrap());
                }
            } else {
                return rebind_next<not_valve>();
            }
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator+(Callable&& callable) const {
            if constexpr (is_self_of<valves_group>) {
                return self()->append_postroute(valvify_or(stl::forward<Callable>(callable)));
            } else {
                return valves_group{postrouting_valve{valvify_or(stl::forward<Callable>(callable))},
                                    routes()};
            }
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator-(Callable&& callable) const {
            if constexpr (is_self_of<valves_group>) {
                return self()->append_preroute(valvify_or(stl::forward<Callable>(callable)));
            } else {
                return valves_group{prerouting_valve{valvify_or(stl::forward<Callable>(callable))}, routes()};
            }
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator*(Callable&& callable) const {
            if constexpr (is_self_of<valves_group>) {
                return self()->append_mangler(valvify_or(stl::forward<Callable>(callable)));
            } else {
                return valves_group{mangler_valve{valvify_or(stl::forward<Callable>(callable))}, routes()};
            }
        }

        template <typename CallableSegment>
        [[nodiscard]] constexpr auto operator/(CallableSegment&& inp_segment) const {
            using seg_type = stl::remove_cvref_t<CallableSegment>;
            if constexpr (is_routes_of<segment_valve>) {
                if constexpr (istl::template_of_v<segment_valve, seg_type>) {
                    // Prevent double segmenting (flatten the type):
                    //   segment_valve<C2..., segment_valve<C...>>
                    return rebind_self<segment_valve>(
                      stl::tuple_cat(routes().as_tuple(), inp_segment.as_tuple()));
                } else { // append the segment
                    return rebind_self<segment_valve>(stl::tuple_cat(
                      routes().as_tuple(),
                      stl::make_tuple(valvify_or(stl::forward<CallableSegment>(inp_segment)))));
                }
            } else {
                return rebind_next<segment_valve>(stl::forward<CallableSegment>(inp_segment));
            }
        }


        template <typename SegT>
        [[nodiscard]] constexpr auto operator%(SegT&& inp_segment) const {
            using seg_type = stl::remove_cvref_t<SegT>;
            if constexpr (is_routes_of<segment_valve>) {
                if constexpr (istl::template_of_v<segment_valve, seg_type>) {
                    // Prevent double segmenting (flatten the type):
                    //   segment_valve<C2..., segment_valve<C...>>
                    return rebind_self<segment_valve>(
                      stl::tuple_cat(routes().as_tuple(), inp_segment.as_tuple(), stl::make_tuple(endpath)));
                } else { // append the segment
                    return rebind_self<segment_valve>(
                      stl::tuple_cat(routes().as_tuple(),
                                     stl::make_tuple(valvify_or(stl::forward<SegT>(inp_segment))),
                                     stl::make_tuple(endpath)));
                }
            } else {
                return rebind_next<segment_valve>(stl::forward<SegT>(inp_segment), endpath);
            }
        }


        // Convert Custom Contexts into dynamic context
        // For compatibility with the static router
        template <Context CtxT>
            requires(
              !stl::is_void_v<Self> &&
              requires {
                  typename stl::remove_cvref_t<CtxT>::traits_type;
                  requires !istl::cvref_as<CtxT,
                                           basic_context<typename stl::remove_cvref_t<CtxT>::traits_type>>;
              })
        [[nodiscard]] constexpr auto operator()(CtxT&& inp_ctx) {
            using traits_type  = typename stl::remove_cvref_t<CtxT>::traits_type;
            using context_type = basic_context<traits_type>;
            context_type ctx{inp_ctx};
            (*self())(ctx);
            return stl::move(ctx.response);
        }


      private:
        [[nodiscard]] constexpr auto const* self() const noexcept
            requires(!stl::is_void_v<Self>)
        {
            return static_cast<Self const*>(this);
        }

        [[nodiscard]] constexpr auto* self() noexcept
            requires(!stl::is_void_v<Self>)
        {
            return static_cast<Self*>(this);
        }

        [[nodiscard]] constexpr decltype(auto) routes() noexcept {
            if constexpr (stl::is_void_v<Self>) {
                return forward_valve<>{};
            } else {
                if constexpr (is_self_of<valves_group>) {
                    return self()->get_routes();
                } else {
                    return *self();
                }
            }
        }

        [[nodiscard]] constexpr decltype(auto) routes() const noexcept {
            if constexpr (stl::is_void_v<Self>) {
                return forward_valve<>{};
            } else {
                if constexpr (is_self_of<valves_group>) {
                    return self()->get_routes();
                } else {
                    return *self();
                }
            }
        }

        template <template <typename...> typename Templ, typename... T, typename... Args>
        [[nodiscard]] constexpr auto rebind_next(Args&&... nexts) const {
            if constexpr (stl::is_void_v<Self>) {
                return rebind_self<Templ, T...>(stl::forward<Args>(nexts)...);
            } else {
                return rebind_self<Templ, T...>(routes(), stl::forward<Args>(nexts)...);
            }
        }


        // We get a tuple<>, but we want to pass all the individual elements to `valvify`
        // Attention: This means we will be calling `valvify` multiple times for each element
        template <template <typename...> typename Templ, typename... T, typename... TupT>
        [[nodiscard]] constexpr auto rebind_self(stl::tuple<TupT...>&& nexts) const {
            using valve_type =
              Templ<T..., stl::remove_cvref_t<decltype(valvify_or(stl::declval<TupT>()))>...>;
            if constexpr (is_self_of<valves_group>) {
                return self()->replace_route(
                  ([&nexts]<stl::size_t... I>(stl::index_sequence<I...>) constexpr {
                      return valve_type{valvify_or(stl::get<I>(nexts))...};
                  })(stl::index_sequence_for<TupT...>{}));
            } else {
                return ([&nexts]<stl::size_t... I>(stl::index_sequence<I...>) constexpr {
                    return valve_type{valvify_or(stl::get<I>(nexts))...};
                })(stl::index_sequence_for<TupT...>{});
            }
        }

        template <template <typename...> typename Templ, typename... T, typename... Args>
        [[nodiscard]] constexpr auto rebind_self(Args&&... nexts) const {
            static_assert((Valvifiable<Args> && ...),
                          "We're not able to valvify the valve, "
                          "did you include the required headers "
                          "that defines the right valvify function?");
            using valve_type =
              Templ<T..., stl::remove_cvref_t<decltype(valvify_or(stl::declval<Args>()))>...>;
            static_assert(
              stl::is_constructible_v<valve_type,
                                      stl::remove_cvref_t<decltype(valvify_or(stl::declval<Args>()))>...>,
              "The specified valves are unknown even after valvifying them, "
              "did you forget to include the required headers that define "
              "the right valvify functions to convert your valves to the right type?");
            if constexpr (is_self_of<valves_group>) {
                return self()->replace_route(valve_type{valvify_or(stl::forward<Args>(nexts))...});
            } else {
                return valve_type{valvify_or(stl::forward<Args>(nexts))...};
            }
        }
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTER_VALVE_HPP
