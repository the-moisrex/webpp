// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../ip/ipv4.hpp"
#include "./credentials.hpp"
#include "./details/host_ip.hpp"
#include "./details/uri_components_encoding.hpp"
#include "./details/uri_status.hpp"
#include "./details/windows_drive_letter.hpp"
#include "./encoding.hpp"
#include "./port.hpp"

/**
 * Attention: User infos in URIs are DEPRECATED, but we SHOULD parse it anyway
 *   https://httpwg.org/specs/rfc9110.html#http.userinfo
 */
namespace webpp::uri {

    namespace details {

        template <uri_parsing_options Options   = uri_parsing_options{},
                  bool                IsSpecial = true,
                  ParsingURIContext   CtxT>
        static constexpr void parse_authority_pieces(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            using enum uri_status;
            using details::ascii_bitmap;
            using details::FORBIDDEN_DOMAIN_CODE_POINTS;
            using details::FORBIDDEN_HOST_CODE_POINTS;

            using ctx_type = CtxT;
            using iterator = typename ctx_type::iterator;

            webpp_static_constexpr auto forbidden_hosts = ascii_bitmap{
              ctx_type::is_segregated ? ascii_bitmap{FORBIDDEN_HOST_CODE_POINTS, '.'}
                                      : FORBIDDEN_HOST_CODE_POINTS,
              '%'
            };

            webpp_static_constexpr auto forbidden_domains = ascii_bitmap{FORBIDDEN_DOMAIN_CODE_POINTS, '.'};

            webpp_static_constexpr ascii_bitmap interesting_characters =
              IsSpecial
                ? (ctx_type::is_modifiable ? ascii_bitmap{forbidden_domains, ascii_bitmap{UPPER_ALPHA<char>}}
                                           : forbidden_domains)
                : forbidden_hosts;

            auto const authority_begin = ctx.pos;
            auto       host_begin      = authority_begin;
            iterator   colon_pos       = ctx.end; // start of password or port

            bool skip_last_char           = false;
            bool must_contain_credentials = false;

            component_encoder<components::host, ctx_type> coder(ctx);
            coder.start_segment();
            for (;;) {
                bool done; // NOLINT(*-init-variables)
                if constexpr (!IsSpecial) {
                    // for opaque hosts:
                    done = coder.template encode_or_validate<uri_encoding_policy::encode_chars>(
                      C0_CONTROL_ENCODE_SET,
                      interesting_characters);
                } else {
                    // for domain names:
                    // todo: domain to ascii (https://url.spec.whatwg.org/#concept-domain-to-ascii)
                    done = coder.template decode_or_tolower<uri_encoding_policy::encode_chars>(
                      interesting_characters);
                }
                if (done) {
                    if constexpr (Options.empty_host_is_error && !IsSpecial) {
                        if (ctx.pos == authority_begin) {
                            set_error(ctx.status, host_missing);
                            return;
                        }
                    }
                    set_valid(ctx.status, valid);
                    break;
                }

                switch (*ctx.pos) {
                    case '[': // it's not in the beginning because of the credentials may come before it
                        if (!details::parse_host_ipv6(ctx)) {
                            return;
                        }
                        break;
                    case ':': {
                        if constexpr (!Options.parse_credentails && !Options.parse_port) {
                            set_warning(ctx.status, invalid_character);
                            ++ctx.pos;
                            continue;
                        } else if constexpr (!Options.parse_credentails) {
                            set_valid(ctx.status, valid_port);
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

                            // rollback if it's not a port, we rollback and assume it's a password
                            if (get_value(ctx.status) == port_invalid) {
                                must_contain_credentials = true;
                                clear<components::port>(ctx);
                                // it might be a "password" or it's invalid port
                                ctx.pos = pre_port_pos + 1;
                                continue;
                            }

                            coder.end_segment(coder.segment_begin(), pre_port_pos);
                            coder.set_value(host_begin, pre_port_pos);

                            if (pre_port_pos == host_begin) {
                                if constexpr (Options.empty_host_is_error && IsSpecial) {
                                    set_error(ctx.status, host_missing);
                                    return;
                                } else if (ctx.pos == ctx.end) {
                                    set_valid(ctx.status, valid);
                                }
                            }
                            return;
                        }
                        break;
                    }
                    case '\\':
                        if (!IsSpecial) {
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
                    case '.': coder.next_segment(); continue;
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
                            coder.skip_separator();
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
                            coder.skip_separator();
                            continue;
                        }
                        break;
                    case '%':
                        if constexpr (!IsSpecial) {
                            if (!coder.validate_percent_encode()) {
                                set_warning(ctx.status, invalid_character);
                            }
                            continue;
                        } else {
                            set_error(ctx.status, invalid_domain_code_point);
                            return;
                        }

                    // handling tabs and newlines
                    [[unlikely]] case '\t':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\r':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, uri_status::invalid_character);
                            coder.ignore_character();
                            continue;
                        } else {
                            set_error(ctx.status,
                                      IsSpecial ? invalid_domain_code_point : invalid_host_code_point);
                            return;
                        }
                    case '@':
                        must_contain_credentials = false;
                        if constexpr (Options.parse_credentails) {
                            details::parse_credentials(ctx, authority_begin, colon_pos);
                            ++ctx.pos;
                            clear<components::host>(ctx);
                            coder.reset_begin();
                            coder.start_segment();
                            host_begin = ctx.pos;
                            continue;
                        }
                        else {
                            // todo: set an error
                            set_warning(ctx.status, has_credentials);
                            set_warning(ctx.status, invalid_character);
                            return;
                        }
                    [[unlikely]] case '\0':
                        // invalid port
                        if (must_contain_credentials) {
                            return;
                        }
                        if constexpr (Options.eof_is_valid) {
                            if constexpr (Options.empty_host_is_error) {
                                if (ctx.pos == host_begin) {
                                    set_error(ctx.status, host_missing);
                                    return;
                                }
                            }
                            set_valid(ctx.status, valid);
                            break;
                        }
                        [[fallthrough]];
                    default:
                        set_error(ctx.status,
                                  IsSpecial ? invalid_domain_code_point : invalid_host_code_point);
                        return;
                }
                if (must_contain_credentials) {
                    return;
                }
                if (ctx.pos == host_begin) {
                    if constexpr (Options.empty_host_is_error && IsSpecial) {
                        set_error(ctx.status, host_missing);
                        return;
                    } else if (ctx.pos == ctx.end) {
                        set_valid(ctx.status, valid);
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
                if constexpr (ctx_type::is_modifiable) {
                    clear<components::host>(ctx);
                    coder.start_segment();
                    ipv4{ipv4_octets_data}.to_string(coder.get_out_seg());
                    if (skip_last_char) {
                        ++ctx.pos;
                    }
                    return;
                }
            }

            coder.end_segment();
            coder.set_value();
            if (skip_last_char) {
                ++ctx.pos;
            }
        }

    } // namespace details

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_file_host(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#file-host-state

        using ctx_type = CtxT;
        using char_type = typename ctx_type::char_type;

        static_assert(Options.allow_file_hosts,
                      "This function should not be reached if hosts in 'file://' scheme are not allowed.");


        // handling tabs and newlines
        if constexpr (Options.ignore_tabs_or_newlines) {
            while (ctx.pos != ctx.end) {
                switch (*ctx.pos) {
                    [[unlikely]] case '\t':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\r':
                        set_warning(ctx.status, uri_status::invalid_character);
                        ++ctx.pos;
                        continue;
                    default: break;
                }
                break;
            }
        }

        if constexpr (Options.handle_windows_drive_letters) {
            if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) {
                if (*ctx.pos != '/' || *ctx.pos != '\\') {
                    // we have to move one back because the "path" needs to start with a "/" or a "\"
                    --ctx.pos;
                }
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
                set_valid(ctx.status, uri_status::valid_path);
                return;
            }
        }

        webpp_static_constexpr auto parsing_options = []() constexpr noexcept {
            uri_parsing_options options = Options;
            options.parse_credentails   = false;
            options.empty_host_is_error = false;
            options.parse_port          = false;
            return options;
        }();
        details::parse_authority_pieces<parsing_options>(ctx);

        if (has_value<components::host>(ctx)) {
            if constexpr (ctx_type::is_segregated) {
                auto const host = get_output<components::host>(ctx);
                if (host.size() == 1 &&
                    iiequals_fl<details::TABS_OR_NEWLINES<char_type>>("localhost", host.back()))
                {
                    clear<components::host>(ctx);
                }
            } else {
                if (iiequals_fl<details::TABS_OR_NEWLINES<char_type>>(
                      "localhost",
                      get_output_value<components::host>(ctx)))
                {
                    clear<components::host>(ctx);
                }
            }
        }
        if constexpr (Options.handle_windows_drive_letters) {
            if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) {
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
            }
        }
    }

    /// Path start state (I like to call it authority end because it's more RFC like to
    /// say that, but WHATWG likes to call it "path start state")
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_authority_end(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-start-state

        if (ctx.pos == ctx.end) {
            // todo: I'm guessing
            set_valid(ctx.status, uri_status::valid);
            return;
        }
        if (is_special_scheme(ctx.scheme)) {
            for (;;) {
                switch (*ctx.pos) {
                    [[unlikely]] case '\t':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\r':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, uri_status::invalid_character);
                            ++ctx.pos;
                            continue;
                        }
                        set_valid(ctx.status, uri_status::valid_path);
                        break;
                    case '\\': set_warning(ctx.status, uri_status::reverse_solidus_used); [[fallthrough]];
                    case '/':
                    default: set_valid(ctx.status, uri_status::valid_path); break;
                }
                break;
            }
        } else {
            for (;;) {
                switch (*ctx.pos) {
                    case '?':
                        if constexpr (Options.parse_queries) {
                            set_valid(ctx.status, uri_status::valid_queries);
                            ++ctx.pos;
                            clear<components::queries>(ctx);
                        } else {
                            set_warning(ctx.status, uri_status::invalid_character);
                        }
                        return;
                    case '#':
                        if constexpr (Options.parse_fragment) {
                            set_valid(ctx.status, uri_status::valid_fragment);
                            ++ctx.pos;
                            clear<components::fragment>(ctx);
                        } else {
                            set_warning(ctx.status, uri_status::invalid_character);
                        }
                        return;
                    [[unlikely]] case '\t':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\r':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, uri_status::invalid_character);
                            ++ctx.pos;
                            continue;
                        }
                        [[fallthrough]];
                        default: set_valid(ctx.status, uri_status::valid_path);
                        clear<components::path>(ctx);
                        break;
                }
                break;
            }
        }
    }

    /**
     * @brief Parse authority part of the URI (credentials, host, and port)
     * @param ctx Parsing Context containing all the details of the URI and the state of it
     */
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_authority(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // We merged the host parser and authority parser to make it single-pass for most
        // use cases. https://url.spec.whatwg.org/#authority-state
        // https://url.spec.whatwg.org/#host-state

        using enum uri_status;


        if (ctx.pos == ctx.end) {
            if constexpr (Options.empty_host_is_error) {
                set_error(ctx.status, host_missing);
            } else {
                set_valid(ctx.status, valid);
            }
            return;
        }

        if constexpr (Options.allow_file_hosts) {
            if (is_file_scheme(ctx.scheme)) {
                // todo: should we set the status instead?
                parse_file_host(ctx);
                return;
            }
        }

        // Handle missing host situation, and ipv6:
        // attention: since we have merge the authority and host parsing, it's possible to
        // have something like "http://username@:8080/" which the host is missing too
        for (;;) {
            switch (*ctx.pos) {
                case ':':
                    if constexpr (!Options.parse_credentails) {
                        if constexpr (Options.empty_host_is_error) {
                            set_error(ctx.status, host_missing);
                        } else {
                            set_valid(ctx.status, valid);
                        }
                        return;
                    }
                    break;
                case '\0':
                    if constexpr (Options.eof_is_valid) {
                        break;
                    } else {
                        if constexpr (Options.empty_host_is_error) {
                            set_error(ctx.status, host_missing);
                        } else {
                            set_valid(ctx.status, valid);
                        }
                        return;
                    }
                case '?':
                    if (!is_special_scheme(ctx.scheme)) {
                        break;
                    }
                    [[fallthrough]];
                case '\\':
                case '/':
                case '#':
                    if constexpr (Options.empty_host_is_error) {
                        if (is_special_scheme(ctx.scheme)) {
                            set_error(ctx.status, host_missing);
                            return;
                        }
                        set_valid(ctx.status, valid);
                    } else {
                        set_valid(ctx.status, valid);
                        return;
                    }
                    break;
                [[unlikely]] case '\t':
                [[unlikely]] case '\n':
                [[unlikely]] case '\r':
                    if constexpr (Options.ignore_tabs_or_newlines) {
                        set_warning(ctx.status, uri_status::invalid_character);
                        ++ctx.pos;
                        continue;
                    }
                    [[fallthrough]];
                    default: break;
            }
            break;
        }

        if (!is_special_scheme(ctx.scheme)) {
            details::parse_authority_pieces<Options, false>(ctx);
            return;
        }

        details::parse_authority_pieces<Options, true>(ctx);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_AUTHORITY_HPP
