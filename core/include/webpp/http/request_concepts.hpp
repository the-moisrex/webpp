//
// Created by moisrex on 5/9/20.
//

#ifndef WEBPP_REQUEST_CONCEPTS_H
#define WEBPP_REQUEST_CONCEPTS_H

#include "../extensions/extension.hpp"
#include "../logs/log_concepts.hpp"
#include "../std/string_concepts.hpp"
#include "../traits/enable_traits.hpp"

namespace webpp::http {

    /**
     * HTTP Request types:
     *
     * Initial Request: some protocols may use copy constructor to make a copy of the initial request object
     * instead of re-constructing the request from scratch each time. With this, it's possible to calculate
     * anything that you need for every request in the request's constructor.
     */
    template <typename T>
    concept HTTPRequest = requires(stl::remove_cvref_t<T> req) {
        requires EnabledTraits<stl::remove_cvref_t<T>>;
        requires Traits<typename stl::remove_cvref_t<T>::traits_type>;
        req.request_uri();

        // so we can make a copy of it (initial request)
        // requires stl::copy_constructible<stl::remove_cvref_t<T>>;

        // todo: should we uncomment this?
        // requires requires {
        //    stl::remove_cvref_t<T>{req};
        // };

        // requires Protocol<typename stl::remove_cvref_t<T>::protocol_type>
    };


    template <typename T>
    concept HTTPRequestExtension = Extension<T>;

    namespace details {
        template <typename E>
        struct is_request_extension_pack {
            static constexpr bool value = HTTPRequestExtension<E>;
        };
    } // namespace details

    template <typename T>
    concept HTTPRequestExtensionList = ExtensionListOf<T, details::is_request_extension_pack>;

    template <typename T>
    concept HTTPRequestExtensionParent = HTTPRequestExtension<T>;


    // todo: complete this
    template <typename T>
    concept HTTPRequestBodyExtensionList = ExtensionList<T>;


} // namespace webpp::http

#endif // WEBPP_REQUEST_CONCEPTS_H
