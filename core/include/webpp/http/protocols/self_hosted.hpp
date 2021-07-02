// Created by moisrex on 9/4/20.

#ifndef WEBPP_SELF_HOSTED_HPP
#define WEBPP_SELF_HOSTED_HPP

#include "../../server/server_concepts.hpp"
#include "../../std/string_view.hpp"
#include "../app_wrapper.hpp"
#include "../response.hpp"
#include "./common/common_http_protocol.hpp"
#include "./self_hosted_request.hpp"
#include "./shosted/self_hosted_session_manager.hpp"
#include "webpp/application/request.hpp"

namespace webpp::http::inline shosted {

    template <ServerTraits ServerTraitsType, Application App, ExtensionList EList = empty_extension_pack>
    struct self_hosted
      : public common_http_protocol<typename ServerTraitsType::traits_type, App, EList, self_hosted_request> {

        using server_traits_type = SeverTraitsType;
        using traits_type        = typename server_traits_type::traits_type;

      private:
        using super = common_http_protocol<traits_type, App, EList, self_hosted_request>;

      public:
        using request_type = typename super::request_type;
        using server_type  = typename server_traits_type::template server_type<
          shosted::self_hosted_session_manager<traits_type, request_type>>;

        server_type server;

        // todo: SSL options and other options go here

        template <typename... Args>
        self_hosted(Args&&... args) : super{stl::forward<Args>(args)...},
                                      server{this->app} {}

        [[nodiscard]] static constexpr bool is_ssl_available() const noexcept {
            return false; // todo: change this
        }

        void operator()() noexcept {
            this->server();
        }
    };

    //    template <Traits TraitsType, Application App, ExtensionList EList = empty_extension_pack>
    //    using server =

} // namespace webpp::http::inline shosted

#endif // WEBPP_SELF_HOSTED_HPP
