// Created by moisrex on 10/7/20.

#ifndef WEBPP_ACCEPT_ENCODING_HPP
#define WEBPP_ACCEPT_ENCODING_HPP

#include "../../std/string_view.hpp"
#include "../../std/vector.hpp"
#include "../../strings/ascii.hpp"
#include "../../strings/validators.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../http/syntax/common.hpp"

namespace webpp::headers {
    /**
     * RFC:      https://tools.ietf.org/html/rfc7231#section-5.3.4
     * MDN Docs: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Accept-Encoding
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
     */

    template <Traits TraitsType>
    struct accept_encoding {
        using str_v       = typename TraitsType::string_view_type;
        using traits_type = TraitsType;
        using string_tokenizer_type = string_tokenizer<traits_type>;

        struct encoding {
            str_v name;
            float quality = 1.0f; // between 0 and 1
                                  // up to three decimal digits (but 1 or 2 is the max for some browsers)
                                  // default: 1
                                  // https://developer.mozilla.org/en-US/docs/Glossary/Quality_values
        };

        using allowed_encodings_type = istl::vector<traits_type, encoding>;

        // ctor
        constexpr accept_encoding(auto&&... args) noexcept(
          noexcept(data(stl::forward<decltype(args)>(args)...)))
          : data{stl::forward<decltype(args)>(args)...} {}

        /**
         * Known encoding types
         */
        enum encoding_types {
            identity, // Indicates the identity function (i.e. no compression, nor modification). This value
                      // is always considered as acceptable, even if not present.
            gzip,     // A compression format using the Lempel-Ziv coding (LZ77), with a 32-bit CRC.
                      // https://en.wikipedia.org/wiki/LZ77_and_LZ78#LZ77
            compress, // A compression format using the Lempel-Ziv-Welch (LZW) algorithm.
                      // https://en.wikipedia.org/wiki/LZW
            deflate, // A compression format using the zlib structure, with the deflate compression algorithm.
                     // https://en.wikipedia.org/wiki/Zlib
                     // https://en.wikipedia.org/wiki/DEFLATE
            br,      // A compression format using the Brotli algorithm.
                     // https://en.wikipedia.org/wiki/Brotli
            all // Matches any content encoding not already listed in the header. This is the default value if
                // the header is not present. It doesn't mean that any algorithm is supported; merely that no
                // preference is expressed.
        };


        void parse() noexcept {
            if (data.find_first_of('\"') != str_v::npos) {
                _is_valid = false;
                return;
            }
            _allowed_encodings.clear();

            string_tokenizer_type tokenizer(data, ",");
            while (tokenizer.get_next()) {
                auto entry = tokenizer.token();
                http::trim_lws(entry);
                size_t semicolon_pos    = entry.find(';');
                if (semicolon_pos == str_v::npos) {
                    if (entry.find_first_of(http::http_lws) != str_v::npos) {
                        _is_valid = false;
                        return;
                    }
                    _allowed_encodings.insert(ascii::to_lower_copy(entry));
                    continue;
                }
                auto encoding = entry.substr(0, semicolon_pos);
                http::trim_lws(encoding);
                if (encoding.find_first_of(http::http_lws) != str_v::npos) {
                    _is_valid = false;
                    return;
                }
                auto params = entry.substr(semicolon_pos + 1);
                http::trim_lws(params);
                size_t equals_pos        = params.find('=');
                if (equals_pos == str_v::npos) {
                    _is_valid = false;
                    return;
                }
                auto param_name = params.substr(0, equals_pos);
                http::trim_lws(param_name);
                if (!ascii::iequals<ascii::side::second_lowered>(param_name, "q")) {
                    _is_valid = false;
                    return;
                }
                auto qvalue = params.substr(equals_pos + 1);
                http::trim_lws(qvalue);
                if (qvalue.empty()) {
                    _is_valid = false;
                    return;
                }
                if (qvalue[0] == '1') {
                    if (str_v("1.000").starts_with(qvalue)) {
                        _allowed_encodings.insert(ascii::to_lower_copy(encoding));
                        continue;
                    }
                    _is_valid = false;
                    return;
                }
                if (qvalue[0] != '0') {
                    _is_valid = false;
                    return;
                }
                if (qvalue.length() == 1)
                    continue;
                if (qvalue.length() <= 2 || qvalue.length() > 5) {
                    _is_valid = false;
                    return;
                }
                if (qvalue[1] != '.') {
                    _is_valid = false;
                    return;
                }
                bool nonzero_number = false;
                for (size_t i = 2; i < qvalue.length(); ++i) {
                    if (!ascii::is::digit(qvalue[i])) {
                        _is_valid = false;
                        return;
                    }
                    if (qvalue[i] != '0')
                        nonzero_number = true;
                }
                if (nonzero_number)
                    _allowed_encodings.insert(ascii::to_lower_copy(encoding));
            }

            // RFC 7231 5.3.4 "A request without an Accept-Encoding header field implies
            // that the user agent has no preferences regarding content-codings."
            if (_allowed_encodings.empty()) {
                _allowed_encodings.insert("*");
                _is_valid = true;
                return;
            }

            // Any browser must support "identity".
            _allowed_encodings.insert("identity");

            // RFC says gzip == x-gzip; mirror it here for easier matching.
            if (_allowed_encodings.find("gzip") != _allowed_encodings.end())
                _allowed_encodings.insert("x-gzip");
            if (_allowed_encodings.find("x-gzip") != _allowed_encodings.end())
                _allowed_encodings.insert("gzip");

            // RFC says compress == x-compress; mirror it here for easier matching.
            if (_allowed_encodings.find("compress") != _allowed_encodings.end())
                _allowed_encodings.insert("x-compress");
            if (_allowed_encodings.find("x-compress") != _allowed_encodings.end())
                _allowed_encodings.insert("compress");

            _is_valid = true;
        }

        [[nodiscard]] allowed_encodings_type const& allowed_encodings() const noexcept {
            return _allowed_encodings;
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return _is_valid;
        }

      private:
        str_v                  data;
        allowed_encodings_type _allowed_encodings{};
        bool                   _is_valid = false;
    };

} // namespace webpp::headers

#endif // WEBPP_ACCEPT_ENCODING_HPP
