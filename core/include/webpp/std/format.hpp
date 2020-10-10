// Created by moisrex on 5/31/20.

#ifndef WEBPP_FORMAT_H
#define WEBPP_FORMAT_H

#include "./std.hpp"
#include "../libs/fmt.hpp"

#if __cpp_lib_format
#    include <format>
#elif __has_include(fmt(format))
#    include fmt(format)
#    include fmt(printf)
#define WEBPP_FMT_LIB 1
namespace webpp::stl {

    template <typename... Args>
    inline auto format(Args&&... args) {
        return fmt::format(stl::forward<Args>(args)...);
    }

    template <typename... Args>
    inline auto format_to(Args&&... args) {
        return fmt::format_to(stl::forward<Args>(args)...);
    }

} // namespace webpp::std
#else
#    error "We don't have access to <format> nor {fmt} library."
#endif

#endif // WEBPP_FORMAT_H
