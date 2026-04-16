// Created by moisrex on 10/8/20.

#ifndef WEBPP_STRING_TRIM_HPP
#define WEBPP_STRING_TRIM_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "./charset.hpp"

namespace webpp::ascii {

    // todo: do we need \v in here? it's not in chromium source code I think
    static constexpr CharSet auto standard_whitespaces = charset{" \n\r\t\f\v"};

    // trim from start (leading whitespaces)
    template <typename Iter, CharSet CS = decltype(ascii::standard_whitespaces)>
    static constexpr void ltrim(Iter* begin, Iter const* end, CS whitespaces = standard_whitespaces) noexcept {
        while (*begin < *end && whitespaces.contains((*begin)[0])) {
            ++(*begin);
        }
    }

    template <typename Iter, CharSet CS = decltype(ascii::standard_whitespaces)>
    static constexpr void rtrim(Iter const* begin, Iter* end, CS whitespaces = standard_whitespaces) noexcept {
        while (*begin < *end && whitespaces.contains((*end)[-1])) {
            --(*end);
        }
    }

    template <typename Iter, CharSet CS = decltype(standard_whitespaces)>
    static constexpr void trim(Iter* begin, Iter* end, CS whitespaces = standard_whitespaces) noexcept {
        ltrim<Iter, CS>(begin, end, whitespaces);
        rtrim<Iter, CS>(begin, end, whitespaces);
    }

    // trim from start (in place)
    template <istl::StringView StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static constexpr void ltrim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        str.remove_prefix(stl::min(str.find_first_not_of(whitespaces.data(), 0, whitespaces.size()), str.size()));
    }

    template <istl::StringView StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static constexpr void rtrim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        // Explicitly pass npos as the starting position, and whitespaces.size() to avoid out-of-bounds reads
        std::size_t found =
          str.find_last_not_of(whitespaces.data(), stl::remove_cvref_t<StrViewType>::npos, whitespaces.size());
        if (found != stl::remove_cvref_t<StrViewType>::npos) {
            str.remove_suffix(str.size() - found - 1);
        } else {
            str.remove_suffix(str.size());
        }
    }

    template <istl::StringView StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static constexpr void trim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        ltrim<StrViewType>(str, whitespaces);
        rtrim<StrViewType>(str, whitespaces);
    }

    // trim from start (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static constexpr auto ltrim_copy(StrViewType&& inp_str,
                                                   CS            whitespaces = standard_whitespaces) noexcept {
        auto str = istl::view(stl::forward<StrViewType>(inp_str));
        ltrim(str, whitespaces);
        return str;
    }

    // trim from end (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static constexpr auto rtrim_copy(StrViewType&& inp_str,
                                                   CS            whitespaces = standard_whitespaces) noexcept {
        auto str = istl::view(stl::forward<StrViewType>(inp_str));
        rtrim(str, whitespaces);
        return str;
    }

    // trim from both ends (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static constexpr auto trim_copy(StrViewType&& inp_str,
                                                  CS            whitespaces = standard_whitespaces) noexcept {
        auto str = istl::view(stl::forward<StrViewType>(inp_str));
        trim(str, whitespaces);
        return str;
    }

    // trim from start (in place)
    template <CharSet CS = decltype(standard_whitespaces), istl::String StrT = stl::string>
    static constexpr void ltrim(StrT& inp_str, CS whitespaces = standard_whitespaces) noexcept {
        auto const pos = inp_str.find_first_not_of(whitespaces.data());
        if (pos != StrT::npos) {
            inp_str.erase(0, pos);
        }
    }

    // trim from end (in place)
    template <CharSet CS = decltype(standard_whitespaces), istl::String StrT = stl::string>
    static constexpr void rtrim(StrT& inp_str, CS whitespaces = standard_whitespaces) noexcept {
        auto const pos = inp_str.find_last_not_of(whitespaces.data());
        if (pos == StrT::npos) {
            inp_str.clear();
        } else {
            inp_str.erase(pos + 1);
        }
    }

    // trim from both ends (in place)
    template <CharSet CS = decltype(standard_whitespaces)>
    static constexpr void trim(istl::String auto& inp_str, CS whitespaces = standard_whitespaces) noexcept {
        ltrim(inp_str, whitespaces);
        rtrim(inp_str, whitespaces);
    }

    // trim from start (copying)
    template <CharSet CS = decltype(standard_whitespaces), istl::String StrT = stl::string>
    [[nodiscard]] static constexpr auto ltrim_copy(StrT str, CS whitespaces = standard_whitespaces) noexcept {
        ltrim(str, whitespaces);
        return str;
    }

    // trim from end (copying)
    template <CharSet CS = decltype(standard_whitespaces), istl::String StrT = stl::string>
    [[nodiscard]] static constexpr auto rtrim_copy(StrT str, CS whitespaces = standard_whitespaces) noexcept {
        rtrim(str, whitespaces);
        return str;
    }

    // trim from both ends (copying)
    template <CharSet CS = decltype(standard_whitespaces), istl::String StrT = stl::string>
    [[nodiscard]] static constexpr auto trim_copy(StrT str, CS whitespaces = standard_whitespaces) noexcept {
        trim(str, whitespaces);
        return str;
    }
} // namespace webpp::ascii

#endif // WEBPP_STRING_TRIM_HPP
