// Created by moisrex on 7/15/20.

#ifndef WEBPP_LOG_CONCEPTS_HPP
#define WEBPP_LOG_CONCEPTS_HPP

#include "../std/std.hpp"

#include <type_traits>

namespace webpp {

    template <typename T>
    concept Logger = requires(T logger, stl::error_code ec, stl::exception ex) {
        typename T::logger_ref;
        typename T::logger_ptr;
        typename T::logger_type;

        {logger.info("log something")};
        {logger.info("category", "log something")};
        {logger.info("category", "Error.")};
        {logger.info("category", "Error.", ec)};
        {logger.info("category", "Error.", ex)};

        {logger.debug("log something")};
        {logger.debug("category", "log something")};
        {logger.debug("category", "Error.")};
        {logger.debug("category", "Error.", ec)};
        {logger.debug("category", "Error.", ex)};

        {logger.error("log something")};
        {logger.error("category", "log something")};
        {logger.error("category", "Error.")};
        {logger.error("category", "Error.", ec)};
        {logger.error("category", "Error.", ex)};

        {logger.warning("log something")};
        {logger.warning("category", "log something")};
        {logger.warning("category", "Error.")};
        {logger.warning("category", "Error.", ec)};
        {logger.warning("category", "Error.", ex)};

        {logger.critical("log something")};
        {logger.critical("category", "log something")};
        {logger.critical("category", "Error.")};
        {logger.critical("category", "Error.", ec)};
        {logger.critical("category", "Error.", ex)};

        {logger.terminate("log something")};
        {logger.terminate("category", "log something")};
        {logger.terminate("category", "Error.")};
        {logger.terminate("category", "Error.", ec)};
        {logger.terminate("category", "Error.", ex)};
    };

} // namespace webpp

#endif // WEBPP_LOG_CONCEPTS_HPP
