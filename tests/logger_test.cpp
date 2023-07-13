// Created by moisrex on 8/16/20.

#include "../webpp/logs/std_logger.hpp"
#include "common/tests_common_pch.hpp"

#include <cstdio>

using namespace webpp;

std::FILE* output;

inline auto* output_getter() {
    return output;
}

TEST(Logger, STDLoggerTeat) {

    output = std::tmpfile();

    using ol = std_logger<output_getter>;

    ol logger;

    logger.error("one");
    logger.warning("one");
    logger.info("wow", "one");
    logger.debug.error("debugging");

    std::fclose(output);
}
