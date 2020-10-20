//
// Created by moisrex on 10/9/20.
//

#ifndef WEBPP_VALIDATORS_HPP
#define WEBPP_VALIDATORS_HPP

#include "../std/string_view.hpp"
#include "../std/string.hpp"

namespace webpp::ascii::is {


    /**
     * @brief check if the specified character is a whitespace
     * @param c the character to check
     * @return true if c is a whitespace
     */
    [[nodiscard]] constexpr bool whitespace(istl::CharType auto&& c) noexcept {
        // todo: do we need '\v'?
        return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\v';
    }

    /**
     * @brief check if str is right trimmed
     * @param str
     * @return true if there's no whitespaces in the right side of input
     */
    [[nodiscard]] constexpr bool rtrimmed(istl::ConvertibleToStringView auto&& _str) noexcept {
        auto str = istl::string_viewify(_str);
        return !whitespace(*str.rbegin());
    }

    /**
     * @brief check if str is left trimmed
     * @param str
     * @return true if there's no whitespaces in the left side of input
     */
    [[nodiscard]] constexpr bool ltrimmed(istl::ConvertibleToStringView auto&& _str) noexcept {
        auto str = istl::string_viewify(_str);
        return !whitespace(str[0]);
    }

    /**
     * @brief check if str is right and left trimmed
     * @param str
     * @return true if there's no whitespaces in the right and left side of
     * input
     */
    [[nodiscard]] constexpr bool trimmed(istl::ConvertibleToStringView auto&& _str) noexcept {
        return ltrimmed(_str) && rtrimmed(_str);
    }

    /**
     * @brief check if the specified character is a valid number or not
     * @param character
     * @return true if the specified input is an integer
     */
    [[nodiscard]] constexpr bool digit(istl::CharType auto&& c) noexcept {
        return c >= '0' && c <= '9';
    }

    /**
     * @brief is all the characters in the specified string digits
     * @param str
     * @return true/false
     */
    [[nodiscard]] constexpr bool digit(istl::ConvertibleToStringView auto&& _str) noexcept {
        // todo: add SIMD
        auto str = istl::string_viewify(_str);
        for (auto c : str)
            if (!digit(c))
                return false;
        return !str.empty();
    }

    /**
     * @brief is a number or a dot
     * @param c
     * @return
     */
    [[nodiscard]] constexpr bool number(istl::CharType auto&& c) noexcept {
        return digit(c) || c == '.';
    }

    /**
     * @brief check if the specified string is a number (including floating
     * points)
     * @param str
     * @return true if the specified string is a number
     */
    [[nodiscard]] constexpr bool number(istl::ConvertibleToStringView auto&& _str) noexcept {
        // todo: add SIMD
        auto str = istl::string_viewify(_str);
        bool                   is_first = true;
        for (auto const& c : str) {
            if (!digit(c)) {
                if (is_first && c == '.') {
                    is_first = false;
                    continue;
                }
                return false;
            }
        }
        return !str.empty();
    }

    /**
     * @brief check if a character is lowercase
     * @param c
     * @return
     */
    [[nodiscard]] constexpr bool lowercase(istl::CharType auto&& c) noexcept {
        return c >= 'a' && c <= 'z';
    }

    /**
     * @brief checks if a string is completely lowercase or not
     * @param str
     * @return
     */

    [[nodiscard]] constexpr bool lowercase(istl::ConvertibleToStringView auto&& _str) noexcept {
        // todo: add SIMD
        auto str = istl::string_viewify(_str);
        for (auto const& c : str)
            if (!lowercase(c))
                return false;
        return true;
    }

    /**
     * @brief checks if a character is uppercase or not
     * @param c
     * @return
     */
    [[nodiscard]] constexpr bool uppercase(istl::CharType auto&& c) noexcept {
        return c >= 'A' && c <= 'Z';
    }

    /**
     * @brief checks if a string is uppercase or not
     * @param str
     * @return
     */
    [[nodiscard]] constexpr bool uppercase(istl::ConvertibleToStringView auto&& _str) noexcept {
        // todo: add SIMD
        auto str = istl::string_viewify(_str);
        for (auto const& c : str)
            if (!uppercase(c))
                return false;
        return true;
    }

    /**
     * @brief the same as digit function
     * @param str
     * @return true if the specified string is an integer
     */
    [[nodiscard]] constexpr bool integer(istl::ConvertibleToStringView auto&& str) noexcept {
        return digit(str);
    }

    /**
     * Check if the specified string is an integer and can be hold inside
     * uint8_t; which means it's between 0 and 255
     * @param str
     * @return bool
     */

    [[nodiscard]] constexpr bool uint8(istl::ConvertibleToStringView auto&& _str) noexcept {
        auto str = istl::string_viewify(_str);
        return !str.empty() && str.size() <= 3 && digit(str) && to_uint(str) <= 255;
    }

    /**
     * Check if the char is a hexadecimal character
     * @param char
     * @return bool
     */
    [[nodiscard]] constexpr bool hex(istl::CharType auto&& t) noexcept {
        return (t >= '0' && t <= '9') || (t >= 'a' && t <= 'f') || (t >= 'A' && t <= 'F');
    }

    /**
     * check if all of the characters in the string is a hexadecimal
     * character
     * @param str
     * @return bool
     */
    [[nodiscard]] constexpr bool hex(istl::ConvertibleToStringView auto&& _str) noexcept {
        auto str        = istl::string_viewify(_str);
        using char_type = istl::char_type_of<decltype(str)>;

        auto first = stl::cbegin(str);
        if (str.starts_with('-') || str.starts_with('+')) {
            ++first;
        }
        if (!stl::all_of(first, std::cend(str), [](char_type ch) {
          return hex(ch);
        })) {
            return false;
        }
        return !str.empty();
    }

}

#endif // WEBPP_VALIDATORS_HPP
