// Created by moisrex on 5/5/23.

#ifndef WEBPP_IP_CONSTANTS_HPP
#define WEBPP_IP_CONSTANTS_HPP

#include <cstdint>

namespace webpp {

    static constexpr auto max_ipv4_str_len = 16u;
    static constexpr auto max_ipv6_str_len = 64u;

    static constexpr auto uint16_byte_count = sizeof(std::uint16_t); // Number of bytes of data in an uint16_t
    static constexpr auto ipv4_byte_count   = 4u;
    static constexpr auto ipv6_byte_count   = 16u;



} // namespace webpp

#endif // WEBPP_IP_CONSTANTS_HPP
