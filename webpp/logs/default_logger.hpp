#ifndef WEBPP_LOGGER_DEFAULT_HPP
#define WEBPP_LOGGER_DEFAULT_HPP

#include "std_logger.hpp"

namespace webpp {

    using default_logger = stderr_logger;

    /// Default non-dynamic logger, this is being used as the default logger for the dynamic loggers.
    using default_static_logger = stderr_logger;
} // namespace webpp

#endif // WEBPP_LOGGER_DEFAULT_HPP
