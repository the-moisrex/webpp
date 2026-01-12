// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_QUERIES_HPP
#define WEBPP_URI_PARSE_QUERIES_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {


    namespace details {

        template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void set_query_name(CtxT& ctx, BufT& buffer, typename CtxT::iterator seg_beg)
          noexcept(CtxT::is_nothrow) {
            if constexpr (CtxNonModifiableBuffer<BufT, CtxT>) {
                istl::assign(buffer, seg_beg, ctx.pos);
            }
        }

        template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void set_query_value(CtxT& ctx, BufT& buffer, typename CtxT::iterator& seg_beg)
          noexcept(CtxT::is_nothrow) {
            if constexpr (CtxNonModifiableBuffer<BufT, CtxT>) {
                istl::assign(buffer, seg_beg, ctx.pos);
                seg_beg = ctx.pos + 1;
            }
        }

        template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void
        append_query_value(CtxT& ctx, BufT& buffer, diff_type_of<CtxT> count, typename CtxT::iterator seg_beg)
          noexcept(CtxT::is_nothrow) {
            if constexpr (CtxMappedBuffer<BufT, CtxT>) {
                if constexpr (CtxNonModifiableBuffer<BufT, CtxT>) {
                    ctx.pos += count;
                    istl::assign(buffer, seg_beg, ctx.pos);
                } else {
                    buffer  += *ctx.pos;
                    ctx.pos += count;
                }
            } else {
                ctx.pos += count;
            }
        }

        template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void
        next_query([[maybe_unused]] CtxT& ctx, BufT& key_buffer, BufT& value_buffer, typename CtxT::iterator& seg_beg)
          noexcept(CtxT::is_nothrow) {
            if constexpr (CtxT::is_segregated && istl::StringLike<BufT>) {
                if (!key_buffer.empty() || !value_buffer.empty()) {
                    get_storage<components::queries>(ctx).emplace(key_buffer, value_buffer);
                }
                istl::clear(key_buffer);
                istl::clear(value_buffer);
            }
            reset_begin(ctx, seg_beg);
        }
    } // namespace details

    template <uri_options Options, URIContext CtxT>
        requires(!Options.parse_queries)
    static constexpr void parse_queries(CtxT& ctx) noexcept {
        using enum uri_status;
        set(ctx.status, ctx.pos == ctx.end ? valid : invalid_queries_character);
    }

    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_queries)
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state

        using enum uri_status;
        using details::append_query_value;
        using details::ascii_bitmap;
        using details::encode_or_validate_map;
        using details::next_query;
        using details::reset_begin;
        using details::set_query_name;
        using details::set_query_value;
        using details::skip_separator;
        using details::validate_percent_encode;

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
            return;
        }

        webpp_static_constexpr auto base_interesting_characters =
          !CtxT::is_segregated ? ascii_bitmap('%') : ascii_bitmap('%', '=', '&');
        webpp_static_constexpr auto interesting_characters =
          Options.parse_fragment && !Options.state_override
            ? ascii_bitmap(base_interesting_characters, '#')
            : base_interesting_characters;

        auto const query_percent_encode_set =
          is_special_scheme(ctx.status) ? details::SPECIAL_QUERIES_ENCODE_SET : details::QUERIES_ENCODE_SET;
        bool  in_value     = false;
        auto  key_buffer   = create_buffer(ctx);
        auto  value_buffer = create_buffer(ctx);
        auto& out          = get_storage<components::queries>(ctx);
        auto  seg_beg      = ctx.pos;

        // find the end of the queries
        while (!encode_or_validate_map(
          ctx,
          query_percent_encode_set,
          interesting_characters,
          !in_value ? key_buffer : value_buffer))
        {
            switch (*ctx.pos) {
                case '#':
                    if constexpr (Options.parse_fragment && !Options.state_override) {
                        clear_fragment(ctx.out);
                        set(ctx.status, valid_fragment);
                    } else {
                        stl::unreachable();
                    }
                    break;
                case '%':
                    if (!validate_percent_encode(ctx, !in_value ? key_buffer : value_buffer)) {
                        if constexpr (Options.allow_invalid_characters) {
                            set_warning(ctx.status, invalid_character);
                        } else {
                            set(ctx.status, invalid_queries_character);
                            return;
                        }
                    }
                    continue;
                case '=':
                    if (!in_value) {
                        if constexpr (CtxT::is_segregated) {
                            set_query_name(ctx, key_buffer, seg_beg);
                        }
                        skip_separator(ctx, out);
                        reset_begin(ctx, seg_beg);
                    } else {
                        append_query_value(ctx, value_buffer, 1, seg_beg);
                    }
                    in_value = true;
                    continue;
                case '&':
                    if constexpr (CtxT::is_segregated) {
                        set_query_value(ctx, value_buffer, seg_beg);
                        in_value = false;
                    }
                    skip_separator(ctx, out);
                    next_query(ctx, key_buffer, value_buffer, seg_beg);
                    continue;
                default: {
                    if constexpr (Options.allow_invalid_characters) {
                        set_warning(ctx.status, invalid_character);
                    } else {
                        set(ctx.status, invalid_queries_character);
                        return;
                    }
                    skip_separator(ctx, out);
                    // invalid characters are not errors
                    continue;
                }
            }
            break;
        }
        if constexpr (CtxT::is_segregated) {
            if (in_value) {
                set_query_value(ctx, value_buffer, seg_beg);
            } else {
                set_query_name(ctx, key_buffer, seg_beg);
            }
        }
        set_queries(ctx.out, seg_beg, ctx.pos);

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
        } else {
            ++ctx.pos;
        }
        next_query(ctx, key_buffer, value_buffer, seg_beg);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_QUERIES_HPP
