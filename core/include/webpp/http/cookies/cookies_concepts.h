// Created by moisrex on 5/27/20.

#ifndef WEBPP_COOKIES_CONCEPTS_H
#define WEBPP_COOKIES_CONCEPTS_H

namespace webpp {

    template <typename T>
    concept Cookie = requires(T cookie) {
        {cookie.name()};
        {cookie.value()};
        typename T::header_type;
        typename T::traits_type;
    };

    template <typename T>
    concept ResponseCookie = Cookie<T>&& requires(T cookie){

    };

    template <typename T>
    concept RequestCookie = Cookie<T>&& requires(T cookie){

    };

} // namespace webpp

#endif // WEBPP_COOKIES_CONCEPTS_H
