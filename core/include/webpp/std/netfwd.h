// Created by moisrex on 4/4/20.
#ifndef WEBPP_STD_NETFWD_H
#define WEBPP_STD_NETFWD_H

// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#include "std.h"

#if __has_include(<netfwd>)
#    define STD_NETFWD STLLIB_STANDARD
#    include <netfwd>
#elif __has_include(<boost/asio/ts/netfwd.hpp>)
#    define STD_NETFWD STLLIB_BOOST
#    include <boost/asio/ts/netfwd.hpp>
namespace std {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/netfwd>)
#    define STD_NETFWD STLLIB_EXPERIMENTAL
#    include <experimental/netfwd>
namespace std {
    namespace net = experimental::net;
}
#else
#    error STLLIB_NETWORKING_ERROR
#endif

#endif // WEBPP_STD_NETFWD_H
