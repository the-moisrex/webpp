// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_PATH_HPP
#define WEBPP_URI_PARSE_PATH_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./special_schemes.hpp"
#include "./uri_components_encoding.hpp"
#include "./windows_drive_letter.hpp"
#include "io/buffer.hpp"

namespace webpp::uri {

    namespace details {


        // /// A leading surrogate is a code point that is in the range U+D800 to U+DBFF, inclusive.
        // /// https://infra.spec.whatwg.org/#leading-surrogate
        // template <typename CharT = char>
        // static constexpr auto leading_surrogate = charset_range<CharT, 0xD800, 0xDBFF>();

        // /// A trailing surrogate is a code point that is in the range U+DC00 to U+DFFF, inclusive.
        // /// https://infra.spec.whatwg.org/#trailing-surrogate
        // template <typename CharT = char>
        // static constexpr auto trailing_surrogate = charset_range<CharT, 0xDC00, 0xDFFF>();

        // /// A surrogate is a leading surrogate or a trailing surrogate.
        // /// https://infra.spec.whatwg.org/#surrogate
        // template <typename CharT = char>
        // static constexpr auto surrogate = charset(leading_surrogate<CharT>, trailing_surrogate<CharT>);


        // template <typename CharT = char>
        // static constexpr auto url_code_points =
        //   charset(ALPHA_DIGIT<CharT>,
        //           charset<CharT, 19>('!',
        //                              '$',
        //                              '&',
        //                              '(',
        //                              ')',
        //                              '\'',
        //                              '*',
        //                              '+',
        //                              ',',
        //                              '-',
        //                              '.',
        //                              '/',
        //                              ':',
        //                              ';',
        //                              '=',
        //                              '?',
        //                              '@',
        //                              '_',
        //                              '~'),
        //           // and code points in the range U+00A0 to U+10FFFD,
        //           // inclusive, excluding surrogates and noncharacters.
        //           charset_range<CharT, 0x00A0, 0x10FFFD>().except(surrogate<CharT>));


        /// Remove the last segment of a path
        template <ParsingURIContext CtxT>
        static constexpr void pop_back_segment(CtxT& ctx) noexcept(CtxT::is_nothrow) {
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
                pop_back(ctx, out, slash_loc);
            } else {
                pop_back(ctx, out);
            }
        }

        /// Remove the current segment in a path
        template <uri_parsing_options Options, ParsingURIContext CtxT>
        static constexpr void
        clear_segment(CtxT& ctx, CtxBufferOf<CtxT> auto& buffer, typename CtxT::iterator seg_beg) noexcept {
            using ctx_type = CtxT;
            if constexpr (ctx_type::is_segregated && ctx_type::is_modifiable) {
                buffer.clear();
            } else if constexpr (ctx_type::is_modifiable && !ctx_type::is_segregated) {
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
            using ctx_type  = CtxT;
            using char_type = typename ctx_type::char_type;

            auto       pos = seg_beg;
            auto const end = ctx.pos;

            stl::uint8_t dots = 0;
            char_type    prev = 0;

            for (;; ++pos) {
                if (pos == end) {
                    break;
                }

                switch (*pos) {
                    case '%':
                        if (prev != 0) {
                            return false;
                        }
                        prev = '%';
                        continue;
                    case '2':
                        if (prev != '%') {
                            return false;
                        }
                        prev = '2';
                        continue;
                    case 'e':
                    case 'E':
                        if (prev != '2') {
                            return false;
                        }
                        prev = 0;
                        ++dots;
                        continue;
                    case '.':
                        ++dots;
                        continue;
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\r':
                    [[unlikely]] case '\t':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            continue;
                        }
                        [[fallthrough]];

                    // a normal path:
                    [[likely]] default:
                        return false;
                }
                break;
            }

            switch (dots) {
                // single dot found:
                case 1: // .
                    clear_segment<Options>(ctx, buffer, seg_beg);
                    break;

                // two dots found:
                case 2: // ..
                    pop_back_segment(ctx);
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

    } // namespace details

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_opaque_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#cannot-be-a-base-url-path-state

        using enum uri_status;
        using ctx_type = CtxT;
        using iterator = typename ctx_type::iterator;

        // todo: URI Code Points are among interesting characters as well
        webpp_static_constexpr auto interesting_characters =
          details::ascii_bitmap('\0', '%', '#', '?', '\r', '\t', '\n');

        set_opaque(ctx, true);
        auto     buffer  = get_buffer(get_component<components::path>(ctx));
        auto&    out     = get_storage<components::path>(ctx);
        iterator seg_beg = ctx.pos;

        start_segment(ctx, out, buffer);
        for (;;) {
            if (encode_or_validate<uri_encoding_policy::encode_chars>(
                  ctx,
                  buffer,
                  details::C0_CONTROL_ENCODE_SET,
                  interesting_characters))
            {
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

        using details::ascii_bitmap;
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

        auto     buffer  = get_buffer(get_component<components::path>(ctx));
        auto&    out     = get_storage<components::path>(ctx);
        iterator seg_beg = ctx.pos;

        start_segment(ctx, out, buffer);
        details::handle_windows_driver_letter<Options>(ctx, out, buffer, seg_beg);

        while (!encode_or_validate<uri_encoding_policy::encode_chars>(
          ctx,
          details::PATH_ENCODE_SET,
          interesting_chars))
        {
            switch (*ctx.pos) {
                case '\\':
                    set_warning(ctx.status, uri_status::reverse_solidus_used);
                    [[fallthrough]];
                [[likely]] case '/':
                    if (details::handle_dots_in_paths<Options>(ctx, buffer, seg_beg)) {
                        ignore_character(ctx);
                        reset_segment_start(ctx, seg_beg);
                        continue;
                    }
                    next_segment_of(ctx, out, seg_beg, '/');
                    continue;
                [[likely]] case '?':
                    if constexpr (!Options.state_override) {
                        set_valid(ctx.status, uri_status::valid_queries);
                    } else {
                        stl::unreachable();
                    }
                    break;
                case '#':
                    if constexpr (!Options.state_override) {
                        set_valid(ctx.status, uri_status::valid_fragment);
                    } else {
                        stl::unreachable();
                    }
                    break;
                [[likely]] case '%':
                    if (validate_percent_encode<Options.ignore_tabs_or_newlines>(ctx, buffer)) {
                        continue;
                    }
                    set_warning(ctx.status, uri_status::invalid_character);
                    continue;
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t': {
                    set_warning(ctx.status, uri_status::invalid_character);
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
                default: set_warning(ctx.status, uri_status::invalid_character); break;
            }
            break;
        }
        stl::ignore = details::handle_dots_in_paths<Options>(ctx, buffer, seg_beg);
        end_segment(ctx, out, seg_beg);
        set_component_value<components::path>(ctx, seg_beg);

        // ignore the last "?" or "#" character
        if (ctx.pos != ctx.end && (Options.eof_is_valid && *ctx.pos != '\0')) {
            ++ctx.pos;
        } else {
            // handling empty paths
            if constexpr (ctx_type::is_modifiable && !ctx_type::is_segregated) {
                if (is_special_scheme(ctx.status) && !has_value<components::path>(ctx)) {
                    next_segment_of(ctx, out, seg_beg, '/', 0);
                }
            }

            set_valid(ctx.status, uri_status::valid);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_PATH_HPP
