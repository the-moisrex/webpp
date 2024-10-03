// Created by moisrex on 11/1/19.

#ifndef WEBPP_HTTP_ROUTES_ROUTE_HPP
#define WEBPP_HTTP_ROUTES_ROUTE_HPP

#include "../../webpp/http/routes/context.hpp"
#include "../../webpp/http/routes/router_concepts.hpp"
#include "../../webpp/logs/log_concepts.hpp"
#include "../../webpp/std/function_ref.hpp"
#include "../../webpp/std/optional.hpp"
#include "../../webpp/std/type_traits.hpp"

#include <utility>

/**
 * Call a member function while you're inside that class
 */
#define mem_call(member_name)                                                                \
    ([this]<typename... Args>                                                                \
         requires requires(stl::remove_cvref_t<decltype(*this)> that, Args... args) {        \
             that.member_name(::webpp::stl::forward<Args>(args)...);                         \
         }(Args && ... args)                                                                 \
     constexpr noexcept(noexcept(this->member_name(::webpp::stl::forward<Args>(args)...))) { \
         return this->member_name(::webpp::stl::forward<Args>(args)...);                     \
     })

namespace webpp::http {

    /**
     * This struct will help the user to call member functions easily
    template <typename T>
    struct proxy {
        T& obj;

        T* operator->() const noexcept {
            return &obj;
        }

        template <typename Ret, typename... Args>
        auto operator->*(Ret (T::*member_function)(Args&&...)) const noexcept {
            return [&](Args && ... the_args) constexpr {
                return obj->*member_function(stl::forward<Args>(the_args)...);
            };
        }

        template <typename R, typename... Args>
        inline auto operator()(R T::*member_func, Args&&... args) {
            return obj->*member_func(stl::forward<Args>(args)...);
        }
    };
     */

    template <typename Route, typename... Args>
    concept is_callable_route = requires(stl::remove_cvref_t<Route> route) {
        requires requires(Args... args) { route(args...); } ||
                   requires(stl::add_lvalue_reference_t<Args>... args) { route(args...); } ||
                   requires(stl::remove_cvref_t<Args>... args) { route(args...); };
    };

    template <typename Route, typename... Args>
    concept is_nothrow_callable_route =
      stl::is_nothrow_invocable_v<stl::decay_t<Route>, stl::remove_cvref_t<Args>...> ||
      stl::is_nothrow_invocable_v<stl::decay_t<Route>, Args...> ||
      stl::is_nothrow_invocable_v<stl::decay_t<Route>,
                                  stl::add_lvalue_reference_t<stl::remove_cvref_t<Args>>...>;

    namespace details {

        static constexpr auto route_log_cat = "RouteCalling";

        template <typename CallableT, Context CtxT, typename... Args>
        constexpr decltype(auto) run_and_catch(CallableT&& callable, CtxT& ctx, Args&&... args) noexcept {
            using return_type = stl::invoke_result_t<CallableT, Args...>;
            if constexpr (stl::is_nothrow_invocable_v<CallableT, Args...>) {
                // It's noexcept, we call it knowing that.
                return callable(stl::forward<Args>(args)...);
            } else if constexpr (stl::is_invocable_v<CallableT, Args...>) {
                if constexpr (stl::is_void_v<return_type>) {
                    try {
                        callable(stl::forward<Args>(args)...);
                    } catch (stl::exception const& ex) {
                        ctx.logger.error(route_log_cat,
                                         "Error happened calling a route that returns void.",
                                         ex);
                    } catch (...) {
                        ctx.logger.error(route_log_cat,
                                         "Unknown error happened calling a route that returns void.");
                    }
                } else if constexpr (stl::same_as<return_type, bool>) {
                    try {
                        return callable(stl::forward<Args>(args)...);
                    } catch (stl::exception const& ex) {
                        ctx.logger.error(route_log_cat,
                                         "Error happened calling a route that returns bool.",
                                         ex);
                        return false;
                    } catch (...) {
                        ctx.logger.error(route_log_cat,
                                         "Unknown error happened calling a route that returns bool.");
                        return false;
                    }
                } else if constexpr (istl::Optional<return_type>) {
                    try {
                        return callable(stl::forward<Args>(args)...);
                    } catch (stl::exception const& ex) {
                        ctx.logger.error(route_log_cat,
                                         "Error happened calling a route that returns optional value.",
                                         ex);
                        // return 500 error on failure hoping the response type supports it
                        return typename return_type::value_type{http::status_code::internal_server_error};
                    } catch (...) {
                        ctx.logger.error(
                          route_log_cat,
                          "Unknown error happened calling a route that returns optional value.");
                        // return 500 error on failure hoping the response type supports it
                        return typename return_type::value_type{http::status_code::internal_server_error};
                    }
                } else {
                    using optional_type = decltype(stl::make_optional(callable(stl::forward<Args>(args)...)));
                    try {
                        return stl::make_optional(callable(stl::forward<Args>(args)...));
                    } catch (stl::exception const& ex) {
                        ctx.logger.error(route_log_cat, "Error happened calling a route.", ex);
                        return optional_type{stl::nullopt};
                    } catch (...) {
                        ctx.logger.error(route_log_cat, "Unknown error happened calling a route.");
                        return optional_type{stl::nullopt};
                    }
                }

            } else {
                return ctx.error(
                  http::status_code::internal_server_error,
                  stl::invalid_argument("The specified route is not valid. We're not able to call it."));
            }
        }

    } // namespace details

    template <typename RouteT, Context CtxT, HTTPRequest ReqT>
    constexpr decltype(auto) call_route(RouteT&& _route, CtxT&& ctx, ReqT&& req) noexcept {
        using namespace details;

        using route_type   = RouteT;
        using request_type = ReqT;
        using context_type = CtxT;

        if constexpr (is_callable_route<route_type, context_type, request_type>) {
            // requires a context and a request
            return run_and_catch(_route, ctx, ctx, req);
        } else if constexpr (is_callable_route<route_type, request_type, context_type>) {
            // requires a request and a context
            return run_and_catch(_route, ctx, req, ctx);
        } else if constexpr (is_callable_route<route_type, context_type>) {
            // gets a context
            return run_and_catch(_route, ctx, ctx);
        } else if constexpr (is_callable_route<route_type, request_type>) {
            // requires a request
            return run_and_catch(_route, ctx, req);
        } else if constexpr (is_callable_route<route_type>) {
            // requires nothing
            return run_and_catch(_route, ctx);
        } else {
            static_assert_false(route_type,
                                "We don't know how to call your entry route. Change your route's signature.");
        }
    }



    enum class logical_operators {
        none,
        AND,
        OR,
        XOR
    };

    template <typename RouteType, logical_operators Op, typename NextRoute>
    struct basic_route : public istl::make_inheritable<RouteType> {
        using next_route_type = NextRoute;
        using super_t         = istl::make_inheritable<RouteType>;

        static constexpr logical_operators op = Op;
        next_route_type                    next; // NOLINT(misc-non-private-member-variables-in-classes)

        // constexpr basic_route(super_t&&       super = super_t{},
        //                       next_route_type&& _next = next_route_type{}) noexcept
        //   : super_t(stl::forward<RouteType>(super)),
        //     next(stl::forward<next_route_type>(_next)) {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename ParentRouteType, typename NextRouteType>
            requires(!stl::same_as<stl::remove_cvref_t<ParentRouteType>, basic_route>)
        constexpr basic_route(ParentRouteType&& super, NextRouteType&& _next = next_route_type{}) noexcept
          : super_t(stl::forward<ParentRouteType>(super)),
            next(stl::forward<NextRouteType>(_next)) {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr basic_route(basic_route const&) noexcept       = default;
        constexpr basic_route(basic_route&&) noexcept            = default;
        constexpr ~basic_route() noexcept                        = default;
        constexpr basic_route& operator=(basic_route const&)     = default;
        constexpr basic_route& operator=(basic_route&&) noexcept = default;

        using super_t::operator=;
        using super_t::operator();
    };

    template <typename RouteType>
    struct basic_route<RouteType, logical_operators::none, void> : public istl::make_inheritable<RouteType> {
        using super_t = istl::make_inheritable<RouteType>;

        template <typename... Args>
        constexpr basic_route(Args&&... args) noexcept : super_t{stl::forward<Args>(args)...} {}

        constexpr basic_route(basic_route const&) noexcept       = default;
        constexpr basic_route(basic_route&&) noexcept            = default;
        constexpr ~basic_route() noexcept                        = default;
        constexpr basic_route& operator=(basic_route const&)     = default;
        constexpr basic_route& operator=(basic_route&&) noexcept = default;

        using super_t::operator=;
        using super_t::operator();
    };

    template <>
    struct basic_route<void, logical_operators::none, void> {
        template <Context CtxT>
        void operator()(CtxT const&) const noexcept {
            // empty
        }
    };

    /**
     * todo: should we convert router to a tuple like I did with the path class?
     */
    template <typename RouteType     = void,
              logical_operators Op   = logical_operators::none,
              typename NextRouteType = void>
    struct route
      : public basic_route<stl::remove_cvref_t<RouteType>, Op, stl::remove_cvref_t<NextRouteType>> {
        using route_type                      = stl::remove_cvref_t<RouteType>;
        using next_route_type                 = stl::remove_cvref_t<NextRouteType>;
        static constexpr logical_operators op = Op;

      private:
        using super_t = basic_route<route_type, Op, next_route_type>;

        // // todo: use the istl::lazy_conditional
        // template <bool Condition, template <typename...> typename T1, typename T2, typename... C>
        // struct lazy_conditional {};
        //
        // template <template <typename...> typename T1, typename T2, typename... C>
        // struct lazy_conditional<true, T1, T2, C...> {
        //     using type = T1<C...>;
        // };
        //
        // template <template <typename...> typename T1, typename T2, typename... C>
        // struct lazy_conditional<false, T1, T2, C...> {
        //     using type = T2;
        // };
        //
        // template <bool Condition, template <typename...> typename T1, typename T2, typename... C>
        // using lazy_conditional_t = typename lazy_conditional<Condition, T1, T2, C...>::type;
        //
        //
        // template <typename R, typename C, bool IF>
        // struct is_switching_context {
        //     static constexpr bool value = false;
        // };
        //
        // template <typename R, typename C>
        // struct is_switching_context<R, C, true> {
        //     static constexpr bool value = Context<stl::invoke_result_t<R, C>>;
        // };
        //
        // template <typename R, typename C>
        // static constexpr bool is_switching_context_v =
        //   is_switching_context<R, C, !stl::is_void_v<R> && stl::is_invocable_v<R, C>>::value;
        //
        //
        // /**
        //  * Get the "switched context typed" if it's R is a route, otherwise, return the context type if
        //  * the operator(C) of R is doing context switching, otherwise, return the specified default C
        //  * @tparam R
        //  * @tparam C
        //  */
        // template <typename R, typename C>
        // struct lazy_switched_context_type {
        //     using type = lazy_conditional_t<is_switching_context_v<R, C>, stl::invoke_result_t, C, R, C>;
        // };
        //
        // template <typename R, typename C>
        // requires(Route<R, C>) struct lazy_switched_context_type<R, C> {
        //     using type = typename R::template switched_context_type<C>;
        // };

      public:
        static constexpr bool is_route_valid      = !stl::is_void_v<route_type>;
        static constexpr bool is_next_route_valid = !stl::is_void_v<next_route_type>;

        // template <typename R, typename C>
        // using route_switched_context_type =
        //   stl::conditional_t<stl::is_invocable_v<R, C>, typename lazy_switched_context_type<R, C>::type,
        //   C>;
        //
        // template <typename C>
        // using route_context_type = route_switched_context_type<route_type, C>;
        //
        // template <typename C>
        // using next_route_context_type = route_switched_context_type<next_route_type, C>;
        //
        // template <typename C>
        // using switched_context_type = next_route_context_type<route_context_type<C>>;
        //
        // template <typename C>
        // static constexpr bool is_switching_context_recursive = stl::is_same_v<switched_context_type<C>, C>;


        constexpr route() noexcept                   = default;
        constexpr ~route() noexcept                  = default;
        constexpr route(route const&)                = default;
        constexpr route(route&&) noexcept            = default;
        constexpr route& operator=(route&&) noexcept = default;
        constexpr route& operator=(route const&)     = default;

        template <typename... Args>
            requires(stl::is_constructible_v<super_t, Args...>)
        constexpr route(Args&&... args) noexcept : super_t{stl::forward<Args>(args)...} {}


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
                using encapsulated_new_route = route<next_route_type, TheOp, NewRouteType>;
                return route<route_type, op, encapsulated_new_route>{
                  *this,
                  encapsulated_new_route{super_t::next, stl::forward<NewRouteType>(new_route)}
                };
            }
        }

        template <typename AppType, typename NewRoutePtr, bool is_noexcept>
        struct route_with_router_pointer {
            NewRoutePtr new_route;
            AppType*    app = nullptr;

            [[nodiscard]] auto operator()(auto... args) noexcept(is_noexcept) {
                // yes we know app must not be nullptr, but route should only be used with router,
                // and the router will set the app if it can otherwise the router can throw an
                // error at compile time or at least at initialization time instead of when
                // a user's request comes to this specific route. This makes sure the developers
                // will catch this error sooner.
                assert(app != nullptr); // You did not supply the correct app to the router

                return (app->*new_route)(stl::forward<decltype(args)>(args)...);
            }
        };

        template <Context CtxT, HTTPRequest ReqT>
        constexpr auto call_this_route(CtxT&& ctx, ReqT&& req) const noexcept {
            if constexpr (is_route_valid) {
                return call_route(static_cast<super_t>(*this),
                                  stl::forward<CtxT>(ctx),
                                  stl::forward<ReqT>(req));
            } else {
                static_assert_false(CtxT, "This route cannot be called.");
            }
        }

        constexpr auto call_next_route([[maybe_unused]] Context auto&&     ctx,
                                       [[maybe_unused]] HTTPRequest auto&& req) const noexcept {
            if constexpr (is_next_route_valid) {
                return call_route(super_t::next, ctx, req);
            } else {
                return; // void
            }
        }


      public:
        [[nodiscard]] constexpr auto operator>>=(void (*func)()) const noexcept {
            return set_next<logical_operators::none>(func);
        }

        template <typename ReturnType>
        [[nodiscard]] constexpr auto operator>>=(ReturnType (*func)()) const noexcept {
            return set_next<logical_operators::none>(func);
        }

        template <typename RouteT>
        [[nodiscard]] constexpr auto operator>>=(RouteT&& new_route) const noexcept {
            using rt = stl::remove_cvref_t<RouteT>;
            if constexpr (stl::is_member_function_pointer_v<rt>) {
                using mem_func_ptr_t = istl::member_function_pointer_traits<rt>;
                using app_type       = typename mem_func_ptr_t::type;
                return set_next<logical_operators::none>(
                  route_with_router_pointer<app_type,
                                            stl::remove_cvref<RouteT>,
                                            mem_func_ptr_t::is_noexcept>{});
            } else /*if constexpr (PotentialRoute<rt, switched_context_type<fake_context_type>>)*/ {
                return set_next<logical_operators::none>(stl::forward<RouteT>(new_route));
                // } else {
                //     // todo: write tests for this:
                //     return set_next<logical_operators::none>([=](auto... args) {
                //         //                        static_assert(stl::is_invocable_v<RouteT,
                //         //                        decltype(args)...>,
                //         //                                "The specified route can't be called in any
                //         //                                way that our router knows; " "you might need
                //         //                                to change the signature of your route.");
                //         using nrtype = RouteT;
                //         if constexpr (stl::is_invocable_v<nrtype, decltype(args)...>) {
                //             return new_route(stl::forward<decltype(args)>(args)...);
                //         } else if (stl::is_invocable_v<nrtype>) {
                //             return new_route();
                //         } else {
                //             stl::invalid_argument(
                //               "We're unable to run your route. We don't know how."
                //               " Make sure you're using a route signature that's familiar
                //               for us.");
                //         }
                //     });
            }
        }

        [[nodiscard]] constexpr auto operator=(void (*func)()) const noexcept { // NOLINT
            return operator>>=(func);
        }

        template <typename ReturnType>
        [[nodiscard]] constexpr auto operator=(ReturnType (*func)()) const noexcept { // NOLINT
            return operator>>= <ReturnType>(func);
        }

        [[nodiscard]] constexpr auto operator=(auto&& new_route) const noexcept { // NOLINT
            return operator>>=(stl::forward<decltype(new_route)>(new_route));
        }

        // template <typename T, typename Ret, typename... Args>
        // [[nodiscard]] constexpr auto operator>>=(Ret (T::*mem_func_pointer)(Args...)) const
        // noexcept {
        //     using app_type = T;
        // }

        // template <typename T, typename Ret, typename... Args>
        // [[nodiscard]] constexpr auto operator=(Ret (T::*mem_func_pointer)(Args...)) const
        // noexcept {
        //     return operator>>=<T, Ret, Args...>(mem_func_pointer);
        // }


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

        constexpr auto operator()(Context auto&& ctx, HTTPRequest auto&& req) const noexcept {
            // exceptions will be handled by the router, unfortunately we're not able to do that here

            using res_t = stl::remove_cvref_t<decltype(call_this_route(ctx, req))>;
            if constexpr (stl::is_void_v<res_t>) {
                call_this_route(ctx, req); // nothing to worry about, it's void
            } else {
                auto const res = call_this_route(ctx, req);
                if constexpr (!is_next_route_valid) {
                    // we don't have "next route", so let's just return the result of this route to the caller
                    return res;
                } else {
                    // we do have a next route, so let's call it

                    using n_res_t = stl::remove_cvref_t<decltype(call_route(super_t::next, ctx, req))>;

                    if constexpr (stl::same_as<res_t, bool>) {
                        // handling sub-route calls:
                        if constexpr (logical_operators::none == op) {
                            // practically the same as AND, but without converting the result to boolean
                            if constexpr (stl::is_void_v<n_res_t>) {
                                // just because we can't have an optional<void>
                                if (res) {
                                    call_next_route(ctx, req);
                                }
                            } else if constexpr (stl::same_as<n_res_t, bool>) {
                                if (res) {
                                    return call_next_route(ctx, req);
                                }
                                return false;
                            } else if constexpr (istl::Optional<n_res_t>) {
                                if (!res) {
                                    return n_res_t{stl::nullopt};
                                }
                                return call_next_route(ctx, req);
                            } else {
                                if (!res) {
                                    return stl::optional<n_res_t>{stl::nullopt};
                                }
                                return stl::optional<n_res_t>{call_next_route(ctx, req)};
                            }
                        } else if constexpr (logical_operators::AND == op) {
                            // don't rely on operator && for not executing the next route, because the user
                            // may have overloaded the operator &&
                            if constexpr (stl::is_void_v<n_res_t>) {
                                if (res) {
                                    call_next_route(ctx, req);
                                }
                            } else if constexpr (stl::same_as<n_res_t, bool>) {
                                if (res) {
                                    return call_next_route(ctx, req);
                                }
                                return false;
                            } else if constexpr (istl::Optional<n_res_t>) {
                                if (!res) {
                                    return n_res_t{stl::nullopt};
                                }
                                return call_next_route(ctx, req);
                            } else {
                                if (!res) {
                                    return stl::optional<n_res_t>{stl::nullopt};
                                }
                                return stl::optional<n_res_t>{call_next_route(ctx, req)};
                            }

                        } else if constexpr (logical_operators::OR == op) {
                            // Same as "and", we will not use operator ||
                            if constexpr (stl::is_void_v<n_res_t>) {
                                if (res) {
                                    call_next_route(ctx, req);
                                }
                            } else if constexpr (stl::same_as<n_res_t, bool>) {
                                if (res) {
                                    return true;
                                }
                                return call_next_route(ctx, req);
                            } else if constexpr (istl::Optional<n_res_t>) {
                                if (res) {
                                    return n_res_t{stl::nullopt};
                                }
                                return call_next_route(ctx, req);
                            } else {
                                if (res) {
                                    return stl::optional<n_res_t>{stl::nullopt};
                                }
                                return optional<n_res_t>(call_next_route(ctx, req));
                            }
                        } else if constexpr (logical_operators::XOR == op) {
                            // In operator xor, the next route will be called no matter the result of the
                            // current route so there's no need for doing the same thing that we did above,
                            // but since they may have changed the meaning of the operator ^, it's not a bad
                            // idea to do so, but I'm too lazy :)
                            if constexpr (stl::same_as<n_res_t, bool>) {
                                return res ^ call_next_route(ctx, req);
                            } else {
                                static_assert_false(n_res_t, "Cannot use xor operator with non-bool route.");
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
                    } else if constexpr (HTTPResponse<res_t>) {
                        // terminate the sub-route callings and return the response, don't even need to run
                        // the rest of the routes The strings, and other stuff that can be converted to a
                        // response have already been converted to a response in the "call_route" function.
                        return res;
                    } else {
                        // even though the user should not return something useless, and the "call_route"
                        // function takes care of these stuff, we still ignore the result and just run the
                        // next routes
                        ctx.logger.warning("Router", "We're ignoring one of your route's results.");
                        return call_next_route(ctx, req);
                    }
                }
            }
        }

      private:
        template <typename ResT, bool First = false, typename RoutT>
        static void append_route_as_string(istl::String auto& out, RoutT& the_route) {
            // print this route
            if constexpr (stl::is_void_v<ResT>) {
                if constexpr (First) {
                    append_to(out, "action");
                } else {
                    append_to(out, " >> action");
                }
            } else if constexpr (stl::same_as<ResT, bool>) {
                // todo: find out the name or type of the validator
                if constexpr (!First) {
                    switch (op) {
                        case logical_operators::none: append_to(out, " >> "); break;
                        case logical_operators::AND: append_to(out, " && "); break;
                        case logical_operators::OR: append_to(out, " || "); break;
                        case logical_operators::XOR: append_to(out, " ^ "); break;
                    }
                }
                if constexpr (requires { the_route.to_string(out); }) {
                    the_route.to_string(out);
                } else {
                    append_to(out, "condition");
                }
            } else if constexpr (ConvertibleToResponse<ResT>) {
                // todo: find out the type of the response
                if constexpr (First) {
                    append_to(out, "response");
                } else {
                    append_to(out, " >> response");
                }
            } else if constexpr (requires { the_route.to_string(out); }) {
                the_route.to_string(out); // method_route_condition has this method
            } else {
                if constexpr (First) {
                    append_to(out, "[unknown]");
                } else {
                    append_to(out, " >> [unknown]");
                }
            }
        }

      public:
        /**
         * Generate a string representation of this route
         */
        template <Context CtxT, HTTPRequest ReqT, bool First = true>
        void to_string(istl::String auto& out, CtxT&& ctx, ReqT&& req) const {
            using namespace stl;
            using res_t   = remove_cvref_t<decltype(call_this_route(ctx, req))>;
            using n_res_t = remove_cvref_t<decltype(call_next_route(ctx, req))>;

            append_route_as_string<res_t, First>(out, *this);

            // print the next route
            if constexpr (is_next_route_valid) {
                if constexpr (requires { this->next.template to_string<CtxT, ReqT, false>(out, ctx, req); }) {
                    this->next.template to_string<CtxT, ReqT, false>(out, ctx, req);
                } else {
                    append_route_as_string<n_res_t, false>(out, this->next);
                }
            }
        }

        template <typename StrT = stl::string>
        [[nodiscard]] StrT to_string() const {
            StrT out;
            to_string(out);
            return out;
        }
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTES_ROUTE_HPP
