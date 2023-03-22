#ifndef WEBPP_SERVER_CONSTANTS_HPP
#define WEBPP_SERVER_CONSTANTS_HPP

namespace webpp {

    /**
     * This is the default address for listening on when using Self-Hosted protocols
     */
    constexpr auto default_self_hosted_listen_addr = "localhost";

    /**
     * This is the default port to listen to when using the Self-Hosted protocols
     */
    constexpr auto default_self_hosted_listen_port = 80;

    /**
     * This is the default address to listen on when using fcgi protocols
     */
    constexpr auto default_fcgi_listen_addr = "localhost";

    /**
     * This is the default port to listen on when using the fcgi protocols
     */
    constexpr auto default_fcgi_listen_port = 8181;

} // namespace webpp


#endif // WEBPP_SERVER_CONSTANTS_HPP
