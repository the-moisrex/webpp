// Created by moisrex on 4/4/20.
#ifndef WEBPP_STD_NET_H
#define WEBPP_STD_NET_H
// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#include "std.hpp"

#if __has_include(<net>)
#    define STD_NET STLLIB_STANDARD
#    include <net>
#elif __has_include(<boost/asio/ts/net.hpp>)
#    define STD_NET STLLIB_BOOST
#    include <boost/asio/ts/net.hpp>

namespace webpp::stl {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/net>)
#    define STD_NET STLLIB_EXPERIMENTAL
#    include <experimental/net>

namespace webpp::stl {
    namespace net = experimental::net;
}
#else
#    error STLLIB_NETWORKING_ERROR
#endif

#endif // WEBPP_STD_NET_H
