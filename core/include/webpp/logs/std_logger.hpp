// Created by moisrex on 8/16/20.

#ifndef WEBPP_STD_LOGGER_HPP
#define WEBPP_STD_LOGGER_HPP

#include "../std/format.hpp"
#include "../std/string_view.hpp"
#include "../traits/traits.hpp"

#include <cstdio>
#ifndef WEBPP_FMT_LIB
#    include <stdio.h>
#endif

namespace webpp {

    /**
     * A logger class
     * @tparam stream_getter is a callable that gets a FILE* (like stderr, stdin, stdout)
     */
    template <auto stream_getter, bool IsDebug = false>
    struct std_logger {
        using logger_type = std_logger;
        using logger_ref  = logger_type const; // copy the logger, there's nothing to copy
        using logger_ptr  = logger_type*;      // there's a syntax difference, so we can't copy

        static constexpr bool enabled = true; // todo: make this configurable by the user
#if defined(DEBUG) && DEBUG
        static constexpr bool is_debug = true;
#else
        static constexpr bool is_debug = false;
#endif
        static constexpr auto default_category_name = is_debug ? "Debug" : "Default";

        enum struct logging_type : stl::uint_fast8_t { info, warning, error, critical, unknown };

        static constexpr auto logging_type_to_string(logging_type lt) noexcept {
            switch (lt) {
                case logging_type::info: return "INFO";
                case logging_type::warning: return "WARNING";
                case logging_type::error: return "ERROR";
                case logging_type::critical: return "CRITICAL";
                case logging_type::unknown: return "UNKNOWN";
            }
            return "UNSPECIFIED";
        }

        static constexpr stl::size_t logging_type_string_size(logging_type lt) noexcept {
            return stl::string_view{logging_type_to_string(lt)}.size();
        }

        static void log(logging_type                   lt,
                        istl::StringViewifiable auto&& category,
                        istl::StringViewifiable auto&& details) noexcept {
            if constexpr (!is_debug) {
#ifdef WEBPP_FMT_LIB
                fmt::print(stream_getter(),
                           "[{}, {}]: {}\n",
                           logging_type_to_string(lt),
                           stl::forward<decltype(category)>(category),
                           stl::forward<decltype(details)>(details));
#else
                stl::printf(stream_getter(),
                            "[%s, %s]: %s\n",
                            logging_type_to_string(lt),
                            stl::forward<decltype(category)>(category),
                            stl::forward<decltype(details)>(details));
#endif
            }
        }

#define WEBPP_LOGGER_SHORTCUT(logging_name)                                                               \
                                                                                                          \
    void logging_name(istl::StringViewifiable auto&& details) const noexcept {                            \
        log(logging_type::logging_name, default_category_name, stl::forward<decltype(details)>(details)); \
    }                                                                                                     \
                                                                                                          \
    void logging_name(istl::StringViewifiable auto&& category, istl::StringViewifiable auto&& details)    \
      const noexcept {                                                                                    \
        log(logging_type::logging_name,                                                                   \
            stl::forward<decltype(category)>(category),                                                   \
            stl::forward<decltype(details)>(details));                                                    \
    }                                                                                                     \
                                                                                                          \
    void logging_name(istl::StringViewifiable auto&& category,                                            \
                      istl::StringViewifiable auto&& details,                                             \
                      stl::error_code const&         ec) const noexcept {                                         \
        if constexpr (!is_debug) {                                                                        \
            stl::size_t space_count = 6 + logging_type_string_size(logging_type::logging_name) +          \
                                      istl::string_viewify(category).size();                              \
            auto old_details = istl::string_viewify(stl::forward<decltype(details)>(details));            \
            auto new_details = fmt::format("{2}\n{1: >{0}}error message: {3}",                            \
                                           stl::move(space_count),                                        \
                                           "",                                                            \
                                           old_details,                                                   \
                                           ec.message());                                                 \
            log(logging_type::logging_name,                                                               \
                stl::forward<decltype(category)>(category),                                               \
                stl::move(new_details));                                                                  \
        }                                                                                                 \
    }                                                                                                     \
                                                                                                          \
    void logging_name(istl::StringViewifiable auto&& category,                                            \
                      istl::StringViewifiable auto&& details,                                             \
                      stl::exception const&          ex) const noexcept {                                          \
        if constexpr (!is_debug) {                                                                        \
            stl::size_t space_count = 6 + logging_type_string_size(logging_type::logging_name) +          \
                                      istl::string_viewify(category).size();                              \
            auto old_details = istl::string_viewify(stl::forward<decltype(details)>(details));            \
            auto new_details = fmt::format("{2}\n{1: >{0}}error message: {3}",                            \
                                           stl::move(space_count),                                        \
                                           "",                                                            \
                                           old_details,                                                   \
                                           ex.what());                                                    \
            log(logging_type::logging_name,                                                               \
                stl::forward<decltype(category)>(category),                                               \
                stl::move(new_details));                                                                  \
        }                                                                                                 \
    }                                                                                                     \
                                                                                                          \
    void logging_name(istl::StringViewifiable auto&& details, stl::error_code const& ec) const noexcept { \
        return logging_name(default_category_name, stl::forward<decltype(details)>(details), ec);         \
    }                                                                                                     \
                                                                                                          \
    void logging_name(istl::StringViewifiable auto&& details, stl::exception const& ex) const noexcept {  \
        return logging_name(default_category_name, stl::forward<decltype(details)>(details), ex);         \
    }


        WEBPP_LOGGER_SHORTCUT(info)
        WEBPP_LOGGER_SHORTCUT(warning)
        WEBPP_LOGGER_SHORTCUT(error)
        WEBPP_LOGGER_SHORTCUT(critical)
        WEBPP_LOGGER_SHORTCUT(unkown)


        [[no_unique_address]] struct std_logger_debugger {

            WEBPP_LOGGER_SHORTCUT(info)
            WEBPP_LOGGER_SHORTCUT(warning)
            WEBPP_LOGGER_SHORTCUT(error)
            WEBPP_LOGGER_SHORTCUT(critical)
            WEBPP_LOGGER_SHORTCUT(unkown)
        } debug{};
#undef WEBPP_LOGGER_SHORTCUT
    };

    inline auto stderr_functor() noexcept {
        return stderr;
    }

    inline auto stdout_functor() noexcept {
        return stderr;
    }

    using stderr_logger = std_logger<stderr_functor>;
    using stdout_logger = std_logger<stdout_functor>;

} // namespace webpp

#endif // WEBPP_STD_LOGGER_HPP
