// Created by moisrex on 4/7/20.

#ifndef WEBPP_THREAD_POOL_HPP
#define WEBPP_THREAD_POOL_HPP

#include "../../libs/asio.hpp"

#ifdef WEBPP_BOOST_ASIO
#    include <boost/asio/thread_pool.hpp>
#else
#    include <asio/thread_pool.hpp>
#endif

namespace webpp {

    using asio_thread_pool = asio::thread_pool;

} // namespace webpp

#endif // WEBPP_THREAD_POOL_HPP
