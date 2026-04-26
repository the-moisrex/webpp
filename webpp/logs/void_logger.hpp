#ifndef WEBPP_LOGGER_VOID_LOGGER_HPP
#define WEBPP_LOGGER_VOID_LOGGER_HPP

namespace webpp {

    /**
     * Void Logger is a logger that logs nothing.
     * Its usage is that it lets you disable the logging whenever you want
     * it for other loggers like dynamic logger
     */
    struct [[nodiscard]] void_logger {
        constexpr void log(auto&&...) {}
    };

} // namespace webpp

#endif // WEBPP_LOGGER_VOID_LOGGER_HPP
