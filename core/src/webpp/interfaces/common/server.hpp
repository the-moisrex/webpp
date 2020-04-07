#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "../../../../include/webpp/std/buffer.h"
#include "../../../../include/webpp/std/internet.h"
#include "../../../../include/webpp/std/io_context.h"
#include "connection.hpp"
#include "constants.hpp"
#include <boost/asio/thread_pool.hpp>
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
        using io_context_t = std::net::io_context;

        // I share this publicly because I know this file will not be used in a
        // header file so the final user doesn't have access to this class.
        io_context_t io;

      private:
        std::vector<connection> connections;
        std::net::ip::tcp::acceptor acceptor;
        boost::asio::thread_pool pool;

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

                    if (!io.stopped())
                        accept();
                });
        }

      public:
        server(endpoint_t endpoints) noexcept : acceptor(io, endpoints){};

        void run() noexcept {
            // Run until the tasks finishes normally.
            // Don't worry, we'll accept another connection when we finish one
            // of them
            for (;;) {
                try {
                    io.run();
                    break;
                } catch (std::exception const& err) {
                    // TODO: what should I do here?
                }
            }
        }

        void stop() noexcept { io.stop(); }
    };

} // namespace webpp::common

#endif // WEBPP_INTERFACES_COMMON_SERVER_H