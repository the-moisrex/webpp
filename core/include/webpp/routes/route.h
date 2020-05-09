// Created by moisrex on 11/1/19.

#ifndef WEBPP_ROUTES_ROUTE_H
#define WEBPP_ROUTES_ROUTE_H

#include "../http/request.h"
#include "../http/response.h"
#include "../utils/functional.h"
#include "context.h"

#include <type_traits>
#include <utility>

namespace webpp::routes {





    /**
     * Check if we can convert T to U
     * @tparam T
     * @tparam U
     */
    template <typename T, typename U>
    struct can_convert
      : ::std::integral_constant<
          bool, (!::std::is_void_v<T> &&
                 !::std::is_void_v<U>)&&(::std::is_convertible_v<T, U> ||
                                         ::std::is_constructible_v<T, U> ||
                                         ::std::is_assignable_v<T, U>)> {};

    template <typename T, typename U>
    constexpr bool can_convert_v = can_convert<T, U>::value;

    /**
     * Check if we can convert T to a string
     * @tparam Traits
     * @tparam U
     */
    template <typename Traits, typename U>
    struct can_convert_to_string
      : ::std::integral_constant<
          bool, (can_convert_v<Traits, U, typename Traits::string_type> ||
                 can_convert_v<Traits, U, typename Traits::string_view_type>)> {
    };

    template <typename Traits, typename U>
    constexpr bool can_convert_to_string_v =
      can_convert_to_string<Traits, U>::value;





    template <typename HandleExceptionCallable, typename Callable,
              typename... Args>
    constexpr auto
    run_and_catch(HandleExceptionCallable const& handle_exception,
                  Callable const&                c, Args... args) noexcept {
        using RetType = ::std::invoke_result_t<Callable, Args...>;
        if constexpr (::std::is_nothrow_invocable_r_v<RetType, Callable,
                                                      Args...>) {
            // It's noexcept, we call it knowing that.
            return callable(::std::forward<Args>(args)...);
        } else if constexpr (::std::is_invocable_r_v<RetType, Callable,
                                                     Args...>) {
            try {
                return callable(::std::forward<Args>(args)...);
            } catch (...) {
                handle_exception(::std::current_exception());
                return false; // todo: check this
            }
        } else {
            throw ::std::invalid_argument(
              "The specified route is not valid. We're not able to call it.");
        }
    }

    /**
     * Handle the return type of a route::operator()
     * @tparam RetType
     * @param ret
     * @return
     */
    template <typename RetType>
    constexpr auto handle_callable_return_type(RetType&& ret) noexcept {
        if constexpr (::std::is_void_v<RetType>) {
            // it's an "Unknown route"
            return;
        } else if constexpr (Response<RetType>) {
            // It was a "Response route"
        } else if constexpr (Context<RetType>) {
            // It's a "Context Switching route"

        } else if constexpr (::std::is_same_v<RetType, bool>) {
            // It's a "Conditional route"
            return ret;
        }
    }

    template <typename C, Context ContextType>
    inline bool call_route(C& c, ContextType& context) noexcept {
        using req_t     = typename ContextType::request_type const&;
        using res_t     = typename ContextType::response_type&;
        using callable  = ::std::decay_t<C>;
        using context_t = ContextType&;
        auto callback   = ::std::forward<C>(c);

        constexpr auto handle_exception = [](auto err) {

        };


        // TODO: add more overrides. You can simulate "dependency injection" here

        if constexpr (::std::is_invocable_v<callable, req_t>) {
            return handle_callback_return_type(
              run_and_catch(handle_exception, callback, context.request));
        } else if constexpr (::std::is_invocable_v<callable, res_t>) {
            return handle_callback_return_type(
              run_and_catch(handle_exception, callback, context.response));
        } else if constexpr (::std::is_invocable_v<callable, req_t, res_t>) {
            return handle_callback_return_type(run_and_catch(
              handle_exception, callback, context.request, context.response));
        } else if constexpr (::std::is_invocable_v<callable, res_t, req_t>) {
            return handle_callback_return_type(run_and_catch(
              handle_exception, callback, context.response, context.request));
        } else if constexpr (::std::is_invocable_v<callable, context_t>) {
            return handle_callback_return_type(
              run_and_catch(handle_exception, callback, context));
        } else if constexpr (::std::is_invocable_v<callable>) {
            return handle_callback_return_type(
              run_and_catch(handle_exception, callback));
        } else {
            throw ::std::invalid_argument(
              "The specified route cannot be called.");
        }
    }

    template <typename T>
    concept RouteResponse = Response<T> || ConvertibleToResponse<T> ||
                            Context<T> || ::std::same_as<T, bool>;

    // todo: complete this concept
    template <typename T>
    concept Route = requires(T r) {
        typename T::next_route_type;
        typename T::route_type;
        { r() }
        ->RouteResponse;
    };

    template <typename T>
    concept NextRoute = Route<T> || ::std::same_as<T, void>;


    enum class logical_operators { none, AND, OR, XOR };

    template <typename RouteType, logical_operators Op, typename NextRoute>
    struct basic_route : public make_inheritable<RouteType> {
        using next_valve_type =
          std::remove_reference_t<std::remove_cv_t<NextRoute>>;

        constexpr static logical_operators op = Op;
        next_valve_type                    next;

        constexpr basic_route(RouteType&&       super,
                              next_valve_type&& _next) noexcept
          : RouteType(std::move(super)),
            next(std::move(_next)) {
        }

        constexpr basic_route(RouteType const&       super,
                              const next_valve_type& _next) noexcept
          : RouteType(super),
            next(_next) {
        }

        constexpr basic_route(RouteType const&  super,
                              next_valve_type&& _next) noexcept
          : RouteType(super),
            next(std::move(_next)) {
        }

        constexpr basic_route(RouteType&&            super,
                              next_valve_type const& _next) noexcept
          : RouteType(std::move(super)),
            next(_next) {
        }

        constexpr basic_route(basic_route const& v) noexcept = default;
        constexpr basic_route(basic_route&& v) noexcept      = default;

        constexpr basic_route&
                               operator=(basic_route const& v) noexcept = default;
        constexpr basic_route& operator=(basic_route&&) noexcept = default;
    };

    template <Route RouteType>
    struct basic_route<RouteType, logical_operators::none, void>
      : public make_inheritable<RouteType> {
        using RouteType::RouteType;
    };



    template <Route RouteType, logical_operators Op = logical_operators::none,
              NextRoute NextRouteType = void>
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

        template <typename... Args,
                  ::std::enable_if_t<
                    ::std::is_constructible_v<super_t, Args...>, int> = 0>
        constexpr route(Args&&... args) noexcept
          : super_t{::std::forward<Args>(args)...} {
        }

        constexpr route(route const&) noexcept = default;

        constexpr route(route&&) noexcept = default;

        using super_t::operator=;

        /**
         * Run the migration
         * @return the response or the context or ...
         */
        template <typename... Args>
        inline auto operator()(Args&&... args) noexcept {
            return super_t::operator()(std::forward<Args>(args)...);
        }


        /**
         * @tparam NewValveType
         * @param valve
         */
        template <logical_operators TheOp, typename NewRouteType>
        [[nodiscard]] constexpr auto
        set_next(NewRouteType&& new_route) const noexcept {
            if constexpr (std::is_void_v<next_route_type>) {
                // this part will only execute when the "next_valve_type" is
                // void

                // the first way (A<X, void> and B<Y, void> === A<X, B<Y, void>>
                return valve<route_type, TheOp, NewRouteType>(
                  *this, std::forward<NewRouteType>(new_route));
            } else {
                // this means this function has a "next" valve already,
                // so it goes to the next's next valve
                // this way we recursively create a valve type and return it.
                auto n =
                  basic_route<route_type, TheOp, NewRouteType>::next.set_next(
                    std::forward<NewRouteType>(new_route));
                return route<route_type, op, decltype(n)>{*this, n};
            }
        }

        [[nodiscard]] constexpr auto
        operator&&(NextRoute auto&& new_route) const noexcept {
            return set_next<logical_operators::AND>(
              std::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] constexpr auto
        operator&(NextRoute auto&& new_route) const noexcept {
            return set_next<logical_operators::AND>(
              std::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] constexpr auto
        operator||(NextRoute auto&& new_route) const noexcept {
            return set_next<logical_operators::OR>(
              std::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] constexpr auto
        operator|(NextRoute auto&& new_route) const noexcept {
            return set_next<logical_operators::OR>(
              std::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] constexpr auto
        operator^(NextRoute auto&& new_route) const noexcept {
            return set_next<logical_operators::XOR>(
              std::forward<decltype(new_route)>(new_route));
        }

        [[nodiscard]] inline bool
        call_this_route(Context auto&& ctx) const noexcept {
            // todo handle the return types
            return super_t::operator()(std::forward<decltype(ctx)>(ctx));
        }

        [[nodiscard]] inline bool
        call_next_route(Context auto&& ctx) const noexcept {
            // todo handle the return types
            if constexpr (::std::is_void_v<next_route_type>) {
                return true; // it's the last route in this sub route, doesn't
                             // matter what I return here; at least not yet
            } else {
                return super_t::next::operator()(
                  std::forward<decltype(ctx)>(ctx));
            }
        }

        [[nodiscard]] inline bool
        operator()(Context auto&& ctx) const noexcept {
            using context_type = decltype(ctx);
            if constexpr (logical_operators::none == op) {
                call_this_route(std::forward<context_type>(ctx));
                call_next_route(std::forward<context_type>(ctx));
                return true;
            } else if constexpr (logical_operators::AND == op) {
                return call_this_route(std::forward<context_type>(ctx)) &&
                       call_next_route(std::forward<context_type>(ctx));
            } else if constexpr (logical_operators::OR == op) {
                return call_this_route(std::forward<context_type>(ctx)) ||
                       call_next_route(std::forward<context_type>(ctx));
            } else if constexpr (logical_operators::XOR == op) {
                return call_this_route(std::forward<context_type>(ctx)) ^
                       call_next_route(std::forward<context_type>(ctx));
            } else {
                // should not happen ever.
                return true;
            }
        }
    };
} // namespace webpp::routes





/*
struct empty_condition {
    template <typename RequestType>
    [[nodiscard]] constexpr bool
    operator()(RequestType const&) const noexcept {
        return true;
    }
};

/**
 * Dynamic version of the above valve class.
 * todo: use a better name maybe?
 * /
template <typename Traits, typename Interface>
struct dynamic_valve {
    static_assert(
      is_traits_v<Traits>,
      "The specified template parameter is not a valid traits type.");

    using traits    = Traits;
    using interface = Interface;
    using req_t     = request_t<traits, interface> const&;

  protected:
    std::function<bool(req_t)> func;

  public:
    dynamic_valve() noexcept                = default;
    dynamic_valve(dynamic_valve const&)     = default;
    dynamic_valve(dynamic_valve&&) noexcept = default;
    dynamic_valve& operator=(dynamic_valve&&) noexcept = default;
    dynamic_valve& operator=(dynamic_valve const&) noexcept = default;

    template <typename NewValve>
    dynamic_valve&
    set_next(NewValve&& v, logical_operators const& op) noexcept(
      std::is_nothrow_invocable_v<NewValve, req_t>) {
        switch (op) {
            case logical_operators::AND:
                return operator&&(std::forward<NewValve>(v));
            case logical_operators::OR:
                return operator||(std::forward<NewValve>(v));
            case logical_operators::XOR:
                return operator^(std::forward<NewValve>(v));
        }
        return *this;
    }

    template <typename NewValve>
    dynamic_valve& operator&&(NewValve&& v) noexcept(
      std::is_nothrow_invocable_v<NewValve, req_t>) {
        func = [=, *this](req_t req) {
            if (func)
                return func(req) && v(req);
            return v(req);
        };
        return *this;
    }

    template <typename NewValve>
    dynamic_valve& operator||(NewValve&& v) noexcept(
      std::is_nothrow_invocable_v<NewValve, req_t>) {
        func = [=, *this](req_t req) {
            if (func)
                return func(req) || v(req);
            return v(req);
        };
        return *this;
    }

    template <typename NewValve>
    dynamic_valve& operator&(NewValve&& v) noexcept(
      std::is_nothrow_invocable_v<NewValve, req_t>) {
        return operator&&(v);
    }

    template <typename NewValve>
    dynamic_valve& operator|(NewValve&& v) noexcept(
      std::is_nothrow_invocable_v<NewValve, req_t>) {
        return operator||(v);
    }

    template <typename NewValve>
    dynamic_valve& operator^(NewValve&& v) noexcept(
      std::is_nothrow_invocable_v<NewValve, req_t>) {
        func = [=, *this](req_t req) {
            bool one = func ? func(req) : true;
            bool two = v(req);
            return (one && !two) || (!one && two);
        };
        return *this;
    }

    [[nodiscard]] bool operator()(req_t req) const
      noexcept(std::is_nothrow_invocable_v<decltype(func), req_t>) {
        return func(req);
    }
};

struct empty_t : public valve<empty_condition> {
    using valve<empty_condition>::valve;

    /**
     * EmptyValve || AnyValve == EmptyValve
     * /
    template <typename NewValve>
    [[nodiscard]] constexpr auto
    operator||(NewValve&& ) const noexcept {
        return empty_t{};
    }

    /**
     * EmptyValve || AnyValve == EmptyValve
     * /
    template <typename NewValve>
    [[nodiscard]] constexpr auto
    operator|(NewValve&& ) const noexcept {
        return empty_t{};
    }

    /**
     *  EmptyValve && AnyValve == AnyValve
     * /
    template <typename NewValve>
    [[nodiscard]] constexpr auto operator&&(NewValve&& v) const noexcept {
        return std::forward<NewValve>(v);
    }

    /**
     * EmptyValve && AnyValve == AnyValve
     * /
    template <typename NewValve>
    [[nodiscard]] constexpr auto operator&(NewValve&& v) const noexcept {
        return std::forward<NewValve>(v);
    }
};
constexpr empty_t empty;
 */


#endif // WEBPP_ROUTES_ROUTE_H
