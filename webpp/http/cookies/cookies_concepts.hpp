// Created by moisrex on 5/27/20.

#ifndef WEBPP_HTTP_COOKIES_CONCEPTS_HPP
#define WEBPP_HTTP_COOKIES_CONCEPTS_HPP

#include "../../traits/traits.hpp"

namespace webpp::http {

    template <typename T>
    concept Cookie = requires(T cookie) {
        cookie.name();
        cookie.value();
    };

    template <typename T>
    concept ResponseCookie = Cookie<T> && requires(T cookie) {
        cookie.max_age();
        cookie.expires();
        cookie.path();
        cookie.secure();
        cookie.http_only();
    };

    template <typename T>
    concept RequestCookie = Cookie<T> && requires(T cookie) { cookie.is_valid(); };

    /**
     * A cookie encryptor/decryptor is a class that will encrypt and decrypt a cookie.
     * The types of cookie cryptors that I expect myself to implement:
     *   - [ ] CookieAESCryptor: requires a key, so it requires configuration
     *   - [ ] CookieCryptHash: hashes the value/[name]; requires database access or at least keep-alive-state
     *     - [ ] Is this a session?
     *   - [ ] CookieSessionizer: requires database ot keep-alive-storage; the value/[or optionally the name]
     *                            will be stored somewhere and the id will be replaced
     */
    template <typename T>
    concept CookieCryptor = requires(T encryptor) {
        encryptor.encrypt_value("value");
        encryptor.encrypt_name("name");
        // encryptor.encrypt(Cookie);
    };

} // namespace webpp::http

#endif // WEBPP_HTTP_COOKIES_CONCEPTS_HPP
