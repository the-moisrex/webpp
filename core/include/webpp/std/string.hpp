// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_H
#define WEBPP_STRING_H

#include "./std.hpp"

#include <string>

#include "../traits/traits_concepts.hpp"

// Traits aware string:
namespace webpp::istl {

    template <Traits TraitsType, typename CharT = typename TraitsType::char_type,
              typename CharTraits = typename TraitsType::char_traits,
              typename Allocator  = typename TraitsType::allocator>
    using basic_string = stl::basic_string<CharT, CharTraits, Allocator>;


    template <typename T>
    concept String = requires(stl::remove_cvref_t<T> str) {
        //        { T{"str"} };
        str.empty();
        str.at(0);
        str.data();
        str.c_str();
        {str = "str"};
        str.size();
        str.capacity();
        str.shrink_to_fit();
        str.clear();
        str.append("str");
        str.starts_with('a');
        str.ends_with('a');
        str.substr('a');
        str.begin();
        str.end();
        str.cbegin();
        str.cend();
        stl::remove_cvref_t<T>::npos;

        typename stl::remove_cvref_t<T>::value_type;
        typename stl::remove_cvref_t<T>::allocator_type;
    };

    template <typename T>
    concept ConvertibleToString = !istl::CharType<T> && requires(stl::remove_cvref_t<T> str) {
        requires requires {
            stl::basic_string{str};
        }
        || requires {
            str.data();
            str.size();
            stl::basic_string{str.data(), str.size()};
        }
        || requires {
            str.c_str();
            str.size();
            stl::basic_string{str.c_str(), str.size()};
        };
    };


    [[nodiscard]] constexpr auto to_string(ConvertibleToString auto&& str, auto const& allocator) noexcept {
        if constexpr (String<decltype(str)>) {
            return str;
        } else if constexpr (requires { stl::basic_string{str, allocator}; }) {
            return stl::basic_string{str, allocator};
        } else if constexpr (requires {
                                 str.c_str();
                                 str.size();
                                 stl::basic_string{str.c_str(), str.size(), allocator};
                             }) {
            return stl::basic_string{str.c_str(), str.size(), allocator};
        } else if constexpr (requires {
                                 str.data();
                                 str.size();
                                 stl::basic_string{str.data(), str.size(), allocator};
                             }) {
            return stl::basic_string{str.data(), str.size(), allocator};
        } else if constexpr (requires { str.str(); }) {
            return to_string(str.str(), allocator);
        } else {
            throw stl::invalid_argument("The specified input is not convertible to string");
        }
    };

    /**
     * Get the underlying data of the specified string
     */
    [[nodiscard]] constexpr auto string_data(ConvertibleToString auto&& str) noexcept {
        if constexpr (requires { str.data(); }) {
            return str.data();
        } else if constexpr (requires { str.c_str(); }) {
            return str.c_str(); // this is const, but that's that caller's problem now :)
        } else {
            return &str[0]; // it'll throw an error if it didn't work, so let's do this
        }
    };

    template <typename T>
    concept ComparableToString = requires(T obj) {
        {obj == ""};
    }
    || requires(T obj) {
        {"" == obj};
    };

} // namespace webpp::istl

#endif // WEBPP_STRING_H
