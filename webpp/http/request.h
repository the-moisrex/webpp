#ifndef WEBPP_REQUEST_H
#define WEBPP_REQUEST_H

#include "body.h"
#include "header.h"

/**
 *
 * What request class needs:
 *
 * - [ ] The INTERFACE:
 *       Because otherwise, the interface class itself has to instantiate this
 *       class and copy/move all the data over to here. So in that situation
 *       the interface has to process all the functionalities that this class
 *       can provide. But that's not what we want because there will be a lot of
 *       things that the user may not need in order to create and pass a
 *       response.
 * - [ ] The headers
 * - [ ] The body
 *
 *
 * Only the interface should be instantiating this class. There should be no
 * need for anybody else to do it.
 *
 * The user has access to this class. This class should be accessed as a const
 * variable since the user should not be able to make any changes here. Every
 * action that the user has to make should be in the "response" class or other
 * relative classes that finally end up in that class. This class is only for
 * giving informations that the user or other modules need.
 *
 */

namespace webpp {

    class request {
      private:

      public:

        auto method() const noexcept { return interface->method(); }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        auto scheme() const noexcept { return interface->scheme(); }

        /**
         * @brief returns something like "HTTP/1.1" or ...
         */
        auto server_protocol() const noexcept {
            return interface->server_protocol();
        }

        auto query() const noexcept { return interface->query(); }

        auto remote_port() const noexcept { return interface->remote_port(); }

        auto server_port() const noexcept { return interface->server_port(); }

        auto server_addr() const noexcept { return interface->server_addr(); }

        auto remote_addr() const noexcept { return interface->remote_addr(); }

        auto server_name() const noexcept { return interface->server_name(); }

        auto request_uri() const noexcept { return interface->request_uri(); }

        auto header(std::string_view const& name) const noexcept {
            return interface->header(name);
        }

        auto headers() const noexcept { return interface->headers(); }
        auto body() const noexcept { return interface->body(); }
    };
} // namespace webpp

#endif // WEBPP_REQUEST_H
