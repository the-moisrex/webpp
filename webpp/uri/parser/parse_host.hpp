// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_HOST_HPP
#define WEBPP_URI_PARSE_HOST_HPP

#include "../uri_status.hpp"
#include "./parse_authority_pieces.hpp"
#include "./uri_components.hpp"
#include "./windows_drive_letter.hpp"

namespace webpp::uri {

    template <istl::StringLike T>
    [[nodiscard]] static constexpr bool is_localhost_string(T&& host) noexcept {
        using char_type = istl::char_type_of_t<T>;
        return iiequals_fl<details::TABS_OR_NEWLINES<char_type>>("localhost", stl::forward<T>(host));
    }

    /**
     * https://url.spec.whatwg.org/#file-host-state
     */
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_file_host(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        static_assert(Options.allow_file_hosts,
                      "This function should not be reached if hosts in 'file://' scheme are not allowed.");


        // handling tabs and newlines
        // if constexpr (Options.ignore_tabs_or_newlines) {
        //     while (ctx.pos != ctx.end) {
        //         switch (*ctx.pos) {
        //             [[unlikely]] case '\t':
        //             [[unlikely]] case '\n':
        //             [[unlikely]] case '\r':
        //                 set_warning(ctx.status, uri_status::invalid_character);
        //                 ++ctx.pos;
        //                 continue;
        //             default: break;
        //         }
        //         break;
        //     }
        // }

        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter<Options>(ctx.pos, ctx.end)) {
                for (;;) {
                    switch (*ctx.pos) {
                        case '/':
                        case '\\': break;
                        default:
                            // we have to move one back because the "path" needs to start with a "/" or a "\"
                            --ctx.pos;
                            if (ctx.pos == ctx.beg) {
                                break;
                            }
                            continue;
                    }
                    break;
                }
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
                set_valid(ctx.status, uri_status::valid_path);
                return;
            }
        }

        webpp_static_constexpr auto parsing_options = []() consteval {
            uri_parsing_options options = Options;
            options.parse_credentials   = false;
            options.empty_host_is_error = false;
            options.parse_port          = false;
            return options;
        }();
        details::parse_authority_pieces<parsing_options>(ctx);

        if (has_value<components::host>(ctx)) {
            if (is_localhost_string(get_component<components::host>(ctx))) {
                clear<components::host>(ctx);
            }
        }
        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter<Options>(ctx.pos, ctx.end)) {
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
            }
        }
    }

    template <uri_parsing_options Options, ParsingURIContext CtxT, typename Iter = typename CtxT::iterator>
    static constexpr void host_parser(CtxT& ctx, Iter pos, Iter end) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#concept-host-parser
        using enum uri_status;


        // handle the leading characters:
        for (;; ++pos) {
            if (pos == end) {
                // todo
                return;
            }
            switch (*pos) {
                case '[':
                    // If input starts with U+005B '[', then
                    if (*stl::prev(end) != ']') {
                        set_error(ctx.status, ipv6_unclosed);
                        return;
                    }
                    stl::ignore = details::parse_host_ipv6(ctx);
                    return;
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t':
                    if constexpr (Options.ignore_tabs_or_newlines) {
                        continue;
                    }
                    [[fallthrough]];
                default: break;
            }
            break;
        }
    }

    template <uri_parsing_options Options, ParsingURIContext CtxT>
    static constexpr void parse_hostname(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#host-state
        // https://url.spec.whatwg.org/#hostname-state

        using enum uri_status;

        // If state override is given and url’s scheme is "file", then decrease pointer by 1 and set state to
        // file host state.
        if constexpr (Options.state_override) {
            if (is_file_scheme(ctx.status)) {
                set_valid(ctx.status, valid_file_host);
                return;
            }
        }

        bool inside_brackets = false;
        for (;; ++ctx.pos) {
            switch (*ctx.pos) {
                case ':': {
                    if (!inside_brackets) {
                        // todo
                    }
                }
                case '\\':
                    if (!is_special_scheme(ctx.status)) {
                        // todo: append to buffer
                        break;
                    }
                    [[fallthrough]];
                case '/':
                case '?':
                case '#':
                case '?':
                case '\0': continue; // todo
                case '[':
                case ']': inside_brackets = *ctx.pos == '['; [[fallthrough]];
                default: break;
            }
            // append to the buffer
        }
    }


} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_HOST_HPP
