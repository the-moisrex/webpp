// Created by moisrex on 8/30/20.

#ifndef WEBPP_ENABLE_TRAITS_HPP
#define WEBPP_ENABLE_TRAITS_HPP

#include "traits.hpp"

namespace webpp {

    /**
     * By inheriting from this you'll make your type "TraitsEnabled".
     */
    template <Traits TraitsType>
    struct enable_traits {
        using traits_type       = TraitsType;
        using logger_type       = traits::logger<traits_type>;
        using logger_ref        = typename logger_type::logger_ref;
        using char_type         = traits::char_type<traits_type>;
        using string_type       = traits::string<traits_type>;
        using string_view_type  = traits::string_view<traits_type>;
        using local_allocator   = traits::local_allocator<traits_type, char_type>;
        using general_allocator = traits::general_allocator<traits_type, char_type>;
        using alloc_list =
          traits::alloc_list<char_type>; // It's a tuple-like type capable of containing multiple allocators

        [[no_unique_address]] alloc_list allocs{};
        [[no_unique_address]] logger_ref logger{};

        template <Allocator... AllocT>
        requires((istl::tuple_contains<alloc_list, AllocT>::value &&
                  ...)) constexpr explicit enable_traits(logger_ref logger_obj = logger_type{},
                                                         AllocT const&... alloc) noexcept
          : logger{logger_obj},
            allocs{alloc...} {}

        constexpr explicit enable_traits(logger_ref logger_obj, alloc_list const& the_allocs) noexcept
          : logger{logger_obj},
            allocs{the_allocs} {}

        // todo: remove this and see if anyone somewhere is using it when they should be using copy ctor
        constexpr enable_traits()                         = default;
        constexpr enable_traits(enable_traits const&)     = default;
        constexpr enable_traits(enable_traits&&) noexcept = default;
        constexpr enable_traits& operator=(enable_traits const&) = default;
        constexpr enable_traits& operator=(enable_traits&&) noexcept = default;


        /**
         * Get the allocator of the specified type form the allocator list
         */
        template <template <typename> typename AllocT = traits::general_allocator, typename T = char_type>
        [[nodiscard]] auto const& alloc() const noexcept {
            using char_alloc_type = AllocT<char_type>;
            using new_alloc_type  = AllocT<T>;
            static_assert(
              istl::tuple_contains<alloc_list, char_alloc_type>::value,
              "The specified allocator doesn't exists in the allocator pack specified in this traits type.");

            if constexpr (stl::same_as<T, char_type>) {
                return stl::get<AllocT>(allocs);
            } else {
                if constexpr (stl::same_as<new_alloc_type, char_alloc_type>) {
                    // some allocators are heterogeneous, like std::pmr stuff
                    // so even though the T != char_type, but alloc<T> == alloc<char_type>
                    return stl::get<char_alloc_type>(allocs);
                } else {
                    // using copy constructor of the new allocator
                    return new_alloc_type{stl::get<char_alloc_type>(allocs)};
                }
            }
        }

        /**
         * Get the local allocator (suitable for in-function local usage).
         */
        template <typename T>
        [[nodiscard]] auto const& local_alloc() const noexcept {
            return alloc<traits::local_allocator, T>();
        }

        /**
         * Get the general allocator (suitable for general usage but might be slower than other special ones)
         */
        template <typename T>
        [[nodiscard]] auto const& general_alloc() const noexcept {
            return alloc<traits::general_allocator, T>();
        }


        /**
         * This is designed to pass the allocator and logger and other necessary stuff from this traits
         * enabled struct and create an object of the specified type and pass those traits to it.
         * This is there to make sure we don't have to pass all these stuff around ourselves and this can
         * manage those passings.
         */
        template <EnabledTraits T, typename... Args>
        [[nodiscard]] constexpr auto instantiate(Args&&... args) noexcept {
            return T{stl::forward<Args>(args)..., logger, allocs};
        }

        constexpr void swap(EnabledTraits auto& other) noexcept {
            using stl::swap;
            swap(allocs, other.allocs);
            swap(logger, other.logger);
        }
    };

} // namespace webpp

#endif // WEBPP_ENABLE_TRAITS_HPP
