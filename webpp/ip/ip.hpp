// Created by moisrex on 5/5/23.

#ifndef WEBPP_IP_CONSTANTS_HPP
#define WEBPP_IP_CONSTANTS_HPP

#include "../std/utility.hpp"

#include <cstdint>

namespace webpp {

    // Maximum IPv4 dotted-decimal string (same as INET_ADDRSTRLEN - 1)
    static constexpr auto max_ipv4_str_len = 15U;

    // length of "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"
    // Maximum IPv6 hexadecimal string (same as INET6_ADDRSTRLEN - 1)
    static constexpr auto max_ipv6_str_len = 45U;

    static constexpr auto uint16_byte_count = sizeof(std::uint16_t); // Number of bytes of data in an uint16_t

    // ipv4 octet/byte count
    static constexpr auto ipv4_byte_count = 4U;
    // ipv6 octet/byte count
    static constexpr auto ipv6_byte_count = 16U;

    static constexpr std::uint8_t ipv4_max_prefix = 32U;
    static constexpr std::uint8_t ipv6_max_prefix = 128U;

    /**
     * This is the combination of both of ipv4 and ipv6 parsing status enums:
     *   - inet_pton4_status
     *   - inet_pton6_status
     *
     * Attention: keep the underlying type of this enum match the uri_status' underlying type, they are
     * convertible
     */
    enum struct ip_address_status : stl::uint_fast16_t { // NOLINT(*-enum-size)
        valid             = 255U,
        valid_special     = 254U, // valid ip, found the specified special character at the end (prefix, ...)
        too_little_octets = 253U, // not enough octets
        too_many_octets   = 252U, // found too many octets
        invalid_octet_range  = 251U, // at least one octet is not in range
        invalid_leading_zero = 250U, // the octet is starting with an invalid leading zero
        invalid_character    = 249U, // found a non-standard character
        bad_ending           = 248U, // The ip ended badly
        invalid_prefix       = 247U, // The ip has and invalid prefix
        invalid_colon_usage  = 246U  // the ip is using colon where it shouldn't
    };

    /**
     * Status of the result of a ipv4 parse.
     * The numbers stated for the values of the states are designed to be used in a uint8_t and still be
     * able to use that uint8_t for a ipv4/ipv6 prefix (which only requires 0-128)
     */
    enum struct inet_pton4_status : stl::uint_fast16_t { // NOLINT(*-enum-size)
        valid = stl::to_underlying(ip_address_status::valid),

        // valid ipv4, special character found at the end
        valid_special = stl::to_underlying(ip_address_status::valid_special),

        // not enough octets
        too_little_octets = stl::to_underlying(ip_address_status::too_little_octets),

        // found too many octets
        too_many_octets = stl::to_underlying(ip_address_status::too_many_octets),

        // at least one octet is not in range
        invalid_octet_range = stl::to_underlying(ip_address_status::invalid_octet_range),

        // the octet is starting with an invalid leading zero
        invalid_leading_zero = stl::to_underlying(ip_address_status::invalid_leading_zero),

        // found a non-standard character
        invalid_character = stl::to_underlying(ip_address_status::invalid_character),

        // The ip ended badly
        bad_ending = stl::to_underlying(ip_address_status::bad_ending),

        // The ip has and invalid prefix
        invalid_prefix = stl::to_underlying(ip_address_status::invalid_prefix)
    };

    /**
     * Status of the result of a ipv6 parse.
     * The numbers stated for the values of the states are designed to be used in a uint8_t and still be
     * able to use that uint8_t for a ipv4/ipv6 prefix (which only requires 0-128)
     */
    enum struct inet_pton6_status : stl::uint_fast16_t { // NOLINT(*-enum-size)
        valid = stl::to_underlying(ip_address_status::valid),

        // valid ipv6, special character found at the end
        valid_special = stl::to_underlying(ip_address_status::valid_special),

        // found a non-standard character
        invalid_character = stl::to_underlying(ip_address_status::invalid_character),

        // The ip ended badly
        bad_ending = stl::to_underlying(ip_address_status::bad_ending),

        // The ip has and invalid prefix
        invalid_prefix = stl::to_underlying(ip_address_status::invalid_prefix),

        // at least one octet is not in range
        invalid_octet_range = stl::to_underlying(ip_address_status::invalid_octet_range),

        // the ip is using colon where it shouldn't
        invalid_colon_usage = stl::to_underlying(ip_address_status::invalid_colon_usage)
    };


    [[nodiscard]] static constexpr bool is_valid(inet_pton4_status status) noexcept {
        using enum inet_pton4_status;
        return status == valid || status == valid_special;
    }

    [[nodiscard]] static constexpr bool is_valid(inet_pton6_status status) noexcept {
        using enum inet_pton6_status;
        return status == valid || status == valid_special;
    }

    [[nodiscard]] static constexpr bool is_valid(ip_address_status status) noexcept {
        using enum ip_address_status;
        return status == valid || status == valid_special;
    }



} // namespace webpp

#endif // WEBPP_IP_CONSTANTS_HPP
