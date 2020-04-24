#ifndef STRINGS_H
#define STRINGS_H

#include "../common/meta.h"

#include <algorithm>
#include <string>
#include <string_view>
#include <type_traits>

namespace webpp {

    /**
     * Getting the character type
     * It's used for character encoding
     */
    //    template <typename StringTypeRaw>
    //    using auto_char_type = std::conditional_t<
    //      std::is_integral_v<StringTypeRaw>,
    //      std::remove_cvref_t<StringTypeRaw>,
    //      std::conditional_t<std::is_integral_v<typename
    //      StringTypeRaw::value_type>,
    //                         typename StringTypeRaw::value_type, char>>;

    /**
     * Getting the appropriate string type to use.
     * If the specified string type cannot be changed, the string_view will
     * be used, otherwise, string itself.
     */
    //    template <typename StringTypeRaw, typename CharT>
    //    using auto_string_type = std::conditional_t<
    //      std::is_empty_v<StringTypeRaw>, std::basic_string<CharT>,
    //      std::conditional_t<
    //        std::is_array_v<StringTypeRaw> ||
    //          std::is_convertible_v<StringTypeRaw, char const* const> ||
    //          std::is_const_v<StringTypeRaw>,
    //        std::basic_string_view<CharT>, StringTypeRaw>>;

    // trim from start (in place)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>>
    inline void ltrim(std::basic_string_view<CharT, CharTraits>& s) noexcept {
        if (auto found = std::find_if_not(s.begin(), s.end(),
                                          [](auto const& c) -> bool {
                                              // todo: should we use std::isspace here?
                                              return std::isspace(c);
                                          });
            found != s.end()) {
            s.remove_prefix(
              static_cast<decltype(s.size())>(std::distance(s.begin(), found)));
        }
    }

    // trim from end (in place)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>>
    inline void rtrim(std::basic_string_view<CharT, CharTraits>& s) noexcept {
        if (auto found = std::find_if_not(s.rbegin(), s.rend(),
                                          [](auto const& c) -> bool {
                                              return std::isspace(c);
                                          });
            found != s.rend()) {
            s.remove_suffix(static_cast<decltype(s.size())>(
              std::distance(s.rbegin(), found)));
        }
    }

    // trim from both ends (in place)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>>
    inline void trim(std::basic_string_view<CharT, CharTraits>& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>>
    [[nodiscard]] inline std::basic_string_view<CharT, CharTraits>
    ltrim_copy(std::basic_string_view<CharT, CharTraits> s) noexcept {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>>
    [[nodiscard]] inline std::basic_string_view<CharT, CharTraits>
    rtrim_copy(std::basic_string_view<CharT, CharTraits> s) noexcept {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>>
    [[nodiscard]] inline std::basic_string_view<CharT, CharTraits>
    trim_copy(std::basic_string_view<CharT, CharTraits> s) noexcept {
        trim(s);
        return s;
    }

    // trim from start (in place)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    inline void
    ltrim(std::basic_string<CharT, CharTraits, Allocator>& s) noexcept {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                    return !std::isspace(ch);
                }));
    }

    // trim from end (in place)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    inline void
    rtrim(std::basic_string<CharT, CharTraits, Allocator>& s) noexcept {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](int ch) {
                                 return !std::isspace(ch);
                             })
                  .base(),
                s.end());
    }

    // trim from both ends (in place)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    inline void
    trim(std::basic_string<CharT, CharTraits, Allocator>& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    [[nodiscard]] inline std::basic_string<CharT, CharTraits, Allocator>
    ltrim_copy(std::basic_string<CharT, CharTraits, Allocator> s) noexcept {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    [[nodiscard]] inline std::basic_string<CharT, CharTraits, Allocator>
    rtrim_copy(std::basic_string<CharT, CharTraits, Allocator> s) noexcept {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    [[nodiscard]] inline std::basic_string<CharT, CharTraits, Allocator>
    trim_copy(std::basic_string<CharT, CharTraits, Allocator> s) noexcept {
        trim(s);
        return s;
    }

    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    inline void
    to_lower(std::basic_string<CharT, CharTraits, Allocator>& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        std::transform(str.cbegin(), str.cend(), str.begin(),
                       [](auto const& c) {
                           return std::tolower(c);
                       });
    }

    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    inline void
    to_upper(std::basic_string<CharT, CharTraits, Allocator>& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        std::transform(str.cbegin(), str.cend(), str.begin(),
                       [](auto const& c) {
                           return std::tolower(c);
                       });
    }

    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    [[nodiscard]] inline std::basic_string<CharT, CharTraits, Allocator>
    to_lower_copy(
      std::basic_string<CharT, CharTraits, Allocator> str) noexcept {
        to_lower(str);
        return str;
    }

    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>,
              typename Allocator  = std::allocator<CharT>>
    [[nodiscard]] inline std::basic_string<CharT, CharTraits, Allocator>
    to_upper_copy(
      std::basic_string<CharT, CharTraits, Allocator> str) noexcept {
        to_upper(str);
        return str;
    }

    template <typename T, typename CharT = char,
              typename CharTraits = std::char_traits<CharT>>
    [[nodiscard]] constexpr inline bool
    starts_with(std::basic_string_view<CharT, CharTraits> const& str,
                T&& data) noexcept {
#ifdef CXX20
        return str.starts_with(std::forward<T>(data));
#else
        return str.rfind(std::forward<T>(data), 0) == 0;
#endif
    }

    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>>
    [[nodiscard]] constexpr inline bool
    ends_with(std::basic_string_view<CharT, CharTraits> const& str,
              CharT                                            c) noexcept {
        return !str.empty() && str.back() == c;
    }

    template <typename CharT      = char,
              typename CharTraits = std::char_traits<CharT>>
    [[nodiscard]] constexpr inline bool ends_with(
      std::basic_string_view<CharT, CharTraits> const& str,
      std::basic_string_view<CharT, CharTraits> const& ending) noexcept {
#ifdef CXX20
        return str.ends_with(ending);
#else
        if (str.length() >= ending.length()) {
            return (0 == str.compare(str.length() - ending.length(),
                                     ending.length(), ending));
        }
        return false;
#endif
    }

} // namespace webpp

#endif // STRINGS_H
