// Created by moisrex on 5/5/20.

#ifndef WEBPP_HTTP_APPLICATION_WRAPPER_H
#define WEBPP_HTTP_APPLICATION_WRAPPER_H

#include "../application/application_concepts.hpp"
#include "../common/meta.hpp"
#include "../std/type_traits.hpp"
#include "http_concepts.hpp"
#include "routes/router_concepts.hpp"
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

        struct enable_throws {};

        // ctor that passes the enabled_traits object to daddy :)
        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, ETT, Args...>)
        explicit constexpr http_app_wrapper(ETT& et_obj, Args&&... args)
          : application_type{et_obj, stl::forward<Args>(args)...} {}

        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, Args..., ETT> &&
                     !stl::is_constructible_v<application_type, ETT, Args...>)
        explicit constexpr http_app_wrapper(ETT& et_obj, Args&&... args)
          : application_type{et_obj, stl::forward<Args>(args)..., et_obj} {}

        template <EnabledTraits ETT, typename... Args>
            requires(stl::is_constructible_v<application_type, Args...> &&
                     !stl::is_constructible_v<application_type, Args..., ETT> &&
                     !stl::is_constructible_v<application_type, ETT, Args...>)
        explicit constexpr http_app_wrapper([[maybe_unused]] ETT& etraits, Args&&... args)
          : application_type{stl::forward<Args>(args)...} {}

        [[nodiscard]] constexpr HTTPResponse auto response(HTTPRequest auto& req) {
            if constexpr (requires {
                              {
                                  application_type::response(req)
                              } -> HTTPResponse;
                          })
            {
                return application_type::response(req);
            } else if constexpr (requires {
                                     {
                                         application_type::response()
                                     } -> HTTPResponse;
                                 })
            {
                return application_type::response();
            } else {
                return req.response();
            }
        }

        constexpr void error(HTTPRequest auto& req, http::status_code err, HTTPResponse auto& res) {
            if constexpr (requires { application_type::error(req, err, res); }) {
                application_type::error(req, err, res);
            } else if constexpr (requires { application_type::error(err, res); }) {
                application_type::error(err, res);
            } else if constexpr (requires { application_type::error(err, req, res); }) {
                application_type::error(err, req, res);
            } else if constexpr (requires { application_type::error(req, res, err); }) {
                application_type::error(req, res, err);
            } else if constexpr (requires { application_type::error(res, req, err); }) {
                application_type::error(res, req, err);
            } else if constexpr (requires { application_type::error(res, err, req); }) {
                application_type::error(res, err, req);
            } else {
                res      = err;
                res.body = fmt::format(
                  "<!DOCTYPE html>\n"
                  "<html>\n"
                  "  <head>\n"
                  "    <title>{0} - {1}</title>\n"
                  "  </head>\n"
                  "  <body>\n"
                  "    <h1>{0} - {1}</h1>\n"
                  "  </body>\n"
                  "</html>\n",
                  static_cast<status_code_type>(err),
                  http::status_code_reason_phrase(err));
            }
        }

        /**
         * The default error message provider; if the application doesn't provide one, we use this as the
         * default message provider function to generate error messages.
         *
         * todo: handle exceptions that get thrown from here as well
         * todo: add more ways of printing the errors and logging them
         */
        [[nodiscard]] constexpr HTTPResponse auto error(HTTPRequest auto& req, http::status_code err) {
            if constexpr (requires {
                              {
                                  application_type::error(req, err)
                              } -> HTTPResponse;
                          })
            {
                return application_type::error(req, err);
            } else if constexpr (requires {
                                     {
                                         application_type::error(err)
                                     } -> HTTPResponse;
                                 })
            {
                return application_type::error(err);
            } else if constexpr (requires {
                                     {
                                         application_type::error(err, req)
                                     } -> HTTPResponse;
                                 })
            {
                return application_type::error(err, req);
            } else {
                auto res = response(req);
                error(req, err, res);
                return res;
            }
        }

        template <HTTPRequest ReqType>
        [[nodiscard]] constexpr HTTPResponse auto operator()(ReqType&& req) noexcept {
            if constexpr (stl::is_nothrow_invocable_v<application_type, ReqType>) {
                return fix_response(req, application_type::operator()(req));
            } else if constexpr (stl::is_nothrow_invocable_v<application_type>) {
                return fix_response(req, application_type::operator()());
            } else if constexpr (stl::is_invocable_v<application_type, ReqType>) {
                using request_type = stl::remove_cvref_t<ReqType>;
                using etraits_type = typename request_type::enable_traits_type;
                using return_type  = stl::invoke_result_t<application_type, ReqType>;
                if constexpr (stl::is_constructible_v<return_type, etraits_type>) {
                    try {
                        return operator()(stl::forward<ReqType>(req), enable_throws{});
                    } catch (stl::exception const& ex) {
                        // todo: log
                        return_type res{req.get_traits()};
                        error(req, http::status_code::internal_server_error, res);
                        return res;
                    } catch (...) {
                        // todo: log
                        return_type res{req.get_traits()};
                        error(req, http::status_code::internal_server_error, res);
                        return res;
                    }
                } else if constexpr (stl::is_default_constructible_v<return_type>) {
                    try {
                        return operator()(stl::forward<ReqType>(req), enable_throws{});
                    } catch (stl::exception const& ex) {
                        // todo: log
                        return_type res;
                        error(req, http::status_code::internal_server_error, res);
                        return res;
                    } catch (...) {
                        // todo: log
                        return_type res;
                        error(req, http::status_code::internal_server_error, res);
                        return res;
                    }
                } else {
                    static_assert_false(ReqType,
                                        "We know how to call your app, but we don't know how "
                                        "to initialize your response type in case an error is "
                                        "thrown from your app; mark your operator() as 'noexcept'.");
                }
            } else if constexpr (stl::is_invocable_v<application_type>) {
                using return_type = stl::invoke_result_t<application_type>;
                if constexpr (stl::is_constructible_v<return_type, ReqType>) {
                    try {
                        return operator()(req, enable_throws{});
                    } catch (stl::exception const& ex) {
                        // todo: log
                        return_type res{req};
                        error(req, http::status_code::internal_server_error, res);
                        return res;
                    } catch (...) {
                        // todo: log
                        return_type res{req};
                        error(req, http::status_code::internal_server_error, res);
                        return res;
                    }
                } else if constexpr (stl::is_default_constructible_v<return_type>) {
                    try {
                        return operator()(req, enable_throws{});
                    } catch (stl::exception const& ex) {
                        // todo: log
                        return_type res;
                        error(req, http::status_code::internal_server_error, res);
                        return res;
                    } catch (...) {
                        // todo: log
                        return_type res;
                        error(req, http::status_code::internal_server_error, res);
                        return res;
                    }
                } else {
                    static_assert_false(ReqType,
                                        "We know how to call your app, but we don't know how "
                                        "to initialize your response type in case an error is "
                                        "thrown from your app; mark your operator() as 'noexcept'.");
                }
            } else {
                static_assert_false(application_type,
                                    "We don't know how to call your application, "
                                    "it doesn't have app::operator()(request) or app::operator()().");
            }
        }

        // enable_throws will enable throwing the exceptions and won't catch them here.
        // The usage of this is when you want to catch the exceptions yourself, maybe you already have a
        // mechanism to catch per-request exceptions, and you want to use that one instead of response-only
        // exception catching.
        // For example, reading response headers and bodies may throw exceptions, and you want to catch both
        // of them in one go instead of two passes.
        template <HTTPRequest ReqType>
        [[nodiscard]] constexpr HTTPResponse auto operator()(ReqType&& req, enable_throws) {
            if constexpr (stl::is_invocable_v<application_type, ReqType>) {
                return fix_response(req, application_type::operator()(req));
            } else if constexpr (stl::is_invocable_v<application_type>) {
                return fix_response(req, application_type::operator()());
            } else {
                static_assert_false(application_type,
                                    "We don't know how to call your application, "
                                    "it doesn't have app::operator()(request) or app::operator()().");
            }
        }

      private:
        /**
         * Final conversion practices of the response body happens here.
         */
        template <typename ResType, typename ReqType>
        [[nodiscard]] constexpr HTTPResponse auto fix_response(ReqType&& req, ResType&& res) noexcept {
            using res_type = stl::remove_cvref_t<ResType>;
            // todo: add more fixes
            if constexpr (stl::same_as<res_type, http::status_code>) {
                return error(req, res);
            } else if constexpr (HTTPResponse<res_type>) {
                return res;
            } else {
                static_assert_false(res_type,
                                    "The return type of your application is not a valid response "
                                    "type that our Protocols can read and send to the users.");
            }
        }
    };

} // namespace webpp::http

#endif
