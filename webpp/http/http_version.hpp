// Created by moisrex on 10/29/20.

#ifndef WEBPP_HTTP_VERSION_HPP
#define WEBPP_HTTP_VERSION_HPP

#include "../convert/casts.hpp"
#include "../std/string_view.hpp"
#include "../strings/size.hpp"

#include <array>
#include <climits>
#include <cstdint>

namespace webpp::http {

    // Wrapper for an HTTP (major,minor) version pair.
    struct [[nodiscard]] version {
      private:
        static constexpr auto          uint_16_bits     = sizeof(stl::uint16_t) * CHAR_BIT;
        static constexpr stl::uint32_t minor_value_mask = 0xFFFFU;

        // parse version from string
        [[nodiscard]] static constexpr stl::uint32_t parse_string(stl::string_view const str) noexcept {
            auto dot = str.find('.');
            if (dot == stl::string_view::npos) [[unlikely]] {
                return 0;
            }
            auto major = to_uint16(str.substr(0, dot));
            auto minor = to_uint16(str.substr(dot + 1));
            if (!major.has_value() || !minor.has_value()) {
                return 0;
            }
            return static_cast<stl::uint32_t>(
              static_cast<stl::uint32_t>((static_cast<stl::uint32_t>(major) << uint_16_bits)) | minor);
        }


      public:
        // Default constructor (major=0, minor=0).
        constexpr version() noexcept = default;

        explicit constexpr version(stl::string_view const str) noexcept : value(parse_string(str)) {}

        constexpr version(version const&) noexcept            = default;
        constexpr version(version&&) noexcept                 = default;
        constexpr version& operator=(version const&) noexcept = default;
        constexpr version& operator=(version&&) noexcept      = default;

        constexpr ~version() = default;

        // Build from unsigned major/minor pair.
        constexpr version(stl::uint16_t const major, stl::uint16_t const minor) noexcept
          : value(static_cast<stl::uint32_t>(
              static_cast<stl::uint32_t>((static_cast<stl::uint32_t>(major) << uint_16_bits)) | minor)) {}

        // Major version number.
        [[nodiscard]] constexpr stl::uint16_t major_value() const noexcept {
            return static_cast<stl::uint16_t>(value >> uint_16_bits);
        }

        // Minor version number.
        [[nodiscard]] constexpr stl::uint16_t minor_value() const noexcept {
            return static_cast<stl::uint16_t>(value & minor_value_mask);
        }

        // Get an unknown HTTP version
        [[nodiscard]] static constexpr version unknown() noexcept {
            return {};
        }

        [[nodiscard]] constexpr bool is_unknown() const noexcept {
            return value == 0;
        }

        /**
         * The string you get usually from SERVER_PROTOCOL env can be parsed with this method.
         * Examples of input: “HTTP/1.0”, “HTTP/1.1”, or “HTTP/2.0”
         */
        [[nodiscard]] static constexpr version from_server_protocol(stl::string_view str) noexcept {
            constexpr stl::string_view http_string{"HTTP", 4};
            constexpr stl::string_view https_string{"S/", 2};
            if (!str.starts_with(http_string)) {
                return unknown();
            }
            str.remove_prefix(http_string.size());
            if (str.starts_with('/')) {
                return version{str.substr(1)};
            }
            if (str.starts_with(https_string)) {
                return version{str.substr(https_string.size())};
            }
            return unknown();
        }

        template <typename CharT>
        [[nodiscard]] static constexpr version from_string(stl::string_view const str) noexcept {
            return version{str};
        }

        // Overloaded operators:

        [[nodiscard]] constexpr bool operator==(version const& other) const noexcept {
            return value == other.value;
        }

        [[nodiscard]] constexpr bool operator!=(version const& other) const noexcept {
            return value != other.value;
        }

        [[nodiscard]] constexpr bool operator>(version const& other) const noexcept {
            return value > other.value;
        }

        [[nodiscard]] constexpr bool operator>=(version const& other) const noexcept {
            return value >= other.value;
        }

        [[nodiscard]] constexpr bool operator<(version const& other) const noexcept {
            return value < other.value;
        }

        [[nodiscard]] constexpr bool operator<=(version const& other) const noexcept {
            return value <= other.value;
        }

        // todo: use <=> operator

      private:
        // Packed like this:
        // [ major | minor ]
        //  16 bits  16 bits
        stl::uint32_t value = 0U;
    };

    /**
     * A list of http::version
     */
    template <stl::size_t N>
    struct version_list : stl::array<version, N> {
        using array_type = stl::array<version, N>;

        template <typename... T>
        explicit constexpr version_list(T&&... versions) noexcept : array_type{stl::forward<T>(versions)...} {}

        [[nodiscard]] constexpr bool include_version(version ver) const noexcept {
            for (auto const& cur : *this) {
                if (ver == cur) {
                    return true;
                }
            }
            return false;
        }
    };

    static constexpr version http_0_9{"0.9"};
    static constexpr version http_1_0{"1.0"};
    static constexpr version http_1_1{"1.1"};
    static constexpr version http_2_0{"2.0"};
    static constexpr version http_3_0{"3.0"};

    static constexpr version_list<5> all_http_versions{http_0_9, http_1_0, http_1_1, http_2_0, http_3_0};

    // todo: create a supported version list (exclude http/0.9 since no one is using it anymore)

} // namespace webpp::http


#endif // WEBPP_HTTP_VERSION_HPP
