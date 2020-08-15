#ifndef WEBPP_INTERFACE_COMMON_CONNECTION_H
#define WEBPP_INTERFACE_COMMON_CONNECTION_H

#include "../../../std/buffer.hpp"
#include "../../../std/internet.hpp"
#include "../../../std/socket.hpp"
#include "constants.hpp"

#include <memory>
#include <system_error>

/**
 * The reason that this file is here and not in the include directory is because
 * we want to hide every boost related library from the final users of this
 * framework. Hiding implementations are good ways to do this.
 */

namespace webpp::common {

    class connection {
      public:
        using socket_t = stl::net::ip::tcp::socket;

      private:
        socket_t                      socket;
        stl::array<char, buffer_size> buffer;

        void read() noexcept {
            // we share ourselves, so the connection keeps itself alive.
            socket.async_read_some(stl::net::buffer(buffer), [this](stl::error_code const& err,
                                                                    stl::size_t bytes_transferred) noexcept {
                if (!err) {
                    // we need to parse, store, read more, or
                    // write something
                }
            });
        }

        void write() noexcept {}

      public:
        connection(socket_t socket) noexcept : socket(stl::move(socket)) {}
        connection(connection const&) = delete;
        connection(connection&&)      = default;
        connection& operator=(connection const&) = delete;
        connection& operator=(connection&&) = default;

        /**
         * Start async operations
         */
        void start() noexcept;

        /**
         * We're shutting down everything, keep up!
         */
        void stop() noexcept;
    };

} // namespace webpp::common

#endif // WEBPP_INTERFACE_COMMON_CONNECTION_H
