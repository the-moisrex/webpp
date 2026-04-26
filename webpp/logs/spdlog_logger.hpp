// Created by moisrex on 8/17/20.

#ifndef WEBPP_SPDLOG_LOGGER_HPP
#define WEBPP_SPDLOG_LOGGER_HPP

#if __has_include(<spdlog/spdlog.h>)
#    define WEBPP_SPDLOG 1
#    include "../std/string_view.hpp"

#    include <memory>
#    include <spdlog/spdlog.h>
#endif

#ifdef WEBPP_SPDLOG

namespace webpp {

    struct spdlog_logger {
        using logger_type = spdlog_logger;
        using logger_ref  = logger_type const&;
        using logger_ptr  = logger_type*;

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

        void log(istl::StringViewifiable auto&& category, istl::StringViewifiable auto&& details) const noexcept {
            spdlogger->log("[{}] {}",
                           stl::forward<decltype(category)>(category),
                           stl::forward<decltype(details)>(details));
        }
    };

} // namespace webpp
#endif

#endif // WEBPP_SPDLOG_LOGGER_HPP
