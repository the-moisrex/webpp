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
        using details::encode_or_validate;
        using details::set_component_value;
        using details::validate_percent_encode;
        using enum uri_status;

        using ctx_type  = CtxT;
        using char_type = typename ctx_type::char_type;

        if (ctx.pos == ctx.end) {
            set_valid(ctx.status, valid);
            return;
        }

        auto const          seg_beg = ctx.pos;
        ParsingOutput auto& out     = get_storage<components::fragment>(ctx);

        while (!encode_or_validate(ctx, out, details::FRAGMENT_ENCODE_SET, charset<char_type, 1>('%'))) {
            switch (*ctx.pos) {
                case '%':
                    if (validate_percent_encode<Options.ignore_tabs_or_newlines>(ctx, out)) {
                        continue;
                    }
                    break;
                default: break;
            }
            set_warning(ctx.status, invalid_character);
        }
        set_component_value<components::fragment>(ctx, seg_beg);
        set_valid(ctx.status, valid);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_FRAGMENT_HPP
