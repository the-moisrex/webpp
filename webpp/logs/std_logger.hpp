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

    namespace details {
        enum struct logging_type : stl::uint_fast8_t { info, warning, error, critical, unknown };
    }

    /**
     * A logger class
     * @tparam stream_getter is a callable that gets a FILE* (like stderr, stdin, stdout)
     */
    template <auto stream_getter, bool IsDebug = false>
    struct std_logger {
        using logger_type = std_logger;
        using logger_ref  = logger_type;  // copy the logger, there's nothing to copy
        using logger_ptr  = logger_type*; // there's a syntax difference, so we can't copy

        static constexpr bool enabled = true; // todo: make this configurable by the user
#if defined(DEBUG) && DEBUG
        static constexpr bool is_debug = true;
#else
        static constexpr bool is_debug = false;
#endif
        static constexpr auto default_category_name = is_debug ? "Debug" : "Default";


        static constexpr auto logging_type_to_string(details::logging_type lt) noexcept {
            switch (lt) {
                using enum details::logging_type;
                case info: return "INFO";
                case warning: return "WARNING";
                case error: return "ERROR";
                case critical: return "CRITICAL";
                case unknown: return "UNKNOWN";
            }
            return "UNSPECIFIED";
        }

        static constexpr stl::size_t logging_type_string_size(details::logging_type lt) noexcept {
            return stl::string_view{logging_type_to_string(lt)}.size();
        }

        template <istl::StringViewifiable CatStrT, istl::StringViewifiable DetStrT>
        static void log(details::logging_type lt, CatStrT&& category, DetStrT&& details) noexcept {
            if constexpr (!is_debug) {
#ifdef WEBPP_FMT_LIB
                fmt::print(stream_getter(),
                           "[{}, {}]: {}\n",
                           logging_type_to_string(lt),
                           stl::forward<CatStrT>(category),
                           stl::forward<DetStrT>(details));
#else
                stl::fprintf(stream_getter(),
                             "[%s, %s]: %s\n",
                             logging_type_to_string(lt),
                             istl::string_viewify(stl::forward<CatStrT>(category)).data(),
                             istl::string_viewify(stl::forward<DetStrT>(details)).data());
#endif
            }
        }

#define WEBPP_LOGGER_SHORTCUT(logging_name)                                                               \
                                                                                                          \
    template <istl::StringViewifiable DetStrT>                                                            \
    void logging_name(DetStrT&& details) const noexcept {                                                 \
        log(details::logging_type::logging_name, default_category_name, stl::forward<DetStrT>(details));  \
    }                                                                                                     \
                                                                                                          \
    template <istl::StringViewifiable CatStrT, istl::StringViewifiable DetStrT>                           \
    void logging_name(CatStrT&& category, DetStrT&& details) const noexcept {                             \
        log(details::logging_type::logging_name,                                                          \
            stl::forward<CatStrT>(category),                                                              \
            stl::forward<DetStrT>(details));                                                              \
    }                                                                                                     \
                                                                                                          \
    template <istl::StringViewifiable CatStrT, istl::StringViewifiable DetStrT>                           \
    void logging_name(CatStrT&& category, DetStrT&& details, stl::error_code const& ec) const noexcept {  \
        if constexpr (!is_debug) {                                                                        \
            stl::size_t space_count = 6 + logging_type_string_size(details::logging_type::logging_name) + \
                                      istl::string_viewify(category).size();                              \
            auto old_details = istl::string_viewify(stl::forward<DetStrT>(details));                      \
            auto new_details = fmt::format("{2}\n{1: >{0}}error message: {3}",                            \
                                           stl::move(space_count),                                        \
                                           "",                                                            \
                                           old_details,                                                   \
                                           ec.message());                                                 \
            log(details::logging_type::logging_name,                                                      \
                stl::forward<CatStrT>(category),                                                          \
                stl::move(new_details));                                                                  \
        }                                                                                                 \
    }                                                                                                     \
                                                                                                          \
    template <istl::StringViewifiable CatStrT, istl::StringViewifiable DetStrT>                           \
    void logging_name(CatStrT&& category, DetStrT&& details, stl::exception const& ex) const noexcept {   \
        if constexpr (!is_debug) {                                                                        \
            stl::size_t space_count = 6 + logging_type_string_size(details::logging_type::logging_name) + \
                                      istl::string_viewify(category).size();                              \
            auto old_details = istl::string_viewify(stl::forward<DetStrT>(details));                      \
            auto new_details = fmt::format("{2}\n{1: >{0}}error message: {3}",                            \
                                           stl::move(space_count),                                        \
                                           "",                                                            \
                                           old_details,                                                   \
                                           ex.what());                                                    \
            log(details::logging_type::logging_name,                                                      \
                stl::forward<CatStrT>(category),                                                          \
                stl::move(new_details));                                                                  \
        }                                                                                                 \
    }                                                                                                     \
                                                                                                          \
    template <istl::StringViewifiable StrT>                                                               \
    void logging_name(StrT&& details, stl::error_code const& ec) const noexcept {                         \
        return logging_name(default_category_name, stl::forward<StrT>(details), ec);                      \
    }                                                                                                     \
                                                                                                          \
    template <istl::StringViewifiable StrT>                                                               \
    void logging_name(StrT&& details, stl::exception const& ex) const noexcept {                          \
        return logging_name(default_category_name, stl::forward<StrT>(details), ex);                      \
    }


        WEBPP_LOGGER_SHORTCUT(info)
        WEBPP_LOGGER_SHORTCUT(warning)
        WEBPP_LOGGER_SHORTCUT(error)
        WEBPP_LOGGER_SHORTCUT(critical)
        WEBPP_LOGGER_SHORTCUT(unknown)


        [[no_unique_address]] struct std_logger_debugger {

            using logger_type             = std_logger;
            using logger_ref              = logger_type const; // copy the logger, there's nothing to copy
            using logger_ptr              = logger_type*; // there's a syntax difference, so we can't copy
            static constexpr bool enabled = true;         // todo: make this configurable by the user

            WEBPP_LOGGER_SHORTCUT(info)
            WEBPP_LOGGER_SHORTCUT(warning)
            WEBPP_LOGGER_SHORTCUT(error)
            WEBPP_LOGGER_SHORTCUT(critical)
            WEBPP_LOGGER_SHORTCUT(unknown)
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
