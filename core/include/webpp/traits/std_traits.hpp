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
        using char_type   = CharT;
        using char_traits = CharTraits;

        template <typename Type>
        using allocator = Allocator<Type>;

        using string_view_type = stl::basic_string_view<char_type, char_traits>;
        using string_type      = stl::basic_string<char_type, char_traits, allocator<char_type>>;

        using stringstream_type  = stl::basic_stringstream<char_type, char_traits, allocator<char_type>>;
        using ostringstream_type = stl::basic_ostringstream<char_type, char_traits, allocator<char_type>>;

        template <typename Type>
        using ostream_iterator_type = stl::ostream_iterator<Type, char_type, char_traits>;

        using ostream_type = stl::basic_ostream<char_type, char_traits>;
        static auto constexpr stderr_func() noexcept {
            return stderr;
        };

        using logger_type = std_logger<stderr_func>;
    };

    using std_traits = basic_std_traits<char, stl::char_traits<char>, stl::allocator>;

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

    using fake_traits_type = std_traits;

} // namespace webpp

#endif // WEBPP_STD_TRAITS_H
