// Created by moisrex on 9/23/20.

#ifndef WEBPP_BOOST_PCH_H
#define WEBPP_BOOST_PCH_H

#if __has_include(<boost/asio.hpp>)
#    include <boost/asio.hpp>
#    include <boost/bind.hpp>
#    define webpp_has_boost 1
#endif

#endif // WEBPP_BOOST_PCH_H
