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
        static constexpr void pop_back_path(CtxT& ctx, auto& buffer) noexcept(CtxT::is_nothrow) {
            // remove the last segment
            if constexpr (CtxT::is_segregated) {
                auto& path = uri::path(ctx.out);
                if (!path.empty()) {
                    path.pop_back();
                }
            } else if constexpr (CtxT::is_modifiable) {
                if (buffer.empty()) {
                    return;
                }

                auto const prev_slash = buffer.find_last_of('/');
                if (prev_slash == buffer.npos) {
                    buffer.clear();
                } else {
                    buffer.resize(prev_slash);
                }
            } else {
                set(ctx.status, uri_status::modification_required);
            }
        }

        // https://url.spec.whatwg.org/#shorten-a-urls-path
        template <URIContext CtxT>
        static constexpr void shorten_urls_path(CtxT& ctx, auto& buffer) noexcept(CtxT::is_nothrow) {
            using details::has_normalized_windows_driver_letter;

            // If url's scheme is "file", path size is 1, and path[0] is a normalized Windows
            // drive letter, then return.
            if (is_file_scheme(ctx.status)) {
                // is single normalized drive path
                bool is_norm = false;

                if constexpr (CtxT::is_segregated) {
                    if (buffer.size() == 1) {
                        auto const& segment = buffer.front();
                        is_norm = segment.size() == 2 && has_normalized_windows_driver_letter(segment.begin());
                    }
                } else if (buffer.size() == 2) {
                    is_norm = has_normalized_windows_driver_letter(buffer.begin());
                } else if (buffer.size() == 3 && buffer.front() == '/') {
                    is_norm = has_normalized_windows_driver_letter(buffer.begin() + 1);
                }

                if (is_norm) {
                    return;
                }
            }

            // Remove path's last item, if any.
            pop_back_path(ctx, buffer);
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
            // https://url.spec.whatwg.org/#path-state

            // if neither c is U+002F (/), nor url is special and c is U+005C (\), append the empty string
            // to url’s path.
            bool const empty_at_end =
              lend == ctx.end || (*lend != '/' && !(is_special_scheme(ctx.status) && *lend == '\\'));

            switch (dots_count(lbeg, lend)) {
                // single dot found:
                case 1: // .
                    if constexpr (CtxT::is_segregated) {
                        clear_segment(ctx, buffer);

                        if (empty_at_end) [[unlikely]] {
                            push_segment(uri::path(ctx.out), buffer);
                        }
                    } else if constexpr (!CtxT::is_modifiable) {
                        set(ctx.status, uri_status::modification_required);
                        return true;
                    } else {
                        if (empty_at_end) [[unlikely]] {
                            buffer.push_back('/');
                        }
                    }
                    lend = lbeg;
                    break;

                // two dots found:
                case 2: // ..

                    if constexpr (CtxT::is_segregated) {
                        auto& path = uri::path(ctx.out);

                        // Shorten url’s path.
                        shorten_urls_path(ctx, path);

                        clear_segment(ctx, buffer);

                        if (empty_at_end) [[unlikely]] {
                            push_segment(path, buffer);
                        }

                    } else if constexpr (!CtxT::is_modifiable) {
                        set(ctx.status, uri_status::modification_required);
                        return true;
                    } else {
                        // Shorten url’s path.
                        shorten_urls_path(ctx, buffer);

                        if (empty_at_end) [[unlikely]] {
                            buffer.push_back('/');
                        }
                    }
                    lend = lbeg;
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
            percent_char      = 0b10U,                    // characters: %
            dot               = 0b100U,                   // at least one dot in the segment/path
            encoding_required = 0b1000U,                  // Path encode sets
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

        bool const is_special    = is_special_scheme(ctx.status);
        auto       buffer        = create_buffer(ctx);
        bool       first_segment = is_file_scheme(ctx.status);
        bool       last_segment  = false;
        for (;;) {
            iterator const lbeg   = ctx.pos;
            stl::uint8_t   status = 0;
            for (;;) {
                status |= or_all(details::path_interesting_chars, +stop_token, ctx.pos, ctx.end);

                // If url is special and c is U+005C (\), ...
                // todo: this check can be optimized by having another table for non-special URLs
                if (!is_special && ctx.pos != ctx.end && *ctx.pos == '\\') [[unlikely]] {
                    status &= static_cast<stl::uint8_t>(~+slash);
                    ++ctx.pos;
                    continue;
                }
                break;
            }
            iterator   lend   = ctx.pos;
            auto const length = static_cast<stl::size_t>(stl::distance(lbeg, lend));

            status &= static_cast<stl::uint8_t>(~+stop_token);

            // verify percent encoded path segments
            if ((status & +percent_char) != 0) {
                assert(lbeg != lend);
                for (iterator pos = lbeg;;) {
                    if (*pos == '%') {
                        if (!next_percent_encode(pos, lend)) [[unlikely]] {
                            set_warning(ctx.status, invalid_character);
                        } else {
                            --pos;
                        }
                    }
                    if (pos == lend) {
                        break;
                    }
                    ++pos;
                }
            }

            // handle dots (full segment is given)
            if (Options.handle_dots_in_paths && (status & (dot | percent_char)) != 0 && length <= stl::size("%2e%2e")) {
                if (details::handle_dots_in_paths(ctx, buffer, lbeg, lend)) [[unlikely]] {
                    if (has_error(ctx.status)) [[unlikely]] {
                        return;
                    }
                    status |= +skip_segment;
                }
            }

            // handle path segments
            if ((status & +slash) != 0) {
                if (*lend == '\\') [[unlikely]] {
                    assert(is_special);
                    if constexpr (!CtxT::is_modifiable) {
                        set(ctx.status, modification_required);
                        return;
                    } else {
                        // If url is special and c is U+005C (\), invalid-reverse-solidus validation error.
                        set_warning(ctx.status, reverse_solidus_used);
                    }
                }
                ++ctx.pos;
                if (ctx.pos == ctx.end && !stl::exchange(last_segment, true)) {
                    --ctx.pos;
                }
            }


            // push path segment
            if ((status & +skip_segment) == 0) {
                // handle windows drive letter
                // todo: we can optimize handling of the windows driver letter by only doing it once outside the loop
                bool modify_colon = false;
                if constexpr (Options.handle_windows_drive_letters) {
                    if (stl::exchange(first_segment, false) && details::is_windows_driver_letter(lbeg, lend))
                      [[unlikely]]
                    {
                        // If url’s scheme is "file", url’s path is empty, and buffer is a Windows drive letter,
                        // then replace the second code point in buffer with U+003A (:).
                        modify_colon = true;
                    }
                }


                // percent encode unicode code points
                bool const requires_encoding = (status & +encoding_required) != 0;

                // Append buffer to url’s path.
                if constexpr (CtxT::is_segregated) {
                    if (requires_encoding) {
                        encode_uri_component<uri_encoding_policy::encode_chars>(
                          lbeg,
                          lend,
                          buffer,
                          details::PATH_ENCODE_SET);
                    } else {
                        push_segment(buffer, segment{lbeg, lend});
                        if (modify_colon) [[unlikely]] {
                            if constexpr (!CtxT::is_modifiable) {
                                if (*stl::next(lbeg) != ':') [[unlikely]] {
                                    set(ctx.status, modification_required);
                                    return;
                                }
                            } else {
                                buffer.at(1) = ':';
                            }
                        }
                    }
                    push_segment(path(ctx.out), buffer);
                    clear_segment(ctx, buffer);
                } else if constexpr (CtxT::is_modifiable) {
                    buffer.push_back('/');
                    if (requires_encoding) {
                        encode_uri_component<uri_encoding_policy::encode_chars>(
                          lbeg,
                          lend,
                          buffer,
                          details::PATH_ENCODE_SET);
                    } else {
                        if (modify_colon) [[unlikely]] {
                            buffer.push_back(*lbeg);
                            buffer.push_back(':');
                            push_segment(buffer, segment{stl::next(lbeg, 2), lend});
                        } else {
                            push_segment(buffer, segment{lbeg, lend});
                        }
                    }
                } else if ((modify_colon && *stl::next(lbeg) != ':') || requires_encoding) [[unlikely]] {
                    set(ctx.status, modification_required);
                    return;
                }
            }

            // handle end of path
            if ((status & +termination_chars) != 0) {
                if constexpr (!Options.state_override) {
                    assert(*lend == '#' || *lend == '?');
                    set(ctx.status, *lend == '?' ? valid_queries : valid_fragment);
                    ++ctx.pos;
                }
                break;
            }
            if (ctx.pos == ctx.end) {
                set(ctx.status, valid);
                break;
            }
        }

        if constexpr (!CtxT::is_segregated) {
            set_path(ctx.out, stl::move(buffer));
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PATH_HPP
