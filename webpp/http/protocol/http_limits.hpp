
#ifndef WEBPP_HTTP_PROTOCOL_LIMITS_HPP
#define WEBPP_HTTP_PROTOCOL_LIMITS_HPP

#include <cstdint>

namespace webpp::http {

    /// Request method (verb) length
    ///   - 32 or 64 bytes = safe for any practical verb
    ///   - 256 bytes = extremely generous
    ///   - 1024 bytes = RFC-purist
    static constexpr std::size_t max_method_length = 32U; // byte

    // Request line in HTTP/1 length
    static constexpr std::size_t max_request_line_length = 8190U; // 8KiB is enough

    /// Maximum number of Accept-Encoding values
    static constexpr std::size_t max_supported_accept_encoding_values = 10U;

    /// Maximum number of Accept values
    static constexpr std::size_t max_supported_accept_values = 10U;

    /// Maximum number of Content-Encoding values
    static constexpr std::size_t max_supported_content_encoding_values = 10U;
} // namespace webpp::http

#endif // WEBPP_HTTP_PROTOCOL_LIMITS_HPP
