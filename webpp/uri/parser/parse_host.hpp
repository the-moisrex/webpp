// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_HOST_HPP
#define WEBPP_URI_PARSE_HOST_HPP

#include "../uri_status.hpp"
#include "./parse_authority_pieces.hpp"
#include "./uri_components.hpp"
#include "./windows_drive_letter.hpp"

namespace webpp::uri {

    /**
     * https://url.spec.whatwg.org/#file-host-state
     */
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_file_host(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using ctx_type  = CtxT;
        using char_type = typename ctx_type::char_type;

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

        if constexpr (Options.handle_windows_drive_letters) {
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
            if (details::starts_with_windows_driver_letter<Options>(ctx.pos, ctx.end)) {
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
            }
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_HOST_HPP
