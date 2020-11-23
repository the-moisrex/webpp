// Created by moisrex on 5/28/20.

#ifndef WEBPP_HEADER_CONCEPTS_H
#define WEBPP_HEADER_CONCEPTS_H

#include "../extensions/extension.hpp"
#include "../std/concepts.hpp"
#include "../traits/traits_concepts.hpp"

namespace webpp {

    template <typename T>
    concept Header = requires(T h) {
        requires Traits<typename T::traits_type>;
        { T::header_direction };
        { T::is_mutable } -> stl::same_as<bool>;
    };

    template <typename T>
    concept HeaderField = requires(T f) {
       requires Traits<typename T::traits_type>;
        { T::header_direction };
        { T::is_mutable } -> stl::same_as<bool>;
        requires requires (typename T::traits_type::string_view_type str_view) {
            { f.is_name(str_view) } -> stl::same_as<bool>;
        };
    };
//    &&HasExtensionSupport<T>;

//    template <typename T>
//    concept HeaderExtension = ;

//    template <typename T>
//    concept HeaderFieldExtension = ;

} // namespace webpp

#endif // WEBPP_HEADER_CONCEPTS_H
