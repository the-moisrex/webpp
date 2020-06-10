// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_H
#define WEBPP_STRING_H

#include "./std.h"

#if __has_include(<string>)
#    define STD_STRING STLLIB_STANDARD
#    include <string>
#elif __has_include(<experimental/string>)
#    define STD_STRING STLLIB_EXPERIMENTAL
#    include <experimental/string>
#elif __has_include(<boost/utility/string.hpp>)
#    define STD_STRING STLLIB_BOOST
#    include <boost/utility/string.hpp>
namespace webpp::std {
    using basic_string = boost::basic_string;
    using string       = boost::string;
} // namespace webpp::std
#endif


#include "../traits/traits_concepts.h"

// Traits aware string:
namespace webpp::stl {

    template <Traits TraitsType,
              typename CharT      = typename TraitsType::char_type,
              typename CharTraits = typename TraitsType::char_traits,
              typename Allocator  = typename TraitsType::allocator>
    using basic_string = stl::basic_string<CharT, CharTraits, Allocator>;


} // namespace webpp::stl

#endif // WEBPP_STRING_H
