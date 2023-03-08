// Created by moisrex on 5/1/20.

#ifndef WEBPP_ROUTES_CONTEXT_H
#define WEBPP_ROUTES_CONTEXT_H

#include "../../extensions/extension.hpp"
#include "../../extensions/extension_wrapper.hpp"
#include "../../memory/object.hpp"
#include "../../traits/enable_traits.hpp"
#include "../../uri/path_traverser.hpp"
#include "../bodies/string.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "router_concepts.hpp"

namespace webpp::http {


    namespace details {


        template <HTTPRequest RequestType, typename RootExtensions>
        struct common_context_methods : public enable_traits<typename RequestType::traits_type> {
            using request_type    = RequestType;
            using traits_type     = typename request_type::traits_type;
            using etraits         = enable_traits<traits_type>;
            using root_extensions = RootExtensions;
            using response_type   = simple_response<traits_type, root_extensions>;
            using request_ref     = request_type&;
            using request_cref    = request_type const&;


            constexpr common_context_methods(request_ref inp_req) noexcept : etraits{inp_req} {}

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
            template <Extension... NewExtensions, typename... Args>
            [[nodiscard]] constexpr HTTPResponse auto create_response(Args&&... args) const noexcept {
                return response_type::template create<NewExtensions...>(*this, stl::forward<Args>(args)...);
            }


            /**
             * Generate a response while passing the specified arguments as the body of that response
             */
            template <Extension... NewExtensions, typename... Args>
            [[nodiscard]] constexpr HTTPResponse auto response_body(Args&&... args) const noexcept {
                using new_response_type =
                  typename response_type::template apply_extensions_type<NewExtensions...>;
                return new_response_type::with_body(*this, stl::forward<Args>(args)...);
            }


            [[nodiscard]] constexpr static bool is_debug() noexcept {
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

            [[nodiscard]] constexpr HTTPResponse auto
            error(http::status_code_type error_code) const noexcept {
                using str_t = traits::general_string<traits_type>;
                auto msg    = object::make_general<str_t>(this->alloc_pack);
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



            [[nodiscard]] constexpr HTTPResponse auto error(http::status_code_type error_code,
                                                            auto&&                 data) const noexcept {
                using data_type = stl::remove_cvref_t<decltype(data)>;
                if constexpr (istl::StringViewifiable<data_type>) {
                    auto res                = create_response(istl::string_viewify(data));
                    res.headers.status_code = error_code;
                    return res;
                } else if constexpr (requires {
                                         { data.what() } -> istl::StringViewifiable;
                                     }) {
                    auto res                = create_response(data.what());
                    res.headers.status_code = error_code;
                    return res;
                } else {
                    auto res                = create_response();
                    res.headers.status_code = error_code;
                    return res;
                }
            }
        };
    } // namespace details

    template <typename MergedRootExtensions>
    struct context_descriptor;


    template <HTTPRequest RequestType, typename EList, typename RootExtensions>
    struct common_context_view : public details::common_context_methods<RequestType, RootExtensions>,
                                 public extension_wrapper<EList> {
        using request_type           = RequestType;
        using traits_type            = typename request_type::traits_type;
        using mother_extensions_type = EList;
        using extension_wrapper_type = extension_wrapper<EList>;
        using root_extensions        = RootExtensions;
        using response_type          = simple_response<traits_type, root_extensions>;
        using basic_context_type     = common_context_view;
        using request_ref            = request_type&;
        using request_cref           = request_type const&;

      private:
        using context_methods = details::common_context_methods<RequestType, RootExtensions>;

      public:
        // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        request_ref request;
        // NOLINTEND(misc-non-private-member-variables-in-classes)
        // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

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

    template <typename EList>
    struct static_context_view : public EList {
        using base_type              = EList;
        using traits_type            = typename base_type::traits_type;
        using root_extensions        = typename base_type::root_extensions;
        using mother_extensions_type = typename root_extensions::template mother_extensions<traits_type>;
        using final_context_parent   = EList;
        using request_type           = typename base_type::request_type;
        using basic_context_type     = typename final_context_parent::basic_context_type;
        using etraits                = typename final_context_parent::etraits;

        static_assert(Context<final_context_parent>,
                      "The specified extension list type doesn't include basic_context; "
                      "did you forget to inherit from the "
                      "passed basic context type in your extension?");

        template <typename Ext>
        [[nodiscard]] static constexpr bool has_extension() noexcept {
            return istl::contains_parameter<root_extensions, Ext>;
        }

        /**
         * Append some extensions to this context type and get the type back
         */
        template <typename... E>
        using context_type_with_appended_extensions =
          typename istl::unique_parameters<typename root_extensions::template appended<E...>>::
            template extensie_type<traits_type, context_descriptor<root_extensions>, request_type>;



        constexpr static_context_view(request_type& inp_req) : final_context_parent{inp_req} {}


        template <Context CtxT>
        constexpr static_context_view(CtxT const& ctx) noexcept : EList{ctx} {}

        constexpr static_context_view(static_context_view const&) noexcept       = default;
        constexpr static_context_view(static_context_view&&) noexcept            = default;
        constexpr static_context_view& operator=(static_context_view const&)     = default;
        constexpr static_context_view& operator=(static_context_view&&) noexcept = default;
        constexpr ~static_context_view()                                         = default;

        /**
         * Clone this context and append the new extensions along the way.
         */
        template <Extension... E>
        [[nodiscard]] constexpr auto clone() const noexcept {
            using local_context_type = context_type_with_appended_extensions<E...>;
            static_assert(Context<local_context_type>,
                          "Web++ Internal Bug: the context_type is not valid for some reason!");
            return local_context_type{*this};
        }
    };



    /**
     * This struct helps the extension pack to find the correct type of the
     * extensions from the unified extension pack for context
     * Used by routers, to be passed to the extension_pack.
     */
    template <typename MergedRootExtensions>
    struct context_descriptor {

        template <Extension ExtensionType>
        using extractor_type = typename ExtensionType::context_extensions;


        template <ExtensionList RootExtensions,
                  typename TraitsType,
                  typename EList, // extension_pack
                  typename ReqType>
        using mid_level_extensie_type = common_context_view<ReqType, EList, MergedRootExtensions>;


        template <ExtensionList RootExtensions, typename TraitsType, typename EList, typename ReqType>
        using final_extensie_type = static_context_view<EList>;
    };


    // Don't use ReqType::root_extensions directly, we merge its extensions with the router's extensions
    template <HTTPRequest ReqType, typename MergedRootExtensions = typename ReqType::root_extensions>
    using simple_context =
      typename MergedRootExtensions::template extensie_type<typename ReqType::traits_type,
                                                            context_descriptor<MergedRootExtensions>,
                                                            ReqType>;


    /**
     * The standard and dynamic context which will own its data
     */
    template <Traits TraitsType = default_dynamic_traits>
    struct basic_context : details::common_context_methods<basic_request<TraitsType>, empty_extension_pack> {
        using traits_type         = TraitsType;
        using request_type        = basic_request<traits_type>;
        using static_context_type = simple_context<request_type>;
        using response_type       = basic_response<traits_type>;
        using string_type         = traits::general_string<traits_type>;
        using slug_type           = string_type;
        using path_traverser_type = uri::path_traverser<slug_type>;

        // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        request_type  request;
        response_type response;
        // NOLINTEND(misc-non-private-member-variables-in-classes)
        // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)


      private:
        using context_methods =
          details::common_context_methods<basic_request<TraitsType>, empty_extension_pack>;

        path_traverser_type traverser;

      public:
        constexpr basic_context(request_type& req) : context_methods{req} {}

        template <Context CtxT>
        constexpr basic_context(CtxT const& ctx) noexcept : basic_context{ctx.request} {}

        constexpr basic_context(basic_context&& ctx) noexcept        = default;
        constexpr basic_context(basic_context const& ctx) noexcept   = default;
        constexpr basic_context& operator=(basic_context const&)     = default;
        constexpr basic_context& operator=(basic_context&&) noexcept = default;
        constexpr ~basic_context()                                   = default;

        /**
         * Clone this context and append the new extensions along the way.
         */
        [[nodiscard]] constexpr auto clone() const noexcept {
            return basic_context{*this};
        }

        template <typename T>
        [[nodiscard]] constexpr bool check_segment(T&& slug) noexcept {
            return traverser.check_segment(stl::forward<T>(slug));
        }
    };

    using context = basic_context<>;

} // namespace webpp::http

#endif // WEBPP_ROUTES_CONTEXT_H
