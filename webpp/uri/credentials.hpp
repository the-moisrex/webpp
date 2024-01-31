// Created by moisrex on 1/23/24.

#ifndef WEBPP_URI_CREDENTIALS_HPP
#define WEBPP_URI_CREDENTIALS_HPP

#include "../std/string.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components_encoding.hpp"
#include "./details/uri_status.hpp"

namespace webpp::uri {

    namespace details {

        template <typename... T, typename Iter = typename parsing_uri_context<T...>::iterator>
        static constexpr void parse_credentials(
          parsing_uri_context<T...>& ctx,
          Iter                       beg,
          Iter                       password_token_pos) noexcept(parsing_uri_context<T...>::is_nothrow) {
            // todo: add "needs_encoding"
            // todo: See if there's a way to find the last atsign position instead of running this function for every atsign
            // todo: use already parsed host

            using details::ascii_bitmap;
            using details::component_encoder;
            using details::USER_INFO_ENCODE_SET;

            using ctx_type = parsing_uri_context<T...>;
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
                // out.reserve(password.size() + 1); // much better chance of removing one
                // memory allocation
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

#endif // WEBPP_URI_CREDENTIALS_HPP
