// Created by moisrex on 11/7/20.

#ifndef WEBPP_BEAST_HPP
#define WEBPP_BEAST_HPP

#include "../../std/string_view.hpp"
#include "beast_request.hpp"
#include "common/common_protocol.hpp"

#include <boost/beast.hpp>

namespace webpp {


    template <ServerTraits ServerTraitsType, Application App, ExtensionList EList = empty_extension_pack>
    struct beast : public common_protocol<typename ServerTraitsType::traits_type, App, EList> {
        using server_traits_type = SeverTraitsType;
        using traits_type        = typename server_traits_type::traits_type;
        using application_type   = App;
        using protocol_type      = beast<traits_type, application_type, extension_list>;
        using server_type =
          typename server_traits_type::template server_type<beast_session_manager<traits_type, request_type>>;

        template <typename BodyType, typename FieldType = boost::beast::http::fields>
        using request_type = simple_request<traits_type, extension_list, beast_request, BodyType, FieldType>;

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

        void operator()() noexcept {
            server();
        }
    };


} // namespace webpp

#endif // WEBPP_BEAST_HPP
