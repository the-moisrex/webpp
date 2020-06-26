// Created by moisrex on 5/9/20.

#ifndef WEBPP_RESPONSE_CONCEPTS_H
#define WEBPP_RESPONSE_CONCEPTS_H

#include "../extensions/extension.h"
#include "../traits/std_traits.h"

namespace webpp {

    template <typename ResponseType, typename = void>
    struct is_response : stl::false_type {};

    template <typename ResponseType>
    struct is_response<
      ResponseType,
      stl::void_t<typename ResponseType::body_type, typename ResponseType::header_type,
                  typename ResponseType::traits_type,
                  decltype(stl::declval<ResponseType>().body, stl::declval<ResponseType>().header, (void)0)>>
      : stl::true_type {};

    struct fake_response_type {
        using body_type   = void;
        using header_type = void;
        using traits_type = fake_traits_type;
        stl::true_type body;
        stl::true_type header;
    };

    template <typename ResponseType>
    constexpr bool is_response_v = is_response<ResponseType>::value;

    template <typename ResType>
    concept Response = is_response_v<ResType>;

    template <typename T>
    concept ConvertibleToResponse =
      Response<T> || stl::is_convertible_v<T, typename std_traits_from_string<T>::type> ||
      stl::is_convertible_v<T, typename std_traits_from_string_view<T>::type>;


    template <typename T>
    concept ResponseExtension = Extension<T>;

    template <typename E>
    struct is_response_extension_list {
        static constexpr bool value = ResponseExtension<E>;
    };

    template <typename T>
    concept ResponseExtensionList = ExtensionListOf<T, is_response_extension_list>;

} // namespace webpp

#endif // WEBPP_RESPONSE_CONCEPTS_H
