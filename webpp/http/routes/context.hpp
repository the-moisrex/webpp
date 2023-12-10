// Created by moisrex on 5/1/20.

#ifndef WEBPP_HTTP_ROUTES_CONTEXT_HPP
#define WEBPP_HTTP_ROUTES_CONTEXT_HPP

#include "../../traits/enable_traits.hpp"
#include "../../uri/path_traverser.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "router_concepts.hpp"

namespace webpp::http {


    namespace details {


        template <HTTPRequest RequestType>
        struct common_context_methods : public enable_traits<typename RequestType::traits_type> {
            using request_type  = RequestType;
            using traits_type   = typename request_type::traits_type;
            using etraits       = enable_traits<traits_type>;
            using response_type = simple_response<traits_type>;
            using request_ref   = request_type&;
            using request_cref  = request_type const&;

            template <HTTPRequest ReqT>
            constexpr common_context_methods(ReqT const& inp_req) noexcept : etraits{inp_req} {}

            template <Context CtxT>
                requires(stl::same_as<typename stl::remove_cvref_t<CtxT>::request_type, request_type>)
            constexpr common_context_methods(CtxT const& ctx) noexcept
              : common_context_methods{ctx.request} {}

            constexpr common_context_methods(common_context_methods&& ctx) noexcept        = default;
            constexpr common_context_methods(common_context_methods const& ctx) noexcept   = default;
            constexpr common_context_methods& operator=(common_context_methods const&)     = default;
            constexpr common_context_methods& operator=(common_context_methods&&) noexcept = default;
            constexpr ~common_context_methods()                                            = default;

            // todo: add more error handling templates here.
            // todo: let the user customize error templates with extensions
            // todo: add all the features of returning a response each body type should have at least one method here


            /**
             * Generate a response
             */
            template <typename... Args>
            [[nodiscard]] constexpr HTTPResponse auto create_response(Args&&... args) const noexcept {
                return response_type::create(*this, stl::forward<Args>(args)...);
            }

            /**
             * Generate a response while passing the specified arguments as the body of that response
             */
            template <typename... Args>
            [[nodiscard]] constexpr HTTPResponse auto response_body(Args&&... args) const noexcept {
                return with_body(*this, stl::forward<Args>(args)...);
            }

            [[nodiscard]] static constexpr bool is_debug() noexcept {
                // todo: configure this in cmake
#ifdef DEBUG
                return true;
#else
                return false;
#endif
            }

            [[nodiscard]] constexpr HTTPResponse auto error(http::status_code error_code) const noexcept {
                return error(static_cast<http::status_code_type>(error_code));
            }

            template <typename DataType>
            [[nodiscard]] constexpr HTTPResponse auto error(http::status_code error_code,
                                                            DataType&&        data) const noexcept {
                return error(static_cast<http::status_code_type>(error_code), stl::forward<DataType>(data));
            }

            /**
             * @brief Generate an HTTP response with an error message.
             *
             * This function generates an HTTP response with an error message based on the given error code.
             *
             * @param error_code The error code indicating the type of error.
             *
             * @return An HTTP response with the error message.
             */
            [[nodiscard]] constexpr HTTPResponse auto error(
              http::status_code_type error_code) const noexcept {
                using str_t = traits::string<traits_type>;
                str_t msg{general_alloc_for<str_t>(*this)};
                fmt::format_to(stl::back_inserter(msg),
                               R"(<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=0,viewport-fit=cover">
    <title>{0} Error: {1}</title>
  </head>
  <body>
    <h1>Error {0}: {1}</h1>
  </body>
</html>
)",
                               error_code,
                               http::status_code_reason_phrase(error_code));
                return error(error_code, stl::move(msg));
            }

            /**
             * @brief Generates an HTTP response for a given error code and data.
             *
             * This function generates an HTTP response based on the provided error code and data.
             *
             * @param error_code The error code to use for the response.
             * @param data The data to be used to construct the response.
             * @return An HTTP response generated based on the provided error code and data.
             */
            [[nodiscard]] constexpr HTTPResponse auto error(http::status_code_type error_code,
                                                            auto&&                 data) const noexcept {
                using data_type = stl::remove_cvref_t<decltype(data)>;
                if constexpr (istl::StringViewifiable<data_type>) {
                    // data type is a string
                    auto res = create_response(istl::string_viewify(data));
                    res.headers.status_code(error_code);
                    return res;
                } else if constexpr (requires {
                                         {
                                             data.what()
                                         } -> istl::StringViewifiable;
                                     })
                {
                    // standard exception, use .what to get the error message
                    auto res = create_response(data.what());
                    res.headers.status_code(error_code);
                    return res;
                } else {
                    // we don't know what the error message is, ignoring it
                    auto res = create_response();
                    res.headers.status_code(error_code);
                    return res;
                }
            }
        };
    } // namespace details

    template <HTTPRequest RequestType>
    struct common_context_view : public details::common_context_methods<RequestType> {
        using request_type       = RequestType;
        using traits_type        = typename request_type::traits_type;
        using response_type      = simple_response<traits_type>;
        using basic_context_type = common_context_view;
        using request_ref        = request_type&;
        using request_cref       = request_type const&;

      private:
        using context_methods = details::common_context_methods<RequestType>;

      public:
        // NOLINTBEGIN(*-non-private-member-variables-in-classes)
        request_ref request;

        // NOLINTEND(*-non-private-member-variables-in-classes)

        constexpr common_context_view(request_ref inp_req) noexcept
          : context_methods{inp_req},
            request{inp_req} {}

        template <Context CtxT>
            requires(stl::same_as<typename stl::remove_cvref_t<CtxT>::request_type, request_type>)
        constexpr common_context_view(CtxT const& ctx) noexcept : common_context_view{ctx.request} {}

        constexpr common_context_view(common_context_view&& ctx) noexcept        = default;
        constexpr common_context_view(common_context_view const& ctx) noexcept   = default;
        constexpr common_context_view& operator=(common_context_view const&)     = default;
        constexpr common_context_view& operator=(common_context_view&&) noexcept = default;
        constexpr ~common_context_view()                                         = default;
    };

    template <HTTPRequest ReqType>
    using simple_context = common_context_view<ReqType>;

    /**
     * The standard and dynamic context which will own its data
     */
    template <Traits TraitsType = default_dynamic_traits>
    struct basic_context : details::common_context_methods<basic_request<TraitsType>> {
        using traits_type         = TraitsType;
        using request_type        = basic_request<traits_type>;
        using static_context_type = simple_context<request_type>;
        using response_type       = basic_response<traits_type>;
        using string_type         = traits::string<traits_type>;
        using slug_type           = string_type;
        using path_traverser_type = uri::path_traverser<string_type>;
        using dynamic_route_type  = dynamic_route<traits_type>;
        using dynamic_route_ptr   = dynamic_route_type*;

        // NOLINTBEGIN(*-non-private-member-variables-in-classes)
        request_type  request;
        response_type response;
        // NOLINTEND(*-non-private-member-variables-in-classes)


      private:
        using context_methods = details::common_context_methods<basic_request<TraitsType>>;

        path_traverser_type traverser;
        dynamic_route_ptr   current_route_ptr = nullptr;

      public:
        template <HTTPRequest ReqT>
            requires(!istl::cvref_as<ReqT, request_type>)
        constexpr basic_context(ReqT& req)
          : context_methods{req},
            request{req},
            response{req.get_traits()},
            traverser{request.uri()} {}

        constexpr basic_context(request_type& req)
          : context_methods{req},
            request{req},
            response{req.get_traits()},
            traverser{request.uri()} {}

        template <Context CtxT>
            requires(!istl::cvref_as<CtxT, basic_context>)
        constexpr basic_context(CtxT const& ctx) noexcept : basic_context(ctx.request) {}

        constexpr basic_context(basic_context&& ctx) noexcept        = default;
        constexpr basic_context(basic_context const& ctx) noexcept   = default;
        constexpr basic_context& operator=(basic_context const&)     = default;
        constexpr basic_context& operator=(basic_context&&) noexcept = default;
        constexpr ~basic_context()                                   = default;

        /**
         * Clone this context
         */
        [[nodiscard]] constexpr auto clone() const noexcept {
            return basic_context{*this};
        }

        /**
         * @brief Check if a given variable is a valid path slug.
         */
        template <typename T>
        [[nodiscard]] constexpr bool check_segment(T&& slug) noexcept {
            return traverser.check_segment(stl::forward<T>(slug));
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return request.empty() && response.empty();
        }

        constexpr path_traverser_type& path_traverser() noexcept {
            return traverser;
        }

        constexpr path_traverser_type const& path_traverser() const noexcept {
            return traverser;
        }

        template <typename URIOrStringType>
        constexpr void reset_path(URIOrStringType&& new_path) {
            request.uri(stl::forward<URIOrStringType>(new_path));
            traverser = request.uri();
        }

        constexpr dynamic_route_type const& current_route() const noexcept {
            return *current_route_ptr;
        }

        constexpr dynamic_route_type& current_route() noexcept {
            return *current_route_ptr;
        }

        constexpr void current_route(dynamic_route_type& new_route) noexcept {
            current_route_ptr = stl::addressof(new_route);
            traverser.reset();
        }
    };

    using context = basic_context<>;

} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTES_CONTEXT_HPP
