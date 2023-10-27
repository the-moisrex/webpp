// Created by moisrex on 10/8/20.

#ifndef WEBPP_TRIM_HPP
#define WEBPP_TRIM_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "charset.hpp"

namespace webpp::ascii {

    // todo: do we need \v in here? it's not in chromium source code I think
    static constexpr CharSet auto standard_whitespaces = charset{" \n\r\t\f\v"};

    // trim from start (leading whitespaces)
    template <typename Iter, CharSet CS = decltype(ascii::standard_whitespaces)>
    static inline void ltrim(Iter* begin, Iter const* end, CS whitespaces = standard_whitespaces) noexcept {
        while (*begin < *end && whitespaces.contains((*begin)[0])) {
            ++(*begin);
        }
    }

    template <typename Iter, CharSet CS = decltype(ascii::standard_whitespaces)>
    static inline void rtrim(Iter const* begin, Iter* end, CS whitespaces = standard_whitespaces) noexcept {
        while (*begin < *end && whitespaces.contains((*end)[-1])) {
            --(*end);
        }
    }

    template <typename Iter, CharSet CS = decltype(standard_whitespaces)>
    static inline void trim(Iter* begin, Iter* end, CS whitespaces = standard_whitespaces) noexcept {
        ltrim<Iter, CS>(begin, end, whitespaces);
        rtrim<Iter, CS>(begin, end, whitespaces);
    }

    // trim from start (in place)
    template <istl::StringView StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static inline void ltrim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        str.remove_prefix(
          stl::min(str.find_first_not_of(whitespaces.data(), 0, whitespaces.size()), str.size()));
    }

    template <istl::StringView StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static inline void rtrim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        std::size_t found = str.find_last_not_of(whitespaces.data());
        if (found != stl::remove_cvref_t<StrViewType>::npos) {
            str.remove_suffix(str.size() - found - 1);
        } else {
            str.remove_suffix(str.size());
        }
    }

    template <istl::StringView StrViewType, CharSet CS = decltype(standard_whitespaces)>
    static inline void trim(StrViewType& str, CS whitespaces = standard_whitespaces) noexcept {
        ltrim<StrViewType>(str, whitespaces);
        rtrim<StrViewType>(str, whitespaces);
    }


    // trim from start (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto ltrim_copy(StrViewType&& inp_str,
                                                CS            whitespaces = standard_whitespaces) noexcept {
        auto str = istl::string_viewify(stl::forward<StrViewType>(inp_str));
        ltrim(str, whitespaces);
        return str;
    }

    // trim from end (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto rtrim_copy(StrViewType&& inp_str,
                                                CS            whitespaces = standard_whitespaces) noexcept {
        auto str = istl::string_viewify(stl::forward<StrViewType>(inp_str));
        rtrim(str, whitespaces);
        return str;
    }

    // trim from both ends (copying)
    template <istl::StringViewifiable StrViewType, CharSet CS = decltype(standard_whitespaces)>
    [[nodiscard]] static inline auto trim_copy(StrViewType&& inp_str,
                                               CS            whitespaces = standard_whitespaces) noexcept {
        auto str = istl::string_viewify(stl::forward<StrViewType>(inp_str));
        trim(str, whitespaces);
        return str;
    }

    // trim from start (in place)
    template <CharSet CS = decltype(standard_whitespaces), istl::String StrT = stl::string>
    static inline void ltrim(StrT& inp_str, CS whitespaces = standard_whitespaces) noexcept {
        const auto pos = inp_str.find_first_not_of(whitespaces.data());
        if (pos != StrT::npos) {
            inp_str.erase(0, pos);
        }
    }

    // trim from end (in place)
    template <CharSet CS = decltype(standard_whitespaces), istl::String StrT = stl::string>
    static inline void rtrim(StrT& inp_str, CS whitespaces = standard_whitespaces) noexcept {
        const auto pos = inp_str.find_last_not_of(whitespaces.data());
        if (pos == StrT::npos) {
            inp_str.clear();
        } else {
            inp_str.erase(pos + 1);
        }
    }

    // trim from both ends (in place)
    template <CharSet CS = decltype(standard_whitespaces)>
    static inline void trim(istl::String auto& inp_str, CS whitespaces = standard_whitespaces) noexcept {
        ltrim(inp_str, whitespaces);
        rtrim(inp_str, whitespaces);
    }

    // trim from start (copying)
    template <CharSet CS = decltype(standard_whitespaces), istl::Stringifiable StrT = stl::string_view>
    [[nodiscard]] static inline auto
    ltrim_copy(StrT&& inp_str, auto const& allocator, CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::stringify(stl::forward<StrT>(inp_str), allocator);
        ltrim(str, whitespaces);
        return str;
    }

    // trim from end (copying)
    template <CharSet CS = decltype(standard_whitespaces), istl::Stringifiable StrT = stl::string_view>
    [[nodiscard]] static inline auto
    rtrim_copy(StrT&& inp_str, auto const& allocator, CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::stringify(stl::forward<StrT>(inp_str), allocator);
        rtrim(str, whitespaces);
        return str;
    }

    // trim from both ends (copying)
    template <CharSet CS = decltype(standard_whitespaces), istl::Stringifiable StrT = stl::string_view>
    [[nodiscard]] inline auto
    trim_copy(StrT&& inp_str, auto const& allocator, CS whitespaces = standard_whitespaces) noexcept {
        auto str = istl::stringify(stl::forward<StrT>(inp_str), allocator);
        trim(str, whitespaces);
        return str;
    }
} // namespace webpp::ascii

#endif // WEBPP_TRIM_HPP
