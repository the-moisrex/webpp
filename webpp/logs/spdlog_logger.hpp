// Created by moisrex on 8/17/20.

#ifndef WEBPP_SPDLOG_LOGGER_HPP
#define WEBPP_SPDLOG_LOGGER_HPP

#if __has_include(<spdlog/spdlog.h>)
#    define WEBPP_SPDLOG 1
#    include "../std/string_view.hpp"
#    include "../traits/traits.hpp"

#    include <memory>
#    include <spdlog/spdlog.h>
#endif

#ifdef WEBPP_SPDLOG
#    include "../common/meta.hpp"

namespace webpp {

    template <bool IsDebug = is_debug_build>
    struct spdlog_logger {
        using logger_type = spdlog_logger;
        using logger_ref  = logger_type const&;
        using logger_ptr  = logger_type*;

        static constexpr bool enabled = true;
#    if defined(DEBUG) && DEBUG
        using debugger_type            = spdlog_logger<true>;
        static constexpr bool is_debug = true;
#    else
        struct debugger_type {
            debugger_type(auto) = default;
            void log(auto...);
            void info(auto...);
            void warning(auto...);
            void error(auto...);
            void critical(auto...);
            void trace(auto...);
        };
        static constexpr bool is_debug = false;
#    endif

        stl::shared_ptr<spdlog::logger> const spdlogger;

        spdlog_logger() : spdlogger{spdlog::default_logger()} {}
        spdlog_logger(stl::shared_ptr<spdlog::logger> logger) noexcept : spdlogger{logger} {}
        spdlog_logger(istl::StringViewifiable auto&& logger_name)
          : spdlogger{spdlog::get(istl::string_viewify(logger_name).c_str())} {}

#    define WEBPP_LOGGER_SHORTCUT(func_name, logging_name)                                                 \
                                                                                                           \
        void func_name(istl::StringViewifiable auto&& details) const noexcept {                            \
            if constexpr (!is_debug) {                                                                     \
                spdlogger->logging_name("{}", stl::forward<decltype(details)>(details));                   \
            }                                                                                              \
        }                                                                                                  \
                                                                                                           \
        void func_name(istl::StringViewifiable auto&& category, istl::StringViewifiable auto&& details)    \
          const noexcept {                                                                                 \
            if constexpr (!is_debug) {                                                                     \
                spdlogger->logging_name("[{}] {}",                                                         \
                                        stl::forward<decltype(category)>(category),                        \
                                        stl::forward<decltype(details)>(details));                         \
            }                                                                                              \
        }                                                                                                  \
                                                                                                           \
        void func_name(istl::StringViewifiable auto&& category,                                            \
                       istl::StringViewifiable auto&& details,                                             \
                       stl::error_code const&         ec) const noexcept {                                         \
            if constexpr (!is_debug) {                                                                     \
                spdlogger->logging_name("[{}] {}; error message: {}",                                      \
                                        stl::forward<decltype(category)>(category),                        \
                                        stl::forward<decltype(details)>(details),                          \
                                        ec.message());                                                     \
            }                                                                                              \
        }                                                                                                  \
                                                                                                           \
        void func_name(istl::StringViewifiable auto&& category,                                            \
                       istl::StringViewifiable auto&& details,                                             \
                       stl::exception const&          ex) const noexcept {                                          \
            if constexpr (!is_debug) {                                                                     \
                spdlogger->logging_name("[{}] {}; error message: {}",                                      \
                                        stl::forward<decltype(category)>(category),                        \
                                        stl::forward<decltype(details)>(details),                          \
                                        ex.what());                                                        \
            }                                                                                              \
        }                                                                                                  \
                                                                                                           \
        void func_name(istl::StringViewifiable auto&& details, stl::error_code const& ec) const noexcept { \
            if constexpr (!is_debug) {                                                                     \
                spdlogger->logging_name("{}; error message: {}",                                           \
                                        stl::forward<decltype(details)>(details),                          \
                                        ec.message());                                                     \
            }                                                                                              \
        }                                                                                                  \
                                                                                                           \
        void func_name(istl::StringViewifiable auto&& details, stl::exception const& ex) const noexcept {  \
            if constexpr (!is_debug) {                                                                     \
                spdlogger->logging_name("{}; error message: {}",                                           \
                                        stl::forward<decltype(details)>(details),                          \
                                        ex.what());                                                        \
            }                                                                                              \
        }


        WEBPP_LOGGER_SHORTCUT(info, info)
        WEBPP_LOGGER_SHORTCUT(warning, warn)
        WEBPP_LOGGER_SHORTCUT(error, error)
        WEBPP_LOGGER_SHORTCUT(critical, critical)
        WEBPP_LOGGER_SHORTCUT(trace, trace)

        [[no_unique_address]] debugger_type debug{spdlogger};

#    undef WEBPP_LOGGER_SHORTCUT
    };

} // namespace webpp
#endif

#endif // WEBPP_SPDLOG_LOGGER_HPP
