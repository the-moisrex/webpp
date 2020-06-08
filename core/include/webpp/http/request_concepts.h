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

    template <typename E>
    struct is_request_extension_pack {
        static constexpr bool value = RequestExtension<E>;
    };

    template <typename T>
    concept RequestExtensionList =
      ExtensionListOf<T, is_request_extension_pack>;


} // namespace webpp

#endif // WEBPP_REQUEST_CONCEPTS_H
