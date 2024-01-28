#ifndef WEBPP_UTILS_STRINGS_H
#define WEBPP_UTILS_STRINGS_H

#include "../libs/eve.hpp"
#include "../std/string.hpp"
#include "../std/string_concepts.hpp"
#include "../std/string_view.hpp"
#include "size.hpp"

#include <algorithm>


#ifdef WEBPP_EVE
#    include <eve/algo/transform.hpp>
#    include <eve/algo/views/zip.hpp>
#    include <eve/function/add.hpp>
#    include <eve/function/sub.hpp>
#    include <eve/wide.hpp>
#endif

// todo: it's possible to use ASCII lookup table instead of if-else statements for changing the case
namespace webpp::ascii {

    /**
     * This function does not work with the std::locale thus it's faster. It should
     * only be used where you need the default locale in which uses 'A' and 'Z'
     * chars. One example is in the HTTP headers which changing the locale should
     * not affect that.
     */
    template <istl::CharType CharT>
    [[nodiscard]] constexpr CharT to_upper_copy(CharT inp_char) noexcept {
        webpp_static_constexpr CharT diff = 'a' - 'A';
        return inp_char >= 'a' && inp_char <= 'z' ? inp_char - diff : inp_char;
    }

    template <istl::CharType CharT>
        requires(!stl::is_const_v<CharT>)
    static constexpr void to_upper(CharT& inp_char) noexcept {
        webpp_static_constexpr CharT diff = 'a' - 'A';
        if (inp_char >= 'a' && inp_char <= 'z') {
            inp_char -= diff;
        }
    }

    /**
     * This function does not work with the std::locale thus it's faster. It should
     * only be used where you need the default locale in which uses 'A' and 'Z'
     * chars. One example is in the HTTP headers which changing the locale should
     * not affect that.
     */
    template <istl::CharType CharT>
    [[nodiscard]] constexpr CharT to_lower_copy(CharT inp_char) noexcept {
        webpp_static_constexpr CharT diff = 'a' - 'A';
        return inp_char >= 'A' && inp_char <= 'Z' ? inp_char + diff : inp_char;
    }

    template <istl::CharType CharT>
        requires(!stl::is_const_v<CharT>)
    static constexpr void to_lower(CharT& inp_char) noexcept {
        webpp_static_constexpr CharT diff = 'a' - 'A';
        if (inp_char >= 'A' && inp_char <= 'Z') {
            inp_char += diff;
        }
    }

    /**
     * The "algo" namespace is used for when there are multiple algorithms for the same purpose.
     */
    namespace algo {
        // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_ALGO(method)                                                           \
    template <istl::CharType CharT>                                                  \
        requires(!stl::is_const_v<CharT>)                                            \
    static constexpr void simple_##method(CharT* start, const CharT* end) noexcept { \
        for (; start != end; ++start) {                                              \
            method(*start);                                                          \
        }                                                                            \
    }                                                                                \
                                                                                     \
    static constexpr void simple_##method(istl::Stringifiable auto& str) noexcept {  \
        using str_t          = stl::remove_cvref_t<decltype(str)>;                   \
        using char_type      = istl::char_type_of_t<str_t>;                          \
        const char_type* end = istl::string_data(str) + size(str);                   \
        for (char_type* pos = istl::string_data(str); pos != end; ++pos) {           \
            method(*pos);                                                            \
        }                                                                            \
    }

        WEBPP_ALGO(to_lower)
        WEBPP_ALGO(to_upper)
#undef WEBPP_ALGO

#ifdef WEBPP_EVE

        static inline void eve_to_lower(istl::String auto& str) noexcept {
            using value_type  = typename stl::remove_cvref_t<decltype(str)>::value_type;
            using char_type   = stl::make_unsigned_t<value_type>;
            auto       start  = reinterpret_cast<char_type*>(str.data());
            auto const finish = start + str.size();
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
            auto const finish = start + str.size();
            eve::algo::transform_inplace(eve::algo::as_range(start, finish), [](auto c) {
                static constexpr char_type alphabet_length = 'z' - 'a';
                static constexpr char_type a_A_offset      = 'a' - 'A';
                return eve::sub[(c - 'a') <= alphabet_length](c, a_A_offset);
            });
        }

#endif
    } // namespace algo

    // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_TO_METHOD(method, chosen_algorithm, constexpr_state)                                    \
    template <istl::CharType CharT>                                                                   \
    static constexpr_state void method(CharT* pos) noexcept {                                         \
        for (; *pos != '\0'; ++pos)                                                                   \
            method(*pos);                                                                             \
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
    [[nodiscard]] constexpr bool starts_with(istl::StringViewifiable auto&& inp_str, T&& data) noexcept {
        return istl::string_viewify(inp_str).starts_with(stl::forward<T>(data));
    }

    [[nodiscard]] constexpr bool ends_with(istl::StringViewifiable auto&&          inp_str,
                                           istl::char_type_of_t<decltype(inp_str)> inp_char) noexcept {
        auto const str = istl::string_viewify(inp_str);
        return !str.empty() && str.back() == inp_char;
    }

    [[nodiscard]] constexpr bool ends_with(istl::StringViewifiable auto&& inp_str,
                                           istl::StringViewifiable auto&& inp_ending) noexcept {
        return istl::string_viewify(inp_str).ends_with(istl::string_viewify(inp_ending));
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

    [[nodiscard]] static consteval stl::uint64_t broadcast(stl::uint8_t const val) noexcept {
        return 0x101'0101'0101'0101ULL * val; // NOLINT(*-magic-numbers)
    }

    template <typename InpIter, typename OutIter>
    constexpr void lower_to(InpIter inp, OutIter out, stl::size_t const length) noexcept {
        using char_type       = typename stl::iterator_traits<InpIter>::value_type;
        using difference_type = typename stl::iterator_traits<InpIter>::difference_type;
        // if constexpr (sizeof(char_type) == sizeof(char)) {
        //     webpp_static_constexpr auto broadcast_80 = broadcast(0x80U);
        //     webpp_static_constexpr auto A_pack       = broadcast(128 - 'A');
        //     webpp_static_constexpr auto Z_pack       = broadcast(128 - 'Z' - 1);
        //     stl::size_t                 index        = 0;
        //
        //     // using SWAR (SIMD Within A Register)
        //     for (; index - sizeof(stl::uint64_t) <= length;
        //          index                       += sizeof(stl::uint64_t))
        //     {
        //         webpp_assume(*inp < 128 && *inp >= 0);
        //         stl::uint64_t word{};
        //         stl::memcpy(&word, inp + index, sizeof(word));
        //         word ^= (((word + A_pack) ^ (word + Z_pack)) & broadcast_80) >> 2U;
        //         stl::memcpy(out + index, &word, sizeof(word));
        //     }
        //
        //     // the remaining characters:
        //     if (index < length) {
        //         stl::uint64_t word{};
        //         stl::memcpy(&word, inp + index, length - index);
        //         word ^= (((word + A_pack) ^ (word + Z_pack)) & broadcast_80) >> 2U;
        //         stl::memcpy(out + index, &word, length - index);
        //     }
        // } else {
        auto const end        = inp + static_cast<difference_type>(length);
        while (inp != end) {
            *out++ = to_lower_copy<char_type>(*inp++);
        }
        // }
    }

    template <typename InpIter, typename OutIter>
    constexpr void upper_to(InpIter inp, OutIter out, stl::size_t const length) noexcept {
        using difference_type = typename stl::iterator_traits<InpIter>::difference_type;
        auto const end        = inp + static_cast<difference_type>(length);
        while (inp != end) {
            *out++ = to_upper_copy(*inp++);
        }
    }

    template <typename It, typename EIt = It, istl::String StrT>
    constexpr void lower_to(StrT& out, It beg, EIt end) {
#if __cpp_lib_string_resize_and_overwrite
        out.resize_and_overwrite(end - beg, [beg](auto* ptr, stl::size_t const length) constexpr noexcept {
            lower_to(beg, ptr, length);
            return length;
        });
#else
        using size_type = typename StrT::size_type;
        out.resize(static_cast<size_type>(end - beg));
        lower_to(beg, out.begin(), static_cast<stl::size_t>(end - beg));
#endif
    }

    template <typename It, typename EIt = It, istl::String StrT>
    constexpr void upper_to(StrT& out, It beg, EIt end) {
#if __cpp_lib_string_resize_and_overwrite
        out.resize_and_overwrite(end - beg, [beg](auto* ptr, stl::size_t const length) constexpr noexcept {
            upper_to(beg, ptr, length);
            return length;
        });
#else
        using size_type = typename StrT::size_type;
        out.resize(static_cast<size_type>(end - beg));
        upper_to(beg, out.begin(), static_cast<stl::size_t>(end - beg));
#endif
    }

} // namespace webpp::ascii


#endif // WEBPP_UTILS_STRINGS_H
