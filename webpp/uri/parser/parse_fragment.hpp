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
        using details::copy_percent_encoded;
        using details::encode_or_validate;

        // No need to handle ctx.pos == ctx.end since encode_or_validate already checks the bounds.
        // Also we for relative URLs we need `set_fragment` to set the output even if it's empty.

        set_flag(ctx.status, has_non_null_fragment);

        auto buffer = create_buffer(ctx);
        while (!encode_or_validate(ctx, buffer, details::FRAGMENT_ENCODE_SET, charset('%'))) {
            if (*ctx.pos == '%') {
                if (copy_percent_encoded(ctx, buffer)) {
                    continue;
                }
            } else if constexpr (!CtxT::is_modifiable) {
                // The scan stopped on a character a modifiable context would percent-encode;
                // a view keeps the input bytes as-is and has nowhere to encode it into.
                set(ctx.status, modification_required);
                return;
            }
            [[unlikely]] { set_warning(ctx.status, invalid_character); }
        }
        end_segment(ctx, buffer);
        set_fragment(ctx.comps, stl::move(buffer));
        set(ctx.status, valid);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_FRAGMENT_HPP
