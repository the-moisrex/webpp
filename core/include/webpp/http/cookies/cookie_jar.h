// Created by moisrex on 5/27/20.

#ifndef WEBPP_HTTP_COOKIE_JAR_H
#define WEBPP_HTTP_COOKIE_JAR_H

#include "../../std/unordered_set.h"
#include "./cookie.h"

#include <functional>
#include <map>
#include <memory>

namespace webpp {

    /**
     * @brief The cookies class (it's a basic_cookie jar for cookies)
     * This class should only be created by header classes (owners of data)
     *
     * This class will be used by both response and request header classes; and
     * since the basic_cookie class cannot hold its data for a long time, this
     * class has to put new cookies into the header classes before the
     * string_views's in basic_cookie class go out of scope.
     */
    template <Cookie CookieType>
    struct basic_cookie_jar
      : public istl::unordered_set<typename CookieType::traits_type, CookieType,
                                   cookie_hash<CookieType>,
                                   cookie_equals<CookieType>> {

      public:
        using traits_type = typename CookieType::traits_type;
        using cookie_type = CookieType;
        using condition   = stl::function<bool(cookie_type const&)>;

      private:
        using super = istl::unordered_set<typename CookieType::traits_type,
                                          CookieType, cookie_hash<CookieType>,
                                          cookie_equals<CookieType>>;

      public:
        using super::unordered_set; // constructors

        typename super::const_iterator
        find(typename cookie_type::name_t const& name) const noexcept {
            return stl::find_if(super::cbegin(), super::cend(),
                                [&](auto const& a) {
                                    return a.name() == name;
                                });
        }

        typename super::const_iterator
        find(cookie_type const& c) const noexcept {
            return stl::find_if(super::cbegin(), super::cend(),
                                [&](auto const& a) {
                                    return a.same_as(c);
                                });
        }
    };

    template <Traits TraitsType>
    struct request_cookie_jar
      : public basic_cookie_jar<request_cookie<TraitsType>> {

        using string_view_type = typename TraitsType::string_view_type;

        /**
         * Parse Cookie header value
         * @param source
         */
        explicit request_cookie_jar(string_view_type const& source) noexcept {
            // todo
        }
    };


    template <Traits TraitsType>
    struct response_cookie_jar
      : public basic_cookie_jar<response_cookie<TraitsType>> {
      private:
        using super = basic_cookie_jar<response_cookie<TraitsType>>;

      public:
        using traits_type = TraitsType;
        using cookie_type = response_cookie<traits_type>;
        using condition   = typename super::condition;

      private:
        /**
         * @brief This function will make sure that the cookies are stay unique
         * in the cookie jar
         */
        template <typename T>
        void make_it_unique(typename super::const_iterator const& dont_touch,
                            T const& check) noexcept {
            for (auto it = super::begin(); it != super::end(); it++) {
                if (check(*it) && dont_touch != it &&
                    dont_touch->same_as(*it)) {
                    super::erase(it);
                    break;
                }
            }
        }

        /**
         * @brief change every basic_cookie if meats some condition
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
         */
        template <typename T>
        void change_if(typename cookie_type::name_t const& _name,
                       T const&                            change) noexcept {
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
        template <typename... Args>
        response_cookie_jar(Args&&... args)
          : super{stl::forward<Args>(args)...} {
        }

        template <typename Iter>
        explicit response_cookie_jar(Iter const& _start,
                                     Iter const& _end) noexcept
          : super{} {
            // todo: parse a list of strings which represent cookies
        }

        template <typename Name, class... Args>
        stl::pair<typename super::iterator, bool> emplace(Name&& name,
                                                          Args&&... args) {
            auto found = super::find(
              name); // we don't have a problem here because we are sure
                     // that the domain and the path are not the same
            // here. so we just look for the name
            if (found != super::cend())
                super::erase(found);
            return static_cast<super*>(this)->emplace(
              stl::forward<Name>(name), stl::forward<Args>(args)...);
        }

        template <typename Name, class... Args>
        typename super::iterator
        emplace_hint(typename super::const_iterator hint, Name&& name,
                     Args&&... args) noexcept {
            auto found = super::find(
              name); // we don't have a problem here because we are sure
            // that the domain and the path are not the same
            // here. so we just look for the name
            if (found != super::cend())
                super::erase(found);
            return static_cast<super*>(this)->emplace_hint(
              hint, stl::forward<Name>(name), stl::forward<Args>(args)...);
        }

        stl::pair<typename super::iterator, bool>
        insert(const typename super::value_type& value) {
            auto found = super::find(value);
            if (found != super::cend())
                super::erase(found);
            return static_cast<super*>(this)->insert(value);
        }


        stl::pair<typename super::iterator, bool>
        insert(typename super::value_type&& value) {
            auto found = super::find(value);
            if (found != super::cend())
                super::erase(found);
            return static_cast<super*>(this)->insert(stl::move(value));
        }

        typename super::iterator
        insert(typename super::const_iterator    hint,
               const typename super::value_type& value) {
            auto found = super::find(value);
            if (found != super::cend())
                super::erase(found);
            return static_cast<super*>(this)->insert(hint, value);
        }

        typename super::iterator insert(typename super::const_iterator hint,
                                        typename super::value_type&&   value) {
            auto found = super::find(value);
            if (found != super::cend())
                super::erase(found);
            return static_cast<super*>(this)->insert(hint, stl::move(value));
        }

        void insert(stl::initializer_list<typename super::value_type> ilist) {
            for (const auto& it : ilist) {
                auto found = super::find(it);
                if (found != super::cend())
                    super::erase(found);
            }
            static_cast<super*>(this)->insert(ilist);
        }

        template <class InputIt>
        void insert(InputIt first, InputIt last) {
            for (auto it = first; it != last;) {
                auto found = super::find(*it);
                if (found != super::cend())
                    super::erase(found);
                else
                    ++it;
            }
            return static_cast<super*>(this)->insert(first, last);
        }

        //        insert_return_type insert(node_type&& nh) {}
        //        iterator insert(const_iterator hint, node_type&& nh) {}


        /**
         * @brief Change ever basic_cookie's name to the specified value
         * @param _name
         * @return
         * @return
         * @return
         */
        auto& name(typename cookie_type::name_t const& _name) noexcept {
            if (auto first = super::begin(); first != super::end()) {
                first->_name = _name;
                super::erase(stl::next(first), super::end()); // remove the rest
            }
            return *this;
        }

        /**
         * @brief performing rename
         * @param old_name
         * @param new_name
         * @return
         */
        auto& name(typename cookie_type::name_t const& old_name,
                   typename cookie_type::name_t const& new_name) noexcept {
            super::erase(super::find(new_name));
            if (auto found = super::find(old_name); found != super::end())
                found->_name = new_name;
            return *this;
        }

        auto& name(typename super::const_iterator const& it,
                   typename cookie_type::name_t&&        new_name) noexcept {
            super::erase(super::find(new_name));
            it->_name = stl::move(new_name);
            return *this;
        }

        auto& name(typename super::const_iterator const& it,
                   typename cookie_type::name_t const&   new_name) noexcept {
            return name(it, (typename cookie_type::name_t)(new_name));
        }

        /**
         * @brief rename the cookies that meed the condition
         * @param condition
         * @param new_name
         * @return
         */
        auto& name(condition const&                    _condition,
                   typename cookie_type::name_t const& new_name) noexcept {
            super::erase(super::find(new_name));
            if (auto found =
                  stl::find_if(super::begin(), super::end(), _condition);
                found != super::end()) {
                found->_name = new_name;
            }
            return *this;
        }
        /**
         * @brief mark all cookies as encrypted
         * @param _encrypted
         * @return
         */
        auto& encrypted(
          typename cookie_type::encrypted_t const& _encrypted) noexcept {
            for (auto& c : *this)
                c._encrypted = _encrypted;
            return *this;
        }

        auto& encrypted(
          condition const&                         _condition,
          typename cookie_type::encrypted_t const& _encrypted) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_encrypted = _encrypted;
            });
            return *this;
        }


        auto& encrypted(
          typename cookie_type::name_t const&      _name,
          typename cookie_type::encrypted_t const& _encrypted) noexcept {
            change_if(_name, [&](auto& it) {
                it->_encrypted = _encrypted;
            });
            return *this;
        }

        auto& encrypted(typename super::const_iterator const& it,
                        typename cookie_type::encrypted_t _encrypted) noexcept {
            it->_encrypted = _encrypted;
            return *this;
        }

        /**
         * @brief mark all cookies as secure
         * @param _secure
         * @return
         */
        auto& secure(typename cookie_type::secure_t const& _secure) noexcept {
            for (auto& c : *this)
                c._secure = _secure;
            return *this;
        }

        auto& secure(condition const&                      _condition,
                     typename cookie_type::secure_t const& _secure) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_secure = _secure;
            });
            return *this;
        }


        auto& secure(typename cookie_type::name_t const&   _name,
                     typename cookie_type::secure_t const& _secure) noexcept {
            change_if(_name, [&](auto& it) {
                it->_secure = _secure;
            });
            return *this;
        }


        auto& secure(typename super::const_iterator const& it,
                     typename cookie_type::secure_t        _secure) noexcept {
            it->_secure = _secure;
            return *this;
        }

        /**
         * @brief make every basic_cookie host_only
         * @param _host_only
         * @return
         */
        auto& host_only(
          typename cookie_type::host_only_t const& _host_only) noexcept {
            for (auto& c : *this)
                c._host_only = _host_only;
            return *this;
        }


        auto& host_only(
          condition const&                         _condition,
          typename cookie_type::host_only_t const& _host_only) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_host_only = _host_only;
            });
            return *this;
        }

        auto& host_only(
          typename cookie_type::name_t const&      _name,
          typename cookie_type::host_only_t const& _host_only) noexcept {
            change_if(_name, [&](auto& it) {
                it->_host_only = _host_only;
            });
            return *this;
        }

        auto& host_only(typename super::const_iterator const& it,
                        typename cookie_type::host_only_t _host_only) noexcept {
            it->_host_only = _host_only;
            return *this;
        }

        /**
         * @brief enable basic_cookie name prefix in all cookies
         * @param _prefix
         * @return
         */
        auto& prefix(typename cookie_type::prefix_t const& _prefix) noexcept {
            for (auto& c : *this)
                c._prefix = _prefix;
            return *this;
        }

        auto& prefix(typename cookie_type::name_t const&   _name,
                     typename cookie_type::prefix_t const& _prefix) noexcept {
            change_if(_name, [&](auto& it) {
                it->_prefix = _prefix;
            });
            return *this;
        }

        auto& prefix(condition const&                      _condition,
                     typename cookie_type::prefix_t const& _prefix) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_prefix = _prefix;
            });
            return *this;
        }

        auto& prefix(typename super::const_iterator const& it,
                     typename cookie_type::prefix_t        _prefix) noexcept {
            it->_prefix = _prefix;
            return *this;
        }

        /**
         * @brief set a comment for every basic_cookie
         * @param _comment
         * @return
         */
        auto&
        comment(typename cookie_type::comment_t const& _comment) noexcept {
            for (auto& c : *this)
                c._comment = _comment;
            return *this;
        }

        auto&
        comment(condition const&                       _condition,
                typename cookie_type::comment_t const& _comment) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_comment = _comment;
            });
            return *this;
        }

        auto&
        comment(typename cookie_type::name_t const&    _name,
                typename cookie_type::comment_t const& _comment) noexcept {
            change_if(_name, [&](auto& it) {
                it->_comment = _comment;
            });
            return *this;
        }

        auto& comment(typename super::const_iterator const& it,
                      typename cookie_type::comment_t&&     _comment) noexcept {
            it->_comment = stl::move(_comment);
            return *this;
        }

        auto&
        comment(typename super::const_iterator const&  it,
                typename cookie_type::comment_t const& _comment) noexcept {
            return comment(it, (typename cookie_type::comment_t){_comment});
        }

        /**
         * @brief make same_site enabled for every basic_cookie
         * @param _same_site
         * @return
         */
        auto& same_site(
          typename cookie_type::same_site_t const& _same_site) noexcept {
            for (auto& c : *this)
                c._same_site = _same_site;
            return *this;
        }

        auto& same_site(
          typename cookie_type::name_t const&      _name,
          typename cookie_type::same_site_t const& _same_site) noexcept {
            change_if(_name, [&](auto& it) {
                it->_same_site = _same_site;
            });
            return *this;
        }

        auto& same_site(
          condition const&                         _condition,
          typename cookie_type::same_site_t const& _same_site) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_same_site = _same_site;
            });
            return *this;
        }

        auto& same_site(typename super::const_iterator const& it,
                        typename cookie_type::same_site_t _same_site) noexcept {
            it->_same_site = _same_site;
            return *this;
        }

        /**
         * @brief set the same expiration date for every basic_cookie
         * @param _expires
         * @return
         */
        auto& expires(typename cookie_type::date_t const& _expires) noexcept {
            for (auto& c : *this)
                c._expires = _expires;
            return *this;
        }

        auto& expires(typename cookie_type::name_t const& _name,
                      typename cookie_type::date_t const& _expires) noexcept {
            change_if(_name, [&](auto& it) {
                it->_expires = _expires;
            });
            return *this;
        }

        auto& expires(condition const&                    _condition,
                      typename cookie_type::date_t const& _expires) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_expires = _expires;
            });
            return *this;
        }

        auto& expires(typename super::const_iterator const& it,
                      typename cookie_type::date_t&&        _expires) noexcept {
            it->_expires = _expires;
            return *this;
        }

        auto& expires(typename super::const_iterator const& it,
                      typename cookie_type::date_t const&   _expires) noexcept {
            return expires(it, (typename cookie_type::date_t)(_expires));
        }

        /**
         * @brief set the max_age for every basic_cookie
         * @param _max_age
         * @return
         */
        auto&
        max_age(typename cookie_type::max_age_t const& _max_age) noexcept {
            for (auto& c : *this)
                c._max_age = _max_age;
            return *this;
        }

        auto&
        max_age(typename cookie_type::name_t const&    _name,
                typename cookie_type::max_age_t const& _max_age) noexcept {
            change_if(_name, [&](auto& it) {
                it->_max_age = _max_age;
            });
            return *this;
        }

        auto&
        max_age(condition const&                       _condition,
                typename cookie_type::max_age_t const& _max_age) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_max_age = _max_age;
            });
            return *this;
        }

        auto& max_age(typename super::const_iterator const& it,
                      typename cookie_type::max_age_t       _max_age) noexcept {
            it->_max_age = _max_age;
            return *this;
        }

        auto& value(typename cookie_type::value_t const& _value) noexcept {
            for (auto& c : *this)
                c._value = _value;
            return *this;
        }

        auto& value(typename cookie_type::name_t const&  _name,
                    typename cookie_type::value_t const& _value) noexcept {
            change_if(_name, [&](auto& it) {
                it->_value = _value;
            });
            return *this;
        }

        auto& value(typename super::const_iterator const& it,
                    typename cookie_type::value_t const&  _value) noexcept {
            return value(it, (typename cookie_type::value_t)(_value));
        }

        auto& value(typename super::const_iterator const& it,
                    typename cookie_type::value_t&&       _value) noexcept {
            it->_value = stl::move(_value);
            return *this;
        }

        auto& value(condition const&                     _condition,
                    typename cookie_type::value_t const& _value) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_value = _value;
            });
            return *this;
        }

        auto& path(typename cookie_type::path_t const& _path) noexcept {
            change_all([&](auto& it) {
                it->_path = _path;
                make_it_unique(it, [&](auto const& c) {
                    return c._path == _path;
                });
            });
            return *this;
        }

        auto& path(typename cookie_type::name_t const& _name,
                   typename cookie_type::path_t const& _path) noexcept {
            change_if(_name, [&](auto& it) {
                it->_path = _path;
                make_it_unique(it, [&](auto const& c) {
                    return c._path == _path;
                });
            });
            return *this;
        }

        auto& path(condition const&                    _condition,
                   typename cookie_type::path_t const& _path) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_path = _path;
                make_it_unique(it, [&](auto const& c) {
                    return c._path == _path;
                });
            });
            return *this;
        }

        auto& path(typename super::const_iterator const& it,
                   typename cookie_type::path_t&&        _path) noexcept {
            it->_path = _path;
            make_it_unique(it, [&](auto const& c) {
                return c._path == it->_path;
            });
            return *this;
        }

        auto& path(typename super::const_iterator const& it,
                   typename cookie_type::path_t const&   _path) noexcept {
            return path(it, (typename cookie_type::path_t)(_path));
        }

        /**
         * @brief change every basic_cookie's domain to the specified value
         * @param _domain
         * @return
         */
        auto& domain(typename cookie_type::domain_t const& _domain) noexcept {
            change_all([&](auto& it) {
                it->_domain = _domain;
                make_it_unique(it, [&](auto const& c) {
                    return c._domain == _domain;
                });
            });
            return *this;
        }

        auto&
        domain(typename cookie_type::name_t const&   _name,
               typename cookie_type::domain_t const& new_domain) noexcept {
            change_if(_name, [&](auto& it) {
                it->_domain = new_domain;
                make_it_unique(it, [&](auto const& c) {
                    return c._domain == new_domain;
                });
            });
            return *this;
        }

        auto& domain(typename super::const_iterator const& it,
                     typename cookie_type::domain_t&& new_domain) noexcept {
            it->_domain = stl::move(new_domain);
            make_it_unique(it, [&](auto const& c) {
                return c._domain == it->_domain;
            });
            return *this;
        }

        auto&
        domain(typename super::const_iterator const& it,
               typename cookie_type::domain_t const& new_domain) noexcept {
            return domain(it, (typename cookie_type::domain_t)(new_domain));
        }

        auto&
        domain(condition const&                      _condition,
               typename cookie_type::domain_t const& new_domain) noexcept {
            change_if(_condition, [&](auto& it) {
                it->_domain = new_domain;
            });
            return *this;
        }
    };

} // namespace webpp

#endif // WEBPP_HTTP_COOKIE_JAR_H
