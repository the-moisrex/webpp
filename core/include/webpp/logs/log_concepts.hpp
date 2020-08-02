// Created by moisrex on 7/15/20.

#ifndef WEBPP_LOG_CONCEPTS_HPP
#define WEBPP_LOG_CONCEPTS_HPP

#include "../std/std.hpp"
#include <type_traits>

namespace webpp {

    template <typename T>
    concept Logger = requires (T logger) {
        {logger.info("log something")};
        {logger.info("category", "log something")};
        {logger.info("category", "Error code is {}.", 1)};

        {logger.debug("log something")};
        {logger.debug("category", "log something")};
        {logger.debug("category", "Error code is {}.", 1)};

        {logger.error("log something")};
        {logger.error("category", "log something")};
        {logger.error("category", "Error code is {}.", 1)};

        {logger.warning("log something")};
        {logger.warning("category", "log something")};
        {logger.warning("category", "Error code is {}.", 1)};

        {logger.critical("log something")};
        {logger.critical("category", "log something")};
        {logger.critical("category", "Error code is {}.", 1)};

    };

}

#endif // WEBPP_LOG_CONCEPTS_HPP
