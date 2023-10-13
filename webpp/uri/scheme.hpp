// Created by moisrex on 10/30/20.

#ifndef WEBPP_SCHEME_HPP
#define WEBPP_SCHEME_HPP

#include "../std/string.hpp"
#include "../strings/charset.hpp"
#include "./common/meta.hpp"
#include "./details/uri_status.hpp"
#include "./uri_components.hpp"

namespace webpp::uri {

    enum struct scheme_status {
#define webpp_def(status) status = stl::to_underlying(uri::uri_status::status)
        webpp_def(valid),                     // Valid scheme
        webpp_def(invalid_character),         // Invalid character found
        webpp_def(empty_string),              // The specified string is empty
        webpp_def(scheme_ended_unexpectedly), // Scheme ended in an unexpected way
        webpp_def(incompatible_schemes),      // The new scheme is not compatible with the
                                              // old one
        webpp_def(missing_following_solidus), // Missing '//' after 'file:'

#undef webpp_def
        no_scheme = stl::to_underlying(uri::uri_status::last) + 1, // no scheme is specified
    };

    [[nodiscard]] static constexpr bool is_valid(scheme_status status) noexcept {
        using enum scheme_status;
        return status == valid || status == no_scheme;
    }

    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(scheme_status status) noexcept {
        switch (status) {
            using enum scheme_status;
            case valid: return "Valid Scheme";
            case invalid_character: return "Invalid character found where there should be a scheme.";
            case empty_string: return "The specified string is empty and not a URI.";
            case no_scheme: return "This URL doesn't have a scheme.";
            case scheme_ended_unexpectedly:
                return "This URI doesn't seem to have enough information, "
                       "not even a qualified scheme.";
            case incompatible_schemes:
                return "The new URI Scheme is not compatible with the old one; "
                       "can't merge them.";
            case missing_following_solidus:
                return "The URI's scheme is not followed by \"//\"; "
                       "more information: https://url.spec.whatwg.org/#special-scheme-missing-following-solidus";
        }
        stl::unreachable();
    }

    /**
     * @return 0 if unknown, otherwise return the port
     */
    template <istl::StringView StrT>
    [[nodiscard]] constexpr stl::uint16_t known_port(StrT scheme) noexcept {
        // NOLINTBEGIN(*-avoid-magic-numbers)
        switch (scheme.size()) {
            case 2:
                if (scheme[0] == 'w' && scheme[1] == 's')
                    return 80u;
                break;
            case 3:
                if (scheme == "wss")
                    return 443u;
                else if (scheme == "ftp")
                    return 21;
                break;
            case 4:
                if (scheme == "http")
                    return 80u;
                break;
            case 5:
                if (scheme == "https")
                    return 443;
                break;
        }
        return 0u;
        // NOLINTEND(*-avoid-magic-numbers)
    }

    template <istl::StringView StrT>
    [[nodiscard]] constexpr bool is_known(StrT scheme) noexcept {
        return known_port(scheme) != 0u;
    }

    /**
     * Parse scheme (or sometimes called Protocol)
     */
    template <typename... T>
    [[nodiscard]] static constexpr scheme_status parse_scheme(
      uri::parsing_uri_components<T...>& ctx) noexcept(uri::parsing_uri_components<T...>::is_nothrow) {
        using out_type  = uri::parsing_uri_components<T...>;
        using char_type = typename out_type::char_type;
        using enum scheme_status;

        webpp_static_constexpr auto alnum_plus =
          charset(ALPHA_DIGIT<char_type>, charset<char_type, 3>{'+', '-', '.'});
        webpp_static_constexpr bool is_overridable = out_type::is_overridable;

        // scheme start (https://url.spec.whatwg.org/#scheme-start-state)
        if (ctx.pos == ctx.end)
            return empty_string;

        if (ALPHA<char_type>.contains(*ctx.pos)) {
            ++ctx.pos;

            // scheme state (https://url.spec.whatwg.org/#scheme-state)
            {
                // handling alpha, num, +, -, .
                for (;;) {
                    if (ctx.pos == ctx.end) [[unlikely]] {
                        return scheme_ended_unexpectedly;
                    }
                    if (!alnum_plus.contains(*ctx.pos))
                        break;

                    ++ctx.pos;
                }

                // handling ":" character
                if (*ctx.pos == ':') {

                    if constexpr (is_overridable) {
                        const auto url_scheme =
                          stl::basic_string_view<char_type>{ctx.pos,
                                                            static_cast<stl::size_t>(ctx.pos - ctx.beg)};
                        if (is_known(url_scheme) != is_known(ctx.out.scheme)) {
                            return incompatible_schemes;
                        }

                        if (ctx.in.scheme == "file" &&
                            (ctx.in.has_credentials() || ctx.out.has_port() || ctx.in.has_host())) {
                            return incompatible_schemes;
                        }

                        ctx.out.clear_port();
                    }

                    ctx.out.set_scheme(ctx.beg, ctx.pos);
                    ++ctx.pos;
                    return valid;
                }
                // else: invalid char
            }
        }
        return invalid_character;
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

#endif // WEBPP_SCHEME_HPP
