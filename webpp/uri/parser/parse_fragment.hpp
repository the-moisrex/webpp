// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_FRAGMENT_HPP
#define WEBPP_URI_PARSE_FRAGMENT_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./uri_components_encoding.hpp"

namespace webpp::uri {

    template <uri_parsing_options Options, ParsingURIContext CtxT>
        requires(!Options.parse_fragment)
    static constexpr void parse_fragment(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#fragment-state
        using enum uri_status;

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, valid);
        } else {
            set_error(ctx.status, unexpected_fragment_found);
        }
    }

    template <uri_parsing_options Options, ParsingURIContext CtxT>
        requires(Options.parse_fragment)
    static constexpr void parse_fragment(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#fragment-state
        using ctx_type  = CtxT;
        using char_type = typename ctx_type::char_type;

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, uri_status::valid);
            return;
        }

        details::component_encoder<components::fragment, ctx_type> encoder{ctx};
        while (!encoder.template encode_or_validate<uri_encoding_policy::encode_chars>(
          details::FRAGMENT_ENCODE_SET,
          charset<char_type, 1>('%')))
        {
            switch (*ctx.pos) {
                case '%':
                    if (encoder.template validate_percent_encode<Options.ignore_tabs_or_newlines>()) {
                        continue;
                    }
                    break;
                default: break;
            }
            set_warning(ctx.status, uri_status::invalid_character);
        }
        encoder.set_value();
        set_valid(ctx.status, uri_status::valid);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_FRAGMENT_HPP
