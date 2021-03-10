// Created by moisrex on 11/28/20.

#ifndef WEBPP_COMMON_HTTP_REQUEST_HPP
#define WEBPP_COMMON_HTTP_REQUEST_HPP

#include "../../../memory/object.hpp"
#include "../../../std/format.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../../version.hpp"
#include "../../request_body.hpp"
#include "../../request_headers.hpp"

namespace webpp::http {

    template <Traits TraitsType, HTTPRequestExtensionParent REL, RootExtensionList RootExtensions>
    struct common_http_request : public enable_traits<TraitsType>, public REL {
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
        // fixme: this local and object will make the request non-copyable.
        // fixme: I am using general allocator here, but we can use a local allocator here for headers
        // using headers_object_type = object::local<headers_type, stack<>, allocator_descriptors_type>;
        using headers_object_type =
          object::object<headers_type, alloc::general_features, allocator_descriptors_type>;
        using body_object_type =
          object::object<body_type, alloc::general_features, allocator_descriptors_type>;
        using general_resource_type = typename headers_object_type::resource_type;

        [[no_unique_address]] general_resource_type alloc_resource{};
        headers_object_type                         headers;
        [[no_unique_address]] body_object_type      body;


        template <typename ET>
        requires(!stl::same_as<stl::remove_cvref_t<ET>, common_http_request> && // not if it's copy/move ctor
                 EnabledTraits<ET>)                                             // it's traits' enabled object
          constexpr common_http_request(ET&& et) noexcept
          : etraits{et},
            REL{},
            headers{et.alloc_pack, alloc_resource},
            body{et.alloc_pack, alloc_resource} {}

        constexpr common_http_request(common_http_request const&)     = default;
        constexpr common_http_request(common_http_request&&) noexcept = default;
        constexpr common_http_request& operator=(common_http_request const&) noexcept = default;
        constexpr common_http_request& operator=(common_http_request&&) noexcept = default;
    };

} // namespace webpp::http

#endif // WEBPP_COMMON_HTTP_REQUEST_HPP
