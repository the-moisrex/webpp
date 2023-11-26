// Created by moisrex on 7/4/20.

#ifndef WEBPP_STD_PMR_TRAITS_H
#define WEBPP_STD_PMR_TRAITS_H

#include "../memory/std_pmr_allocator_pack.hpp"
#include "std_traits.hpp"

namespace webpp {

    // todo: specialize for monotonic buffer resource and stuff
    template <typename CharT>
    struct basic_std_pmr_traits {
        using char_type             = CharT;
        using logger_type           = stderr_logger;
        using allocator_descriptors = stl::pmr::allocator_descriptors;
        using string_view           = stl::basic_string_view<char_type, stl::char_traits<char_type>>;

        template <typename AllocT>
        using string = stl::basic_string<char_type, stl::char_traits<char_type>, AllocT>;
    };

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
