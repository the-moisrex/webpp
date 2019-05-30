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
#include <type_traits>

namespace webpp {

    template <class ServerAdapter = webpp::memory_adapter,
              class ClientAdapter = webpp::cookie_adapter,
              class CacheServerAdapter = webpp::memory_adapter>
    class session {
      protected:
        bool _cache_enabled =
            !std::is_same<ServerAdapter, CacheServerAdapter>::value;
        std::unique_ptr<ServerAdapter> _server_adapter;
        std::unique_ptr<ClientAdapter> _client_adapter;
        std::unique_ptr<CacheServerAdapter> _cache_adapter;

      public:
        session(decltype(_server_adapter)&& server_adapter = nullptr,
                decltype(_client_adapter)&& client_adapter = nullptr,
                decltype(_cache_adapter)&& cache_adapter = nullptr) noexcept
            : _server_adapter{std::move(server_adapter)},
              _client_adapter{std::move(client_adapter)},
              _cache_adapter{std::move(cache_adapter)},
              _cache_enabled{_cache_enabled && _cache_adapter != nullptr} {
            if (!_server_adapter)
                _server_adapter = std::make_shared<ServerAdapter>();

            if (!_client_adapter)
                _client_adapter = std::make_shared<ClientAdapter>();
        }
    };

} // namespace webpp

#endif // SESSION_H
