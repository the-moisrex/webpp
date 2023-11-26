#ifndef WEBPP_HEADERS_HOST_HPP
#define WEBPP_HEADERS_HOST_HPP

#include "../convert/casts.hpp"
#include "../http/codec/tokens.hpp"
#include "../ip/ip_address.hpp"
#include "./domain.hpp"

#include <compare>
#include <cstdint>
#include <variant>

namespace webpp::uri {

    enum struct host_status {
        valid,              // valid but no port
        valid_with_port,    // valid + has port
        invalid_host,       // the host is not a reg-name or an ip addr
        invalid_port,       // unknown error with the port number
        invalid_port_range, // the port is not in the valid port range
        invalid_ipv6        // it's an invalid ipv6 address
    };

    /**
     * HTTP Host Header Field
     * Host authority is a combination of:
     *   - Domain / IPv4 / IPv6
     *   - Port number
     * This can be directly used as a HTTP "Host: " header field's value.
     *
     * Host Field RFC:      https://www.rfc-editor.org/rfc/rfc9110#field.host
     * URI-HOST RFC:        https://www.rfc-editor.org/rfc/rfc3986.html#section-3.2.2
     * WHATWG Host Parsing: https://url.spec.whatwg.org/#host-parsing
     *
     * RFC 1123 allows the first char to be a digit in a domain name as well:
     *  https://www.rfc-editor.org/rfc/rfc1123#section-2
     *
     * Syntax:
     *     Host             = uri-host [ ":" port ]
     *     uri-host         = IP-literal / IPv4address / reg-name
     *     IP-literal       = "[" ( IPv6address / IPvFuture  ) "]"
     *     IPvFuture        = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
     *     IPv6address      =                             6( h16 ":" ) ls32
     *                       /                       "::" 5( h16 ":" ) ls32
     *                       / [               h16 ] "::" 4( h16 ":" ) ls32
     *                       / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
     *                       / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
     *                       / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
     *                       / [ *4( h16 ":" ) h16 ] "::"              ls32
     *                       / [ *5( h16 ":" ) h16 ] "::"              h16
     *                       / [ *6( h16 ":" ) h16 ] "::"
     *
     *     ls32             = ( h16 ":" h16 ) / IPv4address
     *                      ; least-significant 32 bits of address
     *
     *     h16              = 1*4HEXDIG
     *                      ; 16 bits of address represented in hexadecimal
     *
     *     IPv4address      = dec-octet "." dec-octet "." dec-octet "." dec-octet
     *
     *     dec-octet        =  DIGIT                 ; 0-9
     *                       / %x31-39 DIGIT         ; 10-99
     *                       / "1" 2DIGIT            ; 100-199
     *                       / "2" %x30-34 DIGIT     ; 200-249
     *                       / "25" %x30-35          ; 250-255
     *
     *     reg-name         = *( unreserved / pct-encoded / sub-delims )
     */
    struct host_authority {
        using domain_type = domain_name;

        static constexpr stl::uint16_t max_port_number    = 65'535u;
        static constexpr stl::uint16_t default_http_port  = 80u;
        static constexpr stl::uint16_t default_https_port = 443u;

        constexpr ~host_authority() noexcept                                = default;
        constexpr host_authority(host_authority const&) noexcept            = default;
        constexpr host_authority(host_authority&&) noexcept                 = default;
        constexpr host_authority& operator=(host_authority const&) noexcept = default;
        constexpr host_authority& operator=(host_authority&&) noexcept      = default;

        explicit constexpr host_authority(stl::string_view hostname) noexcept {
            if (hostname.empty()) {
                return;
            }

            if (hostname[0] == '[') {
                // IPv6 or invalid
                // If the host starts with a left-bracket, assume the entire host is an
                // IPv6 literal.  Otherwise, assume none of the host is an IPv6 literal.
                // This assumption will be overridden if we find a right-bracket.
                auto ip_addr = hostname;
                ip_addr.remove_prefix(1);
                if (auto const ip_end = stl::find(ip_addr.rbegin(), ip_addr.rend(), ']');
                    ip_end != ip_addr.rend())
                {
                    ip_addr.remove_suffix(static_cast<stl::size_t>(ip_end - ip_addr.rbegin()) + 1);
                    endpoint.emplace<struct ipv6>(ip_addr); // parse and set ipv6
                    status_code            = host_status::valid;
                    auto const bracket_pos = static_cast<stl::size_t>(ip_end.operator->() - hostname.data());
                    parse_port(hostname.data() + bracket_pos + 1, hostname.data() + hostname.size());
                    return;
                }
                status_code = host_status::invalid_ipv6;
                return;
            }

            if (DIGIT<char>.contains(hostname[0])) {
                // might be an IPv4, or it might be a host name

                // first try to parse it as ipv4:
                ipv4_octets octets;
                auto const* host_ptr = hostname.data();
                auto const* host_end = host_ptr + hostname.size(); // NOLINT(*-pro-bounds-pointer-arithmetic)
                switch (inet_pton4(host_ptr, host_end, octets.data(), ':')) {
                    using enum inet_pton4_status;
                    case valid: {
                        // we assumed right, it is an ipv4
                        status_code = host_status::valid;
                        endpoint.emplace<struct ipv4>(octets);
                        return;
                    }
                    case valid_special: {
                        // we might have a port
                        if (':' == *host_ptr) {
                            // doesn't matter if the port was valid or not, we're just going to add it anyway
                            endpoint.emplace<struct ipv4>(octets);

                            parse_port(host_ptr, host_end);
                        } else {
                            // it's not ipv4, let's see if it's a domain or not
                            parse_domain(hostname.data(), hostname.data() + hostname.size());
                        }
                        break;
                    }
                    default: {
                        // it's not ipv4, let's see if it's a domain or not
                        parse_domain(hostname.data(), hostname.data() + hostname.size());
                        return;
                    }
                }
            } else {
                // it is not an IP, it's a host name
                parse_domain(hostname.data(), hostname.data() + hostname.size());
                return;
            }
        }

        [[nodiscard]] constexpr bool has_port() const noexcept {
            return status_code == host_status::valid_with_port;
        }

        [[nodiscard]] constexpr stl::uint16_t port_or(
          stl::uint16_t const default_port = default_http_port) const noexcept {
            return has_port() ? port_value : default_port;
        }

        [[nodiscard]] constexpr stl::uint16_t port() const noexcept {
            return port_value;
        }

        constexpr void status_string(istl::String auto& out) const {
            switch (status_code) {
                using enum host_status;
                case valid:
                case valid_with_port: break;
                case invalid_host: {
                    out += "'";
                    out += endpoint;
                    out += "' is not a valid host name or IP address.";
                    break;
                }
                case invalid_port: {
                    out += "'";
                    out += port_value;
                    out += "' is not a valid port number.";
                    break;
                }
                case invalid_port_range: {
                    out += "Port number '";
                    out += port_value;
                    out += "' should be between 0-65,535 inclusive.";
                    break;
                }
                case invalid_ipv6: {
                    out += "The string '";
                    get<struct ipv6>(endpoint).to_string(out);
                    out += "' is not valid: ";
                    get<struct ipv6>(endpoint).status_to(out);
                    break;
                }
            }
        }

        [[nodiscard]] constexpr host_status status() const noexcept {
            return status_code;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            using enum host_status;
            return status_code == valid || status_code == valid_with_port;
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return is_valid();
        }

        [[nodiscard]] constexpr bool is_ip() const noexcept {
            return is_ipv4() || is_ipv6();
        }

        [[nodiscard]] constexpr bool is_ipv4() const noexcept {
            return stl::holds_alternative<struct ipv4>(endpoint);
        }

        [[nodiscard]] constexpr bool is_ipv6() const noexcept {
            return stl::holds_alternative<struct ipv6>(endpoint);
        }

        [[nodiscard]] constexpr bool is_domain() const noexcept {
            return stl::holds_alternative<domain_type>(endpoint);
        }

        // returns 0.0.0.0 if not found
        [[nodiscard]] constexpr struct ipv4 ipv4() const noexcept {
            if (auto const* ip_ptr = stl::get_if<struct ipv4>(&endpoint); ip_ptr != nullptr) {
                return *ip_ptr;
            }
            return {};
        }

        [[nodiscard]] constexpr struct ipv4 ipv4_or(struct ipv4 default_ip) const noexcept {
            if (auto const* ip_ptr = stl::get_if<struct ipv4>(&endpoint); ip_ptr != nullptr) {
                return *ip_ptr;
            }
            return default_ip;
        }

        // returns ::0 if not found
        [[nodiscard]] constexpr struct ipv6 ipv6() const noexcept {
            if (auto const* ip_ptr = stl::get_if<struct ipv6>(&endpoint); ip_ptr != nullptr) {
                return *ip_ptr;
            }
            return {};
        }

        [[nodiscard]] constexpr struct ipv6 ipv6_or(struct ipv6 default_ip) const noexcept {
            if (auto const* ip_ptr = stl::get_if<struct ipv6>(&endpoint); ip_ptr != nullptr) {
                return *ip_ptr;
            }
            return default_ip;
        }

        [[nodiscard]] constexpr struct ip_address address() const noexcept {
            if (auto const* ip4_ptr = stl::get_if<struct ipv4>(&endpoint); ip4_ptr != nullptr) {
                return {*ip4_ptr};
            }
            if (auto const* ip6_ptr = stl::get_if<struct ipv6>(&endpoint); ip6_ptr != nullptr) {
                return {*ip6_ptr};
            }
            return ip_address::invalid();
        }

        [[nodiscard]] constexpr domain_type domain() const noexcept {
            if (auto const* domain_ptr = get_if<domain_type>(&endpoint); domain_ptr != nullptr) {
                return *domain_ptr;
            }
            return {}; // empty domain
        }

        [[nodiscard]] constexpr domain_type domain_or(domain_type default_domain) const noexcept {
            if (auto const* domain_ptr = stl::get_if<domain_type>(&endpoint); domain_ptr != nullptr) {
                return *domain_ptr;
            }
            return {default_domain};
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(stl::uint16_t rhs_port) const noexcept {
            return port() <=> rhs_port;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(stl::string_view rhs_host) const {
            return *this <=> host_authority{rhs_host};
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(
          host_authority const& rhs_host) const = default;

        [[nodiscard]] explicit constexpr operator struct ipv4() const noexcept {
            return this->ipv4();
        }

        [[nodiscard]] explicit constexpr operator struct ipv6() const noexcept {
            return this->ipv6();
        }

        [[nodiscard]] explicit constexpr operator domain_type() const noexcept {
            return this->domain();
        }


      private:
        constexpr void parse_port(char const* port_ptr, char const* port_end) noexcept {
            if (port_ptr == port_end) {
                return;               // no port here
            }
            if (*port_ptr++ != ':') { // NOLINT(*-pro-bounds-pointer-arithmetic)
                status_code = host_status::invalid_host;
                return;
            }
            int value = 0;
            auto [_, ec]{std::from_chars(port_ptr, port_end, value)};

            if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
                status_code = host_status::invalid_port;
                return;
            }
            if (value < 0 || value > max_port_number) {
                status_code = host_status::invalid_port;
                return;
            }

            port_value  = static_cast<stl::uint16_t>(value);
            status_code = host_status::valid_with_port;
        }

        constexpr void parse_domain(char const* host_ptr, char const* host_end) noexcept {
            auto const* domain_ptr = host_ptr;
            auto const  status     = parse_domain_name(domain_ptr, host_end);
            switch (status) {
                using enum domain_name_status;
                case valid:
                case valid_punycode: {
                    // no port, it's valid
                    endpoint    = domain_type{host_ptr, host_end};
                    status_code = host_status::valid;
                    break;
                }
                case invalid_character: {
                    if (*domain_ptr == ':') {
                        endpoint = domain_type{host_ptr, domain_ptr};
                        // it's a valid domain + (valid/invalid) port
                        parse_port(domain_ptr, host_end);
                    } else {
                        status_code = host_status::invalid_host;
                    }
                    break;
                }
                default: {
                    status_code = host_status::invalid_host;
                }
            }
        }

        using endpoint_variant_type = stl::variant<stl::monostate, struct ipv4, struct ipv6, domain_type>;
        endpoint_variant_type endpoint{stl::monostate{}};
        stl::uint16_t         port_value  = 0;
        host_status           status_code = host_status::invalid_host;
    };

} // namespace webpp::uri

#endif // WEBPP_HEADERS_HOST_HPP
