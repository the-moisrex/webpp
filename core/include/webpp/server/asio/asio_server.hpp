#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "../../namespace.hpp"
// clang-format off
#include asio(ip/tcp)
// clang-format on

#include "../../traits/traits_concepts.hpp"
#include "./asio_thread_pool.hpp"
#include "./asio_connection.hpp"
#include "./asio_constants.hpp"

#include <memory>

namespace webpp {

    /**
     * This class is the server and the connection manager.
     */
    template <Traits TraitsType>
    class server {
      public:
        using traits_type      = TraitsType;
        using socket_type      = asio::ip::tcp::socket;
        using endpoint_type    = asio::ip::tcp::endpoint;
        using acceptor_type    = asio::ip::tcp::acceptor;
        using io_context_type  = asio::io_context;
        using thread_pool_type = typename TraitsType::thread_pool_type;


        // I share this publicly because I know this file will not be used in a
        // header file so the final user doesn't have access to this class.
        io_context_type io;

      private:
        istl::vector<traits_type, connection>    connections;
        istl::vector<traits_type, acceptor_type> acceptors;
        thread_pool_type                         pool;

        void accept() noexcept {
            //            for (auto& acceptor : acceptors)
            //                acceptor.async_accept(
            //                    [this](stl::error_code const& ec, socket_type
            //                    socket) {
            //                        // Check whether the server was stopped by a signal
            //
            //                        // before this completion handler had a chance to run
            //                        if (!acceptor.is_open()) {
            //                            return;
            //                        }
            //
            //                        if (!ec) {
            //                            connections.emplace_back(stl::move(socket));
            //                        } else {
            //                            // TODO: log
            //                        }
            //
            //                        if (!io.stopped())
            //                            accept();
            //                    });
        }

      public:
        // TODO: optimize this
        server(istl::vector<traits_type, endpoint_type> endpoints) noexcept {};

        void run() noexcept {
            // Run until the tasks finishes normally.
            // Don't worry, we'll accept another connection when we finish one
            // of them
            for (;;) {
                try {
                    io.run();
                    break;
                } catch (stl::exception const& err) {
                    // todo: what should I do here?
                }
            }
        }

        void stop() noexcept {
            io.stop();
        }
    };

} // namespace webpp

#endif // WEBPP_INTERFACES_COMMON_SERVER_H


