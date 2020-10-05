// Created by moisrex on 11/1/19.

#ifndef WEBPP_ROUTES_ROUTE_H
#define WEBPP_ROUTES_ROUTE_H

#include "../../logs/log_concepts.hpp"
#include "../../utils/functional.hpp"
#include "./context.hpp"
#include "./route_concepts.hpp"
#include "../../std/optional.hpp"
#include "../../std/type_traits.hpp"

#include <utility>

namespace webpp {

    /**
     * Check if we can convert T to U
     * @tparam T
     * @tparam U
     */
    template <typename T, typename U>
    struct can_convert
      : stl::integral_constant<bool, (!stl::is_void_v<T> && !stl::is_void_v<U>) &&(
                                       stl::is_convertible_v<T, U> || stl::is_constructible_v<T, U> ||
                                       stl::is_assignable_v<T, U>)> {};

    template <typename T, typename U>
    constexpr bool can_convert_v = can_convert<T, U>::value;

    /**
     * Check if we can convert T to a string
     * @tparam Traits
     * @tparam U
     */
    template <typename Traits, typename U>
    struct can_convert_to_string
      : stl::integral_constant<bool, (can_convert_v<U, typename Traits::string_type> ||
                                      can_convert_v<U, typename Traits::string_view_type>)> {};

    template <typename Traits, typename U>
    constexpr bool can_convert_to_string_v = can_convert_to_string<Traits, U>::value;


    /**
     * Handle special return types here
     */
//    constexpr auto handle_results(auto&& ctx, auto&& res) noexcept {
//        using res_t    = decltype(res);
//        using ctx_type = stl::remove_cvref_t<decltype(ctx)>;
//        if constexpr (istl::Optional<res_t>) {
//            // pass it out to the router, we're not able to handle this here
//            return stl::forward<res_t>(res);
//        } else if constexpr (Context<res_t>) {
//            // we're not able to do a context switching here,
//            // route: is responsible for sub-route context switching
//            // router: is responsible for entry-route context switching
//            return stl::forward<res_t>(res);
//        } else if constexpr (stl::is_same_v<res_t, bool>) {
//            return stl::forward<res_t>(res);
//        } else if constexpr (Response<res_t>) {
//            return stl::forward<res_t>(res);
//        } else if constexpr (ConstructibleWithResponse<typename ctx_type::response_type, res_t>) {
//            return ctx.template response<>(stl::forward<res_t>(res));
//            // todo: consider "response extension" injection in order to get the right response type
//        } else {
//            // let's just ignore the result
//            return true;
//        }
//    }

    template <typename Route, typename... Args>
    concept is_callable_route =
      stl::is_invocable_v<stl::decay_t<Route>, stl::remove_cvref_t<Args>...> ||
      stl::is_invocable_v<stl::decay_t<Route>, Args...> ||
      stl::is_invocable_v<stl::decay_t<Route>, stl::add_lvalue_reference_t<stl::remove_cvref_t<Args>>...>;

    template <typename Route, typename... Args>
    concept is_nothrow_callable_route =
      stl::is_nothrow_invocable_v<stl::decay_t<Route>, stl::remove_cvref_t<Args>...> ||
      stl::is_nothrow_invocable_v<stl::decay_t<Route>, Args...> ||
      stl::is_nothrow_invocable_v<stl::decay_t<Route>,
                                  stl::add_lvalue_reference_t<stl::remove_cvref_t<Args>>...>;


    constexpr auto call_route(auto&& _route, Context auto&& ctx, Request auto const& req) noexcept {
        using route_type   = decltype(_route);
        using request_type = decltype(req);
        using ctx_type     = stl::remove_cvref_t<decltype(ctx)>;
        using context_type = stl::add_lvalue_reference_t<ctx_type>;

        constexpr auto run_and_catch = [](auto&& callable, Context auto const& ctx, auto&&... args) noexcept {
            using return_type = stl::invoke_result_t<decltype(callable), decltype(args)...>;
            if constexpr (stl::is_nothrow_invocable_v<decltype(callable), decltype(args)...>) {
                // It's noexcept, we call it knowing that.
                return callable(stl::forward<decltype(args)>(args)...);
            } else if constexpr (stl::is_invocable_v<decltype(callable), decltype(args)...>) {

                if constexpr (stl::is_void_v<return_type>) {
                    try {
                        callable(stl::forward<decltype(args)>(args)...);
                    } catch (...) {
                        // nothing to do
                    }
                } else if constexpr (stl::same_as<return_type, bool>) {
                    try {
                        return callable(stl::forward<decltype(args)>(args)...);
                    } catch (...) { return false; }
                } else if constexpr (istl::Optional<return_type>) {
                    try {
                        return callable(stl::forward<decltype(args)>(args)...);
                    } catch (...) {
                        // return 500 error on failure hoping the response type supports it
                        // todo: add more error handling stuff here to the result
                        return typename return_type::value_type{500u};
                    }
                } else {
                    using optional_type = decltype(stl::make_optional(callable(stl::forward<decltype(args)>(args)...)));
                    try {
                        return stl::make_optional(callable(stl::forward<decltype(args)>(args)...));
                    } catch (...) {
                        return optional_type{stl::nullopt};
                    }
                }

            } else {
                return ctx.error(500u, stl::invalid_argument(
                                         "The specified route is not valid. We're not able to call it."));
            }
        };

        /**
         * Handle special return types here
         */
        constexpr auto handle_results = [](auto&& ctx, auto&& res) noexcept {
            using res_t = stl::remove_cvref_t<decltype(res)>;
            if constexpr (istl::Optional<res_t>) {
                // pass it out to the router, we're not able to handle this here
                return stl::forward<decltype(res)>(res);
            } else if constexpr (Context<res_t>) {
                // we're not able to do a context switching here,
                // route: is responsible for sub-route context switching
                // router: is responsible for entry-route context switching
                return stl::forward<decltype(res)>(res);
            } else if constexpr (stl::is_same_v<res_t, bool>) {
                return stl::forward<decltype(res)>(res);
            } else if constexpr (Response<res_t>) {
                return stl::forward<decltype(res)>(res);
            } else if constexpr (ConstructibleWithResponse<typename ctx_type::response_type, res_t>) {
                return ctx.response(stl::forward<decltype(res)>(res));
                // todo: consider "response extension" injection in order to get the right response type
//            } else if constexpr (istl::ConvertibleToStringView<res_t>) {
//                return ctx.template response<string_response>(istl::to_string_view(stl::forward<decltype(res)>(res)));
            } else {
                // let's just ignore the result
                return true;
            }
        };

        if constexpr (stl::is_invocable_v<route_type, context_type, request_type>) {
            // requires a context and a request
            if constexpr (!stl::is_void_v<stl::invoke_result_t<route_type, context_type, request_type>>) {
                return handle_results(ctx, run_and_catch(_route, ctx, ctx, req));
            } else {
                run_and_catch(_route, ctx, ctx, req);
            }
        } else if constexpr (stl::is_invocable_v<route_type, request_type, context_type>) {
            // requires a request and a context
            if constexpr (!stl::is_void_v<stl::invoke_result_t<route_type, request_type, context_type>>) {
                return handle_results(ctx, run_and_catch(_route, ctx, req, ctx));
            } else {
                run_and_catch(_route, ctx, req, ctx);
            }
        } else if constexpr (stl::is_invocable_v<route_type, context_type>) {
            // gets a context
            if constexpr (!stl::is_void_v<stl::invoke_result_t<route_type, context_type>>) {
                return handle_results(ctx, run_and_catch(_route, ctx, ctx));
            } else {
                run_and_catch(_route, ctx, ctx);
            }
        } else if constexpr (stl::is_invocable_v<route_type, request_type>) {
            // requires a request
            if constexpr (!stl::is_void_v<stl::invoke_result_t<route_type, request_type>>) {
                return handle_results(ctx, run_and_catch(_route, ctx, req));
            } else {
                run_and_catch(_route, ctx, req);
            }
        } else if constexpr (stl::is_invocable_v<route_type>) {
            // requires nothing
            if constexpr (!stl::is_void_v<stl::invoke_result_t<route_type>>) {
                return handle_results(ctx, run_and_catch(_route, ctx));
            } else {
                run_and_catch(_route, ctx);
            }
        } else {
            throw stl::invalid_argument(
              "We don't know how to call your entry route. Change your route's signature.");
        }
    }


    // constexpr auto call_route(auto&& _route, Context auto&& ctx, Request auto const& req) noexcept(
    // is_nothrow_callable_route<decltype(_route), decltype(ctx),
    //  decltype(req)>) requires(is_callable_route<decltype(_route), decltype(ctx),
    //  decltype(req)>) {
    //    // requires a context and a request
    //    if constexpr (!stl::is_void_v<stl::invoke_result_t<decltype(_route), decltype(ctx), decltype(req)>>)
    //    {
    //        return handle_results(ctx, _route(ctx, req));
    //    } else {
    //        _route(ctx, req);
    //    }
    //}
    //
    // constexpr auto call_route(auto&& _route, Context auto&& ctx, Request auto const& req) noexcept(
    //      is_nothrow_callable_route<decltype(_route), decltype(req),
    //                                decltype(ctx)>) requires(is_callable_route<decltype(_route),
    //                                decltype(req),
    //                                                                           decltype(ctx)>) {
    //        // requires a request and a context
    //        if constexpr (!stl::is_void_v<stl::invoke_result_t<decltype(_route), decltype(req),
    //        decltype(ctx)>>)
    //        {
    //            return handle_results(ctx, _route(req, ctx));
    //        } else {
    //            _route(req, ctx);
    //        }
    //    }
    //
    //    constexpr auto call_route(auto&& _route, Context auto&& ctx, Request auto const& req) noexcept(
    //      is_nothrow_callable_route<decltype(_route), decltype(ctx)>)
    //      requires(is_callable_route<decltype(_route),
    //                                                                                             decltype(ctx)>)
    //                                                                                             {
    //        // requires a context
    //        if constexpr (!stl::is_void_v<stl::invoke_result_t<decltype(_route), decltype(ctx)>>) {
    //            return handle_results(ctx, _route(stl::forward<decltype(ctx)>(ctx)));
    //        } else {
    //            _route(stl::forward<decltype(ctx)>(ctx));
    //        }
    //    }
    //
    //
    //    constexpr auto call_route(auto&& _route, Context auto&& ctx, Request auto const& req) noexcept(
    //      is_nothrow_callable_route<decltype(_route), decltype(req)>)
    //      requires(is_callable_route<decltype(_route),
    //                                                                                             decltype(req)>)
    //                                                                                             {
    //        // requires a request
    //        if constexpr (!stl::is_void_v<stl::invoke_result_t<decltype(_route), decltype(req)>>) {
    //            return handle_results(ctx, _route(req));
    //        } else {
    //            _route(req);
    //        }
    //    }
    //
    //    constexpr auto call_route(auto&& _route, Context auto&& ctx, Request auto const& /* req */)
    //    noexcept(
    //      is_nothrow_callable_route<decltype(_route)>) requires(is_callable_route<decltype(_route)>) {
    //        // requires nothing
    //        if constexpr (!stl::is_void_v<stl::invoke_result_t<decltype(_route)>>) {
    //            return handle_results(ctx);
    //        } else {
    //            _route();
    //        }
    //    }


    //        template <typename C, Context ContextType>
    //        bool call_route(C& c, ContextType& context) noexcept {
    //            using req_t     = typename ContextType::request_type const&;
    //            using res_t     = typename ContextType::response_type&;
    //            using callable  = stl::decay_t<C>;
    //            using context_t = ContextType&;
    //            auto callback   = stl::forward<C>(c);
    //
    //            constexpr auto handle_exception = [](auto err) {
    //                // todo: use the log system here
    //            };
    //
    //            // TODO: add more overrides. You can simulate "dependency injection" here
    //
    //            if constexpr (stl::is_invocable_v<callable, req_t>) {
    //                return handle_callback_return_type(
    //                  run_and_catch(handle_exception, callback, context.request));
    //                //            } else if constexpr (stl::is_invocable_v<callable, res_t>) {
    //                //                return handle_callback_return_type(
    //                //                  run_and_catch(handle_exception, callback, context.response));
    //                //            } else if constexpr (stl::is_invocable_v<callable, req_t, res_t>) {
    //                //                return handle_callback_return_type(
    //                //                  run_and_catch(handle_exception, callback, context.request,
    //                //                  context.response));
    //                //            } else if constexpr (stl::is_invocable_v<callable, res_t, req_t>) {
    //                //                return handle_callback_return_type(
    //                //                  run_and_catch(handle_exception, callback, context.response,
    //                //                  context.request));
    //            } else if constexpr (stl::is_invocable_v<callable, context_t>) {
    //                return handle_callback_return_type(run_and_catch(handle_exception, callback,
    //                context));
    //            } else if constexpr (stl::is_invocable_v<callable, context_t, req_t>) {
    //                return handle_callback_return_type(
    //                  run_and_catch(handle_exception, callback, context, context.request));
    //            } else if constexpr (stl::is_invocable_v<callable, req_t, context_t>) {
    //                return handle_callback_return_type(
    //                  run_and_catch(handle_exception, callback, context.request, context));
    //            } else if constexpr (stl::is_invocable_v<callable>) {
    //                return handle_callback_return_type(run_and_catch(handle_exception, callback));
    //            } else {
    //                // todo: move this throw in the log system
    //                throw stl::invalid_argument("The specified route cannot be called.");
    //            }
    //        }


    enum class logical_operators { none, AND, OR, XOR };

    template <typename RouteType, logical_operators Op, typename NextRoute>
    struct basic_route : public make_inheritable<RouteType> {
        using next_route_type = NextRoute;
        using super_t         = make_inheritable<RouteType>;

        constexpr static logical_operators op = Op;
        next_route_type                    next;

        //            constexpr basic_route(super_t&&       super = super_t{},
        //                                  next_route_type&& _next = next_route_type{}) noexcept
        //              : super_t(stl::forward<RouteType>(super)),
        //                next(stl::forward<next_route_type>(_next)) {}
        //
        constexpr basic_route(auto&& super, auto&& _next = next_route_type{}) noexcept
          : super_t(stl::forward<decltype(super)>(super)),
            next(stl::forward<decltype(_next)>(_next)) {}

        constexpr basic_route(basic_route const& v) noexcept = default;
        constexpr basic_route(basic_route&& v) noexcept      = default;

        using super_t::operator=;
        using super_t::operator();
    };

    template <typename RouteType>
    struct basic_route<RouteType, logical_operators::none, void> : public make_inheritable<RouteType> {
        using super_t = make_inheritable<RouteType>;

        template <typename... Args>
        constexpr basic_route(Args&&... args) noexcept : super_t{stl::forward<Args>(args)...} {}

        constexpr basic_route(basic_route const&) noexcept = default;
        constexpr basic_route(basic_route&&) noexcept      = default;

        using super_t::operator=;
        using super_t::operator();
    };

    template <>
    struct basic_route<void, logical_operators::none, void> {
        void operator()(Context auto const&) const noexcept {}
    };

    template <typename RouteType = void, logical_operators Op = logical_operators::none,
              typename NextRouteType = void>
    struct route : public basic_route<stl::remove_cvref_t<RouteType>, Op, stl::remove_cvref_t<NextRouteType>> {

        using route_type                      = stl::remove_cvref_t<RouteType>;
        using next_route_type                 = stl::remove_cvref_t<NextRouteType>;
        using self_type                       = route<RouteType, Op, NextRouteType>;
        static constexpr logical_operators op = Op;

      private:
        using super_t = basic_route<route_type, Op, next_route_type>;

        // todo: use the istl::lazy_conditional
        template <bool Condition, template <typename...> typename T1, typename T2, typename... C>
        struct lazy_conditional {};

        template <template <typename...> typename T1, typename T2, typename... C>
        struct lazy_conditional<true, T1, T2, C...> {
            using type = T1<C...>;
        };

        template <template <typename...> typename T1, typename T2, typename... C>
        struct lazy_conditional<false, T1, T2, C...> {
            using type = T2;
        };

        template <bool Condition, template <typename...> typename T1, typename T2, typename... C>
        using lazy_conditional_t = typename lazy_conditional<Condition, T1, T2, C...>::type;


        template <typename R, typename C, bool IF>
        struct is_switching_context {
            static constexpr bool value = false;
        };

        template <typename R, typename C>
        struct is_switching_context<R, C, true> {
            static constexpr bool value = Context<stl::invoke_result_t<R, C>>;
        };

        template <typename R, typename C>
        static constexpr bool is_switching_context_v =
          is_switching_context<R, C, !stl::is_void_v<R> && stl::is_invocable_v<R, C>>::value;


        /**
         * Get the "switched context typed" if it's R is a route, otherwise, return the context type if
         * the operator(C) of R is doing context switching, otherwise, return the specified default C
         * @tparam R
         * @tparam C
         */
        template <typename R, typename C>
        struct lazy_switched_context_type {
            using type = lazy_conditional_t<is_switching_context_v<R, C>, stl::invoke_result_t, C, R, C>;
        };

        template <Route R, typename C>
        struct lazy_switched_context_type<R, C> {
            using type = typename R::template switched_context_type<C>;
        };

      public:
        constexpr static bool is_route_valid      = !stl::is_void_v<route_type>;
        constexpr static bool is_next_route_valid = !stl::is_void_v<next_route_type>;

        template <typename R, typename C>
        using route_switched_context_type =
          stl::conditional_t<stl::is_invocable_v<R, C>, typename lazy_switched_context_type<R, C>::type, C>;

        template <typename C>
        using route_context_type = route_switched_context_type<route_type, C>;

        template <typename C>
        using next_route_context_type = route_switched_context_type<next_route_type, C>;

        template <typename C>
        using switched_context_type = next_route_context_type<route_context_type<C>>;

        template <typename C>
        static constexpr bool is_switching_context_recursive = stl::is_same_v<switched_context_type<C>, C>;


      public:
        constexpr route() noexcept : super_t{} {}
        constexpr route(route const&) noexcept = default;
        constexpr route(route&&) noexcept      = default;
        route operator=(route const&) = delete;
        route operator=(route&&) = delete;

        template <typename... Args>
        constexpr route(Args&&... args) noexcept : super_t{stl::forward<Args>(args)...} {}

        //            /**
        //             * Run the migration
        //             * @return the response or the context or ...
        //             */
        //            template <typename... Args>
        //            auto operator()(Args&&... args) noexcept {
        //                return super_t::operator()(stl::forward<Args>(args)...);
        //            }

      private:
        template <logical_operators TheOp, typename NewRouteType>
        [[nodiscard]] constexpr auto set_next(NewRouteType&& new_route) const noexcept {
            if constexpr (stl::is_void_v<next_route_type>) {
                // this part will only execute when the "next_route_type" is void

                // the first way (A<X, void> and B<Y, void> === A<X, B<Y, void>>
                return route<route_type, TheOp, NewRouteType>(*this, stl::forward<NewRouteType>(new_route));
            } else {
                // this means this function has a "next" valve already,
                // so it goes to the next's next valve
                // this way we recursively create a valve type and return it.
                route<next_route_type, TheOp, NewRouteType> n{super_t::next,
                                                              stl::forward<NewRouteType>(new_route)};
                return route<route_type, op, decltype(n)>{*this, n};
            }
        }

        //            template <logical_operators TheOp, typename NewRouteType>
        //            [[nodiscard]] constexpr auto set_next_bool_op(NewRouteType&& new_route) const
        //            noexcept {
        //                if constexpr (stl::is_void_v<route_type>) {
        //
        //                    if constexpr (stl::is_void_v<next_route_type>) {
        //                        return route<NewRouteType, TheOp, void>(
        //                          stl::forward<NewRouteType>(new_route));
        //                    } else {
        //                        return route<NewRouteType, TheOp, next_route_type>(
        //                          stl::forward<NewRouteType>(new_route), super_t::next);
        //                    }
        //                } else {
        //                    route<next_route_type, TheOp, NewRouteType> n{super_t::next,
        //                                                                  stl::forward<NewRouteType>(new_route)};
        //                    return route<route_type, op, decltype(n)>{*this, n};
        //                }
        //            }
        //
      public:
        [[nodiscard]] constexpr auto operator&&(/*PotentialRoute*/ auto&& new_route) const noexcept {
            return set_next<logical_operators::AND>(stl::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] constexpr auto operator&(/*PotentialRoute*/ auto&& new_route) const noexcept {
            return set_next<logical_operators::AND>(stl::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] constexpr auto operator||(/*PotentialRoute*/ auto&& new_route) const noexcept {
            return set_next<logical_operators::OR>(stl::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] constexpr auto operator|(/*PotentialRoute*/ auto&& new_route) const noexcept {
            return set_next<logical_operators::OR>(stl::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] constexpr auto operator^(/*PotentialRoute*/ auto&& new_route) const noexcept {
            return set_next<logical_operators::XOR>(stl::forward<decltype(new_route)>(new_route));
        }

      private:
        template <typename AppType, typename NewRoutePtr, bool is_noexcept>
        struct route_with_router_pointer {
            NewRoutePtr        new_route;
            AppType*           app = nullptr;
            [[nodiscard]] auto operator()(auto... args) noexcept(is_noexcept) {
                // yes we know app must not be nullptr, but route should only be used with router,
                // and the router will set the app if it can otherwise the router can throw an
                // error at compile time or at least at initialization time instead of when the
                // a user's request comes to this specific route. This makes sure the developers
                // will catch this error sooner.
                assert(app != nullptr); // You did not supply the correct app to the router

                return (app->*new_route)(stl::forward<decltype(args)>(args)...);
            }
        };

      public:
        [[nodiscard]] constexpr auto operator>>=(auto&& new_route) const noexcept {
            using rt = decltype(new_route);
            if constexpr (stl::is_member_function_pointer_v<rt>) {
                using mem_func_ptr_t = member_function_pointer<rt>;
                using app_type       = typename mem_func_ptr_t::type;
                return set_next<logical_operators::none>(
                  route_with_router_pointer<app_type, stl::remove_cvref<decltype(new_route)>,
                                            mem_func_ptr_t::is_noexcept>{});
            } else /*if constexpr (PotentialRoute<rt, switched_context_type<fake_context_type>>)*/ {
                return set_next<logical_operators::none>(stl::forward<decltype(new_route)>(new_route));
                //                } else {
                //                    // todo: write tests for this:
                //                    return set_next<logical_operators::none>([=](auto... args) {
                //                        //                        static_assert(stl::is_invocable_v<decltype(new_route),
                //                        //                        decltype(args)...>,
                //                        //                                      "The specified route can't be called in any
                //                        //                                      way that our router knows; " "you might need
                //                        //                                      to change the signature of your route.");
                //                        using nrtype = decltype(new_route);
                //                        if constexpr (stl::is_invocable_v<nrtype, decltype(args)...>) {
                //                            return new_route(stl::forward<decltype(args)>(args)...);
                //                        } else if (stl::is_invocable_v<nrtype>) {
                //                            return new_route();
                //                        } else {
                //                            stl::invalid_argument(
                //                              "We're unable to run your route. We don't know how."
                //                              " Make sure you're using a route signature that's familiar
                //                              for us.");
                //                        }
                //                    });
            }
        }

        //            template <typename T, typename Ret, typename... Args>
        //            [[nodiscard]] constexpr auto operator>>=(Ret (T::*mem_func_pointer)(Args...)) const
        //            noexcept {
        //                using app_type = T;
        //            }

        //            template <typename RT>
        //            [[nodiscard]] constexpr auto operator=(RT&& new_route) const noexcept {
        //                return operator>>=<RT>(stl::forward<RT>(new_route));
        //            }
        //
        //            template <typename T, typename Ret, typename... Args>
        //            [[nodiscard]] constexpr auto operator=(Ret (T::*mem_func_pointer)(Args...)) const
        //            noexcept {
        //                return operator>>=<T, Ret, Args...>(mem_func_pointer);
        //            }


      private:
        inline auto run_route(Context auto&& ctx, Request auto const& req) const noexcept {
            // exceptions will be handled by the router, unfortunately we're not able to do that here

            using context_type = stl::add_lvalue_reference_t<stl::remove_cvref_t<decltype(ctx)>>;
            auto res           = call_this_route(ctx, req);
            using res_t        = stl::remove_cvref_t<decltype(res)>;
            using n_res_t      = stl::remove_cvref_t<decltype(call_next_route(ctx, req))>;
            constexpr bool convertible_to_bool = stl::is_void_v<n_res_t> || stl::same_as<n_res_t, bool>;
            if constexpr (stl::same_as<res_t, bool>) {
                // handling sub-route calls:
                if constexpr (logical_operators::none == op) {
                    // practically the same as AND but without converting the result to boolean

                    if constexpr (convertible_to_bool) {
                        if (res)
                            return call_next_route_in_bool(ctx, req);
                        return true; // don't call the next sub-route, but call the next entry-route
                    } else if constexpr (Context<n_res_t> || Response<n_res_t>) {
                        if (res)
                            return stl::make_optional(call_next_route(ctx, req));
                        return stl::nullopt;
                    } else {
                        // let the router deal with this non-sense:
                        return call_next_route(ctx, req);
                    }
                } else if constexpr (logical_operators::AND == op) {
                    // don't rely on operator && for not executing the next route, because the user may
                    // have overloaded the operator &&
                    if constexpr (stl::same_as<n_res_t, bool>) {
                        if (!res)
                            return true; // continue checking other entry-routes, but not sub-routes
                        return call_next_route(ctx, req);
                    } else if constexpr (convertible_to_bool) {
                        if (!res)
                            return true; // continue checking other entry-routes, but not sub-routes
                        return call_next_route_in_bool(ctx, req);
                    } else {
                        if (!res)
                            return decltype(stl::make_optional(call_next_route(ctx, req))){stl::nullopt};
                        return stl::make_optional(call_next_route(ctx, req));
                    }
                } else if constexpr (logical_operators::OR == op) {
                    // Same as "and", we will not use operator ||
                    if constexpr (stl::same_as<n_res_t, bool>) {
                        if (res)
                            return true; // continue checking entry-routes but not the sub-routes
                        return call_next_route(ctx, req);
                    } else if constexpr (convertible_to_bool) {
                        if (res)
                            return true; // continue checking entry-routes but not the sub-routes
                        return call_next_route_in_bool(ctx, req);
                    } else {
                        if (res)
                            return stl::nullopt;
                        return stl::make_optional(call_next_route(ctx, req));
                    }
                } else if constexpr (logical_operators::XOR == op) {
                    // In operator xor, the next route will be called no matter the result of the current
                    // route so there's no need for doing the same thing that we did above, but since they
                    // may have changed the meaning of the operator ^, it's not a bad idea to do so, but
                    // I'm too lazy :)
                    if constexpr (stl::same_as<n_res_t, bool>) {
                        return res ^ call_next_route(ctx, req);
                    } else if constexpr (convertible_to_bool) {
                        return res ^ call_next_route_in_bool(ctx, req);
                    } else {
                        throw stl::invalid_argument("Cannot use xor operator with non-bool route.");
                    }
                } else {
                    // should not happen ever.
                    return;
                }
            } else if constexpr (Context<res_t>) {
                // perform sub-route context switching
                if constexpr (is_next_route_valid) {
                    return call_next_route(res, req);
                } else {
                    // entry-route level context-switching will going to happen in the router:
                    return res;
                }
            } else if constexpr (Response<res_t>) {
                // terminate the sub-route callings and return the response, don't even need to run the
                // rest of the routes
                // The strings, and other stuff that can be converted to a response have already been
                // converted to a response in the "call_route" function.
                return res;
            } else {
                // even though the user should not return something useless, and the "call_route" function
                // takes care of these stuff, we still ignore the result and just run the next routes
                return call_next_route(ctx, req);
            }
        }

        [[nodiscard]] inline auto call_this_route(Context auto&&      ctx,
                                                  Request auto const& req) const noexcept {
            if constexpr (is_route_valid) {
                return call_route(static_cast<super_t>(*this), ctx, req);
            } else {
                return; // void
            }
        }

        [[nodiscard]] inline auto call_next_route([[maybe_unused]] Context auto&&      ctx,
                                                  [[maybe_unused]] Request auto const& req) const noexcept {
            using context_type = stl::remove_cvref_t<decltype(ctx)>;
            if constexpr (is_next_route_valid) {
                return call_route(super_t::next, ctx, req);
            } else {
                return; // void
            }
        }

        [[nodiscard]] inline bool
        call_next_route_in_bool([[maybe_unused]] Context auto&&      ctx,
                                [[maybe_unused]] Request auto const& req) const noexcept {
            using res_type = decltype(call_next_route(stl::forward<decltype(ctx)>(ctx), req));
            using res_t    = stl::remove_cvref_t<res_type>;
            if constexpr (stl::same_as<res_t, bool>) {
                return call_next_route(stl::forward<decltype(ctx)>(ctx), req);
            } else {
                // ignore the results
                (void) call_next_route(stl::forward<decltype(ctx)>(ctx), req);
                return true;
            }
        }

      public:
        /**
         * This member function will run any route that returns bool and checks their results to see if
         * the routes in this route-chain can be run (the rest of the non-bool routes).
         *
         * The operator() is guaranteed not to run those routes that return bool so you can first check
         * this function, then if it returns true, you can call the route.
         */
        // template <typename ContextType>
        // requires(Context<stl::remove_cvref_t<ContextType>>)
        //   [[nodiscard]] constexpr bool matches(ContextType&&       ctx,
        //                                        Request auto const& req) const noexcept {
        //     using context_type  = decltype(ctx);
        //     using request_type  = decltype(req);
        //     using ret_type      = ;
        //     using next_ret_type = ;
        //
        //     // bool none/and/or/xor empty         => true,             don't run
        //     // bool none/and void                 => bool,             run bool, but don't run void (?)
        //     // bool or void                       => bool,             run bool, but don't run void (?)
        //     // bool xor void                      => true,             run bool, but don't run void (?)
        //     // bool none/and response             => bool,             run bool, but don't run response
        //     // bool none/and context              => bool,             run bool, but don't run context
        //     // bool none/and route                => bool and route.matches
        //
        //     // bool or empty                      => bool,             run
        //     // bool or void                       => bool,             run bool, but don't run void
        //     // bool or response                   => bool,             run bool, but don't run response
        //     // bool or context                    => bool,             run bool, but don't run context
        //     // bool or route                      => bool and route.matches
        //
        //     constexpr auto is_next_route_match = [this](auto&& ctx,
        //                                             auto const& req) -> bool {
        //         if constexpr (requires(next_route_type _route) {
        //                           {_route.matches(stl::forward<decltype(ctx)>(ctx), req)};
        //                       }) {
        //             // if it's a route itself (nexted route), it'll use that matches member function
        //             return super_t::next.matches(stl::forward<decltype(ctx)>(ctx), req);
        //         } else if constexpr (stl::same_as<next_ret_type, bool>) {
        //             // call the route because it returns a bool, this route won't be run again in the operator()
        //             return call_next_route(stl::forward<decltype(ctx)>(ctx), req);
        //         } else { // Request, ConvertibleToRequest, Entry-Route level Context
        //         Switching, ...
        //             // there's not going to be a Sub-Route level Context Switching here, because if there's one,
        //             // it's a "route" and thus has a ".matches" member function.
        //             return true;
        //         }
        //     };
        //
        //     if constexpr (!is_route_valid) {
        //         if constexpr (!is_next_route_valid) {
        //             return false;
        //         } else {
        //             return is_next_route_match(super_t::next,
        //             stl::forward<decltype(ctx)>(ctx), req);
        //         }
        //     } else if constexpr (requires(route_type _route) {
        //             {_route.matches(stl::forward<decltype(ctx)>(ctx), req)};
        //         }) {
        //         // It's a nested route
        //         auto res = super_t::matches(stl::forward<decltype(ctx)>(ctx), req);
        //         if constexpr (logical_operators::none == op || logical_operators::AND == op)
        //         {
        //             if (!res) // not relying on && to not execute the next route
        //                 return false;
        //             return is_next_route_match(ctx, req);
        //         } else if constexpr (logical_operators::OR == op) {
        //             if (res)
        //                 return true;
        //             return is_next_route_match(ctx, req);
        //         } else if constexpr (logical_operators::XOR == op) {
        //             return res ^ is_next_route_match(ctx, req);
        //         } else {
        //             return false; // this should never happen
        //         }
        //     } else if constexpr (stl::same_as<ret_type, bool>) {
        //         if constexpr (logical_operators::none == op || logical_operators::AND == op)
        //         {
        //             // don't rely on operator && for not executing the next route, because the user may
        //             // have overloaded the operator &&
        //             if (!call_this_route(ctx, req))
        //                 return false;
        //             return is_next_route_match(ctx, req);
        //         } else if constexpr (logical_operators::OR == op) {
        //             // Same as "and", we will not use operator ||
        //             if (call_this_route(ctx, req))
        //                 return true;
        //             return is_next_route_match(ctx, req);
        //         } else if constexpr (logical_operators::XOR == op) {
        //             // In operator xor, the next route will be called no matter the result of the current
        //             // route so there's no need for doing the same thing that we did above, but since they
        //             // may have changed the meaning of the operator ^, it's not a bad idea to do so, but
        //             // I'm too lazy :)
        //             return call_this_route(ctx, req) ^ is_next_route_match(ctx, req);
        //         } else {
        //             return false; // should not be possible
        //         }
        //     } else if constexpr (Context<ret_type>) {
        //         // entry/sub-route level context switching
        //         // switch the context and try the next route if available
        //
        //         // switch_context none/and/or/xor empty    => true,                    don't run
        //         // switch_context none/and/or/xor void     => true,                    don't run
        //         // switch_context none/and/or/xor response => true,                    don't run
        //         // switch_context none/and/xor route       => true op route.matches,   run(?)
        //         // switch_context none/and bool            => true and bool,           run
        //         // switch_context none/and context         => (?)
        //
        //         // switch_context or bool                  => true,                    don't run
        //         // switch_context or route                 => true and route.matches,  run(?)
        //         // switch_context or context               => (?)
        //
        //         // todo
        //         // switched_ctx >>= ((get and "testing" >>= switching_ctx >>= page1) || (post and "testing" >>= page2))
        //         // [switched_ctx none [[get and ["testing" none [switching_ctx none page1]]] or [post and ["testing" none page2]]]]
        //         if constexpr (logical_operators::none == op || logical_operators::AND == op)
        //         {
        //             auto nctx = call_this_route(ctx, req);
        //             using new_context_type = decltype(nctx);
        //             return is_next_route_match(nctx, req);
        //         } else if constexpr (logical_operators::OR == op) {
        //             // true || ... == true
        //             // so we don't need to call the this or next route at all, it's going to be a entry-route level context switching
        //             return true;
        //         } else if constexpr (logical_operators::XOR == op) {
        //         } else {
        //             return false; // should never happen
        //         }
        //     } else { // void, Response, ConvertibleToResponse, ...
        //         return true; // we don't care about the next route, whatever it is, this
        //         route has to run
        //     }
        // }


        [[nodiscard]] auto operator()(Context auto&& ctx, Request auto const& req) const noexcept {
            using context_type = stl::remove_cvref_t<decltype(ctx)>;
            using request_type = stl::remove_cvref_t<decltype(req)>;

            return run_route(ctx, req);
        }
    };


} // namespace webpp

#endif // WEBPP_ROUTES_ROUTE_H
