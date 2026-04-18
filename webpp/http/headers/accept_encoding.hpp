// Created by moisrex on 10/7/20.

#ifndef WEBPP_ACCEPT_ENCODING_HPP
#define WEBPP_ACCEPT_ENCODING_HPP

#include "../../http/codec/common.hpp"
#include "../../std/cstdint.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/to_case.hpp"
#include "../../strings/validators.hpp"
#include "../protocol/http_limits.hpp"
#include "./header_concepts.hpp"

#include <array>
#include <cstdint>

namespace webpp::http {


    /**
     * Known encoding types for the Accept-Encoding header.
     * `unknown` is used for any encoding not explicitly listed.
     */
    enum struct [[nodiscard]] encoding_type : stl::uint8_t {
        unknown  = 0, // Unknown or custom encoding.
        all      = 1, // Matches any content encoding not already listed (equivalent to "*").
        identity = 2, // Indicates no compression or modification; always acceptable even if not present.
        gzip     = 3, // Lempel‑Ziv coding (LZ77) with 32‑bit CRC.
        compress = 4, // Lempel‑Ziv‑Welch (LZW) algorithm.
        deflate  = 5, // zlib structure with deflate compression.
        br       = 6, // Brotli algorithm.
    };

    /**
     * Returns the canonical string representation of an encoding type.
     */
    [[nodiscard]] constexpr stl::string_view to_string(encoding_type const enc) noexcept {
        using enum encoding_type;
        switch (enc) {
            case identity: return {"identity"};
            case gzip: return {"gzip"};
            case compress: return {"compress"};
            case deflate: return {"deflate"};
            case br: return {"br"};
            case all: return {"*"};
            default: break;
        }
        return {};
    }

    /**
     * RFC:      https://tools.ietf.org/html/rfc7231#section-5.3.4
     * MDN Docs: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Accept-Encoding
     * Wiki:     https://en.wikipedia.org/wiki/HTTP_compression
     *
     * Syntax:
     *   Accept-Encoding: gzip
     *   Accept-Encoding: compress
     *   Accept-Encoding: deflate
     *   Accept-Encoding: br
     *   Accept-Encoding: identity
     *   Accept-Encoding: *
     *
     * Multiple algorithms, weighted with the quality value syntax:
     *   Accept-Encoding: deflate, gzip;q=1.0, *;q=0.5
     *
     * todo: add support for pack200-gzip, exi, zstd
     *
     * @tparam EncodingEnum   The enumeration type that defines all known encoding values.
     *                        Must contain at least the values of `encoding_type`.
     * @tparam MaxSupported   Maximum number of encoding entries that will be parsed.
     *                        Additional entries beyond this limit are ignored.
     */
    template <typename EncodingEnum = encoding_type, std::size_t MaxSupported = max_supported_accept_encodings>
    struct [[nodiscard]] basic_accept_encoding : header_field_base<basic_accept_encoding<EncodingEnum, MaxSupported>> {
        static constexpr stl::string_view header_name = "accept-encoding";

        using encoding_types = EncodingEnum;

        static_assert(
          requires {
              EncodingEnum::unknown;
              EncodingEnum::all;
              EncodingEnum::identity;
          },
          "It must have these fields at least.");

        /**
         * A single compression algorithm entry with its quality value.
         */
        struct [[nodiscard]] compression_algo {
            // known type, or EncodingEnum::unknown for custom strings
            EncodingEnum encoding;

            // between 0 and 1, default 1.0
            // up to three decimal digits (but 1 or 2 is the max for some browsers)
            // https://developer.mozilla.org/en-US/docs/Glossary/Quality_values
            float quality = 1.0F;

            // only used when encoding == EncodingEnum::unknown
            stl::string_view name;
        };

        /**
         * Constructor from the raw header value.
         */
        explicit constexpr basic_accept_encoding(stl::string_view const src) noexcept
          : header_field_base<basic_accept_encoding>(src) {
            parse();
        }


      private:
        constexpr void parse() noexcept {
            _count = 0; // no valid, so far
            if (this->view().find_first_of('\"') != stl::string_view::npos) {
                return;
            }

            string_tokenizer<stl::string_view> tokenizer(this->view());
            while (_count < MaxSupported && tokenizer.next(charset{','})) {
                auto entry = tokenizer.token();
                http::trim_lws(entry);
                std::size_t const semicolon_pos = entry.find(';');
                if (semicolon_pos == stl::string_view::npos) {
                    if (entry.find_first_of(http::http_lws.string_view()) != stl::string_view::npos) {
                        _count = 0; // not valid
                        return;
                    }
                    _allowed_encodings[_count++] =
                      compression_algo{.encoding = to_known_encoding(entry), .quality = 1.0F, .name = entry};
                    continue;
                }
                auto encoding = entry.substr(0, semicolon_pos);
                http::trim_lws(encoding);
                if (encoding.find_first_of(http::http_lws.string_view()) != stl::string_view::npos) {
                    _count = 0; // not valid
                    return;
                }
                auto params = entry.substr(semicolon_pos + 1);
                http::trim_lws(params);
                std::size_t const equals_pos = params.find('=');
                if (equals_pos == stl::string_view::npos) {
                    _count = 0; // not valid
                    return;
                }
                auto param_name = params.substr(0, equals_pos);
                http::trim_lws(param_name);
                if (!ascii::iequals_sl(param_name, 'q')) { // size is checked inside iequals
                    _count = 0;                            // not valid
                    return;
                }
                auto qvalue = params.substr(equals_pos + 1);
                http::trim_lws(qvalue);
                if (qvalue.empty()) {
                    _count = 0; // not valid
                    return;
                }
                float qval = parse_qvalue(qvalue);
                if (qval < 0.0F) {
                    _count = 0; // not valid
                    return;
                }
                if (qval >= 0.0F) {
                    auto known = to_known_encoding(encoding);
                    _allowed_encodings[_count++] =
                      compression_algo{.encoding = known, .quality = qval, .name = encoding};
                }
            }

            // RFC 7231 5.3.4: if no encodings are listed, treat as "*" (all) with quality 1.0.
            if (_count == 0) {
                _allowed_encodings[_count++] =
                  compression_algo{.encoding = EncodingEnum::all,
                                   .quality  = 1.0F,
                                   .name     = to_string(EncodingEnum::all)};
                return;
            }

            // Ensure identity is always present (browsers must support it).
            bool has_identity = false;
            for (std::size_t i = 0; i < _count; ++i) {
                if (_allowed_encodings[i].encoding == EncodingEnum::identity) {
                    has_identity = true;
                    break;
                }
            }
            if (!has_identity && _count < MaxSupported) {
                _allowed_encodings[_count++] =
                  compression_algo{.encoding = EncodingEnum::identity,
                                   .quality  = 1.0F,
                                   .name     = to_string(EncodingEnum::identity)};
            }

            // RFC says gzip == x-gzip, compress == x-compress. We treat them as the same enum value,
            // so no extra mirroring is needed.
        }

        /**
         * Parses a quality value string like "0.5" or "1" or "1.000".
         * Returns the parsed float, or -1.0f on error.
         */
        static constexpr float parse_qvalue(std::string_view qvalue) noexcept {
            if (qvalue.empty()) {
                return -1.0F;
            }
            if (qvalue[0] == '1') {
                if (qvalue == "1" || qvalue == "1.0" || qvalue == "1.00" || qvalue == "1.000") {
                    return 1.0F;
                }
                return -1.0F;
            }
            if (qvalue[0] != '0') {
                return -1.0F;
            }
            if (qvalue.size() == 1) {
                return 0.0F;
            }
            constexpr std::size_t min_qvalue_length = 3; // e.g., "0.1"
            constexpr std::size_t max_qvalue_length = 5; // e.g., "0.123"
            if (qvalue.size() < min_qvalue_length || qvalue.size() > max_qvalue_length) {
                return -1.0F;
            }
            if (qvalue[1] != '.') {
                return -1.0F;
            }
            constexpr float base_fraction       = 0.1F;
            float           val                 = 0.0F;
            float           fraction_multiplier = base_fraction;
            for (std::size_t i = 2; i < qvalue.size(); ++i) {
                if (!ascii::is::digit(qvalue[i])) {
                    return -1.0F;
                }
                val                 += fraction_multiplier * static_cast<float>(qvalue[i] - '0');
                fraction_multiplier *= base_fraction;
            }
            return val;
        }

        /**
         * Converts a string to the known EncodingEnum.
         * Case‑insensitive matching is used.
         */
        template <ascii::char_case Case = ascii::char_case::unknown>
        [[nodiscard]] static constexpr EncodingEnum to_known_encoding(std::string_view str) noexcept {
            constexpr auto the_case = ascii::char_case_to_side(Case, ascii::char_case::lowered);
            if (str.empty()) [[unlikely]] {
                return EncodingEnum::unknown;
            }

            switch (str[0]) {
                [[unlikely]] case 'G':
                [[likely]] case 'g':
                    if (ascii::iequals<the_case>(str, "gzip")) {
                        return EncodingEnum::gzip;
                    }
                    break;
                [[unlikely]] case 'B':
                [[likely]] case 'b':
                    if (ascii::iequals<the_case>(str, "br")) {
                        return EncodingEnum::br;
                    }
                    break;
                [[unlikely]] case 'D':
                [[likely]] case 'd':
                    if (ascii::iequals<the_case>(str, "deflate")) {
                        return EncodingEnum::deflate;
                    }
                    break;
                [[unlikely]] case 'C':
                [[unlikely]] case 'c': // unlikely because it's a deprecated algorithm
                    if (ascii::iequals<the_case>(str, "compress")) {
                        return EncodingEnum::compress;
                    }
                    break;
                [[unlikely]] case 'x':
                [[unlikely]] case 'X': // browsers rarely use x- prefix
                    if (ascii::iequals<the_case>(str, "x-gzip")) {
                        return EncodingEnum::gzip;
                    }
                    if (ascii::iequals<the_case>(str, "x-compress")) {
                        return EncodingEnum::compress;
                    }
                    break;
                case '*': return EncodingEnum::all;
                default: break;
            }
            return EncodingEnum::unknown;
        }

      public:
        /**
         * Returns the array of parsed encodings.
         */
        [[nodiscard]] constexpr std::span<compression_algo const> allowed_encodings() const noexcept {
            return {_allowed_encodings.data(), _count};
        }

        /**
         * Checks whether the given encoding is allowed.
         * Accepts one or more strings (for e.g. "gzip", "x-gzip").
         */
        template <ascii::char_case Case = ascii::char_case::unknown>
        [[nodiscard]] constexpr bool is_allowed(std::convertible_to<std::string_view> auto&&... str) const noexcept {
            return get<Case>(std::forward<decltype(str)>(str)...) != nullptr;
        }

        /**
         * Finds the entry matching one of the provided string(s).
         * Returns a pointer to the compression_algo, or nullptr if not found.
         */
        template <ascii::char_case Case = ascii::char_case::unknown>
        [[nodiscard]] constexpr compression_algo const* get(
          std::convertible_to<std::string_view> auto&&... str) const noexcept {
            if (!is_valid()) {
                return nullptr;
            }
            for (std::size_t i = 0; i < _count; ++i) {
                auto const& algo = _allowed_encodings[i];
                bool        matches =
                  ((ascii::iequals<ascii::char_case_to_side(ascii::char_case::unknown, Case)>(algo.name, str) || ...));
                if (matches) {
                    return &algo;
                }
            }
            return nullptr;
        }

        /**
         * Finds the entry for a specific EncodingEnum value.
         */
        [[nodiscard]] constexpr compression_algo const* get(EncodingEnum const type) const noexcept {
            for (std::size_t i = 0; i < _count; ++i) {
                if (_allowed_encodings[i].encoding == type) {
                    return &_allowed_encodings[i];
                }
            }
            return nullptr;
        }

        /**
         * Checks whether a specific EncodingEnum value is allowed.
         */
        [[nodiscard]] constexpr bool is_allowed(EncodingEnum const type) const noexcept {
            if (auto const* algo = get(type)) {
                return algo->quality > 0.0F;
            }
            if (auto const* star = get(EncodingEnum::all)) {
                return star->quality > 0.0F;
            }
            return type == EncodingEnum::identity;
        }

        /**
         * Returns the best algorithm according to quality values and order.
         * (Currently returns identity; can be enhanced later.)
         */
        [[nodiscard]] constexpr EncodingEnum best_algorithm() const noexcept {
            // TODO: implement selection based on quality and order
            return EncodingEnum::identity;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return _count != 0;
        }

      private:
        using count_type = stl::make_unsigned_t<istl::integer_max_t<MaxSupported, stl::size_t>>;

        stl::array<compression_algo, MaxSupported> _allowed_encodings{};

        // count == 0 is considered invalid
        count_type _count = 0;
    };


} // namespace webpp::http

#endif // WEBPP_ACCEPT_ENCODING_HPP
