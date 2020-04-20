// Created by moisrex on 4/4/20.
#ifndef WEBPP_STD_BUFFER_H
#define WEBPP_STD_BUFFER_H
// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#include "std.h"

#if __has_include(<buffer>)
#    include <buffer>
#    define STD_BUFFER STLLIB_STANDARD
#elif __has_include(<boost/asio/ts/buffer.hpp>)
#    define STD_BUFFER STLLIB_BOOST
#    include <boost/asio/ts/buffer.hpp>
namespace std {
    namespace net = boost::asio;
} // namespace std
#elif __has_include(<experimental/buffer>)
#    define STD_BUFFER STLLIB_EXPERIMENTAL
#    include <experimental/buffer>
namespace std {
    namespace net = experimental::net;
}
#else
#    error STLLIB_NETWORKING_ERROR
#endif

#endif // WEBPP_STD_BUFFER_H
