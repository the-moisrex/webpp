// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_COOKIES_HPP
#define WEBPP_REQUEST_COOKIES_HPP

#include "cookie.hpp"

namespace webpp::http {

    /**
     * todo: do we really need StringView type? char type is always char anyway, right?
     * @tparam StringViewType
     */
    template <istl::StringView StringViewType = stl::string_view>
    struct request_cookie {
        using string_view_type = StringViewType;
        using char_type        = istl::char_type_of_t<string_view_type>;

        static constexpr auto valid_cookie_name_chars  = details::VALID_COOKIE_NAME;
        static constexpr auto valid_cookie_value_chars = details::VALID_COOKIE_VALUE;

        /**
         * There's no point in instantiating this class without any cookies
         */
        constexpr request_cookie() noexcept = delete;
        constexpr ~request_cookie()         = default;

        /**
         * Source here is in "name = value" syntax and only one single cookie
         */
        explicit constexpr request_cookie(string_view_type const source) noexcept {
            // parsing name and value
            details::parse_SE_value(source, _name, _value, _valid);
        }

        using name_t  = string_view_type;
        using value_t = string_view_type;

        constexpr request_cookie(request_cookie const& c)     = default;
        constexpr request_cookie(request_cookie&& c) noexcept = default;

        constexpr request_cookie(name_t i_name, value_t i_value) noexcept
          : _name(ascii::trim_copy(i_name)),
            _value(ascii::trim_copy(i_value)) {}

        constexpr request_cookie& operator=(request_cookie const& c)     = default;
        constexpr request_cookie& operator=(request_cookie&& c) noexcept = default;

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return is_valid();
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            // todo
            // The _valid may not catch all the validness conditions, so we have
            // to do other validation checks ourselves.
            if (!_valid) {
                return false;
            }
            return false;
        }

        constexpr auto name() const noexcept {
            // string_view is cheap to copy
            return _name;
        }

        constexpr auto value() const noexcept {
            // string_view is cheap to copy
            return _value;
        }

        [[nodiscard]] constexpr bool operator==(request_cookie const& cookie) const noexcept {
            return name() == cookie.name() && value() == cookie.value();
        }

        [[nodiscard]] constexpr bool operator!=(request_cookie const& cookie) const noexcept {
            return name() != cookie.name() && value() != cookie.value();
        }


      private:
        name_t  _name;
        value_t _value;
        bool    _valid = false;
    };


} // namespace webpp::http

#endif // WEBPP_REQUEST_COOKIES_HPP
