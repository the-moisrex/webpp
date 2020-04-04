#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "../../../../include/webpp/std/buffer.h"
#include "../../../../include/webpp/std/io_context.h"
#include "connection.hpp"
#include "constants.hpp"
#include <memory>
#include <set>

namespace webpp::common {

    /**
     * This class is the server and the connection manager.
     */
    class server {
      public:
        using socket_t = std::net::ip::tcp::socket;
        using socket_t = std::net::ip::tcp::endpoint;

      private:
        std::set<connection> connections;
        std::net::io_context io;
        std::net::ip::tcp::acceptor acceptor;
        std::error_code ec;

        void accept() noexcept {
            acceptor.async_accept(
                [this](std::error_code const& ec, socket_t socket) {
                    // Check whether the server was stopped by a signal before
                    // this completion handler had a chance to run
                    if (!acceptor.is_open()) {
                        return;
                    }

                    if (!ec) {
                        connections.emplace(std::move(socket));
                    } else {
                        // TODO: log
                    }

                    accept();
                });
        }

      public:
        server(endpoint_t endpoints) noexcept : acceptor(io, endpoints){};

        void run() noexcept { io.run(ec); }
    };

} // namespace webpp::common

#endif // WEBPP_INTERFACES_COMMON_SERVER_H