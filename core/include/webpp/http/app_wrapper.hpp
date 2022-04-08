// Created by moisrex on 5/5/20.

#ifndef WEBPP_HTTP_APPLICATION_WRAPPER_H
#define WEBPP_HTTP_APPLICATION_WRAPPER_H

#include "../application/application_concepts.hpp"
#include "../std/type_traits.hpp"
#include "request_concepts.hpp"
#include "response_concepts.hpp"
#include "routes/context_concepts.hpp"
#include "status_code.hpp"

#include <cstdint>

namespace webpp::http {

    /**
     * This is the application master which lets the user to combine multiple
     * application and use them as a single application and pass to the
     * protocols when they need to.
     */
    template <Application... AppTypes>
    struct master_application : private AppTypes... {
        HTTPResponse auto operator()(Context auto& ctx) noexcept {
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
    template <Traits TraitsType, Application AppType>
    struct http_app_wrapper : public AppType {
        using application_type        = AppType;
        using traits_type             = TraitsType;
        using logger_type             = traits::logger<traits_type>;
        using logger_ref              = typename logger_type::logger_ref;
        using string_view_type        = traits::string_view<traits_type>;
        using char_type               = istl::char_type_of<string_view_type>;
        using general_char_alloc_type = traits::general_allocator<traits_type, char_type>;
        using allocator_pack_type     = traits::allocator_pack_type<traits_type>;

        template <typename AllocType>
        static constexpr bool app_requires_logger_and_allocator =
          ConstructibleWithLoggerAndAllocator<application_type, logger_ref, AllocType>;

        template <typename AllocType>
        static constexpr bool app_requires_allocator =
          ConstructibleWithAllocator<application_type, AllocType>;

        static constexpr bool app_requires_logger  = ConstructibleWithLogger<application_type, logger_ref>;
        static constexpr bool app_requires_nothing = stl::is_default_constructible_v<application_type>;


        // ctor that passes the enabled_traits object to daddy :)
        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, ETT, Args...>)
        http_app_wrapper(ETT& et_obj, Args&&... args)
          : application_type{et_obj, stl::forward<Args>(args)...} {}

        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, Args..., ETT> &&
                     !stl::is_constructible_v<application_type, ETT, Args...>)
        http_app_wrapper(ETT& et_obj, Args&&... args)
          : application_type{et_obj, stl::forward<Args>(args)..., et_obj} {}

        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, logger_ref, allocator_pack_type, Args...> &&
                     !stl::is_constructible_v<application_type, Args..., ETT> &&
                     !stl::is_constructible_v<application_type, ETT, Args...>)
        http_app_wrapper(ETT& et_obj, Args&&... args)
          : application_type{et_obj.logger, et_obj.alloc_pack, stl::forward<Args>(args)...} {}

        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, allocator_pack_type, logger_ref, Args...> &&
                     !stl::is_constructible_v<application_type, logger_ref, allocator_pack_type, Args...> &&
                     !stl::is_constructible_v<application_type, Args..., ETT> &&
                     !stl::is_constructible_v<application_type, ETT, Args...>)
        http_app_wrapper(ETT& et_obj, Args&&... args)
          : application_type{et_obj.alloc_pack, et_obj.logger, stl::forward<Args>(args)...} {}

        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, allocator_pack_type, Args...> &&
                     !stl::is_constructible_v<application_type, allocator_pack_type, logger_ref, Args...> &&
                     !stl::is_constructible_v<application_type, logger_ref, allocator_pack_type, Args...> &&
                     !stl::is_constructible_v<application_type, Args..., ETT> &&
                     !stl::is_constructible_v<application_type, ETT, Args...>)
        http_app_wrapper(ETT& et_obj, Args&&... args)
          : application_type{et_obj.alloc_pack, stl::forward<Args>(args)...} {}

        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, logger_ref, Args...> &&
                     !stl::is_constructible_v<application_type, allocator_pack_type, Args...> &&
                     !stl::is_constructible_v<application_type, allocator_pack_type, logger_ref, Args...> &&
                     !stl::is_constructible_v<application_type, logger_ref, allocator_pack_type, Args...> &&
                     !stl::is_constructible_v<application_type, Args..., ETT> &&
                     !stl::is_constructible_v<application_type, ETT, Args...>)
        http_app_wrapper(ETT& et_obj, Args&&... args)
          : application_type{et_obj.logger, stl::forward<Args>(args)...} {}

        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, Args...> &&
                     !stl::is_constructible_v<application_type, logger_ref, Args...> &&
                     !stl::is_constructible_v<application_type, allocator_pack_type, Args...> &&
                     !stl::is_constructible_v<application_type, allocator_pack_type, logger_ref, Args...> &&
                     !stl::is_constructible_v<application_type, logger_ref, allocator_pack_type, Args...> &&
                     !stl::is_constructible_v<application_type, Args..., ETT> &&
                     !stl::is_constructible_v<application_type, ETT, Args...>)
        http_app_wrapper(ETT&, Args&&... args) : application_type{stl::forward<Args>(args)...} {}

        // todo: add support for Allocator constructors and even references to other stuff


        /**
         * The default error message provider; if the application doesn't provide one, we use this as the
         * default message provider function to generate error messages.
         *
         * todo: replace status code with a more sophisticated error type that can hold more information
         */
        [[nodiscard]] HTTPResponse auto error(HTTPRequest auto& req, http::status_code err) {
            if constexpr (requires {
                              { application_type::error(req, err) } -> HTTPResponse;
                          }) {
                return application_type::error(req, err);
            } else {
                return fmt::format("<!doctype html>\n"
                                   "<html>\n"
                                   "  <head>\n"
                                   "    <title>{0} - {1}</title>\n"
                                   "  <head>\n"
                                   "  <body>\n"
                                   "    <h1>{0} - {1}</h1>\n"
                                   "  </body>\n"
                                   "</html>\n",
                                   err,
                                   http::status_code_reason_phrase(err));
            }
        }
    };

} // namespace webpp::http

#endif
