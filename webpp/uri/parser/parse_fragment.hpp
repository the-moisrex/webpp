// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_FRAGMENT_HPP
#define WEBPP_URI_PARSE_FRAGMENT_HPP

#include "../encoding.hpp"
#include "./constants.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {

    template <uri_options Options, URIContext CtxT>
        requires(!Options.parse_fragment)
    static constexpr void parse_fragment(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#fragment-state
        using enum uri_status;
        set(ctx.status, ctx.pos == ctx.end ? valid : unexpected_fragment_found);
    }

    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_fragment)
    static constexpr void parse_fragment(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#fragment-state
        using enum uri_status;
        using char_type = typename CtxT::char_type;

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
            return;
        }

        auto const          seg_beg = ctx.pos;
        ParsingOutput auto& out     = fragment(ctx.out);
        while (!encode_or_validate(ctx, out, details::FRAGMENT_ENCODE_SET, charset<char_type, 1>('%'))) {
            if (*ctx.pos == '%' && validate_percent_encode(ctx, out)) {
                continue;
            }
            set_warning(ctx.status, invalid_character);
        }
        set_fragment(ctx.out, seg_beg, ctx.pos);
        set(ctx.status, valid);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_FRAGMENT_HPP
