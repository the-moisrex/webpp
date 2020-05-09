// Created by moisrex on 10/30/19.

#ifndef WEBPP_BASIC_INTERFACE_H
#define WEBPP_BASIC_INTERFACE_H

#include "../traits/traits_concepts.h"
#include "./basic_interface_concepts.h"

namespace webpp {

    /**
     * This class will do what all interfaces have in common.
     * All interfaces should extend from this base class.
     *
     * Responsibilities of this class:
     *   - Creating request
     *   - Routing
     */
    template <Traits TraitsT, Interface InterfaceT>
    class basic_interface {
      private:
        bool _hijack_stdio = false;

      public:
        using traits    = TraitsT;
        using interface = InterfaceT;

        //        webpp::router_t<traits, interface> router;

        auto hijack_stio() const noexcept {
            return _hijack_stdio;
        }

        /**
         * This will hijack stdio (cout, cin, cerr) while processing the
         * requests. This will only affect the next requests and not the ones
         * that are currently being processed.
         * TODO: enable hijacking right away instead of constantly checking for it later (if possible)
         */
        void hijack_stio(bool _hijack_stdio) noexcept {
            this->_hijack_stdio = _hijack_stdio;
        }
    };
} // namespace webpp

#endif // WEBPP_BASIC_INTERFACE_H
