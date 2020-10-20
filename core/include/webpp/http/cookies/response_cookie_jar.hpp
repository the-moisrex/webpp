// Created by moisrex on 8/19/20.

#ifndef WEBPP_RESPONSE_COOKIE_JAR_HPP
#define WEBPP_RESPONSE_COOKIE_JAR_HPP

#include "../../traits/traits.hpp"
#include "./cookie_jar.hpp"
#include "./response_cookie.hpp"

namespace webpp {


    template <typename Allocator = stl::allocator<char>,
              istl::String StringType =
                stl::basic_string<char, stl::char_traits<char>, to_alloc<Allocator, char>>>
    struct response_cookie_jar : public basic_cookie_jar<response_cookie<StringType>,
                                                         to_alloc<Allocator, response_cookie<StringType>>> {

        using allocator_type  = to_alloc<Allocator, response_cookie<StringType>>;
        using string_type     = StringType;
        using cookie_type     = response_cookie<string_type>;
        using cookie_jar_type = response_cookie_jar<Allocator, StringType>;
        using super           = basic_cookie_jar<cookie_type, allocator_type>;
        using iterator        = typename super::iterator;
        using const_iterator  = typename super::const_iterator;

        /**
         * @brief This function will make sure that the cookies are stay unique in the cookie jar
         */
        template <typename T>
        void make_it_unique(const_iterator const& dont_touch, T const& check) noexcept {
            for (auto it = super::begin(); it != super::end(); it++) {
                if (check(*it) && dont_touch != it && dont_touch->same_as(*it)) {
                    super::erase(it);
                    break;
                }
            }
        }

        iterator remove_const(const_iterator const& citer) noexcept {
            if (citer == super::cend())
                return super::end();
            auto it = super::begin();
            stl::advance(it, stl::distance(it, citer.base()));
            return it;
        }

        iterator remove_const(typename super::const_reverse_iterator const& citer) noexcept {
            if (citer == super::rcend())
                return super::end();
            auto it = super::begin();
            stl::advance(it, stl::distance(it, citer.base()));
            return it;
        }

        iterator remove_const(iterator& iter) noexcept {
            return iter;
        }

        iterator remove_const(typename super::reverse_iterator& iter) noexcept {
            if (iter == super::rend())
                return super::end();
            auto it = super::begin();
            stl::advance(it, stl::distance(it, iter.base()));
            return it;
        }

      public:
        template <typename... Args>
        constexpr response_cookie_jar(Args&&... args) noexcept(noexcept(super(stl::forward<Args>(args)...)))
          : super{stl::forward<Args>(args)...} {}

        template <typename Iter>
        explicit response_cookie_jar(Iter const& _start, Iter const& _end) noexcept : super{} {
            // todo: parse a list of strings which represent cookies
        }


#define DEFINE_METHOD(method_name)                                                       \
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


        DEFINE_METHOD(name)
        DEFINE_METHOD(value)
        DEFINE_METHOD(secure)
        DEFINE_METHOD(http_only)
        DEFINE_METHOD(encrypted)
        DEFINE_METHOD(prefix)
        DEFINE_METHOD(comment)
        DEFINE_METHOD(same_site)
        DEFINE_METHOD(expires)
        DEFINE_METHOD(max_age)
        DEFINE_METHOD(path)
        DEFINE_METHOD(domain)

        void fix() noexcept {
            // todo
            // 1. trim the names
            // 2. remove duplicates
        }
    };

} // namespace webpp

#endif // WEBPP_RESPONSE_COOKIE_JAR_HPP
