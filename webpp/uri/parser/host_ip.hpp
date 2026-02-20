// Created by moisrex on 12/28/23.

#ifndef WEBPP_URI_HOST_IP_HPP
#define WEBPP_URI_HOST_IP_HPP

#include "../../ip/inet_pton.hpp"
#include "../../std/string.hpp"
#include "../../strings/hex.hpp"
#include "./uri_context.hpp"

namespace webpp::uri::details {

    static constexpr charset VALID_IPV4{'.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',
                                        'b', 'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F'};
    static constexpr auto    INVALID_IPV4 = inverse<256U>(VALID_IPV4);

    static constexpr charset VALID_IPV6{'.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
                                        'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F', '[', ']', ':'};
    static constexpr auto    INVALID_IPV6 = inverse<256U>(VALID_IPV6);

    /// Checks the last octet of a possible ipv4 address to see if we should parse the host as an ipv4, or
    /// we should parse it normally.
    /// If the last host segment is
    ///   - a number, or
    ///   - a hexadecimal starting with 0x, or 0X
    ///   - an octal value,
    /// then the host MUST be an ipv4, otherwise, it's an INVALID HOST.
    template <uri_options Options, typename Iter, URIContext CtxT>
    static constexpr bool is_possible_ends_with_ipv4(Iter beg, Iter fin, CtxT& ctx) noexcept {
        // https://url.spec.whatwg.org/#ends-in-a-number-checker

        assert(fin != ctx.end);

        // Pruning trailing dots (empty IPv4 octets) only for this checker.
        // The full IPv4 parser still receives the original input and decides if trailing dots are valid.
        while (*fin == '.') [[unlikely]] {
            set_warning(ctx.status, uri_status::ipv4_trailing_empty_octet);
            if (--fin == beg) [[unlikely]] {
                return false;
            }
            if constexpr (Options.allow_multiple_trailing_empty_ipv4_octets) {
                break;
            }
        }

        enum struct operation_type : stl::uint8_t {
            op_no  = 0U,      // return false
            op_dot = 0b1U,
            op_dec = 0b10U,
            op_hex = 0b100U,  // hex characters [a-fA-F]
            op_x   = 0b1000U, // x or X in 0x
        };

        webpp_static_constexpr auto interesting_characters = categorize<operation_type, 256U>(
          cat{.set = u8".", .value = operation_type::op_dot},
          cat{.set = u8"0123456789", .value = operation_type::op_dec},
          cat{.set = u8"abcdefABCDEF", .value = operation_type::op_hex},
          cat{.set = u8"xX", .value = operation_type::op_x});

        bool is_hex      = false;
        bool must_be_hex = false;
        for (auto pos = fin; pos != beg; --pos) {
            switch (static_cast<operation_type>(or_one(interesting_characters, *pos))) {
                case operation_type::op_dec: continue;
                case operation_type::op_dot: break;
                case operation_type::op_x:
                    // next characters now must be ".0x"
                    // NOLINTNEXTLINE(*-inc-dec-in-conditions)
                    if (pos - beg < 1 || *--pos != '0' || (pos != beg && *--pos != '.')) {
                        return false;
                    }
                    is_hex = true;
                    break;
                case operation_type::op_hex: must_be_hex = true; continue;
                case operation_type::op_no: return false;
                default: assert(false); stl::unreachable();
            }
            break;
        }

        // the size and what not will be checked while parsing the IPv4
        return !must_be_hex || is_hex;
    }

    /**
     * @brief Parse ipv4 of a host
     * @details This is not the same as inet_pton4 even though it has a close implementation, but host's ipv4
     *          requires us to support hexadecimal and octal numbers and a few other questionable and
     *          possibly error-prone features which as an implementer, I disagree with the WHATWG standard.
     * @returns true if we need to continue parsing (has nothing to do with it being valid or not)
     */
    template <uri_options Options, typename Iter, URIContext CtxT>
    static constexpr bool parse_host_ipv4(Iter src, Iter end, stl::uint8_t* out, CtxT& ctx) noexcept {
        // https://url.spec.whatwg.org/#concept-ipv4-parser
        using enum uri_status;

        // NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic)

        // 256 ^ 4 + 1 = any number bigger than 255, we chose 256; multiplied by 4 so we can check
        // if it's an invalid character or out of range without putting 2 if statements on the main loop
        // Octet 4294967295 (256 ^ 4) is a valid first octet, anything bigger is invalid.
        webpp_static_constexpr auto invalid_num =
          static_cast<stl::uint64_t>(stl::numeric_limits<stl::uint32_t>::max()) + 1;

        if (src == end) [[unlikely]] {
            set(ctx.status, ip_bad_ending);
            return false;
        }

        int           octets = 1;
        stl::uint64_t octet  = 0;
        if (*src != '.') {
            for (;;) {
                stl::uint64_t octet_base = 10;

                // find the current octet's base
                if (Options.allow_ipv4_hex_octal_octets && *src == '0') {
                    // octet, hex, or a series of zeros (000000)
                    // assume it's octal (all zero decimals will be parsed correctly as octal)
                    octet_base = 8;
                    ++src;
                    if (src != end && (*src == 'x' || *src == 'X')) {
                        octet_base = 16; // it's definitely hex or invalid octet now
                        ++src;
                    }
                }


                // parse an octet
                octet = 0;
                for (; src != end; ++src) {
                    stl::uint64_t digit  = octet;
                    digit               *= octet_base;
                    if (Options.allow_ipv4_hex_octal_octets && octet_base == 16) [[unlikely]] {
                        digit += ascii::hex_digit<stl::uint64_t, true, invalid_num>(*src);
                    } else {
                        digit += ascii::hex_digit<stl::uint64_t, false, invalid_num>(*src);
                    }
                    if (digit >= invalid_num) {
                        if (*src == '.') {
                            ++src;
                            break;
                        }
                        set(ctx.status, ip_invalid_character);
                        return false;
                    }
                    octet = digit;
                }

                if (Options.allow_ipv4_hex_octal_octets && octet_base != 10 && octet != 0) [[unlikely]] {
                    set_warning(ctx.status, ipv4_non_decimal_octet);
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
                    set(ctx.status, ip_invalid_octet_range);
                    return false;
                }

                *out++ = static_cast<stl::uint8_t>(octet);
                ++octets;
            }
        }

        if (src != end && *src == '.') [[unlikely]] {
            if constexpr (Options.allow_multiple_trailing_empty_ipv4_octets) {
                for (; src != end; ++src) {
                    if (*src != '.') [[unlikely]] {
                        set(ctx.status, ip_invalid_character);
                        return false;
                    }
                }
            } else if constexpr (Options.allow_trailing_empty_ipv4_octet) {
                // empty octet at the end is found:
                if (++src == end) {
                    set_warning(ctx.status, ipv4_trailing_empty_octet);
                } else {
                    set(ctx.status, ip_invalid_character);
                    return false;
                }
            } else {
                set(ctx.status, ip_invalid_character);
                return false;
            }
        }

        // the last octet can fill multiple octets
        if constexpr (Options.allow_multiple_trailing_empty_ipv4_octets || Options.allow_trailing_empty_ipv4_octet) {
            for (; octets != 5; ++octets) {
                *out++  = static_cast<stl::uint8_t>(octet >> static_cast<stl::uint64_t>((4 - octets) * 8));
                octet  &= ~(0xFFULL << static_cast<stl::uint64_t>((4 - octets) * 8));
            }
        } else if (octets != 5) [[unlikely]] {
            set(ctx.status, ip_too_little_octets);
            return false;
        }
        if (octet != 0) [[unlikely]] {
            set(ctx.status, ip_too_many_octets);
            return false;
        }

        // NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic)
        return true;
    }

    template <typename Iter>
    static constexpr void render_ipv6(stl::uint8_t const* data, Iter& out) noexcept {
        *out++ = '[';
        out    = inet_ntop6(data, out);
        *out++ = ']';
    }

    /**
     * @brief Parse ipv6 of a host (starts with '[' and ends with ']')
     * @returns true if we need to continue parsing (has nothing to do with it being valid or not)
     */
    template <URIContext CtxT>
    static constexpr void parse_host_ipv6(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using enum uri_status;

        auto                                      buffer = create_buffer(ctx);
        stl::array<stl::uint8_t, ipv6_byte_count> ipv6_bytes{};

        // todo: do we need this check?
        if (has_hostname(ctx.out)) [[unlikely]] {
            set(ctx.status, invalid_domain_code_point);
            return;
        }

        assert(*ctx.pos == '[');
        ++ctx.pos; // first char should be '[' now

        switch (auto const ipv6_parsing_result = inet_pton6(ctx.pos, ctx.end, ipv6_bytes.data(), ']')) {
            case inet_pton6_status::valid:
                set(ctx.status, ipv6_unclosed);
                break;
            [[likely]] case inet_pton6_status::valid_special:
                if (*ctx.pos != ']') [[unlikely]] {
                    set(ctx.status, ipv6_unclosed);
                    break;
                }
                if constexpr (URIModifiableComponents<typename CtxT::component_type>) {
                    // re-generate the IPv6 string
                    istl::resize_and_overwrite(
                      buffer,
                      max_ipv6_str_len + 3,
                      [&](auto* buf, stl::size_t const max_len) noexcept {
                          auto const beg = buf;
                          render_ipv6(ipv6_bytes.data(), buf);
                          auto const len = stl::distance(beg, buf);
                          assert(len <= static_cast<stl::ptrdiff_t>(max_len));
                          return len;
                      });
                    set_hostname(ctx.out, stl::move(buffer));
                } else {
                    end_segment(ctx, buffer);
                    set_hostname(ctx.out, buffer);
                }
                // set_hostname(ctx.out, ipv6_bytes);
                set_flag(ctx.status, has_non_empty_host);
                switch (*++ctx.pos) {
                    case '/': set(ctx.status, valid_path); break;
                    case ':':
                        set(ctx.status, valid_port);
                        ++ctx.pos;
                        break;
                    case '#':
                        set(ctx.status, valid_fragment);
                        ++ctx.pos;
                        break;
                    case '?':
                        set(ctx.status, valid_queries);
                        ++ctx.pos;
                        break;
                    [[unlikely]] default:
                        set(ctx.status, ipv6_char_after_closing);
                        break;
                }
                break;
            default:
                set(ctx.status, static_cast<uri_status>(error_bit | stl::to_underlying(ipv6_parsing_result)));
                break;
        }
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_HOST_IP_HPP
