// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_HOST_HPP
#define WEBPP_URI_PARSE_HOST_HPP

#include "../../ip/ipv4.hpp"
#include "../../strings/charset.hpp"
#include "../uri_status.hpp"
#include "./host_ip.hpp"
#include "./idna_to_ascii.hpp"
#include "./uri_components.hpp"
#include "./windows_drive_letter.hpp"

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
            return true;
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

    } // namespace details

    template <uri_options Options, URIContext CtxT, typename Iter = typename CtxT::iterator>
    static constexpr void opaque_host_parser(CtxT& ctx, Iter pos, Iter end) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#concept-opaque-host-parser
        using enum uri_status;
        using details::ascii_bitmap;
        using details::encode_or_validate;
        using details::next_percent_encode;

        // in opaque hosts, IPv6 should work also; in specs, it's being checked in `host parsing` before
        // we get into opaque parsing.
        if (!details::handle_ipv6(ctx, pos, end)) {
            // either found a valid ipv6, an error occurred, or it's an empty string.
            return;
        }

        ctx.pos = pos;

        // A URL code point is an ASCII alphanumeric, "!", "$", "&", "'", "(", ")", "*", "+", ",", "-", ".",
        // "/", ":", ";", "=", "?", "@", "_", "~", or a scalar value greater than U+007F.
        webpp_static_constexpr ascii_bitmap ascii_url_code_points{ALPHA_DIGIT<char>, details::SUB_DELIMS<char>};
        webpp_static_constexpr ascii_bitmap url_code_points_or_percent{
          details::NON_ASCII_CODE_UNITS,
          ascii_bitmap{ascii_url_code_points, '/', ':', '?', '@', '-', '.', '_', '~', '%'}
        };
        webpp_static_constexpr ascii_bitmap invalid_url_units = ascii_bitmap{}.except(url_code_points_or_percent);
        webpp_static_constexpr ascii_bitmap
          invalid_host_chars{details::FORBIDDEN_HOST_CODE_POINTS, invalid_url_units, ascii_bitmap{'%'}};

        auto buffer = create_buffer(ctx);
        while (!encode_or_validate(ctx, buffer, details::C0_CONTROL_ENCODE_SET, invalid_host_chars)) {
            if (details::FORBIDDEN_HOST_CODE_POINTS.contains(*ctx.pos)) [[unlikely]] {
                set(ctx.status, invalid_host_code_point);
                return;
            }
            if (*ctx.pos == '%') {
                if (!next_percent_encode(ctx, buffer)) [[unlikely]] {
                    set_warning(ctx.status, invalid_character);
                }
                continue;
            }
            set_warning(ctx.status, invalid_character);
            if constexpr (CtxT::is_modifiable) {
                encode_uri_component<uri_encoding_policy::encode_chars>(
                  *ctx.pos,
                  buffer,
                  details::C0_CONTROL_ENCODE_SET);
                ++ctx.pos;
            } else if (details::C0_CONTROL_ENCODE_SET.contains(*ctx.pos)) {
                set(ctx.status, modification_required);
                return;
            } else {
                ++ctx.pos;
            }
        }
        end_segment(ctx, buffer);
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
        using id_type  = stl::uint8_t;
        using iterator = typename CtxT::iterator;

        // note: we don't need to check for IPv6 as the first step, we can check later.

        // If isOpaque is true, then return the result of opaque-host parsing input.
        if (!is_special_scheme(ctx.status)) {
            opaque_host_parser<Options>(ctx, ctx.pos, ctx.end);
            return;
        }

        // Assert: input is not the empty string.
        assert(ctx.pos != ctx.end);

        // Let domain be the result of running UTF-8 decode without BOM on the percent-decoding of input.


        enum struct cp_type : id_type {
            upper_val     = 0b1U,        // upper case ascii chars
            no_ipv4_val   = 0b10U,       // invalid IPv4 Characters
            no_ipv6_val   = 0b100U,      // invalid IPv6 Characters
            x_val         = 0b1000U,     // character x
            n_val         = 0b1'0000U,   // character n
            dash_val      = 0b10'0000U,  // character -
            authority_end = 0b100'0000U, // characters: / \ ? #
            forb_val =
              static_cast<id_type>(~0U) & static_cast<id_type>(~static_cast<id_type>(0b100'0000U)), // Forbidden/Unicode
            xnd_val   = x_val | n_val | dash_val | no_ipv4_val,
            no_ip_val = no_ipv4_val | no_ipv6_val,
        };

        // todo: this is making compile time worse
        webpp_static_constexpr auto authority_end_cps      = charset('/', '\\', '#', '?');
        webpp_static_constexpr auto interesting_characters = categorize<id_type, 256U>(
          cat{.set = details::NON_ASCII_CODE_UNITS, .value = stl::to_underlying(cp_type::forb_val)},
          cat{.set   = details::FORBIDDEN_HOST_CODE_POINTS.except(authority_end_cps),
              .value = stl::to_underlying(cp_type::forb_val)},
          cat{.set   = details::INVALID_IPV4.except(authority_end_cps),
              .value = stl::to_underlying(cp_type::no_ipv4_val)},
          cat{.set   = details::INVALID_IPV6.except(authority_end_cps),
              .value = stl::to_underlying(cp_type::no_ipv6_val)},
          cat{.set = UPPER_ALPHA<char8_t>, .value = stl::to_underlying(cp_type::upper_val)},
          cat{.set = u8"xX", .value = stl::to_underlying(cp_type::x_val)},
          cat{.set = u8"nN", .value = stl::to_underlying(cp_type::n_val)},
          cat{.set = u8"/\\?#", .value = stl::to_underlying(cp_type::authority_end)},
          cat{.set = u8"-", .value = stl::to_underlying(cp_type::dash_val)});

        // todo: UTF-16 and UTF-32 may contain big invalid code points, this can't check for those

        // check all the characters and see what's there and what's not in order to avoid going into the slow
        // path portion of the code which checks for everything and properly converts things to things.
        iterator const sbeg = ctx.pos;
        for (;;) {
            auto const status =
              or_all<id_type>(interesting_characters, stl::to_underlying(cp_type::authority_end), ctx.pos, ctx.end);
            switch (status & ~stl::to_underlying(cp_type::authority_end)) {
                case stl::to_underlying(cp_type::upper_val):
                    // todo: does a simple to_lower would suffice?
                    break;
                case 0: // possible IPv4
                    if (details::verify_possible_ipv4<Options>(ctx, sbeg, ctx.pos)) {
                        return;
                    }
                    set_hostname(ctx.out, segment{sbeg, ctx.pos});
                    set_flag(ctx.status, has_non_null_host);
                    return;
                case stl::to_underlying(cp_type::no_ip_val):
                    // fast path:
                    // the host is fully in valid ascii characters already, and also we don't need to check for
                    // ipv4 either, it includes invalid ipv4 characters.
                    set_hostname(ctx.out, segment{sbeg, ctx.pos});
                    set_flag(ctx.status, has_non_null_host);
                    return;
                [[unlikely]] case stl::to_underlying(cp_type::forb_val):
                    break; // forbidden code points
                [[unlikely]] default:
                    // 'x', 'n' and '-' were found
                    if ((status & stl::to_underlying(cp_type::no_ipv6_val)) == 0 &&
                        !details::handle_ipv6(ctx, sbeg, ctx.pos))
                    {
                        // either found a valid ipv6, an error occurred, or it's an empty string.
                        return;
                    }

                    if ((status | stl::to_underlying(cp_type::xnd_val)) == status &&
                        details::starts_with(sbeg, ctx.pos, stl::string_view{"xn-"}))
                    {
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
            auto out = create_buffer(ctx);

            // Let asciiDomain be the result of running domain to ASCII with domain and false.
            auto const to_ascii_res = idna::domain_to_ascii<Options>(sbeg, ctx.pos, out);
            if (!is_valid(to_ascii_res)) [[unlikely]] {
                set_error(ctx.status, to_ascii_res);
                return;
            }
            set_hostname(ctx.out, stl::move(out));
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
        static_assert(Options.allow_file_hosts,
                      "This function should not be reached if hosts in 'file://' scheme are not allowed.");
        assert(has_flags(ctx.status, file_scheme));

        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) [[unlikely]] {
                set_warning(ctx.status, windows_drive_letter_as_host);
                set(ctx.status, valid_path);
                return;
            }
        }


        host_parser<Options>(ctx);

        if (has_error(ctx.status)) [[unlikely]] {
            return;
        }

        set(ctx.status, valid_path_start);

        // if buffer is the empty string, then:
        //   - Set url's host to the empty string.
        //   - If state override is given, then return.
        //   - Set state to path start state.
        // if (!has_host) [[likely]] {
        //     clear_hostname(ctx.out);
        //     set_flag(ctx.status, has_non_null_host);
        //     set(ctx.status, valid_path_start);
        //     return;
        // }


        // If c is the EOF code point, U+002F (/), U+005C (\), U+003F (?), or U+0023 (#), then ...
        // assert(ctx.pos == ctx.end || *stl::prev(ctx.pos) == '/' || *stl::prev(ctx.pos) == '\\' ||
        //        *stl::prev(ctx.pos) == '?' || *stl::prev(ctx.pos) == '#');

        // If host is "localhost", then set host to the empty string.
        // Empty string != null
        if (ctx.pos == ctx.end || (has_hostname(ctx.out) && is_localhost_string(hostname(ctx.out)))) {
            clear_hostname(ctx.out);
            set_flag(ctx.status, has_non_null_host);
        }
        if constexpr (Options.handle_windows_drive_letters && !Options.state_override) {
            if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) {
                set_warning(ctx.status, windows_drive_letter_as_host);
            }
        }
    }


} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_HOST_HPP
