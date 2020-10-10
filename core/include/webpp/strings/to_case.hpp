#ifndef WEBPP_UTILS_STRINGS_H
#define WEBPP_UTILS_STRINGS_H

#include "../common/meta.hpp"
#include "../libs/eve.hpp"
#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
#include "../std/string_view.hpp"
#include "../traits/std_traits.hpp"
#include "../traits/traits_concepts.hpp"

// clang-format off
#include eve(wide)
#include eve(function/store)
#include eve(function/add)
#include eve(function/sub)
#include eve(function/any)
#include eve(function/is_less)
// clang-format on

#include <algorithm>
#include <type_traits>

namespace webpp::ascii {

    template <typename T>
    struct constexpr_array_type {
        static constexpr bool is_array = false;
    };

    template <typename T, stl::size_t N>
    struct constexpr_array_type<const T (&)[N]> {
        using array_type                          = T;
        static constexpr bool        is_array     = true;
        static constexpr stl::size_t array_length = N - (stl::is_same_v<T, char> || stl::is_same_v<T, wchar_t>); // for strings, we don't want to count the last null character
    };

    // it's best to use this for counting strings and not other types of arrays or containers, there's one edge case you should be aware; it may not count the last char if it's the size is known at compile time.
    template <typename T>
    [[nodiscard]] constexpr stl::size_t size(T&& str) noexcept {
        if constexpr (requires(T el) { el.size(); }) {
            return str.size();
        } else if constexpr (constexpr_array_type<T>::is_array) {
            return constexpr_array_type<T>::array_length; // todo: I'm not sure if this works or not
        } else if constexpr (stl::is_same_v<stl::remove_cvref_t<T>, char*>) {
            return stl::strlen(str);
        } else {
            // todo: is it possible to optimize this with SIMD?
            const T* end = str;
            while (*end++ != 0)
                ;
            return end - str - 1;
        }
    }



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


    /**
     * This function does not work with the std::locale thus it's faster. It should
     * only be used where you need the default locale in which uses 'A' and 'Z'
     * chars. One example is in the HTTP headers which changing the locale should
     * not affect that.
     */
    [[nodiscard]] constexpr auto to_upper_copy(istl::CharType auto&& c) noexcept {
        using char_type          = stl::remove_cvref_t<decltype(c)>;
        constexpr char_type diff = 'a' - 'A';
        return c >= 'a' && c <= 'z' ? c - diff : c;
    }

    template <typename CharT>
    requires(stl::is_integral_v<CharT> && !stl::is_const_v<CharT>) inline void to_upper(CharT& c) noexcept {
        using char_type          = stl::remove_cvref_t<decltype(c)>;
        static constexpr char_type diff = 'a' - 'A';
        if (c >= 'a' && c <= 'z')
            c -= diff;
    }

    /**
     * This function does not work with the std::locale thus it's faster. It should
     * only be used where you need the default locale in which uses 'A' and 'Z'
     * chars. One example is in the HTTP headers which changing the locale should
     * not affect that.
     */
    [[nodiscard]] constexpr auto to_lower_copy(istl::CharType auto&& c) noexcept {
        using char_type          = stl::remove_cvref_t<decltype(c)>;
        constexpr char_type diff = 'a' - 'A';
        return c >= 'A' && c <= 'Z' ? c + diff : c;
    }

    template <typename CharT>
    requires(stl::is_integral_v<CharT> && !stl::is_const_v<CharT>) inline void to_lower(CharT& c) noexcept {
        using char_type          = stl::remove_cvref_t<decltype(c)>;
        static constexpr char_type diff = 'a' - 'A';
        if (c >= 'A' && c <= 'Z')
            c += diff;
    }


    /**
     * The "algo" namespace is used for when there are multiple algorithms for the same purpose.
     */
    namespace algo {
#define WEBPP_ALGO(method)                                                                                   \
    template <typename CharT>                                                                                \
    requires(stl::is_integral_v<CharT> &&                                                                    \
             !stl::is_const_v<CharT>) inline void simple_##method(CharT* start, const CharT* end) noexcept { \
        using char_type = CharT;                                                                             \
        auto* it        = start;                                                                             \
        for (; it != end; ++it)                                                                              \
            method(*it);                                                                                     \
    }                                                                                                        \
                                                                                                             \
    inline void simple_##method(istl::ConvertibleToString auto& str) noexcept {                              \
        using str_t          = stl::remove_cvref_t<decltype(str)>;                                           \
        using char_type      = istl::char_type_of<str_t>;                                                    \
        char_type*       it  = istl::string_data(str);                                                       \
        const char_type* end = it + size(str);                                                               \
        for (; it != end; ++it)                                                                              \
            method(*it);                                                                                     \
    }

        WEBPP_ALGO(to_lower)
        WEBPP_ALGO(to_upper)
#undef WEBPP_ALGO

#ifdef WEBPP_EVE
        //        inline void eve_to_lower(std::string &str) noexcept
        //        {
        //            using char_type = char;
        //            constexpr auto simd_size = 64;
        //            using simd_type = eve::wide<char_type, eve::fixed<simd_size>>;
        //            using simd_utype = eve::wide<unsigned char, eve::fixed<simd_size>>;
        //
        //            char_type* it = str.data();
        //            const auto size = str.size();
        //            const char_type* end = it + size;
        //            const char_type* almost_end = end - (size % simd_size);
        //
        //            simd_utype shift(32);
        //            for (; it != almost_end; it += simd_size)
        //            {
        //                auto values  = eve::bit_cast(simd_type{it}, eve::as_<simd_utype>());
        //                auto shifted = values+shift;
        //                auto data = eve::if_else( eve::saturated_(eve::sub)(values - 65,25) == 0
        //                  , shifted
        //                  , values
        //                );
        //                eve::store( eve::bit_cast(data,eve::as_<simd_type>()), it);
        //            }
        //
        //            // doing the rest
        //            for (it -= simd_size; it != end; ++it)
        //                *it = std::tolower(*it);
        //        }


        inline void eve_to_lower(istl::ConvertibleToString auto& str) noexcept {
            using char_type                 = istl::char_type_of<decltype(str)>;
            using simd_type                 = eve::wide<char_type>;
            using simd_utype                = eve::wide<stl::make_unsigned_t<char_type>>;
            static constexpr auto simd_size = simd_type::size();

            char_type*       it    = istl::string_data(str);
            const auto       _size = size(str);
            const char_type* end   = it + _size;
            if (_size > simd_size) {
                const char_type* almost_end = end - (_size % simd_size);
                // const simd_utype shift(0b00100000);
                const simd_utype diff('a' - 'A');
                const simd_utype big_a('A');
                for (; it != almost_end; it += simd_size) {
                    const auto values = eve::bit_cast(simd_type{it}, eve::as_<simd_utype>());
                    const auto data =
                      eve::if_else(eve::is_less(eve::sub(values, big_a), 25), eve::add(values, diff), values);
                    eve::store(eve::bit_cast(data, eve::as_<simd_type>()), it);
                }
                // do the rest
                simple_to_lower(it - simd_size, end);
            } else {
                simple_to_lower(it, end);
            }
        }

        inline void eve_to_upper(istl::ConvertibleToString auto& str) noexcept {
            using char_type                 = istl::char_type_of<decltype(str)>;
            using simd_type                 = eve::wide<char_type>;
            using simd_utype                = eve::wide<stl::make_unsigned_t<char_type>>;
            static constexpr auto simd_size = simd_type::size();

            char_type*       it    = istl::string_data(str);
            const auto       _size = size(str);
            const char_type* end   = it + _size;
            if (_size > simd_size) {
                const char_type* almost_end = end - (_size % simd_size);
                // const simd_utype shift(0b01011111);
                const simd_utype diff('a' - 'A');
                const simd_utype small_a('a');
                for (; it != almost_end; it += simd_size) {
                    const auto values = eve::bit_cast(simd_type{it}, eve::as_<simd_utype>());
                    // const auto shifted = values & shift;
                    const auto data = eve::if_else(eve::is_less(eve::sub(values, small_a), 25),
                                                   eve::sub(values, diff), values);
                    eve::store(eve::bit_cast(data, eve::as_<simd_type>()), it);
                }
                // do the rest
                simple_to_upper(it - simd_size, end);
            } else {
                simple_to_upper(it, end);
            }
        }
#endif
    } // namespace algo


#define WEBPP_TO_METHOD(method, chosen_algorithm)                                           \
    template <typename CharT>                                                               \
    requires(stl::is_integral_v<CharT>) inline void method(CharT* it) noexcept {            \
        for (; *it != '\0'; ++it)                                                           \
            method(*it);                                                                    \
    }                                                                                       \
                                                                                            \
    inline void method(istl::ConvertibleToString auto& str) noexcept {                      \
        algo::chosen_algorithm##_##method(stl::forward<decltype(str)>(str));                \
    }                                                                                       \
                                                                                            \
    [[nodiscard]] inline auto method##_copy(istl::ConvertibleToString auto _str,            \
                                            auto const&                    allocator) noexcept {               \
        auto str = istl::to_string(stl::move(_str), allocator);                             \
        method(str);                                                                        \
        return str;                                                                         \
    }                                                                                       \
                                                                                            \
                                                                                            \
    [[nodiscard]] inline auto method##_copy(istl::ConvertibleToString auto _str) noexcept { \
        using char_type = istl::char_type_of<decltype(_str)>;                               \
        auto str        = istl::to_string(stl::move(_str), stl::allocator<char_type>());    \
        method(str);                                                                        \
        return str;                                                                         \
    }

#ifdef WEBPP_EVE
    WEBPP_TO_METHOD(to_upper, eve)
    WEBPP_TO_METHOD(to_lower, eve)
#else
    WEBPP_TO_METHOD(to_upper, simple)
    WEBPP_TO_METHOD(to_lower, simple)
#endif


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


} // namespace webpp

#endif // WEBPP_UTILS_STRINGS_H
