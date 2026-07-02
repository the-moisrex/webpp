// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../uri_status.hpp"
#include "./parse_authority_pieces.hpp"
#include "./special_schemes.hpp"
#include "uri_components.hpp"
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
        // use cases.
        // https://url.spec.whatwg.org/#authority-state
        // https://url.spec.whatwg.org/#host-state

        using enum uri_status;

        if (ctx.pos == ctx.end) [[unlikely]] {
            set(ctx.status, host_missing);
            return;
        }

        // if (is_file_scheme(ctx.status)) {
        //     set(ctx.status, valid_file_host);
        //     return;
        // }

        // Handle missing host situation:
        // attention: since we have merged the authority and host parsing, it's possible to
        // have something like "http://username@:8080/" which the host is missing too
        switch (*ctx.pos) {
            case ':':
                if constexpr (!Options.parse_credentials) {
                    set(ctx.status, host_missing);
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
                if (is_special_scheme(ctx.status)) [[unlikely]] {
                    set(ctx.status, host_missing);
                    return;
                }
                set(ctx.status, valid);
                break;
            default: break;
        }

        details::parse_authority_pieces<Options>(ctx);
    }

    template <URIContext CtxT>
    static constexpr void parse_authority_for_real(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#authority-state
        using enum uri_status;
        using enum uri_encoding_policy;
        using details::USER_INFO_ENCODE_SET;
        using stl::begin;
        using stl::end;

        bool       at_sign_seen = false;
        auto       username     = create_buffer(ctx);
        auto       password     = create_buffer(ctx);
        auto       buffer       = create_buffer(ctx);
        auto const beg          = ctx.pos;
        for (;; ++ctx.pos) {
            char const c_val = ctx.pos != ctx.end ? *ctx.pos : '\0';
            switch (c_val) {
                case '@': {
                    // Invalid-credentials validation error.
                    set_warning(ctx.status, contains_credentials);

                    // If atSignSeen is true, then prepend "%40" to buffer.
                    if (at_sign_seen) [[unlikely]] {
                        if constexpr (!CtxT::is_modifiable) {
                            set(ctx.status, modification_required);
                            return;
                        } else {
                            buffer.push_back('%');
                            buffer.push_back('4');
                            buffer.push_back('0');
                        }
                    }

                    // Set atSignSeen to true.
                    at_sign_seen = true;

                    // For each codePoint in buffer:
                    auto const buf_beg             = begin(buffer);
                    auto const endp                = end(buffer);
                    bool       password_token_seen = false;
                    for (auto pos = beg; pos != endp; ++pos) {
                        // If codePoint is U+003A (:) and passwordTokenSeen is false, then set passwordTokenSeen to true
                        // and continue.
                        if (*pos == ':' && !password_token_seen) {
                            password_token_seen = true;
                            continue;
                        }

                        // Let encodedCodePoints be the result of running UTF-8 percent-encode codePoint using the
                        // userinfo percent-encode set.
                        // If passwordTokenSeen is true, then append encodedCodePoints to url’s password.
                        // Otherwise, append encodedCodePoints to url’s username.
                        auto& encoded_buffer = password_token_seen ? password : username;
                        if constexpr (CtxT::is_modifiable) {
                            encode_uri_component<encode_chars>(*pos, encoded_buffer, USER_INFO_ENCODE_SET);
                        } else if (!USER_INFO_ENCODE_SET.contains(*pos)) {
                            // validation-only mode
                            encoded_buffer.beg = buf_beg;
                            encoded_buffer.end = pos;
                        } else [[unlikely]] {
                            set(ctx.status, modification_required);
                            return;
                        }
                    }

                    // Set buffer to the empty string.
                    clear_segment(ctx, buffer);
                    continue;
                }
                case '\\':
                    // url is special and c is U+005C (\)
                    if (!is_special_scheme(ctx.status)) {
                        break;
                    }
                    [[fallthrough]];
                case '/':
                case '?':
                case '#':
                case '\0':

                    // If atSignSeen is true and buffer is the empty string, host-missing validation error, return
                    // failure.
                    if (at_sign_seen && is_empty(buffer)) [[unlikely]] {
                        set(ctx.status, host_missing);
                        return;
                    }

                    // Decrease pointer by buffer’s code point length + 1, set buffer to the empty string, and set state
                    // to host state.
                    ctx.pos = beg;
                    set(ctx.status, valid_host);
                    return;
                default: break;
            }

            // Otherwise, append c to buffer.
            if (CtxT::is_modifiable) {
                buffer.push_back(*ctx.pos);
            } else {
                ++buffer.end;
            }
        }
    }

    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_authority2(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#authority-state

        using enum uri_status;

        if (ctx.pos == ctx.end) [[unlikely]] {
            // If atSignSeen is true and buffer is the empty string, host-missing validation error, return failure.
            set(ctx.status, host_missing);
            return;
        }

        assert(!has_error(ctx.status));
        auto const beg = ctx.pos;

        // we go against the standard and first pretend the URL has no authority, but if things fail, we're gonna
        // rollback, parse the authority, and then try again with the host parser.
        host_parser<Options>(ctx);

        if (!has_error(ctx.status)) [[likely]] {
            return;
        }

        // rollback
        clear_hostname(ctx.out);
        set(ctx.status, valid_authority);
        ctx.pos = beg;

        // now we parse authority
        parse_authority_for_real(ctx);

        // try again
        host_parser<Options>(ctx);
    }

    /// Path start state (I like to call it authority end because it's more RFC like to
    /// say that, but WHATWG likes to call it "path start state")
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_path_start(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-start-state

        using enum uri_status;

        if (is_special_scheme(ctx.status)) [[likely]] {
            if (ctx.pos != ctx.end) {
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
            }
            set(ctx.status, valid_path);
            return;
        }

        if constexpr (Options.state_override) {
            if (ctx.pos == ctx.end && !has_flags(ctx.status, has_non_null_host)) {
                // Otherwise, if state override is given and url’s host is null, append the empty string to url’s path
                // For owning/non-segregated components this is represented as a single '/'.
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
                set(ctx.status, valid);
                return;
            }
        } else if (ctx.pos == ctx.end) {
            // state override is not give, c is EOF, and the URL is not special.
            set(ctx.status, valid);
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
                    return;
                case '#':
                    if constexpr (Options.parse_fragment) {
                        ++ctx.pos;
                        clear_fragment(ctx.out);
                        set_flag(ctx.status, has_non_null_fragment);
                    }
                    set(ctx.status, valid_fragment);
                    return;
                default: break;
            }
        }

        // Otherwise, if c is not the EOF code point:
        if (*ctx.pos == '/') {
            // If c is not U+002F (/), then decrease pointer by 1.
            ++ctx.pos;
        }
        set(ctx.status, valid_path);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_AUTHORITY_HPP
