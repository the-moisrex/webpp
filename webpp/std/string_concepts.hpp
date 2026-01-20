// Created by moisrex on 8/9/20.

#ifndef WEBPP_STD_STRING_CONCEPTS_HPP
#define WEBPP_STD_STRING_CONCEPTS_HPP

#include "concepts.hpp"

#include <string>

namespace webpp::istl {

    template <typename T>
    concept SingleCharacter = part_of<T, unsigned char, char, wchar_t, char16_t, char8_t, char32_t>;

    template <typename T>
    concept CharType = istl::SingleCharacter<stl::remove_cvref_t<T>>;

    namespace details {
        template <typename T>
        struct char_type_of {
            using type = void;
        };

        template <typename T>
            requires requires {
                typename T::value_type;
                requires stl::is_trivial_v<typename T::value_type>;
                requires stl::is_standard_layout_v<typename T::value_type>;
            }
        struct char_type_of<T> {
            using type = typename T::value_type;
        };

        template <SingleCharacter T>
        struct char_type_of<T> {
            using type = T;
        };

        template <typename T>
        struct allocator_type_extractor {
            using type = typename T::allocator_type;
        };

        template <typename T>
        concept has_allocator_type = requires { typename stl::remove_cvref_t<T>::allocator_type; };
    } // namespace details

    /**
     * Get the underlying character type in a string/string view/c style string
     */
    template <typename T>
    struct char_type_of : details::char_type_of<stl::decay_t<stl::remove_pointer_t<stl::decay_t<T>>>> {};

    template <typename T>
    using char_type_of_t = typename char_type_of<T>::type;

    template <typename T>
    struct char_traits_type_of {
        using type = stl::char_traits<char_type_of_t<T>>;
    };

    template <typename T>
        requires requires { typename T::traits_type; }
    struct char_traits_type_of<T> {
        using type = typename T::traits_type;
    };

    template <typename T>
    using char_traits_type_of_t = typename char_traits_type_of<T>::type;


    template <typename T>
    using char_type_of_t_string_literals =
      stl::remove_cvref_t<stl::remove_pointer_t<stl::remove_all_extents_t<stl::remove_cvref_t<T>>>>;


    template <typename T>
    concept UTF8 = sizeof(char_type_of_t<T>) == sizeof(char8_t);

    template <typename T>
    concept UTF16 = sizeof(char_type_of_t<T>) == sizeof(char16_t);

    template <typename T>
    concept UTF32 = sizeof(char_type_of_t<T>) == sizeof(char32_t);

    template <typename T>
    concept StringLiteral = (!stl::same_as<char_type_of_t_string_literals<T>, stl::remove_cvref_t<T>>) &&
                            CharType<char_type_of_t_string_literals<T>>;

} // namespace webpp::istl

#endif // WEBPP_STD_STRING_CONCEPTS_HPP
