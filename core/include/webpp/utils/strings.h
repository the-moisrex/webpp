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
    inline void ltrim(std::string_view& s) noexcept {
        if (auto found = std::find_if_not(s.begin(), s.end(),
                                          [](auto const& c) -> bool {
                                              return std::isspace(c);
                                          });
            found != s.end()) {
            s.remove_prefix(
              static_cast<decltype(s.size())>(std::distance(s.begin(), found)));
        }
    }

    // trim from end (in place)
    inline void rtrim(std::string_view& s) noexcept {
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
    inline void trim(std::string_view& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    [[nodiscard]] inline std::string_view
    ltrim_copy(std::string_view s) noexcept {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    [[nodiscard]] inline std::string_view
    rtrim_copy(std::string_view s) noexcept {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    [[nodiscard]] inline std::string_view
    trim_copy(std::string_view s) noexcept {
        trim(s);
        return s;
    }

    // trim from start (in place)
    inline void ltrim(std::string& s) noexcept {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                    return !std::isspace(ch);
                }));
    }

    // trim from end (in place)
    inline void rtrim(std::string& s) noexcept {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](int ch) {
                                 return !std::isspace(ch);
                             })
                  .base(),
                s.end());
    }

    // trim from both ends (in place)
    inline void trim(std::string& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    [[nodiscard]] inline std::string ltrim_copy(std::string s) noexcept {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    [[nodiscard]] inline std::string rtrim_copy(std::string s) noexcept {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    [[nodiscard]] inline std::string trim_copy(std::string s) noexcept {
        trim(s);
        return s;
    }

    inline void to_lower(std::string& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        std::transform(str.cbegin(), str.cend(), str.begin(),
                       [](auto const& c) {
                           return std::tolower(c);
                       });
    }

    inline void to_upper(std::string& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        std::transform(str.cbegin(), str.cend(), str.begin(),
                       [](auto const& c) {
                           return std::tolower(c);
                       });
    }

    [[nodiscard]] inline std::string to_lower_copy(std::string str) noexcept {
        to_lower(str);
        return str;
    }

    [[nodiscard]] inline std::string to_upper_copy(std::string str) noexcept {
        to_upper(str);
        return str;
    }

    template <typename T>
    [[nodiscard]] constexpr inline bool starts_with(std::string_view const& str,
                                                    T&& data) noexcept {
#ifdef CXX20
        return str.starts_with(std::forward<T>(data));
#else
        return str.rfind(std::forward<T>(data), 0) == 0;
#endif
    }

    [[nodiscard]] constexpr inline bool ends_with(std::string_view const& str,
                                                  char c) noexcept {
        return !str.empty() && str.back() == c;
    }

    [[nodiscard]] constexpr inline bool
    ends_with(std::string_view const& str,
              std::string_view const& ending) noexcept {
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
