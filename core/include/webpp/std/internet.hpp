// Created by moisrex on 4/4/20.
#ifndef WEBPP_STD_INTERNET_H
#define WEBPP_STD_INTERNET_H

// we rather use boost::asio instead of experimental version of std because
// currently experimental version of internet is buggy. Hopefully that will be
// fixed later.

#include "./std.hpp"

#if __has_include(<internet>)
#    define STD_INTERNET STLLIB_STANDARD
#    include <internet>
#    include <system_error>
namespace webpp::stl::net {
    // a fix for different error_code in boost vs in std
    using error_code      = stl::error_code;
    using error_category  = stl::error_category;
    using error_condition = stl::error_condition;
} // namespace webpp::stl::net
#elif __has_include(<boost/asio/ts/internet.hpp>)
#    define STD_INTERNET STLLIB_BOOST
#    include <boost/asio/ts/internet.hpp>
namespace webpp::stl::net {
    using namespace boost::asio;
    using error_code      = boost::system::error_code;
    using error_category  = boost::system::error_category;
    using error_condition = boost::system::error_condition;
} // namespace webpp::stl::net
#elif __has_include(<experimental/internet>)
#    define STD_INTERNET STLLIB_EXPERIMENTAL
#    include <experimental/internet>
namespace webpp::stl {
    namespace net = experimental::net;
}
#else
#    error STLLIB_NETWORKING_ERROR
#endif



#endif // WEBPP_STD_INTERNET_H
