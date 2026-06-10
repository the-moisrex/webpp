// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../uri_status.hpp"
#include "./parse_authority_pieces.hpp"
#include "./special_schemes.hpp"
#include "uri_context.hpp"

/**
 * Attention: User infos in URIs are DEPRECATED, but we SHOULD parse it anyway
 *   https://httpwg.org/specs/rfc9110.html#http.userinfo
 */
namespace webpp::uri {

    /**
     * @brief Parse authority part of the URI (credentials, host, and port)
     * @param ctx Parsing Context containing all the details of the URI and the state of it
     */
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_authority(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // We merged the host parser and authority parser to make it single-pass for most
        // use cases. https://url.spec.whatwg.org/#authority-state
        // https://url.spec.whatwg.org/#host-state

        using enum uri_status;

        if (ctx.pos == ctx.end) [[unlikely]] {
            set(ctx.status, Options.empty_host_is_error ? host_missing : valid);
            return;
        }

        if (Options.allow_file_hosts && is_file_scheme(ctx.status)) {
            set(ctx.status, valid_file_host);
            return;
        }

        // Handle missing host situation:
        // attention: since we have merged the authority and host parsing, it's possible to
        // have something like "http://username@:8080/" which the host is missing too
        switch (*ctx.pos) {
            case ':':
                if constexpr (!Options.parse_credentials) {
                    set_if<Options.empty_host_is_error>(ctx.status, host_missing);
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
                    if (is_special_scheme(ctx.status)) [[unlikely]] {
                        set(ctx.status, host_missing);
                        return;
                    }
                    set(ctx.status, valid);
                } else {
                    set(ctx.status, valid);
                    return;
                }
                break;
            default: break;
        }

        details::parse_authority_pieces<Options>(ctx);
    }

    /// Path start state (I like to call it authority end because it's more RFC like to
    /// say that, but WHATWG likes to call it "path start state")
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_path_start(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-start-state

        using enum uri_status;

        if (ctx.pos == ctx.end) {
            // Otherwise, if state override is given and url’s host is null, append the empty string to url’s path
            // For owning/non-segregated components this is represented as a single '/'.
            if (is_special_scheme(scheme(ctx.out)) && has_flags(ctx.status, has_non_null_host)) {
                auto buffer = create_buffer(ctx);
                if constexpr (CtxT::is_segregated) {
                    ++ctx.pos;
                    push_segment(path(ctx.out), stl::move(buffer));
                } else if constexpr (CtxT::is_modifiable) {
                    buffer.push_back('/');
                    ++ctx.pos;
                    set_path(ctx.out, stl::move(buffer));
                } else {
                    set(ctx.status, modification_required);
                    return;
                }
            }
            set(ctx.status, valid);
            return;
        }

        if (is_special_scheme(ctx.status)) [[likely]] {
            switch (*ctx.pos) {
                [[unlikely]] case '\\':
                    set_warning(ctx.status, reverse_solidus_used);
                    [[fallthrough]];
                case '/':
                    // If c is neither U+002F (/) nor U+005C (\), then decrease pointer by 1.
                    // Which means we have to ++ctx.pos otherwise.
                    ++ctx.pos;
                    break;
                default: break;
            }
            set(ctx.status, valid_path);
            return;
        }

        if constexpr (!Options.state_override) {
            switch (*ctx.pos) {
                case '?':
                    if constexpr (Options.parse_queries) {
                        ++ctx.pos;
                        clear_queries(ctx.out);
                        set_flag(ctx.status, has_non_null_queries);
                    }
                    set(ctx.status, valid_queries);
                    break;
                case '#':
                    if constexpr (Options.parse_fragment) {
                        ++ctx.pos;
                        clear_fragment(ctx.out);
                        set_flag(ctx.status, has_non_null_fragment);
                    }
                    set(ctx.status, valid_fragment);
                    break;
                default: break;
            }
        } else if (!has_flags(ctx.status, has_non_null_host) && ctx.pos == ctx.end) {
            // Otherwise, if state override is given and url’s host is null, append the empty string to
            // url’s path.
            if constexpr (!CtxT::is_modifiable) {
                set(ctx.status, modification_required);
            } else if constexpr (CtxT::is_segregated) {
                push_segment(path(ctx.out), create_buffer(ctx));
            } else {
                path(ctx.out).push_back('/');
            }
            set(ctx.status, valid);
            return;
        }

        // if (*ctx.pos == '/') {
        //     // If c is not U+002F (/), then decrease pointer by 1.
        //     ++ctx.pos;
        // }
        set(ctx.status, valid_path);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_AUTHORITY_HPP
