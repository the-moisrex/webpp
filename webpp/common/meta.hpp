// Created by moisrex on 7/20/19.

#ifndef WEBPP_META_HPP
#define WEBPP_META_HPP

// NOLINTBEGIN(*-avoid-do-while, *-macro-usage)

#if __cplusplus >= 199711L
#    define CXX98
#endif

#if __cplusplus >= 201103L
#    define CXX11
#endif

#if __cplusplus >= 201402L
#    define CXX14
#endif

#if __cplusplus >= 201703L
#    define CXX17
#endif

#if __cplusplus >= 202002L
#    define CXX20
#endif

#if __cplusplus >= 202302L
#    define CXX23
#endif

#ifdef CXX23
#    define IF_CXX23(val) val
#else
#    define IF_CXX23(val)
#endif

// For usage in a constexpr function, C++20 doesn't allow for usage of "static constexpr" stuff inside
// "constexpr" functions, but C++23 does.
// https://wg21.link/P2647#feature-test-macro
#if defined(__cpp_constexpr) && __cpp_constexpr >= 202300L
#    define webpp_static_constexpr static constexpr
#else
#    define webpp_static_constexpr constexpr
#endif

// the static_assert alternative based on this article:
// https://devblogs.microsoft.com/oldnewthing/20200311-00/?p=103553
// https://devblogs.microsoft.com/oldnewthing/20200319-00/?p=103572
// But this way is "type dependent"; meaning, you have to have a type (which you usually do).
// static_assert(!sizeof(decltype(input)*), "not able to do this");

namespace webpp::details {
    template <bool err>
    struct constexpr_assert_type {
        constexpr void operator()([[maybe_unused]] auto anything) const noexcept {}
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



#if defined(NDEBUG) || !defined(DEBUG) || !defined(_DEBUG)
#    define NDEBUG
namespace webpp {
    static constexpr bool is_debug_build = false;
}
#else
#    define DEBUG
namespace webpp {
    static constexpr bool is_debug_build = true;
}
#endif


////////////////////////////// [[assume(...)]] fallback: //////////////////////////////

#ifdef __has_cpp_attribute
#    define WEBPP_HAS_CPP_ATTRIBUTE(name) __has_cpp_attribute(name)
#else
#    define WEBPP_HAS_CPP_ATTRIBUTE(name) 0
#endif

#if defined(__has_attribute)
#    define WEBPP_HAS_ATTR(attr) __has_attribute(attr)
#else
#    define WEBPP_HAS_ATTR(attr) 0
#endif

#ifdef __has_builtin
#    define WEBPP_HAS_BUILTIN(name) __has_builtin(name)
#else
#    define WEBPP_HAS_BUILTIN(name) 0
#endif

/// Use `webpp_assume(expr);` to inform the compiler that the `expr` is true.
/// Do not rely on side-effects of the `expr` being run.
/// Attention: DO NOT call (specially `extern`) functions with side-effects inside this, it's "potentially
#if WEBPP_HAS_CPP_ATTRIBUTE(assume) || defined(__cpp_assume)
#    define webpp_assume(...) [[assume(__VA_ARGS__)]]
#elif defined(__clang__) || defined(__INTEL_COMPILER) || WEBPP_HAS_BUILTIN(__builtin_assume) || \
  defined(_MSC_VER) || defined(__ICC)
#    if defined(_MSC_VER) || defined(__ICC) // ICC seems to support both __builtin_assume and __assume
#        define webpp_assume_func(...) __assume(__VA_ARGS__)
#    else
#        define webpp_assume_func(...) __builtin_assume(__VA_ARGS__)
#    endif

namespace webpp::details {

    auto non_constexpr() -> void {}

    template <typename BoolLike>
#    ifdef __cpp_concepts
        requires requires(BoolLike val) { static_cast<bool>(val); }
#    endif
    constexpr auto constexpr_only_if(BoolLike const& condition) noexcept -> void {
        if (!condition) {
            non_constexpr();
        }
    }

} // namespace webpp::details

#    if defined(__cpp_if_consteval) && CXX23
#        define webpp_assume(...)                                   \
            do {                                                    \
                if consteval {                                      \
                    webpp::details::constexpr_only_if(__VA_ARGS__); \
                } else {                                            \
                    webpp_assume_func(__VA_ARGS__);                 \
                }                                                   \
            } while (false)
#    elif defined(__cpp_lib_is_constant_evaluated)
#        include <type_traits>
#        define webpp_assume(...)                                   \
            do {                                                    \
                if (std::is_constant_evaluated()) {                 \
                    webpp::details::constexpr_only_if(__VA_ARGS__); \
                } else {                                            \
                    webpp_assume_func(__VA_ARGS__);                 \
                }                                                   \
            } while (false)
#    else
#        define webpp_assume(...) webpp_assume_func(__VA_ARGS__)
#    endif
#elif WEBPP_HAS_ATTR(assume)
//   https://gcc.gnu.org/onlinedocs/gcc/Statement-Attributes.html#index-assume-statement-attribute
#    define webpp_assume(...) __attribute__((assume(__VA_ARGS__)))
#elif WEBPP_HAS_ATTR(__assume__) // GCC 13, but GCC 13 should support [[assume]]; I don't know if it's needed
#    define webpp_assume(...) __attribute__((__assume__(__VA_ARGS__)))
#elif defined(__GNUC__)
#    define webpp_assume(...)            \
        do {                             \
            if (!(__VA_ARGS__))          \
                __builtin_unreachable(); \
        } while (false)
#else
namespace webpp {
    [[noreturn]] inline void invoke_undefined_behaviour() {}
} // namespace webpp
#    define webpp_assume(...)                        \
        do {                                         \
            if (!(__VA_ARGS__))                      \
                webpp::invoke_undefined_behaviour(); \
        } while (false)
#endif
// NOLINTEND(*-avoid-do-while, *-macro-usage)

#endif // WEBPP_META_HPP
