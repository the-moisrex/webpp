// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_BODY_HPP
#define WEBPP_REQUEST_BODY_HPP

#include "../extensions/extension.hpp"
#include "http_concepts.hpp"


namespace webpp::http {

    /**
     * Request body
     */
    template <typename TraitsType, HTTPRequestBodyCommunicator CommunicatorType, typename EList>
    struct request_body : public EList, public CommunicatorType {
        using traits_type                 = TraitsType;
        using request_body_extension_list = EList;

        template <typename... Args>
        constexpr request_body(Args&&... args) noexcept
          : request_body_extension_list{stl::forward<Args>(args)...} {}
    };

    template <typename ServerType>
    struct request_body_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_body_extensions;

        template <typename RootExtensions, typename TraitsType, typename BEList>
        using mid_level_extensie_type =
          request_body<TraitsType, typename ServerType::request_body_communicator, BEList>;
    };

    template <Traits TraitsType, typename ServerType>
    using simple_request_body =
      typename ServerType::root_extensions::template extensie_type<TraitsType,
                                                                   request_body_descriptor<ServerType>>;


} // namespace webpp::http

#endif // WEBPP_REQUEST_BODY_HPP
