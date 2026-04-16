#ifndef WEBPP_CONTENT_TYPE_HPP
#define WEBPP_CONTENT_TYPE_HPP

#include "../../std/string_view.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"

namespace webpp::http {

    struct basic_content_type {
        using string_tokenizer_type = string_tokenizer<stl::string_view>;

      private:
        stl::string_view _raw;
        stl::string_view _media_type;
        stl::string_view _boundary;
        stl::string_view _charset;

      public:
        constexpr explicit basic_content_type(stl::string_view const str) noexcept : _raw{str} {
            parse();
        }

        constexpr basic_content_type& operator=(stl::string_view const str) noexcept {
            _raw = str;
            parse();
            return *this;
        }

        // A Content-Type header is only valid if it contains at least a valid media type.
        // For instance, "text/html" is valid, but an empty string or just parameters without a media type is not.
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return !_media_type.empty();
        }

        [[nodiscard]] constexpr stl::string_view view() const noexcept {
            return _raw;
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

      private:
        constexpr void parse() noexcept {
            using webpp::charset;
            if (_raw.empty()) {
                return;
            }

            string_tokenizer_type tok{_raw};

            // 1. Extract the main media type (everything before the first ';')
            if (tok.next(charset<char, 1>{';'}, _media_type)) {
                _media_type = ascii::trim_copy(_media_type);

                // 2. Process parameters (e.g., charset=utf-8; boundary="---123")
                while (!tok.at_end()) {
                    // Skip over semicolons and spaces between parameters
                    tok.skip(charset<char, 2>{';', ' '});
                    if (tok.at_end()) {
                        break;
                    }

                    stl::string_view key;
                    // Read parameter name up to '=' or ';'
                    if (tok.next(charset<char, 2>{'=', ';'}, key)) {
                        key = ascii::trim_copy(key);

                        if (tok.expect(charset<char, 1>{'='})) {
                            tok.skip(charset{' ', '\t'}); // skip optional spaces after '='
                            if (tok.at_end()) {
                                break;
                            }

                            stl::string_view value;

                            // CAPTURE START POSITION HERE
                            auto const* value_start = tok.token_end();

                            if (*value_start == '"') {
                                // Delegate to parse_quoted when encountering double quotes
                                auto const pq = parse_quoted(value_start, _raw.end(), '"');
                                assign_parameter(key, pq.value);
                                // Advance our tokenizer past the extracted quote
                                tok.reset(pq.next, _raw.end());
                            } else if (tok.next(charset<char, 1>{';'}, value)) {
                                assign_parameter(key, ascii::trim_copy(value));
                            } else {
                                // Last parameter in the string
                                // USE CAPTURED POSITION INSTEAD OF tok.token_begin()
                                value = stl::string_view{value_start, _raw.end()};
                                assign_parameter(key, ascii::trim_copy(value));
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
            } else {
                // No parameters found, the entire string is the media type
                _media_type = ascii::trim_copy(_raw);
            }
        }

        constexpr void assign_parameter(stl::string_view key, stl::string_view value) noexcept {
            if (ascii::iequals_sl(key, "boundary")) {
                _boundary = value;
            } else if (ascii::iequals_sl(key, "charset")) {
                _charset = value;
            }
        }
    };

} // namespace webpp::http

#endif // WEBPP_CONTENT_TYPE_HPP
