
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
 *    [ ] Decryptions
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

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>

namespace webpp {

    struct cookie_hash;
    class cookies;

    class cookie {
      public:
        enum class same_site_value { NONE, LAX, STRICT };
        std::map<std::string, std::string> attrs;
        using date_t = std::chrono::time_point<std::chrono::system_clock>;

        using name_t = std::string;
        using value_t = std::string;
        using domain_t = std::string;
        using path_t = std::string;
        using expires_t = std::unique_ptr<date_t>;
        using max_age_t = unsigned long;
        using same_site_t = same_site_value;
        using secure_t = bool;
        using host_only_t = bool;
        using prefix_t = bool;
        using encrypted_t = bool;
        using comment_t = std::string;

      private:
        // I made them mutable because the cookies class wants to change them
        // manually and the cookies class is not a mutable container
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
        cookie() = default;
        cookie(const cookie&) noexcept;
        cookie(cookie&&) noexcept;
        // TODO: implement this:
        cookie(std::string source) noexcept;
        cookie(std::string __name, std::string __value) noexcept;

        cookie& operator=(const cookie&) noexcept;
        cookie& operator=(cookie&&) noexcept;

        inline decltype(_name) const& name() const noexcept { return _name; }
        inline decltype(_value) const& value() const noexcept { return _value; }
        inline decltype(_comment) const& commnet() const noexcept {
            return _comment;
        }
        inline decltype(_domain) const& domain() const noexcept {
            return _domain;
        }
        inline decltype(_max_age) const& max_age() const noexcept {
            return _max_age;
        }
        inline decltype(_secure) const& secure() const noexcept {
            return _secure;
        }
        inline decltype(_host_only) const& host_only() const noexcept {
            return _host_only;
        }
        inline decltype(_prefix) const& prefix() const noexcept {
            return _prefix;
        }
        inline date_t const& expires() const noexcept { return *_expires; }
        inline decltype(_same_site) const& same_site() const noexcept {
            return _same_site;
        }
        inline decltype(_path) const& path() const noexcept { return _path; }

        cookie& name(std::string __name) noexcept;
        cookie& value(std::string __value) noexcept;

        cookie& comment(std::string const& __comment) noexcept;

        cookie& comment(std::string&& __comment) noexcept;

        cookie& domain(std::string const& __domain) noexcept;
        cookie& domain(std::string&& __domain) noexcept;

        cookie& path(std::string const& __path) noexcept;

        cookie& path(std::string&& __path) noexcept;

        cookie& max_age(decltype(_max_age) __max_age) noexcept;

        cookie& prefix(decltype(_prefix) __prefix) noexcept;

        cookie& same_site(decltype(_same_site) __same_site) noexcept;

        cookie& secure(decltype(_secure) __secure) noexcept;

        cookie& host_only(decltype(_host_only) __host_only) noexcept;

        bool remove() const noexcept;

        cookie& remove(bool __remove) noexcept;

        cookie& expires(date_t __expires) noexcept;

        /**
         * @brief sets exipiration time relative to now.
         */
        template <typename D, typename T>
        inline cookie&
        expires_in(std::chrono::duration<D, T> const& __dur) noexcept {
            _expires = std::make_unique<date_t>(
                date_t{std::chrono::system_clock::now() + __dur});
            return *this;
        }

        inline decltype(_encrypted) encrypted() const noexcept {
            return _encrypted;
        }

        /**
         * @brief encrypt the value
         * @param __encrypted
         * @return
         */
        cookie& encrypted(decltype(_encrypted) __encrypted) noexcept;

        /**
         * @brief decryptable encryption
         */
        decltype(_value) encrypted_value() const noexcept;

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
        friend class cookies;
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
     */
    class cookies
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
        void
        make_unique(const const_iterator& dont_touch,
                    std::function<bool(cookie const&)> const& check) noexcept;

        /**
         * @brief change every cookie if meats some condition
         * @param if_statement
         * @param change
         */
        void change_if(condition const& if_statement,
                       std::function<void(iterator&)> const& change) noexcept;

        void change_if(cookie::name_t const& _name,
                       std::function<void(iterator&)> const& change) noexcept;

        void change_all(std::function<void(iterator&)> const& change) noexcept;

      public:
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

#if __cplusplus > 201402L
//        insert_return_type insert(node_type&& nh) {}
//        iterator insert(const_iterator hint, node_type&& nh) {}
#endif // C++17

        /**
         * @brief mark all cookies as encrypted
         * @param _encrypted
         * @return
         */
        cookies& encrypted(cookie::encrypted_t const& _encrypted) noexcept;
        cookies& encrypted(condition const& _condition,
                           cookie::encrypted_t const& _encrypted) noexcept;
        cookies& encrypted(cookie::name_t const& _name,
                           cookie::encrypted_t const& _encrypted) noexcept;
        cookies& encrypted(const_iterator const& it,
                           cookie::encrypted_t _encrypted) noexcept;

        /**
         * @brief mark all cookies as secure
         * @param _secure
         * @return
         */
        cookies& secure(cookie::secure_t const& _secure) noexcept;
        cookies& secure(condition const& _condition,
                        cookie::secure_t const& _secure) noexcept;
        cookies& secure(cookie::name_t const& _name,
                        cookie::secure_t const& _secure) noexcept;
        cookies& secure(const_iterator const& it,
                        cookie::secure_t _secure) noexcept;

        /**
         * @brief make every cookie host_only
         * @param _host_only
         * @return
         */
        cookies& host_only(cookie::host_only_t const& _host_only) noexcept;
        cookies& host_only(condition const& _condition,
                           cookie::host_only_t const& _host_only) noexcept;
        cookies& host_only(cookie::name_t const& _name,
                           cookie::host_only_t const& _host_only) noexcept;
        cookies& host_only(const_iterator const& it,
                           cookie::host_only_t _host_only) noexcept;

        /**
         * @brief enable cookie name prefix in all cookies
         * @param _prefix
         * @return
         */
        cookies& prefix(cookie::prefix_t const& _prefix) noexcept;
        cookies& prefix(cookie::name_t const& _name,
                        cookie::prefix_t const& _prefix) noexcept;
        cookies& prefix(condition const& _condition,
                        cookie::prefix_t const& _prefix) noexcept;
        cookies& prefix(const_iterator const& it,
                        cookie::prefix_t _prefix) noexcept;

        /**
         * @brief set a comment for every cookie
         * @param _comment
         * @return
         */
        cookies& comment(cookie::comment_t const& _comment) noexcept;
        cookies& comment(condition const& _condition,
                         cookie::comment_t const& _comment) noexcept;
        cookies& comment(cookie::name_t const& _name,
                         cookie::comment_t const& _comment) noexcept;
        cookies& comment(const_iterator const& it,
                         cookie::comment_t&& _comment) noexcept;
        cookies& comment(const_iterator const& it,
                         cookie::comment_t const& _comment) noexcept;

        /**
         * @brief make same_site enabled for every cookie
         * @param _same_site
         * @return
         */
        cookies& same_site(cookie::same_site_t const& _same_site) noexcept;
        cookies& same_site(cookie::name_t const& _name,
                           cookie::same_site_t const& _same_site) noexcept;
        cookies& same_site(condition const& _condition,
                           cookie::same_site_t const& _same_site) noexcept;
        cookies& same_site(const_iterator const& it,
                           cookie::same_site_t _same_site) noexcept;

        /**
         * @brief set the same expiriration date for every cookie
         * @param _expires
         * @return
         */
        cookies& expires(cookie::date_t const& _expires) noexcept;
        cookies& expires(cookie::name_t const& _name,
                         cookie::date_t const& _expires) noexcept;
        cookies& expires(condition const& _condition,
                         cookie::date_t const& _expires) noexcept;
        cookies& expires(const_iterator const& it,
                         cookie::date_t&& _expires) noexcept;
        cookies& expires(const_iterator const& it,
                         cookie::date_t const& _expires) noexcept;

        /**
         * @brief set the max_age for every cookie
         * @param _max_age
         * @return
         */
        cookies& max_age(cookie::max_age_t const& _max_age) noexcept;
        cookies& max_age(cookie::name_t const& _name,
                         cookie::max_age_t const& _max_age) noexcept;
        cookies& max_age(condition const& _condition,
                         cookie::max_age_t const& _max_age) noexcept;
        cookies& max_age(const_iterator const& it,
                         cookie::max_age_t _max_age) noexcept;
        cookies& value(cookie::value_t const& _value) noexcept;
        cookies& value(cookie::name_t const& _name,
                       cookie::value_t const& _value) noexcept;
        cookies& value(const_iterator const& it,
                       cookie::value_t const& _value) noexcept;
        cookies& value(const_iterator const& it,
                       cookie::value_t&& _value) noexcept;
        cookies& value(condition const& _condition,
                       cookie::value_t const& _value) noexcept;

        cookies& path(cookie::path_t const& _path) noexcept;
        cookies& path(cookie::name_t const& _name,
                      cookie::path_t const& _path) noexcept;
        cookies& path(condition const& _condition,
                      cookie::path_t const& _path) noexcept;
        cookies& path(const_iterator const& it,
                      cookie::path_t&& _path) noexcept;
        cookies& path(const_iterator const& it,
                      cookie::path_t const& _path) noexcept;

        /**
         * @brief change every cookie's domain to the specified value
         * @param _domain
         * @return
         */
        cookies& domain(cookie::domain_t const& _domain) noexcept;
        cookies& domain(cookie::name_t const& _name,
                        cookie::domain_t const& new_domain) noexcept;
        cookies& domain(const_iterator const& it,
                        cookie::domain_t&& new_domain) noexcept;
        cookies& domain(const_iterator const& it,
                        cookie::domain_t const& new_domain) noexcept;
        cookies& domain(condition const& _condition,
                        cookie::domain_t const& new_domain) noexcept;

        /**
         * @brief Change ever cookie's name to the specified value
         * @param _name
         * @return
         */
        cookies& name(cookie::name_t const& _name) noexcept;

        /**
         * @brief performing rename
         * @param old_name
         * @param new_name
         * @return
         */
        cookies& name(cookie::name_t const& old_name,
                      cookie::name_t const& new_name) noexcept;
        cookies& name(const_iterator const& it,
                      cookie::name_t&& new_name) noexcept;
        cookies& name(const_iterator const& it,
                      cookie::name_t const& new_name) noexcept;

        /**
         * @brief rename the cookies that meed the condition
         * @param condition
         * @param new_name
         * @return
         */
        cookies& name(condition const& _condition,
                      cookie::name_t const& new_name) noexcept;
    };

} // namespace webpp

#endif // WEBPP_COOKIES_H
