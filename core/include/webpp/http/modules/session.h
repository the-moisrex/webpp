#ifndef SESSION_H
#define SESSION_H

/**
 *
 * TODO:
 *    - [ ] Flash data
 *    - [ ] Long live data
 *    - [ ] Timed data
 *    - [ ] Route specific sessions
 *
 *
 * Possible Client Adapters to implement:
 *    - [ ] In Cookies
 *    - [ ] In HTTP-Headers
 *    - [ ] In GET Method
 *    - [ ] In All Methods
 *    - [ ] Combination of all above ways
 *
 *
 * Possible Server Adapters to implement:
 *    - [ ] In a folder
 *    - [ ] In predefined file (your own thing)
 *    - [ ] In cutsom file (json, xml, Excel, ...)
 *    - [ ] In memory (for long lived versions of builds like FastCGI)
 *    - [ ] In cookies (encrypted or plain text)
 *    - [ ] In predefined database (sqlite)
 *    - [ ] In custom database (User configured database)
 */

#include "session/client-adapter/cookie_adapter.h"
#include "session/server-adapter/memory_adapter.h"

#include <memory>
#include <string>
#include <thread>
#include <type_traits>

namespace webpp {

    template <class ServerAdapter = webpp::memory_adapter,
              class ClientAdapter = webpp::cookie_adapter>
    class session {
      public:
        using key_t = std::string;

      protected:
        /**
         * @brief The cache for the ServerAdapter in sessions.
         */
        class cache_proxy_t {
          private:
            std::thread tr;
            bool        _cache_enabled =
              !std::is_same<ServerAdapter, memory_adapter>::value;

          public:
            cache_proxy_t(
              std::unique_ptr<ServerAdapter>&& server_adapter) noexcept
              : tr(server_adapter) {
            }

            ~cache_proxy_t() noexcept {
                if (tr.joinable())
                    tr.join();
            }
        } proxy;

      public:
        session(decltype(_server_adapter)&& server_adapter = nullptr,
                decltype(_client_adapter)&& client_adapter = nullptr) noexcept
          : _server_adapter{std::move(server_adapter)},
            _client_adapter{std::move(client_adapter)} {
            if (!_server_adapter)
                _server_adapter = std::make_shared<ServerAdapter>();

            if (!_client_adapter)
                _client_adapter = std::make_shared<ClientAdapter>();
        }

        template <typename T>
        T& get(session::key_t key) noexcept {
            return proxy.get(key);
        }
    };

} // namespace webpp

#endif // SESSION_H
