// Created by moisrex on 5/5/20.

#ifndef WEBPP_APPLICATION_H
#define WEBPP_APPLICATION_H

#include "./request_concepts.hpp"
#include "./response_concepts.hpp"
#include "./routes/context_concepts.hpp"
#include "./status_code.hpp"
#include "../application/application_concepts.hpp"
#include "../std/type_traits.hpp"

#include <cstdint>

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
     * This type helps to initialize the application and make sure the correct things get passed to the
     * application constructor.
     *
     * todo: add rebind feature here
     * todo: add other version of constructor as well here
     */
    template <Traits TraitsType, typename AppType>
    struct http_app_wrapper : public stl::remove_cvref_t<AppType> {
        using application_type = stl::remove_cvref_t<AppType>;
        using traits_type      = stl::remove_cvref_t<TraitsType>;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
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


        template <typename AllocType = allocator_type>
        requires(app_requires_logger_and_allocator<AllocType>)
          http_app_wrapper(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : application_type{logger, alloc} {}

        template <typename AllocType = allocator_type>
        requires(app_requires_logger && !app_requires_logger_and_allocator<AllocType>)
          http_app_wrapper(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : application_type{logger} {}

        template <typename AllocType = allocator_type>
        requires(app_requires_allocator<AllocType> && !app_requires_logger_and_allocator<AllocType> &&
                 !app_requires_logger)
          http_app_wrapper(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : application_type{alloc} {}

        template <typename AllocType = allocator_type>
        requires(app_requires_nothing && !app_requires_allocator<AllocType> && !app_requires_logger &&
                 !app_requires_logger_and_allocator<AllocType>)
          http_app_wrapper(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : application_type{} {}


        /**
         * The default error message provider; if the application doesn't provide one, we use this as the
         * default message provider function to generate error messages.
         *
         * todo: replace status code with a more sophisticated error type that can hold more information
         */
        [[nodiscard]] Response auto error(Request auto const& req, http::status_code err) {
            if constexpr (requires{
                            {application_type::error(req, err)} -> Response;
                          }) {
                return application_type::error(req, err);
            } else {
                return stl::format(FMT_COMPILE(
                                  "<!doctype html>\n"
                                     "<html>\n"
                                     "  <head>\n"
                                     "    <title>{0} - {1}</title>\n"
                                     "  <head>\n"
                                     "  <body>\n"
                                     "    <h1>{0} - {1}</h1>\n"
                                     "  </body>\n"
                                     "</html>\n"
                ), err, http::status_code_reason_phrase(err));
            }
        }

    };

} // namespace webpp

#endif // WEBPP_APPLICATION_H
