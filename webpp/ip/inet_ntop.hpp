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

    // NOLINTBEGIN(*-macro-usage, *-avoid-do-while)
    /**
     * Convert an IPv4 to string
     * It's fast, but it's not pretty, I know :)
     */
    template <typename Iter = char*>
    static constexpr Iter inet_ntop4(stl::uint8_t const* src, Iter out) noexcept {
#define WEBPP_PUT_CHAR()                                       \
    do {                                                       \
        if (*src < 10) {                                       \
            *out++ = static_cast<char>('0' + *src);            \
        } else if (*src < 100) {                               \
            *out++ = static_cast<char>('0' + *src / 10);       \
            *out++ = static_cast<char>('0' + *src % 10);       \
        } else {                                               \
            *out++ = static_cast<char>('0' + *src / 100);      \
            *out++ = static_cast<char>('0' + *src % 100 / 10); \
            *out++ = static_cast<char>('0' + *src % 10);       \
        }                                                      \
    } while (false)
        WEBPP_PUT_CHAR();
        ++src;
        *out++ = '.';
        WEBPP_PUT_CHAR();
        ++src;
        *out++ = '.';
        WEBPP_PUT_CHAR();
        ++src;
        *out++ = '.';
        WEBPP_PUT_CHAR();
        *out = '\0';
        return out;
#undef WEBPP_PUT_CHAR
    }

    // NOLINTEND(*-macro-usage, *-avoid-do-while)




    /**
     * Convert IPv6 binary address into presentation (printable) format
     */
    template <typename Iter = char*>
    static constexpr Iter inet_ntop6(stl::uint8_t const* src, Iter out) noexcept {
        using ascii::details::hex_chars;

        using char_type = stl::iter_value_t<Iter>;

        if (src == nullptr) {
            return nullptr;
        }

        *out = '\0';

        char_type           hexa[8 * 5];
        auto*               hex_ptr = static_cast<char_type*>(hexa);
        stl::uint8_t const* src_ptr = src;


        int j             = 0;
        int longest_count = 0;
        int longest_index = -1;

        for (int i = 0; i != 8; ++i) {
            bool skip = true;

            char_type* octet_ptr = hex_ptr;
            *octet_ptr++         = '\0';
            *octet_ptr++         = '\0';
            *octet_ptr++         = '\0';
            *octet_ptr++         = '\0';
            *octet_ptr           = '\0';
            octet_ptr            = hex_ptr;

            stl::uint8_t low_hex_8bit  = *src_ptr++;
            stl::uint8_t high_hex_8bit = low_hex_8bit >> 4U;

            if (high_hex_8bit != 0U) {
                skip         = false;
                *octet_ptr++ = hex_chars<char_type>[high_hex_8bit];
            }

            high_hex_8bit = low_hex_8bit & 0x0FU;
            if (!skip || (high_hex_8bit != 0U)) {
                skip         = false;
                *octet_ptr++ = hex_chars<char_type>[high_hex_8bit];
            }

            low_hex_8bit = *src_ptr++;

            high_hex_8bit = low_hex_8bit >> 4U;
            if (!skip || (high_hex_8bit != 0U)) {
                *octet_ptr++ = hex_chars<char_type>[high_hex_8bit];
            }

            high_hex_8bit  = low_hex_8bit & 0x0FU;
            *octet_ptr      = hex_chars<char_type>[high_hex_8bit];
            hex_ptr       += 5;



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
                for (hex_ptr = hexa + i * 5; *hex_ptr != '\0'; ++hex_ptr) {
                    *out++ = *hex_ptr;
                }
                *out++ = ':';
            }
            for (hex_ptr = hexa + 7 * 5; *hex_ptr != '\0'; ++hex_ptr) {
                *out++ = *hex_ptr;
            }
        } else {
            int index = 0;
            for (; index != longest_index; ++index) {
                for (hex_ptr = hexa + index * 5; *hex_ptr != '\0'; ++hex_ptr) {
                    *out++ = *hex_ptr;
                }
                if (index != 7) {
                    *out++ = ':';
                }
            }
            // check for leading zero
            *out++  = ':';
            index  += longest_count;
            for (; index != 8; ++index) {
                for (hex_ptr = hexa + index * 5; *hex_ptr != '\0'; ++hex_ptr) {
                    *out++ = *hex_ptr;
                }
                if (index != 7) {
                    *out++ = ':';
                }
            }
        }


        *out = '\0';
        return out;
    }

    // Get the string size without converting it to string
    [[nodiscard]] static constexpr stl::size_t inet_ntop4_size(stl::uint8_t const* oct) noexcept {
        // NOLINTBEGIN(*-implicit-bool-conversion)
        return 7U +                                 // 3 (dots) + 4 (base digits)
               (oct[0] >= 10U) + (oct[0] >= 100U) + // Octet 1 (LSB)
               (oct[1] >= 10U) + (oct[1] >= 100U) + // Octet 2
               (oct[2] >= 10U) + (oct[2] >= 100U) + // Octet 3
               (oct[3] >= 10U) + (oct[3] >= 100U);  // Octet 4 (MSB)
        // NOLINTEND(*-implicit-bool-conversion)
    }

    // Get the string size without converting to string
    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::array<int, 9> prefix_sum{}; // fill with zero
        int                longest_count = 0;
        int                longest_index = -1;
        int                current_run   = 0;

        // Step 1: Compute length for each group and prefix sum
        // Step 2: Find the longest run of zero groups
        for (int i = 0; i < 8; ++i) {
            stl::uint16_t const group_val =
              static_cast<stl::uint16_t>(src[2 * i] << 8U) | // NOLINT(*-implicit-widening-*)
              static_cast<stl::uint16_t>(src[(2 * i) + 1]);
            int len = 1;

            if (group_val == 0) {
                current_run++;
                if (current_run > longest_count) {
                    longest_count = current_run;
                    longest_index = i - current_run + 1;
                } else if (current_run == longest_count) {
                    longest_index = i - current_run + 1; // Prefer later runs
                }
            } else {
                // Calculate leading zero nibbles using bit scan operations
                int const clz = stl::countl_zero(static_cast<stl::uint32_t>(group_val) << 16U);
                int const leading_zero_nibbles = clz / 4;
                len                            = 4 - leading_zero_nibbles;

                current_run = 0;
            }
            prefix_sum[static_cast<stl::size_t>(i + 1)] = prefix_sum[static_cast<stl::size_t>(i)] + len;
        }


        // Check for IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 && src[10] == 0xFFU && src[11] == 0xFFU) {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Step 3: Calculate total length based on the longest run
        int total_length = 0;
        if (longest_count >= 1) {
            int const groups_before = longest_index;
            int const groups_after  = 8 - (longest_index + longest_count);
            int const sum_before    = prefix_sum[static_cast<stl::size_t>(longest_index)];
            int const sum_after =
              prefix_sum[8] - prefix_sum[static_cast<stl::size_t>(longest_index + longest_count)];
            int const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            int const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length            = sum_before + sum_after + colons_before + colons_after + 2;
        } else {
            total_length = prefix_sum[8] + 7;
        }

        return static_cast<stl::size_t>(total_length);
    }

    // NOLINTEND(*-magic-numbers)
    // NOLINTEND(*-pro-bounds-pointer-arithmetic)

} // namespace webpp

#endif // WEBPP_IP_INET_NTOP_HPP
