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
        if constexpr (Options.multiple_trailing_empty_ipv4_octets) {
            while (*fin == '.') {
                set_warning(ctx.status, uri_status::ipv4_empty_octet);
                if (--fin == beg) {
                    return false;
                }
            }
        } else {
            if (*fin == '.') {
                set_warning(ctx.status, uri_status::ipv4_empty_octet);
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
    template <uri_parsing_options Options, typename Iter, typename... T>
    static constexpr bool
    parse_host_ipv4(Iter src, Iter end, stl::uint8_t* out, parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#concept-ipv4-parser

        using ctx_type  = parsing_uri_context<T...>;
        using char_type = typename stl::iterator_traits<Iter>::value_type;

        auto const beg = ctx.pos;

        char_type octet_base = 0;
        bool      saw_digit  = false;
        int       octets     = 0;
        *out                 = 0;
        while (src != end) {
            auto const cur_char = *src++;
            switch (cur_char) {
                case '.':
                    if (octets == 4 && (!Options.allow_ipv4_empty_octets || octet_base == 0)) {
                        set_error(ctx.status, uri_status::ip_bad_ending);
                        return false;
                    }
                    octet_base = 0;
                    *++out     = 0;
                    break;
                case 'X':
                    if constexpr (ctx_type::is_modifiable) {
                        stl::unreachable();
                    }
                    [[fallthrough]];
                case 'x':
                    if (octet_base == 8) {
                        octet_base = 16; // it's hex
                    } else {
                        set_error(ctx.status, uri_status::ip_invalid_character);
                        return false;
                    }
                    break;
                case '0':
                    if (octet_base == 0) {
                        octet_base = 8; // octal or hex
                        continue;
                    }
                    break;
            }
            // 256 = any number bigger than 255
            auto new_i = ascii::hex_digit_value<int, !ctx_type::is_modifiable>(cur_char, 256);
            if (new_i == 256) {
                set_error(ctx.status, uri_status::ip_invalid_character);
                return false;
            }
            new_i += *out * octet_base;
            if (new_i > 255) {
                set_error(ctx.status, uri_status::ip_invalid_octet_range);
                return false;
            }
            *out = static_cast<uint8_t>(new_i);
            if (++octets > 4) {
                set_error(ctx.status, uri_status::ip_too_many_octets);
                return false;
            }
        }
        if (octets != 4) {
            set_error(ctx.status, uri_status::ip_too_little_octets);
            return false;
        }
        return true;
    }

    /**
     * @brief Parse ipv6 of a host (starts with '[' and ends with ']')
     * @returns true if we need to continue parsing (has nothing to do with it being valid or not)
     */
    template <typename... T>
    static constexpr bool parse_host_ipv6(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        webpp_assume(ctx.pos < ctx.end);

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
