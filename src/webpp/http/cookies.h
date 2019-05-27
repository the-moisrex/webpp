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
 *    [ ] Move the definitions of the cookie jar into cookies.cpp file
 *    [ ] Consider renaming "cookies" to "cookie_jar"
 *    [ ] Add doxygen documentations/comments to the declations
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
        mutable max_age_t _max_age;
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

        inline cookie& comment(std::string const& __comment) noexcept {
            _comment = __comment;
            return *this;
        }

        inline cookie& comment(std::string&& __comment) noexcept {
            _comment = std::move(__comment);
            return *this;
        }

        inline cookie& domain(std::string const& __domain) noexcept {
            _domain = __domain;
            return *this;
        }
        inline cookie& domain(std::string&& __domain) noexcept {
            _domain = std::move(__domain);
            return *this;
        }

        inline cookie& path(std::string const& __path) noexcept {
            _path = __path;
            return *this;
        }

        inline cookie& path(std::string&& __path) noexcept {
            _path = std::move(__path);
            return *this;
        }

        inline cookie& max_age(decltype(_max_age)&& __max_age) noexcept {
            _max_age = std::move(__max_age);
            return *this;
        }

        inline cookie& max_age(decltype(_max_age) const& __max_age) noexcept {
            _max_age = __max_age;
            return *this;
        }

        inline cookie& prefix(decltype(_prefix) __prefix) noexcept {
            _prefix = std::move(__prefix);
            return *this;
        }

        inline cookie& same_site(decltype(_same_site) __same_site) noexcept {
            _same_site = std::move(__same_site);
            return *this;
        }

        inline cookie& secure(decltype(_secure) __secure) noexcept {
            _secure = std::move(__secure);
            return *this;
        }

        inline cookie& host_only(decltype(_host_only) __host_only) noexcept {
            _host_only = std::move(__host_only);
            return *this;
        }

        inline bool remove() const noexcept {
            using namespace std::chrono;
            return *_expires < system_clock::now();
        }

        inline cookie& remove(bool __remove) noexcept {
            using namespace std::chrono;
            if (__remove) {
                // set the expire date one year before now:
                expires(system_clock::now() -
                        duration<int, std::ratio<60 * 60 * 24 * 365>>(1));
            } else if (remove()) {
                // set the expire date one year from now:
                expires(system_clock::now() +
                        duration<int, std::ratio<60 * 60 * 24 * 365>>(1));
            }
            // remove max-age if it exists because we're going with expires
            max_age(0);
            return *this;
        }

        inline cookie& expires(date_t __expires) noexcept {
            _expires.reset(new date_t{std::move(__expires)});
            return *this;
        }

        /**
         * @brief sets exipiration time relative to now.
         */
        template <typename D, typename T>
        inline cookie&
        expires_in(std::chrono::duration<D, T> const& __dur) noexcept {
            _expires.reset(
                new date_t{std::chrono::system_clock::now() + __dur});
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
        inline cookie& encrypted(decltype(_encrypted) __encrypted) noexcept {
            _encrypted = __encrypted;
            return *this;
        }

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
                    std::function<bool(cookie const&)> const& check) noexcept {
            for (auto it = begin(); it != end(); it++)
                if (check(*it) && dont_touch != it &&
                    dont_touch->same_as(*it)) {
                    erase(it);
                    break;
                }
        }

        /**
         * @brief change every cookie if meats some condition
         * @param if_statement
         * @param change
         */
        void change_if(condition const& if_statement,
                       std::function<void(iterator&)> const& change) noexcept {
            for (auto it = begin(); it != end(); it++)
                if (if_statement(*it))
                    change(it);
        }

        void change_if(cookie::name_t const& _name,
                       std::function<void(iterator&)> const& change) noexcept {
            for (auto it = begin(); it != end(); it++)
                if (it->_name == _name)
                    change(it);
        }

        void change_all(std::function<void(iterator&)> const& change) noexcept {
            for (auto it = begin(); it != end(); it++)
                change(it);
        }

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

        std::pair<iterator, bool> insert(const value_type& value) {
            auto found = find(value);
            if (found != cend())
                erase(found);
            return static_cast<super*>(this)->insert(value);
        }

        std::pair<iterator, bool> insert(value_type&& value) {
            auto found = find(value);
            if (found != cend())
                erase(found);
            return static_cast<super*>(this)->insert(std::move(value));
        }

        iterator insert(const_iterator hint, const value_type& value) {
            auto found = find(value);
            if (found != cend())
                erase(found);
            return static_cast<super*>(this)->insert(hint, value);
        }

        iterator insert(const_iterator hint, value_type&& value) {
            auto found = find(value);
            if (found != cend())
                erase(found);
            return static_cast<super*>(this)->insert(hint, std::move(value));
        }

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

        void insert(std::initializer_list<value_type> ilist) {
            for (auto it = ilist.begin(); it != ilist.end(); it++) {
                auto found = find(*it);
                if (found != cend())
                    erase(found);
            }
            return static_cast<super*>(this)->insert(ilist);
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
        cookies& encrypted(cookie::encrypted_t const& _encrypted) noexcept {
            for (auto& c : *this)
                c._encrypted = _encrypted;
            return *this;
        }
        cookies& encrypted(condition const& _condition,
                           cookie::encrypted_t const& _encrypted) noexcept {
            change_if(_condition,
                      [&](auto& it) { it->_encrypted = _encrypted; });
            return *this;
        }
        cookies& encrypted(cookie::name_t const& _name,
                           cookie::encrypted_t const& _encrypted) noexcept {
            change_if(_name, [&](auto& it) { it->_encrypted = _encrypted; });
            return *this;
        }
        cookies& encrypted(const_iterator const& it,
                           cookie::encrypted_t _encrypted) noexcept {
            it->_encrypted = std::move(_encrypted);
            return *this;
        }

        /**
         * @brief mark all cookies as secure
         * @param _secure
         * @return
         */
        cookies& secure(cookie::secure_t const& _secure) noexcept {
            for (auto& c : *this)
                c._secure = _secure;
            return *this;
        }
        cookies& secure(condition const& _condition,
                        cookie::secure_t const& _secure) noexcept {
            change_if(_condition, [&](auto& it) { it->_secure = _secure; });
            return *this;
        }
        cookies& secure(cookie::name_t const& _name,
                        cookie::secure_t const& _secure) noexcept {
            change_if(_name, [&](auto& it) { it->_secure = _secure; });
            return *this;
        }
        cookies& secure(const_iterator const& it,
                        cookie::secure_t _secure) noexcept {
            it->_secure = std::move(_secure);
            return *this;
        }

        /**
         * @brief make every cookie host_only
         * @param _host_only
         * @return
         */
        cookies& host_only(cookie::host_only_t const& _host_only) noexcept {
            for (auto& c : *this)
                c._host_only = _host_only;
            return *this;
        }

        cookies& host_only(condition const& _condition,
                           cookie::host_only_t const& _host_only) noexcept {
            change_if(_condition,
                      [&](auto& it) { it->_host_only = _host_only; });
            return *this;
        }

        cookies& host_only(cookie::name_t const& _name,
                           cookie::host_only_t const& _host_only) noexcept {
            change_if(_name, [&](auto& it) { it->_host_only = _host_only; });
            return *this;
        }

        cookies& host_only(const_iterator const& it,
                           cookie::host_only_t _host_only) noexcept {
            it->_host_only = std::move(_host_only);
            return *this;
        }

        /**
         * @brief enable cookie name prefix in all cookies
         * @param _prefix
         * @return
         */
        cookies& prefix(cookie::prefix_t const& _prefix) noexcept {
            for (auto& c : *this)
                c._prefix = _prefix;
            return *this;
        }

        cookies& prefix(cookie::name_t const& _name,
                        cookie::prefix_t const& _prefix) noexcept {
            change_if(_name, [&](auto& it) { it->_prefix = _prefix; });
            return *this;
        }

        cookies& prefix(condition const& _condition,
                        cookie::prefix_t const& _prefix) noexcept {
            change_if(_condition, [&](auto& it) { it->_prefix = _prefix; });
            return *this;
        }

        cookies& prefix(const_iterator const& it,
                        cookie::prefix_t _prefix) noexcept {
            it->_prefix = std::move(_prefix);
            return *this;
        }

        /**
         * @brief set a comment for every cookie
         * @param _comment
         * @return
         */
        cookies& comment(cookie::comment_t const& _comment) noexcept {
            for (auto& c : *this)
                c._comment = _comment;
            return *this;
        }
        cookies& comment(condition const& _condition,
                         cookie::comment_t const& _comment) noexcept {
            change_if(_condition, [&](auto& it) { it->_comment = _comment; });
            return *this;
        }
        cookies& comment(cookie::name_t const& _name,
                         cookie::comment_t const& _comment) noexcept {
            change_if(_name, [&](auto& it) { it->_comment = _comment; });
            return *this;
        }
        cookies& comment(const_iterator const& it,
                         cookie::comment_t&& _comment) noexcept {
            it->_comment = std::move(_comment);
            return *this;
        }
        cookies& comment(const_iterator const& it,
                         cookie::comment_t const& _comment) noexcept {
            return comment(it, cookie::comment_t{_comment});
        }

        /**
         * @brief make same_site enabled for every cookie
         * @param _same_site
         * @return
         */
        cookies& same_site(cookie::same_site_t const& _same_site) noexcept {
            for (auto& c : *this)
                c._same_site = _same_site;
            return *this;
        }
        cookies& same_site(cookie::name_t const& _name,
                           cookie::same_site_t const& _same_site) noexcept {
            change_if(_name, [&](auto& it) { it->_same_site = _same_site; });
            return *this;
        }
        cookies& same_site(condition const& _condition,
                           cookie::same_site_t const& _same_site) noexcept {
            change_if(_condition,
                      [&](auto& it) { it->_same_site = _same_site; });
            return *this;
        }
        cookies& same_site(const_iterator const& it,
                           cookie::same_site_t _same_site) noexcept {
            it->_same_site = std::move(_same_site);
            return *this;
        }

        /**
         * @brief set the same expiriration date for every cookie
         * @param _expires
         * @return
         */
        cookies& expires(cookie::date_t const& _expires) noexcept {
            for (auto& c : *this)
                c._expires.reset(new cookie::date_t{_expires});
            return *this;
        }

        cookies& expires(cookie::name_t const& _name,
                         cookie::date_t const& _expires) noexcept {
            change_if(_name, [&](auto& it) {
                it->_expires.reset(new cookie::date_t{_expires});
            });
            return *this;
        }

        cookies& expires(condition const& _condition,
                         cookie::date_t const& _expires) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_expires.reset(new cookie::date_t{_expires});
            });
            return *this;
        }

        cookies& expires(const_iterator const& it,
                         cookie::date_t&& _expires) noexcept {
            it->_expires.reset(new cookie::date_t{_expires});
            return *this;
        }

        cookies& expires(const_iterator const& it,
                         cookie::date_t const& _expires) noexcept {
            return expires(it, cookie::date_t(_expires));
        }

        /**
         * @brief set the max_age for every cookie
         * @param _max_age
         * @return
         */
        cookies& max_age(cookie::max_age_t const& _max_age) noexcept {
            for (auto& c : *this)
                c._max_age = _max_age;
            return *this;
        }
        cookies& max_age(cookie::name_t const& _name,
                         cookie::max_age_t const& _max_age) noexcept {
            change_if(_name, [&](auto& it) { it->_max_age = _max_age; });
            return *this;
        }
        cookies& max_age(condition const& _condition,
                         cookie::max_age_t const& _max_age) noexcept {
            change_if(_condition, [&](auto& it) { it->_max_age = _max_age; });
            return *this;
        }
        cookies& max_age(const_iterator const& it,
                         cookie::max_age_t&& _max_age) noexcept {
            it->_max_age = std::move(_max_age);
            return *this;
        }
        cookies& max_age(const_iterator const& it,
                         cookie::max_age_t const& _max_age) noexcept {
            return max_age(it, cookie::max_age_t(_max_age));
        }

        cookies& value(cookie::value_t const& _value) noexcept {
            for (auto& c : *this)
                c._value = _value;
            return *this;
        }

        cookies& value(cookie::name_t const& _name,
                       cookie::value_t const& _value) noexcept {
            change_if(_name, [&](auto& it) { it->_value = _value; });
            return *this;
        }
        cookies& value(const_iterator const& it,
                       cookie::value_t const& _value) noexcept {
            return value(it, cookie::value_t(_value));
        }
        cookies& value(const_iterator const& it,
                       cookie::value_t&& _value) noexcept {
            it->_value = std::move(_value);
            return *this;
        }

        cookies& value(condition const& _condition,
                       cookie::value_t const& _value) noexcept {
            change_if(_condition, [&](auto& it) { it->_value = _value; });
            return *this;
        }

        cookies& path(cookie::path_t const& _path) noexcept {
            change_all([&](auto& it) {
                it->_path = _path;
                make_unique(it,
                            [&](auto const& c) { return c._path == _path; });
            });
            return *this;
        }

        cookies& path(cookie::name_t const& _name,
                      cookie::path_t const& _path) noexcept {
            change_if(_name, [&](auto& it) {
                it->_path = _path;
                make_unique(it,
                            [&](auto const& c) { return c._path == _path; });
            });
            return *this;
        }

        cookies& path(condition const& _condition,
                      cookie::path_t const& _path) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_path = _path;
                make_unique(it,
                            [&](auto const& c) { return c._path == _path; });
            });
            return *this;
        }
        cookies& path(const_iterator const& it,
                      cookie::path_t&& _path) noexcept {
            it->_path = _path;
            make_unique(it,
                        [&](auto const& c) { return c._path == it->_path; });
            return *this;
        }
        cookies& path(const_iterator const& it,
                      cookie::path_t const& _path) noexcept {
            return path(it, cookie::path_t(_path));
        }
        /**
         * @brief change every cookie's domain to the specified value
         * @param _domain
         * @return
         */
        cookies& domain(cookie::domain_t const& _domain) noexcept {
            change_all([&](auto& it) {
                it->_domain = _domain;
                make_unique(
                    it, [&](auto const& c) { return c._domain == _domain; });
            });
            return *this;
        }

        cookies& domain(cookie::name_t const& _name,
                        cookie::domain_t const& new_domain) noexcept {
            change_if(_name, [&](auto& it) {
                it->_domain = new_domain;
                make_unique(
                    it, [&](auto const& c) { return c._domain == new_domain; });
            });
            return *this;
        }

        cookies& domain(const_iterator const& it,
                        cookie::domain_t&& new_domain) noexcept {
            it->_domain = std::move(new_domain);
            make_unique(
                it, [&](auto const& c) { return c._domain == it->_domain; });
            return *this;
        }

        cookies& domain(const_iterator const& it,
                        cookie::domain_t const& new_domain) noexcept {
            return domain(it, cookie::domain_t(new_domain));
        }

        cookies& domain(condition const& _condition,
                        cookie::domain_t const& new_domain) noexcept {
            change_if(_condition, [&](auto& it) { it->_domain = new_domain; });
            return *this;
        }

        /**
         * @brief Change ever cookie's name to the specified value
         * @param _name
         * @return
         */
        cookies& name(cookie::name_t const& _name) noexcept {
            change_all([&](auto& it) {
                it->_name = _name;
                make_unique(it,
                            [&](auto const& c) { return c._name == _name; });
            });
            return *this;
        }

        /**
         * @brief performing rename
         * @param old_name
         * @param new_name
         * @return
         */
        cookies& name(cookie::name_t const& old_name,
                      cookie::name_t const& new_name) noexcept {
            change_if(old_name, [&](auto& it) {
                it->_name = new_name;
                make_unique(it,
                            [&](auto const& c) { return c._name == new_name; });
            });
            return *this;
        }

        cookies& name(const_iterator const& it,
                      cookie::name_t&& new_name) noexcept {
            it->_name = std::move(new_name);
            make_unique(it,
                        [&](auto const& c) { return c._name == it->_name; });
            return *this;
        }

        cookies& name(const_iterator const& it,
                      cookie::name_t const& new_name) noexcept {
            return name(it, cookie::name_t(new_name));
        }

        /**
         * @brief rename the cookies that meed the condition
         * @param condition
         * @param new_name
         * @return
         */
        cookies& name(condition const& _condition,
                      cookie::name_t const& new_name) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_name = new_name;
                make_unique(it,
                            [&](auto const& c) { return c._name == new_name; });
            });
            return *this;
        }
    };

} // namespace webpp

#endif // WEBPP_COOKIES_H
