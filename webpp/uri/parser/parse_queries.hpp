// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_QUERIES_HPP
#define WEBPP_URI_PARSE_QUERIES_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./uri_components_encoding.hpp"

namespace webpp::uri {


    namespace details {

        template <ParsingURIContext CtxT>
        static constexpr void
        set_query_name(CtxT& ctx, CtxBufferOf<CtxT> auto& buffer, typename CtxT::iterator seg_beg)
          noexcept(CtxT::is_nothrow) {
            if constexpr (!CtxT::is_modifiable) {
                istl::assign(buffer.first, seg_beg, ctx.pos);
            }
        }

        template <ParsingURIContext CtxT>
        static constexpr void
        set_query_value(CtxT& ctx, CtxBufferOf<CtxT> auto& buffer, typename CtxT::iterator& seg_beg)
          noexcept(CtxT::is_nothrow) {
            if constexpr (!CtxT::is_modifiable) {
                istl::assign(buffer.second, seg_beg, ctx->pos);
                seg_beg = ctx->pos + 1;
            }
        }

        template <ParsingURIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void
        append_query_value(CtxT& ctx, BufT& buffer, diff_type_of<CtxT> count, typename CtxT::iterator seg_beg)
          noexcept(CtxT::is_nothrow) {
            if constexpr (CtxMappedBuffer<CtxT, BufT>) {
                if constexpr (!CtxT::is_modifiable) {
                    ctx.pos += count;
                    istl::assign(buffer.second, seg_beg, ctx.pos);
                } else {
                    buffer.second += *ctx.pos;
                    ctx.pos       += count;
                }
            }
        }

        template <ParsingURIContext CtxT>
        static constexpr void next_query(
          [[maybe_unused]] CtxT&   ctx,
          CtxBufferOf<CtxT> auto&  buffer,
          typename CtxT::iterator& seg_beg) noexcept(CtxT::is_nothrow) {
            if constexpr (CtxT::is_segregated) {
                if (!buffer.first.empty() || !buffer.second.empty()) {
                    get_output<components::queries>(ctx).emplace(buffer);
                }
                istl::clear(buffer.first);
                istl::clear(buffer.second);
            }
            reset_begin(ctx, seg_beg);
        }
    } // namespace details

    template <uri_parsing_options Options, ParsingURIContext CtxT>
        requires(!Options.parse_queries)
    static constexpr void parse_queries(CtxT& ctx) noexcept {
        using enum uri_status;

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, valid);
        } else {
            set_error(ctx.status, invalid_queries_character);
        }
    }

    template <uri_parsing_options Options, ParsingURIContext CtxT>
        requires(Options.parse_queries)
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state

        using enum uri_status;
        using details::append_query_value;
        using details::ascii_bitmap;
        using details::next_query;

        using ctx_type    = CtxT;
        using buffer_type = typename CtxT::map_value_type;

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, valid);
            return;
        }

        webpp_static_constexpr auto base_interesting_characters =
          !ctx_type::is_segregated ? ascii_bitmap('%', '\r', '\n', '\t', '\0')
                                   : ascii_bitmap('%', '=', '&', '\r', '\n', '\t', '\0');
        webpp_static_constexpr auto interesting_characters =
          Options.parse_fragment && !Options.state_override
            ? ascii_bitmap(base_interesting_characters, '#')
            : base_interesting_characters;

        auto const query_percent_encode_set =
          is_special_scheme(ctx.status) ? details::SPECIAL_QUERIES_ENCODE_SET : details::QUERIES_ENCODE_SET;
        bool        in_value = false;
        buffer_type buffer;
        auto&       out     = get_output<components::queries>(ctx);
        auto        seg_beg = ctx.pos;

        // find the end of the queries
        while (!encode_or_validate_map<uri_encoding_policy::encode_chars>(
          ctx,
          query_percent_encode_set,
          interesting_characters,
          in_value,
          buffer))
        {
            switch (*ctx.pos) {
                case '#':
                    if constexpr (Options.parse_fragment && !Options.state_override) {
                        clear<components::fragment>(ctx);
                        set_valid(ctx.status, valid_fragment);
                    } else {
                        stl::unreachable();
                    }
                    break;
                case '%':
                    if (!validate_percent_encode<Options.ignore_tabs_or_newlines>(ctx, buffer)) {
                        if constexpr (Options.allow_invalid_characters) {
                            set_warning(ctx.status, invalid_character);
                        } else {
                            set_error(ctx.status, invalid_queries_character);
                            return;
                        }
                    }
                    continue;
                case '=':
                    if (!in_value) {
                        if constexpr (ctx_type::is_segregated) {
                            set_query_name(ctx);
                        }
                        skip_separator(ctx, out);
                        reset_begin(ctx, seg_beg);
                    } else {
                        append_query_value(ctx, buffer, 1);
                    }
                    in_value = true;
                    continue;
                case '&':
                    if constexpr (ctx_type::is_segregated) {
                        set_query_value(ctx);
                        in_value = false;
                    }
                    skip_separator(ctx, out);
                    next_query(ctx, buffer, seg_beg);
                    continue;
                [[unlikely]] case '\0':
                    if constexpr (Options.eof_is_valid) {
                        break;
                    }
                    [[fallthrough]];
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t':
                    if constexpr (Options.ignore_tabs_or_newlines) {
                        set_warning(ctx.status, invalid_character);
                        ignore_character(ctx);
                        continue;
                    }
                    [[fallthrough]];
                default: {
                    if constexpr (Options.allow_invalid_characters) {
                        set_warning(ctx.status, invalid_character);
                    } else {
                        set_error(ctx.status, invalid_queries_character);
                        return;
                    }
                    skip_separator(ctx, out);
                    // invalid characters are not errors
                    continue;
                }
            }
            break;
        }
        if constexpr (ctx_type::is_segregated) {
            if (in_value) {
                set_query_value(ctx);
            } else {
                set_query_name(ctx);
            }
        }
        set_value(ctx);

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, valid);
        } else {
            ++ctx.pos;
        }
        next_query(ctx, buffer, seg_beg);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_QUERIES_HPP
