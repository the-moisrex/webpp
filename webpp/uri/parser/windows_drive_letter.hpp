// Created by moisrex on 1/13/24.

#ifndef WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
#define WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP

#include "./constants.hpp"
#include "./special_schemes.hpp"

#include <iterator>

namespace webpp::uri::details {

    template <URIContext CtxT, ParsingOutput OutT>
    constexpr void start_segment([[maybe_unused]] CtxT& ctx, OutT& out, CtxBufferOf<CtxT> auto& buffer)
      noexcept(CtxT::is_nothrow || !VectorOutput<OutT>) {
        if constexpr (VectorOutput<OutT> && CtxT::is_modifiable) {
            // the non-modifiable version is the one that needs to be set, the modified versions already
            // contain the right value at this point in time
            istl::emplace_one(out, out.get_allocator());
            buffer = out.begin() + static_cast<diff_type_of<CtxT>>(out.size() - 1);
        }
    }

    /// Call this when you're done with the current segment (e.g.: reaching a dot for host, or a slash
    /// for path)
    template <URIContext CtxT, ParsingOutput OutT>
    static constexpr void
    end_segment(CtxT& ctx, OutT& out, typename CtxT::iterator& inp_beg, typename CtxT::iterator end)
      noexcept(CtxT::is_nothrow || !VectorOutput<OutT>) {
        if constexpr (VectorOutput<OutT> && !CtxT::is_modifiable) {
            // the non-modifiable version is the one that needs to be set, the modified versions already
            // contain the right value at this point in time
            istl::emplace_one(out, inp_beg, end);
            reset_begin(ctx, inp_beg);
        }
    }

    template <URIContext CtxT, ParsingOutput OutT>
    static constexpr void end_segment(CtxT& ctx, OutT& out, typename CtxT::iterator& beg)
      noexcept(CtxT::is_nothrow || !VectorOutput<OutT>) {
        end_segment(ctx, out, beg, ctx.pos);
    }

    /// 1. Skip the separator, and
    /// 2. Set the segment start
    template <URIContext CtxT, ParsingOutput OutT>
    static constexpr void next_segment(
      CtxT&                    ctx,
      OutT&                    out,
      CtxBufferOf<CtxT> auto&  buffer,
      typename CtxT::iterator& beg,
      diff_type_of<CtxT>       sep_count = 1) noexcept(CtxT::is_nothrow) {
        if constexpr (SegregatedOutput<OutT>) {
            if constexpr (CtxT::is_modifiable) {
                skip_separator(ctx, out, sep_count);
                reset_segment_start(ctx, beg);
                start_segment(ctx, out, buffer);
            } else {
                end_segment(ctx, out, beg);
                skip_separator(ctx, out, sep_count);
                reset_segment_start(ctx, beg);
            }
        } else {
            skip_separator(ctx, out, sep_count);
            end_segment(ctx, out, beg);
            reset_segment_start(ctx, beg);
        }
    }

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

    template <uri_options Options, typename Iter, typename EIter = Iter>
    [[nodiscard]] static constexpr bool starts_with_windows_driver_letter(Iter pos, EIter end) noexcept {
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

    template <uri_options Options, URIContext CtxT, ParsingOutput OutT>
    static constexpr void
    handle_windows_driver_letter(CtxT& ctx, OutT& out, CtxBufferOf<CtxT> auto& buffer, typename CtxT::iterator& seg_beg)
      noexcept(CtxT::is_nothrow) {
        using ctx_type  = CtxT;
        using char_type = typename ctx_type::char_type;
        if constexpr (Options.handle_windows_drive_letters) {
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
                default: {
                    // ignoring first (back-)slash character
                    for (size_type index = 0; index != 3 && pos != ctx.end; ++pos) {
                        switch (*pos) {
                            case '\\': set_warning(ctx.status, uri_status::reverse_solidus_used); [[fallthrough]];
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
                        case '/':
                        case '\\': letters[2] = '/'; break;
                        case '?':
                        case '#': --pos; break;
                        default: return;
                    }
                }
            }

            set_warning(ctx.status, uri_status::windows_drive_letter_used);
            next_segment(ctx, out, buffer, seg_beg);
            append_inplace_of(ctx, buffer, letters[0]);
            append_inplace_of(ctx, buffer, letters[1]);
            ignore_character(ctx, pos - ctx.pos - 1);
            if (letters[2] == '/') {
                if constexpr (!ctx_type::is_segregated) {
                    append_inplace_of(ctx, buffer, '/');
                    next_segment(ctx, buffer, buffer, seg_beg, 0);
                } else {
                    next_segment(ctx, out, buffer, seg_beg);
                }
            } else {
                end_segment(ctx, out, seg_beg);
            }
        }
    }

} // namespace webpp::uri::details

#endif // WEBPP_URI_WINDOWS_DRIVE_LETTERS_HPP
