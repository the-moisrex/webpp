// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_PORT_HPP
#define WEBPP_URI_PARSE_PORT_HPP

#include "./special_schemes.hpp"
#include "./uri_components.hpp"

namespace webpp::uri {

    static constexpr stl::uint16_t max_port_number       = 65'535U;
    static constexpr stl::uint16_t well_known_upper_port = 1024;

    template <uri_options Options, URIContext CtxT>
        requires(!Options.parse_port)
    static constexpr void parse_port(CtxT& ctx) noexcept {
        // any path that we take, we shouldn't be allowed to get there if Options.parse_port is false,
        // so, we just set an error and get out of this situation since it's most-likely a bug in parsing.
        set(ctx.status, uri_status::port_invalid);
    }

    template <uri_options Options , URIContext CtxT>
        requires(Options.parse_port)
    static constexpr void parse_port(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#port-state

        using enum uri_status;

        using port_type = stl::uint32_t; // we use a bigger size to detect overflows from 65535-99999

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
                        set(ctx.status, port_out_of_range);
                        return;
                    }
                    continue;
                case '\\':
                    if (!is_special_scheme(ctx.status)) [[unlikely]] {
                        set(ctx.status, port_invalid);
                        return;
                    }
                    [[fallthrough]];
                case '/':
                case '?':
                case '#': break;

                default:
                    if constexpr (Options.state_override) {
                        // a = new URL("https://example.com:100/");
                        // a.port = "200what?";
                        // a.port === '200'
                        break;
                    } else {
                        set(ctx.status, port_invalid);
                        return;
                    }
            }
            break;
        }

        // ignoring the leading zeros
        while (beg != ctx.pos - 1 && *beg == '0') [[unlikely]] {
            ++beg;
        }

        // it's unsigned, we don't need to check for it being lower than 0
        if (port_value == known_port(get_output_view<components::scheme>(ctx))) {
            clear_port(ctx.out);
        } else {
            if constexpr (requires { istl::deptr(ctx.out).set_port(static_cast<stl::uint16_t>(port_value)); }) {
                // store the integer port value
                set_port(ctx, static_cast<stl::uint16_t>(port_value));
            } else {
                // store it as a string
                // set value already sets the flag
                set_port(ctx, beg, ctx.pos);
            }
        }

        if constexpr (!Options.state_override) {
            // https://url.spec.whatwg.org/#path-start-state
            set(ctx.status, valid_authority_end);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PORT_HPP
