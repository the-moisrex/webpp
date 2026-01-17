// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_QUERIES_HPP
#define WEBPP_URI_PARSE_QUERIES_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {


    namespace details {

        template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void set_query_name(CtxT& ctx, BufT& buffer) noexcept(CtxT::is_nothrow) {
            if constexpr (CtxNonModifiableBuffer<BufT, CtxT>) {
                istl::assign(buffer, seg_beg, ctx.pos);
            }
        }

        template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void set_query_value(CtxT& ctx, BufT& buffer) noexcept(CtxT::is_nothrow) {
            if constexpr (CtxNonModifiableBuffer<BufT, CtxT>) {
                istl::assign(buffer, seg_beg, ctx.pos);
                seg_beg = ctx.pos + 1;
            }
        }

        template <URIContext CtxT, CtxBufferOf<CtxT> BufT>
        static constexpr void append_query_value(CtxT& ctx, BufT& buffer, diff_type_of<CtxT> count)
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
        static constexpr void next_query([[maybe_unused]] CtxT& ctx, BufT& key_buffer, BufT& value_buffer)
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

    /// Parse into string-based queries components
    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_queries && !URIStructuredComponents<typename CtxT::component_type>)
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state

        using enum uri_status;
        using details::ascii_bitmap;
        using details::skip_separator;
        using details::validate_percent_encode;
        using enum uri_encoding_policy;

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
            return;
        }

        webpp_static_constexpr auto interesting_characters =
          Options.parse_fragment && !Options.state_override ? ascii_bitmap('%', '#') : ascii_bitmap('%');

        auto const query_percent_encode_set =
          is_special_scheme(ctx.status) ? details::SPECIAL_QUERIES_ENCODE_SET : details::QUERIES_ENCODE_SET;
        auto buffer = create_buffer(ctx);

        // find the end of the queries
        while (!encode_or_validate<encode_chars>(ctx, buffer, query_percent_encode_set, interesting_characters)) {
            switch (*ctx.pos) {
                case '#':
                    if constexpr (Options.parse_fragment && !Options.state_override) {
                        clear_fragment(ctx.out);
                        ++ctx.pos;
                        set(ctx.status, valid_fragment);
                        return;
                    } else {
                        stl::unreachable();
                    }
                    break;
                case '%':
                    if (!validate_percent_encode(ctx, buffer)) {
                        if constexpr (Options.allow_invalid_characters) {
                            set_warning(ctx.status, invalid_character);
                        } else {
                            set(ctx.status, invalid_queries_character);
                            return;
                        }
                    }
                    continue;
                default: {
                    if constexpr (Options.allow_invalid_characters) {
                        set_warning(ctx.status, invalid_character);
                    } else {
                        set(ctx.status, invalid_queries_character);
                        return;
                    }
                    skip_separator(ctx, buffer);
                    // invalid characters are not errors
                    continue;
                }
            }
            break;
        }
        end_segment(ctx, buffer);
        set_queries(ctx.out, buffer);

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
        }
    }

    /// Parse into a Structured queries (usually a vector<pair<string, string>>)
    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_queries)
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state

        using enum uri_status;
        using details::ascii_bitmap;
        using details::skip_separator;
        using details::validate_percent_encode;
        using enum uri_encoding_policy;

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
            return;
        }

        webpp_static_constexpr auto interesting_characters =
          Options.parse_fragment && !Options.state_override
            ? ascii_bitmap('%', '=', '&', '#')
            : ascii_bitmap('%', '=', '&');

        auto const query_percent_encode_set =
          is_special_scheme(ctx.status) ? details::SPECIAL_QUERIES_ENCODE_SET : details::QUERIES_ENCODE_SET;
        bool  in_value     = false;
        auto  key_buffer   = create_buffer(ctx);
        auto  value_buffer = create_buffer(ctx);

        // find the end of the queries
        while (!encode_or_validate<encode_chars>(
          ctx,
          !in_value ? key_buffer : value_buffer,
          query_percent_encode_set,
          interesting_characters))
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
                        end_segment(ctx, key_buffer);
                        // todo:
                        skip_separator(ctx, out);
                    } else {
                        skip_separator(ctx, value_buffer);
                    }
                    in_value = true;
                    continue;
                case '&':
                    if constexpr (CtxT::is_segregated) {
                        set_query_value(ctx, value_buffer);
                        in_value = false;
                    }
                    skip_separator(ctx, out);
                    next_query(ctx, key_buffer, value_buffer);
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
