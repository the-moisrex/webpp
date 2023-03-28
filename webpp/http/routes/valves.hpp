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
    using next_route = istl::function_ref<bool(basic_context<TraitsType>&)>;

    /**
     * Checks if the type is an actual `valve<traits_type, ...>` type.
     */
    template <typename T>
    concept Valve = requires {
                        typename T::type_traits;
                        requires Traits<typename T::traits_type>;
                        requires stl::is_invocable_v<T, basic_context<typename T::traits_type>&>;
                    };

    template <typename TraitsType, typename T>
    concept Mangler = Traits<TraitsType> && stl::is_invocable_v<T,                          // type
                                                                basic_context<TraitsType>&, // context
                                                                next_route<TraitsType>      // next valve
                                                                >;

    template <typename TraitsType, typename T>
    concept RouteSetter = Traits<TraitsType> && stl::is_invocable_v<T, basic_dynamic_router<TraitsType>&>;

    template <typename Router, typename T>
    concept ValveRequiresSetup = requires(T callable, Router& router) { callable.setup(router); };


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


    template <typename PreRoute>
    struct prerouting_valve : PreRoute {};

    template <typename PostRoute>
    struct postrouting_valve : PostRoute {};

    template <Traits TraitsType, typename Callable>
    struct callable_valve;
    template <typename Callable>
    struct not_valve;
    template <typename Callable>
    struct negative_valve;
    template <typename Callable>
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
    template <Traits TraitsType, typename ManglerType, typename NextCallable>
    struct mangler_valve;



    static constexpr struct endpath_valve {

        template <Traits TraitsType>
        [[nodiscard]] constexpr bool operator()(basic_context<TraitsType>& ctx) const noexcept {
            return ctx.path_traverser().at_end();
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" / endpath");
        }
    } endpath;



    template <typename Callable, typename ContextType>
    struct valve_traits {
        using callable_type      = stl::remove_cvref_t<Callable>;
        using context_type       = ContextType;
        using request_type       = typename context_type::request_type;
        using response_type      = typename context_type::response_type;
        using response_body_type = typename response_type::body_type;
        using invocable_inorder_type =
          istl::invocable_inorder<callable_type, context_type&, request_type&, response_type&>;
        using traits_type      = typename context_type::traits_type;
        using string_view_type = traits::string_view<traits_type>;
        using return_type      = stl::conditional_t<istl::StringViewifiable<Callable>,
                                               string_view_type,
                                               stl::remove_cvref_t<typename invocable_inorder_type::result>>;

        // static_assert(invocable_inorder_type::value, "We're not able to call your route.");

        static constexpr auto call(Callable&     callable,
                                   context_type& ctx) noexcept(invocable_inorder_type::is_nothrow)
            requires(invocable_inorder_type::value)
        {
            return istl::invoke_inorder(callable, ctx, ctx.request, ctx.response);
        }

        template <typename T>
            requires(istl::cvref_as<T, Callable> && istl::StringViewifiable<Callable>)
        static constexpr auto call(T&&           segment,
                                   context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            return ctx.check_segment(stl::forward<T>(segment));
        }

        template <typename R>
        static constexpr bool is_positive(R&& ret) noexcept {
            using ret_type = stl::remove_cvref_t<R>;
            if constexpr (stl::same_as<ret_type, bool>) {
                return ret;
            } else if constexpr (stl::is_void_v<ret_type>) {
                return true;
            } else if constexpr (stl::is_pointer_v<ret_type>) {
                return ret != nullptr;
            } else if constexpr (istl::Optional<ret_type>) {
                return bool{ret};
            } else {
                // istl::nothing_type, HTTPResponse<ret_type>, ...
                return false;
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
            requires istl::cvref_as<C, Callable>
        static constexpr void call_set(C&&           segment,
                                       context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<C>(segment), ctx);
            } else {
                set_response(call(stl::forward<C>(segment), ctx), ctx);
            }
        }


        template <typename C>
            requires istl::cvref_as<C, Callable>
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
            requires stl::constructible_from<C, T...>
        static constexpr C convert(T&&... args) noexcept(stl::is_nothrow_constructible_v<C, T...>) {
            return {stl::forward<T>(args)...};
        }
    };

    // remove the unneeded void type
    template <template <typename, typename...> typename ValveTemplate, typename TraitsType, typename... T>
    struct route_optimizer<ValveTemplate<TraitsType, void, T...>>
      : route_optimizer<ValveTemplate<TraitsType, T...>> {};


    // move pre-routing hooks to the left because they supposed to be called before everything
    template <template <typename, typename, typename, typename...> typename ValveTemplate,
              typename Left,
              typename Pre,
              typename... Right,
              typename TraitsType>
    struct route_optimizer<ValveTemplate<TraitsType, Left, prerouting_valve<Pre>, Right...>>
      : route_optimizer<ValveTemplate<TraitsType,
                                      prerouting_valve<typename route_optimizer<Pre>::type>,
                                      typename route_optimizer<Left>::type,
                                      typename route_optimizer<Right>::type...>> {

        using left_parent = route_optimizer<Left>;
        using pre_parent  = route_optimizer<Pre>;
        using parent_type =
          route_optimizer<ValveTemplate<TraitsType,
                                        prerouting_valve<typename route_optimizer<Pre>::type>,
                                        typename route_optimizer<Left>::type,
                                        typename route_optimizer<Right>::type...>>;

        template <istl::cvref_as<Left> LT, istl::cvref_as<prerouting_valve<Pre>> PT, typename... RT>
            requires istl::cvref_as<Right..., RT...>
        static constexpr auto convert(LT&& lhs, PT&& pre, RT&&... rhs) noexcept {
            return parent_type::convert(
              pre_parent::convert(stl::forward<PT>(pre)),
              left_parent::convert(stl::forward<LT>(lhs)),
              route_optimizer<stl::remove_cvref_t<RT>>::convert(stl::forward<RT>(rhs))...);
        }
    };

    // move post-routing hooks to the right because they supposed to be called after everything
    template <template <typename, typename, typename...> typename ValveTemplate,
              typename Post,
              typename... Right,
              typename TraitsType>
        requires(sizeof...(Right) > 0)
    struct route_optimizer<ValveTemplate<TraitsType, postrouting_valve<Post>, Right...>>
      : route_optimizer<ValveTemplate<TraitsType,
                                      typename route_optimizer<Right>::type...,
                                      postrouting_valve<typename route_optimizer<Post>::type>>> {
        using post_parent = route_optimizer<Post>;
        using parent_type =
          route_optimizer<ValveTemplate<TraitsType,
                                        typename route_optimizer<Right>::type...,
                                        postrouting_valve<typename route_optimizer<Post>::type>>>;
        template <istl::cvref_as<postrouting_valve<Post>> PT, typename... RT>
            requires(istl::cvref_as<Right..., RT...>)
        static constexpr auto convert(PT&& post, RT&&... rhs) noexcept {
            return parent_type::convert(
              route_optimizer<stl::remove_cvref_t<RT>>::convert(stl::forward<RT>(rhs))...,
              post_parent::convert(stl::forward<PT>(post)));
        }
    };

    // !!C == C
    template <typename C>
        requires requires {
                     typename C::traits_type;
                     requires requires(C callable, basic_context<typename C::traits_type> & ctx) {
                                  { callable(ctx) } -> stl::same_as<bool>;
                              };
                 }
    struct route_optimizer<not_valve<not_valve<C>>> : route_optimizer<C> {
        using parent_type = route_optimizer<C>;
        using return_type = typename parent_type::type;

        template <istl::cvref_as<C> T>
        static constexpr return_type convert(T&& next) noexcept {
            return parent_type::convert(stl::forward<T>(next));
        }
    };

    // !!C == +C
    template <typename C>
    struct route_optimizer<not_valve<not_valve<C>>> : route_optimizer<positive_valve<C>> {
        using parent_type = route_optimizer<positive_valve<C>>;
        using return_type = typename parent_type::type;

        template <istl::cvref_as<C> T>
        static constexpr return_type convert(T&& next) noexcept {
            return parent_type::convert(stl::forward<T>(next));
        }
    };


    // C || !C == +C
    template <typename C, typename TraitsType>
    struct route_optimizer<or_valve<TraitsType, C, not_valve<C>>> : route_optimizer<positive_valve<C>> {
        using parent_type = route_optimizer<positive_valve<C>>;
        using return_type = typename parent_type::type;

        template <istl::cvref_as<C> T>
        static constexpr return_type convert(T&& next) noexcept {
            return parent_type::convert(stl::forward<T>(next));
        }
    };

    // !C || C == +C
    template <typename C, typename TraitsType>
    struct route_optimizer<or_valve<TraitsType, not_valve<C>, C>> : route_optimizer<positive_valve<C>> {
        using parent_type = route_optimizer<positive_valve<C>>;
        using return_type = typename parent_type::type;

        template <istl::cvref_as<C> T>
        static constexpr return_type convert(T&& next) noexcept {
            return parent_type::convert(stl::forward<T>(next));
        }
    };

    // C && C == C
    template <typename C, typename TraitsType>
    struct route_optimizer<and_valve<TraitsType, C, C>> : route_optimizer<C> {
        using parent_type = route_optimizer<C>;
        using return_type = typename parent_type::type;

        template <istl::cvref_as<C> T>
        static constexpr return_type convert(T&& next, [[maybe_unused]] T&&) noexcept {
            return parent_type::convert(stl::forward<T>(next));
        }
    };

    // C && !C == -C
    template <typename C, typename TraitsType>
    struct route_optimizer<and_valve<TraitsType, C, not_valve<C>>> : route_optimizer<negative_valve<C>> {
        using parent_type = route_optimizer<negative_valve<C>>;
        using return_type = typename parent_type::type;

        template <istl::cvref_as<C> T>
        static constexpr return_type convert(T&& next) noexcept {
            return parent_type::convert(stl::forward<T>(next));
        }
    };

    // !C && C == -C
    template <typename C, typename TraitsType>
    struct route_optimizer<and_valve<TraitsType, not_valve<C>, C>> : route_optimizer<negative_valve<C>> {
        using parent_type = route_optimizer<negative_valve<C>>;
        using return_type = typename parent_type::type;

        template <istl::cvref_as<C> T>
        static constexpr return_type convert(T next) noexcept {
            return parent_type::convert(stl::forward<T>(next));
        }
    };

    // remove first voids
    template <typename... C, typename TraitsType>
    struct route_optimizer<forward_valve<TraitsType, void, C...>>
      : route_optimizer<forward_valve<TraitsType, C...>> {
        using parent_type = route_optimizer<forward_valve<TraitsType, C...>>;
        using return_type = typename parent_type::type;

        template <typename... T>
            requires istl::cvref_as<C..., T...>
        static constexpr return_type convert(T&&... next_callables) noexcept {
            return parent_type::convert(stl::forward<T>(next_callables)...);
        }
    };

    // remove double forwarding (flatten the type)
    template <typename C1, typename... C, typename TraitsType>
    struct route_optimizer<forward_valve<TraitsType, forward_valve<TraitsType, C1>, C...>>
      : route_optimizer<forward_valve<TraitsType, C1, C...>> {
        using parent_type       = route_optimizer<forward_valve<TraitsType, C1, C...>>;
        using return_type       = typename parent_type::type;
        using sub_callable_type = forward_valve<TraitsType, C1>;

        template <typename... N>
            requires istl::cvref_as<C..., N...>
        static constexpr return_type convert(istl::cvref_as<sub_callable_type> auto&& sub_callable,
                                             N&&... next_callables) noexcept {
            return parent_type::convert(sub_callable.get_callable(), stl::forward<N>(next_callables)...);
        }
    };

    // forward<segment, ...> = segment<forward, ...>
    // template <typename... C, typename... SC, typename TraitsType>
    // struct route_optimizer<forward_valve<TraitsType, segment_valve<TraitsType, SC...>, C...>>
    //   : route_optimizer<segment_valve<TraitsType, SC..., route_optimizer<forward_valve<TraitsType, C...>>>>
    //   {
    //     using next_t       = route_optimizer<forward_valve<TraitsType, C...>>;
    //     using next_type    = typename next_t::type;
    //     using out_seg_type = segment_valve<TraitsType, SC..., next_type>;
    //     using parent_type  = route_optimizer<out_seg_type>;
    //     using return_type  = typename parent_type::type;
    //     using seg_type     = segment_valve<TraitsType, SC...>;
    //
    //     template <istl::cvref_as<seg_type> SegT, typename NewCallable>
    //     static constexpr return_type convert(SegT&& seg, NewCallable&& next) noexcept {
    //         return parent_type::convert(stl::forward<SegT>(seg),
    //                                     next_t::convert(stl::forward<NewCallable>(next)));
    //     }
    // };

    // remove double segmenting (flatten the type)
    template <typename... C1s, typename... C, typename TraitsType>
    struct route_optimizer<segment_valve<TraitsType, segment_valve<TraitsType, C1s...>, C...>>
      : route_optimizer<segment_valve<TraitsType, C1s..., C...>> {
        using parent_type  = route_optimizer<segment_valve<TraitsType, C1s..., C...>>;
        using return_type  = typename parent_type::type;
        using sub_seg_type = segment_valve<TraitsType, C1s...>;

        template <istl::cvref_as<sub_seg_type> SS, typename... CT>
            requires istl::cvref_as<C..., CT...>
        static constexpr return_type convert(SS&& sub_seg, CT&&... next_segs) noexcept {
            return parent_type::convert(sub_seg.get_segment(), stl::forward<CT>(next_segs)...);
        }
    };


    // todo: C || Positive == ?
    // todo: C && Negative == ?





    template <Traits TraitsType = default_dynamic_traits, typename Self = void>
    struct valve {
        using traits_type = TraitsType;

      private:
        using context_type  = basic_context<traits_type>;
        using response_type = basic_response<traits_type>;
        using request_type  = basic_request<traits_type>;

      public:
        template <typename Callable>
        [[nodiscard]] constexpr auto operator>>(Callable&& callable) const {
            return rebind_next<forward_valve, traits_type, Self>(stl::forward<Callable>(callable));
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto operator%(Callable&& callable) const {
            return rebind_next<mangler_valve, traits_type, Self>(stl::forward<Callable>(callable));
        }

        template <typename Callable>
        [[nodiscard]] constexpr auto operator&&(Callable&& callable) const {
            return rebind_next<and_valve, traits_type, Self>(stl::forward<Callable>(callable));
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator||(Callable&& callable) const {
            return rebind_next<or_valve, traits_type, Self>(stl::forward<Callable>(callable));
        }


        [[nodiscard]] constexpr auto operator!() const {
            return rebind_next<not_valve, Self>();
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator+(Callable&& callable) const {
            return rebind_next<forward_valve, postrouting_valve, traits_type, Self>(
              stl::forward<Callable>(callable));
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator-(Callable&& callable) const {
            return rebind_next<forward_valve, prerouting_valve, traits_type, Self>(
              stl::forward<Callable>(callable));
        }



        template <typename CallableSegment>
        [[nodiscard]] constexpr auto operator/(CallableSegment&& inp_segment) const {
            return rebind_next<segment_valve, traits_type, Self>(stl::forward<CallableSegment>(inp_segment));
        }


        template <typename SegT>
            requires(istl::StringView<SegT> || stl::is_array_v<stl::remove_cvref_t<SegT>>)
        [[nodiscard]] constexpr auto operator/(SegT&& inp_segment) const {
            auto const seg_v = istl::string_viewify(stl::forward<SegT>(inp_segment));
            return rebind_next<segment_valve, traits_type, Self>(seg_v);
        }

        template <typename SegT>
            requires(istl::StringView<SegT> || stl::is_array_v<stl::remove_cvref_t<SegT>>)
        [[nodiscard]] constexpr auto operator%(SegT&& inp_segment) const {
            auto const seg_v = istl::string_viewify(stl::forward<SegT>(inp_segment));
            return rebind_next<segment_valve, traits_type, Self>(seg_v, endpath);
        }



        // Convert Custom Contexts into dynamic context
        // For compatibility with the static router
        template <Context CtxT>
            requires(!istl::cvref_as<CtxT, context_type> && !stl::is_void_v<Self>)
        [[nodiscard]] constexpr response_type operator()(CtxT&& inp_ctx) {
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

        template <template <typename...> typename Templ, typename... T, typename Arg>
            requires stl::is_member_function_pointer_v<stl::remove_cvref_t<Arg>>
        [[nodiscard]] constexpr auto rebind_next(Arg&& next) const noexcept {
            using mem_type = member_function_callable<traits_type, stl::remove_cvref_t<Arg>>;
            return rebind_next<Templ, T...>(mem_type{stl::forward<Arg>(next)});
        }

        template <template <typename...> typename Templ, typename... T, typename... Args>
        [[nodiscard]] constexpr auto rebind_next(Args&&... nexts) const noexcept {
            using valve_type      = Templ<T..., stl::remove_cvref_t<Args>...>;
            using optimized_route = route_optimizer<valve_type>;
            if constexpr (stl::is_void_v<Self>) {
                return optimized_route::convert(stl::forward<Args>(nexts)...);
            } else {
                return optimized_route::convert(*self(), stl::forward<Args>(nexts)...);
            }
        }


        // for pre/post routing
        template <template <typename...> typename WrapperTempl,
                  template <typename...>
                  typename Templ,
                  typename... T,
                  typename Arg>
            requires stl::is_member_function_pointer_v<stl::remove_cvref_t<Arg>>
        [[nodiscard]] constexpr auto rebind_next(Arg&& next) const noexcept {
            using mem_type = member_function_callable<traits_type, stl::remove_cvref_t<Arg>>;
            return rebind_next<WrapperTempl, Templ, T...>(mem_type{stl::forward<Arg>(next)});
        }

        // for pre/post routing
        template <template <typename...> typename WrapperTempl,
                  template <typename...>
                  typename Templ,
                  typename... T,
                  typename... Args>
        [[nodiscard]] constexpr auto rebind_next(Args&&... nexts) const noexcept {
            using templ_type      = Templ<stl::remove_cvref_t<Args>...>;
            using valve_type      = WrapperTempl<T..., templ_type>;
            using optimized_route = route_optimizer<valve_type>;
            if constexpr (stl::is_void_v<Self>) {
                return optimized_route::convert(templ_type{stl::forward<Args>(nexts)...});
            } else {
                return optimized_route::convert(*self(), templ_type{stl::forward<Args>(nexts)...});
            }
        }
    };



    template <Traits TraitsType, typename Callable>
    struct callable_valve : valve<TraitsType, callable_valve<TraitsType, Callable>> {
        using valve_type   = valve<TraitsType, callable_valve<TraitsType, Callable>>;
        using context_type = basic_context<TraitsType>;

      private:
        Callable next;

      public:
        constexpr callable_valve(Callable inp_next) : next{stl::move(inp_next)} {}
        constexpr callable_valve(callable_valve const&)                     = default;
        constexpr callable_valve(callable_valve&&) noexcept                 = default;
        constexpr callable_valve& operator=(callable_valve&&) noexcept      = default;
        constexpr callable_valve& operator=(callable_valve const&) noexcept = default;
        constexpr ~callable_valve()                                         = default;


        using valve_type::operator();

        constexpr void operator()(context_type& ctx) {
            using next_traits = valve_traits<Callable, context_type>;
            next_traits::call_set(next, ctx);
        }


        constexpr void to_string(istl::String auto& out) const {
            valve_to_string(out, next);
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, Callable>)
        constexpr void setup(RouterT& router) {
            next.setup(router);
        }
    };


    template <Traits TraitsType, typename Callable>
        requires Valve<Callable>
    struct callable_valve<TraitsType, Callable> : Callable {};



    template <Traits TraitsType, typename NextCallable, typename ManglerType>
    struct mangler_valve : valve<TraitsType, mangler_valve<TraitsType, ManglerType, NextCallable>> {
        using valve_type   = valve<TraitsType, mangler_valve<TraitsType, ManglerType, NextCallable>>;
        using context_type = basic_context<TraitsType>;

      private:
        struct next_callable {
          private:
            NextCallable next;

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

            constexpr bool operator()(context_type& ctx) {
                return valve_traits<NextCallable, context_type>::call_set_get(next, ctx);
            }
        } next;

        ManglerType mangler;

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

        constexpr void operator()(context_type& ctx) {
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


    template <Traits TraitsType, typename... Callables>
    struct forward_valve : valve<TraitsType, forward_valve<TraitsType, Callables...>> {
        using valve_type   = valve<TraitsType, forward_valve<TraitsType, Callables...>>;
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
            requires stl::constructible_from<tuple_type, C1, Cs...>
        constexpr forward_valve(forward_valve<C1> const& c1, Cs&&... funcs) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, C1, Cs...>)
          : forward_valve{c1.callable, stl::forward<Cs>(funcs)...} {}

        template <typename... Cs>
            requires stl::constructible_from<tuple_type, Cs...>
        constexpr forward_valve(Cs&&... funcs) noexcept(stl::is_nothrow_constructible_v<tuple_type, Cs...>)
          : callables{stl::forward<Cs>(funcs)...} {}

        using valve_type::operator();

        constexpr void operator()(context_type& ctx) {
            stl::apply(
              [&ctx]<typename... T>(T&&... funcs) constexpr {
                  (valve_traits<T, context_type>::call_set(stl::forward<T>(funcs), ctx), ...);
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


        template <typename RouterT>
            requires((ValveRequiresSetup<RouterT, Callables> || ...))
        constexpr void setup(RouterT& router) {
            stl::apply(
              [&router]<typename... T>(T&&... inp_callables) constexpr {
                  (([]([[maybe_unused]] auto&& callable, [[maybe_unused]] RouterT& router) constexpr {
                       if constexpr (ValveRequiresSetup<RouterT, T>) {
                           callable.setup(router);
                       }
                   })(inp_callables, router),
                   ...);
              },
              callables);
        }
    };

    template <Traits TraitsType, typename Callable>
    struct forward_valve<TraitsType, Callable> : valve<TraitsType, forward_valve<TraitsType, Callable>> {
        using valve_type   = valve<TraitsType, forward_valve<TraitsType, Callable>>;
        using context_type = basic_context<TraitsType>;

      private:
        Callable next;

      public:
        template <stl::convertible_to<Callable> C>
        constexpr forward_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}
        constexpr forward_valve(forward_valve const&)                     = default;
        constexpr forward_valve(forward_valve&&) noexcept                 = default;
        constexpr forward_valve& operator=(forward_valve&&) noexcept      = default;
        constexpr forward_valve& operator=(forward_valve const&) noexcept = default;
        constexpr ~forward_valve()                                        = default;


        using valve_type::operator();

        constexpr void operator()(context_type& ctx) {
            using next_traits = valve_traits<Callable, context_type>;
            next_traits::call_set(next, ctx);
        }


        constexpr void to_string(istl::String auto& out) const {
            out.append(" >>");
            valve_to_string(out, next);
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, Callable>)
        constexpr void setup(RouterT& router) {
            next.setup(router);
        }
    };


    template <Traits TraitsType>
    struct forward_valve<TraitsType> : valve<TraitsType, void> {
        using valve_type   = valve<TraitsType, void>;
        using context_type = basic_context<TraitsType>;

        using valve_type::operator();

        constexpr void operator()(context_type&) const {}

        constexpr void to_string(istl::String auto& out) const {
            out.append(" >> [empty]");
        }
    };


    template <Traits TraitsType>
    struct forward_valve<TraitsType, void> : forward_valve<TraitsType> {};


    template <typename Callable>
    struct not_valve : valve<typename Callable::traits_type, not_valve<Callable>> {
        using traits_type  = typename Callable::traits_type;
        using valve_type   = valve<traits_type, not_valve<Callable>>;
        using next_type    = Callable;
        using context_type = basic_context<traits_type>;

      private:
        next_type next;

      public:
        template <stl::convertible_to<Callable> C>
        constexpr not_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}
        constexpr not_valve(not_valve const&)                     = default;
        constexpr not_valve(not_valve&&) noexcept                 = default;
        constexpr not_valve& operator=(not_valve&&) noexcept      = default;
        constexpr not_valve& operator=(not_valve const&) noexcept = default;
        constexpr ~not_valve()                                    = default;


        using valve_type::operator();

        [[nodiscard]] constexpr bool operator()(context_type& ctx) {
            using ctraits = valve_traits<next_type, context_type>;
            return !ctraits::call_set_get(next, ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" !(");
            valve_to_string(out, next);
            out.append(")");
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, Callable>)
        constexpr void setup(RouterT& router) {
            next.setup(router);
        }
    };


    template <typename Callable>
    struct negative_valve : valve<typename Callable::traits_type, negative_valve<Callable>> {
        using traits_type  = typename Callable::traits_type;
        using valve_type   = valve<traits_type, negative_valve<Callable>>;
        using next_type    = Callable;
        using context_type = basic_context<traits_type>;

      private:
        next_type next;

      public:
        template <stl::convertible_to<Callable> C>
        constexpr negative_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}
        constexpr negative_valve(negative_valve const&)                     = default;
        constexpr negative_valve(negative_valve&&) noexcept                 = default;
        constexpr negative_valve& operator=(negative_valve&&) noexcept      = default;
        constexpr negative_valve& operator=(negative_valve const&) noexcept = default;
        constexpr ~negative_valve()                                         = default;


        using valve_type::operator();

        constexpr bool operator()(context_type& ctx) {
            using ctraits = valve_traits<next_type, context_type>;

            ctraits::call_set(next, ctx);
            return false;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" false(");
            valve_to_string(out, next);
            out.append(")");
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, Callable>)
        constexpr void setup(RouterT& router) {
            next.setup(router);
        }
    };

    template <typename Callable>
    struct positive_valve : valve<typename Callable::traits_type, positive_valve<Callable>> {
        using traits_type  = typename Callable::traits_type;
        using valve_type   = valve<traits_type, positive_valve<Callable>>;
        using next_type    = Callable;
        using context_type = basic_context<traits_type>;

      private:
        next_type next;

      public:
        template <stl::convertible_to<Callable> C>
        constexpr positive_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}
        constexpr positive_valve(positive_valve const&)                     = default;
        constexpr positive_valve(positive_valve&&) noexcept                 = default;
        constexpr positive_valve& operator=(positive_valve&&) noexcept      = default;
        constexpr positive_valve& operator=(positive_valve const&) noexcept = default;
        constexpr ~positive_valve()                                         = default;


        using valve_type::operator();

        constexpr bool operator()(context_type& ctx) {
            using ctraits = valve_traits<next_type, context_type>;

            ctraits::call_set(next, ctx);
            return true;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" true(");
            valve_to_string(out, next);
            out.append(")");
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, Callable>)
        constexpr void setup(RouterT& router) {
            next.setup(router);
        }
    };



    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct and_valve : valve<TraitsType, and_valve<TraitsType, LeftCallable, RightCallable>> {
        using valve_type   = valve<TraitsType, and_valve<TraitsType, LeftCallable, RightCallable>>;
        using left_type    = LeftCallable;
        using right_type   = RightCallable;
        using context_type = basic_context<TraitsType>;

      private:
        left_type  lhs;
        right_type rhs; // RHS (Right Hand Side) is the "next" valve

      public:
        template <stl::convertible_to<left_type> L, stl::convertible_to<right_type> R>
        constexpr and_valve(L&& inp_lhs, R&& inp_rhs) noexcept
          : lhs{stl::forward<L>(inp_lhs)},
            rhs{stl::forward<R>(inp_rhs)} {}
        constexpr and_valve(and_valve const&) noexcept            = default;
        constexpr and_valve(and_valve&&) noexcept                 = default;
        constexpr and_valve& operator=(and_valve const&) noexcept = default;
        constexpr and_valve& operator=(and_valve&&) noexcept      = default;
        constexpr ~and_valve()                                    = default;

        using valve_type::operator();

        constexpr bool operator()(context_type& ctx) {
            using left_traits  = valve_traits<left_type, context_type>;
            using right_traits = valve_traits<right_type, context_type>;

            if (left_traits::call_set_get(lhs, ctx)) {
                return right_traits::call_set_get(rhs, ctx);
            }
            return false;
        }


        constexpr void to_string(istl::String auto& out) const {
            out.append(" (");
            valve_to_string(out, lhs);
            out.append(" && ");
            valve_to_string(out, rhs);
            out.append(")");
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, left_type> || ValveRequiresSetup<RouterT, right_type>)
        constexpr void setup(RouterT& router) {
            if constexpr (ValveRequiresSetup<RouterT, left_type>) {
                lhs.setup(router);
            }
            if constexpr (ValveRequiresSetup<RouterT, right_type>) {
                rhs.setup(router);
            }
        }
    };

    template <Traits TraitsType, typename RightCallable>
    struct and_valve<TraitsType, void, RightCallable> : forward_valve<TraitsType, RightCallable> {};


    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct or_valve : valve<TraitsType, or_valve<TraitsType, LeftCallable, RightCallable>> {
        using valve_type   = valve<TraitsType, or_valve<TraitsType, LeftCallable, RightCallable>>;
        using left_type    = LeftCallable;
        using right_type   = RightCallable;
        using context_type = basic_context<TraitsType>;

      private:
        left_type  lhs;
        right_type rhs; // RHS (Right Hand Side) is the "next" valve

      public:
        template <stl::convertible_to<left_type> L, stl::convertible_to<right_type> R>
        constexpr or_valve(L&& inp_lhs, R&& inp_rhs) noexcept
          : lhs{stl::forward<L>(inp_lhs)},
            rhs{stl::forward<R>(inp_rhs)} {}
        constexpr or_valve(or_valve const&) noexcept            = default;
        constexpr or_valve(or_valve&&) noexcept                 = default;
        constexpr or_valve& operator=(or_valve const&) noexcept = default;
        constexpr or_valve& operator=(or_valve&&) noexcept      = default;
        constexpr ~or_valve()                                   = default;

        using valve_type::operator();

        constexpr bool operator()(context_type& ctx) {
            using left_traits  = valve_traits<left_type, context_type>;
            using right_traits = valve_traits<right_type, context_type>;

            if (!left_traits::call_set_get(lhs, ctx)) {
                return right_traits::call_set_get(rhs, ctx);
            }
            return false;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" (");
            valve_to_string(out, lhs);
            out.append(" || ");
            valve_to_string(out, rhs);
            out.append(")");
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, left_type> || ValveRequiresSetup<RouterT, right_type>)
        constexpr void setup(RouterT& router) {
            if constexpr (ValveRequiresSetup<RouterT, left_type>) {
                lhs.setup(router);
            }
            if constexpr (ValveRequiresSetup<RouterT, right_type>) {
                rhs.setup(router);
            }
        }
    };

    template <Traits TraitsType, typename RightCallable>
    struct or_valve<TraitsType, void, RightCallable> : forward_valve<TraitsType, RightCallable> {};


    template <Traits TraitsType, typename... CallableSegments>
    struct segment_valve : valve<TraitsType, segment_valve<TraitsType, CallableSegments...>> {
        using valve_type   = valve<TraitsType, segment_valve<TraitsType, CallableSegments...>>;
        using tuple_type   = stl::tuple<CallableSegments...>;
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
            requires istl::are_all_v<stl::is_convertible,
                                     istl::type_list<CallableSegments...>,
                                     istl::type_list<C1, Cs...>>
        constexpr segment_valve(segment_valve<TraitsType, C1> const& c1,
                                Cs&&... segs) noexcept(stl::is_nothrow_constructible_v<tuple_type, C1, Cs...>)
          : segments{c1.get_segment(), stl::forward<Cs>(segs)...} {}

        template <typename... CS1, typename... Cs>
            requires istl::are_all_v<stl::is_convertible,
                                     istl::type_list<CallableSegments...>,
                                     istl::type_list<CS1..., Cs...>>
        constexpr segment_valve(segment_valve<TraitsType, CS1...> const& c1, Cs&&... segs) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, CS1..., Cs...>)
          : segments{stl::tuple_cat(c1.get_segments(), stl::tuple<Cs...>{stl::forward<Cs>(segs)...})} {}

        template <typename... Cs>
            requires istl::
              are_all_v<stl::is_convertible, istl::type_list<CallableSegments...>, istl::type_list<Cs...>>
          constexpr segment_valve(Cs&&... segs) noexcept(stl::is_nothrow_constructible_v<tuple_type, Cs...>)
          : segments{stl::forward<Cs>(segs)...} {}

        constexpr tuple_type const& get_segments() const noexcept {
            return segments;
        }

        using valve_type::operator();

        constexpr bool operator()(context_type& ctx) {
            return stl::apply(
              [&ctx]<typename... T>(T&&... callables) constexpr {
                  return (valve_traits<T, context_type>::call_set_get(stl::forward<T>(callables), ctx) &&
                          ...);
              },
              segments);
        }


        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... callables) constexpr {
                  (([&out](auto&& callable) constexpr {
                       if constexpr (istl::StringViewifiable<T>) {
                           out.append(" /");
                       }
                       valve_to_string(out, callable);
                   })(callables),
                   ...);
              },
              segments);
        }


        template <typename RouterT>
            requires((ValveRequiresSetup<RouterT, CallableSegments> || ...))
        constexpr void setup(RouterT& router) {
            stl::apply(
              [&router]<typename... T>(T&&... callables) constexpr {
                  (([]([[maybe_unused]] auto&& callable, [[maybe_unused]] RouterT& router) constexpr {
                       if constexpr (ValveRequiresSetup<RouterT, T>) {
                           callable.setup(router);
                       }
                   })(callables, router),
                   ...);
              },
              segments);
        }
    };


    template <Traits TraitsType>
    struct segment_valve<TraitsType> : valve<TraitsType, segment_valve<TraitsType>> {
        using valve_type   = valve<TraitsType, segment_valve<TraitsType>>;
        using context_type = basic_context<TraitsType>;

        using valve_type::operator();

        constexpr void operator()(context_type&) const noexcept {}

        constexpr void to_string(istl::String auto& out) const {
            out.append(" / [empty]");
        }
    };

    template <Traits TraitsType, typename... Cs>
    struct segment_valve<TraitsType, void, Cs...> : segment_valve<TraitsType, Cs...> {};


    template <Traits TraitsType, typename CallableSegment>
    struct segment_valve<TraitsType, CallableSegment>
      : valve<TraitsType, segment_valve<TraitsType, CallableSegment>> {
        using valve_type   = valve<TraitsType, segment_valve<TraitsType, CallableSegment>>;
        using context_type = basic_context<TraitsType>;

      private:
        CallableSegment segment;

      public:
        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <stl::convertible_to<CallableSegment> C>
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

        using valve_type::operator();

        constexpr bool operator()(context_type& ctx) {
            using segment_traits = valve_traits<CallableSegment, context_type>;
            return segment_traits::call_set_get(segment, ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" /");
            valve_to_string(out, segment);
        }


        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, CallableSegment>)
        constexpr void setup(RouterT& router) {
            segment.setup(router);
        }
    };


    template <Traits TraitsType, typename MemPtr>
    struct member_function_callable : valve<TraitsType, member_function_callable<TraitsType, MemPtr>> {
        using valve_type      = valve<TraitsType, member_function_callable<TraitsType, MemPtr>>;
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

        using valve_type::operator();

        constexpr void operator()(context_type& ctx) {
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


} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTER_VALVES_HPP
