// Created by moisrex on 8/26/20.

#ifndef WEBPP_ASIO_TRAITS_HPP
#define WEBPP_ASIO_TRAITS_HPP

#include "../server_concepts.hpp"
#include "./asio_server.hpp"
#include "./asio_thread_pool.hpp"

namespace webpp {

    template <Traits TraitsType, ThreadPool ThreadPoolType = asio_thread_pool>
    struct asio_traits {
        using traits_type      = TraitsType;
        using thread_pool_type = ThreadPoolType;

        template <SessionManager SessionType>
        using server_type = asio_server<traits_type, SessionType, thread_pool_type>;
    };

} // namespace webpp

#endif // WEBPP_ASIO_TRAITS_HPP
