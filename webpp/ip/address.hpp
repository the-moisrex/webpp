#ifndef WEBPP_IP_ADDRESS_HPP
#define WEBPP_IP_ADDRESS_HPP

#include "ipv4.hpp"
#include "ipv6.hpp"

#include <compare>
#include <variant>

namespace webpp {

    /**
     * This is the combination of both of ipv4 and ipv6 parsing status enums:
     *   - inet_pton4_status
     *   - inet_pton6_status
     */
    enum struct ip_address_status : stl::uint_fast8_t {
        valid                = 255u,
        too_little_octets    = 254u, // not enough octets
        too_many_octets      = 253u, // found too many octets
        invalid_octet_range  = 252u, // at least one octet is not in range
        invalid_leading_zero = 251u, // the octet is starting with an invalid leading zero
        invalid_character    = 250u, // found a non-standard character
        bad_ending           = 249u, // The ip ended badly
        invalid_octet        = 248u, // Found an invalid character in the octets
        invalid_prefix       = 247u, // The ip has and invalid prefix
        invalid_colon_usage  = 246u  // the ip is using colon where it shouldn't
    };



    /**
     * Represents an IPv4 or IPv6
     */
    struct address : stl::variant<ipv4, ipv6> {
        using variant_type = stl::variant<ipv4, ipv6>;

        // variant ctor
        using stl::variant<ipv4, ipv6>::variant;

      private:
        constexpr void parse(stl::string_view ip) noexcept {
            // first, let's try parsing it as an ipv4 address
            if (ipv4 ip4{ip}; ip4.is_valid()) {
                // it's ipv4
                this->emplace<ipv4>(ip4);
            } else if (ip4.status() == inet_pton4_status::invalid_octet) {
                this->emplace<ipv6>(ip);
            } else {
                this->emplace<ipv4>(ip4);
            }
        }

      public:
        ////////////////////////////// Common Constructors //////////////////////////////

        template <istl::StringViewifiable StrT>
        constexpr address(StrT&& ip) noexcept {
            parse(istl::string_viewify(stl::forward<StrT>(ip)));
        }

        template <istl::StringViewifiable StrT>
        constexpr address& operator=(StrT&& ip) noexcept {
            parse(istl::string_viewify(stl::forward<StrT>(ip)));
            return *this;
        }

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

        [[nodiscard]] constexpr bool operator==(ipv4 ip) const noexcept {
            return is_v4() && as_v4() == ip;
        }

        [[nodiscard]] constexpr bool operator==(ipv6 ip) const noexcept {
            return is_v6() && as_v6() == ip;
        }

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr bool operator==(StrT&& ip) const noexcept {
            return *this == address{stl::forward<StrT>(ip)};
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(ipv4 ip) const noexcept {
            if (!is_v4()) {
                return stl::partial_ordering::unordered;
            }
            return as_v4() <=> ip;
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(ipv6 ip) const noexcept {
            if (!is_v6()) {
                return stl::partial_ordering::unordered;
            }
            return as_v6() <=> ip;
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(address const& ip) const noexcept {
            if (is_v4()) {
                if (ip.is_v4()) {
                    return as_v4() <=> ip.as_v4();
                }
            } else if (is_v6()) {
                if (ip.is_v6()) {
                    return as_v6() <=> ip.as_v6();
                }
            }
            return stl::partial_ordering::unordered;
        }

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr stl::partial_ordering operator<=>(StrT&& ip) const noexcept {
            return *this <=> address{stl::forward<StrT>(ip)};
        }

        // Run the specified function/lambda with the right pick
        template <typename Func>
        constexpr auto pick(Func&& func) const noexcept(noexcept(func(ipv4{})) && noexcept(func(ipv6{}))) {
            if (auto* v4 = get_if<ipv4>(&as_variant())) {
                return func(*v4);
            } else {
                return func(get<ipv6>(as_variant()));
            }
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

        [[nodiscard]] constexpr stl::uint8_t prefix() const noexcept {
            return pick([](auto&& ip) constexpr noexcept -> stl::uint8_t {
                return ip.prefix();
            });
        }

        [[nodiscard]] constexpr ip_address_status status() const noexcept {
            auto const prefix_val = prefix();
            if (prefix_val <= 128) {
                return ip_address_status::valid;
            }
            return static_cast<ip_address_status>(prefix_val);
        }

        template <typename StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT string(Args&&... args) const {
            return pick([&](auto&& ip) constexpr {
                return ip.template string<StrT>(stl::forward<Args>(args)...);
            });
        }

        constexpr void to_string(istl::String auto& out) const {
            return pick([&](auto&& ip) constexpr {
                return ip.to_string(out);
            });
        }

        template <typename StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT status_string(Args&&... args) const {
            return pick([&](auto&& ip) constexpr {
                return ip.template status_string<StrT>(stl::forward<Args>(args)...);
            });
        }

        constexpr void status_to(istl::String auto& out) const {
            return pick([&](auto&& ip) constexpr {
                return ip.status_to(out);
            });
        }
    };
} // namespace webpp

#endif // WEBPP_IP_ADDRESS_HPP
