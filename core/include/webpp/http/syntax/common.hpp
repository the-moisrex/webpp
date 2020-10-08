// Created by moisrex on 10/8/20.

#ifndef WEBPP_COMMON_HPP
#define WEBPP_COMMON_HPP

#include "../../strings/trim.hpp"

namespace webpp::http {

    static constexpr auto http_lws = " \t";


    // Return true if the character is HTTP "linear white space" (SP | HT).
    // This definition corresponds with the HTTP_LWS macro, and does not match
    // newlines.
    static constexpr bool is_lws(istl::CharType auto c) noexcept {
        using char_type = stl::remove_cvref_t<decltype(c)>;
        using str_v = stl::basic_string_view<char_type>;
        constexpr str_v whitespaces(http_lws);
        return whitespaces.find(c) != str_v::npos;
    }

    // Trim HTTP_LWS chars from the beginning and end of the string.
    static inline auto trim_lws(auto&&...args) noexcept {
        return ascii::trim(stl::forward<decltype(args)>(args)..., http_lws);
    }

    static inline auto ltrim_lws(auto&&...args) noexcept {
        return ascii::ltrim(stl::forward<decltype(args)>(args)..., http_lws);
    }

    static inline auto rtrim_lws(auto&&...args) noexcept {
        return ascii::rtrim(stl::forward<decltype(args)>(args)..., http_lws);
    }

    static inline auto ltrim_lws_copy(auto&&...args) noexcept {
        return ascii::ltrim_copy(stl::forward<decltype(args)>(args)..., http_lws);
    }

    static inline auto rtrim_lws_copy(auto&&...args) noexcept {
        return ascii::rtrim_copy(stl::forward<decltype(args)>(args)..., http_lws);
    }

    static inline auto trim_lws_copy(auto&&...args) noexcept {
        return ascii::trim_copy(stl::forward<decltype(args)>(args)..., http_lws);
    }
}

#endif // WEBPP_COMMON_HPP
