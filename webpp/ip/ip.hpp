// Created by moisrex on 5/5/23.

#ifndef WEBPP_IP_CONSTANTS_HPP
#define WEBPP_IP_CONSTANTS_HPP

#include <cstdint>

namespace webpp {

    // Maximum IPv4 dotted-decimal string (same as INET_ADDRSTRLEN - 1)
    static constexpr auto max_ipv4_str_len = 15u;

    // length of "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"
    // Maximum IPv6 hexadecimal string (same as INET6_ADDRSTRLEN - 1)
    static constexpr auto max_ipv6_str_len = 45u;

    static constexpr auto uint16_byte_count = sizeof(std::uint16_t); // Number of bytes of data in an uint16_t

    // ipv4 octet/byte count
    static constexpr auto ipv4_byte_count = 4u;
    // ipv6 octet/byte count
    static constexpr auto ipv6_byte_count = 16u;

} // namespace webpp

#endif // WEBPP_IP_CONSTANTS_HPP
