// Created by moisrex on 10/7/20.

#ifndef WEBPP_HTTP_HEADERS_KEEP_ALIVE_HPP
#define WEBPP_HTTP_HEADERS_KEEP_ALIVE_HPP

#include "../../convert/casts.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "./header_concepts.hpp"

namespace webpp::http {
    constexpr void parse_keep_alive_value(
      stl::string_view const            value,
      integer_cast_result<stl::size_t>& timeout,
      integer_cast_result<stl::size_t>& max) noexcept {
        if (value.empty()) {
            return;
        }

        string_tokenizer<stl::string_view> tok{value};
        while (tok.next(charset<char, 1>{','})) {
            auto const param_str = ascii::trim_copy(tok.token());
            if (param_str.empty()) {
                continue;
            }

            auto const eq_idx = param_str.find('=');
            if (eq_idx != stl::string_view::npos) {
                auto const key = ascii::trim_copy(param_str.substr(0, eq_idx));
                auto const val = ascii::trim_copy(param_str.substr(eq_idx + 1));

                // Reject negative numbers explicitly
                if (!val.empty() && val.front() == '-') {
                    continue;
                }

                if (ascii::iequals_sl(key, "timeout")) {
                    timeout = to_size_t(val);
                } else if (ascii::iequals_sl(key, "max")) {
                    max = to_size_t(val);
                }
            }
        }
    }

    /**
     * @brief Keep-Alive Header Field
     *
     * Defines how long and for how many requests a persistent connection should be kept open.
     * Example: Keep-Alive: timeout=5, max=1000
     */
    struct [[nodiscard]] basic_keep_alive : header_field_base<basic_keep_alive> {
        static constexpr stl::string_view header_name = "keep-alive";

      private:
        integer_cast_result<stl::size_t> _timeout{integer_casting_errors::invalid_character};
        integer_cast_result<stl::size_t> _max{integer_casting_errors::invalid_character};

      public:
        constexpr explicit basic_keep_alive(stl::string_view const str) noexcept
          : header_field_base<basic_keep_alive>{str} {
            parse_keep_alive_value(view(), _timeout, _max);
        }

        /**
         * @brief Check if the keep-alive header has at least one successfully parsed property
         */
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return _timeout.has_value() || _max.has_value();
        }

        [[nodiscard]] constexpr bool has_timeout() const noexcept {
            return _timeout.has_value();
        }

        [[nodiscard]] constexpr stl::size_t timeout() const noexcept {
            return _timeout.value_or(0);
        }

        [[nodiscard]] constexpr bool has_max() const noexcept {
            return _max.has_value();
        }

        [[nodiscard]] constexpr stl::size_t max() const noexcept {
            return _max.value_or(0);
        }
    };

} // namespace webpp::http

#endif // WEBPP_HTTP_HEADERS_KEEP_ALIVE_HPP
