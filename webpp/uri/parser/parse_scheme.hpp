// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_SCHEME_HPP
#define WEBPP_URI_PARSE_SCHEME_HPP

#include "../../common/meta.hpp"
#include "../../std/utility.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/peek.hpp"
#include "../uri_status.hpp"
#include "./special_schemes.hpp"
#include "./uri_components.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {

    namespace details {


        // these exit because the user input may contain newlines and tabs so we can't use them cleanly
        template <typename CharT = char>
        static constexpr stl::array<CharT, 4> file_scheme{'f', 'i', 'l', 'e'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 4> http_scheme{'h', 't', 't', 'p'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 5> https_scheme{'h', 't', 't', 'p', 's'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 3> ftp_scheme{'f', 't', 'p'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 3> wss_scheme{'w', 's', 's'};
        template <typename CharT = char>
        static constexpr stl::array<CharT, 2> ws_scheme{'w', 's'};

        template <URIContext CtxT, stl::size_t N>
        constexpr void set_scheme(CtxT& ctx, stl::array<typename CtxT::char_type, N> const scheme)
          noexcept(CtxT::is_nothrow) {
            if constexpr (CtxT::is_modifiable) {
                set_scheme(ctx, scheme.begin(), scheme.end());
            } else {
                set_scheme(ctx, ctx.beg, ctx.pos);
            }
        }

        template <URIContext CtxT>
        constexpr void set_scheme(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            if constexpr (!CtxT::is_modifiable) {
                set_scheme(ctx, ctx.beg, ctx.pos);
            } else {
                auto& out_str = get_storage<components::scheme>(ctx);
                ascii::lower_to(out_str, ctx.beg, ctx.pos);
            }
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void relative_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // relative scheme state (https://url.spec.whatwg.org/#relative-state)
            // https://url.spec.whatwg.org/#relative-slash-state
            using enum uri_status;
            if (ctx.pos == ctx.end) {
                set(ctx.status, valid);
                return;
            }

            if constexpr (CtxT::has_base_uri) {
                // Assert base's scheme is not file
                assert(!is_file_scheme(scheme(ctx.base)));

                set_scheme(ctx, scheme(ctx.base));
            }
            switch (*ctx.pos) {
                case '/': break;
                case '\\':
                    if (is_special_scheme(ctx.status)) [[unlikely]] {
                        set_warning(ctx.status, reverse_solidus_used);
                    }
                    break;
                default: break;
            }
            ++ctx.pos;
            if (ctx.pos == ctx.end) {
                set(ctx.status, valid);
                return;
            }


            // from now on in the algorithms: relative slash state
            // https://url.spec.whatwg.org/#relative-slash-state
            if constexpr (CtxT::has_base_uri) {
                set_username(ctx, username(ctx.base));
                set_password(ctx, password(ctx.base));
                set_hostname(ctx, hostname(ctx.base));
                set_port(ctx, port(ctx.base));
                set_path(ctx, path(ctx.base)); // todo: https://infra.spec.whatwg.org/#list-clone
                set_queries(ctx, queries(ctx.base));
            }
            switch (*ctx.pos) {
                case '?':
                    clear_queries(ctx.out);
                    set(ctx.status, valid_queries);
                    ++ctx.pos;
                    return;
                case '#':
                    clear_fragment(ctx.out);
                    set(ctx.status, valid_fragment);
                    ++ctx.pos;
                    return;
                default: break;
            }
            clear_queries(ctx.out);
            // todo: https://url.spec.whatwg.org/#shorten-a-urls-path
            set(ctx.status, valid_path);
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void file_slash_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#file-slash-state
            using enum uri_status;

            using ctx_type = CtxT;
            if (ctx.pos != ctx.end) {
                switch (*ctx.pos) {
                    case '\\': set_warning(ctx.status, reverse_solidus_used); [[fallthrough]];
                    case '/': set(ctx.status, Options.allow_file_hosts ? valid_file_host : valid_path); return;
                    default: break;
                }
            }
            if constexpr (ctx_type::has_base_uri) {
                if (is_file_scheme(scheme(ctx.base))) {
                    set_scheme(ctx, scheme(ctx.base));

                    // todo:
                    // 2. If the code point substring from pointer to the end of input does not
                    //    start with a Windows drive letter and base's path[0] is a normalized
                    //    Windows drive letter, then append base's path[0] to url's path.
                    //    This is a (platform-independent) Windows drive letter quirk.
                }
            }
            set(ctx.status, valid_path);
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void file_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#file-state
            using enum uri_status;
            using char_type = typename CtxT::char_type;

            // set scheme to "file"
            set_flag(ctx.status, scheme_type::file_scheme);
            details::set_scheme(ctx, details::file_scheme<char_type>);

            // Set url’s host to the empty string.
            clear_hostname(ctx.out);

            // if constexpr (ctx_type::has_base_uri) {
            //     // set scheme to "file"
            //     set_scheme(ctx,
            //                                   scheme(ctx.base).data(),
            //                                   scheme(ctx.base).data() + scheme(ctx.base).size());
            // }

            for (;; ++ctx.pos) {
                if (ctx.pos == ctx.end) {
                    set(ctx.status, valid);
                    return;
                }

                switch (*ctx.pos) {
                    case '\\': set_warning(ctx.status, reverse_solidus_used); [[fallthrough]];
                    case '/': file_slash_state<Options>(ctx); return;
                    default: break;
                }
                if constexpr (Options.allow_file_hosts) {
                    set(ctx.status, valid_file_host);
                    return;
                }
                break;
            }

            if constexpr (CtxT::has_base_uri) {
                if (is_file_scheme(scheme(ctx.base))) {
                    // todo
                }
            }

            set(ctx.status, valid_path);
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void no_scheme_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#no-scheme-state

            using enum uri_status;
            if constexpr (CtxT::has_base_uri) {
                if (path(ctx.base)) { // todo: specs say opaque path
                    for (; ctx.pos != ctx.end; ++ctx.pos) {
                        if (*ctx.pos == '#') [[unlikely]] {
                            if constexpr (Options.parse_fragment) {
                                set_scheme(ctx, scheme(ctx.base));
                                set_path(ctx, path(ctx.base));
                                set_queries(ctx, queries(ctx.base));
                                clear_fragment(ctx.out);
                                set(ctx.status, valid_fragment);
                                return;
                            } else {
                                break;
                            }
                        }
                        break;
                    }
                } else if (!is_file_scheme(scheme(ctx.base))) {
                    relative_state<Options>(ctx);
                    return;
                } else {
                    file_state<Options>(ctx);
                    return;
                }
            }
            set(ctx.status, missing_scheme_non_relative_url);
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void special_authority_ignore_slashes_state(CtxT& ctx) noexcept {
            // special authority ignore slashes state
            // (https://url.spec.whatwg.org/#special-authority-ignore-slashes-state)
            using enum uri_status;

            for (; ctx.pos != ctx.end; ++ctx.pos) {
                switch (*ctx.pos) {
                    case '\\':
                    case '/':
                        set_warning(ctx.status, missing_following_solidus);
                        continue;
                    [[likely]] default:
                        break;
                }
                break;
            }
            set(ctx.status, valid_authority);
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void special_relative_or_authority_state(CtxT& ctx) noexcept {
            // special authority slashes state
            // (https://url.spec.whatwg.org/#special-authority-slashes-state):
            if (ascii::inc_if<Options>(ctx, '/', '/')) {
                special_authority_ignore_slashes_state<Options>(ctx);
                return;
            }
            set_warning(ctx.status, uri_status::missing_following_solidus);
            relative_state<Options>(ctx);
        }

    } // namespace details

    /**
     * Parse scheme (or sometimes called Protocol)
     */
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_scheme(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::encoded_scheme;
        using char_type = typename CtxT::char_type;
        using enum uri_status;

        webpp_static_constexpr auto alnum_plus = details::ascii_bitmap(details::ASCII_ALPHA_DIGIT, '+', '-', '.');

        // scheme start (https://url.spec.whatwg.org/#scheme-start-state)
        if (ctx.pos == ctx.end) [[unlikely]] {
            ctx.status = +empty_string;
            return;
        }

        // handling of the first character:
        if (!details::ASCII_ALPHA.contains(*ctx.pos)) [[unlikely]] {
            // if state override is not given, set buffer to the empty string, state to no
            // scheme state, and start over (from the first code point in input).
            //
            // no scheme state (https://url.spec.whatwg.org/#no-scheme-state)
            if constexpr (!Options.state_override) {
                ctx.pos = ctx.beg;
                clear_scheme(ctx.out);
                details::no_scheme_state<Options>(ctx);
            } else {
                // otherwise, return failure
                set(ctx.status, scheme_setter_invalid_input);
            }
            return;
        }

        // this is designed to find out which scheme type we're dealing with here
        stl::uint64_t scheme_code   = 0ULL;
        scheme_code                |= static_cast<stl::uint64_t>(ascii::to_lower_copy(*ctx.pos));
        scheme_code               <<= details::one_byte;
        ++ctx.pos;

        // scheme state (https://url.spec.whatwg.org/#scheme-state)
        // handling alpha, num, +, -, .
        for (;; ++ctx.pos) {
            if (ctx.pos == ctx.end) [[unlikely]] {
                set(ctx.status, scheme_ended_unexpectedly);
                return;
            }
            if (*ctx.pos == ':') {
                break;
            }
            if (!alnum_plus.contains(*ctx.pos)) [[unlikely]] {
                set(ctx.status, invalid_scheme_character);
                return;
            }
            scheme_code  |= static_cast<stl::uint64_t>(ascii::to_lower_copy(*ctx.pos));
            scheme_code <<= details::one_byte;
        }

        if (ctx.pos == ctx.end) [[unlikely]] {
            set(ctx.status, scheme_ended_unexpectedly);
            return;
        }

        // If url’s scheme is "file" and its host is an empty host, then return.
        if (Options.state_override && is_file_scheme(ctx.status) && has_hostname(ctx.out)) [[unlikely]] {
            return;
        }

        switch (scheme_code) {
            [[likely]] case encoded_scheme("http"):
                details::set_scheme(ctx, details::http_scheme<char_type>);
                break;
            [[likely]] case encoded_scheme("https"):
                details::set_scheme(ctx, details::https_scheme<char_type>);
                break;
            case encoded_scheme("ws"): details::set_scheme(ctx, details::ws_scheme<char_type>); break;
            case encoded_scheme("wss"): details::set_scheme(ctx, details::wss_scheme<char_type>); break;
            case encoded_scheme("ftp"): details::set_scheme(ctx, details::ftp_scheme<char_type>); break;
            case encoded_scheme("file"): {
                // If url includes credentials or has a non-null port, and buffer is "file", then return
                // If url’s scheme is a special scheme and buffer is not a special scheme, then return.
                if (Options.state_override &&
                    (has_flags(ctx.status, has_credentials, has_non_null_port) || !is_special_scheme(ctx.status)))
                  [[unlikely]]
                {
                    return;
                }
                ++ctx.pos;
                // If remaining does not start with "//", special-scheme-missing-following-solidus
                // validation error.
                if (!ascii::inc_if(ctx.pos, ctx.end, '/', '/')) [[unlikely]] {
                    set_warning(ctx.status, missing_following_solidus);
                }
                details::file_state<Options>(ctx);
                return;
            }
            [[unlikely]] default: {
                // If url’s scheme is not a special scheme and buffer is a special scheme, then return.
                if (Options.state_override && !is_special_scheme(ctx.status)) [[unlikely]] {
                    return;
                }

                details::set_scheme(ctx);
                ++ctx.pos;
                set_flag(ctx.status, scheme_type::not_special);
                if (ascii::inc_if(ctx.pos, ctx.end, '/')) {
                    // https://url.spec.whatwg.org/#path-or-authority-state
                    if (ascii::inc_if(ctx.pos, ctx.end, '/')) [[likely]] {
                        set(ctx.status, valid_authority);
                        return;
                    }
                    set(ctx.status, valid_path);
                    return;
                }

                clear_path(ctx.out);
                set(ctx.status, valid_opaque_path);
                return;
            }
        }

        // If url’s scheme is a special scheme and buffer is not a special scheme, then return.
        if (Options.state_override && !is_special_scheme(ctx.status)) [[unlikely]] {
            return;
        }

        ++ctx.pos;
        set_flag(ctx.status, scheme_type::special_scheme);

        if constexpr (CtxT::has_base_uri) {
            if (scheme(ctx.out) == scheme(ctx.base)) {
                // todo: Assert: base is special (and therefore does not have an opaque path).
                details::special_relative_or_authority_state<Options>(ctx);
                return;
            }
        }

        /// https://url.spec.whatwg.org/#special-authority-slashes-state
        if (!ascii::inc_if(ctx.pos, ctx.end, '/', '/')) [[unlikely]] {
            set_warning(ctx.status, missing_following_solidus);
        }
        details::special_authority_ignore_slashes_state<Options>(ctx);
    }

} // namespace webpp::uri
#endif // WEBPP_URI_PARSE_SCHEME_HPP
