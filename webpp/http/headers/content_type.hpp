#ifndef WEBPP_CONTENT_TYPE_HPP
#define WEBPP_CONTENT_TYPE_HPP

#include "../../std/string_view.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"

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

      private:
        constexpr void parse() noexcept {
            using webpp::charset;
            if (_raw.empty()) {
                return;
            }

            string_tokenizer_type tok{_raw};

            // 1. Extract the main media type (everything before the first ';')
            if (tok.next(charset<char, 1>{';'}, _media_type)) {
                _media_type = trim(_media_type);

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
                        key = trim(key);

                        // If stopped at '=', the tokenizer is already positioned after '='
                        if (*tok.token_end() == '=') {
                            stl::string_view value;
                            // Read value up to next ';', respecting quoted strings
                            if (tok.next(charset<char, 1>{';'}, charset<char, 1>{'"'}, value)) {
                                assign_parameter(key, clean_quotes(trim(value)));
                            } else {
                                // Last parameter in the string
                                value = stl::string_view{tok.token_begin(), _raw.end()};
                                assign_parameter(key, clean_quotes(trim(value)));
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
            } else {
                // No parameters found, the entire string is the media type
                _media_type = trim(_raw);
            }
        }

      public:
        [[nodiscard]] constexpr stl::string_view view() const noexcept {
            return _raw;
        }

        // Assuming mime_type is constructible from stl::string_view
        // [[nodiscard]] mime_type media_type() const noexcept {
        //     return mime_type{_media_type};
        // }

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
        constexpr void assign_parameter(stl::string_view key, stl::string_view value) noexcept {
            if (ascii::iequals_sl(key, "boundary")) {
                _boundary = value;
            } else if (ascii::iequals_sl(key, "charset")) {
                _charset = value;
            }
        }

        // Note: Does NOT unescape backslash‑escaped characters inside quoted strings.
        [[nodiscard]] static constexpr stl::string_view clean_quotes(stl::string_view val) noexcept {
            if (val.length() >= 2 && val.front() == '"' && val.back() == '"') {
                val.remove_prefix(1);
                val.remove_suffix(1);
            }
            return val;
        }

        [[nodiscard]] static constexpr stl::string_view trim(stl::string_view val) noexcept {
            while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) {
                val.remove_prefix(1);
            }
            while (!val.empty() && (val.back() == ' ' || val.back() == '\t')) {
                val.remove_suffix(1);
            }
            return val;
        }
    };

} // namespace webpp::http

#endif // WEBPP_CONTENT_TYPE_HPP
