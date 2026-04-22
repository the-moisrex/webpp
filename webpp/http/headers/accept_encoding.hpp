// Created by moisrex on 10/7/20.

#ifndef WEBPP_ACCEPT_ENCODING_HPP
#define WEBPP_ACCEPT_ENCODING_HPP

#include "../../http/codec/common.hpp"
#include "../../std/cstdint.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../protocol/http_limits.hpp"
#include "./header_concepts.hpp"
#include "./parsers.hpp"

#include <array>
#include <cstdint>

namespace webpp::http {

    template <typename EncodingEnum>
    constexpr stl::size_t render_accept_encoding_entry(
      char*                  out,
      stl::size_t            max_length,
      EncodingEnum const     encoding,
      float const            quality,
      stl::string_view const name) noexcept {
        auto* ptr    = out;
        auto  append = [&](stl::string_view const value) constexpr {
            auto const length = render_header_text(ptr, max_length, value);
            stl::advance(ptr, static_cast<stl::ptrdiff_t>(length));
            max_length -= length;
        };

        auto const encoding_name = name.empty() ? to_string(encoding) : name;
        append(encoding_name);
        if (quality != 1.0F) {
            append("; q=");
            auto const qvalue_length = render_qvalue(ptr, max_length, quality);
            stl::advance(ptr, static_cast<stl::ptrdiff_t>(qvalue_length));
            max_length -= qvalue_length;
        }

        return static_cast<stl::size_t>(ptr - out);
    }


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

    template <typename EncodingEnum, ascii::char_case Case = ascii::char_case::unknown>
    [[nodiscard]] constexpr EncodingEnum to_known_encoding(std::string_view str) noexcept {
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

    template <typename CompressionAlgo, typename EncodingEnum, std::size_t MaxSupported, typename CountType>
    constexpr void parse_accept_encoding(
      stl::string_view const                     value,
      stl::array<CompressionAlgo, MaxSupported>& allowed_encodings,
      CountType&                                 count) noexcept {
        count = 0;
        if (value.find_first_of('\"') != stl::string_view::npos) {
            return;
        }

        string_tokenizer<stl::string_view> tokenizer(value);
        while (count < MaxSupported && tokenizer.next(charset{','})) {
            auto entry = tokenizer.token();
            http::trim_lws(entry);
            std::size_t const semicolon_pos = entry.find(';');
            if (semicolon_pos == stl::string_view::npos) {
                if (entry.find_first_of(http::http_lws.string_view()) != stl::string_view::npos) {
                    continue;
                }
                allowed_encodings[count++] =
                  CompressionAlgo{.encoding = to_known_encoding<EncodingEnum>(entry), .quality = 1.0F, .name = entry};
                continue;
            }
            auto encoding = entry.substr(0, semicolon_pos);
            http::trim_lws(encoding);
            if (encoding.find_first_of(http::http_lws.string_view()) != stl::string_view::npos) {
                continue;
            }
            auto params = entry.substr(semicolon_pos + 1);
            http::trim_lws(params);
            std::size_t const equals_pos = params.find('=');
            if (equals_pos == stl::string_view::npos) {
                continue;
            }
            auto param_name = params.substr(0, equals_pos);
            http::trim_lws(param_name);
            if (!ascii::iequals_sl(param_name, 'q')) {
                continue;
            }
            auto qvalue = params.substr(equals_pos + 1);
            http::trim_lws(qvalue);
            if (qvalue.empty()) {
                continue;
            }
            float const qval = parse_qvalue(qvalue);
            if (qval < 0.0F) {
                continue;
            }
            auto known                 = to_known_encoding<EncodingEnum>(encoding);
            allowed_encodings[count++] = CompressionAlgo{.encoding = known, .quality = qval, .name = encoding};
        }

        if (count == 0) {
            allowed_encodings[count++] =
              CompressionAlgo{.encoding = EncodingEnum::identity,
                              .quality  = 1.0F,
                              .name     = to_string(EncodingEnum::identity)};
            return;
        }

        bool has_identity = false;
        for (std::size_t i = 0; i < count; ++i) {
            if (allowed_encodings[i].encoding == EncodingEnum::identity) {
                has_identity = true;
                break;
            }
        }
        if (!has_identity && count < MaxSupported) {
            allowed_encodings[count++] =
              CompressionAlgo{.encoding = EncodingEnum::identity,
                              .quality  = 1.0F,
                              .name     = to_string(EncodingEnum::identity)};
        }
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
    template <typename EncodingEnum = encoding_type, std::size_t MaxSupported = max_supported_accept_encoding_values>
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
          : header_field_base<basic_accept_encoding>(src),
            _value(src) {
            parse_accept_encoding<compression_algo, EncodingEnum, MaxSupported>(src, _allowed_encodings, _count);
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
            if (auto const* algo = get<Case>(std::forward<decltype(str)>(str)...)) {
                return algo->quality > 0.0F;
            }
            if (auto const* star = get(EncodingEnum::all)) {
                return star->quality > 0.0F;
            }
            // If they passed "identity" and it wasn't explicitly forbidden, it's allowed.
            bool const is_id =
              ((ascii::iequals<ascii::char_case_to_side(ascii::char_case::unknown, Case)>(
                  "identity",
                  std::forward<decltype(str)>(str)) ||
                ...));
            return is_id;
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

        stl::string_view                           _value;
        stl::array<compression_algo, MaxSupported> _allowed_encodings{};

        // count == 0 is considered invalid
        count_type _count = 0;

      public:
        [[nodiscard]] constexpr stl::string_view value_string() const noexcept {
            return _value;
        }
    };

    template <typename EncodingEnum, std::size_t MaxSupported>
    constexpr stl::size_t render(char*                                                    out,
                                 stl::size_t                                              max_length,
                                 basic_accept_encoding<EncodingEnum, MaxSupported> const& header) noexcept {
        if (!header.is_valid()) {
            return 0;
        }

        auto* ptr = out;
        for (auto const& algo : header.allowed_encodings()) {
            if (ptr != out) {
                auto const separator_length = render_header_text(ptr, max_length, ", ");
                stl::advance(ptr, static_cast<stl::ptrdiff_t>(separator_length));
                max_length -= separator_length;
            }
            auto const entry_length =
              render_accept_encoding_entry(ptr, max_length, algo.encoding, algo.quality, algo.name);
            stl::advance(ptr, static_cast<stl::ptrdiff_t>(entry_length));
            max_length -= entry_length;
        }

        return static_cast<stl::size_t>(ptr - out);
    }


} // namespace webpp::http

#endif // WEBPP_ACCEPT_ENCODING_HPP
