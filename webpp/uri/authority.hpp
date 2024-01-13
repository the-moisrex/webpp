// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_AUTHORITY_HPP
#define WEBPP_URI_AUTHORITY_HPP

#include "../std/string.hpp"
#include "../std/utility.hpp"
#include "details/constants.hpp"
#include "details/host_ip.hpp"
#include "details/uri_components_encoding.hpp"
#include "details/uri_status.hpp"
#include "details/windows_drive_letter.hpp"
#include "encoding.hpp"
#include "host_authority.hpp"
#include "port.hpp"

/**
 * Attention: User infos in URIs are DEPRECATED in URI, but we SHOULD parse it
 *   https://httpwg.org/specs/rfc9110.html#http.userinfo
 */
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

                ctx.out.clear_username(); // todo: it's optimizable
                component_encoder<components::username, ctx_type> user_encoder{ctx};
                user_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
                  username_beg,
                  username_end,
                  USER_INFO_ENCODE_SET);

                // parse password
                if (password_token_pos != ctx.end) {
                    iterator const password_beg = password_token_pos + 1;
                    iterator const password_end = atsign_pos;

                    ctx.out.clear_password(); // todo: it's optimizable
                    component_encoder<components::password, ctx_type> pass_encoder{ctx};
                    pass_encoder.template encode_or_set<uri_encoding_policy::encode_chars>(
                      password_beg,
                      password_end,
                      USER_INFO_ENCODE_SET);
                }
            }
        }

        template <uri_parsing_options Options = uri_parsing_options{}, bool IsSpecial = true, typename... T>
        static constexpr void parse_authority_pieces(parsing_uri_context<T...>& ctx) noexcept(
          parsing_uri_context<T...>::is_nothrow) {
            using enum uri_status;
            using details::ascii_bitmap;
            using details::FORBIDDEN_DOMAIN_CODE_POINTS;
            using details::FORBIDDEN_HOST_CODE_POINTS;

            using ctx_type = parsing_uri_context<T...>;
            using iterator = typename ctx_type::iterator;

            webpp_static_constexpr auto forbidden_hosts = ascii_bitmap{
              ctx_type::is_segregated ? ascii_bitmap{FORBIDDEN_HOST_CODE_POINTS, '.'}
                                      : FORBIDDEN_HOST_CODE_POINTS,
              '%'
            };

            webpp_static_constexpr auto forbidden_domains = ascii_bitmap{FORBIDDEN_DOMAIN_CODE_POINTS, '.'};

            webpp_static_constexpr ascii_bitmap interesting_characters =
              IsSpecial ? forbidden_hosts : forbidden_domains;

            auto const authority_begin = ctx.pos;
            auto       host_begin      = authority_begin;
            iterator   colon_pos       = ctx.end; // start of password or port

            bool skip_last_char           = false;
            bool must_contain_credentials = false;

            component_encoder<components::host, ctx_type> coder(ctx);
            coder.start_segment();
            for (;;) {
                bool done; // NOLINT(*-init-variables)
                if constexpr (!IsSpecial) {
                    // for opaque hosts:
                    done = coder.template encode_or_validate<uri_encoding_policy::encode_chars>(
                      C0_CONTROL_ENCODE_SET,
                      interesting_characters);
                } else {
                    // for domain names:
                    // todo: domain to ascii (https://url.spec.whatwg.org/#concept-domain-to-ascii)
                    done = coder.template decode_or_validate<uri_encoding_policy::encode_chars>(
                      interesting_characters);
                }
                if (done) {
                    if constexpr (Options.empty_host_is_error && !IsSpecial) {
                        if (ctx.pos == authority_begin) {
                            set_error(ctx.status, host_missing);
                            return;
                        }
                    }
                    set_valid(ctx.status, valid);
                    break;
                }

                switch (*ctx.pos) {
                    case '[': // it's not in the beginning because of the credentials may come before it
                        if (!details::parse_host_ipv6(ctx)) {
                            return;
                        }
                        break;
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
                            if (get_value(ctx.status) == port_invalid) {
                                must_contain_credentials = true;
                                ctx.out.clear_port();
                                // it might be a "password" or it's invalid port
                                ctx.pos = pre_port_pos + 1;
                                continue;
                            }

                            coder.end_segment(coder.segment_begin(), pre_port_pos);
                            coder.set_value(host_begin, pre_port_pos);

                            if (pre_port_pos == host_begin) {
                                if constexpr (Options.empty_host_is_error && IsSpecial) {
                                    set_error(ctx.status, host_missing);
                                    return;
                                } else if (ctx.pos == ctx.end) {
                                    set_valid(ctx.status, valid);
                                }
                            }
                            return;
                        }
                        break;
                    }
                    case '\\':
                        if (!IsSpecial) {
                            // todo: check for non-specials
                            break;
                        }
                        [[fallthrough]];
                    case '/':
                        // escape if invalid port found
                        if (must_contain_credentials) {
                            return;
                        }
                        set_valid(ctx.status, valid_path);
                        break;
                    case '.':
                        // if constexpr (ctx_type::is_segregated) {
                        //     coder.end_segment();
                        //     coder.skip_separator();
                        //     coder.reset_begin();
                        //     coder.start_segment();
                        // } else {
                        //     coder.skip_separator();
                        // }
                        coder.next_segment();
                        continue;
                    case '?':
                        // escape if invalid port found
                        if (must_contain_credentials) {
                            return;
                        }
                        if constexpr (Options.parse_queries) {
                            skip_last_char = true;
                            set_valid(ctx.status, valid_queries);
                        } else {
                            set_warning(ctx.status, invalid_character);
                            coder.skip_separator();
                            continue;
                        }
                        break;
                    case '#':
                        // escape if invalid port found
                        if (must_contain_credentials) {
                            return;
                        }
                        if constexpr (Options.parse_fragment) {
                            skip_last_char = true;
                            set_valid(ctx.status, valid_fragment);
                        } else {
                            set_warning(ctx.status, invalid_character);
                            coder.skip_separator();
                            continue;
                        }
                        break;
                    case '%':
                        if constexpr (!IsSpecial) {
                            if (!coder.validate_percent_encode()) {
                                set_warning(ctx.status, invalid_character);
                            }
                            continue;
                        } else {
                            set_error(ctx.status, invalid_domain_code_point);
                            return;
                        }
                    case '@':
                        must_contain_credentials = false;
                        if constexpr (Options.parse_credentails) {
                            details::parse_credentials(ctx, authority_begin, colon_pos);
                            ++ctx.pos;
                            ctx.out.clear_hostname();
                            coder.reset_begin();
                            coder.start_segment();
                            host_begin = ctx.pos;
                            continue;
                        } else {
                            // todo: set an error
                            set_warning(ctx.status, has_credentials);
                            set_warning(ctx.status, invalid_character);
                            return;
                        }
                    [[unlikely]] case '\0':
                        // invalid port
                        if (must_contain_credentials) {
                            return;
                        }
                        if constexpr (Options.eof_is_valid) {
                            if constexpr (Options.empty_host_is_error) {
                                if (ctx.pos == host_begin) {
                                    set_error(ctx.status, host_missing);
                                    return;
                                }
                            }
                            set_valid(ctx.status, valid);
                            break;
                        }
                        [[fallthrough]];
                    default:
                        set_error(ctx.status,
                                  IsSpecial ? invalid_domain_code_point : invalid_host_code_point);
                        return;
                }
                if (must_contain_credentials) {
                    return;
                }
                if (ctx.pos == host_begin) {
                    if constexpr (Options.empty_host_is_error && IsSpecial) {
                        set_error(ctx.status, host_missing);
                        return;
                    } else if (ctx.pos == ctx.end) {
                        set_valid(ctx.status, valid);
                    }
                }
                break;
            }

            // Parse IPv4 (if it ends with ipv4 octet)
            if (details::is_possible_ends_with_ipv4<Options>(host_begin, ctx.pos - 1, ctx)) {
                // we don't need to initialize it to zero
                stl::array<stl::uint8_t, 4> ipv4_octets_data; // NOLINT(*-init)
                bool const                  should_continue =
                  details::parse_host_ipv4<Options>(host_begin, ctx.pos, ipv4_octets_data.data(), ctx);
                if (!should_continue) {
                    return;
                }
                if constexpr (ctx_type::is_modifiable) {
                    ctx.out.clear_hostname();
                    coder.start_segment();
                    ipv4{ipv4_octets_data}.to_string(coder.get_out_seg());
                    if (skip_last_char) {
                        ++ctx.pos;
                    }
                    return;
                }
            }

            coder.end_segment();
            coder.set_value();
            if (skip_last_char) {
                ++ctx.pos;
            }
        }

    } // namespace details

    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_file_host(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#file-host-state

        static constexpr auto parsing_options = [] constexpr noexcept {
            uri_parsing_options options = Options;
            options.parse_credentails   = false;
            options.empty_host_is_error = false;
            options.parse_port          = false;
            return options;
        }();
        details::parse_authority_pieces<parsing_options>(ctx);

        if (ctx.out.has_hostname()) {
            if (ctx.out.get_hostname() == "localhost") {
                ctx.out.clear_hostname();
            } else if (details::starts_with_windows_driver_letter(ctx.pos, ctx.end)) {
                set_warning(ctx.status, uri_status::windows_drive_letter_as_host);
            }
        }
    }

    /// Path start state (I like to call it authority end because it's more RFC like to
    /// say that, but WHATWG likes to call it "path start state")
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
     * @param ctx Parsing Context containing all the details of the URI and the state of
     * it
     */
    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_authority(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // We merged the host parser and authority parser to make it single-pass for most
        // use cases. https://url.spec.whatwg.org/#authority-state
        // https://url.spec.whatwg.org/#host-state

        using enum uri_status;


        if (ctx.pos == ctx.end) {
            if constexpr (Options.empty_host_is_error) {
                set_error(ctx.status, host_missing);
            } else {
                set_valid(ctx.status, valid);
            }
            return;
        }

        if (ctx.is_special && ctx.out.get_scheme() == "file") {
            // todo: should we set the status instead?
            parse_file_host(ctx);
            return;
        }

        // Handle missing host situation, and ipv6:
        // attention: since we have merge the authority and host parsing, it's possible to
        // have something like "http://username@:8080/" which the host is missing too
        switch (*ctx.pos) {
            case ':':
                if constexpr (!Options.parse_credentails) {
                    if constexpr (Options.empty_host_is_error) {
                        set_error(ctx.status, host_missing);
                    } else {
                        set_valid(ctx.status, valid);
                    }
                    return;
                }
                break;
            case '\0':
                if constexpr (Options.eof_is_valid) {
                    break;
                } else {
                    if constexpr (Options.empty_host_is_error) {
                        set_error(ctx.status, host_missing);
                    } else {
                        set_valid(ctx.status, valid);
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
                    if (ctx.is_special) {
                        set_error(ctx.status, host_missing);
                        return;
                    }
                    set_valid(ctx.status, valid);
                } else {
                    set_valid(ctx.status, valid);
                    return;
                }
                break;
            default: break;
        }

        if (!ctx.is_special) {
            details::parse_authority_pieces<Options, false>(ctx);
            return;
        }

        details::parse_authority_pieces<Options, true>(ctx);
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

#endif // WEBPP_URI_AUTHORITY_HPP
