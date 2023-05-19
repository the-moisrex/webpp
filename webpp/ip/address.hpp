#ifndef WEBPP_IP_ADDRESS_HPP
#define WEBPP_IP_ADDRESS_HPP

#include "ipv4.hpp"
#include "ipv6.hpp"

#include <variant>

namespace webpp {


    // this is a boilerplate utility to let the users of "address" class easily and without any exceptions
    // use the .pick member function.
    struct invalid_ip_address {
        // NOLINTBEGIN(readability-convert-member-functions-to-static)
        [[nodiscard]] constexpr operator bool() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool is_zero() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool is_loopback() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool is_link_local() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool is_broadcast() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool is_nonroutable() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool is_private() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool is_multicast() const noexcept {
            return false;
        }
        [[nodiscard]] constexpr bool has_prefix() const noexcept {
            return false;
        }
        // NOLINTEND(readability-convert-member-functions-to-static)
    };


    /**
     * Represents an IPv4 or IPv6
     */
    struct address : stl::variant<stl::monostate, ipv4, ipv6> {
        using variant_type = stl::variant<stl::monostate, ipv4, ipv6>;

        // variant ctor
        using stl::variant<stl::monostate, ipv4, ipv6>::variant;

        ////////////////////////////// IPv4 Constructors //////////////////////////////

        // NOLINTBEGIN(bugprone-easily-swappable-parameters)
        constexpr address(ipv4_octet octet1,
                          ipv4_octet octet2,
                          ipv4_octet octet3,
                          ipv4_octet octet4,
                          ipv4_octet prefix_val = prefix_status(inet_pton4_status::valid)) noexcept
          : address{ipv4{octet1, octet2, octet3, octet4, prefix_val}} {}

        constexpr address(ipv4_octet       octet1,
                          ipv4_octet       octet2,
                          ipv4_octet       octet3,
                          ipv4_octet       octet4,
                          stl::string_view subnet) noexcept
          : address{ipv4{octet1, octet2, octet3, octet4, subnet}} {}

        constexpr explicit address(stl::uint32_t ip,
                                   ipv4_octet    prefix = prefix_status(inet_pton4_status::valid)) noexcept
          : address{ipv4{ip, prefix}} {}

        template <istl::StringViewifiable StrT>
        constexpr explicit address(stl::uint32_t ip, StrT&& subnet) noexcept
          : address{ipv4{ip, stl::forward<StrT>(subnet)}} {}

        constexpr address(ipv4_octets ip,
                          ipv4_octet  prefix = prefix_status(inet_pton4_status::valid)) noexcept
          : address{ipv4{ip, prefix}} {}

        template <istl::StringViewifiable StrT>
        constexpr address(ipv4_octets ip, StrT&& subnet) noexcept
          : address{ipv4{ip, stl::forward<StrT>(subnet)}} {}

        constexpr address(ipv4_octets ip, ipv4_octets subnet) noexcept : address{ipv4{ip, subnet}} {}

        // NOLINTEND(bugprone-easily-swappable-parameters)


        ////////////////////////////// IPv6 Constructors //////////////////////////////

        constexpr address(ipv6::octets8_t const& _octets,
                          stl::uint8_t prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : address{ipv6{_octets, prefix_value}} {}

        constexpr address(ipv6::octets16_t const& _octets,
                          stl::uint8_t prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : address{ipv6{_octets, prefix_value}} {}

        constexpr address(ipv6::octets32_t const& _octets,
                          stl::uint8_t prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : address{ipv6{_octets, prefix_value}} {}

        constexpr address(ipv6::octets64_t const& _octets,
                          stl::uint8_t prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : address{ipv6{_octets, prefix_value}} {}

        ////////////////////////////// Common Functions //////////////////////////////

        // Run the specified function/lambda with the right pick
        template <typename Func>
        constexpr auto pick(Func&& func) const noexcept(noexcept(func(ipv4{})) && noexcept(func(ipv6{}))) {
            if (auto* v4 = get_if<ipv4>(&as_variant())) {
                return func(*v4);
            } else if (auto* v6 = get_if<ipv6>(&as_variant())) {
                return func(*v6);
            } else {
                return func(as_none());
            }
        }

        [[nodiscard]] static constexpr invalid_ip_address as_none() noexcept {
            return {};
        }

        [[nodiscard]] constexpr ipv4 const& as_v4() const {
            return get<ipv4>(as_variant());
        }

        [[nodiscard]] constexpr ipv4& as_v4() {
            return get<ipv4>(as_variant());
        }

        [[nodiscard]] constexpr ipv6 const& as_v6() const {
            return get<ipv6>(as_variant());
        }

        [[nodiscard]] constexpr ipv6& as_v6() {
            return get<ipv6>(as_variant());
        }

        [[nodiscard]] constexpr bool is_v4() const noexcept {
            return stl::holds_alternative<ipv4>(as_variant());
        }

        [[nodiscard]] constexpr bool is_v6() const noexcept {
            return stl::holds_alternative<ipv6>(as_variant());
        }

        // Get the address as a variant
        [[nodiscard]] constexpr variant_type const& as_variant() const noexcept {
            return static_cast<variant_type const&>(*this);
        }


        [[nodiscard]] constexpr variant_type& as_variant() noexcept {
            return static_cast<variant_type&>(*this);
        }

        [[nodiscard]] constexpr bool has_prefix() const noexcept {
            return pick([](auto&& ip) constexpr noexcept {
                return ip.has_prefix();
            });
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return pick([](auto&& ip) constexpr noexcept {
                return ip.is_valid();
            });
        }

        [[nodiscard]] constexpr bool is_zero() const noexcept {
            return pick([](auto&& ip) constexpr noexcept {
                return ip.is_zero();
            });
        }

        [[nodiscard]] constexpr bool is_broadcast() const noexcept {
            return pick([](auto&& ip) constexpr noexcept {
                return ip.is_broadcast();
            });
        }

        [[nodiscard]] constexpr bool is_multicast() const noexcept {
            return pick([](auto&& ip) constexpr noexcept {
                return ip.is_multicast();
            });
        }

        [[nodiscard]] constexpr bool is_nonroutable() const noexcept {
            return pick([](auto&& ip) constexpr noexcept {
                return ip.is_nonroutable();
            });
        }

        [[nodiscard]] constexpr bool is_private() const noexcept {
            return pick([](auto&& ip) constexpr noexcept {
                return ip.is_private();
            });
        }

        [[nodiscard]] constexpr bool is_loopback() const noexcept {
            return pick([](auto&& ip) constexpr noexcept {
                return ip.is_loopback();
            });
        }
    };
} // namespace webpp

#endif // WEBPP_IP_ADDRESS_HPP
