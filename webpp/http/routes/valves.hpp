// Created by moisrex on 3/20/23.

#ifndef WEBPP_HTTP_ROUTER_VALVE_HPP
#define WEBPP_HTTP_ROUTER_VALVE_HPP

#include "../../std/tuple.hpp"
#include "grouping_valves.hpp"
#include "logical_valves.hpp"
#include "path_valves.hpp"

#include <any>
#include <typeinfo>

namespace webpp::http {

    template <typename Self>
    struct valve {

        template <typename Callable>
        [[nodiscard]] constexpr auto operator>>(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            if constexpr (istl::template_of_v<forward_valve, Self>) {
                if constexpr (istl::template_of_v<forward_valve, callable_type>) {
                    // Prevent double forwarding (flatten the type):
                    //   forward_valve<C2..., forward_valve<C...>>
                    return rebind_self<forward_valve>(
                      stl::tuple_cat(self()->as_tuple(), callable.as_tuple()));
                } else { // append the segment
                    return rebind_self<forward_valve>(
                      stl::tuple_cat(self()->as_tuple(),
                                     stl::make_tuple(valvify(stl::forward<Callable>(callable)))));
                }
            } else if constexpr (istl::template_of_v<segment_valve, Self>) {
                // forward<segment, ...> = segment<forward, ...>
                // Operator precedence of '>>' is lower than the precedence of '/' and '%', so we're
                // changing that here:
                return rebind_self<segment_valve>(
                  stl::tuple_cat(self()->as_tuple(),
                                 stl::make_tuple(valvify(stl::forward<Callable>(callable)))));
            } else {
                return rebind_next<forward_valve>(stl::forward<Callable>(callable));
            }
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto operator&&([[maybe_unused]] Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            // todo: C && Negative == ?
            if constexpr (stl::same_as<Self, callable_type>) {                   // C && C = C
                return *this;
            } else if constexpr (stl::same_as<not_valve<Self>, callable_type>) { // C && !C == -C
                return rebind_self<negative_valve>(*self());
            } else if constexpr (stl::same_as<Self, not_valve<callable_type>>) { // !C && C == -C
                return rebind_self<negative_valve>(stl::forward<Callable>(callable));
            } else {
                return rebind_next<and_valve>(stl::forward<Callable>(callable));
            }
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator||(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            // todo: C || Positive == ?
            if constexpr (stl::same_as<Self, callable_type>) {                   // C || C = C
                return *this;
            } else if constexpr (stl::same_as<not_valve<Self>, callable_type>) { // C || !C == +C
                return rebind_self<positive_valve>(*self());
            } else if constexpr (stl::same_as<Self, not_valve<callable_type>>) { // !C || C == -C
                return rebind_self<positive_valve>(stl::forward<Callable>(callable));
            } else {
                return rebind_next<or_valve>(stl::forward<Callable>(callable));
            }
        }


        [[nodiscard]] constexpr auto operator!() const {
            if constexpr (istl::remove_template_of_v<not_valve, Self>) {
                // This only checks against the default traits'-type's return type
                if constexpr (stl::same_as<bool, typename valve_traits<Self>::return_type>) { // !!C == C
                    return self()->unwrap();
                } else {                                                                      // !!C = +C
                    return rebind_self<positive_valve>(self()->unwrap());
                }
            } else {
                return rebind_next<not_valve>();
            }
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator+(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            if constexpr (istl::template_of_v<postrouting_valve, Self>) {
                if constexpr (istl::template_of_v<postrouting_valve, callable_type>) {
                    return rebind_self<postrouting_valve>(
                      stl::tuple_cat(self()->as_tuple(), callable.as_tuple()));
                } else {
                    return rebind_self<postrouting_valve>(
                      stl::tuple_cat(self()->as_tuple(),
                                     stl::make_tuple(valvify(stl::forward<Callable>(callable)))));
                }
            } else {
                return rebind_self<forward_valve>(
                  rebind_next<postrouting_valve>(stl::forward<Callable>(callable)));
            }
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator-(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            if constexpr (istl::template_of_v<prerouting_valve, Self>) {
                if constexpr (istl::template_of_v<prerouting_valve, callable_type>) {
                    // pre<..., pre<C>> = pre<..., C>
                    return rebind_self<prerouting_valve>(
                      stl::tuple_cat(self()->as_tuple(), callable.as_tuple()));
                } else { // append the callable to the end of the prerouting valve
                    return rebind_self<prerouting_valve>(
                      stl::tuple_cat(self()->as_tuple(),
                                     stl::make_tuple(valvify(stl::forward<Callable>(callable)))));
                }
            } else {
                return rebind_next<forward_valve>(
                  rebind_self<prerouting_valve>(stl::forward<Callable>(callable)));
            }
        }


        template <typename CallableSegment>
        [[nodiscard]] constexpr auto operator/(CallableSegment&& inp_segment) const {
            using seg_type = stl::remove_cvref_t<CallableSegment>;
            if constexpr (istl::template_of_v<segment_valve, Self>) {
                if constexpr (istl::template_of_v<segment_valve, seg_type>) {
                    // Prevent double segmenting (flatten the type):
                    //   segment_valve<C2..., segment_valve<C...>>
                    return rebind_self<segment_valve>(
                      stl::tuple_cat(self()->as_tuple(), inp_segment.as_tuple()));
                } else { // append the segment
                    return rebind_self<segment_valve>(
                      stl::tuple_cat(self()->as_tuple(),
                                     stl::make_tuple(valvify(stl::forward<CallableSegment>(inp_segment)))));
                }
            } else {
                return rebind_next<segment_valve>(stl::forward<CallableSegment>(inp_segment));
            }
        }


        template <typename SegT>
        [[nodiscard]] constexpr auto operator%(SegT&& inp_segment) const {
            using seg_type = stl::remove_cvref_t<SegT>;
            if constexpr (istl::template_of_v<segment_valve, Self>) {
                if constexpr (istl::template_of_v<segment_valve, seg_type>) {
                    // Prevent double segmenting (flatten the type):
                    //   segment_valve<C2..., segment_valve<C...>>
                    return rebind_self<segment_valve>(
                      stl::tuple_cat(self()->as_tuple(), inp_segment.as_tuple(), stl::make_tuple(endpath)));
                } else { // append the segment
                    return rebind_self<segment_valve>(
                      stl::tuple_cat(self()->as_tuple(),
                                     stl::make_tuple(valvify(stl::forward<SegT>(inp_segment))),
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
        [[nodiscard]] constexpr Self const* self() const noexcept
            requires(!stl::is_void_v<Self>)
        {
            return static_cast<Self const*>(this);
        }

        [[nodiscard]] constexpr Self* self() noexcept
            requires(!stl::is_void_v<Self>)
        {
            return static_cast<Self*>(this);
        }

        template <typename T>
        using is_prerouting = istl::template_of<prerouting_valve, T>;

        template <typename T>
        using is_postrouting = istl::template_of<postrouting_valve, T>;

        template <typename T>
        using is_normal_valve = stl::negation<stl::disjunction<is_prerouting<T>, is_postrouting<T>>>;

        // template <template <typename...> typename Templ, typename... T, Mangler Arg>
        //     requires(!stl::is_void_v<Self>)
        // [[nodiscard]] constexpr auto rebind_next(Arg&& next) const noexcept {
        //     using mangler_type    = valvified_type<Arg>;
        //     using templ_type      = Templ<Self, T...>;
        //     using valve_type      = mangler_valve<templ_type, mangler_type>;
        //     using optimized_route = route_optimizer<valve_type>;
        //     return optimized_route::convert(*self(), valvify<Arg>(stl::forward<Arg>(next)));
        // }

        template <template <typename...> typename Templ, typename... T, typename... Args>
        [[nodiscard]] constexpr auto rebind_next(Args&&... nexts) const {
            if constexpr (stl::is_void_v<Self>) {
                return rebind_self<Templ, T...>(stl::forward<Args>(nexts)...);
            } else {
                return rebind_self<Templ, T...>(*self(), stl::forward<Args>(nexts)...);
            }
        }


        struct sorted_tag {};

        // We get a tuple<>, but we want to pass all the individual elements to `valvify`
        // Attention: This means we will be calling `valvify` multiple times for each element
        template <template <typename...> typename Templ, typename... T, typename... TupT>
        [[nodiscard]] constexpr auto rebind_self(stl::tuple<TupT...>&& nexts, sorted_tag) const {
            using valve_type = Templ<T..., valvified_type<TupT>...>;
            return ([&nexts]<stl::size_t... I>(stl::index_sequence<I...>) constexpr {
                return valve_type{valvify(stl::get<I>(nexts))...};
            })(stl::index_sequence_for<TupT...>{});
        }

        template <template <typename...> typename Templ, typename... T, typename... TupT>
        [[nodiscard]] constexpr auto rebind_self(stl::tuple<TupT...>&& nexts) const {
            auto sorted = istl::tuple_reorder_elements(
              nexts,
              istl::integer_sequence_cat_t<istl::indexes_if<is_prerouting, TupT...>,
                                           istl::indexes_if<is_normal_valve, TupT...>,
                                           istl::indexes_if<is_postrouting, TupT...>>{});
            return rebind_self<Templ, T...>(stl::move(sorted), sorted_tag{});
        }

        template <template <typename...> typename Templ, typename... T, typename... Args>
        [[nodiscard]] constexpr auto rebind_self(Args&&... nexts) const {
            using valve_type = Templ<T..., valvified_type<Args>...>;
            return valve_type{valvify(stl::forward<Args>(nexts))...};
        }
    };





    template <typename MemPtr>
    struct member_function_callable : valve<member_function_callable<MemPtr>> {
        using valve_type      = valve<member_function_callable<MemPtr>>;
        using member_ptr_type = MemPtr;
        using mem_traits      = istl::member_function_pointer_traits<member_ptr_type>;
        using object_type     = typename mem_traits::type;
        using object_ptr      = object_type*;

      private:
        struct method_holder {
            member_ptr_type mem_ptr;
            object_ptr      obj = nullptr;

            template <typename... Args>
                requires(mem_traits::template is_invocable<Args...>)
            constexpr decltype(auto) operator()(Args&&... args) const noexcept(mem_traits::is_noexcept) {
                return (obj->*mem_ptr)(stl::forward<Args>(args)...);
            }
        } holder;

      public:
        constexpr member_function_callable(member_function_callable const&) noexcept            = default;
        constexpr member_function_callable(member_function_callable&&) noexcept                 = default;
        constexpr member_function_callable& operator=(member_function_callable const&) noexcept = default;
        constexpr member_function_callable& operator=(member_function_callable&&) noexcept      = default;
        constexpr ~member_function_callable() noexcept                                          = default;

        constexpr member_function_callable(member_ptr_type inp_func) noexcept : holder{.mem_ptr = inp_func} {}

        using valve_type::operator();

        template <Traits TraitsType>
        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type    = basic_context<TraitsType>;
            using callable_traits = valve_traits<method_holder, context_type>;
            callable_traits::call_set(holder, ctx);
        }

        constexpr void set_object(object_ptr inp_obj) noexcept {
            holder.obj = inp_obj;
        }


        template <typename RouterType>
        constexpr void setup(RouterType& router) {
            for (auto& object : router.objects) {
                if (object.type() == typeid(object_type)) {
                    set_object(stl::any_cast<object_type>(stl::addressof(object)));
                    return;
                }
            }

            // default constructing it if it's possible and use that object
            if constexpr (stl::is_default_constructible_v<object_type>) {
                router.objects.emplace_back(object_type{});
                set_object(stl::any_cast<object_type>(stl::addressof(router.objects.back())));
            } else if constexpr (stl::is_constructible_v<object_type, RouterType&>) {
                router.objects.emplace_back(object_type{router});
                set_object(stl::any_cast<object_type>(stl::addressof(router.objects.back())));
            } else {
                router.logger.error(
                  "DRouter",
                  fmt::format(
                    "You have not specified an object with typeid of '{}' in your dynamic router,"
                    " but you've tried to register a member function pointer of that type in router."
                    " Try registering your objects before registering the objects in the router"
                    " to get rid of this error.",
                    typeid(object_type).name()));
            }
        }


        constexpr void to_string(istl::String auto& out) const {
            valve_to_string(out, holder.mem_ptr);
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

#endif // WEBPP_HTTP_ROUTER_VALVE_HPP
