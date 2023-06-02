// Created by moisrex on 1/22/21.

#ifndef WEBPP_STD_CHRONO_HPP
#define WEBPP_STD_CHRONO_HPP

#include "std.hpp"

// A polyfill of some sort for std::chrono (We need C++20 std::chrono::utc_clock)
// If you needed to change the conditions here, make sure to change the conditions of date.cmake file as well
#if !defined(__cpp_lib_chrono) || __cpp_lib_chrono < 201907L
#    include <date/date.h>
#    include <date/tz.h>
namespace webpp::stl::chrono {
    // using namespace ::std::chrono;
    using ::std::chrono::duration;
    using ::std::chrono::high_resolution_clock;
    using ::std::chrono::microseconds;
    using ::std::chrono::milliseconds;
    using ::std::chrono::nanoseconds;
    using ::std::chrono::seconds;
    using ::std::chrono::system_clock;
    using ::std::chrono::time_point;
    using namespace ::date;

    namespace chrono_literals {
        using namespace ::date::literals;
    }
} // namespace webpp::stl::chrono
#else
#    include <chrono>
namespace webpp::stl::chrono {
    using namespace ::std::chrono;
}
#endif

#endif // WEBPP_STD_CHRONO_HPP
