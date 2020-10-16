// Created by moisrex on 10/8/20.

#ifndef WEBPP_CASSERT_HPP
#define WEBPP_CASSERT_HPP

#include <cassert>

// see this tweet: https://twitter.com/incomputable/status/1247235812271497216
#ifdef NDEBUG
#    undef assert
#    define assert(c)                \
        if (c) {                     \
        } else {                     \
            __builtin_unreachable(); \
        }
#endif

#ifdef __has_cpp_attribute
#  define WEBPP_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define WEBPP_HAS_CPP_ATTRIBUTE(x) 0
#endif

// [[noreturn]] is disabled on MSVC and NVCC because of bogus unreachable code
// warnings.
#if WEBPP_EXCEPTIONS && WEBPP_HAS_CPP_ATTRIBUTE(noreturn) && !WEBPP_MSC_VER && \
    !WEBPP_NVCC
#  define WEBPP_NORETURN [[noreturn]]
#else
#  define WEBPP_NORETURN
#endif

namespace webpp::details {
    WEBPP_NORETURN void assert_fail(const char* file, int line, const char* message) {
        // Use unchecked std::fprintf to avoid triggering another assertion when writing to stderr fails
        std::fprintf(stderr, "%s:%d: assertion failed: %s", file, line, message);
        // Chosen instead of std::abort to satisfy Clang in CUDA mode during device code pass.
        std::terminate();
    }
}

#ifndef webpp_assert
#  ifdef NDEBUG
// webpp_assert is not empty to avoid -Werror=empty-body.
#    define webpp_assert(condition, message) ((void)0)
#  else
#    define webpp_assert(condition, message)                                    \
      ((condition) /* void() fails with -Winvalid-constexpr on clang 4.0.1 */ \
           ? (void)0                                                          \
           : ::webpp::detail::assert_fail(__FILE__, __LINE__, (message)))
#  endif
#endif

#endif // WEBPP_CASSERT_HPP
