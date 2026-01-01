// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_HOST_HPP
#define WEBPP_URI_PARSE_HOST_HPP

#include "../uri_status.hpp"
#include "./idna_to_ascii.hpp"
#include "./parse_authority_pieces.hpp"
#include "./uri_components.hpp"
#include "./windows_drive_letter.hpp"

namespace webpp::uri {

    template <typename CharT>
    [[nodiscard]] static constexpr bool is_localhost_string(stl::basic_string_view<CharT> const host) noexcept {
        return iiequals_fl("localhost", host);
    }

    /**
     * https://url.spec.whatwg.org/#file-host-state
     */
    template <uri_options Options = uri_options{}, URIContext CtxT>
    static constexpr void parse_file_host(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        static_assert(Options.allow_file_hosts,
                      "This function should not be reached if hosts in 'file://' scheme are not allowed.");

        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter<Options>(ctx.pos, ctx.end)) [[unlikely]] {
                while (*ctx.pos != '/' && *ctx.pos != '\\') {
                    // we have to move one back because the "path" needs to start with a "/" or a "\"
                    if (--ctx.pos == ctx.beg) {
                        break;
                    }
                }
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
                set_valid(ctx.status, uri_status::valid_path);
                return;
            }
        }

        webpp_static_constexpr auto parsing_options = []() consteval {
            uri_options options         = Options;
            options.parse_credentials   = false;
            options.empty_host_is_error = false;
            options.parse_port          = false;
            return options;
        }();
        details::parse_authority_pieces<parsing_options>(ctx);

        if (has_value<components::host>(ctx) && is_localhost_string(get_component<components::host>(ctx))) {
            clear<components::host>(ctx);
        }
        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter<Options>(ctx.pos, ctx.end)) {
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
            }
        }
    }

    namespace details {

        template <typename Iter>
        [[nodiscard]] static constexpr bool starts_with(Iter& pos, Iter end, auto str) noexcept {
            auto spos = stl::begin(str);
            auto send = stl::end(str);
            if (send < end) {
                return false;
            }
            for (; pos != end && spos != send; ++pos, ++spos) {
                if (head(pos, end) != *pos) {
                    return false;
                }
            }
            return true;
        }

        /// @returns should continue parsing or not
        /// @returns false if either found a valid ipv6, an error occurred, or it's an empty string.
        template <typename Iter, URIContext CtxT>
        [[nodiscard]] static constexpr bool handle_ipv6(CtxT& ctx, Iter& pos, Iter end) noexcept(CtxT::is_nothrow) {
            using enum uri_status;
            assert(pos != end);
            if (*pos != '[') [[likely]] {
                return true;
            }
            if (*stl::prev(end) != ']') [[unlikely]] {
                set_error(ctx.status, ipv6_unclosed);
                return false;
            }
            static_cast<void>(details::parse_host_ipv6(ctx));
            return false;
        }
    } // namespace details

    template <uri_options Options, URIContext CtxT, typename Iter = typename CtxT::iterator>
    static constexpr void opaque_host_parser(CtxT& ctx, Iter pos, Iter end) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#concept-opaque-host-parser
        using enum uri_status;

        // in opaque hosts, IPv6 should work also; in specs, it's being checked in `host parsing` before
        // we get into opaque parsing.
        if (!details::handle_ipv6(ctx, pos, end)) {
            // either found a valid ipv6, an error occurred, or it's an empty string.
            return;
        }

        // todo
    }

    /**
     * Parse hostname
     * Make sure to use `set_flag(ctx.status, scheme_type::special_scheme)` if the uri is opaque before
     * calling this function; we don't provide `isOpaque` that the specs say because of that feature.
     */
    template <uri_options Options, URIContext CtxT, typename Iter = typename CtxT::iterator>
    static constexpr void host_parser(CtxT& ctx, Iter pos, Iter end) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#concept-host-parser
        using enum uri_status;
        using details::ascii_bitmap;
        using details::is_possible_ends_with_ipv4;

        // note: we don't need to check for IPv6 as the first step, we can check later.

        // If isOpaque is true, then return the result of opaque-host parsing input.
        if (!is_special_scheme(ctx.status)) {
            opaque_host_parser(ctx, pos, end);
            return;
        }

        // Assert: input is not the empty string.
        assert(pos != end);

        // Let domain be the result of running UTF-8 decode without BOM on the percent-decoding of input.

        using id_type = stl::uint8_t;

        webpp_static_constexpr id_type upper_val   = 0b1U;                               // upper case ascii chars
        webpp_static_constexpr id_type no_ipv4_val = 0b10U;                              // invalid IPv4 Characters
        webpp_static_constexpr id_type no_ipv6_val = 0b100U;                             // invalid IPv6 Characters
        webpp_static_constexpr id_type x_val       = 0b1000U;                            // character x
        webpp_static_constexpr id_type n_val       = 0b1'0000U;                          // character n
        webpp_static_constexpr id_type dash_val    = 0b10'0000U;                         // character -
        webpp_static_constexpr id_type nt_val      = 0b100'0000U;                        // newlines and tabs
        webpp_static_constexpr id_type forb_val    = static_cast<id_type>(~0 & ~nt_val); // Forbidden/Unicode
        webpp_static_constexpr id_type xnd_val     = x_val | n_val | dash_val | no_ipv4_val;
        webpp_static_constexpr id_type no_ip_val   = no_ipv4_val | no_ipv6_val;

        webpp_static_constexpr auto interesting_characters = categorize<id_type, 256U>(
          cat{.set = details::NON_ASCII_CODE_UNITS, .value = forb_val},
          cat{.set = details::FORBIDDEN_HOST_CODE_POINTS, .value = forb_val},
          cat{.set = details::INVALID_IPV4, .value = no_ipv4_val},
          cat{.set = details::INVALID_IPV6, .value = no_ipv6_val},
          cat{.set = details::TABS_OR_NEWLINES, .value = nt_val},
          cat{.set = UPPER_ALPHA<char8_t>, .value = upper_val},
          cat{.set = u8"xX", .value = x_val},
          cat{.set = u8"nN", .value = n_val},
          cat{.set = u8"-", .value = dash_val});

        // todo: UTF-16 and UTF-32 may contain big invalid code points, this can't check for those

        // check all the characters and see what's there and what's not in order to avoid going into the slow
        // path portion of the code which checks for everything and properly converts things to things.
        auto const status = or_all<id_type>(interesting_characters, pos, end);
        switch (status) {
            case upper_val:
                // todo: does a simple to_lower would suffice?
                break;
                [[fallthrough]];
            case 0: // possible IPv4
                if (is_possible_ends_with_ipv4<Options>(pos, end, ctx)) {
                    details::parse_host_ipv4(pos, end, ctx);
                    return;
                }
                set_value<components::host>(ctx, pos, end);
                return;
            case no_ip_val | nt_val:
                if constexpr (CtxT::is_modifiable) {
                    break;
                }
                // todo: strip the newlines or trim if it's non-modifiable
                [[fallthrough]];
            [[likely]] case no_ip_val:
                // fast path:
                // the host is fully in valid ascii characters already, and also we don't need to check for
                // ipv4 either, it includes invalid ipv4 characters.
                set_value<components::host>(ctx, pos, end);
                return;
            [[unlikely]] case forb_val:
                break; // forbidden code points:
            [[unlikely]] default:
                // 'x', 'n' and '-' were found
                if ((status & no_ipv6_val) == 0) {
                    if (!details::handle_ipv6(ctx, pos, end)) {
                        // either found a valid ipv6, an error occurred, or it's an empty string.
                        return;
                    }
                }

                if ((status | xnd_val) == status) {
                    // if it starts with `xn-`, then we go the slow path
                    if (starts_with(pos, end, "xn-")) {
                        // todo: we already know if newlines and tabs exist or not
                        break;
                    }
                }
                break;
        }

        // slow path:

        // If asciiDomain ends in a number, then return the result of IPv4 parsing asciiDomain.
        if (is_possible_ends_with_ipv4<Options>(pos, end, ctx)) {
            details::parse_host_ipv4(pos, end, ctx);
            return;
        }

        // Return asciiDomain.
        if constexpr (CtxT::is_modifiable) {
            auto  out       = get_buffer<components::host>(ctx);
            auto& prev_host = get_storage<components::host>(ctx);

            // Let asciiDomain be the result of running domain to ASCII with domain and false.
            auto const to_ascii_res = idna::domain_to_ascii<Options>(prev_host, pos, end, out);
            if (!is_valid(to_ascii_res)) {
                set_error<components::host>(ctx, to_ascii_res);
                return;
            }
            set_value<components::host>(ctx, out);
        } else {
            // todo
        }
    }

    template <uri_options Options, URIContext CtxT>
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
                case '[':
                case ']': inside_brackets = *ctx.pos == '['; [[fallthrough]];
                default: break;
            }
            // append to the buffer
        }
    }


} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_HOST_HPP
