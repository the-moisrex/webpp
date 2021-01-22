// Created by moisrex on 1/22/21.

#ifndef WEBPP_STD_CHRONO_HPP
#define WEBPP_STD_CHRONO_HPP

#include "std.hpp"

// A polyfill of some sort for std::chrono
#if defined(__cpp_lib_chrono) && __cpp_lib_chrono < 201907L
#    include <date/date.h>
#    include <date/tz.h>
namespace webpp::stl::chrono {
    using namespace ::std::chrono;
    using namespace ::date;
} // namespace webpp::stl::chrono
#else
#    include <chrono>
#endif

#endif // WEBPP_STD_CHRONO_HPP
