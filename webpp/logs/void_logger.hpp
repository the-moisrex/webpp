#ifndef WEBPP_LOGGER_VOID_LOGGER_HPP
#define WEBPP_LOGGER_VOID_LOGGER_HPP


namespace webpp {

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
