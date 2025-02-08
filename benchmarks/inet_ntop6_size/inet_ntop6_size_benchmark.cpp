#include "../../webpp/ip/ipv6.hpp"
#include "../benchmark.hpp"

#include <cstdint>
#include <string_view>

namespace stl {
    using namespace std;
}

static constexpr stl::string_view some_valid_ipv6s[]{
  "2001:db8:3333:4444:5555:6666:7777:8888",
  "2001:db8:3333:4444:cccc:dddd:eeee:ffff",
  "2001:db8::",          // implies that the last six segments are zero
  "2001:db8::1234:5678", // implies that the middle four segments are zero
  "2001:0db8:0001::0ab9:c0a8:0102",
  "2001:db8:1::ab9:c0a8:102",
  "2001:0db8:85a3::8a2e:0370:7334",
  "2001:db8:1234::5678",
  "2001:db8::1",
  "2001::1",
  "::1234:5678:91.123.4.56",     // implies that the first four ipv6 segments are zero
  "2001:db8:3333:4444:5555:6666:1.2.3.4",
  "2001:db8::123.123.123.123",   // implies that the last four ipv6 segments are zero
  "::1234:5678:1.2.3.4",         // implies that the first four ipv6 segments are zero
  "2001:db8::1234:5678:5.6.7.8", // implies that the middle two ipv6 segments are zero
  "::1",
  "::ffff:192.0.2.128",          // IPv4-mapped IPv6 address
  "::FFFF:129.144.52.38",
  "::FAFF:129.144.52.38",        // not a IPv4-Compatible IPv6 Address, but looks like one
  "1::129.144.52.38",            // not a IPv4-Compatible IPv6 Address, but looks like one
  "::",

  // AI Generated:
  "2001:db8:0:0:0:0:0:1",                   // loopback address
  "2001:db8:ffff:ffff:ffff:ffff:ffff:ffff", // largest address
  "::",                                     // unspecified address
  "::1",                                    // loopback address shorthand
  "2001:db8::",                             // compressed address
  "2001:db8:0:1:0:0:0:0",                   // sequential address
  "2001:db8:0:0:0:ff:0:0",                  // multicast address
  "fe80::1",                                // link-local address
  "ff01::1",                                // multicast address
  "ff02::1",                                // multicast address
  "ff02::2",                                // multicast address
  "ff02::3",                                // multicast address
  "ff02::4",                                // multicast address
  "ff02::5",                                // multicast address
  "ff02::6",                                // multicast address
  "ff02::7",                                // multicast address
  "ff02::8",                                // multicast address
  "ff02::9",                                // multicast address
  "ff02::a",                                // multicast address
  "ff02::b",                                // multicast address
  "ff02::c",                                // multicast address
  "ff02::d",                                // multicast address
  "ff02::e",                                // multicast address
  "ff02::f",                                // multicast address
  "fe80::217:f2ff:fe07:ed62",               // link-local address
  "fe80::217:f2ff:fe07:ed63",               // link-local address
  "fe80::217:f2ff:fe07:ed64",               // link-local address
  "fe80::217:f2ff:fe07:ed65",               // link-local address
  "fe80::217:f2ff:fe07:ed66",               // link-local address
  "fe80::217:f2ff:fe07:ed67",               // link-local address
  "fe80::217:f2ff:fe07:ed68",               // link-local address
  "fe80::217:f2ff:fe07:ed69",               // link-local address
  "fe80::217:f2ff:fe07:ed6a",               // link-local address
  "fe80::217:f2ff:fe07:ed6b",               // link-local address
  "fe80::217:f2ff:fe07:ed6c",               // link-local address
  "fe80::217:f2ff:fe07:ed6d",               // link-local address
  "fe80::217:f2ff:fe07:ed6e",               // link-local address
  "fe80::217:f2ff:fe07:ed6f",               // link-local address
  "fe80::217:f2ff:fe07:ed70",               // link-local address
  "fe80::217:f2ff:fe07:ed71",               // link-local address
  "fe80::217:f2ff:fe07:ed72",               // link-local address
  "fe80::217:f2ff:fe07:ed73",               // link-local address
  "fe80::217:f2ff:fe07:ed74",               // link-local address
  "fe80::217:f2ff:fe07:ed75",               // link-local address
  "fe80::217:f2ff:fe07:ed76",               // link-local address
  "fe80::217:f2ff:fe07:ed77",               // link-local address
  "fe80::217:f2ff:fe07:ed78",               // link-local address
  "fe80::217:f2ff:fe07:ed79",               // link-local address
  "fe80::217:f2ff:fe07:ed7a",               // link-local address
  "fe80::217:f2ff:fe07:ed7b",               // link-local address
  "fe80::217:f2ff:fe07:ed7c",               // link-local address
  "fe80::217:f2ff:fe07:ed7d",               // link-local address
  "fe80::217:f2ff:fe07:ed7e",               // link-local address
  "fe80::217:f2ff:fe07:ed7f",               // link-local address
  "fe80::217:f2ff:fe07:ed80",               // link-local address
  "fe80::217:f2ff:fe07:ed81",               // link-local address
  "fe80::217:f2ff:fe07:ed82",               // link-local address
  "fe80::217:f2ff:fe07:ed83",               // link-local address
  "fe80::217:f2ff:fe07:ed84",               // link-local address
  "fe80::217:f2ff:fe07:ed85",               // link-local address
  "fe80::217:f2ff:fe07:ed86",               // link-local address
  "fe80::217:f2ff:fe07:ed87",               // link-local address
  "fe80::217:f2ff:fe07:ed88",               // link-local address
  "fe80::217:f2ff:fe07:ed89",               // link-local address
  "fe80::217:f2ff:fe07:ed8a",               // link-local address
  "fe80::217:f2ff:fe07:ed8b",               // link-local address
  "fe80::217:f2ff:fe07:ed8c",               // link-local address
  "fe80::217:f2ff:fe07:ed8d",               // link-local address
  "fe80::217:f2ff:fe07:ed8e",               // link-local address
  "fe80::217:f2ff:fe07:ed8f",               // link-local address
  "fe80::217:f2ff:fe07:ed90",               // link-local address
  "fe80::217:f2ff:fe07:ed91",               // link-local address
  "fe80::217:f2ff:fe07:ed92",               // link-local address
  "fe80::217:f2ff:fe07:ed93",               // link-local address
  "fe80::217:f2ff:fe07:ed94",               // link-local address
  "fe80::217:f2ff:fe07:ed95",               // link-local address
  "fe80::217:f2ff:fe07:ed96",               // link-local address
  "fe80::217:f2ff:fe07:ed97",               // link-local address
  "fe80::217:f2ff:fe07:ed98",               // link-local address
  "fe80::217:f2ff:fe07:ed99",               // link-local address
  "fe80::217:f2ff:fe07:ed9a",               // link-local address
  "fe80::217:f2ff:fe07:ed9b",               // link-local address
  "fe80::217:f2ff:fe07:ed9c",               // link-local address
  "fe80::217:f2ff:fe07:ed9d",               // link-local address
  "fe80::217:f2ff:fe07:ed9e",               // link-local address
  "fe80::217:f2ff:fe07:ed9f",               // link-local address
};

// Get the string size
[[nodiscard]] static constexpr stl::size_t inet_ntop4_size(stl::uint8_t const* oct) noexcept {
    // NOLINTBEGIN(*-implicit-bool-conversion)
    return 7U +                                 // 3 (dots) + 4 (base digits)
           (oct[0] >= 10U) + (oct[0] >= 100U) + // Octet 1 (LSB)
           (oct[1] >= 10U) + (oct[1] >= 100U) + // Octet 2
           (oct[2] >= 10U) + (oct[2] >= 100U) + // Octet 3
           (oct[3] >= 10U) + (oct[3] >= 100U);  // Octet 4 (MSB)
    // NOLINTEND(*-implicit-bool-conversion)
}

namespace v1 {


    [[nodiscard]] static constexpr int hex_group_size_optimized(stl::uint16_t group) noexcept {
        if (group == 0) {
            return 1;
        }
        return 4 - std::countl_zero(group) / 4;
    }

    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (!src) {
            return 0U;
        }

        stl::uint16_t groups[8];
        for (int i = 0; i < 8; ++i) {
            // groups[i] = std::byteswap(reinterpret_cast<stl::uint16_t const*>(src)[i]);
            groups[i] = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
        }

        // Early IPv4-mapped check (optimized)
        if (groups[0] == 0 && groups[1] == 0 && groups[2] == 0 && groups[3] == 0 && groups[4] == 0 &&
            groups[5] == 0xffff)
        {
            return 7 + inet_ntop4_size(src + 12);
        }

        int group_sizes[8];
        for (int i = 0; i < 8; ++i) {
            group_sizes[i] = hex_group_size_optimized(groups[i]);
        }

        // Optimized zero-run detection (same as before)
        int longest_run   = 0;
        int current_run   = 0;
        int longest_start = -1;

        for (int i = 0; i < 8; ++i) {
            current_run = (groups[i] == 0) ? (current_run + 1) : 0;
            if (current_run > longest_run) {
                longest_run   = current_run;
                longest_start = i - current_run + 1;
            }
        }

        stl::size_t size = 0;
        if (longest_run > 1) {
            // groups before the "::"
            for (int i = 0; i < longest_start; ++i) {
                size += group_sizes[i] + (i > 0);
            }
            size += 2; // For "::"
            // groups after the "::"
            for (int i = longest_start + longest_run; i < 8; ++i) {
                size += group_sizes[i] + (i > longest_start + longest_run);
            }
        } else {
            // No compression
            for (int i = 0; i < 8; ++i) {
                size += group_sizes[i] + (i > 0);
            }
        }

        return size;
    }

} // namespace v1

namespace v2 {

    [[nodiscard]] stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        // Precompute all 16-bit groups to avoid redundant calculations
        stl::array<stl::uint16_t, 8> group_vals;
        for (int i = 0; i < 8; ++i) {
            group_vals[i] = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
        }

        // Step 1: Compute length for each group and prefix sum
        stl::array<stl::size_t, 8> len;
        stl::array<stl::size_t, 9> prefix_sum = {0};

        for (int i = 0; i < 8; ++i) {
            stl::uint16_t const group_val = group_vals[i];
            if (group_val == 0) {
                len[i] = 1;
            } else {
                // Calculate leading zero nibbles using bit scan operations
                int const lz                   = stl::countl_zero(static_cast<uint32_t>(group_val) << 16);
                int const leading_zero_nibbles = lz / 4;
                len[i]                         = 4 - leading_zero_nibbles;
            }
            prefix_sum[i + 1] = prefix_sum[i] + len[i];
        }

        // Step 2: Find the longest run of zero groups
        int longest_count = 0;
        int longest_index = -1;
        int current_run   = 0;

        for (int i = 0; i < 8; ++i) {
            if (group_vals[i] == 0) {
                current_run++;
                if (current_run > longest_count) {
                    longest_count = current_run;
                    longest_index = i - current_run + 1;
                } else if (current_run == longest_count) {
                    longest_index = i - current_run + 1; // Prefer later runs
                }
            } else {
                current_run = 0;
            }
        }

        // Check for IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 && group_vals[5] == 0xFFFF) {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Step 3: Calculate total length based on longest run
        stl::size_t total_length = 0;
        if (longest_count >= 1) {
            int const         groups_before = longest_index;
            int const         groups_after  = 8 - (longest_index + longest_count);
            stl::size_t const sum_before    = prefix_sum[longest_index];
            stl::size_t const sum_after     = prefix_sum[8] - prefix_sum[longest_index + longest_count];
            stl::size_t const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            stl::size_t const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length                    = sum_before + sum_after + colons_before + colons_after + 2;
        } else {
            total_length = prefix_sum[8] + 7;
        }

        return total_length;
    }

} // namespace v2

namespace v3 {


    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        // Step 1: Compute length for each group
        stl::array<stl::size_t, 8U> len;
        for (int i = 0; i < 8; ++i) {
            auto const group_val = (static_cast<stl::uint16_t>(src[2U * i]) << 8U) | src[(2U * i) + 1U];
            if (group_val == 0) {
                len[i] = 1;
            } else {
                int leading_zeros = 0;
                for (int nibble_pos = 0; nibble_pos < 4; ++nibble_pos) {
                    stl::uint8_t const nibble = (group_val >> (12U - nibble_pos * 4U)) & 0xFU;
                    if (nibble != 0) {
                        break;
                    }
                    leading_zeros++;
                }
                len[i] = 4 - leading_zeros;
                if (len[i] < 1) {
                    len[i] = 1;
                }
            }
        }

        // Step 2: Find the longest run of zero groups
        int longest_count = 0;
        int longest_index = -1;
        int current_run   = 0;
        for (int i = 0; i < 8; ++i) {
            auto const group_val = (static_cast<stl::uint16_t>(src[2U * i]) << 8U) | src[(2U * i) + 1U];
            if (group_val == 0) {
                current_run++;
                if (current_run > longest_count) {
                    longest_count = current_run;
                    longest_index = i - current_run + 1;
                } else if (current_run == longest_count) {
                    longest_index = i - current_run + 1; // later run takes precedence
                }
            } else {
                current_run = 0;
            }
        }

        // Check for IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 && src[10] == 0xFF && src[11] == 0xFFU) {
            // Calculate IPv4 string length
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Step 3: Calculate total length based on longest run
        stl::size_t total_length = 0;
        if (longest_count >= 1) {
            int const   groups_before = longest_index;
            int const   groups_after  = 8 - (longest_index + longest_count);
            stl::size_t sum_before    = 0;
            for (int i = 0; i < groups_before; ++i) {
                sum_before += len[i];
            }
            stl::size_t sum_after = 0;
            for (int i = longest_index + longest_count; i < 8; ++i) {
                sum_after += len[i];
            }
            int const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            int const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length            = sum_before + sum_after + colons_before + colons_after + 2;
        } else {
            stl::size_t sum_all = 0;
            for (int i = 0; i < 8; ++i) {
                sum_all += len[i];
            }
            total_length = sum_all + 7;
        }

        return total_length;
    }

} // namespace v3

namespace v4 {


    [[nodiscard]] inline int hex_group_size(stl::uint16_t group) noexcept {
        if (group == 0) {
            return 1;
        }
        int len = 0;
        if (group >= 0x1000) {
            len++;
        }
        if (group >= 0x0100) {
            len++;
        }
        if (group >= 0x0010) {
            len++;
        }
        if (group >= 0x0001) {
            len++;
        }
        return len;
    }

    [[nodiscard]] stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (!src) {
            return 0U;
        }

        stl::uint16_t groups[8];
        for (int i = 0; i < 8; ++i) {
            groups[i] = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
        }

        // Early IPv4-mapped check
        if (groups[0] == 0 && groups[1] == 0 && groups[2] == 0 && groups[3] == 0 && groups[4] == 0 &&
            groups[5] == 0xffff)
        {
            return 7 + inet_ntop4_size(src + 12);
        }

        int longest_run   = 0;
        int current_run   = 0;
        int longest_start = -1;
        for (int i = 0; i < 8; ++i) {
            if (groups[i] == 0) {
                current_run++;
            } else {
                if (current_run > longest_run) {
                    longest_run   = current_run;
                    longest_start = i - current_run;
                }
                current_run = 0;
            }
        }
        if (current_run > longest_run) {
            longest_run   = current_run;
            longest_start = 8 - current_run;
        }

        stl::size_t size = 0;
        if (longest_run > 1) {
            for (int i = 0; i < longest_start; ++i) {
                size += hex_group_size(groups[i]);
            }
            size += 2; // For "::"
            for (int i = longest_start + longest_run; i < 8; ++i) {
                size += hex_group_size(groups[i]);
            }
            size +=
              (longest_start > 0 ? longest_start - 1 : 0) +
              (8 - (longest_start + longest_run) > 0 ? 8 - (longest_start + longest_run) - 1 : 0); // Colons
        } else {
            for (int i = 0; i < 8; ++i) {
                size += hex_group_size(groups[i]);
            }
            size += 7; // 7 colons
        }

        return size;
    }

} // namespace v4

namespace v5 {

    [[nodiscard]] inline int hex_group_size(stl::uint16_t group) noexcept {
        if (group == 0) {
            return 1;
        }
        int len = 0;
        if (group >= 0x1000) {
            len++;
        }
        if (group >= 0x0100) {
            len++;
        }
        if (group >= 0x0010) {
            len++;
        }
        if (group >= 0x0001) {
            len++;
        }
        return len;
    }

    [[nodiscard]] stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (!src) {
            return 0U;
        }

        stl::uint16_t groups[8];
        for (int i = 0; i < 8; ++i) {
            groups[i] = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
        }

        // Early IPv4-mapped check
        if (groups[0] == 0 && groups[1] == 0 && groups[2] == 0 && groups[3] == 0 && groups[4] == 0 &&
            groups[5] == 0xffff)
        {
            return 7 + inet_ntop4_size(src + 12);
        }

        int longest_run   = 0;
        int current_run   = 0;
        int longest_start = -1;
        for (int i = 0; i < 8; ++i) {
            if (groups[i] == 0) {
                current_run++;
            } else {
                if (current_run > longest_run) {
                    longest_run   = current_run;
                    longest_start = i - current_run;
                }
                current_run = 0;
            }
        }
        if (current_run > longest_run) {
            longest_run   = current_run;
            longest_start = 8 - current_run;
        }

        stl::size_t size = 0;
        if (longest_run > 1) {
            for (int i = 0; i < longest_start; ++i) {
                size += hex_group_size(groups[i]);
            }
            size += 2; // For "::"
            for (int i = longest_start + longest_run; i < 8; ++i) {
                size += hex_group_size(groups[i]);
            }
            size +=
              (longest_start > 0 ? longest_start - 1 : 0) +
              (8 - (longest_start + longest_run) > 0 ? 8 - (longest_start + longest_run) - 1 : 0); // Colons
        } else {
            for (int i = 0; i < 8; ++i) {
                size += hex_group_size(groups[i]);
            }
            size += 7; // 7 colons
        }

        return size;
    }

} // namespace v5

namespace v6 {


    [[nodiscard]] inline int hex_group_size_optimized(stl::uint16_t group) noexcept {
        if (group == 0) {
            return 1;
        }
        return 4 - std::countl_zero(group) / 4;
    }

    [[nodiscard]] stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (!src) {
            return 0U;
        }

        stl::uint16_t groups[8];
        for (int i = 0; i < 8; ++i) {
            groups[i] = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
        }

        // Early IPv4-mapped check
        if (groups[0] == 0 && groups[1] == 0 && groups[2] == 0 && groups[3] == 0 && groups[4] == 0 &&
            groups[5] == 0xffff)
        {
            return 7 + inet_ntop4_size(src + 12);
        }

        int longest_run   = 0;
        int current_run   = 0;
        int longest_start = -1;
        for (int i = 0; i < 8; ++i) {
            if (groups[i] == 0) {
                current_run++;
            } else {
                if (current_run > longest_run) {
                    longest_run   = current_run;
                    longest_start = i - current_run;
                }
                current_run = 0;
            }
        }
        if (current_run > longest_run) {
            longest_run   = current_run;
            longest_start = 8 - current_run;
        }

        stl::size_t size             = 0;
        bool        compression_used = false;
        if (longest_run > 1) {
            for (int i = 0; i < longest_start; ++i) {
                if (i > 0) {
                    size++; // Colon before group (except first)
                }
                size += hex_group_size_optimized(groups[i]);
            }
            size             += 2; // For "::"
            compression_used  = true;
            for (int i = longest_start + longest_run; i < 8; ++i) {
                if (i > longest_start + longest_run) {
                    size++; // Colon before group (except first after ::)
                }
                size += hex_group_size_optimized(groups[i]);
            }
        } else {
            for (int i = 0; i < 8; ++i) {
                if (i > 0) {
                    size++; // Colon before group (except first)
                }
                size += hex_group_size_optimized(groups[i]);
            }
        }
        return size;
    }

} // namespace v6

namespace v7 {


    [[nodiscard]] inline int hex_group_size_optimized(stl::uint16_t group) noexcept {
        if (group == 0) {
            return 1;
        }
        return 4 - std::countl_zero(group) / 4;
    }

    [[nodiscard]] stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (!src) {
            return 0U;
        }

        stl::uint16_t groups[8];
        for (int i = 0; i < 8; ++i) {
            groups[i] = std::byteswap(
              static_cast<stl::uint16_t>(src[2 * i] | (static_cast<stl::uint16_t>(src[2 * i + 1]) << 8)));
        }

        // Early IPv4-mapped check (optimized)
        if (groups[0] == 0 && groups[1] == 0 && groups[2] == 0 && groups[3] == 0 && groups[4] == 0 &&
            groups[5] == 0xffff)
        {
            return 7 + inet_ntop4_size(src + 12);
        }

        // Optimized zero-run detection
        int longest_run   = 0;
        int current_run   = 0;
        int longest_start = -1;

        for (int i = 0; i < 8; ++i) {
            current_run = (groups[i] == 0) ? (current_run + 1) : 0;
            if (current_run > longest_run) {
                longest_run   = current_run;
                longest_start = i - current_run + 1;
            }
        }

        stl::size_t size = 0;
        if (longest_run > 1) {
            // groups before the "::"
            for (int i = 0; i < longest_start; ++i) {
                size += hex_group_size_optimized(groups[i]) + (i > 0); // Add colon and size
            }
            size += 2;                                                 // For "::"
            // groups after the "::"
            for (int i = longest_start + longest_run; i < 8; ++i) {
                size += hex_group_size_optimized(groups[i]) + (i > longest_start + longest_run);
            }
        } else {
            // No compression
            for (int i = 0; i < 8; ++i) {
                size += hex_group_size_optimized(groups[i]) + (i > 0); // Add colon and size
            }
        }

        return size;
    }

} // namespace v7

namespace v8 {


    [[nodiscard]] stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        // Precompute all 16-bit groups to avoid redundant calculations
        stl::array<stl::uint16_t, 8> group_vals;
        for (int i = 0; i < 8; ++i) {
            group_vals[i] = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
        }

        // Step 1: Compute length for each group and prefix sum
        stl::array<stl::size_t, 8> len;
        stl::array<stl::size_t, 9> prefix_sum = {0};

        for (int i = 0; i < 8; ++i) {
            stl::uint16_t const group_val = group_vals[i];
            if (group_val == 0) {
                len[i] = 1;
            } else {
                // Calculate leading zero nibbles using bit scan operations
                int const lz                   = __builtin_clz(static_cast<uint32_t>(group_val) << 16);
                int const leading_zero_nibbles = lz / 4;
                len[i]                         = 4 - leading_zero_nibbles;
            }
            prefix_sum[i + 1] = prefix_sum[i] + len[i];
        }

        // Step 2: Find the longest run of zero groups
        int longest_count = 0;
        int longest_index = -1;
        int current_run   = 0;

        for (int i = 0; i < 8; ++i) {
            if (group_vals[i] == 0) {
                current_run++;
                if (current_run > longest_count) {
                    longest_count = current_run;
                    longest_index = i - current_run + 1;
                } else if (current_run == longest_count) {
                    longest_index = i - current_run + 1; // Prefer later runs
                }
            } else {
                current_run = 0;
            }
        }

        // Check for IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 && group_vals[5] == 0xFFFF) {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Step 3: Calculate total length based on longest run
        stl::size_t total_length = 0;
        if (longest_count >= 1) {
            int const         groups_before = longest_index;
            int const         groups_after  = 8 - (longest_index + longest_count);
            stl::size_t const sum_before    = prefix_sum[longest_index];
            stl::size_t const sum_after     = prefix_sum[8] - prefix_sum[longest_index + longest_count];
            stl::size_t const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            stl::size_t const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length                    = sum_before + sum_after + colons_before + colons_after + 2;
        } else {
            total_length = prefix_sum[8] + 7;
        }

        return total_length;
    }


} // namespace v8

namespace v9 {

    // Get the string size without converting to string
    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::array<stl::size_t, 8> len{};
        stl::size_t                total_length      = 0;
        int                        longest_run_index = -1;
        int                        longest_run_count = 0;

        // Optimized combined loop
        for (int i = 0, current_run_count = 0; i < 8; ++i) {
            stl::uint16_t group_val = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];

            if (group_val == 0) {
                len[i] = 1;
                current_run_count++;
                if (current_run_count > longest_run_count) {
                    longest_run_count = current_run_count;
                    longest_run_index = i - current_run_count + 1;
                } else if (current_run_count == longest_run_count) {
                    longest_run_index = i - current_run_count + 1; // Favor later runs
                }
            } else {
                len[i]             = 4 - (stl::countl_zero(static_cast<uint32_t>(group_val) << 16) / 4);
                total_length      += len[i]; // Accumulate total length (without colons yet)
                current_run_count  = 0;
            }
        }

        // IPv4-mapped case check (optimized - no group_vals array access)
        if (longest_run_index == 0 && longest_run_count == 5 &&
            ((static_cast<stl::uint16_t>(src[10]) << 8) | src[11]) == 0xFFFF)
        {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        if (longest_run_count > 0) {
            int         groups_before = longest_run_index;
            int         groups_after  = 8 - (longest_run_index + longest_run_count);
            stl::size_t sum_before    = 0;
            for (int i = 0; i < groups_before; ++i) {
                sum_before += len[i];
            }
            stl::size_t sum_after = 0;
            for (int i = longest_run_index + longest_run_count; i < 8; ++i) {
                sum_after += len[i];
            }

            total_length = sum_before + sum_after + 2; // Sum of lengths + "::"
            if (groups_before > 0) {
                total_length += (groups_before - 1);   // Colons before "::"
            }
            if (groups_after > 0) {
                total_length += (groups_after - 1);    // Colons after "::"
            }
        } else {
            total_length += 7;                         // Add 7 colons for the full IPv6 address
        }

        return total_length;
    }


} // namespace v9

namespace v10 {

    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::size_t                  total_length = 0;
        stl::array<stl::size_t, 8>   group_lengths; // Store group lengths for compression adjustment
        stl::array<stl::uint16_t, 8> group_vals; // Store group values for zero run detection (optional - can
                                                 // be recalculated)

        int longest_run_index = -1;
        int longest_run_count = 0;
        int current_run_count = 0;

        for (int i = 0; i < 8; ++i) {
            stl::uint16_t group_val = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
            group_vals[i] = group_val; // Store for later zero-run check (if needed - can optimize out)
            stl::size_t group_length = 4;

            if (group_val == 0) {
                group_length = 1;
                current_run_count++;
                if (current_run_count > longest_run_count) {
                    longest_run_count = current_run_count;
                    longest_run_index = i - current_run_count + 1;
                } else if (current_run_count == longest_run_count) {
                    longest_run_index = i - current_run_count + 1; // Favor later runs
                }
            } else {
                if (group_val < 0x1000) {
                    group_length--;
                }
                if (group_val < 0x0100) {
                    group_length--;
                }
                if (group_val < 0x0010) {
                    group_length--;
                }
                current_run_count = 0;        // Reset run count
            }
            group_lengths[i]  = group_length;
            total_length     += group_length; // Accumulate even if we might compress later, we'll adjust
        }

        // IPv4-mapped case check - moved after main loop for possible integration benefit (group_vals already
        // computed)
        if (longest_run_index == 0 && longest_run_count == 5 && group_vals[5] == 0xFFFF) {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }


        if (longest_run_count >= 2) { // Compress only if run is 2 or more (as per IPv6 rules)
            int         groups_before = longest_run_index;
            int         groups_after  = 8 - (longest_run_index + longest_run_count);
            stl::size_t sum_before    = 0;
            for (int i = 0; i < groups_before; ++i) {
                sum_before += group_lengths[i];
            }
            stl::size_t sum_after = 0;
            for (int i = longest_run_index + longest_run_count; i < 8; ++i) {
                sum_after += group_lengths[i];
            }

            total_length = sum_before + sum_after + 2; // Sum of lengths + "::"
            if (groups_before > 0) {
                total_length += (groups_before - 1);   // Colons before "::"
            }
            if (groups_after > 0) {
                total_length += (groups_after - 1);    // Colons after "::"
            }
            return total_length;
        } else {
            return total_length + 7; // No compression, add 7 colons (as in v2, but now only if no
                                     // compression)
        }
    }


} // namespace v10

namespace v11 {


    stl::size_t branchless_group_length_v2(stl::uint16_t group_val) noexcept {
        if (group_val == 0) {
            return 1; // Still need a branch for zero case
        }

        stl::size_t length = 4;
        length -= (group_val < 0x1000); // Subtract 1 if less than 0x1000 (boolean true/false becomes 1/0)
        length -= (group_val < 0x0100); // Subtract 1 if less than 0x0100
        length -= (group_val < 0x0010); // Subtract 1 if less than 0x0010
        return length;
    }

    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::size_t                  total_length = 0;
        stl::array<stl::size_t, 8>   group_lengths;
        stl::array<stl::uint16_t, 8> group_vals;

        int longest_run_index = -1;
        int longest_run_count = 0;
        int current_run_count = 0;

        for (int i = 0; i < 8; ++i) {
            stl::uint16_t group_val = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
            group_vals[i]           = group_val;

            stl::size_t group_length  = branchless_group_length_v2(group_val); // Use more branchless version
            group_lengths[i]          = group_length;
            total_length             += group_length;

            if (group_val == 0) {
                current_run_count++;
                if (current_run_count > longest_run_count) {
                    longest_run_count = current_run_count;
                    longest_run_index = i - current_run_count + 1;
                } else if (current_run_count == longest_run_count) {
                    longest_run_index = i - current_run_count + 1;
                }
            } else {
                current_run_count = 0;
            }
        }

        // IPv4-mapped case check
        if (longest_run_index == 0 && longest_run_count == 5 && group_vals[5] == 0xFFFF) {
            return 7 + inet_ntop4_size(src + 12);
        }


        if (longest_run_count >= 2) { // Compression logic
            int         groups_before = longest_run_index;
            int         groups_after  = 8 - (longest_run_index + longest_run_count);
            stl::size_t sum_before    = 0;
            for (int i = 0; i < groups_before; ++i) {
                sum_before += group_lengths[i];
            }
            stl::size_t sum_after = 0;
            for (int i = longest_run_index + longest_run_count; i < 8; ++i) {
                sum_after += group_lengths[i];
            }

            total_length = sum_before + sum_after + 2;
            if (groups_before > 0) {
                total_length += (groups_before - 1);
            }
            if (groups_after > 0) {
                total_length += (groups_after - 1);
            }
            return total_length;
        } else {
            return total_length + 7;
        }
    }



} // namespace v11

namespace v12 {

    // Branchless group length calculation
    inline stl::size_t branchless_group_length_v3(stl::uint16_t group_val) noexcept {
        stl::size_t length  = 4;
        length             -= (group_val < 0x1000);
        length             -= (group_val < 0x0100);
        length             -= (group_val < 0x0010);
        return length;
    }

    // Get the string size without converting to string - Branchless Core
    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::size_t                  total_length = 0;
        stl::array<stl::size_t, 8>   group_lengths;
        stl::array<stl::uint16_t, 8> group_vals;

        int longest_run_index = -1;
        int longest_run_count = 0;
        int current_run_count = 0;

        for (int i = 0; i < 8; ++i) {
            stl::uint16_t group_val = (static_cast<stl::uint16_t>(src[2 * i]) << 8) | src[2 * i + 1];
            group_vals[i]           = group_val;

            stl::size_t group_length = branchless_group_length_v3(group_val); // Use truly branchless version!
            group_lengths[i]         = group_length;
            total_length += group_length;

            if (group_val == 0) {
                current_run_count++;
                if (current_run_count > longest_run_count) {
                    longest_run_count = current_run_count;
                    longest_run_index = i - current_run_count + 1;
                } else if (current_run_count == longest_run_count) {
                    longest_run_index = i - current_run_count + 1;
                }
            } else {
                current_run_count = 0;
            }
        }

        // IPv4-mapped case check
        if (longest_run_index == 0 && longest_run_count == 5 && group_vals[5] == 0xFFFF) {
            return 7 + inet_ntop4_size(src + 12);
        }

        if (longest_run_count >= 2) { // Compression logic
            int         groups_before = longest_run_index;
            int         groups_after  = 8 - (longest_run_index + longest_run_count);
            stl::size_t sum_before    = 0;
            for (int i = 0; i < groups_before; ++i) {
                sum_before += group_lengths[i];
            }
            stl::size_t sum_after = 0;
            for (int i = longest_run_index + longest_run_count; i < 8; ++i) {
                sum_after += group_lengths[i];
            }

            total_length = sum_before + sum_after + 2;
            if (groups_before > 0) {
                total_length += (groups_before - 1);
            }
            if (groups_after > 0) {
                total_length += (groups_after - 1);
            }
            return total_length;
        } else {
            return total_length + 7;
        }
    }


} // namespace v12

// v13 = v2 + v9 (combined loops)
namespace v13 {


    // Get the string size without converting to string
    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        // Step 1: Compute length for each group and prefix sum
        stl::array<stl::size_t, 8> len;
        stl::array<stl::size_t, 9> prefix_sum{}; // fill with zero
        int                        longest_count = 0;
        int                        longest_index = -1;
        int                        current_run   = 0;

        for (int i = 0; i < 8; ++i) {
            stl::uint16_t const group_val =
              (static_cast<stl::uint16_t>(src[2U * i]) << 8U) | src[(2U * i) + 1U];

            if (group_val == 0) {
                len[i] = 1;

                current_run++;
                if (current_run > longest_count) {
                    longest_count = current_run;
                    longest_index = i - current_run + 1;
                } else if (current_run == longest_count) {
                    longest_index = i - current_run + 1; // Prefer later runs
                }
            } else {
                // Calculate leading zero nibbles using bit scan operations
                int const clz                  = stl::countl_zero(static_cast<uint32_t>(group_val) << 16U);
                int const leading_zero_nibbles = clz / 4;
                len[i]                         = 4 - leading_zero_nibbles;

                current_run = 0;
            }
            prefix_sum[i + 1] = prefix_sum[i] + len[i];
        }


        // Check for IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 &&
            ((static_cast<stl::uint16_t>(src[10]) << 8U) | src[11]) == 0xFFFF)
        {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Step 3: Calculate total length based on longest run
        stl::size_t total_length = 0;
        if (longest_count >= 1) {
            int const         groups_before = longest_index;
            int const         groups_after  = 8 - (longest_index + longest_count);
            stl::size_t const sum_before    = prefix_sum[longest_index];
            stl::size_t const sum_after     = prefix_sum[8] - prefix_sum[longest_index + longest_count];
            stl::size_t const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            stl::size_t const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length                    = sum_before + sum_after + colons_before + colons_after + 2;
        } else {
            total_length = prefix_sum[8] + 7;
        }

        return total_length;
    }

} // namespace v13

// v14 = v13 + eliminating the "len"
namespace v14 {


    // Get the string size without converting to string
    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::array<stl::size_t, 9> prefix_sum{}; // fill with zero
        int                        longest_count = 0;
        int                        longest_index = -1;
        int                        current_run   = 0;

        // Step 1: Compute length for each group and prefix sum
        // Step 2: Find the longest run of zero groups
        for (int i = 0; i < 8; ++i) {
            stl::uint16_t const group_val =
              (static_cast<stl::uint16_t>(src[2U * i]) << 8U) | src[(2U * i) + 1U];
            stl::size_t len = 1;

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
            prefix_sum[i + 1] = prefix_sum[i] + len;
        }


        // Check for IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 &&
            ((static_cast<stl::uint16_t>(src[10]) << 8U) | src[11]) == 0xFFFF)
        {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Step 3: Calculate total length based on longest run
        stl::size_t total_length = 0;
        if (longest_count >= 1) {
            int const         groups_before = longest_index;
            int const         groups_after  = 8 - (longest_index + longest_count);
            stl::size_t const sum_before    = prefix_sum[longest_index];
            stl::size_t const sum_after     = prefix_sum[8] - prefix_sum[longest_index + longest_count];
            stl::size_t const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            stl::size_t const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length                    = sum_before + sum_after + colons_before + colons_after + 2;
        } else {
            total_length = prefix_sum[8] + 7;
        }

        return total_length;
    }


} // namespace v14

namespace v15 {


    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::array<stl::size_t, 9> prefix_sum{}; // fill with zero
        int                        longest_count = 0;
        int                        longest_index = -1;
        int                        current_run   = 0;

        // Step 1: Compute length for each group and prefix sum
        // Step 2: Find the longest run of zero groups
        for (int i = 0; i < 8; ++i) {
            stl::size_t len = 1;

            if (src[2U * i] == 0U && src[(2U * i) + 1U] == 0U) {
                current_run++;
                if (current_run > longest_count) {
                    longest_count = current_run;
                    longest_index = i - current_run + 1;
                } else if (current_run == longest_count) {
                    longest_index = i - current_run + 1; // Prefer later runs
                }
            } else {
                // Calculate leading zero nibbles using bit scan operations
                stl::uint32_t const group_val =
                  (static_cast<stl::uint32_t>(src[2U * i]) << 24U) | (src[(2U * i) + 1U] << 16U);
                int const clz                  = stl::countl_zero(group_val);
                int const leading_zero_nibbles = clz / 4;
                len                            = 4 - leading_zero_nibbles;

                current_run = 0;
            }
            prefix_sum[i + 1] = prefix_sum[i] + len;
        }


        // Check for IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 && src[10] == 0xFFU && src[11] == 0xFFU) {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Step 3: Calculate total length based on longest run
        stl::size_t total_length = 0;
        if (longest_count >= 1) {
            int const         groups_before = longest_index;
            int const         groups_after  = 8 - (longest_index + longest_count);
            stl::size_t const sum_before    = prefix_sum[longest_index];
            stl::size_t const sum_after     = prefix_sum[8] - prefix_sum[longest_index + longest_count];
            stl::size_t const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            stl::size_t const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length                    = sum_before + sum_after + colons_before + colons_after + 2;
        } else {
            total_length = prefix_sum[8] + 7;
        }

        return total_length;
    }


} // namespace v15

namespace v16 {


    // Get the string size without converting to string
    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::array<stl::size_t, 9> prefix_sum{}; // fill with zero
        int                        longest_count = 0;
        int                        longest_index = -1;
        int                        current_run   = 0;

        // Step 1: Compute length for each group and prefix sum
        // Step 2: Find the longest run of zero groups
        for (int i = 0; i < 8; ++i) {
            stl::size_t         len = 1;
            stl::uint32_t const group_val =
              (static_cast<stl::uint32_t>(src[2U * i]) << 24U) | (src[(2U * i) + 1U] << 16U);

            if (group_val == 0U) {
                current_run++;
                if (current_run > longest_count) {
                    longest_count = current_run;
                    longest_index = i - current_run + 1;
                } else if (current_run == longest_count) {
                    longest_index = i - current_run + 1; // Prefer later runs
                }
            } else {
                // Calculate leading zero nibbles using bit scan operations
                int const clz                  = stl::countl_zero(group_val);
                int const leading_zero_nibbles = clz / 4;
                len                            = 4 - leading_zero_nibbles;

                current_run = 0;
            }
            prefix_sum[i + 1] = prefix_sum[i] + len;
        }


        // Check for IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 && src[10] == 0xFFU && src[11] == 0xFFU) {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Step 3: Calculate total length based on longest run
        stl::size_t total_length = 0;
        if (longest_count >= 1) {
            int const         groups_before = longest_index;
            int const         groups_after  = 8 - (longest_index + longest_count);
            stl::size_t const sum_before    = prefix_sum[longest_index];
            stl::size_t const sum_after     = prefix_sum[8] - prefix_sum[longest_index + longest_count];
            stl::size_t const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            stl::size_t const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length                    = sum_before + sum_after + colons_before + colons_after + 2;
        } else {
            total_length = prefix_sum[8] + 7;
        }

        return total_length;
    }


} // namespace v16

namespace badV17 {

    [[nodiscard]] static constexpr stl::size_t inet_ntop6_size(stl::uint8_t const* src) noexcept {
        if (src == nullptr) {
            return 0U;
        }

        stl::array<stl::size_t, 9> prefix_sum{}; // fill with zero
        int                        longest_count = 0;
        int                        longest_index = -1;
        int                        current_run   = 0;

        // Process groups in chunks of four where possible to find consecutive zeros faster
        int i = 0;
        while (i < 8) {
            // Check if we can process four groups (64 bits) at once
            if (i <= 4) { // i+3 <= 7
                uint64_t four_groups;
                // Use memcpy to avoid alignment issues and ensure constexpr compatibility
                stl::memcpy(&four_groups, src + 2 * i, sizeof(four_groups));
                if (four_groups == 0) {
                    current_run += 4;
                    // Update longest_count and longest_index if this run is the longest
                    if (current_run > longest_count) {
                        longest_count = current_run;
                        longest_index = i - current_run + 1;
                    } else if (current_run == longest_count) {
                        // Prefer the latter run for tie-breaking
                        int new_index = i - current_run + 1;
                        if (new_index > longest_index) {
                            longest_index = new_index;
                        }
                    }
                    i += 4;
                    continue;
                }
            }

            // Process individual group
            stl::uint16_t const group_val =
              (static_cast<stl::uint16_t>(src[2U * i]) << 8U) | src[(2U * i) + 1U];
            stl::size_t len = 1;

            if (group_val == 0) {
                current_run++;
                if (current_run > longest_count) {
                    longest_count = current_run;
                    longest_index = i - current_run + 1;
                } else if (current_run == longest_count) {
                    longest_index = i - current_run + 1; // Prefer later runs
                }
            } else {
                // Correctly calculate leading zero nibbles using countl_zero on 16-bit group_val
                int const clz                  = stl::countl_zero(group_val);
                int const leading_zero_nibbles = clz / 4;
                len                            = 4 - leading_zero_nibbles;

                current_run = 0;
            }
            prefix_sum[i + 1] = prefix_sum[i] + len;
            i++;
        }

        // Handle IPv4-mapped case (::ffff:x.x.x.x)
        if (longest_index == 0 && longest_count == 5 && src[10] == 0xFFU && src[11] == 0xFFU) {
            return 7 + inet_ntop4_size(src + 12); // "::ffff:" + IPv4
        }

        // Calculate total length based on the longest zero run
        stl::size_t total_length = 0;
        if (longest_count >= 1) {
            int const         groups_before = longest_index;
            int const         groups_after  = 8 - (longest_index + longest_count);
            stl::size_t const sum_before    = prefix_sum[longest_index];
            stl::size_t const sum_after     = prefix_sum[8] - prefix_sum[longest_index + longest_count];
            stl::size_t const colons_before = (groups_before > 0) ? (groups_before - 1) : 0;
            stl::size_t const colons_after  = (groups_after > 0) ? (groups_after - 1) : 0;
            total_length = sum_before + sum_after + colons_before + colons_after + 2; // 2 colons for the ::
        } else {
            total_length = prefix_sum[8] + 7; // 7 colons for non-compressed format
        }

        return total_length;
    }

} // namespace badV17

static constexpr auto ips = []() consteval {
    stl::array<webpp::pure_ipv6, sizeof(some_valid_ipv6s) / sizeof(stl::string_view)> ips;
    int                                                                               i = 0;
    for (auto _ip : some_valid_ipv6s) {
        webpp::pure_ipv6 const ip6{_ip};
        ips[i] = ip6;
        ++i;
    }
    return ips;
}();

#define benchmark_version(Ver)                                     \
    static void IPV6Size_##Ver(benchmark::State& state) {          \
        for (auto _ : state) {                                     \
            for (auto ip6 : ips) {                                 \
                auto octets = ip6.octets();                        \
                auto len    = Ver::inet_ntop6_size(octets.data()); \
                benchmark::DoNotOptimize(len);                     \
            }                                                      \
        }                                                          \
    }                                                              \
    BENCHMARK(IPV6Size_##Ver)


benchmark_version(v1);
benchmark_version(v2);
benchmark_version(v3);
benchmark_version(v4);
benchmark_version(v5);
benchmark_version(v6);
benchmark_version(v7);
benchmark_version(v8);
benchmark_version(v9);
benchmark_version(v10);
benchmark_version(v11);
benchmark_version(v12);
benchmark_version(v13);
benchmark_version(v14);
benchmark_version(v15);
benchmark_version(v16);
benchmark_version(badV17);

static void IPV6Size_StringSize(benchmark::State& state) {
    for (auto _ : state) {
        for (auto ip6 : ips) {
            auto len = ip6.ip_string().size();
            benchmark::DoNotOptimize(len);
        }
    }
}

BENCHMARK(IPV6Size_StringSize);
