// Created by moisrex on 5/1/20.

#ifndef WEBPP_CONTEXT_H
#define WEBPP_CONTEXT_H

#include "extensions/map.h"
#include "priority.h"

namespace webpp::routes {


    enum class extension_method : uint_fast8_t {
        pre_subroute,
        post_subroute,
        pre_entryroute,
        post_entryroute,
        pre_globalroute,
        post_globalroute,
        post_thisroute,
        pre_termination
    };

    template <typename A, extension_method em, typename ContextArgType,
              typename = void>
    struct has_context_extension_method : std::false_type {};

    /**
     * pre_subroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_subroute, ContextArgType,
      std::void_t<decltype(
        std::declval<A>().pre_subroute(std::declval<ContextArgType&>()),
        (void)0)>> : std::true_type {};

    /**
     * post_subroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_subroute, ContextArgType,
      std::void_t<decltype(
        std::declval<A>().post_subroute(std::declval<ContextArgType&>()),
        (void)0)>> : std::true_type {};


    /**
     * pre_entryroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_entryroute, ContextArgType,
      std::void_t<decltype(
        std::declval<A>().pre_entryroute(std::declval<ContextArgType&>()),
        (void)0)>> : std::true_type {};

    /**
     * post_entryroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_entryroute, ContextArgType,
      std::void_t<decltype(
        std::declval<A>().post_entryroute(std::declval<ContextArgType&>()),
        (void)0)>> : std::true_type {};

    /**
     * pre_globalroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_globalroute, ContextArgType,
      std::void_t<decltype(
        std::declval<A>().pre_globalroute(std::declval<ContextArgType&>()),
        (void)0)>> : std::true_type {};

    /**
     * post_globalroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_globalroute, ContextArgType,
      std::void_t<decltype(
        std::declval<A>().post_globalroute(std::declval<ContextArgType&>()),
        (void)0)>> : std::true_type {};

    /**
     * post_thisroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_thisroute, ContextArgType,
      std::void_t<decltype(
        std::declval<A>().post_thisroute(std::declval<ContextArgType&>()),
        (void)0)>> : std::true_type {};

    /**
     * pre_termination
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_termination, ContextArgType,
      std::void_t<decltype(
        std::declval<A>().pre_termination(std::declval<ContextArgType&>()),
        (void)0)>> : std::true_type {};


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
     *   todo: add "is_context_extension"
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
     */
    template <typename Traits, typename Interface, typename... ExtensionTypes>
    struct basic_context : public std::decay_t<ExtensionTypes>... {

        static_assert(is_traits_v<Traits>,
                      "The specified template parameter is not a valid traits");

      public:
        using traits          = Traits;
        using interface       = Interface;
        using request_type    = request_t<Traits, Interface>;
        using response_type   = response<Traits>;
        using extension_types = std::tuple<std::decay_t<ExtensionTypes>...>;


        // public fields:

        priority&           priority;
        request_type const& request;
        response_type&      response;



        /**
         * This method will generate a new context based on this context and
         * adds the context extensions to the context as well.
         * @tparam ExtensionType
         * @return
         */
        template <typename... ExtensionType>
        constexpr auto clone() const noexcept {
            using new_context =
              basic_context<traits, interface, ExtensionType...>;
            return new_context{.priority = this->priority,
                               .request  = this->request,
                               .response = this->response};
        }
    };



    /**
     * This is the "Extension Aware Context"
     * Extension aware context is a context that simplifies creation of the
     * context extensions.
     *
     * Extension aware context uses "Extension As Field" features.
     *
     * @tparam Traits
     * @tparam Interface
     * @tparam ExtensionTypes
     */
    template <typename Traits, typename Interface, typename... ExtensionTypes>
    struct context
      : public basic_context<Traits, Interface, ExtensionTypes...> {

        using string_type      = typename Traits::string_type;
        using string_view_type = typename Traits::string_view_type;

        template <typename KeyT = string_type, typename ValueT = string_type>
        auto map() const noexcept {
            using new_extension =
              extensions::as_data<extensions::map<Traits, KeyT, ValueT>>;
            using new_context_t =
              context<Traits, Interface, ExtensionTypes..., new_extension>;
            return new_context_t{*this};
        }
    };


} // namespace webpp::routes

#endif // WEBPP_CONTEXT_H
