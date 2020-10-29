// Created by moisrex on 10/29/20.

#ifndef WEBPP_ENUM_HPP
#define WEBPP_ENUM_HPP

#include "./type_traits.hpp"
#include "./concepts.hpp"

namespace webpp::istl {

    template <typename EnumType, typename T>
    concept EnumOf = stl::is_enum_v<EnumType> && (stl::same_as<T, EnumType> ||
      (stl::same_as<T, stl::underlying_type_t<T>> && requires (T mod) {
        static_cast<stl::underlying_type_t<T>>(mod) == mod;
    }));

    /**
     * Convert an integral type, or enum type itself to an enum type of the specified type (of type EnumType).
     */
    template <typename EnumType, typename T>
    constexpr EnumType to_enum_of(T data) noexcept {
        static_assert(stl::is_enum_v<EnumType>, "The specified type is not an enum.");

        if constexpr (stl::same_as<T, EnumType>) {
            return data;
        } else {
            return static_cast<EnumType>(data); // we'd get an error if this fails, and that's what we want.
        }
    }

}

#endif // WEBPP_ENUM_HPP
