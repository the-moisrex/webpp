#ifndef WEBPP_CASTS_H
#define WEBPP_CASTS_H

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/size.hpp"
#include "../traits/traits.hpp"

#include <charconv>
#include <stdexcept>

namespace webpp {

    /**
     * In this algorithm we're using begin, end, ... because in some string types (like utf-8), the chars
     * are not exactly stored the way we want them to be for that.
     *
     * todo: check overflows as well
     * todo: add another error system as well (that is not using exceptions)
     */
    template <typename T, T base = 10, bool throw_mistakes = false>
    constexpr T to(istl::StringViewifiable auto&& _str) noexcept(!throw_mistakes) {
        /**
         * glib's implementation if you need help: https://fossies.org/linux/glib/glib/gstrfuncs.c
         */

        constexpr bool is_signed = stl::is_same_v<T, stl::make_signed_t<T>>;
        const auto     str       = istl::string_viewify(stl::forward<decltype(_str)>(_str));
        using char_type          = istl::char_type_of<decltype(str)>;
        T    ret                 = 0;
        if (!str.size())
            return ret;

        auto c                   = str.begin();
        if constexpr (is_signed) {
            if (*c == '-' || *c == '+')
                c++; // first character can be - or +
        }
        for (; c != str.end(); c++) {
            auto ch = *c;
            if constexpr (base <= 10) {
                ch -= '0';
                if constexpr (throw_mistakes) {
                    if (ch <= '0' || ch >= '9')
                        throw stl::invalid_argument("The specified string is not a number");
                }
            } else if (base > 10) {
                if (ch >= 'a')
                    ch -= 'a' - 10;
                else if (ch >= 'A')
                    ch -= 'A' - 10;
                else
                    ch -= '0';
                if constexpr (throw_mistakes) {
                    if (ch > base) {
                        throw stl::invalid_argument("The specified string is not a number");
                    }
                }
            }
            ret *= base;
            ret += static_cast<T>(ch);
        }
        if constexpr (is_signed) {
            ret *= str.front() == '-' ? -1 : 1;
        }
        return ret;
    }

#define WEBPP_TO_FUNCTION(name, type)                                            \
    template <type base = 10, bool throw_mistakes = false>                       \
    constexpr auto to_##name(istl::StringViewifiable auto&& str) noexcept {      \
        return to<type, base, throw_mistakes>(stl::forward<decltype(str)>(str)); \
    }

    WEBPP_TO_FUNCTION(int, int);
    WEBPP_TO_FUNCTION(int8, int8_t);
    WEBPP_TO_FUNCTION(int16, int16_t);
    WEBPP_TO_FUNCTION(int32, int32_t);
    WEBPP_TO_FUNCTION(int64, int64_t);
    WEBPP_TO_FUNCTION(uint, unsigned int);
    WEBPP_TO_FUNCTION(uint8, uint8_t);
    WEBPP_TO_FUNCTION(uint16, uint16_t);
    WEBPP_TO_FUNCTION(uint32, uint32_t);
    WEBPP_TO_FUNCTION(uint64, uint64_t);

#undef WEBPP_TO_FUNCTION


    //////////////////////////////////////////////////////////////////////


    // todo: add allocator support here:
    template <Traits TraitsType, typename ValueType, typename... R>
    constexpr auto to_str_copy(ValueType value, R&&... args) noexcept {
        using char_type           = typename TraitsType::char_type;
        using str_t               = typename TraitsType::string_type;
        using size_type           = typename str_t::size_type;
        constexpr size_type _size = ascii::digit_count<ValueType>() + 1;
        if constexpr (stl::is_same_v<char_type, char>) {
            str_t str(_size, '\0');
            auto [p, _] = stl::to_chars(str.data(), str.data() + _size, value, stl::forward<R>(args)...);
            str.resize(p - str.data());
            return str;
        } else {
            char str[_size];
            auto [p, _]    = stl::to_chars(str, str + _size, value, stl::forward<R>(args)...);
            auto  the_size = static_cast<size_type>(p - str);
            str_t res(the_size, '\0');
            auto  it = res.begin();
            for (auto _c = str; *_c; ++_c) {
                *it++ = static_cast<char_type>(*_c);
            }
            return res;
        }
    }

    template <typename ValueType, typename... R>
    constexpr auto append_to(char* ptr, ValueType value, R&&... args) noexcept {
        constexpr stl::size_t _size = ascii::digit_count<ValueType>() + 1;
        return stl::to_chars(ptr, ptr + _size, value, stl::forward<R>(args)...);
    }

    // todo: GCC's to_chars implementation doesn't support floating point numbers
    template <typename ValueType, typename... R>
    constexpr bool append_to(istl::String auto& str, ValueType value, R&&... args) noexcept {
        constexpr stl::size_t   _size = ascii::digit_count<ValueType>() + 1;
        stl::array<char, _size> chars;
        if (auto res = stl::to_chars(chars.data(), chars.data() + _size, value, stl::forward<R>(args)...);
            res.ec == stl::errc()) {
            str.append(chars.data(), (res.ptr - chars.data()));
            return true;
        }
        return false;
    }



} // namespace webpp

#endif // WEBPP_CASTS_H
