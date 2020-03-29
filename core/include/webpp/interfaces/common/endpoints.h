#ifndef WEBPP_INTERFACES_COMMON_ENDPOINTS
#define WEBPP_INTERFACES_COMMON_ENDPOINTS

#include <cstdint>
#include <string>
#include <system_error>

namespace webpp {
    /**
     * The endpoint may change to std::net::ip::tcp::endpoint (I'm not sure
     * about the namespaces); but until 2023, I'm gonna use this instead.
     */
    struct endpoint {
        std::string address;
        uint_fast16_t port;
        std::error_code ec;

        // TODO: check the address here and the port.
    };

} // namespace webpp

#endif // WEBPP_INTERFACES_COMMON_ENDPOINTS