// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_HOST_HPP
#define WEBPP_URI_PARSE_HOST_HPP

#include "../../ip/ipv4.hpp"
#include "../../strings/charset.hpp"
#include "../uri_status.hpp"
#include "./host_ip.hpp"
#include "./idna_to_ascii.hpp"
#include "./uri_components.hpp"
#include "./uri_context.hpp"
#include "./windows_drive_letter.hpp"
#include "special_schemes.hpp"

#include <utility>

namespace webpp::uri {

    template <typename CharT>
    [[nodiscard]] static constexpr bool is_localhost_string(stl::basic_string_view<CharT> const host) noexcept {
        return iiequals_fl("localhost", host);
    }

    namespace details {

        template <typename Iter>
        [[nodiscard]] static constexpr bool starts_with(Iter pos, Iter end, auto str) noexcept {
            auto       spos = stl::begin(str);
            auto const send = stl::end(str);
            for (; pos != end && spos != send; ++pos, ++spos) {
                if (*spos != *pos) {
                    return false;
                }
            }
            return spos == send;
        }

        /// @returns should continue parsing or not
        /// @returns false if either found a valid ipv6, an error occurred, or it's an empty string.
        template <typename Iter, URIContext CtxT>
        [[nodiscard]] static constexpr bool handle_ipv6(CtxT& ctx, Iter pos, Iter end) noexcept(CtxT::is_nothrow) {
            using enum uri_status;
            assert(pos != end);
            if (*pos != '[') [[likely]] {
                return true;
            }
            if (*stl::prev(end) != ']') [[unlikely]] {
                set(ctx.status, ipv6_unclosed);
                return false;
            }
            auto const ppos = ctx.pos;
            auto const pend = ctx.end;
            ctx.pos         = pos;
            ctx.end         = end;
            static_cast<void>(details::parse_host_ipv6(ctx));
            ctx.pos = ppos;
            ctx.end = pend;
            return false;
        }

        template <uri_options Options, URIContext CtxT, typename Iter = typename CtxT::iterator>
        [[nodiscard]] static constexpr bool verify_possible_ipv4(CtxT& ctx, Iter pos, Iter end)
          noexcept(CtxT::is_nothrow) {
            using enum uri_status;
            if (pos == end) [[unlikely]] {
                return false;
            }
            assert(end > pos);
            if (details::is_possible_ends_with_ipv4<Options>(pos, stl::prev(end), ctx)) {
                stl::array<stl::uint8_t, 4> ipv4_octets_data; // NOLINT(*-init)
                if (!details::parse_host_ipv4<Options>(pos, end, ipv4_octets_data.data(), ctx)) {
                    set_flag(ctx.status, has_non_null_host);
                }
                if constexpr (CtxT::is_modifiable) {
                    auto buffer = create_buffer(ctx);
                    pure_ipv4{ipv4_octets_data}.to_string(buffer);
                    set_hostname(ctx.out, stl::move(buffer));
                    set_flag(ctx.status, has_non_null_host);
                } else {
                    if (!pure_ipv4{ipv4_octets_data}.str_equal(pos, end)) [[unlikely]] {
                        set(ctx.status, modification_required);
                        return false;
                    }
                }
                return true;
            }
            return false;
        }

        // A URL code point is an ASCII alphanumeric, "!", "$", "&", "'", "(", ")", "*", "+", ",", "-", ".",
        // "/", ":", ";", "=", "?", "@", "_", "~", or a scalar value greater than U+007F.
        static constexpr ascii_bitmap ascii_url_code_points{ALPHA_DIGIT<char>, details::SUB_DELIMS<char>};
        static constexpr ascii_bitmap url_code_points_or_percent{
          details::NON_ASCII_CODE_UNITS,
          ascii_bitmap{ascii_url_code_points, '/', ':', '?', '@', '-', '.', '_', '~', '%'}
        };
        static constexpr ascii_bitmap invalid_url_units = ascii_bitmap{}.except(url_code_points_or_percent);
        static constexpr ascii_bitmap invalid_host_chars{
          details::FORBIDDEN_HOST_CODE_POINTS,
          invalid_url_units,
          ascii_bitmap{'%'}};




        enum struct host_cp_type : stl::uint8_t {
            upper_val     = 0b1U,        // upper case ascii chars
            no_ipv4_val   = 0b10U,       // invalid IPv4 Characters
            no_ipv6_val   = 0b100U,      // invalid IPv6 Characters
            x_val         = 0b1000U,     // character x
            n_val         = 0b1'0000U,   // character n
            dash_val      = 0b10'0000U,  // character -
            special_chars = 0b100'0000U, // characters: / \ ? # %
            forb_val      = static_cast<stl::uint8_t>(~0U) &
                            static_cast<stl::uint8_t>(~static_cast<stl::uint8_t>(0b100'0000U)), // Forbidden/Unicode
            xnd_val       = x_val | n_val | dash_val | no_ipv4_val,
            no_ip_val     = no_ipv4_val | no_ipv6_val,
        };

        [[nodiscard]] static consteval stl::uint8_t operator+(host_cp_type const code_point) noexcept {
            return static_cast<stl::uint8_t>(code_point);
        }

        static constexpr auto specials               = charset('/', '\\', '#', '?', '%', ':', '@');
        static constexpr auto host_interesting_chars = categorize<stl::uint8_t, 256U>(
          cat{.set = details::NON_ASCII_CODE_UNITS.except(specials), .value = +host_cp_type::forb_val},
          cat{.set = details::FORBIDDEN_HOST_CODE_POINTS.except(specials), .value = +host_cp_type::forb_val},
          cat{.set = details::INVALID_IPV4.except(specials), .value = +host_cp_type::no_ipv4_val},
          cat{.set = details::INVALID_IPV6.except(specials), .value = +host_cp_type::no_ipv6_val},
          cat{.set = UPPER_ALPHA<char8_t>, .value = +host_cp_type::upper_val},
          cat{.set = u8"xX", .value = +host_cp_type::x_val},
          cat{.set = u8"nN", .value = +host_cp_type::n_val},
          cat{.set = specials, .value = +host_cp_type::special_chars},
          cat{.set = u8"-", .value = +host_cp_type::dash_val});

        // The above code slows down compile time; so we use this:
        // in GDB:
        //    dump binary memory data.bin &host_interesting_chars (char*)&host_interesting_chars +
        //    sizeof(host_interesting_chars)
        // in Shell:
        //    xxd -i data.bin
        // static constexpr stl::array<stl::uint8_t, 256U> host_interesting_chars = {
        //   0xbf, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xbf, 0xbf, 0x06, 0x06, 0xbf, 0x06, 0x06, 0x06, 0x06,
        //   0x06, 0x06, 0x06, 0x06, 0x06, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x40,
        //   0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        //   0x00, 0x00, 0x00, 0x00, 0xbf, 0x00, 0xbf, 0x00, 0xbf, 0x40, 0xbf, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        //   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x11, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x09, 0x01,
        //   0x01, 0xbf, 0x40, 0xbf, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        //   0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xbf, 0x00,
        //   0x00, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
        //   0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
        //   0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
        //   0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
        //   0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
        //   0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
        //   0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
        //   0xbf, 0xbf, 0xbf, 0xbf};



    } // namespace details

    template <URIContext CtxT, typename Iter = typename CtxT::iterator>
    static constexpr void opaque_host_parser(CtxT& ctx, Iter pos, Iter end) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#concept-opaque-host-parser
        using enum uri_status;
        using details::ascii_bitmap;
        using details::C0_CONTROL_ENCODE_SET;
        using details::encode_or_validate;
        using details::invalid_host_chars;
        using details::next_percent_encode;

        // in opaque hosts, IPv6 should work also; in specs, it's being checked in `host parsing` before
        // we get into opaque parsing.
        if (!details::handle_ipv6(ctx, pos, end)) {
            // either found a valid ipv6, an error occurred, or it's an empty string.
            return;
        }

        ctx.pos = pos;

        auto buffer = create_buffer(ctx);
        while (!encode_or_validate(ctx, buffer, C0_CONTROL_ENCODE_SET, invalid_host_chars)) {
            switch (peek(ctx)) {
                case '/':
                case '?':
                case '#':
                case '\0': break;
                case '%':
                    if (!next_percent_encode(ctx, buffer)) [[unlikely]] {
                        set_warning(ctx.status, invalid_character);
                    }
                    continue;
                default:
                    if (details::FORBIDDEN_HOST_CODE_POINTS.contains(*ctx.pos)) [[unlikely]] {
                        set(ctx.status, invalid_host_code_point);
                        return;
                    }
                    // set_warning(ctx.status, invalid_character);
                    if constexpr (CtxT::is_modifiable) {
                        encode_uri_component<uri_encoding_policy::encode_chars>(
                          *ctx.pos,
                          buffer,
                          C0_CONTROL_ENCODE_SET);
                        ++ctx.pos;
                    } else if (C0_CONTROL_ENCODE_SET.contains(*ctx.pos)) {
                        set(ctx.status, modification_required);
                        return;
                    } else {
                        ++ctx.pos;
                    }
            }
            break;
        }
        // end_segment(ctx, buffer);
        set_hostname(ctx.out, stl::move(buffer));
        set_flag(ctx.status, has_non_null_host);
    }

    /**
     * Parse hostname
     * Make sure to use `set_flag(ctx.status, scheme_type::special_scheme)` if the uri is opaque before
     * calling this function; we don't provide `isOpaque` that the specs say because of that feature.
     */
    template <uri_options Options, URIContext CtxT>
    static constexpr void host_parser(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#concept-host-parser
        using enum uri_status;
        using details::ascii_bitmap;
        using details::host_interesting_chars;
        using enum details::host_cp_type;
        using iterator = typename CtxT::iterator;

        // If isOpaque is true, then return the result of opaque-host parsing input.
        if (!is_special_scheme(ctx.status)) {
            opaque_host_parser(ctx, ctx.pos, ctx.end);
            return;
        }

        // Assert: input is not the empty string.
        assert(ctx.pos != ctx.end);

        // Let domain be the result of running UTF-8 decode without BOM on the percent-decoding of input.

        // check all the characters and see what's there and what's not in order to avoid going into the slow
        // path portion of the code which checks for everything and properly converts things to things.
        iterator const sbeg   = ctx.pos;
        auto           buffer = create_buffer(ctx);
        for (;;) {
            iterator const lbeg   = ctx.pos;
            auto           status = or_all(host_interesting_chars, +special_chars, ctx.pos, ctx.end);

            if ((status | +special_chars) == status) {
                switch (*ctx.pos) {
                    case '%':
                        // handle percent-encoded hosts
                        if constexpr (!CtxT::is_modifiable) {
                            set(ctx.status, modification_required);
                            return;
                        } else {
                            buffer.append(lbeg, ctx.pos);
                            if (!details::next_percent_encode(ctx, buffer)) [[unlikely]] {
                                // If host is failure, then return failure.
                                // `file://example.com%/` was found
                                set(ctx.status, invalid_host_code_point);
                                return;
                            }
                            continue;
                        }
                        break;

                    case '@':
                        if (get_value(ctx.status) == valid_authority) [[unlikely]] {
                            return;
                        }

                        // pretend it didn't happen and essentially `@` is not an special character
                        while (ctx.pos != ctx.end && *ctx.pos == '@') {
                            ++ctx.pos;
                            status |= or_all(host_interesting_chars, +special_chars, ctx.pos, ctx.end);
                        }
                        break;
                    case ':':
                        // in file-host-state we don't use `:` as a special character
                        if (is_file_scheme(ctx.status)) {
                            // pretend `:` is not an special character
                            while (ctx.pos != ctx.end && *ctx.pos == ':') {
                                ++ctx.pos;
                                status |= or_all(host_interesting_chars, +special_chars, ctx.pos, ctx.end);
                            }
                        }
                        break;
                    case '?':
                    case '#':
                    case '\\': // URL is special
                        set_warning(ctx.status, reverse_solidus_used);
                        [[fallthrough]];
                    case '/': break;
                    default:
                        assert(false);
                        stl::unreachable();
                        break;
                }
            }

            status &= static_cast<stl::uint8_t>(~+special_chars);
            switch (status) {
                case +upper_val:
                    // todo: does a simple to_lower would suffice?
                    break;
                case +x_val:
                case +n_val:
                case 0: // possible IPv4
                    if (details::verify_possible_ipv4<Options>(ctx, sbeg, ctx.pos)) {
                        return;
                    }
                    set_hostname(ctx.out, segment{sbeg, ctx.pos});
                    set_flag(ctx.status, has_non_null_host);
                    return;
                case +no_ip_val:
                    // fast path:
                    // the host is fully in valid ascii characters already, and also we don't need to check for
                    // ipv4 either, it includes invalid ipv4 characters.
                    set_hostname(ctx.out, segment{sbeg, ctx.pos});
                    set_flag(ctx.status, has_non_null_host);
                    return;
                [[unlikely]] case +forb_val:
                    break; // forbidden code points
                [[unlikely]] default:

                    // 'x', 'n' and '-' were found
                    if ((status & +no_ipv6_val) == 0 && !details::handle_ipv6(ctx, sbeg, ctx.pos)) {
                        // either found a valid ipv6, an error occurred, or it's an empty string.
                        return;
                    }

                    if ((status | +xnd_val) == status && details::starts_with(sbeg, ctx.pos, stl::string_view{"xn-"})) {
                        // if it starts with `xn-`, then we go the slow path
                        break;
                    }
                    break;
            }
            break;
        }

        // slow path:

        // If asciiDomain ends in a number, then return the result of IPv4 parsing asciiDomain.
        if (details::verify_possible_ipv4<Options>(ctx, sbeg, ctx.pos)) {
            return;
        }

        // Return asciiDomain.
        if constexpr (CtxT::is_modifiable) {
            // Let asciiDomain be the result of running domain to ASCII with domain and false.
            auto const to_ascii_res = idna::domain_to_ascii<Options>(sbeg, ctx.pos, buffer);
            if (!is_valid(to_ascii_res)) [[unlikely]] {
                set_error(ctx.status, to_ascii_res);
                return;
            }
            set_hostname(ctx.out, stl::move(buffer));
            set_flag(ctx.status, has_non_null_host);
        } else {
            // Only validate, no conversion:
            auto const ascii_status = idna::verify_domain_ascii<Options>(sbeg, ctx.pos);
            if (!is_valid(ascii_status)) [[unlikely]] {
                set(ctx.status, ascii_status);
                return;
            }
            set_hostname(ctx.out, segment{sbeg, ctx.pos});
            set_flag(ctx.status, has_non_null_host);
        }
    }

    /**
     * https://url.spec.whatwg.org/#file-host-state
     */
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_file_host(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using enum uri_status;
        assert(has_flags(ctx.status, file_scheme));

        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) [[unlikely]] {
                set_warning(ctx.status, windows_drive_letter_as_host);
                set(ctx.status, valid_path);
                return;
            }
        }

        if (ctx.pos == ctx.end) [[unlikely]] {
            clear_hostname(ctx.out);
            set_flag(ctx.status, has_non_null_host);
            set(ctx.status, Options.state_override ? valid : valid_path_start);
            return;
        }

        host_parser<Options>(ctx);
        if (has_error(ctx.status)) [[unlikely]] {
            return;
        }

        // if buffer is the empty string, then:
        //   - Set url's host to the empty string.
        //   - If state override is given, then return.
        //   - Set state to path start state.
        if (!has_hostname(ctx.out)) [[unlikely]] {
            clear_hostname(ctx.out);
            set_flag(ctx.status, has_non_null_host);
            set(ctx.status, Options.state_override ? valid : valid_path_start);
            return;
        }

        set(ctx.status, valid_path_start);

        // If c is the EOF code point, U+002F (/), U+005C (\), U+003F (?), or U+0023 (#), then ...
        [[maybe_unused]] char const code_point = ctx.pos == ctx.end ? '\0' : *ctx.pos;
        assert(code_point == '\0' || code_point == '/' || code_point == '\\' || code_point == '?' || code_point == '#');

        // If host is "localhost", then set host to the empty string.
        // Empty string != null
        if (ctx.pos == ctx.end || is_localhost_string(hostname(ctx.out))) {
            clear_hostname(ctx.out);
            set_flag(ctx.status, has_non_null_host);
        }
        // if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
        //     if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) {
        //         set_warning(ctx.status, windows_drive_letter_as_host);
        //     }
        // }
    }

    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_host(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#host-state
        // https://url.spec.whatwg.org/#hostname-state
        //
        // host state:
        //     When it encounters a colon (:) outside of brackets, it accepts it, finalizes parsing the
        //     host, and transitions to the port state.
        //     This is because a “host” in a URL can optionally include a port, e.g., example.com:8080.
        // hostname state:
        //     If the parser is explicitly running in hostname state via a state
        //     override(e.g., when modifying just the hostname via an API), and it encounters a colon( :), it returns
        //     failure. This is because a “hostname” strictly cannot contain a port.
        using enum uri_status;

        // If state override is given and url’s scheme is "file", then decrease pointer by 1 and set state to file host
        // state.
        if constexpr (Options.state_override) {
            if (is_file_scheme(ctx.status)) {
                set(ctx.status, valid_file_host);
                return;
            }
        }

        if (ctx.pos != ctx.end) [[likely]] {
            // we assume the input is a valid hostname, and if it's not, we deal with the consequences later.
            host_parser<Options>(ctx);
            if (has_error(ctx.status)) [[unlikely]] {
                return;
            }
        }

        switch (peek(ctx)) {
            case ':':

                // If buffer is the empty string, host-missing validation error, return failure.
                if (!has_hostname(ctx.out)) [[unlikely]] {
                    set(ctx.status, host_missing);
                    return;
                }

                // If state override is given and state override is hostname state, then return failure.
                if constexpr (Options.state_override) {
                    if (get_value(ctx.status) == valid_hostname) [[unlikely]] {
                        set(ctx.status,
                            is_special_scheme(ctx.status) ? invalid_domain_code_point : invalid_host_code_point);
                        return;
                    }
                }

                ++ctx.pos;
                set(ctx.status, valid_port);
                break;
            case '\\':
                if (!is_special_scheme(ctx.status)) {
                    break;
                }
                set_warning(ctx.status, reverse_solidus_used);
                [[fallthrough]];
            case '/':
            case '?':
            case '#':
            case '\0':
                // If url is special and buffer is the empty string, host-missing validation error, return failure.
                if (is_special_scheme(ctx.status) && !has_hostname(ctx.out)) [[unlikely]] {
                    set(ctx.status, host_missing);
                    return;
                }

                // Otherwise, if state override is given, buffer is the empty string, and either url includes
                // credentials or url’s port is non-null, then return failure.
                if constexpr (Options.state_override) {
                    if (!has_hostname(ctx.out) &&
                        (has_credentials(ctx.out) || has_flags(ctx.status, has_non_null_port))) [[unlikely]]
                    {
                        set(ctx.status, host_missing);
                        return;
                    }
                }

                // Set url’s host to host, buffer to the empty string, and state to path start state.
                // If state override is given, then return.
                set(ctx.status, Options.state_override ? valid : valid_path_start);
                break;
            default: assert(false);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_HOST_HPP
