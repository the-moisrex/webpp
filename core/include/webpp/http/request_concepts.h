//
// Created by moisrex on 5/9/20.
//

#ifndef WEBPP_REQUEST_CONCEPTS_H
#define WEBPP_REQUEST_CONCEPTS_H

#include "../extensions/extension.h"
#include "../traits/traits_concepts.h"
#include "./interfaces/basic_interface_concepts.h"

namespace webpp {

    template <typename T>
    concept Request = requires(T x) {
        typename T::traits;
        typename T::interface;
    }
    &&Traits<typename T::traits>&& Interface<typename T::interface>;


    template <typename T>
    concept RequestExtension = Extension<T>;


    template <typename... T>
    struct is_request_extension_list {
        static constexpr bool value = false;
    };

    template <RequestExtension... T>
    struct is_request_extension_list<::std::tuple<T...>> {
        static constexpr bool value = true;
    };

    template <RequestExtension... T>
    struct is_request_extension_list<typelist<T...>> {
        static constexpr bool value = true;
    };

    template <RequestExtension... T>
    struct is_request_extension_list<extension_pack<T...>> {
        static constexpr bool value = true;
    };

    template <typename T>
    concept RequestExtensionList = ExtensionList<T, is_request_extension_list>;


} // namespace webpp

#endif // WEBPP_REQUEST_CONCEPTS_H
