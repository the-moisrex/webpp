// Created by moisrex on 10/24/20.

#ifndef WEBPP_HEADERS_ALLOW_HPP
#define WEBPP_HEADERS_ALLOW_HPP

#include "../../strings/charset.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "../codec/tokens.hpp"
#include "../verbs.hpp"
#include "./header_concepts.hpp"

#include <climits>
#include <cstdint>

namespace webpp::http {

    // Highest bits used as flags to save space
    constexpr stl::uint64_t allow_unknown_flag = 1ULL << (sizeof(stl::uint64_t) * CHAR_BIT - 1);
    constexpr stl::uint64_t allow_valid_flag   = allow_unknown_flag >> 1U;
    constexpr stl::uint64_t allow_methods_mask = ~(allow_valid_flag | allow_unknown_flag);

    constexpr stl::size_t render_allow(char* out, stl::size_t max_length, stl::string_view const value) noexcept {
        auto* ptr = out;
        auto append = [&](stl::string_view const part) constexpr {
            auto const length = render_header_text(ptr, max_length, part);
            ptr += length;
            max_length -= length;
        };

        string_tokenizer<stl::string_view> tok{value};
        while (tok.next(charset<char, 1>{','})) {
            auto const method = ascii::trim_copy(tok.token());
            if (method.empty()) {
                continue;
            }

            if (ptr != out) {
                append(", ");
            }
            append(method);
        }

        return static_cast<stl::size_t>(ptr - out);
    }

    constexpr void parse_allow(stl::string_view const value, stl::uint64_t& data) noexcept {
        data = allow_valid_flag; // Assume valid by default

        if (value.empty()) [[unlikely]] {
            return;
        }

        string_tokenizer<stl::string_view> tok{value};

        // Methods are separated by commas
        while (tok.next(charset<char, 1>{','})) {
            auto method_str = ascii::trim_copy(tok.token());
            if (method_str.empty()) {
                continue;
            }

            // A valid HTTP method must be a valid HTTP token
            for (char const cur : method_str) {
                if (!is_http_token(cur)) {
                    data &= ~allow_valid_flag; // Mark as invalid
                    break;
                }
            }

            auto const method = string_to_verb(method_str);
            if (method != verb::unknown) {
                // Set the bit corresponding to the underlying verb integer value
                assert(+method < 62);
                data |= (1ULL << +method);
            } else {
                data |= allow_unknown_flag;
            }
        }
    }

    /**
     * The Allow entity-header field lists the set of methods supported
     * by the resource identified by the Request-URI.
     *
     * Example of use:
     *     Allow: GET, HEAD, PUT
     */
    struct [[nodiscard]] basic_allow : header_field_base<basic_allow> {
        static constexpr stl::string_view header_name = "allow";

      private:
        stl::uint64_t    _data = allow_valid_flag;
        stl::string_view _value;

      public:
        constexpr explicit basic_allow(stl::string_view const str) noexcept
          : header_field_base<basic_allow>{str},
            _value(str) {
            parse_allow(str, _data);
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            // Evaluates to false if the string contained any invalid HTTP tokens
            return (_data & allow_valid_flag) != 0;
        }

        /**
         * Check if a known verb is present in the Allow header
         */
        [[nodiscard]] constexpr bool contains(verb const method) const noexcept {
            if (method == verb::unknown) [[unlikely]] {
                return false;
            }
            return (_data & (1ULL << stl::to_underlying(method))) != 0;
        }

        /**
         * Check if a specific method string (case-sensitive as per HTTP spec) is present.
         */
        [[nodiscard]] constexpr bool contains(stl::string_view const method_str) const noexcept {
            if (auto const method = string_to_verb(method_str); method != verb::unknown) [[likely]] {
                return contains(method);
            }

            // If we don't have any unknown methods parsed, this unknown queried string isn't there
            if (!has_unknown_methods()) {
                return false;
            }

            // Fallback: tokenize and check for the custom method match on the fly
            string_tokenizer<stl::string_view> tok{_value};
            while (tok.next(charset{','})) {
                if (ascii::trim_copy(tok.token()) == method_str) {
                    return true;
                }
            }
            return false;
        }

        /**
         * Retrieve the raw bitmask of the known allowed methods.
         */
        [[nodiscard]] constexpr stl::uint64_t methods_mask() const noexcept {
            return _data & allow_methods_mask;
        }

        /**
         * Check whether any non-standard/unknown method is included.
         */
        [[nodiscard]] constexpr bool has_unknown_methods() const noexcept {
            return (_data & allow_unknown_flag) != 0;
        }

        [[nodiscard]] constexpr stl::string_view value_string() const noexcept {
            return _value;
        }
    };

    constexpr stl::size_t render(char* out, stl::size_t const max_length, basic_allow const& header) noexcept {
        if (!header.is_valid()) {
            return 0;
        }

        return render_allow(out, max_length, header.value_string());
    }

} // namespace webpp::http

#endif // WEBPP_HEADERS_ALLOW_HPP
