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

    template <Traits TraitsType, typename CharT = typename TraitsType::char_type,
              typename CharTraits = typename TraitsType::char_traits>
    using basic_string_view = ::std::basic_string_view<CharT, CharTraits>;



    template <typename T>
    concept StringView = requires(stl::remove_cvref_t<T> str) {
//        { T{"str"} };
        str.empty();
        str.at(0);
        str.data();
        str.size();
        str.remove_suffix(1);
        str.remove_prefix(1);
        str.starts_with('a');
        str.ends_with('a');
        str.substr('a');
        str.begin();
        str.end();
        str.cbegin();
        str.cend();
        stl::remove_cvref_t<T>::npos;

        typename stl::remove_cvref_t<T>::value_type;
    } && !requires (stl::remove_cvref_t<T> str){
        // the things that std::string has
        str.clear();
        str.shrink_to_fit();
        str.capacity();
        {str = "str"};
    };


    template <typename T>
    concept ConvertibleToStringView = !istl::CharType<T> && requires(stl::remove_cvref_t<T> str) {
        requires requires {
            stl::basic_string_view{str};
        }
        || requires {
            str.data();
            str.size();
            stl::basic_string_view{str.data(), str.size()};
        }
        || requires {
            str.c_str();
            str.size();
            stl::basic_string_view{str.c_str(), str.size()};
        };
    };


    [[nodiscard]] constexpr auto to_string_view(ConvertibleToStringView auto&& str) noexcept {
        if constexpr (StringView<decltype(str)>) {
            return str;
        } else if constexpr (requires { stl::basic_string_view{str}; }) {
            return stl::basic_string_view{str};
        } else if constexpr (requires {
                                 str.c_str();
                                 str.size();
                                 stl::basic_string_view{str.c_str(), str.size()};
                             }) {
            return stl::basic_string_view{str.c_str(), str.size()};
        } else if constexpr (requires {
                                 str.data();
                                 str.size();
                                 stl::basic_string_view{str.data(), str.size()};
                             }) {
            return stl::basic_string_view{str.data(), str.size()};
        } else if constexpr (requires { str.str(); }) {
            return to_string_view(str.str());
        } else {
            throw stl::invalid_argument("The specified input is not convertible to string_view");
        }
    };

    template <typename T>
    using char_type_of = typename decltype(to_string_view(stl::declval<T>()))::value_type;

    template <typename T>
    using char_traits_type_of = typename decltype(to_string_view(stl::declval<T>()))::traits_type;

} // namespace webpp::istl

#endif // WEBPP_STRING_VIEW_H
