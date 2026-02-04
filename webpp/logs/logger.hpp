#ifndef WEBPP_LOGGER_DEFAULT_HPP
#define WEBPP_LOGGER_DEFAULT_HPP

#include "../traits/dynamic_scoping.hpp"
#include "std_logger.hpp"

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
     * This is act as a Locally Bound Global, even though it's not exactly implement it.
     */
    static constexpr struct [[nodiscard]] logger_tag {
        // The reason why we're not making the logger type itself a Locally Bound Global, is because the default logger
        // type is not yet known, so we make it in a way that the first use of `logger.X` will determine the logger's
        // global type and cannot be changed after that.

        // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_LOGGER_SHORTCUT(NAME)                                        \
    template <typename Arg1, typename... Args>                             \
    void NAME(Arg1&& arg1, Args&&... args) const noexcept {                \
        static constexpr global_binding<default_logger<Arg1>> self;        \
        self->info(stl::forward<Arg1>(arg1), stl::forward<Args>(args)...); \
    }

        WEBPP_LOGGER_SHORTCUT(info)
        WEBPP_LOGGER_SHORTCUT(warning)
        WEBPP_LOGGER_SHORTCUT(error)
        WEBPP_LOGGER_SHORTCUT(critical)
        WEBPP_LOGGER_SHORTCUT(unknown)

#undef WEBPP_LOGGER_SHORTCUT
    } logger;


} // namespace webpp

#endif // WEBPP_LOGGER_DEFAULT_HPP
