// Created by moisrex on 4/22/20.

#ifndef WEBPP_ENCODED_WORD_H
#define WEBPP_ENCODED_WORD_H

#include "../std/string_view.hpp"
#include "../std/string.hpp"

namespace webpp::http {

    /**
     * Information about the "Encoded-Word" syntax (RFC 2047):
     *   - https://en.wikipedia.org/wiki/MIME#Encoded-Word
     *   - https://tools.ietf.org/html/rfc2047
     *   - https://stackoverflow.com/a/4410331
     *   - https://dzone.com/articles/utf-8-in-http-headers
     *
     * Possible helpful implementations:
     *   - https://github.com/nwellnhof/Email-MIME-RFC2047
     *   - https://github.com/mathiasbynens/q-encoding
     *   - https://github.com/sassmann/rfc2047/blob/master/rfc2047.c
     *   - https://github.com/Jim-CodeHub/libmime
     *   - Chinese one:
     *      - https://github.com/wamdmlab/AstroServ/blob/8b5a2214a10a157bd2c073f8f9113f1c3040dce9/astroDB_cache/lib/acl/lib_acl_cpp/include/acl_cpp/mime/rfc2047.hpp
     *      - https://github.com/wamdmlab/AstroServ/blob/8b5a2214a10a157bd2c073f8f9113f1c3040dce9/astroDB_cache/lib/acl/lib_acl_cpp/src/mime/rfc2047.cpp
     *
     * The format is:
     *   "=?charset?encoding?encoded text?="
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     * Syntax:
     *
     *    encoded-word = "=?" charset "?" encoding "?" encoded-text "?="
     *
     *    charset = token    ; see section 3
     *
     *    encoding = token   ; see section 4
     *
     *    token = 1*<Any CHAR except SPACE, CTLs, and especials>
     *
     *    especials = "(" / ")" / "<" / ">" / "@" / "," / ";" / ":" / "
     *                <"> / "/" / "[" / "]" / "?" / "." / "="
     *
     *   encoded-text = 1*<Any printable ASCII character other than "?"
     *                     or SPACE>
     *                  ; (but see "Use of encoded-words in message
     *                  ; headers", section 5)
     */
    template <Traits TraitsType>
    struct encoded_word {
        using traits_type = TraitsType;
        using string_view_type = typename traits_type::string_view_type;
        using string_type = typename traits_type::string_type;
        using char_type = typename string_type::value_type;
        using allocator_type = typename traits_type::template allocator<typename string_type::value_type>;

      private:
        string_view_type input;
        string_type output;

      public:

        encoded_word(istl::StringViewifiable auto&& _input, allocator_type const &alloc = allocator_type{})
          : input(istl::string_viewify(stl::forward<decltype(_input)>(input))),
            output(alloc)
        {}

        encoded_word() = delete;
        encoded_word(encoded_word const&) = default;
        encoded_word(encoded_word &&) noexcept = default;

        [[nodiscard]] string_view_type charset() const noexcept {

        }

        [[nodiscard]] string_view_type encoding() const noexcept {

        }

        [[nodiscard]] string_view_type encoded() const noexcept {

        }

        [[nodiscard]] string_type const& decoded() const noexcept {

        }

        bool decode() noexcept {

        }

        bool encode() noexcept {

        }


    };

    /*
    * If the given string contains non-ASCII characters,
    * encodes the given string using RFC 2047 'Q' or 'B' word encoding.
    *
    * The given text must already be encoded in the character set
    * given in charset (default is UTF-8).
    *
    * Returns the encoded string, or the original string if it
    * consists only of ASCII characters.
    */
    static std::string encodeWord(const std::string& text, const std::string& charset = "UTF-8", char encoding = 'q');

    /*
    * Decodes a string containing encoded-word's according to the rules specified in
    * RFC 2047 and returns the decoded string. Both Q and B encodings are supported.
    *
    * If toCharset is not provided, no decoded string conversion is performed (ie.
    * string is simply decoded to the charset specified in encodedWord string)
    * If toCharset is provided, returned string is converted to the specified
    * charset. For a list of supported encodings, see Poco:TextEncodingRegistry.
     */
    static std::string decodeWord(const std::string& encodedWord, std::string toCharset = "");



}

#endif // WEBPP_ENCODED_WORD_H
