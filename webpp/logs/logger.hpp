#ifndef WEBPP_LOGGER_DEFAULT_HPP
#define WEBPP_LOGGER_DEFAULT_HPP

#include "../traits/dynamic_scoping.hpp"
#include "./log_concepts.hpp"
#include "./std_logger.hpp"

namespace webpp {

    template <typename DependentType = void, int priority = 1>
    struct default_logger_type : default_logger_type<DependentType, priority - 1> {};

    template <typename DependentType>
    struct default_logger_type<DependentType, 0> {
        using type = stderr_logger;
    };

    template <typename DependentType>
    using default_logger = typename default_logger_type<DependentType>::type;

    /// Default non-dynamic logger, this is being used as the default logger for the dynamic loggers.
    using default_static_logger = stderr_logger;

    /**
     * Logger Tag.
     * This logger points to the default logger type.
     * This is a Locally Bound Global.
     */
    template <Logger L = default_logger<void>, auto ID = 0>
    struct [[nodiscard]] basic_logger : global_binding<L, ID> {
        // The reason why we're not making the logger type itself a Locally Bound Global, is because the default logger
        // type is not yet known, so we make it in a way that the first use of `logger.X` will determine the logger's
        // global type and cannot be changed after that.
        using logger_type = L;

      private:
        static constexpr global_binding<logger_type> self{};

      public:
        template <typename... Args>
        void log(log_level const            level,
                 stl::predicate auto const& pred,
                 stl::string_view const     category,
                 stl::string_view const     details) const {
            if (pred()) {
                self->log(level, category, details);
            }
        }

        template <typename... Args>
        void log(log_level const level, stl::predicate auto const& pred, stl::string_view const details) const {
            if (pred()) {
                self->log(level, stl::string_view{"Default"}, details);
            }
        }

        template <typename... Args>
        void log(log_level const level, stl::string_view const details) const {
            self->log(level, stl::string_view{"Default"}, details);
        }

        template <typename... Args>
        void log(log_level const level, stl::string_view const category, stl::string_view const details) const {
            self->log(level, category, details);
        }

        // todo: add std::exception and std::error_code

        // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_LOGGER_SHORTCUT(NAME)                        \
    template <typename... Args>                            \
    void NAME(Args&&... args) const {                      \
        log(log_level::NAME, stl::forward<Args>(args)...); \
    }

        WEBPP_LOGGER_SHORTCUT(trace)
        WEBPP_LOGGER_SHORTCUT(debug)
        WEBPP_LOGGER_SHORTCUT(info)
        WEBPP_LOGGER_SHORTCUT(warn)
        WEBPP_LOGGER_SHORTCUT(warning)
        WEBPP_LOGGER_SHORTCUT(err)
        WEBPP_LOGGER_SHORTCUT(error)
        WEBPP_LOGGER_SHORTCUT(critical)

#undef WEBPP_LOGGER_SHORTCUT
    };

    static constexpr basic_logger<> logger;


} // namespace webpp

#endif // WEBPP_LOGGER_DEFAULT_HPP
