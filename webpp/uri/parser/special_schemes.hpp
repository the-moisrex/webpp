// Created by moisrex on 10/21/23.

#ifndef WEBPP_SPECIAL_SCHEMES_HPP
#define WEBPP_SPECIAL_SCHEMES_HPP

#include "../../std/string_like.hpp"
#include "../../std/string_view.hpp"
#include "../uri_status.hpp"
#include "./constants.hpp"
#include "./iiequals.hpp"

#include <cstdint>

namespace webpp::uri {

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

    static constexpr uri_status_type scheme_mask =
      stl::to_underlying(uri_status::special_scheme) | stl::to_underlying(uri_status::file_scheme);

    enum struct scheme_type : uri_status_type {                          // NOLINT(*-enum-size)
        not_special    = stl::to_underlying(uri_status::unparsed),       // everything else
        special_scheme = stl::to_underlying(uri_status::special_scheme), // http(s), ws(s), ftp
        file_scheme    = stl::to_underlying(uri_status::file_scheme),
    };

    [[nodiscard]] static constexpr scheme_type scheme_type_of(uri_status const status) noexcept {
        return static_cast<scheme_type>(stl::to_underlying(status) & scheme_mask);
    }

    template <istl::StringLike StrT>
    [[nodiscard]] static constexpr bool is_file_scheme(StrT scheme) noexcept {
        using char_type = istl::char_type_of_t<StrT>;
        return iiequals_fl<details::TABS_OR_NEWLINES<char_type>>("file", stl::forward<StrT>(scheme));
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
    template <istl::StringView StrT, bool CheckSpecialCharacters = true>
    [[nodiscard]] static constexpr stl::uint16_t known_port(StrT scheme) noexcept {
        using details::encoded_scheme;

        // NOLINTBEGIN(*-magic-numbers)
        stl::uint64_t scheme_code = 0ULL;
        for (auto const ith_char : scheme) {
            if constexpr (CheckSpecialCharacters) {
                switch (ith_char) {
                    case '\r':
                    case '\n':
                    case '\t': continue;
                    default: break;
                }
            }
            scheme_code  |= static_cast<stl::uint64_t>(ascii::to_lower_copy(ith_char));
            scheme_code <<= details::one_byte;
        }
        switch (scheme_code) {
            case encoded_scheme("http"):
            case encoded_scheme("ws"): return 80U;
            case encoded_scheme("https"):
            case encoded_scheme("wss"): return 443U;
            case encoded_scheme("ftp"): return 21U;
            default: break;
        }
        return 0U;
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
    template <istl::StringView StrT>
    [[nodiscard]] static constexpr bool is_special_scheme(StrT scheme) noexcept {
        return known_port(scheme) != 0U || is_file_scheme(scheme);
    }

    [[nodiscard]] static constexpr bool is_special_scheme(scheme_type const scheme) noexcept {
        return scheme != scheme_type::not_special;
    }

    [[nodiscard]] static constexpr bool is_special_scheme(uri_status_type const status) noexcept {
        return (status & scheme_mask) != 0ULL;
    }

    [[nodiscard]] static constexpr bool is_special_scheme(uri_status const status) noexcept {
        return is_special_scheme(stl::to_underlying(status));
    }

    [[nodiscard]] static constexpr bool is_file_scheme(uri_status_type const status) noexcept {
        return (status & stl::to_underlying(scheme_type::file_scheme)) ==
               stl::to_underlying(scheme_type::file_scheme);
    }

    static constexpr void set_flag(uri_status_type& status, scheme_type const value) noexcept {
        set_flag(status, static_cast<uri_status>(stl::to_underlying(value)));
    }

} // namespace webpp::uri

#endif // WEBPP_SPECIAL_SCHEMES_HPP
