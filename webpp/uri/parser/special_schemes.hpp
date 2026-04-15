// Created by moisrex on 10/21/23.

#ifndef WEBPP_SPECIAL_SCHEMES_HPP
#define WEBPP_SPECIAL_SCHEMES_HPP

#include "../uri_status.hpp"
#include "./iiequals.hpp"

#include <cstdint>
#include <string_view>

namespace webpp::uri {

    enum struct [[nodiscard]] special_scheme : stl::uint8_t {
        unknown = 0, // not special at all
        http,
        https,
        file,
        ftp,
        ws,
        wss
    };

    [[nodiscard]] static constexpr stl::string_view to_string(special_scheme const scheme) noexcept {
        using enum special_scheme;
        switch (scheme) {
            case unknown: return {};
            case http: return {"http"};
            case https: return {"https"};
            case file: return {"file"};
            case ftp: return {"ftp"};
            case ws: return {"ws"};
            case wss: return {"wss"};
            default: break;
        }
        return {};
    }

    namespace details {

        static constexpr stl::uint64_t one_byte = sizeof(stl::byte) * 8U;

        static consteval stl::uint64_t encoded_scheme(char const* str) noexcept {
            stl::uint64_t scheme_code = 0ULL;
            for (; *str != '\0'; ++str) { // NOLINT(*-pro-bounds-pointer-arithmetic)
                scheme_code  |= static_cast<stl::uint64_t>(*str);
                scheme_code <<= one_byte;
            }
            return scheme_code;
        }

    } // namespace details

    static constexpr uri_status_type scheme_mask = +uri_status::special_scheme | +uri_status::file_scheme;

    enum struct scheme_type : uri_status_type {       // NOLINT(*-enum-size)
        not_special    = +uri_status::unparsed,       // everything else
        special_scheme = +uri_status::special_scheme, // http(s), ws(s), ftp
        file_scheme    = +uri_status::file_scheme,
    };

    [[nodiscard]] static constexpr uri_status_type operator+(scheme_type const scheme) noexcept {
        return stl::to_underlying(scheme);
    }

    [[nodiscard]] static constexpr scheme_type scheme_type_of(uri_status const status) noexcept {
        return static_cast<scheme_type>(+status & scheme_mask);
    }

    template <typename CharT>
    [[nodiscard]] static constexpr bool is_file_scheme(stl::basic_string_view<CharT> const scheme) noexcept {
        return iiequals_fl("file", scheme);
    }

    [[nodiscard]] static constexpr bool is_file_scheme(scheme_type const scheme) noexcept {
        return scheme == scheme_type::file_scheme;
    }

    [[nodiscard]] static constexpr bool is_file_scheme(uri_status const status) noexcept {
        return is_file_scheme(scheme_type_of(status));
    }

    /**
     * @return 0 if unknown, otherwise return the port
     */
    template <typename CharT>
    [[nodiscard]] static constexpr stl::uint16_t known_port(stl::basic_string_view<CharT> const scheme) noexcept {
        // NOLINTBEGIN(*-magic-numbers)
        switch (scheme.size()) {
            case 2U:
                if (iiequals_fl("ws", scheme)) {
                    return 80U;
                }
                break;
            case 3U:
                if (iiequals_fl("wss", scheme)) {
                    return 443U;
                }
                if (iiequals_fl("ftp", scheme)) {
                    return 21U;
                }
                break;
            case 4U:
                if (iiequals_fl("http", scheme)) {
                    return 80U;
                }
                // "file" scheme is not checked since it would be "0" anyway
                break;
            case 5U:
                if (iiequals_fl("https", scheme)) {
                    return 443U;
                }
                break;
            default: break;
        }
        return 0U;
        // NOLINTEND(*-magic-numbers)
    }

    template <typename CharT>
    [[nodiscard]] static constexpr special_scheme to_special_scheme(
      stl::basic_string_view<CharT> const scheme) noexcept {
        // NOLINTBEGIN(*-magic-numbers)
        using enum special_scheme;
        switch (scheme.size()) {
            case 2U:
                if (iiequals_fl("ws", scheme)) {
                    return ws;
                }
                break;
            case 3U:
                if (iiequals_fl("wss", scheme)) {
                    return wss;
                }
                if (iiequals_fl("ftp", scheme)) {
                    return ftp;
                }
                break;
            case 4U:
                if (iiequals_fl("http", scheme)) {
                    return http;
                }
                if (iiequals_fl("file", scheme)) {
                    return file;
                }
                break;
            case 5U:
                if (iiequals_fl("https", scheme)) {
                    return https;
                }
                break;
            default: break;
        }
        return unknown;
        // NOLINTEND(*-magic-numbers)
    }

    /**
     * A special/known scheme is this:
     *   A URL is special if its scheme is a special scheme.
     *   A URL is not special if its scheme is not a special scheme.
     *
     * Special scheme 	Default port
     *      ftp             21
     *      file            null
     *      http            80
     *      https           443
     *      ws              80
     *      wss             443
     *
     * The only usage of this is in URL parsing.
     *
     * from https://url.spec.whatwg.org/#is-special
     */
    template <typename CharT>
    [[nodiscard]] static constexpr bool is_special_scheme(stl::basic_string_view<CharT> const scheme) noexcept {
        return known_port(scheme) != 0U || is_file_scheme(scheme);
    }

    [[nodiscard]] static constexpr bool is_special_scheme(scheme_type const scheme) noexcept {
        return scheme != scheme_type::not_special;
    }

    [[nodiscard]] static constexpr bool is_special_scheme(special_scheme const scheme) noexcept {
        return scheme != special_scheme::unknown;
    }

    [[nodiscard]] static constexpr bool is_special_scheme(uri_status_type const status) noexcept {
        return (status & scheme_mask) != 0ULL;
    }

    [[nodiscard]] static constexpr bool is_special_scheme(uri_status const status) noexcept {
        return is_special_scheme(+status);
    }

    [[nodiscard]] static constexpr bool is_file_scheme(uri_status_type const status) noexcept {
        return (status & +scheme_type::file_scheme) == +scheme_type::file_scheme;
    }

    static constexpr void set_flag(uri_status_type& status, scheme_type const value) noexcept {
        set_flag(status, static_cast<uri_status>(+value));
    }

} // namespace webpp::uri

#endif // WEBPP_SPECIAL_SCHEMES_HPP
