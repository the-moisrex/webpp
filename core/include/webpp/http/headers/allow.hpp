// Created by moisrex on 10/24/20.

#ifndef WEBPP_ALLOW_HPP
#define WEBPP_ALLOW_HPP

namespace webpp::http {

    /**
     *
     * The Allow entity-header field lists the set of methods supported
     * by the resource identified by the Request-URI. The purpose of this
     * field is strictly to inform the recipient of valid methods
     * associated with the resource. An Allow header field MUST be
     * present in a 405 (Method Not Allowed) response.
     *
     *     Allow   = "Allow" ":" #Method
     *
     * Example of use:
     *
     *     Allow: GET, HEAD, PUT
     *
     * This field cannot prevent a client from trying other methods.
     * However, the indications given by the Allow header field value
     * SHOULD be followed. The actual set of allowed methods is defined
     * by the origin server at the time of each request.
     *
     * The Allow header field MAY be provided with a PUT request to
     * recommend the methods to be supported by the new or modified
     * resource. The server is not required to support these methods and
     * SHOULD include an Allow header in the response giving the actual
     * supported methods.
     *
     * A proxy MUST NOT modify the Allow header field even if it does not
     * understand all the methods specified, since the user agent might
     * have other means of communicating with the origin server.
     */
    struct allow {

    };
}

#endif // WEBPP_ALLOW_HPP
