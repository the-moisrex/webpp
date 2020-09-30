// Created by moisrex on 9/18/20.

#ifndef WEBPP_REQUEST_PARSER_HPP
#define WEBPP_REQUEST_PARSER_HPP

#include "./http_version.hpp"
#include "../../traits/traits_concepts.hpp"
#include "../../utils/strings.hpp"

namespace webpp {


    /**
     * This class will parse HTTP requests but not fully. This class is designed to only parse the parts that
     * are necessary to parse and avoid parsing places that you might not need them to be parsed and leaves
     * those places to the other related classes.
     * For example, even though this class parses the HTTP headers, but it doesn't understand each header
     * attributes or values. For instance, the cookies will be parsed by the cookie class and this class
     * will not parse those.
     *
     * @tparam TraitsType
     */
    template <Traits TraitsType>
    struct http_request_parser {
        using traits_type = TraitsType;
        using string_type = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using status_code_type = uint_fast16_t;

        // todo: add utilities so the user is able to change these limits
        static constexpr auto METHOD_LIMIT = 10; // return HTTP error 501 (not implemented)
        static constexpr auto URI_LIMIT = 8000; // return HTTP error 414 (URI too long)

        string_view_type method_view{};
        string_view_type request_target_view{};
        string_view_type http_version_view{};

        // get the parsed http version
        [[nodiscard]] http_version get_http_version() const noexcept {
            return {http_version_view};
        }

        // parse the request status line (the first line of the request)
        status_code_type parse_request_line(string_view_type str) noexcept  {
            // https://tools.ietf.org/html/rfc7230#section-3.1.1
            //
            // request-line   = method SP request-target SP HTTP-version CRLF
            // method is case-sensitive
            // SP is single space
            // HTTP-version = HTTP-name "/" DIGIT "." DIGIT
            // HTTP-name = %x48.54.54.50 ; HTTP

            // -------------------------------- parsing method ------------------------------------

            if (auto sp = str.substr(0, METHOD_LIMIT).find(' '); sp != string_view_type::npos) { // find the first SP
                method_view = str.substr(0, sp);
                str.remove_prefix(sp + 1);
            } else {
                return 501; // Now we must return a 501 (Not Implemented) error message to the user
            }

            // ------------------------------ parsing request target (path) ------------------------------

            if (auto sp = str.substr(0, URI_LIMIT).find(' '); sp != string_view_type::npos) { // find the second SP
                request_target_view = str.substr(0, sp);
                str.remove_prefix(sp + 1);
            } else {
                return 414; // URI is either too long or the request has a wrong syntax
            }

            // ------------------------------ parsing http version ------------------------------

            static constexpr char http_prefix[]{"HTTP/"};
            if (!str.starts_with(http_prefix)) {
                return 400; // Bad Request
            }

//            const auto CRLF = str.find("\r\n", size(http_prefix));
//            if (CRLF == string_view_type::npos) {
//                return 400; // Bad Request
//            }

            http_version_view = str.substr(size(http_prefix) - 1, 3); // 1.1 and 1.0 are 3 chars
            if (http_version_view != "1.0" && http_version_view != "1.1") { // todo: add 2.0 and 0.9 and others as well
                return 505; // HTTP Version Not Supported
            }

            // str.remove_prefix(CRLF + 2); // move the string_view to the next line

            return 200; // so far, it's a good request
        }



        // parse one line of a header (A header field as it called in the RFC)
        status_code_type parse_header_field(string_view_type &str) noexcept {


        }


        // parse the header fully
        void parse_header(string_view_type str) noexcept {
            auto finish_line = str.end();
            for (auto it = str.begin(); it != finish_line; ++it) {

            }
        }

    };

}

#endif // WEBPP_REQUEST_PARSER_HPP
