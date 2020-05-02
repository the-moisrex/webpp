// Created by moisrex on 5/1/20.

#ifndef WEBPP_CONTEXT_H
#define WEBPP_CONTEXT_H

#include "priority.h"

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
     *   - [ ] Having access to the context class in the callables
     *   - [ ] Having access to the request and the response
     *   - [ ] Termination of continuation of checking the sub-routes by parents
     *   - [ ] Termination of continuation of checking the entry-routes by any
     *         previous routes, or sub-routes.
     *   - [ ] Context modification
     *     - [ ] Sub-Route local context modification by any previous sub-routes
     *     - [ ] Inter-Entry-Route context modification by any previous
     *           (sub/entry) routes
     *   - [ ] Entry-Route prioritization
     *     - [ ] Auto prioritization
     *     - [ ] Manual prioritization
     *     - [ ] Hinted prioritization
     *     - [ ] On-The-Fly Re-Prioritization
     *   - [ ] Dynamic route generation / Dynamic route switching
     *   - [ ] Context Passing pattern
     *   - [ ] Context extensions
     *   - [ ] Deactivated routes
     *
     *
     * Public fields:
     *   - priority   : to check/change this route chain
     *   - request    : a const reference to the the request object
     *   - response   : a non-const reference to the response object
     *
     * public types:
     *   - context    : the type of itself
     *   - traits
     *   - interface
     *   - req_t
     *   - res_t
     *   - request_type
     *   - response_type
     *
     * public methods:
     *   - auto clone<extensions...>()
     *       get a clone of itself with different type
     *       designed to add extensions
     */
    template <typename Traits, typename Interface>
    struct context_base {

        static_assert(is_traits_v<Traits>,
                      "The specified template parameter is not a valid traits");

      public:
        using traits        = Traits;
        using interface     = Interface;
        using request_type  = request_t<Traits, Interface>;
        using response_type = response<Traits>;


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
        auto clone() const noexcept {
        }
    };
} // namespace webpp::routes

#endif // WEBPP_CONTEXT_H
