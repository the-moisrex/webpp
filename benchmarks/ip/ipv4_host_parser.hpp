// Created by moisrex on 12/30/23.

#ifndef IPV4_HOST_PARSER_HPP
#define IPV4_HOST_PARSER_HPP

#include "../../webpp/strings/hex.hpp"

#include <cstdint>
#include <iterator>

namespace webpp::v1 {
    /**
     * This is stripped version
     */
    template <typename Iter, typename... T>
    static constexpr bool parse_host_ipv4(Iter src, Iter end, uint8_t* out) noexcept {
        // https://url.spec.whatwg.org/#concept-ipv4-parser

        // NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        using char_type = typename std::iterator_traits<Iter>::value_type;

        // 256 ^ 4 + 1 = any number bigger than 255, we chose 256; multiplied by 4 so we can check
        // if it's an invalid character or out of range without putting 2 if statements on the main loop
        // Octet 4294967295 (256 ^ 4) is a valid first octet, anything bigger is invalid.
        constexpr auto invalid_num = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());


        char_type octet_base = 0;
        int       octets     = 1;
        uint64_t  octet      = 0;
        for (;;) {
            // find the current octet's base
            if (*src == '0') {
                octet_base = 8; // octal or hex
                // NOLINTNEXTLINE(*-inc-dec-in-conditions)
                if (++src != end && (*src == 'x' || *src == 'X')) {
                    octet_base = 16;
                    ++src;
                }
            } else {
                octet_base = 10;
            }

            // parse
            while (src != end) {
                auto const cur_char = *src++;
                if (cur_char == '.') {
                    if (octets == 4 && octet == 0) {
                        return false;
                    }

                    if (octet > 255) {
                        return false;
                    }
                    *out++ = static_cast<uint8_t>(octet);
                    octet  = 0;
                    ++octets;
                    break; // go back and find the base again for the next octet
                }

                octet *= octet_base;
                if (octet_base == 16) {
                    octet += ascii::hex_digit_safe<uint64_t, true>(cur_char, invalid_num);
                } else {
                    octet += ascii::hex_digit_safe<uint64_t, true, false>(cur_char, invalid_num);
                }
                if (octet >= invalid_num) {
                    return false;
                }
            }

            if (src == end) {
                break;
            }
        }

        for (;;) {
            *out++  = static_cast<uint8_t>(octet >> static_cast<uint64_t>((4 - octets) * 8));
            octet  &= ~(0xFFULL << ((4 - octets) * 8ULL));
            if (octets++ == 4) {
                break;
            }
        }
        if (octet != 0) {
            return false;
        }

        // NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        return true;
    }
} // namespace webpp::v1

#endif // IPV4_HOST_PARSER_HPP
