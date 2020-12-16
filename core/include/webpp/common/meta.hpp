// Created by moisrex on 7/20/19.

#ifndef WEBPP_META_H
#define WEBPP_META_H

#if __cplusplus >= 199711L
#    define CXX98 1
#endif

#if __cplusplus >= 201103L
#    define CXX11 1
#endif

#if __cplusplus >= 201402L
#    define CXX14 1
#endif

#if __cplusplus >= 201703L
#    define CXX17 1
#endif

#if __cplusplus >= 202002L
#    define CXX20 1
#endif

// the static_assert alternative based on this article:
// https://devblogs.microsoft.com/oldnewthing/20200311-00/?p=103553
// But this way is "type dependent"; meaning, you have to have a type (which you usually do).
// static_assert(!sizeof(decltype(input)*), "not able to do this");

namespace webpp::details {
    template <bool err>
    struct constexpr_assert_type {
        constexpr void operator()(auto) const noexcept {}
    };

    template <>
    struct constexpr_assert_type<false> {
        constexpr void operator()(auto obj) noexcept {
            obj.template operator()<false>();
        }
    };
}

#define constexpr_assert(ERR, StrLiteral) \
    webpp::details::constexpr_assert_type<bool(ERR)>{}(   \
      []<bool errv>() constexpr noexcept { static_assert(errv, StrLiteral); });

#define static_assert_false(BasedOn, StrLiteral) static_assert(!sizeof(BasedOn*), StrLiteral);

#endif // WEBPP_META_H
