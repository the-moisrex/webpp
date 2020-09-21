#ifndef WEBPP_UTILS_STRINGS_H
#define WEBPP_UTILS_STRINGS_H

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
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

    /**
     * This function does not work with the std::locale thus it's faster. It should
     * only be used where you need the default locale in which uses 'A' and 'Z'
     * chars. One example is in the HTTP headers which changing the locale should
     * not affect that.
     */
    constexpr auto to_upper_copy(istl::CharType auto&& c) noexcept {
        using char_type          = stl::remove_cvref_t<decltype(c)>;
        constexpr char_type diff = 'a' - 'A';
        return c >= 'a' && c <= 'z' ? c - diff : c;
    }

    template <typename CharT>
    requires(stl::is_integral_v<CharT>) inline void to_upper(CharT& c) noexcept {
        using char_type          = stl::remove_cvref_t<decltype(c)>;
        constexpr char_type diff = 'a' - 'A';
        c                        = c >= 'a' && c <= 'z' ? c - diff : c;
    }

    /**
     * This function does not work with the std::locale thus it's faster. It should
     * only be used where you need the default locale in which uses 'A' and 'Z'
     * chars. One example is in the HTTP headers which changing the locale should
     * not affect that.
     */
    constexpr auto to_lower_copy(istl::CharType auto&& c) noexcept {
        using char_type          = stl::remove_cvref_t<decltype(c)>;
        constexpr char_type diff = 'a' - 'A';
        return c >= 'A' && c <= 'Z' ? c + diff : c;
    }

    template <typename CharT>
    requires(stl::is_integral_v<CharT>) inline void to_lower(CharT& c) noexcept {
        using char_type          = stl::remove_cvref_t<decltype(c)>;
        constexpr char_type diff = 'a' - 'A';
        c                        = c >= 'A' && c <= 'Z' ? c + diff : c;
    }

// we've tried to use SIMD, but GCC and Clang's optimization beats us with this
// algorithm:
#define WEBPP_TO_METHOD(method)                                                  \
    template <typename CharT>                                                    \
    requires(stl::is_integral_v<CharT>) inline void method(CharT* it) noexcept { \
        for (; *it != '\0'; ++it)                                                \
            method(*it);                                                         \
    }                                                                            \
                                                                                 \
    inline void method(istl::ConvertibleToString auto& str) noexcept {           \
        using str_t          = stl::remove_cvref_t<decltype(str)>;               \
        using char_type      = istl::char_type_of<str_t>;                        \
        char_type*       it  = istl::string_data(str);                           \
        const char_type* end = it + size(str);                                   \
        for (; it != end; ++it)                                                  \
            method(*it);                                                         \
    }                                                                            \
                                                                                 \
    [[nodiscard]] inline auto method##_copy(istl::ConvertibleToString auto _str, \
                                            auto const&                    allocator) noexcept {    \
        auto str = istl::to_string(stl::move(_str), allocator);                  \
        method(str);                                                             \
        return str;                                                              \
    }                                                                            \
                                                                                 \
    [[nodiscard]] inline auto method##_copy(istl::String auto str) noexcept {    \
        method(str);                                                             \
        return str;                                                              \
    }


    WEBPP_TO_METHOD(to_upper)
    WEBPP_TO_METHOD(to_lower)

#undef WEBPP_TO_METHOD


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

    /**
     * todo: check performance of this and if all are the same, remove the
     * unnecessary ones
     */
    constexpr bool iequal(istl::ConvertibleToStringView auto&& _str1,
                          istl::ConvertibleToStringView auto&& _str2) noexcept {
        using str1_type = decltype(_str1);
        using str2_type = decltype(_str2);
        using str1_t    = stl::remove_cvref_t<str1_type>;
        using str2_t    = stl::remove_cvref_t<str2_type>;

        auto str1 = istl::to_string_view(_str1);
        auto str2 = istl::to_string_view(_str2);
        if (str1.size() != str2.size())
            return false;

        if constexpr (istl::String<str1_t> && istl::String<str1_t> && stl::is_rvalue_reference_v<str1_type> &&
                      stl::is_rvalue_reference_v<str2_type>) {
            to_lower(_str1);
            to_lower(_str2);
            return _str1 == _str2;
        } else if constexpr (istl::String<str1_t> && stl::is_rvalue_reference_v<str1_type>) {
            to_lower(_str1);
            return _str1 == to_lower_copy(_str2, _str1.get_allocator());
        } else if constexpr (istl::String<str2_t> && stl::is_rvalue_reference_v<str2_type>) {
            to_lower(_str2);
            return to_lower_copy(_str1, _str2.get_allocator()) == _str2;
        } else {
            return stl::equal(str1.cbegin(), str1.cend(), str2.cbegin(), [](auto&& c1, auto&& c2) {
                return c1 == c2 || to_lower(c1) == to_lower(c2);
            });
        }
    }

    //    template <typename ValueType, typename... R>
    //    requires(stl::is_integral_v<stl::remove_cvref_t<ValueType>>)
    //    constexpr auto to_str(istl::String auto& output, ValueType value, R&&...
    //    args) noexcept {
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
    //            auto [p, _]      = stl::to_chars(str, str + _size, value,
    //            stl::forward<R>(args)...); auto the_size =
    //            static_cast<size_type>(p - str); str_t     res(the_size, '\0');
    //            auto      it = res.begin();
    //            for (auto _c = str; *_c; ++_c) {
    //                *it++ = static_cast<char_type>(*_c);
    //            }
    //            return res;
    //        }
    //    }
    //

    template <typename T, stl::size_t N>
    constexpr stl::size_t size(const T (&array)[N]) noexcept {
        return N;
    }

    template <typename T>
    requires requires(T el) {
        el.size();
    }
    constexpr auto size(T&& str) noexcept {
        return str.size();
    }

    template <typename T>
    requires(istl::CharType<T>) constexpr stl::size_t size(T const* str) noexcept {
        if constexpr (stl::is_same_v<T, char>) {
            return stl::strlen(str);
        } else {
            const T* end = str;
            while (*end++ != 0)
                ;
            return end - str - 1;
        }
    }

} // namespace webpp

#endif // WEBPP_UTILS_STRINGS_H
