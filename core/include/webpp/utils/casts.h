#ifndef WEBPP_CASTS_H
#define WEBPP_CASTS_H

#include "../traits/std_traits.h"

#include <charconv>
#include <stdexcept>
#include <string_view>

namespace webpp {

    template <Traits TraitsType, typename T, bool is_signed = true,
              bool throw_mistakes = false>
    constexpr T to(typename TraitsType::string_view_type const& str) noexcept(
      !throw_mistakes) {
        T ret = 0;
        if (str.size() > 0) {
            // todo: minus is not used!!
            if constexpr (is_signed) {
                auto c = str.cbegin();
                if (*c == '-' || *c == '+')
                    c++; // first character can be - or +
                for (; c != str.cend(); c++) {
                    if constexpr (throw_mistakes) {
                        if (*c <= '0' || *c >= '9')
                            throw stl::invalid_argument(
                              "The specified string is not a number");
                    }
                    ret *= 10;
                    ret += static_cast<T>(*c - '0');
                }
                ret *= str.front() == '-' ? -1 : 1;
            } else {
                for (auto const& c : str) {
                    if constexpr (throw_mistakes) {
                        if (c <= '0' || c >= '9')
                            throw stl::invalid_argument(
                              "The specified string is not a number");
                    }
                    ret *= 10;
                    ret += static_cast<T>(c - '0');
                }
            }
        }
        return ret;
    }

    template <Traits TraitsType>
    constexpr auto
    to_int(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, int>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_int8(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, int8_t>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_int16(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, int16_t>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_int32(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, int32_t>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_int64(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, int64_t>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_uint(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, unsigned int>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_uint8(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, uint8_t>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_uint16(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, uint16_t>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_uint32(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, uint32_t>(str);
    }

    template <Traits TraitsType>
    constexpr auto
    to_uint64(typename TraitsType::string_view_type const& str) noexcept {
        return to<TraitsType, uint64_t>(str);
    }


    //////////////////////////////////////////////////////////////////////

    template <typename INT>
    constexpr auto digit_count() noexcept {
        uint_fast8_t t = 0;
        INT          a = stl::numeric_limits<int>::max();
        while (a /= 10)
            ++t;
        return t;
    }


    template <Traits TraitsType, typename ValueType, typename... R>
    constexpr auto to_str(ValueType value, R&&... args) noexcept {
        using char_type           = typename TraitsType::char_type;
        using str_t               = typename TraitsType::string_type;
        using size_type           = typename str_t::size_type;
        constexpr size_type _size = digit_count<ValueType>() + 1;
        if constexpr (stl::is_same_v<char_type, char>) {
            str_t str(_size, '\0');
            auto [p, _] = stl::to_chars(str.data(), str.data() + _size, value,
                                        stl::forward<R>(args)...);
            str.resize(p - str.data());
            return str;
        } else {
            char str[_size];
            auto [p, _] =
              stl::to_chars(str, str + _size, value, stl::forward<R>(args)...);
            size_type __size = static_cast<size_type>(p - str);
            str_t     res(__size, '\0');
            auto      it = res.begin();
            for (auto _c = str; *_c; ++_c) {
                *it++ = static_cast<char_type>(*_c);
            }
            return res;
        }
    }



} // namespace webpp

#endif // WEBPP_CASTS_H
