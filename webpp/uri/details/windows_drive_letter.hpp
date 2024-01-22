// Created by moisrex on 1/13/24.

#ifndef WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
#define WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP

#include "constants.hpp"

namespace webpp::uri::details {

    template <typename Iter>
    [[nodiscard]] static constexpr bool has_windows_driver_letter(Iter pos) noexcept {
        // https://url.spec.whatwg.org/#windows-drive-letter
        return ASCII_ALPHA.contains(*pos) && (pos[1] == ':' || pos[1] == '|');
    }

    template <typename Iter>
    [[nodiscard]] static constexpr bool has_normalized_windows_driver_letter(Iter pos) noexcept {
        // https://url.spec.whatwg.org/#normalized-windows-drive-letter
        return ASCII_ALPHA.contains(*pos) && pos[1] == ':';
    }

    template <typename Iter, typename EIter = Iter>
    [[nodiscard]] static constexpr bool starts_with_windows_driver_letter(Iter pos, EIter end) noexcept {
        // https://url.spec.whatwg.org/#start-with-a-windows-drive-letter
        webpp_assume(end >= pos);
        switch (end - pos) {
            case 0:
            case 1: return false;
            case 2: return has_windows_driver_letter(pos);
            default:
                if (!has_windows_driver_letter(pos)) {
                    return false;
                }
                switch (pos[2]) {
                    case '/':
                    case '\\':
                    case '?':
                    case '#': return true;
                    default: return false;
                }
        }
    }

    template <typename Iter, typename EIter = Iter>
    [[nodiscard]] static constexpr bool starts_with_windows_driver_letter_slashes(
      Iter  pos,
      EIter end) noexcept {
        // https://url.spec.whatwg.org/#start-with-a-windows-drive-letter
        webpp_assume(end >= pos);
        switch (end - pos) {
            case 0:
            case 1: return false;
            case 2: return has_windows_driver_letter(pos);
            default:
                if (*pos == '/' || *pos == '\\') {
                    ++pos;
                }
                if (!has_windows_driver_letter(pos)) {
                    return false;
                }
                switch (pos[2]) {
                    case '/':
                    case '\\':
                    case '?':
                    case '#': return true;
                    default: return false;
                }
        }
    }


} // namespace webpp::uri::details

#endif // WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
