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
    // trim from start (leading whitespaces)
    template <typename Iter, istl::StringView StrViewType = decltype(http_lws)>
    static inline void ltrim_lws(Iter* begin, Iter const* end
                             ) noexcept {
        ascii::ltrim<Iter, StrViewType>(begin, end, http_lws);
    }

    template <typename Iter, istl::StringView StrViewType = decltype(http_lws)>
    static inline void rtrim_lws(Iter const* begin, Iter* end) noexcept {
        ascii::rtrim<Iter, StrViewType>(begin, end, http_lws);
    }

    template <typename Iter, istl::StringView StrViewType = decltype(http_lws)>
    static inline void trim_lws(Iter* begin, Iter* end) noexcept {
        ascii::trim<Iter, StrViewType>(begin, end, http_lws);
    }

    // trim from start (in place)
    template <istl::StringView StrViewType = decltype(http_lws)>
    static inline void ltrim_lws(StrViewType& str) noexcept {
        ascii::ltrim<StrViewType>(str, http_lws);
    }

    template <istl::StringView StrViewType = decltype(http_lws)>
    static inline void rtrim_lws(StrViewType& str) noexcept {
        ascii::rtrim<StrViewType>(str, http_lws);
    }

    template <istl::StringView StrViewType = decltype(http_lws)>
    static inline void trim_lws(StrViewType& str, StrViewType whitespaces = http_lws) noexcept {
        ascii::trim<StrViewType>(str, http_lws);
    }


    // trim from start (copying)
    template <istl::StringView StrViewType = decltype(http_lws)>
    [[nodiscard]] static inline auto ltrim_copy_lws(istl::StringViewifiable auto&& s) noexcept {
        return ascii::ltrim_copy<StrViewType>(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from end (copying)
    template <istl::StringView StrViewType = decltype(http_lws)>
    [[nodiscard]] static inline auto rtrim_copy_lws(istl::StringViewifiable auto&& s) noexcept {
        return ascii::rtrim_copy<StrViewType>(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from both ends (copying)
    template <istl::StringView StrViewType = decltype(http_lws)>
    [[nodiscard]] static inline auto trim_copy_lws(istl::StringViewifiable auto&& s) noexcept {
        return ascii::trim_copy<StrViewType>(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from start (in place)
    template <istl::StringView StrViewType = decltype(http_lws)>
    static inline void ltrim_lws(istl::String auto& s) noexcept {
        ascii::ltrim<StrViewType>(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from end (in place)
    template <istl::StringView StrViewType = decltype(http_lws)>
    static inline void rtrim_lws(istl::String auto& s) noexcept {
        ascii::rtrim<StrViewType>(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from both ends (in place)
    template <istl::StringView StrViewType = decltype(http_lws)>
    static inline void trim_lws(istl::String auto& s) noexcept {
        ascii::trim<StrViewType>(stl::forward<decltype(s)>(s), http_lws);
    }

    // trim from start (copying)
    template <istl::StringView StrViewType = decltype(http_lws)>
    [[nodiscard]] static inline auto ltrim_copy_lws(istl::Stringifiable auto&& s, auto const& allocator) noexcept {
        return ascii::ltrim_copy<StrViewType>(stl::forward<decltype(s)>(s), allocator, http_lws);
    }

    // trim from end (copying)
    template <istl::StringView StrViewType = decltype(http_lws)>
    [[nodiscard]] static inline auto rtrim_copy_lws(istl::Stringifiable auto&& s, auto const& allocator) noexcept {
        return ascii::rtrim_copy<StrViewType>(stl::forward<decltype(s)>(s), allocator, http_lws);
    }

    // trim from both ends (copying)
    template <istl::StringView StrViewType = decltype(http_lws)>
    [[nodiscard]] inline auto trim_copy_lws(istl::Stringifiable auto&& s, auto const& allocator) noexcept {
        return ascii::trim_copy<StrViewType>(stl::forward<decltype(s)>(s), allocator, http_lws);
    }
}

#endif // WEBPP_COMMON_HPP
