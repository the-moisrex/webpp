// Created by moisrex on 11/4/20.

#ifndef WEBPP_CONSTANTS_HPP
#define WEBPP_CONSTANTS_HPP

#include "../../strings/charset.hpp"

namespace webpp::uri::details {

    /**
     * source:
     * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/encodeURI
     */
    template<istl::CharType char_type>
    static constexpr auto ALLOWED_CHARACTERS_IN_URI =
      charset(ALPHA<char_type>, DIGIT<char_type>,
              charset<char_type, 20>{';', ',', '/', '?', ':', '@', '&',  '=', '+', '$',
                                     '-', '_', '.', '!', '~', '*', '\'', '(', ')', '#'});
    /**
     * This is the character set corresponds to the second part of the "scheme" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    template<istl::CharType char_type>
    static constexpr auto SCHEME_NOT_FIRST =
      charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 3>{'+', '-', '.'});

    /**
     * This is the character set corresponds to the "unreserved" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    template<istl::CharType char_type>
    static constexpr auto UNRESERVED =
      charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 4>{'-', '.', '_', '~'});

    /**
     * This is the character set corresponds to the "sub-delims" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    template<istl::CharType char_type>
    static constexpr auto SUB_DELIMS =
      charset<char_type, 11>('!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=');

    /**
     * This is the character set corresponds to the "userinfo" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded".
     */
    template<istl::CharType char_type>
    static constexpr auto USER_INFO_NOT_PCT_ENCODED =
      charset(UNRESERVED<char_type>, SUB_DELIMS<char_type>, charset<char_type, 1>{':'});

    /**
     * This is the character set corresponds to the last part of the "IPvFuture" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    template<istl::CharType char_type>
    static constexpr auto IPV_FUTURE_LAST_PART =
      charset(UNRESERVED<char_type>, SUB_DELIMS<char_type>, charset<char_type, 1>{':'});

    /**
     * This is the character set corresponds to the "reg-name" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded".
     */
    template<istl::CharType char_type>
    static constexpr auto REG_NAME_NOT_PCT_ENCODED = charset(UNRESERVED<char_type>, SUB_DELIMS<char_type>);

    /**
     * This is the character set corresponds to the "pchar" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded".
     */
    template<istl::CharType char_type>
    static constexpr auto PCHAR_NOT_PCT_ENCODED =
      charset(UNRESERVED<char_type>, SUB_DELIMS<char_type>, charset<char_type, 2>{':', '@'});

    /**
     * This is the character set corresponds to the "query" syntax and the "fragment" syntax
     * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
     * leaving out "pct-encoded".
     */
    template<istl::CharType char_type>
    static constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED =
      charset(PCHAR_NOT_PCT_ENCODED<char_type>, charset<char_type, 2>{'/', '?'});



    template <istl::CharType char_type>
    static constexpr auto LEGAL_PATH_CHARS =
      charset(PCHAR_NOT_PCT_ENCODED<char_type>, charset<char_type, 1>('/'));


}

#endif // WEBPP_CONSTANTS_HPP
