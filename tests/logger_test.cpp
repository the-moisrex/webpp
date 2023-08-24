// Created by moisrex on 8/16/20.

#include "../webpp/logs/spdlog_logger.hpp"
#include "../webpp/logs/std_logger.hpp"
#include "common/tests_common_pch.hpp"

#include <cstdio>

using namespace webpp;

std::FILE* output;

inline auto* output_getter() {
    return output;
}

TEST(LoggerTests, LoggerConcepts) {
    EXPECT_TRUE(Logger<stdout_logger>);
    EXPECT_TRUE(Logger<stderr_logger>);
    EXPECT_TRUE(Logger<spdlog_logger<>>);
}

TEST(LoggerTests, STDLoggerTeat) {

    output = std::tmpfile();

    using ol = std_logger<output_getter>;

    ol logger;

    logger.error("one");
    logger.warning("one");
    logger.info("wow", "one");
    logger.error(if_debug, "debugging");

    std::fclose(output);
}
