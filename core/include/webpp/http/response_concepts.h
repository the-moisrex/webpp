// Created by moisrex on 5/9/20.

#ifndef WEBPP_RESPONSE_CONCEPTS_H
#define WEBPP_RESPONSE_CONCEPTS_H

#include "../extensions/extension.h"
#include "../traits/std_traits.h"

namespace webpp {

    struct fake_response_type {
        using body_type   = void;
        using header_type = void;
        using traits_type = fake_traits_type;
        stl::true_type body;
        stl::true_type header;
    };

    template <typename ResType>
    concept Response = requires (ResType res) {
        typename ResType::body_type;
        typename ResType::header_type;
        typename ResType::traits_type;
        {res.body};
        {res.header};
    };

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
