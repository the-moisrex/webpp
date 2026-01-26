// Created by moisrex on 8/16/20.

#ifndef WEBPP_STD_LOGGER_HPP
#define WEBPP_STD_LOGGER_HPP

#include "../common/meta.hpp"
#include "../std/format.hpp"
#include "../std/string_view.hpp"
#include "../traits/traits.hpp"

#include <cstdio>

namespace webpp {

    namespace details {
        enum struct logging_type : stl::uint_fast8_t {
            info,
            warning,
            error,
            critical,
            unknown
        };
    } // namespace details

    /**
     * A logger class
     * @tparam stream_getter is a callable that gets a FILE* (like stderr, stdin, stdout)
     */
    template <auto stream_getter, bool IsDebug = is_debug_build>
    struct [[nodiscard]] std_logger {
        using logger_type = std_logger;
        using logger_ref  = logger_type;  // copy the logger, there's nothing to copy
        using logger_ptr  = logger_type*; // there's a syntax difference, so we can't copy

        static constexpr bool is_debug              = IsDebug;
        static constexpr auto default_category_name = is_debug ? "Debug" : "Default";

        static constexpr auto logging_type_to_string(details::logging_type const ltype) noexcept {
            switch (ltype) {
                using enum details::logging_type;
                case info: return "INFO";
                case warning: return "WARNING";
                case error: return "ERROR";
                case critical: return "CRITICAL";
                case unknown: return "UNKNOWN";
            }
            return "UNSPECIFIED";
        }

        static constexpr stl::size_t logging_type_string_size(details::logging_type const ltype) noexcept {
            return stl::string_view{logging_type_to_string(ltype)}.size();
        }

        template <typename CharT>
        static void log(details::logging_type               ltype,
                        stl::basic_string_view<CharT> const category,
                        stl::basic_string_view<CharT>       details) noexcept {
            if constexpr (!is_debug) {
#ifdef WEBPP_FMT_LIB
                fmt::print(stream_getter(), "[{}, {}]: {}\n", logging_type_to_string(ltype), category, details);
#else
                stl::fprintf(stream_getter(),
                             "[%s, %s]: %s\n",
                             logging_type_to_string(ltype),
                             category.data(),
                             details.data());
#endif
            }
        }

#define WEBPP_LOGGER_SHORTCUT(logging_name)                                                                           \
                                                                                                                      \
    template <typename CharT>                                                                                         \
    void logging_name(stl::basic_string_view<CharT> const details) const noexcept {                                   \
        log(details::logging_type::logging_name, default_category_name, details);                                     \
    }                                                                                                                 \
                                                                                                                      \
    template <typename CharT>                                                                                         \
    void logging_name(stl::basic_string_view<CharT> const category, stl::basic_string_view<CharT> const details)      \
      const noexcept {                                                                                                \
        log(details::logging_type::logging_name, category, details);                                                  \
    }                                                                                                                 \
                                                                                                                      \
    template <typename CharT>                                                                                         \
    void logging_name(stl::basic_string_view<CharT> const category,                                                   \
                      stl::basic_string_view<CharT> const details,                                                    \
                      stl::error_code const&              ec) const noexcept {                                                     \
        if constexpr (!is_debug) {                                                                                    \
            stl::size_t space_count =                                                                                 \
              6 + logging_type_string_size(details::logging_type::logging_name) + istl::view(category).size();        \
            auto old_details = istl::view(details);                                                                   \
            auto new_details =                                                                                        \
              fmt::format("{2}\n{1: >{0}}error message: {3}", stl::move(space_count), "", old_details, ec.message()); \
            log(details::logging_type::logging_name, category, stl::move(new_details));                               \
        }                                                                                                             \
    }                                                                                                                 \
                                                                                                                      \
    template <typename CharT>                                                                                         \
    void logging_name(stl::basic_string_view<CharT> const category,                                                   \
                      stl::basic_string_view<CharT> const details,                                                    \
                      stl::exception const&               ex) const noexcept {                                                      \
        if constexpr (!is_debug) {                                                                                    \
            stl::size_t space_count =                                                                                 \
              6 + logging_type_string_size(details::logging_type::logging_name) + istl::view(category).size();        \
            auto old_details = istl::view(details);                                                                   \
            auto new_details =                                                                                        \
              fmt::format("{2}\n{1: >{0}}error message: {3}", stl::move(space_count), "", old_details, ex.what());    \
            log(details::logging_type::logging_name, category, stl::move(new_details));                               \
        }                                                                                                             \
    }                                                                                                                 \
                                                                                                                      \
    template <typename CharT>                                                                                         \
    void logging_name(stl::basic_string_view<CharT> const details, stl::error_code const& ec) const noexcept {        \
        logging_name(default_category_name, details, ec);                                                             \
    }                                                                                                                 \
                                                                                                                      \
    template <typename CharT>                                                                                         \
    void logging_name(stl::basic_string_view<CharT> const details, stl::exception const& ex) const noexcept {         \
        logging_name(default_category_name, details, ex);                                                             \
    }                                                                                                                 \
                                                                                                                      \
    template <typename... OptsT>                                                                                      \
    void logging_name(if_debug_tag, OptsT&&... opts) const noexcept {                                                 \
        if constexpr (is_debug) {                                                                                     \
            this->logging_name(stl::forward<OptsT>(opts)...);                                                         \
        }                                                                                                             \
    }


        WEBPP_LOGGER_SHORTCUT(info)
        WEBPP_LOGGER_SHORTCUT(warning)
        WEBPP_LOGGER_SHORTCUT(error)
        WEBPP_LOGGER_SHORTCUT(critical)
        WEBPP_LOGGER_SHORTCUT(unknown)


#undef WEBPP_LOGGER_SHORTCUT
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
