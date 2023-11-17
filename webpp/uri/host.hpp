// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

#include "../ip/inet_pton.hpp"
#include "../memory/allocators.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "./details/constants.hpp"
#include "./details/uri_components_encoding.hpp"

namespace webpp::uri {


    namespace details {


        template <typename... T>
        static constexpr void
        parse_opaque_host(parsing_uri_context<T...>& ctx) noexcept(parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#concept-opaque-host-parser

            using ctx_type = parsing_uri_context<T...>;

            webpp_static_constexpr auto interesting_characters =
              ctx_type::is_segregated ? ascii_bitmap{FORBIDDEN_HOST_CODE_POINTS, '.'}
                                      : FORBIDDEN_HOST_CODE_POINTS;

            component_encoder<components::host, ctx_type> encoder(ctx);
            encoder.start_segment();
            for (;;) {
                if (encoder.template encode_or_validate<uri_encoding_policy::encode_chars>(
                      C0_CONTROL_ENCODE_SET,
                      interesting_characters)) {
                    set_valid(ctx.status, uri_status::valid);
                    encoder.end_segment();
                    break;
                }

                switch (*ctx.pos) {
                    case '%': {
                        if (ctx.pos + 2 < ctx.end) {
                            if (validate_percent_encode(ctx.pos, ctx.end)) {
                                continue;
                            }
                            ++ctx.pos;
                        }
                        set_warning(ctx.status, uri_status::invalid_character);
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
                        ++ctx.pos;
                        return;
                    case '?':
                        set_valid(ctx.status, uri_status::valid_queries);
                        encoder.end_segment();
                        encoder.set_value();
                        ++ctx.pos;
                        return;
                    case '.':
                        encoder.end_segment();
                        ++ctx.pos;
                        continue;
                    [[unlikely]] default:
                        set_error(ctx.status, uri_status::invalid_host_code_point);
                        return;
                }
                break;
            }
            encoder.set_value();
        }

    } // namespace details

    /// Path start state (I like to call it authority end because it's more RFC like to say that,
    /// but WHATWG likes to call it "path start state")
    template <typename... T>
    static constexpr void
    parse_authority_end(parsing_uri_context<T...>& ctx) noexcept(parsing_uri_context<T...>::is_nothrow) {
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
                    set_valid(ctx.status, uri_status::valid_queries);
                    ++ctx.pos;
                    ctx.out.clear_queries();
                    return;
                case '#':
                    set_valid(ctx.status, uri_status::valid_fragment);
                    ++ctx.pos;
                    ctx.out.clear_fragment();
                    return;
                default:
                    set_valid(ctx.status, uri_status::valid_path);
                    ctx.out.clear_path();
                    return;
            }
        }
    }

    template <typename... T>
    static constexpr void
    parse_file_host(parsing_uri_context<T...>& ctx) noexcept(parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#file-host-state

        // todo
        if (ctx.pos != ctx.end) {
            switch (*ctx.pos) {
                case '\0':
                case '/':
                case '\\':
                case '?':
                case '#': break;
            }
        }
    }


    /// Parse the host port
    template <typename... T>
    static constexpr void
    parse_host(parsing_uri_context<T...>& ctx) noexcept(parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#host-state
        // todo

        using details::ascii_bitmap;

        using ctx_type = parsing_uri_context<T...>;

        if (ctx.pos == ctx.end) {
            set_error(ctx.status, uri_status::host_missing);
            return;
        }

        auto const scheme = ctx.out.get_scheme();

        if (scheme == "file") {
            // todo: should we set the status instead?
            parse_file_host(ctx);
            return;
        }

        webpp_static_constexpr auto end_of_host_chars =
          ctx_type::is_segregated ? ascii_bitmap{details::FORBIDDEN_DOMAIN_CODE_POINTS, '.'}
                                  : details::FORBIDDEN_DOMAIN_CODE_POINTS;



        auto const beg = ctx.pos;

        // handle missing host situation:
        switch (*ctx.pos) {
            case '[': {
                ++ctx.pos;
                stl::array<stl::uint8_t, ipv6_byte_count> ipv6_bytes{};
                switch (auto const ipv6_parsing_result =
                          inet_pton6(ctx.pos, ctx.end, ipv6_bytes.data(), ']')) {
                    case inet_pton6_status::valid: set_error(ctx.status, uri_status::ipv6_unclosed); return;
                    case inet_pton6_status::valid_special:
                        if (*ctx.pos == ']') {
                            ++ctx.pos;
                            ctx.out.set_host(beg, ctx.pos);
                            if (ctx.pos == ctx.end) {
                                set_valid(ctx.status, uri_status::valid);
                                return;
                            }
                            switch (*ctx.pos) {
                                case '/': set_valid(ctx.status, uri_status::valid_path); return;
                                case ':': set_valid(ctx.status, uri_status::valid_port); break;
                                case '#': set_valid(ctx.status, uri_status::valid_fragment); break;
                                case '?': set_valid(ctx.status, uri_status::valid_queries); break;
                                default: set_error(ctx.status, uri_status::ipv6_char_after_closing); return;
                            }
                            ++ctx.pos;
                            return;
                        }
                        set_error(ctx.status, uri_status::ipv6_unclosed);
                        return;
                    default:
                        set_error(
                          ctx.status,
                          static_cast<uri_status>(error_bit | stl::to_underlying(ipv6_parsing_result)));
                        return;
                }
                break;
            }
            case '\\':
            case '#':
            case '\0':
            case '/':
            case '?':
                if (!ctx.is_special) {
                    break;
                }
                [[fallthrough]];
            case ':': set_error(ctx.status, uri_status::host_missing); return;
            default: break;
        }

        if (!ctx.is_special) {
            details::parse_opaque_host(ctx);
            return;
        }

        details::component_encoder<details::components::host, ctx_type> decoder(ctx);
        decoder.start_segment();
        for (;;) {
            if (decoder.template decode_or_validate<uri_encoding_policy::encode_chars>(end_of_host_chars)) {
                if constexpr (!ctx_type::is_segregated && !ctx_type::is_modifiable) {
                    if (ctx.pos == beg) {
                        set_error(ctx.status, uri_status::host_missing);
                        return;
                    }
                    set_valid(ctx.status, uri_status::valid);
                    break;
                }
            }

            switch (*ctx.pos) {
                case ':': set_valid(ctx.status, uri_status::valid_port); break;
                case '#':
                    set_valid(ctx.status, uri_status::valid_fragment);
                    break;
                [[unlikely]] case '\0':
                    if (ctx.pos == beg) {
                        set_error(ctx.status, uri_status::host_missing);
                        return;
                    }
                    set_valid(ctx.status, uri_status::valid);
                    break;
                case '/':
                case '\\':
                    decoder.end_segment();
                    decoder.set_value();
                    set_valid(ctx.status, uri_status::valid_path);
                    return;
                case '?': set_valid(ctx.status, uri_status::valid_path); break;
                case '.':
                    if constexpr (ctx_type::is_segregated) {
                        decoder.end_segment();
                        ++ctx.pos;
                        decoder.reset_begin();
                        decoder.start_segment();
                    }
                    continue;
                [[unlikely]] default:
                    set_warning(ctx.status, uri_status::invalid_character);
                    ++ctx.pos;
                    continue;
            }
            break;
        }
        decoder.end_segment();
        decoder.set_value();
        if (ctx.pos != ctx.end) {
            ++ctx.pos;
        }
    }


    template <istl::String StringType = stl::string>
    struct basic_host
      : stl::vector<StringType, rebind_allocator<typename StringType::allocator_type, StringType>> {
        using string_type = StringType;
        using super =
          stl::vector<string_type, rebind_allocator<typename string_type::allocator_type, string_type>>;

        template <typename... T>
        explicit constexpr basic_host(T&&... args) : super{stl::forward<T>(args)...} {}


        template <istl::StringViewifiable StrT>
        constexpr basic_host& operator=(StrT&& inp_str) {
            const auto str =
              istl::stringify_of<string_type>(stl::forward<StrT>(inp_str), this->get_allocator());
            // todo: split it based on the domains
            this->push_back(str);
            return *this;
        }

        /**
         * Top Level Domain; sometimes called the extension
         */
        [[nodiscard]] constexpr auto tld() const {
            return this->back();
        }

        void append_to(istl::String auto& str) const {
            for (auto const& subdomain : *this) {
                str.append(subdomain);
            }
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_HOST_HPP
