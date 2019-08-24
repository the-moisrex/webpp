#ifndef VALIDATION_H
#define VALIDATION_H

#include "../std/string_view.h"
#include "../utils/casts.h"
#include "../utils/charset.h"
#include <algorithm>
#include <regex>

namespace webpp {

    namespace is {

        /**
         * @brief check if the string is empty or not
         * @return true if the string is empty
         */
        constexpr inline bool empty(std::string_view const& str) noexcept {
            return str.empty();
        }

        /**
         * @brief check if these two are equal
         */
        template <typename T1, typename T2>
        constexpr inline bool equals(T1 const& first,
                                     T2 const& second) noexcept {
            return first == second;
        }

        /**
         * @brief check if str contains seed
         * @param str
         * @param seed
         * @return true if it does contain it
         */
        constexpr inline bool contains(std::string_view const& str,
                                       std::string_view const& seed) noexcept {
            return str.find(seed) == std::string::npos;
        }

        /**
         * @brief check if the container contains value
         */
        template <typename T>
        constexpr inline bool
        contains(std::initializer_list<T> const& container,
                 T const& value) noexcept {
            return std::find(std::cbegin(container), std::cend(container),
                             value) != std::cend(container);
        }

        /**
         * @brief check if the container contains key
         */
        template <template <class, class...> class Container, class T1,
                  class... Args>
        constexpr inline bool
        contains_key(Container<T1, Args...> const& container,
                     T1 const& key) noexcept {
            return container.find(key) != std::end(container);
        }

        /**
         * @brief check if the container contains the value
         */
        template <template <class, class, class...> class Container, class T1,
                  class T2, class... Args>
        constexpr inline bool
        contains_value(Container<T1, T2, Args...> const& container,
                       T2 const& value) noexcept {
            for (auto pair : container)
                if (pair.second == value)
                    return true;
            return false;
        }

        /**
         * @brief check if the specified character is a whitespace
         * @param c the character to check
         * @return true if c is a whitespace
         */
        constexpr inline bool whitespace(char const& c) noexcept {
            return c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
                   c == '\f' || c == '\v';
            // TODO: consider using std::isspace
        }

        /**
         * @brief check if str is right trimmed
         * @param str
         * @return true if there's no whitespaces in the right side of input
         */
        constexpr inline bool rtrimmed(std::string_view const& str) noexcept {
            return !whitespace(*str.rbegin());
        }

        /**
         * @brief check if str is left trimmed
         * @param str
         * @return true if there's no whitespaces in the left side of input
         */
        constexpr inline bool ltrimmed(std::string_view const& str) noexcept {
            return !whitespace(str[0]);
        }

        /**
         * @brief check if str is right and left trimmed
         * @param str
         * @return true if there's no whitespaces in the right and left side of
         * input
         */
        constexpr inline bool trimmed(std::string_view const& str) noexcept {
            return ltrimmed(str) && rtrimmed(str);
        }

        /**
         * @brief check if the specified character is a valid number or not
         * @param character
         * @return true if the specified input is an integer
         */
        constexpr inline bool digit(char const& c) noexcept {
            return c >= '0' && c <= '9';
        }

        /**
         * @brief is all the characters in the specified string digits
         * @param str
         * @return true/false
         */
        constexpr inline bool digit(std::string_view const& str) noexcept {
            for (auto const& c : str)
                if (!digit(c))
                    return false;
            return true;
        }

        /**
         * @brief is a number or a dot
         * @param c
         * @return
         */
        constexpr inline bool number(char const& c) noexcept {
            return digit(c) || '.';
        }

        /**
         * @brief check if the specified string is a number (including floating
         * points)
         * @param str
         * @return true if the specified string is a number
         */
        constexpr inline bool number(std::string_view const& str) noexcept {
            bool is_first = true;
            for (auto const& c : str) {
                if (!digit(c)) {
                    if (is_first && c == '.') {
                        is_first = false;
                        continue;
                    }
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief check if a character is lowercase
         * @param c
         * @return
         */
        constexpr inline bool lowercase(char const& c) noexcept {
            return c >= 'a' && c <= 'z';
        }

        /**
         * @brief checks if a string is completely lowercase or not
         * @param str
         * @return
         */
        constexpr inline bool lowercase(std::string_view const& str) noexcept {
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
        constexpr inline bool uppercase(char const& c) noexcept {
            return c >= 'A' && c <= 'Z';
        }

        /**
         * @brief checks if a string is uppercase or not
         * @param str
         * @return
         */
        constexpr inline bool uppercase(std::string_view const& str) noexcept {
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
        constexpr inline bool integer(char const& str) noexcept {
            return digit(str);
        }

        /**
         * @brief the same as digit function
         * @param str
         * @return true if the specified string is an integer
         */
        constexpr inline bool integer(std::string_view const& str) noexcept {
            return digit(str);
        }

        /**
         * @brief check if the specified str is an email or not
         * @param str
         * @return true if the specified str is an email
         */
        constexpr inline bool email(std::string_view const& str) noexcept {
            // TODO: Try not using regular expression, it's slow
            static const std::regex pattern{
                "^[_A-Za-z0-9-\\+]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+(\\.[A-Za-"
                "z0-9]+)*(\\.[A-Za-z]{2,})$"};
            return std::regex_match(str, pattern);
        }

        constexpr bool FQDN(std::string_view const& str) noexcept;
        constexpr bool url(std::string_view const& str) noexcept;

        /**
         * @brief checks if the specified str is an ipv4
         * @param str
         * @return true if str is a valid ipv4
         */
        constexpr bool ipv4(std::string_view const& str) noexcept;

        /**
         * @brief this function template will check if the ipv4 with it's prefix
         * is valid or not.
         * @example 192.168.0.1/24, 192.168.0.1:24
         */
        template <std::size_t N>
        constexpr inline bool
        ipv4_prefix(std::string_view const& str,
                    charset_t<N> const& devider_chars) noexcept {
            if (auto found = std::find_if(
                    std::rbegin(str), std::rend(str),
                    [&](const auto& c) { return devider_chars.contains(c); });
                found != std::rend(str)) {
                auto index = std::distance(std::begin(str), found.base()) - 1;
                if (!ipv4(str.substr(0, index)))
                    return false;
                if (auto prefix = str.substr(index + 1); is::digit(prefix)) {
                    auto _prefix = to_uint(prefix);
                    return _prefix >= 0 && _prefix <= 32;
                }
                return false;
            }
            return false;
        }

        constexpr inline bool
        ipv4_prefix(std::string_view const& str) noexcept {
            return ipv4_prefix(
                str, charset_t<2>(std::initializer_list<char>{':', '/'}));
        }

        /**
         * This function checks to make sure the given address
         * is a valid IPv6 address according to the rules in
         * RFC 3986 (https://tools.ietf.org/html/rfc3986).
         *
         * @param[in] address
         *     This is the IPv6 address to validate.
         *
         * @return
         *     An indication of whether or not the given address
         *     is a valid IPv6 address is returned.
         */
        constexpr bool ipv6(std::string_view const& str) noexcept;

        template <std::size_t N>
        constexpr bool ipv6_prefix(std::string_view const& str,
                                   charset_t<N> const& devider_chars) noexcept {
            if (auto found = std::find_if(
                    std::rbegin(str), std::rend(str),
                    [&](const auto& c) { return devider_chars.contains(c); });
                found != std::rend(str)) {
                auto index = std::distance(std::begin(str), found.base()) - 1;
                if (!ipv4(str.substr(0, index)))
                    return false;
                if (auto prefix = str.substr(index + 1); is::digit(prefix)) {
                    int _prefix = to_uint(prefix);
                    return _prefix >= 0 && _prefix <= 128;
                }
                return false;
            }
            return false;
        }

        constexpr bool ipv6_prefix(std::string_view const& str) noexcept {
            return ipv6_prefix(str,
                               charset_t<1>(std::initializer_list<char>{'/'}));
        }

        /**
         * @brief check if the specified string is an ipv4 or ipv6
         * @param str
         * @return true if str is ipv4 or ipv6
         */
        constexpr inline bool ip(std::string_view const& str) noexcept {
            return ipv4(str) || ipv6(str);
        }

        constexpr bool ip_range(std::string_view const& str) noexcept;
        constexpr bool ipv4_range(std::string_view const& str) noexcept;
        constexpr bool ipv6_range(std::string_view const& str) noexcept;
        // bool isImage(something) noexcept;
        constexpr bool hex_color(std::string_view const& str) noexcept;
        constexpr bool name_color(std::string_view const& str) noexcept;

        constexpr inline bool color(std::string_view const& str) noexcept {
            return hex_color(str) || name_color(str);
        }

        constexpr bool mimetype(std::string_view const& str) noexcept;
        constexpr bool UUID(std::string_view const& str) noexcept;
        constexpr bool port(std::string_view const& str) noexcept;
        constexpr bool mongoid(std::string_view const& str) noexcept;
        // you may want to change the string to a date of some sort or add both
        bool today(std::string_view const& str) noexcept;
        bool tomorrow(std::string_view const& str) noexcept;
        bool yesterday(std::string_view const& str) noexcept;
        bool this_year(std::string_view const& str) noexcept;
        bool next_year(std::string_view const& str) noexcept;
        bool prev_year(std::string_view const& str) noexcept;
        bool this_month(std::string_view const& str) noexcept;
        bool next_month(std::string_view const& str) noexcept;
        bool prev_month(std::string_view const& str) noexcept;
        bool this_week(std::string_view const& str) noexcept;
        bool next_week(std::string_view const& str) noexcept;
        bool prev_week(std::string_view const& str) noexcept;
        bool between(std::string_view const& str, std::string_view const& from,
                     std::string_view const& after) noexcept;
        bool after(std::string_view const& str,
                   std::string_view const& pointintime) noexcept;
        bool before(std::string_view const& str,
                    std::string_view const& pointintime) noexcept;

        bool base64(std::string_view const& str) noexcept;
        bool escaped(std::string_view const& str) noexcept;

        bool username(std::string_view const& str) noexcept;
    } // namespace is
} // namespace webpp

#endif // VALIDATION_H
