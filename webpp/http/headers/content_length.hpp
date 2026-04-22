// Created by moisrex on 2026/04/18.

#ifndef WEBPP_CONTENT_LENGTH_HPP
#define WEBPP_CONTENT_LENGTH_HPP

#include "../../convert/casts.hpp"
#include "../../strings/trim.hpp"
#include "./header_concepts.hpp"
#include "./parsers.hpp"

namespace webpp::http {

    constexpr void parse_content_length(stl::string_view value, integer_cast_result<stl::size_t>& result) noexcept {
        if (value.empty()) [[unlikely]] {
            result = integer_casting_errors::invalid_character;
            return;
        }
        ascii::trim(value);
        result = to_size_t(value);
    }

    /**
     * @brief Content-Length Header Field
     *
     * The Content-Length header indicates the size of the message body, in bytes,
     * sent to the recipient. It satisfies the `HeaderField` concept.
     */
    struct basic_content_length : header_field_base<basic_content_length> {
        // Required by the HeaderField concept for identification
        static constexpr stl::string_view header_name = "content-length";

      private:
        // Use the casting result directly to track both length and validity state
        integer_cast_result<stl::size_t> _length{integer_casting_errors::invalid_character};

      public:
        /**
         * @brief Construct a new content length object and parse the input
         *
         * @param str The raw header string value to parse
         */
        constexpr explicit basic_content_length(stl::string_view const str) noexcept
          : header_field_base<basic_content_length>{str} {
            parse_content_length(str, _length);
        }

        /**
         * @brief Check if the content-length was successfully and safely parsed
         *
         * A Content-Length header is valid if it contains only numeric characters
         * in base 10 and fits within the bounds of `std::size_t`.
         */
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return _length.has_value();
        }

        /**
         * @brief Get the parsed length in bytes
         *
         * @return std::size_t The length value. Defaults to 0 if invalid.
         */
        [[nodiscard]] constexpr stl::size_t value() const noexcept {
            return _length.value_or(0);
        }
    };

    static constexpr void render(char*& out, stl::size_t max_length, basic_content_length const& header) noexcept {
        render_decimal(out, max_length, header.value());
    }

} // namespace webpp::http

#endif // WEBPP_CONTENT_LENGTH_HPP
