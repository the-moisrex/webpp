// Created by moisrex on 12/18/22.

#ifndef WEBPP_CONTENT_ENCODING_HPP
#define WEBPP_CONTENT_ENCODING_HPP

#include "../../std/cstdint.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "../codec/common.hpp"
#include "../protocol/http_limits.hpp"
#include "./header_concepts.hpp"

#include <array>
#include <limits>
#include <span>

namespace webpp::http {

    template <stl::size_t MaxSupported = max_supported_content_encoding_values, stl::integral C = stl::size_t>
    constexpr void parse_content_encoding_value(
      stl::string_view const                      value,
      stl::array<stl::string_view, MaxSupported>& encodings,
      C&                                          count) noexcept {
        count = 0;

        if (value.empty()) {
            return;
        }

        // Content-Encoding shouldn't have quotes, semi-colons (no params), equal signs, or stars.
        if (value.find_first_of("\"=;*") != stl::string_view::npos) [[unlikely]] {
            count = stl::numeric_limits<C>::max(); // valid = false
            return;
        }

        string_tokenizer<stl::string_view> tokenizer{value};
        while (count < MaxSupported && tokenizer.next(charset{','})) {
            auto token = tokenizer.token();
            trim_lws(token);

            if (token.empty()) {
                continue;
            }

            // Internal spaces within a single token are invalid
            if (token.find_first_of(http_lws.string_view()) != stl::string_view::npos) [[unlikely]] {
                count = stl::numeric_limits<C>::max(); // valid = false
                return;
            }

            encodings[count++] = token;
        }
    }

    /**
     * @brief Content-Encoding Header Field
     * RFC 9110 Section 8.4.1
     *
     * Indicates the content codings applied to the payload.
     */
    template <stl::size_t MaxSupported = max_supported_content_encoding_values>
    struct [[nodiscard]] basic_content_encoding : header_field_base<basic_content_encoding<MaxSupported>> {
        static constexpr stl::string_view header_name = "content-encoding";

        using encoding_type = stl::string_view;
        using storage_type  = stl::array<encoding_type, MaxSupported>;
        using count_type    = stl::make_unsigned_t<istl::integer_max_t<MaxSupported, stl::size_t>>;

        constexpr explicit basic_content_encoding(stl::string_view const str) noexcept
          : header_field_base<basic_content_encoding>{str} {
            parse_content_encoding_value<MaxSupported>(this->view(), _encodings, _count);
        }

        [[nodiscard]] constexpr stl::span<encoding_type const> encodings() const noexcept {
            return {_encodings.data(), _count};
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return _count != stl::numeric_limits<count_type>::max();
        }

        /**
         * Case-insensitive check if a specific encoding is present.
         */
        [[nodiscard]] constexpr bool contains(stl::string_view const encoding) const noexcept {
            if (!is_valid()) [[unlikely]] {
                return false;
            }
            for (count_type i = 0; i < _count; ++i) {
                if (ascii::iequals(_encodings[i], encoding)) {
                    return true;
                }
            }
            return false;
        }

      private:
        storage_type _encodings{};
        count_type   _count = 0;
    };

} // namespace webpp::http

#endif // WEBPP_CONTENT_ENCODING_HPP
