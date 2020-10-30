// Created by moisrex on 10/29/20.

#ifndef WEBPP_FCGI_REQUEST_MANAGER_HPP
#define WEBPP_FCGI_REQUEST_MANAGER_HPP

namespace webpp {

    /**
     * Since in the FCGI protocol the session manager has to handle multiple clients (multiple requests
     * from different clients sent by one server through one connection), I decided to have another layer
     * that only deals with one single request at a time.
     *
     * So FastCGI Request Manager is designed to handle only one request at a time.
     */
    struct fcgi_request_manager {

    };

}

#endif // WEBPP_FCGI_REQUEST_MANAGER_HPP
