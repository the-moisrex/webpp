#ifndef WEBPP_HTTP_COOKIES_H
#define WEBPP_HTTP_COOKIES_H

/*
 * Documentations about cookies:
 *
 *
 *  Standard RFC:
 *    https://tools.ietf.org/html/rfc6265
 *  Set-Cookie:
 *    https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
 *  Cookie:
 *    https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cookie
 *  Date Format:
 *    https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Date
 *
 *
 * -----------------------------------------------------------------------------
 * Other close implementations:
 *
 *  Drogon:
 *    https://github.com/an-tao/drogon/blob/master/lib/inc/drogon/Cookie.h
 *    https://github.com/an-tao/drogon/blob/master/lib/src/Cookie.cc
 *
 *  Pistache:
 *    https://github.com/oktal/pistache/blob/master/include/pistache/cookie.h
 *    https://github.com/oktal/pistache/blob/master/src/common/cookie.cc
 *
 *  Cgicc:
 *    https://github.com/cmakified/cgicc/blob/master/cgicc/HTTPCookie.h
 *    https://github.com/cmakified/cgicc/blob/master/cgicc/HTTPCookie.cpp
 *
 *
 *
 *
 * set-cookie-header = "Set-Cookie:" SP set-cookie-string
 * set-cookie-string = cookie-pair *( ";" SP cookie-av )
 * cookie-pair       = cookie-name "=" cookie-value
 * cookie-name       = token
 * cookie-value      = *cookie-octet / ( DQUOTE *cookie-octet DQUOTE )
 * cookie-octet      = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E
 *                       ; US-ASCII characters excluding CTLs,
 *                       ; whitespace DQUOTE, comma, semicolon,
 *                       ; and backslash
 * token             = <token, defined in [RFC2616], Section 2.2>
 *
 * cookie-av         = expires-av / max-age-av / domain-av /
 *                     path-av / secure-av / httponly-av /
 *                     extension-av
 * expires-av        = "Expires=" sane-cookie-date
 * sane-cookie-date  = <rfc1123-date, defined in [RFC2616], Section 3.3.1>
 * max-age-av        = "Max-Age=" non-zero-digit *DIGIT
 *                       ; In practice, both expires-av and max-age-av
 *                       ; are limited to dates representable by the
 *                       ; user agent.
 * non-zero-digit    = %x31-39
 *                       ; digits 1 through 9
 * domain-av         = "Domain=" domain-value
 * domain-value      = <subdomain>
 *                       ; defined in [RFC1034], Section 3.5, as
 *                       ; enhanced by [RFC1123], Section 2.1
 * path-av           = "Path=" path-value
 * path-value        = <any CHAR except CTLs or ";">
 * secure-av         = "Secure"
 * httponly-av       = "HttpOnly"
 * extension-av      = <any CHAR except CTLs or ";">
 *
 * -----------------------------------------------------------------------------
 * TODO:
 *    [ ] Encryption for the basic_cookie name
 *    [ ] Decryption
 *    [ ] Pre Defaults in the cookie jar
 *    [ ] Implement "Cookie2:" and "Set-Cookie2:" obsolete headers
 *    [X] Add *_if methods in cookies
 *    [X] Add customization of cookies in the cookie jar. e.g:
 *         encrypted("cookiename", true)
 *    [X] Move the definitions of the basic_cookie jar into cookies.cpp file
 *    [X] Consider renaming "cookies" to "cookie_jar"
 *    [ ] Add doxygen documentations/comments to the declarations
 *    [ ] Does user's browser support cookies
 *    [ ] Does user's browser support cookies but now it's disabled
 */

#include "../../std/optional.hpp"
#include "../../std/unordered_map.hpp"
#include "../../std/unordered_set.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/to_case.hpp"
#include "../../traits/std_traits.hpp"
#include "./cookies_concepts.hpp"

#include <chrono>
#include <string_view>
#include <type_traits>

namespace webpp {


    /**
     * Even though these algorithms are capable of handling other char types than "char" itself, they
     * don't need to. Other string types cannot be used in a HTTP header since HTTP header is using ASCII
     */
    namespace details {

        template <istl::CharType CharT>
        constexpr static auto VALID_COOKIE_NAME =
          charset(ALPHA_DIGIT<CharT>, charset<CharT, 16>{'!', '#', '$', '%', '&', '\'', '*', '+',
                                                                  '-', '.', '^', '_', '`', '|', '~'});

        template <istl::CharType CharT>
        constexpr static auto VALID_COOKIE_VALUE = charset(
          ALPHA_DIGIT<CharT>,
          charset<CharT, 28>{'!', '#', '$', '%', '&', '\'', '(', ')', '*', '+', '-', '.', '/', ':',
                               '<', '=', '>', '?', '@', '[',  ']', '^', '_', '`', '{', '|', '}', '~'});


        void parse_SE_name(istl::StringView auto& str, auto& _name, bool& _valid) noexcept {
            using name_t           = stl::remove_cvref_t<decltype(_name)>;
            using string_view_type = stl::remove_cvref_t<decltype(str)>;
            using char_type        = typename name_t::value_type;
            ltrim(str);
            if (auto equal_pos = str.find_first_not_of(VALID_COOKIE_NAME<char_type>.data());
                equal_pos != string_view_type::npos) {
                // setting the name we found it
                _name = name_t{str.substr(0, equal_pos)};

                // prepare the string for the next value
                str.remove_prefix(equal_pos);
                _valid = true;
            } else {
                // there's no value in the string
                _valid = false;
            }
        }

        void parse_SE_value(istl::StringView auto& str, auto& _name, auto& _value,
                                   bool& _valid) noexcept {
            using name_t           = stl::remove_cvref_t<decltype(_name)>;
            using value_t          = stl::remove_cvref_t<decltype(_value)>;
            using string_view_type = stl::remove_cvref_t<decltype(str)>;
            using char_type        = typename name_t::value_type;

            parse_SE_name(str);
            if (!_valid)
                return; // do not continue if there's no name
            ltrim(str);
            if (starts_with(str, '='))
                str.remove_prefix(1);
            ltrim(str);
            if (starts_with(str, '"')) {
                if (auto d_quote_end = str.find_first_not_of(VALID_COOKIE_VALUE<char_type>.data(), 1);
                    d_quote_end != string_view_type::npos) {
                    if (str[d_quote_end] == '"') {
                        _value = str.substr(1, d_quote_end - 1);
                        str.remove_prefix(d_quote_end);
                    } else {
                        // You can't use non double quote chars when you used
                        // one already. You can't even use backslash to escape,
                        // so there's no worry here
                        _valid = false;
                        return;
                    }
                } else {
                    // It won't be a valid string if there's a double quote
                    // without another one finishing it off.
                    _valid = false;
                    return;
                }
            } else {
                // there's no double quote in the value
                if (auto semicolon_pos = str.find_first_not_of(VALID_COOKIE_VALUE<char_type>.data());
                    semicolon_pos != string_view_type::npos) {
                    _value = str.substr(0, semicolon_pos);
                    str.remove_prefix(semicolon_pos);
                } else {
                    _value = str;
                    str.remove_prefix(str.size() - 1);
                }
            }

            // Attention: here we are not checking the rest of the string.
            // There might be invalid characters after this. We have to
            // check the whole string for validation. But if it's determined
            // that it's invalid so far, it really is invalid.
        }
    } // namespace details




    //    template <typename Traits, header_type HeaderType = header_type::response>
    //    class basic_cookie : public stl::conditional_t<HeaderType == header_type::response,
    //                                                   response_cookie<Traits>, request_cookie<Traits>> {
    //        using super = stl::conditional_t<HeaderType == header_type::response, response_cookie<Traits>,
    //                                         request_cookie<Traits>>;
    //
    //      public:
    ////        static constexpr auto header_direction = HeaderType;
    //
    //        using super::super;
    //    };
    //
    // hash function of std::unordered_set<webpp::basic_cookie>
    //    template <Cookie CookieType>
    //    struct cookie_hash {
    //
    //        template <class T>
    //        void hash_combine(stl::size_t& seed, const T& v) {
    //            stl::hash<T> hasher;
    //            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
    //        }
    //
    //
    //        using result_type = stl::size_t;
    //
    //        result_type operator()(CookieType const& c) const noexcept {
    //            // change the "same_as" method too if you ever touch this function
    //            cookie_hash::result_type seed = 0;
    //            hash_combine(seed, c.name());
    //            if constexpr (CookieType::header_direction ==
    //                          header_type::response) {
    //                hash_combine(seed, c.domain());
    //                hash_combine(seed, c.path());
    //            }
    //            //    hash_combine(seed, c._value);
    //            //    hash_combine(seed, c._prefix);
    //            //    hash_combine(seed, c._secure);
    //            //    if (c._expires)
    //            //        hash_combine(seed,
    //            //        c._expires->time_since_epoch().count());
    //            //    hash_combine(seed, c._max_age);
    //            //    hash_combine(seed, c._same_site);
    //            //    hash_combine(seed, c._comment);
    //            //    hash_combine(seed, c._host_only);
    //            //    hash_combine(seed, c._encrypted);
    //            return seed;
    //        }
    //    };
    //
    //    template <Cookie CookieType>
    //    struct cookie_equals {
    //        using cookie_type = CookieType;
    //
    //        bool operator()(const cookie_type& lhs,
    //                        const cookie_type& rhs) const noexcept {
    //            if constexpr (cookie_type::header_direction ==
    //                          header_type::response) {
    //                return lhs.name() == rhs.name() &&
    //                       lhs.domain() == rhs.domain() && lhs.path() ==
    //                       rhs.path();
    //            } else {
    //                return lhs.name() == rhs.name();
    //            }
    //        }
    //    };



} // namespace webpp

#endif // WEBPP_HTTP_COOKIES_H
