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
    static constexpr T byteswap(T value) noexcept {
#    if __cpp_lib_type_trait_variable_templates
        static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
#    endif
        // #    ifdef __cpp_lib_ranges
        //         auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
        //         std::ranges::reverse(value_representation);
        //         return std::bit_cast<T>(value_representation);
        // #    else

        // NOLINTBEGIN(*-magic-numbers)
        if constexpr (sizeof(T) == sizeof(uint64_t)) {
            auto const val = static_cast<stl::uint64_t>(value);
            return (val << 56U & 0xFF00'0000'0000'0000ULL) | (val << 40U & 0x00FF'0000'0000'0000ULL) |
                   (val << 24U & 0x0000'FF00'0000'0000ULL) | (val << 8U & 0x0000'00FF'0000'0000ULL) |
                   (val >> 8U & 0x0000'0000'FF00'0000ULL) | (val >> 24U & 0x0000'0000'00FF'0000ULL) |
                   (val >> 40U & 0x0000'0000'0000'FF00ULL) | (val >> 56U & 0x0000'0000'0000'00FFULL);
        } else if constexpr (sizeof(T) == sizeof(uint32_t)) {
            auto const val = static_cast<stl::uint32_t>(value);
            return (val << 24U & 0xFF00'0000UL) | (val << 8U & 0x00FF'0000UL) | (val >> 8U & 0x0000'FF00UL) |
                   (val >> 24U & 0x0000'00FFUL);
        } else {
            auto const val = static_cast<stl::uint16_t>(value);
            return (val << 8 & 0xFF00U) | (val >> 8U & 0x00FFU);
        }
        return value;
        // NOLINTEND(*-magic-numbers)
        // #    endif
    }
#endif
} // namespace webpp::stl

#endif // WEBPP_STD_BIT_HPP
