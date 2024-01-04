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

namespace webpp::v2 {
    template <typename Iter, typename... T>
    static constexpr bool parse_host_ipv4(Iter src, Iter end, stl::uint8_t* out) noexcept {
        // https://url.spec.whatwg.org/#concept-ipv4-parser

        // NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        using char_type = typename stl::iterator_traits<Iter>::value_type;

        // 256 ^ 4 + 1 = any number bigger than 255, we chose 256; multiplied by 4 so we can check
        // if it's an invalid character or out of range without putting 2 if statements on the main loop
        // Octet 4294967295 (256 ^ 4) is a valid first octet, anything bigger is invalid.
        webpp_static_constexpr auto invalid_num =
          static_cast<stl::uint64_t>(stl::numeric_limits<stl::uint32_t>::max()) + 1;

        if (src == end) {
            return false;
        }

        char_type     octet_base = 10;
        int           octets     = 1;
        stl::uint64_t octet      = 0;
        for (;;) {
            // find the current octet's base
            if (*src == '0') {
                // octet, hex, or a seris of zeros (000000)
                octet_base = 8; // asume it's octal (all zero decimals will be parsed correctly as octals)
                // NOLINTNEXTLINE(*-inc-dec-in-conditions)
                if (++src != end && (*src == 'x' || *src == 'X')) {
                    octet_base = 16; // it's definitely hex or invalid octet now
                    ++src;
                }
            } else if (*src == '.') {
                return false;
            } else {
                octet_base = 10;
            }

            // parse an octet
            char_type cur_char;
            while (src != end) {
                cur_char = *src++;

                stl::uint64_t digit  = octet;
                digit               *= octet_base;
                if (octet_base == 16) [[unlikely]] {
                    digit += ascii::hex_digit_safe<uint64_t, true>(cur_char, invalid_num);
                } else {
                    digit += ascii::hex_digit_safe<uint64_t, true, false>(cur_char, invalid_num);
                }
                if (digit >= invalid_num) {
                    if (cur_char != '.') [[unlikely]] {
                        return false;
                    }
                    break; // invalid character, or a dot
                }
                octet = digit;
            }


            if (src == end) {
                break;
            }

            // dealing with invalid octet range or invalid characters
            if (octet > 255) [[unlikely]] {
                return false;
            }

            *out++ = static_cast<stl::uint8_t>(octet);
            octet  = 0;
            ++octets;

            if (octets == 5) [[unlikely]] { // empty octet (two dots after each other)
                // empty octet at the end is found:
                break;
            }
        }

        // the last octet can fill multiple octets
        for (; octets != 5; ++octets) {
            *out++  = static_cast<stl::uint8_t>(octet >> static_cast<stl::uint64_t>((4 - octets) * 8));
            octet  &= ~(0xFFULL << ((4 - octets) * 8ULL));
        }
        if (octet != 0) {
            return false;
        }

        // NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        return true;
    }

} // namespace webpp::v2

namespace webpp::v3 {
    template <typename Iter, typename... T>
    static constexpr bool parse_host_ipv4(Iter src, Iter end, stl::uint8_t* out) noexcept {
        // https://url.spec.whatwg.org/#concept-ipv4-parser

        // NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        using char_type = typename stl::iterator_traits<Iter>::value_type;

        // 256 ^ 4 + 1 = any number bigger than 255, we chose 256; multiplied by 4 so we can check
        // if it's an invalid character or out of range without putting 2 if statements on the main loop
        // Octet 4294967295 (256 ^ 4) is a valid first octet, anything bigger is invalid.
        webpp_static_constexpr auto invalid_num =
          static_cast<stl::uint64_t>(stl::numeric_limits<stl::uint32_t>::max()) + 1;

        if (src == end) {
            return false;
        }

        char_type     octet_base = 10;
        int           octets     = 1;
        stl::uint64_t octet      = 0;
        for (;;) {
            // find the current octet's base
            if (*src == '0') {
                // octet, hex, or a seris of zeros (000000)
                octet_base = 8; // asume it's octal (all zero decimals will be parsed correctly as octals)
                // NOLINTNEXTLINE(*-inc-dec-in-conditions)
                if (++src != end && (*src == 'x' || *src == 'X')) {
                    octet_base = 16; // it's definitely hex or invalid octet now
                    ++src;
                }
            } else if (*src == '.') {
                return false;
            } else {
                octet_base = 10;
            }

            // parse an octet
            char_type cur_char;
            while (src != end) {
                cur_char = *src++;

                stl::uint64_t digit  = octet;
                digit               *= octet_base;
                if (octet_base == 16) [[unlikely]] {
                    digit += ascii::hex_digit<stl::uint64_t, true, invalid_num>(cur_char);
                } else {
                    digit += ascii::hex_digit<stl::uint64_t, false, invalid_num>(cur_char);
                }
                if (digit >= invalid_num) {
                    if (cur_char != '.') [[unlikely]] {
                        return false;
                    }
                    break; // invalid character, or a dot
                }
                octet = digit;
            }


            if (src == end) {
                break;
            }

            // dealing with invalid octet range or invalid characters
            if (octet > 255) [[unlikely]] {
                return false;
            }

            *out++ = static_cast<stl::uint8_t>(octet);
            octet  = 0;
            ++octets;

            if (octets == 5) [[unlikely]] { // empty octet (two dots after each other)
                // empty octet at the end is found:
                break;
            }
        }

        // the last octet can fill multiple octets
        for (; octets != 5; ++octets) {
            *out++  = static_cast<stl::uint8_t>(octet >> static_cast<stl::uint64_t>((4 - octets) * 8));
            octet  &= ~(0xFFULL << ((4 - octets) * 8ULL));
        }
        if (octet != 0) {
            return false;
        }

        // NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        return true;
    }

} // namespace webpp::v3

namespace webpp::v4 {

    template <typename Iter, typename... T>
    static constexpr bool parse_host_ipv4(Iter src, Iter end, stl::uint8_t* out) noexcept {
        // NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic)

        // 256 ^ 4 + 1 = any number bigger than 255, we chose 256; multiplied by 4 so we can check
        // if it's an invalid character or out of range without putting 2 if statements on the main loop
        // Octet 4294967295 (256 ^ 4) is a valid first octet, anything bigger is invalid.
        webpp_static_constexpr auto invalid_num =
          static_cast<stl::uint64_t>(stl::numeric_limits<stl::uint32_t>::max()) + 1;

        if (src == end) {
            return false;
        }

        int           octets = 1;
        stl::uint64_t octet  = 0;
        if (*src != '.') {
            for (;;) {
                stl::uint64_t octet_base = 10;
                // find the current octet's base
                if (*src == '0') {
                    // octet, hex, or a seris of zeros (000000)
                    octet_base = 8; // asume it's octal (all zero decimals will be parsed correctly as
                                    // octals)
                    // NOLINTNEXTLINE(*-inc-dec-in-conditions)
                    if (++src != end && (*src == 'x' || (*src == 'X'))) {
                        octet_base = 16; // it's definitely hex or invalid octet now
                        ++src;
                    }
                }


                // parse an octet
                octet = 0;
                for (; src != end; ++src) {
                    stl::uint64_t digit  = octet;
                    digit               *= octet_base;
                    if (octet_base == 16) [[unlikely]] {
                        digit += ascii::hex_digit<stl::uint64_t, true, invalid_num>(*src);
                    } else {
                        digit += ascii::hex_digit<stl::uint64_t, false, invalid_num>(*src);
                    }
                    if (digit >= invalid_num) {
                        if (*src == '.') {
                            ++src;
                            break; // invalid character
                        }
                        return false;
                    }
                    octet = digit;
                }


                if (src == end) {
                    break;
                }
                if (*src == '.') {
                    --src;
                    break;
                }

                // dealing with invalid octet range or invalid characters
                if (octet > 255) [[unlikely]] {
                    return false;
                }

                *out++ = static_cast<stl::uint8_t>(octet);
                ++octets;

                // if (octets == 5) [[unlikely]] { // empty octet (two dots after each other)
                //     // this also could be "too-many-octets" kinda situation, but we're not gonna parse
                //     // around to find out
                //     set_error(ctx.status, uri_status::ip_bad_ending);
                //     return false;
                // }
            }
        }

        if (src != end && *src == '.') {
            // empty octet at the end is found:
        }

        // the last octet can fill multiple octets
        for (; octets != 5; ++octets) {
            *out++  = static_cast<stl::uint8_t>(octet >> static_cast<stl::uint64_t>((4 - octets) * 8));
            octet  &= ~(0xFFULL << static_cast<stl::uint64_t>((4 - octets) * 8));
        }
        if (octet != 0) {
            return false;
        }

        // NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        return true;
    }

} // namespace webpp::v4
#endif // IPV4_HOST_PARSER_HPP
