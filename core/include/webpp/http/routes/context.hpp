// Created by moisrex on 5/1/20.

#ifndef WEBPP_ROUTES_CONTEXT_H
#define WEBPP_ROUTES_CONTEXT_H

#include "../../extensions/extension.hpp"
#include "../../traits/enable_traits.hpp"
#include "../bodies/string.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "./context_concepts.hpp"
#include "./extensions/map.hpp"

namespace webpp {


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
    template <Traits TraitsType, typename EList, Request RequestType, Response ResponseType>
    struct basic_context : public EList, public enable_traits<TraitsType> {
        using traits_type        = TraitsType;
        using allocator_type     = typename traits_type::template allocator<typename traits_type::char_type>;
        using elist_type         = EList;
        using request_type       = RequestType;
        using response_type      = ResponseType;
        using basic_context_type = basic_context<TraitsType, EList, RequestType, ResponseType>;
        using logger_type        = typename traits_type::logger_type;
        using logger_ref         = typename logger_type::logger_ref;
        using etraits            = enable_traits<TraitsType>;

      public:
        constexpr basic_context(logger_ref            logger = logger_type{},
                                allocator_type const& _alloc = allocator_type{}) noexcept
          : elist_type{},
            enable_traits<TraitsType>(logger, _alloc) {}


        template <typename... Args>
        constexpr basic_context(logger_ref            logger = logger_type{},
                                allocator_type const& _alloc = allocator_type{}, Args&&... args) noexcept
          : enable_traits<TraitsType>(logger, _alloc),
            elist_type{stl::forward<Args>(args)...} {}

        template <typename... Args>
        constexpr basic_context(Args&&... args) noexcept
          : elist_type{stl::forward<Args>(args)...},
            etraits{} {}

        basic_context(basic_context&& ctx) noexcept      = default;
        basic_context(basic_context const& ctx) noexcept = default;

        /**
         * Generate a response
         */
        template <typename... NewExtensions, typename... Args>
        [[nodiscard]] Response auto response(Args&&... args) const noexcept {
            using new_response_type =
              typename response_type::template apply_extensions_type<NewExtensions...>;
            // todo: write an auto extension finder based on the Args that get passed
            return new_response_type{stl::forward<Args>(args)...};
        }

        [[nodiscard]] Response auto error(status_code_type error_code) const noexcept {
            return error(error_code,
                         stl::format("Error {}: {}", error_code, status_reason_phrase(error_code)));
        }

        [[nodiscard]] Response auto error(status_code_type error_code, auto&& data) const noexcept {
            using data_type = stl::remove_cvref_t<decltype(data)>;
            if constexpr (istl::ConvertibleToStringView<data_type>) {
                auto res                = response<string_response>(istl::to_string_view(data));
                res.headers.status_code = error_code;
                return res;
            } else if constexpr (requires {
                                     { data.what() }
                                     ->istl::StringViewfiable;
                                 }) {
                auto res                = response<string_response>(data.what());
                res.headers.status_code = error_code;
                return res;
            } else {
                auto res                = response<>();
                res.headers.status_code = error_code;
                return res;
            }
        }

        // todo: add all the features of returning a response each body type should have at least one method here
    };

    template <Traits TraitsType, typename ContextDescriptorType, typename OriginalExtensionList,
              typename EList, typename ReqType>
    struct final_context final : public EList {
        using elist_type                   = EList;
        using traits_type                  = TraitsType;
        using context_descriptor_type      = ContextDescriptorType;
        using original_extension_pack_type = OriginalExtensionList;
        using request_type                 = stl::remove_cvref_t<ReqType>;
        using basic_context_type           = typename EList::basic_context_type;
        using logger_type                  = typename traits_type::logger_type;
        using logger_ref                   = typename logger_type::logger_ref;

        // should we use a char_type as allocator? change this if you think it's not the correct type
        using allocator_type = typename traits_type::template allocator<typename traits_type::char_type>;


        /**
         * Append some extensions to this context type and get the type back
         */
        template <typename... E>
        using context_type_with_appended_extensions =
          typename original_extension_pack_type::template appended<E...>::unique::template extensie_type<
            traits_type, context_descriptor_type, request_type>;

        constexpr final_context(logger_ref            logger = logger_type{},
                                allocator_type const& alloc  = allocator_type{}) noexcept
          : /*basic_context_type{alloc},*/
            EList(logger, alloc) {}

        //        template <typename... Args>
        //        constexpr final_context(Args&&... args) noexcept : EList{stl::forward<Args>(args)...} {
        //        }
        //
        //        template <typename... Args>
        //        constexpr final_context(request_type* req, Args&&... args) noexcept
        //          : basic_context_type{req},
        //            EList{stl::forward<Args>(args)...} {
        //        }

        //        template <typename... Args>
        //        constexpr final_context(request_type& req, Args&&... args) noexcept
        //          : basic_context_type{req},
        //            EList{stl::forward<Args>(args)...} {
        //        }


        //        constexpr final_context(request_type* req) noexcept : basic_context_type{req} {}
        //        constexpr final_context(request_type& req) noexcept : basic_context_type{req} {}

        constexpr final_context(final_context const&) noexcept = default;
        constexpr final_context(final_context&&) noexcept      = default;

        template <Traits NTraitsType, typename NContextDescriptorType, typename NOriginalExtensionList,
                  typename NEList, typename NReqType>
        constexpr final_context(final_context<NTraitsType, NContextDescriptorType, NOriginalExtensionList,
                                              NEList, NReqType> const& ctx) noexcept
          : EList{ctx.get_allocator()} {}

        /**
         * Clone this context and append the new extensions along the way.
         */
        template <Extension... E>
        [[nodiscard]] constexpr auto clone() const noexcept {
            using context_type = context_type_with_appended_extensions<E...>;
            return context_type{*this};
        }

        // todo: these methods need to be noexcept. They call unknown stuff.

        //        void call_pre_subroute_methods() noexcept {
        //            ((has_context_extension_method<ExtensionTypes,
        //                                           extension_method::pre_subroute,
        //                                           basic_context>::value
        //                ? (ExtensionTypes::template
        //                pre_subroute<basic_context_type>(
        //                     *this),
        //                   (void)0)
        //                : (void)0),
        //             ...);
        //        }
        //
        //
        //        void call_pre_entryroute_methods() noexcept {
        //            ((has_context_extension_method<ExtensionTypes,
        //                                           extension_method::pre_entryroute,
        //                                           basic_context>::value
        //                ? (ExtensionTypes::template
        //                pre_entryroute<basic_context_type>(
        //                     *this),
        //                   (void)0)
        //                : (void)0),
        //             ...);
        //        }
        //
        //
        //        void call_pre_firstroute_methods() noexcept {
        //            ((has_context_extension_method<ExtensionTypes,
        //                                           extension_method::pre_firstroute,
        //                                           basic_context>::value
        //                ? (ExtensionTypes::template
        //                pre_firstroute<basic_context_type>(
        //                     *this),
        //                   (void)0)
        //                : (void)0),
        //             ...);
        //        }
        //
        //
        //        void call_post_subroute_methods() noexcept {
        //            ((has_context_extension_method<ExtensionTypes,
        //                                           extension_method::post_subroute,
        //                                           basic_context>::value
        //                ? (ExtensionTypes::template
        //                post_subroute<basic_context_type>(
        //                     *this),
        //                   (void)0)
        //                : (void)0),
        //             ...);
        //        }
        //
        //        void call_post_entryroute_methods() noexcept {
        //            ((has_context_extension_method<ExtensionTypes,
        //                                           extension_method::post_entryroute,
        //                                           basic_context>::value
        //                ? (ExtensionTypes::template
        //                post_entryroute<basic_context_type>(
        //                     *this),
        //                   (void)0)
        //                : (void)0),
        //             ...);
        //        }
        //
        //
        //        void call_post_lastroute_methods() noexcept {
        //            ((has_context_extension_method<ExtensionTypes,
        //                                           extension_method::post_lastroute,
        //                                           basic_context>::value
        //                ? (ExtensionTypes::template
        //                post_lastroute<basic_context_type>(
        //                     *this),
        //                   (void)0)
        //                : (void)0),
        //             ...);
        //        }
    };


    /**
     * Dynamic context
     * todo
     */
    struct context {};


    /**
     * This struct helps the extension pack to find the correct type of the
     * extensions from the unified extension pack for context
     * Used by routers, to be passed to the extension_pack.
     */
    struct context_descriptor {

        template <Extension ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::context_extensions;
            };
        };


        template <Extension ExtensionType>
        using related_extension_pack_type = typename ExtensionType::context_extensions;


        template <ExtensionList ExtensionListType, Traits TraitsType,
                  typename EList, // extension_pack
                  typename ReqType>
        using mid_level_extensie_type = basic_context<
          TraitsType, EList, ReqType,
          // getting the extensie_type of the basic_response
          typename ExtensionListType::template extensie_type<TraitsType, basic_response_descriptor>>;


        template <ExtensionList OriginalExtensionListType, Traits TraitsType, typename EList, Request ReqType>
        using final_extensie_type =
          final_context<TraitsType, context_descriptor, OriginalExtensionListType, EList, ReqType>;
    };



    template <Request ReqType, /* fixme: ExtensionList */ typename ExtensionListType = empty_extension_pack>
    using simple_context = typename ExtensionListType::template extensie_type<typename ReqType::traits_type,
                                                                              context_descriptor, ReqType>;

    // todo: move this into "context_concepts.h" file
    //        using std_context_type = typename context_descriptor::template final_extensie_type<
    //          empty_extension_pack, fake_traits_type,
    //          basic_context<empty_extension_pack, fake_request_type, fake_response_type>,
    //          fake_request_type>;

    using fake_context_type = simple_context<fake_request_type>;
} // namespace webpp

#endif // WEBPP_ROUTES_CONTEXT_H
