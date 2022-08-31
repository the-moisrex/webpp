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


        // when this object is a mother of another class, this method can help get the tratis object.
        constexpr enable_owner_traits& get_traits() noexcept {
            return *this;
        }

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
                     })
        constexpr enable_traits(T&& obj) noexcept : alloc_pack{obj.alloc_pack},
                                                    logger{obj.logger} {}

        constexpr enable_traits(alloc_pack_ref alloc_pack_obj, logger_ref logger_obj = {}) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr enable_traits(logger_ref logger_obj, alloc_pack_ref alloc_pack_obj) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr explicit enable_traits(enable_traits const&) noexcept = default;
        constexpr explicit enable_traits(enable_traits&&) noexcept      = default;

        constexpr enable_traits& operator=(enable_traits const& rhs) {
            logger     = rhs.logger;
            alloc_pack = rhs.alloc_pack;
        }

        constexpr enable_traits& operator=(enable_traits&& rhs) noexcept {
            logger     = stl::move(rhs.logger);
            alloc_pack = rhs.alloc_pack;
            return *this;
        }

        constexpr void swap(EnabledTraits auto& other) noexcept {
            using stl::swap;
            swap(alloc_pack, other.alloc_pack);
            swap(logger, other.logger);
        }

        // when this object is a mother of another class, this method can help get the tratis object.
        constexpr enable_traits& get_traits() noexcept {
            return *this;
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



    // I added these here because they are traits' related and also allocator pack related, but their intent
    // is to simplify the users of the traits not allocator packs directly
    namespace alloc {

        /**
         * Check if the specified type T holds an allocator
         */
        template <typename T>
        concept AllocatorHolder = requires(T holder) {
                                      typename T::allocator_pack_type;
                                      requires AllocatorPack<typename T::allocator_pack_type>;
                                      { holder.alloc_pack } -> AllocatorPack;
                                  };

        template <typename T, AllocatorHolder AllocHolder>
        static constexpr auto local_allocator(AllocHolder& holder) noexcept {
            return holder.alloc_pack.template local_allocator<T>();
        }

        template <typename T, AllocatorHolder AllocHolder>
        static constexpr auto general_allocator(AllocHolder& holder) noexcept {
            return holder.alloc_pack.template general_allocator<T>();
        }

        template <typename T, AllocatorHolder AllocHolder>
        static constexpr auto allocator_for(AllocHolder& holder) noexcept {
            return holder.alloc_pack.template get_allocator_for<T>();
        }

        template <typename T, AllocatorHolder AllocHolder>
        static constexpr auto local_alloc_for(AllocHolder& holder) noexcept {
            return holder.alloc_pack.template local_alloc_for<T>();
        }

        template <typename T, AllocatorHolder AllocHolder, typename... Args>
        static constexpr auto allocate_unique_general(AllocHolder& holder, Args&&... args) noexcept {
            return holder.alloc_pack.template allocate_unique_general<T>(stl::forward<Args>(args)...);
        }

        template <typename T, AllocatorHolder AllocHolder, typename... Args>
        static constexpr auto allocate_unique_local(AllocHolder& holder, Args&&... args) noexcept {
            return holder.alloc_pack.template allocate_unique_local<T>(stl::forward<Args>(args)...);
        }

    } // namespace alloc


} // namespace webpp

#endif // WEBPP_ENABLE_TRAITS_HPP
