// Created by moisrex on 4/7/20.

#ifndef WEBPP_THREAD_POOL_H
#define WEBPP_THREAD_POOL_H

#include "../../libs/asio.hpp"

// clang-format off
#include asio_include(thread_pool)
// clang-format on

namespace webpp {

    using asio_thread_pool = asio::thread_pool;

} // namespace webpp

#endif // WEBPP_THREAD_POOL_H
