// Created by moisrex on 5/27/20.

#ifndef WEBPP_HTTP_COOKIE_JAR_H
#define WEBPP_HTTP_COOKIE_JAR_H

#include "../../std/vector.hpp"
#include "./cookie.hpp"

#include <algorithm>
#include <functional>
#include <map>
#include <memory>

namespace webpp {

    template <typename T, typename CookieType>
    concept cookie_condition = stl::is_invocable_r_v<bool, T, CookieType const&>;

    template <typename T, typename CookieType>
    concept cookie_changer = stl::is_invocable_v<T, CookieType&>;

    template <typename T, typename CookieJarType>
    concept cookie_iterator =
      stl::same_as<stl::decay_t<T>, stl::decay_t<typename CookieJarType::iterator>> ||
      stl::same_as<stl::decay_t<T>, stl::decay_t<typename CookieJarType::const_iterator>> ||
      stl::same_as<stl::decay_t<T>, stl::decay_t<typename CookieJarType::reverse_iterator>> ||
      stl::same_as<stl::decay_t<T>, stl::decay_t<typename CookieJarType::const_reverse_iterator>>;

    /**
     * @brief The cookies class (it's a basic_cookie jar for cookies)
     * This class should only be created by header classes (owners of data)
     *
     * This class will be used by both response and request header classes; and
     * since the basic_cookie class cannot hold its data for a long time, this
     * class has to put new cookies into the header classes before the
     * string_views's in basic_cookie class go out of scope.
     *
     *
     * istl::unordered_set<typename CookieType::traits_type, CookieType,
     *                              cookie_hash<CookieType>,
     *                              cookie_equals<CookieType>>
     *
     */
    template <Cookie CookieType>
    struct basic_cookie_jar : public istl::vector<typename CookieType::traits_type, CookieType> {

      public:
        using traits_type      = typename CookieType::traits_type;
        using cookie_type      = CookieType;
        using string_type      = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;

      private:
        using super = istl::vector<traits_type, cookie_type>;

      public:
        template <typename... Args>
        basic_cookie_jar(Args&&... args) : super{std::forward<Args>(args)...} {}

        typename super::const_iterator find(typename cookie_type::name_t const& name) const noexcept {
            return stl::find_if(super::cbegin(), super::cend(), [&](auto const& a) {
                return a.name() == name;
            });
        }

        typename super::const_iterator find(cookie_type const& c) const noexcept {
            return stl::find_if(super::cbegin(), super::cend(), [&](auto const& a) {
                return a.same_as(c);
            });
        }

        void remove_duplicates() noexcept {
            constexpr auto predicate = [](auto const& a, auto const& b) {
                return trim_copy(a.name()) < trim_copy(b.name()) ||
                       a.domain() < b.domain();
            };
            stl::sort(super::begin(), super::end(), predicate);
            super::erase(
              super::begin(),
              super::begin() +
                stl::distance(super::begin(),
                              stl::unique(super::rbegin(), super::rend(), [](auto const& a, auto const& b) {
                                  return a.same_as(b);
                              }).base()));
        }
    };

    template <Traits TraitsType>
    struct request_cookie_jar : public basic_cookie_jar<request_cookie<TraitsType>> {

        static constexpr bool is_mutable = false;

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
    struct response_cookie_jar : public basic_cookie_jar<response_cookie<TraitsType>> {
      private:
        using super = basic_cookie_jar<response_cookie<TraitsType>>;

      public:
        using traits_type     = TraitsType;
        using cookie_type     = response_cookie<traits_type>;
        using cookie_jar_type = response_cookie_jar<traits_type>;

        static constexpr bool is_mutable = true;

      private:
        /**
         * @brief This function will make sure that the cookies are stay unique
         * in the cookie jar
         */
        template <typename T>
        void make_it_unique(typename super::const_iterator const& dont_touch, T const& check) noexcept {
            for (auto it = super::begin(); it != super::end(); it++) {
                if (check(*it) && dont_touch != it && dont_touch->same_as(*it)) {
                    super::erase(it);
                    break;
                }
            }
        }


        typename super::iterator remove_const(typename super::const_iterator const& citer) noexcept {
            if (citer == super::cend())
                return super::end();
            auto it = super::begin();
            stl::advance(it, stl::distance(it, citer.base()));
            return it;
        }

        typename super::iterator remove_const(typename super::const_reverse_iterator const& citer) noexcept {
            if (citer == super::rcend())
                return super::end();
            auto it = super::begin();
            stl::advance(it, stl::distance(it, citer.base()));
            return it;
        }

        typename super::iterator remove_const(typename super::iterator& iter) noexcept {
            return iter;
        }

        typename super::iterator remove_const(typename super::reverse_iterator& iter) noexcept {
            if (iter == super::rend())
                return super::end();
            auto it = super::begin();
            stl::advance(it, stl::distance(it, iter.base()));
            return it;
        }

      public:
        template <typename... Args>
        response_cookie_jar(Args&&... args) : super{stl::forward<Args>(args)...} {}

        template <typename Iter>
        explicit response_cookie_jar(Iter const& _start, Iter const& _end) noexcept : super{} {
            // todo: parse a list of strings which represent cookies
        }


#define define_method(method_name)                                                       \
    auto& method_name(typename cookie_type::method_name##_t const& old_value,            \
                      typename cookie_type::method_name##_t const& new_value) noexcept { \
        for (auto& cookie : *this)                                                       \
            if (cookie.method_name() == old_value)                                       \
                cookie.method_name(new_value);                                           \
        return *this;                                                                    \
    }                                                                                    \
                                                                                         \
    auto& method_name(typename cookie_type::method_name##_t const& old_value,            \
                      cookie_changer<cookie_type> auto const&      _changer) noexcept {       \
        for (auto& cookie : *this)                                                       \
            if (cookie.method_name() == old_value)                                       \
                _changer(cookie);                                                        \
        return *this;                                                                    \
    }                                                                                    \
                                                                                         \
    auto& method_name(cookie_condition<cookie_type> auto const& _condition,              \
                      cookie_changer<cookie_type> auto const&   _changer) noexcept {       \
        for (auto& cookie : *this)                                                       \
            if (_condition(cookie))                                                      \
                _changer(cookie);                                                        \
        return *this;                                                                    \
    }                                                                                    \
                                                                                         \
    auto& method_name(cookie_condition<cookie_type> auto const&    _condition,           \
                      typename cookie_type::method_name##_t const& new_value) noexcept { \
        for (auto& cookie : *this)                                                       \
            if (_condition(cookie))                                                      \
                cookie.method_name(new_value);                                           \
        return *this;                                                                    \
    }                                                                                    \
                                                                                         \
    auto& method_name(cookie_iterator<cookie_jar_type> auto const& it,                   \
                      typename cookie_type::method_name##_t const& new_value) noexcept { \
        if (auto iter = remove_const(it); iter != super::cend())                         \
            iter->method_name(new_value);                                                \
        return *this;                                                                    \
    }                                                                                    \
                                                                                         \
    auto& method_name(cookie_iterator<cookie_jar_type> auto&       it,                   \
                      typename cookie_type::method_name##_t const& new_value) noexcept { \
        if (auto iter = remove_const(it); iter != super::cend())                         \
            iter->method_name(new_value);                                                \
        return *this;                                                                    \
    }                                                                                    \
                                                                                         \
    auto& method_name(typename cookie_type::method_name##_t const& new_value) noexcept { \
        for (auto& cookie : *this)                                                       \
            cookie.method_name(new_value);                                               \
        return *this;                                                                    \
    }


        define_method(name)

          define_method(value)

            define_method(secure)

              define_method(host_only)

                define_method(encrypted)

                  define_method(prefix)

                    define_method(comment)

                      define_method(same_site)

                        define_method(expires)

                          define_method(max_age)

                            define_method(path)

                              define_method(domain)


                                void fix() noexcept {
            // todo
            // 1. trim the names
            // 2. remove duplicates
        }

    }; // namespace webpp

} // namespace webpp

#endif // WEBPP_HTTP_COOKIE_JAR_H
