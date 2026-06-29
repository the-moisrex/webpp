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
        using char_type = stl::iter_value_t<Iter>;

        switch (end - pos) {
            case 0:
            case 1: return false;
            case 2: return has_windows_driver_letter(pos);
            default: {
                stl::array<char_type, 3> letters{};
                using size_type = typename stl::array<char_type, 3>::size_type;

                // ignoring first (back-)slash character
                for (size_type index = 0; index != 3 && pos != end; ++pos) {
                    switch (*pos) {
                        case '\\':
                        case '/':
                            if (index == 0) {
                                continue;
                            }
                            [[fallthrough]];
                        [[likely]] default:
                            assert(index < letters.size());
                            letters.at(index) = *pos;
                            ++index;
                            continue;
                    }
                    break;
                }
                if (!has_windows_driver_letter(letters.begin())) {
                    return false;
                }

                switch (letters[2]) {
                    case '/':
                    case '\\':
                    case '?':
                    case '#': return true;
                    default: break;
                }
            }
        }
        return false;
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
