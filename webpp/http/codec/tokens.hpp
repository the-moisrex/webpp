#ifndef WEBPP_HTTP_CODEC_TOKENS_HPP
#define WEBPP_HTTP_CODEC_TOKENS_HPP

#include "../../strings/charset.hpp"

namespace webpp::http {


    static constexpr charset<char, 2> CRLF{0x0D, 0x0A}; // CR(\r), LF(\n)

    // OWS: optional whitespace
    // OWS = *( SP / HTAB )
    //
    // RWS: required whitespace
    // RWS = 1*( SP / HTAB )
    //
    // BWS: bad whitespaces
    // BWS = OWS
    static constexpr charset<char, 2> OWS{0x20, 0x09}; // SP, HTAB


    /**
     * This is the "Token" specified in the HTTP Semantics RFC (https://httpwg.org/specs/rfc9110.html#tokens)
     *   token          = 1*tchar
     *   tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
     *                        / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
     *                        / DIGIT / ALPHA
     *                        ; any VCHAR, except delimiters
     **/
    static constexpr charmap_full token_charmap{
      ALPHA_DIGIT<>,
      charset{'!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~'}
    };

    /**
     * Verifies that the given val is a valid HTTP token
     * per the rules defined in RFC 7230
     * See https://tools.ietf.org/html/rfc7230#section-3.2.6
     */
    [[nodiscard]] static constexpr bool is_http_token(char const item) noexcept {
        return token_charmap.contains(item);
    }

    // Some projects like proxygen allow '_' as well by default, but that's not the standard
    static constexpr charset host_charset{
      ALPHA_DIGIT<char>,
      charset{'.', '-'}
    };


} // namespace webpp::http

#endif
