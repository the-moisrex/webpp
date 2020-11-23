// Created by moisrex on 11/7/20.

#ifndef WEBPP_BEAST_HPP
#define WEBPP_BEAST_HPP

#include "../../std/string_view.hpp"
#include "../application.hpp"
#include "../request.hpp"
#include "../response.hpp"

#include <boost/beast.hpp>

namespace webpp {


    template <ServerTraits ServerTraitsType, Application App, ExtensionList EList = empty_extension_pack>
    struct beast : public enable_traits<typename ServerTraitsType::traits_type> {
      public:
        using server_traits_type = SeverTraitsType;
        using traits_type        = typename server_traits_type::traits_type;
        using application_type   = App;
        using extension_list     = stl::remove_cvref_t<EList>;
        using protocol_type     = beast<traits_type, application_type, extension_list>;
        using str_view_type      = typename TraitsType::string_view_type;
        using str_type           = typename TraitsType::string_type;
        using request_type = simple_request<traits_type, beast_request, protocol_type, extension_list>;
        using allocator_type           = typename request_type::allocator_type;
        using logger_type              = typename traits_type::logger_type;
        using logger_ref               = typename logger_type::logger_ref;
        using etraits                  = enable_traits<traits_type>;
        using app_wrapper_type         = http_app_wrapper<traits_type, application_type>;
        using server_type              = typename server_traits_type::template server_type<
          beast_session_manager<traits_type, request_type>>;

        server_type              server;
        app_wrapper_type         app

        // todo: SSL options and other options go here

        template <typename AllocType = allocator_type>
        beast(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            server{logger, alloc},
            app{logger, alloc} {}

        [[nodiscard]] static constexpr bool is_ssl_available() const noexcept {
            return false; // todo: change this
        }

        void operator()() noexcept {
            server();
        }
    };


}

#endif // WEBPP_BEAST_HPP
