#ifndef WEBPP_CASTS_H
#define WEBPP_CASTS_H

#include <string_view>

namespace webpp {

    template <typename T, bool is_signed = true>
    constexpr inline T to(std::string_view const& str) noexcept {
        T ret = 0;
        if constexpr (is_signed) {
            bool minus = false;
            for (auto const& i : str) {
                if (i == '-') {
                    minus = true;
                    continue;
                }
                ret *= 10;
                ret += static_cast<T>(i - '0');
            }
        } else {
            for (auto const& i : str) {
                ret *= 10;
                ret += static_cast<T>(i - '0');
            }
        }
        return ret;
    }

    constexpr auto to_int(std::string_view const& str) noexcept {
        return to<int, true>(str);
    }

    constexpr auto to_int8(std::string_view const& str) noexcept {
        return to<int8_t, true>(str);
    }

    constexpr auto to_int16(std::string_view const& str) noexcept {
        return to<int16_t, true>(str);
    }
    constexpr auto to_int32(std::string_view const& str) noexcept {
        return to<int32_t, true>(str);
    }
    constexpr auto to_int64(std::string_view const& str) noexcept {
        return to<int64_t, true>(str);
    }
    constexpr auto to_uint(std::string_view const& str) noexcept {
        return to<unsigned int, true>(str);
    }
    constexpr auto to_uint8(std::string_view const& str) noexcept {
        return to<uint8_t, false>(str);
    }
    constexpr auto to_uint16(std::string_view const& str) noexcept {
        return to<uint16_t, false>(str);
    }
    constexpr auto to_uint32(std::string_view const& str) noexcept {
        return to<uint32_t, false>(str);
    }
    constexpr auto to_uint64(std::string_view const& str) noexcept {
        return to<uint64_t, false>(str);
    }

} // namespace webpp

#endif // WEBPP_CASTS_H
