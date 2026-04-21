#ifndef WEBPP_CONTENT_TYPE_HPP
#define WEBPP_CONTENT_TYPE_HPP

#include "../../strings/charset.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "./header_concepts.hpp"

namespace webpp::http {

    constexpr stl::size_t render_content_type(
      char*                  out,
      stl::size_t            max_length,
      stl::string_view const media_type,
      stl::string_view const boundary = {},
      stl::string_view const charset  = {}) noexcept {
        auto* ptr    = out;
        auto  append = [&](stl::string_view const value) constexpr {
            auto const length = render_header_text(ptr, max_length, value);
            stl::advance(ptr, static_cast<stl::ptrdiff_t>(length));
            max_length -= length;
        };

        append(media_type);
        if (!boundary.empty()) {
            append("; boundary=");
            append(boundary);
        }
        if (!charset.empty()) {
            append("; charset=");
            append(charset);
        }

        return static_cast<stl::size_t>(ptr - out);
    }

    constexpr void assign_content_type_parameter(
      stl::string_view& boundary,
      stl::string_view& charset,
      stl::string_view  key,
      stl::string_view  value) noexcept {
        if (ascii::iequals_sl(key, "boundary")) {
            boundary = value;
        } else if (ascii::iequals_sl(key, "charset")) {
            charset = value;
        }
    }

    constexpr void parse_content_type(
      stl::string_view const value,
      stl::string_view&      media_type,
      stl::string_view&      boundary,
      stl::string_view&      charset) noexcept {
        media_type = {};
        boundary   = {};
        charset    = {};
        if (value.empty()) {
            return;
        }

        string_tokenizer<stl::string_view> tok{value};

        // 1. Extract the main media type (everything before the first ';')
        if (tok.next(webpp::charset<char, 1>{';'}, media_type)) {
            media_type = ascii::trim_copy(media_type);

            // 2. Process parameters (e.g., charset=utf-8; boundary="---123")
            while (!tok.at_end()) {
                // Skip over semicolons and spaces between parameters
                tok.skip(webpp::charset<char, 2>{';', ' '});
                if (tok.at_end()) {
                    break;
                }

                stl::string_view key;
                // Read parameter name up to '=' or ';'
                if (tok.next(webpp::charset<char, 2>{'=', ';'}, key)) {
                    key = ascii::trim_copy(key);

                    if (tok.expect(webpp::charset<char, 1>{'='})) {
                        tok.skip(webpp::charset{' ', '\t'}); // skip optional spaces after '='
                        if (tok.at_end()) {
                            break;
                        }

                        stl::string_view value_part;
                        auto const*      value_start = tok.token_end();

                        if (*value_start == '"') {
                            auto const parsed_quote = parse_quoted(value_start, value.end(), '"');
                            assign_content_type_parameter(boundary, charset, key, parsed_quote.value);
                            tok.reset(parsed_quote.next, value.end());
                        } else if (tok.next(webpp::charset<char, 1>{';'}, value_part)) {
                            assign_content_type_parameter(boundary, charset, key, ascii::trim_copy(value_part));
                        } else {
                            value_part = stl::string_view{value_start, value.end()};
                            assign_content_type_parameter(boundary, charset, key, ascii::trim_copy(value_part));
                            break;
                        }
                    }
                } else {
                    break;
                }
            }
        } else {
            // No parameters found, the entire string is the media type
            media_type = ascii::trim_copy(value);
        }
    }

    struct basic_content_type : header_field_base<basic_content_type> {
        static constexpr stl::string_view header_name = "content-type";

      private:
        stl::string_view _media_type;
        stl::string_view _boundary;
        stl::string_view _charset;

      public:
        constexpr explicit basic_content_type(std::string_view const str) noexcept : header_field_base{str} {
            parse_content_type(str, _media_type, _boundary, _charset);
        }

        // A Content-Type header is only valid if it contains at least a valid media type.
        // For instance, "text/html" is valid, but an empty string or just parameters without a media type is not.
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return !_media_type.empty();
        }

        [[nodiscard]] constexpr stl::string_view media_type_string() const noexcept {
            return _media_type;
        }

        [[nodiscard]] constexpr stl::string_view boundary() const noexcept {
            return _boundary;
        }

        [[nodiscard]] constexpr bool is_multipart() const noexcept {
            return _media_type.starts_with("multipart/");
        }

        [[nodiscard]] constexpr stl::string_view charset() const noexcept {
            return _charset;
        }
    };

    constexpr stl::size_t render(char* out, stl::size_t const max_length, basic_content_type const& header) noexcept {
        if (!header.is_valid()) {
            return 0;
        }

        return render_content_type(out, max_length, header.media_type_string(), header.boundary(), header.charset());
    }

} // namespace webpp::http

#endif // WEBPP_CONTENT_TYPE_HPP
