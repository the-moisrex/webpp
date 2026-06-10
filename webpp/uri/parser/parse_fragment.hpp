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
        set(ctx.status, ctx.pos == ctx.end ? valid : fragment_not_supported);
    }

    template <uri_options Options, URIContext CtxT>
        requires(Options.parse_fragment)
    static constexpr void parse_fragment(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#fragment-state
        using enum uri_status;
        using details::encode_or_validate;
        using details::next_percent_encode;

        set_flag(ctx.status, has_non_null_fragment);

        if (ctx.pos == ctx.end) {
            set(ctx.status, valid);
            return;
        }

        auto buffer = create_buffer(ctx);
        while (!encode_or_validate(ctx, buffer, details::FRAGMENT_ENCODE_SET, charset('%'))) {
            if (*ctx.pos == '%' && next_percent_encode(ctx, buffer)) {
                continue;
            }
            set_warning(ctx.status, invalid_character);
        }
        end_segment(ctx, buffer);
        set_fragment(ctx.out, stl::move(buffer));
        set(ctx.status, valid);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_FRAGMENT_HPP
