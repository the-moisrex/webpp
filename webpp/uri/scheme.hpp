// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_SCHEME_HPP
#define WEBPP_URI_SCHEME_HPP

#include "../common/meta.hpp"
#include "../std/string.hpp"
#include "../std/string_like.hpp"
#include "../std/utility.hpp"
#include "../strings/charset.hpp"
#include "../strings/peek.hpp"
#include "./details/special_schemes.hpp"
#include "./details/uri_components.hpp"
#include "./details/uri_status.hpp"

#include <cassert>

namespace webpp::uri {

    namespace details { // states

        template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
        static constexpr void relative_state(parsing_uri_context<T...>& ctx) noexcept {
            // relative scheme state (https://url.spec.whatwg.org/#relative-state)

            using ctx_type = parsing_uri_context<T...>;
            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
                return;
            }

            if constexpr (ctx_type::has_base_uri) {
                // Assert base's scheme is not file
                assert(!is_file_scheme(ctx.base.scheme()));

                set_value<components::scheme>(ctx, ctx.base.get_scheme());
            }
            switch (*ctx.pos) {
                case '/': break;
                case '\\':
                    if (is_special_scheme(ctx.scheme)) {
                        set_warning(ctx.status, uri_status::reverse_solidus_used);
                    }
                    break;
                default: break;
            }
            ++ctx.pos;
            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
                return;
            }


            // from now on in the algorithms: realtive slash state
            // https://url.spec.whatwg.org/#relative-slash-state
            if constexpr (ctx_type::has_base_uri) {
                set_value<components::username>(ctx, ctx.base.get_username());
                set_value<components::password>(ctx, ctx.base.get_password());
                set_value<components::host>(ctx, ctx.base.get_hostname());
                set_value<components::port>(ctx, ctx.base.get_port());
                set_value<components::path>(
                  ctx,
                  ctx.base.get_path()); // todo: https://infra.spec.whatwg.org/#list-clone
                set_value<components::queries>(ctx, ctx.base.get_queries());
            }
            switch (*ctx.pos) {
                case '?':
                    clear<components::queries>(ctx);
                    set_valid(ctx.status, uri_status::valid_queries);
                    ++ctx.pos;
                    break;
                case '#':
                    clear<components::fragment>(ctx);
                    set_valid(ctx.status, uri_status::valid_fragment);
                    ++ctx.pos;
                    break;
                case '\0':
                    if constexpr (Options.eof_is_valid) {
                        set_valid(ctx.status, uri_status::valid);
                        return;
                    } else {
                        set_warning(ctx.status, uri_status::invalid_character);
                    }
                    [[fallthrough]];
                default:
                    clear<components::queries>(ctx);
                    // todo: https://url.spec.whatwg.org/#shorten-a-urls-path
                    set_valid(ctx.status, uri_status::valid_path);
                    break;
            }
        }

        template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
        static constexpr void file_slash_state(parsing_uri_context<T...>& ctx) noexcept(
          parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#file-slash-state

            using ctx_type = parsing_uri_context<T...>;
            if (ctx.pos != ctx.end) {
                switch (*ctx.pos) {
                    case '\\': set_warning(ctx.status, uri_status::reverse_solidus_used); [[fallthrough]];
                    case '/':
                        if constexpr (Options.allow_file_hosts) {
                            set_valid(ctx.status, uri_status::valid_file_host);
                        } else {
                            set_valid(ctx.status, uri_status::valid_path);
                        }
                        return;
                    default: break;
                }
            }
            if constexpr (ctx_type::has_base_uri) {
                if (is_file_scheme(ctx.base.get_scheme())) {
                    set_value<components::scheme>(ctx, ctx.base.get_scheme());

                    // todo:
                    // 2. If the code point substring from pointer to the end of input does not
                    //    start with a Windows drive letter and base's path[0] is a normalized
                    //    Windows drive letter, then append base's path[0] to url's path.
                    //    This is a (platform-independent) Windows drive letter quirk.
                }
            }
            set_valid(ctx.status, uri_status::valid_path);
        }

        template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
        static constexpr void file_state(parsing_uri_context<T...>& ctx) noexcept(
          parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#file-state

            using ctx_type = parsing_uri_context<T...>;

            if constexpr (ctx_type::has_base_uri) {
                // set scheme to "file"
                set_value<components::scheme>(ctx,
                                              ctx.base.get_scheme().data(),
                                              ctx.base.get_scheme().data() + 4);
            }
            clear<components::host>(ctx);

            if (ctx.pos == ctx.end) {
                set_valid(ctx.status, uri_status::valid);
                return;
            }

            switch (*ctx.pos) {
                case '\\': set_warning(ctx.status, uri_status::reverse_solidus_used); [[fallthrough]];
                case '/': file_slash_state<Options>(ctx); return;
                default:
                    if constexpr (Options.allow_file_hosts) {
                        set_valid(ctx.status, uri_status::valid_file_host);
                        return;
                    } else {
                        break;
                    }
            }

            if constexpr (ctx_type::has_base_uri) {
                if (is_file_scheme(ctx.base.get_scheme())) {
                    // todo
                }
            }

            set_valid(ctx.status, uri_status::valid_path);
        }

        template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
        static constexpr void no_scheme_state(parsing_uri_context<T...>& ctx) noexcept(
          parsing_uri_context<T...>::is_nothrow) {
            // https://url.spec.whatwg.org/#no-scheme-state

            using ctx_type = parsing_uri_context<T...>;

            if constexpr (ctx_type::has_base_uri) {
                if (ctx.base.has_path()) { // todo: specs say opaque path
                    if (*ctx.pos == '#') {
                        set_value<components::scheme>(ctx, ctx.base.get_scheme());
                        set_value<components::path>(ctx, ctx.base.get_path());
                        set_value<components::queries>(ctx, ctx.base.get_queries());
                        clear<components::fragment>(ctx);
                        set_valid(ctx.status, uri_status::valid_fragment);
                        return;
                    }
                } else if (is_file_scheme(ctx.base.get_scheme())) {
                    relative_state<Options>(ctx);
                    return;
                } else {
                    file_state<Options>(ctx);
                    return;
                }
            }
            set_error(ctx.status, uri_status::missing_scheme_non_relative_url);
        }

        template <typename... T>
        static constexpr void special_authority_ignore_slashes_state(
          parsing_uri_context<T...>& ctx) noexcept {
            // special authority ignore slashes state
            // (https://url.spec.whatwg.org/#special-authority-ignore-slashes-state)
            while (ascii::inc_if_any(ctx.pos, ctx.end, '\\', '/')) {
                set_warning(ctx.status, uri_status::missing_following_solidus);
            }
            set_valid(ctx.status, uri_status::valid_authority);
        }

        template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
        static constexpr void special_relative_or_authority_state(parsing_uri_context<T...>& ctx) noexcept {
            // special authority slashes state
            // (https://url.spec.whatwg.org/#special-authority-slashes-state):
            if (ascii::inc_if(ctx.pos, ctx.end, '/', '/')) {
                special_authority_ignore_slashes_state(ctx);
                return;
            }
            set_warning(ctx.status, uri_status::missing_following_solidus);
            relative_state<Options>(ctx);
        }

    } // namespace details

    /**
     * Parse scheme (or sometimes called Protocol)
     */
    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_scheme(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        using ctx_type  = parsing_uri_context<T...>;
        using char_type = typename ctx_type::char_type;
        using enum uri_status;

        webpp_static_constexpr auto alnum_plus =
          details::ascii_bitmap(details::ascii_bitmap{ALPHA_DIGIT<char_type>}, '+', '-', '.');

        // scheme start (https://url.spec.whatwg.org/#scheme-start-state)
        if (ctx.pos == ctx.end) [[unlikely]] {
            ctx.status = stl::to_underlying(empty_string);
            return;
        }

        if (!details::ASCII_ALPHA.contains(*ctx.pos)) [[unlikely]] {
            // Otherwise, if state override is not given, set buffer to the empty string, state to no
            // scheme state, and start over (from the first code point in input).
            //
            // no scheme state (https://url.spec.whatwg.org/#no-scheme-state)
            ctx.pos = ctx.beg;
            clear<components::scheme>(ctx);
            details::no_scheme_state<Options>(ctx);
            return;
        }

        ++ctx.pos;

        // scheme state (https://url.spec.whatwg.org/#scheme-state)
        // handling alpha, num, +, -, .
        ctx.pos = alnum_plus.find_first_not_in(ctx.pos, ctx.end);
        if (ctx.pos == ctx.end) [[unlikely]] {
            ctx.status = stl::to_underlying(scheme_ended_unexpectedly);
            return;
        }

        // handling ":" character
        if (*ctx.pos == ':') [[likely]] {
            ctx.out.set_lowered_scheme(ctx.beg, ctx.pos);
            ++ctx.pos;

            if (is_file_scheme(get_output_value<components::scheme>(ctx))) [[unlikely]] {
                ctx.scheme = scheme_type::file;
                // If remaining does not start with "//", special-scheme-missing-following-solidus
                // validation error.
                if (!ascii::inc_if(ctx.pos, ctx.end, '/', '/')) [[unlikely]] {
                    set_warning(ctx.status, missing_following_solidus);
                }
                details::file_state<Options>(ctx);
                return;
            }
            if (is_special_scheme(ctx.out.get_scheme())) [[likely]] {
                ctx.scheme = scheme_type::special_scheme;
                // todo: first check the constexpr if
                if constexpr (ctx_type::has_base_uri) {
                    if (ctx.out.get_scheme() == ctx.base.get_scheme()) {
                        // todo: Assert: base is special (and therefore does not have an opaque path).
                        details::special_relative_or_authority_state<Options>(ctx);
                    }
                }

                /// https://url.spec.whatwg.org/#special-authority-slashes-state
                if (!ascii::inc_if(ctx.pos, ctx.end, '/', '/')) [[unlikely]] {
                    set_warning(ctx.status, missing_following_solidus);
                }
                details::special_authority_ignore_slashes_state(ctx);
                return;
            }

            if (ascii::inc_if(ctx.pos, ctx.end, '/')) {
                // https://url.spec.whatwg.org/#path-or-authority-state
                if (ascii::inc_if(ctx.pos, ctx.end, '/')) [[likely]] {
                    set_valid(ctx.status, valid_authority);
                    return;
                }
                set_valid(ctx.status, valid_path);
                return;
            }

            clear<components::path>(ctx);
            set_valid(ctx.status, valid_opaque_path);
        } else {
            set_error(ctx.status, invalid_scheme_character);
        }
    }

    /**
     * Scheme or Protocol
     * @tparam StringType
     */
    template <istl::StringLike StringType = stl::string>
    struct basic_scheme : StringType {
        using string_type = StringType;
        using char_type   = istl::char_type_of_t<string_type>;
        using iterator    = typename string_type::iterator;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;

        using StringType::StringType;
        using StringType::operator=;

        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return this->empty();
        }

        /**
         * @brief Replace the value with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        constexpr void set_raw_value(iterator beg, iterator end) noexcept(!is_modifiable) {
            istl::assign(static_cast<string_type&>(*this), beg, end);
        }

        template <istl::StringView StrVT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr StrVT view() const noexcept {
            return StrVT{this->data(), this->size()};
        }

        [[nodiscard]] constexpr bool is_special() const noexcept {
            return is_special_scheme(this->view());
        }

        /**
         * @brief check if we have value
         * @return true if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !this->empty();
        }

        template <istl::StringLike NStrT = stl::string_view>
        constexpr void to_string(NStrT& out, bool const append_separators = false) const
          noexcept(!istl::ModifiableString<NStrT>) {
            // out.reserve(out.size() + this->size() + 1);
            istl::append(out, static_cast<string_type const&>(*this));
            if constexpr (istl::ModifiableString<NStrT>) {
                if (append_separators) {
                    if (!this->empty()) {
                        out.push_back(':');
                        out.append("//");
                    } else {
                        out.append("//");
                    }
                }
            }
        }

        template <istl::StringLike NStrT = stl::string_view, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const
          noexcept(!istl::ModifiableString<NStrT>) {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_SCHEME_HPP
