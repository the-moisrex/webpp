// Created by moisrex on 10/8/20.

#ifndef WEBPP_CODEC_COMMON_HPP
#define WEBPP_CODEC_COMMON_HPP

#include "../../strings/trim.hpp"

namespace webpp::http {

    static constexpr auto http_lws = charset(" \t");
    using http_lws_type            = stl::remove_cvref_t<decltype(http_lws)>;

    // Return true if the character is HTTP "linear white space" (SP | HT).
    // This definition corresponds with the HTTP_LWS macro, and does not match
    // newlines.
    template <istl::CharType CharT>
    static constexpr bool is_lws(CharT const code_point) noexcept {
        using str_v = stl::basic_string_view<CharT>;
        constexpr str_v whitespaces(http_lws);
        return whitespaces.find(code_point) != str_v::npos;
    }

    // Trim HTTP_LWS chars from the beginning and end of the string.
    // trim from start (leading whitespaces)
    template <typename Iter>
    static void ltrim_lws(Iter* begin, Iter const* end) noexcept {
        ascii::ltrim(begin, end, http_lws);
    }

    template <typename Iter>
    static void rtrim_lws(Iter const* begin, Iter* end) noexcept {
        ascii::rtrim(begin, end, http_lws);
    }

    template <typename Iter>
    static void trim_lws(Iter* begin, Iter* end) noexcept {
        ascii::trim(begin, end, http_lws);
    }

    // trim from start (in place)
    template <typename CharT>
    static void ltrim_lws(stl::basic_string_view<CharT>& str) noexcept {
        ascii::ltrim(str, http_lws);
    }

    template <typename CharT>
    static void rtrim_lws(stl::basic_string_view<CharT>& str) noexcept {
        ascii::rtrim(str, http_lws);
    }

    template <typename CharT>
    static void trim_lws(stl::basic_string_view<CharT>& str) noexcept {
        ascii::trim(str, http_lws);
    }

    // trim from start (copying)
    template <typename CharT>
    [[nodiscard]] static auto ltrim_copy_lws(stl::basic_string_view<CharT> const str) noexcept {
        return ascii::ltrim_copy(str, http_lws);
    }

    // trim from end (copying)
    template <typename CharT>
    [[nodiscard]] static auto rtrim_copy_lws(stl::basic_string_view<CharT> const str) noexcept {
        return ascii::rtrim_copy(str, http_lws);
    }

    // trim from both ends (copying)
    template <typename CharT>
    [[nodiscard]] static auto trim_copy_lws(stl::basic_string_view<CharT> const str) noexcept {
        return ascii::trim_copy(str, http_lws);
    }

    // trim from start (in place)
    static void ltrim_lws(istl::String auto& str) noexcept {
        ascii::ltrim(str, http_lws);
    }

    // trim from end (in place)
    static void rtrim_lws(istl::String auto& str) noexcept {
        ascii::rtrim(str, http_lws);
    }

    // trim from both ends (in place)
    static void trim_lws(istl::String auto& str) noexcept {
        ascii::trim(str, http_lws);
    }

    // trim from start (copying)
    template <typename CharT, typename AllocT>
    [[nodiscard]] static auto ltrim_copy_lws(
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT> const& str) noexcept {
        return ascii::ltrim_copy(str, http_lws);
    }

    // trim from end (copying)
    template <typename CharT, typename AllocT>
    [[nodiscard]] static auto rtrim_copy_lws(
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT> const& str) noexcept {
        return ascii::rtrim_copy(str, http_lws);
    }

    // trim from both ends (copying)
    template <typename CharT, typename AllocT>
    [[nodiscard]] static auto trim_copy_lws(
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT> const& str) noexcept {
        return ascii::trim_copy(str, http_lws);
    }
} // namespace webpp::http

#endif // WEBPP_CODEC_COMMON_HPP
