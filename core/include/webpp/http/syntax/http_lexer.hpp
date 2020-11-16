// Created by moisrex on 9/29/20.

#ifndef WEBPP_HTTP_LEXER_HPP
#define WEBPP_HTTP_LEXER_HPP

#include "../../traits/traits_concepts.hpp"
#include "../../std/vector.hpp"
#include "../../http/status_code.hpp"
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
        using char_type = typename string_view_type::value_type;

        static constexpr stl::array<char_type, 2> CRLF{{0x0D, 0x0A}}; // CR(\r), LF(\n)
        static constexpr stl::array<char_type, 2> OWS{{0x20, 0x09}}; // SP, HTAB

        string_view_type raw_view{};
        string_view_type body_view{};
        istl::vector<traits_type, stl::array<string_view_type, 2>> header_views{};
        enum http::status_code status_code = http::status_code::ok;

        inline auto consume_next(auto &&...what_to_find) noexcept {
            auto res = raw_view.find(stl::forward<decltype(what_to_find)>(what_to_find)...);
            if (res != string_view_type::npos) {
                raw_view.remove_prefix(res);
            }
        }

        enum http::status_code next_line() noexcept {
            if (raw_view.starts_with(CRLF.data())) {
                body_view = raw_view.substr(CRLF.size());
                return http::status_code::ok;
            }
            if (auto colon = raw_view.find(':'); colon != string_view_type::npos) {
                if (auto after_spaces = raw_view.find_first_not(OWS.data(), colon + 1); after_spaces != string_view_type::npos) {
                    if (auto CRLF_found = raw_view.find(CRLF.data(), after_spaces + 1);
                        CRLF_found != string_view_type::npos) {
                        header_views.emplace_back(stl::array<string_view_type, 2>{
                          raw_view.substr(0, colon),
                          raw_view.substr(after_spaces, CRLF_found - after_spaces)
                        });
                        return http::status_code::ok; // Good so far
                    }
                }
            }
            return http::status_code::bad_request; // Bad Request
        }

        http::status_code_type consume_all() noexcept {
        }

    };

}

#endif // WEBPP_HTTP_LEXER_HPP
