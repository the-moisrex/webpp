// Created by moisrex on 10/30/19.

#ifndef WEBPP_BASIC_INTERFACE_H
#define WEBPP_BASIC_INTERFACE_H

namespace webpp {

    /**
     * This class will do what all interfaces have in common.
     * All interfaces should extend from this base class.
     *
     * Responsibilities of this class:
     *   - Creating request
     *   - Routing
     */
    class basic_interface {
      protected:
        /**
         * There will be no more requests. Finish the program
         */
        void done() noexcept { exit(EXIT_SUCCESS); }

      public:
    };
} // namespace webpp

#endif // WEBPP_BASIC_INTERFACE_H
