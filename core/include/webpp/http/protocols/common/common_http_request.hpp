// Created by moisrex on 11/28/20.

#ifndef WEBPP_COMMON_HTTP_REQUEST_HPP
#define WEBPP_COMMON_HTTP_REQUEST_HPP

#include "../../../std/format.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../../version.hpp"
#include "../../request_body.hpp"
#include "../../request_headers.hpp"
#include "../../../memory/object.hpp"


namespace webpp::http {

    template <Traits TraitsType, HTTPRequestExtensionParent REL, RootExtensionList RootExtensions>
    struct common_http_request : public REL, public enable_traits<TraitsType> {
        using root_extensions            = RootExtensions;
        using traits_type                = TraitsType;
        using request_extension_list     = REL;
        using etraits                    = enable_traits<traits_type>;
        using string_type                = traits::local_string_allocator<traits_type>;
        using string_view_type           = traits::string_view<traits_type>;
        using char_type                  = typename string_type::value_type;
        using allocator_descriptors_type = traits::allocator_descriptors<traits_type>;
        using local_allocator_type       = traits::local_allocator<traits_type, char_type>;
        using general_allocator_type     = traits::general_allocator<traits_type, char_type>;
        using request_extensions         = REL;
        using headers_type = simple_request_headers<traits_type, root_extensions, local_allocator_type>;
        using body_type    = simple_request_body<traits_type, root_extensions>;
        using headers_object_type = object::local<headers_type, stack<>, allocator_descriptors_type>;
        using body_object_type =
          object::object<body_type, alloc::general_features, allocator_descriptors_type>;

        headers_object_type headers{this->alloc_pack};
        body_object_type    body{this->alloc_pack};

        using etraits::etraits;
    };


} // namespace webpp::http

#endif // WEBPP_COMMON_HTTP_REQUEST_HPP
