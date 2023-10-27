// Created by moisrex on 6/1/20.

#ifndef WEBPP_OPTIONAL_H
#define WEBPP_OPTIONAL_H


#include "concepts.hpp"
#include "std.hpp"

#include <optional>
namespace webpp::istl {

    /**
     * Check if the specified type is a specialization of std::optional
     * We could use is_specialization_of for this, but it'll require including <type_traits>
     * so we just implement it manually here.
     */
    template <typename T>
    struct is_std_optional {
        static constexpr bool value = false;
    };

    template <typename T>
    struct is_std_optional<stl::optional<T>> {
        static constexpr bool value = true;
    };

    template <typename T>
    concept Optional =
      (is_std_optional<stl::remove_cvref_t<T>>::value || requires(stl::remove_cvref_t<T> obj) {
          typename stl::remove_cvref_t<T>::value_type;
          { obj.value() } -> stl::same_as<typename stl::remove_cvref_t<T>::value_type>;
          { obj.value_or(obj) } -> stl::same_as<typename stl::remove_cvref_t<T>::value_type>;
          { static_cast<bool>(obj) } -> stl::same_as<bool>; // convertible to bool
      });

    template <typename Q, typename T>
    concept OptionalOfType = Optional<T> && stl::same_as<typename T::value_type, Q>;

    template <template <typename> typename Q, typename T>
    concept OptionalOf = Optional<T> && Q<T>::value;

} // namespace webpp::istl

#endif // WEBPP_OPTIONAL_H
