// Created by moisrex on 5/9/20.

#ifndef WEBPP_RESPONSE_CONCEPTS_H
#define WEBPP_RESPONSE_CONCEPTS_H

#include "../extensions/extension.h"
#include "../traits/std_traits.h"

namespace webpp {

    template <typename ResponseType, typename = void>
    struct is_response : std::false_type {};

    template <typename ResponseType>
    struct is_response<
      ResponseType,
      std::void_t<
        typename ResponseType::body_t, typename ResponseType::header_t,
        typename ResponseType::traits, typename ResponseType::str_t,
        typename ResponseType::str_view_t,
        decltype(
          std::declval<ResponseType>().body,
          std::declval<ResponseType>().header,
          std::declval<ResponseType>().operator typename ResponseType::str_t(),
          std::declval<ResponseType>().
          operator typename ResponseType::str_view_t(),
          (void)0)>> : std::true_type {};

    template <typename ResponseType>
    constexpr bool is_response_v = is_response<ResponseType>::value;

    template <typename ResType>
    concept Response = is_response_v<ResType>;

    template <typename T>
    concept ConvertibleToResponse =
      Response<T> ||
      std::is_convertible_v<T, typename std_traits_from_string<T>::type> ||
      std::is_convertible_v<T, typename std_traits_from_string_view<T>::type>;


    template <typename T>
    concept ResponseExtension = Extension<T>;

    template <typename E>
    struct is_response_extension_list {
        static constexpr bool value = ResponseExtension<E>;
    };

    template <typename T>
    concept ResponseExtensionList =
      ExtensionListOf<T, is_response_extension_list>;

} // namespace webpp

#endif // WEBPP_RESPONSE_CONCEPTS_H
