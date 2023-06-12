// Created by moisrex on 6/12/23.

#ifndef WEBPP_STD_BIT_HPP
#define WEBPP_STD_BIT_HPP

#include "./std.hpp"

#include <bit>
#ifndef __cpp_lib_byteswap
#    include "./concepts.hpp"
#endif

namespace webpp::stl {

    // fallback implementation of byteswap that works on C++20
#ifndef __cpp_lib_byteswap
    template <std::integral T>
    constexpr static T byteswap(T value) noexcept {
        static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
        auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
        std::ranges::reverse(value_representation);
        return std::bit_cast<T>(value_representation);
    }
#endif
} // namespace webpp::stl

#endif // WEBPP_STD_BIT_HPP
