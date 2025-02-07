// Created by moisrex on 6/12/23.

#ifndef WEBPP_SOCKET_BYTES_HPP
#define WEBPP_SOCKET_BYTES_HPP

#include "../std/bit.hpp"

namespace webpp {

    /**
     * Host byte-order TO Network byte-order
     * Network byte-order == std::endian::big
     */
    template <typename T>
    [[nodiscard]] static constexpr T hton(T value) noexcept {
        if constexpr (stl::endian::native == stl::endian::big) {
            return value;
        } else {
            return stl::byteswap(value);
        }
    }

    /**
     * Network byte-order TO Host byte-order
     * Network byte-order == std::endian::big
     */
    template <typename T>
    [[nodiscard]] static constexpr T ntoh(T value) noexcept {
        if constexpr (stl::endian::native == stl::endian::big) {
            return value;
        } else {
            return stl::byteswap(value);
        }
    }
} // namespace webpp

#endif // WEBPP_SOCKET_BYTES_HPP
