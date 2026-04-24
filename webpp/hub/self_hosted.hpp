// Created by moisrex on 9/4/20.

#ifndef WEBPP_SELF_HOSTED_HPP
#define WEBPP_SELF_HOSTED_HPP

#include "../http/app_wrapper.hpp"
#include "../http/protocol/common_http_protocol.hpp"
#include "../http/request.hpp"
#include "../http/response.hpp"
#include "../std/string_view.hpp"
#include "self_hosted_request.hpp"
#include "self_hosted_session_manager.hpp"

namespace webpp {

    template <Application App>
    struct host : public common_http_protocol<App> {
      private:
        using super = common_http_protocol<App>;

      public:
        using app_wrapper_type = typename super::app_wrapper_type;
        using request_type     = typename super::request_type;

        server_type server;

        // todo: SSL options and other options go here

        template <typename... Args>
        explicit host(Args&&... args)
          : super{stl::forward<Args>(args)...},
            server{this->app} {}

        [[nodiscard]] static constexpr bool is_ssl_available() noexcept {
            return false; // todo: change this
        }

        void operator()() noexcept {
            this->server();
        }
    };

} // namespace webpp

#endif // WEBPP_SELF_HOSTED_HPP
