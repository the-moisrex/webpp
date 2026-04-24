#ifndef WEBPP_IP_ADDRESS_HPP
#define WEBPP_IP_ADDRESS_HPP

#include "ipv4.hpp"
#include "ipv6.hpp"

#include <compare>
#include <variant>

namespace webpp {

    /**
     * Represents an IPv4 or IPv6
     */
    struct ip_address : stl::variant<ipv4, ipv6> {
        using variant_type = stl::variant<ipv4, ipv6>;

        // variant ctor
        using stl::variant<ipv4, ipv6>::variant;

      private:
        template <typename CharT>
        constexpr void parse(stl::basic_string_view<CharT> ip_addr) noexcept {
            // first, let's try parsing it as an ipv4 address
            if (ipv4 const ip4{ip_addr}; ip4.status() == inet_pton4_status::invalid_character) {
                *this = ipv6{ip_addr};
            } else {
                // either it's a valid ipv4, or it's completely invalid ip address,
                // but either way, we store the ipv4 version because it contains the error message,
                // and that's what the user may want
                *this = ip4;
            }
        }

      public:
        ////////////////////////////// Common Constructors //////////////////////////////

        // return an invalid address
        static constexpr ip_address invalid() noexcept {
            return {ipv4::invalid()};
        }

        // invalid ipv4
        constexpr ip_address() noexcept : ip_address{ipv4{prefix_status(inet_pton4_status::invalid_character)}} {}

        explicit constexpr ip_address(stl::string_view const ip_addr) noexcept {
            parse(ip_addr);
        }

        template <typename CharT>
        explicit constexpr ip_address(stl::basic_string_view<CharT> const ip_addr) noexcept {
            parse(ip_addr);
        }

        template <typename CharT>
        constexpr ip_address& operator=(stl::basic_string_view<CharT> const ip_addr) noexcept {
            parse(ip_addr);
            return *this;
        }

        ////////////////////////////// IPv4 Constructors //////////////////////////////

        // NOLINTBEGIN(bugprone-easily-swappable-parameters)
        constexpr ip_address(
          ipv4_octet const octet1,
          ipv4_octet const octet2,
          ipv4_octet const octet3,
          ipv4_octet const octet4,
          ipv4_octet const prefix_val = prefix_status(inet_pton4_status::valid)) noexcept
          : ip_address{
              ipv4{octet1, octet2, octet3, octet4, prefix_val}
        } {}

        constexpr ip_address(
          ipv4_octet const       octet1,
          ipv4_octet const       octet2,
          ipv4_octet const       octet3,
          ipv4_octet const       octet4,
          stl::string_view const subnet) noexcept
          : ip_address{
              ipv4{octet1, octet2, octet3, octet4, subnet}
        } {}

        constexpr explicit ip_address(stl::uint32_t const ip_int,
                                      ipv4_octet const    prefix = prefix_status(inet_pton4_status::valid)) noexcept
          : ip_address{
              ipv4{ip_int, prefix}
        } {}

        template <typename CharT>
        constexpr explicit ip_address(stl::uint32_t const ip_addr, stl::basic_string_view<CharT> const subnet) noexcept
          : ip_address{
              ipv4{ip_addr, subnet}
        } {}

        explicit constexpr ip_address(ipv4_octets const ip_addr,
                                      ipv4_octet const  prefix = prefix_status(inet_pton4_status::valid)) noexcept
          : ip_address{
              ipv4{ip_addr, prefix}
        } {}

        template <typename CharT>
        constexpr ip_address(ipv4_octets const ip_addr, stl::basic_string_view<CharT> const subnet) noexcept
          : ip_address{
              ipv4{ip_addr, subnet}
        } {}

        constexpr ip_address(ipv4_octets const ip_addr, ipv4_octets const subnet) noexcept
          : ip_address{
              ipv4{ip_addr, subnet}
        } {}

        // NOLINTEND(bugprone-easily-swappable-parameters)


        ////////////////////////////// IPv6 Constructors //////////////////////////////

        explicit constexpr ip_address(
          ipv6::octets8_t const& inp_octets,
          stl::uint8_t const     prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : ip_address{
              ipv6{inp_octets, prefix_value}
        } {}

        explicit constexpr ip_address(
          ipv6::octets16_t const& inp_octets,
          stl::uint8_t const      prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : ip_address{
              ipv6{inp_octets, prefix_value}
        } {}

        explicit constexpr ip_address(
          ipv6::octets32_t const& inp_octets,
          stl::uint8_t const      prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : ip_address{
              ipv6{inp_octets, prefix_value}
        } {}

        explicit constexpr ip_address(
          ipv6::octets64_t const& inp_octets,
          stl::uint8_t const      prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : ip_address{
              ipv6{inp_octets, prefix_value}
        } {}

        ////////////////////////////// Common Functions //////////////////////////////

        [[nodiscard]] constexpr bool operator==(ipv4 const ip_addr) const noexcept {
            return is_v4() && as_v4() == ip_addr;
        }

        [[nodiscard]] constexpr bool operator==(ipv6 const& ip_addr) const noexcept {
            return is_v6() && as_v6() == ip_addr;
        }

        [[nodiscard]] constexpr bool operator==(stl::string_view const ip_str) const noexcept {
            return this->template operator== <char>(ip_str);
        }

        template <typename CharT>
        [[nodiscard]] constexpr bool operator==(stl::basic_string_view<CharT> const ip_str) const noexcept {
            // this implementation works too, but it's not "noexcept":
            //   *this == address{stl::forward<StrT>(ip)};
            ip_address const addr{ip_str};
            if (addr.index() == index()) {
                if (auto const* ip4 = get_if<ipv4>(&as_variant())) {
                    return *ip4 == addr.as_v4();
                }
                if (auto const* ip6 = get_if<ipv6>(&as_variant())) {
                    return *ip6 == addr.as_v6();
                }
            }
            return false;
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(ipv4 const ip_addr) const noexcept {
            if (!is_v4()) {
                return stl::partial_ordering::unordered;
            }
            return as_v4() <=> ip_addr;
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(ipv6 const& ip_addr) const noexcept {
            if (!is_v6()) {
                return stl::partial_ordering::unordered;
            }
            return as_v6() <=> ip_addr;
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(ip_address const& ip_addr) const noexcept {
            if (is_v4()) {
                if (ip_addr.is_v4()) {
                    return as_v4() <=> ip_addr.as_v4();
                }
            } else if (is_v6()) {
                if (ip_addr.is_v6()) {
                    return as_v6() <=> ip_addr.as_v6();
                }
            }
            return stl::partial_ordering::unordered;
        }

        template <typename CharT>
        [[nodiscard]] constexpr stl::partial_ordering operator<=>(
          stl::basic_string_view<CharT> const ip_addr) const noexcept {
            return *this <=> ip_address{ip_addr};
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(stl::string_view const ip_addr) const noexcept {
            return this->operator<=> <char>(ip_addr);
        }

        // Run the specified function/lambda with the right pick
        template <typename Func>
        constexpr decltype(auto) pick(Func&& func) const noexcept(noexcept(func(ipv4{})) && noexcept(func(ipv6{}))) {
            if (auto const* ip_addr_v4 = get_if<ipv4>(&as_variant())) {
                return stl::forward<Func>(func)(*ip_addr_v4);
            }
            return stl::forward<Func>(func)(get<ipv6>(as_variant()));
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
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.has_prefix();
            });
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.is_valid();
            });
        }

        [[nodiscard]] constexpr bool is_zero() const noexcept {
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.is_zero();
            });
        }

        [[nodiscard]] constexpr bool is_broadcast() const noexcept {
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.is_broadcast();
            });
        }

        [[nodiscard]] constexpr bool is_multicast() const noexcept {
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.is_multicast();
            });
        }

        [[nodiscard]] constexpr bool is_nonroutable() const noexcept {
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.is_nonroutable();
            });
        }

        [[nodiscard]] constexpr bool is_private() const noexcept {
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.is_private();
            });
        }

        [[nodiscard]] constexpr bool is_loopback() const noexcept {
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.is_loopback();
            });
        }

        [[nodiscard]] constexpr stl::uint8_t prefix() const noexcept {
            return pick([](auto&& ip_addr) constexpr noexcept {
                return ip_addr.prefix();
            });
        }

        [[nodiscard]] constexpr ip_address_status status() const noexcept {
            auto const prefix_val = prefix();
            // technically if it's ipv4 and the prefix is between 32-128, then it's invalid, but that's
            // not going to happen, so it's unnecessary to check that condition
            if (prefix_val <= ipv6_max_prefix) {
                return ip_address_status::valid;
            }
            return static_cast<ip_address_status>(prefix_val);
        }

        template <typename StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT string(Args&&... args) const {
            return pick([&](auto&& ip_addr) constexpr {
                return ip_addr.template string<StrT>(stl::forward<Args>(args)...);
            });
        }

        constexpr void to_string(istl::String auto& out) const {
            return pick([&](auto&& ip_addr) constexpr {
                return ip_addr.to_string(out);
            });
        }

        template <typename StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT status_string(Args&&... args) const {
            return pick([&](auto&& ip_addr) constexpr {
                return ip_addr.template status_string<StrT>(stl::forward<Args>(args)...);
            });
        }

        constexpr void status_to(istl::String auto& out) const {
            return pick([&](auto&& ip_addr) constexpr {
                return ip_addr.status_to(out);
            });
        }
    };

    template <typename T>
    concept IPAddress = istl::part_of<stl::remove_cvref_t<T>, ipv4, ipv6, ip_address>;

} // namespace webpp

#endif // WEBPP_IP_ADDRESS_HPP
