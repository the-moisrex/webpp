// Created by moisrex on 10/21/23.

#ifndef WEBPP_SPECIAL_SCHEMES_HPP
#define WEBPP_SPECIAL_SCHEMES_HPP

#include "../../std/string_view.hpp"

#include <cstdint>

namespace webpp::uri {

    /**
     * @return 0 if unknown, otherwise return the port
     */
    template <istl::StringView StrT>
    [[nodiscard]] constexpr stl::uint16_t known_port(StrT scheme) noexcept {
        // NOLINTBEGIN(*-magic-numbers)
        switch (scheme.size()) {
            case 2:
                if (scheme[0] == 'w' && scheme[1] == 's')
                    return 80u;
                break;
            case 3:
                if (scheme == "wss")
                    return 443u;
                else if (scheme == "ftp")
                    return 21;
                break;
            case 4:
                if (scheme == "http")
                    return 80u;
                break;
            case 5:
                if (scheme == "https")
                    return 443;
                break;
        }
        return 0u;
        // NOLINTEND(*-magic-numbers)
    }

    /**
     * An special/known scheme is this:
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
    [[nodiscard]] constexpr bool is_special_scheme(StrT scheme) noexcept {
        return known_port(scheme) != 0u || scheme == "file";
    }

} // namespace webpp::uri

#endif // WEBPP_SPECIAL_SCHEMES_HPP
