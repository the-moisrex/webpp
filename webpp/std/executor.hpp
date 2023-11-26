// Created by moisrex on 4/4/20.
#ifndef WEBPP_STD_EXECUTOR_H
#define WEBPP_STD_EXECUTOR_H
// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#include "std.hpp"

#if __has_include(<executor>)
#    include <executor>
#    define STD_EXECUTOR STLLIB_STANDARD
#elif __has_include(<boost/asio/ts/executor.hpp>)
#    define STD_EXECUTOR STLLIB_BOOST
#    include <boost/asio/ts/executor.hpp>

namespace webpp::stl {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/executor>)
#    define STD_EXECUTOR STLLIB_EXPERIMENTAL
#    include <experimental/executor>

namespace webpp::stl {
    namespace net = experimental::net;
}
#else
#    error STLLIB_NETWORKING_ERROR
#endif

#endif // WEBPP_STD_EXECUTOR_H
