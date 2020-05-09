// Created by moisrex on 4/28/20.

#ifndef WEBPP_STD_TRAITS_H
#define WEBPP_STD_TRAITS_H

#include "./traits.h"

#include <iterator>
#include <ostream>
#include <string>
#include <string_view>

namespace webpp {

    template <typename CharT, typename CharTraits = ::std::char_traits<CharT>,
              template <typename> typename Allocator = ::std::allocator>
    struct basic_std_traits {
        using char_type   = CharT;
        using char_traits = CharTraits;

        template <typename Type>
        using allocator = Allocator<Type>;

        using string_view_type = std::basic_string_view<char_type, char_traits>;
        using string_type =
          std::basic_string<char_type, char_traits, allocator<char_type>>;

        using stringstream_type =
          std::basic_stringstream<char_type, char_traits, allocator<char_type>>;
        using ostringstream_type =
          std::basic_ostringstream<char_type, char_traits,
                                   allocator<char_type>>;

        template <typename Type>
        using ostream_iterator_type =
          std::ostream_iterator<Type, char_type, char_traits>;

        using ostream_type = std::basic_ostream<char_type, char_traits>;
    };

    using std_traits =
      basic_std_traits<char, ::std::char_traits<char>, ::std::allocator>;

    template <typename T>
    struct std_traits_from_string_view {
        using type =
          basic_std_traits<typename T::value_type, typename T::traits_type,
                           ::std::allocator>;
    };

    template <typename T>
    struct std_traits_from_string {
        using type =
          basic_std_traits<typename T::value_type, typename T::traits_type,
                           ::std::allocator_traits<typename T::allocator_type>::
                             template rebind_alloc>;
    };

} // namespace webpp

#endif // WEBPP_STD_TRAITS_H
