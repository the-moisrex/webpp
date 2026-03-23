#ifndef WEBPP_IP_VALIDATION_HPP
#define WEBPP_IP_VALIDATION_HPP

#include "../std/concepts.hpp"
#include "../std/string_view.hpp"
#include "../std/utility.hpp"
#include "../strings/charset.hpp"
#include "../strings/validators.hpp"
#include "./inet_pton.hpp"

#include <array>
#include <type_traits>

namespace webpp::is {

    /**
     * Check if the specified Integer is an octet of a subnet mask
     * @tparam Integer
     * @param octet
     * @return
     */
    template <stl::integral Integer>
    [[nodiscard]] constexpr bool subnet_octet(Integer octet) noexcept {
        using uint_type = stl::make_unsigned_t<Integer>;
        webpp_static_constexpr auto mask =
          static_cast<uint_type>(static_cast<uint_type>(1) << ((sizeof(Integer) * CHAR_BIT) - 1));
        while ((static_cast<uint_type>(octet) & mask) == mask) {
            octet <<= 1U;
        }
        return octet == 0U;
    }

    /**
     * Check if the specified string is a valid ipv4 subnet mask or not
     * @param subnet
     * @return bool an indication weather or not the specified string is a
     * valid ipv4 subnet mask or not
     */
    template <typename CharT>
    [[nodiscard]] constexpr bool subnet(stl::basic_string_view<CharT> subnet) noexcept {
        stl::size_t next_dot = 0;
        for (uint8_t octet_index = 0U; octet_index != 4U; octet_index++) {
            next_dot       = subnet.find('.');
            auto octet_str = subnet.substr(0, next_dot);
            if (octet_str.size() > 3UL || !ascii::is::digit(octet_str)) {
                return false;
            }
            if (unsigned const octet_int = to_uint(octet_str); octet_int > 255U || subnet_octet(octet_int)) {
                return false;
            }
            subnet.remove_prefix(octet_str.size() + (octet_index != 3U));
        }
        return subnet.empty();
    }

    /**
     * Check if the specified input is a valid subnet ipv4 mask or not
     * @param octets
     * @return bool an indication weather or not the specified input is a
     * valid ipv4 subnet mask or not
     */
    [[nodiscard]] constexpr bool subnet(stl::array<stl::uint8_t, ipv4_byte_count> const octets) noexcept {
        // std::ranges::all_of
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
    template <typename CharT>
    [[nodiscard]] constexpr bool ipv4(stl::basic_string_view<CharT> const str) noexcept {
        using enum inet_pton4_status;
        auto                                      beg = str.begin();
        stl::array<stl::uint8_t, ipv4_byte_count> out; // NOLINT(*-member-init)
        return inet_pton4(beg, str.end(), out.data()) == valid;
    }

    [[nodiscard]] constexpr bool ipv4(stl::string_view const str) noexcept {
        return ipv4<char>(str);
    }

    /**
     * @brief this function template will check if the ipv4 with its prefix
     * is valid or not.
     * @example 192.168.0.1/24, 192.168.0.1:24
     */
    template <typename CharT>
    [[nodiscard]] constexpr bool ipv4_prefix(stl::basic_string_view<CharT> const str) noexcept {
        using enum inet_pton4_status;
        stl::uint8_t                              prefix_val = stl::to_underlying(valid);
        stl::array<stl::uint8_t, ipv4_byte_count> bin; // NOLINT(*-member-init)
        auto                                      beg = str.begin();
        return is_valid(inet_pton4(beg, str.end(), bin.data(), prefix_val)) && prefix_val <= ipv4_max_prefix;
    }

    [[nodiscard]] constexpr bool ipv4_prefix(stl::string_view const str) noexcept {
        return ipv4_prefix<char>(str);
    }

    /**
     * This function checks to make sure the given address
     * is a valid IPv6 address according to the rules in
     * RFC 3986 (https://tools.ietf.org/html/rfc3986).
     *
     * @param[in] ip_addr
     *     This is the IPv6 address to validate.
     *
     * @return
     *     An indication of whether or not the given address
     *     is a valid IPv6 address is returned.
     */
    template <typename CharT>
    [[nodiscard]] constexpr bool ipv6(stl::basic_string_view<CharT> const ip_addr) noexcept {
        using enum inet_pton6_status;
        auto                                      beg = ip_addr.begin();
        stl::array<stl::uint8_t, ipv6_byte_count> out;              // NOLINT(*-member-init)
        return inet_pton6(beg, ip_addr.end(), out.data()) == valid; // valid_special is not valid here
    }

    [[nodiscard]] constexpr bool ipv6(stl::string_view const ip_addr) noexcept {
        return ipv6<char>(ip_addr);
    }

    template <typename CharT>
    [[nodiscard]] constexpr bool ipv6_prefix(stl::basic_string_view<CharT> const ip_addr) noexcept {
        using enum inet_pton6_status;
        stl::uint8_t                              prefix_val = stl::to_underlying(valid);
        stl::array<stl::uint8_t, ipv6_byte_count> bin; // NOLINT(*-member-init)
        auto                                      beg = ip_addr.begin();
        return is_valid(inet_pton6(beg, ip_addr.end(), bin.data(), prefix_val, '/')) && prefix_val <= ipv6_max_prefix;
    }

    [[nodiscard]] constexpr bool ipv6_prefix(stl::string_view const ip_addr) noexcept {
        return ipv6_prefix<char>(ip_addr);
    }

    /**
     * @brief check if the specified string is an ipv4 or ipv6
     * @param ip_str
     * @return true if str is ipv4 or ipv6
     * TODO: start supporting IPvF (IP version Future)
     */
    template <typename CharT>
    [[nodiscard]] constexpr bool ip(stl::basic_string_view<CharT> const ip_str) noexcept {
        return is::ipv4(ip_str) || ipv6(ip_str);
    }

    [[nodiscard]] constexpr bool ip(stl::string_view const ip_addr) noexcept {
        return ip<char>(ip_addr);
    }

} // namespace webpp::is

#endif // WEBPP_IP_VALIDATION_HPP
