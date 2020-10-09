// Created by moisrex on 10/8/20.

#ifndef WEBPP_TRIM_HPP
#define WEBPP_TRIM_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"

namespace webpp::ascii {

    // todo: do we need \v in here? it's not in chromium source code I think
    static constexpr stl::string_view standard_whitespaces{" \n\r\t\f\v"};

    // trim from start (leading whitespaces)
    template <typename Iter, istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void ltrim(Iter* begin, Iter const* end,
                             StrViewType whitespaces = standard_whitespaces) noexcept {
        while (*begin < *end && whitespaces.find((*begin)[0]) != decltype(whitespaces)::npos)
            ++(*begin);
    }

    template <typename Iter, istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void rtrim(Iter const* begin, Iter* end,
                             StrViewType whitespaces = standard_whitespaces) noexcept {
        while (*begin < *end && whitespaces.find((*end)[-1]) != decltype(whitespaces)::npos)
            --(*end);
    }

    template <typename Iter, istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void trim(Iter* begin, Iter* end,
                             StrViewType whitespaces = standard_whitespaces) noexcept {
        ltrim<Iter, StrViewType>(begin, end, whitespaces);
        rtrim<Iter, StrViewType>(begin, end, whitespaces);
    }

    // trim from start (in place)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void ltrim(StrViewType& str, StrViewType whitespaces = standard_whitespaces) noexcept {
        str.remove_prefix(stl::min(str.find_first_not_of(whitespaces.data()), str.size()));
    }

    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void rtrim(StrViewType& str, StrViewType whitespaces = standard_whitespaces) noexcept {
        str.remove_suffix(stl::min(str.find_last_not_of(whitespaces.data()), str.size()));
    }

    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void trim(StrViewType& str, StrViewType whitespaces = standard_whitespaces) noexcept {
        ltrim<StrViewType>(str, whitespaces);
        rtrim<StrViewType>(str, whitespaces);
    }


    // trim from start (copying)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto ltrim_copy(istl::ConvertibleToStringView auto&& s,
                                                StrViewType whitespaces = standard_whitespaces) noexcept {
        auto str = istl::to_string_view(stl::forward<decltype(s)>(s));
        ltrim(str, whitespaces);
        return str;
    }

    // trim from end (copying)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto rtrim_copy(istl::ConvertibleToStringView auto&& s,
                                                StrViewType whitespaces = standard_whitespaces) noexcept {
        auto str = istl::to_string_view(stl::forward<decltype(s)>(s));
        rtrim(str, whitespaces);
        return str;
    }

    // trim from both ends (copying)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto trim_copy(istl::ConvertibleToStringView auto&& s,
                                               StrViewType whitespaces = standard_whitespaces) noexcept {
        auto str = istl::to_string_view(stl::forward<decltype(s)>(s));
        trim(str, whitespaces);
        return str;
    }

    // trim from start (in place)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void ltrim(istl::String auto& s, StrViewType whitespaces = standard_whitespaces) noexcept {
        s.erase(0, s.find_first_not_of(whitespaces.data()));
    }

    // trim from end (in place)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void rtrim(istl::String auto& s, StrViewType whitespaces = standard_whitespaces) noexcept {
        s.erase(s.find_last_not_of(whitespaces.data()));
    }

    // trim from both ends (in place)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    static inline void trim(istl::String auto& s, StrViewType whitespaces = standard_whitespaces) noexcept {
        ltrim(s, whitespaces);
        rtrim(s, whitespaces);
    }

    // trim from start (copying)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto ltrim_copy(istl::ConvertibleToString auto&& s, auto const& allocator,
                                                StrViewType whitespaces = standard_whitespaces) noexcept {
        auto str = istl::to_string(stl::forward<decltype(s)>(s), allocator);
        ltrim(str, whitespaces);
        return str;
    }

    // trim from end (copying)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto rtrim_copy(istl::ConvertibleToString auto&& s, auto const& allocator,
                                                StrViewType whitespaces = standard_whitespaces) noexcept {
        auto str = istl::to_string(stl::forward<decltype(s)>(s), allocator);
        rtrim(str, whitespaces);
        return str;
    }

    // trim from both ends (copying)
    template <istl::StringView StrViewType = decltype(standard_whitespaces)>
    [[nodiscard]] inline auto trim_copy(istl::ConvertibleToString auto&& s, auto const& allocator,
                                        StrViewType whitespaces = standard_whitespaces) noexcept {
        auto str = istl::to_string(stl::forward<decltype(s)>(s), allocator);
        trim(str, whitespaces);
        return str;
    }
} // namespace webpp::ascii

#endif // WEBPP_TRIM_HPP
