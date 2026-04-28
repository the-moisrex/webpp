// Created by moisrex on 8/17/20.

#ifndef WEBPP_SPDLOG_LOGGER_HPP
#define WEBPP_SPDLOG_LOGGER_HPP

#if __has_include(<spdlog/spdlog.h>)
#    define WEBPP_SPDLOG 1

#    include <cassert>
#    include <memory>
#    include <string_view>
#    include <utility>
#    include <version>

#    if defined(__cpp_lib_format) && !defined(SPDLOG_FMT_EXTERNAL) && !defined(SPDLOG_USE_STD_FORMAT)
#        define SPDLOG_USE_STD_FORMAT 1
#    endif

#    include "./log_concepts.hpp"

#    include <spdlog/common.h>
#    include <spdlog/spdlog.h>
#endif

#ifdef WEBPP_SPDLOG

namespace webpp {

    [[nodiscard]] static constexpr spdlog::level::level_enum to_spdlog_log_level(log_level const level) noexcept {
        using spdlog::level::level_enum;
        assert(stl::to_underlying(log_level::off) == stl::to_underlying(level_enum::off));
        return static_cast<level_enum>(level);
        // switch (level) {
        //     case log_level::trace: return level_enum::trace;
        //     case log_level::debug: return level_enum::debug;
        //     case log_level::info: return level_enum::info;
        //     case log_level::warn: return level_enum::warn;
        //     case log_level::err: return level_enum::err;
        //     case log_level::critical: return level_enum::critical;
        //     case log_level::off: return level_enum::off;
        //     default: break;
        // }
        // return level_enum::n_levels;
    }

    struct [[nodiscard]] spdlog_logger {
        using logger_type = spdlog_logger;

      private:
        stl::shared_ptr<spdlog::logger> spdlogger;

      public:
        spdlog_logger() : spdlogger{spdlog::default_logger()} {}

        explicit spdlog_logger(stl::shared_ptr<spdlog::logger> inp_logger) noexcept
          : spdlogger{stl::move(inp_logger)} {}

        explicit spdlog_logger(stl::string_view logger_name)
          : spdlogger{spdlog::get({logger_name.data(), logger_name.size()})} {}

        spdlog_logger(spdlog_logger const&)                = default;
        spdlog_logger(spdlog_logger&&) noexcept            = default;
        spdlog_logger& operator=(spdlog_logger const&)     = default;
        spdlog_logger& operator=(spdlog_logger&&) noexcept = default;
        ~spdlog_logger()                                   = default;

        /// Get a handle to the underlying spdlog logger
        spdlog::logger& get_handle() noexcept {
            return *spdlogger;
        }

        void log(log_level const level, stl::string_view const category, stl::string_view const details) const {
            spdlogger->log(to_spdlog_log_level(level), "[{}] {}", category, details);
        }

        void log(log_level const        level,
                 stl::string_view const category,
                 stl::string_view const details,
                 stl::string_view const more_details) const {
            spdlogger->log(to_spdlog_log_level(level), "[{}] {}. {}", category, details, more_details);
        }
    };

} // namespace webpp
#endif

#endif // WEBPP_SPDLOG_LOGGER_HPP
