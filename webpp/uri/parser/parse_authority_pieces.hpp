// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_AUTHORITY_HPP
#define WEBPP_URI_PARSE_AUTHORITY_HPP

#include "../../ip/ipv4.hpp"
#include "./host_ip.hpp"
#include "./parse_credentials.hpp"
#include "./parse_port.hpp"
#include "./special_schemes.hpp"

namespace webpp::uri::details {

    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_authority_pieces(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using enum uri_status;
        using details::ascii_bitmap;
        using details::FORBIDDEN_DOMAIN_CODE_POINTS;
        using details::FORBIDDEN_HOST_CODE_POINTS;
        using iterator = typename CtxT::iterator;

        webpp_static_constexpr ascii_bitmap forbidden_hosts{FORBIDDEN_HOST_CODE_POINTS, '%'};

        webpp_static_constexpr ascii_bitmap normal_chars = forbidden_hosts;
        webpp_static_constexpr ascii_bitmap special_chars =
          CtxT::is_modifiable ? ascii_bitmap{FORBIDDEN_DOMAIN_CODE_POINTS, ascii_bitmap{UPPER_ALPHA<char>}}
                              : FORBIDDEN_DOMAIN_CODE_POINTS;

        bool const is_special               = is_special_scheme(ctx.status);
        auto const authority_begin          = ctx.pos;
        auto       host_begin               = authority_begin;
        iterator   colon_pos                = ctx.end; // start of password or port
        bool       skip_last_char           = false;
        bool       must_contain_credentials = false;
        auto       buffer                   = create_buffer(ctx);
        for (;;) {
            bool done; // NOLINT(*-init-variables)
            if (!is_special) {
                // for opaque hosts:
                done = encode_or_validate(ctx, buffer, C0_CONTROL_ENCODE_SET, normal_chars);
            } else {
                // for domain names:
                // todo: domain to ascii (https://url.spec.whatwg.org/#concept-domain-to-ascii)
                done = decode_or_tolower(ctx, buffer, special_chars);
            }
            if (done) {
                if (Options.empty_host_is_error && !is_special && ctx.pos == authority_begin) {
                    set(ctx.status, host_missing);
                    return;
                }
                set(ctx.status, valid_path);
                break;
            }

            switch (*ctx.pos) {
                case '[': // it's not in the beginning because of the credentials may come before it
                    details::parse_host_ipv6(ctx);
                    break;
                case ':':
                    if constexpr (!Options.parse_credentials && !Options.parse_port) {
                        set_warning(ctx.status, invalid_character);
                        ++ctx.pos;
                        continue;
                    } else if constexpr (!Options.parse_credentials) {
                        set(ctx.status, valid_port);
                    } else if constexpr (!Options.parse_port) {
                        // it must not be a port or a credential, so it must be invalid?
                        set(ctx.status, is_special ? invalid_domain_code_point : invalid_host_code_point);
                    } else {
                        // the first colon is the start of the password section
                        if (colon_pos == ctx.end) {
                            colon_pos = ctx.pos;
                        }

                        // assume it's a port (even though it might be the start of the password)
                        auto const pre_port_pos = ctx.pos;
                        ++ctx.pos;
                        set(ctx.status, valid_port);
                        parse_port<Options>(ctx);

                        // rollback if it's not a port, we roll back and assume it's a password
                        if (has(ctx.status, port_invalid)) {
                            must_contain_credentials = true;
                            clear_port(ctx.out);
                            unset_flag(ctx.status, has_non_null_port);
                            // it might be a "password" or it's invalid port
                            ctx.pos = pre_port_pos + 1;
                            continue;
                        }

                        set_hostname(ctx.out, host_begin, pre_port_pos);

                        if (pre_port_pos == host_begin) {
                            if (Options.empty_host_is_error && is_special) [[unlikely]] {
                                set(ctx.status, host_missing);
                                return;
                            }
                            if (ctx.pos == ctx.end) {
                                set(ctx.status, valid_path);
                            }
                        }
                        return;
                    }
                    break;
                case '\\':
                    if (!is_special) {
                        // todo: check for non-specials
                        break;
                    }
                    [[fallthrough]];
                case '/':
                    // escape if invalid port found
                    if (must_contain_credentials) [[unlikely]] {
                        return;
                    }
                    set(ctx.status, valid_path);
                    break;
                case '?':
                    // escape if invalid port found
                    if (must_contain_credentials) [[unlikely]] {
                        return;
                    }
                    if constexpr (Options.parse_queries) {
                        skip_last_char = true;
                        set(ctx.status, valid_queries);
                    } else {
                        set_warning(ctx.status, invalid_character);
                        skip_separator(ctx, buffer);
                        continue;
                    }
                    break;
                case '#':
                    // escape if invalid port found
                    if (must_contain_credentials) [[unlikely]] {
                        return;
                    }
                    if constexpr (Options.parse_fragment) {
                        skip_last_char = true;
                        set(ctx.status, valid_fragment);
                    } else {
                        set_warning(ctx.status, invalid_character);
                        skip_separator(ctx, buffer);
                        continue;
                    }
                    break;
                case '%':
                    if (!is_special) {
                        if (!validate_percent_encode(ctx, buffer)) [[unlikely]] {
                            set_warning(ctx.status, invalid_character);
                        }
                        continue;
                    }
                    set(ctx.status, invalid_domain_code_point);
                    return;
                case '@':
                    if constexpr (Options.parse_credentials) {
                        details::parse_credentials(ctx, authority_begin, colon_pos);
                        ++ctx.pos;
                        clear_hostname(ctx.out);
                        host_begin               = ctx.pos;
                        must_contain_credentials = false;
                        continue;
                    } else {
                        set(ctx.status, credentials_not_supported);
                        return;
                    }
                default: set(ctx.status, is_special ? invalid_domain_code_point : invalid_host_code_point); return;
            }
            if (must_contain_credentials) {
                return;
            }
            if (ctx.pos == host_begin) [[unlikely]] {
                clear_hostname(ctx.out);
                if (Options.empty_host_is_error && is_special) {
                    set(ctx.status, host_missing);
                    return;
                }
                if (ctx.pos == ctx.end) {
                    set(ctx.status, valid);
                    return;
                }
            }
            break;
        }

        // Parse IPv4 (if it ends with ipv4 octet)
        if (details::is_possible_ends_with_ipv4<Options>(host_begin, ctx.pos - 1, ctx)) {
            // we don't need to initialize it to zero
            stl::array<stl::uint8_t, 4> ipv4_octets_data; // NOLINT(*-init)
            bool const                  should_continue =
              details::parse_host_ipv4<Options>(host_begin, ctx.pos, ipv4_octets_data.data(), ctx);
            if (!should_continue) {
                return;
            }
            if constexpr (istl::String<decltype(buffer)>) {
                clear_hostname(ctx.out);
                pure_ipv4{ipv4_octets_data}.to_string(buffer);
                if (skip_last_char) {
                    ++ctx.pos;
                }
                return;
            }
        }

        set_hostname(ctx.out, host_begin, ctx.pos);
        if (skip_last_char) {
            ++ctx.pos;
        }
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_PARSE_AUTHORITY_HPP
