// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../std/string.hpp"
#include "../std/utility.hpp"
#include "details/constants.hpp"
#include "details/special_schemes.hpp"
#include "details/uri_components.hpp"
#include "details/uri_status.hpp"
#include "encoding.hpp"

/**
 * Attention: User infos in URIs are DEPRECATED in URI, but we SHOULD parse it
 *   https://httpwg.org/specs/rfc9110.html#http.userinfo
 */
namespace webpp::uri {


    template <typename... T>
    static constexpr void parse_authority(uri::parsing_uri_context<T...>& ctx) noexcept(
      uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#authority-state

        using ctx_type = uri::parsing_uri_context<T...>;
        using iterator = typename ctx_type::iterator;

        const auto interesting_characters = ctx.is_special
                                              ? details::ascii_bitmap{'@', ':', '/', '?', '#', '\0', '\\'}
                                              : details::ascii_bitmap{'@', ':', '/', '?', '#', '\0'};
        iterator   atsign_pos             = ctx.end;
        iterator   password_token_pos     = ctx.end;
        auto const beg                    = ctx.pos;
        for (;; ++ctx.pos) {
            ctx.pos = interesting_characters.find_first_in(ctx.pos, ctx.end);
            if (ctx.pos == ctx.end) {
                break;
            }
            switch (*ctx.pos) {
                case '@': {
                    uri::set_warning(ctx.status, uri_status::has_credentials);
                    atsign_pos = ctx.pos;

                    // append to the username and password
                    iterator const username_beg = beg;
                    iterator const username_end = stl::min(password_token_pos, atsign_pos);
                    iterator const password_beg = password_token_pos + 1;
                    iterator const password_end = atsign_pos;
                    if constexpr (ctx_type::is_modifiable) {
                        if (atsign_pos != ctx.end) {
                            encode_uri_component(username_beg,
                                                 username_end,
                                                 ctx.out.username_ref(),
                                                 details::USER_INFO_ENCODE_SET);

                            if (password_token_pos != ctx.end) {
                                encode_uri_component(password_beg,
                                                     password_end,
                                                     ctx.out.password_ref(),
                                                     details::USER_INFO_ENCODE_SET);
                            }
                        }
                    } else {
                        if (atsign_pos != ctx.end) {
                            ctx.out.username(username_beg, username_end);

                            if (password_token_pos != ctx.end) {
                                ctx.out.password(password_beg, password_end);
                            }
                        }
                    }

                    break;
                }
                case ':':
                    if (password_token_pos == ctx.end) {
                        password_token_pos = ctx.pos;
                        continue;
                    }
                    break; // append the ":" to the username/password
                case '/':
                case '?':
                case '#':
                case '\\': // the check has been done before, we don't need to check to see if the URL has a
                           // special scheme here (as it said by WHATWG)
                case '\0':
                    if (atsign_pos != ctx.end && ctx.pos == beg) {
                        uri::set_error(ctx.status, uri_status::host_missing);
                        return;
                    }
                    // There was no username and password
                    // todo: we could do this in one pass instead of going back here
                    ctx.pos = atsign_pos == ctx.end ? beg : atsign_pos + 1;
                    uri::set_valid(ctx.status, uri_status::valid_host);
                    return;
            }
        }
    }


    template <istl::String StringType = stl::string>
    struct basic_username : StringType {
        using string_type = StringType;
        using char_type   = typename string_type::value_type;

        using StringType::StringType;
        using StringType::operator=;

        [[nodiscard]] constexpr string_type const& as_string() const noexcept {
            return static_cast<string_type const&>(*this);
        }

        [[nodiscard]] constexpr string_type& as_string() noexcept {
            return static_cast<string_type&>(*this);
        }

        /**
         * Get the string in the encoded shape
         */
        constexpr void append_to(istl::String auto& out) const {
            if (!this->empty()) {
                encode_uri_component(as_string(), out, details::USER_INFO_NOT_PCT_ENCODED<char_type>);
            }
        }

        [[nodiscard]] constexpr string_type to_string() const {
            string_type res{this->get_allocator()};
            append_to(res);
            return res;
        }

        constexpr void append_raw_to(istl::String auto& out) const {
            if (!this->empty()) {
                out.append(as_string());
            }
        }

        /**
         * Convert to string without encoding it
         */
        [[nodiscard]] constexpr string_type to_raw_string() const {
            string_type res{this->get_allocator()};
            append_raw_to(res);
            return res;
        }
    };

    template <istl::String StringType = stl::string>
    struct basic_password : StringType {
        using string_type = StringType;
        using char_type   = typename string_type::value_type;

        using StringType::StringType;
        using StringType::operator=;

        [[nodiscard]] constexpr string_type const& as_string() const noexcept {
            return static_cast<string_type const&>(*this);
        }

        [[nodiscard]] constexpr string_type& as_string() noexcept {
            return static_cast<string_type&>(*this);
        }


        /**
         * Get the string in the encoded shape
         */
        constexpr void append_to(istl::String auto& out) const {
            if (!this->empty()) {
                // out.reserve(password.size() + 1); // much better chance of removing one memory allocation
                out += '@';
                encode_uri_component(as_string(), out, details::USER_INFO_NOT_PCT_ENCODED<char_type>);
            }
        }

        [[nodiscard]] constexpr string_type to_string() const {
            string_type res{this->get_allocator()};
            append_to(res);
            return res;
        }

        constexpr void append_raw_to(istl::String auto& out) const {
            if (!this->empty()) {
                // out.reserve(out.size() + password.size() + 1);
                out.push_back('@');
                out.append(as_string());
            }
        }

        /**
         * Convert to string without encoding it
         */
        [[nodiscard]] constexpr string_type to_raw_string() const {
            string_type res{this->get_allocator()};
            append_raw_to(res);
            return res;
        }
    };


} // namespace webpp::uri

#endif // WEBPP_URI_AUTHORITY_HPP
