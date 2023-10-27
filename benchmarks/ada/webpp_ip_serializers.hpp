// Created by moisrex on 5/4/23.

#ifndef WEBPP_BENCH_IP_INET_NTOP_HPP
#define WEBPP_BENCH_IP_INET_NTOP_HPP

#include <concepts>
#include <cstdint>
#include <cstring>
#include <utility>


namespace webpp::v1 {

    /**
     * This is almost has the same idea as C++23's std::string::resize_and_overwrite, but it appends the
     * string to the end of the string instead of overwriting the whole thing
     */
    template <typename Op>
    constexpr void
    resize_and_append(auto& out, std::size_t counts, Op&& op) noexcept(noexcept(op(out.data()))) {
        auto const str_size = out.size();
        out.resize(out.size() + counts); // unfortunately resize writes zeros!!
        auto const ret    = std::forward<Op>(op)(out.data() + str_size);
        using return_type = std::remove_cvref_t<decltype(ret)>;
        if constexpr (std::is_integral_v<return_type>) {
            out.erase(ret);
        } else {
            out.erase(static_cast<std::size_t>(ret - out.data()));
        }
    }

    // Maximum IPv4 dotted-decimal string (same as INET_ADDRSTRLEN)
    static constexpr auto max_ipv4_str_len = 15u;

    // sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"
    // Maximum IPv6 hexadecimal string (same as INET6_ADDRSTRLEN)
    static constexpr auto max_ipv6_str_len = 45u;

    static constexpr auto uint16_byte_count = sizeof(std::uint16_t); // Number of bytes of data in an uint16_t

    // ipv4 octet/byte count
    static constexpr auto ipv4_byte_count = 4u;
    // ipv6 octet/byte count
    static constexpr auto ipv6_byte_count = 16u;

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    // NOLINTBEGIN(*-magic-numbers)

    // NOLINTBEGIN(cppcoreguidelines-macro-usage)
    /**
     * Convert an IPv4 to string
     * It's fast, but it's not pretty, I know :)
     */
    static char* inet_ntop4(const std::uint8_t* src, char* out) noexcept {
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
    // NOLINTEND(cppcoreguidelines-macro-usage)



    namespace details {
        static constexpr const char* hex_chars = "0123456789abcdef";
    }

    /**
     * Convert IPv6 binary address into presentation (printable) format
     */
    static char* inet_ntop6(const std::uint8_t* src, char* out) noexcept {

        if (!src) {
            return nullptr;
        }

        *out = '\0';

        char                hexa[8 * 5];
        char*               hex_ptr   = hexa;
        const std::uint8_t* src_ptr   = src;
        char*               octet_ptr = hex_ptr;


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

            std::uint8_t x8  = *src_ptr++;
            std::uint8_t hx8 = x8 >> 4u;

            if (hx8 != 0u) {
                skip         = false;
                *octet_ptr++ = details::hex_chars[hx8];
            }

            hx8 = x8 & 0x0fu;
            if (!skip || (hx8 != 0u)) {
                skip         = false;
                *octet_ptr++ = details::hex_chars[hx8];
            }

            x8 = *src_ptr++;

            hx8 = x8 >> 4u;
            if (!skip || (hx8 != 0u)) {
                *octet_ptr++ = details::hex_chars[hx8];
            }

            hx8          = x8 & 0x0fu;
            *octet_ptr++ = details::hex_chars[hx8];
            hex_ptr += 5;



            // find runs of zeros for :: convention
            if (src[i + i] == 0u && src[i + i + 1] == 0u) {
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
            if (longest_count == 5 && src[10] == 0xffu && src[11] == 0xffu) {
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
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

} // namespace webpp::v1

namespace webpp::v2 {

    /**
     * Convert an IPv4 to string
     * It's fast, but it's not pretty, I know :)
     */
    static char* inet_ntop4(const std::uint8_t* src, char* out) noexcept {
#define WEBPP_PUT_CHAR(inp)                               \
    if (*src < 10) {                                      \
        *out++ = static_cast<char>('0' + inp);            \
    } else if (inp < 100) {                               \
        *out++ = static_cast<char>('0' + inp / 10);       \
        *out++ = static_cast<char>('0' + inp % 10);       \
    } else {                                              \
        *out++ = static_cast<char>('0' + inp / 100);      \
        *out++ = static_cast<char>('0' + inp % 100 / 10); \
        *out++ = static_cast<char>('0' + inp % 10);       \
    }
        WEBPP_PUT_CHAR(src[0])
        *out++ = '.';
        WEBPP_PUT_CHAR(src[1])
        *out++ = '.';
        WEBPP_PUT_CHAR(src[2])
        *out++ = '.';
        WEBPP_PUT_CHAR(src[3])
        *out = '\0';
        return out;
#undef WEBPP_PUT_CHAR
    }
    // NOLINTEND(cppcoreguidelines-macro-usage)

} // namespace webpp::v2

namespace webpp::v3 {

    /**
     * Convert an IPv4 to string
     * It's fast, but it's not pretty, I know :)
     */
    static char* inet_ntop4(const std::uint8_t* src, char* out) noexcept {
#define WEBPP_PUT_CHAR(inp) out = std::to_chars(out, out + 3, inp).ptr;

        WEBPP_PUT_CHAR(src[0])
        *out++ = '.';
        WEBPP_PUT_CHAR(src[1])
        *out++ = '.';
        WEBPP_PUT_CHAR(src[2])
        *out++ = '.';
        WEBPP_PUT_CHAR(src[3])
        *out = '\0';
        return out;
#undef WEBPP_PUT_CHAR
    }
    // NOLINTEND(cppcoreguidelines-macro-usage)

} // namespace webpp::v3

#endif // WEBPP_BENCH_IP_INET_NTOP_HPP
