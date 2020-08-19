#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "connection.hpp"
#include "constants.hpp"
#include "webpp/std/buffer.hpp"
#include "webpp/std/internet.hpp"
#include "webpp/std/io_context.hpp"
#include "webpp/std/vector.hpp"
#include "webpp/traits/traits_concepts.hpp"

#include <boost/asio/thread_pool.hpp>
#include <memory>

namespace webpp::common {

    /**
     * This class is the server and the connection manager.
     */
    template <Traits TraitsType>
    class server {
      public:
        using traits_type     = TraitsType;
        using socket_type     = stl::net::ip::tcp::socket;
        using endpoint_type   = stl::net::ip::tcp::endpoint;
        using io_context_type = stl::net::io_context;

        // I share this publicly because I know this file will not be used in a
        // header file so the final user doesn't have access to this class.
        io_context_type io;

      private:
        stl::vector<connection>                  connections;
        stl::vector<stl::net::ip::tcp::acceptor> acceptors;
        boost::asio::thread_pool                 pool;

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

} // namespace webpp::common

#endif // WEBPP_INTERFACES_COMMON_SERVER_H