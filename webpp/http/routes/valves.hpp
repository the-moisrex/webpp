// Created by moisrex on 3/20/23.

#ifndef WEBPP_HTTP_ROUTER_VALVES_HPP
#define WEBPP_HTTP_ROUTER_VALVES_HPP

#include "../../std/function_ref.hpp"
#include "../../std/string.hpp"
#include "../../std/string_view.hpp"
#include "../../std/tuple.hpp"
#include "../../traits/traits.hpp"
#include "../http_concepts.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "./context.hpp"

#include <any>

namespace webpp::http {

    template <Traits>
    struct basic_dynamic_router;

    template <typename TraitsType>
    using next_route = istl::function_ref<void(basic_context<TraitsType>&)>;

    template <typename TraitsType, typename T>
    concept Valve = Traits<TraitsType> && stl::is_invocable_v<T, basic_context<TraitsType>&>;

    template <typename TraitsType, typename T>
    concept Mangler = Traits<TraitsType> && stl::is_invocable_v<T,                          // type
                                                                basic_context<TraitsType>&, // context
                                                                next_route<TraitsType>      // next valve
                                                                >;

    template <typename TraitsType, typename T>
    concept RouteSetter = Traits<TraitsType> && stl::is_invocable_v<T, basic_dynamic_router<TraitsType>&>;


    template <typename Callable>
    static constexpr void valve_to_string(istl::String auto& out, Callable& func) {
        using mem_traits = istl::member_function_pointer_traits<Callable>;
        if constexpr (istl::StringViewifiable<Callable>) {
            out.append(" \"");
            out += istl::string_viewify(func);
            out.append("\"");
        } else if constexpr (requires { func.to_string(out); }) {
            func.to_string(out);
        } else if constexpr (mem_traits::value) {
            out.append(" ");
            out.append(typeid(typename mem_traits::type).name());
            out.append("::");
            out.append(typeid(Callable).name());
        } else {
            out.append(" ");
            out.append(typeid(Callable).name());
        }
    }


    template <Traits TraitsType, typename PreRoute, typename Callable>
    struct prerouting_valve;
    template <Traits TraitsType, typename Callable, typename PostRoute>
    struct postrouting_valve;
    template <Traits TraitsType, typename Callable>
    struct not_valve;
    template <Traits TraitsType, typename Callable>
    struct negative_valve;
    template <Traits TraitsType, typename Callable>
    struct positive_valve;
    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct and_valve;
    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct or_valve;
    template <Traits TraitsType, typename... Callables>
    struct forward_valve;
    template <Traits TraitsType, typename... CallableSegments>
    struct segment_valve;
    template <Traits TraitsType, typename MemPtr>
    struct member_function_callable;

    template <typename Callable, typename ContextType>
    struct valve_traits {
        using callable_type      = stl::remove_cvref_t<Callable>;
        using context_type       = ContextType;
        using request_type       = typename context_type::request_type;
        using response_type      = typename context_type::response_type;
        using response_body_type = typename response_type::body_type;
        using invocable_inorder_type =
          istl::invocable_inorder<callable_type, context_type&, request_type&, response_type&>;
        using return_type = stl::remove_cvref_t<typename invocable_inorder_type::result>;

        // static_assert(invocable_inorder_type::value, "We're not able to call your route.");

        static constexpr auto call(Callable&     callable,
                                   context_type& ctx) noexcept(invocable_inorder_type::is_nothrow)
            requires(invocable_inorder_type::value)
        {
            return istl::invoke_inorder(callable, ctx, ctx.request, ctx.response);
        }

        template <typename T>
            requires(istl::same_as_cvref<T, Callable>)
        static constexpr auto call(T&&           segment,
                                   context_type& ctx) noexcept(invocable_inorder_type::is_nothrow)
            requires(istl::StringViewifiable<Callable>)
        {
            return ctx.check_segment(stl::forward<T>(segment));
        }

        template <typename R>
        static constexpr bool is_positive(R&& ret) noexcept {
            if constexpr (stl::same_as<return_type, bool>) {
                return ret;
            } else if constexpr (stl::is_void_v<return_type> || HTTPResponse<return_type>) {
                return true;
            } else if constexpr (stl::is_pointer_v<return_type>) {
                return ret != nullptr;
            } else if constexpr (istl::Optional<return_type>) {
                return bool{ret};
            } else {
                // istl::nothing_type, ...
                return false;
            }
        }


        template <typename R>
        static constexpr bool is_prerouting_positive(R&& ret) noexcept {
            if constexpr (HTTPResponse<return_type>) {
                return false;
            } else {
                return is_positive(ret);
            }
        }

        template <typename R>
        static constexpr bool is_postrouting_positive(R&& ret) noexcept {
            if constexpr (HTTPResponse<return_type>) {
                return true;
            } else {
                return is_positive(ret);
            }
        }

        template <typename R>
        static constexpr void set_response(R&& ret, context_type& ctx) {
            using ret_t = stl::remove_cvref_t<R>;
            if constexpr (stl::is_void_v<ret_t> || istl::part_of<ret_t, bool, istl::nothing_type>) {
                // ignore the result
            } else if constexpr (HTTPResponse<ret_t> || HTTPResponseBody<ret_t> ||
                                 HTTPSerializableBody<ret_t, response_body_type>) {
                ctx.response = stl::forward<R>(ret);
            } else if constexpr (stl::same_as<ret_t, http::status_code>) {
                ctx.response = ret;
            } else if constexpr (stl::integral<ret_t>) {
                ctx.response = static_cast<http::status_code>(ret);
            } else if constexpr (istl::Optional<ret_t>) {
                if (ret) {
                    set_response(stl::move(*ret), ctx);
                }
            } else if constexpr (stl::is_pointer_v<ret_t>) {
                if (ret != nullptr) {
                    set_response(*ret, ctx);
                }
            } else {
                static_assert_false(ret_t, "We don't know what to do with your route's return type.");
            }
        }


        template <typename R>
        static constexpr bool set_get_response(R&& ret, context_type& ctx) {
            bool const res = is_positive(ret);
            set_response(stl::forward<R>(ret), ctx);
            return res;
        }


        template <typename C>
            requires istl::same_as_cvref<C, Callable>
        static constexpr void call_and_set(C&&           segment,
                                           context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<C>(segment), ctx);
            } else {
                set_response(call(stl::forward<C>(segment), ctx), ctx);
            }
        }


        template <typename C>
            requires istl::same_as_cvref<C, Callable>
        static constexpr bool call_set_get(C&&           segment,
                                           context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<C>(segment), ctx);
                return true;
            } else {
                return set_get_response(call(stl::forward<C>(segment), ctx), ctx);
            }
        }
    };





    template <typename C>
    struct route_optimizer {
        using type = C;

        template <typename... T>
        static constexpr C convert(T&&... args) noexcept {
            return {stl::forward<T>(args)...};
        }
    };

    // remove the unneeded void type
    template <template <typename, typename, typename...> typename CallableTemplate,
              typename TraitsType,
              typename... T>
    struct route_optimizer<CallableTemplate<TraitsType, void, T...>>
      : route_optimizer<CallableTemplate<TraitsType, T...>> {};


    // move pre-routing hooks to the left because they supposed to be called before everything
    template <template <typename, typename, typename> typename CallableTemplate,
              typename Left,
              typename Pre,
              typename Right,
              typename TraitsType>
    struct route_optimizer<CallableTemplate<TraitsType, Left, prerouting_valve<TraitsType, Pre, Right>>>
      : route_optimizer<CallableTemplate<TraitsType,
                                         prerouting_valve<TraitsType,
                                                          typename route_optimizer<Pre>::type,
                                                          typename route_optimizer<Left>::type>,
                                         typename route_optimizer<Right>::type>> {

        using left_parent  = route_optimizer<Left>;
        using right_parent = route_optimizer<Right>;
        using pre_parent   = route_optimizer<Pre>;
        using preroute_type =
          prerouting_valve<TraitsType, typename pre_parent::type, typename left_parent::type>;
        using parent_type =
          route_optimizer<CallableTemplate<TraitsType, preroute_type, typename pre_parent::type>>;
        using return_type = typename parent_type::type;
        using rhs_type    = prerouting_valve<TraitsType, Pre, Right>;
        using lhs_type    = Left;

        static constexpr return_type convert(lhs_type&& lhs, rhs_type&& rhs) noexcept {
            return parent_type::convert(preroute_type{pre_parent::convert(stl::move(rhs.get_preroute())),
                                                      left_parent::convert(stl::move(lhs))},
                                        right_parent::convert(stl::move(rhs.get_callable())));
        }
    };

    // move post-routing hooks to the right because they supposed to be called after everything
    template <template <typename, typename, typename> typename CallableTemplate,
              typename Left,
              typename Post,
              typename Right,
              typename TraitsType>
    struct route_optimizer<CallableTemplate<TraitsType, postrouting_valve<TraitsType, Left, Post>, Right>>
      : route_optimizer<CallableTemplate<TraitsType,
                                         typename route_optimizer<Left>::type,
                                         postrouting_valve<TraitsType,
                                                           typename route_optimizer<Right>::type,
                                                           typename route_optimizer<Post>::type>>> {
        using left_parent  = route_optimizer<Left>;
        using right_parent = route_optimizer<Right>;
        using post_parent  = route_optimizer<Post>;
        using postroute_type =
          postrouting_valve<TraitsType, typename right_parent::type, typename post_parent::type>;
        using parent_type =
          route_optimizer<CallableTemplate<TraitsType, typename left_parent::type, postroute_type>>;
        using return_type = typename parent_type::type;
        using lhs_type    = postrouting_valve<TraitsType, Left, Post>;
        using rhs_type    = Right;

        static constexpr return_type convert(lhs_type&& lhs, rhs_type&& rhs) noexcept {
            return parent_type::convert(left_parent::convert(stl::move(lhs.get_callable())),
                                        postroute_type{right_parent::convert(stl::move(rhs)),
                                                       post_parent::convert(stl::move(lhs.get_postroute()))});
        }
    };

    // !!C == C
    template <typename C, typename TraitsType>
    struct route_optimizer<not_valve<TraitsType, not_valve<TraitsType, C>>> : route_optimizer<C> {
        using parent_type = route_optimizer<C>;
        using return_type = typename parent_type::type;

        static constexpr return_type convert(C next) noexcept {
            return parent_type::convert(stl::forward<C>(next));
        }
    };

    // C || !C == +C
    template <typename C, typename TraitsType>
    struct route_optimizer<or_valve<TraitsType, C, not_valve<TraitsType, C>>>
      : route_optimizer<positive_valve<TraitsType, C>> {
        using parent_type = route_optimizer<positive_valve<TraitsType, C>>;
        using return_type = typename parent_type::type;

        static constexpr return_type convert(C next) noexcept {
            return parent_type::convert(stl::forward<C>(next));
        }
    };

    // !C || C == +C
    template <typename C, typename TraitsType>
    struct route_optimizer<or_valve<TraitsType, not_valve<TraitsType, C>, C>>
      : route_optimizer<positive_valve<TraitsType, C>> {
        using parent_type = route_optimizer<positive_valve<TraitsType, C>>;
        using return_type = typename parent_type::type;

        static constexpr return_type convert(C next) noexcept {
            return parent_type::convert(stl::forward<C>(next));
        }
    };

    // C && C == C
    template <typename C, typename TraitsType>
    struct route_optimizer<and_valve<TraitsType, C, C>> : route_optimizer<C> {
        using parent_type = route_optimizer<C>;
        using return_type = typename parent_type::type;

        static constexpr return_type convert(C next, [[maybe_unused]] C) noexcept {
            return parent_type::convert(stl::forward<C>(next));
        }
    };

    // C && !C == -C
    template <typename C, typename TraitsType>
    struct route_optimizer<and_valve<TraitsType, C, not_valve<TraitsType, C>>>
      : route_optimizer<negative_valve<TraitsType, C>> {
        using parent_type = route_optimizer<negative_valve<TraitsType, C>>;
        using return_type = typename parent_type::type;

        static constexpr return_type convert(C next) noexcept {
            return parent_type::convert(stl::forward<C>(next));
        }
    };

    // !C && C == -C
    template <typename C, typename TraitsType>
    struct route_optimizer<and_valve<TraitsType, not_valve<TraitsType, C>, C>>
      : route_optimizer<negative_valve<TraitsType, C>> {
        using parent_type = route_optimizer<negative_valve<TraitsType, C>>;
        using return_type = typename parent_type::type;

        static constexpr return_type convert(C next) noexcept {
            return parent_type::convert(stl::forward<C>(next));
        }
    };

    // remove first voids
    template <typename... C, typename TraitsType>
    struct route_optimizer<forward_valve<TraitsType, void, C...>>
      : route_optimizer<forward_valve<TraitsType, C...>> {
        using parent_type = route_optimizer<forward_valve<TraitsType, C...>>;
        using return_type = typename parent_type::type;

        static constexpr return_type convert(C... next_callables) noexcept {
            return parent_type::convert(stl::forward<C>(next_callables)...);
        }
    };

    // remove double forwarding
    template <typename C1, typename... C, typename TraitsType>
    struct route_optimizer<forward_valve<TraitsType, forward_valve<TraitsType, C1>, C...>>
      : route_optimizer<forward_valve<TraitsType, C1, C...>> {
        using parent_type       = route_optimizer<forward_valve<TraitsType, C1, C...>>;
        using return_type       = typename parent_type::type;
        using sub_callable_type = forward_valve<TraitsType, C1>;

        template <typename... N>
        static constexpr return_type convert(sub_callable_type const& sub_callable,
                                             N... next_callables) noexcept {
            return parent_type::convert(sub_callable.get_callable(), stl::forward<N>(next_callables)...);
        }
    };

    // forward<segment, ...> = segment<forward, ...>
    template <typename... C, typename... SC, typename TraitsType>
    struct route_optimizer<forward_valve<TraitsType, segment_valve<TraitsType, SC...>, C...>>
      : route_optimizer<segment_valve<TraitsType, SC..., route_optimizer<forward_valve<TraitsType, C...>>>> {
        using next_t       = route_optimizer<forward_valve<TraitsType, C...>>;
        using next_type    = typename next_t::type;
        using out_seg_type = segment_valve<TraitsType, SC..., next_type>;
        using parent_type  = route_optimizer<out_seg_type>;
        using return_type  = typename parent_type::type;
        using seg_type     = segment_valve<TraitsType, SC...>;

        template <typename NewCallable>
        static constexpr return_type convert(seg_type const& seg, NewCallable&& next) noexcept {
            return parent_type::convert(seg, next_t::convert(stl::forward<NewCallable>(next)));
        }
    };

    // remove double segmenting
    template <typename... C1s, typename... C, typename TraitsType>
    struct route_optimizer<segment_valve<TraitsType, segment_valve<TraitsType, C1s...>, C...>>
      : route_optimizer<segment_valve<TraitsType, C1s..., C...>> {
        using parent_type  = route_optimizer<segment_valve<TraitsType, C1s..., C...>>;
        using return_type  = typename parent_type::type;
        using sub_seg_type = segment_valve<TraitsType, C1s...>;

        static constexpr return_type convert(sub_seg_type sub_seg, C... next_segs) noexcept {
            return parent_type::convert(sub_seg.get_segment(), stl::forward<C>(next_segs)...);
        }
    };


    // todo: C || Positive == ?
    // todo: C && Negative == ?





    template <Traits TraitsType, typename Self>
    struct valve {
        using traits_type   = TraitsType;
        using context_type  = basic_context<traits_type>;
        using response_type = basic_response<traits_type>;
        using request_type  = basic_request<traits_type>;



#define WEBPP_DEFINE_FUNC_BASE(Ret, ...)                                                         \
                                                                                                 \
    template <typename T>                                                                        \
    [[nodiscard]] constexpr auto operator>>(Ret (T::*inp_func)(__VA_ARGS__)) const {             \
        using mem_ptr_type = Ret (T::*)(__VA_ARGS__);                                            \
        using forward_type =                                                                     \
          forward_valve<traits_type, Self, member_function_callable<traits_type, mem_ptr_type>>; \
        return rebind_next<forward_type>(inp_func);                                              \
    }                                                                                            \
                                                                                                 \
    template <typename T>                                                                        \
    [[nodiscard]] constexpr auto operator>>(Ret (T::*inp_func)(__VA_ARGS__) const) const {       \
        using mem_ptr_type = Ret (T::*)(__VA_ARGS__) const;                                      \
        using mem_type     = member_function_callable<traits_type, mem_ptr_type>;                \
        using forward_type = forward_valve<traits_type, Self, mem_type>;                         \
        return rebind_next<forward_type>(mem_type{inp_func});                                    \
    }

#define WEBPP_DEFINE_FUNC(...)                                \
    WEBPP_DEFINE_FUNC_BASE(response_type, __VA_ARGS__)        \
    WEBPP_DEFINE_FUNC_BASE(response_type const&, __VA_ARGS__) \
    WEBPP_DEFINE_FUNC_BASE(response_type&, __VA_ARGS__)       \
    WEBPP_DEFINE_FUNC_BASE(void, __VA_ARGS__)                 \
    WEBPP_DEFINE_FUNC_BASE(context_type, __VA_ARGS__)         \
    WEBPP_DEFINE_FUNC_BASE(context_type&, __VA_ARGS__)        \
    WEBPP_DEFINE_FUNC_BASE(context_type const&, __VA_ARGS__)


        WEBPP_DEFINE_FUNC(request_type)
        WEBPP_DEFINE_FUNC(request_type&)
        WEBPP_DEFINE_FUNC(request_type const&)
        WEBPP_DEFINE_FUNC(request_type&&)
        WEBPP_DEFINE_FUNC(request_type, response_type)
        WEBPP_DEFINE_FUNC(request_type&, response_type)
        WEBPP_DEFINE_FUNC(request_type const&, response_type)
        WEBPP_DEFINE_FUNC(request_type&&, response_type)
        WEBPP_DEFINE_FUNC(request_type, response_type&)
        WEBPP_DEFINE_FUNC(request_type&, response_type&)
        WEBPP_DEFINE_FUNC(request_type const&, response_type&)
        WEBPP_DEFINE_FUNC(request_type&&, response_type&)
        WEBPP_DEFINE_FUNC(request_type, response_type const&)
        WEBPP_DEFINE_FUNC(request_type&, response_type const&)
        WEBPP_DEFINE_FUNC(request_type const&, response_type const&)
        WEBPP_DEFINE_FUNC(request_type&&, response_type const&)
        WEBPP_DEFINE_FUNC(request_type, context_type)
        WEBPP_DEFINE_FUNC(request_type&, context_type)
        WEBPP_DEFINE_FUNC(request_type const&, context_type)
        WEBPP_DEFINE_FUNC(request_type&&, context_type)
        WEBPP_DEFINE_FUNC(request_type, context_type&)
        WEBPP_DEFINE_FUNC(request_type&, context_type&)
        WEBPP_DEFINE_FUNC(request_type const&, context_type&)
        WEBPP_DEFINE_FUNC(request_type&&, context_type&)
        WEBPP_DEFINE_FUNC(request_type, context_type const&)
        WEBPP_DEFINE_FUNC(request_type&, context_type const&)
        WEBPP_DEFINE_FUNC(request_type const&, context_type const&)
        WEBPP_DEFINE_FUNC(request_type&&, context_type const&)


        WEBPP_DEFINE_FUNC(context_type&)
        WEBPP_DEFINE_FUNC(context_type)
        WEBPP_DEFINE_FUNC(context_type const&)
        WEBPP_DEFINE_FUNC(context_type&, response_type)
        WEBPP_DEFINE_FUNC(context_type, response_type)
        WEBPP_DEFINE_FUNC(context_type const&, response_type)
        WEBPP_DEFINE_FUNC(context_type&, response_type&)
        WEBPP_DEFINE_FUNC(context_type, response_type&)
        WEBPP_DEFINE_FUNC(context_type const&, response_type&)
        WEBPP_DEFINE_FUNC(context_type&, response_type const&)
        WEBPP_DEFINE_FUNC(context_type, response_type const&)
        WEBPP_DEFINE_FUNC(context_type const&, response_type const&)
        WEBPP_DEFINE_FUNC(context_type&, request_type)
        WEBPP_DEFINE_FUNC(context_type, request_type)
        WEBPP_DEFINE_FUNC(context_type const&, request_type)
        WEBPP_DEFINE_FUNC(context_type&, request_type&)
        WEBPP_DEFINE_FUNC(context_type, request_type&)
        WEBPP_DEFINE_FUNC(context_type const&, request_type&)
        WEBPP_DEFINE_FUNC(context_type&, request_type const&)
        WEBPP_DEFINE_FUNC(context_type, request_type const&)
        WEBPP_DEFINE_FUNC(context_type const&, request_type const&)


#undef WEBPP_DEFINE_FUNC
#undef WEBPP_DEFINE_FUNC_BASE


        template <typename Callable>
        [[nodiscard]] constexpr auto operator>>(Callable&& callable) const {
            using forward_type = forward_valve<traits_type, Self, stl::remove_cvref_t<Callable>>;
            return rebind_next<forward_type>(stl::forward<Callable>(callable));
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto operator&&(Callable&& callable) const {
            using and_type = and_valve<traits_type, Self, stl::remove_cvref_t<Callable>>;
            return rebind_next<and_type>(stl::forward<Callable>(callable));
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator||(Callable&& callable) const {
            using or_type = or_valve<traits_type, Self, stl::remove_cvref_t<Callable>>;
            return rebind_next<or_type>(stl::forward<Callable>(callable));
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator!() const {
            return rebind_next<not_valve<traits_type, Self>>();
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator+(Callable&& callable) const {
            using postroute_type = postrouting_valve<traits_type, Self, stl::remove_cvref_t<Callable>>;
            return rebind_next<postroute_type>(stl::forward<Callable>(callable));
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator-(Callable&& callable) const {
            using preroute_type = prerouting_valve<traits_type, Self, stl::remove_cvref_t<Callable>>;
            return rebind_next<preroute_type>(stl::forward<Callable>(callable));
        }



        template <typename CallableSegment>
        [[nodiscard]] constexpr auto operator/(CallableSegment&& inp_segment) const {
            using seg_type = segment_valve<traits_type, Self, stl::remove_cvref_t<CallableSegment>>;
            return rebind_next<seg_type>(stl::forward<CallableSegment>(inp_segment));
        }


        template <typename SegT>
            requires(istl::StringView<SegT> || stl::is_array_v<stl::remove_cvref_t<SegT>>)
        [[nodiscard]] constexpr auto operator/(SegT&& inp_segment) const {
            auto const seg_v   = istl::string_viewify(stl::forward<SegT>(inp_segment));
            using seg_t        = stl::remove_cvref_t<decltype(seg_v)>;
            using segment_type = segment_valve<traits_type, Self, seg_t>;
            return rebind_next<segment_type>(seg_v);
        }


      private:
        template <typename NextType, typename T>
        [[nodiscard]] constexpr auto rebind_next(T&& next) const noexcept {
            using optimized_route = route_optimizer<NextType>;
            if constexpr (stl::is_void_v<Self>) {
                return optimized_route::convert(stl::forward<T>(next));
            } else {
                return optimized_route::convert(*static_cast<Self const*>(this), stl::forward<T>(next));
            }
        }


        template <typename NextType>
        [[nodiscard]] constexpr auto rebind_next() const noexcept {
            using optimized_route = route_optimizer<NextType>;
            if constexpr (stl::is_void_v<Self>) {
                return optimized_route::convert();
            } else {
                return optimized_route::convert(*static_cast<Self const*>(this));
            }
        }
    };



    template <Traits TraitsType, typename... Callables>
    struct mangler_valve : valve<TraitsType, mangler_valve<TraitsType, Callables...>> {};


    template <Traits TraitsType, typename... Callables>
    struct forward_valve : valve<TraitsType, forward_valve<TraitsType, Callables...>> {
        using tuple_type   = stl::tuple<Callables...>;
        using context_type = basic_context<TraitsType>;

      private:
        tuple_type callables;

      public:
        constexpr forward_valve(forward_valve const&)                     = default;
        constexpr forward_valve(forward_valve&&) noexcept                 = default;
        constexpr forward_valve& operator=(forward_valve&&) noexcept      = default;
        constexpr forward_valve& operator=(forward_valve const&) noexcept = default;
        constexpr ~forward_valve()                                        = default;

        template <typename C1, typename... Cs>
        constexpr forward_valve(forward_valve<C1> const& c1, Cs&&... funcs) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, C1, Cs...>)
          : forward_valve{c1.callable, stl::forward<Cs>(funcs)...} {}

        template <typename... Cs>
        constexpr forward_valve(Cs&&... funcs) noexcept(stl::is_nothrow_constructible_v<tuple_type, Cs...>)
          : callables{stl::forward<Cs>(funcs)...} {}

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            stl::apply(
              [&ctx]<typename... T>(T&&... funcs) constexpr {
                  (valve_traits<T, context_type>::call_and_set(stl::forward<T>(funcs), ctx), ...);
              },
              callables);
        }

        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... funcs) constexpr {
                  ((out.append(" >>"), valve_to_string(out, funcs)), ...);
              },
              callables);
        }
    };

    template <Traits TraitsType, typename Callable>
    struct forward_valve<TraitsType, Callable> : valve<TraitsType, forward_valve<TraitsType, Callable>> {
        using context_type = basic_context<TraitsType>;

      private:
        Callable next;

      public:
        constexpr forward_valve(Callable inp_next) : next{stl::move(inp_next)} {}
        constexpr forward_valve(forward_valve const&)                     = default;
        constexpr forward_valve(forward_valve&&) noexcept                 = default;
        constexpr forward_valve& operator=(forward_valve&&) noexcept      = default;
        constexpr forward_valve& operator=(forward_valve const&) noexcept = default;
        constexpr ~forward_valve()                                        = default;


        constexpr void operator()(context_type& ctx) {
            using next_traits = valve_traits<Callable, context_type>;
            next_traits::call_and_set(next, ctx);
        }


        constexpr void to_string(istl::String auto& out) const {
            out.append(" >>");
            valve_to_string(out, next);
        }
    };


    template <Traits TraitsType>
    struct forward_valve<TraitsType> : valve<TraitsType, void> {
        constexpr void operator()(basic_context<TraitsType>&) const {}

        constexpr void to_string(istl::String auto& out) const {
            out.append(" >> [empty]");
        }
    };


    template <Traits TraitsType>
    struct forward_valve<TraitsType, void> : forward_valve<TraitsType> {};

    template <Traits TraitsType, typename PreRoute, typename Callable>
    struct prerouting_valve : valve<TraitsType, prerouting_valve<TraitsType, PreRoute, Callable>> {
      private:
        PreRoute pre;
        Callable callable;

      public:
        constexpr prerouting_valve(PreRoute inp_pre, Callable inp_callable)
          : pre{stl::move(inp_pre)},
            callable{stl::move(inp_callable)} {}
        constexpr prerouting_valve(prerouting_valve const&)                     = default;
        constexpr prerouting_valve(prerouting_valve&&) noexcept                 = default;
        constexpr prerouting_valve& operator=(prerouting_valve&&) noexcept      = default;
        constexpr prerouting_valve& operator=(prerouting_valve const&) noexcept = default;
        constexpr ~prerouting_valve()                                           = default;


        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type    = basic_context<TraitsType>;
            using pre_traits      = valve_traits<PreRoute, context_type>;
            using callable_traits = valve_traits<Callable, context_type>;

            auto pre_res = pre_traits::call(pre, ctx);
            if (pre_traits::is_prerouting_positive(pre_res)) {
                pre_traits::set_response(stl::move(pre_res), ctx);
                callable_traits::set_response(callable_traits::call(callable, ctx), ctx);
            }
        }

        [[nodiscard]] constexpr Callable const& get_callable() const noexcept {
            return callable;
        }

        [[nodiscard]] constexpr Callable& get_callable() noexcept {
            return callable;
        }

        [[nodiscard]] constexpr PreRoute& get_preroute() noexcept {
            return pre;
        }

        [[nodiscard]] constexpr PreRoute const& get_preroute() const noexcept {
            return pre;
        }


        constexpr void to_string(istl::String auto& out) const {
            out.append(" -");
            valve_to_string(out, pre);
            out.append(" >>");
            valve_to_string(out, callable);
        }
    };

    template <Traits TraitsType, typename PreRoute>
    struct prerouting_valve<TraitsType, PreRoute, void>
      : valve<TraitsType, prerouting_valve<TraitsType, PreRoute, void>> {
      private:
        PreRoute pre;

      public:
        constexpr prerouting_valve(PreRoute inp_pre) : pre{stl::move(inp_pre)} {}
        constexpr prerouting_valve(prerouting_valve const&)                     = default;
        constexpr prerouting_valve(prerouting_valve&&) noexcept                 = default;
        constexpr prerouting_valve& operator=(prerouting_valve&&) noexcept      = default;
        constexpr prerouting_valve& operator=(prerouting_valve const&) noexcept = default;
        constexpr ~prerouting_valve()                                           = default;

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using pre_traits   = valve_traits<PreRoute, context_type>;

            pre_traits::call_and_set(pre, ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" -");
            valve_to_string(out, pre);
        }
    };


    template <Traits TraitsType, typename Callable, typename PostRoute>
    struct postrouting_valve : valve<TraitsType, postrouting_valve<TraitsType, Callable, PostRoute>> {
      private:
        Callable  callable;
        PostRoute post;

      public:
        constexpr postrouting_valve(Callable inp_callable, PostRoute inp_post)
          : callable{stl::move(inp_callable)},
            post{stl::move(inp_post)} {}
        constexpr postrouting_valve(postrouting_valve const&)                     = default;
        constexpr postrouting_valve(postrouting_valve&&) noexcept                 = default;
        constexpr postrouting_valve& operator=(postrouting_valve&&) noexcept      = default;
        constexpr postrouting_valve& operator=(postrouting_valve const&) noexcept = default;
        constexpr ~postrouting_valve()                                            = default;


        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type    = basic_context<TraitsType>;
            using callable_traits = valve_traits<Callable, context_type>;
            using post_traits     = valve_traits<PostRoute, context_type>;

            auto callable_res = callable_traits::call(callable, ctx);
            if (callable_traits::is_postrouting_positive(callable_res)) {
                callable_traits::set_response(stl::move(callable_res), ctx);
                post_traits::call_and_set(post, ctx);
            }
        }

        [[nodiscard]] constexpr Callable const& get_callable() const noexcept {
            return callable;
        }

        [[nodiscard]] constexpr Callable& get_callable() noexcept {
            return callable;
        }

        [[nodiscard]] constexpr PostRoute& get_postroute() noexcept {
            return post;
        }

        [[nodiscard]] constexpr PostRoute const& get_postroute() const noexcept {
            return post;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" >>");
            valve_to_string(out, callable);
            out.append(" +");
            valve_to_string(out, post);
        }
    };

    template <Traits TraitsType, typename PostRoute>
    struct postrouting_valve<TraitsType, void, PostRoute>
      : valve<TraitsType, postrouting_valve<TraitsType, void, PostRoute>> {
      private:
        PostRoute post;

      public:
        constexpr postrouting_valve(PostRoute inp_post) : post{stl::move(inp_post)} {}
        constexpr postrouting_valve(postrouting_valve const&)                     = default;
        constexpr postrouting_valve(postrouting_valve&&) noexcept                 = default;
        constexpr postrouting_valve& operator=(postrouting_valve&&) noexcept      = default;
        constexpr postrouting_valve& operator=(postrouting_valve const&) noexcept = default;
        constexpr ~postrouting_valve()                                            = default;


        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using post_traits  = valve_traits<PostRoute, context_type>;

            post_traits::call_and_set(post, ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" +");
            valve_to_string(out, post);
        }
    };


    template <Traits TraitsType, typename Callable>
    struct not_valve : valve<TraitsType, not_valve<TraitsType, Callable>> {
        using next_type = Callable;

      private:
        next_type next;

      public:
        constexpr not_valve(Callable inp_next) : next{stl::move(inp_next)} {}
        constexpr not_valve(not_valve const&)                     = default;
        constexpr not_valve(not_valve&&) noexcept                 = default;
        constexpr not_valve& operator=(not_valve&&) noexcept      = default;
        constexpr not_valve& operator=(not_valve const&) noexcept = default;
        constexpr ~not_valve()                                    = default;


        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using ctraits      = valve_traits<next_type, context_type>;

            auto res = ctraits::call(next, ctx);
            if (!ctraits::is_positive(res)) {
                ctraits::set_response(stl::move(res), ctx);
            }
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" !(");
            valve_to_string(out, next);
            out.append(")");
        }
    };


    template <Traits TraitsType>
    struct not_valve<TraitsType, void> : valve<TraitsType, not_valve<TraitsType, void>> {
        constexpr void operator()(basic_context<TraitsType>&) const noexcept {}

        constexpr void to_string(istl::String auto& out) const {
            out.append(" !([empty])");
        }
    };

    template <Traits TraitsType, typename Callable>
    struct negative_valve : valve<TraitsType, negative_valve<TraitsType, Callable>> {
        using next_type = Callable;

      private:
        next_type next;

      public:
        constexpr negative_valve(Callable inp_next) : next{stl::move(inp_next)} {}
        constexpr negative_valve(negative_valve const&)                     = default;
        constexpr negative_valve(negative_valve&&) noexcept                 = default;
        constexpr negative_valve& operator=(negative_valve&&) noexcept      = default;
        constexpr negative_valve& operator=(negative_valve const&) noexcept = default;
        constexpr ~negative_valve()                                         = default;


        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using ctraits      = valve_traits<next_type, context_type>;

            ctraits::call_and_set(next, ctx);
            return false;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" false(");
            valve_to_string(out, next);
            out.append(")");
        }
    };

    template <Traits TraitsType, typename Callable>
    struct positive_valve : valve<TraitsType, positive_valve<TraitsType, Callable>> {
        using next_type = Callable;

      private:
        next_type next;

      public:
        constexpr positive_valve(Callable inp_next) : next{stl::move(inp_next)} {}
        constexpr positive_valve(positive_valve const&)                     = default;
        constexpr positive_valve(positive_valve&&) noexcept                 = default;
        constexpr positive_valve& operator=(positive_valve&&) noexcept      = default;
        constexpr positive_valve& operator=(positive_valve const&) noexcept = default;
        constexpr ~positive_valve()                                         = default;


        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using ctraits      = valve_traits<next_type, context_type>;

            ctraits::call_and_set(next, ctx);
            return true;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" true(");
            valve_to_string(out, next);
            out.append(")");
        }
    };



    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct and_valve : valve<TraitsType, and_valve<TraitsType, LeftCallable, RightCallable>> {
        using left_type  = LeftCallable;
        using right_type = RightCallable;

      private:
        left_type  lhs;
        right_type rhs; // RHS (Right Hand Side) is the "next" valve

      public:
        constexpr and_valve(left_type&& inp_lhs, right_type&& inp_rhs) noexcept
          : lhs{stl::move(inp_lhs)},
            rhs{stl::move(inp_rhs)} {}
        constexpr and_valve(left_type const& inp_lhs, right_type&& inp_rhs) noexcept
          : lhs{inp_lhs},
            rhs{stl::move(inp_rhs)} {}
        constexpr and_valve(left_type const& inp_lhs, right_type const& inp_rhs) noexcept
          : lhs{inp_lhs},
            rhs{inp_rhs} {}
        constexpr and_valve(left_type&& inp_lhs, right_type const& inp_rhs) noexcept
          : lhs{stl::move(inp_lhs)},
            rhs{inp_rhs} {}
        constexpr and_valve(and_valve const&) noexcept            = default;
        constexpr and_valve(and_valve&&) noexcept                 = default;
        constexpr and_valve& operator=(and_valve const&) noexcept = default;
        constexpr and_valve& operator=(and_valve&&) noexcept      = default;
        constexpr ~and_valve()                                    = default;

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using left_traits  = valve_traits<left_type, context_type>;
            using right_traits = valve_traits<right_type, context_type>;

            if (left_traits::call_set_get(lhs, ctx)) {
                right_traits::call_and_set(rhs, ctx);
            }
        }


        constexpr void to_string(istl::String auto& out) const {
            out.append(" (");
            valve_to_string(out, lhs);
            out.append(" && ");
            valve_to_string(out, rhs);
            out.append(")");
        }
    };

    template <Traits TraitsType, typename RightCallable>
    struct and_valve<TraitsType, void, RightCallable> : forward_valve<TraitsType, RightCallable> {};


    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct or_valve : valve<TraitsType, or_valve<TraitsType, LeftCallable, RightCallable>> {
        using left_type  = LeftCallable;
        using right_type = RightCallable;

      private:
        left_type  lhs;
        right_type rhs; // RHS (Right Hand Side) is the "next" valve

      public:
        constexpr or_valve(left_type&& inp_lhs, right_type&& inp_rhs) noexcept
          : lhs{stl::move(inp_lhs)},
            rhs{stl::move(inp_rhs)} {}
        constexpr or_valve(left_type const& inp_lhs, right_type&& inp_rhs) noexcept
          : lhs{inp_lhs},
            rhs{stl::move(inp_rhs)} {}
        constexpr or_valve(left_type const& inp_lhs, right_type const& inp_rhs) noexcept
          : lhs{inp_lhs},
            rhs{inp_rhs} {}
        constexpr or_valve(left_type&& inp_lhs, right_type const& inp_rhs) noexcept
          : lhs{stl::move(inp_lhs)},
            rhs{inp_rhs} {}
        constexpr or_valve(or_valve const&) noexcept            = default;
        constexpr or_valve(or_valve&&) noexcept                 = default;
        constexpr or_valve& operator=(or_valve const&) noexcept = default;
        constexpr or_valve& operator=(or_valve&&) noexcept      = default;
        constexpr ~or_valve()                                   = default;

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using left_traits  = valve_traits<left_type, context_type>;
            using right_traits = valve_traits<right_type, context_type>;

            if (!left_traits::call_set_get(lhs, ctx)) {
                right_traits::call_and_set(rhs, ctx);
            }
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" (");
            valve_to_string(out, lhs);
            out.append(" || ");
            valve_to_string(out, rhs);
            out.append(")");
        }
    };

    template <Traits TraitsType, typename RightCallable>
    struct or_valve<TraitsType, void, RightCallable> : forward_valve<TraitsType, RightCallable> {};


    template <Traits TraitsType, typename... CallableSegments>
    struct segment_valve : valve<TraitsType, segment_valve<TraitsType, CallableSegments...>> {
        using tuple_type = stl::tuple<CallableSegments...>;

        using context_type = basic_context<TraitsType>;

      private:
        tuple_type segments;

      public:
        constexpr segment_valve(segment_valve const&)                     = default;
        constexpr segment_valve(segment_valve&&) noexcept                 = default;
        constexpr segment_valve& operator=(segment_valve&&) noexcept      = default;
        constexpr segment_valve& operator=(segment_valve const&) noexcept = default;
        constexpr ~segment_valve()                                        = default;

        template <typename C1, typename... Cs>
        constexpr segment_valve(segment_valve<TraitsType, C1> const& c1,
                                Cs&&... segs) noexcept(stl::is_nothrow_constructible_v<tuple_type, C1, Cs...>)
          : segments{c1.get_segment(), stl::forward<Cs>(segs)...} {}

        template <typename... CS1, typename... Cs>
        constexpr segment_valve(segment_valve<TraitsType, CS1...> const& c1, Cs&&... segs) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, CS1..., Cs...>)
          : segments{stl::tuple_cat(c1.get_segments(), stl::tuple<Cs...>{stl::forward<Cs>(segs)...})} {}

        template <typename... Cs>
        constexpr segment_valve(Cs&&... segs) noexcept(stl::is_nothrow_constructible_v<tuple_type, Cs...>)
          : segments{stl::forward<Cs>(segs)...} {}

        constexpr tuple_type const& get_segments() const noexcept {
            return segments;
        }

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            stl::apply(
              [&ctx]<typename... T>(T&&... callables) constexpr {
                  (valve_traits<T, context_type>::call_set_get(stl::forward<T>(callables), ctx) && ...);
              },
              segments);
        }


        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... callables) constexpr {
                  ((out.append(" /"), valve_to_string(out, callables)), ...);
              },
              segments);
        }
    };


    template <Traits TraitsType>
    struct segment_valve<TraitsType> : valve<TraitsType, segment_valve<TraitsType>> {
        constexpr void operator()(basic_context<TraitsType>&) const noexcept {}

        constexpr void to_string(istl::String auto& out) const {
            out.append(" / [empty]");
        }
    };

    template <Traits TraitsType, typename... Cs>
    struct segment_valve<TraitsType, void, Cs...> : segment_valve<TraitsType, Cs...> {};


    template <Traits TraitsType, typename CallableSegment>
    struct segment_valve<TraitsType, CallableSegment>
      : valve<TraitsType, segment_valve<TraitsType, CallableSegment>> {
        using context_type = basic_context<TraitsType>;

      private:
        CallableSegment segment;

      public:
        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename C>
            requires(!istl::same_as_cvref<C, segment_valve>)
        constexpr segment_valve(C&& seg) noexcept(stl::is_nothrow_constructible_v<CallableSegment, C>)
          : segment{stl::forward<C>(seg)} {}
        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr segment_valve(segment_valve const&)                     = default;
        constexpr segment_valve(segment_valve&&) noexcept                 = default;
        constexpr segment_valve& operator=(segment_valve&&) noexcept      = default;
        constexpr segment_valve& operator=(segment_valve const&) noexcept = default;
        constexpr ~segment_valve()                                        = default;

        constexpr CallableSegment const& get_segment() const noexcept {
            return segment;
        }

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using segment_traits = valve_traits<CallableSegment, context_type>;

            segment_traits::set_response(segment_traits::call(segment, ctx), ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" /");
            valve_to_string(out, segment);
        }
    };


    template <Traits TraitsType, typename MemPtr>
    struct member_function_callable : valve<TraitsType, member_function_callable<TraitsType, MemPtr>> {
        using member_ptr_type = MemPtr;
        using traits_type     = TraitsType;
        using context_type    = basic_context<traits_type>;
        using mem_traits      = istl::member_function_pointer_traits<member_ptr_type>;
        using response_type   = basic_response<traits_type>;
        using request_type    = basic_request<traits_type>;
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

        constexpr void operator()(context_type& ctx) {
            using callable_traits = valve_traits<method_holder, context_type>;
            callable_traits::set_response(callable_traits::call(holder, ctx), ctx);
        }

        constexpr void set_object(object_ptr inp_obj) noexcept {
            holder.obj = inp_obj;
        }


        template <typename RouterType>
        constexpr void setup(RouterType& router) {
            for (auto& object : router.objects) {
                if (object.type() == typeid(object_type)) {
                    set_method(&stl::any_cast<object_type>(object));
                    return;
                }
            }

            // default constructing it if it's possible and use that object
            if constexpr (stl::is_default_constructible_v<object_type>) {
                router.objects.emplace_back(object_type{});
                set_object(&stl::any_cast<object_type>(router.objects.back()));
            } else if constexpr (stl::is_constructible_v<object_type, RouterType&>) {
                router.objects.emplace_back(object_type{router});
                set_object(&stl::any_cast<object_type>(router.objects.back()));
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
            out.append(" >>");
            valve_to_string(out, holder.mem_ptr);
        }
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTER_VALVES_HPP
