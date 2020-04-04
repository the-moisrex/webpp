#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "../../../../include/webpp/std/buffer.h"
#include "../../../../include/webpp/std/internet.h"
#include "../../../../include/webpp/std/io_context.h"
#include "connection.hpp"
#include "constants.hpp"
#include <memory>
#include <vector>

namespace webpp::common {

    /**
     * This class is the server and the connection manager.
     */
    class server {
      public:
        using socket_t = std::net::ip::tcp::socket;
        using endpoint_t = std::net::ip::tcp::endpoint;
#if STD_IO_CONTEXT == STLLIB_BOOST
        using error_code_t = boost::system::error_code;
#else
        using error_code_t = std::error_code;
#endif

      private:
        std::vector<connection> connections;
        std::net::io_context io;
        std::net::ip::tcp::acceptor acceptor;
        error_code_t ec;

        void accept() noexcept {
            acceptor.async_accept(
                [this](error_code_t const& ec, socket_t socket) {
                    // Check whether the server was stopped by a signal before
                    // this completion handler had a chance to run
                    if (!acceptor.is_open()) {
                        return;
                    }

                    if (!ec) {
                        connections.emplace_back(std::move(socket));
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