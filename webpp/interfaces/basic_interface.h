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
        router _router;

        /**
         * There will be no more requests. Finish the program
         */
        void done() noexcept { exit(EXIT_SUCCESS); }

        /**
         * Run a request through chains of routes
         * @param req
         */
        void run_request(request& req) noexcept { _router.run(req); }

      public:
    };
} // namespace webpp

#endif // WEBPP_BASIC_INTERFACE_H
