#ifndef WEBPP_REQUEST_H
#define WEBPP_REQUEST_H

#include "headers.hpp"
#include "body.h"

/**
 *
 * What request class needs:
 *
 * - [ ] The INTERFACE:
 *       Because otherwise, the interface class itself has to instantiate this
 *       class and copy/move all the data over to here. So in that situation
 *       the interface has to process all the funcionalities that this class
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
 * relative calsses that finally end up in that class. This class is only for
 * giving informations that the user or other modules need.
 *
 */

namespace webpp {

    template <class Interface>
    class request {
      private:
        Interface* _interface = nullptr; // the interface lives longer than this class

      public:
        request(Interface* interface) noexcept;

        char const* method() const noexcept {
          return _interface->method();
        }

        /**
         * @brief returns the request scheme (http/https/...)
         */
        char const* scheme() const noexcept {
          return _interface->scheme();
        }

        /**
         * @brief returns something like "HTTP/1.1" or ...
         */
        char const* server_protocol() const noexcept {
          return _interface->server_protocol();
        }

        char const* query() const noexcept {
          return _interface->query();
        }
        
        int remote_port() const noexcept {
          return _interface->remote_port();
        }

        int server_port() const noexcept {
          return _interface->server_port();
        }

        char const* server_addr() const noexcept {
          return _interface->server_addr();
        }

        char const* remote_addr() const noexcept {
          return _interface->remote_addr();
        }

        char const* server_name() const noexcept {
          return _interface->server_name();
        }

        char const* request_uri() const noexcept {
          return _interface->request_uri();
        }

        char const* header(char const* const name) const noexcept {
          return _interface->header(name);
        }

        webpp::headers headers() const noexcept {
          return _interface->headers();
        }

        webpp::body body() const noexcept {
          return _interface->body();
        }

    };
} // namespace webpp

#endif // WEBPP_REQUEST_H
