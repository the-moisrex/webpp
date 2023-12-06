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
        using traits_type      = TraitsType;
        using non_owner_type   = enable_traits<traits_type>;
        using logger_type      = traits::logger<traits_type>;
        using logger_ref       = typename logger_type::logger_ref;
        using string_view_type = traits::string_view<traits_type>;
        using char_type        = typename string_view_type::value_type;
        using string_type      = traits::general_string<traits_type>;

        template <typename T = stl::byte>
        using general_allocator_type = traits::general_allocator<traits_type, T>;

        template <typename T = stl::byte>
        using monotonic_allocator_type = traits::local_allocator<traits_type, T>;

        static constexpr bool is_resource_owner = true;

#ifdef DEBUG
        static constexpr bool debug = true;
#else
        static constexpr bool debug = false;
#endif

      private:
        [[no_unique_address]] general_allocator_type<stl::byte> alloc =
          traits_type::general_allocator_descriptor::template construct_allocator<stl::byte>();

      public:
        // NOLINTBEGIN(*-non-private-member-variables-in-classes)
        [[no_unique_address]] logger_type logger{};

        // NOLINTEND(*-non-private-member-variables-in-classes)


        // NOLINTBEGIN(*-forwarding-reference-overload)

        // a copy constructor essentially; works on enable_traits as well
        template <typename T>
            requires(!stl::same_as<stl::remove_cvref_t<T>, enable_owner_traits> && EnabledTraits<T>)
        explicit constexpr enable_owner_traits(T&& obj) noexcept
          : alloc{obj.alloc},
            logger{stl::forward<T>(obj).logger} {}

        explicit constexpr enable_owner_traits(general_allocator_type<stl::byte> const& inp_alloc) noexcept
          : alloc{inp_alloc} {}

        // NOLINTEND(*-forwarding-reference-overload)

        explicit constexpr enable_owner_traits(logger_ref logger_obj) noexcept : logger{logger_obj} {}

        constexpr enable_owner_traits() noexcept(
          stl::is_nothrow_default_constructible_v<logger_type>)            = default;
        constexpr enable_owner_traits(enable_owner_traits const&) noexcept = default;
        constexpr enable_owner_traits(enable_owner_traits&&) noexcept      = default;
        constexpr ~enable_owner_traits()                                   = default;

        constexpr enable_owner_traits& operator=(enable_owner_traits const& rhs) {
            if (this != &rhs) {
                logger = rhs.logger;
            }
            return *this;
        }

        constexpr enable_owner_traits& operator=(enable_owner_traits&& rhs) noexcept {
            logger = stl::move(rhs.logger);
            return *this;
        }

        constexpr void swap(EnabledTraits auto& other) noexcept {
            using stl::swap;
            swap(logger, other.logger);
        }

        // when this object is a mother of another class, this method can help get the traits' object.
        constexpr enable_owner_traits& get_traits() noexcept {
            return *this;
        }

        constexpr enable_owner_traits const& get_traits() const noexcept {
            return *this;
        }

        template <typename T>
        [[nodiscard]] constexpr general_allocator_type<T> general_allocator() const noexcept {
            return general_allocator_type<T>{construct_allocator_from(alloc)};
        }
    };

    /**
     * By inheriting from this you'll make your type "TraitsEnabled".
     */
    template <typename TraitsType>
    struct enable_traits {
        using traits_type        = TraitsType;
        using non_owner_type     = enable_traits<traits_type>;
        using enable_traits_type = enable_traits;
        using logger_type        = traits::logger<traits_type>;
        using logger_ref         = typename logger_type::logger_ref;
        using string_view_type   = traits::string_view<traits_type>;
        using char_type          = typename string_view_type::value_type;
        using string_type        = traits::general_string<traits_type>;

        template <typename T = stl::byte>
        using general_allocator_type = traits::general_allocator<traits_type, T>;

        template <typename T = stl::byte>
        using monotonic_allocator_type = traits::local_allocator<traits_type, T>;

        static_assert(Traits<traits_type>, "The specified TraitsType is not of a valid Traits.");

        static constexpr bool is_resource_owner = false;

#ifdef DEBUG
        static constexpr bool debug = true;
#else
        static constexpr bool debug = false;
#endif

      private:
        [[no_unique_address]] general_allocator_type<stl::byte> alloc;

      public:
        // NOLINTBEGIN(*-non-private-member-variables-in-classes)
        [[no_unique_address]] logger_ref logger;

        // NOLINTEND(*-non-private-member-variables-in-classes)

        // NOLINTBEGIN(*-forwarding-reference-overload)

        // a copy constructor essentially; works on enable_owner_traits as well
        template <typename T>
            requires(!stl::same_as<stl::remove_cvref_t<T>, enable_traits> && EnabledTraits<T>)
        explicit constexpr enable_traits(T&& obj) noexcept
          : alloc{obj.template general_allocator<stl::byte>()},
            logger{stl::forward<T>(obj).logger} {}

        // NOLINTEND(*-forwarding-reference-overload)

        constexpr enable_traits(enable_traits const&) noexcept = default;
        constexpr enable_traits(enable_traits&&) noexcept      = default;
        constexpr ~enable_traits()                             = default;

        constexpr enable_traits& operator=(enable_traits const& rhs) {
            if (this != &rhs) {
                logger = rhs.logger;
            }
            return *this;
        }

        constexpr enable_traits& operator=(enable_traits&& rhs) noexcept {
            logger = stl::move(rhs.logger);
            return *this;
        }

        constexpr void swap(EnabledTraits auto& other) noexcept {
            using stl::swap;
            swap(logger, other.logger);
        }

        // when this object is a mother of another class, this method can help get the traits object.
        constexpr enable_traits& get_traits() noexcept {
            return *this;
        }

        constexpr enable_traits const& get_traits() const noexcept {
            return *this;
        }

        template <typename T>
        [[nodiscard]] constexpr general_allocator_type<T> general_allocator() const noexcept {
            return general_allocator_type<T>{alloc};
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
    struct enable_traits_with : T, enable_traits<TraitsType> {
        using enabled_type = T;
        using enable_traits<TraitsType>::enable_traits;

        explicit constexpr enable_traits_with(enable_traits<TraitsType> const& etraits) noexcept
          : enable_traits<TraitsType>{etraits} {}

        explicit constexpr enable_traits_with(enable_traits<TraitsType>&& etraits) noexcept
          : enable_traits<TraitsType>{stl::move(etraits)} {}

        constexpr enable_traits_with(enable_traits_with const&) noexcept            = default;
        constexpr enable_traits_with(enable_traits_with&&) noexcept                 = default;
        constexpr ~enable_traits_with() noexcept                                    = default;
        constexpr enable_traits_with& operator=(enable_traits_with const&) noexcept = default;
        constexpr enable_traits_with& operator=(enable_traits_with&&) noexcept      = default;
    };

    template <Traits TraitsType, EnabledTraits T>
        requires(EnabledTraitsOf<TraitsType, T>)
    struct enable_traits_with<TraitsType, T> : T {
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
    struct enable_traits_access<T> : enable_traits_with<typename T::traits_type, T> {
        using enable_traits_with<typename T::traits_type, T>::enable_traits_with;
    };

    template <Traits TraitsType>
    struct enable_traits_access<TraitsType> : enable_owner_traits<TraitsType> {
        using enable_owner_traits<TraitsType>::enable_owner_traits;
    };

    template <Traits TraitsType>
    struct enable_traits_access<enable_traits<TraitsType>> : enable_traits<TraitsType> {
        using enable_traits<TraitsType>::enable_traits;
    };

    template <Traits TraitsType>
    struct enable_traits_access<enable_owner_traits<TraitsType>> : enable_owner_traits<TraitsType> {
        using enable_owner_traits<TraitsType>::enable_owner_traits;
    };

    template <Traits TraitsType, typename T>
    struct enable_traits_access<enable_traits_with<TraitsType, T>> : enable_traits_with<TraitsType, T> {
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

        /// construct T and pass the etraits itself the first argument
        template <typename... Args>
            requires(stl::is_constructible_v<T, etraits, Args...>)
        explicit constexpr enable_traits_for(Args&&... args) noexcept(
          stl::is_nothrow_constructible_v<T, etraits, Args...>)
          : T{et, stl::forward<Args>(args)...} {}

        /// pass a general allocator as the first argument
        template <typename... Args>
            requires(!stl::is_constructible_v<T, etraits, Args...> &&
                     requires {
                         typename T::allocator_type;
                         requires stl::is_constructible_v<T, typename T::allocator_type const&, Args...>;

                         // allocators are convertible to each other
                         requires stl::convertible_to<
                           typename T::allocator_type,
                           typename etraits::template general_allocator_type<
                             typename stl::allocator_traits<typename T::allocator_type>::value_type>>;
                     })
        explicit constexpr enable_traits_for(Args&&... args) noexcept(
          stl::is_nothrow_constructible_v<T, typename T::allocator_type const&, Args...>)
          : T{general_alloc_for<T>(et), stl::forward<Args>(args)...} {}

        constexpr enable_traits_for(enable_traits_for const&) noexcept            = default;
        constexpr enable_traits_for(enable_traits_for&&) noexcept                 = default;
        constexpr ~enable_traits_for() noexcept                                   = default;
        constexpr enable_traits_for& operator=(enable_traits_for const&) noexcept = default;
        constexpr enable_traits_for& operator=(enable_traits_for&&) noexcept      = default;
    };

    template <typename T>
    concept TraitsAccess = Traits<T> || requires {
        typename T::traits_type;
        requires Traits<typename T::traits_type>;
        requires stl::same_as<T, enable_traits<typename T::traits_type>> ||
                   stl::same_as<T, enable_owner_traits<typename T::traits_type>> || requires {
                       typename T::enabled_type;
                       requires stl::
                         same_as<T, enable_traits_with<typename T::traits_type, typename T::enabled_type>>;
                   };
    };

    // template <typename T, AllocatorHolder AllocHolder>
    // [[nodiscard]] static constexpr auto const& local_allocator(AllocHolder&& holder) noexcept {
    //     return stl::forward<AllocHolder>(holder).template local_allocator<T>();
    // }

    /// get a general allocator from the specified allocator holder
    template <typename T, AllocatorHolder AllocHolder>
    [[nodiscard]] static constexpr decltype(auto) general_allocator(AllocHolder const& holder) noexcept {
        return holder.template general_allocator<T>();
    }

    /// if the specified type is an Allocator itself, we extract the value type from the allocator and return
    /// the correspondning allocator
    template <Allocator T, AllocatorHolder AllocHolder>
    [[nodiscard]] static constexpr decltype(auto) general_allocator(AllocHolder const& holder) noexcept {
        return holder.template general_allocator<typename stl::allocator_traits<T>::value_type>();
    }

    // template <typename T, AllocatorHolder AllocHolder>
    //     requires requires {  typename T::allocator_type; }
    // [[nodiscard]] static constexpr auto const& local_alloc_for(AllocHolder&& holder) noexcept {
    //     return stl::forward<AllocHolder>(holder).template local_alloc_for<typename T::allocator_type>();
    // }

    template <typename T, AllocatorHolder AllocHolder>
        requires requires { typename T::allocator_type; }
    [[nodiscard]] static constexpr decltype(auto) general_alloc_for(AllocHolder const& holder) noexcept {
        using value_type = typename stl::allocator_traits<typename T::allocator_type>::value_type;
        return holder.template general_allocator<value_type>();
    }

    // template <typename T, AllocatorHolder AllocHolder, typename... Args>
    // [[nodiscard]] static constexpr auto allocate_unique_general(AllocHolder&& holder,
    //                                                             Args&&... args) noexcept {
    //     return stl::forward<AllocHolder>(holder).template allocate_unique_general<T>(
    //       stl::forward<Args>(args)...);
    // }

    // template <typename T, AllocatorHolder AllocHolder, typename... Args>
    // static constexpr auto allocate_unique_local(AllocHolder&& holder, Args&&... args) noexcept {
    //     return stl::forward<AllocHolder>(holder).template allocate_unique_local<T>(
    //       stl::forward<Args>(args)...);
    // }



} // namespace webpp

#endif // WEBPP_ENABLE_TRAITS_HPP
