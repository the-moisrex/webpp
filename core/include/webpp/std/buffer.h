// Created by moisrex on 4/4/20.

// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#if __has_include(<buffer>)
#include <buffer>
#elif __has_include(<boost/asio/ts/buffer.hpp>)
#include <boost/asio/ts/buffer.hpp>
namespace std {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/buffer>)
#include <experimental/buffer>
namespace std {
    namespace net = experimental::net;
}
#else
#error "We don't even have access to networking TS."
#endif
