// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../std/string.hpp"
#include "../std/utility.hpp"
#include "details/constants.hpp"
#include "details/uri_components.hpp"
#include "encoding.hpp"

namespace webpp::uri {


    template <typename... T>
    static constexpr void parse_authority(uri::parsing_uri_context<T...>& ctx) noexcept(
      uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#authority-state

        using ctx_type = uri::parsing_uri_context<T...>;

        bool at_char_seen        = false;
        bool password_token_seen = false;
        for (; ctx.pos != ctx.end; ++ctx.pos) {
            switch (*ctx.pos) {
                case '@':
                    ctx.status = stl::to_underlying(uri_status::has_credentials);

                    if constexpr (ctx_type::is_modifiable) {
                        if (at_char_seen) {
                            if (password_token_seen) {
                                ctx.out.append_password("%40");
                            } else {
                                ctx.out.append_username("%40");
                            }
                        }
                    }
                    at_char_seen = true;

                    // todo: continue


                    break;
                case ':':
                    if (password_token_seen) {
                        // todo
                    }
                    break;
                case '\\':
                    if (!is_special_scheme(ctx.out.get_scheme(ctx.whole())))
                        break;
                    [[fallthrough]];
                case '/':
                case '?':
                case '#':
                case '\0':
                    if (at_char_seen && ctx.out.empty()) {
                        ctx.status = stl::to_underlying(uri_status::host_missing);
                        return;
                    }
                    ctx.status |= stl::to_underlying(uri_status::valid_host);
                    return;
            }
        }
    }


    template <istl::String StringType = stl::string>
    struct basic_user_info {
        using string_type = stl::remove_cvref_t<StringType>;
        using char_type   = typename string_type::value_type;


        string_type username{};
        string_type password{};

        /**
         * Get the string in the encoded shape
         */
        constexpr void append_to(istl::String auto& out) const {
            if (!username.empty()) {
                encode_uri_component(username, out, details::USER_INFO_NOT_PCT_ENCODED<char_type>);
            }
            if (!password.empty()) {
                // out.reserve(password.size() + 1); // much better chance of removing one memory allocation
                out += '@';
                encode_uri_component(password, out, details::USER_INFO_NOT_PCT_ENCODED<char_type>);
            }
        }

        [[nodiscard]] constexpr string_type to_string() const {
            string_type res{username.get_allocator()};
            append_to(res);
            return res;
        }

        constexpr void append_raw_to(istl::String auto& out) const {
            const bool user_empty = !username.empty();
            const bool pass_empty = !password.empty();
            if (user_empty && pass_empty) {
                // out.reserve(out.size() + username.size() + password.size() + 1);
                out.append(username);
                out.push_back('@');
                out.append(password);
            } else if (pass_empty) {
                out.append(username);
            } else if (user_empty) {
                // out.reserve(out.size() + password.size() + 1);
                out.push_back('@');
                out.append(password);
            }
        }

        /**
         * Convert to string without encoding it
         */
        [[nodiscard]] constexpr string_type to_raw_string() const {
            string_type res{username.get_allocator()};
            append_raw_to(res);
            return res;
        }
    };


} // namespace webpp::uri

#endif // WEBPP_URI_AUTHORITY_HPP
