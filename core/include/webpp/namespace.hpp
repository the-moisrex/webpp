// Created by moisrex on 8/19/20.

#ifndef WEBPP_ASIO_NAMESPACE_HPP
#define WEBPP_ASIO_NAMESPACE_HPP

#include "./std/std.hpp"

#if __has_include(<boost/asio.hpp>)
#    define WEBPP_BOOST_ASIO
#endif

#if __has_include(<asio/asio.hpp>)
#    define WEBPP_ASIO
#endif

#ifdef WEBPP_BOOST_ASIO
#    define asio(asio_file) <boost/asio/asio_file.hpp>
#elif defined(WEBPP_ASIO)
#    define asio(asio_file) <asio/asio_file.hpp>
#endif

#ifdef WEBPP_BOOST_ASIO
namespace boost::asio {}
#endif

#ifdef WEBPP_ASIO
namespace asio {}
#endif

namespace webpp::asio {
#ifdef WEBPP_BOOST_ASIO
    using namespace ::boost::asio;
#endif
#ifdef WEBPP_ASIO
    using namespace ::asio;
#endif
} // namespace webpp::asio

#endif // WEBPP_NAMESPACE_HPP
