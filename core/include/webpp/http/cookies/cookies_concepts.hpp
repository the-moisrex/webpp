// Created by moisrex on 5/27/20.

#ifndef WEBPP_COOKIES_CONCEPTS_H
#define WEBPP_COOKIES_CONCEPTS_H

#include "../../traits/traits.hpp"

namespace webpp {

    template <typename T>
    concept Cookie = requires(T cookie) {
        {cookie.name()};
        {cookie.value()};
    };

    template <typename T>
    concept ResponseCookie = Cookie<T> && requires (T cookie) {
      cookie.max_age();
      cookie.expires();
      cookie.path();
      cookie.secure();
      cookie.http_only();
    };

    template <typename T>
    concept RequestCookie = Cookie<T> && requires (T cookie) {
      cookie.is_valid();
    };

} // namespace webpp

#endif // WEBPP_COOKIES_CONCEPTS_H
