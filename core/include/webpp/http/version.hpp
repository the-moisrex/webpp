// Created by moisrex on 10/29/20.

#ifndef WEBPP_VERSION_HPP
#define WEBPP_VERSION_HPP

#include "../convert/casts.hpp"
#include "../std/string_view.hpp"

#include <cstdint>

namespace webpp::http {

    // Wrapper for an HTTP (major,minor) version pair.
    class version {
      private:
        // parse version from string
        constexpr uint32_t parse_string(auto&& str) noexcept {
            auto dot   = str.find('.');
            auto major = to_uint16(str.substr(0, dot));
            auto minor = to_uint16(str.substr(dot + 1, str.size()));
            return major << 16u | minor;
        }

      public:
        // Default constructor (major=0, minor=0).
        constexpr version() noexcept = default;

        template <typename T>
        requires (!stl::same_as<stl::remove_cvref_t<T>, version> && istl::StringViewifiable<T>)
        constexpr version(T&& str) noexcept
          : value(parse_string(istl::string_viewify(stl::forward<decltype(str)>(str)))) {}

        constexpr version(version const&) noexcept = default;
        constexpr version(version &&) noexcept = default;
        version& operator=(version const&) noexcept = default;
        version& operator=(version&&) noexcept = default;

        // Build from unsigned major/minor pair.
        constexpr version(uint16_t major, uint16_t minor) noexcept : value(major << 16u | minor) {}

        // Major version number.
        [[nodiscard]] constexpr uint16_t major_value() const noexcept {
            return value >> 16u;
        }

        // Minor version number.
        [[nodiscard]] constexpr uint16_t minor_value() const noexcept {
            return value & 0xffffu;
        }

        // Overloaded operators:

        [[nodiscard]] constexpr bool operator==(const version& v) const noexcept {
            return value == v.value;
        }
        [[nodiscard]] constexpr bool operator!=(const version& v) const noexcept {
            return value != v.value;
        }
        [[nodiscard]] constexpr bool operator>(const version& v) const noexcept {
            return value > v.value;
        }
        [[nodiscard]] constexpr bool operator>=(const version& v) const noexcept {
            return value >= v.value;
        }
        [[nodiscard]] constexpr bool operator<(const version& v) const noexcept {
            return value < v.value;
        }
        [[nodiscard]] constexpr bool operator<=(const version& v) const noexcept {
            return value <= v.value;
        }
        // todo: use <=> operator

      private:
        uint32_t value = 0; // Packed as <major>:<minor>
    };

    template <stl::size_t N>
    struct version_list : public stl::array<version, N> {
        using array_type = stl::array<version, N>;

        template <typename ...T>
        constexpr version_list(T&& ...versions) noexcept : array_type{stl::forward<T>(versions)...} {}

        [[nodiscard]] constexpr bool include_version(version ver) noexcept {
            for (auto const& v : *this) {
                if (ver == v)
                    return true;
            }
            return false;
        }
    };


    static constexpr version http_0_9{"0.9"};
    static constexpr version http_1_0{"1.0"};
    static constexpr version http_1_1{"1.1"};
    static constexpr version http_2_0{"2.0"};
    // todo: add version 3

    static constexpr version_list<4> all_http_versions{
      http_0_9, http_1_0, http_1_1, http_2_0
    };

    // todo: create a supported version list (exclude http/0.9 since no one is using it anymore)

}


#endif // WEBPP_VERSION_HPP
