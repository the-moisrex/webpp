// Created by moisrex on 4/4/20.

#ifndef WEBPP_STD_IO_CONTEXT_H
#define WEBPP_STD_IO_CONTEXT_H

// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#include "./std.h"

#if __has_include(<io_context>)
#    include <io_context>
#    define STD_IO_CONTEXT STLLIB_STANDARD
#elif __has_include(<boost/asio/ts/io_context.hpp>)
#    include <boost/asio/ts/io_context.hpp>
#    define STD_IO_CONTEXT STLLIB_BOOST
namespace webpp::std {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/io_context>)
#    include <experimental/io_context>
#    define STD_IO_CONTEXT STLLIB_EXPERIMENTAL
namespace webpp::std {
    namespace net = experimental::net;
}
#else
#    error STLLIB_NETWORKING_ERROR
#endif

#endif // WEBPP_STD_IO_CONTEXT_H
