// Created by moisrex on 10/24/20.

#ifndef WEBPP_ACCEPT_HPP
#define WEBPP_ACCEPT_HPP

namespace webpp::http {

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
    struct accept {

    };

}

#endif // WEBPP_ACCEPT_HPP
