// Created by moisrex on 4/28/20.

#ifndef WEBPP_STD_TRAITS_H
#define WEBPP_STD_TRAITS_H

#include "../logs/std_logger.hpp"
#include "../memory/std_allocator_pack.hpp"

#include <string>
#include <string_view>

namespace webpp {

    template <typename CharT,
              typename CharTraits = stl::char_traits<CharT>,
              typename Allocator  = stl::allocator_descriptors>
    struct basic_std_traits {
        using traits_type           = basic_std_traits<CharT, CharTraits, Allocator>;
        using char_type             = CharT;
        using char_traits           = CharTraits;
        using logger_type           = stderr_logger;
        using allocator_descriptors = Allocator;
        using string_view           = stl::basic_string_view<char_type, char_traits>;

        template <typename AllocT>
        using string = stl::basic_string<char_type, char_traits, AllocT>;
    };

    // Even though these were some good ideas, they go against my new high level view of the traits system

    //    template <typename T>
    //    struct std_traits_from_string_view {
    //        using type = basic_std_traits<typename T::value_type, typename T::traits_type,
    //        stl::allocator_pack>;
    //    };
    //
    //    template <typename T>
    //    struct std_traits_from_string {
    //        using type =
    //          basic_std_traits<typename T::value_type, typename T::traits_type,
    //                           stl::allocator_traits<typename T::allocator_type>::template rebind_alloc>;
    //    };
    //
    //    template <typename T>
    //    struct std_traits_from {};
    //
    //    template <typename CharT, typename CharTraitsT>
    //    struct std_traits_from<stl::basic_string_view<CharT, CharTraitsT>> {
    //        using type = basic_std_traits<CharT, CharTraitsT>;
    //    };
    //
    //    template <typename CharT, typename CharTraitsT, typename AllocatorType>
    //    struct std_traits_from<stl::basic_string<CharT, CharTraitsT, AllocatorType>> {
    //        using type =
    //          basic_std_traits<CharT, CharTraitsT, stl::allocator_traits<AllocatorType>::template
    //          rebind_alloc>;
    //    };


    using std_traits    = basic_std_traits<char>;
    using std_wtraits   = basic_std_traits<wchar_t>;
    using std_u8traits  = basic_std_traits<char8_t>;
    using std_u16traits = basic_std_traits<char16_t>;
    using std_u32traits = basic_std_traits<char32_t>;

} // namespace webpp

#endif // WEBPP_STD_TRAITS_H
