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
    struct common_protocol : public enable_traits<TraitsType>, public stl::remove_cvref_t<EList> {
        using traits_type      = stl::remove_cvref_t<TraitsType>;
        using application_type = App;
        using extension_list   = stl::remove_cvref_t<EList>;
        using string_view_type = typename traits_type::string_view_type;
        using string_type      = typename traits_type::string_type;
        using etraits          = enable_traits<traits_type>;
        using app_wrapper_type = http_app_wrapper<traits_type, application_type>;

        app_wrapper_type app;

        template <typename AllocType = allocator_type>
        common_protocol(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{logger, alloc} {}
    };

} // namespace webpp

#endif // WEBPP_COMMON_PROTOCOL_HPP
