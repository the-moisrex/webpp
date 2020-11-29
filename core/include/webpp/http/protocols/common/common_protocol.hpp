// Created by moisrex on 11/28/20.

#ifndef WEBPP_COMMON_PROTOCOL_HPP
#define WEBPP_COMMON_PROTOCOL_HPP

#include "../../../server/server_concepts.hpp"
#include "../../../std/string_view.hpp"
#include "../../app_wrapper.hpp"
#include "../../request.hpp"


namespace webpp {


    template <Traits TraitsType, Application App, ExtensionList EList,
              template <typename...> typename RequestTemplate, typename... RequestTemplateArgs>
    struct common_protocol : public enable_traits<TraitsType>, public stl::remove_cvref_t<EList> {
        using traits_type      = TraitsType;
        using application_type = App;
        using extension_list   = stl::remove_cvref_t<EList>;
        using request_type     = RequestType;
        using protocol_type    = ProtoType;
        using string_view_type = typename TraitsType::string_view_type;
        using string_type      = typename TraitsType::string_type;
        using request_type     = simple_request<traits_type, extension_list, RequestTemplate, RequestTemplateArgs...>;
        using allocator_type   = typename request_type::allocator_type;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using etraits          = enable_traits<traits_type>;
        using app_wrapper_type = http_app_wrapper<traits_type, application_type>;

        app_wrapper_type app

          template <typename AllocType = allocator_type>
          common_protocol(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{logger, alloc} {}
    };

} // namespace webpp

#endif // WEBPP_COMMON_PROTOCOL_HPP
