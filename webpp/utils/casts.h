#ifndef CASTS_H
#define CASTS_H

#include "../std/string_view.h"
#include "../validators/validators.h"
#include <cmath>

namespace webpp {

    /**
     * @brief to int; this function will not throw any exceptions
     * @param str
     * @return
     */
    constexpr inline unsigned int
    to_uint(std::string_view const& str) noexcept {
        unsigned int s = 0;
        auto len = str.size();
        for (std::size_t i = 0; i < len; i++)
            s += static_cast<unsigned int>(str[len - i - 1] - '0') *
                 static_cast<unsigned int>(std::pow(10, i));
        return s;
    }

    constexpr inline uint8_t to_uint8(std::string_view const& str) noexcept {
        uint8_t s = 0;
        auto len = str.size();
        for (std::size_t i = 0; i < len; i++)
            s += static_cast<uint8_t>(str[len - i - 1] - '0') *
                 static_cast<uint8_t>(std::pow(10, i));
        return s;
    }

} // namespace webpp

#endif // CASTS_H
