#ifndef WEBPP_REQUEST_H
#define WEBPP_REQUEST_H

#include "../memory/object.hpp"
#include "../std/format.hpp"
#include "../traits/enable_traits.hpp"
#include "../version.hpp"
#include "./headers.hpp"
#include "./protocols/common/common_http_request.hpp"
#include "./protocols/protocol_concepts.hpp"
#include "./request_body.hpp"
#include "./request_concepts.hpp"
#include "./request_headers.hpp"
#include "./response_body.hpp"
#include "./version.hpp"

/**
 * Only the protocol should be instantiating this class. There should be no
 * need for anybody else to do it.
 *
 * The user has access to this class. This class should be accessed as a const
 * variable since the user should not be able to make any changes here. Every
 * action that the user has to make should be in the "response" class or other
 * relative classes that finally end up in that class. This class is only for
 * giving information that the user or other modules need.
 *
 */

namespace webpp::http {


    template <Traits TraitsType, HTTPRequestExtensionParent REL, typename ServerType>
    struct common_http_request : public enable_traits<TraitsType>, public REL {
        using server_type                = ServerType;
        using server_ref                 = stl::add_lvalue_reference_t<server_type>;
        using root_extensions            = typename server_type::root_extensions;
        using traits_type                = TraitsType;
        using request_extension_list     = REL;
        using etraits                    = enable_traits<traits_type>;
        using string_type                = traits::general_string<traits_type>;
        using string_view_type           = traits::string_view<traits_type>;
        using char_type                  = traits::char_type<traits_type>;
        using allocator_descriptors_type = traits::allocator_descriptors<traits_type>;
        using local_allocator_type       = traits::local_allocator<traits_type, char_type>;
        using general_allocator_type     = traits::general_allocator<traits_type, char_type>;
        using request_extensions         = REL;
        using headers_type = simple_request_headers<traits_type, root_extensions, local_allocator_type>;
        using body_type    = simple_request_body<traits_type, root_extensions>;

        // todo: are we using the right object here?
        using headers_object_type =
          object::object<headers_type, alloc::general_features, allocator_descriptors_type>;
        using body_object_type =
          object::object<body_type, alloc::general_features, allocator_descriptors_type>;
        using general_resource_type = typename headers_object_type::resource_type_field;

        [[no_unique_address]] general_resource_type
                            alloc_resource{};          // NOLINT(misc-non-private-member-variables-in-classes)
        headers_object_type headers;                   // NOLINT(misc-non-private-member-variables-in-classes)
        [[no_unique_address]] body_object_type body;   // NOLINT(misc-non-private-member-variables-in-classes)
        server_ref                             server; // NOLINT(misc-non-private-member-variables-in-classes)

        constexpr common_http_request(server_ref inp_server) noexcept
          : etraits{inp_server},
            REL{},
            headers{inp_server.alloc_pack, alloc_resource},
            body{inp_server.alloc_pack, alloc_resource},
            server{inp_server} {}


        template <typename ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>,
                                   common_http_request> && // not if it's copy/move ctor
                     EnabledTraits<ET>)                    // it's traits' enabled object
        constexpr common_http_request(ET&& et) noexcept
          : etraits{et},
            REL{},
            headers{et.alloc_pack, alloc_resource},
            body{et.alloc_pack, alloc_resource} {}

        constexpr common_http_request(common_http_request const&)                     = default;
        constexpr common_http_request(common_http_request&&) noexcept                 = default;
        constexpr common_http_request& operator=(common_http_request const&) noexcept = default;
        constexpr common_http_request& operator=(common_http_request&&) noexcept      = default;


        /**
         * Get the Web++ Library version
         */
        [[nodiscard]] string_view_type framework_version() const noexcept {
            return webpp_version;
        }
    };

    /**
     * If you want to add features to all of the request types, you can use this type
     */
    template <Traits TraitsType, typename ReqType>
    struct final_request final : public ReqType {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;

        using ReqType::ReqType;

        constexpr final_request(final_request const&)                = default;
        constexpr final_request(final_request&&) noexcept            = default;
        constexpr final_request& operator=(final_request const&)     = default;
        constexpr final_request& operator=(final_request&&) noexcept = default;
    };

    template <template <typename...> typename MidLevelRequestType, typename ServerType>
    struct request_descriptor {
        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_extensions;

        template <RootExtensionList RootExtensions,
                  Traits            TraitsType,
                  typename RequestEList,
                  typename... extra>
        using mid_level_extensie_type =
          MidLevelRequestType<TraitsType, common_http_request<TraitsType, RequestEList, ServerType>>;

        // empty final extensie
        template <RootExtensionList RootExtensions,
                  Traits            TraitsType,
                  typename MidLevelRequestWithExtensions,
                  typename... extra>
        using final_extensie_type = final_request<TraitsType, MidLevelRequestWithExtensions>;
    };


    template <typename ServerType, template <typename...> typename MidLevelRequestType>
    using simple_request = typename ServerType::root_extensions::template extensie_type<
      typename ServerType::traits_type,
      request_descriptor<MidLevelRequestType, ServerType>>;


} // namespace webpp::http

#endif // WEBPP_REQUEST_H
