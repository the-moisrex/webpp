// Created by moisrex on 11/24/20.

#ifndef WEBPP_FLAGS_HPP
#define WEBPP_FLAGS_HPP

#include "../std/type_traits.hpp"
#include <cstdint>

namespace webpp::flags {

    static constexpr stl::uint8_t none = 0u;

    [[nodiscard]]
#ifdef __cpp_consteval
    consteval
#else
    constexpr
#endif
    auto item(stl::size_t index) noexcept {
        return 0x1u << index;
    }

    /**
     * Flag bit manager
     * This class is designed to manage the flag bits efficiently and more easily
     */
    template <typename Values>
    struct manager {
        using type = Values;
        static constexpr stl::size_t base_size = sizeof(type);
        using base_type = stl::conditional_t<(base_size <= sizeof(stl::uint8_t)), stl::uint8_t,
                          stl::conditional_t<(base_size <= sizeof(stl::uint16_t)), stl::uint16_t,
              stl::conditional_t<(base_size <= sizeof(stl::uint32_t)), stl::uint32_t, stl::uint64_t>>>;
#ifdef __cpp_using_enum
        using enum type;
#endif
        base_type value;

        constexpr manager() = default;
        constexpr manager(base_type val) noexcept : value{val} {}
        constexpr manager(type val) noexcept : value{static_cast<base_type>(val)} {}

        operator base_type() const noexcept {
            return value;
        }

        [[nodiscard]] constexpr bool operator==(base_type v) const noexcept { return value == v; }
        [[nodiscard]] constexpr bool operator==(type v) const noexcept { return operator==(static_cast<base_type>(v)); }

        [[nodiscard]] constexpr bool operator!=(base_type v) const noexcept { return value != v; }
        [[nodiscard]] constexpr bool operator!=(type v) const noexcept { return operator!=(static_cast<base_type>(v)); }

        [[nodiscard]] constexpr bool operator<(base_type v) const noexcept { return value < v; }
        [[nodiscard]] constexpr bool operator<(type v) const noexcept { return operator<(static_cast<base_type>(v)); }

        [[nodiscard]] constexpr bool operator>(base_type v) const noexcept { return value > v; }
        [[nodiscard]] constexpr bool operator>(type v) const noexcept { return operator>(static_cast<base_type>(v)); }

        [[nodiscard]] constexpr bool operator>=(base_type v) const noexcept { return value >= v; }
        [[nodiscard]] constexpr bool operator>=(type v) const noexcept { return operator>=(static_cast<base_type>(v)); }

        [[nodiscard]] constexpr bool operator<=(base_type v) const noexcept { return value <= v; }
        [[nodiscard]] constexpr bool operator<=(type v) const noexcept { return operator<=(static_cast<base_type>(v)); }

        constexpr manager& operator^=(type b) noexcept { value ^= static_cast<base_type>(b); return *this; }
        constexpr manager& operator^=(base_type b) noexcept { value ^= b; return *this; }

        constexpr manager& operator&=(type b) noexcept { value &= static_cast<base_type>(b); return *this; }
        constexpr manager& operator&=(base_type b) noexcept { value &= b; return *this; }

        constexpr manager& operator|=(type b) noexcept { value |= static_cast<base_type>(b); return *this; }
        constexpr manager& operator|=(base_type b) noexcept { value |= b; return *this; }

        constexpr manager& operator=(type b) noexcept { value = static_cast<base_type>(b); return *this; }
        constexpr manager& operator=(base_type b) noexcept { value = b; return *this; }

        constexpr void on(type v) noexcept {
            value |= static_cast<base_type>(v);
        }

        constexpr void off(type v) noexcept {
            value &= ~static_cast<base_type>(v);
        }

        [[nodiscard]] constexpr bool is_on(type v) const noexcept {
            return is_on(static_cast<base_type>(v));
        }

        [[nodiscard]] constexpr bool is_on(base_type v) const noexcept {
            return (value & v) == v;
        }

        [[nodiscard]] constexpr bool is_off(type v) const noexcept {
            return is_off(static_cast<base_type>(v));
        }

        [[nodiscard]] constexpr bool is_off(base_type v) const noexcept {
            return (value & v) != v;
        }

        constexpr void reset() noexcept {
            value = none;
        }

    };

}

#endif // WEBPP_FLAGS_HPP
