// Created by moisrex on 11/28/20.

#ifndef WEBPP_COMMON_PROTOCOL_HPP
#define WEBPP_COMMON_PROTOCOL_HPP

#include "../../../extensions/extension.hpp"
#include "../../../server/server_concepts.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../app_wrapper.hpp"

namespace webpp {

    namespace details {
        template <typename AllocPackType>
        struct allocator_pack_holder {
            using allocator_pack_type = AllocPackType;

            [[no_unique_address]] allocator_pack_type original_alloc_pack{};
        };
    } // namespace details

    // todo: should we inherit from EList? I don't think
    template <Traits TraitsType, Application App, ExtensionList EList>
    struct common_protocol : public details::allocator_pack_holder<traits::allocator_pack_type<TraitsType>>,
                             public enable_traits<TraitsType>,
                             public EList {
        using traits_type                = TraitsType;
        using application_type           = stl::remove_cvref_t<App>;
        using extension_list             = EList;
        using string_view_type           = traits::string_view<traits_type>;
        using char_type                  = traits::char_type<traits_type>;
        using string_type                = traits::general_string<traits_type>;
        using etraits                    = enable_traits<traits_type>;
        using app_wrapper_type           = http_app_wrapper<traits_type, application_type>;
        using allocator_pack_type        = traits::allocator_pack_type<traits_type>;
        using allocator_pack_holder_type = details::allocator_pack_holder<allocator_pack_type>;

        app_wrapper_type app;

        template <typename... Args>
        common_protocol(Args&&... args)
          : allocator_pack_holder_type{},
            etraits{this->original_alloc_pack},
            app{*this, stl::forward<Args>(args)...} {}
    };

} // namespace webpp

#endif // WEBPP_COMMON_PROTOCOL_HPP
