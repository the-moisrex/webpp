#ifndef WEBPP_PROTOCOL_SERVER_CONCEPTS_HPP
#define WEBPP_PROTOCOL_SERVER_CONCEPTS_HPP

#include "../io/io_concepts.hpp"
#include "../ip/endpoint.hpp"

namespace webpp {

    template <typename T>
    concept Server = requires(T& server) {
        requires io::FullIOBackend<typename T::io_backend>;
        requires Endpoint<typename T::endpoint_type>;

        requires requires(typename T::endpoint_type endpoint) {
            // start listening on the specified endpoint
            listen(server, endpoint);
        };

        // check if TLS/SSL is enabled
        { is_encrypted(server) } noexcept -> stl::same_as<bool>;

        // start/stop the server
        start(server);
        stop(server);
        { is_running(server) } noexcept -> stl::same_as<bool>;
    };

} // namespace webpp

#endif // WEBPP_PROTOCOL_SERVER_CONCEPTS_HPP
