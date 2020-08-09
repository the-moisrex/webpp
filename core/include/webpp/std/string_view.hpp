// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_VIEW_H
#define WEBPP_STRING_VIEW_H

#include "../traits/traits_concepts.hpp"
#include "./std.hpp"

#if __has_include(<string_view>)
#    define STD_STRING_VIEW STLLIB_STANDARD
#    include <string_view>
#elif __has_include(<experimental/string_view>)
#    define STD_STRING_VIEW STLLIB_EXPERIMENTAL
#    include <experimental/string_view>
#elif __has_include(<boost/utility/string_view.hpp>)
#    define STD_STRING_VIEW STLLIB_BOOST
#    include <boost/utility/string_view.hpp>
namespace webpp::stl {
    template <typename... T>
    using basic_string_view = boost::basic_string_view<T...>;
    using string_view       = boost::string_view;
} // namespace webpp::stl
#endif

// Traits aware string_view:
namespace webpp::istl {

    template <Traits TraitsType,
              typename CharT      = typename TraitsType::char_type,
              typename CharTraits = typename TraitsType::char_traits>
    using basic_string_view = ::std::basic_string_view<CharT, CharTraits>;



    template <typename T>
    concept StringView = requires(T str) {
        {T{"str"}};
        {str.empty()};
        {str.at(0)};
        {str.data()};
        {str.size()};
        {str.remove_suffix(1)};
        {str.remove_prefix(1)};
        {str.starts_with('a')};
        {str.ends_with('a')};
        {str.substr('a')};
        {T::npos};

        typename T::value_type;
    };


    template <typename T>
    concept ConvertibleToStringView = requires (T str) {
        stl::basic_string_view{str};
    };

    template <typename T>
    using char_type_of = typename decltype(stl::basic_string_view{stl::declval<T>()})::value_type;


} // namespace webpp::stl

#endif // WEBPP_STRING_VIEW_H
