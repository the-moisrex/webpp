// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_CREDENTIALS_HPP
#define WEBPP_URI_PARSE_CREDENTIALS_HPP

#include "./constants.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {

    namespace details {

        template <components Comp, URIContext CtxT>
        static constexpr void encode_or_set(
          CtxT&                                ctx,
          typename CtxT::iterator              pos,
          typename CtxT::iterator              end,
          [[maybe_unused]] CharSet auto const& policy_chars) noexcept(CtxT::is_nothrow) {
            if constexpr (CtxT::is_modifiable) {
                auto& out = get_storage<Comp>(ctx);
                encode_uri_component<uri_encoding_policy::encode_chars>(pos, end, out, policy_chars);
            } else {
                set_value<Comp>(ctx, pos, end);
            }
        }

        template <URIContext CtxT, typename Iter = typename CtxT::iterator>
        static constexpr void parse_credentials(CtxT& ctx, Iter beg, Iter password_token_pos)
          noexcept(CtxT::is_nothrow) {
            // todo: add "needs_encoding"
            // todo: See if there's a way to find the last atsign position instead of running this function for every atsign
            // todo: use already parsed host

            using details::ascii_bitmap;
            using details::USER_INFO_ENCODE_SET;

            using ctx_type = CtxT;
            using iterator = typename ctx_type::iterator;

            // webpp_assume(ctx.pos < ctx.end);

            set_warning(ctx.status, uri_status::has_credentials);
            auto const atsign_pos = ctx.pos;

            // append to the username and password
            if (atsign_pos == ctx.end) {
                return;
            }

            // parse username
            iterator const username_beg = beg;
            iterator const username_end = stl::min(password_token_pos, atsign_pos);

            clear_username > (ctx.out); // todo: it's optimizable
            encode_or_set<components::username>(ctx, username_beg, username_end, USER_INFO_ENCODE_SET);

            // parse password
            if (password_token_pos == ctx.end) {
                return;
            }
            iterator const password_beg = password_token_pos + 1;
            iterator const password_end = atsign_pos;

            clear_password(ctx.out); // todo: it's optimizable
            encode_or_set<components::password>(ctx, password_beg, password_end, USER_INFO_ENCODE_SET);
        }

    } // namespace details

    /// parse username
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_username(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::ascii_bitmap;
        using details::USER_INFO_ENCODE_SET;

        if constexpr (Options.parse_credentials) {
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::has_credentials);

            clear_username(ctx.out);
            encode_or_set<components::username>(ctx, ctx.pos, ctx.end, USER_INFO_ENCODE_SET);
        }
    }

    /// parse password
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_password(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::ascii_bitmap;
        using details::USER_INFO_ENCODE_SET;

        if constexpr (Options.parse_credentials) {
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::has_credentials);

            clear_password(ctx.out);
            encode_or_set<components::password>(ctx, ctx.pos, ctx.end, USER_INFO_ENCODE_SET);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_CREDENTIALS_HPP
