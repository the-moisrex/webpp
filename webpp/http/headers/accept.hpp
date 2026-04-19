// Created by moisrex on 10/24/20.

#ifndef WEBPP_HEADERS_ACCEPT_HPP
#define WEBPP_HEADERS_ACCEPT_HPP

#include "../../http/protocol/http_limits.hpp"
#include "../../std/cstdint.hpp"
#include "../../std/optional.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/charset.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "./header_concepts.hpp"

#include <array>
#include <span>

namespace webpp::http {

    /**
     * @brief A single media range extracted from the Accept header
     */
    struct [[nodiscard]] accept_media_range {
        stl::string_view media_type;
        stl::string_view params;        // Raw parameters string excluding the media type
        float            weight = 1.0F; // q-value (0.0 to 1.0)
    };

    [[nodiscard]] constexpr bool is_wildcard(accept_media_range const range) noexcept {
        return range.media_type == "*/*";
    }

    /**
     * from: https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
     *
     *  The Accept request-header field can be used to specify certain media types which are
     *  acceptable for the response. Accept headers can be used to indicate that the request is
     *  specifically limited to a small set of desired types, as in the case of a
     *  request for an in-line image.
     *
     *        Accept         = "Accept" ":"
     *                         #( media-range [ accept-params ] )
     *
     *        media-range    = ( "* / *"                 ; without the spaces between the slash
     *                         | ( type "/" "*" )
     *                         | ( type "/" subtype )
     *                         ) *( ";" parameter )
     *        accept-params  = ";" "q" "=" qvalue *( accept-extension )
     *        accept-extension = ";" token [ "=" ( token | quoted-string ) ]
     *
     *     The asterisk "*" character is used to group media types into ranges, with "* / *" (without the
     *     spaces before and after the slash, added that because of C++ commenting style)
     *     indicating all media types and "type/ *" indicating all subtypes of that type.
     *     The media-range MAY include media type parameters that are applicable to that range.
     *
     *     Each media-range MAY be followed by one or more accept-params, beginning with the "q" parameter
     *     for indicating a relative quality factor. The first "q" parameter (if any) separates the
     *     media-range parameter(s) from the accept-params. Quality factors allow the user or user agent
     *     to indicate the relative degree of preference for that media-range, using the qvalue scale
     *     from 0 to 1 (section 3.9). The default value is q=1.
     *
     *     Note: Use of the "q" parameter name to separate media type
     *     parameters from Accept extension parameters is due to historical
     *       practice. Although this prevents any media type parameter named
     *     "q" from being used with a media range, such an event is believed
     *       to be unlikely given the lack of any "q" parameters in the IANA
     *     media type registry and the rare usage of any media type
     *     parameters in Accept. Future media types are discouraged from
     *     registering any parameter named "q".
     *
     * The example
     *
     *     Accept: audio/ *; q=0.2, audio/basic
     *
     * SHOULD be interpreted as "I prefer audio/basic, but send me any audio type if it is the best
     * available after an 80% mark-down in quality."
     *
     * If no Accept header field is present, then it is assumed that the client accepts all media types.
     * If an Accept header field is present, and if the server cannot send a response which is acceptable
     * according to the combined Accept field value, then the server SHOULD send a 406 (not acceptable)
     * response.
     *
     * A more elaborate example is
     *
     *        Accept: text/plain; q=0.5, text/html,
     *                text/x-dvi; q=0.8, text/x-c
     *
     * Verbally, this would be interpreted as "text/html and text/x-c are the preferred media types,
     * but if they do not exist, then send the text/x-dvi entity, and if that does not exist, send
     * the text/plain entity."
     *
     * Media ranges can be overridden by more specific media ranges or specific media types. If more
     * than one media range applies to a given type, the most specific reference has precedence. For example,
     *
     *        Accept: text/ *, text/html, text/html;level=1, * / *
     *
     *       have the following precedence:
     *
     *     1) text/html;level=1
     *     2) text/html
     *     3) text/ *
     *        4) * / *
     *
     *       The media type quality factor associated with a given type is determined by finding
     *       the media range with the highest precedence which matches that type. For example,
     *
     *       Accept: text/ *;q=0.3, text/html;q=0.7, text/html;level=1,
     *                text/html;level=2;q=0.4, * / *;q=0.5
     *
     *     would cause the following values to be associated:
     *
     *     text/html;level=1         = 1
     *     text/html                 = 0.7
     *     text/plain                = 0.3
     *
     *     image/jpeg                = 0.5
     *     text/html;level=2         = 0.4
     *     text/html;level=3         = 0.7
     *
     *     Note: A user agent might be provided with a default set of quality
     *       values for certain media ranges. However, unless the user agent is
     *       a closed system which cannot interact with other rendering agents,
     *     this default set ought to be configurable by the user.
     */
    struct [[nodiscard]] basic_accept : header_field_base<basic_accept> {
        static constexpr stl::string_view header_name = "accept";

        using value_type     = accept_media_range;
        using storage_type   = stl::array<value_type, max_supported_accept_values>;
        using const_iterator = storage_type::const_iterator;

      public:
        constexpr explicit basic_accept(stl::string_view const str) noexcept : header_field_base{str} {
            parse();
        }

        // An empty Accept header is valid and implies no explicit restriction.
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return _is_valid;
        }

        [[nodiscard]] constexpr stl::span<value_type const> media_ranges() const noexcept {
            return {_media_ranges.data(), _count};
        }

        [[nodiscard]] constexpr const_iterator begin() const noexcept {
            return _media_ranges.begin();
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept {
            return _media_ranges.begin() + _count;
        }

        /**
         * @brief Iterate over all media ranges in the Accept header.
         *
         * Extracts each media range, parses its q-value, and invokes the callback.
         * Return `false` from the callback to stop parsing early.
         */
        template <typename Callback>
        constexpr void for_each(Callback&& callback) const noexcept {
            if (!_is_valid) {
                return;
            }

            auto&& callback_ref = callback;
            for (auto const& range : media_ranges()) {
                // Reuse the same callback object across iterations, even for rvalues.
                if constexpr (stl::is_same_v<decltype(callback_ref(range)), bool>) {
                    if (!callback_ref(range)) {
                        break;
                    }
                } else {
                    callback_ref(range);
                }
            }
        }

      private:
        constexpr void parse() noexcept {
            _count    = 0;
            _is_valid = true;

            string_tokenizer<stl::string_view> tok{view()};
            while (!tok.at_end()) {
                tok.skip(charset{',', ' '});
                if (tok.at_end()) {
                    break;
                }

                stl::string_view range_str;
                if (tok.next(charset{','}, range_str)) {
                    range_str = ascii::trim_copy(range_str);
                } else {
                    range_str = ascii::trim_copy(stl::string_view{tok.token_begin(), view().end()});
                    tok.reset(view().end());
                }

                if (range_str.empty()) {
                    continue;
                }

                auto const range = parse_media_range(range_str);
                if (!range.has_value()) {
                    _count    = 0;
                    _is_valid = false;
                    return;
                }

                if (_count < _media_ranges.size()) {
                    _media_ranges[_count++] = *range;
                }
            }
        }

        [[nodiscard]] static constexpr stl::optional<accept_media_range> parse_media_range(
          stl::string_view const str) noexcept {
            accept_media_range res;

            string_tokenizer<stl::string_view> tok{str};

            // Extract the main media type (everything before the first ';')
            if (tok.next(charset{';'}, res.media_type)) {
                res.media_type = ascii::trim_copy(res.media_type);

                // Isolate the remaining parameters segment
                auto const semicolon_pos = str.find(';');
                if (semicolon_pos != stl::string_view::npos) {
                    res.params = ascii::trim_copy(str.substr(semicolon_pos + 1));
                }

                // Process parameters to locate the weight ("q" value)
                while (!tok.at_end()) {
                    tok.skip(charset{';', ' '});
                    if (tok.at_end()) {
                        break;
                    }

                    stl::string_view key;
                    if (tok.next(charset{'=', ';'}, key)) {
                        key = ascii::trim_copy(key);

                        if (tok.expect(charset{'='})) {
                            tok.skip(charset{' ', '\t'}); // skip OWS
                            if (tok.at_end()) {
                                return stl::nullopt;
                            }

                            stl::string_view value;
                            auto const*      value_start = tok.token_end();

                            if (*value_start == '"') {
                                auto const parsed = parse_quoted(value_start, str.end(), '"');
                                value             = parsed.value;
                                tok.reset(parsed.next, str.end());
                            } else if (tok.next(charset{';'}, value)) {
                                value = ascii::trim_copy(value);
                            } else {
                                value = ascii::trim_copy(stl::string_view{value_start, str.end()});
                                tok.reset(str.end());
                            }

                            if (ascii::iequals_sl(key, "q")) {
                                auto const parsed_weight = parse_qvalue(value);
                                if (parsed_weight < 0.0F) {
                                    return stl::nullopt;
                                }
                                res.weight = parsed_weight;
                            }
                        } else if (!key.empty()) {
                            return stl::nullopt;
                        }
                    } else {
                        return stl::nullopt;
                    }
                }
            } else {
                // No parameters, the entire token is the media type
                res.media_type = ascii::trim_copy(str);
            }

            if (!is_valid_media_type(res.media_type)) {
                return stl::nullopt;
            }

            return res;
        }

        [[nodiscard]] static constexpr bool is_valid_media_type(stl::string_view const media_type) noexcept {
            if (media_type.empty()) {
                return false;
            }

            auto const slash_pos = media_type.find('/');
            if (slash_pos == stl::string_view::npos || slash_pos == 0 || slash_pos + 1 >= media_type.size()) {
                return false;
            }

            auto const type    = media_type.substr(0, slash_pos);
            auto const subtype = media_type.substr(slash_pos + 1);
            if (type == "*" && subtype != "*") {
                return false;
            }
            if (type.find_first_of(" \t") != stl::string_view::npos ||
                subtype.find_first_of(" \t") != stl::string_view::npos) {
                return false;
            }
            return true;
        }

        /**
         * @brief Parses a quality value string into a float safely.
         *
         * Expects a string representing a float value up to 3 decimals, e.g., "0.9" or "1.000".
         */
        [[nodiscard]] static constexpr float parse_qvalue(stl::string_view str) noexcept {
            if (str.empty()) {
                return -1.0F;
            }

            if (str == "1") {
                return 1.0F;
            }

            if (str.starts_with('1')) {
                auto decimals = str.substr(1);
                if (decimals.empty()) {
                    return 1.0F;
                }
                if (!decimals.starts_with('.')) {
                    return -1.0F;
                }
                decimals.remove_prefix(1);
                if (decimals.size() > 3) {
                    return -1.0F;
                }
                for (auto const digit : decimals) {
                    if (digit != '0') {
                        return -1.0F;
                    }
                }
                return 1.0F;
            }

            if (str == "0") {
                return 0.0F;
            }

            if (str.starts_with("0.")) {
                float           quality      = 0.0F;
                constexpr float base_divisor = 10.0F;
                float           divisor      = base_divisor;
                auto const      decimals     = str.substr(2);

                if (decimals.size() > 3) {
                    return -1.0F;
                }

                for (auto const digit : decimals) {
                    if (digit < '0' || digit > '9') {
                        return -1.0F;
                    }
                    quality += static_cast<float>(digit - '0') / divisor;
                    divisor *= base_divisor;
                }
                return quality;
            }

            return -1.0F;
        }

        storage_type _media_ranges{};
        stl::uint8_t _count    = 0;
        bool         _is_valid = true;
    };

} // namespace webpp::http

#endif // WEBPP_HEADERS_ACCEPT_HPP
