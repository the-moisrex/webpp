// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_CONCEPTS_H
#define WEBPP_EXTENSION_CONCEPTS_H

#include "../std/tuple.h"

#include <type_traits>

namespace webpp {

    template <typename T>
    using is_extension = std::conjunction<
      ::std::is_default_constructible<T>, ::std::is_move_constructible<T>,
      ::std::is_move_assignable<T>, ::std::is_copy_assignable<T>>;

    template <typename T>
    concept Extension = is_extension<T>::value;

    template <template <typename...> typename E, typename... T>
    concept ExtensionList = stl::TupleOf<is_extension, E, T...> || requires {
        (Extension<T> && ...);
    };


    template <typename T>
    concept ExtensionWithDependencies = requires {
        Extension<T>;
        typename T::required_extensions;
        Extension<typename T::required_extensions>;
    };

} // namespace webpp

#endif // WEBPP_EXTENSION_CONCEPTS_H
