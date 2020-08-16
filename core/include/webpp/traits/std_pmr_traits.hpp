// Created by moisrex on 7/4/20.

#ifndef WEBPP_STD_PMR_TRAITS_H
#define WEBPP_STD_PMR_TRAITS_H

#include "std_traits.hpp"

namespace webpp {

    template <typename CharT, typename CharTraits = stl::char_traits<CharT>,
              template <typename> typename Allocator = stl::pmr::polymorphic_allocator>
    using basic_std_pmr_traits = basic_std_traits<CharT, CharTraits, Allocator>;


    using std_pmr_traits =
      basic_std_pmr_traits<char, stl::char_traits<char>, stl::pmr::polymorphic_allocator>;

    template <typename T>
    struct std_pmr_traits_from_string_view {
        using type = basic_std_pmr_traits<typename T::value_type, typename T::traits_type,
                                          stl::pmr::polymorphic_allocator>;
    };

    template <typename T>
    struct std_pmr_traits_from_string {
        using type =
          basic_std_pmr_traits<typename T::value_type, typename T::traits_type,
                               stl::allocator_traits<typename T::allocator_type>::template rebind_alloc>;
    };

    template <typename T>
    struct std_pmr_traits_from {};

    template <typename CharT, typename CharTraitsT>
    struct std_pmr_traits_from<stl::basic_string_view<CharT, CharTraitsT>> {
        using type = basic_std_pmr_traits<CharT, CharTraitsT>;
    };

    template <typename CharT, typename CharTraitsT, typename AllocatorType>
    struct std_pmr_traits_from<stl::basic_string<CharT, CharTraitsT, AllocatorType>> {
        using type = basic_std_pmr_traits<CharT, CharTraitsT,
                                          stl::allocator_traits<AllocatorType>::template rebind_alloc>;
    };

} // namespace webpp

#endif // WEBPP_STD_PMR_TRAITS_H
