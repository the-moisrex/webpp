// Created by moisrex on 5/1/20.

#ifndef WEBPP_ROUTES_CONTEXT_H
#define WEBPP_ROUTES_CONTEXT_H

#include "../http/request.h"
#include "../http/response.h"
#include "./context_concepts.h"
#include "./extensions/map.h"

namespace webpp::routes {


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
    template <ContextExtension... ExtensionTypes>
    struct basic_context : public ::std::decay_t<ExtensionTypes>... {

      public:
        // todo: use extension pack instead of tuple:
        using extension_types = ::std::tuple<::std::decay_t<ExtensionTypes>...>;
        using basic_context_type = basic_context<ExtensionTypes...>;

        template <ContextExtension... NewExtensionTypes>
        using rebind = basic_context<ExtensionTypes..., NewExtensionTypes...>;


        /**
         * This method will generate a new context based on this context and
         * adds the context extensions to the context as well.
         * @tparam ExtensionType
         * @return
         */
        template <ContextExtension... ExtensionType>
        constexpr auto clone() const noexcept {
            using new_context =
              basic_context<traits_type, interface_type, ExtensionType...>;
            return new_context{.request  = this->request,
                               .response = this->response};
        }

        // todo: these methods need to be noexcept. They call unknown stuff.

        inline void call_pre_subroute_methods() noexcept {
            ((has_context_extension_method<ExtensionTypes,
                                           extension_method::pre_subroute,
                                           basic_context>::value
                ? (ExtensionTypes::template pre_subroute<basic_context_type>(
                     *this),
                   (void)0)
                : (void)0),
             ...);
        }


        inline void call_pre_entryroute_methods() noexcept {
            ((has_context_extension_method<ExtensionTypes,
                                           extension_method::pre_entryroute,
                                           basic_context>::value
                ? (ExtensionTypes::template pre_entryroute<basic_context_type>(
                     *this),
                   (void)0)
                : (void)0),
             ...);
        }


        inline void call_pre_firstroute_methods() noexcept {
            ((has_context_extension_method<ExtensionTypes,
                                           extension_method::pre_firstroute,
                                           basic_context>::value
                ? (ExtensionTypes::template pre_firstroute<basic_context_type>(
                     *this),
                   (void)0)
                : (void)0),
             ...);
        }


        inline void call_post_subroute_methods() noexcept {
            ((has_context_extension_method<ExtensionTypes,
                                           extension_method::post_subroute,
                                           basic_context>::value
                ? (ExtensionTypes::template post_subroute<basic_context_type>(
                     *this),
                   (void)0)
                : (void)0),
             ...);
        }

        inline void call_post_entryroute_methods() noexcept {
            ((has_context_extension_method<ExtensionTypes,
                                           extension_method::post_entryroute,
                                           basic_context>::value
                ? (ExtensionTypes::template post_entryroute<basic_context_type>(
                     *this),
                   (void)0)
                : (void)0),
             ...);
        }


        inline void call_post_lastroute_methods() noexcept {
            ((has_context_extension_method<ExtensionTypes,
                                           extension_method::post_lastroute,
                                           basic_context>::value
                ? (ExtensionTypes::template post_lastroute<basic_context_type>(
                     *this),
                   (void)0)
                : (void)0),
             ...);
        }
    };


    /**
     * This is the "Extension Aware Context"
     * Extension aware context is a context that simplifies creation of the
     * context extensions.
     *
     * Extension aware context uses "Extension As Field" features.
     *
     * @tparam TraitsType
     * @tparam InterfaceType
     * @tparam ExtensionTypes
     */
    template <Traits TraitsType, Interface InterfaceType,
              ContextExtension... ExtensionTypes>
    struct context
      : public basic_context<TraitsType, InterfaceType, ExtensionTypes...> {

        using string_type      = typename TraitsType::string_type;
        using string_view_type = typename TraitsType::string_view_type;

        template <typename KeyT = string_type, typename ValueT = string_type>
        auto map() const noexcept {
            using new_extension =
              extensions::as_data<extensions::map<TraitsType, KeyT, ValueT>>;
            using new_context_t = context<TraitsType, InterfaceType,
                                          ExtensionTypes..., new_extension>;
            return new_context_t{*this};
        }
    };


} // namespace webpp::routes

#endif // WEBPP_ROUTES_CONTEXT_H
