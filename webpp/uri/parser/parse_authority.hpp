// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../uri_status.hpp"
#include "./parse_authority_pieces.hpp"

/**
 * Attention: User infos in URIs are DEPRECATED, but we SHOULD parse it anyway
 *   https://httpwg.org/specs/rfc9110.html#http.userinfo
 */
namespace webpp::uri {

    /**
     * @brief Parse authority part of the URI (credentials, host, and port)
     * @param ctx Parsing Context containing all the details of the URI and the state of it
     */
    template <uri_options Options = uri_options{}, URIContext CtxT>
    static constexpr void parse_authority(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // We merged the host parser and authority parser to make it single-pass for most
        // use cases. https://url.spec.whatwg.org/#authority-state
        // https://url.spec.whatwg.org/#host-state

        using enum uri_status;

        if (ctx.pos == ctx.end) {
            set_error_if<Options.empty_host_is_error>(ctx.status, host_missing);
            return;
        }

        if constexpr (Options.allow_file_hosts) {
            if (is_file_scheme(ctx.status)) {
                set_valid(ctx.status, valid_file_host);
                return;
            }
        }

        // Handle missing host situation:
        // attention: since we have merged the authority and host parsing, it's possible to
        // have something like "http://username@:8080/" which the host is missing too
        for (;;) {
            switch (*ctx.pos) {
                case ':':
                    if constexpr (!Options.parse_credentials) {
                        set_error_if<Options.empty_host_is_error>(ctx.status, host_missing);
                        return;
                    }
                    break;
                case '?':
                    if (!is_special_scheme(ctx.status)) {
                        break;
                    }
                    [[fallthrough]];
                case '\\':
                case '/':
                case '#':
                    if constexpr (Options.empty_host_is_error) {
                        if (is_special_scheme(ctx.status)) {
                            set_error(ctx.status, host_missing);
                            return;
                        }
                        set_valid(ctx.status, valid);
                    } else {
                        set_valid(ctx.status, valid);
                        return;
                    }
                    break;
                default: break;
            }
            break;
        }

        if (!is_special_scheme(ctx.status)) {
            details::parse_authority_pieces<Options, false>(ctx);
            return;
        }

        details::parse_authority_pieces<Options, true>(ctx);
    }

    /// Path start state (I like to call it authority end because it's more RFC like to
    /// say that, but WHATWG likes to call it "path start state")
    template <uri_options Options = uri_options{}, URIContext CtxT>
    static constexpr void parse_authority_end(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-start-state

        using enum uri_status;

        if (ctx.pos == ctx.end) {
            // todo: I'm guessing
            set_valid(ctx.status, valid);
            return;
        }
        if (is_special_scheme(ctx.status)) {
            for (;;) {
                switch (*ctx.pos) {
                    [[unlikely]] case '\t':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\r':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, invalid_character);
                            ++ctx.pos;
                            continue;
                        }
                        set_valid(ctx.status, valid_path);
                        break;
                    case '\\': set_warning(ctx.status, reverse_solidus_used); [[fallthrough]];
                    case '/':
                    default: set_valid(ctx.status, valid_path); break;
                }
                break;
            }
        } else {
            for (;;) {
                switch (*ctx.pos) {
                    case '?':
                        if constexpr (Options.state_override) {
                            break;
                        } else if constexpr (Options.parse_queries) {
                            set_valid(ctx.status, valid_queries);
                            ++ctx.pos;
                            clear<components::queries>(ctx);
                        } else {
                            set_warning(ctx.status, invalid_character);
                        }
                        return;
                    case '#':
                        if constexpr (Options.state_override) {
                            break;
                        } else if constexpr (Options.parse_fragment) {
                            set_valid(ctx.status, valid_fragment);
                            ++ctx.pos;
                            clear<components::fragment>(ctx);
                        } else {
                            set_warning(ctx.status, invalid_character);
                        }
                        return;
                    default:
                        set_valid(ctx.status, valid_path);
                        clear<components::path>(ctx);
                        return;
                }
                break;
            }

            // Otherwise, if state override is given and url’s host is null, append the empty string to
            // url’s path.
            if constexpr (Options.state_override) {
                if (!has_value<components::host>(ctx)) {
                    get_storage<components::path>(ctx).emplace_back();
                }
            }
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_AUTHORITY_HPP
