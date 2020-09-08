#ifndef WEBPP_CASTS_H
#define WEBPP_CASTS_H

#include "../std/string_view.hpp"
#include "../traits/traits_concepts.hpp"

#include <charconv>
#include <stdexcept>

namespace webpp {

    template <typename T, bool is_signed = true, bool throw_mistakes = false>
    constexpr T to(istl::ConvertibleToStringView auto&& _str) noexcept(!throw_mistakes) {
        stl::basic_string_view str{_str};
        T                      ret = 0;
        if (str.size() > 0) {
            // todo: minus is not used!!
            if constexpr (is_signed) {
                auto c = str.cbegin();
                if (*c == '-' || *c == '+')
                    c++; // first character can be - or +
                for (; c != str.cend(); c++) {
                    if constexpr (throw_mistakes) {
                        if (*c <= '0' || *c >= '9')
                            throw stl::invalid_argument("The specified string is not a number");
                    }
                    ret *= 10;
                    ret += static_cast<T>(*c - '0');
                }
                ret *= str.front() == '-' ? -1 : 1;
            } else {
                for (auto const& c : str) {
                    if constexpr (throw_mistakes) {
                        if (c <= '0' || c >= '9')
                            throw stl::invalid_argument("The specified string is not a number");
                    }
                    ret *= 10;
                    ret += static_cast<T>(c - '0');
                }
            }
        }
        return ret;
    }

#define WEBPP_TO_FUNCTION(name, type) \
    constexpr auto to_##name(istl::ConvertibleToStringView auto&& str) noexcept {\
        return to<type>(str);\
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

    template <typename INT>
    constexpr auto digit_count() noexcept {
        uint_fast8_t t = 0;
        INT          a = stl::numeric_limits<int>::max();
        while (a /= 10)
            ++t;
        return t;
    }


    // todo: add allocator support here:
    template <Traits TraitsType, typename ValueType, typename... R>
    constexpr auto to_str_copy(ValueType value, R&&... args) noexcept {
        using char_type           = typename TraitsType::char_type;
        using str_t               = typename TraitsType::string_type;
        using size_type           = typename str_t::size_type;
        constexpr size_type _size = digit_count<ValueType>() + 1;
        if constexpr (stl::is_same_v<char_type, char>) {
            str_t str(_size, '\0');
            auto [p, _] = stl::to_chars(str.data(), str.data() + _size, value, stl::forward<R>(args)...);
            str.resize(p - str.data());
            return str;
        } else {
            char str[_size];
            auto [p, _]      = stl::to_chars(str, str + _size, value, stl::forward<R>(args)...);
            auto the_size = static_cast<size_type>(p - str);
            str_t     res(the_size, '\0');
            auto      it = res.begin();
            for (auto _c = str; *_c; ++_c) {
                *it++ = static_cast<char_type>(*_c);
            }
            return res;
        }
    }




} // namespace webpp

#endif // WEBPP_CASTS_H
