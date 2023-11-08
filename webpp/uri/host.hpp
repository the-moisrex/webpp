// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

#include "../ip/inet_pton.hpp"
#include "../memory/allocators.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "./details/special_schemes.hpp"
#include "./details/uri_components.hpp"

namespace webpp::uri {


    namespace details {

        template <typename... T>
        static constexpr void host_parsing_state(uri::parsing_uri_context<T...>& ctx) noexcept(
          uri::parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#host-parsing
        }
    } // namespace details

    /// Path start state (I like to call it authority end because it's more RFC like to say that,
    /// but WHATWG likes to call it "path start state")
    template <typename... T>
    static constexpr void parse_authority_end(uri::parsing_uri_context<T...>& ctx) noexcept(
      uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#path-start-state

        if (ctx.pos == ctx.end) {
            // todo: I'm guessing
            uri::set_valid(ctx.status, uri_status::valid);
            return;
        }
        if (is_special_scheme(ctx.out.scheme())) {
            switch (*ctx.pos) {
                case '\\': uri::set_warning(ctx.status, uri_status::reverse_solidus_used); [[fallthrough]];
                case '/': ++ctx.pos; break;
            }
        } else {
            switch (*ctx.pos) {
                case '?':
                    uri::set_valid(ctx.status, uri_status::valid_queries);
                    ++ctx.pos;
                    ctx.out.clear_queries();
                    return;
                case '#':
                    uri::set_valid(ctx.status, uri_status::valid_fragment);
                    ++ctx.pos;
                    ctx.out.clear_fragment();
                    return;
                default:
                    uri::set_valid(ctx.status, uri_status::valid_path);
                    ctx.out.clear_path();
                    return;
            }
        }
    }

    template <typename... T>
    static constexpr void parse_file_host(uri::parsing_uri_context<T...>& ctx) noexcept(
      uri::parsing_uri_context<T...>::is_nothrow) {
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
    parse_host(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#host-state
        // todo

        using details::ascii_bitmap;

        using ctx_type = uri::parsing_uri_context<T...>;

        if (ctx.pos == ctx.end) {
            uri::set_error(ctx.status, uri_status::host_missing);
            return;
        }

        auto const scheme     = ctx.out.scheme();
        const bool is_special = is_special_scheme(scheme);

        if (scheme == "file") {
            // todo: should we set the status instead?
            parse_file_host(ctx);
            return;
        }

        const auto end_of_host_chars = is_special
                                         ? ascii_bitmap{details::ALLOWED_CHARACTERS_IN_URI<char>}.except(
                                             ascii_bitmap{':', '\\', '\0', '/', '?', '#', '[', ']'})
                                         : ascii_bitmap{details::ALLOWED_CHARACTERS_IN_URI<char>}.except(
                                             ascii_bitmap{':', '\0', '/', '?', '#', '[', ']'});


        auto const beg = ctx.pos;

        // handle missing host situation:
        switch (*ctx.pos) {
            case '[': {
                ++ctx.pos;
                stl::array<stl::uint8_t, ipv6_byte_count> ipv6_bytes{};
                auto const ipv6_parsing_result = inet_pton6(ctx.pos, ctx.end, ipv6_bytes.data());
                switch (ipv6_parsing_result) {
                    case inet_pton6_status::valid:
                        uri::set_error(ctx.status, uri_status::ipv6_unclosed);
                        return;
                    case inet_pton6_status::valid_special:
                        if (*ctx.pos == ']') {
                            ++ctx.pos;
                            if (ctx.pos == ctx.end) {
                                uri::set_valid(ctx.status, uri_status::valid);
                                return;
                            }
                            switch (*ctx.pos) {
                                case ':': uri::set_valid(ctx.status, uri_status::valid_port); break;
                                case '/': uri::set_valid(ctx.status, uri_status::valid_path); break;
                                case '#': uri::set_valid(ctx.status, uri_status::valid_fragment); break;
                                case '?': uri::set_valid(ctx.status, uri_status::valid_queries); break;
                                default:
                                    uri::set_error(ctx.status, uri_status::ipv6_char_after_closing);
                                    return;
                            }
                            ++ctx.pos;
                            return;
                        }
                        uri::set_error(ctx.status, uri_status::ipv6_unclosed);
                        return;
                    default: uri::set_error(ctx.status, static_cast<uri_status>(ipv6_parsing_result)); return;
                }
                break;
            }
            case '\\':
            case '#':
            case '\0':
            case '/':
            case '?':
                if (is_special) {
                    break;
                }
                [[fallthrough]];
            case ':': uri::set_error(ctx.status, uri_status::host_missing); return;
        }

        bool inside_brackets = false;
        for (;;) {
            if constexpr (ctx_type::is_modifiable) {
                // changes ctx.pos
                if (decode_uri_component(ctx.pos, ctx.end, ctx.out.host_ref(), end_of_host_chars)) {
                    if (is_special && ctx.pos == beg) {
                        uri::set_error(ctx.status, uri_status::host_missing);
                        return;
                    }
                    uri::set_valid(ctx.status, uri_status::valid);
                }
            } else {
                ctx.pos = end_of_host_chars.find_first_not_in(ctx.pos, ctx.end);
            }

            switch (*ctx.pos) {
                case ':':
                    if (!inside_brackets) {
                        uri::set_valid(ctx.status, uri_status::valid_port);
                    }
                    break;
                case '#':
                    uri::set_valid(ctx.status, uri_status::valid_fragment);
                    break;
                [[unlikely]] case '\0':
                    if (is_special && ctx.pos == beg) {
                        uri::set_error(ctx.status, uri_status::host_missing);
                        return;
                    }
                    uri::set_valid(ctx.status, uri_status::valid);
                    break;
                case '[': { // not an ipv6 since this is not the first character
                    inside_brackets = true;
                    ++ctx.pos;
                    continue;
                }
                case ']': {
                    inside_brackets = false;
                    ++ctx.pos;
                    continue;
                }
                case '\\':
                case '/':
                case '?':
                    uri::set_valid(ctx.status, uri_status::valid_path);
                    break;
                [[unlikely]] default:
                    uri::set_warning(ctx.status, uri_status::invalid_character);
                    ++ctx.pos;
                    continue;
            }
            break;
        }

        if constexpr (!ctx_type::is_modifiable) {
            ctx.out.host(beg, ctx.pos);
        }
        ++ctx.pos;
    }


    template <istl::String StringType = stl::string>
    struct basic_host : stl::vector<stl::remove_cvref_t<StringType>,
                                    rebind_allocator<typename stl::remove_cvref_t<StringType>::allocator_type,
                                                     stl::remove_cvref_t<StringType>>> {
        using string_type = stl::remove_cvref_t<StringType>;
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
