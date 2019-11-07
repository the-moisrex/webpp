// Created by moisrex on 11/1/19.

#ifndef WEBPP_VALVE_H
#define WEBPP_VALVE_H

#include "../router.h"
#include <string_view>
#include <type_traits>
#include <utility>

namespace webpp {

    enum class logical_operators { AND, OR, XOR };

    template <typename NextValve>
    struct basic_valve {
        NextValve next;
        logical_operators op;

        constexpr basic_valve(NextValve&& next, logical_operators op)
            : next(std::move(next)), op(op) {}

        constexpr basic_valve(NextValve const& next, logical_operators op)
            : next(next), op(op) {}

        constexpr basic_valve(basic_valve const& v) noexcept
            : next(v.valve), op(v.op) {}
        constexpr basic_valve(basic_valve&& v) noexcept = default;

        constexpr basic_valve& operator=(basic_valve const& v) noexcept {
            if (&v == this)
                return *this;
            next = v.next;
            op = v.op;
            return *this;
        }

        constexpr basic_valve& operator=(basic_valve&&) noexcept = default;
    };

    template <>
    struct basic_valve<void> {};

    template <typename ValveType, typename NextValve = void>
    class valve : public basic_valve<NextValve>, public ValveType {
      public:
        using type = ValveType;
        using next_valve_type = NextValve;

        using ValveType::ValveType;
        using basic_valve<NextValve>::basic_valve;
        constexpr valve() noexcept = default;

        /**
         * @tparam NewValveType
         * @param valve
         */
        template <typename NewValveType, typename NewNextValve>
        [[nodiscard]] constexpr auto
        set_next(valve<NewValveType, NewNextValve>&& v,
                 logical_operators the_op) const noexcept {

            if constexpr (std::is_void_v<next_valve_type>) {
                // this part will only execute when the "next_valve_type" is
                // void

                // the first way (A<X, void> and B<Y, void> === A<X, B<Y, void>>
                return valve<ValveType, valve<NewValveType, NewNextValve>>(
                    std::forward<valve<NewValveType, NewNextValve>>(v), the_op);
            } else {
                // this means this function has a "next" valve already,
                // so it goes to the next's next valve
                // this way we recursively create a valve type and return it.
                auto n = basic_valve<NextValve>::next.set_next(
                    std::forward<NewNextValve>(v), the_op);
                return valve<ValveType, decltype(n)>{
                    .op = basic_valve<ValveType>::op, .next = n};
            }
        }

        template <typename NewValveType, typename NewNextValve>
        [[nodiscard]] constexpr auto
        operator&&(valve<NewValveType, NewNextValve>&& v) const noexcept {
            return set_next(std::forward<valve<NewValveType, NewNextValve>>(v),
                            logical_operators::AND);
        }

        template <typename NewValveType, typename NewNextValve>
        [[nodiscard]] constexpr auto
        operator&(valve<NewValveType, NewNextValve>&& v) const noexcept {
            return set_next(std::forward<valve<NewValveType, NewNextValve>>(v),
                            logical_operators::AND);
        }

        template <typename NewValveType, typename NewNextValve>
        [[nodiscard]] constexpr auto
        operator||(valve<NewValveType, NewNextValve>&& v) const noexcept {
            return set_next(std::forward<valve<NewValveType, NewNextValve>>(v),
                            logical_operators::OR);
        }

        template <typename NewValveType, typename NewNextValve>
        [[nodiscard]] constexpr auto
        operator|(valve<NewValveType, NewNextValve>&& v) const noexcept {
            return set_next(std::forward<valve<NewValveType, NewNextValve>>(v),
                            logical_operators::OR);
        }

        template <typename NewValveType, typename NewNextValve>
        [[nodiscard]] constexpr auto
        operator^(valve<NewValveType, NewNextValve>&& v) const noexcept {
            return set_next(std::forward<valve<NewValveType, NewNextValve>>(v),
                            logical_operators::XOR);
        }

        template <typename Interface>
        [[nodiscard]] bool operator()(request_t<Interface>& req) const
            noexcept {
            return false;
        }
    };

    template <typename ValveType, typename Interface>
    bool calculate(ValveType const& v, request_t<Interface> const& req,
                   bool last_value = true) noexcept {
        auto res = v();
        switch (v.logic_op()) {
        case logical_operators::AND:
            if (!last_value || !res)
                return false;
            if (v.has_next())
                return calculate(*v.next_valve(), req, res);
            else
                return v();
        case logical_operators::OR:
            break;
        case logical_operators::XOR:
            break;
        }
    }

    struct method_t {
      private:
        std::string_view method_string;

      public:
        constexpr method_t(std::string_view str) noexcept
            : method_string(str) {}
        constexpr method_t() noexcept = default;

        template <typename Interface>
        [[nodiscard]] bool operator()(request_t<Interface>& req) noexcept {
            return req.request_method() == method_string;
        }
    };

    struct method : public valve<method_t> {
        using valve::valve;
    };

} // namespace webpp

#endif // WEBPP_VALVE_H
