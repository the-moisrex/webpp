// Created by moisrex on 9/29/20.

#ifndef WEBPP_HTTP_LEXER_HPP
#define WEBPP_HTTP_LEXER_HPP

#include "../../traits/traits_concepts.hpp"
#include "../../std/vector.hpp"
#include <array>

namespace webpp {

    // https://tools.ietf.org/html/rfc7230#section-3.2
    //
    //     header-field   = field-name ":" OWS field-value OWS
    //
    //     field-name     = token
    //     field-value    = *( field-content / obs-fold )
    //     field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
    //     field-vchar    = VCHAR / obs-text
    //
    //     obs-fold       = CRLF 1*( SP / HTAB )
    //                    ; obsolete line folding
    //                    ; see Section 3.2.4 in the RFC
    //
    //     OWS            = *( SP / HTAB )
    //                    ; optional whitespace
    //     RWS            = 1*( SP / HTAB )
    //                    ; required whitespace
    //     BWS            = OWS
    //                    ; "bad" whitespace
    //
    //     token          = 1*tchar
    //
    //     tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
    //                    / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
    //                    / DIGIT / ALPHA
    //                    ; any VCHAR, except delimiters

    template <Traits TraitsType>
    struct http_lexer {
        using traits_type = TraitsType;
        using string_type = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;

        string_view_type raw_view{};
        string_view_type body_view{};
        istl::vector<traits_type, stl::array<string_view_type, 2>> header_views{};

        void next_line() noexcept {

        }
    };

}

#endif // WEBPP_HTTP_LEXER_HPP
