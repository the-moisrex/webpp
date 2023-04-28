#ifndef WEBPP_HTTP_SYNTAX_TOKENS_HPP
#define WEBPP_HTTP_SYNTAX_TOKENS_HPP

namespace webpp::http {

    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)

    static constexpr char CRLF[]{0x0D, 0x0A}; // CR(\r), LF(\n)

    // OWS: optional whitespace
    // OWS = *( SP / HTAB )
    //
    // RWS: required whitespace
    // RWS = 1*( SP / HTAB )
    //
    // BWS: bad whitespaces
    // BWS = OWS
    static constexpr char OWS[]{0x20, 0x09}; // SP, HTAB

    /**
     * This is the "Token" specified in the HTTP Semantics RFC (https://httpwg.org/specs/rfc9110.html#tokens)
     *   token          = 1*tchar
     *   tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
     *                        / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
     *                        / DIGIT / ALPHA
     *                        ; any VCHAR, except delimiters
     **/
    static constexpr bool token_char_map[256]{
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, true,  false, true,  true,  true,  true,  true,  false, false, true,  true,  false,
      true,  true,  false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  false, false,
      false, false, false, false, false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
      true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
      true,  false, false, false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
      true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
      true,  true,  true,  false, true,  false, true,  false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      false};


    static constexpr bool is_http_token(char item) noexcept {
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
        return token_char_map[static_cast<unsigned char>(item)];
        // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
    }


    // NOLINTEND(cppcoreguidelines-avoid-c-arrays)
} // namespace webpp::http

#endif
