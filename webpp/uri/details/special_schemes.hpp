// Created by moisrex on 10/21/23.

#ifndef WEBPP_SPECIAL_SCHEMES_HPP
#define WEBPP_SPECIAL_SCHEMES_HPP

#include "../../std/string_view.hpp"
#include "../../strings/iequals.hpp"

#include <cstdint>

namespace webpp::uri {

    template <istl::StringView StrT>
    [[nodiscard]] constexpr bool is_file_scheme(StrT scheme) noexcept {
        return ascii::iequals<ascii::char_case_side::first_lowered>("file", stl::forward<StrT>(scheme));
    }

    /**
     * @return 0 if unknown, otherwise return the port
     */
    template <istl::StringView StrT>
    [[nodiscard]] constexpr stl::uint16_t known_port(StrT scheme) noexcept {
        // NOLINTBEGIN(*-magic-numbers)
        switch (scheme.size()) {
            case 2:
                if ((scheme[0] == 'w' || scheme[0] == 'W') && (scheme[1] == 's' || scheme[1] == 'S')) {
                    return 80U;
                }
                break;
            case 3:
                if (ascii::iequals<ascii::char_case_side::first_lowered>("wss", scheme)) {
                    return 443U;
                } else if (ascii::iequals<ascii::char_case_side::first_lowered>("ftp", scheme)) {
                    return 21;
                }
                break;
            case 4:
                if (ascii::iequals<ascii::char_case_side::first_lowered>("http", scheme)) {
                    return 80U;
                }
                break;
            case 5:
                if (ascii::iequals<ascii::char_case_side::first_lowered>("https", scheme)) {
                    return 443U;
                }
                break;
            default: break;
        }
        return 0U;
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
        return known_port(scheme) != 0U || is_file_scheme(scheme);
    }

} // namespace webpp::uri

#endif // WEBPP_SPECIAL_SCHEMES_HPP
