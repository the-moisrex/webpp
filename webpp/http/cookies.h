
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
 *    [ ] Encryption for the cookie name
 *    [ ] Decryption
 *    [ ] Pre Defaults in the cookie jar
 *    [ ] Implement "Cookie2:" and "Set-Cookie2:" obsolete headers
 *    [X] Add *_if methods in cookies
 *    [X] Add customization of cookies in the cookie jar. e.g:
 *         encrypted("cookiename", true)
 *    [X] Move the definitions of the cookie jar into cookies.cpp file
 *    [ ] Consider renaming "cookies" to "cookie_jar"
 *    [ ] Add doxygen documentations/comments to the declations
 *    [ ] Does user's browser support cookies
 *    [ ] Does user's browser support cookies but now it's disabled
 */

#include "../utils/strings.h"
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string_view>
#include <unordered_set>

namespace webpp {

    struct cookie_hash;
    class cookie_jar;

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
     * The cookie class can be instantiated by the developer and also the header
     * classes; and also this class will be used in both requests and responses.
     * This makes this class very hard to obtain because it also should be just
     * a representation of data and not the owner of the data; so the
     * cookie_jar class has to immediately write this data to the header data
     * and change the pointers/remove the whole cookie class that the developer
     * created.
     */
    class cookie {
      public:
        enum class same_site_value { NONE, LAX, STRICT };

        // TODO: consider using "variant<string, string_view>" instead of
        // string_view
        using date_t = std::chrono::time_point<std::chrono::system_clock>;
        using name_t = std::string;
        using value_t = std::string;
        using domain_t = std::string;
        using path_t = std::string;
        using expires_t = std::optional<date_t>;
        using max_age_t = unsigned long;
        using same_site_t = same_site_value;
        using secure_t = bool;
        using host_only_t = bool;
        using prefix_t = bool;
        using encrypted_t = bool;
        using comment_t = std::string;

        std::map<std::string, std::string> attrs;

      private:
        mutable name_t _name;
        mutable value_t _value;
        mutable domain_t _domain;
        mutable path_t _path;
        mutable expires_t _expires;
        mutable max_age_t _max_age = 0;
        mutable same_site_t _same_site = same_site_value::NONE;
        mutable secure_t _secure = false;
        mutable host_only_t _host_only = false;
        mutable encrypted_t _encrypted = false;
        mutable prefix_t _prefix = false;
        mutable comment_t _comment;

      public:
        /**
         * empty cookie
         */
        cookie() noexcept = default;
        cookie(const cookie& c) = default;
        cookie(cookie&& c) noexcept = default;
        // TODO: implement this:
        explicit cookie(std::string_view source) noexcept;
        cookie(name_t __name, value_t __value) noexcept
            : _name(trim_copy(__name)), _value(trim_copy(__value)) {}

        cookie& operator=(const cookie& c) = default;
        cookie& operator=(cookie&& c) noexcept = default;

        inline auto const& name() const noexcept { return _name; }
        inline auto const& value() const noexcept { return _value; }
        inline auto const& comment() const noexcept { return _comment; }
        inline auto const& domain() const noexcept { return _domain; }
        inline auto const& max_age() const noexcept { return _max_age; }
        inline auto const& secure() const noexcept { return _secure; }
        inline auto const& host_only() const noexcept { return _host_only; }
        inline auto const& prefix() const noexcept { return _prefix; }
        inline auto const& expires() const noexcept { return *_expires; }
        inline auto const& same_site() const noexcept { return _same_site; }
        inline auto const& path() const noexcept { return _path; }
        inline auto const& encrypted() const noexcept { return _encrypted; }

        cookie& name(name_t _name) noexcept;
        cookie& value(value_t __value) noexcept;
        cookie& comment(comment_t __comment) noexcept;
        cookie& domain(domain_t __domain) noexcept;
        cookie& path(path_t __path) noexcept;
        cookie& max_age(max_age_t __max_age) noexcept;
        cookie& prefix(prefix_t __prefix) noexcept;
        cookie& same_site(same_site_t __same_site) noexcept;
        cookie& secure(secure_t __secure) noexcept;
        cookie& host_only(host_only_t __host_only) noexcept;
        cookie& expires(date_t __expires) noexcept;
        cookie& remove(bool __remove = true) noexcept;
        bool is_removed() const noexcept;

        /**
         * @brief sets exipiration time relative to now.
         */
        template <typename D, typename T>
        inline cookie&
        expires_in(std::chrono::duration<D, T> const& __dur) noexcept {
            _expires = std::chrono::system_clock::now() + __dur;
            return *this;
        }

        /**
         * @brief encrypt the value
         * @param __encrypted
         * @return
         */
        cookie& encrypted(encrypted_t __encrypted) noexcept;

        /**
         * @brief decryptable encryption
         */
        value_t encrypted_value() const noexcept;

        std::ostream& operator<<(std::ostream& out) const noexcept;
        bool operator==(cookie const& c) const noexcept;
        bool operator<(cookie const& c) const noexcept;
        bool operator>(cookie const& c) const noexcept;
        bool operator<=(cookie const& c) const noexcept;
        bool operator>=(cookie const& c) const noexcept;
        std::string render() const noexcept;

        /**
         * @brief this method will return true if the specified cookie and this
         * cookie have the same  name, domain, and path. so they will replace
         * each other if they put in the same cookie jar
         * @param c
         * @return true if they have the same name, domain, and path
         */
        bool same_as(cookie const& c) const noexcept;

        friend inline void swap(cookie& first, cookie& second) noexcept {
            using std::swap;
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

        friend struct cookie_hash;
        friend class cookie_jar;
    };

    // hash function of std::unordered_set<webpp::cookie>
    struct cookie_hash {
        using argument_type = webpp::cookie;
        using result_type = std::size_t;
        result_type operator()(argument_type const& c) const noexcept;
    };

    struct cookie_equals {
        bool operator()(const cookie& lhs, const cookie& rhs) const noexcept;
    };

    /**
     * @brief The cookies class (it's a cookie jar for cookies)
     * This class should only be created by header classes (owners of data)
     *
     * This class will be used by both response and request header classes; and
     * since the cookie class cannot hold its data for a long time, this class
     * has to put new cookies into the header classes before the string_views's
     * in cookie class go out of scope.
     */
    class cookie_jar
        : public std::unordered_set<webpp::cookie, cookie_hash, cookie_equals> {

      public:
        using condition = std::function<bool(cookie const&)>;

      private:
        using super =
            std::unordered_set<webpp::cookie, cookie_hash, cookie_equals>;

        // Defaults:
        //        cookie::path_t _path;
        //        cookie::domain_t _domain;
        //        cookie::prefix_t _prefix;
        //        cookie::secure_t _secure;
        //        cookie::max_age_t _max_age;
        //        cookie::encrypted_t _encrypted;
        //        cookie::comment_t _comment;
        //        decltype(cookie::attrs) _attrs;
        //        cookie::same_site_t _same_site;
        //        cookie::host_only_t _host_only;

        //        inline void set_defaults_for(cookie& c) {
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
        void make_it_unique(const const_iterator& dont_touch,
                            T const& check) noexcept {
            for (auto it = begin(); it != end(); it++) {
                if (check(*it) && dont_touch != it &&
                    dont_touch->same_as(*it)) {
                    erase(it);
                    break;
                }
            }
        }

        /**
         * @brief change every cookie if meats some condition
         * @param if_statement
         * @param change = std::function<void(iterator&)>
         */
        template <typename T>
        void change_if(condition const& if_statement,
                       T const& change) noexcept {
            for (auto it = begin(); it != end(); it++)
                if (if_statement(*it))
                    change(it);
        }

        /**
         * Change every cookie named something
         * @param _name
         * @param change
         */
        template <typename T>
        void change_if(cookie::name_t const& _name, T const& change) noexcept {
            for (auto it = begin(); it != end(); it++)
                if (it->_name == _name)
                    change(it);
        }

        template <typename T>
        void change_all(T const& change) noexcept {
            for (auto it = begin(); it != end(); it++)
                change(it);
        }

      public:
        using super::unordered_set; // constructors

        const_iterator find(cookie::name_t const& name) const noexcept;
        const_iterator find(cookie const& c) const noexcept;

        template <typename Name, class... Args>
        std::pair<iterator, bool> emplace(Name&& name, Args&&... args) {
            auto found =
                find(name); // we don't have a problem here because we are sure
                            // that the domain and the path are not the same
                            // here. so we just look for the name
            if (found != cend())
                erase(found);
            return static_cast<super*>(this)->emplace(
                std::forward<Name>(name), std::forward<Args>(args)...);
        }

        template <typename Name, class... Args>
        iterator emplace_hint(const_iterator hint, Name&& name,
                              Args&&... args) noexcept {
            auto found =
                find(name); // we don't have a problem here because we are sure
                            // that the domain and the path are not the same
                            // here. so we just look for the name
            if (found != cend())
                erase(found);
            return static_cast<super*>(this)->emplace_hint(
                hint, std::forward<Name>(name), std::forward<Args>(args)...);
        }

        std::pair<iterator, bool> insert(const value_type& value);
        std::pair<iterator, bool> insert(value_type&& value);
        iterator insert(const_iterator hint, const value_type& value);
        iterator insert(const_iterator hint, value_type&& value);
        void insert(std::initializer_list<value_type> ilist);

        template <class InputIt>
        void insert(InputIt first, InputIt last) {
            for (auto it = first; it != last;) {
                auto found = find(*it);
                if (found != cend())
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
        cookie_jar& encrypted(cookie::encrypted_t const& _encrypted) noexcept;
        cookie_jar& encrypted(condition const& _condition,
                              cookie::encrypted_t const& _encrypted) noexcept;
        cookie_jar& encrypted(cookie::name_t const& _name,
                              cookie::encrypted_t const& _encrypted) noexcept;
        cookie_jar& encrypted(const_iterator const& it,
                              cookie::encrypted_t _encrypted) noexcept;

        /**
         * @brief mark all cookies as secure
         * @param _secure
         * @return
         */
        cookie_jar& secure(cookie::secure_t const& _secure) noexcept;
        cookie_jar& secure(condition const& _condition,
                           cookie::secure_t const& _secure) noexcept;
        cookie_jar& secure(cookie::name_t const& _name,
                           cookie::secure_t const& _secure) noexcept;
        cookie_jar& secure(const_iterator const& it,
                           cookie::secure_t _secure) noexcept;

        /**
         * @brief make every cookie host_only
         * @param _host_only
         * @return
         */
        cookie_jar& host_only(cookie::host_only_t const& _host_only) noexcept;
        cookie_jar& host_only(condition const& _condition,
                              cookie::host_only_t const& _host_only) noexcept;
        cookie_jar& host_only(cookie::name_t const& _name,
                              cookie::host_only_t const& _host_only) noexcept;
        cookie_jar& host_only(const_iterator const& it,
                              cookie::host_only_t _host_only) noexcept;

        /**
         * @brief enable cookie name prefix in all cookies
         * @param _prefix
         * @return
         */
        cookie_jar& prefix(cookie::prefix_t const& _prefix) noexcept;
        cookie_jar& prefix(cookie::name_t const& _name,
                           cookie::prefix_t const& _prefix) noexcept;
        cookie_jar& prefix(condition const& _condition,
                           cookie::prefix_t const& _prefix) noexcept;
        cookie_jar& prefix(const_iterator const& it,
                           cookie::prefix_t _prefix) noexcept;

        /**
         * @brief set a comment for every cookie
         * @param _comment
         * @return
         */
        cookie_jar& comment(cookie::comment_t const& _comment) noexcept;
        cookie_jar& comment(condition const& _condition,
                            cookie::comment_t const& _comment) noexcept;
        cookie_jar& comment(cookie::name_t const& _name,
                            cookie::comment_t const& _comment) noexcept;
        cookie_jar& comment(const_iterator const& it,
                            cookie::comment_t&& _comment) noexcept;
        cookie_jar& comment(const_iterator const& it,
                            cookie::comment_t const& _comment) noexcept;

        /**
         * @brief make same_site enabled for every cookie
         * @param _same_site
         * @return
         */
        cookie_jar& same_site(cookie::same_site_t const& _same_site) noexcept;
        cookie_jar& same_site(cookie::name_t const& _name,
                              cookie::same_site_t const& _same_site) noexcept;
        cookie_jar& same_site(condition const& _condition,
                              cookie::same_site_t const& _same_site) noexcept;
        cookie_jar& same_site(const_iterator const& it,
                              cookie::same_site_t _same_site) noexcept;

        /**
         * @brief set the same expiriration date for every cookie
         * @param _expires
         * @return
         */
        cookie_jar& expires(cookie::date_t const& _expires) noexcept;
        cookie_jar& expires(cookie::name_t const& _name,
                            cookie::date_t const& _expires) noexcept;
        cookie_jar& expires(condition const& _condition,
                            cookie::date_t const& _expires) noexcept;
        cookie_jar& expires(const_iterator const& it,
                            cookie::date_t&& _expires) noexcept;
        cookie_jar& expires(const_iterator const& it,
                            cookie::date_t const& _expires) noexcept;

        /**
         * @brief set the max_age for every cookie
         * @param _max_age
         * @return
         */
        cookie_jar& max_age(cookie::max_age_t const& _max_age) noexcept;
        cookie_jar& max_age(cookie::name_t const& _name,
                            cookie::max_age_t const& _max_age) noexcept;
        cookie_jar& max_age(condition const& _condition,
                            cookie::max_age_t const& _max_age) noexcept;
        cookie_jar& max_age(const_iterator const& it,
                            cookie::max_age_t _max_age) noexcept;
        cookie_jar& value(cookie::value_t const& _value) noexcept;
        cookie_jar& value(cookie::name_t const& _name,
                          cookie::value_t const& _value) noexcept;
        cookie_jar& value(const_iterator const& it,
                          cookie::value_t const& _value) noexcept;
        cookie_jar& value(const_iterator const& it,
                          cookie::value_t&& _value) noexcept;
        cookie_jar& value(condition const& _condition,
                          cookie::value_t const& _value) noexcept;

        cookie_jar& path(cookie::path_t const& _path) noexcept;
        cookie_jar& path(cookie::name_t const& _name,
                         cookie::path_t const& _path) noexcept;
        cookie_jar& path(condition const& _condition,
                         cookie::path_t const& _path) noexcept;
        cookie_jar& path(const_iterator const& it,
                         cookie::path_t&& _path) noexcept;
        cookie_jar& path(const_iterator const& it,
                         cookie::path_t const& _path) noexcept;

        /**
         * @brief change every cookie's domain to the specified value
         * @param _domain
         * @return
         */
        cookie_jar& domain(cookie::domain_t const& _domain) noexcept;
        cookie_jar& domain(cookie::name_t const& _name,
                           cookie::domain_t const& new_domain) noexcept;
        cookie_jar& domain(const_iterator const& it,
                           cookie::domain_t&& new_domain) noexcept;
        cookie_jar& domain(const_iterator const& it,
                           cookie::domain_t const& new_domain) noexcept;
        cookie_jar& domain(condition const& _condition,
                           cookie::domain_t const& new_domain) noexcept;

        /**
         * @brief Change ever cookie's name to the specified value
         * @param _name
         * @return
         * @return
         * @return
         */
        cookie_jar& name(cookie::name_t const& _name) noexcept;

        /**
         * @brief performing rename
         * @param old_name
         * @param new_name
         * @return
         */
        cookie_jar& name(cookie::name_t const& old_name,
                         cookie::name_t const& new_name) noexcept;
        cookie_jar& name(const_iterator const& it,
                         cookie::name_t&& new_name) noexcept;
        cookie_jar& name(const_iterator const& it,
                         cookie::name_t const& new_name) noexcept;

        /**
         * @brief rename the cookies that meed the condition
         * @param condition
         * @param new_name
         * @return
         */
        cookie_jar& name(condition const& _condition,
                         cookie::name_t const& new_name) noexcept;

    };

} // namespace webpp

#endif // WEBPP_COOKIES_H
