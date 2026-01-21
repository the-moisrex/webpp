#ifndef WEBPP_LOGGER_DEFAULT_HPP
#define WEBPP_LOGGER_DEFAULT_HPP

#include "std_logger.hpp"

namespace webpp {

    template <typename T, int priority = 1>
    struct default_logger_type : default_logger_type<T, priority - 1> {};

    template <typename T>
    struct default_logger_type<T, 0> {
        using type = stderr_logger;
    };

    template <typename T>
    using default_logger = typename default_logger_type<T>::type;

    /// Default non-dynamic logger, this is being used as the default logger for the dynamic loggers.
    using default_static_logger = stderr_logger;

} // namespace webpp

#endif // WEBPP_LOGGER_DEFAULT_HPP
