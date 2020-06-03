// Created by moisrex on 5/28/20.

#ifndef WEBPP_HEADER_CONCEPTS_H
#define WEBPP_HEADER_CONCEPTS_H

#include "../extensions/extension.h"
#include "../std/concepts.h"

namespace webpp {

    template <typename T>
    concept Header = requires(T h) {
        typename T::traits_type;
        {T::header_direction};
        { T::is_mutable }
        ->std::same_as<bool>;
    };

    template <typename T>
    concept HeaderField = requires(T f) {
        typename T::traits_type;
        {T::header_direction};
        { T::is_mutable }
        ->std::same_as<bool>;
        { f.is_name(typename(typename T::traits_type)::string_view_type{}) }
        ->std::same_as<bool>;
    }
    &&HasExtensionSupport<T>;

    template <typename T>
    concept HeaderExtension = ;

    template <typename T>
    concept HeaderFieldExtension = ;

} // namespace webpp

#endif // WEBPP_HEADER_CONCEPTS_H
