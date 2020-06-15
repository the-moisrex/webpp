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

#include "../../std/unordered_map.h"
#include "../../std/unordered_set.h"
#include "../../traits/std_traits.h"
#include "../../utils/charset.h"
#include "../../utils/strings.h"
#include "../common.h"
#include "./cookies_concepts.h"

#include <chrono>
#include <string_view>
#include <type_traits>

namespace webpp {

    template <Traits TraitsType, bool Mutable>
    struct basic_cookie_common {

        using traits_type = TraitsType;
        using char_type   = typename TraitsType::char_type;

        /**
         * Getting the appropriate string type to use.
         * If the specified string type cannot be changed, the string_view will
         * be used, otherwise, string itself.
         */
        using string_type      = typename TraitsType::string_type;
        using string_view_type = typename TraitsType::string_view_type;
        using storing_string_type =
          stl::conditional_t<Mutable, string_type, string_view_type>;

        enum class same_site_value { NONE, LAX, STRICT };

        using name_t  = storing_string_type;
        using value_t = storing_string_type;

      protected:
        name_t  _name;
        value_t _value;
        bool    _valid = false;

        constexpr static auto VALID_COOKIE_NAME = charset<char_type>(
          ALPHA_DIGIT<char_type>,
          charset_t<char_type, 16>{'!', '#', '$', '%', '&', '\'', '*', '+', '-',
                                   '.', '^', '_', '`', '|', '~'});

        constexpr static auto VALID_COOKIE_VALUE = charset<char_type>(
          ALPHA_DIGIT<char_type>,
          charset_t<char_type, 28>{'!', '#', '$', '%', '&', '\'', '(',
                                   ')', '*', '+', '-', '.', '/',  ':',
                                   '<', '=', '>', '?', '@', '[',  ']',
                                   '^', '_', '`', '{', '|', '}',  '~'});

        void parse_SE_name(string_view_type& str) noexcept {
            ltrim<traits_type>(str);
            if (auto equal_pos =
                  str.find_first_not_of(VALID_COOKIE_NAME.data());
                equal_pos != string_view_type::npos) {
                // setting the name we found it
                _name = str.substr(0, equal_pos);

                // prepare the string for the next value
                str.remove_prefix(equal_pos);
                _valid = true;
            } else {
                // there's no value in the string
                _valid = false;
            }
        }

        void parse_SE_value(string_view_type& str) noexcept {
            parse_SE_name(str);
            if (!_valid)
                return; // do not continue if there's no name
            ltrim<traits_type>(str);
            if (starts_with<traits_type>(str, '='))
                str.remove_prefix(1);
            ltrim<traits_type>(str);
            if (starts_with<traits_type>(str, '"')) {
                if (auto d_quote_end =
                      str.find_first_not_of(VALID_COOKIE_VALUE.data(), 1);
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
                if (auto semicolon_pos =
                      str.find_first_not_of(VALID_COOKIE_VALUE.data());
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

      public:
        /**
         * empty basic_cookie
         */
        basic_cookie_common()                                 = default;
        basic_cookie_common(const basic_cookie_common& c)     = default;
        basic_cookie_common(basic_cookie_common&& c) noexcept = default;

        basic_cookie_common(name_t __name, value_t __value) noexcept
          : _name(trim_copy<traits_type>(__name)),
            _value(trim_copy<traits_type>(__value)) {
        }

        basic_cookie_common& operator=(const basic_cookie_common& c) = default;
        basic_cookie_common&
        operator=(basic_cookie_common&& c) noexcept = default;

        explicit operator bool() {
            return is_valid();
        }

        bool is_valid() const noexcept {
            // todo
            // The _valid may not catch all the validness conditions, so we have
            // to do other validation checks ourselves.
            if (!_valid)
                return false;
        }

        auto const& name() const noexcept {
            return _name;
        }
        auto const& value() const noexcept {
            return _value;
        }

        auto& name(name_t __name) noexcept {
            trim<traits_type>(__name);
            this->_name = stl::move(__name);
            return *this;
        }

        auto& value(value_t __value) noexcept {
            trim<traits_type>(__value);
            _value = stl::move(__value);
            return *this;
        }
    };

    template <Traits TraitsType>
    struct request_cookie : public basic_cookie_common<TraitsType, false> {
      private:
        using super = basic_cookie_common<TraitsType, false>;

      public:
        static constexpr auto header_direction = header_type::request;
        static constexpr bool is_mutable       = false;

        constexpr request_cookie() noexcept {};

        /**
         * Source here is in "name = value" syntax and only one single cookie
         * @param source
         */
        explicit request_cookie(
          typename super::string_view_type const& source) noexcept {
            super::parse_SE_value(source); // parsing name and value
        }
    };

    template <Traits TraitsType>
    struct response_cookie : public basic_cookie_common<TraitsType, true> {
      private:
        using super  = basic_cookie_common<TraitsType, true>;
        using self_t = response_cookie<TraitsType>;

        void parse_SE_options(typename super::string_view_type& str) noexcept {
            super::parse_SE_value(str);
            // todo
        }

      public:
        using traits_type = TraitsType;
        using date_t      = stl::chrono::time_point<stl::chrono::system_clock>;
        using domain_t    = typename super::storing_string_type;
        using path_t      = typename super::storing_string_type;
        using expires_t   = date_t;
        using optional_expires_t = stl::optional<date_t>;
        using max_age_t          = unsigned long;
        using same_site_t        = typename super::same_site_value;
        using secure_t           = bool;
        using host_only_t        = bool;
        using prefix_t           = bool;
        using encrypted_t        = bool;
        using comment_t          = typename super::storing_string_type;

        using attrs_t =
          stl::unordered_map<TraitsType, typename super::storing_string_type,
                             typename super::storing_string_type>;


      private:
        domain_t           _domain;
        path_t             _path;
        optional_expires_t _expires;
        comment_t          _comment;
        max_age_t          _max_age   = 0;
        same_site_t        _same_site = super::same_site_value::NONE;
        secure_t           _secure    = false;
        host_only_t        _host_only = false;
        encrypted_t        _encrypted = false;
        prefix_t           _prefix    = false;

        // todo: encapsulate this
        attrs_t attrs;

      public:
        static constexpr auto header_direction = header_type::response;
        static constexpr bool is_mutable       = true;

        constexpr response_cookie() noexcept {};

        explicit response_cookie(typename super::name_t  name,
                                 typename super::value_t value) noexcept
          : super{stl::move(name), stl::move(value)} {
        }

        /**
         * Parse response cookie (A single Set-Cookie header value)
         * There are not many reasons to use this constructor
         * @param source
         */
        explicit response_cookie(
          typename super::string_view_type source) noexcept {
            parse_SE_options(source); // parse name, value, and options
        }

        [[nodiscard]] auto const& comment() const noexcept {
            return _comment;
        }
        [[nodiscard]] auto const& domain() const noexcept {
            return _domain;
        }
        [[nodiscard]] auto const& max_age() const noexcept {
            return _max_age;
        }
        [[nodiscard]] auto const& secure() const noexcept {
            return _secure;
        }
        [[nodiscard]] auto const& host_only() const noexcept {
            return _host_only;
        }
        [[nodiscard]] auto const& prefix() const noexcept {
            return _prefix;
        }
        [[nodiscard]] auto const& expires() const noexcept {
            return *_expires;
        }
        [[nodiscard]] auto const& same_site() const noexcept {
            return _same_site;
        }
        [[nodiscard]] auto const& path() const noexcept {
            return _path;
        }
        [[nodiscard]] auto const& encrypted() const noexcept {
            return _encrypted;
        }
        [[nodiscard]] auto const& name() const noexcept {
            return super::name();
        }
        [[nodiscard]] auto const& value() const noexcept {
            return super::value();
        }
        self_t& name(typename super::name_t __name) noexcept {
            return static_cast<self_t&>(super::name(__name));
        }
        self_t& value(typename super::value_t __value) noexcept {
            return static_cast<self_t&>(super::value(__value));
        }

        auto& comment(comment_t __comment) noexcept {
            _comment = stl::move(__comment);
            return *this;
        }

        auto& domain(domain_t __domain) noexcept {
            _domain = stl::move(__domain);
            return *this;
        }
        auto& path(path_t __path) noexcept {
            _path = stl::move(__path);
            return *this;
        }
        auto& max_age(max_age_t __max_age) noexcept {
            _max_age = __max_age;
            return *this;
        }
        auto& prefix(prefix_t __prefix) noexcept {
            _prefix = __prefix;
            return *this;
        }
        auto& same_site(same_site_t __same_site) noexcept {
            _same_site = __same_site;
            return *this;
        }
        auto& secure(secure_t __secure) noexcept {
            _secure = __secure;
            return *this;
        }
        auto& host_only(host_only_t __host_only) noexcept {
            _host_only = __host_only;
            return *this;
        }
        auto& expires(date_t __expires) noexcept {
            _expires = __expires;
            return *this;
        }

        auto& remove(bool __remove = true) noexcept {
            if (__remove) {
                // set the expire date 10 year before now:
                expires(
                  stl::chrono::system_clock::now() -
                  stl::chrono::duration<int, stl::ratio<60 * 60 * 24 * 365>>(
                    10));
            } else if (is_removed()) {
                // set the expire date 1 year from now:
                expires(
                  stl::chrono::system_clock::now() +
                  stl::chrono::duration<int, stl::ratio<60 * 60 * 24 * 365>>(
                    1));
            }
            // remove max-age if it exists because we're going with expires
            max_age(0);
            return *this;
        }

        [[nodiscard]] bool is_removed() const noexcept {
            return *_expires < stl::chrono::system_clock::now();
        }

        /**
         * @brief sets expiration time relative to now.
         */
        template <typename D, typename T>
        inline auto&
        expires_in(stl::chrono::duration<D, T> const& __dur) noexcept {
            _expires = stl::chrono::system_clock::now() + __dur;
            return *this;
        }

        /**
         * @brief encrypt the value
         * @param __encrypted
         * @return
         */
        auto& encrypted(encrypted_t __encrypted) noexcept {
            _encrypted = __encrypted;
            return *this;
        }

        /**
         * @brief decrypt-able encryption
         */
        typename super::value_t encrypted_value() const noexcept {
            // todo implement this
        }

        stl::basic_ostream<typename super::char_type>& operator<<(
          stl::basic_ostream<typename super::char_type>& out) const noexcept {
            using namespace stl::chrono;
            if (_prefix) {
                if (_secure)
                    out << "__Secure-";
                else if (_host_only)
                    out << "__Host-";
            }
            if (!super::_name.empty()) {
                // FIXME: encode/... name and value here. Programmers are dumb!
                out << super::_name << "=" << super::_value;

                if (!_comment.empty())
                    out << "; Comment=" << _comment;

                if (!_domain.empty())
                    out << "; Domain=" << _domain;

                if (!_path.empty())
                    out << "; Path=" << _path;

                if (_expires) {
                    stl::time_t expires_c  = system_clock::to_time_t(*_expires);
                    stl::tm     expires_tm = *stl::localtime(&expires_c);
                    char        buff[30];
                    // FIXME: check time zone and see if it's ok
                    //            setlocale(LC_ALL, "en_US.UTF-8");
                    if (strftime(buff, sizeof buff, "%a, %d %b %Y %H:%M:%S GMT",
                                 &expires_tm))
                        out << "; Expires=" << buff;
                }

                if (_secure)
                    out << "; Secure";

                if (_host_only)
                    out << "; HttpOnly";

                if (_max_age)
                    out << "; Max-Age=" << _max_age;

                if (_same_site != super::same_site_value::NONE)
                    out << "; SameSite="
                        << (_same_site == super::same_site_value::STRICT
                              ? "Strict"
                              : "Lax");

                // TODO: encode value and check the key here:
                if (!attrs.empty())
                    for (auto const& attr : attrs)
                        out << "; " << attr.first << "=" << attr.second;
            }
            return out;
        }

        bool operator==(request_cookie<TraitsType> const& c) const noexcept {
            return super::_name == c._name && super::_value == c._value;
        }

        bool operator==(response_cookie<TraitsType> const& c) const noexcept {
            return super::_name == c._name && super::_value == c._value &&
                   _prefix == c._prefix && _encrypted == c._encrypted &&
                   _secure == c._secure && _host_only == c._host_only &&
                   _same_site == c._same_site && _comment == c._comment &&
                   _expires == c._expires && _path == c._path &&
                   _domain == c._domain && attrs == c.attrs;
        }

        bool operator<(response_cookie<TraitsType> const& c) const noexcept {
            return _expires < c._expires;
        }

        bool operator>(response_cookie<TraitsType> const& c) const noexcept {
            return _expires > c._expires;
        }

        bool operator<=(response_cookie<TraitsType> const& c) const noexcept {
            return _expires <= c._expires;
        }

        bool operator>=(response_cookie<TraitsType> const& c) const noexcept {
            return _expires >= c._expires;
        }

        [[nodiscard]] typename super::string_type render() const noexcept {
            // todo: don't use streams here
            stl::basic_ostringstream<typename super::char_type> os;
                                                                operator<<(os);
            return os.str();
        }

        /**
         * @brief this method will return true if the specified basic_cookie and
         * this basic_cookie have the same  name, domain, and path. so they will
         * replace each other if they put in the same basic_cookie jar
         * @param c
         * @return true if they have the same name, domain, and path
         */
        [[nodiscard]] bool
        same_as(response_cookie<TraitsType> const& c) const noexcept {
            return super::_name == c._name && _path == c._path &&
                   c._domain == _domain;
        }

        friend inline void swap(response_cookie<TraitsType>& first,
                                response_cookie<TraitsType>& second) noexcept {
            using stl::swap;
            swap(first._valid, second._valid);
            swap(first._name, second._name);
            swap(first._value, second._value);
            swap(first._comment, second._comment);
            swap(first._domain, second._domain);
            swap(first._path, second._path);
            swap(first._max_age, second._max_age);
            swap(first._secure, second._secure);
            swap(first._host_only, second._host_only);
            swap(first._expires, second._expires);
            swap(first._encrypted, second._encrypted);
            swap(first._prefix, second._prefix);
            swap(first._same_site, second._same_site);
        }
    };

    template <typename Traits, header_type HeaderType = header_type::response>
    class basic_cookie
      : public stl::conditional_t<HeaderType == header_type::response,
                                  response_cookie<Traits>,
                                  request_cookie<Traits>> {
        using super =
          stl::conditional_t<HeaderType == header_type::response,
                             response_cookie<Traits>, request_cookie<Traits>>;

      public:
        static constexpr auto header_direction = HeaderType;

        using super::super;
        constexpr basic_cookie() noexcept = default;
    };

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
