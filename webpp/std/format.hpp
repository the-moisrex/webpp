// Created by moisrex on 5/31/20.

#ifndef WEBPP_FORMAT_H
#define WEBPP_FORMAT_H

#include "../libs/fmt.hpp"
#include "std.hpp"

#if __cpp_lib_format
#    include <format>
#    define FMT_COMPILE(formatted_string) (formatted_string)

namespace webpp::fmt {
    using namespace ::std; // to mame std::format available
}
#elif __has_include(<fmt/format.h>)
#    include <fmt/chrono.h>
#    include <fmt/compile.h>
#    include <fmt/format.h>
#    include <fmt/printf.h>
#    define WEBPP_FMT_LIB 1

namespace webpp::fmt {
    using namespace ::fmt;
}

namespace webpp::stl {

    // from: https://twitter.com/vzverovich/status/1327762206734237698?s=20
    // no need for this after this tweet: https://twitter.com/vzverovich/status/1328345929758375946?s=20
    //    namespace details {
    //        template <typename... Args>
    //        struct format_str {
    //            fmt::string_view str;
    //
    //            template <size_t N>
    //            consteval format_str(const char (&s)[N]) : str(s) {
    //                using checker = fmt::detail::format_string_checker<char, fmt::detail::error_handler,
    //                Args...>; fmt::detail::parse_format_string<true>(fmt::string_view(s, N), checker(s,
    //                {}));
    //            }
    //        };
    //    }
    //
    //    template <class... Args>
    //    std::string format(details::format_str<std::type_identity_t<Args>...> fmt, const Args&... args) {
    //        return fmt::format(fmt.str, args...);
    //    }

    template <typename Duration, typename CharT>
    struct formatter;

    template <typename... Args>
    constexpr decltype(auto) format(Args&&... args) noexcept(noexcept(fmt::format(forward<Args>(args)...))) {
        return fmt::format(forward<Args>(args)...);
    }

    template <typename... Args>
    inline decltype(auto) format_to(Args&&... args) {
        return fmt::format_to(forward<Args>(args)...);
    }

} // namespace webpp::stl
#else
#    define webpp_no_fmt
#    error "We don't have access to <format> nor {fmt} library."
#endif


#ifndef webpp_no_fmt
#    include <chrono>

namespace webpp::istl {
#    if WEBPP_FMT_LIB
    template <typename... Args>
    inline auto safe_localtime(Args&&... args) {
        return ::fmt::localtime(stl::forward<Args>(args)...);
    }

#    else
    namespace fmt::detail {};

    // A fallback for when the fmt library is not available

    namespace details {

// Prevents expansion of a preceding token as a function-style macro.
// Usage: f FMT_NOMACRO()
#        define WEBPP_FMT_NOMACRO

        template <typename T = void>
        struct null {};

        inline null<> localtime_r WEBPP_FMT_NOMACRO(...) {
            return null<>();
        }

        inline null<> localtime_s(...) {
            return null<>();
        }

    } // namespace details

    // Thread-safe replacement for std::localtime
    inline std::tm safe_localtime(std::time_t const time) {
        struct dispatcher {
            std::time_t time_;
            std::tm     tm_;

            explicit dispatcher(std::time_t const inp_time) : time_(inp_time) {}

            bool run() {
                using namespace details;
                return handle(localtime_r(&time_, &tm_));
            }

            bool handle(std::tm const* tm) {
                return tm != nullptr;
            }

            bool handle(details::null<>) {
                using namespace details;
                return fallback(localtime_s(&tm_, &time_));
            }

            bool fallback(int const res) {
                return res == 0;
            }

#        if !_MSC_VER
            bool fallback(details::null<>) {
                using namespace fmt::detail;
                std::tm* tm = std::localtime(&time_);
                if (tm) {
                    tm_ = *tm;
                }
                return tm != nullptr;
            }
#        endif
        };

        dispatcher lt(time);
        // Too big time values may be unsupported.
        if (!lt.run()) {
            throw stl::invalid_argument("time_t value out of range");
        }
        return lt.tm_;
    }

    inline std::tm safe_localtime(std::chrono::time_point<std::chrono::system_clock> const time_point) {
        auto const time = std::chrono::system_clock::to_time_t(time_point);
        return *localtime(stl::addressof(time));
    }

#    endif
} // namespace webpp::istl
#endif


#endif // WEBPP_FORMAT_H
