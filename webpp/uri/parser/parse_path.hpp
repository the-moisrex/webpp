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

            // remove the last segment as well
            if constexpr (URIStructuredComponents<typename CtxT::component_type>) {
                if (!ctx.out.path.empty()) {
                    ctx.out.path.pop_back();
                }
            } else if constexpr (URIModifiableComponents<typename CtxT::component_type>) {
                auto& out = [&]() -> auto& {
                    if constexpr (URIHrefComponents<typename CtxT::component_type>) {
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
            auto const out_path = path(ctx.out);

            // If url's scheme is "file", path size is 1, and path[0] is a normalized Windows
            // drive letter, then return.
            if (is_file_scheme(scheme(ctx.out))) {
                auto const is_single_normalized_drive_path = [&]() constexpr noexcept {
                    if (out_path.size() == 2) {
                        return details::has_normalized_windows_driver_letter(out_path.begin());
                    }
                    if (out_path.size() == 3 && out_path.front() == '/') {
                        return details::has_normalized_windows_driver_letter(out_path.begin() + 1);
                    }
                    return false;
                }();

                if (is_single_normalized_drive_path) {
                    return;
                }
            }

            // Remove path's last item, if any.
            pop_back_path(ctx);
        }

        /// We don't need to handle dots in a path if the user is asking us not to
        template <uri_options Options, URIContext CtxT>
            requires(!Options.handle_dots_in_paths)
        static constexpr bool handle_dots_in_paths(
          [[maybe_unused]] CtxT&       ctx,
          [[maybe_unused]] auto&       buffer,
          [[maybe_unused]] stl::size_t segment_start = 0) noexcept {
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

            if (state >= 3) {
                return 0;
            }
            return state;
        }

        // https://url.spec.whatwg.org/#path-state
        // "If neither c is U+002F (/), nor url is special and c is U+005C (\), append
        // the empty string to url's path."
        template <URIContext CtxT, typename BufferT>
        [[nodiscard]] static constexpr bool should_append_empty_path_segment(
          CtxT const&    ctx,
          BufferT const& buffer) noexcept {
            if constexpr (istl::String<BufferT>) {
                return buffer.empty() || buffer.back() != '/';
            } else {
                if (ctx.pos == ctx.end) {
                    return true;
                }
                if (*ctx.pos == '/') {
                    return false;
                }
                return !(is_special_scheme(ctx.status) && *ctx.pos == '\\');
            }
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
        handle_dots_in_paths(CtxT& ctx, auto& buffer, stl::size_t const segment_start = 0) noexcept(CtxT::is_nothrow) {
            using stl::begin;
            using stl::end;

            using buffer_type                      = stl::remove_cvref_t<decltype(buffer)>;
            static constexpr bool is_string_buffer = istl::String<buffer_type>;

            auto segment_begin = begin(buffer);
            if constexpr (is_string_buffer) {
                // For string buffers, `segment_start` points at the beginning of the current segment.
                auto const safe_segment_start = stl::min(segment_start, buffer.size());
                segment_begin                 = begin(buffer) + static_cast<stl::ptrdiff_t>(safe_segment_start);
            }

            switch (dots_count(segment_begin, end(buffer))) {
                // no dots found:
                case 0: return false;

                // single dot found:
                case 1: // .
                    if constexpr (is_string_buffer) {
                        auto const seg_start = static_cast<stl::size_t>(stl::distance(begin(buffer), segment_begin));
                        buffer.resize(seg_start);
                        if (should_append_empty_path_segment(ctx, buffer)) {
                            buffer.push_back('/');
                        }
                    } else {
                        clear_segment(ctx, buffer);
                        if constexpr (CtxT::is_segregated) {
                            if (should_append_empty_path_segment(ctx, buffer)) {
                                push_segment(path(ctx.out), create_buffer(ctx));
                            }
                        }
                    }
                    break;

                // two dots found:
                case 2: // ..
                    if constexpr (is_string_buffer) {
                        // Remove the current ".." segment, then shorten the previous path segment from `buffer`.
                        auto const seg_start = static_cast<stl::size_t>(stl::distance(begin(buffer), segment_begin));
                        buffer.resize(seg_start);

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

                        // WHATWG URL Standard quote: "append the empty string to url's path."
                        if (should_append_empty_path_segment(ctx, buffer)) {
                            buffer.push_back('/');
                        }
                    } else {
                        pop_back_path(ctx);
                        clear_segment(ctx, buffer);
                        if constexpr (CtxT::is_segregated) {
                            if (should_append_empty_path_segment(ctx, buffer)) {
                                push_segment(path(ctx.out), create_buffer(ctx));
                            }
                        }
                    }
                    break;


                // a normal segment found:
                default: return false;
            }

            return true;
        }

    } // namespace details

    template <URIContext CtxT>
    static constexpr void parse_opaque_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#cannot-be-a-base-url-path-state
        using enum uri_status;
        using details::ascii_bitmap;
        using details::encode_or_validate;
        using details::next_percent_encode;

        set_flag(ctx.status, opaque_path);

        auto buffer = create_buffer(ctx);
        while (!encode_or_validate(ctx, buffer, details::C0_CONTROL_ENCODE_SET, ascii_bitmap('%', '#', '?'))) {
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
                    if (!next_percent_encode(ctx, buffer)) {
                        set_warning(ctx.status, invalid_character);
                    }
                    continue;
                default:
                    ++ctx.pos;
                    set_warning(ctx.status, invalid_character);
                    continue;
            }
            end_segment(ctx, buffer);
            set_path(ctx.out, stl::move(buffer));
            clear_segment(ctx, buffer);
            ++ctx.pos; // it's okay, we're not at the end
            return;
        }
        set(ctx.status, valid);
        end_segment(ctx, buffer);
        set_path(ctx.out, stl::move(buffer));
    }

    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-state

        using enum uri_status;
        using details::ascii_bitmap;
        using details::encode_or_validate;
        using details::next_percent_encode;

        webpp_static_constexpr auto encode_set =
          CtxT::is_modifiable || CtxT::is_segregated ? details::PATH_ENCODE_SET : ascii_bitmap();

        // Stop on path delimiters and percent signs, but do not treat the encode set as invalid.
        // Characters such as spaces must be percent-encoded, not dropped.
        webpp_static_constexpr auto interesting_chars_base = ascii_bitmap{'\\', '/', '%'};
        webpp_static_constexpr auto interesting_chars =
          !Options.state_override ? ascii_bitmap(interesting_chars_base, '#', '?') : interesting_chars_base;

        // attention:
        // we should not check to see if we're at the end of the string because if the path is empty, and
        // we're in a special scheme, we have to add "/" to it

        unset_flag(ctx.status, opaque_path);

        auto        buffer        = create_buffer(ctx);
        stl::size_t segment_start = 0U;

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
        auto const existing_path = path(ctx.out);
        if (!existing_path.empty() && (ctx.pos == ctx.end || (*ctx.pos != '/' && *ctx.pos != '\\'))) {
            if constexpr (istl::String<decltype(buffer)>) {
                buffer.append(existing_path.begin(), existing_path.end());
                if (!buffer.empty() && buffer.back() != '/') {
                    buffer.push_back('/');
                }
                segment_start = buffer.size();
            } else {
                set(ctx.status, modification_required);
                return;
            }
        }

        details::handle_windows_driver_letter<Options>(ctx, buffer);
        while (!encode_or_validate(ctx, buffer, encode_set, interesting_chars)) {
            switch (*ctx.pos) {
                case '\\': set_warning(ctx.status, reverse_solidus_used); [[fallthrough]];
                case '/':
                    if (details::handle_dots_in_paths<Options>(ctx, buffer, segment_start)) {
                        ++ctx.pos; // ignore character
                        segment_start = buffer.size();
                        continue;
                    }
                    end_segment(ctx, buffer);
                    if constexpr (CtxT::is_segregated) {
                        push_segment(path(ctx.out), buffer);
                    }
                    details::append_inplace_of(ctx, buffer, '/');
                    segment_start = buffer.size();
                    continue;
                case '?': set_if<!Options.state_override>(ctx.status, valid_queries); break;
                case '#': set_if<!Options.state_override>(ctx.status, valid_fragment); break;
                case '%':
                    if (!next_percent_encode(ctx, buffer)) {
                        set_warning(ctx.status, invalid_character);
                    }
                    continue;
                [[unlikely]] default:
                    set_warning(ctx.status, invalid_character);
                    break;
            }
            break;
        }
        static_cast<void>(details::handle_dots_in_paths<Options>(ctx, buffer, segment_start));
        end_segment(ctx, buffer);

        // https://url.spec.whatwg.org/#path-state
        // If URL is special, host is not null, and path is empty, append the empty string to path.
        if (is_special_scheme(ctx.status) && has_hostname(ctx.out) && buffer.empty()) {
            if constexpr (CtxT::is_segregated) {
                push_segment(path(ctx.out), buffer);
            }
            details::append_inplace_of(ctx, buffer, '/', 0);
        }

        set_path(ctx.out, stl::move(buffer));

        // ignore the last "?" or "#" character
        if (ctx.pos != ctx.end) {
            ++ctx.pos;
        } else {
            set(ctx.status, valid);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PATH_HPP
