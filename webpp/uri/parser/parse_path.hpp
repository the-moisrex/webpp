// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_PATH_HPP
#define WEBPP_URI_PARSE_PATH_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./special_schemes.hpp"
#include "./uri_context.hpp"
#include "./windows_drive_letter.hpp"

namespace webpp::uri {

    namespace details {

        /// Remove the last segment of a path
        template <URIContext CtxT>
        static constexpr void pop_back_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
            using iterator        = typename CtxT::iterator;
            using difference_type = stl::iter_difference_t<iterator>;

            auto& out       = get_path(ctx);
            using path_type = stl::remove_cvref_t<decltype(out)>;

            // remove the last segment as well
            if constexpr (istl::String<path_type>) {
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
                out.erase(out.size() - slash_loc);
            } else {
                // It's a vector, so we just pop the back
                out.pop_back();
            }
        }

        /// Remove the current segment in a path

        /// We don't need to handle dots in a path if the user is asking us not to
        template <uri_options Options, URIContext CtxT>
            requires(!Options.handle_dots_in_paths)
        static constexpr bool handle_dots_in_paths([[maybe_unused]] CtxT&                    ctx,
                                                   [[maybe_unused]] typename CtxT::iterator& seg_beg) noexcept {
            return false;
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

        template <typename IterT>
        [[nodiscard]] static constexpr stl::uint8_t dots_count(IterT pos, IterT end) noexcept {
            stl::uint8_t state = 3; // Start at initial state

            for (; pos != end; ++pos) {
                auto const category = dots_category.at(static_cast<stl::uint8_t>(*pos));
                state               = dots_state_transitions.at(state).at(category);
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
        template <uri_options Options, URIContext CtxT>
            requires(Options.handle_dots_in_paths)
        [[nodiscard]] static constexpr bool
        handle_dots_in_paths(CtxT& ctx, auto& buffer, typename CtxT::iterator& seg_beg) noexcept(CtxT::is_nothrow) {
            auto       pos  = seg_beg;
            auto const end  = ctx.pos;
            auto const dots = dots_count(pos, end);

            switch (dots) {
                // no dots found:
                case 0: return false;

                // single dot found:
                case 1: // .
                    clear_segment(ctx, buffer, seg_beg);
                    break;

                // two dots found:
                case 2: // ..
                    pop_back_path(ctx, buffer, seg_beg);
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

        template <URIContext CtxT>
        constexpr void set_opaque(CtxT& ctx, bool const is_opaque_path) noexcept {
            set_opaque(get_component<components::path>(ctx), is_opaque_path);
        }

    } // namespace details

    template <URIContext CtxT>
    static constexpr void parse_opaque_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#cannot-be-a-base-url-path-state
        using enum uri_status;
        using details::encode_or_validate;
        using details::set_opaque;
        using details::validate_percent_encode;

        // todo: URI Code Points are among interesting characters as well
        webpp_static_constexpr auto interesting_characters = details::ascii_bitmap('%', '#', '?');

        set_opaque(ctx, true);
        auto buffer = create_buffer(ctx);

        for (;;) {
            if (encode_or_validate(ctx, buffer, details::C0_CONTROL_ENCODE_SET, interesting_characters)) {
                set(ctx.status, valid);
                end_segment(ctx, buffer);
                set_path(ctx.out, buffer);
                break;
            }
            switch (*ctx.pos) {
                case '?':
                    clear_queries(ctx.out);
                    set(ctx.status, valid_queries);
                    break;
                case '#':
                    clear_fragment(ctx.out);
                    set(ctx.status, valid_fragment);
                    break;
                case '%':
                    if (validate_percent_encode(ctx, buffer)) {
                        continue;
                    }
                    [[fallthrough]];
                default:
                    ++ctx.pos;
                    set_warning(ctx.status, invalid_character);
                    continue;
            }
            end_segment(ctx, buffer);
            set_path(ctx.out, buffer);
            ++ctx.pos; // it's okay, we're not at the end
            break;
        }
    }

    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-state

        using enum uri_status;
        using details::ascii_bitmap;
        using details::encode_or_validate;
        using details::set_opaque;
        using details::validate_percent_encode;
        using iterator = typename CtxT::iterator;

        webpp_static_constexpr auto encode_set =
          CtxT::is_modifiable || CtxT::is_segregated ? details::PATH_ENCODE_SET : ascii_bitmap();

        webpp_static_constexpr auto interesting_chars_base = ascii_bitmap(encode_set, ascii_bitmap{'\\', '/', '%'});
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

        auto buffer = create_buffer(ctx);

        details::handle_windows_driver_letter<Options>(ctx, buffer);

        while (!encode_or_validate(ctx, buffer, details::PATH_ENCODE_SET, interesting_chars)) {
            switch (*ctx.pos) {
                case '\\': set_warning(ctx.status, reverse_solidus_used); [[fallthrough]];
                case '/':
                    if (details::handle_dots_in_paths<Options>(ctx, buffer)) {
                        ++ctx.pos; // ignore character
                        clear_segment(ctx, buffer);
                        continue;
                    }
                    end_segment(ctx, buffer);
                    push_segment(path(ctx.out), buffer);
                    details::append_inplace_of(buffer, '/');
                    continue;
                case '?': set_if<!Options.state_override>(ctx.status, valid_queries); break;
                case '#': set_if<!Options.state_override>(ctx.status, valid_fragment); break;
                case '%':
                    if (validate_percent_encode(ctx, buffer)) {
                        continue;
                    }
                    set_warning(ctx.status, invalid_character);
                    continue;
                [[unlikely]] default:
                    set_warning(ctx.status, invalid_character);
                    break;
            }
            break;
        }
        static_cast<void>(details::handle_dots_in_paths<Options>(ctx, buffer));
        end_segment(ctx, buffer);
        set_path(ctx.out, buffer);

        // ignore the last "?" or "#" character
        if (ctx.pos != ctx.end) {
            ++ctx.pos;
        } else {
            // handling empty paths
            if constexpr (CtxT::is_modifiable && !CtxT::is_segregated) {
                if (is_special_scheme(ctx.status) && !has_path(ctx.out)) {
                    next_segment_of(ctx, out, buffer, seg_beg, '/', 0);
                }
            }

            set(ctx.status, valid);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PATH_HPP
