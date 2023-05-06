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

    static constexpr const char* hexchars = "0123456789abcdef";

    /**
     * Convert IPv6 binary address into presentation (printable) format
     */
    static constexpr char* inet_ntop6(const stl::uint8_t* src, char* out) noexcept {

        if (src == nullptr)
            return nullptr;

        *out = '\0';

        // check for mapped or compat addresses
        stl::int32_t i = is_v4_mapped(src);
        stl::int32_t j = is_ipv6_address_v4_compat(src);
        if ((i != 0) || (j != 0)) {
            *out++ = ':';
            *out++ = ':';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = 'f';
            *out++ = ':';
            return inet_ntop4(src + 12, out);
        }

        char         hexa[8][5];
        stl::int32_t k = 0;
        for (i = 0; i < 16; i += 2) {
            j         = 0;
            bool skip = true;

            hexa[k][0] = '\0';
            hexa[k][1] = '\0';
            hexa[k][2] = '\0';
            hexa[k][3] = '\0';
            hexa[k][4] = '\0';

            stl::uint8_t x8  = src[i];
            stl::uint8_t hx8 = x8 >> 4;

            if (hx8 != 0) {
                skip         = false;
                hexa[k][j++] = hexchars[hx8];
            }

            hx8 = x8 & 0x0f;
            if (!skip || (hx8 != 0)) {
                skip         = false;
                hexa[k][j++] = hexchars[hx8];
            }

            x8 = src[i + 1];

            hx8 = x8 >> 4;
            if (!skip || (hx8 != 0)) {
                hexa[k][j++] = hexchars[hx8];
            }

            hx8          = x8 & 0x0f;
            hexa[k][j++] = hexchars[hx8];

            k++;
        }

        // find runs of zeros for :: convention
        j = 0;
        int zr[8];
        for (i = 7; i >= 0; i--) {
            zr[i] = j;
            if (src[i + i] == 0 && src[i + i + 1] == 0) {
                j++;
            } else {
                j = 0;
            }
            zr[i] = j;
        }

        // find the longest run of zeros
        k = -1;
        j = 0;
        for (i = 0; i < 8; i++) {
            if (zr[i] > j) {
                k = i;
                j = zr[i];
            }
        }

        for (i = 0; i < 8; i++) {
            if (i != k) {
                zr[i] = 0;
            }
        }

        for (i = 0; i < 8; i++) {
            if (zr[i] != 0) {
                // check for leading zero
                if (i == 0) {
                    *out++ = ':';
                }
                *out++ = ':';
                i += (zr[i] - 1);
                continue;
            }
            for (j = 0; hexa[i][j] != '\0'; j++) {
                *out++ = hexa[i][j];
            }
            if (i != 7) {
                *out++ = ':';
            }
        }
        return out;
    }

    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

} // namespace webpp

#endif // WEBPP_IP_INET_NTOP_HPP
