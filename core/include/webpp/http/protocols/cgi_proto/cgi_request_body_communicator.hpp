// Created by moisrex on 12/19/22.

#ifndef WEBPP_CGI_REQUEST_BODY_COMMUNICATOR_HPP
#define WEBPP_CGI_REQUEST_BODY_COMMUNICATOR_HPP

#include "../../../std/type_traits.hpp"
#include "../../../traits/traits.hpp"

namespace webpp::http::cgi_proto {

    /**
     * The middle man between CGI request body and the framework's request body.
     *
     * This type implements HTTPRequestBodyCommunicator
     */
    template <typename ProtocolType>
    struct cgi_request_body_communicator {
        using protocol_type = ProtocolType;
        using traits_type   = typename protocol_type::traits_type;
        using char_type     = traits::char_type<traits_type>;
        using size_type     = stl::streamsize;

        /**
         * Read the body of the string
         */
        [[nodiscard]] size_type read(char_type* data, size_type count) const {
            return protocol_type::read(data, count);
        }

        [[nodiscard]] size_type read(char_type* data) const {
            return protocol_type::read(data);
        }
    };

} // namespace webpp::http::cgi_proto

#endif // WEBPP_CGI_REQUEST_BODY_COMMUNICATOR_HPP
