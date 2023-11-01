// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../std/string.hpp"
#include "../std/utility.hpp"
#include "details/constants.hpp"
#include "details/uri_components.hpp"
#include "details/uri_status.hpp"
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
                case '\\': {
                    if (!is_special_scheme(ctx.out.scheme())) {
                        break;
                    }
                    [[fallthrough]];
                }
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
