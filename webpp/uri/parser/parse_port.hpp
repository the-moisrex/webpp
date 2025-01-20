// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_PORT_HPP
#define WEBPP_URI_PARSE_PORT_HPP

#include "./special_schemes.hpp"
#include "./uri_components.hpp"

namespace webpp::uri {

    static constexpr stl::uint16_t max_port_number       = 65'535U;
    static constexpr stl::uint16_t well_known_upper_port = 1024;

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_port(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#port-state

        using enum uri_status;

        using port_type = stl::uint32_t; // we use a bigger size to detect overflows from 65535-99999

        if (ctx.pos == ctx.end) {
            // It's still valid:
            //   scheme://example.com:
            set_valid(ctx.status, valid);
            return;
        }

        if constexpr (!Options.parse_port) {
            set_warning(ctx.status, invalid_character);
        } else {
            auto      beg        = ctx.pos;
            port_type port_value = 0;

            while (ctx.pos != ctx.end) {
                switch (*ctx.pos) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        // "65535".count() == 5
                        port_value *= 10U; // NOLINT(*-magic-numbers)
                        port_value += static_cast<port_type>(*ctx.pos - '0');
                        ++ctx.pos;
                        if (port_value > max_port_number) [[unlikely]] {
                            set_error(ctx.status, port_out_of_range);
                            return;
                        }
                        continue;
                    case '\\':
                        if (is_special_scheme(ctx.scheme)) {
                            break; // invalid port
                        }
                        [[fallthrough]];
                    case '\0':
                    case '/':
                    case '?':
                    case '#': break;

                    // handling tabs and newlines
                    case '\t':
                    case '\n':
                    case '\r':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, invalid_character);
                            ++ctx.pos;
                            continue;
                        }
                        [[fallthrough]];
                    default: set_error(ctx.status, port_invalid); return;
                }
                break;
            }

            // ignoring the leading zeros
            while (beg != (ctx.pos - 1) && *beg == '0') [[unlikely]] {
                ++beg;
            }

            // it's unsigned, we don't need to check for it being lower than 0
            if (port_value == known_port(get_output_view<components::scheme>(ctx))) {
                clear<components::port>(ctx);
            } else if constexpr (requires { ctx.out.set_port(static_cast<stl::uint16_t>(port_value)); }) {
                // store the integer port value
                ctx.out.set_port(static_cast<stl::uint16_t>(port_value));
            } else if constexpr (requires { ctx.out->set_port(static_cast<stl::uint16_t>(port_value)); }) {
                // store the integer port value
                ctx.out->set_port(static_cast<stl::uint16_t>(port_value));
            } else {
                // store it as a string
                set_value<components::port>(ctx, beg, ctx.pos);
            }

            // https://url.spec.whatwg.org/#path-start-state
            set_valid(ctx.status, valid_authority_end);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PORT_HPP
