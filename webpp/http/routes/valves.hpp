// Created by moisrex on 3/20/23.

#ifndef WEBPP_HTTP_ROUTER_VALVES_HPP
#define WEBPP_HTTP_ROUTER_VALVES_HPP

#include "../../std/function_ref.hpp"
#include "../../std/string.hpp"
#include "../../std/string_view.hpp"
#include "../../std/tuple.hpp"
#include "../../std/types.hpp"
#include "../../traits/traits.hpp"
#include "../http_concepts.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "./context.hpp"

#include <any>
#include <typeinfo>

namespace webpp::http {

    template <Traits>
    struct basic_dynamic_router;

    template <typename TraitsType>
    using next_route = istl::function_ref<bool(basic_context<TraitsType>&)>;

    /**
     * Checks if the type is a valve type.
     */
    template <typename TraitsType, typename T>
    concept ValveOf = Traits<TraitsType> &&
                      (stl::is_invocable_v<T, basic_context<TraitsType>&> || istl::StringViewifiable<T>);

    template <typename T>
    concept Valve = ValveOf<default_dynamic_traits, T>;

    template <typename TraitsType, typename T>
    concept Mangler = Traits<TraitsType> && stl::is_invocable_v<T,                          // type
                                                                basic_context<TraitsType>&, // context
                                                                next_route<TraitsType>      // next valve
                                                                >;

    template <typename TraitsType, typename T>
    concept RouteSetter = Traits<TraitsType> && stl::is_invocable_v<T, basic_dynamic_router<TraitsType>&>;

    template <typename Router, typename T>
    concept ValveRequiresSetup = requires(T callable, Router& router) { callable.setup(router); };

    template <typename T>
    concept Valvifiable = requires(T obj) {
                              { valvify<T>(obj) } -> Valve;
                          };



    template <istl::String StrT, typename Callable>
    static constexpr void valve_to_string(StrT& out, Callable& func) {
        using mem_traits = istl::member_function_pointer_traits<Callable>;
        if constexpr (istl::StringViewifiable<Callable>) {
            out.append(" \"");
            out += istl::string_viewify(func);
            out.append("\"");
        } else if constexpr (requires { func.to_string(out); }) {
            func.to_string(out);
        } else if constexpr (mem_traits::value) {
            out.append(" ");
            out.append(istl::type_name<typename mem_traits::type>());
            out.append("::");
            out.append(istl::type_name<Callable>());
        } else {
            out.append(" ");
            StrT name{istl::type_name<Callable>(), out.get_allocator()};
            istl::replace_all(name, "webpp::http::", "");
            istl::replace_all(name, "webpp::", "");
            out.append(name);
        }
    }


    template <typename Callable>
    struct not_valve;
    template <typename Callable>
    struct negative_valve;
    template <typename Callable>
    struct positive_valve;
    template <typename LeftCallable, typename RightCallable>
    struct and_valve;
    template <typename LeftCallable, typename RightCallable>
    struct or_valve;
    template <typename... Callables>
    struct forward_valve;
    template <typename... CallableSegments>
    struct segment_valve;
    template <typename MemPtr>
    struct member_function_callable;
    template <typename ManglerType, typename NextCallable>
    struct mangler_valve;


    template <typename... PreRoutes>
    struct prerouting_valve : forward_valve<PreRoutes...> {
        using forward_valve<PreRoutes...>::forward_valve;
    };

    template <typename... PostRoutes>
    struct postrouting_valve : forward_valve<PostRoutes...> {
        using forward_valve<PostRoutes...>::forward_valve;
    };


    static constexpr struct endpath_valve {

        template <Traits TraitsType>
        [[nodiscard]] constexpr bool operator()(basic_context<TraitsType>& ctx) const noexcept {
            return ctx.path_traverser().at_end();
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" / endpath");
        }
    } endpath;



    // General Valvifier
    template <typename T>
    [[nodiscard]] static constexpr auto valvify(T&& next) noexcept {
        return stl::forward<T>(next);
    }

    // Member Function Pointer Valvifier
    template <typename T>
        requires stl::is_member_function_pointer_v<stl::remove_cvref_t<T>>
    [[nodiscard]] static constexpr member_function_callable<stl::remove_cvref_t<T>>
    valvify(T&& next) noexcept {
        return {stl::forward<T>(next)};
    }

    // String Views Valvifier
    template <typename T>
        requires(istl::StringView<T> || istl::StringLiteral<T>)
    [[nodiscard]] static constexpr auto valvify(T&& next) noexcept {
        return istl::string_viewify(stl::forward<T>(next));
    }

    // String object is passed
    template <istl::String T>
    [[nodiscard]] static constexpr auto valvify(T&& next) {
        return stl::forward<T>(next);
    }


    template <typename T>
    using valvified_type = stl::remove_cvref_t<decltype(valvify(stl::declval<T>()))>;

    template <typename Callable, typename ContextType = context>
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

        template <istl::cvref_as<Callable> C>
            requires(invocable_inorder_type::value)
        static constexpr return_type
          call(C&& callable, context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            return istl::invoke_inorder(callable, ctx, ctx.request, ctx.response);
        }

        template <typename T>
            requires(istl::cvref_as<T, Callable> && istl::StringViewifiable<Callable>)
        static constexpr auto call(T&& segment, context_type& ctx) noexcept {
            return ctx.check_segment(stl::forward<T>(segment));
        }

        template <Valvifiable T>
            requires(istl::cvref_as<T, Callable> && !istl::StringViewifiable<Callable> &&
                     !invocable_inorder_type::value)
        static constexpr auto call(T&&           segment,
                                   context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            return call(valvify(stl::forward<T>(segment)), ctx);
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




    template <typename Self = void>
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
            if constexpr (stl::same_as<Self, callable_type>) { // C && C = C
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
            if constexpr (stl::same_as<Self, callable_type>) { // C || C = C
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
                } else { // !!C = +C
                    return rebind_self<positive_valve>(self()->unwrap());
                }
            } else {
                return rebind_next<not_valve>();
            }
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator+(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            return rebind_next<postrouting_valve>(stl::forward<Callable>(callable));
        }


        template <typename Callable>
        [[nodiscard]] constexpr auto operator-(Callable&& callable) const {
            using callable_type = stl::remove_cvref_t<Callable>;
            if constexpr (istl::template_of_v<prerouting_valve, Self>) {
                if constexpr (istl::template_of_v<prerouting_valve, callable_type>) {
                    return rebind_self<prerouting_valve>(
                      stl::tuple_cat(self()->as_tuple(), callable.as_tuple()));
                } else { // append the callable to the end of the prerouting valve
                    return rebind_self<prerouting_valve>(
                      stl::tuple_cat(self()->as_tuple(),
                                     stl::make_tuple(valvify(stl::forward<Callable>(callable)))));
                }
            } else {
                return rebind_next<prerouting_valve>(stl::forward<Callable>(callable));
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
                using valve_type = Templ<T..., valvified_type<Args>...>;
                return valve_type{valvify(stl::forward<Args>(nexts))...};
            } else {
                using valve_type = Templ<Self, T..., valvified_type<Args>...>;
                return valve_type{*self(), valvify(stl::forward<Args>(nexts))...};
            }
        }


        // We get a tuple<>, but we want to pass all the individual elements to `valvify`
        // Attention: This means we will be calling `valvify` multiple times for each element
        template <template <typename...> typename Templ, typename... T, typename... TupT>
        [[nodiscard]] constexpr auto rebind_self(stl::tuple<TupT...>&& nexts) const {
            using valve_type = Templ<T..., valvified_type<TupT>...>;
            return ([&nexts]<stl::size_t... I>(stl::index_sequence<I...>) constexpr {
                return valve_type{valvify(stl::get<I>(nexts))...};
            })(stl::index_sequence_for<TupT...>{});
        }

        template <template <typename...> typename Templ, typename... T, typename... Args>
        [[nodiscard]] constexpr auto rebind_self(Args&&... nexts) const {
            using valve_type = Templ<T..., valvified_type<Args>...>;
            return valve_type{valvify(stl::forward<Args>(nexts))...};
        }
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
                  ((out.append(" >>"), valve_to_string(out, funcs)), ...);
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


    template <typename Callable>
    struct not_valve : valve<not_valve<Callable>> {
        using valve_type = valve<not_valve<Callable>>;
        using next_type  = Callable;

      private:
        [[no_unique_address]] next_type next;

      public:
        template <stl::convertible_to<Callable> C>
        constexpr not_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}
        constexpr not_valve(not_valve const&)                     = default;
        constexpr not_valve(not_valve&&) noexcept                 = default;
        constexpr not_valve& operator=(not_valve&&) noexcept      = default;
        constexpr not_valve& operator=(not_valve const&) noexcept = default;
        constexpr ~not_valve()                                    = default;


        using valve_type::operator();

        template <Traits TraitsType>
        [[nodiscard]] constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using ctraits = valve_traits<next_type, basic_context<TraitsType>>;
            return !ctraits::call_set_get(next, ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" !(");
            valve_to_string(out, next);
            out.append(")");
        }

        [[nodiscard]] constexpr next_type const& unwrap() const noexcept {
            return next;
        }

        template <typename RouterT>
            requires(ValveRequiresSetup<RouterT, Callable>)
        constexpr void setup(RouterT& router) {
            next.setup(router);
        }
    };


    template <typename Callable>
    struct negative_valve : valve<negative_valve<Callable>> {
        using valve_type = valve<negative_valve<Callable>>;
        using next_type  = Callable;

      private:
        [[no_unique_address]] next_type next;

      public:
        template <stl::convertible_to<Callable> C>
        constexpr negative_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}
        constexpr negative_valve(negative_valve const&)                     = default;
        constexpr negative_valve(negative_valve&&) noexcept                 = default;
        constexpr negative_valve& operator=(negative_valve&&) noexcept      = default;
        constexpr negative_valve& operator=(negative_valve const&) noexcept = default;
        constexpr ~negative_valve()                                         = default;


        using valve_type::operator();

        template <Traits TraitsType>
        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using ctraits      = valve_traits<next_type, context_type>;

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
    struct positive_valve : valve<positive_valve<Callable>> {
        using valve_type = valve<positive_valve<Callable>>;
        using next_type  = Callable;

      private:
        [[no_unique_address]] next_type next;

      public:
        template <stl::convertible_to<Callable> C>
        constexpr positive_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}
        constexpr positive_valve(positive_valve const&)                     = default;
        constexpr positive_valve(positive_valve&&) noexcept                 = default;
        constexpr positive_valve& operator=(positive_valve&&) noexcept      = default;
        constexpr positive_valve& operator=(positive_valve const&) noexcept = default;
        constexpr ~positive_valve()                                         = default;


        using valve_type::operator();

        template <Traits TraitsType>
        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using ctraits      = valve_traits<next_type, context_type>;

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



    template <typename LeftCallable, typename RightCallable>
    struct and_valve : valve<and_valve<LeftCallable, RightCallable>> {
        using valve_type = valve<and_valve<LeftCallable, RightCallable>>;
        using left_type  = LeftCallable;
        using right_type = RightCallable;

      private:
        [[no_unique_address]] left_type  lhs;
        [[no_unique_address]] right_type rhs; // RHS (Right Hand Side) is the "next" valve

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

        template <Traits TraitsType>
        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
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


    template <typename LeftCallable, typename RightCallable>
    struct or_valve : valve<or_valve<LeftCallable, RightCallable>> {
        using valve_type = valve<or_valve<LeftCallable, RightCallable>>;
        using left_type  = LeftCallable;
        using right_type = RightCallable;

      private:
        [[no_unique_address]] left_type  lhs;
        [[no_unique_address]] right_type rhs; // RHS (Right Hand Side) is the "next" valve

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

        template <Traits TraitsType>
        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
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


    template <typename... CallableSegments>
    struct segment_valve : valve<segment_valve<CallableSegments...>>, stl::tuple<CallableSegments...> {
        using valve_type = valve<segment_valve<CallableSegments...>>;
        using tuple_type = stl::tuple<CallableSegments...>;

        template <typename... Args>
            requires stl::constructible_from<tuple_type, Args...>
        constexpr segment_valve(Args&&... args) noexcept(stl::is_nothrow_constructible_v<tuple_type, Args...>)
          : tuple_type{stl::forward<Args>(args)...} {}

        constexpr segment_valve(segment_valve const&)                     = default;
        constexpr segment_valve(segment_valve&&) noexcept                 = default;
        constexpr segment_valve& operator=(segment_valve&&) noexcept      = default;
        constexpr segment_valve& operator=(segment_valve const&) noexcept = default;
        constexpr ~segment_valve()                                        = default;

        using valve_type::operator();

        template <Traits TraitsType>
        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            return stl::apply(
              [&ctx]<typename... T>(T&&... callables) constexpr {
                  return (valve_traits<T, context_type>::call_set_get(stl::forward<T>(callables), ctx) &&
                          ...);
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
              [&out]<typename... T>(T&&... callables) constexpr {
                  (([&out](auto&& callable) constexpr {
                       if constexpr (istl::StringViewifiable<T>) {
                           out.append(" /");
                       }
                       valve_to_string(out, callable);
                   })(callables),
                   ...);
              },
              as_tuple());
        }


        template <typename RouterT>
            requires((ValveRequiresSetup<RouterT, CallableSegments> || ...))
        constexpr void setup(RouterT& inp_router) {
            stl::apply(
              [&inp_router]<typename... T>(T&&... callables) constexpr {
                  (([]([[maybe_unused]] auto&& callable, [[maybe_unused]] RouterT& router) constexpr {
                       if constexpr (ValveRequiresSetup<RouterT, T>) {
                           callable.setup(router);
                       }
                   })(callables, inp_router),
                   ...);
              },
              as_tuple());
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


} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTER_VALVES_HPP
