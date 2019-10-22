#ifndef WEBPP_VALIDATION_H
#define WEBPP_VALIDATION_H

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
        [[nodiscard]] constexpr bool
        empty(std::string_view const& str) noexcept {
            return str.empty();
        }

        /**
         * @brief check if these two are equal
         */
        template <typename T1, typename T2>
        [[nodiscard]] constexpr bool equals(T1 const& first,
                                            T2 const& second) noexcept {
            return first == second;
        }

        /**
         * @brief check if str contains seed
         * @param str
         * @param seed
         * @return true if it does contain it
         */
        [[nodiscard]] constexpr bool
        contains(std::string_view const& str,
                 std::string_view const& seed) noexcept {
            return str.find(seed) == std::string::npos;
        }

        /**
         * @brief check if the container contains value
         */
        template <typename T>
        [[nodiscard]] constexpr bool
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
        [[nodiscard]] constexpr bool
        contains_key(Container<T1, Args...> const& container,
                     T1 const& key) noexcept {
            return container.find(key) != std::end(container);
        }

        /**
         * @brief check if the container contains the value
         */
        template <template <class, class, class...> class Container, class T1,
                  class T2, class... Args>
        [[nodiscard]] constexpr bool
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
        [[nodiscard]] constexpr bool whitespace(char const& c) noexcept {
            return c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
                   c == '\f' || c == '\v';
            // TODO: consider using std::isspace
        }

        /**
         * @brief check if str is right trimmed
         * @param str
         * @return true if there's no whitespaces in the right side of input
         */
        [[nodiscard]] constexpr bool
        rtrimmed(std::string_view const& str) noexcept {
            return !whitespace(*str.rbegin());
        }

        /**
         * @brief check if str is left trimmed
         * @param str
         * @return true if there's no whitespaces in the left side of input
         */
        [[nodiscard]] constexpr bool
        ltrimmed(std::string_view const& str) noexcept {
            return !whitespace(str[0]);
        }

        /**
         * @brief check if str is right and left trimmed
         * @param str
         * @return true if there's no whitespaces in the right and left side of
         * input
         */
        [[nodiscard]] constexpr bool
        trimmed(std::string_view const& str) noexcept {
            return ltrimmed(str) && rtrimmed(str);
        }

        /**
         * @brief check if the specified character is a valid number or not
         * @param character
         * @return true if the specified input is an integer
         */
        [[nodiscard]] constexpr bool digit(char const& c) noexcept {
            return c >= '0' && c <= '9';
        }

        /**
         * @brief is all the characters in the specified string digits
         * @param str
         * @return true/false
         */
        [[nodiscard]] constexpr bool
        digit(std::string_view const& str) noexcept {
            for (auto const& c : str)
                if (!digit(c))
                    return false;
            return !str.empty();
        }

        /**
         * @brief is a number or a dot
         * @param c
         * @return
         */
        [[nodiscard]] constexpr bool number(char const& c) noexcept {
            return digit(c) || c == '.';
        }

        /**
         * @brief check if the specified string is a number (including floating
         * points)
         * @param str
         * @return true if the specified string is a number
         */
        [[nodiscard]] constexpr bool
        number(std::string_view const& str) noexcept {
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
            return !str.empty();
        }

        /**
         * @brief check if a character is lowercase
         * @param c
         * @return
         */
        [[nodiscard]] constexpr bool lowercase(char const& c) noexcept {
            return c >= 'a' && c <= 'z';
        }

        /**
         * @brief checks if a string is completely lowercase or not
         * @param str
         * @return
         */
        [[nodiscard]] constexpr bool
        lowercase(std::string_view const& str) noexcept {
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
        [[nodiscard]] constexpr bool uppercase(char const& c) noexcept {
            return c >= 'A' && c <= 'Z';
        }

        /**
         * @brief checks if a string is uppercase or not
         * @param str
         * @return
         */
        [[nodiscard]] constexpr bool
        uppercase(std::string_view const& str) noexcept {
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
        [[nodiscard]] constexpr bool integer(char const& str) noexcept {
            return digit(str);
        }

        /**
         * @brief the same as digit function
         * @param str
         * @return true if the specified string is an integer
         */
        [[nodiscard]] constexpr bool
        integer(std::string_view const& str) noexcept {
            return digit(str);
        }

        /**
         * Check if the specifed string is an integer and can be hold inside
         * uint8_t; which means it's between 0 and 255
         * @param str
         * @return bool
         */
        [[nodiscard]] constexpr bool
        uint8(std::string_view const& str) noexcept {
            return !str.empty() && str.size() <= 3 && digit(str) &&
                   to_uint(str) <= 255;
        }

        /**
         * Check if the char is a hexadecimal character
         * @param char
         * @return bool
         */
        [[nodiscard]] constexpr bool hex(char const& t) noexcept {
            return (t >= '0' && t <= '9') || (t >= 'a' && t <= 'f') ||
                   (t >= 'A' && t <= 'F');
        }

        /**
         * check if all of the characters in the string is a hexadecimal
         * character
         * @param str
         * @return bool
         */
        [[nodiscard]] constexpr bool hex(std::string_view const& str) noexcept {
            for (auto const& c : str)
                if (!hex(c))
                    return false;
            return !str.empty();
        }

        /**
         * @brief check if the specified str is an email or not
         * @param str
         * @return true if the specified str is an email
         */
        [[nodiscard]] bool email(std::string_view const& str) noexcept;

        [[nodiscard]] constexpr bool FQDN(std::string_view const& str) noexcept;
        [[nodiscard]] constexpr bool url(std::string_view const& str) noexcept;

        /**
         * Check if the specified Integer is an octet of a subnet mask
         * @tparam Integer
         * @param o
         * @return
         */
        template <typename Integer>
        [[nodiscard]] constexpr bool subnet_octet(Integer o) noexcept {
            constexpr auto mask = static_cast<Integer>(1)
                                  << ((sizeof(Integer) * 8) - 1);
            while ((o & mask) == mask)
                o <<= 1;
            return o == 0;
        }

        /**
         * @brief checks if the specified str is an ipv4
         * @param str
         * @return true if str is a valid ipv4
         */
        [[nodiscard]] constexpr bool ipv4(std::string_view str) noexcept {
            std::size_t next_dot = 0;
            for (uint8_t octet_index = 0; octet_index != 4; octet_index++) {
                next_dot = str.find('.');
                auto octet_str = str.substr(0, next_dot);
                if (octet_str.size() > 3 || !is::digit(octet_str) ||
                    to_uint(octet_str) > 255)
                    return false;
                str.remove_prefix(octet_str.size() + (octet_index != 3));
            }
            return str.empty();
        }

        /**
         * Check if the specified string is a valid ipv4 subnet mask or not
         * @param str
         * @return bool an indication weather or not the specified string is a
         * valid ipv4 subnet mask or not
         */
        [[nodiscard]] constexpr bool subnet(std::string_view str) noexcept {
            std::size_t next_dot = 0;
            for (uint8_t octet_index = 0; octet_index != 4; octet_index++) {
                next_dot = str.find('.');
                auto octet_str = str.substr(0, next_dot);
                if (octet_str.size() > 3 || !is::digit(octet_str)) {
                    return false;
                }
                if (auto octet_int = to_uint(octet_str);
                    octet_int > 255 || subnet_octet(octet_int))
                    return false;
                str.remove_prefix(octet_str.size() + (octet_index != 3));
            }
            return str.empty();
        }

        /**
         * Check if the specified input is a valid subnet ipv4 mask or not
         * @param octets
         * @return bool an indication weather or not the specified input is a
         * valid ipv4 subnet mask or not
         */
        [[nodiscard]] constexpr bool
        subnet(std::array<uint8_t, 4> const& octets) noexcept {
            for (auto const& octet : octets)
                if (!subnet_octet(octet))
                    return false;
            return true;
        }

        /**
         * @brief this function template will check if the ipv4 with it's prefix
         * is valid or not.
         * @example 192.168.0.1/24, 192.168.0.1:24
         */
        template <std::size_t N>
        [[nodiscard]] constexpr bool
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

        /**
         * Check if the specified string is a ipv4 plus prefix or not
         * @param str
         * @return
         */
        [[nodiscard]] constexpr bool
        ipv4_prefix(std::string_view const& str) noexcept {
            return ipv4_prefix(str, charset_t<2>{':', '/'});
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
        [[nodiscard]] constexpr bool ipv6(std::string_view address) noexcept {
            bool encountered_double_colons = false;
            std::size_t index = 0;

            if (address.starts_with('[')) {
                if (address.ends_with(']')) {
                    address.remove_suffix(1);
                    address.remove_prefix(1);
                } else {
                    return false;
                }
            }

            while (index < 8u && !address.empty()) {
                auto next_colon = address.find(':');
                auto octet = address.substr(0, next_colon);
                if (octet.empty()) {

                    // ip cannon have two double colon semantics (the first one
                    // and the last one is ok)
                    if (!address.empty() && encountered_double_colons)
                        return false;

                    if (index == 0) {
                        if (!address.starts_with("::")) {
                            return false;
                        }
                        address.remove_prefix(1);
                    }
                    encountered_double_colons = true;
                } else if (octet.size() > 4) {
                    if (ipv4(octet)) {
                        // ipv4 inside ipv6 address should be the last octet
                        return octet.size() == address.size() &&
                               ((!encountered_double_colons && index == 8u) ||
                                encountered_double_colons);
                    } else
                        return false;
                } else if (!is::hex(octet)) {
                    return false;
                }
                if (next_colon != std::string_view::npos)
                    address.remove_prefix(next_colon + 1);
                else
                    address.remove_prefix(octet.size());
                index++;
            }

            return address.empty() &&
                   ((!encountered_double_colons && index == 8u) ||
                    encountered_double_colons);
        }

        template <std::size_t N = 1>
        [[nodiscard]] constexpr bool ipv6_prefix(
            std::string_view const& str,
            charset_t<N> const& devider_chars = charset_t<1>('/')) noexcept {
            if (auto found = std::find_if(
                    std::rbegin(str), std::rend(str),
                    [&](const auto& c) { return devider_chars.contains(c); });
                found != std::rend(str)) {
                auto index = std::distance(std::begin(str), found.base()) - 1;
                if (auto prefix = str.substr(index + 1); is::digit(prefix)) {
                    int _prefix = to_uint(prefix);
                    if (!(_prefix >= 0 && _prefix <= 128))
                        return false;
                } else {
                    return false;
                }
                if (ipv6(str.substr(0, index)))
                    return true;
            }
            return false;
        }

        /**
         * @brief check if the specified string is an ipv4 or ipv6
         * @param str
         * @return true if str is ipv4 or ipv6
         */
        [[nodiscard]] constexpr bool ip(std::string_view const& str) noexcept {
            return ipv4(str) || ipv6(str);
        }

        /**
         * @brief checks if an string is a valid host based on RFC3986
         * (https://tools.ietf.org/html/rfc3986)
         * @param str
         * @return
         */
        [[nodiscard]] constexpr bool
        host(std::string_view const& str) noexcept {
            /**
             * This is the character set corresponds to the "unreserved" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto UNRESERVED =
                charset(ALPHA, DIGIT, charset_t<4>{'-', '.', '_', '~'});

            /**
             * This is the character set corresponds to the "sub-delims" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr charset_t<11> SUB_DELIMS{'!', '$', '&', '\'', '(', ')',
                                               '*', '+', ',', ';',  '='};

            /**
             * This is the character set corresponds to the last part of
             * the "IPvFuture" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto IPV_FUTURE_LAST_PART =
                charset(UNRESERVED, SUB_DELIMS, charset_t<1>{':'});

            /**
             * This is the character set corresponds to the "reg-name" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
             * leaving out "pct-encoded".
             */
            constexpr auto REG_NAME_NOT_PCT_ENCODED =
                charset(UNRESERVED, SUB_DELIMS);

            if (str.empty())
                return false;
            if (str.starts_with('[') && str.ends_with(']')) {
                if (str[1] == 'v') { // future ip
                    if (auto dot_delim = str.find('.');
                        dot_delim != std::string_view::npos) {

                        auto ipvf_version = str.substr(2, dot_delim);
                        if (!HEXDIG.contains(ipvf_version)) {
                            // ERROR: basic_uri is not valid
                            return false;
                        }

                        auto ipvf = str.substr(dot_delim + 1, str.size() - 2);
                        return IPV_FUTURE_LAST_PART.contains(ipvf);
                    }

                } else { // ipv6
                    return is::ipv6(str.substr(1, str.size() - 2));
                }
            } else if (is::digit(str[0]) && is::ipv4(str)) { // ipv4
                return true;
            } else {
                return charset(REG_NAME_NOT_PCT_ENCODED, charset('%'))
                    .contains(str);
            }

            return false;
        }

        /*
         * Check if the specified string is a query (in URI) or not
         * @return bool true if it's a query
         */
        [[nodiscard]] constexpr bool query(std::string_view str) noexcept {
            /**
             * This is the character set corresponds to the "unreserved" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto UNRESERVED =
                charset(ALPHA, DIGIT, charset_t<4>{'-', '.', '_', '~'});

            /**
             * This is the character set corresponds to the "sub-delims" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto SUB_DELIMS = charset_t<11>(
                '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=');
            /**
             * This is the character set corresponds to the "pchar" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
             * leaving out "pct-encoded".
             */
            constexpr auto PCHAR_NOT_PCT_ENCODED =
                charset(UNRESERVED, SUB_DELIMS, webpp::charset_t<2>{':', '@'});

            /**
             * This is the character set corresponds to the "query" syntax
             * and the "fragment" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
             * leaving out "pct-encoded".
             */
            constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED =
                charset(PCHAR_NOT_PCT_ENCODED, charset_t<2>{{'/', '?'}});

            return QUERY_OR_FRAGMENT_NOT_PCT_ENCODED.contains(str);
        }

        [[nodiscard]] constexpr bool
        ip_range(std::string_view const& str) noexcept;
        [[nodiscard]] constexpr bool
        ipv4_range(std::string_view const& str) noexcept;
        [[nodiscard]] constexpr bool
        ipv6_range(std::string_view const& str) noexcept;
        // bool isImage(something) noexcept;

        /**
         * Check if the specified string is a hexadecimal color
         * @param str
         * @return
         */
        [[nodiscard]] constexpr bool
        hex_color(std::string_view const& str) noexcept {
            if (!str.starts_with('#'))
                return false;
            switch (str.size()) {
            case 3 + 1:
            case 6 + 1:
            case 8 + 1:
                return is::hex(str.substr(1));
            default:
                return false;
            }
        }

        /**
         * Check if the specified string is an RGB color
         * @param str
         * @return
         */
        [[nodiscard]] bool rgb_color(std::string_view str) noexcept;

        /**
         * Check if the specified string is a RGBA HTML color
         * @param str
         * @return bool
         */
        [[nodiscard]] bool rgba_color(std::string_view str) noexcept;

        /**
         * Check if the specified string is a valid HSL and HSLA color or not
         * @param str
         * @return bool
         */
        [[nodiscard]] bool hsl_color(std::string_view str) noexcept;

        /**
         * Check if the specified string is a valid HSLA color or not
         * It's just an alias for hsl_color. Read more about the reason here:
         * https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#hsl()
         *
         * @param str
         * @return bool
         */
        [[nodiscard]] bool hsla_color(std::string_view str) noexcept;

        /**
         * Check if the specified string is a valid HTML color
         * @param str
         * @return bool
         */
        [[nodiscard]] bool name_color(std::string str) noexcept;

        /**
         * Check if the specified string is a valid string representation of a
         * color or not.
         * @param str
         * @return bool
         */
        [[nodiscard]] bool color(std::string_view const& str) noexcept;

        [[nodiscard]] constexpr bool
        mimetype(std::string_view const& str) noexcept;
        [[nodiscard]] constexpr bool UUID(std::string_view const& str) noexcept;
        [[nodiscard]] constexpr bool port(std::string_view const& str) noexcept;
        [[nodiscard]] constexpr bool
        mongoid(std::string_view const& str) noexcept;
        // you may want to change the string to a date of some sort or add both
        [[nodiscard]] bool today(std::string_view const& str) noexcept;
        [[nodiscard]] bool tomorrow(std::string_view const& str) noexcept;
        [[nodiscard]] bool yesterday(std::string_view const& str) noexcept;
        [[nodiscard]] bool this_year(std::string_view const& str) noexcept;
        [[nodiscard]] bool next_year(std::string_view const& str) noexcept;
        [[nodiscard]] bool prev_year(std::string_view const& str) noexcept;
        [[nodiscard]] bool this_month(std::string_view const& str) noexcept;
        [[nodiscard]] bool next_month(std::string_view const& str) noexcept;
        [[nodiscard]] bool prev_month(std::string_view const& str) noexcept;
        [[nodiscard]] bool this_week(std::string_view const& str) noexcept;
        [[nodiscard]] bool next_week(std::string_view const& str) noexcept;
        [[nodiscard]] bool prev_week(std::string_view const& str) noexcept;
        [[nodiscard]] bool between(std::string_view const& str,
                                   std::string_view const& from,
                                   std::string_view const& after) noexcept;
        [[nodiscard]] bool after(std::string_view const& str,
                                 std::string_view const& pointintime) noexcept;
        [[nodiscard]] bool before(std::string_view const& str,
                                  std::string_view const& pointintime) noexcept;

        [[nodiscard]] bool base64(std::string_view const& str) noexcept;
        [[nodiscard]] bool escaped(std::string_view const& str) noexcept;

        [[nodiscard]] bool username(std::string_view const& str) noexcept;
    } // namespace is
} // namespace webpp

#endif // WEBPP_VALIDATION_H
