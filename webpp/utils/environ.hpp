#ifndef WEBPP_UTILS_ENVIRON_HPP
#define WEBPP_UTILS_ENVIRON_HPP

#if defined(_WIN32)
#    include <cstdlib> // for _environ
extern "C" {
#    if defined(_MSC_VER)
extern char** _environ;
#    else
extern char** __environ;
#    endif
}
#else
#    include <unistd.h> // for POSIX environ
#endif

namespace webpp {

    /// Helper function that returns the current environment pointer cross-platform.
    [[nodiscard]] static inline char** get_environ() noexcept {
#if defined(_WIN32)
#    if defined(_MSC_VER)
        return _environ;
#    else
        return __environ;
#    endif
#else
        return environ;
#endif
    }
} // namespace webpp

#endif // WEBPP_UTILS_ENVIRON_HPP
