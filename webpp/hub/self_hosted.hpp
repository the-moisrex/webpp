// Created by moisrex on 9/4/20.

#ifndef WEBPP_SELF_HOSTED_HPP
#define WEBPP_SELF_HOSTED_HPP

#include "../http/app_wrapper.hpp"
#include "../http/protocol/common_http_protocol.hpp"
#include "../http/request.hpp"
#include "../http/response.hpp"
#include "../server/default_server_traits.hpp"
#include "../server/server_concepts.hpp"
#include "../std/string_view.hpp"
#include "self_hosted_request.hpp"
#include "self_hosted_session_manager.hpp"

namespace webpp {

    template <Application App, Traits TraitsType = default_traits>
    struct host : public common_http_protocol<TraitsType, App> {

        using server_traits_type = ServerTraitsType;
        using traits_type        = typename server_traits_type::traits_type;

      private:
        using super = common_http_protocol<traits_type, App>;

      public:
        using app_wrapper_type = typename super::app_wrapper_type;
        using connection_type  = istl::nothing_type; // fixme: just a placeholder
        using request_type     = typename super::request_type;
        using server_type      = typename server_traits_type::template server_type<
          shosted::self_hosted_session_manager<traits_type, app_wrapper_type, request_type, connection_type>>;

        server_type server;

        // todo: SSL options and other options go here

        template <typename... Args>
        self_hosted(Args&&... args) : super{stl::forward<Args>(args)...},
                                      server{this->app} {}

        [[nodiscard]] static constexpr bool is_ssl_available() noexcept {
            return false; // todo: change this
        }

        void operator()() noexcept {
            this->server();
        }
    };

    //    template <Traits TraitsType, Application App>
    //    using server =

} // namespace webpp

#endif // WEBPP_SELF_HOSTED_HPP
