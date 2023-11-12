#ifndef WEBPP_IP_VALIDATION_HPP
#define WEBPP_IP_VALIDATION_HPP

#include "../std/concepts.hpp"
#include "../std/string_view.hpp"
#include "../std/utility.hpp"
#include "../strings/charset.hpp"
#include "../strings/validators.hpp"
#include "./inet_pton.hpp"

#include <array>

namespace webpp::is {

    /**
     * Check if the specified Integer is an octet of a subnet mask
     * @tparam Integer
     * @param octet
     * @return
     */
    template <stl::integral Integer>
    [[nodiscard]] constexpr bool subnet_octet(Integer octet) noexcept {
        webpp_static_constexpr auto mask = static_cast<Integer>(1) << ((sizeof(Integer) * 8) - 1);
        while ((octet & mask) == mask) {
            octet <<= 1U;
        }
        return octet == 0U;
    }


    /**
     * Check if the specified string is a valid ipv4 subnet mask or not
     * @param str
     * @return bool an indication weather or not the specified string is a
     * valid ipv4 subnet mask or not
     */
    template <istl::StringViewifiable StrV = stl::string_view>
    [[nodiscard]] constexpr bool subnet(StrV&& subnet_str) noexcept {
        auto        str      = istl::string_viewify(stl::forward<StrV>(subnet_str));
        stl::size_t next_dot = 0;
        for (uint8_t octet_index = 0U; octet_index != 4U; octet_index++) {
            next_dot       = str.find('.');
            auto octet_str = str.substr(0, next_dot);
            if (octet_str.size() > 3UL || !ascii::is::digit(octet_str)) {
                return false;
            }
            if (auto octet_int = to_uint(octet_str); octet_int > 255U || subnet_octet(octet_int)) {
                return false;
            }
            str.remove_prefix(octet_str.size() + (octet_index != 3U));
        }
        return str.empty();
    }

    /**
     * Check if the specified input is a valid subnet ipv4 mask or not
     * @param octets
     * @return bool an indication weather or not the specified input is a
     * valid ipv4 subnet mask or not
     */
    [[nodiscard]] constexpr bool subnet(stl::array<stl::uint8_t, ipv4_byte_count> octets) noexcept {
        for (auto const& octet : octets) {
            if (!subnet_octet(octet)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief checks if the specified str is an ipv4
     * @param str
     * @return true if str is a valid ipv4
     */
    template <istl::StringViewifiable StrV = stl::string_view>
    [[nodiscard]] constexpr bool ipv4(StrV&& inp_str) noexcept {
        using enum inet_pton4_status;
        auto const                                str = istl::string_viewify(stl::forward<StrV>(inp_str));
        auto                                      beg = str.begin();
        stl::array<stl::uint8_t, ipv4_byte_count> out; // NOLINT(*-member-init)
        return inet_pton4(beg, str.end(), out.data()) == valid;
    }

    /**
     * @brief this function template will check if the ipv4 with it's prefix
     * is valid or not.
     * @example 192.168.0.1/24, 192.168.0.1:24
     */
    template <istl::StringViewifiable StrV = stl::string_view>
    [[nodiscard]] constexpr bool ipv4_prefix(StrV&& inp_str) noexcept {
        using enum inet_pton4_status;
        auto                                      str = istl::string_viewify(stl::forward<StrV>(inp_str));
        stl::uint8_t                              prefix_val = stl::to_underlying(valid);
        stl::array<stl::uint8_t, ipv4_byte_count> bin; // NOLINT(*-member-init)
        auto                                      beg = str.begin();
        return is_valid(inet_pton4(beg, str.end(), bin.data(), prefix_val)) && prefix_val <= ipv4_max_prefix;
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
    template <istl::StringViewifiable StrV = stl::string_view>
    [[nodiscard]] constexpr bool ipv6(StrV&& ip_addr) noexcept {
        using enum inet_pton6_status;
        auto const                                ip_str = istl::string_viewify(stl::forward<StrV>(ip_addr));
        auto                                      beg    = ip_str.begin();
        stl::array<stl::uint8_t, ipv6_byte_count> out;             // NOLINT(*-member-init)
        return inet_pton6(beg, ip_str.end(), out.data()) == valid; // valid_special is not valid here
    }

    template <istl::StringViewifiable StrV = stl::string_view>
    [[nodiscard]] constexpr bool ipv6_prefix(StrV&& ip_addr) noexcept {
        using enum inet_pton6_status;
        auto                                      str = istl::string_viewify(stl::forward<StrV>(ip_addr));
        stl::uint8_t                              prefix_val = stl::to_underlying(valid);
        stl::array<stl::uint8_t, ipv6_byte_count> bin; // NOLINT(*-member-init)
        auto                                      beg = str.begin();
        return is_valid(inet_pton6(beg, str.end(), bin.data(), prefix_val, '/')) &&
               prefix_val <= ipv6_max_prefix;
    }

    /**
     * @brief check if the specified string is an ipv4 or ipv6
     * @param str
     * @return true if str is ipv4 or ipv6
     * TODO: start supporting IPvF (IP version Future)
     */
    template <istl::StringViewifiable StrV = stl::string_view>
    [[nodiscard]] constexpr bool ip(StrV&& ip_str) noexcept {
        const auto str = istl::string_viewify(stl::forward<StrV>(ip_str));
        return is::ipv4(str) || ipv6(str);
    }


} // namespace webpp::is

#endif // WEBPP_IP_VALIDATION_HPP
