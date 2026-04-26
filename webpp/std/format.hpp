// Created by moisrex on 5/31/20.

#ifndef WEBPP_FORMAT_H
#define WEBPP_FORMAT_H

#include "../libs/fmt.hpp"
#include "std.hpp"

#ifdef __cpp_lib_format
#    include <format>
#    define FMT_COMPILE(formatted_string) (formatted_string)
#    define WEBPP_FMT_STD_LIB             1

namespace webpp::fmt {
    using namespace ::std; // to mame std::format available
} // namespace webpp::fmt
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


#endif // WEBPP_FORMAT_H
