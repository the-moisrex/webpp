// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_CREDENTIALS_HPP
#define WEBPP_URI_PARSE_CREDENTIALS_HPP

#include "./constants.hpp"
#include "./uri_components_encoding.hpp"

namespace webpp::uri {

    namespace details {

        template <ParsingURIContext CtxT, typename Iter = typename CtxT::iterator>
        static constexpr void parse_credentials(CtxT& ctx, Iter beg, Iter password_token_pos)
          noexcept(CtxT::is_nothrow) {
            // todo: add "needs_encoding"
            // todo: See if there's a way to find the last atsign position instead of running this function for every atsign
            // todo: use already parsed host

            using details::ascii_bitmap;
            using details::component_encoder;
            using details::USER_INFO_ENCODE_SET;

            using ctx_type = CtxT;
            using iterator = typename ctx_type::iterator;

            webpp_assume(ctx.pos < ctx.end);

            set_warning(ctx.status, uri_status::has_credentials);
            auto const atsign_pos = ctx.pos;

            // append to the username and password
            if (atsign_pos != ctx.end) {
                // parse username
                iterator const username_beg = beg;
                iterator const username_end = stl::min(password_token_pos, atsign_pos);

                clear<components::username>(ctx); // todo: it's optimizable
                component_encoder<components::username, ctx_type> user_encoder{ctx};
                user_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
                  username_beg,
                  username_end,
                  USER_INFO_ENCODE_SET);

                // parse password
                if (password_token_pos != ctx.end) {
                    iterator const password_beg = password_token_pos + 1;
                    iterator const password_end = atsign_pos;

                    clear<components::password>(ctx); // todo: it's optimizable
                    component_encoder<components::password, ctx_type> pass_encoder{ctx};
                    pass_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
                      password_beg,
                      password_end,
                      USER_INFO_ENCODE_SET);
                }
            }
        }

    } // namespace details

    /// parse username
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_username(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::ascii_bitmap;
        using details::component_encoder;
        using details::USER_INFO_ENCODE_SET;

        using ctx_type = CtxT;

        if constexpr (Options.parse_credentials) {
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::has_credentials);

            clear<components::username>(ctx);
            component_encoder<components::username, ctx_type> user_encoder{ctx};
            user_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
              ctx.pos,
              ctx.end,
              USER_INFO_ENCODE_SET);
        }
    }

    /// parse password
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_password(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using details::ascii_bitmap;
        using details::component_encoder;
        using details::USER_INFO_ENCODE_SET;

        using ctx_type = CtxT;

        if constexpr (Options.parse_credentials) {
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::has_credentials);

            clear<components::password>(ctx);
            component_encoder<components::password, ctx_type> pass_encoder{ctx};
            pass_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
              ctx.pos,
              ctx.end,
              USER_INFO_ENCODE_SET);
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_CREDENTIALS_HPP
