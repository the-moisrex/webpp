#ifndef WEBPP_UTILS_STRINGS_H
#define WEBPP_UTILS_STRINGS_H

#include "../common/meta.hpp"
#include "../libs/eve.hpp"
#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
#include "../std/string_view.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/traits.hpp"
#include "size.hpp"

#include <algorithm>


#ifdef WEBPP_EVE
#    include <eve/algo/transform.hpp>
#    include <eve/algo/views/zip.hpp>
#    include <eve/function/add.hpp>
#    include <eve/function/sub.hpp>
#    include <eve/wide.hpp>
#endif


namespace webpp::ascii {

    /**
     * This function does not work with the std::locale thus it's faster. It should
     * only be used where you need the default locale in which uses 'A' and 'Z'
     * chars. One example is in the HTTP headers which changing the locale should
     * not affect that.
     */
    template <istl::CharType CharT>
    [[nodiscard]] constexpr CharT to_upper_copy(CharT c) noexcept {
        webpp_static_constexpr CharT diff = 'a' - 'A';
        return c >= 'a' && c <= 'z' ? c - diff : c;
    }

    template <istl::CharType CharT>
        requires(!stl::is_const_v<CharT>)
    static constexpr void to_upper(CharT& c) noexcept {
        webpp_static_constexpr CharT diff = 'a' - 'A';
        if (c >= 'a' && c <= 'z')
            c -= diff;
    }

    /**
     * This function does not work with the std::locale thus it's faster. It should
     * only be used where you need the default locale in which uses 'A' and 'Z'
     * chars. One example is in the HTTP headers which changing the locale should
     * not affect that.
     */
    template <istl::CharType CharT>
    [[nodiscard]] constexpr auto to_lower_copy(CharT c) noexcept {
        webpp_static_constexpr CharT diff = 'a' - 'A';
        return c >= 'A' && c <= 'Z' ? c + diff : c;
    }

    template <istl::CharType CharT>
        requires(!stl::is_const_v<CharT>)
    static constexpr void to_lower(CharT& c) noexcept {
        using char_type                       = stl::remove_cvref_t<decltype(c)>;
        webpp_static_constexpr char_type diff = 'a' - 'A';
        if (c >= 'A' && c <= 'Z')
            c += diff;
    }


    /**
     * The "algo" namespace is used for when there are multiple algorithms for the same purpose.
     */
    namespace algo {
#define WEBPP_ALGO(method)                                                           \
    template <istl::CharType CharT>                                                  \
        requires(!stl::is_const_v<CharT>)                                            \
    static constexpr void simple_##method(CharT* start, const CharT* end) noexcept { \
        auto* it = start;                                                            \
        for (; it != end; ++it)                                                      \
            method(*it);                                                             \
    }                                                                                \
                                                                                     \
    static constexpr void simple_##method(istl::Stringifiable auto& str) noexcept {  \
        using str_t          = stl::remove_cvref_t<decltype(str)>;                   \
        using char_type      = istl::char_type_of_t<str_t>;                          \
        char_type*       it  = istl::string_data(str);                               \
        const char_type* end = it + size(str);                                       \
        for (; it != end; ++it)                                                      \
            method(*it);                                                             \
    }

        WEBPP_ALGO(to_lower)
        WEBPP_ALGO(to_upper)
#undef WEBPP_ALGO

#ifdef WEBPP_EVE

        static inline void eve_to_lower(istl::String auto& str) noexcept {
            using value_type  = typename stl::remove_cvref_t<decltype(str)>::value_type;
            using char_type   = stl::make_unsigned_t<value_type>;
            auto       start  = reinterpret_cast<char_type*>(str.data());
            const auto finish = start + str.size();
            eve::algo::transform_inplace(eve::algo::as_range(start, finish), [](auto c) {
                static constexpr char_type alphabet_length = 'z' - 'a';
                static constexpr char_type a_A_offset      = 'a' - 'A';
                return eve::add[(c - 'A') <= alphabet_length](c, a_A_offset);
            });
        }

        static inline void eve_to_upper(istl::String auto& str) noexcept {
            using value_type  = typename stl::remove_cvref_t<decltype(str)>::value_type;
            using char_type   = stl::make_unsigned_t<value_type>;
            auto       start  = reinterpret_cast<char_type*>(str.data());
            const auto finish = start + str.size();
            eve::algo::transform_inplace(eve::algo::as_range(start, finish), [](auto c) {
                static constexpr char_type alphabet_length = 'z' - 'a';
                static constexpr char_type a_A_offset      = 'a' - 'A';
                return eve::sub[(c - 'a') <= alphabet_length](c, a_A_offset);
            });
        }

#endif
    } // namespace algo


#define WEBPP_TO_METHOD(method, chosen_algorithm, constexpr_state)                                    \
    template <istl::CharType CharT>                                                                   \
    static constexpr_state void method(CharT* it) noexcept {                                          \
        for (; *it != '\0'; ++it)                                                                     \
            method(*it);                                                                              \
    }                                                                                                 \
                                                                                                      \
    static constexpr_state void method(istl::Stringifiable auto& str) noexcept {                      \
        algo::chosen_algorithm##_##method(stl::forward<decltype(str)>(str));                          \
    }                                                                                                 \
                                                                                                      \
    [[nodiscard]] static constexpr_state auto method##_copy(istl::Stringifiable auto _str,            \
                                                            auto const&              allocator) noexcept {         \
        auto str = istl::stringify(stl::move(_str), allocator);                                       \
        method(str);                                                                                  \
        return str;                                                                                   \
    }                                                                                                 \
                                                                                                      \
                                                                                                      \
    [[nodiscard]] static constexpr_state auto method##_copy(istl::Stringifiable auto _str) noexcept { \
        using char_type = istl::char_type_of_t<decltype(_str)>;                                       \
        auto str        = istl::stringify(stl::move(_str), stl::allocator<char_type>());              \
        method(str);                                                                                  \
        return str;                                                                                   \
    }

#ifdef WEBPP_EVE
    WEBPP_TO_METHOD(to_upper, eve, inline)
    WEBPP_TO_METHOD(to_lower, eve, inline)
#else
    WEBPP_TO_METHOD(to_upper, simple, constexpr)
    WEBPP_TO_METHOD(to_lower, simple, constexpr)
#endif


#undef WEBPP_TO_METHOD


    template <typename T>
    [[nodiscard]] constexpr bool starts_with(istl::StringViewifiable auto&& _str, T&& data) noexcept {
        return istl::string_viewify(_str).starts_with(stl::forward<T>(data));
    }

    [[nodiscard]] constexpr bool ends_with(istl::StringViewifiable auto&&       _str,
                                           istl::char_type_of_t<decltype(_str)> c) noexcept {
        const auto str = istl::string_viewify(_str);
        return !str.empty() && str.back() == c;
    }

    [[nodiscard]] constexpr bool ends_with(istl::StringViewifiable auto&& _str,
                                           istl::StringViewifiable auto&& _ending) noexcept {
        return istl::string_viewify(_str).ends_with(istl::string_viewify(_ending));
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


} // namespace webpp::ascii

#endif // WEBPP_UTILS_STRINGS_H
