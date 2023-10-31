// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_CONSTANTS_HPP
#define WEBPP_URI_CONSTANTS_HPP

#include "../../strings/charset.hpp"

namespace webpp::uri::details {

    // NOLINTBEGIN(*-magic-numbers)

    /**
     * source:
     * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/encodeURI
     */
    template <istl::CharType char_type>
    static constexpr auto ALLOWED_CHARACTERS_IN_URI =
      charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 20>{';', ',', '/',  '?', ':', '@', '&',
                                                                         '=', '+', '$',  '-', '_', '.', '!',
                                                                         '~', '*', '\'', '(', ')', '#'});
    /**
     * This is the character set corresponds to the second part of the "scheme" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    template <istl::CharType char_type>
    static constexpr auto SCHEME_NOT_FIRST =
      charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 3>{'+', '-', '.'});

    /**
     * This is the character set corresponds to the "unreserved" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    template <istl::CharType char_type>
    static constexpr auto UNRESERVED =
      charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 4>{'-', '.', '_', '~'});

    /**
     * This is the character set corresponds to the "sub-delims" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    template <istl::CharType char_type>
    static constexpr auto SUB_DELIMS =
      charset<char_type, 11>('!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=');

    /**
     * This is the character set corresponds to the "userinfo" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded".
     */
    template <istl::CharType char_type>
    static constexpr auto USER_INFO_NOT_PCT_ENCODED =
      charset(UNRESERVED<char_type>, SUB_DELIMS<char_type>, charset<char_type, 1>{':'});

    /**
     * This is the character set corresponds to the last part of the "IPvFuture" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    template <istl::CharType char_type>
    static constexpr auto IPV_FUTURE_LAST_PART =
      charset(UNRESERVED<char_type>, SUB_DELIMS<char_type>, charset<char_type, 1>{':'});

    /**
     * This is the character set corresponds to the "reg-name" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded".
     */
    template <istl::CharType char_type>
    static constexpr auto REG_NAME_NOT_PCT_ENCODED = charset(UNRESERVED<char_type>, SUB_DELIMS<char_type>);

    /**
     * This is the character set corresponds to the "pchar" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded".
     */
    template <istl::CharType char_type>
    static constexpr auto PCHAR_NOT_PCT_ENCODED =
      charset(UNRESERVED<char_type>, SUB_DELIMS<char_type>, charset<char_type, 2>{':', '@'});

    /**
     * This is the character set corresponds to the "query" syntax and the "fragment" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded".
     */
    template <istl::CharType char_type>
    static constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED =
      charset(PCHAR_NOT_PCT_ENCODED<char_type>, charset<char_type, 2>{'/', '?'});


    /**
     * This is the character set almost corresponds to the "query" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded", except that '+' is also excluded, because
     * for some web services (e.g. AWS S3) a '+' is treated as
     * synonymous with a space (' ') and thus gets misinterpreted.
     */
    template <istl::CharType char_type>
    static constexpr auto QUERY_NOT_PCT_ENCODED_WITHOUT_PLUS =
      charset(UNRESERVED<char_type>,
              charset('!', '$', '&', '\'', '(', ')', '*', ',', ';', '=', ':', '@', '/', '?'));

    template <istl::CharType char_type>
    static constexpr auto LEGAL_PATH_CHARS =
      charset(PCHAR_NOT_PCT_ENCODED<char_type>, charset<char_type, 1>('/'));


    using ascii_bitmap = bitmap<256U>;

    /// https://infra.spec.whatwg.org/#c0-control
    static constexpr auto C0_CONTROL_SET = bitmap_range<0X0ULL, 0X001FULL>();



    /// https://url.spec.whatwg.org/#c0-control-percent-encode-set
    /// C0 controls and all code points greater than U+007E (~)
    static constexpr ascii_bitmap C0_CONTROL_ENCODE_SET{bitmap_range<0X0ULL, 0X001FULL, 256U>(),
                                                        bitmap_range<0X007EULL, 0X00FFULL>()};

    /// https://url.spec.whatwg.org/#fragment-percent-encode-set
    /// C0 control percent-encode set and U+0020 SPACE, U+0022 ("), U+003C (<), U+003E (>), and U+0060 (`)
    static constexpr ascii_bitmap FRAGMENT_ENCODE_SET{C0_CONTROL_ENCODE_SET,
                                                      ascii_bitmap{charset(' ', '"', '<', '>', '`')}};


    /// https://url.spec.whatwg.org/#query-percent-encode-set
    /// C0 control percent-encode set and U+0020 SPACE, U+0022 ("), U+0023 (#), U+003C (<), and U+003E (>).
    static constexpr ascii_bitmap QUERIES_ENCODE_SET{C0_CONTROL_ENCODE_SET,
                                                     ascii_bitmap{charset(' ', '"', '<', '>', '#')}};

    /// https://url.spec.whatwg.org/#special-query-percent-encode-set
    /// query percent-encode set and U+0027 (').
    static constexpr ascii_bitmap SPECIAL_QUERIES_ENCODE_SET{QUERIES_ENCODE_SET, ascii_bitmap{charset('\'')}};

    /// https://url.spec.whatwg.org/#path-percent-encode-set
    /// query percent-encode set and U+003F (?), U+0060 (`), U+007B ({), and U+007D (}).
    static constexpr ascii_bitmap PATH_ENCODE_SET{QUERIES_ENCODE_SET,
                                                  ascii_bitmap{charset('?', '`', '{', '}')}};

    /// https://url.spec.whatwg.org/#userinfo-percent-encode-set
    /// path percent-encode set and U+002F (/), U+003A (:), U+003B (;), U+003D (=), U+0040 (@), U+005B ([) to
    /// U+005E (^), inclusive, and U+007C (|).
    static constexpr ascii_bitmap USER_INFO_ENCODE_SET{PATH_ENCODE_SET,
                                                       ascii_bitmap{charset('/', ':', ';', '=', '@', '|')},
                                                       bitmap_range<'[', '^', 256U>()};

    /// https://url.spec.whatwg.org/#component-percent-encode-set
    /// userinfo percent-encode set and U+0024 ($) to U+0026 (&), inclusive, U+002B (+), and U+002C (,).
    static constexpr ascii_bitmap COMPONENT_ENCODE_SET{USER_INFO_ENCODE_SET,
                                                       ascii_bitmap{charset('+', ',')},
                                                       bitmap_range<'$', '&', 256U>()};


    // NOLINTEND(*-magic-numbers)

} // namespace webpp::uri::details

#endif // WEBPP_URI_CONSTANTS_HPP
