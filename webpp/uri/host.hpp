// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

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
            ctx.status |= stl::to_underlying(uri_status::valid);
            return;
        }
        if (is_special_scheme(ctx.out.scheme())) {
            switch (*ctx.pos) {
                case '\\':
                    ctx.status |= stl::to_underlying(uri_status::reverse_solidus_used);
                    [[fallthrough]];
                case '/': ++ctx.pos; break;
            }
        } else {
            switch (*ctx.pos) {
                case '?':
                    ctx.status |= stl::to_underlying(uri_status::valid_queries);
                    ++ctx.pos;
                    ctx.out.clear_queries();
                    return;
                case '#':
                    ctx.status |= stl::to_underlying(uri_status::valid_fragment);
                    ++ctx.pos;
                    ctx.out.clear_fragment();
                    return;
                default:
                    ctx.status |= stl::to_underlying(uri_status::valid_path);
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


        if (ctx.pos == ctx.end) {
            ctx.status = stl::to_underlying(uri_status::host_missing);
            return;
        }

        auto const scheme     = ctx.out.scheme();
        const bool is_special = is_special_scheme(scheme);

        if (scheme == "file") {
            parse_file_host(ctx);
            return;
        }

        const auto invalid_charsets = is_special
                                        ? ascii_bitmap{details::ALLOWED_CHARACTERS_IN_URI<char>}.except(
                                            ascii_bitmap{':', '\\', '\0', '/', '?', '#', '['})
                                        : ascii_bitmap{details::ALLOWED_CHARACTERS_IN_URI<char>}.except(
                                            ascii_bitmap{':', '\0', '/', '?', '#', '['});


        auto const beg = ctx.pos;
        for (;;) {
            ctx.pos = invalid_charsets.find_first_in(ctx.pos, ctx.end);

            switch (*ctx.pos) {
                case ':':
                case '\\':
                case '\0':
                case '/':
                case '?':
                case '#':
                    if (is_special) {
                        ctx.status = stl::to_underlying(uri_status::host_missing);
                        return;
                    }
                    break;
                case '[':
                    // todo
                    break;
                [[unlikely]] default:
                    ctx.status |= stl::to_underlying(uri_status::invalid_character);
                    ++ctx.pos;
                    continue;
            }
            break;
        }
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
