// Created by moisrex on 10/29/20.

#ifndef WEBPP_FCGI_SESSION_MANAGER_HPP
#define WEBPP_FCGI_SESSION_MANAGER_HPP

namespace webpp {

    /**
     * The session manager for the FastCGI protocol has to take care of all of the connections by all the
     * clients because it can only respond to one server (well, usually).
     * Even though the design of the session manager and the connection type is designed in a way that
     * it can handle multiple request from multiple separate servers, but still a single fcgi session
     * manager should be able to handle multiple HTTP requests and not just one.
     * To solve this issue, it's better to have a "request manager" class as well.
     */
    struct fcgi_session_manager {

    };

}

#endif // WEBPP_FCGI_SESSION_MANAGER_HPP
