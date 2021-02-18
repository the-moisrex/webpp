// Created by moisrex on 8/30/20.

#ifndef WEBPP_ENABLE_TRAITS_HPP
#define WEBPP_ENABLE_TRAITS_HPP

#include "../std/tuple.hpp"
#include "traits.hpp"

namespace webpp {

    template <Traits TraitsType>
    struct enable_owner_traits {
        using traits_type         = TraitsType;
        using logger_type         = traits::logger<traits_type>;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;

        [[no_unique_address]] allocator_pack_type alloc_pack{};
        [[no_unique_address]] logger_type         logger{};


        // template <typename... ResType>
        // requires((allocator_pack_type::template has_resource_object<ResType> &&
        //           ...)) // check if the allocator pack has the resources
        //   constexpr explicit enable_traits(logger_ref logger_obj = logger_type{},
        //                                    ResType&... resources) noexcept
        //   : logger{logger_obj},
        //           alloc_pack{stl::forward<ResType>(resources)...} {}
    };


    /**
     * By inheriting from this you'll make your type "TraitsEnabled".
     */
    template <Traits TraitsType>
    struct enable_traits {
        using traits_type         = TraitsType;
        using logger_type         = traits::logger<traits_type>;
        using logger_ref          = typename logger_type::logger_ref;
        using string_view_type    = traits::string_view<traits_type>;
        using char_type           = istl::char_type_of<string_view_type>;
        using string_type         = traits::general_string<traits_type>;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;
        using alloc_pack_ref =
          stl::conditional_t<sizeof(allocator_pack_type) <= sizeof(allocator_pack_type*) &&
                               stl::is_trivially_copy_constructible_v<allocator_pack_type>,
                             allocator_pack_type,
                             allocator_pack_type&>;

        [[no_unique_address]] alloc_pack_ref alloc_pack;
        [[no_unique_address]] logger_ref     logger;

        // a copy constructor essentially; works on enable_owner_traits as well
        template <typename T>
        requires(!stl::same_as<stl::remove_cvref_t<T>, enable_traits> &&
                 requires(stl::remove_cvref_t<T> et) {
                     requires EnabledTraits<stl::remove_cvref_t<T>>;
                     requires stl::same_as<typename stl::remove_cvref_t<T>::traits_type, traits_type>;
                     requires stl::same_as<typename stl::remove_cvref_t<T>::logger_type, logger_type>;
                     requires stl::same_as<typename stl::remove_cvref_t<T>::allocator_pack_type,
                                           allocator_pack_type>;
                 }) constexpr enable_traits(T&& obj) noexcept
          : alloc_pack{obj.alloc_pack},
            logger{obj.logger} {}

        constexpr enable_traits(alloc_pack_ref alloc_pack_obj, logger_ref logger_obj = {}) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr enable_traits(logger_ref logger_obj, alloc_pack_ref alloc_pack_obj) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr explicit enable_traits(enable_traits const&) noexcept = default;
        constexpr explicit enable_traits(enable_traits&&) noexcept      = default;
        constexpr enable_traits& operator=(enable_traits const&) noexcept = default;
        constexpr enable_traits& operator=(enable_traits&&) noexcept = default;

        constexpr void swap(EnabledTraits auto& other) noexcept {
            using stl::swap;
            swap(alloc_pack, other.alloc_pack);
            swap(logger, other.logger);
        }
    };



    template <Traits TraitsType, typename T>
    struct enable_traits_with : public T, public enable_traits<TraitsType> {
        using enable_traits<TraitsType>::enable_traits;
    };

    template <Traits TraitsType, EnabledTraits T>
    struct enable_traits_with<TraitsType, T> : public T {
        using T::T;
    };

} // namespace webpp

#endif // WEBPP_ENABLE_TRAITS_HPP
