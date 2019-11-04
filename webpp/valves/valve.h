// Created by moisrex on 11/1/19.

#ifndef WEBPP_VALVE_H
#define WEBPP_VALVE_H

#include "../router.h"
#include <functional>

namespace webpp {

    enum class logical_operators { NONE, AND, OR, XOR };

    template <typename PrevValve>
    struct basic_valve {

        // true: and, false: or
        logical_operators op = logical_operators::NONE;

        PrevValve prev;

        basic_valve(basic_valve const& v) noexcept : op(v.op), prev(v.valve) {}
        basic_valve(basic_valve&& v) noexcept = default;

        basic_valve& operator=(basic_valve const& v) noexcept {
            if (&v == this)
                return *this;
            op = v.op;
            prev = v.prev;
            return *this;
        }

        basic_valve& operator=(basic_valve&&) noexcept = default;
    };

    template <>
    struct basic_valve<void> {};

    template <typename ValveType, typename PrevValve = void>
    class valve : protected basic_valve<PrevValve>, public ValveType {
      public:
        using type = ValveType;
        using prev_valve_type = PrevValve;

      protected:
      public:
        valve() noexcept = default;

        template <typename NewValveType>
        auto operator&&(NewValveType const& v) noexcept {

            // the first way
            valve<ValveType, typename NewValveType::prev_valve_type> one{
                .op = logical_operators::AND, .prev = *this};

            // the second way
            valve<typename NewValveType::type, NewValveType> two;
        }

        valve& operator&&(valve&& v) noexcept {
            set_prev(v, logical_operators::AND);
            return *this;
        }

        auto operator&(valve&& a) noexcept { return operator&&(std::move(a)); }
        auto operator&(valve const& a) noexcept { return operator&&(a); }

        valve& operator||(valve const& v) noexcept {
            set_prev(v, logical_operators::OR);
            return *this;
        }

        valve& operator||(valve&& v) noexcept {
            set_prev(std::move(v), logical_operators::OR);
            return *this;
        }

        auto operator|(valve const& v) noexcept { return operator||(v); }
        auto operator|(valve&& v) noexcept { return operator||(std::move(v)); }

        valve& operator^(valve const& v) noexcept {
            set_prev(v, logical_operators::XOR);
            return *this;
        }

        valve& operator^(valve&& v) noexcept {
            set_prev(std::move(v), logical_operators::XOR);
            return *this;
        }

        [[nodiscard]] bool has_prev() const noexcept { return prev != nullptr; }

        [[nodiscard]] auto logic_op() const noexcept { return op; }

        [[nodiscard]] auto& prev_valve() noexcept { return prev; }

        template <typename Interface>
        [[nodiscard]] bool operator()(request_t<Interface>& req) noexcept {
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
            if (v.has_prev())
                return calculate(*v.prev_valve(), req, res);
            else
                return v();
        case logical_operators::OR:
            break;
        case logical_operators::XOR:
            break;
        }
    }

    struct method : public valve {
      private:
        std::string method_string;

      public:
        explicit method(std::string const& str) noexcept : method_string(str) {}
        explicit method(std::string& str) noexcept
            : method_string(std::move(str)) {}

        template <typename Interface>
        [[nodiscard]] bool operator()(request_t<Interface>& req) noexcept {
            return req.request_method() == method_string;
        }
    };

} // namespace webpp

#endif // WEBPP_VALVE_H
