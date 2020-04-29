// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_H
#define WEBPP_STRING_H

#include "std.h"

#if __has_include(<string>)
#    define STD_STRING STLLIB_STANDARD
#    include <string_view>
#elif __has_include(<experimental/string>)
#    define STD_STRING STLLIB_EXPERIMENTAL
#    include <experimental/string>
#elif __has_include(<boost/utility/string.hpp>)
#    define STD_STRING_VIEW STLLIB_BOOST
#    include <boost/utility/string.hpp>
namespace std {
    using basic_string = boost::basic_string;
    using string       = boost::string;
} // namespace std
#endif


#include "../traits/std_traits.h"

// Traits aware string:
namespace webpp::stl {

    template <typename Traits     = webpp::std_traits,
              typename CharT      = typename Traits::char_type,
              typename CharTraits = typename Traits::char_traits,
              typename Allocator  = typename Traits::allocator>
    using basic_string = ::std::basic_string<CharT, CharTraits, Allocator>;


} // namespace webpp::stl

#endif // WEBPP_STRING_H
