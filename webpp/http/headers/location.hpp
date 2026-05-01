// Created by moisrex on 2026/05/01.

#ifndef WEBPP_HTTP_HEADERS_LOCATION_HPP
#define WEBPP_HTTP_HEADERS_LOCATION_HPP

#include "../../std/iterator.hpp"
#include "../../strings/trim.hpp"
#include "./header_concepts.hpp"

#include <string_view>

namespace webpp::http {

    /**
     * @brief Location Header Field
     *
     * The Location response-header field is used to redirect the recipient to a location
     * other than the Request-URI for completion of the request or identification of a new resource.
     *
     * Example:
     *   Location: http://www.w3.org/pub/WWW/People.html
     */
    struct [[nodiscard]] basic_location : header_field_base<basic_location> {
        // Required by the HeaderField concept
        static constexpr stl::string_view header_name = "location";

      private:
        stl::string_view _uri;

      public:
        /**
         * @brief Construct a new Location object and parse the input
         *
         * @param str The raw header string value to parse
         */
        constexpr explicit basic_location(stl::string_view const str) noexcept
          : header_field_base<basic_location>{str} {
            // A Location header simply contains a URI reference.
            // We trim surrounding whitespace as per HTTP header conventions.
            _uri = ascii::trim_copy(str);
        }

        /**
         * @brief Check if the Location header is valid
         *
         * A Location header must at least contain a non-empty URI.
         * Complex URI validation is typically deferred to the URI parser.
         */
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return !_uri.empty();
        }

        /**
         * @brief Get the parsed URI reference
         */
        [[nodiscard]] constexpr stl::string_view uri() const noexcept {
            return _uri;
        }

        [[nodiscard]] constexpr stl::size_t length() const noexcept {
            return _uri.size();
        }
    };

    /**
     * @brief Render the Location header value to an output buffer
     */
    static constexpr void render(char*& out, stl::size_t const max_length, basic_location const& header) noexcept {
        if (!header.is_valid() || header.length() > max_length) [[unlikely]] {
            return;
        }

        istl::iter_append(out, header.uri());
    }

} // namespace webpp::http

#endif // WEBPP_HTTP_HEADERS_LOCATION_HPP
