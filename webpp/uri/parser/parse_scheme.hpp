// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_SCHEME_HPP
#define WEBPP_URI_PARSE_SCHEME_HPP

#include "../../common/meta.hpp"
#include "../../std/utility.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/peek.hpp"
#include "../uri_status.hpp"
#include "./parse_path.hpp"
#include "./special_schemes.hpp"
#include "./uri_components.hpp"
#include "./uri_context.hpp"
#include "./windows_drive_letter.hpp"

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
                set_scheme(ctx.out, create_buffer(ctx, scheme.begin(), scheme.end()));
            } else {
                set_scheme(ctx.out, create_buffer(ctx, ctx.beg, ctx.pos));
            }
        }

        template <URIContext CtxT, typename ValueType>
        constexpr auto base_component_buffer(CtxT& ctx, ValueType const& value) noexcept(CtxT::is_nothrow) {
            // ValueType can be one of (vec_type, map_type, seg_type)
            return create_buffer(ctx, value.begin(), value.end());
        }

        template <URIContext CtxT>
        constexpr void set_scheme(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            if constexpr (!CtxT::is_modifiable) {
                set_scheme(ctx.out, create_buffer(ctx, ctx.beg, ctx.pos));
            } else {
                auto out_str = create_buffer(ctx);
                ascii::lower_to(out_str, ctx.beg, ctx.pos);
                set_scheme(ctx.out, stl::move(out_str));
            }
        }

        template <URIContext CtxT>
        static constexpr void copy_from_base(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            using enum uri_status;

            if constexpr (!stl::is_void_v<typename CtxT::base_type>) {
                set_username(ctx.out, base_component_buffer(ctx, username(ctx.base)));
                set_password(ctx.out, base_component_buffer(ctx, password(ctx.base)));
                set_hostname(ctx.out, base_component_buffer(ctx, hostname(ctx.base)));
                set_port(ctx.out, base_component_buffer(ctx, port(ctx.base)));
                set_path(
                  ctx.out,
                  base_component_buffer(ctx, path_view(ctx.base))); // todo: https://infra.spec.whatwg.org/#list-clone
                set_queries(ctx.out, base_component_buffer(ctx, queries_view(ctx.base)));
                if (has_hostname(ctx.base)) {
                    set_flag(ctx.status, has_non_null_host);
                }
                if (has_queries(ctx.out)) {
                    set_flag(ctx.status, has_non_null_queries);
                }
            }
        }

        template <URIContext CtxT>
        static constexpr void special_authority_ignore_slashes_state(CtxT& ctx) noexcept {
            // https://url.spec.whatwg.org/#special-authority-ignore-slashes-state
            using enum uri_status;

            for (; ctx.pos != ctx.end; ++ctx.pos) {
                switch (*ctx.pos) {
                    case '\\':
                    case '/':
                        // Otherwise, special-scheme-missing-following-solidus validation error.
                        set_warning(ctx.status, missing_following_solidus);
                        continue;
                    [[likely]] default:
                        break;
                }
                break;
            }
            // If c is neither U+002F (/) nor U+005C (\), then set state to authority state and decrease pointer by 1.
            set(ctx.status, valid_authority);
        }

        template <URIContext CtxT>
        static constexpr void relative_slash_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#relative-slash-state

            using enum uri_status;

            if (ctx.pos == ctx.end) [[unlikely]] {
                set(ctx.status, valid);
                return;
            }

            // If url is special and c is U+002F (/) or U+005C (\), then:
            if (is_special_scheme(ctx.status) && (*ctx.pos == '/' || *ctx.pos == '\\')) {
                if (*ctx.pos == '\\') [[unlikely]] {
                    set_warning(ctx.status, reverse_solidus_used);
                }
                ++ctx.pos;
                // Set state to special authority ignore slashes state.
                special_authority_ignore_slashes_state(ctx);
                return;
            }

            // Otherwise, if c is U+002F (/), then set state to authority state.
            if (*ctx.pos == '/') {
                ++ctx.pos;
                set(ctx.status, valid_authority);
                return;
            }

            // Otherwise, set url’s username to base’s username, url’s password to base’s password, url’s host to
            // base’s host, url’s port to base’s port, state to path state, and then, decrease pointer by 1.
            copy_from_base(ctx);
            set(ctx.status, ctx.pos == ctx.end ? valid : valid_path);
        }

        template <URIContext CtxT>
        static constexpr void relative_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // relative scheme state (https://url.spec.whatwg.org/#relative-state)
            // https://url.spec.whatwg.org/#relative-slash-state
            using enum uri_status;

            if constexpr (!stl::is_void_v<typename CtxT::base_type>) {
                // Assert base's scheme is not file
                assert(!is_file_scheme(scheme(ctx.base)));

                // Set url’s scheme to base’s scheme.
                set_scheme(ctx.out, base_component_buffer(ctx, scheme(ctx.base)));
            }

            if (ctx.pos == ctx.end) [[unlikely]] {
                set(ctx.status, valid);
                return;
            }

            // If c is U+002F (/), then set state to relative slash state.
            // Otherwise... set url’s username, password, host, port, path, and query to base’s ...
            if (*ctx.pos == '/' || (*ctx.pos == '\\' && is_special_scheme(ctx.status))) {
                if (*ctx.pos == '\\') [[unlikely]] {
                    set_warning(ctx.status, reverse_solidus_used);
                }
                ++ctx.pos;
                relative_slash_state(ctx);
                return;
            }


            // from now on in the algorithms: relative slash state
            // https://url.spec.whatwg.org/#relative-slash-state
            copy_from_base(ctx);

            switch (*ctx.pos) {
                case '?':
                    // If c is U+003F (?), then set url’s query to the empty string, and state to query state.
                    clear_queries(ctx.out);
                    set_flag(ctx.status, has_non_null_queries);
                    set(ctx.status, valid_queries);
                    ++ctx.pos;
                    return;
                case '#':
                    // Otherwise, if c is U+0023 (#), set url’s fragment to the empty string and state to fragment
                    // state.
                    clear_fragment(ctx.out);
                    set_flag(ctx.status, has_non_null_fragment);
                    set(ctx.status, valid_fragment);
                    ++ctx.pos;
                    return;
                default: break;
            }
            // Otherwise, if c is not the EOF code point:
            //   - Set url’s query to null.
            //   - Shorten url’s path.
            //   - Set state to path state and decrease pointer by 1.
            clear_queries(ctx.out);
            unset_flag(ctx.status, has_non_null_queries);
            details::shorten_urls_path(ctx);
            set(ctx.status, valid_path);
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void file_slash_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#file-slash-state
            using enum uri_status;

            if (ctx.pos == ctx.end) [[unlikely]] {
                set(ctx.status, valid);
                return;
            }

            // Our file scheme might have a host for some reason!
            switch (*ctx.pos) {
                case '\\': set_warning(ctx.status, reverse_solidus_used); [[fallthrough]];
                case '/':
                    ++ctx.pos;
                    set(ctx.status, Options.allow_file_hosts ? valid_file_host : valid_path);
                    return;
                default: break;
            }

            if constexpr (!stl::is_void_v<typename CtxT::base_type>) {
                if (is_file_scheme(scheme(ctx.base))) {
                    set_scheme(ctx.out, base_component_buffer(ctx, scheme(ctx.base)));

                    if constexpr (Options.handle_windows_drive_letters) {
                        // 2. If the code point substring from pointer to the end of input does not
                        //    start with a Windows drive letter and base's path[0] is a normalized
                        //    Windows drive letter, then append base's path[0] to url's path.
                        //    This is a (platform-independent) Windows drive letter quirk.
                        if (has_path(ctx.base) && !details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) {
                            auto const first_seg = details::first_path_segment(ctx.base);
                            if (first_seg.size() >= 2 &&
                                details::has_normalized_windows_driver_letter(first_seg.begin())) [[unlikely]]
                            {
                                if constexpr (CtxT::is_modifiable) {
                                    push_segment(uri::path(ctx.out), first_seg);
                                } else {
                                    set(ctx.status, modification_required);
                                    return;
                                }
                            }
                        }
                    }
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
            // Empty string != null
            clear_hostname(ctx.out);
            set_flag(ctx.status, has_non_null_host);

            if (ctx.pos == ctx.end) [[unlikely]] {
                set(ctx.status, valid);
                return;
            }

            switch (*ctx.pos) {
                [[unlikely]] case '\\':
                    set_warning(ctx.status, reverse_solidus_used);
                    [[fallthrough]];
                case '/':
                    ++ctx.pos;
                    file_slash_state<Options>(ctx);
                    return;
                default: break;
            }

            if constexpr (!stl::is_void_v<typename CtxT::base_type>) {
                if (is_file_scheme(scheme(ctx.base))) {
                    // Set url’s host to base’s host, url’s path to a clone of base’s path,
                    // and url’s query to base’s query.
                    set_hostname(ctx.out, base_component_buffer(ctx, hostname(ctx.base)));
                    set_path(ctx.out, base_component_buffer(ctx, path_view(ctx.base))); // list clone
                    set_flag(ctx.status, file_scheme);
                    set_queries(ctx.out, base_component_buffer(ctx, queries_view(ctx.base)));
                    if (has_hostname(ctx.base)) {
                        set_flag(ctx.status, has_non_null_host);
                    }
                    if (has_queries(ctx.out)) {
                        set_flag(ctx.status, has_non_null_queries);
                    }

                    // If c is U+003F (?), then set url’s query to the empty string and state to query state.
                    // Otherwise, if c is U+0023 (#), set url’s fragment to the empty string and state to fragment
                    // state.
                    switch (*ctx.pos) {
                        case '?':
                            clear_queries(ctx.out);
                            unset_flag(ctx.status, has_non_null_queries);
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

                    // Otherwise, if c is not the EOF code point: Set url’s query to null.
                    clear_queries(ctx.out);
                    unset_flag(ctx.status, has_non_null_queries);

                    // If the code point substring from pointer to the end of input does not start
                    // with a Windows drive letter, then shorten url’s path.
                    // Otherwise: File-invalid-Windows-drive-letter validation error.
                    // Set url’s path to « ».
                    if constexpr (Options.handle_windows_drive_letters) {
                        if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) [[unlikely]] {
                            set_warning(ctx.status, windows_drive_letter_in_relative_url);
                            clear_path(ctx.out);
                        } else {
                            details::shorten_urls_path(ctx);
                        }
                    } else {
                        details::shorten_urls_path(ctx);
                    }

                    // Set state to path state and decrease pointer by 1.
                    // Pointer adjustment is implicit in this parser architecture; `ctx.pos` remains
                    // on the current code point and `parse_path` consumes it next.
                    set(ctx.status, valid_path);
                    return;
                }
            }

            set(ctx.status, Options.allow_file_hosts ? valid_file_host : valid_path);
        }

        template <uri_options Options, URIContext CtxT>
        static constexpr void no_scheme_state(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            // https://url.spec.whatwg.org/#no-scheme-state

            using enum uri_status;
            if constexpr (!stl::is_void_v<typename CtxT::base_type>) {
                auto const base_scheme = scheme(ctx.base);

                if (ctx.pos != ctx.end && is_opaque_path(ctx.base)) {
                    if (*ctx.pos == '#') {
                        // Otherwise, if base has an opaque path and c is U+0023 (#), set url’s scheme to base’s scheme,
                        // url’s path to base’s path, url’s query to base’s query, url’s fragment to the empty string,
                        // and set state to fragment state.
                        set_scheme(ctx.out, base_component_buffer(ctx, base_scheme));
                        set_path(ctx.out, base_component_buffer(ctx, path_view(ctx.base)));
                        set_flag(ctx.status, opaque_path);
                        set_queries(ctx.out, base_component_buffer(ctx, queries_view(ctx.base)));
                        if (!queries(ctx.out).empty()) {
                            set_flag(ctx.status, has_non_null_queries);
                        }
                        clear_fragment(ctx.out);
                        set(ctx.status, valid_fragment);
                        ++ctx.pos;
                        return;
                    }
                    // ... or base has an opaque path and c is not U+0023 (#), missing-scheme-non-relative-URL
                    // validation error, return failure.
                } else if (!is_file_scheme(base_scheme)) {
                    // Otherwise, if base’s scheme is not "file", set state to relative state and decrease pointer by 1.
                    relative_state(ctx);
                    return;
                } else {
                    // Otherwise, set state to file state and decrease pointer by 1.
                    file_state<Options>(ctx);
                    return;
                }
            }

            // If base is null, or ..., missing-scheme-non-relative-URL validation error, return failure.
            set(ctx.status, ctx.pos == ctx.end ? empty_string : missing_scheme_non_relative_url);
        }

        template <URIContext CtxT>
        static constexpr void special_authority_slashes_state(CtxT& ctx) noexcept {
            // https://url.spec.whatwg.org/#special-authority-slashes-state

            // If c is U+002F (/) and remaining starts with U+002F (/), then set state to special authority ignore
            // slashes state and increase pointer by 1.
            if (ascii::inc_if(ctx.pos, ctx.end, '/', '/')) {
                special_authority_ignore_slashes_state(ctx);
                return;
            }
            // Otherwise, special-scheme-missing-following-solidus validation error, set state to special authority
            // ignore slashes state and decrease pointer by 1.
            set_warning(ctx.status, uri_status::missing_following_solidus);
            special_authority_ignore_slashes_state(ctx);
        }

        template <URIContext CtxT>
        static constexpr void special_relative_or_authority_state(CtxT& ctx) noexcept {
            // https://url.spec.whatwg.org/#special-relative-or-authority-state

            // If c is U+002F (/) and remaining starts with U+002F (/), then set state to special authority ignore
            // slashes state and increase pointer by 1.
            if (ascii::inc_if(ctx.pos, ctx.end, '/', '/')) {
                special_authority_ignore_slashes_state(ctx);
                return;
            }

            // Otherwise, special-scheme-missing-following-solidus validation error, set state to relative state and
            // decrease pointer by 1.
            set_warning(ctx.status, uri_status::missing_following_solidus);
            relative_state(ctx);
        }

    } // namespace details

    /**
     * Parse scheme (or sometimes called Protocol)
     */
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_scheme(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::encoded_scheme;
        using enum uri_status;
        using char_type = typename CtxT::char_type;

        webpp_static_constexpr auto alnum_plus = details::ascii_bitmap(details::ASCII_ALPHA_DIGIT, '+', '-', '.');

        // scheme start (https://url.spec.whatwg.org/#scheme-start-state)
        // WHATWG: "If c is not an ASCII alpha, then ... set state to no scheme state, and decrease pointer by 1."
        // EOF also needs to continue into no-scheme resolution when a base URL exists.
        // handling of the first character:
        if (ctx.pos == ctx.end || !details::ASCII_ALPHA.contains(*ctx.pos)) [[unlikely]] {
            // if state override is not given, set buffer to the empty string, state to no
            // scheme state, and start over (from the first code point in input).
            //
            // no scheme state (https://url.spec.whatwg.org/#no-scheme-state)
            if constexpr (!Options.state_override) {
                clear_scheme(ctx.out);
                details::no_scheme_state<Options>(ctx);
            } else {
                // otherwise, return failure
                set(ctx.status, ctx.pos == ctx.end ? empty_string : scheme_setter_invalid_input);
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
                if constexpr (!Options.state_override) {
                    clear_scheme(ctx.out);
                    ctx.pos = ctx.beg;
                    details::no_scheme_state<Options>(ctx);
                } else {
                    set(ctx.status, scheme_ended_unexpectedly);
                }
                return;
            }
            if (*ctx.pos == ':') {
                break;
            }
            if (!alnum_plus.contains(*ctx.pos)) [[unlikely]] {
                if constexpr (!Options.state_override) {
                    clear_scheme(ctx.out);
                    ctx.pos = ctx.beg;
                    details::no_scheme_state<Options>(ctx);
                } else {
                    set(ctx.status, invalid_scheme_character);
                }
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
                    (has_flags(ctx.status, contains_credentials, has_non_null_port) || !is_special_scheme(ctx.status)))
                  [[unlikely]]
                {
                    return;
                }
                ++ctx.pos;
                // If remaining does not start with "//", special-scheme-missing-following-solidus
                // validation error.
                auto* pos_copy = ctx.pos;
                if (!ascii::inc_if(2U, pos_copy, ctx.end, '/', '\\')) [[unlikely]] {
                    set_warning(ctx.status, missing_following_solidus);
                }
                details::file_state<Options>(ctx);
                return;
            }
            [[unlikely]] default: {
                // If url’s scheme is a special scheme and buffer is not a special scheme, then return.
                if (Options.state_override && is_special_scheme(ctx.status)) [[unlikely]] {
                    return;
                }

                details::set_scheme(ctx);
                ++ctx.pos; // colon
                set_flag(ctx.status, scheme_type::not_special);

                // Otherwise, if remaining starts with an U+002F (/), set state to path or authority state and increase
                // pointer by 1.
                if (ascii::inc_if(ctx.pos, ctx.end, '/')) {
                    // https://url.spec.whatwg.org/#path-or-authority-state
                    // If c is U+002F (/), then set state to authority state.
                    if (ascii::inc_if(ctx.pos, ctx.end, '/')) [[likely]] {
                        set(ctx.status, valid_authority);
                        return;
                    }
                    set(ctx.status, valid_path);
                    return;
                }

                // Otherwise, set url’s path to the empty string and set state to opaque path state.
                clear_path(ctx.out);
                set(ctx.status, valid_opaque_path);
                return;
            }
        }

        ++ctx.pos;
        set_flag(ctx.status, scheme_type::special_scheme);

        // Otherwise, if url is special, base is non-null, and base’s scheme is url’s scheme:
        if constexpr (!stl::is_void_v<typename CtxT::base_type>) {
            if (scheme(ctx.out) == scheme(ctx.base)) {
                // todo: Assert: base is special (and therefore does not have an opaque path).
                details::special_relative_or_authority_state(ctx);
                return;
            }
        }

        // Otherwise, if url is special, set state to special authority slashes state.
        details::special_authority_slashes_state(ctx);
    }

} // namespace webpp::uri
#endif // WEBPP_URI_PARSE_SCHEME_HPP
