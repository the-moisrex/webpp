// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_CREDENTIALS_HPP
#define WEBPP_URI_PARSE_CREDENTIALS_HPP

#include "./constants.hpp"
#include "./special_schemes.hpp"
#include "./uri_context.hpp"

namespace webpp::uri {


    // https://url.spec.whatwg.org/#cannot-have-a-username-password-port
    template <URIComponents CompT>
    [[nodiscard]] static constexpr bool cannot_have_a_username_password_port(
      CompT const&          comps,
      uri_status_type const status) noexcept {
        return !has_hostname(comps) || is_file_scheme(status);
    }

    /// parse username
    /// This function doesn't care about boundaries, encodes and validates
    /// This function is not being used inside the URI parsing at all
    template <uri_options Options, URIContext CtxT>
    static constexpr void parse_username(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        using enum uri_encoding_policy;
        if constexpr (Options.parse_credentials) {
            // https://url.spec.whatwg.org/#dom-url-username
            // If this URL cannot have a username/password/port, then return.
            if (cannot_have_a_username_password_port(ctx.out, ctx.status)) {
                return;
            }

            clear_username(ctx.out);
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::contains_credentials);

            auto user_buffer = create_buffer(ctx);
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
            // https://url.spec.whatwg.org/#dom-url-password
            // If this URL cannot have a username/password/port, then return.
            if (cannot_have_a_username_password_port(ctx.out, ctx.status)) {
                return;
            }

            clear_password(ctx.out);
            if (ctx.pos == ctx.end) {
                return;
            }

            webpp_assume(ctx.pos < ctx.end);
            set_warning(ctx.status, uri_status::contains_credentials);

            auto pass_buffer = create_buffer(ctx);
            encode_uri_component<encode_chars>(ctx.pos, ctx.end, pass_buffer, USER_INFO_ENCODE_SET);
            set_password(ctx.out, stl::move(pass_buffer));
        }
    }

} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_CREDENTIALS_HPP
