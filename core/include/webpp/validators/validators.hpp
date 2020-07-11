#ifndef WEBPP_VALIDATION_H
#define WEBPP_VALIDATION_H

#include "../std/string_view.h"
#include "../utils/casts.h"
#include "../utils/charset.h"
#include "../utils/strings.h"

#include <algorithm>
#include <regex>

namespace webpp {

    namespace is {

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
        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        contains(stl::basic_string_view<CharT> const& str,
                 stl::basic_string_view<CharT> const& seed) noexcept {
            return str.find(seed) == stl::basic_string_view<CharT>::npos;
        }

        /**
         * @brief check if the container contains value
         */
        template <typename T>
        [[nodiscard]] constexpr bool
        contains(stl::initializer_list<T> const& container,
                 T const&                        value) noexcept {
            return stl::find(stl::cbegin(container), stl::cend(container),
                             value) != stl::cend(container);
        }

        /**
         * @brief check if the container contains key
         */
        template <template <class, class...> class Container, class T1,
                  class... Args>
        [[nodiscard]] constexpr bool
        contains_key(Container<T1, Args...> const& container,
                     T1 const&                     key) noexcept {
            return container.find(key) != stl::end(container);
        }

        /**
         * @brief check if the container contains the value
         */
        template <template <class, class, class...> class Container, class T1,
                  class T2, class... Args>
        [[nodiscard]] constexpr bool
        contains_value(Container<T1, T2, Args...> const& container,
                       T2 const&                         value) noexcept {
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
        template <typename CharT = char>
        [[nodiscard]] constexpr bool whitespace(CharT const& c) noexcept {
            return c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
                   c == '\f' || c == '\v';
            // TODO: consider using std::isspace
        }

        /**
         * @brief check if str is right trimmed
         * @param str
         * @return true if there's no whitespaces in the right side of input
         */
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        rtrimmed(typename TraitsType::string_view_type const& str) noexcept {
            return !whitespace<typename TraitsType::char_type>(*str.rbegin());
        }

        /**
         * @brief check if str is left trimmed
         * @param str
         * @return true if there's no whitespaces in the left side of input
         */
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        ltrimmed(typename TraitsType::string_view_type const& str) noexcept {
            return !whitespace<typename TraitsType::char_type>(str[0]);
        }

        /**
         * @brief check if str is right and left trimmed
         * @param str
         * @return true if there's no whitespaces in the right and left side of
         * input
         */
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        trimmed(typename TraitsType::string_view_type const& str) noexcept {
            return ltrimmed<TraitsType>(str) && rtrimmed<TraitsType>(str);
        }

        /**
         * @brief check if the specified character is a valid number or not
         * @param character
         * @return true if the specified input is an integer
         */
        template <typename CharT = char>
        requires(stl::is_integral_v<CharT>)
          [[nodiscard]] constexpr bool digit(CharT c) noexcept {
            return c >= '0' && c <= '9';
        }

        /**
         * @brief is all the characters in the specified string digits
         * @param str
         * @return true/false
         */
        template <typename CharT          = char,
                  typename CharTraitsType = stl::char_traits<CharT>>
        [[nodiscard]] constexpr bool digit(
          stl::basic_string_view<CharT, CharTraitsType> const& str) noexcept {
            for (auto c : str)
                if (!digit<CharT>(c))
                    return false;
            return !str.empty();
        }

        /**
         * @brief is a number or a dot
         * @param c
         * @return
         */
        template <typename CharT = char>
        requires(stl::is_integral_v<CharT>)
          [[nodiscard]] constexpr bool number(CharT c) noexcept {
            return digit<CharT>(c) || c == '.';
        }

        /**
         * @brief check if the specified string is a number (including floating
         * points)
         * @param str
         * @return true if the specified string is a number
         */
        template <typename CharT = char>
        requires(stl::is_integral_v<CharT>) [[nodiscard]] constexpr bool number(
          stl::basic_string_view<CharT> const& str) noexcept {
            bool is_first = true;
            for (auto const& c : str) {
                if (!digit<CharT>(c)) {
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
        template <typename CharT = char>
        [[nodiscard]] constexpr bool lowercase(CharT const& c) noexcept {
            return c >= 'a' && c <= 'z';
        }

        /**
         * @brief checks if a string is completely lowercase or not
         * @param str
         * @return
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        lowercase(stl::basic_string_view<CharT> const& str) noexcept {
            for (auto const& c : str)
                if (!lowercase<CharT>(c))
                    return false;
            return true;
        }

        /**
         * @brief checks if a character is uppercase or not
         * @param c
         * @return
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool uppercase(CharT const& c) noexcept {
            return c >= 'A' && c <= 'Z';
        }

        /**
         * @brief checks if a string is uppercase or not
         * @param str
         * @return
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        uppercase(stl::basic_string_view<CharT> const& str) noexcept {
            for (auto const& c : str)
                if (!uppercase<CharT>(c))
                    return false;
            return true;
        }

        /**
         * @brief the same as digit function
         * @param str
         * @return true if the specified string is an integer
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool integer(CharT const& str) noexcept {
            return digit<CharT>(str);
        }

        /**
         * @brief the same as digit function
         * @param str
         * @return true if the specified string is an integer
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        integer(stl::basic_string_view<CharT> const& str) noexcept {
            return digit<CharT>(str);
        }

        /**
         * Check if the specified string is an integer and can be hold inside
         * uint8_t; which means it's between 0 and 255
         * @param str
         * @return bool
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        uint8(stl::basic_string_view<CharT> const& str) noexcept {
            return !str.empty() && str.size() <= 3 && digit(str) &&
                   to_uint(str) <= 255;
        }

        /**
         * Check if the char is a hexadecimal character
         * @param char
         * @return bool
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool hex(CharT const& t) noexcept {
            return (t >= '0' && t <= '9') || (t >= 'a' && t <= 'f') ||
                   (t >= 'A' && t <= 'F');
        }

        /**
         * check if all of the characters in the string is a hexadecimal
         * character
         * @param str
         * @return bool
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        hex(stl::basic_string_view<CharT> const& str) noexcept {
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
        template <Traits TraitsType>
        [[nodiscard]] bool
        email(typename TraitsType::string_view_type const& str) noexcept {
            // TODO: Do not use regular expression, it's slow; use CTRE
            static const stl::regex pattern{
              "^[_A-Za-z0-9-\\+]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+(\\.[A-Za-"
              "z0-9]+)*(\\.[A-Za-z]{2,})$"};
            return stl::regex_match(stl::string(str), pattern);
        }

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        FQDN(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        url(stl::basic_string_view<CharT> const& str) noexcept;

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
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        ipv4(typename TraitsType::string_view_type str) noexcept {
            using traits_type    = TraitsType;
            stl::size_t next_dot = 0;
            for (uint8_t octet_index = 0; octet_index != 4; octet_index++) {
                next_dot       = str.find('.');
                auto octet_str = str.substr(0, next_dot);
                if (octet_str.size() > 3 || !is::digit(octet_str) ||
                    to_uint<traits_type>(octet_str) > 255)
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
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        subnet(typename TraitsType::string_view_type str) noexcept {
            stl::size_t next_dot = 0;
            for (uint8_t octet_index = 0; octet_index != 4; octet_index++) {
                next_dot       = str.find('.');
                auto octet_str = str.substr(0, next_dot);
                if (octet_str.size() > 3 || !is::digit(octet_str)) {
                    return false;
                }
                if (auto octet_int = to_uint<TraitsType>(octet_str);
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
        subnet(stl::array<uint8_t, 4> const& octets) noexcept {
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
        template <Traits TraitsType, stl::size_t N>
        [[nodiscard]] constexpr bool
        ipv4_prefix(typename TraitsType::string_view_type const& str,
                    charset_t<typename TraitsType::char_type, N> const&
                      divider_chars) noexcept {

            using traits_type = TraitsType;

            if (auto found = stl::find_if(stl::rbegin(str), stl::rend(str),
                                          [&](const auto& c) {
                                              return divider_chars.contains(c);
                                          });
                found != stl::rend(str)) {
                auto index = stl::distance(stl::begin(str), found.base()) - 1;
                if (!is::ipv4<traits_type>(str.substr(0, index)))
                    return false;
                if (auto prefix = str.substr(index + 1); is::digit(prefix)) {
                    auto _prefix = to_uint<traits_type>(prefix);
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
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        ipv4_prefix(typename TraitsType::string_view_type const& str) noexcept {
            using char_type = typename TraitsType::char_type;
            return ipv4_prefix<TraitsType>(str,
                                           charset_t<char_type, 2>{':', '/'});
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
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        ipv6(typename TraitsType::string_view_type address) noexcept {
            using traits_type = TraitsType;
            using str_view_t  = typename traits_type::string_view_type;

            bool        encountered_double_colons = false;
            stl::size_t index                     = 0;

            if (starts_with<traits_type>(address, '[')) {
                if (ends_with<traits_type>(address, ']')) {
                    address.remove_suffix(1);
                    address.remove_prefix(1);
                } else {
                    return false;
                }
            }

            while (index < 8u && !address.empty()) {
                auto next_colon = address.find(':');
                auto octet      = address.substr(0, next_colon);
                if (octet.empty()) {
                    // ip cannon have two double colon semantics (the first one
                    // and the last one is ok)
                    if (!address.empty() && encountered_double_colons)
                        return false;

                    if (index == 0) {
                        if (!starts_with<traits_type>(address, "::")) {
                            return false;
                        }
                        address.remove_prefix(1);
                    }
                    encountered_double_colons = true;
                } else if (octet.size() > 4) {
                    if (is::ipv4<traits_type>(octet)) {
                        // ipv4 inside ipv6 address should be the last octet
                        return octet.size() == address.size() &&
                               ((!encountered_double_colons && index == 8u) ||
                                encountered_double_colons);
                    } else
                        return false;
                } else if (!is::hex(octet)) {
                    return false;
                }
                if (next_colon != str_view_t::npos)
                    address.remove_prefix(next_colon + 1);
                else
                    address.remove_prefix(octet.size());
                index++;
            }

            return address.empty() &&
                   ((!encountered_double_colons && index == 8u) ||
                    encountered_double_colons);
        }

        template <Traits TraitsType, stl::size_t N = 1>
        [[nodiscard]] constexpr bool ipv6_prefix(
          typename TraitsType::string_view_type const&        str,
          charset_t<typename TraitsType::char_type, N> const& divider_chars =
            charset_t<typename TraitsType::char_type, 1>('/')) noexcept {
            using traits_type = TraitsType;
            using char_type   = typename TraitsType::char_type;

            if (auto found = stl::find_if(stl::rbegin(str), stl::rend(str),
                                          [&](const auto& c) {
                                              return divider_chars.contains(c);
                                          });
                found != stl::rend(str)) {
                auto index = stl::distance(stl::begin(str), found.base()) - 1;
                if (auto prefix = str.substr(index + 1); is::digit(prefix)) {
                    int _prefix = to_uint<traits_type>(prefix);
                    if (!(_prefix >= 0 && _prefix <= 128))
                        return false;
                } else {
                    return false;
                }
                if (is::ipv6<traits_type>(str.substr(0, index)))
                    return true;
            }
            return false;
        }

        /**
         * @brief check if the specified string is an ipv4 or ipv6
         * @param str
         * @return true if str is ipv4 or ipv6
         * TODO: start supporting IPvF (IP version Future)
         */
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        ip(typename TraitsType::string_view_type const& str) noexcept {
            return is::ipv4<TraitsType>(str) || ipv6<TraitsType>(str);
        }

        /**
         * @brief checks if an string is a valid host based on RFC3986
         * (https://tools.ietf.org/html/rfc3986)
         * @param str
         * @return
         */
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool
        host(typename TraitsType::string_view_type const& str) noexcept {
            using traits_type = TraitsType;
            using char_type   = typename traits_type::char_type;
            using str_view_t  = typename traits_type::string_view_type;

            /**
             * This is the character set corresponds to the "unreserved" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto UNRESERVED =
              charset(ALPHA<char_type>, DIGIT<char_type>,
                      charset_t<char_type, 4>{'-', '.', '_', '~'});

            /**
             * This is the character set corresponds to the "sub-delims" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr charset_t<char_type, 11> SUB_DELIMS{
              '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='};

            /**
             * This is the character set corresponds to the last part of
             * the "IPvFuture" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto IPV_FUTURE_LAST_PART = charset<char_type>(
              UNRESERVED, SUB_DELIMS, charset_t<char_type, 1>{':'});

            /**
             * This is the character set corresponds to the "reg-name" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
             * leaving out "pct-encoded".
             */
            constexpr auto REG_NAME_NOT_PCT_ENCODED =
              charset<char_type>(UNRESERVED, SUB_DELIMS);

            if (str.empty())
                return false;
            if (starts_with<traits_type>(str, '[') &&
                ends_with<traits_type>(str, ']')) {
                if (str[1] == 'v') { // future ip
                    if (auto dot_delim = str.find('.');
                        dot_delim != str_view_t::npos) {
                        auto ipvf_version = str.substr(2, dot_delim);
                        if (!HEXDIG<char_type>.contains(ipvf_version)) {
                            // ERROR: basic_uri is not valid
                            return false;
                        }

                        auto ipvf = str.substr(dot_delim + 1, str.size() - 2);
                        return IPV_FUTURE_LAST_PART.contains(ipvf);
                    }

                } else { // ipv6
                    return is::ipv6<std_traits>(str.substr(1, str.size() - 2));
                }
            } else if (is::digit(str[0]) &&
                       is::ipv4<traits_type>(str)) { // ipv4
                return true;
            } else {
                constexpr auto ccc = charset<char_type>(
                  REG_NAME_NOT_PCT_ENCODED, charset_t<char_type, 1>({'%'}));
                return ccc.contains(str);
            }

            return false;
        }

        /*
         * Check if the specified string is a query (in URI) or not
         * @return bool true if it's a query
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        query(stl::basic_string_view<CharT> str) noexcept {
            /**
             * This is the character set corresponds to the "unreserved" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr auto UNRESERVED =
              charset<CharT>(ALPHA<CharT>, DIGIT<CharT>,
                             charset_t<CharT, 4>{'-', '.', '_', '~'});

            /**
             * This is the character set corresponds to the "sub-delims" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
             */
            constexpr charset_t<CharT, 11> SUB_DELIMS{
              '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='};
            /**
             * This is the character set corresponds to the "pchar" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
             * leaving out "pct-encoded".
             */
            constexpr auto PCHAR_NOT_PCT_ENCODED = charset<CharT>(
              UNRESERVED, SUB_DELIMS, charset_t<CharT, 2>{':', '@'});

            /**
             * This is the character set corresponds to the "query" syntax
             * and the "fragment" syntax
             * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
             * leaving out "pct-encoded".
             */
            constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED = charset<CharT>(
              PCHAR_NOT_PCT_ENCODED, charset_t<CharT, 2>{'/', '?'});

            return QUERY_OR_FRAGMENT_NOT_PCT_ENCODED.contains(str);
        }

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        ip_range(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        ipv4_range(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        ipv6_range(stl::basic_string_view<CharT> const& str) noexcept;
        // bool isImage(something) noexcept;

        /**
         * Check if the specified string is a hexadecimal color
         * @param str
         * @return
         */
        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        hex_color(stl::basic_string_view<CharT> const& str) noexcept {
            if (!starts_with(str, '#'))
                return false;
            switch (str.size()) {
                case 3 + 1:
                case 6 + 1:
                case 8 + 1: return is::hex(str.substr(1));
                default: return false;
            }
        }

        /**
         * Check if the specified string is an RGB color
         * @param str
         * @return
         */
        template <typename CharT = char>
        [[nodiscard]] bool
        rgb_color(stl::basic_string_view<CharT> sstr) noexcept {
            // TODO: there are better ways to do it, check performance

            constexpr stl::initializer_list<CharT const*> numbers =
              "0123456789";

            trim(sstr);
            if (!starts_with<CharT>(sstr, "rgb(") ||
                !starts_with<CharT>(sstr, "RGB("))
                return false;
            sstr.remove_prefix(4);
            sstr.remove_suffix(1);
            rtrim(sstr);
            auto it = sstr.find_first_not_of(numbers);
            if (!is::uint8(sstr.substr(0, it)))
                return false;
            sstr.remove_suffix(it);
            ltrim(sstr);
            if (starts_with(sstr, ','))
                return false;
            sstr.remove_prefix(1);
            ltrim(sstr);
            it = sstr.find_first_not_of(numbers);
            if (!is::uint8(sstr.substr(0, it)))
                return false;
            sstr.remove_prefix(it);
            ltrim(sstr);
            if (!starts_with(sstr, ','))
                return false;
            sstr.remove_prefix(1);
            ltrim(sstr);
            it = sstr.find_first_not_of(numbers);
            if (!is::uint8(sstr.substr(0, it)))
                return false;
            sstr.remove_prefix(it);
            ltrim(sstr);
            return sstr.empty();
        }

        /**
         * Check if the specified string is a RGBA HTML color
         * @param str
         * @return bool
         */
        template <typename CharT = char>
        [[nodiscard]] bool
        rgba_color(stl::basic_string_view<CharT> str) noexcept {
            // TODO: there are better ways to do it, check performance
            constexpr stl::initializer_list<CharT const*> numbers =
              "0123456789";
            return true; // TODO: I'm just gonna make it compilable
        }

        /**
         * Check if the specified string is a valid HSL and HSLA color or not
         * @param str
         * @return bool
         */
        template <typename CharT = char>
        [[nodiscard]] bool
        hsl_color(stl::basic_string_view<CharT> str) noexcept {
            return true; // FIXME: implement this
        }

        /**
         * Check if the specified string is a valid HSLA color or not
         * It's just an alias for hsl_color. Read more about the reason here:
         * https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#hsl()
         *
         * @param str
         * @return bool
         */
        template <typename CharT = char>
        [[nodiscard]] bool
        hsla_color(stl::basic_string_view<CharT> str) noexcept {
            return hsl_color(str);
        }

        /**
         * Check if the specified string is a valid HTML color
         * @param str
         * @return bool
         */
        template <typename CharT = char>
        [[nodiscard]] bool name_color(stl::basic_string<CharT> str) noexcept {
            // converting to lower case
            stl::transform(str.begin(), str.end(), str.begin(),
                           [](unsigned char c) {
                               return stl::tolower(c);
                           });

            static constexpr stl::initializer_list<const CharT*> names = {
              "algae green",
              "aliceblue",
              "alien green",
              "antiquewhite",
              "aquamarine",
              "army brown",
              "ash gray",
              "avocado green",
              "aztech purple",
              "azure",
              "baby blue",
              "bashful pink",
              "basket ball orange",
              "battleship gray",
              "bean red",
              "bee yellow",
              "beer",
              "beetle green",
              "beige",
              "black",
              "black cat",
              "black cow",
              "black eel",
              "blanchedalmond",
              "blonde",
              "blood red",
              "blossom pink",
              "blue angel",
              "blue diamond",
              "blue dress",
              "blue eyes",
              "blue gray",
              "blue green",
              "blue hosta",
              "blue ivy",
              "blue jay",
              "blue koi",
              "blue lagoon",
              "blue lotus",
              "blue orchid",
              "blue ribbon",
              "blue whale",
              "blue zircon",
              "blueberry blue",
              "blush pink",
              "blush red",
              "brass",
              "bright gold",
              "bright neon pink",
              "bronze",
              "brown bear",
              "brown sugar",
              "bullet shell",
              "burgundy",
              "burlywood",
              "burnt pink",
              "butterfly blue",
              "cadillac pink",
              "camel brown",
              "camouflage green",
              "cantaloupe",
              "caramel",
              "carbon gray",
              "carnation pink",
              "celeste",
              "champagne",
              "charcoal",
              "chartreuse",
              "cherry red",
              "chestnut",
              "chestnut red",
              "chilli pepper",
              "chocolate",
              "cinnamon",
              "cloudy gray",
              "clover green",
              "cobalt blue",
              "coffee",
              "columbia blue",
              "construction cone orange",
              "cookie brown",
              "copper",
              "coral",
              "coral blue",
              "corn yellow",
              "cornflower blue",
              "cornsilk",
              "cotton candy",
              "cranberry",
              "cream",
              "crimson",
              "crocus purple",
              "crystal blue",
              "cyan opaque",
              "cyan or aqua",
              "dark carnation pink",
              "dark forest green",
              "dark goldenrod",
              "dark orange",
              "dark orchid",
              "dark salmon",
              "dark sea green",
              "dark slate blue",
              "dark slate grey",
              "dark turquoise",
              "dark violet",
              "day sky blue",
              "deep peach",
              "deep pink",
              "deep sky blue",
              "denim blue",
              "denim dark blue",
              "desert sand",
              "dimorphotheca magenta",
              "dodger blue",
              "dollar bill green",
              "dragon green",
              "dull purple",
              "earth blue",
              "eggplant",
              "electric blue",
              "emerald green",
              "fall leaf brown",
              "fern green",
              "ferrari red",
              "fire engine red",
              "firebrick",
              "flamingo pink",
              "forest green",
              "frog green",
              "ginger brown",
              "glacial blue ice",
              "golden brown",
              "goldenrod",
              "granite",
              "grape",
              "grapefruit",
              "gray",
              "gray cloud",
              "gray dolphin",
              "gray goose",
              "gray wolf",
              "grayish turquoise",
              "green",
              "green apple",
              "green onion",
              "green peas",
              "green snake",
              "green thumb",
              "green yellow",
              "greenish blue",
              "gunmetal",
              "halloween orange",
              "harvest gold",
              "hazel green",
              "heliotrope purple",
              "hot pink",
              "hummingbird green",
              "iceberg",
              "iguana green",
              "indigo",
              "iridium",
              "jade green",
              "jasmine purple",
              "jeans blue",
              "jellyfish",
              "jet gray",
              "jungle green",
              "kelly green",
              "khaki",
              "khaki rose",
              "lapis blue",
              "lava red",
              "lavender blue",
              "lavender pinocchio",
              "lawn green",
              "lemon chiffon",
              "light aquamarine",
              "light blue",
              "light coral",
              "light cyan",
              "light jade",
              "light pink",
              "light salmon",
              "light sea green",
              "light sky blue",
              "light slate",
              "light slate blue",
              "light slate gray",
              "light steel blue",
              "lilac",
              "lime green",
              "lipstick pink",
              "love red",
              "lovely purple",
              "macaroni and cheese",
              "macaw blue green",
              "magenta",
              "mahogany",
              "mango orange",
              "marble blue",
              "maroon",
              "mauve",
              "medium aquamarine",
              "medium forest green",
              "medium orchid",
              "medium purple",
              "medium sea green",
              "medium spring green",
              "medium turquoise",
              "medium violet red",
              "metallic silver",
              "midnight",
              "midnight blue",
              "milk white",
              "mint green",
              "mist blue",
              "misty rose",
              "moccasin",
              "mocha",
              "mustard",
              "navy blue",
              "nebula green",
              "neon pink",
              "night",
              "northern lights blue",
              "oak brown",
              "ocean blue",
              "oil",
              "orange gold",
              "orange salmon",
              "pale blue lily",
              "pale violet red",
              "papaya orange",
              "parchment",
              "pastel blue",
              "peach",
              "pearl",
              "periwinkle",
              "pig pink",
              "pine green",
              "pink",
              "pink bow",
              "pink bubble gum",
              "pink cupcake",
              "pink daisy",
              "pink lemonade",
              "pink rose",
              "pistachio green",
              "platinum",
              "plum",
              "plum pie",
              "plum purple",
              "plum velvet",
              "powder blue",
              "puce",
              "pumpkin orange",
              "purple",
              "purple amethyst",
              "purple daffodil",
              "purple dragon",
              "purple flower",
              "purple haze",
              "purple iris",
              "purple jam",
              "purple mimosa",
              "purple monster",
              "purple sage bush",
              "red",
              "red dirt",
              "red fox",
              "red wine",
              "robin egg blue",
              "rogue pink",
              "rose",
              "rose gold",
              "rosy brown",
              "rosy finch",
              "royal blue",
              "rubber ducky yellow",
              "ruby red",
              "rust",
              "saffron",
              "sage green",
              "salad green",
              "sand",
              "sandstone",
              "sandy brown",
              "sangria",
              "sapphire blue",
              "scarlet",
              "school bus yellow",
              "sea blue",
              "sea green",
              "sea turtle green",
              "seashell",
              "seaweed green",
              "sedona",
              "sepia",
              "shamrock green",
              "shocking orange",
              "sienna",
              "silk blue",
              "sky blue",
              "slate blue",
              "slate gray",
              "slime green",
              "smokey gray",
              "spring green",
              "steel blue",
              "stoplight go green",
              "sun yellow",
              "sunrise orange",
              "tan brown",
              "tangerine",
              "taupe",
              "tea green",
              "teal",
              "thistle",
              "tiffany blue",
              "tiger orange",
              "tron blue",
              "tulip pink",
              "turquoise",
              "tyrian purple",
              "valentine red",
              "vampire gray",
              "vanilla",
              "velvet maroon",
              "venom green",
              "viola purple",
              "violet",
              "violet red",
              "water",
              "watermelon pink",
              "white",
              "windows blue",
              "wisteria purple",
              "wood",
              "yellow",
              "yellow green",
              "zombie green"};

            auto it = stl::lower_bound(stl::cbegin(names), stl::end(names), str,
                                       [](const auto& l, const auto& r) {
                                           return l < r;
                                       });
            return it != stl::end(names) && *it == str;
        }

        /**
         * Check if the specified string is a valid string representation of a
         * color or not.
         * @param str
         * @return bool
         */
        template <typename CharT = char>
        [[nodiscard]] bool
        color(stl::basic_string_view<CharT> const& str) noexcept {
            return hex_color(str) ||
                   name_color(stl::basic_string<CharT>(str)) ||
                   rgb_color(str) || rgba_color(str) || hsl_color(str);
        }

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        mimetype(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        UUID(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        port(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] constexpr bool
        mongoid(stl::basic_string_view<CharT> const& str) noexcept;


        // you may want to change the string to a date of some sort or add both
        template <typename CharT = char>
        [[nodiscard]] bool
        today(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        tomorrow(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        yesterday(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        this_year(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        next_year(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        prev_year(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        this_month(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        next_month(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        prev_month(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        this_week(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        next_week(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        prev_week(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        between(stl::basic_string_view<CharT> const& str,
                stl::basic_string_view<CharT> const& from,
                stl::basic_string_view<CharT> const& after) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        after(stl::basic_string_view<CharT> const& str,
              stl::basic_string_view<CharT> const& pointintime) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        before(stl::basic_string_view<CharT> const& str,
               stl::basic_string_view<CharT> const& pointintime) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        base64(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        escaped(stl::basic_string_view<CharT> const& str) noexcept;

        template <typename CharT = char>
        [[nodiscard]] bool
        username(stl::basic_string_view<CharT> const& str) noexcept;
    } // namespace is
} // namespace webpp

#endif // WEBPP_VALIDATION_H
