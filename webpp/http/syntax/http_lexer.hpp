// Created by moisrex on 9/29/20.

#ifndef WEBPP_HTTP_LEXER_HPP
#define WEBPP_HTTP_LEXER_HPP

#include "../../http/status_code.hpp"
#include "../../memory/allocators.hpp"
#include "../../std/string_view.hpp"
#include "../../std/vector.hpp"
#include "../../strings/size.hpp"
#include "tokens.hpp"

#include <array>

namespace webpp::http {

    /**
     * https://tools.ietf.org/html/rfc7230#section-3.2
     *
     *     header-field   = field-name ":" OWS field-value OWS
     *
     *     field-name     = token
     *     field-value    = *( field-content / obs-fold )
     *     field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
     *     field-vchar    = VCHAR / obs-text
     *
     *     obs-fold       = CRLF 1*( SP / HTAB )
     *                    ; obsolete line folding
     *                    ; see Section 3.2.4 in the RFC
     *
     *     OWS            = *( SP / HTAB )
     *                    ; optional whitespace
     *     RWS            = 1*( SP / HTAB )
     *                    ; required whitespace
     *     BWS            = OWS
     *                    ; "bad" whitespace
     *
     *     token          = 1*tchar
     *
     *     tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
     *                    / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
     *                    / DIGIT / ALPHA
     *                    ; any VCHAR, except delimiters
     *
     *
     *
     * http_lexer class needs will be a traits-free class.
     */
    template <istl::StringView StrViewT, Allocator AllocT>
    struct http_lexer {
        using string_view_type      = StrViewT;
        using char_type             = istl::char_type_of_t<string_view_type>;
        using allocator_type        = AllocT;
        using header_view_type      = stl::array<string_view_type, 2>;
        using header_view_allocator = rebind_allocator<allocator_type, header_view_type>;
        using header_views_type     = stl::vector<header_view_type, header_view_allocator>;

        string_view_type  raw_view{};
        string_view_type  body_view{};
        header_views_type header_views{};
        http::status_code status_code = http::status_code::ok;

        inline auto consume_next(auto&&... what_to_find) noexcept {
            auto res = raw_view.find(stl::forward<decltype(what_to_find)>(what_to_find)...);
            if (res != string_view_type::npos) {
                raw_view.remove_prefix(res);
            }
        }

        http::status_code next_line() noexcept {
            if (raw_view.starts_with(CRLF)) {
                body_view = raw_view.substr(ascii::size(CRLF));
                return http::status_code::ok;
            }
            if (auto colon = raw_view.find(':'); colon != string_view_type::npos) {
                if (auto after_spaces = raw_view.find_first_not(OWS, colon + 1);
                    after_spaces != string_view_type::npos) {
                    if (auto CRLF_found = raw_view.find(CRLF, after_spaces + 1);
                        CRLF_found != string_view_type::npos) {
                        header_views.emplace_back(stl::array<string_view_type, 2>{
                          raw_view.substr(0, colon),
                          raw_view.substr(after_spaces, CRLF_found - after_spaces)});
                        return http::status_code::ok; // Good so far
                    }
                }
            }
            return http::status_code::bad_request; // Bad Request
        }

        http::status_code consume_all() noexcept {
            return http::status_code::ok;
        }
    };

} // namespace webpp::http

#endif // WEBPP_HTTP_LEXER_HPP
