// Created by moisrex on 5/27/20.

#ifndef WEBPP_HTTP_COOKIE_JAR_H
#define WEBPP_HTTP_COOKIE_JAR_H

#include "../../std/vector.hpp"
#include "./cookie.hpp"

#include <algorithm>
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
    template <Cookie CookieType, typename Allocator = stl::allocator<CookieType>>
    struct basic_cookie_jar : public stl::vector<CookieType, Allocator> {

      public:
        using cookie_type = CookieType;
        using string_type = typename cookie_type::string_type;

      private:
        using super = stl::vector<cookie_type, Allocator>;

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
                return trim_copy(a.name()) < trim_copy(b.name()) || a.domain() < b.domain();
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


} // namespace webpp

#endif // WEBPP_HTTP_COOKIE_JAR_H
