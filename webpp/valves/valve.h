// Created by moisrex on 11/1/19.

#ifndef WEBPP_VALVE_H
#define WEBPP_VALVE_H

#include "../router.h"
#include <functional>

namespace webpp {

    enum class logical_operators { NONE, AND, OR, XOR };

    class valve {
      public:
      protected:
        // true: and, false: or
        logical_operators op = logical_operators::NONE;
        std::function<bool(request_t<Interface>)> func;

        void set_next(valve const& v, logical_operators the_op) noexcept {
            if (next) {
                next->set_next(v, the_op);
            } else {
                next = std::make_unique<valve>(v);
                next->op = the_op;
            }
        }

        void set_next(valve&& v, logical_operators the_op) noexcept {
            // todo: next->op != logical_operators::NONE
            if (next) {
                next->set_next(std::move(v), the_op);
            } else {
                next = std::make_unique<valve>(std::move(v));
                next->op = the_op;
            }
        }

      public:
        valve() noexcept = default;
        valve(valve const& v) noexcept
            : op(v.op), next(std::make_unique<valve>(*v.next)) {}
        valve(valve&& v) noexcept = default;

        valve& operator=(valve const& v) noexcept {
            if (&v == this)
                return *this;
            op = v.op;
            next = std::make_unique<valve>(*v.next);
            return *this;
        }

        valve& operator=(valve&&) noexcept = default;

        valve& operator&&(valve const& v) noexcept {
            set_next(v, logical_operators::AND);
            return *this;
        }

        valve& operator&&(valve&& v) noexcept {
            set_next(v, logical_operators::AND);
            return *this;
        }

        auto operator&(valve&& a) noexcept { return operator&&(std::move(a)); }
        auto operator&(valve const& a) noexcept { return operator&&(a); }

        valve& operator||(valve const& v) noexcept {
            set_next(v, logical_operators::OR);
            return *this;
        }

        valve& operator||(valve&& v) noexcept {
            set_next(std::move(v), logical_operators::OR);
            return *this;
        }

        auto operator|(valve const& v) noexcept { return operator||(v); }
        auto operator|(valve&& v) noexcept { return operator||(std::move(v)); }

        valve& operator^(valve const& v) noexcept {
            set_next(v, logical_operators::XOR);
            return *this;
        }

        valve& operator^(valve&& v) noexcept {
            set_next(std::move(v), logical_operators::XOR);
            return *this;
        }

        [[nodiscard]] bool has_next() const noexcept { return next != nullptr; }

        [[nodiscard]] auto logic_op() const noexcept { return op; }

        [[nodiscard]] auto& next_valve() noexcept { return next; }

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
