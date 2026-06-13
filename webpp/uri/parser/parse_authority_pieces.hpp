// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_AUTHORITY_HPP
#define WEBPP_URI_PARSE_AUTHORITY_HPP

#include "../../ip/ipv4.hpp"
#include "./host_ip.hpp"
#include "./idna_to_ascii.hpp"
#include "./parse_credentials.hpp"
#include "./parse_port.hpp"
#include "./special_schemes.hpp"

namespace webpp::uri::details {

    template <uri_options Options, URIContext CtxT, typename HostBufferT>
    [[nodiscard]] static constexpr bool
    set_parsed_hostname(CtxT& ctx, bool const is_special, HostBufferT& normalized_host) noexcept(CtxT::is_nothrow) {
        if constexpr (CtxT::is_modifiable) {
            if (is_special) {
                auto const host_is_empty = [&]() constexpr noexcept {
                    if constexpr (CtxT::is_modifiable) {
                        return normalized_host.empty();
                    } else {
                        return normalized_host.beg == normalized_host.end;
                    }
                }();
                if (host_is_empty) {
                    clear_hostname(ctx.out);
                    unset_flag(ctx.status, uri_status::has_non_null_host);
                    clear_segment(ctx, normalized_host);
                    return true;
                }
                auto       host_out = create_buffer(ctx);
                auto const to_ascii_res =
                  idna::domain_to_ascii<Options>(normalized_host.begin(), normalized_host.end(), host_out);
                if (!idna::is_valid(to_ascii_res)) [[unlikely]] {
                    idna::set_error(ctx.status, to_ascii_res);
                    return false;
                }
                set_hostname(ctx.out, stl::move(host_out));
                set_flag(ctx.status, uri_status::has_non_null_host);
            } else {
                set_hostname(ctx.out, stl::move(normalized_host));
                set_flag(ctx.status, uri_status::has_non_null_host);
            }
            clear_segment(ctx, normalized_host);
        } else {
            end_segment(ctx, normalized_host);
            set_hostname(ctx.out, normalized_host);
            set_flag(ctx.status, uri_status::has_non_null_host);
        }
        return true;
    }

    template <uri_options Options, URIContext CtxT>
    [[nodiscard]] static constexpr bool recover_credentials(
      CtxT&                   ctx,
      typename CtxT::iterator colon_pos,
      typename CtxT::iterator authority_begin,
      auto&                   buffer) noexcept(CtxT::is_nothrow) {
        if constexpr (Options.parse_credentials) {
            auto scan_pos       = ctx.pos;
            auto next_colon_pos = colon_pos;
            auto last_at_pos    = ctx.end;
            auto last_colon_pos = colon_pos;
            for (; scan_pos != ctx.end; ++scan_pos) {
                switch (*scan_pos) {
                    case '/':
                    case '\\':
                    case '?':
                    case '#': break;
                    case ':':
                        if (next_colon_pos == ctx.end) {
                            next_colon_pos = scan_pos;
                        }
                        continue;
                    case '@':
                        last_at_pos    = scan_pos;
                        last_colon_pos = next_colon_pos;
                        continue;
                    default: continue;
                }
                break;
            }
            if (last_at_pos != ctx.end) {
                colon_pos = last_colon_pos;
                ctx.pos   = last_at_pos;
                parse_credentials(ctx, authority_begin, colon_pos);
                ++ctx.pos;
                clear_hostname(ctx.out);
                unset_flag(ctx.status, uri_status::has_non_null_host);
                clear_segment(ctx, buffer);
                return true;
            }
        }
        return false;
    }

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
                if (must_contain_credentials) [[unlikely]] {
                    return;
                }
                if (!is_special && ctx.pos == authority_begin) [[unlikely]] {
                    set(ctx.status, host_missing);
                    return;
                }
                set(ctx.status, valid_path);
                break;
            }

            if (must_contain_credentials) [[unlikely]] {
                if (*ctx.pos == '@') {
                    // confirmed: this section was credentials, not a port
                    unset_flag(ctx.status, port_invalid);
                } else if (*ctx.pos == '/' || *ctx.pos == '\\' || *ctx.pos == '?' || *ctx.pos == '#') {
                    // no '@' before authority delimiter: keep port_invalid
                    return;
                } else {
                    // keep scanning for a confirming '@' without parsing as host/port
                    ++ctx.pos;
                    continue;
                }
            }

            switch (*ctx.pos) {
                case '[': // it's not in the beginning because of the credentials may come before it
                    details::parse_host_ipv6(ctx);
                    return;
                case ':':
                    if constexpr (!Options.parse_credentials && !Options.parse_port) {
                        set(ctx.status, port_not_supported);
                        return;
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
                        parse_port<Options>(ctx);

                        // rollback if it's not a port, we roll back and assume it's a password
                        if (has(ctx.status, port_invalid)) {
                            must_contain_credentials = true;
                            clear_port(ctx.out);
                            unset_flag(ctx.status, has_non_null_port);
                            // keep port_invalid for now; we'll unset it only if we later see another '@'
                            ctx.pos = pre_port_pos + 1;
                            continue;
                        }

                        if (has_error(ctx.status)) [[unlikely]] {
                            // prevent possible segfaults
                            return;
                        }

                        if (!details::set_parsed_hostname<Options>(ctx, is_special, buffer)) [[unlikely]] {
                            return;
                        }

                        if (pre_port_pos == host_begin) {
                            set(ctx.status, host_missing);
                            return;
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
                    skip_last_char = true;
                    set(ctx.status, valid_path);
                    break;
                case '?':
                    skip_last_char = true;
                    set(ctx.status, valid_queries);
                    break;
                case '#':
                    skip_last_char = true;
                    set(ctx.status, valid_fragment);
                    break;
                case '%':
                    if (!is_special) {
                        if (!next_percent_encode(ctx, buffer)) [[unlikely]] {
                            set_warning(ctx.status, invalid_character);
                        }
                        continue;
                    }
                    if (recover_credentials<Options>(ctx, colon_pos, authority_begin, buffer)) {
                        must_contain_credentials = false;
                        host_begin               = ctx.pos;
                        continue;
                    }
                    set(ctx.status, invalid_domain_code_point);
                    return;
                case '@':
                    if constexpr (Options.parse_credentials) {
                        if (recover_credentials<Options>(ctx, colon_pos, authority_begin, buffer)) {
                            must_contain_credentials = false;
                            host_begin               = ctx.pos;
                        }
                        continue;
                    } else {
                        set(ctx.status, credentials_not_supported);
                        return;
                    }
                [[unlikely]] default:
                    if (is_special && recover_credentials<Options>(ctx, colon_pos, authority_begin, buffer)) {
                        must_contain_credentials = false;
                        host_begin               = ctx.pos;
                        continue;
                    }
                    set(ctx.status, is_special ? invalid_domain_code_point : invalid_host_code_point);
                    return;
            }
            if (ctx.pos == host_begin) [[unlikely]] {
                clear_hostname(ctx.out);
                unset_flag(ctx.status, has_non_null_host);
                if ((is_special || has_flags(ctx.status, contains_credentials))) [[unlikely]] {
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
        if (details::is_possible_ends_with_ipv4<Options>(host_begin, stl::prev(ctx.pos), ctx)) {
            // we don't need to initialize it to zero
            stl::array<stl::uint8_t, 4> ipv4_octets_data; // NOLINT(*-init)
            bool const                  should_continue =
              details::parse_host_ipv4<Options>(host_begin, ctx.pos, ipv4_octets_data.data(), ctx);
            if (!should_continue) {
                set_flag(ctx.status, has_non_null_host);
                return;
            }
            if constexpr (istl::String<decltype(buffer)>) {
                clear_hostname(ctx.out);
                buffer.clear();
                pure_ipv4{ipv4_octets_data}.to_string(buffer);
                set_hostname(ctx.out, stl::move(buffer));
                set_flag(ctx.status, has_non_null_host);
                if (skip_last_char) {
                    ++ctx.pos;
                }
                return;
            }
        }

        if (!details::set_parsed_hostname<Options>(ctx, is_special, buffer)) [[unlikely]] {
            return;
        }
        if (skip_last_char) {
            ++ctx.pos;
        }
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_PARSE_AUTHORITY_HPP
