// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_CONCEPTS_H
#define WEBPP_EXTENSION_CONCEPTS_H

#include "../std/tuple.h"

#include <type_traits>

namespace webpp {

    template <typename T>
    concept Extension =
      !::std::is_final_v<T> && ::std::is_default_constructible_v<T> &&
      ::std::is_move_constructible_v<T> && ::std::is_move_assignable_v<T> &&
      ::std::is_copy_assignable_v<T> && ::std::is_class_v<T> &&
      !::stl::is_tuple<T>::value && !::std::is_integral_v<T>;


    template <typename... T>
    struct extension_tuple {
        static constexpr bool value = false;
    };

    template <Extension... T>
    struct extension_tuple<::std::tuple<T...>> {
        static constexpr bool value = true;
    };

    template <typename T>
    concept ExtensionList = Extension<T> ||
                            (::stl::Tuple<T> && extension_tuple<T>::value);

    template <typename T>
    concept ExtensionWithDependencies = Extension<T>&& requires {
        typename T::required_extensions;
    }
    &&Extension<typename T::required_extensions>;

} // namespace webpp

#endif // WEBPP_EXTENSION_CONCEPTS_H
