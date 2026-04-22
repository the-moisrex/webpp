// Created by moisrex on 10/7/20.

#ifndef WEBPP_HTTP_HEADERS_KEEP_ALIVE_HPP
#define WEBPP_HTTP_HEADERS_KEEP_ALIVE_HPP

#include "../../convert/casts.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "./header_concepts.hpp"
#include "./parsers.hpp"

namespace webpp::http {
    static constexpr void render_keep_alive(
      char*&                                    out,
      stl::size_t                               max_length,
      integer_cast_result<stl::uint16_t> const& timeout,
      integer_cast_result<stl::uint16_t> const& max) noexcept {
        using istl::iter_append;
        constexpr stl::size_t estimated_length = stl::size("timeout=65535, max=65535");
        if (max_length < estimated_length) [[unlikely]] {
            return;
        }
        if (timeout.has_value()) {
            max_length -= iter_append(out, "timeout=");
            max_length -= render_decimal(out, max_length, timeout.value());
            if (max.has_value()) {
                max_length -= iter_append(out, ',', ' ');
            }
        }
        if (max.has_value()) {
            max_length -= iter_append(out, "max=");
            render_decimal(out, max_length, max.value());
        }
    }

    constexpr void parse_keep_alive(
      stl::string_view const              value,
      integer_cast_result<stl::uint16_t>& timeout,
      integer_cast_result<stl::uint16_t>& max) noexcept {
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
                    timeout = to_uint16(val);
                } else if (ascii::iequals_sl(key, "max")) {
                    max = to_uint16(val);
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
        integer_cast_result<stl::uint16_t> _timeout{integer_casting_errors::invalid_character};
        integer_cast_result<stl::uint16_t> _max{integer_casting_errors::invalid_character};

      public:
        constexpr explicit basic_keep_alive(stl::string_view const str) noexcept
          : header_field_base<basic_keep_alive>{str} {
            parse_keep_alive(str, _timeout, _max);
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

        [[nodiscard]] constexpr integer_cast_result<stl::uint16_t> const& timeout_result() const noexcept {
            return _timeout;
        }

        [[nodiscard]] constexpr integer_cast_result<stl::uint16_t> const& max_result() const noexcept {
            return _max;
        }
    };

    static constexpr void render(char*& out, stl::size_t const max_length, basic_keep_alive const& header) noexcept {
        if (!header.is_valid()) [[unlikely]] {
            return;
        }
        render_keep_alive(out, max_length, header.timeout_result(), header.max_result());
    }

} // namespace webpp::http

#endif // WEBPP_HTTP_HEADERS_KEEP_ALIVE_HPP
