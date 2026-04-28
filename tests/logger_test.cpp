// Created by moisrex on 8/16/20.

#include "../webpp/logs/logger.hpp"

#include "../webpp/logs/spdlog_logger.hpp"
#include "../webpp/logs/std_logger.hpp"
#include "../webpp/logs/void_logger.hpp"
#include "./common/test.hpp"

#include <cstdio>

using namespace webpp;

std::FILE* output;

inline auto* output_getter() {
    return output;
}

TEST(LoggerTests, LoggerConcepts) {
    EXPECT_TRUE(Logger<stdout_logger>);
    EXPECT_TRUE(Logger<stderr_logger>);
    EXPECT_TRUE(Logger<void_logger>);
#ifdef WEBPP_SPDLOG
    EXPECT_TRUE(Logger<spdlog_logger>);
#endif
}

TEST(LoggerTests, STDLoggerTeat) {
    output = std::tmpfile();

    basic_logger<std_logger<output_getter>> cur_logger;
    std_logger<output_getter>               ccl;
    dynamic_scope                           scope{cur_logger, ccl};

    cur_logger.error("one");
    cur_logger.warn("one");
    cur_logger.info("wow", "one");
    cur_logger.error(if_debug, "debugging");

    std::fclose(output);
}
