// Created by moisrex on 4/4/20.

// we rather use boost::asio instead of experimental version of std because
// currently experimental version of socket is buggy. Hopefully that will be
// fixed later.

#if __has_include(<socket>)
#include <socket>
#elif __has_include(<boost/asio/ts/socket.hpp>)
#include <boost/asio/ts/socket.hpp>
namespace std {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/socket>)
#include <experimental/socket>
namespace std {
    namespace net = experimental::net;
}
#else
#error "We don't even have access to networking TS."
#endif
