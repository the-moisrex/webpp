// Created by moisrex on 4/4/20.
#ifndef WEBPP_STD_TIMER_H
#define WEBPP_STD_TIMER_H

// we rather use boost::asio instead of experimental version of std because
// currently experimental version of timer is buggy. Hopefully that will be
// fixed later.

#include "std.hpp"

#if __has_include(<timer>)
#    define STD_TIMER STLLIB_STANDARD
#    include <timer>
#elif __has_include(<boost/asio/ts/timer.hpp>)
#    define STD_TIMER STLLIB_BOOST
#    include <boost/asio/ts/timer.hpp>
namespace webpp::stl {
    namespace net = boost::asio;
} // namespace webpp::stl
#elif __has_include(<experimental/timer>)
#    define STD_TIMER STLLIB_EXPERIMENTAL
#    include <experimental/timer>
namespace webpp::stl {
    namespace net = experimental::net;
} // namespace webpp::stl
#else
#    error STLLIB_NETWORKING_ERROR
#endif

#endif // WEBPP_STD_TIMER_H
