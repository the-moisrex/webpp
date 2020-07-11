// Created by moisrex on 4/22/20.

#ifndef WEBPP_ENCODED_WORD_H
#define WEBPP_ENCODED_WORD_H

namespace webpp::encoding {

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
     *
     * The format is:
     *   "=?charset?encoding?encoded text?="
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     */
}

#endif // WEBPP_ENCODED_WORD_H
