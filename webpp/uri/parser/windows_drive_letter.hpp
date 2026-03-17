// Created by moisrex on 1/13/24.

#ifndef WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
#define WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP

#include "./constants.hpp"
#include "./special_schemes.hpp"

#include <iterator>

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

    template <uri_options Options, URIContext CtxT>
        requires(!Options.handle_windows_drive_letters)
    static constexpr void handle_windows_driver_letter(CtxT&, auto&) noexcept(CtxT::is_nothrow) {
        // do nothing
    }

    template <uri_options Options, URIContext CtxT>
        requires(Options.handle_windows_drive_letters)
    static constexpr void handle_windows_driver_letter(CtxT& ctx, auto& buffer) noexcept(CtxT::is_nothrow) {
        using enum uri_status;
        using char_type = typename CtxT::char_type;

        if (!is_file_scheme(ctx.status)) {
            return;
        }

        // https://url.spec.whatwg.org/#start-with-a-windows-drive-letter
        stl::array<char_type, 3> letters{};
        using size_type = typename stl::array<char_type, 3>::size_type;

        auto pos = ctx.pos;
        switch (ctx.end - pos) {
            case 0:
            case 1: return;
            case 2:
                if (!has_windows_driver_letter(pos)) {
                    return;
                }
                letters[0]  = *pos;
                letters[1]  = ':'; // normalizing it
                pos        += 2;
                break;
            default:
                // ignoring first (back-)slash character
                for (size_type index = 0; index != 3 && pos != ctx.end; ++pos) {
                    switch (*pos) {
                        case '\\': set_warning(ctx.status, reverse_solidus_used); [[fallthrough]];
                        case '/':
                            if (index == 0) {
                                continue;
                            }
                            [[fallthrough]];
                        [[likely]] default:
                            letters.at(index) = *pos;
                            ++index;
                            continue;
                    }
                    break;
                }
                if (!has_windows_driver_letter(letters.begin())) {
                    return;
                }
                letters[1] = ':'; // normalizing the windows drive letter

                switch (letters[2]) {
                    case '/': break;
                    case '\\': letters[2] = '/'; break;
                    case '?':
                    case '#': --pos; break;
                    // case '\0':
                    default: return;
                }
        }

        if constexpr (!CtxT::is_modifiable) {
            set(ctx.status, modification_required);
            // return;
        } else {
            // If buffer starts with a Windows drive letter, the parser records a warning and
            // normalizes the drive separator to a colon.
            set_warning(ctx.status, windows_drive_letter_used);
            if (ctx.pos != ctx.end && (*ctx.pos == '/' || *ctx.pos == '\\')) {
                buffer.push_back('/');
                ++ctx.pos;
            }
            buffer.push_back(letters[0]);
            buffer.push_back(letters[1]);
            ctx.pos += 2;
            ctx.pos += pos - ctx.pos - 1; // ignore characters
            if (letters[2] == '/') {
                buffer.push_back('/');
                ++ctx.pos;
            }
            end_segment(ctx, buffer);
        }
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
