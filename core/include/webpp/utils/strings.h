#ifndef WEBPP_UTILS_STRINGS_H
#define WEBPP_UTILS_STRINGS_H

#include "../common/meta.h"
#include "../traits/std_traits.h"
#include "../traits/traits_concepts.h"

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
    template <Traits TraitsType>
    inline void ltrim(typename TraitsType::string_view_type& s) noexcept {
        if (auto found = stl::find_if_not(s.begin(), s.end(),
                                          [](auto const& c) -> bool {
                                              return c == ' ' || c == '\n' ||
                                                     c == '\r' || c == '\t' ||
                                                     c == '\f' || c == '\v';
                                          });
            found != s.end()) {
            s.remove_prefix(static_cast<decltype(s.size())>(
              ::std::distance(s.begin(), found)));
        }
    }


    // trim from end (in place)
    template <Traits TraitsType>
    inline void rtrim(typename TraitsType::string_view_type& s) noexcept {
        if (auto found = stl::find_if_not(s.rbegin(), s.rend(),
                                          [](auto const& c) -> bool {
                                              return ::std::isspace(c);
                                          });
            found != s.rend()) {
            s.remove_suffix(static_cast<decltype(s.size())>(
              ::std::distance(s.rbegin(), found)));
        }
    }

    // trim from both ends (in place)
    template <Traits TraitsType>
    inline void trim(typename TraitsType::string_view_type& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    template <Traits TraitsType>
    [[nodiscard]] inline typename TraitsType::string_view_type
    ltrim_copy(typename TraitsType::string_view_type s) noexcept {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    template <Traits TraitsType>
    [[nodiscard]] inline typename TraitsType::string_view_type
    rtrim_copy(typename TraitsType::string_view_type s) noexcept {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    template <Traits TraitsType>
    [[nodiscard]] inline typename TraitsType::string_view_type
    trim_copy(typename TraitsType::string_view_type s) noexcept {
        trim(s);
        return s;
    }

    // trim from start (in place)
    template <Traits TraitsType>
    inline void ltrim(typename TraitsType::string_type& s) noexcept {
        s.erase(s.begin(), stl::find_if(s.begin(), s.end(), [](auto c) {
                    return c != ' ' && c != '\n' && c != '\r' && c != '\t' &&
                           c != '\f' && c != '\v';
                }));
    }

    // trim from end (in place)
    template <Traits TraitsType>
    inline void rtrim(typename TraitsType::string_type& s) noexcept {
        s.erase(stl::find_if(s.rbegin(), s.rend(),
                             [](auto c) {
                                 return c != ' ' && c != '\n' && c != '\r' &&
                                        c != '\t' && c != '\f' && c != '\v';
                             })
                  .base(),
                s.end());
    }

    // trim from both ends (in place)
    template <Traits TraitsType>
    inline void trim(typename TraitsType::string_type& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    template <Traits TraitsType>
    [[nodiscard]] inline typename TraitsType::string_type
    ltrim_copy(typename TraitsType::string_type s) noexcept {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    template <Traits TraitsType>
    [[nodiscard]] inline typename TraitsType::string_type
    rtrim_copy(typename TraitsType::string_type s) noexcept {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    template <Traits TraitsType>
    [[nodiscard]] inline typename TraitsType::string_type
    trim_copy(typename TraitsType::string_type s) noexcept {
        trim(s);
        return s;
    }

    template <Traits TraitsType>
    inline void to_lower(typename TraitsType::stirng_type& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        stl::transform(str.cbegin(), str.cend(), str.begin(), [](auto c) {
            return ::std::tolower(c);
        });
    }

    template <Traits TraitsType>
    inline void to_upper(typename TraitsType::string_type& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        stl::transform(str.cbegin(), str.cend(), str.begin(),
                       [](auto const& c) {
                           return ::std::tolower(c);
                       });
    }

    template <Traits TraitsType>
    [[nodiscard]] inline typename TraitsType::string_type
    to_lower_copy(typename TraitsType::string_type str) noexcept {
        to_lower(str);
        return str;
    }

    template <Traits TraitsType>
    [[nodiscard]] inline typename TraitsType::string_type
    to_upper_copy(typename TraitsType::string_type str) noexcept {
        to_upper(str);
        return str;
    }

    template <Traits TraitsType, typename T>
    [[nodiscard]] constexpr bool
    starts_with(typename TraitsType::string_view_type const& str,
                T&&                                          data) noexcept {
#ifdef CXX20
        return str.starts_with(::std::forward<T>(data));
#else
        return str.rfind(::std::forward<T>(data), 0) == 0;
#endif
    }

    template <Traits TraitsType>
    [[nodiscard]] constexpr bool
    ends_with(typename TraitsType::string_view_type const& str,
              typename TraitsType::char_type               c) noexcept {
        return !str.empty() && str.back() == c;
    }

    template <Traits TraitsType>
    [[nodiscard]] constexpr bool
    ends_with(typename TraitsType::string_view_type const& str,
              typename TraitsType::string_view_type const& ending) noexcept {
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

#endif // WEBPP_UTILS_STRINGS_H
