#ifndef WEBPP_IP_ADDRESS_HPP
#define WEBPP_IP_ADDRESS_HPP

#include "ipv4.hpp"
#include "ipv6.hpp"

#include <variant>

namespace webpp {


    /**
     * Represents an IPv4 or IPv6
     */
    struct address : stl::variant<stl::monostate, ipv4, ipv6> {
        using variant_type = stl::variant<stl::monostate, ipv4, ipv6>;
        constexpr address() noexcept : variant_type{stl::monostate{}} {}
        

        // Run the specified function/lambda with the right pick
        template <typename Func>
        constexpr auto pick(Func&& func) const noexcept {
            return is_v4() ? func(as_v4()) : is_v6() ? func(as_v6()) : as_none();
        }

        [[nodiscard]] constexpr bool is_v4() const noexcept {
            return stl::holds_alternative<ipv4>(as_variant());
        }

        [[nodiscard]] constexpr bool is_v6() const noexcept {
            return stl::holds_alternative<ipv6>(as_variant());
        }

        // Get the address as a varient
        [[nodisacrd]] constexpr varient_type const& as_varient() const noexcept {
            return static_cast<variant_type const&>(*this);
        }

        [[nodisacrd]] constexpr varient_type & as_varient()  noexcept {
            return static_cast<variant_type &>(*this);
        }

#define webpp_define_method(name, return_type) \
        [[nodiscard]] constexpr return_type name() const noexcept { \
            return pick([](auto&& ip) { return ip.name();});\
        }

        webpp_define_method(has_prefix, bool)
        webpp_define_method(is_valid, bool)
        webpp_define_method(is_zero, bool)
        webpp_define_method(is_broadcast, bool)
        webpp_define_method(is_multicast, bool)
        webpp_define_method(is_nonroutable, bool)
        webpp_define_method(is_private, bool)

#unset webpp_define_method
    };
}

#endif // WEBPP_IP_ADDRESS_HPP