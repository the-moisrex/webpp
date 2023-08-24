// Created by moisrex on 7/15/20.

#ifndef WEBPP_LOG_CONCEPTS_HPP
#define WEBPP_LOG_CONCEPTS_HPP

#include "../std/concepts.hpp"
#include "../std/type_traits.hpp"

#include <system_error>

namespace webpp {

    /**
     * A simple helper to do the logging only if it's being run on debug build
     */
    inline constexpr struct if_debug_tag {
    } if_debug;

    namespace details {

        template <typename T>
        concept SimpleLogger = requires(T logger, stl::error_code ec, stl::exception ex) {
                                   typename T::logger_ref;
                                   typename T::logger_ptr;
                                   typename T::logger_type;


#define WEBPP_LOGGER_CONCEPT(logger_name)                      \
    logger.logger_name("log something");                       \
    logger.logger_name("category", "log something");           \
    logger.logger_name("category", "Error.");                  \
    logger.logger_name("category", "Error.", ec);              \
    logger.logger_name("category", "Error.", ex);              \
    logger.logger_name(if_debug, "log something");             \
    logger.logger_name(if_debug, "category", "log something"); \
    logger.logger_name(if_debug, "category", "Error.");        \
    logger.logger_name(if_debug, "category", "Error.", ec);    \
    logger.logger_name(if_debug, "category", "Error.", ex);

                                   WEBPP_LOGGER_CONCEPT(info)
                                   WEBPP_LOGGER_CONCEPT(warning)
                                   WEBPP_LOGGER_CONCEPT(error)
                                   WEBPP_LOGGER_CONCEPT(critical)

#undef WEBPP_LOGGER_CONCEPT
                               };
    } // namespace details

    template <typename T>
    concept Logger = details::SimpleLogger<stl::remove_cvref_t<T>>;


} // namespace webpp

#endif // WEBPP_LOG_CONCEPTS_HPP
