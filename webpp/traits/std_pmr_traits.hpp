// Created by moisrex on 7/4/20.

#ifndef WEBPP_STD_PMR_TRAITS_H
#define WEBPP_STD_PMR_TRAITS_H

#include "../std/memory_resource.hpp"
#include "std_traits.hpp"

namespace webpp {

#ifndef webpp_has_memory_resource

    /// fallback on std traits
    template <typename CharT>
    using basic_std_pmr_traits = basic_std_traits<CharT>;

#else

    namespace details {

        /**
         * @brief a wrapper for a monotonic buffer resource in order to customize the
         * "construct_allocator_from" CPO
         * @tparam T the type that the allocator is needed for
         */
        template <typename T>
        struct resource_wrapper {
            stl::pmr::monotonic_buffer_resource resource;

            /// CPO specialization for getting the allocator
            /// Even though the standard doesn't specify "noexcept", it is
            /// We're ignoring the requested type, polymorphic_allocatr<stl::byte> should be able to allocate
            /// anything and everything.
            [[nodiscard]] friend stl::pmr::polymorphic_allocator<T> tag_invoke(
              [[maybe_unused]] stl::tag_t<construct_allocator_from> tag,
              resource_wrapper&                                     self) noexcept {
                return stl::pmr::polymorphic_allocator<T>{
                  static_cast<stl::pmr::memory_resource*>(&self.resource)};
            }
        };

    } // namespace details

    /**
     * @brief Traits Types which uses std::pmr allocators
     * @tparam CharT Character Type to use
     */
    template <typename CharT>
    struct basic_std_pmr_traits {
        using char_type   = CharT;
        using logger_type = stderr_logger;
        using string_view = stl::basic_string_view<char_type>;

        struct allocator_descriptor {
            template <typename T = stl::byte>
            using allocator_type = stl::pmr::polymorphic_allocator<T>;

            template <typename T>
            [[nodiscard]] static allocator_type<T> construct_allocator() noexcept {
                return {};
            }
        };

        /// standard library doesn't provide a monotonic allocator for std::allocator (only for pmr)
        /// "void" is used to indicate that we don't provide one
        // struct monotonic_allocator_descriptor {
        //     /// we're ignoring the returned type
        //     template <typename T = stl::byte>
        //     using allocator_type = stl::pmr::polymorphic_allocator<T>;

        //     template <typename T>
        //     using alloc_traits = stl::allocator_traits<allocator_type<T>>;

        //     /// return a resource instead of an allocator, the library will call "construct_allocator_from"
        //     /// CPO on it Even though the standard doesn't specify "noexcept", it is
        //     template <typename T>
        //     [[nodiscard]] static auto construct_allocator(void* const       buffer,
        //                                                   stl::size_t const size) noexcept {
        //         return details::resource_wrapper<T>{
        //           stl::pmr::monotonic_buffer_resource{buffer, size}
        //         };
        //     }
        // };

        static_assert(GeneralAllocatorDescriptor<allocator_descriptor>,
                      "Wrong Descriptions for allocator descriptor");

        template <typename AllocT>
        using string = stl::basic_string<char_type, stl::char_traits<char_type>, AllocT>;
    };



#endif

    //    template <typename T>
    //    struct std_pmr_traits_from_string_view {
    //        using type = basic_std_pmr_traits<typename T::value_type, typename T::traits_type,
    //                                          stl::pmr::polymorphic_allocator>;
    //    };
    //
    //    template <typename T>
    //    struct std_pmr_traits_from_string {
    //        using type =
    //          basic_std_pmr_traits<typename T::value_type, typename T::traits_type,
    //                               stl::allocator_traits<typename T::allocator_type>::template
    //                               rebind_alloc>;
    //    };
    //
    //    template <typename T>
    //    struct std_pmr_traits_from {};
    //
    //    template <typename CharT, typename CharTraitsT>
    //    struct std_pmr_traits_from<stl::basic_string_view<CharT, CharTraitsT>> {
    //        using type = basic_std_pmr_traits<CharT, CharTraitsT>;
    //    };
    //
    //    template <typename CharT, typename CharTraitsT, typename AllocatorType>
    //    struct std_pmr_traits_from<stl::basic_string<CharT, CharTraitsT, AllocatorType>> {
    //        using type = basic_std_pmr_traits<CharT, CharTraitsT,
    //                                          stl::allocator_traits<AllocatorType>::template rebind_alloc>;
    //    };


    struct std_pmr_traits : basic_std_pmr_traits<char> {};

    struct std_pmr_wtraits : basic_std_pmr_traits<wchar_t> {};

    struct std_pmr_u8traits : basic_std_pmr_traits<char8_t> {};

    struct std_pmr_u16traits : basic_std_pmr_traits<char16_t> {};

    struct std_pmr_u32traits : basic_std_pmr_traits<char32_t> {};

} // namespace webpp


#endif // WEBPP_STD_PMR_TRAITS_H
