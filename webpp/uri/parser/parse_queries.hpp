// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_QUERIES_HPP
#define WEBPP_URI_PARSE_QUERIES_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./uri_components_encoding.hpp"

namespace webpp::uri {

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        requires(!Options.parse_queries)
    static constexpr void parse_queries(CtxT& ctx) noexcept {
        using enum uri_status;

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, valid);
        } else if constexpr (Options.allow_invalid_characters) {
            set_warning(ctx.status, invalid_character);
            switch (*ctx.pos) {
                case '#':
                    if constexpr (Options.parse_fragment) {
                        set_valid(ctx.status, valid_fragment);
                        break;
                    }
                    [[fallthrough]];
                default:
                    // we don't know what else we should do, so let's just return error
                    // this shouldn't happen much since we if the option is set not to parse the queries, then
                    // we shouldn't be in this situation that the `?` would be considered as the start of a
                    // query.
                    set_error(ctx.status, invalid_queries_character);
                    break;
            }
        } else {
            set_error(ctx.status, invalid_queries_character);
        }
    }

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        requires(Options.parse_queries)
    static constexpr void parse_queries(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#query-state

        using enum uri_status;

        using ctx_type = CtxT;

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, valid);
            return;
        }

        webpp_static_constexpr auto base_interesting_characters =
          !ctx_type::is_segregated ? details::ascii_bitmap('%', '\r', '\n', '\t', '\0')
                                   : details::ascii_bitmap('%', '=', '&', '\r', '\n', '\t', '\0');
        webpp_static_constexpr auto interesting_characters =
          Options.parse_fragment && !Options.state_override
            ? details::ascii_bitmap(base_interesting_characters, '#')
            : base_interesting_characters;

        auto const query_percent_encode_set =
          is_special_scheme(ctx.status) ? details::SPECIAL_QUERIES_ENCODE_SET : details::QUERIES_ENCODE_SET;

        bool in_value = false;

        details::component_encoder<components::queries, ctx_type> encoder{ctx};

        // find the end of the queries
        while (!encoder.template encode_or_validate_map<uri_encoding_policy::encode_chars>(
          query_percent_encode_set,
          interesting_characters,
          in_value))
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
                    if (!encoder.template validate_percent_encode<Options.ignore_tabs_or_newlines>()) {
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
                            encoder.set_query_name();
                        }
                        encoder.skip_separator();
                        encoder.reset_begin();
                    } else {
                        encoder.append_query_value(1);
                    }
                    in_value = true;
                    continue;
                case '&':
                    if constexpr (ctx_type::is_segregated) {
                        encoder.set_query_value();
                        in_value = false;
                    }
                    encoder.skip_separator();
                    encoder.next_query();
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
                        encoder.ignore_character();
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
                    encoder.skip_separator();
                    // invalid characters are not errors
                    continue;
                }
            }
            break;
        }
        if constexpr (ctx_type::is_segregated) {
            if (in_value) {
                encoder.set_query_value();
            } else {
                encoder.set_query_name();
            }
        }
        encoder.set_value();

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, valid);
        } else {
            ++ctx.pos;
        }
        encoder.next_query();
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_QUERIES_HPP
