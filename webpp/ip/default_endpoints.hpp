// Created by moisrex on 6/14/23.

#ifndef WEBPP_DEFAULT_ENDPOINTS_HPP
#define WEBPP_DEFAULT_ENDPOINTS_HPP

#include "./endpoint.hpp"

namespace webpp {

    static constexpr ipv4 ipv4_localhost{127u, 0u, 0u, 1u};
    static constexpr ipv6 ipv6_localhost{
      {0ull, 1ull}
    }; // ::1

    static constexpr ipv4 ipv4_all{0u, 0u, 0u, 0u};
    static constexpr ipv6 ipv6_all{
      {0ull, 0ull}
    }; // ::

#ifdef DEBUG
    static constexpr stl::uint16_t default_http_port  = 8080u;
    static constexpr stl::uint16_t default_https_port = 8443u;
#else
    static constexpr stl::uint16_t default_http_port  = 80u;
    static constexpr stl::uint16_t default_https_port = 443u;
#endif

    /**
     * Return Standard Endpoints and limit them based on the specified arguments.
     * This is a little utility to quickly get the endpoints that you need your server to start binding to.
     */
    [[nodiscard]] consteval auto
    standard_endpoints(ip_endpoint::protocol proto, auto ip, stl::uint16_t port) noexcept {
        return stl::array{
          ip_endpoint{proto, ip, port}
        };
    }

    [[nodiscard]] consteval auto standard_endpoints(ip_endpoint::protocol proto,
                                                    IPAddress auto&&... ip) noexcept {
        return stl::array<ip_endpoint, sizeof...(ip) * 2>{
          ip_endpoint{proto, ip,  default_http_port}
          ...,
          {proto, ip, default_https_port}
          ...
        };
    }

    [[nodiscard]] consteval auto standard_endpoints(stl::uint16_t port, IPAddress auto&&... ip) noexcept {
        return stl::array<ip_endpoint, sizeof...(ip) * 2>{
          ip_endpoint{ip_endpoint::tcp, ip, port}
          ...,
          {ip_endpoint::udp, ip, port}
          ...
        };
    }

    [[nodiscard]] consteval auto standard_endpoints(IPAddress auto&&... ip) noexcept {
        return stl::array<ip_endpoint, sizeof...(ip) * 4>{
          ip_endpoint{ip_endpoint::tcp, ip,  default_http_port}
          ...,
          {ip_endpoint::udp, ip,  default_http_port}
          ...,
          {ip_endpoint::tcp, ip, default_https_port}
          ...,
          {ip_endpoint::udp, ip,  default_http_port}
          ...
        };
    }

    [[nodiscard]] consteval auto standard_endpoints(ip_endpoint::protocol proto) noexcept {
        return stl::array<ip_endpoint, 4>{
          ip_endpoint{proto, ipv4_all,  default_http_port},
          {proto, ipv4_all, default_https_port},
          {proto, ipv6_all,  default_http_port},
          {proto, ipv6_all, default_https_port}
        };
    }

    /**
     * All possible standard endpoints:
     *   - TCP:80  -> http
     *   - TCP:443 -> https up to HTTP/2
     *   - UDP:80  -> HTTP/3 using QUIC (most-likely not going to be used by standard browsers)
     *   - UDP:443 -> HTTP/3 using QUIC with SSL/TLS
     */
    [[nodiscard]] consteval auto standard_endpoints() noexcept {
        return stl::array<ip_endpoint, 8>{
          ip_endpoint{ip_endpoint::tcp, ipv4_all,  default_http_port},
          {ip_endpoint::tcp, ipv4_all, default_https_port},
          {ip_endpoint::tcp, ipv6_all,  default_http_port},
          {ip_endpoint::tcp, ipv6_all, default_https_port},
          {ip_endpoint::udp, ipv4_all,  default_http_port},
          {ip_endpoint::udp, ipv4_all, default_https_port},
          {ip_endpoint::udp, ipv6_all,  default_http_port},
          {ip_endpoint::udp, ipv6_all, default_https_port},
        };
    }

    namespace tcp {
        // http port
        static constexpr ip_endpoint ipv4_localhost_http_endpoint{
          ip_endpoint::tcp,
          ipv4_localhost,
          default_http_port};
        static constexpr ip_endpoint ipv6_localhost_http_endpoint{
          ip_endpoint::tcp,
          ipv6_localhost,
          default_http_port};

        // https port
        static constexpr ip_endpoint ipv4_localhost_https_endpoint{
          ip_endpoint::tcp,
          ipv4_localhost,
          default_https_port};
        static constexpr ip_endpoint ipv6_localhost_https_endpoint{
          ip_endpoint::tcp,
          ipv6_localhost,
          default_https_port};

        // http
        static constexpr ip_endpoint ipv6_all_http_endpoint{ip_endpoint::tcp, ipv6_all, default_http_port};
        static constexpr ip_endpoint ipv4_all_http_endpoint{ip_endpoint::tcp, ipv4_all, default_http_port};

        // https
        static constexpr ip_endpoint ipv6_all_https_endpoint{ip_endpoint::tcp, ipv6_all, default_https_port};
        static constexpr ip_endpoint ipv4_all_https_endpoint{ip_endpoint::tcp, ipv4_all, default_https_port};


        static constexpr ip_endpoint_array<2> ipv6_all_endpoint{ipv6_all_https_endpoint,
                                                                ipv6_all_http_endpoint};
        static constexpr ip_endpoint_array<2> ipv4_all_endpoint{ipv4_all_https_endpoint,
                                                                ipv4_all_http_endpoint};
        static constexpr ip_endpoint_array<4> all_endpoint{
          ipv4_all_https_endpoint,
          ipv4_all_http_endpoint,
          ipv6_all_https_endpoint,
          ipv6_all_http_endpoint};

        static constexpr ip_endpoint_array<2> ipv6_localhost_endpoint{ipv6_localhost_https_endpoint,
                                                                      ipv6_localhost_http_endpoint};
        static constexpr ip_endpoint_array<2> ipv4_localhost_endpoint{ipv4_localhost_https_endpoint,
                                                                      ipv4_localhost_http_endpoint};
        static constexpr ip_endpoint_array<4> localhost_endpoint{
          ipv4_localhost_https_endpoint,
          ipv4_localhost_http_endpoint,
          ipv6_localhost_https_endpoint,
          ipv6_localhost_http_endpoint};

    } // namespace tcp

    namespace udp {
        // http port
        static constexpr ip_endpoint ipv4_localhost_http_endpoint{
          ip_endpoint::udp,
          ipv4_localhost,
          default_http_port};
        static constexpr ip_endpoint ipv6_localhost_http_endpoint{
          ip_endpoint::udp,
          ipv6_localhost,
          default_http_port};

        // https port
        static constexpr ip_endpoint ipv4_localhost_https_endpoint{
          ip_endpoint::udp,
          ipv4_localhost,
          default_https_port};
        static constexpr ip_endpoint ipv6_localhost_https_endpoint{
          ip_endpoint::udp,
          ipv6_localhost,
          default_https_port};

        // http
        static constexpr ip_endpoint ipv6_all_http_endpoint{ip_endpoint::udp, ipv6_all, default_http_port};
        static constexpr ip_endpoint ipv4_all_http_endpoint{ip_endpoint::udp, ipv4_all, default_http_port};

        // https
        static constexpr ip_endpoint ipv6_all_https_endpoint{ip_endpoint::udp, ipv6_all, default_https_port};
        static constexpr ip_endpoint ipv4_all_https_endpoint{ip_endpoint::udp, ipv4_all, default_https_port};


        static constexpr ip_endpoint_array<2> ipv6_all_endpoint{ipv6_all_https_endpoint,
                                                                ipv6_all_http_endpoint};
        static constexpr ip_endpoint_array<2> ipv4_all_endpoint{ipv4_all_https_endpoint,
                                                                ipv4_all_http_endpoint};
        static constexpr ip_endpoint_array<4> all_endpoint{
          ipv4_all_https_endpoint,
          ipv4_all_http_endpoint,
          ipv6_all_https_endpoint,
          ipv6_all_http_endpoint};

        static constexpr ip_endpoint_array<2> ipv6_localhost_endpoint{ipv6_localhost_https_endpoint,
                                                                      ipv6_localhost_http_endpoint};
        static constexpr ip_endpoint_array<2> ipv4_localhost_endpoint{ipv4_localhost_https_endpoint,
                                                                      ipv4_localhost_http_endpoint};
        static constexpr ip_endpoint_array<4> localhost_endpoint{
          ipv4_localhost_https_endpoint,
          ipv4_localhost_http_endpoint,
          ipv6_localhost_https_endpoint,
          ipv6_localhost_http_endpoint};

    } // namespace udp

    static constexpr auto localhost_endpoints = standard_endpoints(ipv4_localhost, ipv6_localhost);
    static constexpr auto all_endpoints       = standard_endpoints();

} // namespace webpp

#endif // WEBPP_DEFAULT_ENDPOINTS_HPP
