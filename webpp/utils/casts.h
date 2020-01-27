#ifndef WEBPP_CASTS_H
#define WEBPP_CASTS_H

#include <string_view>

namespace webpp {

    template <typename T, bool is_signed = true, bool throw_mistakes = false>
    constexpr inline T
    to(std::string_view const& str) noexcept(!throw_mistakes) {
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
                            throw std::invalid_argument(
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
                            throw std::invalid_argument(
                                "The specified string is not a number");
                    }
                    ret *= 10;
                    ret += static_cast<T>(c - '0');
                }
            }
        }
        return ret;
    }

    constexpr auto to_int(std::string_view const& str) noexcept {
        return to<int>(str);
    }
    constexpr auto to_int8(std::string_view const& str) noexcept {
        return to<int8_t>(str);
    }
    constexpr auto to_int16(std::string_view const& str) noexcept {
        return to<int16_t>(str);
    }
    constexpr auto to_int32(std::string_view const& str) noexcept {
        return to<int32_t>(str);
    }
    constexpr auto to_int64(std::string_view const& str) noexcept {
        return to<int64_t>(str);
    }
    constexpr auto to_uint(std::string_view const& str) noexcept {
        return to<unsigned int>(str);
    }
    constexpr auto to_uint8(std::string_view const& str) noexcept {
        return to<uint8_t>(str);
    }
    constexpr auto to_uint16(std::string_view const& str) noexcept {
        return to<uint16_t>(str);
    }
    constexpr auto to_uint32(std::string_view const& str) noexcept {
        return to<uint32_t>(str);
    }
    constexpr auto to_uint64(std::string_view const& str) noexcept {
        return to<uint64_t>(str);
    }

} // namespace webpp

#endif // WEBPP_CASTS_H
