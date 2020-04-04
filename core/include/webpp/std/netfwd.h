// Created by moisrex on 4/4/20.

// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#if __has_include(<netfwd>)
#include <netfwd>
#elif __has_include(<boost/asio/ts/netfwd.hpp>)
#include <boost/asio/ts/netfwd.hpp>
namespace std {
    namespace net = boost::asio;
}
#elif __has_include(<experimental/netfwd>)
#include <experimental/netfwd>
namespace std {
    namespace net = experimental::net;
}
#else
#error "We don't even have access to networking TS."
#endif
