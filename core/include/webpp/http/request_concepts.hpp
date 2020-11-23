//
// Created by moisrex on 5/9/20.
//

#ifndef WEBPP_REQUEST_CONCEPTS_H
#define WEBPP_REQUEST_CONCEPTS_H

#include "../extensions/extension.hpp"
#include "../logs/log_concepts.hpp"
#include "../traits/enable_traits.hpp"
#include "../traits/std_traits.hpp"
//#include "./protocols/protocol_concepts.hpp"

namespace webpp {

    /**
     * Request types:
     *
     * Initial Request: some protocols may use copy constructor to make a copy of the initial request object
     * instead of re-constructing the request from scratch each time. With this, it's possible to calculate
     * anything that you need for every request in the request's constructor.
     */
    template <typename T>
    concept Request = requires(stl::remove_cvref_t<T> req) {
        /* requires Protocol<typename stl::remove_cvref_t<T>::protocol_type>&& */
        requires Traits<typename stl::remove_cvref_t<T>::traits_type>;
        requires EnabledTraits<stl::remove_cvref_t<T>>;
        requires stl::copy_constructible<stl::remove_cvref_t<T>>; // so we can make a copy of it (initial request)
        req.request_uri();
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
        struct protocol_type {
            protocol_type() = default;
            struct app_type {} app;
        };
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
