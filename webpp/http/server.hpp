// Created by moisrex on 2/15/23.

#ifndef WEBPP_HTTP_SERVER_HPP
#define WEBPP_HTTP_SERVER_HPP

#include "../traits/default_traits.hpp"
#include "./protocols/common/common_http_protocol.hpp"
#include "http_concepts.hpp"

namespace webpp::http {

    struct application_holder {};

    /**
     * Dynamic Server
     *
     * Implements HTTPProtocol
     */
    struct server : common_http_protocol<default_traits, application_holder> {
        using protocol_type    = common_http_protocol<default_traits, application_holder>;
        using traits_type      = default_traits;
        using application_type = application_holder;

        int operator()() noexcept {
            return EXIT_SUCCESS;
        }
    };


    static_assert(HTTPProtocol<server>, "The dynamic server is not a valid protocol.");
} // namespace webpp::http

#endif // WEBPP_HTTP_SERVER_HPP
