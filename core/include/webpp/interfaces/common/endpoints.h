#ifndef WEBPP_INTERFACES_COMMON_ENDPOINTS
#define WEBPP_INTERFACES_COMMON_ENDPOINTS

#include <cstdint>
#include <string>

namespace webpp::common {
    /**
     * The endpoint may change to std::net::ip::tcp::endpoint (I'm not sure
     * about the namespaces); but until 2023, I'm gonna use this instead.
     */
    struct endpoint {
        std::string address;
        uint_fast8_t port;
    };

} // namespace webpp::common

#endif // WEBPP_INTERFACES_COMMON_ENDPOINTS