// Created by moisrex on 4/4/20.
#ifndef WEBPP_STD_SOCKET_H
#define WEBPP_STD_SOCKET_H

// We rather use boost::asio instead of experimental version of std because
// currently experimental version of socket is buggy. Hopefully that will be
// fixed later.

// The can use __has_include here because the project is already in C++20 and
// __has_include is a C++20 feature and we're testing <socket> header which
// is not yet part of C++ standard but hopefully will be added in C++23

#include "./std.h"

#if __has_include(<socket>)
#    define STD_SOCKET STLLIB_STANDARD
#    include <socket>
#elif __has_include(<boost/asio/ts/socket.hpp>)
#    define STD_SOCKET STLLIB_BOOST
#    include <boost/asio/ts/socket.hpp>
namespace webpp::std {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/socket>)
#    define STD_SOCKET STLLIB_EXPERIMENTAL
#    include <experimental/socket>
namespace webpp::std {
    namespace net = experimental::net;
}
#else
#    error STLLIB_NETWORKING_ERROR
#endif

#endif // WEBPP_STD_SOCKET_T
