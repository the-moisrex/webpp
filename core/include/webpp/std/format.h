// Created by moisrex on 5/31/20.

#ifndef WEBPP_FORMAT_H
#define WEBPP_FORMAT_H

#include "./std.h"

#if __cpp_lib_format
#    include <format>
#elif __has_include(<fmt/format.h>)
#    include <fmt/format.h>
namespace webpp::std {
    using namespace fmt;
} // namespace webpp::std
#else
#    error \
      "We don't have access to <format> nor {fmt} library. Provide at least one of them."
#endif

#endif // WEBPP_FORMAT_H
