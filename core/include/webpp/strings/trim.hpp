// Created by moisrex on 10/8/20.

#ifndef WEBPP_TRIM_HPP
#define WEBPP_TRIM_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "./charset.hpp"

namespace webpp::ascii {

    // todo: do we need \v in here? it's not in chromium source code I think
    static constexpr CharSet auto standard_whitespaces = charset{" \n\r\t\f\v"};

    // trim from start (leading whitespaces)
    template <typename Iter, CharSet CS = decltype(ascii::standard_whitespaces)>
    static inline void ltrim(Iter* begin, Iter const* end,
                             CS whitespaces = standard_whitespaces) noexcept {
        while (*begin < *end && whitespaces.contains((*begin)[0]))
            ++(*begin);
    }

    template <typename Iter, CharSet CS = decltype(ascii::standard_whitespaces)>
    static inline void rtrim(Iter const* begin, Iter* end,
                             CS whitespaces = standard_whitespaces) noexcept {
        while (*begin < *end && whitespaces.contains((*end)[-1]))
            --(*end);
    }

    template <typename Iter, CharSet CS = decltype(standard_whitespaces)>
    static inline void trim(Iter* begin, Iter* end,
                             CS whitespaces = standard_whitespaces) noexcept {
        ltrim<Iter, CS>(begin, end, whitespaces);
        rtrim<Iter, CS>(begin, end, whitespaces);
    }

    // trim from start (in place)
    template <istl::StringView StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static inline void ltrim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        str.remove_prefix(stl::min(str.find_first_not_of(whitespaces.data(), 0, whitespaces.size()), str.size()));
    }

    template <istl::StringView StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static inline void rtrim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        std::size_t found = str.find_last_not_of(whitespaces.data());
        if (found != stl::remove_cvref_t<StrViewType>::npos)
            str.remove_suffix(str.size() - found - 1);
        else
            str.remove_suffix(str.size());
    }

    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static inline void trim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        ltrim<StrViewType>(str, whitespaces);
        rtrim<StrViewType>(str, whitespaces);
    }


    // trim from start (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto ltrim_copy(StrViewType&& s,
                                                CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::string_viewify(stl::forward<StrViewType>(s));
        ltrim(str, whitespaces);
        return str;
    }

    // trim from end (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto rtrim_copy(StrViewType&& s,
                                                CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::string_viewify(stl::forward<StrViewType>(s));
        rtrim(str, whitespaces);
        return str;
    }

    // trim from both ends (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto trim_copy(StrViewType&& s,
                                               CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::string_viewify(stl::forward<StrViewType>(s));
        trim(str, whitespaces);
        return str;
    }

    // trim from start (in place)
    template <CharSet CS = decltype(standard_whitespaces)>
    static inline void ltrim(istl::String auto& s, CS whitespaces = standard_whitespaces) noexcept {
        const auto pos = s.find_first_not_of(whitespaces.data());
        if (pos != stl::remove_cvref_t<decltype(s)>::npos)
            s.erase(0, pos);
    }

    // trim from end (in place)
    template <CharSet CS = decltype(standard_whitespaces)>
    static inline void rtrim(istl::String auto& s, CS whitespaces = standard_whitespaces) noexcept {
        const auto pos = s.find_last_not_of(whitespaces.data());
        if (pos == stl::remove_cvref_t<decltype(s)>::npos) {
            s.clear();
        } else {
            s.erase(pos + 1);
        }
    }

    // trim from both ends (in place)
    template <CharSet CS = decltype(standard_whitespaces)>
    static inline void trim(istl::String auto& s, CS whitespaces = standard_whitespaces) noexcept {
        ltrim(s, whitespaces);
        rtrim(s, whitespaces);
    }

    // trim from start (copying)
    template <CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto ltrim_copy(istl::Stringifiable auto&& s, auto const& allocator,
                                                CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::stringify(stl::forward<decltype(s)>(s), allocator);
        ltrim(str, whitespaces);
        return str;
    }

    // trim from end (copying)
    template <CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto rtrim_copy(istl::Stringifiable auto&& s, auto const& allocator,
                                                CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::stringify(stl::forward<decltype(s)>(s), allocator);
        rtrim(str, whitespaces);
        return str;
    }

    // trim from both ends (copying)
    template <CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] inline auto trim_copy(istl::Stringifiable auto&& s, auto const& allocator,
                                        CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::stringify(stl::forward<decltype(s)>(s), allocator);
        trim(str, whitespaces);
        return str;
    }
} // namespace webpp::ascii

#endif // WEBPP_TRIM_HPP
