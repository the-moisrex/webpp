// Created by moisrex on 4/28/20.

#ifndef WEBPP_STD_TRAITS_H
#define WEBPP_STD_TRAITS_H

#include "./traits.h"

#include <iterator>
#include <ostream>
#include <string>
#include <string_view>

namespace webpp {

    template <typename CharT>
    struct basic_std_traits {
        using char_type   = CharT;
        using char_traits = std::char_traits<char_type>;

        template <typename Type>
        using allocator = std::allocator<Type>;

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

    using std_traits = basic_std_traits<char>;

    // todo: add concept


} // namespace webpp

#endif // WEBPP_STD_TRAITS_H
