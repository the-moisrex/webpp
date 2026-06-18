// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_PORT_HPP
#define WEBPP_URI_PARSE_PORT_HPP

#include "./special_schemes.hpp"
#include "./uri_components.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {

    static constexpr stl::uint16_t max_port_number       = 65'535U;
    static constexpr stl::uint16_t well_known_upper_port = 1024;

    namespace details {

        enum struct port_operation_type : stl::uint8_t {
            op_invalid = 0, // must be zero
            op_digit   = 1,
            op_break   = 2,
        };
        static constexpr auto port_table = categorize<256U>(
          // cat{.set = ALL_ASCII<char>, .value = operation_type::op_invalid},
          cat{.set = "0123456789", .value = port_operation_type::op_digit},
          cat{.set = "\\/?#", .value = port_operation_type::op_break});

    } // namespace details

    template <uri_options Options, URIContext CtxT>
        requires(!Options.parse_port)
    static constexpr void parse_port(CtxT& ctx) noexcept {
        // any path that we take, we shouldn't be allowed to get there if Options.parse_port is false,
        // so, we just set an error and get out of this situation since it's most-likely a bug in parsing.
        set(ctx.status, uri_status::port_invalid);
    }

    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_port)
    static constexpr void parse_port(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#port-state
        using enum uri_status;
        using enum details::port_operation_type;
        using port_type = stl::uint32_t; // we use a bigger size to detect overflows from 65535-99999

        auto      beg            = ctx.pos;
        port_type port_value     = 0;
        bool      skip_character = false;
        for (; ctx.pos != ctx.end; ++ctx.pos) {
            auto const code_unit = *ctx.pos;
            switch (static_cast<details::port_operation_type>(or_one(details::port_table, code_unit))) {
                case op_digit:
                    port_value *= 10U; // NOLINT(*-magic-numbers)
                    port_value += static_cast<port_type>(code_unit - '0');
                    if (port_value > max_port_number) [[unlikely]] {
                        set(ctx.status, port_out_of_range);
                        return;
                    }
                    continue;

                case op_break:
                    if (code_unit == '\\' && !is_special_scheme(ctx.status)) [[unlikely]] {
                        set(ctx.status, port_invalid);
                        return;
                    }
                    skip_character = true;
                    break;

                [[unlikely]] case op_invalid:
                    if constexpr (Options.state_override) {
                        // todo: add an option to make sure this would be an error. WHATWG is written by stupid people
                        // a = new URL("https://example.com:100/");
                        // a.port = "200what?";
                        // a.port === '200'
                        break;
                    } else {
                        set(ctx.status, port_invalid);
                        return;
                    }
                default: assert(false); stl::unreachable();
            }
            break;
        }

        // it's unsigned, we don't need to check for it being lower than 0
        if (port_value == known_port(scheme(ctx.out)) && port_value != 0) {
            clear_port(ctx.out);
            unset_flag(ctx.status, has_non_null_port);
        } else {
            assert(port_value <= max_port_number);

            // set the port:
            if constexpr (PortNumberAssignable<typename CtxT::component_type>) {
                set_port(ctx.out, static_cast<stl::uint16_t>(port_value));
            } else {
                // ignoring the leading zeros
                while (beg < stl::prev(ctx.pos) && *beg == '0') [[unlikely]] {
                    ++beg;
                }
                set_port(ctx.out, create_buffer(ctx, beg, ctx.pos));
            }
            set_flag(ctx.status, has_non_null_port);
        }

        if (skip_character && ctx.pos != ctx.end) {
            ++ctx.pos;
        }

        // If state override is given, then return failure
        // Set state to path start state and decrease pointer by 1.
        // https://url.spec.whatwg.org/#path-start-state
        set(ctx.status, Options.state_override ? valid : valid_path_start);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PORT_HPP
