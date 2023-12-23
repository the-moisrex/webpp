// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../ip/inet_pton.hpp"
#include "../std/string.hpp"
#include "../std/utility.hpp"
#include "details/constants.hpp"
#include "details/uri_components_encoding.hpp"
#include "details/uri_status.hpp"
#include "encoding.hpp"
#include "host_authority.hpp"
#include "port.hpp"

/**
 * Attention: User infos in URIs are DEPRECATED in URI, but we SHOULD parse it
 *   https://httpwg.org/specs/rfc9110.html#http.userinfo
 */
namespace webpp::uri {


    namespace details {


        /**
         * @brief Parse ipv6 of a host (starts with '[' and ends with ']')
         * @returns true if we need to continue parsing (has nothing to do with it being valid or not)
         */
        template <typename... T>
        static constexpr bool parse_host_ipv6(parsing_uri_context<T...>& ctx) noexcept(
          parsing_uri_context<T...>::is_nothrow) {
            webpp_assume(ctx.pos < ctx.end);

            auto const                                beg = ctx.pos;
            stl::array<stl::uint8_t, ipv6_byte_count> ipv6_bytes{};

            ++ctx.pos; // first char should be '[' now

            // todo: use context's output host for storing ipv6 bytes if the host supports it
            switch (auto const ipv6_parsing_result = inet_pton6(ctx.pos, ctx.end, ipv6_bytes.data(), ']')) {
                case inet_pton6_status::valid: set_error(ctx.status, uri_status::ipv6_unclosed); return false;
                case inet_pton6_status::valid_special:
                    if (*ctx.pos == ']') {
                        ++ctx.pos;
                        ctx.out.set_hostname(beg, ctx.pos);
                        if (ctx.pos == ctx.end) {
                            set_valid(ctx.status, uri_status::valid);
                            return false;
                        }
                        switch (*ctx.pos) {
                            case '/': set_valid(ctx.status, uri_status::valid_path); return false;
                            case ':': set_valid(ctx.status, uri_status::valid_port); break;
                            case '#': set_valid(ctx.status, uri_status::valid_fragment); break;
                            case '?': set_valid(ctx.status, uri_status::valid_queries); break;
                            default: set_error(ctx.status, uri_status::ipv6_char_after_closing); return false;
                        }
                        ++ctx.pos;
                        return false;
                    }
                    set_error(ctx.status, uri_status::ipv6_unclosed);
                    return false;
                default:
                    set_error(ctx.status,
                              static_cast<uri_status>(error_bit | stl::to_underlying(ipv6_parsing_result)));
                    return false;
            }
            return true;
        }

        template <typename... T>
        static constexpr void parse_opaque_host(parsing_uri_context<T...>& ctx) noexcept(
          parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#concept-opaque-host-parser

            webpp_assume(ctx.pos != ctx.end);

            // todo: opaque host also needs to check for credentials
            using ctx_type = parsing_uri_context<T...>;

            webpp_static_constexpr auto interesting_characters = ascii_bitmap{
              ctx_type::is_segregated ? ascii_bitmap{FORBIDDEN_HOST_CODE_POINTS, '.'}
                                      : FORBIDDEN_HOST_CODE_POINTS,
              '%'
            };

            switch (*ctx.pos) {
                case '[': {
                    if (!details::parse_host_ipv6(ctx)) {
                        return;
                    }
                    break;
                }
                default: break;
            }

            component_encoder<components::host, ctx_type> encoder(ctx);
            encoder.start_segment();
            for (;;) {
                if (encoder.template encode_or_validate<uri_encoding_policy::encode_chars>(
                      C0_CONTROL_ENCODE_SET,
                      interesting_characters))
                {
                    set_valid(ctx.status, uri_status::valid);
                    encoder.end_segment();
                    break;
                }

                switch (*ctx.pos) {
                    case '%': {
                        if (!encoder.validate_percent_encode()) {
                            set_warning(ctx.status, uri_status::invalid_character);
                        }
                        continue;
                    }
                    case '/':
                        encoder.end_segment();
                        set_valid(ctx.status, uri_status::valid_path);
                        break;
                    case '#':
                        set_valid(ctx.status, uri_status::valid_fragment);
                        encoder.end_segment();
                        encoder.set_value();
                        encoder.skip_separator();
                        return;
                    case '?':
                        set_valid(ctx.status, uri_status::valid_queries);
                        encoder.end_segment();
                        encoder.set_value();
                        encoder.skip_separator();
                        return;
                    case '.':
                        encoder.end_segment();
                        encoder.skip_separator();
                        continue;
                    [[unlikely]] default:
                        set_error(ctx.status, uri_status::invalid_host_code_point);
                        return;
                }
                break;
            }
            encoder.set_value();
        }

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
            using details::components;
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
                component_encoder<components::username, ctx_type> user_encoder{ctx};
                user_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
                  username_beg,
                  username_end,
                  USER_INFO_ENCODE_SET);

                // parse password
                if (password_token_pos != ctx.end) {
                    iterator const                                    password_beg = password_token_pos + 1;
                    iterator const                                    password_end = atsign_pos;
                    component_encoder<components::password, ctx_type> pass_encoder{ctx};
                    pass_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
                      password_beg,
                      password_end,
                      USER_INFO_ENCODE_SET);
                }
            }
        }

        template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
        static constexpr void parse_authority_pieces(parsing_uri_context<T...>& ctx) noexcept(
          parsing_uri_context<T...>::is_nothrow) {
            using enum uri_status;

            using ctx_type = parsing_uri_context<T...>;
            using iterator = typename ctx_type::iterator;

            webpp_static_constexpr auto interesting_characters =
              ascii_bitmap{FORBIDDEN_DOMAIN_CODE_POINTS, '.'};

            auto const authority_begin = ctx.pos;
            auto       host_begin      = authority_begin;
            iterator   colon_pos       = ctx.end; // start of password or port

            component_encoder<components::host, ctx_type> decoder(ctx);
            decoder.start_segment();
            for (;;) {
                // todo: domain to ascii (https://url.spec.whatwg.org/#concept-domain-to-ascii)
                if (decoder.template decode_or_validate<uri_encoding_policy::encode_chars>(
                      interesting_characters))
                {
                    if constexpr (Options.empty_host_is_error) {
                        if (ctx.pos == authority_begin) {
                            set_error(ctx.status, host_missing);
                            return;
                        }
                    }
                    set_valid(ctx.status, valid);
                    break;
                }

                switch (*ctx.pos) {
                    case '[': {
                        if (!details::parse_host_ipv6(ctx)) {
                            return;
                        }
                        break;
                    }
                    case ':': {
                        if constexpr (!Options.parse_credentails && !Options.parse_port) {
                            set_warning(ctx.status, invalid_character);
                            ++ctx.pos;
                            continue;
                        } else if constexpr (!Options.parse_credentails) {
                            set_valid(ctx.status, valid_port);
                        } else {
                            // the first colon is the start of the password section
                            if (colon_pos == ctx.end) {
                                colon_pos = ctx.pos;
                            }

                            // assume it's a port (even though it might be the start of the password)
                            auto const pre_port_pos = ctx.pos;
                            ++ctx.pos;
                            set_valid(ctx.status, valid_port);
                            parse_port(ctx);

                            // rollback if it's not a port, we rollback and assume it's a password
                            if (get_value(ctx.status) != valid_authority_end) {
                                // it might be a "password" or an "invalid port"
                                ctx.pos = pre_port_pos + 1;
                                continue;
                            }

                            decoder.end_segment(decoder.segment_begin(), pre_port_pos);
                            decoder.set_value(host_begin, pre_port_pos);
                            return;
                        }
                        break;
                    }
                    case '\\':
                        if (!ctx.is_special) {
                            // todo: check for non-specials
                            break;
                        }
                        [[fallthrough]];
                    case '/':
                        decoder.end_segment();
                        decoder.set_value();
                        set_valid(ctx.status, valid_path);
                        return;
                    case '.':
                        if constexpr (ctx_type::is_segregated) {
                            decoder.end_segment();
                            ++ctx.pos;
                            decoder.reset_begin();
                            decoder.start_segment();
                        } else {
                            decoder.skip_separator();
                        }
                        continue;
                    case '?':
                        if constexpr (Options.parse_queries) {
                            set_valid(ctx.status, valid_queries);
                        } else {
                            set_warning(ctx.status, invalid_character);
                        }
                        break;
                    case '#':
                        if constexpr (Options.parse_fragment) {
                            set_valid(ctx.status, valid_fragment);
                        } else {
                            set_warning(ctx.status, invalid_character);
                        }
                        break;
                    case '@':
                        if constexpr (Options.parse_credentails) {
                            details::parse_credentials(ctx, authority_begin, colon_pos);
                            ++ctx.pos;
                            ctx.out.clear_hostname();
                            decoder.reset_begin();
                            decoder.start_segment();
                            host_begin = ctx.pos;
                            continue;
                        } else {
                            // todo: set an error
                            set_warning(ctx.status, has_credentials);
                            set_warning(ctx.status, invalid_character);
                            return;
                        }
                    [[unlikely]] case '\0':
                        if constexpr (Options.eof_is_valid) {
                            if constexpr (Options.empty_host_is_error) {
                                if (ctx.pos == authority_begin) {
                                    set_error(ctx.status, host_missing);
                                    return;
                                }
                            }
                            set_valid(ctx.status, valid);
                            break;
                        }
                        [[fallthrough]];
                    [[unlikely]] default:
                        set_warning(ctx.status, invalid_character);
                        decoder.skip_separator();
                        continue;
                }
                if (ctx.pos == host_begin) {
                    if constexpr (Options.empty_host_is_error) {
                        set_error(ctx.status, host_missing);
                    } else {
                        set_valid(ctx.status, valid);
                    }
                    return;
                }
                break;
            }

            decoder.end_segment();
            decoder.set_value();
            if (ctx.pos != ctx.end) {
                ++ctx.pos;
            }
        }

    } // namespace details

    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_file_host(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#file-host-state

        details::parse_authority_pieces<uri_parsing_options{
          .eof_is_valid        = Options.eof_is_valid,
          .parse_credentails   = false,
          .empty_host_is_error = false,
          .parse_punycodes     = Options.parse_punycodes,
          .parse_port          = false,
          .parse_queries       = Options.parse_queries,
          .parse_fragment      = Options.parse_fragment,
        }>(ctx);

        if (ctx.out.has_hostname() && ctx.out.get_hostname() == "localhost") {
            ctx.out.clear_hostname();
        }
    }

    /// Path start state (I like to call it authority end because it's more RFC like to say that,
    /// but WHATWG likes to call it "path start state")
    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_authority_end(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#path-start-state

        if (ctx.pos == ctx.end) {
            // todo: I'm guessing
            set_valid(ctx.status, uri_status::valid);
            return;
        }
        if (ctx.is_special) {
            switch (*ctx.pos) {
                case '\\': set_warning(ctx.status, uri_status::reverse_solidus_used); [[fallthrough]];
                case '/':
                default: set_valid(ctx.status, uri_status::valid_path); break;
            }
        } else {
            switch (*ctx.pos) {
                case '?':
                    if constexpr (Options.parse_queries) {
                        set_valid(ctx.status, uri_status::valid_queries);
                        ++ctx.pos;
                        ctx.out.clear_queries();
                    } else {
                        set_warning(ctx.status, uri_status::invalid_character);
                    }
                    return;
                case '#':
                    if constexpr (Options.parse_fragment) {
                        set_valid(ctx.status, uri_status::valid_fragment);
                        ++ctx.pos;
                        ctx.out.clear_fragment();
                    } else {
                        set_warning(ctx.status, uri_status::invalid_character);
                    }
                    return;
                default:
                    set_valid(ctx.status, uri_status::valid_path);
                    ctx.out.clear_path();
                    break;
            }
        }
    }

    /**
     * @brief Parse authority part of the URI (credentials, host, and port)
     * @param ctx Parsing Context containing all the details of the URI and the state of it
     */
    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_authority(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // We merged the host parser and authority parser to make it single-pass for most use cases.
        // https://url.spec.whatwg.org/#authority-state
        // https://url.spec.whatwg.org/#host-state

        if (ctx.pos == ctx.end) {
            if constexpr (Options.empty_host_is_error) {
                set_error(ctx.status, uri_status::host_missing);
            } else {
                set_valid(ctx.status, uri_status::valid);
            }
            return;
        }

        if (ctx.is_special && ctx.out.get_scheme() == "file") {
            // todo: should we set the status instead?
            parse_file_host(ctx);
            return;
        }

        // Handle missing host situation, and ipv6:
        // attention: since we have merge the authority and host parsing, it's possible to have
        // something like "http://username@:8080/" which the host is missing too
        switch (*ctx.pos) {
            case ':':
                if constexpr (!Options.parse_credentails) {
                    if constexpr (Options.empty_host_is_error) {
                        set_error(ctx.status, uri_status::host_missing);
                    } else {
                        set_valid(ctx.status, uri_status::valid);
                    }
                    return;
                }
                break;
            case '\0':
                if constexpr (Options.eof_is_valid) {
                    break;
                } else {
                    if constexpr (Options.empty_host_is_error) {
                        set_error(ctx.status, uri_status::host_missing);
                    } else {
                        set_valid(ctx.status, uri_status::valid);
                    }
                    return;
                }
            case '?':
                if (!ctx.is_special) {
                    break;
                }
                [[fallthrough]];
            case '\\':
            case '/':
            case '#':
                if constexpr (Options.empty_host_is_error) {
                    set_error(ctx.status, uri_status::host_missing);
                } else {
                    set_valid(ctx.status, uri_status::valid);
                }
                return;
            default: break;
        }

        if (!ctx.is_special) {
            details::parse_opaque_host(ctx);
            return;
        }

        details::parse_authority_pieces<Options>(ctx);
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
