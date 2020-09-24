// Created by moisrex on 9/18/20.

#ifndef WEBPP_HTTP_VERSION_HPP
#define WEBPP_HTTP_VERSION_HPP

#include "../../std/string_view.hpp"
#include "../../utils/casts.hpp"

#include <cstdint>

namespace webpp {

    // Wrapper for an HTTP (major,minor) version pair.
    class http_version {
      private:
        // parse version from string
        uint32_t parse_string(auto&& str) noexcept {
            auto dot   = stl::find(str, '.');
            auto major = to_uint16(str.substr(0, dot));
            auto minor = to_uint16(str.substr(dot + 1, str.size()));
            return major << 16u | minor;
        }

      public:
        // Default constructor (major=0, minor=0).
        constexpr http_version() noexcept = default;

        constexpr http_version(istl::ConvertibleToStringView auto&& str) noexcept
          : value(parse_string(istl::to_string_view(str))) {}

        // Build from unsigned major/minor pair.
        constexpr http_version(uint16_t major, uint16_t minor) noexcept : value(major << 16u | minor) {}

        // Major version number.
        [[nodiscard]] constexpr uint16_t major_value() const noexcept {
            return value >> 16u;
        }

        // Minor version number.
        [[nodiscard]] constexpr uint16_t minor_value() const noexcept {
            return value & 0xffffu;
        }

        // Overloaded operators:

        [[nodiscard]] constexpr bool operator==(const http_version& v) const noexcept {
            return value == v.value;
        }
        [[nodiscard]] constexpr bool operator!=(const http_version& v) const noexcept {
            return value != v.value;
        }
        [[nodiscard]] constexpr bool operator>(const http_version& v) const noexcept {
            return value > v.value;
        }
        [[nodiscard]] constexpr bool operator>=(const http_version& v) const noexcept {
            return value >= v.value;
        }
        [[nodiscard]] constexpr bool operator<(const http_version& v) const noexcept {
            return value < v.value;
        }
        [[nodiscard]] constexpr bool operator<=(const http_version& v) const noexcept {
            return value <= v.value;
        }
        // todo: use <=> operator

      private:
        uint32_t value = 0; // Packed as <major>:<minor>
    };

} // namespace webpp

#endif // WEBPP_HTTP_VERSION_HPP
