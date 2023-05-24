#ifndef WEBPP_IP_VALIDATION_HPP
#define WEBPP_IP_VALIDATION_HPP

#include "../std/concepts.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "../strings/validators.hpp"
#include "./inet_pton.hpp"

#include <array>

namespace webpp::is {

    /**
     * Check if the specified Integer is an octet of a subnet mask
     * @tparam Integer
     * @param o
     * @return
     */
    template <stl::integral Integer>
    [[nodiscard]] constexpr bool subnet_octet(Integer o) noexcept {
        constexpr auto mask = static_cast<Integer>(1) << ((sizeof(Integer) * 8) - 1);
        while ((o & mask) == mask)
            o <<= 1;
        return o == 0;
    }


    /**
     * Check if the specified string is a valid ipv4 subnet mask or not
     * @param str
     * @return bool an indication weather or not the specified string is a
     * valid ipv4 subnet mask or not
     */
    [[nodiscard]] constexpr bool subnet(istl::StringViewifiable auto&& _str) noexcept {
        auto        str      = istl::string_viewify(_str);
        stl::size_t next_dot = 0;
        for (uint8_t octet_index = 0u; octet_index != 4u; octet_index++) {
            next_dot       = str.find('.');
            auto octet_str = str.substr(0, next_dot);
            if (octet_str.size() > 3ul || !ascii::is::digit(octet_str)) {
                return false;
            }
            if (auto octet_int = to_uint(octet_str); octet_int > 255u || subnet_octet(octet_int))
                return false;
            str.remove_prefix(octet_str.size() + (octet_index != 3u));
        }
        return str.empty();
    }

    /**
     * Check if the specified input is a valid subnet ipv4 mask or not
     * @param octets
     * @return bool an indication weather or not the specified input is a
     * valid ipv4 subnet mask or not
     */
    [[nodiscard]] constexpr bool subnet(stl::array<uint8_t, 4ul> octets) noexcept {
        for (auto const& octet : octets)
            if (!subnet_octet(octet))
                return false;
        return true;
    }

    /**
     * @brief checks if the specified str is an ipv4
     * @param str
     * @return true if str is a valid ipv4
     */
    [[nodiscard]] constexpr bool ipv4(istl::StringViewifiable auto&& _str) noexcept {
        using enum inet_pton4_status;
        auto const                  str = istl::string_viewify(_str);
        auto                        it  = str.data();
        stl::array<stl::uint8_t, 4> out;
        return inet_pton4(it, it + str.size(), out.data()) == valid;
    }

    /**
     * @brief this function template will check if the ipv4 with it's prefix
     * is valid or not.
     * @example 192.168.0.1/24, 192.168.0.1:24
     */
    [[nodiscard]] constexpr bool ipv4_prefix(istl::StringViewifiable auto&& _str) noexcept {
        using enum inet_pton4_status;
        auto                        str     = istl::string_viewify(_str);
        stl::uint8_t                _prefix = static_cast<stl::uint8_t>(valid);
        stl::array<stl::uint8_t, 4> bin; // NOLINT(cppcoreguidelines-pro-type-member-init)
        return inet_pton4(str.data(), str.data() + str.size(), bin.data(), _prefix) == valid &&
               _prefix <= 32u;
    }

    /**
     * This function checks to make sure the given address
     * is a valid IPv6 address according to the rules in
     * RFC 3986 (https://tools.ietf.org/html/rfc3986).
     *
     * @param[in] address
     *     This is the IPv6 address to validate.
     *
     * @return
     *     An indication of whether or not the given address
     *     is a valid IPv6 address is returned.
     */
    [[nodiscard]] constexpr bool ipv6(istl::StringViewifiable auto&& _address) noexcept {
        using enum inet_pton6_status;
        auto const                    ip = istl::string_viewify(_address);
        auto                          it = ip.data();
        stl::array<stl::uint8_t, 16u> out;
        return inet_pton6(it, it + ip.size(), out.data()) == valid;
    }

    [[nodiscard]] constexpr bool ipv6_prefix(istl::StringViewifiable auto&& _str) noexcept {
        using enum inet_pton6_status;
        auto                          str     = istl::string_viewify(_str);
        stl::uint8_t                  _prefix = static_cast<stl::uint8_t>(valid);
        stl::array<stl::uint8_t, 16u> bin; // NOLINT(cppcoreguidelines-pro-type-member-init)
        return inet_pton6(str.data(), str.data() + str.size(), bin.data(), _prefix) == valid &&
               _prefix <= 128u;
    }

    /**
     * @brief check if the specified string is an ipv4 or ipv6
     * @param str
     * @return true if str is ipv4 or ipv6
     * TODO: start supporting IPvF (IP version Future)
     */
    [[nodiscard]] constexpr bool ip(istl::StringViewifiable auto&& _str) noexcept {
        return is::ipv4(_str) || ipv6(_str);
    }


} // namespace webpp::is

#endif // WEBPP_IP_VALIDATION_HPP
