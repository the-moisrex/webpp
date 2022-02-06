// Created by moisrex on 11/7/20.

#ifndef WEBPP_BEAST_HPP
#define WEBPP_BEAST_HPP

#include "../../std/string_view.hpp"
#include "beast_proto/beast_server.hpp"
#include "common/common_http_protocol.hpp"

#include <boost/beast.hpp>

namespace webpp::http {


    template <Application App, ExtensionList EList = empty_extension_pack>
    struct beast : public common_protocol<typename ServerTraitsType::traits_type, App, EList> {
        using server_traits_type = SeverTraitsType;
        using traits_type        = typename server_traits_type::traits_type;
        using application_type   = App;
        using protocol_type      = beast<traits_type, application_type, extension_list>;
        using server_type        = beast_proto::beast_server<traits_type, request_type>;

        server_type      server;
        app_wrapper_type app;

        // todo: SSL options and other options go here

        template <typename AllocType = allocator_type>
        beast(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            server{logger, alloc},
            app{logger, alloc} {}

        [[nodiscard]] static constexpr bool is_ssl_available() const noexcept {
            return false; // todo: change this
        }

        int operator()() noexcept {
            return server();
        }
    };


} // namespace webpp::http

#endif // WEBPP_BEAST_HPP
