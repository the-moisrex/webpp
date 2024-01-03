// Created by moisrex on 12/28/23.

#ifndef WEBPP_URI_HOST_IP_HPP
#define WEBPP_URI_HOST_IP_HPP

#include "../../ip/inet_pton.hpp"
#include "../../strings/hex.hpp"
#include "./uri_components.hpp"

namespace webpp::uri::details {

    /// Checks the last octet of a possible ipv4 address to see if we should parse the host as an ipv4, or
    /// we should parse it normally.
    /// If the last host segment is
    ///   - a number, or
    ///   - an hexadecimal starting with 0x, or 0X
    ///   - an octal value,
    /// then the host MUST be an ipv4, otherwise, it's an INVALID HOST.
    template <uri_parsing_options Options, typename Iter, typename... T>
    static constexpr bool
    is_possible_ends_with_ipv4(Iter beg, Iter fin, parsing_uri_context<T...>& ctx) noexcept {
        // https://url.spec.whatwg.org/#ends-in-a-number-checker
        using ctx_type = parsing_uri_context<T...>;

        webpp_assume(fin != ctx.end);

        // Prunning last dot characters (considering them as empty IPv4 octets)
        // todo: move this to the end of the algorithm if possible
        if constexpr (Options.allow_multiple_trailing_empty_ipv4_octets) {
            while (*fin == '.') {
                set_warning(ctx.status, uri_status::ipv4_trailing_empty_octet);
                if (--fin == beg) {
                    return false;
                }
            }
        } else {
            if (*fin == '.') {
                set_warning(ctx.status, uri_status::ipv4_trailing_empty_octet);
                if (--fin == beg) {
                    return false;
                }
            }
        }

        bool is_hex      = false;
        bool must_be_hex = false;
        auto pos         = fin;
        for (; pos != beg; --pos) {
            switch (*pos) {
                case '.': break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': continue;
                case 'X':
                    if constexpr (ctx_type::is_modifiable) {
                        stl::unreachable();
                    }
                    [[fallthrough]];
                case 'x':
                    // next characters now must be ".0x"
                    // NOLINTNEXTLINE(*-inc-dec-in-conditions)
                    if (pos - beg <= 2 || *--pos != '0' || *--pos != '.') {
                        return false;
                    }
                    is_hex = true;
                    break;
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                    // if it's modifiable, then we should be lowercasing the characters before we reach
                    // here in this function.
                    if constexpr (ctx_type::is_modifiable) {
                        stl::unreachable();
                    }
                    [[fallthrough]];
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                    must_be_hex = true;
                    continue;
                [[likely]] default:
                    return false;
            }
            break;
        }

        // the size and what not will be check while parsing the IPv4
        return !must_be_hex || is_hex;
    }

    /**
     * @brief Parse ipv4 of a host
     * @details This is not the same as inet_pton4 even though it has a close implementation, but host's ipv4
     *          requires us to support hexadecimal and octal numbers and a few other questionable and
     *          possibly error-prone features which as an implementer, I disagree with the WHATWG standard.
     * @returns true if we need to continue parsing (has nothing to do with it being valid or not)
     */
    template <uri_parsing_options Options = {}, typename Iter, typename... T>
    static constexpr bool
    parse_host_ipv4(Iter src, Iter end, stl::uint8_t* out, parsing_uri_context<T...>& ctx) noexcept {
        // https://url.spec.whatwg.org/#concept-ipv4-parser

        // NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        using ctx_type  = parsing_uri_context<T...>;
        using char_type = typename stl::iterator_traits<Iter>::value_type;

        // 256 ^ 4 + 1 = any number bigger than 255, we chose 256; multiplied by 4 so we can check
        // if it's an invalid character or out of range without putting 2 if statements on the main loop
        // Octet 4294967295 (256 ^ 4) is a valid first octet, anything bigger is invalid.
        webpp_static_constexpr auto invalid_num =
          static_cast<stl::uint64_t>(stl::numeric_limits<stl::uint32_t>::max()) + 1;

        if (src == end) {
            set_error(ctx.status, uri_status::ip_bad_ending);
            return false;
        }

        int           octets     = 1;
        stl::uint64_t octet      = 0;
        stl::uint64_t octet_base = 10;
        for (;;) {
            // find the current octet's base
            if (*src == '0') {
                // octet, hex, or a seris of zeros (000000)
                if constexpr (Options.allow_ipv4_octal_octets) {
                    octet_base = 8; // asume it's octal (all zero decimals will be parsed correctly as octals)
                }
                if constexpr (Options.allow_ipv4_hex_octets) {
                    // NOLINTNEXTLINE(*-inc-dec-in-conditions)
                    if (++src != end && (*src == 'x' || (ctx_type::is_modifiable && *src == 'X'))) {
                        octet_base = 16; // it's definitely hex or invalid octet now
                        ++src;
                    }
                }
            } else if (*src == '.') {
                break;
            } else {
                octet_base = 10;
            }

            // parse an octet
            char_type cur_char;
            octet = 0;
            while (src != end) {
                cur_char = *src++;

                // todo: possible integer overflow
                stl::uint64_t digit  = octet;
                digit               *= octet_base;
                if (Options.allow_ipv4_hex_octets && octet_base == 16) [[unlikely]] {
                    digit += ascii::hex_digit<stl::uint64_t, true, invalid_num>(cur_char);
                } else {
                    digit += ascii::hex_digit<stl::uint64_t, false, invalid_num>(cur_char);
                }
                if (cur_char == '.') {
                    break; // invalid character
                }
                octet = digit;
            }

            // invalid character found, we're gonna check this at end of each octet instead of for each
            // character we parse; we're guessing this would make the correct path, faster than the failure
            // path
            if (octet >= invalid_num) [[unlikely]] {
                set_error(ctx.status, uri_status::ip_invalid_character);
                return false;
            }

            if constexpr (Options.allow_ipv4_hex_octets || Options.allow_ipv4_octal_octets) {
                if (octet_base != 10 && octet != 0) [[unlikely]] {
                    set_warning(ctx.status, uri_status::ipv4_non_decimal_octet);
                }
            }

            if (src == end || *src == '.') {
                break;
            }

            // dealing with invalid octet range or invalid characters
            if (octet > 255) [[unlikely]] {
                set_error(ctx.status, uri_status::ip_invalid_octet_range);
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

        if (src != end && *src == '.') {
            if constexpr (Options.allow_multiple_trailing_empty_ipv4_octets) {
                for (; src != end; ++src) {
                    if (*src != '.') {
                        set_error(ctx.status, uri_status::invalid_character);
                        return false;
                    }
                }
            } else if constexpr (Options.allow_trailing_empty_ipv4_octet) {
                // empty octet at the end is found:
                if (++src == end) {
                    set_warning(ctx.status, uri_status::ipv4_trailing_empty_octet);
                }
            }
        }

        // the last octet can fill multiple octets
        for (; octets != 5; ++octets) {
            *out++  = static_cast<stl::uint8_t>(octet >> static_cast<stl::uint64_t>((4 - octets) * 8));
            octet   &= ~(0xFFULL << static_cast<stl::uint64_t>((4 - octets) * 8));
        }
        if (octet != 0) {
            set_error(ctx.status, uri_status::ip_too_many_octets);
            return false;
        }

        // NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        return true;
    }

    /**
     * @brief Parse ipv6 of a host (starts with '[' and ends with ']')
     * @returns true if we need to continue parsing (has nothing to do with it being valid or not)
     */
    template <typename... T>
    static constexpr bool parse_host_ipv6(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        auto const                                beg = ctx.pos;
        stl::array<stl::uint8_t, ipv6_byte_count> ipv6_bytes{};

        ++ctx.pos; // first char should be '[' now

        // todo: use context's output host for storing ipv6 bytes if the host supports it
        switch (auto const ipv6_parsing_result = inet_pton6(ctx.pos, ctx.end, ipv6_bytes.data(), ']')) {
            case inet_pton6_status::valid: set_error(ctx.status, uri_status::ipv6_unclosed); return false;
            case inet_pton6_status::valid_special:
                if (*ctx.pos == ']') {
                    ++ctx.pos;
                    ctx.out.set_hostname(beg, ctx.pos);
                    if (ctx.pos == ctx.end) {
                        set_valid(ctx.status, uri_status::valid);
                        return false;
                    }
                    switch (*ctx.pos) {
                        case '/': set_valid(ctx.status, uri_status::valid_path); return false;
                        case ':': set_valid(ctx.status, uri_status::valid_port); break;
                        case '#': set_valid(ctx.status, uri_status::valid_fragment); break;
                        case '?': set_valid(ctx.status, uri_status::valid_queries); break;
                        default: set_error(ctx.status, uri_status::ipv6_char_after_closing); return false;
                    }
                    ++ctx.pos;
                    return false;
                }
                set_error(ctx.status, uri_status::ipv6_unclosed);
                return false;
            default:
                set_error(ctx.status,
                          static_cast<uri_status>(error_bit | stl::to_underlying(ipv6_parsing_result)));
                return false;
        }
        return true;
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_HOST_IP_HPP
