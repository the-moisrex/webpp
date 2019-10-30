#ifndef WEBPP_SERVER_H
#define WEBPP_SERVER_H

#include "router.h"

namespace webpp {

    template <typename Interface>
    class server : Interface {

        // TODO: write some template meta codes here to verify the interface

      public:
        using Interface::Interface;

      private:
        /**
         * This method will generate the request
         */
        request create_request() noexcept {
            request req;

            return req;
        }
    };

}; // namespace webpp

#endif // WEBPP_SERVER_H
