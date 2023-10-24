// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_SCHEME_HPP
#define WEBPP_URI_SCHEME_HPP

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../strings/charset.hpp"
#include "./details/special_schemes.hpp"
#include "./details/uri_components.hpp"
#include "./details/uri_status.hpp"

namespace webpp::uri {

    namespace details { // states


        template <typename... T>
        static constexpr void relative_state(uri::parsing_uri_context<T...>& ctx) noexcept {
            // relative scheme state (https://url.spec.whatwg.org/#relative-state)

            using ctx_type = uri::parsing_uri_context<T...>;

            if constexpr (ctx_type::has_base_uri) {
                // Assert base's scheme is not file
                assert(ctx.base.get_scheme(ctx.whole()) != "file");

                ctx.out.set_scheme(ctx.base.get_scheme(ctx.whole()));
            }
        }



        template <typename... T>
        static constexpr void file_host_state(uri::parsing_uri_context<T...>& ctx) noexcept(
          uri::parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#file-slash-state

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


        template <typename... T>
        static constexpr void file_slash_state(uri::parsing_uri_context<T...>& ctx) noexcept(
          uri::parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#file-slash-state

            using ctx_type = uri::parsing_uri_context<T...>;
            if (ctx.pos != ctx.end) {
                switch (*ctx.pos) {
                    case '\\':
                        ctx.status |= stl::to_underlying(uri_status::reverse_solidus_used);
                        [[fallthrough]];
                    case '/': file_host_state(ctx); return;
                }
            }
            if constexpr (ctx_type::has_base_uri) {
                if (ctx.base.get_scheme() == "file") {
                    ctx.out.set_scheme(ctx.base.get_scheme());

                    // todo:
                    // 2. If the code point substring from pointer to the end of input does not
                    //    start with a Windows drive letter and base's path[0] is a normalized
                    //    Windows drive letter, then append base's path[0] to url's path.
                    //    This is a (platform-independent) Windows drive letter quirk.
                }
            }
            ctx.status |= stl::to_underlying(uri_status::valid_path);
        }

        template <typename... T>
        static constexpr void
        file_state(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#file-state

            using ctx_type = uri::parsing_uri_context<T...>;

            if constexpr (ctx_type::has_base_uri) {
                // set scheme to "file"
                ctx.set_scheme(ctx.base.scheme.data(), ctx.base.data() + 4);
            }
            ctx.out.clear_host();

            // todo: check for end
            switch (*ctx.pos) {
                case '\\':
                    ctx.status |= stl::to_underlying(uri_status::reverse_solidus_used);
                    [[fallthrough]];
                case '/': file_slash_state(ctx); return;
            }

            if constexpr (ctx_type::has_base_uri) {
                if (ctx.base.get_scheme() == "file") {
                    // todo
                }
            }

            ctx.status |= stl::to_underlying(uri_status::valid_path);
        }


        template <typename... T>
        static constexpr void no_scheme_state(uri::parsing_uri_context<T...>& ctx) noexcept(
          uri::parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#no-scheme-state

            using ctx_type = uri::parsing_uri_context<T...>;

            if constexpr (ctx_type::has_base_uri) {
                if (ctx.base.has_opaque_path()) {
                    if (*ctx.pos == '#') {
                        ctx.out.set_scheme(ctx.base.get_scheme(ctx.whole()));
                        ctx.out.set_path(ctx.base.get_path(ctx.whole()));
                        ctx.out.set_query(ctx.base.get_query(ctx.whole()));
                        ctx.out.clear_fragment();
                        ctx.status |= stl::to_underlying(uri_status::valid_fragment);
                        return;
                    }
                } else if (ctx.base.get_scheme() != "file") {
                    relative_state(ctx);
                    return;
                } else {
                    file_state(ctx);
                    return;
                }
            }
            ctx.status = stl::to_underlying(uri_status::missing_scheme_non_relative_url);
        }


        template <typename... T>
        static constexpr void special_authority_slashes_state(uri::parsing_uri_context<T...>& ctx) noexcept {
            /// https://url.spec.whatwg.org/#special-authority-slashes-state

            if (ctx.end - ctx.pos >= 2 && (ctx.pos[0] == '/' && ctx.pos[1] == '/')) {
                ctx.pos += 2;
                special_authority_ignore_slashes_state(ctx);
                return;
            }
            ctx.status |= stl::to_underlying(uri_status::missing_following_solidus);
        }

        template <typename... T>
        static constexpr void
        special_authority_ignore_slashes_state(uri::parsing_uri_context<T...>& ctx) noexcept {
            // special authority ignore slashes state
            // (https://url.spec.whatwg.org/#special-authority-ignore-slashes-state)
            if (ctx.pos != ctx.end) {
                switch (*ctx.pos) {
                    case '\\':
                    case '/': ctx.status |= stl::to_underlying(uri_status::missing_following_solidus);
                }
                ++ctx.pos;

                // todo: set authority
                ctx.status |= stl::to_underlying(uri_status::valid_authority);
                return;
            }
            ctx.status |= stl::to_underlying(uri_status::missing_following_solidus);
        }

        template <typename... T>
        static constexpr void
        special_relative_or_authority_state(uri::parsing_uri_context<T...>& ctx) noexcept {

            // special authority slashes state
            // (https://url.spec.whatwg.org/#special-authority-slashes-state):
            if (ctx.end - ctx.pos >= 2 && (ctx.pos[0] == '/' && ctx.pos[1] == '/')) {
                ctx.pos += 2;
                special_authority_ignore_slashes_state(ctx);
                return;
            }
            ctx.status |= stl::to_underlying(uri_status::missing_following_solidus);
            relative_state(ctx);
        }

        template <typename... T>
        static constexpr void path_or_authority_state(uri::parsing_uri_context<T...>& ctx) noexcept {
            // https://url.spec.whatwg.org/#path-or-authority-state

            if (ctx.pos != ctx.end && *ctx.pos == '/') {
                ++ctx.pos;
                ctx.status |= stl::to_underlying(uri_status::valid_authority);
                return;
            }
            ctx.status |= stl::to_underlying(uri_status::valid_path);
        }


    } // namespace details

    /**
     * Parse scheme (or sometimes called Protocol)
     */
    template <typename... T>
    static constexpr void
    parse_scheme(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        using ctx_type  = uri::parsing_uri_context<T...>;
        using char_type = typename ctx_type::char_type;
        using enum uri_status;

        webpp_static_constexpr auto alnum_plus =
          charset(ALPHA_DIGIT<char_type>, charset<char_type, 3>{'+', '-', '.'});

        // scheme start (https://url.spec.whatwg.org/#scheme-start-state)
        if (ctx.pos == ctx.end) {
            ctx.status = stl::to_underlying(empty_string);
            return;
        }

        if (ALPHA<char_type>.contains(*ctx.pos)) {
            ++ctx.pos;

            // scheme state (https://url.spec.whatwg.org/#scheme-state)
            {
                // handling alpha, num, +, -, .
                for (;;) {
                    if (ctx.pos == ctx.end) [[unlikely]] {
                        ctx.status = stl::to_underlying(scheme_ended_unexpectedly);
                        return;
                    }
                    if (!alnum_plus.contains(*ctx.pos))
                        break;

                    ++ctx.pos;
                }

                // handling ":" character
                if (*ctx.pos == ':') {

                    // if constexpr (ctx_type::is_overridable) {
                    //     const auto url_scheme =
                    //       stl::basic_string_view<char_type>{ctx.pos,
                    //                                         static_cast<stl::size_t>(ctx.pos - ctx.beg)};
                    //     if (is_special_scheme(url_scheme) != is_special_scheme(ctx.out.scheme)) {
                    //         ctx.status = stl::to_underlying(incompatible_schemes);
                    //         return;
                    //     }


                    //     if (ctx.in.get_scheme(ctx.whole()) == "file") {
                    //         if (ctx.in.has_credentials() || ctx.out.has_port() || ctx.in.has_host()) {
                    //             ctx.status = stl::to_underlying(incompatible_schemes);
                    //             return;
                    //         }
                    //     }

                    //     ctx.out.clear_port();
                    // }

                    ctx.out.set_scheme(ctx.beg, ctx.pos);
                    ++ctx.pos;

                    if (ctx.out.get_scheme(ctx.whole()) == "file") {
                        // If remaining does not start with "//", special-scheme-missing-following-solidus
                        // validation error.
                        if (ctx.end - ctx.pos >= 2 && (ctx.pos[0] == '/' && ctx.pos[1] == '/')) [[likely]] {
                            ctx.pos += 2;
                        } else {
                            ctx.status |= stl::to_underlying(uri_status::missing_following_solidus);
                        }
                        details::file_state(ctx);
                        return;
                    } else if (is_special_scheme(ctx.out.get_scheme(ctx.whole()))) [[likely]] {
                        // todo: first check the constexpr if
                        if constexpr (ctx_type::has_base_uri) {
                            if (ctx.out.get_scheme(ctx.whole()) == ctx.base.get_scheme(ctx.whole())) {
                                // todo: Assert: base is special (and therefore does not have an opaque path).
                                details::special_relative_or_authority_state(ctx);
                            }
                        }
                    }

                    if (ctx.pos != ctx.end && *ctx.pos == '/') {
                        ++ctx.pos;
                        details::path_or_authority_state(ctx);
                        return;
                    } else {
                        ctx.out.clear_path();
                        ctx.status |= stl::to_underlying(uri_status::valid_opaque_path);
                        return;
                    }
                } else {
                    // Otherwise, if state override is not given, set buffer to the empty string, state to no
                    // scheme state, and start over (from the first code point in input).
                    //
                    // no scheme state (https://url.spec.whatwg.org/#no-scheme-state)
                    ctx.pos = ctx.beg;
                    ctx.out.clear_scheme();
                    details::no_scheme_state(ctx);
                    return;
                }
            }
        }
        ctx.status = stl::to_underlying(invalid_character);
    }




    /**
     * Scheme or Protocol
     * @tparam StringType
     */
    template <istl::StringLike StringType = stl::string>
    struct basic_scheme : StringType {
        using string_type = StringType;
        using char_type   = istl::char_type_of_t<string_type>;

        template <typename... T>
        constexpr basic_scheme(T&&... args) : string_type{stl::forward<T>(args)...} {}

        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return this->empty();
        }

        void append_to(istl::String auto& out) const {
            if (!this->empty()) {
                // out.reserve(out.size() + this->size() + 1);
                out.append(*this);
                out.push_back(':');
                out.append("//");
            } else {
                out.append("//");
            }
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_SCHEME_HPP
