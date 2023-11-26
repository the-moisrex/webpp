// Created by moisrex on 10/7/23.
// source of this file: ada project / scheme-inl.h

#ifndef WEBPP_BENCH_ADA_SCHEME_HPP
#define WEBPP_BENCH_ADA_SCHEME_HPP

#include <cstdint>
#include <string_view>

namespace ada::scheme {

    /**
     * @namespace ada::scheme::details
     * @brief Includes the definitions for scheme specific entities
     */
    namespace details {
        // for use with is_special and get_special_port
        // Spaces, if present, are removed from URL.
        constexpr std::string_view is_special_list[] =
          {"http", " ", "https", "ws", "ftp", "wss", "file", " "};
        // for use with get_special_port
        constexpr uint16_t special_ports[] = {80, 0, 443, 80, 21, 443, 0, 0};
    } // namespace details

    constexpr bool is_special(std::string_view scheme) {
        if (scheme.empty()) {
            return false;
        }
        int                    hash_value = (2 * scheme.size() + (unsigned) (scheme[0])) & 7;
        const std::string_view target     = details::is_special_list[hash_value];
        return (target[0] == scheme[0]) && (target.substr(1) == scheme.substr(1));
    }

    constexpr uint16_t get_special_port(std::string_view scheme) noexcept {
        if (scheme.empty()) {
            return 0;
        }
        int                    hash_value = (2 * scheme.size() + (unsigned) (scheme[0])) & 7;
        const std::string_view target     = details::is_special_list[hash_value];
        if ((target[0] == scheme[0]) && (target.substr(1) == scheme.substr(1))) {
            return details::special_ports[hash_value];
        } else {
            return 0;
        }
    }

} // namespace ada::scheme

#endif // WEBPP_BENCH_ADA_SCHEME_HPP
