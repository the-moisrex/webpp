// Created by moisrex on 11/1/19.

#ifndef WEBPP_VALVE_H
#define WEBPP_VALVE_H

#include "../router.h"
#include <memory>

namespace webpp {

    enum class logical_operators { NONE, AND, OR, XOR };

    template <typename ValveType, typename NextValveType>
    class valve : public ValveType {
      public:
      protected:
        // true: and, false: or
        logical_operators op = logical_operators::NONE;
        std::unique_ptr<NextValveType> next;

        void set_next(valve const& v, logical_operators the_op) noexcept {
            if (next) {
                next->set_next(v, the_op);
            } else {
                next = std::make_unique<NextValveType>(v);
                next->op = the_op;
            }
        }

        void set_next(valve&& v, logical_operators the_op) noexcept {
            if (next) {
                next->set_next(std::move(v), the_op);
            } else {
                next = std::make_unique<NextValveType>(std::move(v));
                next->op = the_op;
            }
        }

      public:
        valve() noexcept = default;
        valve(valve const& v) noexcept
            : op(v.op), next(std::make_unique<NextValveType>(*v.next)) {}
        valve(valve&& v) noexcept = default;

        valve& operator=(valve<NextValveType> const& v) noexcept {
            if (&v == this)
                return *this;
            op = v.op;
            next = std::make_unique<NextValveType>(*v.next);
            return *this;
        }

        valve& operator=(valve<NextValveType>&&) noexcept = default;

        valve& operator&&(NextValveType const& v) noexcept {
            set_next(v, logical_operators::AND);
            return *this;
        }

        valve& operator&&(NextValveType&& v) noexcept {
            set_next(v, logical_operators::AND);
            return *this;
        }

        auto operator&(NextValveType&& a) noexcept {
            return operator&&(std::move(a));
        }
        auto operator&(NextValveType const& a) noexcept {
            return operator&&(a);
        }

        valve& operator||(NextValveType const& v) noexcept {
            set_next(v, logical_operators::OR);
            return *this;
        }

        valve& operator||(NextValveType&& v) noexcept {
            set_next(std::move(v), logical_operators::OR);
            return *this;
        }

        auto operator|(NextValveType const& v) noexcept {
            return operator||(v);
        }
        auto operator|(NextValveType&& v) noexcept {
            return operator||(std::move(v));
        }

        valve& operator^(NextValveType const& v) noexcept {
            set_next(v, logical_operators::XOR);
            return *this;
        }

        valve& operator^(NextValveType&& v) noexcept {
            set_next(std::move(v), logical_operators::XOR);
            return *this;
        }

        [[nodiscard]] bool has_next() const noexcept { return next != nullptr; }

        [[nodiscard]] auto logic_op() const noexcept { return op; }

        [[nodiscard]] auto& next_valve() noexcept { return next; }
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

    struct method_condition {
      private:
        std::string method_string;

      public:
        explicit method_condition(std::string const& str) noexcept
            : method_string(str) {}
        explicit method_condition(std::string& str) noexcept
            : method_string(std::move(str)) {}

        template <typename Interface>
        constexpr bool operator()(request_t<Interface> const& req) const
            noexcept {
            return req.request_method() == method_string;
        }
    };

    using method = valve<method_condition>;

    auto get = method{"GET"};

} // namespace webpp

#endif // WEBPP_VALVE_H
