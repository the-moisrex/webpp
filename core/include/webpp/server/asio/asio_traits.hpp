// Created by moisrex on 8/26/20.

#ifndef WEBPP_ASIO_TRAITS_HPP
#define WEBPP_ASIO_TRAITS_HPP

#include "../server_concepts.hpp"
#include "./asio_server.hpp"

namespace webpp {

    template <Traits TraitsType, Session SessionType>
    struct asio_traits {
        using traits_type = TraitsType;
        using session_type = SessionType;
        using server_type = asio_server<traits_type, session_type>;
    };

}

#endif // WEBPP_ASIO_TRAITS_HPP
