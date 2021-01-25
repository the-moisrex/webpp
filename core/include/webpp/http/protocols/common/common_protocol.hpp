// Created by moisrex on 11/28/20.

#ifndef WEBPP_COMMON_PROTOCOL_HPP
#define WEBPP_COMMON_PROTOCOL_HPP

#include "../../../extensions/extension.hpp"
#include "../../../server/server_concepts.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../app_wrapper.hpp"

namespace webpp {


    // todo: should we inherit from EList? I don't think
    template <Traits TraitsType, Application App, ExtensionList EList>
    struct common_protocol : public enable_traits<TraitsType, true>, public EList {
        using traits_type      = TraitsType;
        using application_type = stl::remove_cvref_t<App>;
        using extension_list   = EList;
        using string_view_type = traits::string_view<traits_type>;
        using char_type        = traits::char_type<traits_type>;
        using string_type      = traits::general_string<traits_type>;
        using etraits          = enable_traits<traits_type>;
        using app_wrapper_type = http_app_wrapper<traits_type, application_type>;

        app_wrapper_type app;

        common_protocol(application_type&& bare_app) : etraits{}, app{stl::move(bare_app)} {}
        common_protocol(application_type const& bare_app) : etraits{}, app{bare_app} {}

        common_protocol()
          : etraits{},
            app{this->logger, this->get_allocator()} {}
    };

} // namespace webpp

#endif // WEBPP_COMMON_PROTOCOL_HPP
