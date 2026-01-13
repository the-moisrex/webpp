// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_CREDENTIALS_HPP
#define WEBPP_URI_PARSE_CREDENTIALS_HPP

#include "./constants.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {

    namespace details {

        template <URIContext CtxT, typename Iter = typename CtxT::iterator>
        static constexpr void parse_credentials(CtxT& ctx, Iter authority_beg, Iter colon_pos)
          noexcept(CtxT::is_nothrow) {
            // todo: add "needs_encoding"
            // todo: See if there's a way to find the last atsign position instead of running this function for every atsign
            // todo: use already parsed host

            using details::ascii_bitmap;
            using details::USER_INFO_ENCODE_SET;
            using enum uri_encoding_policy;

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
            {
                iterator const username_beg = authority_beg;
                iterator const username_end = stl::min(colon_pos, atsign_pos);

                clear_username(ctx.out); // todo: it's optimizable
                auto user_buffer = create_buffer(ctx.out);
                encode_uri_component<encode_chars>(username_beg, username_end, user_buffer, USER_INFO_ENCODE_SET);
                set_username(ctx.out, stl::move(user_buffer));
            }

            // parse password
            {
                if (colon_pos == ctx.end) {
                    return;
                }
                iterator const password_beg = colon_pos + 1;
                iterator const password_end = atsign_pos;

                clear_password(ctx.out); // todo: it's optimizable
                auto pass_buffer = create_buffer(ctx.out);
                encode_uri_component<encode_chars>(password_beg, password_end, pass_buffer, USER_INFO_ENCODE_SET);
                set_password(ctx.out, stl::move(pass_buffer));
            }
        }

    } // namespace details

    /// parse username
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_username(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using enum uri_encoding_policy;
        if constexpr (Options.parse_credentials) {
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::has_credentials);

            clear_username(ctx.out);
            auto user_buffer = create_buffer(ctx.out);
            encode_uri_component<encode_chars>(ctx.pos, ctx.end, user_buffer, details::USER_INFO_ENCODE_SET);
            set_username(ctx.out, stl::move(user_buffer));
        }
    }

    /// parse password
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_password(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::ascii_bitmap;
        using details::USER_INFO_ENCODE_SET;
        using enum uri_encoding_policy;

        if constexpr (Options.parse_credentials) {
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::has_credentials);

            clear_password(ctx.out);
            auto pass_buffer = create_buffer(ctx.out);
            encode_uri_component<encode_chars>(ctx.pos, ctx.end, pass_buffer, USER_INFO_ENCODE_SET);
            set_password(ctx.out, stl::move(pass_buffer));
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_CREDENTIALS_HPP
