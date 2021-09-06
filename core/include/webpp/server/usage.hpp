#ifndef WEBPP_SERVER_USAGE_HPP
#define WEBPP_SERVER_USAGE_HPP

namespace webpp::host {
    using percent_type = unsigned short;

    /**
     * The options for is_busy
     */
    struct usage_options {
        percent_type cpu    = 95; // percent
        percent_type memory = 95; // percent
    };

    // check if the running host is busy or not
    // todo: implement this
    bool is_busy(usage_options);
} // namespace webpp::host

#endif // WEBPP_SERVER_USAGE_HPP
