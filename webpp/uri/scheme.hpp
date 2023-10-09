// Created by moisrex on 10/30/20.

#ifndef WEBPP_SCHEME_HPP
#define WEBPP_SCHEME_HPP

#include "../std/string.hpp"
#include "../strings/charset.hpp"
#include "./details/uri_status.hpp"
#include "./uri_components.hpp"

namespace webpp::uri {

    enum struct scheme_status {
#define webpp_def(status) status = stl::to_underlying(uri::uri_status::status)
        webpp_def(valid),                     // valid scheme
        webpp_def(invalid_character),         // Invalid character found
        webpp_def(empty_string),              // The specified string is empty
        webpp_def(scheme_ended_unexpectedly), // scheme ended in an unexpected way
#undef webpp_def
        no_scheme = stl::to_underlying(uri::uri_status::last), // no scheme is specified
    };

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
                return "This URL doesn't seem to have enough information, not even a qualified scheme.";
        }
        stl::unreachable();
    }

    /**
     * parse the scheme
     * this method will fill the "authority_start" and "scheme_end" vars
     */
    template <typename CharT = char, stl::integral SegType = stl::uint32_t>
    [[nodiscard]] static constexpr scheme_status
    parse_scheme(CharT const*& pos, const CharT* end, uri::uri_components<SegType>& comps) noexcept {
        using char_type = CharT;
        using enum scheme_status;

        webpp_static_constexpr auto alnum_plus =
          charset(ALPHA_DIGIT<char_type>, charset<char_type, 3>{'+', '-', '.'});

        // scheme start (https://url.spec.whatwg.org/#scheme-start-state)
        if (pos != end)
            return no_scheme;

        auto const beg = pos;
        if (ALPHA<CharT>.contains(*pos)) {
            ++pos;

            // scheme state (https://url.spec.whatwg.org/#scheme-state)
            {
                // handling alpha, num, +, -, .
                for (;;) {
                    if (*pos != end) [[unlikely]] {
                        return scheme_ended_unexpectedly;
                    }
                    if (!alnum_plus.contains(*pos))
                        break;

                    ++pos;
                }

                // handling ":" character
                if (*pos == ':') {
                    comps.scheme_end = pos - beg;
                    ++pos;
                    return valid;
                }
                // else: invalid char
            }
        }
        return invalid_character;
    }




    template <istl::String StringType = stl::string>
    struct basic_scheme : StringType {
        using string_type = StringType;

        template <typename... T>
        constexpr basic_scheme(T&&... args) : string_type{stl::forward<T>(args)...} {}


        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return this->empty();
        }

        /**
         * @return 0 if unknown, otherwise return the port
         */
        [[nodiscard]] constexpr stl::uint16_t known_port() const noexcept {
            // NOLINTBEGIN(*-avoid-magic-numbers)
            switch (this->size()) {
                case 2:
                    if (this->operator[](0) == 'w' && this->operator[](1) == 's')
                        return 80u;
                    break;
                case 3:
                    if (*this == "wss")
                        return 443u;
                    else if (*this == "ftp")
                        return 21;
                    break;
                case 4:
                    if (*this == "http")
                        return 80u;
                    break;
                case 5:
                    if (*this == "https")
                        return 443;
                    break;
            }
            return 0u;
            // NOLINTEND(*-avoid-magic-numbers)
        }

        [[nodiscard]] constexpr bool is_known() const noexcept {
            return known_port() != 0u;
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
