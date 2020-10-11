#ifndef WEBPP_MODP_B64_IMPL_HPP
#define WEBPP_MODP_B64_IMPL_HPP

/**
 * \file
 * <PRE>
 * MODP_B64 - High performance base64 encoder/decoder
 * Version 1.3 -- 17-Mar-2006
 * http://modp.com/release/base64
 *
 * Copyright &copy; 2005, 2006  Nick Galbreath -- nickg [at] modp [dot] com
 * All rights reserved.
 *
 * 2020: C++ification is done by Mohammad Bahoosh (checkout web++ project for more information)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the modp.com nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This is the standard "new" BSD license:
 * http://www.opensource.org/licenses/bsd-license.php
 * </PRE>
 *
 *
 * The default implementation is the standard b64 encoding with padding.
 * It's easy to change this to use "URL safe" characters and to remove
 * padding.  See the modp_b64.c source code for details.
 *
 */

#include "modp_b64_data.hpp"

namespace webpp::modp_b64 {

    /**
     * Given a source string of length len, this returns the amount of
     * memory the destination string should have.
     *
     * remember, this is integer math
     * 3 bytes turn into 4 chars
     * ceiling[len / 3] * 4 + 1
     *
     * +1 is for any extra null.
     */
    static constexpr auto encode_len(auto A) noexcept {
        return (A + 2) / 3 * 4 + 1;
    }

    /**
     * Given a base64 string of length len,
     *   this returns the amount of memory required for output string
     *  It maybe be more than the actual number of bytes written.
     * NOTE: remember this is integer math
     * this allocates a bit more memory than traditional versions of b64
     * decode  4 chars turn into 3 bytes
     * floor[len * 3/4] + 2
     */
    static constexpr auto decode_len(auto A) noexcept {
        return (A / 4 * 3 + 2);
    }

    /**
     * Will return the strlen of the output from encoding.
     * This may be less than the required number of bytes allocated.
     *
     * This allows you to 'deserialized' a struct
     * \code
     * char* b64encoded = "...";
     * int len = strlen(b64encoded);
     *
     * struct datastuff foo;
     * if (encode_strlen(sizeof(struct datastuff)) != len) {
     *    // wrong size
     *    return false;
     * } else {
     *    // safe to do;
     *    if (decode((char*) &foo, b64encoded, len) == -1) {
     *      // bad characters
     *      return false;
     *    }
     * }
     * // foo is filled out now
     * \endcode
     */
    static constexpr auto encode_strlen(auto A) noexcept {
        return ((A + 2) / 3 * 4);
    }

    static constexpr std::size_t error = static_cast<std::size_t>(-1);

    static constexpr auto BADCHAR = 0x01FFFFFF;

/**
 * you can control if we use padding by commenting out this
 * next line.  However, I highly recommend you use padding and not
 * using it should only be for compatibility with a 3rd party.
 * Also, 'no padding' is not tested!
 */
#define DOPAD 1

/*
 * if we aren't doing padding
 * set the pad character to NULL
 */
#ifndef DOPAD
#    undef CHARPAD
#    define CHARPAD '\0'
#endif

    /**
     * Encode a raw binary string into base 64.
     * src contains the bytes
     * len contains the number of bytes in the src
     * dest should be allocated by the caller to contain
     *   at least encode_len(len) bytes (see below)
     *   This will contain the null-terminated b64 encoded result
     * returns length of the destination string plus the ending null byte
     *    i.e.  the result will be equal to strlen(dest) + 1
     *
     * Example
     *
     * \code
     * char* src = ...;
     * int srclen = ...; //the length of number of bytes in src
     * char* dest = (char*) malloc(encode_len);
     * int len = encode(dest, src, sourcelen);
     * if (len == -1) {
     *   printf("Error\n");
     * } else {
     *   printf("b64 = %s\n", dest);
     * }
     * \endcode
     *
     */
    [[nodiscard]] std::size_t encode(char* dest, const char* str, std::size_t len) {
        std::size_t   i = 0;
        std::uint8_t* p = (std::uint8_t*) dest;

        /* unsigned here is important! */
        std::uint8_t t1, t2, t3;

        if (len > 2) {
            for (; i < len - 2; i += 3) {
                t1   = str[i];
                t2   = str[i + 1];
                t3   = str[i + 2];
                *p++ = e0[t1];
                *p++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
                *p++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
                *p++ = e2[t3];
            }
        }

        switch (len - i) {
            case 0: break;
            case 1:
                t1   = str[i];
                *p++ = e0[t1];
                *p++ = e1[(t1 & 0x03) << 4];
                *p++ = CHARPAD;
                *p++ = CHARPAD;
                break;
            default: /* case 2 */
                t1   = str[i];
                t2   = str[i + 1];
                *p++ = e0[t1];
                *p++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
                *p++ = e2[(t2 & 0x0F) << 2];
                *p++ = CHARPAD;
        }

        *p = '\0';
        return p - (std::uint8_t*) dest;
    }


    /**
     * Decode a base64 encoded string
     *
     * src should contain exactly len bytes of b64 characters.
     *     if src contains -any- non-base characters (such as white
     *     space, -1 is returned.
     *
     * dest should be allocated by the caller to contain at least
     *    len * 3 / 4 bytes.
     *
     * Returns the length (strlen) of the output, or -1 if unable to
     * decode
     *
     * \code
     * char* src = ...;
     * int srclen = ...; // or if you don't know use strlen(src)
     * char* dest = (char*) malloc(decode_len(srclen));
     * int len = decode(dest, src, sourcelen);
     * if (len == -1) { error }
     * \endcode
     */
    [[nodiscard]] std::size_t decode(char* dest, const char* src, std::size_t len) {
        if (len == 0)
            return 0;
        if constexpr (std::endian::native == std::endian::big) { /* BIG ENDIAN -- SUN / IBM / MOTOROLA */

#ifdef DOPAD
            /* if padding is used, then the message must be at least
               4 chars and be a multiple of 4.
               there can be at most 2 pad chars at the end */
            if (len < 4 || (len % 4 != 0))
                return error;
            if (src[len - 1] == CHARPAD) {
                len--;
                if (src[len - 1] == CHARPAD) {
                    len--;
                }
            }
#endif /* DOPAD */

            std::size_t i;
            int         leftover = len % 4;
            std::size_t chunks   = (leftover == 0) ? len / 4 - 1 : len / 4;

            std::uint8_t*  p       = (std::uint8_t*) dest;
            std::uint32_t  x       = 0;
            std::uint32_t* destInt = (std::uint32_t*) p;
            std::uint32_t* srcInt  = (std::uint32_t*) src;
            std::uint32_t  y       = *srcInt++;
            for (i = 0; i < chunks; ++i) {
                x = d0[y >> 24 & 0xff] | d1[y >> 16 & 0xff] | d2[y >> 8 & 0xff] | d3[y & 0xff];

                if (x >= BADCHAR)
                    return error;
                *destInt = x << 8;
                p += 3;
                destInt = (std::uint32_t*) p;
                y       = *srcInt++;
            }

            switch (leftover) {
                case 0:
                    x = d0[y >> 24 & 0xff] | d1[y >> 16 & 0xff] | d2[y >> 8 & 0xff] | d3[y & 0xff];
                    if (x >= BADCHAR)
                        return error;
                    *p++ = ((std::uint8_t*) &x)[1];
                    *p++ = ((std::uint8_t*) &x)[2];
                    *p   = ((std::uint8_t*) &x)[3];
                    return (chunks + 1) * 3;
                case 1:
                    x  = d3[y >> 24];
                    *p = (std::uint8_t) x;
                    break;
                case 2:
                    x  = d3[y >> 24] * 64 + d3[(y >> 16) & 0xff];
                    *p = (std::uint8_t)(x >> 4);
                    break;
                default: /* case 3 */
                    x    = (d3[y >> 24] * 64 + d3[(y >> 16) & 0xff]) * 64 + d3[(y >> 8) & 0xff];
                    *p++ = (std::uint8_t)(x >> 10);
                    *p   = (std::uint8_t)(x >> 2);
                    break;
            }

            if (x >= BADCHAR)
                return error;
            return 3 * chunks + (6 * leftover) / 8;


        } else { // Little Endian - Intel and Friends

#ifdef DOPAD
            /*
             * if padding is used, then the message must be at least
             * 4 chars and be a multiple of 4
             */
            if (len < 4 || (len % 4 != 0))
                return error; /* error */
            /* there can be at most 2 pad chars at the end */
            if (src[len - 1] == CHARPAD) {
                len--;
                if (src[len - 1] == CHARPAD) {
                    len--;
                }
            }
#endif

            std::size_t i;
            int         leftover = len % 4;
            std::size_t chunks   = (leftover == 0) ? len / 4 - 1 : len / 4;

            std::uint8_t*       p = (std::uint8_t*) dest;
            std::uint32_t       x = 0;
            const std::uint8_t* y = (std::uint8_t*) src;
            for (i = 0; i < chunks; ++i, y += 4) {
                x = d0[y[0]] | d1[y[1]] | d2[y[2]] | d3[y[3]];
                if (x >= BADCHAR)
                    return error;
                *p++ = ((std::uint8_t*) (&x))[0];
                *p++ = ((std::uint8_t*) (&x))[1];
                *p++ = ((std::uint8_t*) (&x))[2];
            }

            switch (leftover) {
                case 0:
                    x = d0[y[0]] | d1[y[1]] | d2[y[2]] | d3[y[3]];

                    if (x >= BADCHAR)
                        return error;
                    *p++ = ((std::uint8_t*) (&x))[0];
                    *p++ = ((std::uint8_t*) (&x))[1];
                    *p   = ((std::uint8_t*) (&x))[2];
                    return (chunks + 1) * 3;
                    break;
                case 1: /* with padding this is an impossible case */
                    x  = d0[y[0]];
                    *p = *((std::uint8_t*) (&x)); // i.e. first char/byte in int
                    break;
                case 2: // * case 2, 1  output byte */
                    x  = d0[y[0]] | d1[y[1]];
                    *p = *((std::uint8_t*) (&x)); // i.e. first char
                    break;
                default:                                   /* case 3, 2 output bytes */
                    x    = d0[y[0]] | d1[y[1]] | d2[y[2]]; /* 0x3c */
                    *p++ = ((std::uint8_t*) (&x))[0];
                    *p   = ((std::uint8_t*) (&x))[1];
                    break;
            }

            if (x >= BADCHAR)
                return error;

            return 3 * chunks + (6 * leftover) / 8;
        }
    }

} // namespace webpp::modp_b64

#undef DOPAD

#endif