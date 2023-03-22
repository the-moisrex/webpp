// Created by moisrex on 10/29/20.

#ifndef WEBPP_HTTP_VERSION_HPP
#define WEBPP_HTTP_VERSION_HPP

#include "../convert/casts.hpp"
#include "../std/string_view.hpp"

#include <cstdint>

namespace webpp::http {

    // Wrapper for an HTTP (major,minor) version pair.
    class version {
      private:
        static constexpr auto uint_16_bits     = sizeof(stl::uint16_t) * 8u;
        static constexpr auto minor_value_mask = 0xffffu;

        // parse version from string
        constexpr stl::uint32_t parse_string(auto&& str) noexcept {
            auto dot   = str.find('.');
            auto major = to_uint16(str.substr(0, dot));
            auto minor = to_uint16(str.substr(dot + 1, str.size()));
            return static_cast<stl::uint32_t>(major << uint_16_bits | minor);
        }


      public:
        // Default constructor (major=0, minor=0).
        constexpr version() noexcept = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename T>
            requires(!stl::same_as<stl::remove_cvref_t<T>, version> && istl::StringViewifiable<T>)
        constexpr version(T&& str) noexcept
          : value(parse_string(istl::string_viewify(stl::forward<decltype(str)>(str)))) {}
        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr version(version const&) noexcept            = default;
        constexpr version(version&&) noexcept                 = default;
        constexpr version& operator=(version const&) noexcept = default;
        constexpr version& operator=(version&&) noexcept      = default;

        constexpr ~version() = default;

        // Build from unsigned major/minor pair.
        constexpr version(stl::uint16_t major, stl::uint16_t minor) noexcept
          : value(static_cast<stl::uint32_t>(major << uint_16_bits | minor)) {}

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
        [[nodiscard]] static constexpr version from_server_protocol(istl::StringView auto str) noexcept {
            constexpr auto http_string = "HTTP"; // todo: make static when C++23 support is good
            if (!str.starts_with(http_string)) {
                return unknown();
            }
            str.remove_prefix(ascii::size(http_string));
            if (str.starts_with("/")) {
                return {str.substr(1)};
            } else if (str.starts_with("S/")) {
                return {str.substr(2)};
            }
            return unknown();
        }


        [[nodiscard]] static constexpr version from_string(istl::StringView auto str) noexcept {
            return {str};
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
        uint32_t value = 0ul; // Packed as <major>:<minor>
    };


    /**
     * A list of http::version
     */
    template <stl::size_t N>
    struct version_list : public stl::array<version, N> {
        using array_type = stl::array<version, N>;

        template <typename... T>
        constexpr version_list(T&&... versions) noexcept : array_type{stl::forward<T>(versions)...} {}

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
    static constexpr version http_3_0{"3.0"};

    static constexpr version_list<5> all_http_versions{http_0_9, http_1_0, http_1_1, http_2_0, http_3_0};

    // todo: create a supported version list (exclude http/0.9 since no one is using it anymore)

} // namespace webpp::http


#endif // WEBPP_HTTP_VERSION_HPP
