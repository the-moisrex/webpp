// Created by moisrex on 5/27/20.

#ifndef WEBPP_COOKIES_CONCEPTS_H
#define WEBPP_COOKIES_CONCEPTS_H

#include "../../traits/traits_concepts.hpp"

namespace webpp {

    template <typename T>
    concept Cookie = requires(T cookie) {
        {cookie.name()};
        {cookie.value()};
//        {T::header_direction};
        typename T::traits_type;
    }
    &&Traits<typename T::traits_type>;

    // todo: complete this
    template <typename T>
    concept ResponseCookie = Cookie<T>;

    // todo: complete this
    template <typename T>
    concept RequestCookie = Cookie<T>;

} // namespace webpp

#endif // WEBPP_COOKIES_CONCEPTS_H
