// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_VIEW_H
#define WEBPP_STRING_VIEW_H

#include "../traits/traits_concepts.h"
#include "./std.h"

#if __has_include(<string_view>)
#    define STD_STRING_VIEW STLLIB_STANDARD
#    include <string_view>
#elif __has_include(<experimental/string_view>)
#    define STD_STRING_VIEW STLLIB_EXPERIMENTAL
#    include <experimental/string_view>
#elif __has_include(<boost/utility/string_view.hpp>)
#    define STD_STRING_VIEW STLLIB_BOOST
#    include <boost/utility/string_view.hpp>
namespace webpp::std {
    using basic_string = boost::basic_string;
    using string       = boost::string;
} // namespace webpp::std
#endif

// Traits aware string_view:
namespace webpp::stl {

    template <Traits TraitsType,
              typename CharT      = typename TraitsType::char_type,
              typename CharTraits = typename TraitsType::char_traits>
    using basic_string_view = ::std::basic_string_view<CharT, CharTraits>;

} // namespace webpp::stl

#endif // WEBPP_STRING_VIEW_H
