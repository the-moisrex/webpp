// Created by moisrex on 10/24/20.

#ifndef WEBPP_HEADERS_ALLOW_HPP
#define WEBPP_HEADERS_ALLOW_HPP

#include "../../strings/charset.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "../verbs.hpp"
#include "./header_concepts.hpp"

#include <climits>
#include <cstdint>

namespace webpp::http {

    constexpr void parse_allow_value(stl::string_view const value, stl::uint64_t& methods, bool& has_unknown) noexcept {
        methods     = 0;
        has_unknown = false;

        if (value.empty()) {
            return;
        }

        string_tokenizer<stl::string_view> tok{value};

        // Methods are separated by commas
        while (tok.next(charset<char, 1>{','})) {
            auto method_str = ascii::trim_copy(tok.token());
            if (method_str.empty()) {
                continue;
            }

            auto const method = string_to_verb(method_str);
            if (method != verb::unknown) {
                // Set the bit corresponding to the underlying verb integer value
                assert(+method < (sizeof(stl::uint64_t) * CHAR_BIT));
                methods |= (1ULL << +method);
            } else {
                has_unknown = true;
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
        stl::uint64_t _methods     = 0;
        bool          _has_unknown = false;

      public:
        constexpr explicit basic_allow(stl::string_view const str) noexcept : header_field_base<basic_allow>{str} {
            parse_allow_value(view(), _methods, _has_unknown);
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            // An empty Allow header is technically valid, and it strictly informs
            // the recipient of supported methods (which can be none or fully unknown).
            return true;
        }

        /**
         * Check if a known verb is present in the Allow header
         */
        [[nodiscard]] constexpr bool contains(verb const method) const noexcept {
            if (method == verb::unknown) [[unlikely]] {
                return false;
            }
            return (_methods & (1ULL << stl::to_underlying(method))) != 0;
        }

        /**
         * Check if a specific method string (case-sensitive as per HTTP spec) is present.
         */
        [[nodiscard]] constexpr bool contains(stl::string_view const method_str) const noexcept {
            if (auto const method = string_to_verb(method_str); method != verb::unknown) [[likely]] {
                return contains(method);
            }

            // If we don't have any unknown methods parsed, this unknown queried string isn't there
            if (!_has_unknown) {
                return false;
            }

            // Fallback: tokenize and check for the custom method match on the fly
            string_tokenizer<stl::string_view> tok{view()};
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
            return _methods;
        }

        /**
         * Check whether any non-standard/unknown method is included.
         */
        [[nodiscard]] constexpr bool has_unknown_methods() const noexcept {
            return _has_unknown;
        }
    };


} // namespace webpp::http

#endif // WEBPP_HEADERS_ALLOW_HPP
