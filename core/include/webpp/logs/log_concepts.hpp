// Created by moisrex on 7/15/20.

#ifndef WEBPP_LOG_CONCEPTS_HPP
#define WEBPP_LOG_CONCEPTS_HPP

#include "../std/std.hpp"

#include <type_traits>

namespace webpp {

    template <typename T>
    concept Logger = requires(stl::remove_cvref_t<T> logger, stl::error_code ec, stl::exception ex) {
        typename stl::remove_cvref_t<T>::logger_ref;
        typename stl::remove_cvref_t<T>::logger_ptr;
        typename stl::remove_cvref_t<T>::logger_type;

        logger.enabled->stl::same_as<bool>;

#define WEBPP_LOGGER_CONCEPT(logger_name)            \
    logger.logger_name("log something");             \
    logger.logger_name("category", "log something"); \
    logger.logger_name("category", "Error.");        \
    logger.logger_name("category", "Error.", ec);    \
    logger.logger_name("category", "Error.", ex);

        WEBPP_LOGGER_CONCEPT(info)
        WEBPP_LOGGER_CONCEPT(warning)
        WEBPP_LOGGER_CONCEPT(error)
        WEBPP_LOGGER_CONCEPT(critical)

#undef WEBPP_LOGGER_CONCEPT
    };

} // namespace webpp

#endif // WEBPP_LOG_CONCEPTS_HPP
