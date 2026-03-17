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
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_file_host(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        static_assert(Options.allow_file_hosts,
                      "This function should not be reached if hosts in 'file://' scheme are not allowed.");

        assert(has_flags(ctx.status, uri_status::file_scheme));

        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) [[unlikely]] {
                while (*ctx.pos != '/' && *ctx.pos != '\\') {
                    // we have to move one back because the "path" needs to start with a "/" or a "\"
                    if (--ctx.pos == ctx.beg) {
                        break;
                    }
                }
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
                set(ctx.status, uri_status::valid_path);
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

        if (has_hostname(ctx.out) && is_localhost_string(hostname(ctx.out))) {
            clear_hostname(ctx.out);
        }
        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) {
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
            }
        }
    }

    namespace details {

        template <typename Iter>
        [[nodiscard]] static constexpr bool starts_with(Iter& pos, Iter end, auto str) noexcept {
            auto       spos = stl::begin(str);
            auto const send = stl::end(str);
            for (; pos != end && spos != send; ++pos, ++spos) {
                if (*spos != *pos) {
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
                set(ctx.status, ipv6_unclosed);
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
        using id_type  = stl::uint8_t;
        using iterator = typename CtxT::iterator;

        // note: we don't need to check for IPv6 as the first step, we can check later.

        // If isOpaque is true, then return the result of opaque-host parsing input.
        if (!is_special_scheme(ctx.status)) {
            opaque_host_parser(ctx, pos, end);
            return;
        }

        // Assert: input is not the empty string.
        assert(pos != end);

        // Let domain be the result of running UTF-8 decode without BOM on the percent-decoding of input.


        enum struct cp_type : id_type {
            upper_val   = 0b1U,                      // upper case ascii chars
            no_ipv4_val = 0b10U,                     // invalid IPv4 Characters
            no_ipv6_val = 0b100U,                    // invalid IPv6 Characters
            x_val       = 0b1000U,                   // character x
            n_val       = 0b1'0000U,                 // character n
            dash_val    = 0b10'0000U,                // character -
            forb_val    = static_cast<id_type>(~0U), // Forbidden/Unicode
            xnd_val     = x_val | n_val | dash_val | no_ipv4_val,
            no_ip_val   = no_ipv4_val | no_ipv6_val,
        };

        webpp_static_constexpr auto interesting_characters = categorize<id_type, 256U>(
          cat{.set = details::NON_ASCII_CODE_UNITS, .value = cp_type::forb_val},
          cat{.set = details::FORBIDDEN_HOST_CODE_POINTS, .value = cp_type::forb_val},
          cat{.set = details::INVALID_IPV4, .value = cp_type::no_ipv4_val},
          cat{.set = details::INVALID_IPV6, .value = cp_type::no_ipv6_val},
          cat{.set = UPPER_ALPHA<char8_t>, .value = cp_type::upper_val},
          cat{.set = u8"xX", .value = cp_type::x_val},
          cat{.set = u8"nN", .value = cp_type::n_val},
          cat{.set = u8"-", .value = cp_type::dash_val});

        // todo: UTF-16 and UTF-32 may contain big invalid code points, this can't check for those

        // check all the characters and see what's there and what's not in order to avoid going into the slow
        // path portion of the code which checks for everything and properly converts things to things.
        iterator const sbeg   = pos;
        auto const     status = or_all<id_type>(interesting_characters, pos, end);
        switch (status) {
            case cp_type::upper_val:
                // todo: does a simple to_lower would suffice?
                break;
            case 0: // possible IPv4
                if (is_possible_ends_with_ipv4<Options>(pos, end, ctx)) {
                    details::parse_host_ipv4<Options>(pos, end, ctx);
                    return;
                }
                set_hostname(ctx, pos, end);
                return;
            case cp_type::no_ip_val:
                // fast path:
                // the host is fully in valid ascii characters already, and also we don't need to check for
                // ipv4 either, it includes invalid ipv4 characters.
                set_hostname(ctx, pos, end);
                return;
            [[unlikely]] case cp_type::forb_val:
                break; // forbidden code points:
            [[unlikely]] default:
                // 'x', 'n' and '-' were found
                if ((status & stl::to_underlying(cp_type::no_ipv6_val)) == 0 && !details::handle_ipv6(ctx, pos, end)) {
                    // either found a valid ipv6, an error occurred, or it's an empty string.
                    return;
                }

                if ((status | stl::to_underlying(cp_type::xnd_val)) == status && starts_with(pos, end, "xn-")) {
                    // if it starts with `xn-`, then we go the slow path
                    // todo: we already know if newlines and tabs exist or not
                    break;
                }
                break;
        }

        // slow path:

        // If asciiDomain ends in a number, then return the result of IPv4 parsing asciiDomain.
        if (is_possible_ends_with_ipv4<Options>(pos, end, ctx)) {
            details::parse_host_ipv4<Options>(pos, end, ctx);
            return;
        }

        // Return asciiDomain.
        if constexpr (CtxT::is_modifiable) {
            auto out = create_buffer(ctx);

            // Let asciiDomain be the result of running domain to ASCII with domain and false.
            auto const to_ascii_res = idna::domain_to_ascii<Options>(sbeg, pos, end, out);
            if (!is_valid(to_ascii_res)) [[unlikely]] {
                set_error(ctx.status, to_ascii_res);
                return;
            }
            set_hostname(ctx.out, out);
        } else {
            // Only validate, no conversion:
            auto const ascii_status = idna::verify_domain_ascii<Options>(sbeg, pos);
            if (!is_valid(ascii_status)) [[unlikely]] {
                set(ctx.status, ascii_status);
                return;
            }
            set_hostname(ctx.out, sbeg, pos);
        }
    }

    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_hostname(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#host-state
        // https://url.spec.whatwg.org/#hostname-state
        using enum uri_status;

        // If state override is given and url’s scheme is "file", then decrease pointer by 1 and set state to
        // file host state.
        if (Options.state_override && is_file_scheme(ctx.status)) {
            set(ctx.status, valid_file_host);
            return;
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
