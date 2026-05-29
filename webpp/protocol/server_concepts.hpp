#ifndef WEBPP_PROTOCOL_SERVER_CONCEPTS_HPP
#define WEBPP_PROTOCOL_SERVER_CONCEPTS_HPP

#include "../ip/endpoint.hpp"

namespace webpp {

    template <typename T>
    concept Server = requires(T& server) {
        requires Endpoint<typename T::endpoint_type>;

        requires requires(typename T::endpoint_type endpoint) {
            // start listening on the specified endpoint
            listen(server, endpoint);

            // start the server
            run(server);
        };
    };

} // namespace webpp

#endif // WEBPP_PROTOCOL_SERVER_CONCEPTS_HPP
