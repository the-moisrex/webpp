#ifndef WEBPP_CASTS_H
#define WEBPP_CASTS_H

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/size.hpp"
#include "../traits/traits.hpp"

#include <charconv>
#include <stdexcept>

#ifndef __cpp_lib_to_chars
#    include "../std/format.hpp"
#endif

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

        const auto str = istl::string_viewify(stl::forward<decltype(_str)>(_str));
        T          ret = 0;
        if (!str.size())
            return ret;

        auto c = str.begin();
        if (*c == '-' || *c == '+')
            c++; // first character can be - or +
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
        ret *= static_cast<T>(str.front() == '-' ? -1 : 1);
        return ret;
    }

#define WEBPP_TO_FUNCTION(name, type)                                            \
    template <type base = 10, bool throw_mistakes = false>                       \
    constexpr auto to_##name(istl::StringViewifiable auto&& str) noexcept {      \
        return to<type, base, throw_mistakes>(stl::forward<decltype(str)>(str)); \
    }

    WEBPP_TO_FUNCTION(int, int)
    WEBPP_TO_FUNCTION(int8, int8_t)
    WEBPP_TO_FUNCTION(int16, int16_t)
    WEBPP_TO_FUNCTION(int32, int32_t)
    WEBPP_TO_FUNCTION(int64, int64_t)
    WEBPP_TO_FUNCTION(uint, unsigned int)
    WEBPP_TO_FUNCTION(uint8, uint8_t)
    WEBPP_TO_FUNCTION(uint16, uint16_t)
    WEBPP_TO_FUNCTION(uint32, uint32_t)
    WEBPP_TO_FUNCTION(uint64, uint64_t)

#undef WEBPP_TO_FUNCTION


    //////////////////////////////////////////////////////////////////////


    // todo: add allocator support here and remove the traits:
    //
    //    template <Traits TraitsType, typename ValueType, typename... R>
    //    constexpr auto to_str_copy(ValueType value, R&&... args) noexcept {
    //        using traits_type         = TraitsType;
    //        using char_type           = traits::char_type<traits_type>;
    //        using str_t               = traits::string<traits_type>;
    //        using size_type           = typename str_t::size_type;
    //        constexpr size_type _size = ascii::digit_count<ValueType>() + 1;
    //        if constexpr (stl::is_same_v<char_type, char>) {
    //            str_t str(_size, '\0');
    //            auto [p, _] = stl::to_chars(str.data(), str.data() + _size, value,
    //            stl::forward<R>(args)...); str.resize(p - str.data()); return str;
    //        } else {
    //            char str[_size];
    //            auto [p, _]    = stl::to_chars(str, str + _size, value, stl::forward<R>(args)...);
    //            auto  the_size = static_cast<size_type>(p - str);
    //            str_t res(the_size, '\0');
    //            auto  it = res.begin();
    //            for (auto _c = str; *_c; ++_c) {
    //                *it++ = static_cast<char_type>(*_c);
    //            }
    //            return res;
    //        }
    //    }

    template <typename ValueType, typename... R>
    constexpr auto append_to(char* ptr, ValueType value, [[maybe_unused]] R&&... args) noexcept {
#ifdef __cpp_lib_to_chars
        constexpr stl::size_t _size = ascii::digit_count<ValueType>() + 1;
        return stl::to_chars(ptr, ptr + _size, value, stl::forward<R>(args)...);
#else
        // todo: use floating point format options feom "args"
        fmt::format_to(ptr, "{}", value);
        return true;
#endif
    }

    template <typename ValueType, typename... R>
    constexpr bool append_to(istl::String auto& str, ValueType value, R&&... args) noexcept {
        using value_type                 = stl::remove_cvref_t<ValueType>;
        constexpr stl::size_t value_size = sizeof(value_type);

        // converting any value type to 3 possible float value types
        using float_type =
          stl::conditional_t<(value_size <= sizeof(float)),
                             float,
                             stl::conditional_t<(value_size == sizeof(double)), double, long double>>;


        if constexpr (istl::StringViewifiable<value_type>) {
            str.append(value);
            (append_to(str, stl::forward<R>(args)), ...);
            return true;
        } else {
#ifdef __cpp_lib_to_chars
            constexpr stl::size_t   _size = ascii::digit_count<value_type>() + 1;
            stl::array<char, _size> chars;
            if (auto res = stl::to_chars(chars.data(),
                                         chars.data() + _size,
                                         static_cast<float_type>(value), // to remove ambiguity
                                         stl::forward<R>(args)...);
                res.ec == stl::errc()) {
                str.append(chars.data(), static_cast<stl::size_t>(res.ptr - chars.data()));
                return true;
            }
            return false;
#else
            fmt::format_to(stl::back_inserter(str), "{}", value);
            return true;
#endif
        }
    }



} // namespace webpp

#endif // WEBPP_CASTS_H
