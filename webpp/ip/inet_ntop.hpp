// Created by moisrex on 5/4/23.

#ifndef WEBPP_IP_INET_NTOP_HPP
#define WEBPP_IP_INET_NTOP_HPP

#include "ip.hpp"

#include <cstring>


namespace webpp {

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

    // NOLINTBEGIN(cppcoreguidelines-macro-usage)
    /**
     * Convert an IPv4 to string
     * It's fast, but it's not pretty, I know :)
     */
    static constexpr char* inet_ntop4(const stl::uint8_t* src, char* out) noexcept {
#define WEBPP_PUT_CHAR()                                   \
    if (*src < 10) {                                       \
        *out++ = static_cast<char>('0' + *src);            \
    } else if (*src < 100) {                               \
        *out++ = static_cast<char>('0' + *src / 10);       \
        *out++ = static_cast<char>('0' + *src % 10);       \
    } else {                                               \
        *out++ = static_cast<char>('0' + *src / 100);      \
        *out++ = static_cast<char>('0' + *src % 100 / 10); \
        *out++ = static_cast<char>('0' + *src % 10);       \
    }
        WEBPP_PUT_CHAR()
        ++src;
        *out++ = '.';
        WEBPP_PUT_CHAR()
        ++src;
        *out++ = '.';
        WEBPP_PUT_CHAR()
        ++src;
        *out++ = '.';
        WEBPP_PUT_CHAR()
        return out;
#undef WEBPP_PUT_CHAR
    }
    // NOLINTEND(cppcoreguidelines-macro-usage)


    /**
     * Determine whether the address is a mapped IPv4 address
     * @return bool
     */
    [[nodiscard]] constexpr bool is_v4_mapped(const stl::uint8_t* octets) noexcept {
        return (octets[0] == 0) && (octets[1] == 0) && (octets[2] == 0) && (octets[3] == 0) &&
               (octets[4] == 0) && (octets[5] == 0) && (octets[6] == 0) && (octets[7] == 0) &&
               (octets[8] == 0) && (octets[9] == 0) && (octets[10] == 0xff) && (octets[11] == 0xff);
    }

    /**
     * Determine whether the address is compatible with ipv4
     * @return bool
     */
    [[nodiscard]] constexpr bool is_ipv6_address_v4_compat(const stl::uint8_t* octets) noexcept {
        return (octets[0] == 0x00) && (octets[1] == 0x00) && (octets[2] == 0x00) && (octets[3] == 0x00) &&
               (octets[4] == 0x00) && (octets[5] == 0x00) && (octets[6] == 0x00) && (octets[7] == 0x00) &&
               (octets[8] == 0x00) && (octets[9] == 0x00) && (octets[10] == 0xff) && (octets[11] == 0xff);
    }


    /**
     * Convert IPv6 binary address into presentation (printable) format
     */
    static constexpr char* inet_ntop6(const stl::uint8_t* src, char* out) noexcept {
        IF_CXX23(static) constexpr const char* hex_chars = "0123456789abcdef";

        if (!src) {
            return nullptr;
        }

        *out = '\0';

        // check for mapped or compat addresses
        if (is_v4_mapped(src) || is_ipv6_address_v4_compat(src)) {
            *out++ = ':';
            *out++ = ':';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = ':';
            return inet_ntop4(src + 12, out);
        }

        char                hexa[8 * 5];
        char*               hex_ptr   = hexa;
        const stl::uint8_t* src_ptr   = src;
        char*               octet_ptr = hex_ptr;
        for (int i = 0; i != 8; ++i) {
            bool skip = true;

            octet_ptr    = hex_ptr;
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            *octet_ptr++ = '\0';
            octet_ptr    = hex_ptr;

            stl::uint8_t x8  = *src_ptr++;
            stl::uint8_t hx8 = x8 >> 4u;

            if (hx8 != 0u) {
                skip         = false;
                *octet_ptr++ = hex_chars[hx8];
            }

            hx8 = x8 & 0x0fu;
            if (!skip || (hx8 != 0u)) {
                skip         = false;
                *octet_ptr++ = hex_chars[hx8];
            }

            x8 = *src_ptr++;

            hx8 = x8 >> 4u;
            if (!skip || (hx8 != 0u)) {
                *octet_ptr++ = hex_chars[hx8];
            }

            hx8          = x8 & 0x0fu;
            *octet_ptr++ = hex_chars[hx8];
            hex_ptr += 5;
        }

        // find runs of zeros for :: convention
        int j             = 0;
        int longest_count = 0;
        int longest_index = -1;
        for (stl::int32_t i = 7; i >= 0; i--) {
            if (src[i + i] == 0 && src[i + i + 1] == 0) {
                j++;
                if (j > longest_count) {
                    longest_index = i;
                    longest_count = j;
                }
            } else {
                j = 0;
            }
        }

        for (int i = 0; i != 8; ++i) {
            if (i == longest_index) {
                // check for leading zero
                if (i == 0) {
                    *out++ = ':';
                }
                *out++ = ':';
                i += longest_count - 1;
            } else {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                if (i != 7) {
                    *out++ = ':';
                }
            }
        }

        *out = '\0';
        return out;
    }

    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

} // namespace webpp

#endif // WEBPP_IP_INET_NTOP_HPP
