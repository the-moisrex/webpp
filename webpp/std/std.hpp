// Created by moisrex on 4/4/20.

#ifndef WEBPP_STD_STD_H
#define WEBPP_STD_STD_H

#define STLLIB_STANDARD         0
#define STLLIB_EXPERIMENTAL     1
#define STLLIB_BOOST            2
#define STLLIB_NETWORKING_ERROR "We don't even have access to networking TS."
#define BOOST_ASIO_NO_DEPRECATED

#if __has_include(<version>)
#    include <version>
#endif

namespace webpp::stl {
    using namespace ::std;
} // namespace webpp::stl

#endif // WEBPP_STD_STD_H
