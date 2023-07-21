// Created by moisrex on 8/30/20.

#ifndef WEBPP_ENABLE_TRAITS_HPP
#define WEBPP_ENABLE_TRAITS_HPP

#include "../std/tuple.hpp"
#include "traits.hpp"

namespace webpp {

    template <typename TraitsType>
    struct enable_traits;

    template <Traits TraitsType>
    struct enable_owner_traits {
        using traits_type         = TraitsType;
        using non_owner_type      = enable_traits<traits_type>;
        using logger_type         = traits::logger<traits_type>;
        using logger_ref          = typename logger_type::logger_ref;
        using string_view_type    = traits::string_view<traits_type>;
        using char_type           = istl::char_type_of_t<string_view_type>;
        using string_type         = traits::general_string<traits_type>;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;
        using alloc_pack_ref =
          stl::conditional_t<sizeof(allocator_pack_type) <= sizeof(allocator_pack_type*) &&
                               stl::is_trivially_copy_constructible_v<allocator_pack_type>,
                             allocator_pack_type,
                             allocator_pack_type&>;
        static constexpr bool is_resource_owner = true;

#ifdef DEBUG
        static constexpr bool debug = true;
#else
        static constexpr bool debug = false;
#endif

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        [[no_unique_address]] allocator_pack_type alloc_pack{};
        [[no_unique_address]] logger_type         logger{};
        // NOLINTEND(misc-non-private-member-variables-in-classes)


        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        // a copy constructor essentially; works on enable_traits as well
        template <typename T>
            requires(!stl::same_as<stl::remove_cvref_t<T>, enable_owner_traits> && EnabledTraits<T>)
        constexpr enable_owner_traits(T&& obj) noexcept : alloc_pack{obj.alloc_pack},
                                                          logger{obj.logger} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr enable_owner_traits(alloc_pack_ref alloc_pack_obj, logger_ref logger_obj = {}) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr enable_owner_traits(logger_ref logger_obj, alloc_pack_ref alloc_pack_obj) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr enable_owner_traits() noexcept(
          stl::is_nothrow_default_constructible_v<allocator_pack_type>&&
            stl::is_nothrow_default_constructible_v<logger_type>)          = default;
        constexpr enable_owner_traits(enable_owner_traits const&) noexcept = default;
        constexpr enable_owner_traits(enable_owner_traits&&) noexcept      = default;
        constexpr ~enable_owner_traits()                                   = default;

        constexpr enable_owner_traits& operator=(enable_owner_traits const& rhs) {
            if (this != &rhs) {
                logger     = rhs.logger;
                alloc_pack = rhs.alloc_pack;
            }
            return *this;
        }

        constexpr enable_owner_traits& operator=(enable_owner_traits&& rhs) noexcept {
            logger     = stl::move(rhs.logger);
            alloc_pack = rhs.alloc_pack;
            return *this;
        }

        constexpr void swap(EnabledTraits auto& other) noexcept {
            using stl::swap;
            swap(alloc_pack, other.alloc_pack);
            swap(logger, other.logger);
        }

        // when this object is a mother of another class, this method can help get the traits' object.
        constexpr enable_owner_traits& get_traits() noexcept {
            return *this;
        }

        constexpr enable_owner_traits const& get_traits() const noexcept {
            return *this;
        }
    };


    /**
     * By inheriting from this you'll make your type "TraitsEnabled".
     */
    template <typename TraitsType>
    struct enable_traits {
        using traits_type         = TraitsType;
        using non_owner_type      = enable_traits<traits_type>;
        using enable_traits_type  = enable_traits;
        using logger_type         = traits::logger<traits_type>;
        using logger_ref          = typename logger_type::logger_ref;
        using string_view_type    = traits::string_view<traits_type>;
        using char_type           = istl::char_type_of_t<string_view_type>;
        using string_type         = traits::general_string<traits_type>;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;
        using alloc_pack_ref =
          stl::conditional_t<sizeof(allocator_pack_type) <= sizeof(allocator_pack_type*) &&
                               stl::is_trivially_copy_constructible_v<allocator_pack_type>,
                             allocator_pack_type,
                             allocator_pack_type&>;

        static_assert(Traits<traits_type>, "The specified TraitsType is not of a valid Traits.");

        static constexpr bool is_resource_owner = false;

#ifdef DEBUG
        static constexpr bool debug = true;
#else
        static constexpr bool debug = false;
#endif

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        [[no_unique_address]] alloc_pack_ref alloc_pack;
        [[no_unique_address]] logger_ref     logger;
        // NOLINTEND(misc-non-private-member-variables-in-classes)

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        // a copy constructor essentially; works on enable_owner_traits as well
        template <typename T>
            requires(!stl::same_as<stl::remove_cvref_t<T>, enable_traits> && EnabledTraits<T>)
        constexpr enable_traits(T&& obj) noexcept : alloc_pack{obj.alloc_pack},
                                                    logger{obj.logger} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr enable_traits(alloc_pack_ref alloc_pack_obj, logger_ref logger_obj = {}) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr enable_traits(logger_ref logger_obj, alloc_pack_ref alloc_pack_obj) noexcept
          : alloc_pack{alloc_pack_obj},
            logger{logger_obj} {}

        constexpr enable_traits(enable_traits const&) noexcept = default;
        constexpr enable_traits(enable_traits&&) noexcept      = default;
        constexpr ~enable_traits()                             = default;

        constexpr enable_traits& operator=(enable_traits const& rhs) {
            if (this != &rhs) {
                logger     = rhs.logger;
                alloc_pack = rhs.alloc_pack;
            }
            return *this;
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

        // when this object is a mother of another class, this method can help get the traits object.
        constexpr enable_traits& get_traits() noexcept {
            return *this;
        }

        constexpr enable_traits const& get_traits() const noexcept {
            return *this;
        }
    };


    /**
     * If the passed type is already a enable_owner_traits or enable_traits,
     * then we can just omit using them and convert them into non-owner enable traits.
     */
    template <Traits TraitsType>
    struct enable_traits<enable_owner_traits<TraitsType>> : enable_traits<TraitsType> {
        using enable_traits<TraitsType>::enable_traits;
    };

    template <Traits TraitsType>
    struct enable_traits<enable_traits<TraitsType>> : enable_traits<TraitsType> {
        using enable_traits<TraitsType>::enable_traits;
    };



    /**
     * Inheriting from this means you can inherit from type T and we make sure either T is traits' enabled,
     * or if it's not, we provide one for you.
     */
    template <Traits TraitsType, typename T>
    struct enable_traits_with : public T, public enable_traits<TraitsType> {
        using enabled_type = T;
        using enable_traits<TraitsType>::enable_traits;

        constexpr enable_traits_with(enable_traits<TraitsType> const& et) noexcept
          : enable_traits<TraitsType>{et} {}
        constexpr enable_traits_with(enable_traits<TraitsType>&& et) noexcept
          : enable_traits<TraitsType>{stl::move(et)} {}

        constexpr enable_traits_with(enable_traits_with const&) noexcept            = default;
        constexpr enable_traits_with(enable_traits_with&&) noexcept                 = default;
        constexpr ~enable_traits_with() noexcept                                    = default;
        constexpr enable_traits_with& operator=(enable_traits_with const&) noexcept = default;
        constexpr enable_traits_with& operator=(enable_traits_with&&) noexcept      = default;
    };

    template <Traits TraitsType, EnabledTraits T>
        requires(EnabledTraitsOf<TraitsType, T>)
    struct enable_traits_with<TraitsType, T> : public T {
        using enabled_type = T;
        using T::T;

        constexpr enable_traits_with(enable_traits_with const&) noexcept            = default;
        constexpr enable_traits_with(enable_traits_with&&) noexcept                 = default;
        constexpr ~enable_traits_with() noexcept                                    = default;
        constexpr enable_traits_with& operator=(enable_traits_with const&) noexcept = default;
        constexpr enable_traits_with& operator=(enable_traits_with&&) noexcept      = default;
    };

    /**
     * If
     *  - traits type:          enable owner traits
     *  - enable traits type:   enable traits
     *  - enable owner traits:  enable owner traits
     * It's almost the opposite of enable_traits itself which only holds a reference, this one holds the
     * owner too if a enable_traits is not passed to it.
     */
    template <typename TraitsType>
    struct enable_traits_access;

    template <typename T>
        requires requires {
                     typename T::traits_type;
                     requires Traits<typename T::traits_type>;
                 }
    struct enable_traits_access<T> : public enable_traits_with<typename T::traits_type, T> {
        using enable_traits_with<typename T::traits_type, T>::enable_traits_with;
    };

    template <Traits TraitsType>
    struct enable_traits_access<TraitsType> : public enable_owner_traits<TraitsType> {
        using enable_owner_traits<TraitsType>::enable_owner_traits;
    };

    template <Traits TraitsType>
    struct enable_traits_access<enable_traits<TraitsType>> : public enable_traits<TraitsType> {
        using enable_traits<TraitsType>::enable_traits;
    };

    template <Traits TraitsType>
    struct enable_traits_access<enable_owner_traits<TraitsType>> : public enable_owner_traits<TraitsType> {
        using enable_owner_traits<TraitsType>::enable_owner_traits;
    };

    template <Traits TraitsType, typename T>
    struct enable_traits_access<enable_traits_with<TraitsType, T>>
      : public enable_traits_with<TraitsType, T> {
        using enable_traits_with<TraitsType, T>::enable_traits_with;
    };


    /**
     * Make T an owner
     * Usages:
     * @code
     *   enable_traits_for<my_class> obj;
     * @endcode
     */
    template <typename T, typename TraitsType = typename T::traits_type>
    struct enable_traits_for;

    template <EnabledTraits T>
        requires(T::is_resource_owner)
    struct enable_traits_for<T> : T {
        using T::T;
    };

    template <typename T, Traits TraitsType>
    struct enable_traits_for<T, TraitsType> : T {
        using etraits = enable_owner_traits<TraitsType>;

      private:
        etraits et{};

      public:
        using T::T;

        template <typename... Args>
            requires(stl::is_constructible_v<T, etraits, Args...>)
        constexpr enable_traits_for(Args&&... args) noexcept(
          stl::is_nothrow_constructible_v<T, etraits, Args...>)
          : T{et, stl::forward<Args>(args)...} {}

        // pass a general allocator as the first argument
        template <typename... Args>
            requires(
              !stl::is_constructible_v<T, etraits, Args...> &&
              requires {
                  typename T::allocator_type;
                  requires stl::is_constructible_v<T, typename T::allocator_type const&, Args...>;
                  requires etraits::allocator_pack_type::template has_allocator<typename T::allocator_type>;
              })
        constexpr enable_traits_for(Args&&... args) noexcept(
          stl::is_nothrow_constructible_v<T, typename T::allocator_type const&, Args...>)
          : T{alloc::general_alloc_for<T>(et), stl::forward<Args>(args)...} {}


        constexpr enable_traits_for(enable_traits_for const&) noexcept            = default;
        constexpr enable_traits_for(enable_traits_for&&) noexcept                 = default;
        constexpr ~enable_traits_for() noexcept                                   = default;
        constexpr enable_traits_for& operator=(enable_traits_for const&) noexcept = default;
        constexpr enable_traits_for& operator=(enable_traits_for&&) noexcept      = default;
    };



    template <typename T>
    concept TraitsAccess =
      Traits<T> ||
      requires {
          typename T::traits_type;
          requires Traits<typename T::traits_type>;
          requires stl::same_as<T, enable_traits<typename T::traits_type>> ||
                     stl::same_as<T, enable_owner_traits<typename T::traits_type>> ||
                     requires {
                         typename T::enabled_type;
                         requires stl::
                           same_as<T, enable_traits_with<typename T::traits_type, typename T::enabled_type>>;
                     };
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

        template <feature_pack FPack, typename T, AllocatorHolder AllocHolder>
        static constexpr auto featured_alloc_for(AllocHolder& holder) noexcept {
            return holder.alloc_pack.template featured_alloc_for<FPack, T>();
        }

        template <feature_pack FPack, typename T, AllocatorHolder AllocHolder>
        static constexpr auto featured_alloc(AllocHolder& holder) noexcept {
            return holder.alloc_pack.template get_allocator<FPack, T>();
        }

        template <typename T, AllocatorHolder AllocHolder>
        static constexpr auto local_alloc_for(AllocHolder& holder) noexcept {
            return holder.alloc_pack.template local_alloc_for<T>();
        }

        template <typename T, AllocatorHolder AllocHolder>
        static constexpr auto general_alloc_for(AllocHolder& holder) noexcept {
            return holder.alloc_pack.template general_alloc_for<T>();
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
