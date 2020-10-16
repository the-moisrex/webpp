// Created by moisrex on 10/15/20.

#ifndef WEBPP_POSIX_TRAITS_HPP
#define WEBPP_POSIX_TRAITS_HPP

#include "../server_concepts.hpp"
#include "./asio_server.hpp"
#include "./asio_thread_pool.hpp"

namespace webpp::posix {


    template <Traits TraitsType, ThreadPool ThreadPoolType = posix_thread_pool>
    struct posix_traits {
        using traits_type      = TraitsType;
        using thread_pool_type = ThreadPoolType;

        template <SessionManager SessionType>
        using server_type = asio_server<traits_type, SessionType, thread_pool_type>;
    };


}

#endif // WEBPP_POSIX_TRAITS_HPP
