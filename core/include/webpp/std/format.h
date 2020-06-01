// Created by moisrex on 5/31/20.

#ifndef WEBPP_FORMAT_H
#define WEBPP_FORMAT_H

#include "./std.h"

#if __has_include(<format>)
#    include <format>
#else
#    include <fmt/format.h>
namespace webpp::std {
    using namespace fmt;
} // namespace webpp::std
#endif

#endif // WEBPP_FORMAT_H
