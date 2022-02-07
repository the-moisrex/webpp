// Created by moisrex on 11/7/20.

#ifndef WEBPP_BEAST_HPP
#define WEBPP_BEAST_HPP

#include "../../std/string_view.hpp"
#include "beast_proto/beast_server.hpp"
#include "common/common_http_protocol.hpp"


namespace webpp::http {


    template <Application App, Traits TraitsType = default_traits, ExtensionList EList = empty_extension_pack>
    struct beast : public common_http_protocol<TraitsType, App, EList> {
        using traits_type      = TraitsType;
        using application_type = App;
        using extension_list   = EList;
        using protocol_type    = beast<application_type, traits_type, extension_list>;
        using server_type      = beast_proto::beast_server<traits_type, extension_list>;
        using app_wrapper_type = http_app_wrapper<traits_type, application_type>;

        server_type      server;
        app_wrapper_type app;

      private:
        using super = common_http_protocol<TraitsType, App, EList>;

      public:
        template <typename... Args>
        beast(Args&&... args) : super{stl::forward<Args>(args)...},
                                server{*this},
                                app{*this} {}

        [[nodiscard]] static constexpr bool is_ssl_available() noexcept {
            return false; // todo: change this
        }

        int operator()() noexcept {
            return server();
        }
    };


} // namespace webpp::http

#endif // WEBPP_BEAST_HPP
