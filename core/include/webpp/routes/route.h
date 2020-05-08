// Created by moisrex on 11/1/19.

#ifndef WEBPP_VALVE_H
#define WEBPP_VALVE_H

#include "../http/request.h"
#include "../http/response.h"
#include "../utils/functional.h"
#include "context.h"

#include <type_traits>
#include <utility>

namespace webpp::routes {


    template <typename T>
    concept RouteResponse =
      Response<T> || ConvertibleToResponse<T> || Context<T>;

    // todo: complete this concept
    template <typename T>
    concept Route = requires(T r) {
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



    template <Route RouteType, logical_operators Op, NextRoute NextRouteType>
    struct route : public basic_route<RouteType, Op, NextRouteType> {

        using route_t                         = RouteType;
        using next_route_t                    = NextRouteType;
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
            if constexpr (std::is_void_v<next_route_t>) {
                // this part will only execute when the "next_valve_type" is
                // void

                // the first way (A<X, void> and B<Y, void> === A<X, B<Y, void>>
                return valve<route_t, TheOp, NewRouteType>(
                  *this, std::forward<NewRouteType>(new_route));
            } else {
                // this means this function has a "next" valve already,
                // so it goes to the next's next valve
                // this way we recursively create a valve type and return it.
                auto n =
                  basic_route<route_t, TheOp, NewRouteType>::next.set_next(
                    std::forward<NewRouteType>(new_route));
                return route<route_t, op, decltype(n)>{*this, n};
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

        template <typename RequestType>
        [[nodiscard]] bool operator()(RequestType const& req) const noexcept {
            if constexpr (std::is_void_v<NextValve>) {
                return ValveType::operator()(req);
            } else {
                switch (basic_valve<ValveType, NextValve>::op) {
                    case logical_operators::AND:
                        return ValveType::operator()(req) &&
                               basic_valve<ValveType, NextValve>::next.
                               operator()(req);
                    case logical_operators::OR:
                        return ValveType::operator()(req) ||
                               basic_valve<ValveType, NextValve>::next.
                               operator()(req);
                    case logical_operators::XOR:
                        return ValveType::operator()(req) ^
                               basic_valve<ValveType, NextValve>::next.
                               operator()(req);
                    default: return false;
                }
            }
        }
    };





    /*
    struct empty_condition {
        template <typename RequestType>
        [[nodiscard]] constexpr bool
        operator()(RequestType const&) const noexcept {
            return true;
        }
    };

    template <typename ValveType, typename NextValve = void>
    class valve : public basic_valve<ValveType, NextValve> {
      public:
        using type            = ValveType;
        using next_valve_type = NextValve;

        using basic_valve<ValveType, NextValve>::basic_valve;
        constexpr valve() noexcept = default;

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

} // namespace webpp::routes

#endif // WEBPP_VALVE_H
