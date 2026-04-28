// Created by moisrex on 8/16/20.

#ifndef WEBPP_STD_LOGGER_HPP
#define WEBPP_STD_LOGGER_HPP

#include "../std/string_view.hpp"
#include "./log_concepts.hpp"

#include <cstdio>
#ifdef __cpp_lib_print
#    include <print>
#else
#    include "../std/format.hpp"
#endif

namespace webpp {

    /**
     * A logger class
     * @tparam stream_getter is a callable that gets a FILE* (like stderr, stdin, stdout)
     */
    template <auto stream_getter>
    struct [[nodiscard]] std_logger {
        static void log(log_level const        level,
                        stl::string_view const category,
                        stl::string_view const details) noexcept {
#ifdef __cpp_lib_print
            std::print(stream_getter(), "[{}, {}]: {}\n", to_string(level), category, details);
#elif defined(WEBPP_FMT_LIB)
            fmt::print(stream_getter(), "[{}, {}]: {}\n", to_string(level), category, details);
#else
            stl::fprintf(stream_getter(), "[%s, %s]: %s\n", to_string(level).data(), category.data(), details.data());
#endif
        }

        static void log(log_level const        level,
                        stl::string_view const category,
                        stl::string_view const details,
                        stl::string_view       more_details) noexcept {
#ifdef __cpp_lib_print
            std::print(stream_getter(), "[{}, {}]: {}. {}\n", to_string(level), category, details, more_details);
#elif defined(WEBPP_FMT_LIB)
            fmt::print(stream_getter(), "[{}, {}]: {}. {}\n", to_string(level), category, details, more_details);
#else
            stl::fprintf(stream_getter(),
                         "[%s, %s]: %s. %s\n",
                         to_string(level).data(),
                         category.data(),
                         details.data(),
                         more_details.data());
#endif
        }
    };

    inline auto stderr_functor() noexcept {
        return stderr;
    }

    inline auto stdout_functor() noexcept {
        return stdout;
    }

    using stderr_logger = std_logger<stderr_functor>;
    using stdout_logger = std_logger<stdout_functor>;

} // namespace webpp

#endif // WEBPP_STD_LOGGER_HPP
