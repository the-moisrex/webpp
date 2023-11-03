// Created by moisrex on 5/4/23.

#ifndef WEBPP_IP_INET_NTOP_HPP
#define WEBPP_IP_INET_NTOP_HPP

#include "../std/iterator.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/hex.hpp"
#include "ip.hpp"

#include <cstring>


namespace webpp {

    // NOLINTBEGIN(*-pro-bounds-pointer-arithmetic)
    // NOLINTBEGIN(*-magic-numbers)

    // NOLINTBEGIN(*-macro-usage)
    /**
     * Convert an IPv4 to string
     * It's fast, but it's not pretty, I know :)
     */
    template <typename Iter = char*>
    static constexpr Iter inet_ntop4(const stl::uint8_t* src, Iter out) noexcept {
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
        *out = '\0';
        return out;
#undef WEBPP_PUT_CHAR
    }
    // NOLINTEND(*-macro-usage)




    /**
     * Convert IPv6 binary address into presentation (printable) format
     */
    template <typename Iter = char*>
    static constexpr Iter inet_ntop6(const stl::uint8_t* src, Iter out) noexcept {

        using ascii::details::hex_chars;

        using char_type = typename istl::char_type_of_t<typename stl::iterator_traits<Iter>::pointer>;

        if (src == nullptr) {
            return nullptr;
        }

        *out = '\0';

        char_type           hexa[8 * 5];
        char_type*          hex_ptr   = static_cast<char*>(hexa);
        const stl::uint8_t* src_ptr   = src;
        char_type*          octet_ptr = hex_ptr;


        int j             = 0;
        int longest_count = 0;
        int longest_index = -1;

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
            stl::uint8_t hx8 = x8 >> 4U;

            if (hx8 != 0U) {
                skip         = false;
                *octet_ptr++ = hex_chars<char_type>[hx8];
            }

            hx8 = x8 & 0x0FU;
            if (!skip || (hx8 != 0U)) {
                skip         = false;
                *octet_ptr++ = hex_chars<char_type>[hx8];
            }

            x8 = *src_ptr++;

            hx8 = x8 >> 4U;
            if (!skip || (hx8 != 0U)) {
                *octet_ptr++ = hex_chars<char_type>[hx8];
            }

            hx8          = x8 & 0x0FU;
            *octet_ptr++ = hex_chars<char_type>[hx8];
            hex_ptr += 5;



            // find runs of zeros for :: convention
            if (src[i + i] == 0U && src[i + i + 1] == 0U) {
                j++;
                if (j >= longest_count) {
                    longest_index = i - j + 1;
                    longest_count = j;
                }
            } else {
                j = 0;
            }
        }


        if (longest_index == 0) {
            *out++ = ':';

            // check for ipv4-mapped or ipv4-compatible addresses (which is deprecated now)
            // if (longest_count == 6) {
            //     *out++ = ':';
            //     return inet_ntop4(src + 12, out);
            // } else
            if (longest_count == 5 && src[10] == 0xFFU && src[11] == 0xFFU) {
                *out++ = ':';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = 'f';
                *out++ = ':';
                return inet_ntop4(src + 12, out);
            }
        }

        if (longest_index == -1) {
            for (int i = 0; i != 7; ++i) {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                *out++ = ':';
            }
            for (hex_ptr = hexa + 7 * 5; *hex_ptr != '\0'; hex_ptr++) {
                *out++ = *hex_ptr;
            }
        } else {
            int i = 0;
            for (; i != longest_index; ++i) {
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; hex_ptr++) {
                    *out++ = *hex_ptr;
                }
                if (i != 7) {
                    *out++ = ':';
                }
            }
            // check for leading zero
            *out++ = ':';
            i += longest_count;
            for (; i != 8; ++i) {
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

    // NOLINTEND(*-magic-numbers)
    // NOLINTEND(*-pro-bounds-pointer-arithmetic)

} // namespace webpp

#endif // WEBPP_IP_INET_NTOP_HPP
