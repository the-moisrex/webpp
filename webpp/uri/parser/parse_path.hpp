// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_PATH_HPP
#define WEBPP_URI_PARSE_PATH_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./special_schemes.hpp"
#include "./uri_components.hpp"
#include "./uri_context.hpp"
#include "./windows_drive_letter.hpp"

#include <utility>

namespace webpp::uri {

    namespace details {

        /// Extract the first segment of a path from the URI Context
        /// Constraint: path actually must have a first segment.
        template <URIComponents CompT>
        [[nodiscard]] static constexpr decltype(auto) first_path_segment(CompT const& comps) noexcept {
            decltype(auto) out_path = path(comps);

            if constexpr (URIStructuredComponents<CompT>) {
                // In segregated mode, the path is stored as a container of segments
                assert(!out_path.empty());
                return out_path.front();
            } else {
                // In continuous mode, the path is a flat string
                using char_type     = typename CompT::char_type;
                using str_view_type = stl::basic_string_view<char_type>;
                str_view_type path_view{out_path.data(), out_path.size()};

                // Find the start of the first segment (skip leading slashes)
                auto const start = path_view.find_first_not_of('/');
                if (start == str_view_type::npos) { // Empty segment
                    return str_view_type{};
                }

                // Find the end of the first segment
                auto const end = path_view.find_first_of('/', start);
                if (end == str_view_type::npos) {
                    return path_view.substr(start);
                }

                return path_view.substr(start, end - start);
            }
        }

        /// Remove the last segment of a path
        template <URIContext CtxT>
        static constexpr void pop_back_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            using iterator        = typename CtxT::iterator;
            using difference_type = stl::iter_difference_t<iterator>;

            // remove the last segment as well
            if constexpr (CtxT::is_segregated) {
                if (!ctx.out.path.empty()) {
                    ctx.out.path.pop_back();
                }
            } else if constexpr (CtxT::is_modifiable) {
                auto& out = [&]() -> auto& {
                    if constexpr (URIHrefContext<CtxT>) {
                        return ctx.out.href;
                    } else {
                        return ctx.out.path;
                    }
                }();
                if (out.empty()) {
                    return;
                }

                stl::size_t slash_loc = 0;

                // find the last slash
                auto const beg = out.begin();
                auto       cur = beg + static_cast<difference_type>(out.size() - 1);
                if (cur != beg) {
                    ++slash_loc;
                    --cur;
                }
                for (; cur != beg && *cur != '/'; --cur) {
                    ++slash_loc;
                }
                out.erase(out.size() - slash_loc);
            }
        }

        // https://url.spec.whatwg.org/#shorten-a-urls-path
        template <URIContext CtxT>
        static constexpr void shorten_urls_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            using details::has_normalized_windows_driver_letter;
            decltype(auto) out_path = path(ctx.out);

            // If url's scheme is "file", path size is 1, and path[0] is a normalized Windows
            // drive letter, then return.
            if (is_file_scheme(scheme(ctx.out))) {
                // is single normalized drive path
                bool is_norm = false;

                if constexpr (CtxT::is_segregated) {
                    if (out_path.size() == 1) {
                        auto const& segment = out_path.front();
                        is_norm = segment.size() == 2 && has_normalized_windows_driver_letter(segment.begin());
                    }
                } else if (out_path.size() == 2) {
                    is_norm = has_normalized_windows_driver_letter(out_path.begin());
                } else if (out_path.size() == 3 && out_path.front() == '/') {
                    is_norm = has_normalized_windows_driver_letter(out_path.begin() + 1);
                }

                if (is_norm) {
                    return;
                }
            }

            // Remove path's last item, if any.
            pop_back_path(ctx);
        }

        // Character Category Lookup Table
        static constexpr auto dots_category = []() consteval {
            // NOLINTBEGIN(*-magic-numbers, *-member-init)
            stl::array<stl::uint8_t, 256U> category;
            category.fill(5U);     // Default category is 5 (other characters)
            category.at('%') = 1U; // Start of percent-encoding
            category.at('2') = 2U; // Part of %2e
            category.at('e') = 3U; // End of %2e (lowercase)
            category.at('E') = 3U; // End of %2E (uppercase)
            category.at('.') = 4U; // Literal dot
            // NOLINTEND(*-magic-numbers, *-member-init)
            return category;
        }();

        // State Transition Table
        static constexpr stl::array<stl::array<stl::uint8_t, 6>, 11> dots_state_transitions{
          {{0, 0, 0, 0, 0, 0}, // nothing

           // State 1: dot_count=1, matching=0 (valid state)
           {1, 6, 0, 0, 2, 0},
           // State 2: dot_count=2, matching=0 (valid state)
           {2, 8, 0, 0, 0, 0},

           // State 3: dot_count=0, matching=0 (initial state)
           {3, 4, 0, 0, 1, 0},
           // State 4: dot_count=0, matching=1 (after '%')
           {4, 0, 5, 0, 0, 0},
           // State 5: dot_count=0, matching=2 (after '%2')
           {5, 0, 0, 1, 0, 0},

           // State 6: dot_count=1, matching=1 (after '%')
           {6, 0, 7, 0, 0, 0},
           // State 7: dot_count=1, matching=2 (after '%2')
           {7, 0, 0, 2, 0, 0},

           // State 8: dot_count=2, matching=1 (after '%')
           {8, 0, 9, 0, 0, 0},
           // State 9: dot_count=2, matching=2 (after '%2')
           {9, 0, 0, 0, 0, 0},
           // State 10: invalid state
           {10, 0, 0, 0, 0, 0}}
        };

        template <stl::forward_iterator IterT>
        [[nodiscard]] static constexpr stl::uint8_t dots_count(IterT pos, IterT const end) noexcept {
            stl::uint8_t state = 3; // Start at initial state

            for (; pos != end; ++pos) {
                auto const category = dots_category.at(static_cast<stl::uint8_t>(*pos));
                state               = dots_state_transitions.at(state).at(category);
                if (state == 0) {
                    break; // Exit early if sequence becomes invalid
                }
            }

            return state >= 3 ? 0 : state;
        }

        /// Handle special cases:
        ///   /.
        ///   /..
        ///   /%2e
        ///   /%2e%2e
        ///   /.%2e
        ///   /%2e.
        ///
        /// %2E or %2e is equal to a "." (dot)
        ///
        /// It's possible to have newlines and tabs in between these things
        /// @returns true if we found one or two dots
        template <URIContext CtxT>
        [[nodiscard]] static constexpr bool
        handle_dots_in_paths(CtxT& ctx, auto& buffer, typename CtxT::iterator const lbeg, typename CtxT::iterator& lend)
          noexcept(CtxT::is_nothrow) {
            switch (dots_count(lbeg, lend)) {
                // single dot found:
                case 1: // .
                    lend = lbeg;
                    if constexpr (CtxT::is_segregated) {
                        clear_segment(ctx, buffer);
                    } else if constexpr (!CtxT::is_modifiable) {
                        set(ctx.status, uri_status::modification_required);
                        return true;
                    } else {
                        // https://url.spec.whatwg.org/#path-state
                        // "If neither c is U+002F (/), nor url is special and c is U+005C (\), append
                        // the empty string to url's path."
                        if (buffer.empty() || buffer.back() != '/') {
                            buffer.push_back('/');
                        }
                    }
                    break;

                // two dots found:
                case 2: // ..
                    lend = lbeg;
                    if constexpr (CtxT::is_segregated) {
                        auto const& path = uri::path(ctx.out);
                        // don't turn "/.." into an empty path
                        if (path.size() != 1 || !path.front().empty()) [[unlikely]] {
                            pop_back_path(ctx);
                        }
                        clear_segment(ctx, buffer);

                    } else if constexpr (!CtxT::is_modifiable) {
                        set(ctx.status, uri_status::modification_required);
                        return true;
                    } else {
                        // Remove the current ".." segment, then shorten the previous path segment from `buffer`.

                        // Keep a lone root slash ("/") while trimming separator before the previous segment.
                        if (buffer.size() > 1U && buffer.back() == '/') {
                            buffer.pop_back();
                        }

                        auto const prev_slash = buffer.find_last_of('/');
                        if (prev_slash == buffer.npos) {
                            buffer.clear();
                        } else {
                            buffer.resize(prev_slash + 1U);
                        }

                        // https://url.spec.whatwg.org/#path-state
                        // "If neither c is U+002F (/), nor url is special and c is U+005C (\), append
                        // the empty string to url's path."
                        if (buffer.empty() || buffer.back() != '/') {
                            buffer.push_back('/');
                        }
                    }
                    break;


                // a normal segment found:
                default: return false;
            }

            return true;
        }

        template <URIContext CtxT>
        static constexpr void set_or_append_path(CtxT& ctx, auto& buffer) noexcept(CtxT::is_nothrow) {
            if constexpr (CtxT::is_segregated) {
                push_segment(path(ctx.out), stl::move(buffer));
            } else {
                set_path(ctx.out, stl::move(buffer));
            }
        }

        enum struct opaque_cp_type : stl::uint8_t {
            normal_opaque_path = 0,
            stop_token         = 0b1U,
            special_chars      = 0b10U | stop_token,   // characters: % SPACE
            termination_chars  = 0b100U | stop_token,  // characters: ? #
            encoding_required  = 0b1000U | stop_token, // C0 Control encode sets
        };

        [[nodiscard]] static consteval stl::uint8_t operator+(opaque_cp_type const code_point) noexcept {
            return static_cast<stl::uint8_t>(code_point);
        }

        [[nodiscard]] static consteval stl::uint8_t operator~(opaque_cp_type const code_point) noexcept {
            return static_cast<stl::uint8_t>(~+code_point);
        }

        static constexpr auto opaque_interesting_chars = categorize<stl::uint8_t, 256U>(
          cat{.set = C0_CONTROL_ENCODE_SET, .value = +opaque_cp_type::encoding_required},
          cat{.set = u8"?#", .value = +opaque_cp_type::termination_chars},
          cat{.set = u8"% ", .value = +opaque_cp_type::special_chars});

        enum struct path_cp_type : stl::uint8_t {
            normal_path       = 0,
            stop_token        = 0b1U,
            percent_char      = 0b10U | stop_token,       // characters: %
            dot               = 0b100U,                   // at least one dot in the segment/path
            encoding_required = 0b1000U | stop_token,     // Path encode sets
            slash             = 0b10'0000U | stop_token,
            termination_chars = 0b100'0000U | stop_token, // characters: ? #
            skip_segment      = 0b1000'0000U,
        };

        [[nodiscard]] static consteval stl::uint8_t operator+(path_cp_type const code_point) noexcept {
            return static_cast<stl::uint8_t>(code_point);
        }

        [[nodiscard]] static consteval stl::uint8_t operator|(path_cp_type const lhs, path_cp_type rhs) noexcept {
            return static_cast<stl::uint8_t>(+lhs | +rhs);
        }

        [[nodiscard]] static consteval stl::uint8_t operator|(stl::uint8_t const lhs, path_cp_type rhs) noexcept {
            return static_cast<stl::uint8_t>(lhs | +rhs);
        }

        static constexpr auto path_interesting_chars = categorize<stl::uint8_t, 256U>(
          cat{.set = PATH_ENCODE_SET.except(charset('?', '#')), .value = +path_cp_type::encoding_required},
          cat{.set = u8"?#", .value = +path_cp_type::termination_chars},
          cat{.set = u8"%", .value = +path_cp_type::percent_char},
          cat{.set = u8"/\\", .value = +path_cp_type::slash},
          cat{.set = u8".", .value = +path_cp_type::dot});

    } // namespace details

    template <URIContext CtxT>
    static constexpr void parse_opaque_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#cannot-be-a-base-url-path-state
        using enum uri_status;
        using details::ascii_bitmap;
        using enum details::opaque_cp_type;
        using details::encode_or_validate;
        using details::next_percent_encode;
        using details::opaque_interesting_chars;

        set_flag(ctx.status, opaque_path);
        set(ctx.status, valid);

        auto buffer = create_buffer(ctx);
        while (ctx.pos != ctx.end) {
            auto const lbeg   = ctx.pos;
            auto       status = or_all<stl::uint8_t>(opaque_interesting_chars, +stop_token, ctx.pos, ctx.end);

            // append the path to the buffer, possibly encode them as well
            if ((status & +encoding_required) == +encoding_required) {
                if constexpr (CtxT::is_modifiable) {
                    for (auto endp = stl::next(stl::exchange(ctx.pos, lbeg)); ctx.pos != endp; ++ctx.pos) {
                        encode_uri_component<uri_encoding_policy::encode_chars>(
                          *ctx.pos,
                          buffer,
                          details::C0_CONTROL_ENCODE_SET);
                    }
                } else {
                    set(ctx.status, modification_required);
                    return;
                }

                status &= ~encoding_required | +stop_token;
            } else {
                push_segment(buffer, segment{lbeg, ctx.pos});
            }

            switch (status) {
                case +termination_chars:
                    assert(ctx.pos != ctx.end);
                    switch (*ctx.pos) {
                        case '?':
                            clear_queries(ctx.out);
                            set_flag(ctx.status, has_non_null_queries);
                            set(ctx.status, valid_queries);
                            ++ctx.pos;
                            break;
                        case '#':
                            clear_fragment(ctx.out);
                            set_flag(ctx.status, has_non_null_fragment);
                            set(ctx.status, valid_fragment);
                            ++ctx.pos;
                            break;
                        default: assert(false); stl::unreachable();
                    }
                    break;
                case +special_chars:

                    assert(ctx.pos != ctx.end);
                    switch (*ctx.pos) {
                        case '%':
                            if constexpr (CtxT::is_modifiable) {
                                if (!next_percent_encode(ctx, buffer)) [[unlikely]] {
                                    set_warning(ctx.status, invalid_character);
                                }
                            } else {
                                set(ctx.status, modification_required);
                                return;
                            }
                            break;
                        case ' ':
                            // Otherwise, if c is U+0020 SPACE:
                            //   If remaining starts with U+003F (?) or U+0023 (#), then append "%20" to url’s path.
                            //   Otherwise, append U+0020 SPACE to url’s path.
                            ++ctx.pos;
                            if (*ctx.pos == '?' || *ctx.pos == '#') {
                                if constexpr (CtxT::is_modifiable) {
                                    buffer.push_back('%');
                                    buffer.push_back('2');
                                    buffer.push_back('0');
                                } else {
                                    set(ctx.status, modification_required);
                                    return;
                                }
                            } else [[likely]] {
                                if constexpr (CtxT::is_modifiable) {
                                    buffer.push_back(' ');
                                }
                                break;
                            }
                            break;
                        default: assert(false); stl::unreachable();
                    }
                    continue;
                default: continue;
            }
            break;
        }
        end_segment(ctx, buffer);
        details::set_or_append_path(ctx, buffer);
    }

    // namespace details {

    //     template <bool isModifiable>
    //     static constexpr auto encode_set = isModifiable ? details::PATH_ENCODE_SET : ascii_bitmap();

    //     // Stop on path delimiters and percent signs, but do not treat the encode set as invalid.
    //     // Characters such as spaces must be percent-encoded, not dropped.
    //     static constexpr auto interesting_chars_base = ascii_bitmap{'\\', '/', '%'};

    //     template <bool StateOverride>
    //     static constexpr auto path_interesting_chars =
    //       !StateOverride ? ascii_bitmap(interesting_chars_base, '#', '?') : interesting_chars_base;

    // } // namespace details

    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-state

        using enum uri_status;
        using enum details::path_cp_type;
        using iterator = typename CtxT::iterator;

        // attention:
        // we should not check to see if we're at the end of the string because if the path is empty, and
        // we're in a special scheme, we have to add "/" to it

        unset_flag(ctx.status, opaque_path);

        // if constexpr (!CtxT::is_segregated) {
        //     if (ctx.pos == ctx.end || (*ctx.pos != '/' && *ctx.pos != '\\')) {
        //         if (ctx.pos != ctx.beg && (*stl::prev(ctx.pos) == '/' || *stl::prev(ctx.pos) == '\\')) {
        //             --ctx.pos;
        //         } else if constexpr (!CtxT::is_modifiable) {
        //             set(ctx.status, modification_required);
        //             return;
        //         }
        //     }
        // }


        // Prepend the previous URL's path if the new path is not absolute
        //
        // WHATWG URL Standard, path state: "If buffer is a double-dot URL path segment, then ... shorten
        // url's path" and in the default branch "append buffer to url's path".
        // https://url.spec.whatwg.org/#path-state
        //
        // Also in file state, when resolving against a file base, the algorithm says to clone base's path and
        // then shorten it before switching to path state (implemented here via details::shorten_urls_path).
        // https://url.spec.whatwg.org/#file-state
        //
        // That means path state must continue from the existing path list, not start from an empty one.
        // Only flat-string mode should copy the existing path into the buffer.
        // In segregated mode, previous segments already live in ctx.out.path.
        // if constexpr (!CtxT::is_segregated) {
        //     auto const existing_path = path(ctx.out);
        //     if (!existing_path.empty() && (ctx.pos == ctx.end || (*ctx.pos != '/' && *ctx.pos != '\\'))) {
        //         if constexpr (CtxT::is_modifiable) {
        //             buffer.append(existing_path.begin(), existing_path.end());
        //             assert(!buffer.empty());
        //             if (buffer.back() != '/') {
        //                 buffer.push_back('/');
        //             }
        //             // segment_start = buffer.size();
        //         } else {
        //             set(ctx.status, modification_required);
        //             return;
        //         }
        //     }
        // }


        auto buffer = create_buffer(ctx);
        if constexpr (Options.handle_windows_drive_letters) {
            details::handle_windows_driver_letter(ctx, buffer);
        }

        for (;;) {
            iterator const lbeg   = ctx.pos;
            auto           status = or_all(details::path_interesting_chars, +stop_token, ctx.pos, ctx.end);
            iterator       lend   = ctx.pos;
            auto const     length = static_cast<stl::size_t>(stl::distance(lbeg, lend));

            status &= ~+stop_token;

            // percent encode unicode code points
            if ((status & +encoding_required) != 0) {
                if constexpr (CtxT::is_modifiable) {
                    encode_uri_component<uri_encoding_policy::encode_chars>(
                      lbeg,
                      ctx.pos,
                      buffer,
                      details::PATH_ENCODE_SET);
                } else {
                    set(ctx.status, modification_required);
                    return;
                }
            }

            // handle dots (full segment is given)
            else if (
              Options.handle_dots_in_paths && (status & (dot | percent_char)) != 0 && length <= stl::size("%2e%2e"))
            {
                if (details::handle_dots_in_paths(ctx, buffer, lbeg, lend)) [[unlikely]] {
                    if (has_error(ctx.status)) [[unlikely]] {
                        return;
                    }
                    status |= +skip_segment;
                }
            }

            // decode percent encoded path segments
            if ((status & +percent_char) != 0) {
                if (*ctx.pos == '%' && !details::next_percent_encode(ctx, buffer)) [[unlikely]] {
                    set_warning(ctx.status, invalid_character);
                }
                continue;
            }

            // handle path segments
            if ((status & +slash) != 0) {
                if (*ctx.pos == '\\') [[unlikely]] {
                    if constexpr (!CtxT::is_modifiable) {
                        set(ctx.status, modification_required);
                        return;
                    } else {
                        set_warning(ctx.status, reverse_solidus_used);
                    }
                }
                lend = ctx.pos;
                ++ctx.pos;
            }

            if ((status & +skip_segment) == 0) {
                end_segment(ctx, buffer);
                if constexpr (CtxT::is_segregated) {
                    push_segment(buffer, segment{lbeg, lend});
                    push_segment(path(ctx.out), buffer);
                    clear_segment(ctx, buffer);
                } else if constexpr (CtxT::is_modifiable) {
                    buffer.push_back('/');
                    push_segment(buffer, segment{lbeg, lend});
                }
            }
            // handle end of path
            if ((status & +termination_chars) != 0) {
                if constexpr (!Options.state_override) {
                    switch (*ctx.pos) {
                        case '?': set(ctx.status, valid_queries); break;
                        case '#': set(ctx.status, valid_fragment); break;
                        default: assert(false); stl::unreachable();
                    }
                }
                break;
            }
            if (ctx.pos == ctx.end) {
                break;
            }
        }

        // https://url.spec.whatwg.org/#path-state
        // If URL is special, host is not null, and path is empty, append the empty string to path.
        bool is_path_empty = buffer.empty();
        if constexpr (CtxT::is_segregated) {
            is_path_empty &= path(ctx.out).empty();
        }
        if (is_special_scheme(ctx.status) && has_hostname(ctx.out) && is_path_empty) {
            if constexpr (!CtxT::is_segregated) {
                if constexpr (CtxT::is_modifiable) {
                    buffer.push_back('/');
                    set_path(ctx.out, stl::move(buffer));
                } else {
                    set(ctx.status, modification_required);
                    return;
                }
            } else {
                push_segment(path(ctx.out), buffer); // buffer is empty, so this adds an empty segment
            }
        } else if constexpr (!CtxT::is_segregated) {
            set_path(ctx.out, stl::move(buffer));
        }


        // ignore the last "?" or "#" character
        if (ctx.pos != ctx.end) {
            ++ctx.pos;
            return;
        }
        set(ctx.status, valid);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PATH_HPP
