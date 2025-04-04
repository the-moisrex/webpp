// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_PATH_HPP
#define WEBPP_URI_PARSE_PATH_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./special_schemes.hpp"
#include "./uri_components_encoding.hpp"
#include "./windows_drive_letter.hpp"

namespace webpp::uri {

    namespace details {

        template <ParsingURIContext CtxT, ParsingOutput OutT>
        static constexpr void next_segment_of(
          CtxT&                    ctx,
          OutT&                    out,
          CtxBufferOf<CtxT> auto&  buffer,
          typename CtxT::iterator& beg,
          typename CtxT::char_type separator,
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
                if constexpr (CtxT::is_modifiable) {
                    skip_separator(ctx, out, separator, sep_count);
                } else {
                    skip_separator(ctx, out, sep_count);
                }
                end_segment(ctx, out, beg);
                reset_segment_start(ctx, beg);
            }
        }

        template <ParsingURIContext CtxT, ParsingOutput OutT>
        static constexpr void pop_back(
          CtxT&                               ctx,
          OutT&                               out,
          CtxBufferOf<CtxT> auto&             buffer,
          typename CtxT::iterator&            beg,
          [[maybe_unused]] diff_type_of<CtxT> hint = 0) noexcept {
            using difference_type = diff_type_of<CtxT>;
            if constexpr (CtxT::is_modifiable && VectorOutput<OutT>) {
                if (out.size() > 2) {
                    out.pop_back();
                    buffer = out.begin() + static_cast<difference_type>(out.size() - 1);
                } else if (out.size() == 1) {
                    buffer->clear();
                }
            } else if constexpr (VectorOutput<OutT>) {
                if (out.size() > 1) {
                    out.pop_back();
                } else {
                    istl::clear(out.back());
                }
                reset_segment_start(ctx, beg);
            } else if constexpr (CtxT::is_modifiable) {
                using output_t  = stl::remove_cvref_t<decltype(out)>;
                using size_type = typename output_t::size_type;
                if (!out.empty()) {
                    out.erase(out.size() - static_cast<size_type>(hint));
                }
            }
        }

        /// Remove the last segment of a path
        template <ParsingURIContext CtxT>
        static constexpr void
        pop_back_segment(CtxT& ctx, CtxBufferOf<CtxT> auto& buffer, typename CtxT::iterator& seg_beg)
          noexcept(CtxT::is_nothrow) {
            using ctx_type        = CtxT;
            using iterator        = typename ctx_type::iterator;
            using difference_type = typename stl::iterator_traits<iterator>::difference_type;

            auto& out = get_storage<components::path>(ctx);

            // remove the last segment as well
            if constexpr (ctx_type::is_modifiable && !ctx_type::is_segregated) {
                difference_type slash_loc = 0;

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
                pop_back(ctx, out, buffer, seg_beg, slash_loc);
            } else {
                pop_back(ctx, out, buffer, seg_beg);
            }
        }

        /// Remove the current segment in a path
        template <uri_parsing_options Options, ParsingURIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void
        clear_segment(CtxT& ctx, BufT& buffer, typename CtxT::iterator seg_beg) noexcept {
            using ctx_type = CtxT;
            if constexpr (ctx_type::is_segregated && ctx_type::is_modifiable) {
                buffer->clear();
            } else if constexpr (CtxModifiableBuffer<BufT, CtxT> && !ctx_type::is_segregated) {
                if constexpr (!Options.ignore_tabs_or_newlines) {
                    auto const length = static_cast<stl::size_t>(ctx.pos - seg_beg);
                    buffer.erase(buffer.size() - length);
                } else {
                    // we have to manually find the last slash and remove until there, we can't rely on
                    // on the size, because we may have newlines and tabs
                    // todo: optimization is kinda possible, but we're not on a happy path
                    pop_back_segment(ctx, buffer, seg_beg);
                }
            }
            reset_segment_start(ctx, seg_beg);
        }

        /// We don't need to handle dots in a path if the user is asking us not to
        template <uri_parsing_options Options, ParsingURIContext CtxT>
            requires(!Options.handle_dots_in_paths)
        static constexpr bool handle_dots_in_paths(
          [[maybe_unused]] CtxT&                    ctx,
          [[maybe_unused]] typename CtxT::iterator& seg_beg) noexcept {
            return false;
        }

        // Character Category Lookup Table
        template <bool IgnoreWhitespaces = true>
        static constexpr auto dots_category = ([]() consteval {
            // NOLINTBEGIN(*-magic-numbers, *-member-init)
            stl::array<stl::uint8_t, 256U> category;
            category.fill(5U);      // Default category is 5 (other characters)
            category['%'] = 1U;     // Start of percent-encoding
            category['2'] = 2U;     // Part of %2e
            category['e'] = 3U;     // End of %2e (lowercase)
            category['E'] = 3U;     // End of %2E (uppercase)
            category['.'] = 4U;     // Literal dot
            if constexpr (IgnoreWhitespaces) {
                category['\n'] = 0; // Whitespace (ignored)
                category['\r'] = 0; // Whitespace (ignored)
                category['\t'] = 0; // Whitespace (ignored)
            }
            // NOLINTEND(*-magic-numbers, *-member-init)
            return category;
        })();

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

        template <bool IgnoreWhitespace = true, typename IterT>
        [[nodiscard]] static constexpr stl::uint8_t dots_count(IterT pos, IterT end) noexcept {
            stl::uint8_t state = 3; // Start at initial state

            for (; pos != end; ++pos) {
                auto const category = dots_category<IgnoreWhitespace>[static_cast<stl::uint8_t>(*pos)];

                // NOLINTNEXTLINE(*-bounds-constant-array-index)
                state = dots_state_transitions[state][category];
                if (state == 0) {
                    break; // Exit early if sequence becomes invalid
                }
            }

            if (state >= 3) {
                return 0;
            }
            return state;
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
        template <uri_parsing_options Options, ParsingURIContext CtxT>
            requires(Options.handle_dots_in_paths)
        [[nodiscard]] static constexpr bool
        handle_dots_in_paths(CtxT& ctx, CtxBufferOf<CtxT> auto& buffer, typename CtxT::iterator& seg_beg)
          noexcept(CtxT::is_nothrow) {
            auto       pos  = seg_beg;
            auto const end  = ctx.pos;
            auto const dots = dots_count<Options.ignore_tabs_or_newlines>(pos, end);

            switch (dots) {
                // no dots found:
                case 0: return false;

                // single dot found:
                case 1: // .
                    clear_segment<Options>(ctx, buffer, seg_beg);
                    break;

                // two dots found:
                case 2: // ..
                    pop_back_segment(ctx, buffer, seg_beg);
                    clear_segment<Options>(ctx, buffer, seg_beg);
                    break;

                // a normal segment found:
                default: return false;
            }


            // If neither c is U+002F (/), nor url is special and c is U+005C (\), append the empty
            // string to url’s path. This means that for input /usr/.. the result is / and not a lack
            // of a path.
            // if (end == ctx.end || (*end != '/' && *end != '\\')) {
            //     encoder.next_segment_of('/', 0);
            // }
            return true;
        }

        template <typename OutT>
        constexpr void set_opaque([[maybe_unused]] OutT&      path_comp,
                                  [[maybe_unused]] bool const is_opaque_path) noexcept {}

        template <ParsingURIContext CtxT>
        constexpr void set_opaque(CtxT& ctx, bool const is_opaque_path) noexcept {
            set_opaque(get_component<components::path>(ctx), is_opaque_path);
        }

    } // namespace details

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_opaque_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#cannot-be-a-base-url-path-state

        using enum uri_status;
        using details::encode_or_validate;
        using details::end_segment;
        using details::set_component_value;
        using details::set_opaque;
        using details::start_segment;
        using details::validate_percent_encode;
        using ctx_type = CtxT;
        using iterator = typename ctx_type::iterator;

        // todo: URI Code Points are among interesting characters as well
        webpp_static_constexpr auto interesting_characters =
          details::ascii_bitmap('\0', '%', '#', '?', '\r', '\t', '\n');

        set_opaque(ctx, true);
        details::CtxBufferOf<CtxT> auto buffer  = get_buffer<components::path>(ctx);
        ParsingOutput auto&             out     = get_storage<components::path>(ctx);
        iterator                        seg_beg = ctx.pos;

        start_segment(ctx, out, buffer);
        for (;;) {
            if (encode_or_validate(ctx, buffer, details::C0_CONTROL_ENCODE_SET, interesting_characters)) {
                set_valid(ctx.status, valid);
                end_segment(ctx, out, seg_beg);
                set_component_value<components::path>(ctx, seg_beg);
                break;
            }
            switch (*ctx.pos) {
                [[unlikely]] case '\0':
                    if constexpr (Options.eof_is_valid) {
                        set_valid(ctx.status, valid);
                        break;
                    } else {
                        ++ctx.pos;
                        set_warning(ctx.status, invalid_character);
                        continue;
                    }
                case '?':
                    clear<components::queries>(ctx);
                    set_valid(ctx.status, valid_queries);
                    break;
                case '#':
                    clear<components::fragment>(ctx);
                    set_valid(ctx.status, valid_fragment);
                    break;
                case '%':
                    if (validate_percent_encode<Options.ignore_tabs_or_newlines>(ctx, buffer)) {
                        continue;
                    }
                    [[fallthrough]];
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t':
                default:
                    ++ctx.pos;
                    set_warning(ctx.status, invalid_character);
                    continue;
            }
            end_segment(ctx, out, seg_beg);
            set_component_value<components::path>(ctx, seg_beg);
            ++ctx.pos; // it's okay, we're not at the end
            break;
        }
    }

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-state

        using enum uri_status;
        using details::ascii_bitmap;
        using details::encode_or_validate;
        using details::end_segment;
        using details::ignore_character;
        using details::next_segment_of;
        using details::reset_segment_start;
        using details::set_component_value;
        using details::set_opaque;
        using details::start_segment;
        using details::validate_percent_encode;
        using ctx_type = CtxT;
        using iterator = typename ctx_type::iterator;

        webpp_static_constexpr auto encode_set =
          ctx_type::is_modifiable || ctx_type::is_segregated ? details::PATH_ENCODE_SET : ascii_bitmap();

        webpp_static_constexpr auto interesting_chars_base =
          ascii_bitmap(encode_set, ascii_bitmap{'\\', '\0', '/', '%', '\r', '\n', '\t'});
        webpp_static_constexpr auto interesting_chars =
          !Options.state_override ? ascii_bitmap(interesting_chars_base, '#', '?') : interesting_chars_base;

        // attention:
        // we should not check to see if we're at the end of the string because if the path is empty, and
        // we're in a special scheme, we have to add "/" to it

        if (!is_special_scheme(ctx.status)) {
            parse_opaque_path<Options>(ctx);
            return;
        }

        set_opaque(ctx, false);

        auto     buffer  = get_buffer<components::path>(ctx);
        auto&    out     = get_storage<components::path>(ctx);
        iterator seg_beg = ctx.pos;

        start_segment(ctx, out, buffer);
        details::handle_windows_driver_letter<Options>(ctx, out, buffer, seg_beg);

        while (!encode_or_validate(ctx, buffer, details::PATH_ENCODE_SET, interesting_chars)) {
            switch (*ctx.pos) {
                case '\\':
                    set_warning(ctx.status, reverse_solidus_used);
                    [[fallthrough]];
                [[likely]] case '/':
                    if (details::handle_dots_in_paths<Options>(ctx, buffer, seg_beg)) {
                        ignore_character(ctx);
                        reset_segment_start(ctx, seg_beg);
                        continue;
                    }
                    next_segment_of(ctx, out, buffer, seg_beg, '/');
                    continue;
                [[likely]] case '?':
                    if constexpr (!Options.state_override) {
                        set_valid(ctx.status, valid_queries);
                    } else {
                        stl::unreachable();
                    }
                    break;
                case '#':
                    if constexpr (!Options.state_override) {
                        set_valid(ctx.status, valid_fragment);
                    } else {
                        stl::unreachable();
                    }
                    break;
                [[likely]] case '%':
                    if (validate_percent_encode<Options.ignore_tabs_or_newlines>(ctx, buffer)) {
                        continue;
                    }
                    set_warning(ctx.status, invalid_character);
                    continue;
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t': {
                    set_warning(ctx.status, invalid_character);
                    if constexpr (Options.ignore_tabs_or_newlines) {
                        ignore_character(ctx);
                        continue;
                    } else {
                        break;
                    }
                }
                [[unlikely]] case '\0':
                    if constexpr (Options.eof_is_valid) {
                        break;
                    }
                    [[fallthrough]];
                default: set_warning(ctx.status, invalid_character); break;
            }
            break;
        }
        static_cast<void>(details::handle_dots_in_paths<Options>(ctx, buffer, seg_beg));
        end_segment(ctx, out, seg_beg);
        set_component_value<components::path>(ctx, seg_beg);

        // ignore the last "?" or "#" character
        if (ctx.pos != ctx.end && (Options.eof_is_valid && *ctx.pos != '\0')) {
            ++ctx.pos;
        } else {
            // handling empty paths
            if constexpr (ctx_type::is_modifiable && !ctx_type::is_segregated) {
                if (is_special_scheme(ctx.status) && !has_value<components::path>(ctx)) {
                    next_segment_of(ctx, out, buffer, seg_beg, '/', 0);
                }
            }

            set_valid(ctx.status, valid);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PATH_HPP
