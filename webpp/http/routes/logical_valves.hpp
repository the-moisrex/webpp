// Created by moisrex on 4/3/23.

#ifndef WEBPP_LOGICAL_VALVES_HPP
#define WEBPP_LOGICAL_VALVES_HPP

#include "valve_traits.hpp"

namespace webpp::http {
    template <typename Callable>
    struct not_valve : valve<not_valve<Callable>> {
        using valve_type = valve<not_valve<Callable>>;
        using next_type  = Callable;

      private:
        [[no_unique_address]] next_type next;

      public:
        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <stl::convertible_to<Callable> C>
        explicit constexpr not_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)
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
        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <stl::convertible_to<Callable> C>
        explicit constexpr negative_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)
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
        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <stl::convertible_to<Callable> C>
        explicit constexpr positive_valve(C&& inp_next) : next{stl::forward<C>(inp_next)} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

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


} // namespace webpp::http

#endif // WEBPP_LOGICAL_VALVES_HPP
