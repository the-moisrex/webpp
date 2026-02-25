// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_QUERIES_HPP
#define WEBPP_URI_PARSE_QUERIES_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {

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
        using details::encode_or_validate;
        using details::skip_separator;
        using details::next_percent_encode;
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
        while (!encode_or_validate(ctx, buffer, query_percent_encode_set, interesting_characters)) {
            switch (*ctx.pos) {
                case '#':
                    if constexpr (Options.parse_fragment && !Options.state_override) {
                        clear_fragment(ctx.out);
                        ++ctx.pos;
                        set(ctx.status, valid_fragment);
                        break;
                    } else {
                        assert(false);
                        stl::unreachable();
                    }
                    break;
                case '%':
                    if (!next_percent_encode(ctx, buffer)) {
                        if constexpr (Options.allow_invalid_characters) {
                            set_warning(ctx.status, invalid_character);
                        } else {
                            set(ctx.status, invalid_queries_character);
                            return;
                        }
                    }
                    continue;
                default:
                    if constexpr (Options.allow_invalid_characters) {
                        set_warning(ctx.status, invalid_character);
                        skip_separator(ctx, buffer);
                        // invalid characters are not errors
                        continue;
                    } else {
                        set(ctx.status, invalid_queries_character);
                        return;
                    }
            }
            break;
        }
        end_segment(ctx, buffer);
        set_queries(ctx.out, stl::move(buffer));

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
        }
    }

    /// Parse into a Structured queries (usually a vector<pair<string, string>>)
    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_queries && URIStructuredComponents<typename CtxT::component_type>)
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state

        using enum uri_status;
        using details::ascii_bitmap;
        using details::encode_or_validate;
        using details::skip_separator;
        using details::next_percent_encode;
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
        auto& out          = queries(ctx.out);

        // find the end of the queries
        while (!encode_or_validate(ctx,
                                   !in_value ? key_buffer : value_buffer,
                                   query_percent_encode_set,
                                   interesting_characters))
        {
            switch (*ctx.pos) {
                case '#':
                    if constexpr (Options.parse_fragment && !Options.state_override) {
                        clear_fragment(ctx.out);
                        ++ctx.pos;
                        set(ctx.status, valid_fragment);
                        break;
                    } else {
                        assert(false);
                        stl::unreachable();
                    }
                case '%':
                    if (!next_percent_encode(ctx, !in_value ? key_buffer : value_buffer)) {
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
                        ++ctx.pos;
                    } else {
                        skip_separator(ctx, value_buffer);
                    }
                    in_value = true;
                    continue;
                case '&':
                    end_segment(ctx, in_value ? value_buffer : key_buffer);
                    push_segment(out, stl::move(key_buffer), stl::move(value_buffer));
                    clear_segment(ctx, key_buffer);
                    clear_segment(ctx, value_buffer);
                    in_value = false;
                    ++ctx.pos;
                    continue;
                default: {
                    if constexpr (Options.allow_invalid_characters) {
                        set_warning(ctx.status, invalid_character);
                    } else {
                        set(ctx.status, invalid_queries_character);
                        return;
                    }
                    skip_separator(ctx, in_value ? value_buffer : key_buffer);
                    // invalid characters are not errors
                    continue;
                }
            }
            break;
        }
        end_segment(ctx, in_value ? value_buffer : key_buffer);
        push_segment(out, stl::move(key_buffer), stl::move(value_buffer));

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_QUERIES_HPP
