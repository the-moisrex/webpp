// Created by moisrex on 8/30/20.

#ifndef WEBPP_ENABLE_TRAITS_HPP
#define WEBPP_ENABLE_TRAITS_HPP

#include "../std/tuple.hpp"
#include "traits.hpp"

namespace webpp {

    /**
     * By inheriting from this you'll make your type "TraitsEnabled".
     */
    template <Traits TraitsType, bool AllocOwner = false>
    struct enable_traits {
        using traits_type         = TraitsType;
        using logger_type         = traits::logger<traits_type>;
        using logger_ref          = typename logger_type::logger_ref;
        using string_view_type    = traits::string_view<traits_type>;
        using char_type           = istl::char_type_of<string_view_type>;
        using string_type         = traits::general_string<traits_type>;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;
        using alloc_pack_ref      = stl::conditional_t<AllocOwner, allocator_pack_type, allocator_pack_type&>;

        [[no_unique_address]] alloc_pack_ref alloc_pack{};
        [[no_unique_address]] logger_ref     logger{};

        // a copy constructor essentially
        template <typename T>
        requires requires(T et) {
            et.alloc_pack;
            et.logger;
            requires stl::same_as<typename T::traits_type, traits_type>;
            requires stl::same_as<typename T::logger_type, logger_type>;
            requires stl::same_as<typename T::allocator_pack_type, allocator_pack_type>;
        }
        constexpr enable_traits(T& obj) noexcept : alloc_pack{obj.alloc_pack}, logger{obj.logger} {}

        constexpr enable_traits(allocator_pack_type& alloc_pack_obj,
                                logger_ref           logger_obj = logger_type{}) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr enable_traits(logger_ref logger_obj, allocator_pack_type& alloc_pack_obj) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        // template <typename... ResType>
        // requires((allocator_pack_type::template has_resource_object<ResType> &&
        //           ...)) // check if the allocator pack has the resources
        //   constexpr explicit enable_traits(logger_ref logger_obj = logger_type{},
        //                                    ResType&... resources) noexcept
        //   : logger{logger_obj},
        //           alloc_pack{stl::forward<ResType>(resources)...} {}


        constexpr enable_traits()                         = default;
        constexpr enable_traits(enable_traits const&)     = default;
        constexpr enable_traits(enable_traits&&) noexcept = default;
        constexpr enable_traits& operator=(enable_traits const&) = default;
        constexpr enable_traits& operator=(enable_traits&&) noexcept = default;

        constexpr void swap(EnabledTraits auto& other) noexcept {
            using stl::swap;
            swap(alloc_pack, other.alloc_pack);
            swap(logger, other.logger);
        }
    };

} // namespace webpp

#endif // WEBPP_ENABLE_TRAITS_HPP
