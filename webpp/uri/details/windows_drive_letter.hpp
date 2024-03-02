// Created by moisrex on 1/13/24.

#ifndef WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
#define WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP

#include "constants.hpp"

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

    template <uri_parsing_options Options, typename Iter, typename EIter = Iter>
    [[nodiscard]] static constexpr bool starts_with_windows_driver_letter(Iter pos, EIter end) noexcept {
        // https://url.spec.whatwg.org/#start-with-a-windows-drive-letter

        using char_type = typename stl::iterator_traits<Iter>::value_type;

        switch (end - pos) {
            case 0:
            case 1: return false;
            case 2: return has_windows_driver_letter(pos);
            default: {
                stl::array<char_type, 3> letters{};
                // ignoring first (back-)slash character
                for (int index = 0; index != 3 && pos != end; ++pos) {
                    switch (*pos) {
                        [[unlikely]] case '\r':
                        [[unlikely]] case '\n':
                        [[unlikely]] case '\t':
                            if constexpr (Options.ignore_tabs_or_newlines) {
                                continue;
                            }
                            return false;
                        case '\\':
                        case '/':
                            if (index == 0) {
                                continue;
                            }
                            [[fallthrough]];
                        [[likely]] default:
                            letters[index] = *pos;
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

    template <uri_parsing_options Options, ParsingURIContext CtxT>
    static constexpr void handle_windows_driver_letter(
      CtxT&                                      ctx,
      component_encoder<components::path, CtxT>& encoder) noexcept(CtxT::is_nothrow) {
        using ctx_type  = CtxT;
        using char_type = typename ctx_type::char_type;
        if constexpr (Options.handle_windows_drive_letters) {
            if (!is_file_scheme(ctx.scheme)) {
                return;
            }

            // https://url.spec.whatwg.org/#start-with-a-windows-drive-letter
            stl::array<char_type, 3> letters{};
            auto                     pos = ctx.pos;
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
                default: {
                    // ignoring first (back-)slash character
                    for (int index = 0; index != 3 && pos != ctx.end; ++pos) {
                        switch (*pos) {
                            [[unlikely]] case '\r':
                            [[unlikely]] case '\n':
                            [[unlikely]] case '\t':
                                if constexpr (Options.ignore_tabs_or_newlines) {
                                    set_warning(ctx.status, uri_status::invalid_character);
                                    continue;
                                }
                                return;
                            case '\\':
                                set_warning(ctx.status, uri_status::reverse_solidus_used);
                                [[fallthrough]];
                            case '/':
                                if (index == 0) {
                                    continue;
                                }
                                [[fallthrough]];
                            [[likely]] default:
                                letters[index] = *pos;
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
                        case '/':
                        case '\\': letters[2] = '/'; break;
                        case '?':
                        case '#': --pos; break;
                        default: return;
                    }
                }
            }

            set_warning(ctx.status, uri_status::windows_drive_letter_used);
            encoder.next_segment();
            encoder.append_inplace_of(letters[0]);
            encoder.append_inplace_of(letters[1]);
            encoder.ignore_character(pos - ctx.pos - 1);
            if (letters[2] == '/') {
                if constexpr (!ctx_type::is_segregated) {
                    encoder.append_inplace_of('/');
                    encoder.next_segment(0);
                } else {
                    encoder.next_segment();
                }
            } else {
                encoder.end_segment();
            }
        }
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
