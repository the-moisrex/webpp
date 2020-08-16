// Created by moisrex on 5/28/20.

#ifndef WEBPP_COOKIE_EXTENSIONS_H
#define WEBPP_COOKIE_EXTENSIONS_H

#include "../common.hpp"
#include "../header_concepts.hpp"

#include <algorithm>

namespace webpp {


    struct cookies {

        template <HeaderField HF>
        struct cookie_header_field_extension : public HF {

          private:
            using super = HF;

          public:
            using traits_type = typename super::traits_type;

            /**
             * Check if the header value is a cookie; it only checks the key not
             * the value
             */
            bool is_cookie() const noexcept {
                if constexpr (header_type::response == super::header_direction) {
                    return super::is_name("set-cookie") || super::is_name("set-cookie2");
                } else {
                    return super::is_name("cookie");
                }
            }

            /*
             * todo: move this to extensions:
             * Get the header as a cookie. Make sure to check if the cookie is
             * actually valid before using it.
             */
            auto as_cookie() noexcept {
                if constexpr (header_type::response == super::header_direction) {
                    if (is_cookie()) {
                        return response_cookie<traits_type>(value);
                    } else {
                        return response_cookie<traits_type>{}; // empty cookie
                    };
                } else {
                    if (is_cookie()) {
                        return request_cookie<traits_type>(value);
                    } else {
                        return request_cookie<traits_type>{}; // empty cookie
                    };
                }
            }
        };


        template <Header H>
        struct cookie_header_extension : public virtual H {
            using traits_type      = typename H::traits_type;
            using string_type      = typename traits_type::string_type;
            using string_view_type = typename traits_type::string_view_type;

          private:
            using super = H;

          public:
            /**
             * @brief get cookies
             * @return
             * todo: how to make the result mutable?
             */
            auto cookies() const noexcept {
                if constexpr (super::header_direction == header_type::request) {
                    request_cookie_jar<traits_type> cookies;
                    for (auto& c : *this)
                        if (c.is_cookie())
                            cookies.emplace(c);
                    return cookies;
                } else {
                    // response
                    response_cookie_jar<traits_type> cookies;
                    for (auto& c : *this)
                        if (c.is_cookie())
                            cookies.emplace(c);
                    return cookies;
                }
            }

            /**
             * @brief removes cookies in the cookie jar
             * This method will actually remove the cookie from the cookie jar
             * (and also from the header list itself). It doesn't remove the
             * cookies for the user. For removing the cookies for the user, use
             * the cookie jar templated class itself which you can get access to
             * with "cookies()" method.
             */
            void remove_cookies() noexcept {
                for (auto it = super::begin(); it != super::end();) {
                    if (it->is_cookie())
                        it = erase(it);
                    else
                        ++it;
                }
            }


            /**
             * @brief replace cookies in the cookie_jar with all the cookies in
             * the header list
             * @param cookie_jar
             */
            void replace_cookies(CookieJar auto const& _cookies) noexcept {
                remove_cookies();
                for (auto const& c : _cookies) {
                    emplace(c);
                }
            }


            /**
             * Replace a single cookie
             * @param _cookie
             */
            void replace_cookie(Cookie auto const& _cookie_to_find, Cookie auto const& cookie) noexcept {
                replace_cookie(find_cookie(_cookie_to_find), cookie);
            }

            /**
             * Replace a cookie based on its name
             * @param cookie_name
             */
            void replace_cookie(string_view_type const& cookie_name, Cookie auto const& cookie) noexcept {
                replace_cookie(find_cookie(cookie_name), cookie);
            }

            /**
             * Replace a cookie based on its iterator
             * For performance reasons this method does not check if you're
             * passing it a cookie or just a random header; you should check
             * that for yourself.
             * @param _cookie
             */
            void replace_cookie(super::iterator const& it, Cookie auto const& cookie) noexcept {
                if (it == super::end())
                    return;
                super::erase(it);
                emplace(cookie);
            }

            /**
             * Emplace a cookie
             * @param cookie
             */
            void emplace(Cookie auto const& cookie) noexcept {
                if constexpr (header_type::request == super::header_direction()) {
                    super::emplace("Cookie", cookie.request_str());
                } else {
                    super::emplace("Set-Cookie", cookie.response_str());
                }
            }

            /**
             * Find a cookie and return an iterator to it
             * @param _cookie
             * @return
             */
            auto find_cookie(Cookie auto const& _cookie) const noexcept {
                return ::std::find_if(super::begin(), super::end(), [&](auto const& this_header) {
                    return _cookie == this_header.as_cookie();
                });
            }

            /**
             * Find cookies based on its cookie name and return an iterator to
             * it
             * @param cookie_name
             * @return
             */
            auto find_cookie(string_view_type const& cookie_name) const noexcept {
                return ::std::find_if(super::begin(), super::end(), [&](auto const& this_header) {
                    return this_header.as_cookie().name() == cookie_name;
                });
            }
        }
    };

    template <HeaderField HF>
    using header_field_extensions = extension_pack<cookie_header_field_extension<HF>>;
}; // namespace webpp

} // namespace webpp

#endif // WEBPP_COOKIE_EXTENSIONS_H
