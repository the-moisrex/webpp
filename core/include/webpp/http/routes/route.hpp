// Created by moisrex on 11/1/19.

#ifndef WEBPP_ROUTES_ROUTE_H
#define WEBPP_ROUTES_ROUTE_H

#include "../../utils/functional.hpp"
#include "./route_concepts.hpp"

#include <type_traits>
#include <utility>

namespace webpp {

    namespace details {
        /**
         * Check if we can convert T to U
         * @tparam T
         * @tparam U
         */
        template <typename T, typename U>
        struct can_convert
          : stl::integral_constant<bool, (!stl::is_void_v<T> && !stl::is_void_v<U>)&&(
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
          : stl::integral_constant<bool, (can_convert_v<Traits, U, typename Traits::string_type> ||
                                          can_convert_v<Traits, U, typename Traits::string_view_type>)> {};

        template <typename Traits, typename U>
        constexpr bool can_convert_to_string_v = can_convert_to_string<Traits, U>::value;


        template <typename HandleExceptionCallable, typename Callable, typename... Args>
        constexpr auto run_and_catch(HandleExceptionCallable const& handle_exception, Callable const& c,
                                     Args... args) noexcept {
            using RetType = stl::invoke_result_t<Callable, Args...>;
            if constexpr (stl::is_nothrow_invocable_r_v<RetType, Callable, Args...>) {
                // It's noexcept, we call it knowing that.
                return callable(stl::forward<Args>(args)...);
            } else if constexpr (stl::is_invocable_r_v<RetType, Callable, Args...>) {
                try {
                    return callable(stl::forward<Args>(args)...);
                } catch (...) {
                    handle_exception(stl::current_exception());
                    return false; // todo: check this
                }
            } else {
                throw stl::invalid_argument("The specified route is not valid. We're not able to call it.");
            }
        }

        /**
         * Handle the return type of a route::operator()
         */
        template <typename RetType>
        constexpr auto handle_callable_return_type(RetType&& ret) noexcept {
            // todo
            if constexpr (stl::is_void_v<RetType>) {
                // it's an "Unknown route"
                return;
            } else if constexpr (Response<RetType>) {
                // It was a "Response route"
            } else if constexpr (Context<RetType>) {
                // It's a "Context Switching route"

            } else if constexpr (stl::is_same_v<RetType, bool>) {
                // It's a "Conditional route"
                return ret;
            }
        }

        template <typename C, Context ContextType>
        bool call_route(C& c, ContextType& context) noexcept {
            using req_t     = typename ContextType::request_type const&;
            using res_t     = typename ContextType::response_type&;
            using callable  = stl::decay_t<C>;
            using context_t = ContextType&;
            auto callback   = stl::forward<C>(c);

            constexpr auto handle_exception = [](auto err) {
                // todo: use the log system here
            };

            // TODO: add more overrides. You can simulate "dependency injection" here

            if constexpr (stl::is_invocable_v<callable, req_t>) {
                return handle_callback_return_type(
                  run_and_catch(handle_exception, callback, context.request));
                //            } else if constexpr (stl::is_invocable_v<callable, res_t>) {
                //                return handle_callback_return_type(
                //                  run_and_catch(handle_exception, callback, context.response));
                //            } else if constexpr (stl::is_invocable_v<callable, req_t, res_t>) {
                //                return handle_callback_return_type(
                //                  run_and_catch(handle_exception, callback, context.request,
                //                  context.response));
                //            } else if constexpr (stl::is_invocable_v<callable, res_t, req_t>) {
                //                return handle_callback_return_type(
                //                  run_and_catch(handle_exception, callback, context.response,
                //                  context.request));
            } else if constexpr (stl::is_invocable_v<callable, context_t>) {
                return handle_callback_return_type(run_and_catch(handle_exception, callback, context));
            } else if constexpr (stl::is_invocable_v<callable, context_t, req_t>) {
                return handle_callback_return_type(
                  run_and_catch(handle_exception, callback, context, context.request));
            } else if constexpr (stl::is_invocable_v<callable, req_t, context_t>) {
                return handle_callback_return_type(
                  run_and_catch(handle_exception, callback, context.request, context));
            } else if constexpr (stl::is_invocable_v<callable>) {
                return handle_callback_return_type(run_and_catch(handle_exception, callback));
            } else {
                // todo: move this throw in the log system
                throw stl::invalid_argument("The specified route cannot be called.");
            }
        }


        enum class logical_operators { none, AND, OR, XOR };

        template <typename RouteType, logical_operators Op, typename NextRoute>
        struct basic_route : public make_inheritable<RouteType> {
            using next_valve_type = stl::remove_cvref_t<NextRoute>;

            constexpr static logical_operators op = Op;
            next_valve_type                    next;

            constexpr basic_route(RouteType&& super = RouteType{}, next_valve_type&& _next = next_valve_type{}) noexcept
              : RouteType(stl::move(super)),
                next(stl::move(_next)) {
            }

            constexpr basic_route(RouteType const& super = RouteType{}, const next_valve_type& _next = next_valve_type{}) noexcept
              : RouteType(super),
                next(_next) {
            }

            constexpr basic_route(RouteType const& super = RouteType{}, next_valve_type&& _next = next_valve_type{}) noexcept
              : RouteType(super),
                next(stl::move(_next)) {
            }

            constexpr basic_route(RouteType&& super = RouteType{}, next_valve_type const& _next = next_valve_type{}) noexcept
              : RouteType(stl::move(super)),
                next(_next) {
            }

            constexpr basic_route(basic_route const& v) noexcept = default;
            constexpr basic_route(basic_route&& v) noexcept      = default;

            constexpr basic_route& operator=(basic_route const& v) noexcept = default;
            constexpr basic_route& operator=(basic_route&&) noexcept = default;
        };

        template <Route RouteType>
        struct basic_route<RouteType, logical_operators::none, void> : public make_inheritable<RouteType> {
            using super_t = make_inheritable<RouteType>;

            template <typename ...Args>
            constexpr basic_route(Args&& ...args) noexcept : super_t{stl::forward<Args>(args)...} {}
        };

        template <>
        struct basic_route<void, logical_operators::none, void> {
            void operator()(Context auto const&) const noexcept{}
        };

        template <typename RouteType = void, logical_operators Op = logical_operators::none,
                  typename NextRouteType = void>
        struct route : public basic_route<RouteType, Op, NextRouteType> {

            using route_type                      = RouteType;
            using next_route_type                 = NextRouteType;
            static constexpr logical_operators op = Op;

          private:
            using super_t = basic_route<RouteType, Op, NextRouteType>;

          public:
            using super_t::operator();

            constexpr route() noexcept : super_t{} {
            }
            constexpr route(route const&) noexcept = default;
            constexpr route(route&&) noexcept      = default;

            template <typename... Args>
            constexpr route(Args&&... args) noexcept
              : super_t{stl::forward<Args>(args)...} {
            }

            using super_t::operator=;

            /**
             * Run the migration
             * @return the response or the context or ...
             */
            template <typename... Args>
            auto operator()(Args&&... args) noexcept {
                return super_t::operator()(stl::forward<Args>(args)...);
            }

          private:
            template <logical_operators TheOp, typename NewRouteType>
            [[nodiscard]] constexpr auto set_next(NewRouteType&& new_route) const noexcept {
                if constexpr (stl::is_void_v<next_route_type>) {
                    // this part will only execute when the "next_valve_type" is void

                    // the first way (A<X, void> and B<Y, void> === A<X, B<Y, void>>
                    return route<route_type, TheOp, NewRouteType>(*this,
                                                                  stl::forward<NewRouteType>(new_route));
                } else {
                    // this means this function has a "next" valve already,
                    // so it goes to the next's next valve
                    // this way we recursively create a valve type and return it.
                    auto n = basic_route<route_type, TheOp, NewRouteType>::next.set_next(
                      stl::forward<NewRouteType>(new_route));
                    return route<route_type, op, decltype(n)>{*this, n};
                }
            }

          public:
            [[nodiscard]] constexpr auto operator&&(Route auto&& new_route) const noexcept {
                return set_next<logical_operators::AND>(stl::forward<decltype(new_route)>(new_route));
            }

            [[nodiscard]] constexpr auto operator&(Route auto&& new_route) const noexcept {
                return set_next<logical_operators::AND>(stl::forward<decltype(new_route)>(new_route));
            }

            [[nodiscard]] constexpr auto operator||(Route auto&& new_route) const noexcept {
                return set_next<logical_operators::OR>(stl::forward<decltype(new_route)>(new_route));
            }

            [[nodiscard]] constexpr auto operator|(Route auto&& new_route) const noexcept {
                return set_next<logical_operators::OR>(stl::forward<decltype(new_route)>(new_route));
            }

            [[nodiscard]] constexpr auto operator^(Route auto&& new_route) const noexcept {
                return set_next<logical_operators::XOR>(stl::forward<decltype(new_route)>(new_route));
            }

            [[nodiscard]] constexpr auto operator>>=(Route auto&& new_route) const noexcept {
                return set_next<logical_operators::none>(stl::forward<decltype(new_route)>(new_route));
            }

            [[nodiscard]] constexpr auto operator=(Route auto&& new_route) const noexcept {
                return set_next<logical_operators::none>(stl::forward<decltype(new_route)>(new_route));
            }

            [[nodiscard]] bool call_this_route(Context auto&& ctx) const noexcept {
                // todo handle the return types
                return super_t::operator()(stl::forward<decltype(ctx)>(ctx));
            }

            [[nodiscard]] bool call_next_route(Context auto&& ctx) const noexcept {
                // todo handle the return types
                if constexpr (stl::is_void_v<next_route_type>) {
                    return true; // it's the last route in this sub route, doesn't
                                 // matter what I return here; at least not yet
                } else {
                    return super_t::next::operator()(stl::forward<decltype(ctx)>(ctx));
                }
            }

            [[nodiscard]] bool operator()(Context auto&& ctx) const noexcept {
                using context_type = decltype(ctx);
                static_assert(stl::is_invocable_v<route_type, context_type>,
                        "The specified route is not capable of handling this context. "
                        "Your route might be in the wrong place or previous routes in the router "
                        "should've switched the context to something that this route is able to use.");

                // handling sub-route calls:
                if constexpr (logical_operators::none == op) {
                    call_this_route(stl::forward<context_type>(ctx));
                    return call_next_route(stl::forward<context_type>(ctx));
                } else if constexpr (logical_operators::AND == op) {
                    return call_this_route(stl::forward<context_type>(ctx)) &&
                           call_next_route(stl::forward<context_type>(ctx));
                } else if constexpr (logical_operators::OR == op) {
                    return call_this_route(stl::forward<context_type>(ctx)) ||
                           call_next_route(stl::forward<context_type>(ctx));
                } else if constexpr (logical_operators::XOR == op) {
                    return call_this_route(stl::forward<context_type>(ctx)) ^
                           call_next_route(stl::forward<context_type>(ctx));
                } else {
                    // should not happen ever.
                    return true;
                }
            }
        };
    } // namespace details

    template <typename... T>
    using route = details::route<T...>;
} // namespace webpp

#endif // WEBPP_ROUTES_ROUTE_H
