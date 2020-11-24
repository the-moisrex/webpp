// Created by moisrex on 4/28/20.

#ifndef WEBPP_STD_TRAITS_H
#define WEBPP_STD_TRAITS_H

#include "../logs/std_logger.hpp"

#include <iterator>
#include <ostream>
#include <string>
#include <string_view>

namespace webpp {

    template <typename CharT, typename CharTraits = stl::char_traits<CharT>,
              template <typename> typename Allocator = stl::allocator>
    struct basic_std_traits {
        using traits_type = basic_std_traits<CharT, CharTraits, Allocator>;

        using char_type   = CharT;
        using char_traits = CharTraits;

        template <typename Type>
        using allocator = Allocator<Type>;

        // todo: implement the pmr::monotonic_buffer_resource features and use std::allocator as upstream
        template <typename T>
        using monotonic_allocator = Allocator<T>;

        using string_view_type = stl::basic_string_view<char_type, char_traits>;
        using string_type      = stl::basic_string<char_type, char_traits, allocator<char_type>>;

        using logger_type = stderr_logger;
    };


    template <typename T>
    struct std_traits_from_string_view {
        using type = basic_std_traits<typename T::value_type, typename T::traits_type, stl::allocator>;
    };

    template <typename T>
    struct std_traits_from_string {
        using type =
          basic_std_traits<typename T::value_type, typename T::traits_type,
                           stl::allocator_traits<typename T::allocator_type>::template rebind_alloc>;
    };

    template <typename T>
    struct std_traits_from {};

    template <typename CharT, typename CharTraitsT>
    struct std_traits_from<stl::basic_string_view<CharT, CharTraitsT>> {
        using type = basic_std_traits<CharT, CharTraitsT>;
    };

    template <typename CharT, typename CharTraitsT, typename AllocatorType>
    struct std_traits_from<stl::basic_string<CharT, CharTraitsT, AllocatorType>> {
        using type =
          basic_std_traits<CharT, CharTraitsT, stl::allocator_traits<AllocatorType>::template rebind_alloc>;
    };


    using std_traits = basic_std_traits<char>;
    using std_wtraits = basic_std_traits<wchar_t>;
    using std_u8traits = basic_std_traits<char8_t>;
    using std_u16traits = basic_std_traits<char16_t>;
    using std_u32traits = basic_std_traits<char32_t>;

    using fake_traits_type = std_traits;

} // namespace webpp

#endif // WEBPP_STD_TRAITS_H
