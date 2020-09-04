// Created by moisrex on 5/5/20.

#ifndef WEBPP_APPLICATION_H
#define WEBPP_APPLICATION_H

#include "../traits/enable_traits.hpp"
#include "./application_concepts.hpp"
#include "./routes/context_concepts.hpp"

#include <cstdint>
#include <type_traits>

namespace webpp {

    /**
     * This is the application master which lets the user to combine multiple
     * application and use them as a single application and pass to the
     * protocols when they need to.
     */
    template <Application... AppTypes>
    struct master_application : private AppTypes... {
        Response auto operator()(Context auto& ctx) noexcept {
            (AppTypes(ctx), ...); // todo: this is not correct
        }
    };

    /**
     * This type is designed for the interfaces to inherit from in which it helps to initialize the
     * application and make sure the correct things get passed to the application constructor.
     */
    template <Traits TraitsType, typename AppType>
    struct application_wrapper : public enable_traits<stl::remove_cvref_t<TraitsType>> {
        using application_type = stl::remove_cvref_t<AppType>;
        using traits_type      = stl::remove_cvref_t<TraitsType>;
        using etraits          = enable_traits<traits_type>;
        using logger_type      = typename etraits::logger_type;
        using logger_ref       = typename etraits::logger_ref;
        using char_type        = typename traits_type::char_type;
        using allocator_type   = typename traits_type::template allocator<char_type>;

        template <typename AllocType>
        static constexpr bool app_requires_logger_and_allocator =
          ConstructibleWithLoggerAndAllocator<application_type, logger_ref, AllocType>;

        template <typename AllocType>
        static constexpr bool app_requires_allocator =
          ConstructibleWithAllocator<application_type, AllocType>;

        static constexpr bool app_requires_logger  = ConstructibleWithLogger<application_type, logger_ref>;
        static constexpr bool app_requires_nothing = stl::is_default_constructible_v<application_type>;

        application_type app;

        template <typename AllocType = allocator_type>
        requires(app_requires_logger_and_allocator<AllocType>)
          application_wrapper(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{logger, alloc} {};

        template <typename AllocType = allocator_type>
        requires(app_requires_logger && !app_requires_logger_and_allocator<AllocType>)
          application_wrapper(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{logger} {};

        template <typename AllocType = allocator_type>
        requires(app_requires_allocator<AllocType> && !app_requires_logger_and_allocator<AllocType> &&
                 !app_requires_logger)
          application_wrapper(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{alloc} {};

        template <typename AllocType = allocator_type>
        requires(app_requires_nothing && !app_requires_allocator<AllocType> && !app_requires_logger &&
                 !app_requires_logger_and_allocator<AllocType>)
          application_wrapper(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{} {};
    };

} // namespace webpp

#endif // WEBPP_APPLICATION_H
