// Created by moisrex on 11/28/20.

#ifndef WEBPP_COMMON_HTTP_PROTOCOL_HPP
#define WEBPP_COMMON_HTTP_PROTOCOL_HPP

#include "../app_wrapper.hpp"
#include "../http_concepts.hpp"

namespace webpp::http {


    template <Application App>
    struct common_http_protocol {
        using application_type = App;
        using app_wrapper_type = http_app_wrapper<application_type>;

        app_wrapper_type app; // NOLINT(misc-non-private-member-variables-in-classes)

        template <typename... Args>
        explicit constexpr common_http_protocol(Args&&... args) : app{stl::forward<Args>(args)...} {}
    };

} // namespace webpp::http

#endif // WEBPP_COMMON_HTTP_PROTOCOL_HPP
