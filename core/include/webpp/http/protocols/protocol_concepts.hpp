// Created by moisrex on 5/9/20.

#ifndef WEBPP_PROTOCOL_CONCEPTS_H
#define WEBPP_PROTOCOL_CONCEPTS_H

#include "../../application/application_concepts.hpp"
#include "../../traits/enable_traits.hpp"
#include "../request_concepts.hpp"

namespace webpp::http {

    /**
     * Protocol is a "Protocol Type" based on the information that I said in the "server/server_concepts"
     * file.
     */
    template <typename T>
    concept HTTPProtocol = requires(T proto) {
        requires EnabledTraits<T>;
        requires HTTPRequest<typename T::request_type>;
        requires Application<typename T::application_type>;
        requires ApplicationWrapper<typename T::app_wrapper_type>;
        { proto.app } -> ApplicationWrapper; // get the app
        // should be able to pass an app to it as well

        { proto.is_ssl_available() } -> stl::same_as<bool>;
        { proto() } -> stl::same_as<int>;
    };

    // todo
    template <typename T>
    concept HTTPProtocolExtensionList = ExtensionList<T>;


    struct http_protocol_descriptor {
        template <RootExtensionList REL>
        using related_extension_pack_type = typename REL::protocol_extensions;
    };

    // this will apply only the "Mother Extension" and gives you the result of that.
    // this does not apply the child extensions
    template <Traits TraitsType, RootExtensionList REList>
    using apply_protocol_extensions =
      typename REList::template mother_extensie_type<TraitsType, http_protocol_descriptor>;

} // namespace webpp::http

#endif // WEBPP_PROTOCOL_CONCEPTS_H
