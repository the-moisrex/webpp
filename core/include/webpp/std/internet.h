// Created by moisrex on 4/4/20.
#ifndef WEBPP_STD_INTERNET_H
#define WEBPP_STD_INTERNET_H

// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#include "std.h"

#if __has_include(<internet>)
#    define STD_INTERNET STLLIB_STANDARD
#    include <internet>
#elif __has_include(<boost/asio/ts/internet.hpp>)
#    define STD_INTERNET STLLIB_BOOST
#    include <boost/asio/ts/internet.hpp>
namespace std {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/internet>)
#    define STD_INTERNET STLLIB_EXPERIMENTAL
#    include <experimental/internet>
namespace std {
    namespace net = experimental::net;
}
#else
#    error STLLIB_NETWORKING_ERROR
#endif

#endif // WEBPP_STD_INTERNET_H
