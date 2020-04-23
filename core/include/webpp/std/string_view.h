// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_VIEW_H
#define WEBPP_STRING_VIEW_H

#include "std.h"

#if __has_include(<string_view>)
#    define STD_STRING_VIEW STLLIB_STANDARD
#    include <string_view>
#elif __has_include(<experimental/string_view>)
#    define STD_STRING_VIEW STLLIB_EXPERIMENTAL
#    include <experimental/string_view>
#elif __has_include(<boost/utility/string_view.hpp>)
#    define STD_STRING_VIEW STLLIB_BOOST
#    include <boost/utility/string_view.hpp>
namespace std {
    using basic_string_view = boost::basic_string_view;
    using string_view       = boost::string_view;
} // namespace std
#endif


#include "../utils/traits.h"

// Traits aware string_view:
namespace webpp::stl {

    template <typename Traits     = webpp::std_traits,
              typename CharT      = typename Traits::char_type,
              typename CharTraits = typename Traits::char_traits>
    class basic_string_view
      : public ::std::basic_string_view<CharT, CharTraits> {};

} // namespace webpp::stl

#endif // WEBPP_STRING_VIEW_H
