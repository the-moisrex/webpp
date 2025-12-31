// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_AUTHORITY_HPP
#define WEBPP_URI_PARSE_AUTHORITY_HPP

#include "../../ip/ipv4.hpp"
#include "../credentials.hpp"
#include "../encoding.hpp"
#include "./host_ip.hpp"

namespace webpp::uri::details {

    static constexpr ascii_bitmap forbidden_domains{FORBIDDEN_DOMAIN_CODE_POINTS, '.'};

    template <typename CtxT>
    static constexpr auto& init_string_host(CtxT& ctx) {
        if constexpr (CtxT::is_modifiable) {
            auto& host = get_component<components::host>(ctx);
            if constexpr (requires { host.init_domain(); }) {
                host.init_domain();
                return *host.as_domain();
            } else {
                return host;
            }
        } else {
            return istl::nothing;
        }
    }

    template <uri_options Options = uri_options{}, bool IsSpecial = true, URIContext CtxT>
    static constexpr void parse_authority_pieces(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using enum uri_status;
        using details::ascii_bitmap;
        using details::FORBIDDEN_DOMAIN_CODE_POINTS;
        using details::FORBIDDEN_HOST_CODE_POINTS;

        using ctx_type = CtxT;
        using iterator = typename ctx_type::iterator;

        webpp_static_constexpr ascii_bitmap forbidden_hosts{
          ctx_type::is_segregated ? ascii_bitmap{FORBIDDEN_HOST_CODE_POINTS, '.'}
            : FORBIDDEN_HOST_CODE_POINTS,
          '%'
        };


        webpp_static_constexpr ascii_bitmap interesting_characters =
          !IsSpecial                ? forbidden_hosts
          : ctx_type::is_modifiable ? ascii_bitmap{forbidden_domains, ascii_bitmap{UPPER_ALPHA<char>}}
                                    : forbidden_domains;

        auto const authority_begin = ctx.pos;
        auto       host_begin      = authority_begin;
        iterator   colon_pos       = ctx.end; // start of password or port

        bool  skip_last_char           = false;
        bool  must_contain_credentials = false;
        auto& out                      = init_string_host(ctx);
        auto  buffer                   = get_buffer(get_component<components::host>(ctx));
        auto  seg_beg                  = ctx.pos;

        for (;;) {
            bool done; // NOLINT(*-init-variables)
            if constexpr (!IsSpecial) {
                // for opaque hosts:
                done = encode_or_validate(ctx, buffer, C0_CONTROL_ENCODE_SET, interesting_characters);
            } else {
                // for domain names:
                // todo: domain to ascii (https://url.spec.whatwg.org/#concept-domain-to-ascii)
                done = decode_or_tolower(ctx, buffer, interesting_characters);
            }
            if (done) {
                if constexpr (Options.empty_host_is_error && !IsSpecial) {
                    if (ctx.pos == authority_begin) {
                        set_error(ctx.status, host_missing);
                        return;
                    }
                }
                set_valid(ctx.status, valid_path);
                break;
            }

            switch (*ctx.pos) {
                case '[': // it's not in the beginning because of the credentials may come before it
                    if (!details::parse_host_ipv6(ctx)) {
                        return;
                    }
                    break;
                case ':': {
                    if constexpr (!Options.parse_credentials && !Options.parse_port) {
                        set_warning(ctx.status, invalid_character);
                        ++ctx.pos;
                        continue;
                    } else if constexpr (!Options.parse_credentials) {
                        set_valid(ctx.status, valid_port);
                    } else if constexpr (!Options.parse_port) {
                        // it must not be a port or a credential, so it must be invalid?
                        set_error(ctx.status, IsSpecial ? invalid_domain_code_point : invalid_host_code_point);
                    } else {
                        // the first colon is the start of the password section
                        if (colon_pos == ctx.end) {
                            colon_pos = ctx.pos;
                        }

                        // assume it's a port (even though it might be the start of the password)
                        auto const pre_port_pos = ctx.pos;
                        ++ctx.pos;
                        set_valid(ctx.status, valid_port);
                        parse_port(ctx);

                        // rollback if it's not a port, we roll back and assume it's a password
                        if (get_value(ctx.status) == port_invalid) {
                            must_contain_credentials = true;
                            clear<components::port>(ctx);
                            unset_flag(ctx.status, has_non_null_port);
                            // it might be a "password" or it's invalid port
                            ctx.pos = pre_port_pos + 1;
                            continue;
                        }

                        set_component_value<components::host>(ctx, host_begin, pre_port_pos);

                        if (pre_port_pos == host_begin) {
                            if constexpr (Options.empty_host_is_error && IsSpecial) {
                                set_error(ctx.status, host_missing);
                                return;
                            } else if (ctx.pos == ctx.end) {
                                set_valid(ctx.status, valid_path);
                            }
                        }
                        return;
                    }
                    break;
                }
                case '\\':
                    if constexpr (!IsSpecial) {
                        // todo: check for non-specials
                        break;
                    }
                    [[fallthrough]];
                case '/':
                    // escape if invalid port found
                    if (must_contain_credentials) {
                        return;
                    }
                    set_valid(ctx.status, valid_path);
                    break;
                case '.':
                    skip_separator(ctx, out);
                    reset_segment_start(ctx, seg_beg);
                    continue;
                case '?':
                    // escape if invalid port found
                    if (must_contain_credentials) {
                        return;
                    }
                    if constexpr (Options.parse_queries) {
                        skip_last_char = true;
                        set_valid(ctx.status, valid_queries);
                    } else {
                        set_warning(ctx.status, invalid_character);
                        skip_separator(ctx, out);
                        continue;
                    }
                    break;
                case '#':
                    // escape if invalid port found
                    if (must_contain_credentials) {
                        return;
                    }
                    if constexpr (Options.parse_fragment) {
                        skip_last_char = true;
                        set_valid(ctx.status, valid_fragment);
                    } else {
                        set_warning(ctx.status, invalid_character);
                        skip_separator(ctx, out);
                        continue;
                    }
                    break;
                case '%':
                    if constexpr (!IsSpecial) {
                        if (!validate_percent_encode<Options.ignore_tabs_or_newlines>(ctx, buffer)) {
                            set_warning(ctx.status, invalid_character);
                        }
                        continue;
                    } else {
                        set_error(ctx.status, invalid_domain_code_point);
                        return;
                    }
                case '@':
                    must_contain_credentials = false;
                    if constexpr (Options.parse_credentials) {
                        details::parse_credentials(ctx, authority_begin, colon_pos);
                        ++ctx.pos;
                        clear<components::host>(ctx);
                        reset_begin(ctx, seg_beg);
                        host_begin = ctx.pos;
                        continue;
                    } else {
                        // todo: set an error
                        set_warning(ctx.status, has_credentials);
                        set_warning(ctx.status, invalid_character);
                        return;
                    }
                default: set_error(ctx.status, IsSpecial ? invalid_domain_code_point : invalid_host_code_point); return;
            }
            if (must_contain_credentials) {
                return;
            }
            if (ctx.pos == host_begin) {
                if constexpr (Options.empty_host_is_error && IsSpecial) {
                    set_error(ctx.status, host_missing);
                    return;
                } else if (ctx.pos == ctx.end) {
                    set_valid(ctx.status, valid_path);
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
            if constexpr (CtxModifiableStringOutput<decltype(buffer), CtxT>) {
                clear<components::host>(ctx);
                pure_ipv4{ipv4_octets_data}.to_string(buffer);
                if (skip_last_char) {
                    ++ctx.pos;
                }
                return;
            }
        }

        set_component_value<components::host>(ctx, seg_beg);
        if (skip_last_char) {
            ++ctx.pos;
        }
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_PARSE_AUTHORITY_HPP
