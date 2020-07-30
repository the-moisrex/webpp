//
// Created by moisrex on 5/9/20.
//

#ifndef WEBPP_REQUEST_CONCEPTS_H
#define WEBPP_REQUEST_CONCEPTS_H

#include "../extensions/extension.hpp"
#include "../traits/std_traits.hpp"
#include "./interfaces/basic_interface_concepts.hpp"

namespace webpp {

    template <typename T>
    concept Request = Traits<typename T::traits_type>&&
      Interface<typename T::interface_type>&& requires(T req) {
        {req.request_uri()};
    };


    template <typename T>
    concept RequestExtension = Extension<T>;

    template <typename E>
    struct is_request_extension_pack {
        static constexpr bool value = RequestExtension<E>;
    };

    template <typename T>
    concept RequestExtensionList = ExtensionListOf<T, is_request_extension_pack>;

    struct fake_request_type {
        using interface_type = fake_interface_type;
        using traits_type    = fake_traits_type;

        auto request_uri() const noexcept {
            return "/";
        }
    };

} // namespace webpp

#endif // WEBPP_REQUEST_CONCEPTS_H
