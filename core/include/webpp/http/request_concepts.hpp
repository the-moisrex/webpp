//
// Created by moisrex on 5/9/20.
//

#ifndef WEBPP_REQUEST_CONCEPTS_H
#define WEBPP_REQUEST_CONCEPTS_H

#include "../extensions/extension.hpp"
#include "../logs/log_concepts.hpp"
#include "../traits/enable_traits.hpp"
#include "../traits/std_traits.hpp"
#include "./protocols/protocol_concepts.hpp"

namespace webpp {

    template <typename T>
    concept Request = Traits<typename stl::remove_cvref_t<T>::traits_type>&&
      Protocol<typename stl::remove_cvref_t<T>::interface_type>&& requires(stl::remove_cvref_t<T> req) {
        req.request_uri();
        req.get_allocator();
    };


    template <typename T>
    concept RequestExtension = Extension<T>;

    template <typename E>
    struct is_request_extension_pack {
        static constexpr bool value = RequestExtension<E>;
    };

    template <typename T>
    concept RequestExtensionList = ExtensionListOf<T, is_request_extension_pack>;

    struct fake_request_type : public enable_traits<fake_traits_type> {
        struct interface_type {};
        using traits_type    = fake_traits_type;
        using allocator_type = typename traits_type::template allocator<typename traits_type::char_type>;

        auto request_uri() const noexcept {
            return "/";
        }

        auto get_allocator() const noexcept {
            return allocator_type{};
        }
    };

} // namespace webpp

#endif // WEBPP_REQUEST_CONCEPTS_H
