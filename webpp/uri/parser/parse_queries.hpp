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
        set(ctx.status, ctx.pos == ctx.end ? valid : queries_not_supported);
    }

    /// Parse into string-based queries components
    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_queries && !CtxT::is_segregated)
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state

        using enum uri_status;
        using enum uri_encoding_policy;
        using details::ascii_bitmap;
        using details::copy_percent_encoded;
        using details::encode_or_validate;
        using details::skip_separator;

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
            return;
        }

        webpp_static_constexpr auto interesting_characters =
          Options.parse_fragment && !Options.state_override ? ascii_bitmap('&', '%', '#') : ascii_bitmap('&', '%');

        auto const query_percent_encode_set =
          is_special_scheme(ctx.status) ? details::SPECIAL_QUERIES_ENCODE_SET : details::QUERIES_ENCODE_SET;
        auto buffer = create_buffer(ctx);

        // find the end of the queries
        while (!encode_or_validate(ctx, buffer, query_percent_encode_set, interesting_characters)) {
            switch (*ctx.pos) {
                case '&':
                    // remove things like '&&&&&&' in the queries
                    if constexpr (!CtxT::is_modifiable) {
                        // Collapsing a run of "&" would rewrite the input.
                        if (ctx.pos != ctx.beg && *stl::prev(ctx.pos) == '&') [[unlikely]] {
                            set(ctx.status, modification_required);
                            return;
                        }
                    } else if (buffer.empty() || buffer.back() != '&') [[likely]] {
                        buffer.push_back('&');
                    }
                    ++ctx.pos;
                    continue;
                case '#':
                    if constexpr (Options.parse_fragment && !Options.state_override) {
                        clear_fragment(ctx.comps);
                        // Record non-null (but possibly empty) fragment here; the EOF path below
                        // overwrites our state value with "valid", so parse_fragment may never run.
                        set_flag(ctx.status, has_non_null_fragment);
                        ++ctx.pos;
                        if constexpr (URIRelativeComponents<stl::remove_cvref_t<decltype(ctx.comps)>>) {
                            // queries() uses fragment_start to exclude the '#' separator, so an
                            // empty fragment still has to record where it starts.
                            set_fragment(ctx.comps, segment{ctx.pos, ctx.pos});
                        }
                        set(ctx.status, valid_fragment);
                        break;
                    } else {
                        assert(false);
                        stl::unreachable();
                    }
                    break;
                case '%':
                    if (!copy_percent_encoded(ctx, buffer)) [[unlikely]] {
                        if constexpr (Options.allow_invalid_characters) {
                            set_warning(ctx.status, invalid_character);
                        } else {
                            set(ctx.status, invalid_queries_character);
                            return;
                        }
                    }
                    continue;
                [[unlikely]] default:
                    if constexpr (!CtxT::is_modifiable) {
                        // The scan only stops here on a character a modifiable context would
                        // percent-encode, and a view has nowhere to encode it into.
                        set(ctx.status, modification_required);
                        return;
                    } else if constexpr (Options.allow_invalid_characters) {
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
        set_queries(ctx.comps, stl::move(buffer));
        set_flag(ctx.status, has_non_null_queries);

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
        }
    }

    /// Parse into a Structured queries (usually a vector<pair<string, string>>).
    /// The pair's second string is storage-encoded: it is empty when no '=' separator was present,
    /// and otherwise holds '=' followed by the raw value. Keys (first) are always the raw key.
    /// Examples: "?key" -> ("key", ""), "?key=" -> ("key", "="), "?key=1" -> ("key", "=1"),
    /// "?key==" -> ("key", "=="), "?=" -> ("", "=").
    /// Read decoded values with uri::query_value() / uri::query_has_value(); rendering consumes
    /// the encoding directly (first + second reproduces the raw pair bytes).
    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_queries && CtxT::is_segregated)
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state

        using enum uri_status;
        using enum uri_encoding_policy;
        using details::ascii_bitmap;
        using details::copy_percent_encoded;
        using details::encode_or_validate;
        using details::skip_separator;

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
        auto& out          = queries(ctx.comps);

        // find the end of the queries
        while (!encode_or_validate(ctx,
                                   !in_value ? key_buffer : value_buffer,
                                   query_percent_encode_set,
                                   interesting_characters))
        {
            switch (*ctx.pos) {
                case '#':
                    if constexpr (Options.parse_fragment && !Options.state_override) {
                        clear_fragment(ctx.comps);
                        // Record non-null (but possibly empty) fragment here; the EOF path below
                        // overwrites our state value with "valid", so parse_fragment may never run.
                        set_flag(ctx.status, has_non_null_fragment);
                        ++ctx.pos;
                        set(ctx.status, valid_fragment);
                        break;
                    } else {
                        assert(false);
                        stl::unreachable();
                    }
                case '%':
                    if (!copy_percent_encoded(ctx, !in_value ? key_buffer : value_buffer)) [[unlikely]] {
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
                        value_buffer.push_back('=');
                    } else {
                        skip_separator(ctx, value_buffer);
                    }
                    in_value = true;
                    continue;
                case '&':
                    end_segment(ctx, in_value ? value_buffer : key_buffer);
                    // Only push segment if it's not empty (both key and value)
                    if (!key_buffer.empty() || !value_buffer.empty()) {
                        push_segment(out, stl::move(key_buffer), stl::move(value_buffer));
                    } else [[unlikely]] {
                        clear_segment(ctx, key_buffer);
                        clear_segment(ctx, value_buffer);
                    }
                    in_value = false;
                    ++ctx.pos;
                    continue;
                [[unlikely]] default: {
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
        if (!key_buffer.empty() || !value_buffer.empty()) {
            push_segment(out, stl::move(key_buffer), stl::move(value_buffer));
        }
        set_flag(ctx.status, has_non_null_queries);

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_QUERIES_HPP
