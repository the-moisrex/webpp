// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_COOKIES_HPP
#define WEBPP_REQUEST_COOKIES_HPP

#include "./cookie.hpp"

namespace webpp {

    template <istl::StringView StringViewType = stl::string_view>
    struct request_cookie {
        using string_view_type = stl::remove_cvref_t<StringViewType>;
        using char_type        = typename string_view_type::value_type;

        static constexpr auto valid_cookie_name_chars  = details::VALID_COOKIE_NAME<char_type>;
        static constexpr auto valid_cookie_value_chars = details::VALID_COOKIE_VALUE<char_type>;

        /**
         * There's no point in instantiating this class without any cookies
         */
        constexpr request_cookie() noexcept = delete;

        /**
         * Source here is in "name = value" syntax and only one single cookie
         */
        constexpr request_cookie(string_view_type const source) noexcept {
            // parsing name and value
            details::parse_SE_value(source, _name, _value, _valid);
        }


        using name_t  = string_view_type;
        using value_t = string_view_type;

        request_cookie(const request_cookie& c)     = default;
        request_cookie(request_cookie&& c) noexcept = default;

        request_cookie(name_t i_name, value_t i_value) noexcept
          : _name(trim_copy(i_name)),
            _value(trim_copy(i_value)) {}

        request_cookie& operator=(const request_cookie& c) = default;
        request_cookie& operator=(request_cookie&& c) noexcept = default;

        explicit operator bool() {
            return is_valid();
        }

        bool is_valid() const noexcept {
            // todo
            // The _valid may not catch all the validness conditions, so we have
            // to do other validation checks ourselves.
            if (!_valid)
                return false;
            return false;
        }


        auto name() const noexcept {
            // string_view is cheap to copy
            return _name;
        }
        auto value() const noexcept {
            // string_view is cheap to copy
            return _value;
        }

        [[nodiscard]] constexpr bool operator==(request_cookie const& cookie) const noexcept {
            return name() == cookie.name() && value() == cookie.value();
        }

        [[nodiscard]] constexpr bool operator!=(request_cookie const& cookie) const noexcept {
            return name() != cookie.name() && value() != cookie.value();
        }


      protected:
        name_t  _name;
        value_t _value;
        bool    _valid = false;
    };


} // namespace webpp

#endif // WEBPP_REQUEST_COOKIES_HPP
