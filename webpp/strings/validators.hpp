//
// Created by moisrex on 10/9/20.
//

#ifndef WEBPP_VALIDATORS_HPP
#define WEBPP_VALIDATORS_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"

#include <algorithm>

namespace webpp::ascii::is {


    /**
     * @brief check if the specified character is a whitespace
     * @param inp_char the character to check
     * @return true if c is a whitespace
     */
    [[nodiscard]] constexpr bool whitespace(istl::CharType auto inp_char) noexcept {
        // todo: do we need '\v'?
        return inp_char == ' ' || inp_char == '\n' || inp_char == '\r' || inp_char == '\t' ||
               inp_char == '\f' || inp_char == '\v';
    }

    /**
     * @brief check if str is right trimmed
     * @param inp_str
     * @return true if there's no whitespaces in the right side of input
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool rtrimmed(StrT&& inp_str) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        return !whitespace(*str.rbegin());
    }

    /**
     * @brief check if str is left trimmed
     * @param inp_str
     * @return true if there's no whitespaces in the left side of input
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool ltrimmed(StrT&& inp_str) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        return !whitespace(str[0]);
    }

    /**
     * @brief check if str is right and left trimmed
     * @param inp_str
     * @return true if there's no whitespaces in the right and left side of input
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool trimmed(StrT&& inp_str) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        return ltrimmed(str) && rtrimmed(str);
    }

    /**
     * @brief check if the specified character is a valid number or not
     * @param inp_char
     * @return true if the specified input is an integer
     */
    [[nodiscard]] constexpr bool digit(istl::CharType auto inp_char) noexcept {
        return inp_char >= '0' && inp_char <= '9';
    }

    /**
     * @brief is all the characters in the specified string digits
     * @param inp_str
     * @return true if it's all digit (characters like - or + are false)
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool digit(StrT&& inp_str) noexcept {
        // todo: add SIMD
        auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        for (auto cur_char : str) {
            if (!digit(cur_char)) {
                return false;
            }
        }
        return !str.empty();
    }

    /**
     * @brief is a number or a dot
     * @param inp_char
     * @returnt true if it's a digit or a dot (`.`)
     */
    [[nodiscard]] constexpr bool number(istl::CharType auto inp_char) noexcept {
        return digit(inp_char) || inp_char == '.';
    }

    /**
     * @brief check if the specified string is a number (including floating points)
     * @param inp_str
     * @return true if the specified string is a number
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool number(StrT&& inp_str) noexcept {
        // todo: add SIMD
        auto const str      = istl::string_viewify(stl::forward<StrT>(inp_str));
        bool       is_first = true;
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
     * @param inp_char
     * @return
     */
    [[nodiscard]] constexpr bool lowercase(istl::CharType auto inp_char) noexcept {
        return inp_char >= 'a' && inp_char <= 'z';
    }

    /**
     * @brief checks if a string is completely lowercase or not
     * @param inp_str
     * @return
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool lowercase(StrT&& inp_str) noexcept {
        // todo: add SIMD
        for (auto const str = istl::string_viewify(stl::forward<StrT>(inp_str)); auto const& cur_char : str) {
            if (!lowercase(cur_char)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief checks if a character is uppercase or not
     * @param inp_char
     * @return
     */
    [[nodiscard]] constexpr bool uppercase(istl::CharType auto inp_char) noexcept {
        return inp_char >= 'A' && inp_char <= 'Z';
    }

    /**
     * @brief checks if a string is uppercase or not
     * @param inp_str
     * @return
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool uppercase(StrT&& inp_str) noexcept {
        // todo: add SIMD
        for (auto const str = istl::string_viewify(stl::forward<StrT>(inp_str)); auto const& cur_char : str) {
            if (!uppercase(cur_char)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief the same as digit function
     * @param str
     * @return true if the specified string is an integer
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool integer(StrT&& str) noexcept {
        return digit(stl::forward<StrT>(str));
    }

    /**
     * Check if the specified string is an integer and can be hold inside
     * uint8_t; which means it's between 0 and 255
     * @param inp_str
     * @return bool
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool uint8(StrT&& inp_str) noexcept {
        auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        return !str.empty() && str.size() <= 3 && digit(str) && to_uint(str) <= 255;
    }

    /**
     * Check if the char is a hexadecimal character
     * @param inp_char
     * @return bool
     */
    [[nodiscard]] constexpr bool hex(istl::CharType auto const inp_char) noexcept {
        return (inp_char >= '0' && inp_char <= '9') || (inp_char >= 'a' && inp_char <= 'f') ||
               (inp_char >= 'A' && inp_char <= 'F');
    }

    /**
     * check if all of the characters in the string is a hexadecimal character
     * @param inp_str
     * @return bool
     */
    template <istl::StringViewifiable StrT>
    [[nodiscard]] constexpr bool hex(StrT&& inp_str) noexcept {
        using char_type = istl::char_type_of_t<StrT>;

        auto const str   = istl::string_viewify(stl::forward<StrT>(inp_str));
        auto       first = stl::cbegin(str);
        if (str.starts_with('-') || str.starts_with('+')) {
            ++first;
        }
        if (!stl::all_of(first, std::cend(str), [](char_type ch) constexpr noexcept {
                return hex(ch);
            }))
        {
            return false;
        }
        return !str.empty();
    }

} // namespace webpp::ascii::is

#endif // WEBPP_VALIDATORS_HPP
