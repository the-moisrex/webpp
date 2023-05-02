// Created by moisrex on 7/20/19.

#ifndef WEBPP_META_HPP
#define WEBPP_META_HPP

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

#if __cplusplus > 202002L // todo: change the release date of this, when it's known
#    define CXX23 1
#endif

// the static_assert alternative based on this article:
// https://devblogs.microsoft.com/oldnewthing/20200311-00/?p=103553
// https://devblogs.microsoft.com/oldnewthing/20200319-00/?p=103572
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
} // namespace webpp::details

#define constexpr_assert(ERR, StrLiteral)                                                   \
    webpp::details::constexpr_assert_type<bool(ERR)>{}([]<bool errv>() constexpr noexcept { \
        static_assert(errv, StrLiteral);                                                    \
    })

#define constexpr_assert_false(ERR, StrLiteral) constexpr_assert(false, StrLiteral)

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2593r0.html
// GCC 13.1 started supporting static_assert(false, ...)
// Clang 16 doesn't support this, but the current trunk version does
// todo: add Clang version to this mix as well
#if __GNUC__ >= 13 || (__GNUC__ == 13 && (__GNUC_MINOR__ >= 1))
#    define static_assert_false(BasedOn, StrLiteral) static_assert(false, StrLiteral)
#else
#    define static_assert_false(BasedOn, StrLiteral) static_assert(false && !sizeof(BasedOn*), StrLiteral)
#endif

#endif // WEBPP_META_HPP
