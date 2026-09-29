// Created by moisrex on 1/13/24.

#ifndef WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
#define WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP

#include "./constants.hpp"
#include "./special_schemes.hpp"
#include "./uri_context.hpp"

#include <iterator>

namespace webpp::uri::details {

    template <typename Iter>
    [[nodiscard]] static constexpr bool has_windows_driver_letter(Iter pos) noexcept {
        // https://url.spec.whatwg.org/#windows-drive-letter
        return ASCII_ALPHA.contains(*pos) && (pos[1] == ':' || pos[1] == '|');
    }

    template <typename Iter>
    [[nodiscard]] static constexpr bool is_windows_driver_letter(Iter pos, Iter const end) noexcept {
        return end - pos == 2 && has_windows_driver_letter(pos);
    }

    template <typename Iter>
    [[nodiscard]] static constexpr bool has_normalized_windows_driver_letter(Iter pos) noexcept {
        // https://url.spec.whatwg.org/#normalized-windows-drive-letter
        return ASCII_ALPHA.contains(*pos) && pos[1] == ':';
    }

    template <typename Iter>
    [[nodiscard]] static constexpr bool starts_with_windows_driver_letter(Iter pos, Iter const end) noexcept {
        // https://url.spec.whatwg.org/#start-with-a-windows-drive-letter
        auto const length = end - pos;
        if (length < 2 || !has_windows_driver_letter(pos)) {
            return false;
        }
        if (length == 2) {
            return true;
        }
        switch (pos[2]) {
            case '/':
            case '\\':
            case '?':
            case '#': return true;
            default: return false;
        }
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
