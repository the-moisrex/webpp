// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_H
#define WEBPP_STRING_H

#include "./std.hpp"

#if __has_include(<string>)
#    define STD_STRING STLLIB_STANDARD
#    include <string>
#elif __has_include(<experimental/string>)
#    define STD_STRING STLLIB_EXPERIMENTAL
#    include <experimental/string>
#elif __has_include(<boost/utility/string.hpp>)
#    define STD_STRING STLLIB_BOOST
#    include <boost/utility/string.hpp>
namespace webpp::stl {
    using basic_string = boost::basic_string;
    using string       = boost::string;
} // namespace webpp::stl
#endif


#include "../traits/traits_concepts.hpp"

// Traits aware string:
namespace webpp::istl {

    template <Traits TraitsType,
              typename CharT      = typename TraitsType::char_type,
              typename CharTraits = typename TraitsType::char_traits,
              typename Allocator  = typename TraitsType::allocator>
    using basic_string = stl::basic_string<CharT, CharTraits, Allocator>;


    template <typename T>
    concept String = requires(T str) {
        {T{"str"}};
        {str.empty()};
        {str.at(0)};
        {str.data()};
        {str.c_str()};
        {str = "str"};
        {str.size()};
        {str.capacity()};
        {str.shrink_to_fit()};
        {str.clear()};
        {str.append("str")};
        {str.starts_with('a')};
        {str.ends_with('a')};
        {str.substr('a')};
        {T::npos};

        typename T::value_type;
        typename T::allocator_type;
    };

} // namespace webpp::stl

#endif // WEBPP_STRING_H
