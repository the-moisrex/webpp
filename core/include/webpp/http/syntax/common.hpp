// Created by moisrex on 10/8/20.

#ifndef WEBPP_COMMON_HPP
#define WEBPP_COMMON_HPP

#include "../../strings/trim.hpp"

namespace webpp::http {

    static constexpr auto http_lws = charset(" \t");
    using http_lws_type = stl::remove_cvref_t<decltype(http_lws)>;


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
    // trim from start (leading whitespaces)
    template <typename Iter>
    static inline void ltrim_lws(Iter* begin, Iter const* end) noexcept {
        ascii::ltrim(begin, end, http_lws);
    }

    template <typename Iter>
    static inline void rtrim_lws(Iter const* begin, Iter* end) noexcept {
        ascii::rtrim(begin, end, http_lws);
    }

    template <typename Iter>
    static inline void trim_lws(Iter* begin, Iter* end) noexcept {
        ascii::trim(begin, end, http_lws);
    }

    // trim from start (in place)
    static inline void ltrim_lws(istl::StringView auto& str) noexcept {
        ascii::ltrim(str, http_lws);
    }

    static inline void rtrim_lws(istl::StringView auto& str) noexcept {
        ascii::rtrim(str, http_lws);
    }

    static inline void trim_lws(istl::StringView auto& str) noexcept {
        ascii::trim(str, http_lws);
    }


    // trim from start (copying)
    [[nodiscard]] static inline auto ltrim_copy_lws(istl::StringViewifiable auto&& s) noexcept {
        return ascii::ltrim_copy(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from end (copying)
    [[nodiscard]] static inline auto rtrim_copy_lws(istl::StringViewifiable auto&& s) noexcept {
        return ascii::rtrim_copy(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from both ends (copying)
    [[nodiscard]] static inline auto trim_copy_lws(istl::StringViewifiable auto&& s) noexcept {
        return ascii::trim_copy(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from start (in place)
    static inline void ltrim_lws(istl::String auto& s) noexcept {
        ascii::ltrim(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from end (in place)
    static inline void rtrim_lws(istl::String auto& s) noexcept {
        ascii::rtrim(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from both ends (in place)
    static inline void trim_lws(istl::String auto& s) noexcept {
        ascii::trim(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from start (copying)
    [[nodiscard]] static inline auto ltrim_copy_lws(istl::Stringifiable auto&& s, auto const& allocator) noexcept {
        return ascii::ltrim_copy(stl::forward<decltype(s)>(s), allocator, http_lws);
    }

    // trim from end (copying)
    [[nodiscard]] static inline auto rtrim_copy_lws(istl::Stringifiable auto&& s, auto const& allocator) noexcept {
        return ascii::rtrim_copy(stl::forward<decltype(s)>(s), allocator, http_lws);
    }

    // trim from both ends (copying)
    [[nodiscard]] inline auto trim_copy_lws(istl::Stringifiable auto&& s, auto const& allocator) noexcept {
        return ascii::trim_copy(stl::forward<decltype(s)>(s), allocator, http_lws);
    }
}

#endif // WEBPP_COMMON_HPP
