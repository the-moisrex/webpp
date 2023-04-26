// Created by moisrex on 5/1/20.

#ifndef WEBPP_ROUTES_CONTEXT_H
#define WEBPP_ROUTES_CONTEXT_H

#include "../../extensions/extension.hpp"
#include "../../extensions/extension_wrapper.hpp"
#include "../../memory/object.hpp"
#include "../../traits/enable_traits.hpp"
#include "../bodies/string.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "router_concepts.hpp"

namespace webpp::http {

    template <typename MergedRootExtensions>
    struct context_descriptor;


    /**
     *
     * Definitions:
     *   - Context:     An object of arbitrary type that will contain everything
     *                  that routes will need including:
     *                    - some types:
     *                      - Traits
     *                      - Interface
     *                      - Next sub route
     *                    - references to:
     *                      - Request
     *                      - Response
     *                      - Response Route in this route chain
     *                    - Previous entry routes context changes
     *                    - Previous sub routes context changes
     *                    - Original entry routes level context
     *                    - Original sub routes context changes
     *   - Context Passing Pattern:
     *                   A pattern designed to share arbitrary data down the
     *                   routing chain.
     *   - Context extensions:
     *                   A class that extends the original context and will be
     *                   used in context-switching process to add more
     *                   features to the context so it can be used in the
     *                   sub routes down the routing chain.
     *
     *
     * Features we need:
     *   - [X] Having access to the request and the response
     *   - [ ] Termination of continuation of checking the sub-routes by parents
     *   - [ ] Termination of continuation of checking the entry-routes by any
     *         previous routes, or sub-routes.
     *   - [ ] Context modification
     *     - [X] Sub-Route local context modification by any previous sub-routes
     *     - [ ] Inter-Entry-Route context modification by any previous
     *           (sub/entry) routes
     *   - [ ] Entry-Route prioritization
     *     - [ ] Auto prioritization
     *     - [ ] Manual prioritization
     *     - [ ] Hinted prioritization
     *     - [ ] On-The-Fly Re-Prioritization
     *   - [ ] Dynamic route generation / Dynamic route switching
     *   - [X] Context Passing pattern
     *   - [X] Context extensions
     *     - [ ] Deactivate route extension
     *     - [X] Map extension
     *
     *
     * Public fields:
     *   - [X] priority   : to check/change this route chain
     *   - [X] request    : a const reference to the the request object
     *   - [X] response   : a non-const reference to the response object
     *
     * public methods:
     *   - auto clone<extensions...>()
     *       get a clone of itself with different type
     *       designed to add extensions
     *
     *
     * Extension requirements:
     *   - [ ] Having a default constructor
     *
     * Extension collision:
     *   It is possible to try to add an extension to the context and get
     *   compile time errors saying that it's a collision or an ambiguous call
     *   to some method, to solve this, you can use "Extension As Field"
     *   features, which means you can clone a context like this:
     *     return context.clone<as_field<map<traits, string, string>>>();
     *   It's also possible to simplify this line of code with
     *   "Extension aware context" struct.
     *
     *
     * Internal Extension Handling:
     *   We can customize every single route to check if the extension is
     *   present in the returned context and then act accordingly, but that's
     *   not scalable; so in order to do this, we're gonna call "something"
     *   on each extension in these times (if the extension has its method):
     *     - pre_subroute:      Before every call to a sub-route
     *     - post_subroute:     After every call to a sub-route
     *     - pre_entryroute:    Before every call to an entry route
     *     - post_entryroute:   After every call to an entry route
     *     - pre_globalroute:   Before calling the global route
     *     - post_globalroute:  After calling the global route
     *     - post_thisroute:    Call it once; right after this route
     *     - pre_termination:   When we get the final result and we're about
     *                            to send it to the user.
     *
     *
     * todo: Extension dependency:
     *   We need a way of saying that an extension needs another extension to
     *   work.
     *
     *
     * todo: Runtime modification of Initial context type:
     *   We need a way to achieve this; we need a way to specify the initial
     *   context type that will be used for every single time.
     *   Or we need this:
     *     A way to preserve data from request to request. Meaning:
     *     Router extensions.
     *
     *
     */
    template <HTTPRequest RequestType, typename EList, typename RootExtensions>
    struct basic_context
      : public enable_traits_with<typename RequestType::traits_type, extension_wrapper<EList>> {
        using request_type           = RequestType;
        using traits_type            = typename request_type::traits_type;
        using mother_extensions_type = EList;
        using extension_wrapper_type = extension_wrapper<EList>;
        using etraits                = enable_traits_with<traits_type, extension_wrapper_type>;
        using root_extensions        = RootExtensions;
        using response_type          = simple_response<traits_type, root_extensions>;
        using basic_context_type     = basic_context;
        using request_ref            = request_type&;

      public:
        request_ref request;

        constexpr basic_context(request_ref inp_req) noexcept : etraits{inp_req}, request{inp_req} {}

        template <Context CtxT>
        requires(stl::same_as<typename stl::remove_cvref_t<CtxT>::request_type,
                              request_type>) constexpr basic_context(CtxT const& ctx) noexcept
          : basic_context{ctx.request} {}

        constexpr basic_context(basic_context&& ctx) noexcept        = default;
        constexpr basic_context(basic_context const& ctx) noexcept   = default;
        constexpr basic_context& operator=(basic_context const&)     = default;
        constexpr basic_context& operator=(basic_context&&) noexcept = default;


        /**
         * Generate a response
         */
        template <Extension... NewExtensions, typename... Args>
        [[nodiscard]] constexpr HTTPResponse auto response(Args&&... args) const noexcept {
            return response_type::template create<NewExtensions...>(*this, stl::forward<Args>(args)...);
        }



        /**
         * Generate a response while passing the specified arguments as the body of that response
         */
        template <Extension... NewExtensions, typename... Args>
        [[nodiscard]] constexpr HTTPResponse auto response_body(Args&&... args) const noexcept {
            using new_response_type =
              typename response_type::template apply_extensions_type<NewExtensions...>;
            return new_response_type::with_body(stl::forward<Args>(args)...);
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

        [[nodiscard]] constexpr HTTPResponse auto error(http::status_code_type error_code) const noexcept {
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
                auto res                = response<string_body>(istl::string_viewify(data));
                res.headers.status_code = error_code;
                return res;
            } else if constexpr (requires {
                                     { data.what() } -> istl::StringViewifiable;
                                 }) {
                auto res                = response<string_body>(data.what());
                res.headers.status_code = error_code;
                return res;
            } else {
                auto res                = response<>();
                res.headers.status_code = error_code;
                return res;
            }
        }

        // todo: add more error handling templates here.
        // todo: let the user customize error templates with extensions
        // todo: add all the features of returning a response each body type should have at least one method here
    };

    template <typename EList>
    struct final_context final : public EList {
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



        constexpr final_context(request_type& inp_req) : final_context_parent{inp_req} {}


        template <Context CtxT>
        constexpr final_context(CtxT const& ctx) noexcept : EList{ctx} {}

        constexpr final_context(final_context const&) noexcept       = default;
        constexpr final_context(final_context&&) noexcept            = default;
        constexpr final_context& operator=(final_context const&)     = default;
        constexpr final_context& operator=(final_context&&) noexcept = default;

        // final_context() = delete;
        //
        // template <EnabledTraits ET>
        // requires(
        //   !stl::same_as<ET, final_context> // forward-referencing first arg will confuse move and copy
        // )
        // constexpr final_context(ET&& et_obj) noexcept : final_context_parent(et_obj) {}

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
        using mid_level_extensie_type = basic_context<ReqType, EList, MergedRootExtensions>;


        template <ExtensionList RootExtensions, typename TraitsType, typename EList, typename ReqType>
        using final_extensie_type = final_context<EList>;
    };


    // Don't use ReqType::root_extensions directly, we merge its extensions with the router's extensions
    template <HTTPRequest ReqType, typename MergedRootExtensions = typename ReqType::root_extensions>
    using simple_context =
      typename MergedRootExtensions::template extensie_type<typename ReqType::traits_type,
                                                            context_descriptor<MergedRootExtensions>,
                                                            ReqType>;

} // namespace webpp::http

#endif // WEBPP_ROUTES_CONTEXT_H
