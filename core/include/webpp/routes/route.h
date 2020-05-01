// Created by moisrex on 11/1/19.

#ifndef WEBPP_VALVE_H
#define WEBPP_VALVE_H

#include "../http/request.h"

#include <type_traits>
#include <utility>

namespace webpp::routes {

    /**
     * The new routing system:
     *
     * Problems this system will solve compared to the valving system:
     *   - The migrations now can access the context
     *     Previously it was possible to implement this through some ways but
     *     the problem was that the whole thing was based on requests and
     *     responses, which meant that we couldn't pass other objects and types
     *     without modifying the request and response types which adding a new
     *     parameter into their template parameter lists only introduced more
     *     complexity for the library users
     *     Other ways we could've solved this issue:
     *       - Using virtual inheritance for requests
     *         Would've added unnecessary indirection
     *       - Using self made vtable
     *         Would've added unnecessary indirection
     *       - Using void* type or similar ones and cast them later
     *         Would've added unnecessary indirection
     *       - Passing a context object to the migrations/valves to solve this
     *         Which would've introduced complexity because at that point the
     *         requests and the responses were not retrievable from the context.
     *
     *   - The valves are not aware of the requests and other details provided
     *     by the context.
     *     Previously this was an issue since the valves were only capable of
     *     checking if the request is a match for the valve or not and they
     *     couldn't modify the response or even stop the request or provide
     *     information for the migration.
     *
     *   - A better constexpr way of passing valves and migrations to the router
     *     Since the valves and the migrations were a separate entities which
     *     they would become one entity after creating the "route" template
     *     class, it was a problem to create a nicer looking route by the user.
     *
     *   - Using valves as validators and validators as valves.
     *     Using valves as validators are now easier since the user doesn't have
     *     to create a constexpr variable and use that variable in both the
     *     .on method on router and inside the migration.
     *     Now the whole thing is inside the context, and it'll be passed to the
     *     valves/migrations by reference.
     *     Of course using validators as valves require a bit of a hack from the
     *     user but there's just no other way and honestly it's easy enough to
     *     do so!
     *
     *
     * Route types:
     *   - based on position:
     *     - Entry routes
     *     - Sub routes
     *   - based on returned type:
     *     - Condition route
     *     - Response route
     *     - Migrator route
     *
     * Definitions:
     *   - Route:       Possible ways that are capable of handling user requests
     *   - Entry Route: The main routes that will be checked first
     *   - Sub Route:   The routes belong to the each Entry Routes
     *   - Condition route:
     *                  Which can be an entry route or a sub route.
     *                  This kind of route only returns boolean and its job is
     *                  to check if the specified request can be handled by the
     *                  sub routes of this condition route.
     *   - Response route:
     *                  A route that will terminate the route checking process
     *                  and returns a response that will be sent to the client.
     *   - Migrator route:
     *                  A route that will change the context in a way that will
     *                  be useful for the other sub routes or even other entry
     *                  routes.
     *   - Route chain: A route chain is exactly one entry route and undefined
     *                  number of sub routes of that one entry route.
     *   - Context:     An object of arbitrary type that will contain everything
     *                  that routes will need including:
     *                    - some types:
     *                      - Traits
     *                      - Interface
     *                      - Next sub route
     *                    - references to:
     *                      - Request
     *                      - Response
     *                      - Response Route in this route chain
     *                    - Previous entry routes context changes
     *                    - Previous sub routes context changes
     *                    - Original entry routes level context
     *                    - Original sub routes context changes
     *   - Context Passing Pattern:
     *                   A pattern designed to share arbitrary data down the
     *                   routing chain.
     *
     *
     * Features we need:
     *   - [ ] Extendable entry and sub routes
     *     - [ ] Use class operator() as a place to add more sub-routes
     *   - [ ] Having access to the context class in the callables
     *   - [ ] Having access to the request and the response
     *   - [ ] Termination of continuation of checking the sub-routes by parents
     *   - [ ] Termination of continuation of checking the entry-routes by any
     *         previous routes, or sub-routes.
     *   - [ ] Context modification
     *     - [ ] Sub-Route local context modification by any previous sub-routes
     *     - [ ] Inter-Entry-Route context modification by any previous
     *           (sub/entry) routes
     *   - [ ] Entry-Route prioritization
     *     - [ ] Auto prioritization
     *     - [ ] Manual prioritization
     *     - [ ] Hinted prioritization
     *     - [ ] On-The-Fly Re-Prioritization
     *   - [ ] Dynamic route generation / Dynamic route switching
     *   - [ ] Context Passing pattern
     *
     *
     * A bit hard to implement places:
     *   - Prioritization of the entry routes:
     *     It's easy to do so actually but it'll be a problem because I don't
     *     want to use dynamically allocated containers in the implementation
     *     of this. Adding indirection will cause the routing system to be
     *     processed at run-time. That's not something I'm willing to do yet.
     *
     *   - The context type:
     *     It's defined as an arbitrary type, that eases the pain; but there're
     *     still a few problems standing:
     *       - Saving the changes
     *
     *
     * Usage examples:
     *   - .on(get and "/about"_path >> about_page)
     *   - ( get and "/home"_path ) or ( post && "/home/{page}"_tpath ) >> ...
     *   - opath() /"home" / integer("page") /
     *   - empty / action_list("action", {"open", "delete"}) >> []{...}
     *   - (...) >> &non_templated_class_with_templated_callable_operator
     *   - get and "/profile/"_tpath and set_by_class_constructor
     */


    struct empty_condition {
        template <typename RequestType>
        [[nodiscard]] constexpr bool
        operator()(RequestType const& /* req */) const noexcept {
            return true;
        }
    };

    enum class logical_operators { AND, OR, XOR };

    template <typename ValveType, typename NextValve>
    struct basic_valve : public ValveType {
        using next_valve_type =
          std::remove_reference_t<std::remove_cv_t<NextValve>>;

        next_valve_type   next;
        logical_operators op;

        constexpr basic_valve(ValveType&& super, next_valve_type&& _next,
                              logical_operators op) noexcept
          : ValveType(std::move(super)),
            next(std::move(_next)),
            op(op) {
        }

        constexpr basic_valve(ValveType const&       super,
                              const next_valve_type& _next,
                              logical_operators      op) noexcept
          : ValveType(super),
            next(_next),
            op(op) {
        }

        constexpr basic_valve(ValveType const& super, next_valve_type&& _next,
                              logical_operators op) noexcept
          : ValveType(super),
            next(std::move(_next)),
            op(op) {
        }

        constexpr basic_valve(ValveType&& super, next_valve_type const& _next,
                              logical_operators op) noexcept
          : ValveType(std::move(super)),
            next(_next),
            op(op) {
        }

        constexpr basic_valve(basic_valve const& v) noexcept = default;
        constexpr basic_valve(basic_valve&& v) noexcept      = default;

        constexpr basic_valve&
                               operator=(basic_valve const& v) noexcept = default;
        constexpr basic_valve& operator=(basic_valve&&) noexcept = default;
    };

    template <typename ValveType>
    struct basic_valve<ValveType, void> : public ValveType {
        using ValveType::ValveType;
    };

    template <typename ValveType, typename NextValve = void>
    class valve : public basic_valve<ValveType, NextValve> {
      public:
        using type            = ValveType;
        using next_valve_type = NextValve;

        using basic_valve<ValveType, NextValve>::basic_valve;
        constexpr valve() noexcept = default;

        /**
         * @tparam NewValveType
         * @param valve
         */
        template <typename NewValve>
        [[nodiscard]] constexpr auto
        set_next(NewValve&& v, logical_operators the_op) const noexcept {
            if constexpr (std::is_void_v<next_valve_type>) {
                // this part will only execute when the "next_valve_type" is
                // void

                // the first way (A<X, void> and B<Y, void> === A<X, B<Y, void>>
                return valve<ValveType, NewValve>(
                  *this, std::forward<NewValve>(v), std::move(the_op));
            } else {
                // this means this function has a "next" valve already,
                // so it goes to the next's next valve
                // this way we recursively create a valve type and return it.
                auto n = basic_valve<ValveType, NextValve>::next.set_next(
                  std::forward<NewValve>(v), the_op);
                return valve<ValveType, decltype(n)>{*this, n, this->op};
            }
        }

        template <typename NewValve>
        [[nodiscard]] constexpr auto operator&&(NewValve&& v) const noexcept {
            if constexpr (std::is_convertible_v<
                            typename std::decay_t<NewValve>::type,
                            empty_condition>) {
                // AnyValve && EmptyValve == AnyValve
                return *this;
            } else {
                return set_next(std::forward<NewValve>(v),
                                logical_operators::AND);
            }
        }

        template <typename NewValve>
        [[nodiscard]] constexpr auto operator&(NewValve&& v) const noexcept {
            if constexpr (std::is_convertible_v<
                            typename std::decay_t<NewValve>::type,
                            empty_condition>) {
                // AnyValve && EmptyValve == AnyValve
                return *this;
            } else {
                return set_next(std::forward<NewValve>(v),
                                logical_operators::AND);
            }
        }

        template <typename NewValve>
        [[nodiscard]] constexpr auto operator||(NewValve&& v) const noexcept {
            if constexpr (std::is_convertible_v<
                            typename std::decay_t<NewValve>::type,
                            empty_condition>) {
                // AnyValve || EmptyValve == EmptyValve
                return valve<empty_condition>{};
            } else {
                return set_next(std::forward<NewValve>(v),
                                logical_operators::OR);
            }
        }

        template <typename NewValve>
        [[nodiscard]] constexpr auto operator|(NewValve&& v) const noexcept {
            if constexpr (std::is_convertible_v<
                            typename std::decay_t<NewValve>::type,
                            empty_condition>) {
                // AnyValve || EmptyValve == EmptyValve
                return valve<empty_condition>{};
            } else {
                return set_next(std::forward<NewValve>(v),
                                logical_operators::OR);
            }
        }

        template <typename NewValve>
        [[nodiscard]] constexpr auto operator^(NewValve&& v) const noexcept {
            return set_next(std::forward<NewValve>(v), logical_operators::XOR);
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

    /**
     * Dynamic version of the above valve class.
     * todo: use a better name maybe?
     */
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
         */
        template <typename NewValve>
        [[nodiscard]] constexpr auto
        operator||(NewValve&& /* v */) const noexcept {
            return empty_t{};
        }

        /**
         * EmptyValve || AnyValve == EmptyValve
         */
        template <typename NewValve>
        [[nodiscard]] constexpr auto
        operator|(NewValve&& /* v */) const noexcept {
            return empty_t{};
        }

        /**
         *  EmptyValve && AnyValve == AnyValve
         */
        template <typename NewValve>
        [[nodiscard]] constexpr auto operator&&(NewValve&& v) const noexcept {
            return std::forward<NewValve>(v);
        }

        /**
         * EmptyValve && AnyValve == AnyValve
         */
        template <typename NewValve>
        [[nodiscard]] constexpr auto operator&(NewValve&& v) const noexcept {
            return std::forward<NewValve>(v);
        }
    };
    constexpr empty_t empty;

} // namespace webpp::routes

#endif // WEBPP_VALVE_H
