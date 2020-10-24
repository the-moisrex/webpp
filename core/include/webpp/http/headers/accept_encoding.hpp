// Created by moisrex on 10/7/20.

#ifndef WEBPP_ACCEPT_ENCODING_HPP
#define WEBPP_ACCEPT_ENCODING_HPP

#include "../../http/syntax/common.hpp"
#include "../../std/string_view.hpp"
#include "../../std/vector.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/to_case.hpp"
#include "../../strings/validators.hpp"

namespace webpp::http {


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
     */
    template <Traits TraitsType, bool AllowUnknownAlgos = false>
    struct accept_encoding {
        using str_v                 = typename TraitsType::string_view_type;
        using traits_type           = TraitsType;
        using string_tokenizer_type = string_tokenizer<traits_type>;

        /**
         * Known encoding types
         */
        enum encoding_types {
            identity, // Indicates the identity function (i.e. no compression, nor modification). This value
            // is always considered as acceptable, even if not present.
            gzip, // A compression format using the Lempel-Ziv coding (LZ77), with a 32-bit CRC.
            // https://en.wikipedia.org/wiki/LZ77_and_LZ78#LZ77
            compress, // A compression format using the Lempel-Ziv-Welch (LZW) algorithm.
            // https://en.wikipedia.org/wiki/LZW
            deflate, // A compression format using the zlib structure, with the deflate compression algorithm.
            // https://en.wikipedia.org/wiki/Zlib
            // https://en.wikipedia.org/wiki/DEFLATE
            br, // A compression format using the Brotli algorithm.
            // https://en.wikipedia.org/wiki/Brotli
            all // Matches any content encoding not already listed in the header. This is the default value if
            // the header is not present. It doesn't mean that any algorithm is supported; merely that no
            // preference is expressed.
        };

        static constexpr bool allow_unknown_algos = AllowUnknownAlgos;
        using encoding_type = stl::conditional_t<allow_unknown_algos, str_v, encoding_types>;

        struct compression_algo_type {
            encoding_type encoding;
            float         quality = 1.0f; // between 0 and 1
                                  // up to three decimal digits (but 1 or 2 is the max for some browsers)
                                  // default: 1
                                  // https://developer.mozilla.org/en-US/docs/Glossary/Quality_values
        };

        using allowed_encodings_type = istl::vector<traits_type, compression_algo_type>;

        // ctor
        constexpr accept_encoding(auto&&... args) noexcept : data{stl::forward<decltype(args)>(args)...} {}


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
                size_t semicolon_pos = entry.find(';');
                if (semicolon_pos == str_v::npos) {
                    if (entry.find_first_of(http::http_lws) != str_v::npos) {
                        _is_valid = false;
                        return;
                    }
                    if constexpr (allow_unknown_algos) {
                        _allowed_encodings.emplace_back(entry);
                    } else {
                        _allowed_encodings.emplace_back(
                          to_known_algo(entry)); // identity will be used if it's unknown
                    }
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
                size_t equals_pos = params.find('=');
                if (equals_pos == str_v::npos) {
                    _is_valid = false;
                    return;
                }
                auto param_name = params.substr(0, equals_pos);
                http::trim_lws(param_name);
                if (!ascii::iequals<ascii::char_case_side::second_lowered>(
                      param_name, 'q')) { // the size is checked inside of iequals
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
                        if constexpr (allow_unknown_algos) {
                            _allowed_encodings.emplace_back(encoding, 1);
                        } else {
                            _allowed_encodings.emplace_back(to_known_algo(encoding), 1);
                        }
                        continue;
                    }
                    _is_valid = false;
                    return;
                }
                if (qvalue[0] != '0') {
                    _is_valid = false;
                    return;
                }
                auto len = qvalue.length();
                if (len == 1)
                    continue;
                if (len <= 2 || len > 5) {
                    _is_valid = false;
                    return;
                }
                if (qvalue[1] != '.') {
                    _is_valid = false;
                    return;
                }
                float qval = 0.0f;
                float d = 0.1f;
                for (size_t i = 2; i < len; ++i) {
                    if (!ascii::is::digit(qvalue[i])) {
                        _is_valid = false;
                        return;
                    }
                    qval += d * (qvalue[i] - '0');
                    d /= 10;
                }
                if (qval != 0) {
                    if constexpr (allow_unknown_algos) {
                        _allowed_encodings.emplace_back(encoding, qval);
                    } else {
                        _allowed_encodings.emplace_back(to_known_algo(encoding), qval);
                    }
                }
            }

            // RFC 7231 5.3.4 "A request without an Accept-Encoding header field implies
            // that the user agent has no preferences regarding content-codings."
            if (_allowed_encodings.empty()) {
                if constexpr (allow_unknown_algos) {
                    _allowed_encodings.emplace_back("*");
                } else {
                    _allowed_encodings.emplace_back(all);
                }
                _is_valid = true;
                return;
            }

            // Any browser must support "identity".
            if constexpr (allow_unknown_algos) {
                _allowed_encodings.emplace_back("identity");
            } else {
                _allowed_encodings.emplace_back(identity);
            }

            // RFC says gzip == x-gzip; mirror it here for easier matching.
            // if (_allowed_encodings.find("gzip") != _allowed_encodings.end())
            //     _allowed_encodings.emplace_back("x-gzip");
            // if (_allowed_encodings.find("x-gzip") != _allowed_encodings.end())
            //     _allowed_encodings.emplace_back("gzip");

            // RFC says compress == x-compress; mirror it here for easier matching.
            // if (_allowed_encodings.find("compress") != _allowed_encodings.end())
            //     _allowed_encodings.emplace_back("x-compress");
            // if (_allowed_encodings.find("x-compress") != _allowed_encodings.end())
            //     _allowed_encodings.emplace_back("compress");

            _is_valid = true;
        }

        template <ascii::char_case Case = ascii::char_case::unknown>
        [[nodiscard]] static constexpr encoding_types to_known_algo(istl::StringView auto&& str) noexcept {
            constexpr auto the_case = ascii::char_case_to_side(Case, ascii::char_case::lowered);
            if (str.empty()) {
                return all;
            }
            switch(str[0]) {
                [[unlikely]] case 'G':
                [[likely]] case 'g':
                    if (ascii::iequals<the_case>(str, "gzip")) {
                        return gzip;
                    }
                    break;
                [[unlikely]] case 'B':
                [[likely]] case 'b':
                    if (ascii::iequals<the_case>(str, "br")) {
                        return br;
                    }
                    break;
                [[unlikely]] case 'D':
                [[likely]] case 'd':
                    if (ascii::iequals<the_case>(str, "deflate")) {
                        return deflate;
                    }
                    break;
                [[unlikely]] case 'C':
                [[unlikely]] case 'c': // unlikely because it's a deprecated algorithm
                    if (ascii::iequals<the_case>(str, "compress")) {
                        return compress;
                    }
                    break;
                [[unlikely]] case 'x':
                [[unlikely]] case 'X': // unlikely because browsers usually don't use x- prefix
                    if (ascii::iequals<the_case>(str, "x-gzip")) {
                        return gzip;
                    } else if (ascii::iequals<the_case>(str, "x-compress")) {
                        return compress;
                    }
                    break;
            }
            return all;
        }

        [[nodiscard]] allowed_encodings_type const& allowed_encodings() const noexcept {
            return _allowed_encodings;
        }

        /**
         * Check if the specified string is allowed compression algorithm in the specified header
         * @param str
         * @return
         */
        template <ascii::char_case Case = ascii::char_case::unknown>
        [[nodiscard]] bool is_allowed(istl::StringViewifiable auto&&... str) const noexcept {
            return get<Case>(stl::forward<decltype(str)>(str)...) != _allowed_encodings.cend();
        }

        template <ascii::char_case Case = ascii::char_case::unknown>
        [[nodiscard]] auto get(istl::StringViewifiable auto&&... str) const noexcept {
            if (!_is_valid) { // it's not allowed if the string is not a valid accept-encoding header value
                return _allowed_encodings.cend();
            }
            return stl::find_if(_allowed_encodings.cbegin(), _allowed_encodings.cend(), [&](auto&& item) noexcept {
                return (ascii::iequals<ascii::char_case_to_side(ascii::char_case::unknown, Case)>(item.encoding, str) || ...);
            });
        }

        template <encoding_types Type>
        [[nodiscard]] auto get() const noexcept {
            // todo: this doesn't support "*", it's a maybe when it's present
            if constexpr (allow_unknown_algos) {
                if constexpr (gzip == Type) {
                    // RFC says gzip == x-gzip; mirror it here for easier matching.
                    return get<ascii::char_case::lowered>("gzip", "x-gzip");
                } else if constexpr (br == Type) {
                    return get<ascii::char_case::lowered>("br");
                } else if constexpr (compress == Type) {
                    // RFC says compress == x-compress; mirror it here for easier matching.
                    return get<ascii::char_case::lowered>("compress", "x-compress");
                } else if constexpr (deflate == Type) {
                    return get<ascii::char_case::lowered>("deflate");
                } else if constexpr (all == Type) {
                    return get<ascii::char_case::lowered>("*");
                } else if constexpr (identity == Type) {
                    return get<ascii::char_case::lowered>("identity");
                } else {
                    return _allowed_encodings.cend();
                }
            } else {
                return stl::find_if(_allowed_encodings.cbegin(), _allowed_encodings.cend(), [](auto&& item) noexcept {
                    return item.encoding == Type;
                });
            }
        }

        template <encoding_types Type>
        [[nodiscard]] bool is_allowed() const noexcept {
            return get<Type>() != _allowed_encodings.cend();
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return _is_valid;
        }

        [[nodiscard]] encoding_types best_algorithm() const noexcept {
            // todo: fill this
        }

      private:
        str_v                  data;
        allowed_encodings_type _allowed_encodings{};
        bool                   _is_valid = false;
    };

} // namespace webpp::http

#endif // WEBPP_ACCEPT_ENCODING_HPP
