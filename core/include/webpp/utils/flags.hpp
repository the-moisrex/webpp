// Created by moisrex on 11/24/20.

#ifndef WEBPP_FLAGS_HPP
#define WEBPP_FLAGS_HPP

#include "../std/type_traits.hpp"
#include <cstdint>
#include "../libs/magic_enum/include/magic_enum.hpp"

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
        static constexpr stl::size_t base_size = magic_enum::enum_count<type>();
        using base_type = stl::conditional_t<(base_size <= sizeof(stl::uint8_t) * 8), stl::uint8_t,
                          stl::conditional_t<(base_size <= sizeof(stl::uint16_t) * 8), stl::uint16_t,
              stl::conditional_t<(base_size <= sizeof(stl::uint32_t) * 8), stl::uint32_t, stl::uint64_t>>>;
        // There is a clang bug (or magic_enum bug)
        // GitHub issue: https://github.com/Neargye/magic_enum/issues/65
        static constexpr bool are_values_sequential = []() constexpr noexcept -> bool {
            const auto vals = magic_enum::enum_values<type>();
            for (stl::size_t i = 0; i < vals.size(); ++i)
                if (magic_enum::enum_integer(vals[i]) != i)
                    return false;
            return true;
        }();

#ifdef __cpp_using_enum
        using enum type;
#endif
        base_type value = none;

        constexpr manager() = default;

        template <typename ...T>
        requires (stl::same_as<stl::remove_cvref_t<T>, base_type>)
        constexpr manager(T ...val) noexcept : value{(none | ... | val)} {}

        template <typename ...T>
        requires (stl::same_as<stl::remove_cvref_t<T>, type>)
        constexpr manager(T ...val) noexcept : value{(none | ... | value_of(val))} {}

        operator base_type() const noexcept {
            return value;
        }

        [[nodiscard]] static constexpr base_type value_of(type v) noexcept {
            if constexpr (are_values_sequential) {
                return item(magic_enum::enum_integer(v));
            } else {
                return item(magic_enum::enum_index(v).value());
            }
        }

        [[nodiscard]] constexpr bool operator==(base_type v) const noexcept { return value == v; }
        [[nodiscard]] constexpr bool operator==(type v) const noexcept { return operator==(value_of(v)); }

        [[nodiscard]] constexpr bool operator!=(base_type v) const noexcept { return value != v; }
        [[nodiscard]] constexpr bool operator!=(type v) const noexcept { return operator!=(value_of(v)); }

        [[nodiscard]] constexpr bool operator<(base_type v) const noexcept { return value < v; }
        [[nodiscard]] constexpr bool operator<(type v) const noexcept { return operator<(value_of(v)); }

        [[nodiscard]] constexpr bool operator>(base_type v) const noexcept { return value > v; }
        [[nodiscard]] constexpr bool operator>(type v) const noexcept { return operator>(value_of(v)); }

        [[nodiscard]] constexpr bool operator>=(base_type v) const noexcept { return value >= v; }
        [[nodiscard]] constexpr bool operator>=(type v) const noexcept { return operator>=(value_of(v)); }

        [[nodiscard]] constexpr bool operator<=(base_type v) const noexcept { return value <= v; }
        [[nodiscard]] constexpr bool operator<=(type v) const noexcept { return operator<=(value_of(v)); }

        constexpr manager& operator^=(type b) noexcept { value ^= value_of(b); return *this; }
        constexpr manager& operator^=(base_type b) noexcept { value ^= b; return *this; }

        constexpr manager& operator&=(type b) noexcept { value &= value_of(b); return *this; }
        constexpr manager& operator&=(base_type b) noexcept { value &= b; return *this; }

        constexpr manager& operator|=(type b) noexcept { value |= value_of(b); return *this; }
        constexpr manager& operator|=(base_type b) noexcept { value |= b; return *this; }

        constexpr manager& operator=(type b) noexcept { value = value_of(b); return *this; }
        constexpr manager& operator=(base_type b) noexcept { value = b; return *this; }

        constexpr void on(type v) noexcept {
            value |= value_of(v);
        }

        constexpr void off(type v) noexcept {
            value &= ~value_of(v);
        }

        [[nodiscard]] constexpr bool is_on(type v) const noexcept {
            return is_on(value_of(v));
        }

        [[nodiscard]] constexpr bool is_on(base_type v) const noexcept {
            return (value & v) == v;
        }

        [[nodiscard]] constexpr bool is_off(type v) const noexcept {
            return is_off(value_of(v));
        }

        [[nodiscard]] constexpr bool is_off(base_type v) const noexcept {
            return (value & v) != v;
        }

        constexpr void reset(base_type v = none) noexcept {
            value = v;
        }

        constexpr void reset(type v) noexcept {
            value = value_of(v);
        }

    };

}

#endif // WEBPP_FLAGS_HPP
