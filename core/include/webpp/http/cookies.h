#ifndef WEBPP_COOKIES_H
#define WEBPP_COOKIES_H

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

#include "../std/unordered_map.h"
#include "../std/unordered_set.h"
#include "../traits/std_traits.h"
#include "../utils/charset.h"
#include "../utils/strings.h"
#include "./common.h"

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string_view>
#include <type_traits>

namespace webpp {

    template <typename Traits = std_traits, bool Mutable = true,
              header_type HeaderType = header_type::response>
    struct cookie_hash;

    template <typename Traits = std_traits, bool Mutable = true,
              header_type HeaderType = header_type::response>
    class cookie_jar;

    template <typename Traits, bool Mutable>
    struct basic_cookie_common {

        static_assert(
          is_traits_v<Traits>,
          "The specified traits template parameter is not a valid traits.");

        using traits    = Traits;
        using char_type = typename Traits::char_type;

        /**
         * Getting the appropriate string type to use.
         * If the specified string type cannot be changed, the string_view will
         * be used, otherwise, string itself.
         */
        using str_t         = typename Traits::string_type;
        using str_view_t    = typename Traits::string_view_type;
        using storing_str_t = std::conditional_t<Mutable, str_t, str_view_t>;

        enum class same_site_value { NONE, LAX, STRICT };

        using name_t  = storing_str_t;
        using value_t = storing_str_t;

      private:
        mutable name_t  _name;
        mutable value_t _value;
        mutable bool    _valid = false;

        constexpr static auto VALID_COOKIE_NAME = charset<char_type>(
          ALPHA_DIGIT<char_type>,
          charset_t<char_type>{'!', '#', '$', '%', '&', '\'', '*', '+', '-',
                               '.', '^', '_', '`', '|', '~'});

        constexpr static auto VALID_COOKIE_VALUE = charset<char_type>(
          ALPHA_DIGIT<char_type>,
          charset_t<char_type>{'!', '#', '$', '%', '&', '\'', '(',
                               ')', '*', '+', '-', '.', '/',  ':',
                               '<', '=', '>', '?', '@', '[',  ']',
                               '^', '_', '`', '{', '|', '}',  '~'});

        void parse_SE_name(str_view_t& str) noexcept {
            ltrim(str);
            if (auto equal_pos =
                  str.find_first_not_of(VALID_COOKIE_NAME.data());
                equal_pos != str_view_t::npos) {
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

        void parse_SE_value(str_view_t& str) noexcept {
            parse_SE_name(str);
            if (!_valid)
                return; // do not continue if there's no name
            ltrim(str);
            if (starts_with(str, '='))
                str.remove_prefix(1);
            ltrim(str);
            if (starts_with(str, '"')) {
                if (auto d_quote_end =
                      str.find_first_not_of(VALID_COOKIE_VALUE.data(), 1);
                    d_quote_end != str_view_t::npos) {
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
                    semicolon_pos != str_view_t::npos) {
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

        void parse_SE_options(str_view_t& str) noexcept {
        }

        void parse_set_cookie_header(str_view_t str) noexcept {
            parse_SE_value(str);
        }

      public:
        /**
         * empty basic_cookie
         */
        basic_cookie_common()                                 = default;
        basic_cookie_common(const basic_cookie_common& c)     = default;
        basic_cookie_common(basic_cookie_common&& c) noexcept = default;

        basic_cookie_common(name_t __name, value_t __value) noexcept
          : _name(trim_copy(__name)),
            _value(trim_copy(__value)) {
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

        inline auto const& name() const noexcept {
            return _name;
        }
        inline auto const& value() const noexcept {
            return _value;
        }

        auto& name(name_t __name) noexcept {
            trim(__name);
            this->_name = std::move(__name);
            return *this;
        }

        auto& value(value_t __value) noexcept {
            trim(__value);
            _value = std::move(__value);
            return *this;
        }
    };

    template <typename Traits, bool Mutable>
    struct basic_request_cookie : public basic_cookie_common<Traits, Mutable> {
      private:
        using super = basic_cookie_common<Traits, Mutable>;

      public:
        explicit basic_request_cookie(
          typename super::str_view_t const& /* source */) noexcept {
            // TODO: implement this, it's important
        }
    };

    template <typename Traits, bool Mutable>
    struct basic_response_cookie : public basic_cookie_common<Traits, Mutable> {
      private:
        using super = basic_cookie_common<Traits, Mutable>;

      public:
        using date_t      = std::chrono::time_point<std::chrono::system_clock>;
        using domain_t    = typename super::storing_str_t;
        using path_t      = typename super::storing_str_t;
        using expires_t   = std::optional<date_t>;
        using max_age_t   = unsigned long;
        using same_site_t = typename super::same_site_value;
        using secure_t    = bool;
        using host_only_t = bool;
        using prefix_t    = bool;
        using encrypted_t = bool;
        using comment_t   = typename super::storing_str_t;

        using attrs_t =
          stl::unordered_map<Traits, typename super::storing_str_t,
                             typename super::storing_str>;


      private:
        mutable domain_t    _domain;
        mutable path_t      _path;
        mutable expires_t   _expires;
        mutable comment_t   _comment;
        mutable max_age_t   _max_age   = 0;
        mutable same_site_t _same_site = super::same_site_value::NONE;
        mutable secure_t    _secure    = false;
        mutable host_only_t _host_only = false;
        mutable encrypted_t _encrypted = false;
        mutable prefix_t    _prefix    = false;

        // todo: encapsulate this
        attrs_t attrs;

      public:
        explicit basic_response_cookie(
          typename super::str_view_t const& /* source */) noexcept {
            // TODO: implement this, it's important
        }

        inline auto const& comment() const noexcept {
            return _comment;
        }
        inline auto const& domain() const noexcept {
            return _domain;
        }
        inline auto const& max_age() const noexcept {
            return _max_age;
        }
        inline auto const& secure() const noexcept {
            return _secure;
        }
        inline auto const& host_only() const noexcept {
            return _host_only;
        }
        inline auto const& prefix() const noexcept {
            return _prefix;
        }
        inline auto const& expires() const noexcept {
            return *_expires;
        }
        inline auto const& same_site() const noexcept {
            return _same_site;
        }
        inline auto const& path() const noexcept {
            return _path;
        }
        inline auto const& encrypted() const noexcept {
            return _encrypted;
        }

        auto& comment(comment_t __comment) noexcept {
            _comment = std::move(__comment);
            return *this;
        }

        auto& domain(domain_t __domain) noexcept {
            _domain = std::move(__domain);
            return *this;
        }
        auto& path(path_t __path) noexcept {
            _path = std::move(__path);
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
            using namespace std::chrono;
            if (__remove) {
                // set the expire date 10 year before now:
                expires(system_clock::now() -
                        duration<int, std::ratio<60 * 60 * 24 * 365>>(10));
            } else if (is_removed()) {
                // set the expire date 1 year from now:
                expires(system_clock::now() +
                        duration<int, std::ratio<60 * 60 * 24 * 365>>(1));
            }
            // remove max-age if it exists because we're going with expires
            max_age(0);
            return *this;
        }

        [[nodiscard]] bool is_removed() const noexcept {
            using namespace std::chrono;
            return *_expires < system_clock::now();
        }

        /**
         * @brief sets expiration time relative to now.
         */
        template <typename D, typename T>
        inline auto&
        expires_in(std::chrono::duration<D, T> const& __dur) noexcept {
            _expires = std::chrono::system_clock::now() + __dur;
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

        std::basic_ostream<typename super::char_type>& operator<<(
          std::basic_ostream<typename super::char_type>& out) const noexcept {
            using namespace std::chrono;
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
                    std::time_t expires_c  = system_clock::to_time_t(*_expires);
                    std::tm     expires_tm = *std::localtime(&expires_c);
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

        template <bool ISMutable>
        bool operator==(
          basic_request_cookie<Traits, ISMutable> const& c) const noexcept {
            return super::_name == c._name && super::_value == c._value;
        }

        template <bool ISMutable>
        bool operator==(
          basic_response_cookie<Traits, ISMutable> const& c) const noexcept {
            return super::_name == c._name && super::_value == c._value &&
                   _prefix == c._prefix && _encrypted == c._encrypted &&
                   _secure == c._secure && _host_only == c._host_only &&
                   _same_site == c._same_site && _comment == c._comment &&
                   _expires == c._expires && _path == c._path &&
                   _domain == c._domain && attrs == c.attrs;
        }

        template <bool ISMutable>
        bool operator<(
          basic_response_cookie<Traits, ISMutable> const& c) const noexcept {
            return _expires < c._expires;
        }

        template <bool ISMutable>
        bool operator>(
          basic_response_cookie<Traits, ISMutable> const& c) const noexcept {
            return _expires > c._expires;
        }

        template <bool ISMutable>
        bool operator<=(
          basic_response_cookie<Traits, ISMutable> const& c) const noexcept {
            return _expires <= c._expires;
        }

        template <bool ISMutable>
        bool operator>=(
          basic_response_cookie<Traits, ISMutable> const& c) const noexcept {
            return _expires >= c._expires;
        }

        [[nodiscard]] typename super::str_t render() const noexcept {
            std::basic_ostringstream<typename super::char_type> os;
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
        template <bool ISMutable>
        [[nodiscard]] bool same_as(
          basic_response_cookie<Traits, ISMutable> const& c) const noexcept {
            return super::_name == c._name && _path == c._path &&
                   c._domain == _domain;
        }

        template <bool ISMutable>
        friend inline void
        swap(basic_response_cookie<Traits, ISMutable>& first,
             basic_response_cookie<Traits, ISMutable>& second) noexcept {
            using std::swap;
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

    /**
     * Cookie classes are "views of data" type of classes. That means these
     * classes will not own their own data and they are just a representation
     * of data which makes the life of the developers more comfortable.
     *
     * Here, the header classes (which are "owners of data") will have the data
     * these classes need and they provide full access to their data to these
     * classes so they can read and write structured and meaningful data to
     * them.
     *
     * The basic_cookie class can be instantiated by the developer and also the
     * header classes; and also this class will be used in both requests and
     * responses. This makes this class very hard to obtain because it also
     * should be just a representation of data and not the owner of the data; so
     * the cookie_jar class has to immediately write this data to the header
     * data and change the pointers/remove the whole basic_cookie class that the
     * developer created.
     */
    template <typename Traits = std_traits, bool Mutable = true,
              header_type HeaderType = header_type::response>
    class basic_cookie
      : public ::std::conditional_t<HeaderType == header_type::response,
                                    basic_response_cookie<Traits, Mutable>,
                                    basic_request_cookie<Traits, Mutable>> {
        using super =
          ::std::conditional_t<HeaderType == header_type::response,
                               basic_response_cookie<Traits, Mutable>,
                               basic_request_cookie<Traits, Mutable>>;

      public:
        static constexpr auto get_header_type() noexcept {
            return HeaderType;
        }

        friend struct cookie_hash<Traits, Mutable, HeaderType>;
        friend class cookie_jar<Traits, Mutable, HeaderType>;
    };

    // hash function of std::unordered_set<webpp::basic_cookie>
    template <typename Traits, bool Mutable, header_type HeaderType>
    struct cookie_hash {

        template <class T>
        inline void hash_combine(std::size_t& seed, const T& v) {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
        }

        using argument_type = webpp::basic_cookie<Traits, Mutable, HeaderType>;
        using result_type   = std::size_t;

        result_type operator()(argument_type const& c) const noexcept {
            // change the "same_as" method too if you ever touch this function
            cookie_hash::result_type seed = 0;
            hash_combine(seed, c._name);
            if constexpr (HeaderType == header_type::response) {
                hash_combine(seed, c._domain);
                hash_combine(seed, c._path);
            }
            //    hash_combine(seed, c._value);
            //    hash_combine(seed, c._prefix);
            //    hash_combine(seed, c._secure);
            //    if (c._expires)
            //        hash_combine(seed,
            //        c._expires->time_since_epoch().count());
            //    hash_combine(seed, c._max_age);
            //    hash_combine(seed, c._same_site);
            //    hash_combine(seed, c._comment);
            //    hash_combine(seed, c._host_only);
            //    hash_combine(seed, c._encrypted);
            return seed;
        }
    };

    template <typename Traits, bool Mutable, header_type HeaderType>
    struct cookie_equals {
        using cookie_type = basic_cookie<Traits, Mutable, HeaderType>;

        bool operator()(const cookie_type& lhs,
                        const cookie_type& rhs) const noexcept {
            if constexpr (HeaderType == header_type::response) {
                return lhs.name() == rhs.name() &&
                       lhs.domain() == rhs.domain() && lhs.path() == rhs.path();
            } else {
                return lhs.name() == rhs.name();
            }
        }
    };

    /**
     * @brief The cookies class (it's a basic_cookie jar for cookies)
     * This class should only be created by header classes (owners of data)
     *
     * This class will be used by both response and request header classes; and
     * since the basic_cookie class cannot hold its data for a long time, this
     * class has to put new cookies into the header classes before the
     * string_views's in basic_cookie class go out of scope.
     */
    template <typename Traits, bool Mutable, header_type HeaderType>
    class cookie_jar
      : public stl::unordered_set<Traits,
                                  basic_cookie<Traits, Mutable, HeaderType>,
                                  cookie_hash<Traits, Mutable, HeaderType>,
                                  cookie_equals<Traits, Mutable, HeaderType>> {
      public:
        using traits    = Traits;
        using cookie_t  = basic_cookie<Traits, Mutable, HeaderType>;
        using condition = std::function<bool(cookie_t const&)>;

      private:
        using super =
          stl::unordered_set<Traits, basic_cookie<Traits, Mutable, HeaderType>,
                             cookie_hash<Traits, Mutable, HeaderType>,
                             cookie_equals<Traits, Mutable, HeaderType>>;

      private:
        // Defaults:
        //        typename cookie_t::path_t _path;
        //        typename cookie_t::domain_t _domain;
        //        typename cookie_t::prefix_t _prefix;
        //        typename cookie_t::secure_t _secure;
        //        typename cookie_t::max_age_t _max_age;
        //        typename cookie_t::encrypted_t _encrypted;
        //        typename cookie_t::comment_t _comment;
        //        decltype(typename cookie_t::attrs) _attrs;
        //        typename cookie_t::same_site_t _same_site;
        //        typename cookie_t::host_only_t _host_only;

        //        inline void set_defaults_for(basic_cookie& c) {
        //            c.path(_path)
        //                .prefix(_prefix)
        //                .max_age(_max_age)
        //                .domain(_domain)
        //                .comment(_comment)
        //                .encrypted(_encrypted)
        //                .secure(_secure)
        //                .host_only(_host_only)
        //                .same_site(_same_site);
        //            if (!_attrs.empty())
        //                c.attrs.insert(_attrs.begin(), _attrs.end());
        //        }

        /**
         * @brief This function will make sure that the cookies are stay unique
         * in the cookie jar
         * @param check
         */
        template <typename T>
        void make_it_unique(typename super::const_iterator const& dont_touch,
                            T const& check) noexcept {
            for (auto it = super::begin(); it != super::end(); it++) {
                if (check(*it) && dont_touch != it &&
                    dont_touch->same_as(*it)) {
                    erase(it);
                    break;
                }
            }
        }

        /**
         * @brief change every basic_cookie if meats some condition
         * @param if_statement
         * @param change = std::function<void(iterator&)>
         */
        template <typename T>
        void change_if(condition const& if_statement,
                       T const&         change) noexcept {
            for (auto it = super::begin(); it != super::end(); it++)
                if (if_statement(*it))
                    change(it);
        }

        /**
         * Change every basic_cookie named something
         * @param _name
         * @param change
         */
        template <typename T>
        void change_if(typename cookie_t::name_t const& _name,
                       T const&                         change) noexcept {
            for (auto it = super::begin(); it != super::end(); it++)
                if (it->_name == _name)
                    change(it);
        }

        template <typename T>
        void change_all(T const& change) noexcept {
            for (auto it = super::begin(); it != super::end(); it++)
                change(it);
        }

      public:
        using super::unordered_set; // constructors

        typename super::const_iterator
        find(typename cookie_t::name_t const& name) const noexcept {
            return std::find_if(super::cbegin(), super::cend(),
                                [&](auto const& a) {
                                    return a.name() == name;
                                });
        }

        typename super::const_iterator find(cookie_t const& c) const noexcept {
            return std::find_if(super::cbegin(), super::cend(),
                                [&](auto const& a) {
                                    return a.same_as(c);
                                });
        }

        template <typename Name, class... Args>
        std::pair<typename super::iterator, bool> emplace(Name&& name,
                                                          Args&&... args) {
            auto found =
              find(name); // we don't have a problem here because we are sure
                          // that the domain and the path are not the same
                          // here. so we just look for the name
            if (found != super::cend())
                erase(found);
            return static_cast<super*>(this)->emplace(
              std::forward<Name>(name), std::forward<Args>(args)...);
        }

        template <typename Name, class... Args>
        typename super::iterator
        emplace_hint(typename super::const_iterator hint, Name&& name,
                     Args&&... args) noexcept {
            auto found =
              find(name); // we don't have a problem here because we are sure
                          // that the domain and the path are not the same
                          // here. so we just look for the name
            if (found != super::cend())
                erase(found);
            return static_cast<super*>(this)->emplace_hint(
              hint, std::forward<Name>(name), std::forward<Args>(args)...);
        }

        std::pair<typename super::iterator, bool>
        insert(const typename super::value_type& value) {
            auto found = find(value);
            if (found != super::cend())
                erase(found);
            return static_cast<super*>(this)->insert(value);
        }


        std::pair<typename super::iterator, bool>
        insert(typename super::value_type&& value) {
            auto found = find(value);
            if (found != super::cend())
                erase(found);
            return static_cast<super*>(this)->insert(std::move(value));
        }

        typename super::iterator
        insert(typename super::const_iterator    hint,
               const typename super::value_type& value) {
            auto found = find(value);
            if (found != super::cend())
                erase(found);
            return static_cast<super*>(this)->insert(hint, value);
        }

        typename super::iterator insert(typename super::const_iterator hint,
                                        typename super::value_type&&   value) {
            auto found = find(value);
            if (found != super::cend())
                erase(found);
            return static_cast<super*>(this)->insert(hint, std::move(value));
        }

        void insert(std::initializer_list<typename super::value_type> ilist) {
            for (const auto& it : ilist) {
                auto found = find(it);
                if (found != super::cend())
                    erase(found);
            }
            static_cast<super*>(this)->insert(ilist);
        }

        template <class InputIt>
        void insert(InputIt first, InputIt last) {
            for (auto it = first; it != last;) {
                auto found = find(*it);
                if (found != super::cend())
                    erase(found);
                else
                    ++it;
            }
            return static_cast<super*>(this)->insert(first, last);
        }

        //        insert_return_type insert(node_type&& nh) {}
        //        iterator insert(const_iterator hint, node_type&& nh) {}

        /**
         * @brief mark all cookies as encrypted
         * @param _encrypted
         * @return
         */
        cookie_jar&
        encrypted(typename cookie_t::encrypted_t const& _encrypted) noexcept {
            for (auto& c : *this)
                c._encrypted = _encrypted;
            return *this;
        }

        cookie_jar&
        encrypted(condition const&                      _condition,
                  typename cookie_t::encrypted_t const& _encrypted) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_encrypted = _encrypted;
            });
            return *this;
        }


        cookie_jar&
        encrypted(typename cookie_t::name_t const&      _name,
                  typename cookie_t::encrypted_t const& _encrypted) noexcept {
            change_if(_name, [&](auto& it) {
                it->_encrypted = _encrypted;
            });
            return *this;
        }

        cookie_jar&
        encrypted(typename super::const_iterator const& it,
                  typename cookie_t::encrypted_t        _encrypted) noexcept {
            it->_encrypted = _encrypted;
            return *this;
        }

        /**
         * @brief mark all cookies as secure
         * @param _secure
         * @return
         */
        cookie_jar&
        secure(typename cookie_t::secure_t const& _secure) noexcept {
            for (auto& c : *this)
                c._secure = _secure;
            return *this;
        }

        cookie_jar&
        secure(condition const&                   _condition,
               typename cookie_t::secure_t const& _secure) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_secure = _secure;
            });
            return *this;
        }


        cookie_jar&
        secure(typename cookie_t::name_t const&   _name,
               typename cookie_t::secure_t const& _secure) noexcept {
            change_if(_name, [&](auto& it) {
                it->_secure = _secure;
            });
            return *this;
        }


        cookie_jar& secure(typename super::const_iterator const& it,
                           typename cookie_t::secure_t _secure) noexcept {
            it->_secure = _secure;
            return *this;
        }

        /**
         * @brief make every basic_cookie host_only
         * @param _host_only
         * @return
         */
        cookie_jar&
        host_only(typename cookie_t::host_only_t const& _host_only) noexcept {
            for (auto& c : *this)
                c._host_only = _host_only;
            return *this;
        }


        cookie_jar&
        host_only(condition const&                      _condition,
                  typename cookie_t::host_only_t const& _host_only) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_host_only = _host_only;
            });
            return *this;
        }

        cookie_jar&
        host_only(typename cookie_t::name_t const&      _name,
                  typename cookie_t::host_only_t const& _host_only) noexcept {
            change_if(_name, [&](auto& it) {
                it->_host_only = _host_only;
            });
            return *this;
        }

        cookie_jar&
        host_only(typename super::const_iterator const& it,
                  typename cookie_t::host_only_t        _host_only) noexcept {
            it->_host_only = _host_only;
            return *this;
        }

        /**
         * @brief enable basic_cookie name prefix in all cookies
         * @param _prefix
         * @return
         */
        cookie_jar&
        prefix(typename cookie_t::prefix_t const& _prefix) noexcept {
            for (auto& c : *this)
                c._prefix = _prefix;
            return *this;
        }

        cookie_jar&
        prefix(typename cookie_t::name_t const&   _name,
               typename cookie_t::prefix_t const& _prefix) noexcept {
            change_if(_name, [&](auto& it) {
                it->_prefix = _prefix;
            });
            return *this;
        }

        cookie_jar&
        prefix(condition const&                   _condition,
               typename cookie_t::prefix_t const& _prefix) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_prefix = _prefix;
            });
            return *this;
        }

        cookie_jar& prefix(typename super::const_iterator const& it,
                           typename cookie_t::prefix_t _prefix) noexcept {
            it->_prefix = _prefix;
            return *this;
        }

        /**
         * @brief set a comment for every basic_cookie
         * @param _comment
         * @return
         */
        cookie_jar&
        comment(typename cookie_t::comment_t const& _comment) noexcept {
            for (auto& c : *this)
                c._comment = _comment;
            return *this;
        }

        cookie_jar&
        comment(condition const&                    _condition,
                typename cookie_t::comment_t const& _comment) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_comment = _comment;
            });
            return *this;
        }

        cookie_jar&
        comment(typename cookie_t::name_t const&    _name,
                typename cookie_t::comment_t const& _comment) noexcept {
            change_if(_name, [&](auto& it) {
                it->_comment = _comment;
            });
            return *this;
        }

        cookie_jar& comment(typename super::const_iterator const& it,
                            typename cookie_t::comment_t&& _comment) noexcept {
            it->_comment = std::move(_comment);
            return *this;
        }

        cookie_jar&
        comment(typename super::const_iterator const& it,
                typename cookie_t::comment_t const&   _comment) noexcept {
            return comment(it, (typename cookie_t::comment_t){_comment});
        }

        /**
         * @brief make same_site enabled for every basic_cookie
         * @param _same_site
         * @return
         */
        cookie_jar&
        same_site(typename cookie_t::same_site_t const& _same_site) noexcept {
            for (auto& c : *this)
                c._same_site = _same_site;
            return *this;
        }

        cookie_jar&
        same_site(typename cookie_t::name_t const&      _name,
                  typename cookie_t::same_site_t const& _same_site) noexcept {
            change_if(_name, [&](auto& it) {
                it->_same_site = _same_site;
            });
            return *this;
        }

        cookie_jar&
        same_site(condition const&                      _condition,
                  typename cookie_t::same_site_t const& _same_site) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_same_site = _same_site;
            });
            return *this;
        }

        cookie_jar&
        same_site(typename super::const_iterator const& it,
                  typename cookie_t::same_site_t        _same_site) noexcept {
            it->_same_site = _same_site;
            return *this;
        }

        /**
         * @brief set the same expiration date for every basic_cookie
         * @param _expires
         * @return
         */
        cookie_jar&
        expires(typename cookie_t::date_t const& _expires) noexcept {
            for (auto& c : *this)
                c._expires = _expires;
            return *this;
        }

        cookie_jar&
        expires(typename cookie_t::name_t const& _name,
                typename cookie_t::date_t const& _expires) noexcept {
            change_if(_name, [&](auto& it) {
                it->_expires = _expires;
            });
            return *this;
        }

        cookie_jar&
        expires(condition const&                 _condition,
                typename cookie_t::date_t const& _expires) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_expires = _expires;
            });
            return *this;
        }

        cookie_jar& expires(typename super::const_iterator const& it,
                            typename cookie_t::date_t&& _expires) noexcept {
            it->_expires = _expires;
            return *this;
        }

        cookie_jar&
        expires(typename super::const_iterator const& it,
                typename cookie_t::date_t const&      _expires) noexcept {
            return expires(it, (typename cookie_t::date_t)(_expires));
        }

        /**
         * @brief set the max_age for every basic_cookie
         * @param _max_age
         * @return
         */
        cookie_jar&
        max_age(typename cookie_t::max_age_t const& _max_age) noexcept {
            for (auto& c : *this)
                c._max_age = _max_age;
            return *this;
        }

        cookie_jar&
        max_age(typename cookie_t::name_t const&    _name,
                typename cookie_t::max_age_t const& _max_age) noexcept {
            change_if(_name, [&](auto& it) {
                it->_max_age = _max_age;
            });
            return *this;
        }

        cookie_jar&
        max_age(condition const&                    _condition,
                typename cookie_t::max_age_t const& _max_age) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_max_age = _max_age;
            });
            return *this;
        }

        cookie_jar& max_age(typename super::const_iterator const& it,
                            typename cookie_t::max_age_t _max_age) noexcept {
            it->_max_age = _max_age;
            return *this;
        }

        cookie_jar& value(typename cookie_t::value_t const& _value) noexcept {
            for (auto& c : *this)
                c._value = _value;
            return *this;
        }

        cookie_jar& value(typename cookie_t::name_t const&  _name,
                          typename cookie_t::value_t const& _value) noexcept {
            change_if(_name, [&](auto& it) {
                it->_value = _value;
            });
            return *this;
        }

        cookie_jar& value(typename super::const_iterator const& it,
                          typename cookie_t::value_t const& _value) noexcept {
            return value(it, (typename cookie_t::value_t)(_value));
        }

        cookie_jar& value(typename super::const_iterator const& it,
                          typename cookie_t::value_t&& _value) noexcept {
            it->_value = std::move(_value);
            return *this;
        }

        cookie_jar& value(condition const&                  _condition,
                          typename cookie_t::value_t const& _value) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_value = _value;
            });
            return *this;
        }

        cookie_jar& path(typename cookie_t::path_t const& _path) noexcept {
            change_all([&](auto& it) {
                it->_path = _path;
                make_it_unique(it, [&](auto const& c) {
                    return c._path == _path;
                });
            });
            return *this;
        }

        cookie_jar& path(typename cookie_t::name_t const& _name,
                         typename cookie_t::path_t const& _path) noexcept {
            change_if(_name, [&](auto& it) {
                it->_path = _path;
                make_it_unique(it, [&](auto const& c) {
                    return c._path == _path;
                });
            });
            return *this;
        }

        cookie_jar& path(condition const&                 _condition,
                         typename cookie_t::path_t const& _path) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_path = _path;
                make_it_unique(it, [&](auto const& c) {
                    return c._path == _path;
                });
            });
            return *this;
        }

        cookie_jar& path(typename super::const_iterator const& it,
                         typename cookie_t::path_t&&           _path) noexcept {
            it->_path = _path;
            make_it_unique(it, [&](auto const& c) {
                return c._path == it->_path;
            });
            return *this;
        }

        cookie_jar& path(typename super::const_iterator const& it,
                         typename cookie_t::path_t const&      _path) noexcept {
            return path(it, (typename cookie_t::path_t)(_path));
        }

        /**
         * @brief change every basic_cookie's domain to the specified value
         * @param _domain
         * @return
         */
        cookie_jar&
        domain(typename cookie_t::domain_t const& _domain) noexcept {
            change_all([&](auto& it) {
                it->_domain = _domain;
                make_it_unique(it, [&](auto const& c) {
                    return c._domain == _domain;
                });
            });
            return *this;
        }

        cookie_jar&
        domain(typename cookie_t::name_t const&   _name,
               typename cookie_t::domain_t const& new_domain) noexcept {
            change_if(_name, [&](auto& it) {
                it->_domain = new_domain;
                make_it_unique(it, [&](auto const& c) {
                    return c._domain == new_domain;
                });
            });
            return *this;
        }

        cookie_jar& domain(typename super::const_iterator const& it,
                           typename cookie_t::domain_t&& new_domain) noexcept {
            it->_domain = std::move(new_domain);
            make_it_unique(it, [&](auto const& c) {
                return c._domain == it->_domain;
            });
            return *this;
        }

        cookie_jar&
        domain(typename super::const_iterator const& it,
               typename cookie_t::domain_t const&    new_domain) noexcept {
            return domain(it, (typename cookie_t::domain_t)(new_domain));
        }

        cookie_jar&
        domain(condition const&                   _condition,
               typename cookie_t::domain_t const& new_domain) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_domain = new_domain;
            });
            return *this;
        }

        /**
         * @brief Change ever basic_cookie's name to the specified value
         * @param _name
         * @return
         * @return
         * @return
         */
        cookie_jar& name(typename cookie_t::name_t const& _name) noexcept {
            if (auto first = super::begin(); first != super::end()) {
                first->_name = _name;
                erase(std::next(first), super::end()); // remove the rest
            }
            return *this;
        }

        /**
         * @brief performing rename
         * @param old_name
         * @param new_name
         * @return
         */
        cookie_jar& name(typename cookie_t::name_t const& old_name,
                         typename cookie_t::name_t const& new_name) noexcept {
            erase(find(new_name));
            if (auto found = find(old_name); found != super::end())
                found->_name = new_name;
            return *this;
        }

        cookie_jar& name(typename super::const_iterator const& it,
                         typename cookie_t::name_t&& new_name) noexcept {
            erase(find(new_name));
            it->_name = std::move(new_name);
            return *this;
        }

        cookie_jar& name(typename super::const_iterator const& it,
                         typename cookie_t::name_t const& new_name) noexcept {
            return name(it, (typename cookie_t::name_t)(new_name));
        }

        /**
         * @brief rename the cookies that meed the condition
         * @param condition
         * @param new_name
         * @return
         */
        cookie_jar& name(condition const&                 _condition,
                         typename cookie_t::name_t const& new_name) noexcept {
            erase(find(new_name));
            if (auto found =
                  std::find_if(super::begin(), super::end(), _condition);
                found != super::end()) {
                found->_name = new_name;
            }
            return *this;
        }
    };

    /*
    basic_cookie::basic_cookie(const basic_cookie& c) noexcept
        : attrs{c.attrs}, _name{c._name}, _value{c._value}, _domain{c._domain},
          _path{c._path},
          _expires(c._expires ? std::make_unique<date_t>(*c._expires) :
    nullptr), _max_age{c._max_age}, _same_site{c._same_site},
    _secure{c._secure}, _host_only{c._host_only}, _encrypted{c._encrypted},
    _prefix{c._prefix}, _comment{c._comment} {}

    basic_cookie::basic_cookie(basic_cookie&& c) noexcept
        : attrs{std::move(c.attrs)}, _name{std::move(c._name)},
    _value{std::move( c._value)}, _domain{std::move(c._domain)},
    _path{std::move(c._path)}, _expires(std::move(c._expires)),
    _max_age{c._max_age}, _same_site{c._same_site}, _secure{c._secure},
    _host_only{c._host_only}, _encrypted{c._encrypted}, _prefix{c._prefix},
    _comment{std::move( c._comment)} {}

    basic_cookie::basic_cookie(basic_cookie::name_t __name,
    basic_cookie::value_t
    __value) noexcept {
        // these two will trim the strings
        name(std::move(__name));
        value(std::move(__value));
    }

    basic_cookie& basic_cookie::operator=(const basic_cookie& c) noexcept {
        attrs = c.attrs;
        _name = c._name;
        _value = c._value;
        _domain = c._domain;
        _path = c._path;
        _expires = std::make_it_unique<date_t>(*c._expires);
        _max_age = c._max_age;
        _same_site = c._same_site;
        _secure = c._secure;
        _host_only = c._host_only;
        _encrypted = c._encrypted;
        _prefix = c._prefix;
        _comment = c._comment;
        return *this;
    }

    basic_cookie& basic_cookie::operator=(basic_cookie&& c) noexcept {
        attrs = std::move(c.attrs);
        _name = std::move(c._name);
        _value = std::move(c._value);
        _domain = std::move(c._domain);
        _path = std::move(c._path);
        _expires = std::move(c._expires);
        _max_age = c._max_age;
        _same_site = c._same_site;
        _secure = c._secure;
        _host_only = c._host_only;
        _encrypted = c._encrypted;
        _prefix = c._prefix;
        _comment = std::move(c._comment);
        return *this;
    }
    */


} // namespace webpp

#endif // WEBPP_COOKIES_H
