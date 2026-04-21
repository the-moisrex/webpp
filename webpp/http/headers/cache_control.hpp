// Created by moisrex on 2026/04/20.

#ifndef WEBPP_CACHE_CONTROL_HPP
#define WEBPP_CACHE_CONTROL_HPP

#include "../../convert/casts.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "./header_concepts.hpp"

namespace webpp::http {

    // Forward declaration
    struct basic_cache_control;

    // Free function for parsing
    constexpr void parse_cache_control(stl::string_view str, basic_cache_control& ctrl) noexcept;

    /**
     * @brief Cache-Control Header Field
     */
    struct basic_cache_control : header_field_base<basic_cache_control> {
        static constexpr stl::string_view header_name = "cache-control";

        friend constexpr void parse_cache_control(stl::string_view str, basic_cache_control& ctrl) noexcept;

      private:
        // Bit fields for boolean directives to save space
        bool _is_valid         : 1 = true;
        bool _no_cache         : 1 = false;
        bool _no_store         : 1 = false;
        bool _no_transform     : 1 = false;
        bool _must_revalidate  : 1 = false;
        bool _proxy_revalidate : 1 = false;
        bool _public           : 1 = false;
        bool _private          : 1 = false;
        bool _immutable        : 1 = false;

        // Value directives
        integer_cast_result<stl::int64_t> _max_age{integer_casting_errors::invalid_character};
        integer_cast_result<stl::int64_t> _s_maxage{integer_casting_errors::invalid_character};
        integer_cast_result<stl::int64_t> _stale_while_revalidate{integer_casting_errors::invalid_character};
        integer_cast_result<stl::int64_t> _stale_if_error{integer_casting_errors::invalid_character};

      public:
        constexpr explicit basic_cache_control(stl::string_view const str) noexcept
          : header_field_base<basic_cache_control>{str} {
            parse_cache_control(str, *this);
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return _is_valid;
        }

        [[nodiscard]] constexpr bool no_cache() const noexcept {
            return _no_cache;
        }

        [[nodiscard]] constexpr bool no_store() const noexcept {
            return _no_store;
        }

        [[nodiscard]] constexpr bool no_transform() const noexcept {
            return _no_transform;
        }

        [[nodiscard]] constexpr bool must_revalidate() const noexcept {
            return _must_revalidate;
        }

        [[nodiscard]] constexpr bool proxy_revalidate() const noexcept {
            return _proxy_revalidate;
        }

        [[nodiscard]] constexpr bool is_public() const noexcept {
            return _public;
        }

        [[nodiscard]] constexpr bool is_private() const noexcept {
            return _private;
        }

        [[nodiscard]] constexpr bool immutable() const noexcept {
            return _immutable;
        }

        [[nodiscard]] constexpr stl::int64_t max_age() const noexcept {
            return _max_age.value_or(-1);
        }

        [[nodiscard]] constexpr stl::int64_t s_maxage() const noexcept {
            return _s_maxage.value_or(-1);
        }

        [[nodiscard]] constexpr stl::int64_t stale_while_revalidate() const noexcept {
            return _stale_while_revalidate.value_or(-1);
        }

        [[nodiscard]] constexpr stl::int64_t stale_if_error() const noexcept {
            return _stale_if_error.value_or(-1);
        }

        [[nodiscard]] constexpr bool has_max_age() const noexcept {
            return _max_age.has_value();
        }

        [[nodiscard]] constexpr bool has_s_maxage() const noexcept {
            return _s_maxage.has_value();
        }

        [[nodiscard]] constexpr bool has_stale_while_revalidate() const noexcept {
            return _stale_while_revalidate.has_value();
        }

        [[nodiscard]] constexpr bool has_stale_if_error() const noexcept {
            return _stale_if_error.has_value();
        }
    };

    /**
     * @brief Free function to parse the Cache-Control string view into the given struct.
     */
    constexpr void parse_cache_control(stl::string_view str, basic_cache_control& ctrl) noexcept {
        str = ascii::trim_copy(str);
        if (str.empty()) {
            ctrl._is_valid = false;
            return;
        }

        string_tokenizer<stl::string_view> tok{str};

        // Split by comma
        while (tok.next(charset<char, 1>{','})) {
            auto directive = ascii::trim_copy(tok.token());
            if (directive.empty()) {
                continue;
            }

            auto const eq_pos = directive.find('=');
            auto       key =
              (eq_pos != stl::string_view::npos) ? ascii::trim_copy(directive.substr(0, eq_pos)) : directive; // Already
                                                                                                              // trimmed

            // Check value directives
            if (eq_pos != stl::string_view::npos &&
                (ascii::iequals_sl(key, "max-age") || ascii::iequals_sl(key, "s-maxage") ||
                 ascii::iequals_sl(key, "stale-while-revalidate") || ascii::iequals_sl(key, "stale-if-error")))
            {
                auto val = ascii::trim_copy(directive.substr(eq_pos + 1));

                if (val.starts_with('"') && val.ends_with('"') && val.size() >= 2) {
                    val.remove_prefix(1);
                    val.remove_suffix(1);
                }

                if (ascii::iequals_sl(key, "max-age")) {
                    ctrl._max_age = to_int64(val);
                } else if (ascii::iequals_sl(key, "s-maxage")) {
                    ctrl._s_maxage = to_int64(val);
                } else if (ascii::iequals_sl(key, "stale-while-revalidate")) {
                    ctrl._stale_while_revalidate = to_int64(val);
                } else if (ascii::iequals_sl(key, "stale-if-error")) {
                    ctrl._stale_if_error = to_int64(val);
                }
            }

            // Check boolean directives (they can legally contain values, e.g. no-cache="field")
            if (ascii::iequals_sl(key, "no-cache")) {
                ctrl._no_cache = true;
            } else if (ascii::iequals_sl(key, "no-store")) {
                ctrl._no_store = true;
            } else if (ascii::iequals_sl(key, "no-transform")) {
                ctrl._no_transform = true;
            } else if (ascii::iequals_sl(key, "must-revalidate")) {
                ctrl._must_revalidate = true;
            } else if (ascii::iequals_sl(key, "proxy-revalidate")) {
                ctrl._proxy_revalidate = true;
            } else if (ascii::iequals_sl(key, "public")) {
                ctrl._public = true;
            } else if (ascii::iequals_sl(key, "private")) {
                ctrl._private = true;
            } else if (ascii::iequals_sl(key, "immutable")) {
                ctrl._immutable = true;
            }
        }
    }



} // namespace webpp::http

#endif // WEBPP_CACHE_CONTROL_HPP
