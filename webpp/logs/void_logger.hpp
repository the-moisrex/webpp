#ifndef WEBPP_LOGGER_VOID_LOGGER_HPP
#define WEBPP_LOGGER_VOID_LOGGER_HPP


namespace webpp {

    /**
     * Void Logger is a logger that logs nothing.
     * Its usage is that it lets you disable the logging whenever you want
     * it for other loggers like dynamic logger
     *
     * Example Usage:
     * @code
     *   dynamic_logger logger{...};
     *
     *   logger.set_logger(void_logger{});      // disable logging
     *   logger.set_logger(default_logger{});   // enable logging again
     * @endcode
     */
    struct void_logger {
        using logger_type = void_logger;
        using logger_ref  = void_logger;
        using logger_ptr  = void_logger*;

        constexpr void log(auto&&...) {}
        constexpr void info(auto&&...) {}
        constexpr void warning(auto&&...) {}
        constexpr void error(auto&&...) {}
        constexpr void critical(auto&&...) {}
        constexpr void trace(auto&&...) {}
    };

} // namespace webpp

#endif // WEBPP_LOGGER_VOID_LOGGER_HPP
