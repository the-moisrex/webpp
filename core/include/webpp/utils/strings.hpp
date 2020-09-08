#ifndef WEBPP_UTILS_STRINGS_H
#define WEBPP_UTILS_STRINGS_H

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../traits/std_traits.hpp"
#include "../traits/traits_concepts.hpp"

#include <algorithm>
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
    inline void ltrim(istl::StringView auto& s) noexcept {
        if (auto found = stl::find_if_not(s.begin(), s.end(),
                                          [](auto const& c) -> bool {
                                              return c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
                                                     c == '\f' || c == '\v';
                                          });
            found != s.end()) {
            s.remove_prefix(static_cast<decltype(s.size())>(stl::distance(s.begin(), found)));
        }
    }


    // trim from end (in place)
    inline void rtrim(istl::StringView auto& s) noexcept {
        if (auto found = stl::find_if_not(s.rbegin(), s.rend(),
                                          [](auto const& c) -> bool {
                                              return stl::isspace(c);
                                          });
            found != s.rend()) {
            s.remove_suffix(static_cast<decltype(s.size())>(stl::distance(s.rbegin(), found)));
        }
    }

    // trim from both ends (in place)
    inline void trim(istl::StringView auto& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    [[nodiscard]] inline auto ltrim_copy(istl::ConvertibleToStringView auto&& s) noexcept {
        auto str = istl::to_string_view(stl::forward<decltype(s)>(s));
        ltrim(str);
        return str;
    }

    // trim from end (copying)
    [[nodiscard]] inline auto rtrim_copy(istl::ConvertibleToStringView auto&& s) noexcept {
        auto str = istl::to_string_view(stl::forward<decltype(s)>(s));
        rtrim(str);
        return str;
    }

    // trim from both ends (copying)
    [[nodiscard]] inline auto trim_copy(istl::ConvertibleToStringView auto&& s) noexcept {
        auto str = istl::to_string_view(stl::forward<decltype(s)>(s));
        trim(str);
        return str;
    }

    // trim from start (in place)
    inline void ltrim(istl::String auto& s) noexcept {
        s.erase(s.begin(), stl::find_if(s.begin(), s.end(), [](auto c) {
                    return c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != '\f' && c != '\v';
                }));
    }

    // trim from end (in place)
    inline void rtrim(istl::String auto& s) noexcept {
        s.erase(stl::find_if(s.rbegin(), s.rend(),
                             [](auto c) {
                                 return c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != '\f' &&
                                        c != '\v';
                             })
                  .base(),
                s.end());
    }

    // trim from both ends (in place)
    inline void trim(istl::String auto& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    [[nodiscard]] inline auto ltrim_copy(istl::ConvertibleToString auto&& s, auto const& allocator) noexcept {
        auto str = istl::to_string(stl::forward<decltype(s)>(s), allocator);
        ltrim(str);
        return str;
    }

    // trim from end (copying)
    [[nodiscard]] inline auto rtrim_copy(istl::ConvertibleToString auto&& s, auto const& allocator) noexcept {
        auto str = istl::to_string(stl::forward<decltype(s)>(s), allocator);
        rtrim(str);
        return str;
    }

    // trim from both ends (copying)
    [[nodiscard]] inline auto trim_copy(istl::ConvertibleToString auto&& s, auto const& allocator) noexcept {
        auto str = istl::to_string(stl::forward<decltype(s)>(s), allocator);
        trim(str);
        return str;
    }

    inline void to_lower(istl::String auto& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        stl::transform(str.cbegin(), str.cend(), str.begin(), [](auto c) {
            return stl::tolower(c);
        });
    }

    inline void to_upper(istl::String auto& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        stl::transform(str.cbegin(), str.cend(), str.begin(), [](auto const& c) {
            return stl::tolower(c);
        });
    }

    [[nodiscard]] inline auto to_lower_copy(istl::ConvertibleToString auto _str,
                                            auto const&                    allocator) noexcept {
        auto str = istl::to_string(stl::forward<decltype(_str)>(_str), allocator);
        to_lower(str);
        return str;
    }

    [[nodiscard]] inline auto to_lower_copy(istl::String auto str) noexcept {
        to_lower(str);
        return str;
    }

    [[nodiscard]] auto to_upper_copy(istl::ConvertibleToString auto _str, auto const& allocator) noexcept {
        auto str = istl::to_string(stl::forward<decltype(_str)>(_str), allocator);
        to_upper(str);
        return str;
    }

    [[nodiscard]] auto to_upper_copy(istl::String auto str) noexcept {
        to_upper(str);
        return str;
    }

    template <typename T>
    [[nodiscard]] constexpr bool starts_with(istl::ConvertibleToStringView auto&& _str, T&& data) noexcept {
        auto str = istl::to_string_view(_str);
#ifdef CXX20
        return str.starts_with(stl::forward<T>(data));
#else
        return str.rfind(stl::forward<T>(data), 0) == 0;
#endif
    }

    [[nodiscard]] constexpr bool ends_with(istl::ConvertibleToStringView auto&& _str,
                                           istl::char_type_of<decltype(_str)>   c) noexcept {
        auto str = istl::to_string_view(_str);
        return !str.empty() && str.back() == c;
    }

    [[nodiscard]] constexpr bool ends_with(istl::ConvertibleToStringView auto&& _str,
                                           istl::ConvertibleToStringView auto&& _ending) noexcept {
        auto                   str = istl::to_string_view(_str);
        stl::basic_string_view ending{_ending};
#ifdef CXX20
        return str.ends_with(ending);
#else
        if (str.length() >= ending.length()) {
            return (0 == str.compare(str.length() - ending.length(), ending.length(), ending));
        }
        return false;
#endif
    }



    //    template <typename ValueType, typename... R>
    //    requires(stl::is_integral_v<stl::remove_cvref_t<ValueType>>)
    //    constexpr auto to_str(istl::String auto& output, ValueType value, R&&... args) noexcept {
    //        using str_t               = stl::remove_cvref_t<decltype(output)>;
    //        using char_type           = typename str_t::value_type;
    //        using size_type           = typename str_t::size_type;
    //        constexpr size_type _size = digit_count<ValueType>() + 1;
    //        if constexpr (stl::is_same_v<char_type, char>) {
    //            str_t str(_size, '\0');
    //            auto [p, _] = stl::to_chars(str.data(), str.data() + _size, value,
    //            stl::forward<R>(args)...); str.resize(p - str.data()); return str;
    //        } else {
    //            char str[_size];
    //            auto [p, _]      = stl::to_chars(str, str + _size, value, stl::forward<R>(args)...);
    //            auto the_size = static_cast<size_type>(p - str);
    //            str_t     res(the_size, '\0');
    //            auto      it = res.begin();
    //            for (auto _c = str; *_c; ++_c) {
    //                *it++ = static_cast<char_type>(*_c);
    //            }
    //            return res;
    //        }
    //    }
    //

} // namespace webpp

#endif // WEBPP_UTILS_STRINGS_H
