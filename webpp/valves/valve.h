// Created by moisrex on 11/1/19.

#ifndef WEBPP_VALVE_H
#define WEBPP_VALVE_H

#include "../router.h"
#include <memory>

namespace webpp {

    class valve {
      protected:
        // The next valve in the valve chain
        enum class logical_operators {
            NONE,
            AND,
            OR,
            XOR
        } op = logical_operators::NONE;

      public:
        constexpr valve() noexcept = default;

        template <typename Valve>
        constexpr valve& operator&&(Valve const& v) noexcept {
            static_assert(std::is_convertible_v<Valve, bool> ||
                              std::is_function(Valve::operator()),
                          "should be convertible to bool");
            op = logical_operators::AND;
            return *this;
        }

        template <typename Valve>
        constexpr auto operator&(Valve const& a) noexcept {
            return operator&&(a);
        }
    };

    template <typename Interface>
    constexpr bool calculate(valve v,
                             request_t<Interface> const& req) noexcept {
        for (;;) {
            if (!v(req))
                return false;
            if (v.condition == nullptr)
                break;
            v = v.condition;
        };
        return true;
    }

    struct get_t : public valve {
        template <typename Interface>
        constexpr bool operator()(request_t<Interface> const& req) {
            return req.request_method() == "GET";
        }
    };

    constexpr get_t get;

} // namespace webpp

#endif // WEBPP_VALVE_H
