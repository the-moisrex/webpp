// Created by moisrex on 5/31/20.

#ifndef WEBPP_FORMAT_H
#define WEBPP_FORMAT_H

#if __has_include(<format>)
#    include <format>
#else
#    include <fmt/core.h>
#    include <fmt/format.h>
namespace std {

    template <typename... Args>
    using format = fmt::format<Args...>;

    template <typename... Args>
    using format_to = fmt::format_to<Args...>;

} // namespace std
#endif

#endif // WEBPP_FORMAT_H
