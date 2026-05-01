#ifndef WEBPP_CONTENT_TYPE_HPP
#define WEBPP_CONTENT_TYPE_HPP

#include "../std/iterator.hpp"
#include "../strings/charset.hpp"
#include "../strings/iequals.hpp"
#include "../strings/string_tokenizer.hpp"
#include "../strings/trim.hpp"
#include "./header_concepts.hpp"
#include "./parsers.hpp"

namespace webpp::http {

    [[nodiscard]] static constexpr stl::size_t length_of_content_type(
      stl::string_view const media_type,
      stl::string_view const boundary,
      stl::string_view const charset) noexcept {
        stl::size_t length  = media_type.size() + boundary.size() + charset.size();
        length             += boundary.empty() ? 0 : stl::string_view{"; boundary="}.size();
        length             += charset.empty() ? 0 : stl::string_view{"; charset="}.size();
        return length;
    }

    namespace details {
        static constexpr void render_content_type(
          char*&                 out,
          stl::string_view const media_type,
          stl::string_view const boundary = {},
          stl::string_view const charset  = {}) noexcept {
            using istl::iter_append;


            iter_append(out, media_type);
            if (!boundary.empty()) {
                iter_append(out, "; boundary=");
                iter_append(out, boundary);
            }
            if (!charset.empty()) {
                iter_append(out, "; charset=");
                iter_append(out, charset);
            }
        }

        static constexpr void assign_content_type_parameter(
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
    } // namespace details

    static constexpr void parse_content_type(
      stl::string_view const value,
      stl::string_view&      media_type,
      stl::string_view&      boundary,
      stl::string_view&      charset) noexcept {
        using details::assign_content_type_parameter;

        media_type = {};
        boundary   = {};
        charset    = {};
        if (value.empty()) {
            return;
        }

        string_tokenizer<stl::string_view> tok{value};

        // 1. Extract the main media type (everything before the first ';')
        if (tok.next(webpp::charset{';'}, media_type)) {
            media_type = ascii::trim_copy(media_type);

            // 2. Process parameters (e.g., charset=utf-8; boundary="---123")
            while (!tok.at_end()) {
                // Skip over semicolons and spaces between parameters
                tok.skip(webpp::charset{';', ' '});
                if (tok.at_end()) {
                    break;
                }

                stl::string_view key;
                // Read parameter name up to '=' or ';'
                if (tok.next(webpp::charset{'=', ';'}, key)) {
                    key = ascii::trim_copy(key);

                    if (tok.expect(webpp::charset{'='})) {
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
                        } else if (tok.next(webpp::charset{';'}, value_part)) {
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

        [[nodiscard]] constexpr stl::size_t length() const noexcept {
            return length_of_content_type(_media_type, _boundary, _charset);
        }

        [[nodiscard]] constexpr stl::size_t max_length() const noexcept {
            return length();
        }
    };

    constexpr void render(char*& out, stl::size_t const max_length, basic_content_type const& header) noexcept {
        if (!header.is_valid() || header.max_length() > max_length) [[unlikely]] {
            return;
        }

        details::render_content_type(out, header.media_type_string(), header.boundary(), header.charset());
    }

} // namespace webpp::http

#endif // WEBPP_CONTENT_TYPE_HPP
